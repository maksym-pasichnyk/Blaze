#include <jni.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/resource.h>

#include <array>
#include <queue>
#include <array>
#include <thread>
#include <memory>
#include <optional>

//#include <Debug.hpp>
#include <fmt/format.h>
#include <vulkan/vulkan.hpp>
#include <android/configuration.h>
#include <android/native_activity.h>

#include <android/log.h>

enum {
    LOOPER_ID_MAIN = 1,
    LOOPER_ID_INPUT = 2
};

enum {
    APP_CMD_INPUT_CREATED,
    APP_CMD_INPUT_DESTROYED,
    APP_CMD_INIT_WINDOW,
    APP_CMD_TERM_WINDOW,
//    APP_CMD_WINDOW_RESIZED,
//    APP_CMD_WINDOW_REDRAW_NEEDED,
//    APP_CMD_CONTENT_RECT_CHANGED,
//    APP_CMD_GAINED_FOCUS,
//    APP_CMD_LOST_FOCUS,
    APP_CMD_CONFIG_CHANGED,
//    APP_CMD_LOW_MEMORY,
//    APP_CMD_START,
//    APP_CMD_RESUME,
//    APP_CMD_SAVE_STATE,
//    APP_CMD_PAUSE,
//    APP_CMD_STOP,
    APP_CMD_DESTROY,
};

extern void Start(int argc, char** argv);

struct AndroidPlatform {
    friend void ANativeActivity_onCreate(ANativeActivity* activity, void* _savedState, size_t _savedStateLen);

    struct {
//        Debug log{"android_platform"};

        ALooper* looper = nullptr;
        AAssetManager* assets = nullptr;
        AConfiguration* config = nullptr;

        ANativeWindow* window = nullptr;
        ANativeWindow* pendingWindow = nullptr;

        AInputQueue* inputQueue = nullptr;
        AInputQueue* pendingInputQueue = nullptr;

        ARect contentRect{};
        ARect pendingContentRect{};

        bool destroyRequested = false;

        std::mutex mutex;
        std::condition_variable signal;

        std::array<int, 2> pipes{};

        std::optional<std::thread> renderThread;
    } self {};

    explicit AndroidPlatform(AAssetManager* assets) : self {
        .assets = assets,
        .config = AConfiguration_new()
    } {
        AConfiguration_fromAssetManager(self.config, self.assets);

        if (pipe(self.pipes.data())) {
//            self.log.error(fmt::format("could not create pipe: {}", strerror(errno)));
        }
    }

    AndroidPlatform() {
        close(self.pipes[0]);
        close(self.pipes[1]);
    }

    ~AndroidPlatform() {
        AConfiguration_delete(self.config);
    }

    auto getAssets() -> AAssetManager* {
        return self.assets;
    }

    void pollEvents() {
        while (true) {
            int fd = 0;
            const auto ident = ALooper_pollAll(0, &fd, nullptr, nullptr);
            if (ident < 0) {
                break;
            }
            if (ident == LOOPER_ID_MAIN) {
                uint8_t cmd = 0;
                if (read(fd, &cmd, sizeof(uint8_t)) != sizeof(uint8_t)) {
//                    self.log.error("No data on command pipe!");
                    continue;
                }
                _handleEvent(cmd);
            }
            if (ident == LOOPER_ID_INPUT) {
                if (AInputQueue_hasEvents(self.inputQueue)) {
                    AInputEvent *event = nullptr;
                    while (AInputQueue_getEvent(self.inputQueue, &event) >= 0) {
                        const auto type = AInputEvent_getType(event);
//                        self.log.verbose(fmt::format("InputEvent: type = {}", type));
                        if (!AInputQueue_preDispatchEvent(self.inputQueue, event)) {
                            AInputQueue_finishEvent(self.inputQueue, event, _handleInput(event));
                        }
                    }
                }
            }
        }
    }

    auto shouldClose() -> bool {
        return self.destroyRequested;
    }

private:
    void _writeEvent(uint8_t cmd) {
        if (write(self.pipes[1], &cmd, sizeof(uint8_t)) != sizeof(uint8_t)) {
//            self.log.error(fmt::format("Failure writing AndroidPlatform cmd: {}", strerror(errno)));
        }
    }

    void _onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue) {
        if (self.renderThread.has_value()) {
            self.pendingInputQueue = queue;
            _writeEvent(APP_CMD_INPUT_CREATED);

            std::unique_lock lock{self.mutex};
            self.signal.wait(lock, [this] {
                return self.inputQueue != self.pendingInputQueue;
            });
        } else {
            self.inputQueue = queue;

//            self.log.debug("_onInputQueueCreated: render thread is not initialized");
        }
    }

    void _onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue) {
        if (self.renderThread.has_value()) {
            self.pendingInputQueue = nullptr;
            _writeEvent(APP_CMD_INPUT_DESTROYED);

            std::unique_lock lock{self.mutex};
            self.signal.wait(lock, [this] {
                return self.inputQueue != self.pendingInputQueue;
            });
        } else {
//            self.log.debug("_onInputQueueDestroyed: render thread is not initialized");
        }
    }

    void _onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window) {
        if (!self.renderThread.has_value()) {
            self.window = window;
            self.renderThread.emplace([this] {
                self.looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
                ALooper_addFd(
                    self.looper,
                    self.pipes[0],
                    LOOPER_ID_MAIN,
                    ALOOPER_EVENT_INPUT,
                    nullptr,
                    nullptr
                );

                if (self.inputQueue != nullptr) {
                    AInputQueue_attachLooper(
                        self.inputQueue,
                        self.looper,
                        LOOPER_ID_INPUT,
                        nullptr,
                        nullptr
                    );
                }

                Start(0, nullptr);

                if (self.inputQueue != nullptr) {
                    AInputQueue_detachLooper(self.inputQueue);
                }
                ALooper_removeFd(self.looper, self.pipes[0]);
            });
        } else {
            self.pendingWindow = window;
            _writeEvent(APP_CMD_INIT_WINDOW);

            std::unique_lock lock{self.mutex};
            self.signal.wait(lock, [this] {
                return self.window != self.pendingWindow;
            });
        }
    }

    void _onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window) {}

public:
    void _handleEvent(uint8_t cmd) {
        switch (cmd) {
            case APP_CMD_INPUT_CREATED: {
                if (self.inputQueue != nullptr) {
                    AInputQueue_detachLooper(self.inputQueue);
                }
                self.inputQueue = self.pendingInputQueue;
                AInputQueue_attachLooper(
                    self.inputQueue,
                    self.looper,
                    LOOPER_ID_INPUT,
                    nullptr,
                    nullptr
                );
                self.signal.notify_all();
            } break;
            case APP_CMD_INPUT_DESTROYED: {
                AInputQueue_detachLooper(self.inputQueue);
                self.inputQueue = self.pendingInputQueue;
                self.signal.notify_all();
            } break;
            case APP_CMD_INIT_WINDOW: {
                self.window = self.pendingWindow;
                self.signal.notify_all();
            } break;
            case APP_CMD_TERM_WINDOW: {
                self.window = self.pendingWindow;
                self.signal.notify_all();
            } break;
            case APP_CMD_CONFIG_CHANGED:
                AConfiguration_fromAssetManager(self.config, self.assets);
                break;
            case APP_CMD_DESTROY:
                self.destroyRequested = true;
                break;
            default:
                break;
        }
    }

    auto _handleInput(AInputEvent* event) -> int32_t {
        return 0;
    }
};

namespace {
    std::unique_ptr<AndroidPlatform> m_AndroidPlatform;
}

auto AndroidPlatform_getWindow() -> ANativeWindow* {
    return m_AndroidPlatform->self.window;
}

auto AndroidPlatform_getAssets() -> AAssetManager* {
    return m_AndroidPlatform->self.assets;
}

void AndroidPlatform_pollEvents() {
    return m_AndroidPlatform->pollEvents();
}

auto AndroidPlatform_shouldClose() -> bool {
    return m_AndroidPlatform->shouldClose();
}

[[maybe_unused]]
JNIEXPORT void ANativeActivity_onCreate(ANativeActivity* activity, void* _savedState, size_t _savedStateLen) {
    m_AndroidPlatform = std::make_unique<AndroidPlatform>(activity->assetManager);

    activity->callbacks->onDestroy = [](ANativeActivity* activity) {

    };
    activity->callbacks->onStart = [](ANativeActivity* activity) {

    };
    activity->callbacks->onResume = [](ANativeActivity* activity) {

    };
    activity->callbacks->onSaveInstanceState = [](ANativeActivity* activity, size_t* outSize) -> void* {
        return nullptr;
    };
    activity->callbacks->onPause = [](ANativeActivity* activity) {

    };
    activity->callbacks->onStop = [](ANativeActivity* activity) {

    };
    activity->callbacks->onConfigurationChanged = [](ANativeActivity* activity) {

    };
    activity->callbacks->onLowMemory = [](ANativeActivity* activity) {

    };
    activity->callbacks->onWindowFocusChanged = [](ANativeActivity* activity, int focus) {

    };
    activity->callbacks->onNativeWindowCreated = [](ANativeActivity* activity, ANativeWindow* window) {
        m_AndroidPlatform->_onNativeWindowCreated(activity, window);
    };
    activity->callbacks->onNativeWindowDestroyed = [](ANativeActivity* activity, ANativeWindow* window) {
        m_AndroidPlatform->_onNativeWindowDestroyed(activity, window);
    };
    activity->callbacks->onInputQueueCreated = [](ANativeActivity* activity, AInputQueue* queue) {
        m_AndroidPlatform->_onInputQueueCreated(activity, queue);
    };
    activity->callbacks->onInputQueueDestroyed = [](ANativeActivity* activity, AInputQueue* queue) {
        m_AndroidPlatform->_onInputQueueDestroyed(activity, queue);
    };
}
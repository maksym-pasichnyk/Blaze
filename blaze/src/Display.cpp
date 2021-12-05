#include "Display.hpp"

#ifdef __ANDROID__
#include <android/native_window.h>
#else
#include <GLFW/glfw3.h>
#endif

#if __ANDROID__
struct BlazeDisplay::Impl {
    ANativeWindow* window;

    Impl(const std::string& title, int width, int height, bool resizable) {
        extern auto AndroidPlatform_getWindow() -> ANativeWindow*;

        window = AndroidPlatform_getWindow();
    }

    auto shouldClose() const -> bool {
        extern auto AndroidPlatform_shouldClose() -> bool;
        return AndroidPlatform_shouldClose();
    }

    void pollEvents() {
        extern void AndroidPlatform_pollEvents();
        AndroidPlatform_pollEvents();
    }

    auto createSurface(vk::Instance instance) -> vk::SurfaceKHR {
        const auto createInfo = vk::AndroidSurfaceCreateInfoKHR{
            .window = static_cast<ANativeWindow*>(window)
        };
        return instance.createAndroidSurfaceKHR(createInfo);
    }

    auto getInstanceExtensions() -> std::vector<const char *> {
        return {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
        };
    }

    auto getSize() -> std::pair<int, int> {
        return {
            ANativeWindow_getWidth(window),
            ANativeWindow_getHeight(window)
        };
    }

    auto getScale() -> std::pair<float, float> {
        return { 1.0f, 1.0f };
    }

    auto hasFocus() -> bool {
        return true;
//        return glfwGetWindowAttrib(window, GLFW_FOCUSED) != 0;
    }

    auto getMousePosition() -> std::pair<float, float> {
//        double x, y;
//        glfwGetCursorPos(window, &x, &y);
//        return {
//            static_cast<float>(x),
//            static_cast<float>(y)
//        };
        return {0, 0};
    }

    void setMousePosition(std::pair<float, float> pos) {
//        const auto x = static_cast<double>(pos.first);
//        const auto y = static_cast<double>(pos.second);
//
//        glfwSetCursorPos(window, x, y);
    }
};
#else
struct Display::Impl {
    GLFWwindow* _window;

    Impl(const std::string& title, int width, int height, bool resizable) {
        glfwInit();
        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        _window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    }

    ~Impl() {
        glfwDestroyWindow(_window);
    }

    auto shouldClose() const -> bool {
        return glfwWindowShouldClose(_window);
    }

    void pollEvents() {
        glfwPollEvents();
    }

    auto createSurface(vk::Instance instance) -> vk::SurfaceKHR {
        VkSurfaceKHR surface;
        glfwCreateWindowSurface(instance, _window, nullptr, &surface);
        return static_cast<vk::SurfaceKHR>(surface);
    }

    auto getInstanceExtensions() -> std::vector<const char *> {
        uint32_t count = 0;
        auto extensions = glfwGetRequiredInstanceExtensions(&count);
        return {extensions, extensions + count};
    }

    auto getSize() -> std::pair<int, int> {
        int width = 0;
        int height = 0;
        glfwGetWindowSize(_window, &width, &height);
        return {width, height};
    }

    auto getScale() -> std::pair<float, float> {
        auto [w, h] = getSize();

        int fw = 0;
        int fh = 0;
        glfwGetFramebufferSize(_window, &fw, &fh);

        return {
            float(fw) / float(w),
            float(fh) / float(h)
        };
    }

    auto hasFocus() -> bool {
        return glfwGetWindowAttrib(_window, GLFW_FOCUSED) != 0;
    }

    auto getMousePosition() -> std::pair<float, float> {
        double x, y;
        glfwGetCursorPos(_window, &x, &y);
        return {
            static_cast<float>(x),
            static_cast<float>(y)
        };
    }

    void setMousePosition(std::pair<float, float> pos) {
        const auto x = static_cast<double>(pos.first);
        const auto y = static_cast<double>(pos.second);

        glfwSetCursorPos(_window, x, y);
    }

    auto getMousePressed(int button) -> bool {
        return glfwGetMouseButton(_window, button) != 0;
    }
};
#endif

void Display::init(const std::string& title, int width, int height, bool resizable) {
    impl = blaze::make_internal<Impl>(title, width, height, resizable);
}

void Display::pollEvents() {
    impl->pollEvents();
}

auto Display::shouldClose() -> bool {
    return impl->shouldClose();
}

auto Display::createSurface(vk::Instance instance) -> vk::SurfaceKHR {
    return impl->createSurface(instance);
}

auto Display::getInstanceExtensions() const -> std::vector<const char *> {
    return impl->getInstanceExtensions();
}

auto Display::getSize() const -> std::pair<int, int> {
    return impl->getSize();
}

auto Display::getScale() const -> std::pair<float, float> {
    return impl->getScale();
}

auto Display::hasFocus() const -> bool {
    return impl->hasFocus();
}

auto Display::getMousePosition() const -> std::pair<float, float> {
    return impl->getMousePosition();
}

void Display::setMousePosition(std::pair<float, float> pos) {
    impl->setMousePosition(pos);
}

auto Display::getMousePressed(int button) const -> bool {
    return impl->getMousePressed(button);
}

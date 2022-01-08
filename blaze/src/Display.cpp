#include "Display.hpp"
#include "imgui.h"
#include "UserInterface.hpp"

//#ifdef __ANDROID__
//#include <android/native_window.h>
//#else
#include <GLFW/glfw3.h>
//#endif

//#if __ANDROID__
//struct BlazeDisplay::Impl {
//    ANativeWindow* window;
//
//    Impl(const std::string& title, int width, int height, bool resizable) {
//        extern auto AndroidPlatform_getWindow() -> ANativeWindow*;
//
//        window = AndroidPlatform_getWindow();
//    }
//
//    auto shouldClose() const -> bool {
//        extern auto AndroidPlatform_shouldClose() -> bool;
//        return AndroidPlatform_shouldClose();
//    }
//
//    void pollEvents() {
//        extern void AndroidPlatform_pollEvents();
//        AndroidPlatform_pollEvents();
//    }
//
//    auto createSurface(vk::Instance instance) -> vk::SurfaceKHR {
//        const auto createInfo = vk::AndroidSurfaceCreateInfoKHR{
//            .window = static_cast<ANativeWindow*>(window)
//        };
//        return instance.createAndroidSurfaceKHR(createInfo);
//    }
//
//    auto getInstanceExtensions() -> std::vector<const char *> {
//        return {
//            VK_KHR_SURFACE_EXTENSION_NAME,
//            VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
//        };
//    }
//
//    auto getSize() -> std::pair<int, int> {
//        return {
//            ANativeWindow_getWidth(window),
//            ANativeWindow_getHeight(window)
//        };
//    }
//
//    auto getScale() -> const glm::vec2& {
//        return { 1.0f, 1.0f };
//    }
//
//    auto hasFocus() -> bool {
//        return true;
////        return glfwGetWindowAttrib(window, GLFW_FOCUSED) != 0;
//    }
//
//    auto getMousePosition() -> const glm::vec2& {
////        double x, y;
////        glfwGetCursorPos(window, &x, &y);
////        return {
////            static_cast<float>(x),
////            static_cast<float>(y)
////        };
//        return {0, 0};
//    }
//
//    void setMousePosition(const glm::vec2& pos) {
////        const auto x = static_cast<double>(pos.first);
////        const auto y = static_cast<double>(pos.second);
////
////        glfwSetCursorPos(window, x, y);
//    }
//};
//#endif

Display::Display(const std::string& title, int width, int height, bool resizable) {
    glfwInit();
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwSetCharCallback(_window, [](GLFWwindow* window, unsigned int c) {
        extern auto GetUserInterface() -> UserInterface&;
        GetUserInterface().AddInputCharacter(static_cast<char>(c));
    });
//    glfwSetKeyCallback(_window, [] {});
    mouseButtonMapping.fill(-1);
    mouseButtonMapping[size_t(MouseButton::Left)] = GLFW_MOUSE_BUTTON_LEFT;
    mouseButtonMapping[size_t(MouseButton::Right)] = GLFW_MOUSE_BUTTON_RIGHT;
    mouseButtonMapping[size_t(MouseButton::Middle)] = GLFW_MOUSE_BUTTON_MIDDLE;

    keyCodeMapping.fill(-1);
    keyCodeMapping[size_t(KeyCode::eTab)] = GLFW_KEY_TAB;
    keyCodeMapping[size_t(KeyCode::eLeftArrow)] = GLFW_KEY_A;//GLFW_KEY_LEFT;
    keyCodeMapping[size_t(KeyCode::eRightArrow)] = GLFW_KEY_D;//GLFW_KEY_RIGHT;
    keyCodeMapping[size_t(KeyCode::eUpArrow)] = GLFW_KEY_W;//GLFW_KEY_UP;
    keyCodeMapping[size_t(KeyCode::eDownArrow)] = GLFW_KEY_S;//GLFW_KEY_DOWN;
    keyCodeMapping[size_t(KeyCode::ePageUp)] = GLFW_KEY_PAGE_UP;
    keyCodeMapping[size_t(KeyCode::ePageDown)] = GLFW_KEY_PAGE_DOWN;
    keyCodeMapping[size_t(KeyCode::eHome)] = GLFW_KEY_HOME;
    keyCodeMapping[size_t(KeyCode::eEnd)] = GLFW_KEY_END;
    keyCodeMapping[size_t(KeyCode::eInsert)] = GLFW_KEY_INSERT;
    keyCodeMapping[size_t(KeyCode::eDelete)] = GLFW_KEY_DELETE;
    keyCodeMapping[size_t(KeyCode::eBackspace)] = GLFW_KEY_BACKSPACE;
    keyCodeMapping[size_t(KeyCode::eSpace)] = GLFW_KEY_SPACE;
    keyCodeMapping[size_t(KeyCode::eEnter)] = GLFW_KEY_ENTER;
    keyCodeMapping[size_t(KeyCode::eEscape)] = GLFW_KEY_ESCAPE;
    keyCodeMapping[size_t(KeyCode::eKeyPadEnter)] = GLFW_KEY_KP_ENTER;
    keyCodeMapping[size_t(KeyCode::eA)] = GLFW_KEY_A;
    keyCodeMapping[size_t(KeyCode::eC)] = GLFW_KEY_C;
    keyCodeMapping[size_t(KeyCode::eV)] = GLFW_KEY_V;
    keyCodeMapping[size_t(KeyCode::eX)] = GLFW_KEY_X;
    keyCodeMapping[size_t(KeyCode::eY)] = GLFW_KEY_Y;
    keyCodeMapping[size_t(KeyCode::eZ)] = GLFW_KEY_Z;
    keyCodeMapping[size_t(KeyCode::eLeftShift)] = GLFW_KEY_LEFT_SHIFT;
}

Display::~Display() {
    glfwDestroyWindow(_window);
}

auto Display::shouldClose() const -> bool {
    return glfwWindowShouldClose(_window);
}

void Display::close() {
    glfwSetWindowShouldClose(_window, true);
}

void Display::pollEvents() {
    glfwPollEvents();
}

auto Display::createSurface(vk::Instance instance) -> vk::SurfaceKHR {
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(instance, _window, nullptr, &surface);
    return static_cast<vk::SurfaceKHR>(surface);
}

auto Display::getInstanceExtensions() const -> std::vector<const char *> {
    uint32_t count = 0;
    auto extensions = glfwGetRequiredInstanceExtensions(&count);
    return {extensions, extensions + count};
}

auto Display::getSize() const -> glm::ivec2 {
    glm::ivec2 size{};
    glfwGetWindowSize(_window, &size.x, &size.y);
    return size;
}

auto Display::getScale() const -> glm::vec2 {
    glm::ivec2 size{};
    glfwGetFramebufferSize(_window, &size.x, &size.y);
    return glm::vec2(size) / glm::vec2(getSize());
}

auto Display::hasFocus() const -> bool {
    return glfwGetWindowAttrib(_window, GLFW_FOCUSED) != 0;
}

auto Display::getMousePosition() const -> glm::vec2 {
    glm::dvec2 pos;
    glfwGetCursorPos(_window, &pos.x, &pos.y);
    return glm::vec2(pos);
}

void Display::setMousePosition(const glm::vec2& pos) {
    glfwSetCursorPos(_window, pos.x, pos.y);
}

auto Display::getMousePressed(MouseButton button) const -> bool {
    return glfwGetMouseButton(_window, mouseButtonMapping[static_cast<size_t>(button)]) == GLFW_PRESS;
}

auto Display::getKeyPressed(KeyCode keycode) const -> bool {
    return glfwGetKey(_window, keyCodeMapping[static_cast<size_t>(keycode)]) == GLFW_PRESS;
}

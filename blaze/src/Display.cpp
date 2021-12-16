#include "Display.hpp"

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
}

Display::~Display() {
    glfwDestroyWindow(_window);
}

auto Display::shouldClose() const -> bool {
    return glfwWindowShouldClose(_window);
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

auto Display::getMousePressed(int button) const -> bool {
    return glfwGetMouseButton(_window, button) != 0;
}
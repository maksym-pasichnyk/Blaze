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
    glfwSetScrollCallback(_window, [](GLFWwindow* window, double xoffset, double yoffset) {
        extern auto GetUserInterface() -> UserInterface&;
        GetUserInterface().AddScrollMouse(
            static_cast<float>(xoffset),
            static_cast<float>(yoffset)
        );
    });
    mouseButtonMapping.fill(GLFW_KEY_UNKNOWN);
    mouseButtonMapping[size_t(MouseButton::Left)] = GLFW_MOUSE_BUTTON_LEFT;
    mouseButtonMapping[size_t(MouseButton::Right)] = GLFW_MOUSE_BUTTON_RIGHT;
    mouseButtonMapping[size_t(MouseButton::Middle)] = GLFW_MOUSE_BUTTON_MIDDLE;

    keyCodeMapping.fill(GLFW_KEY_UNKNOWN);
    keyCodeMapping[size_t(KeyCode::eSpace)] = GLFW_KEY_SPACE;
    keyCodeMapping[size_t(KeyCode::eApostrophe)] = GLFW_KEY_APOSTROPHE;
    keyCodeMapping[size_t(KeyCode::eComma)] = GLFW_KEY_COMMA;
    keyCodeMapping[size_t(KeyCode::eMinus)] = GLFW_KEY_MINUS;
    keyCodeMapping[size_t(KeyCode::ePeriod)] = GLFW_KEY_PERIOD;
    keyCodeMapping[size_t(KeyCode::eSlash)] = GLFW_KEY_SLASH;
    keyCodeMapping[size_t(KeyCode::e0)] = GLFW_KEY_0;
    keyCodeMapping[size_t(KeyCode::e1)] = GLFW_KEY_1;
    keyCodeMapping[size_t(KeyCode::e2)] = GLFW_KEY_2;
    keyCodeMapping[size_t(KeyCode::e3)] = GLFW_KEY_3;
    keyCodeMapping[size_t(KeyCode::e4)] = GLFW_KEY_4;
    keyCodeMapping[size_t(KeyCode::e5)] = GLFW_KEY_5;
    keyCodeMapping[size_t(KeyCode::e6)] = GLFW_KEY_6;
    keyCodeMapping[size_t(KeyCode::e7)] = GLFW_KEY_7;
    keyCodeMapping[size_t(KeyCode::e8)] = GLFW_KEY_8;
    keyCodeMapping[size_t(KeyCode::e9)] = GLFW_KEY_9;
    keyCodeMapping[size_t(KeyCode::eSemicolon)] = GLFW_KEY_SEMICOLON;
    keyCodeMapping[size_t(KeyCode::eEqual)] = GLFW_KEY_EQUAL;
    keyCodeMapping[size_t(KeyCode::eA)] = GLFW_KEY_A;
    keyCodeMapping[size_t(KeyCode::eB)] = GLFW_KEY_B;
    keyCodeMapping[size_t(KeyCode::eC)] = GLFW_KEY_C;
    keyCodeMapping[size_t(KeyCode::eD)] = GLFW_KEY_D;
    keyCodeMapping[size_t(KeyCode::eE)] = GLFW_KEY_E;
    keyCodeMapping[size_t(KeyCode::eF)] = GLFW_KEY_F;
    keyCodeMapping[size_t(KeyCode::eG)] = GLFW_KEY_G;
    keyCodeMapping[size_t(KeyCode::eH)] = GLFW_KEY_H;
    keyCodeMapping[size_t(KeyCode::eI)] = GLFW_KEY_I;
    keyCodeMapping[size_t(KeyCode::eJ)] = GLFW_KEY_J;
    keyCodeMapping[size_t(KeyCode::eK)] = GLFW_KEY_K;
    keyCodeMapping[size_t(KeyCode::eL)] = GLFW_KEY_L;
    keyCodeMapping[size_t(KeyCode::eM)] = GLFW_KEY_M;
    keyCodeMapping[size_t(KeyCode::eN)] = GLFW_KEY_N;
    keyCodeMapping[size_t(KeyCode::eO)] = GLFW_KEY_O;
    keyCodeMapping[size_t(KeyCode::eP)] = GLFW_KEY_P;
    keyCodeMapping[size_t(KeyCode::eQ)] = GLFW_KEY_Q;
    keyCodeMapping[size_t(KeyCode::eR)] = GLFW_KEY_R;
    keyCodeMapping[size_t(KeyCode::eS)] = GLFW_KEY_S;
    keyCodeMapping[size_t(KeyCode::eT)] = GLFW_KEY_T;
    keyCodeMapping[size_t(KeyCode::eU)] = GLFW_KEY_U;
    keyCodeMapping[size_t(KeyCode::eV)] = GLFW_KEY_V;
    keyCodeMapping[size_t(KeyCode::eW)] = GLFW_KEY_W;
    keyCodeMapping[size_t(KeyCode::eX)] = GLFW_KEY_X;
    keyCodeMapping[size_t(KeyCode::eY)] = GLFW_KEY_Y;
    keyCodeMapping[size_t(KeyCode::eZ)] = GLFW_KEY_Z;
    keyCodeMapping[size_t(KeyCode::eLelfBracket)] = GLFW_KEY_LEFT_BRACKET;
    keyCodeMapping[size_t(KeyCode::eBackslash)] = GLFW_KEY_BACKSLASH;
    keyCodeMapping[size_t(KeyCode::eRightBracket)] = GLFW_KEY_RIGHT_BRACKET;
    keyCodeMapping[size_t(KeyCode::eGraceAccent)] = GLFW_KEY_GRAVE_ACCENT;
    keyCodeMapping[size_t(KeyCode::eWorld1)] = GLFW_KEY_WORLD_1;
    keyCodeMapping[size_t(KeyCode::eWorld2)] = GLFW_KEY_WORLD_2;

    keyCodeMapping[size_t(KeyCode::eEscape)] = GLFW_KEY_ESCAPE;
    keyCodeMapping[size_t(KeyCode::eEnter)] = GLFW_KEY_ENTER;
    keyCodeMapping[size_t(KeyCode::eTab)] = GLFW_KEY_TAB;
    keyCodeMapping[size_t(KeyCode::eBackspace)] = GLFW_KEY_BACKSPACE;
    keyCodeMapping[size_t(KeyCode::eInsert)] = GLFW_KEY_INSERT;
    keyCodeMapping[size_t(KeyCode::eDelete)] = GLFW_KEY_DELETE;
    keyCodeMapping[size_t(KeyCode::eRight)] = GLFW_KEY_RIGHT;
    keyCodeMapping[size_t(KeyCode::eLeft)] = GLFW_KEY_LEFT;
    keyCodeMapping[size_t(KeyCode::eDown)] = GLFW_KEY_DOWN;
    keyCodeMapping[size_t(KeyCode::eUp)] = GLFW_KEY_UP;
    keyCodeMapping[size_t(KeyCode::ePageUp)] = GLFW_KEY_PAGE_UP;
    keyCodeMapping[size_t(KeyCode::ePageDown)] = GLFW_KEY_PAGE_DOWN;
    keyCodeMapping[size_t(KeyCode::eHome)] = GLFW_KEY_HOME;
    keyCodeMapping[size_t(KeyCode::eEnd)] = GLFW_KEY_END;
    keyCodeMapping[size_t(KeyCode::eCapsLock)] = GLFW_KEY_CAPS_LOCK;
    keyCodeMapping[size_t(KeyCode::eScrollLock)] = GLFW_KEY_SCROLL_LOCK;
    keyCodeMapping[size_t(KeyCode::eNumLock)] = GLFW_KEY_NUM_LOCK;
    keyCodeMapping[size_t(KeyCode::ePrintScreen)] = GLFW_KEY_PRINT_SCREEN;
    keyCodeMapping[size_t(KeyCode::ePause)] = GLFW_KEY_PAUSE;
    keyCodeMapping[size_t(KeyCode::eF1)] = GLFW_KEY_F1;
    keyCodeMapping[size_t(KeyCode::eF2)] = GLFW_KEY_F2;
    keyCodeMapping[size_t(KeyCode::eF3)] = GLFW_KEY_F3;
    keyCodeMapping[size_t(KeyCode::eF4)] = GLFW_KEY_F4;
    keyCodeMapping[size_t(KeyCode::eF5)] = GLFW_KEY_F5;
    keyCodeMapping[size_t(KeyCode::eF6)] = GLFW_KEY_F6;
    keyCodeMapping[size_t(KeyCode::eF7)] = GLFW_KEY_F7;
    keyCodeMapping[size_t(KeyCode::eF8)] = GLFW_KEY_F8;
    keyCodeMapping[size_t(KeyCode::eF9)] = GLFW_KEY_F9;
    keyCodeMapping[size_t(KeyCode::eF10)] = GLFW_KEY_F10;
    keyCodeMapping[size_t(KeyCode::eF11)] = GLFW_KEY_F11;
    keyCodeMapping[size_t(KeyCode::eF12)] = GLFW_KEY_F12;
    keyCodeMapping[size_t(KeyCode::eF13)] = GLFW_KEY_F13;
    keyCodeMapping[size_t(KeyCode::eF14)] = GLFW_KEY_F14;
    keyCodeMapping[size_t(KeyCode::eF15)] = GLFW_KEY_F15;
    keyCodeMapping[size_t(KeyCode::eF16)] = GLFW_KEY_F16;
    keyCodeMapping[size_t(KeyCode::eF17)] = GLFW_KEY_F17;
    keyCodeMapping[size_t(KeyCode::eF18)] = GLFW_KEY_F18;
    keyCodeMapping[size_t(KeyCode::eF19)] = GLFW_KEY_F19;
    keyCodeMapping[size_t(KeyCode::eF20)] = GLFW_KEY_F20;
    keyCodeMapping[size_t(KeyCode::eF21)] = GLFW_KEY_F21;
    keyCodeMapping[size_t(KeyCode::eF22)] = GLFW_KEY_F22;
    keyCodeMapping[size_t(KeyCode::eF23)] = GLFW_KEY_F23;
    keyCodeMapping[size_t(KeyCode::eF24)] = GLFW_KEY_F24;
    keyCodeMapping[size_t(KeyCode::eF25)] = GLFW_KEY_F25;
    keyCodeMapping[size_t(KeyCode::eKeyPad0)] = GLFW_KEY_KP_0;
    keyCodeMapping[size_t(KeyCode::eKeyPad1)] = GLFW_KEY_KP_1;
    keyCodeMapping[size_t(KeyCode::eKeyPad2)] = GLFW_KEY_KP_2;
    keyCodeMapping[size_t(KeyCode::eKeyPad3)] = GLFW_KEY_KP_3;
    keyCodeMapping[size_t(KeyCode::eKeyPad4)] = GLFW_KEY_KP_4;
    keyCodeMapping[size_t(KeyCode::eKeyPad5)] = GLFW_KEY_KP_5;
    keyCodeMapping[size_t(KeyCode::eKeyPad6)] = GLFW_KEY_KP_6;
    keyCodeMapping[size_t(KeyCode::eKeyPad7)] = GLFW_KEY_KP_7;
    keyCodeMapping[size_t(KeyCode::eKeyPad8)] = GLFW_KEY_KP_8;
    keyCodeMapping[size_t(KeyCode::eKeyPad9)] = GLFW_KEY_KP_9;
    keyCodeMapping[size_t(KeyCode::eKeyPadDecimal)] = GLFW_KEY_KP_DECIMAL;
    keyCodeMapping[size_t(KeyCode::eKeyPadDivide)] = GLFW_KEY_KP_DIVIDE;
    keyCodeMapping[size_t(KeyCode::eKeyPadMultiply)] = GLFW_KEY_KP_MULTIPLY;
    keyCodeMapping[size_t(KeyCode::eKeyPadSubstract)] = GLFW_KEY_KP_SUBTRACT;
    keyCodeMapping[size_t(KeyCode::eKeyPadAdd)] = GLFW_KEY_KP_ADD;
    keyCodeMapping[size_t(KeyCode::eKeyPadEnter)] = GLFW_KEY_KP_ENTER;
    keyCodeMapping[size_t(KeyCode::eKeyPadEqual)] = GLFW_KEY_KP_EQUAL;
    keyCodeMapping[size_t(KeyCode::eLeftShift)] = GLFW_KEY_LEFT_SHIFT;
    keyCodeMapping[size_t(KeyCode::eLeftControl)] = GLFW_KEY_LEFT_CONTROL;
    keyCodeMapping[size_t(KeyCode::eLeftAlt)] = GLFW_KEY_LEFT_ALT;
    keyCodeMapping[size_t(KeyCode::eLeftSuper)] = GLFW_KEY_LEFT_SUPER;
    keyCodeMapping[size_t(KeyCode::eRightShift)] = GLFW_KEY_RIGHT_SHIFT;
    keyCodeMapping[size_t(KeyCode::eRightControl)] = GLFW_KEY_RIGHT_CONTROL;
    keyCodeMapping[size_t(KeyCode::eRightAlt)] = GLFW_KEY_RIGHT_ALT;
    keyCodeMapping[size_t(KeyCode::eRightSuper)] = GLFW_KEY_RIGHT_SUPER;
    keyCodeMapping[size_t(KeyCode::eMenu)] = GLFW_KEY_MENU;
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

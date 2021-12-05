#include "SharedLibrary.hpp"

#if defined( __unix__ ) || defined( __APPLE__ ) || defined( __QNXNTO__ ) || defined( __Fuchsia__ )
#include <dlfcn.h>
#elif defined( _WIN32 )
#include <windows.h>
#else
#error unsupported platform
#endif


void SharedLibrary::AutoClose::operator()(void* handle) noexcept {
#if defined( __unix__ ) || defined( __APPLE__ ) || defined( __QNXNTO__ ) || defined( __Fuchsia__ )
    dlclose(handle);
#elif defined( _WIN32 )
    FreeLibrary(static_cast<HINSTANCE>(handle));
#else
#error unsupported platform
#endif
}

auto SharedLibrary::open(const std::string &name) noexcept -> std::optional<SharedLibrary> {
#if defined( __unix__ ) || defined( __APPLE__ ) || defined( __QNXNTO__ ) || defined( __Fuchsia__ )
    auto lib = dlopen(name.c_str(), RTLD_NOW | RTLD_LOCAL);
#elif defined( _WIN32 )
    auto lib = LoadLibraryA(name.c_str());
#else
#error unsupported platform
#endif
    if (lib == nullptr) {
        return std::nullopt;
    }
    return SharedLibrary(lib);
}

SharedLibrary::SharedLibrary(void *handle) noexcept {
    m_library.reset(handle);
}

auto SharedLibrary::get(const char *function) const noexcept -> void * {
#if defined( __unix__ ) || defined( __APPLE__ ) || defined( __QNXNTO__ ) || defined( __Fuchsia__ )
    return dlsym(m_library.get(), function);
#elif defined( _WIN32 )
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HINSTANCE>(m_library.get()), function));
#else
#error unsupported platform
#endif
}

#include "android.hpp"

#include <dlfcn.h>

#ifdef __ANDROID__
#include <android/api-level.h>
#else
// stub for desktop compilation
inline int android_get_device_api_level() {
    return -1;
}
#endif

#define SYMBOLS(m)                            \
m(AHardwareBuffer_allocate)                   \
m(AHardwareBuffer_acquire)                    \
m(AHardwareBuffer_release)                    \
m(AHardwareBuffer_describe)                   \
m(AHardwareBuffer_lock)                       \
m(AHardwareBuffer_unlock)                     \
m(AHardwareBuffer_sendHandleToUnixSocket)     \
m(AHardwareBuffer_recvHandleFromUnixSocket)

#define LOAD(name) lib.name = (__typeof__(lib.name)) dlsym(lib.libAndroid, #name);
#define NULLIFY(name) lib.name = nullptr;
#define NOTNULL(name) && lib.name != nullptr
#define AND

namespace libAndroidHelper {
    void init(LibAndroid& lib) {
        lib.libAndroid = dlopen("libandroid.so", RTLD_LAZY | RTLD_LOCAL);
        if (lib.libAndroid == nullptr)
            return;

        SYMBOLS(LOAD)
    }
    
    void deinit(LibAndroid& lib) {
        if (lib.libAndroid != nullptr) {
            dlclose(lib.libAndroid);
            lib.libAndroid = nullptr;
        }

        SYMBOLS(NULLIFY)
    }
    
    bool hwbufferAvailable(LibAndroid& lib) {
        return android_get_device_api_level() >= 26 AND SYMBOLS(NOTNULL);
    }
}









#include "android.hpp"


#include <dlfcn.h>
#include <stddef.h>

#ifdef __ANDROID__
#include <android/api-level.h>
#else
// stub for desktop compilation
inline int android_get_device_api_level() {
    return -1;
}
#endif

namespace libAndroidHelper {
    
    void init(LibAndroid& lib) {
        lib.libAndroid = dlopen("libandroid.so", RTLD_LAZY | RTLD_LOCAL);
        if (lib.libAndroid == NULL) return;
        
        lib.AHardwareBuffer_allocate = (int (*)(const AHardwareBuffer_Desc*, AHardwareBuffer**)) dlsym(lib.libAndroid, "AHardwareBuffer_allocate");
        
        lib.AHardwareBuffer_acquire = (void (*)(AHardwareBuffer*)) dlsym(lib.libAndroid, "AHardwareBuffer_acquire");
        
        lib.AHardwareBuffer_release = (void (*)(AHardwareBuffer*)) dlsym(lib.libAndroid, "AHardwareBuffer_release");
        
        lib.AHardwareBuffer_describe = (void (*)(const AHardwareBuffer*, AHardwareBuffer_Desc*)) dlsym(lib.libAndroid, "AHardwareBuffer_describe");
        
        lib.AHardwareBuffer_lock = (int (*)(AHardwareBuffer*, uint64_t, int32_t, const ARect*, void**)) dlsym(lib.libAndroid, "AHardwareBuffer_lock");
        
        lib.AHardwareBuffer_unlock = (int (*)(AHardwareBuffer*, int32_t*)) dlsym(lib.libAndroid, "AHardwareBuffer_unlock");
        
        lib.AHardwareBuffer_sendHandleToUnixSocket = (int (*)(const AHardwareBuffer*, int)) dlsym(lib.libAndroid, "AHardwareBuffer_sendHandleToUnixSocket");
        
        lib.AHardwareBuffer_recvHandleFromUnixSocket = (int(*)(int, AHardwareBuffer**)) dlsym(lib.libAndroid, "AHardwareBuffer_recvHandleFromUnixSocket");
        
        
    }
    
    void deinit(LibAndroid& lib) {
        if (lib.libAndroid != NULL) {
            dlclose(lib.libAndroid);
            lib.libAndroid = NULL;
        }
        lib.AHardwareBuffer_allocate = NULL;
        lib.AHardwareBuffer_acquire = NULL;
        lib.AHardwareBuffer_release = NULL;
        lib.AHardwareBuffer_describe = NULL;
        lib.AHardwareBuffer_lock = NULL;
        lib.AHardwareBuffer_unlock = NULL;
        lib.AHardwareBuffer_sendHandleToUnixSocket = NULL;
        lib.AHardwareBuffer_recvHandleFromUnixSocket = NULL;
    }
    
    bool hwbufferAvailable(LibAndroid& lib) {
        return android_get_device_api_level() >= 26
        && lib.AHardwareBuffer_allocate != NULL
        && lib.AHardwareBuffer_acquire != NULL
        && lib.AHardwareBuffer_release != NULL
        && lib.AHardwareBuffer_describe != NULL
        && lib.AHardwareBuffer_lock != NULL
        && lib.AHardwareBuffer_unlock != NULL
        && lib.AHardwareBuffer_sendHandleToUnixSocket != NULL
        && lib.AHardwareBuffer_recvHandleFromUnixSocket != NULL;
    }
}









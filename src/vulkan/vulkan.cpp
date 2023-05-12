#include "vulkan.hpp"

#include <dlfcn.h>

namespace vk_wrapper {
    State& State::instance() {
        static State s;
        return s;
    }
    
    State::State() {
        failed = false;
        libAndroid = dlopen("android", RTLD_LAZY);
        if (libAndroid == nullptr) {
            failed = true;
            return;
        }
        android_get_device_api_level = (int(*)()) dlsym(libAndroid, "android_get_device_api_level");
        if (android_get_device_api_level == nullptr) {
            failed = true;
            return;
        }
        if (android_get_device_api_level() < 26) {
            failed = true;
            return;
        }
        
        // TODO check for VK_ANDROID_external_memory_android_hardware_buffer and dependent device extensions
        
        
        
        
        
        
    }
    
}









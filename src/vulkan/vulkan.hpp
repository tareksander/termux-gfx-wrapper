#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>

namespace vk_wrapper {
    
    
    
    struct InstanceState {
        
    };
    
    struct DeviceState {
        
    };
    
    
    
    struct State {
        static State& instance();
        
        bool failed;
        
        void* libAndroid;
        
        int (*android_get_device_api_level)();
        
        
        
        
        // Override to expose extensions
        PFN_vkGetDeviceProcAddr vk_get_device_proc_addr;
        PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr;
        
        
        // get with vk_get_device_proc_addr, override when a wayland surface is passed to it
        PFN_vkGetPhysicalDeviceSurfaceSupportKHR vk_get_physical_device_surface_support;
        
        
        
        VkBool32(*vkGetPhysicalDeviceWaylandPresentationSupport)(VkPhysicalDevice d, uint32_t queue, struct wl_display* display);
        // TODO override surface queries
        
        /*
        Architecture:
        - Create hardware buffers the size of the wl_surface
        - send them to the compositor with a custom protocol
        - the compositor maps them as EGLImages to use when compositing
        - the client creates vulkan memory handles from them, and from that image handles
        - vk present sends the index of the hardware buffer to present to the compositor, which gets used after the next commit (which is also send)
        
        
        
        
        */
        
        
        
        
        
        private:
        State(const State&) = delete;
        State(State&&) = delete;
        State& operator=(const State&) = delete;
        State& operator=(State&&) = delete;
        
        State();
    };
}





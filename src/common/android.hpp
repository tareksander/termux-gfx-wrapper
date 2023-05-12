#pragma once

#ifndef __ANDROID__
#include "hardwarebuffer.hpp"
#else
#include <android/hardware_buffer.h>
#endif


namespace libAndroidHelper {
    struct LibAndroid {
        void* libAndroid;
        
        
        // int AHardwareBuffer_allocate(const AHardwareBuffer_Desc* _Nonnull desc, AHardwareBuffer* _Nullable* _Nonnull outBuffer)
        /**
         * Allocates a buffer that matches the passed AHardwareBuffer_Desc.
         *
         * If allocation succeeds, the buffer can be used according to the
         * usage flags specified in its description. If a buffer is used in ways
         * not compatible with its usage flags, the results are undefined and
         * may include program termination.
         *
         * Available since API level 26.
         *
         * \return 0 on success, or an error number of the allocation fails for
         * any reason. The returned buffer has a reference count of 1.
         */
        int (*AHardwareBuffer_allocate)(const AHardwareBuffer_Desc* desc, AHardwareBuffer** outBuffer);
        
        // void AHardwareBuffer_acquire(AHardwareBuffer* _Nonnull buffer)
        /**
         * Acquire a reference on the given AHardwareBuffer object.
         *
         * This prevents the object from being deleted until the last reference
         * is removed.
         *
         * Available since API level 26.
         */
        void (*AHardwareBuffer_acquire)(AHardwareBuffer* buffer);
        
        // void AHardwareBuffer_release(AHardwareBuffer* _Nonnull buffer)
        /**
         * Remove a reference that was previously acquired with
         * AHardwareBuffer_acquire() or AHardwareBuffer_allocate().
         *
         * Available since API level 26.
         */
        void (*AHardwareBuffer_release)(AHardwareBuffer* buffer);
        
        
        // void AHardwareBuffer_describe(const AHardwareBuffer* _Nonnull buffer, AHardwareBuffer_Desc* _Nonnull outDesc)
        /**
         * Return a description of the AHardwareBuffer in the passed
         * AHardwareBuffer_Desc struct.
         *
         * Available since API level 26.
         */
        void (*AHardwareBuffer_describe)(const AHardwareBuffer* buffer, AHardwareBuffer_Desc* outDesc);
        
        
        // int AHardwareBuffer_lock(AHardwareBuffer* _Nonnull buffer, uint64_t usage, int32_t fence, const ARect* _Nullable rect, void* _Nullable* _Nonnull outVirtualAddress)
        /**
         * Lock the AHardwareBuffer for direct CPU access.
         *
         * This function can lock the buffer for either reading or writing.
         * It may block if the hardware needs to finish rendering, if CPU caches
         * need to be synchronized, or possibly for other implementation-
         * specific reasons.
         *
         * The passed AHardwareBuffer must have one layer, otherwise the call
         * will fail.
         *
         * If \a fence is not negative, it specifies a fence file descriptor on
         * which to wait before locking the buffer. If it's negative, the caller
         * is responsible for ensuring that writes to the buffer have completed
         * before calling this function.  Using this parameter is more efficient
         * than waiting on the fence and then calling this function.
         *
         * The \a usage parameter may only specify AHARDWAREBUFFER_USAGE_CPU_*.
         * If set, then outVirtualAddress is filled with the address of the
         * buffer in virtual memory. The flags must also be compatible with
         * usage flags specified at buffer creation: if a read flag is passed,
         * the buffer must have been created with
         * AHARDWAREBUFFER_USAGE_CPU_READ_RARELY or
         * AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN. If a write flag is passed, it
         * must have been created with AHARDWAREBUFFER_USAGE_CPU_WRITE_RARELY or
         * AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN.
         *
         * If \a rect is not NULL, the caller promises to modify only data in
         * the area specified by rect. If rect is NULL, the caller may modify
         * the contents of the entire buffer. The content of the buffer outside
         * of the specified rect is NOT modified by this call.
         *
         * It is legal for several different threads to lock a buffer for read
         * access; none of the threads are blocked.
         *
         * Locking a buffer simultaneously for write or read/write is undefined,
         * but will neither terminate the process nor block the caller.
         * AHardwareBuffer_lock may return an error or leave the buffer's
         * content in an indeterminate state.
         *
         * If the buffer has AHARDWAREBUFFER_FORMAT_BLOB, it is legal lock it
         * for reading and writing in multiple threads and/or processes
         * simultaneously, and the contents of the buffer behave like shared
         * memory.
         *
         * Available since API level 26.
         *
         * \return 0 on success. -EINVAL if \a buffer is NULL, the usage flags
         * are not a combination of AHARDWAREBUFFER_USAGE_CPU_*, or the buffer
         * has more than one layer. Error number if the lock fails for any other
         * reason.
         */
        int (*AHardwareBuffer_lock)(AHardwareBuffer*  buffer, uint64_t usage, int32_t fence, const ARect* rect, void** outVirtualAddress);
        
        
        // int AHardwareBuffer_unlock(AHardwareBuffer* _Nonnull buffer, int32_t* _Nullable fence)
        /**
         * Unlock the AHardwareBuffer from direct CPU access.
         *
         * Must be called after all changes to the buffer are completed by the
         * caller.  If \a fence is NULL, the function will block until all work
         * is completed.  Otherwise, \a fence will be set either to a valid file
         * descriptor or to -1.  The file descriptor will become signaled once
         * the unlocking is complete and buffer contents are updated.
         * The caller is responsible for closing the file descriptor once it's
         * no longer needed.  The value -1 indicates that unlocking has already
         * completed before the function returned and no further operations are
         * necessary.
         *
         * Available since API level 26.
         *
         * \return 0 on success. -EINVAL if \a buffer is NULL. Error number if
         * the unlock fails for any reason.
         */
        int (*AHardwareBuffer_unlock)(AHardwareBuffer* buffer, int32_t* fence);
        
        // int AHardwareBuffer_sendHandleToUnixSocket(const AHardwareBuffer* _Nonnull buffer, int socketFd)
        /**
         * Send the AHardwareBuffer to an AF_UNIX socket.
         *
         * Available since API level 26.
         *
         * \return 0 on success, -EINVAL if \a buffer is NULL, or an error
         * number if the operation fails for any reason.
         */
        int (*AHardwareBuffer_sendHandleToUnixSocket)(const AHardwareBuffer* buffer, int socketFd);
        
        // int AHardwareBuffer_recvHandleFromUnixSocket(int socketFd, AHardwareBuffer* _Nullable* _Nonnull outBuffer)
        /**
         * Receive an AHardwareBuffer from an AF_UNIX socket.
         *
         * Available since API level 26.
         *
         * \return 0 on success, -EINVAL if \a outBuffer is NULL, or an error
         * number if the operation fails for any reason.
         */
        int (*AHardwareBuffer_recvHandleFromUnixSocket)(int socketFd, AHardwareBuffer** outBuffer);
        
    };
    
    void init(LibAndroid& lib);
    
    void deinit(LibAndroid& lib);
    
    bool hwbufferAvailable(LibAndroid& lib);
}






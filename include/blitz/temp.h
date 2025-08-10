#pragma once

#ifdef PLATFORM_WINDOWS
    #include <winsock2.h>
    #include <windows.h>
    using socket_t = SOCKET;
    using instance_t = HANDLE;
#elif PLATFORM_LINUX
    using socket_t = int;
    using instance_t = int;
#endif
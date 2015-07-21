#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#include <stdint.h>

int64_t lithium_milliseconds(){
    SYSTEMTIME t;
    GetSystemTime(&t);

    return (t.wSecond*1000) + t.wMilliseconds;

}

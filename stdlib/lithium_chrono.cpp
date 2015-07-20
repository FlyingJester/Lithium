#include "lithium_chrono.hpp"
#include <cassert>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>

static int64_t milliseconds(){
    SYSTEMTIME t;
    GetSystemTime(&t);

    return (t.wSecond*1000) + t.wMilliseconds;

}

#else

#include <sys/timeb.h>
static int64_t milliseconds(){
    struct timeb t;
    ftime(&t);
    return (t.time*1000) + t.millitm;   
}
#endif

namespace Lithium{
namespace std{
namespace Chrono{

bool TickAccessor(void *a, struct Value &v, Mode mode){
    assert(a==NULL);
    if(mode==Set) return false;
    
    IntegerToValue(v, milliseconds());
    
    return true;
}

} // namespace Math
} // namespace std
} // namespace Lithium

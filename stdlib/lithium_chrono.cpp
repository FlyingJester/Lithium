#include "lithium_chrono.hpp"
#include <cassert>

extern "C" int64_t lithium_milliseconds();

namespace Lithium{
namespace std{
namespace Chrono{

bool TickAccessor(void *a, struct Value &v, Mode mode){
    assert(a==NULL);
    if(mode==Set) return false;
    
    IntegerToValue(v, lithium_milliseconds());
    
    return true;
}

} // namespace Math
} // namespace std
} // namespace Lithium

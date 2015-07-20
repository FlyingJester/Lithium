#include "lithium_math.hpp"
#include <cassert>

namespace Lithium{
namespace std{
namespace Math{

bool PiAccessor(void *a, struct Value &v, Mode mode){
    assert(a==NULL);
    if(mode==Set) return false;
    
    FloatingToValue(v, 3.141592653589793238f);
    
    return true;
}

} // namespace Math
} // namespace std
} // namespace Lithium

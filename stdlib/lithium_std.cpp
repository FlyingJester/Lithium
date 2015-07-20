#include "lithium_std.hpp"
#include "lithium_math.hpp"
#include "lithium_chrono.hpp"

namespace Lithium{
namespace std{

Context 
    *math = NULL, 
    *chrono = NULL;

static void InitMath(){
    math = new Context(NULL);
    math->AddAccessor("Pi", Math::PiAccessor);
}

static void InitChrono(){
    chrono = new Context(NULL);
    chrono->AddAccessor("Ticks", Chrono::TickAccessor);
}

void InitModules(Context *ctx){
    if(math==NULL)
        InitMath();
    if(chrono==NULL)
        InitChrono();
    
    ctx->AddModule("Math", math);
    ctx->AddModule("Chrono", chrono);
    
}

void DestroyModules(){
    if(math)
        delete math;
    math = NULL;

    if(chrono)
        delete chrono;
    chrono = NULL;
}

} // namespace std
} // namespace Lithium

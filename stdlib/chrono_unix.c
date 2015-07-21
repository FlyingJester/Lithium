#include <sys/timeb.h>
#include <stdint.h>

int64_t lithium_milliseconds(){
    struct timeb t;
    ftime(&t);
    return (t.time*1000) + t.millitm;
}

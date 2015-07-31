#pragma once
#include <stdint.h>

namespace Lithium{
namespace Utils{

template<int N>
void GetArray(uint8_t *i, const uint8_t *bytecode, uint64_t &offset){
    i[0] = bytecode[offset++];
    GetArray<N-1>(i+1, bytecode, offset);
}

template<>
void GetArray<0>(uint8_t *, const uint8_t *, uint64_t &){}

template<typename T>
T GetObject(const uint8_t *bytecode, uint64_t &offset){
    union{T t; uint8_t c[sizeof(T)]; } val;
    GetArray<sizeof(T)>(val.c, bytecode, offset);
    return val.t;
}

template<int N>
void WriteArray(const uint8_t *i, uint8_t *bytecode, uint64_t &offset){
    bytecode[offset++] = *i;
    WriteArray<N-1>(i+1, bytecode, offset);
}

template<>
void WriteArray<0>(const uint8_t *, uint8_t *, uint64_t &){}
 
template<typename T>
void WriteObject(const T &t, uint8_t *bytecode, uint64_t &offset){
    union{T t; uint8_t c[sizeof(T)]; } val;
    val.t = t;
    WriteArray<sizeof(T)>(val.c, bytecode, offset);
}

template<typename T>
void WriteObject(const T &t, uint8_t *bytecode){
    uint64_t offset = 0;
    WriteObject<T>(t, bytecode, offset);
}

template<typename T>
struct name_finder{
    name_finder(const std::string &s_)
      : s(s_){}
    const std::string &s;
    bool operator() (const T &that){
        return that.name==s;
    }
    
    struct name_finder &operator=(const name_finder &that){
        return name_finder(that.s);
    }
    
};

template<typename T, typename C>
void AppendObject(const T &obj, C &append_to){
    uint64_t at = append_to.size();
    append_to.resize(at+4);
    Utils::WriteObject<T>(obj, &(append_to.front()), at);
}

} // namespace Utils
} // namespace Lithium

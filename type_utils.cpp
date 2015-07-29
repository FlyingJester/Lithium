#include "lithium.hpp"
#include "strtoll.h"

#if defined(_MSC_VER)

#define SNPrintfShim sprintf_s

#elif defined(__APPLE__)

#define SNPrintfShim snprintf

#else
#include <cstdio>

#define SNPrintfShim(STRING, N, FORMAT, ARGS) sprintf(STRING, FORMAT, ARGS);

#endif

namespace Lithium {

Value::Type MutualCast(Value::Type a, Value::Type b){
    Value::Type mx = std::max(a, b), mn = std::min(a, b);
    
    if(mn==Value::Null) return Value::Null;
    
    switch(mx){
        case Value::Null: return Value::Null;
        case Value::Boolean: return Value::Boolean;
        case Value::Integer: return mn;
        case Value::Floating:
            if(mn==Value::Boolean)
                return Value::Boolean; 
            else 
                return Value::Floating;
        case Value::String: return Value::String;
    }
    return Value::Null;
}

/* Numeric types are directly converted. Strings may convert -- see strtoll. Booleans fail. */
struct Error ValueToInteger(const struct Value &v, int64_t &out){
    struct Error e = {true};
    switch(v.type){
        case Value::Null:
            e.succeeded = false;
            e.error = "Cannot convert null to int";
            break;
        case Value::Boolean:
            e.succeeded = false;
            e.error = "Cannot convert bool to int";
            break;
        case Value::Integer:
            out = v.value.integer;
            break;
        case Value::Floating:
            out = (int64_t)v.value.floating;
            break;
        case Value::String:
            if(!StrToInt64(v.value.string, &out)){
                e.succeeded = false;
                e.error = std::string("Cannot convert string ``") + v.value.string + "'' to int";
            }
            break;
    }
    return e;
}

/* Numeric types are directly converted. Strings may convert -- see strtoll. Booleans fail. */
struct Error ValueToFloating(const struct Value &v, float &out){
    struct Error e = {true};
    switch(v.type){
        case Value::Null:
            e.succeeded = false;
            e.error = "Cannot convert null to float";
            break;
        case Value::Boolean:
            e.succeeded = false;
            e.error = "Cannot convert bool to float";
            break;
        case Value::Integer:
            out = (float)v.value.integer;
            break;
        case Value::Floating:
            out = v.value.floating;
            break;
        case Value::String:
            if(!StrToFloat(v.value.string, &out)){
                e.succeeded = false;
                e.error = std::string("Cannot convert string ``") + v.value.string + "'' to float";
            }
            break;
    }
    return e;
}

/* Anything can be a string... */
struct Error ValueToString(const struct Value &v, std::string &out){
    char buffer[80];
    struct Error e = {true};
    switch(v.type){
        case Value::Null:
            e.succeeded = false;
            e.error = "Cannot convert null to float";
            break;
        case Value::Boolean:
            out.assign(v.value.boolean?"true":"false");
            break;
        case Value::Integer:
            SNPrintfShim(buffer, 79, "%i", (int)v.value.integer);
            out.assign(buffer);
            break;
        case Value::Floating:
            SNPrintfShim(buffer, 79, "%d", (int)v.value.floating);
            out.assign(buffer);
            break;
        case Value::String:
            out.assign(v.value.string);
            break;
    }
    return e;
}

/* Anything can be a boolean... */
struct Error ValueToBoolean(const struct Value &v, bool &out){
    struct Error e = {true};
    switch(v.type){
        case Value::Null:
            e.succeeded = false;
            e.error = "Cannot convert null to float";
            break;
        case Value::Boolean:
            out = v.value.boolean;
            break;
        case Value::Integer:
            out = v.value.integer>0;
            break;
        case Value::Floating:
            out = v.value.floating>0.0f;
            break;
        case Value::String:
            out = (v.value.string!=NULL) && (v.value.string[0]!='\0');
            break;
    }
    return e;
}

void IntegerToValue(struct Value &v, int64_t in){
    v.type = Value::Integer;
    v.value.integer = in;
}

void FloatingToValue(struct Value &v, float in){
    v.type = Value::Floating;
    v.value.floating = in;
}

void StringToValue(struct Value &v, const std::string &in){
    const uint64_t len = in.size();
    
    v.type = Value::String;
    v.value.string = (char *)malloc((size_t)len+1);
    memcpy(v.value.string, in.c_str(), (size_t)len+1);
}

void BooleanToValue(struct Value &v, bool in){
    v.type = Value::Boolean;
    v.value.boolean = in;
}

}

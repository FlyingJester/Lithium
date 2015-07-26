#include "lithium.hpp"
#include "strtoll.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>

#ifdef _MSC_VER

#define SNPrintfShim sprintf_s

#else

#define SNPrintfShim snprintf

#endif

namespace Lithium{

template<typename T>
struct plus {
    T operator() (const T& a, const T&b) const {
        return a+b;
    }
};

template<typename T>
struct minus {
    T operator() (const T& a, const T&b) const {
        return a-b;
    }
};

template<typename T>
struct multiply {
    T operator() (const T& a, const T&b) const {
        return a*b;
    }
};

template<typename T>
struct divide {
    T operator() (const T& a, const T&b) const {
        return a/b;
    }
};

template<typename T>
struct remainder {
    T operator() (const T& a, const T&b) const {
        return a%b;
    }
};

template<>
struct remainder<double> {
    double operator() (const double a, const double b) const {
        return fmod(a, b);
    }
};

template<>
struct remainder<float> {
    float operator() (const float a, const float b) const {
        return fmod(a, b);
    }
};

void Variable::CopyValueString(){
    if(value.type!=Value::String) return;
    
    const uint64_t nl = strlen(value.value.string);
    char * const c = (char *)malloc((size_t)nl+1);
    memcpy(c, value.value.string, (size_t)nl+1);
    value.value.string = c;
}

Variable::~Variable(){
    if(value.type==Value::String) free(value.value.string);
}   

Property::Property(){ name = "<INVALID>"; }

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

Context::Context(){

}
    
Context::Context(void *obj)
  : object(obj){
    
}

Context::~Context(){

}

struct Error Context::AddModule(const std::string &name, Context *ctx){
    struct name_finder<struct Module> finder(name);
    if(std::find_if(modules.cbegin(), modules.cend(), finder)!=modules.cend()){
        const struct Error e = {false, std::string("Module ") + name + " already exists"};
        return e;
    }
    else{
        struct Module m = {name, ctx};
        modules.push_back(m);
        const struct Error e = {true};
        return e;
    }
}

struct Error Context::SetModule(const std::string &name, Context *ctx){
    struct name_finder<struct Module> finder(name);
    std::vector<struct Module>::iterator i = 
        std::find_if(modules.begin(), modules.end(), finder);
    if(i==modules.end()){
        struct Error e = {false, std::string("Module ") + name + " does not exist"};
        return e;
    }
    else{
        i->ctx = ctx;
        struct Error e = {true};
        return e;
    }
}

Context *Context::GetModule(const std::string &name){
    struct name_finder<struct Module> finder(name);
    
    std::vector<struct Module>::const_iterator i = 
        std::find_if(modules.cbegin(), modules.cend(), finder);

    if(i==modules.cend()) return NULL;
    return i->ctx;
}
        
struct Error Context::AddAccessor(const std::string &name, Accessor a){
    struct name_finder<Property> finder(name);
    
    if(std::find_if(accessors.cbegin(), accessors.cend(), finder)!=accessors.cend()){
        const struct Error e = {false, std::string("Accessor ") + name + " already exists"};
        return e;
    }
    else{
        accessors.push_back(Property(name, a));
        const struct Error e = {true};
        return e;
    }
}

Accessor Context::GetAccessor(const std::string &name){

    struct name_finder<Property> finder(name);
    std::vector<Property>::const_iterator i = 
        std::find_if(accessors.cbegin(), accessors.cend(), finder);

    if(i==accessors.cend()) return NULL;
    return i->accessor;
}

struct Error Context::AddVariable(const std::string &name, struct Value &v, unsigned n){
    struct name_finder<Variable> finder(name);
    if(std::find_if(variables.cbegin(), variables.cend(), finder)!=variables.cend()){
        const struct Error e = {false, std::string("Variable ") + name + " already exists"};
        return e;
    }
    else{
        variables.push_back(Variable(name, v, n));
        const struct Error e = {true};
        return e;
    }
}

struct Value Context::GetVariable(const std::string &name){
    struct name_finder<Variable> finder(name);
    std::vector<Variable>::const_iterator i = 
        std::find_if(variables.cbegin(), variables.cend(), finder);
    if(i==variables.cend()){
        struct Value v = {Value::Null};
        return v;
    }
    else{
        return i->value;
    }
}

struct Error Context::SetVariable(const std::string &name, const struct Value &v){
    struct name_finder<Variable> finder(name);
    std::vector<Variable>::iterator i = 
        std::find_if(variables.begin(), variables.end(), finder);
    if(i==variables.end()){
        struct Error e = {false, std::string("Variable ") + name + " does not exist"};
        return e;
    }
    else{
        i->value = v;
        struct Error e = {true};
        return e;
    }
}

struct Value Context::GetProperty(const std::string &name){
    Accessor a = GetAccessor(name);
    struct Value v = {Value::Null};

    if(a) a(object, v, Get);

    return v;
}

struct Error Context::SetProperty(const std::string &name, const struct Value &v){
    Accessor a = GetAccessor(name);
    
    if(a){
        struct Value temp = v;
        a(object, temp, Set);
        struct Error e = {true};
        return e;
    }
    else{
        struct Error e = {false, std::string("Property ") + name + " does not exist"};
        return e;
    }
}

class Parse {
    unsigned scope; // This should never be needed outside the parse itself.
public:
    
    struct Error err;
    
    static bool IsWhitespace(char c){
        return c==' ' || c=='\t' || c=='\n' || c=='\r';
    }
    
    static bool IsSyntax(char c){
        return (c>=35 && c<=47 && c!=46) || (c>=91 && c<=96) || (c>=123 && c<=126);
    }
    
    static void SkipWhitespace(std::string::const_iterator &i, const std::string::const_iterator end){
        while(i!=end && IsWhitespace(*i)) i++;
    }   
    
    static std::string GetIdentifier(std::string::const_iterator &i, const std::string::const_iterator end){
        SkipWhitespace(i, end);
        const std::string::const_iterator start = i;
        while(i!=end && !IsWhitespace(*i) && !IsSyntax(*i)) i++;
        return std::string(start, i);
    }
    
    template<typename T, Value::Type To>
    bool Arithmetic(struct Value &first, const struct Value &second){
        T t;
        if(To==Value::Integer){
            int64_t n;
            err = ValueToInteger(second, n);
            if(!err.succeeded){
                err.error = std::string("Cannot perform arithmetic: ") + err.error;
                return false;
            }
            first.value.integer = t(first.value.integer, (int64_t)n);
        }
        else if(To==Value::Floating){
            float n;
            err = ValueToFloating(second, n);
            if(!err.succeeded){
                err.error = std::string("Cannot perform arithmetic: ") + err.error;
                return false;
            }
            first.value.floating = t(first.value.floating, n);
        }
        else{
            err.succeeded = false;
            err.error = std::string("Invalid arithmetic type");
            return false;
        }
        
        return true;
    }
    
    template<template<typename> class T>
    inline bool CastingTypedArithmetic(struct Value &first, const struct Value &second, const char *noun = "arithmetic", const char *verb = "calculate"){
        switch(first.type){
            case Value::Null:
                err.succeeded = false;
                err.error = "Invalid Null expression in ";
                err.error += noun;
            return false;
            case Value::Boolean:
                err.succeeded = false;
                err.error = std::string("Cannot ") + verb + " boolean expressions";
            return false;
            case Value::Integer:
                Arithmetic<T<int64_t>, Value::Integer>(first, second);
            return true;
            case Value::Floating:
                Arithmetic<T<float>, Value::Floating>(first, second);
            return true; 
            case Value::String:
                err.succeeded = false;
                err.error = std::string("Cannot ") + verb + " string expressions";
            return true;
        }
        return true;
    }
    
    struct Value Factor(Context *ctx, std::string::const_iterator &i, const std::string::const_iterator end){
        
        SkipWhitespace(i, end);
        std::string value = GetIdentifier(i, end);
        
        struct Value v = {Value::Null};
        
        /* If all of value is decimal digits and *i is a '.', then we should append the next identifier */
        if((*i)=='.' && std::find_if_not(value.cbegin(), value.cend(), IsDecDigit)==value.cend() && !IsWhitespace(*(i+1))){
            value += '.';
            i++;
            value+=GetIdentifier(i, end);
            SkipWhitespace(i, end);
        }
        
        SkipWhitespace(i, end);
                
        err.succeeded = true;
        
        if(IsDecDigit(value[0])){
            if(find(value.cbegin(), value.cend(), '.')!=value.cend()){
                v.type = Value::Floating;
                if(!StrToFloat(value.c_str(), &v.value.floating)){
                    err.succeeded = false;
                    err.error = "Invlalid floating point literal \"";
                    err.error += value + '"';
                }
            }
            else{
                v.type = Value::Integer;
                if(!StrToInt64(value.c_str(), &v.value.integer)){
                    err.succeeded = false;
                    err.error = "Invlalid integer literal \"";
                    err.error += value + '"';
                }
            }
        }
        else if(value[0]=='"'){
            v.type = Value::String;
            v.value.string = (char *)malloc(value.size());
            memcpy(v.value.string, value.c_str()+1, value.size()-2);
            v.value.string[value.size()-1] = 0;
        }
        else if(value[0]=='('){
            
        }
        else if(value=="true"){
            v.type = Value::Boolean;
            v.value.boolean = true;
        }
        else if(value=="false"){
            v.type = Value::Boolean;
            v.value.boolean = false;
        }
        else if(value=="get"){
            const std::string ident = GetIdentifier(i, end);
            SkipWhitespace(i, end);
            
            if(ident=="local"){
                const std::string variable_name = GetIdentifier(i, end);
                SkipWhitespace(i, end);
                v = ctx->GetVariable(variable_name);
                if(v.type==Value::Null){
                    err.succeeded = false;
                    err.error = "Undefined Variable \"";
                    err.error += variable_name + '"';
                }
            }
            else{
                v = ctx->GetProperty(ident);
                if(v.type==Value::Null){
                    err.succeeded = false;
                    err.error = "Undefined Property \"";
                    err.error += ident + '"';
                }
            }
        }
        else if(value=="from"){
            const std::string module_name = GetIdentifier(i, end);
            SkipWhitespace(i, end);
            
            Context *module = ctx->GetModule(module_name);
            if(!module){
                err.succeeded = false;
                err.error = "No Such Module \"";
                err.error +=module_name + '"';
                return v;
            }
            
            std::string ident = GetIdentifier(i, end);
            SkipWhitespace(i, end);
            
            if(ident=="get"){
                ident = GetIdentifier(i, end);
                SkipWhitespace(i, end);
            }
            
            if(ident=="local"){
                err.succeeded = false;
                err.error = "Cannot get value \"local\" of remote object";
                return v;
            }
            else{
                v = module->GetProperty(ident);
                if(v.type==Value::Null){
                    err.succeeded = false;
                    err.error = "Undefined Property \"";
                    err.error += ident + '"';
                }
            }
        }
        else if(value=="local"){
            const std::string ident = GetIdentifier(i, end);
            SkipWhitespace(i, end);
            v = ctx->GetVariable(ident);
            if(v.type==Value::Null){
                err.succeeded = false;
                err.error = "Undefined Variable \"";
                err.error += ident + '"';
            }
        }
        else{
            err.succeeded = false;
            err.error = "Expected literal, sub-expression, or access at \"";
            err.error+=value + '"';
        }

        return v;
    }
    
    struct Value Term(Context *ctx, std::string::const_iterator &i, const std::string::const_iterator end){
    
        err.succeeded = true;
        SkipWhitespace(i, end);
        struct Value first = Factor(ctx, i, end);
        SkipWhitespace(i, end);
        
        if(!err.succeeded) return first;
        
        while((*i)=='*' || (*i)=='/' || (*i)=='%'){
            char w = *i;
            i++;
            SkipWhitespace(i, end);
        
            struct Value second = Factor(ctx, i, end);
            if(w=='*'){
                CastingTypedArithmetic<multiply>(first, second, "multiplication", "multiply");
            }
            if(w=='/'){
                CastingTypedArithmetic<divide>(first, second, "division", "divide");
            }
            if(w=='%'){
                CastingTypedArithmetic<remainder>(first, second, "remainder", "modulus");
            }
            
            if(second.type==Value::String) free(second.value.string);
            
        }
        
        return first;
    }
    
    void CleanScope(Context *ctx){
        /* Clean up the stack. */
        std::vector<Variable>::const_iterator i = ctx->variables.cbegin();
        while(i!=ctx->variables.cend() && i->scope<scope) i++;
        ctx->variables.erase(i, ctx->variables.cend());
    }
    
    static void SkipScope(std::string::const_iterator &i, const std::string::const_iterator end){
        unsigned l_scope = 0;
        while(i!=end && (*i)!='.' && l_scope!=0){
            switch(*i){
                case '"':
                    *i++;
                    while(i!=end && (*i!='"') && (*(i-1)!='\\')){
                        i++;
                    }
                    break;
                case ':': l_scope++; break;
                case '.': l_scope--; break;
            }
            i++;
        }

    }
    
    void Scope(Context *ctx, std::string::const_iterator &i, const std::string::const_iterator end){
        scope++;
        while(i!=end && (*i)!= '.'){
            SkipWhitespace(i, end);
            Statement(ctx, i, end);
            SkipWhitespace(i, end);
            if(i==end){
                err.succeeded = false;
                err.error = "Unexpected end of input before end of scope";
                return;
            }
        }
        /* Move off the '.' */
        i++;
        SkipWhitespace(i, end);

        CleanScope(ctx);
        
        scope--;

    }
    
    void If(Context *ctx, std::string::const_iterator &i, const std::string::const_iterator end){
        SkipWhitespace(i, end);
        
        struct Value v = Expression(ctx, i, end);
                
        if(!err.succeeded) return;
        
        bool c;
        struct Error e = ValueToBoolean(v, c);
        
        if(!e.succeeded){
            err = e;
        }
        else{
            err.succeeded = true;
            SkipWhitespace(i, end);
            if(*i!=':'){
                err.succeeded = false;
                err.error = "Expected ':'";
                return;
            }
            
            i++;
            
            SkipWhitespace(i, end);
            if(c){
                Scope(ctx, i, end);
            }
            else{
                SkipScope(i, end);
                if(i==end){
                    err.succeeded = false;
                    err.error = "Unexpected end of input before end of scope";
                    return;
                }
            }
        }
    }
    
    void Loop(Context *ctx, std::string::const_iterator &i, const std::string::const_iterator end){
        /* This is a bit of a hack... */
        SkipWhitespace(i, end);
        const std::string::const_iterator conditional_start = i;
        struct Value v = Expression(ctx, i, end);
        const std::string::const_iterator conditional_end  = i;
        const std::string conditional(conditional_start, conditional_end);
        
        SkipWhitespace(i, end);
        
        if((*i)!=':'){
            err.succeeded = false;
            err.error = "Expected ':'";
            return;
        }
        
        i++;

        SkipWhitespace(i, end);

        const std::string::const_iterator scope_start  = i;        
        bool c;
        err = ValueToBoolean(v, c);
        if(!err.succeeded)
            return;
        
        while(c){
            i = scope_start;
            Scope(ctx, i, end);
            i = conditional_start;
            v = Expression(ctx, i, conditional_end);
            i = scope_start;
            err = ValueToBoolean(v, c);
            if(!err.succeeded)
                return;
        }
    }
    
    struct Value Expression(Context *ctx, std::string::const_iterator &i, const std::string::const_iterator end){
        err.succeeded = true;
        struct Value first = Term(ctx, i, end);
        
        if(!err.succeeded) return first;
                
        SkipWhitespace(i, end);
        
        while((*i)=='-' || (*i)=='+'){
        
            char w = *i;
            i++;
            SkipWhitespace(i, end);
        
            struct Value second = Term(ctx, i, end);
            if(w=='+'){
                if(first.type==Value::String){
                    std::string s;
                    err = ValueToString(second, s);

                    if(err.succeeded){
                        const uint64_t l = strlen(first.value.string);
                        first.value.string = (char *)realloc(first.value.string, (size_t)(l+s.size()+1));
                        memcpy(first.value.string+l, s.c_str(), s.size()+1);
                    }
                }
                else{
                    CastingTypedArithmetic<plus>(first, second, "addition", "add");
                }
            }
            else if(w=='-'){
                CastingTypedArithmetic<minus>(first, second, "subtraction", "subtract");
            }

            if(second.type==Value::String) free(second.value.string);
            
            if(!err.succeeded) 
                break;
            else        
                SkipWhitespace(i, end);
            
        }
        
        SkipWhitespace(i, end);
        return first;
    }

    struct Value Int(Context *ctx, std::string::const_iterator &i, const std::string::const_iterator end){
        SkipWhitespace(i, end);
        const std::string name = GetIdentifier(i, end);
        SkipWhitespace(i, end);
        
        struct Value v = {Value::Null};
        
        if(i!=end){
            v = Expression(ctx, i, end);
            if(err.succeeded){
                struct Value new_value = {Value::Integer};
                struct Error e = ValueToInteger(v, new_value.value.integer);
                if(!e.succeeded){
                    err = e;
                }
                else{
                    err = ctx->AddVariable(name, new_value, scope);
                    if(err.succeeded)
                        printf("Created integer variable %s with value %i\n", name.c_str(), (int)new_value.value.integer);
                    
                }
            }
        }

        return v;
    }
    
    struct Value Set(Context *ctx, std::string::const_iterator &i, const std::string::const_iterator end){
        SkipWhitespace(i, end);
        const std::string name = GetIdentifier(i, end);
        SkipWhitespace(i, end);

        struct Value v = {Value::Null};

        if(name=="local"){
        
            SkipWhitespace(i, end);
            const std::string variable_name = GetIdentifier(i, end);
            SkipWhitespace(i, end);
            v = Expression(ctx, i, end);
            if(err.succeeded){
                err = ctx->SetVariable(variable_name, v);
            }
        }
        else{
            
            v = Expression(ctx, i, end);
            if(err.succeeded){
                err = ctx->SetProperty(name, v);
            }
        }
        return v;
    }
    
    struct Value To(Context *ctx, std::string::const_iterator &i, const std::string::const_iterator end){
        SkipWhitespace(i, end);
        const std::string module_name = GetIdentifier(i, end);
        SkipWhitespace(i, end);
        
        struct Value v = {Value::Null};

        Context *module = ctx->GetModule(module_name);
        if(!module){
            err.succeeded = false;
            err.error = "No Such Module \"";
            err.error +=module_name + '"';
            return v;
        }
        
        const std::string name = GetIdentifier(i, end);
        SkipWhitespace(i, end);
        
        if(name=="local"){
            err.succeeded = false;
            err.error = "Cannot set value \"local\" of remote object";
            
        }
        else{
            v = Expression(ctx, i, end);
            if(err.succeeded){
                err = module->SetProperty(name, v);
            }
        }
        return v;
    }
    
    bool Statement(Context *ctx, std::string::const_iterator &i, const std::string::const_iterator end){
        SkipWhitespace(i, end);
        const std::string word = GetIdentifier(i, end);
        SkipWhitespace(i, end);
        
        if(word=="int"){
            Int(ctx, i, end);
        }
        else if(word=="if"){
            If(ctx, i, end);   
        }
        else if(word=="loop"){
            If(ctx, i, end);   
        }
        else if(word=="set"){
            Set(ctx, i, end);
        }
        else if(word=="to"){
            To(ctx, i, end);
        }
        else{
            err.succeeded = false;
            err.error = "Expected statement at \"";
            err.error+= word + '"';
            return false;
        }
        
        return true;
    }

};

struct Error Context::Execute(const std::string &s){
    
    std::string::const_iterator i = s.cbegin();
    const std::string::const_iterator end = s.cend();
    
    Parse parser;
    
    parser.SkipWhitespace(i, end);
    
    parser.err.succeeded = true;
    
    while(i!=end && parser.err.succeeded){
        parser.Statement(this, i, end);
    }
    
    if(!parser.err.succeeded){
        puts(parser.err.error.c_str());
    }
    
    return parser.err;
}

    
} // namespace WCL

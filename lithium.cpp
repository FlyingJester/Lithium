#include "lithium.hpp"
#include "bytecode_utils.hpp"
#include "strtoll.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>

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

Context::Context(){

}
    
Context::Context(void *obj)
  : object(obj){
    
}

Context::~Context(){

}

struct Error Context::AddModule(const std::string &name, Context *ctx){
    if(GetModule(name)){
        const struct Error e = {false, std::string("Module ") + name + " already exists"};
        return e;
    }
    else{
        modules[name] = ctx;
        const struct Error e = {true};
        return e;
    }
}

struct Error Context::RemoveModule(const std::string &name){
    std::map<std::string, Context *>::iterator i = modules.find(name);
    if(i==modules.end()){
        struct Error e = {false, std::string("Module ") + name + " does not exist"};
        return e;
    }
    else{
        struct Error e = {true};
        modules.erase(i);
        return e;
    }
}

struct Error Context::SetModule(const std::string &name, Context *ctx){
    std::map<std::string, Context *>::iterator i = modules.find(name);
    if(i==modules.end()){
        struct Error e = {false, std::string("Module ") + name + " does not exist"};
        return e;
    }
    else{
        i->second = ctx;
        struct Error e = {true};
        return e;
    }
}

Context *Context::GetModule(const std::string &name){

    std::map<std::string, Context *>::const_iterator i = modules.find(name);
    if(i==modules.end()) return NULL;
    return i->second;
}
        
struct Error Context::AddAccessor(const std::string &name, Accessor a){
    
    if(GetAccessor(name)){
        const struct Error e = {false, std::string("Accessor ") + name + " already exists"};
        return e;
    }
    /* else */ {
        accessors[name] = a;
        const struct Error e = {true};
        return e;
    }
}

Accessor Context::GetAccessor(const std::string &name){

    std::map<std::string, Accessor>::iterator i = accessors.find(name);

    if(i==accessors.end()) return NULL;
    return i->second;
}

struct Error Context::AddVariable(const std::string &name, struct Value &v){
    if(GetVariable(name).type!=Value::Null){
        const struct Error e = {false, std::string("Variable ") + name + " already exists"};
        return e;
    }
    /* else */ {
        variables[name] = v;
        const struct Error e = {true};
        return e;
    }
}

struct Value Context::GetVariable(const std::string &name){
    std::map<std::string, struct Value>::iterator i = variables.find(name);
    if(i==variables.end()){
        struct Value v = {Value::Null};
        return v;
    }
    else{
        return i->second;
    }
}

struct Error Context::SetVariable(const std::string &name, const struct Value &v){
    std::map<std::string, struct Value>::iterator i = variables.find(name);
    if(i==variables.end()){
        struct Error e = {false, std::string("Variable ") + name + " does not exist"};
        return e;
    }
    else{
        i->second = v;
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

uint32_t Context::VerifyString(const std::string &str){
    uint32_t i = 0;
    while((i<string_table.size()) && (string_table[i] != str))
        i++;

    if(i==string_table.size())
        string_table.push_back(str);
    return i;
}

void Context::VerifyAndWriteStringIndex(const std::string &str){
    Utils::AppendObject<int32_t>(VerifyString(str), token_code);
}

class Parse {
    unsigned scope; // This should never be needed outside the parse itself.
public:
    
    struct Error err;
    
    static bool IsWhitespace(char c){
        return c==' ' || c=='\t' || c=='\n' || c=='\r';
    }
    
    static bool IsSyntax(char c){
        return (c>=35 && c<=47 && c!=46) || (c>=91 && c<=96) || (c>=123 && c<=126) || (c>=58 && c<=64);
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
    
    static bool NotIsDecDigit(char c){
        return !IsDecDigit(c);
    }
    
    struct Value Factor(Context *ctx, std::string::const_iterator &i, const std::string::const_iterator end){
        
        SkipWhitespace(i, end);
        std::string value = GetIdentifier(i, end);
        
        struct Value v = {Value::Null};
        
        /* If all of value is decimal digits and *i is a '.', then we should append the next identifier */
        if((*i)=='.' && std::find_if(value.begin(), value.end(), NotIsDecDigit)==value.end() && !IsWhitespace(*(i+1))){
            value += '.';
            i++;
            value+=GetIdentifier(i, end);
            SkipWhitespace(i, end);
        }
        
        SkipWhitespace(i, end);
                
        err.succeeded = true;
        
        if(IsDecDigit(value[0])){
            if(find(value.begin(), value.end(), '.')!=value.end()){
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
        else if(value[0]=='(' && value.size()==1){
            
            v = Expression(ctx, i, end);
            
            SkipWhitespace(i, end);
            
            if(i!=end && (*i)!=')'){
                err.succeeded = false;
                err.error = "Expected ')'";
                return v;
            }
            
            i++;
            SkipWhitespace(i, end);
            return v;
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
    
    static void SkipScope1(std::string::const_iterator &i, const std::string::const_iterator end){
        unsigned l_scope = 0;
        
        do{
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
        }while(i!=end && l_scope!=0);
    }
    
    static void SkipScope(std::string::const_iterator &i, const std::string::const_iterator end){
        while(i!=end && (*i)!='.'){
            if((*i)=='"'){
                i++;
                while(i!=end && (*i)!='"')
                    i++;
                if(i==end) return;
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

// TODO: scoping!
//        CleanScope(ctx);
        
        scope--;

    }
    
    void If(Context *ctx, std::string::const_iterator &i, const std::string::const_iterator end){
        SkipWhitespace(i, end);
        
        const std::string::const_iterator i_1 = i;
        
        struct Value v = Expression(ctx, i, end);
        
        const std::string conditional(i_1, i);
        
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
                err.error = "Expected ':' after ";
                err.error += conditional;
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
                if((*i)!='.'){
                    err.succeeded = false;
                    err.error = "Expected '.' at '";
                    err.error += *i;
                    err.error += '\'';
                    return;
                }
                
                /* Move off the '.' */
                i++;
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
            err.error = "Expected ':' after ";
            err.error += conditional;
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
// TODO: Scoping
                    err = ctx->AddVariable(name, new_value);
//                    if(err.succeeded)
//                        printf("Created integer variable %s with value %i\n", name.c_str(), (int)new_value.value.integer);
                    
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
    
    std::string::const_iterator i = s.begin();
    const std::string::const_iterator end = s.end();
    
    Parse parser;
    
    parser.SkipWhitespace(i, end);
    
    parser.err.succeeded = true;
    
    while(i!=end && parser.err.succeeded){
        parser.Statement(this, i, end);
    }
    
    if(!parser.err.succeeded){
//        puts(parser.err.error.c_str());
    }
    
    return parser.err;
}

    
} // namespace WCL

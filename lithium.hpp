#pragma once
#include <string>
#include <cstring>
#include <cstdint>
#include <vector>

namespace Lithium{

    struct Value{
        enum Type {Null, Boolean, Integer, Floating, String};
        Type type;
        union{
            int64_t integer;
            float floating;
            char *string;
            bool boolean;
        } value;
    };
    
    Value::Type MutualCast(Value::Type a, Value::Type b);
    inline Value::Type MutualCast(const struct Value &a, const struct Value &b){ return MutualCast(a.type, b.type); }

    class Variable{
        Variable(){}
    public:
        struct Value value;
        std::string name;
        unsigned scope;
       
        void CopyValueString();
       
        Variable(const std::string &n, const struct Value &v, unsigned s)
          : value(v), name(n), scope(s){
            CopyValueString();
        }
              
        Variable(const char *n, const struct Value &v, unsigned s)
          : value(v), name(n), scope(s){
            CopyValueString();
        }
        
        
        ~Variable();
    };
    
    enum Mode {Set, Get};
    
    typedef bool(*Accessor)(void *a, struct Value &v, Mode mode);
    
    class Property{
        Property();
    public:
        Accessor accessor;
        std::string name;
        
        Property(const std::string &n, Accessor a)
          : accessor(a), name(n){ }

    };
   
    struct Error ValueToInteger(const struct Value &v, int64_t &out);
    struct Error ValueToFloating(const struct Value &v, float &out);
    struct Error ValueToString(const struct Value &v, std::string &out);
    struct Error ValueToBoolean(const struct Value &v, bool &out);

    void IntegerToValue(struct Value &v, int64_t in);
    void FloatingToValue(struct Value &v, float in);
    void StringToValue(struct Value &v, const std::string &in);
    void BooleanToValue(struct Value &v, bool in);
   
    struct Error{
        bool succeeded;
        std::string error;
    };
    
    /* A context roughly associates with a single type of object. */
    class Context{
        Context();
        
        struct Module { std::string name; Context *ctx; };
        
        std::vector<Variable> variables;
        std::vector<Property> accessors;
        std::vector<struct Module> modules;
        void *object;
        
        struct Error AddVariable(const std::string &name, struct Value &v, unsigned scope);
        
    public:
        
        friend class Parse;
        
        Context(void *obj);
        ~Context();
        
        struct Error AddModule(const std::string &name, Context *ctx);

        struct Error SetModule(const std::string &name, Context *ctx);
        Context *GetModule(const std::string &name);
        
        /* Accessors only operate on properties */
        struct Error AddAccessor(const std::string &name, Accessor);

        struct Error SetAccessor(const std::string &name, Accessor);
        Accessor GetAccessor(const std::string &name);

        inline struct Error AddVariable(const std::string &name, struct Value &v){
            return AddVariable(name, v, 0);
        }

        struct Value GetVariable(const std::string &name);
        struct Error SetVariable(const std::string &name, const struct Value &v);

        struct Value GetProperty(const std::string &name);
        struct Error SetProperty(const std::string &name, const struct Value &v);

        struct Error Execute(const std::string &s);
    
    };

}

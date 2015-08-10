#pragma once
#include <string>
#include <cstring>
#include <stdint.h>
#include <vector>
#include <map>

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
    
    enum Mode {Set, Get};
    
    typedef bool(*Accessor)(void *a, struct Value &v, Mode mode);
    
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
        
        std::vector<uint8_t> token_code;
        std::map<std::string, uint64_t> token_jump_table;
        std::map<std::string, uint64_t> token_procedure_table;
        /* The string table is a list of immutable strings, 
            such as string constants and variable names */
        std::vector<std::string> string_table;

        std::map<std::string, struct Value> variables;
        std::map<std::string, Accessor> accessors;
        std::map<std::string, Context *> modules;
        void *object;
        
        uint32_t VerifyString(const std::string &str);
        void VerifyAndWriteStringIndex(const std::string &str);
        
        struct Error AddVariable(const std::string &name, struct Value &v);
        
        inline void AddTok(uint8_t t){ token_code.push_back(t); }
        
    public:
        
        friend class Parse;
        
        Context(void *obj);
        ~Context();
        
        struct Error AddModule(const std::string &name, Context *ctx);
        struct Error RemoveModule(const std::string &name);

        struct Error SetModule(const std::string &name, Context *ctx);
        Context *GetModule(const std::string &name);
        
        /* Accessors only operate on properties */
        struct Error AddAccessor(const std::string &name, Accessor);

        struct Error SetAccessor(const std::string &name, Accessor);
        Accessor GetAccessor(const std::string &name);

        struct Value GetVariable(const std::string &name);
        struct Error SetVariable(const std::string &name, const struct Value &v);

        struct Value GetProperty(const std::string &name);
        struct Error SetProperty(const std::string &name, const struct Value &v);

        struct Error Execute(const std::string &s);
    
    };

}

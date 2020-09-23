#ifndef XMEM_ARGS_H
#define XMEM_ARGS_H

#include <initializer_list>

#include "hash_map.h"
#include "string.h"
#include "vector.h"


namespace xmem {


class ArgParser {
public:
    struct Entry {
        String longKey;
        String placeholder;
        String* value;
        String help;

        Entry(
            String longKey,
            String placeholder,
            String* value,
            String help
        )
            : longKey(std::move(longKey))
            , placeholder(std::move(placeholder))
            , value(value)
            , help(std::move(help))
        {}
    };

public:
    ArgParser(std::initializer_list<Entry> entries);

    void parse(int argc, char* argv[]) const;

    void fixed_free_args(Vector<String*> free_args) {
        fixed_free_args_ = std::move(free_args);
    }

private:
    HashMap<String, String*> keyToValue;
    String help;
    Vector<String*> fixed_free_args_;
};


}


#endif

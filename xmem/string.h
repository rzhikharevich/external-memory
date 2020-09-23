#ifndef XMEM_STRING_H
#define XMEM_STRING_H


#include <string>

#include "prelude.h"
#include "string_common.h"


namespace xmem {


class String : public StringCommon<String>, public std::string {
private:
    using Super = std::string;

public:
    using Super::Super;
    using Super::operator=;
};


}


template <>
struct std::hash<xmem::String> {
    usize operator()(const xmem::String& s) const {
        return std::hash<std::string>()(s);
    }
};


#endif

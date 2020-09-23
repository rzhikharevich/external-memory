#ifndef XMEM_STRING_COMMON_H
#define XMEM_STRING_COMMON_H

#include <cstring>


namespace xmem {


template <typename Self>
class StringCommon {
public:
    bool starts_with(const Self& prefix) const {
        if (prefix.size() > self().size()) {
            return false;
        }

        return std::memcmp(self().data(), prefix.data(), prefix.size()) == 0;
    }

private:
    const Self& self() const {
        return static_cast<const Self&>(*this);
    }
};


}


#endif

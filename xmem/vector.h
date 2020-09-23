#ifndef XMEM_VECTOR_H
#define XMEM_VECTOR_H

#include <vector>


namespace xmem {


template <typename Value>
class Vector : public std::vector<Value> {
private:
    using Super = std::vector<Value>;

public:
    using Super::Super;
};


}


#endif

#ifndef XMEM_TYPE_TRAITS_H
#define XMEM_TYPE_TRAITS_H

#include <type_traits>


namespace xmem {


template <typename T>
using remove_cvref_t = std::remove_volatile_t<std::remove_const_t<T>>;


}


#endif

#ifndef XMEM_EXCEPTION_H
#define XMEM_EXCEPTION_H

#include <exception>
#include <sstream>
#include <utility>

#include "string.h"
#include "type_traits.h"


namespace xmem {


class Exception;


template <typename E, class T>
std::enable_if_t<
    std::is_same<remove_cvref_t<E>, Exception>::value,
    E&&
> operator<<(E&& exc, const T& x);


class Exception : public std::exception {
    template <typename E, class T>
    friend std::enable_if_t<
        std::is_same<remove_cvref_t<E>, Exception>::value,
        E&&
    > operator<<(E&& exc, const T& x);

public:
    const char* what() const noexcept override {
        return Str.data();
    }

private:
    String Str;
};


template <typename E, class T>
std::enable_if_t<
    std::is_same<remove_cvref_t<E>, Exception>::value,
    E&&
> operator<<(E&& exc, const T& x) {
    std::ostringstream stream;
    stream << x;
    exc.Str += stream.str();
    return std::forward<E>(exc);
}


#define XMEM_ENSURE(cond, msg) \
    if (!(cond)) throw (xmem::Exception() << msg);


}


#endif

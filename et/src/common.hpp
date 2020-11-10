#ifndef UUID_73051518_6403_4F4E_97A5_BE49AD1C26A8
#define UUID_73051518_6403_4F4E_97A5_BE49AD1C26A8

#include <boost/container/vector.hpp>

#include <algorithm>
#include <cassert>
#include <functional>
#include <type_traits>
#include <utility>

// We are going to support arbitrary element types in our vector,
// including custom non-trivially copyable types. We have to consider:
// - that element types are arbitrary complex and should be perfectly
//   forwarded everywhere.
// - addition and multiplication doesn't have to be commutative.
// - optimizing division to multiplication by inverse may be invalid.

// Type of result of applying binary operation Op to T and U.

template<typename Op,typename T,typename U>
using op_result_t = decltype(Op{}(std::declval<T>(),std::declval<U>()));

// Calculate inverse of x when it's possible to optimize a/b to a*(b^(-1)).
// We prefer to not specialize this for integers to return floats as that will give
// a different result type (int/int=double) which make sence for math
// (and Python), but not C++.

template<typename T,
         typename = std::enable_if_t<std::is_floating_point_v<T>>>
constexpr T multiplicative_inverse(T x)
{
    return T(1)/x;
}

// A trait that figures out if multiplication_inverse is callable for given T.

template<typename T,typename = void>
struct has_multiplicative_inverse : std::false_type {};

template<typename T>
struct has_multiplicative_inverse<T,
    std::void_t<decltype(multiplicative_inverse(std::declval<T>()))>> : std::true_type {};

template<typename T>
constexpr inline bool has_multiplicative_inverse_v = has_multiplicative_inverse<T>{};

#endif

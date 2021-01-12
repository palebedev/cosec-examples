#ifndef UUID_3A387A6A_7DE4_4EDC_BB28_BDC6013EDB81
#define UUID_3A387A6A_7DE4_4EDC_BB28_BDC6013EDB81

#include "common.hpp"

#include <iterator>

namespace naive
{
    template<typename T>
    class vector : public boost::container::vector<T>
    {
    public:
        using boost::container::vector<T>::vector;
    };

    // Type of our vector as a result of applying Op to element types T and U.

    template<typename Op,typename T,typename U>
    using vec_result_t = vector<op_result_t<Op,T,U>>;

    // Operators are free functions for symmetry to allow operations
    // between vectors of mixed types which should follow the
    // same rules operations on mixed scalar types work.

    // We shouldn't use auto as return type for proper SFINAE:
    // vectors are addable, if their element types have a valid operator+.
    template<typename T,typename U>
    vec_result_t<std::plus<>,const T&,const U&> operator+(const vector<T>& v1,const vector<U>& v2)
    {
        assert(v1.size()==v2.size());
        vec_result_t<std::plus<>,const T&,const U&> ret(v1.size(),boost::container::default_init);
        std::transform(v1.begin(),v1.end(),v2.begin(),ret.begin(),std::plus{});
        return ret;
    }

    // A SFINAE trait that checks if the result of applying Op to T and U
    // can be stored without conversion in container of Ret.
    template<typename Ret,typename Op,typename T,typename U>
    using can_reuse_for_t = std::enable_if_t<std::is_same_v<Ret,
        std::remove_cvref_t<op_result_t<Op,T,U>>>>;

    // If the first argument can be moved from and is the same type as the
    // result should be, we can use it to avoid allocating new memory for result.
    template<typename T,typename U,
             typename = can_reuse_for_t<T,std::plus<>,T,const U&>>
    vec_result_t<std::plus<>,T,const U&> operator+(vector<T>&& v1,const vector<U>& v2)
    {
        assert(v1.size()==v2.size());
        std::transform(std::move_iterator{v1.begin()},std::move_iterator{v1.end()},
                       v2.begin(),v1.begin(),std::plus{});
        // FIXME: Since C++20, rvalue reference are also implicitly moveable from for return,
        // but clang doesn't implement this yet.
        return std::move(v1);
    }

    // In general we can't assume operator+ is commutative,
    // so we can't delegate to previous implementation.
    template<typename T,typename U,
             typename = can_reuse_for_t<U,std::plus<>,const T&,U>>
    vec_result_t<std::plus<>,const T&,U> operator+(const vector<T>& v1,vector<U>&& v2)
    {
        assert(v1.size()==v2.size());
        std::transform(v1.begin(),v1.end(),std::move_iterator{v2.begin()},v2.begin(),std::plus{});
        return v2;
    }

    // If both vectors are rvalues of same element type, both of the previous overloads
    // are viable, but neither is better then the other (and first one).
    // We provide a tie breaker for this case only, so we can assume T=U=common_type(T,U).
    template<typename T,
             typename = can_reuse_for_t<T,std::plus<>,T,T>>
    vec_result_t<std::plus<>,T,T> operator+(vector<T>&& v1,vector<T>&& v2)
    {
        return std::move(v1)+v2;
    }

    // Same for for subtraction.

    template<typename T,typename U>
    vec_result_t<std::minus<>,const T&,const U&> operator-(const vector<T>& v1,const vector<U>& v2)
    {
        assert(v1.size()==v2.size());
        vec_result_t<std::minus<>,const T&,const U&> ret(v1.size(),boost::container::default_init);
        std::transform(v1.begin(),v1.end(),v2.begin(),ret.begin(),std::minus{});
        return ret;
    }

    template<typename T,typename U,
             typename = can_reuse_for_t<T,std::minus<>,T,const U&>>
    vec_result_t<std::minus<>,T,const U&> operator-(vector<T>&& v1,const vector<U>& v2)
    {
        assert(v1.size()==v2.size());
        std::transform(std::move_iterator{v1.begin()},std::move_iterator{v1.end()},
                       v2.begin(),v1.begin(),std::minus{});
        return v1;
    }

    template<typename T,typename U,
             typename = can_reuse_for_t<U,std::minus<>,const T&,U>>
    vec_result_t<std::minus<>,const T&,U> operator-(const vector<T>& v1,vector<U>&& v2)
    {
        assert(v1.size()==v2.size());
        std::transform(v1.begin(),v1.end(),std::move_iterator{v2.begin()},v2.begin(),std::minus{});
        return v2;
    }

    template<typename T,
             typename = can_reuse_for_t<T,std::minus<>,T,T>>
    vec_result_t<std::minus<>,T,T> operator-(vector<T>&& v1,vector<T>&& v2)
    {
        return std::move(v1)+v2;
    }

    // Not a universal reference for scalar, since we could only steal it for the last element
    // of the vector which is likely not worth the complication.
    template<typename T,typename U>
    vec_result_t<std::multiplies<>,const T&,const U&> operator*(const vector<T>& v,const U& s)
    {
        vec_result_t<std::multiplies<>,const T&,const U&> ret(v.size(),boost::container::default_init);
        // With one argument of a binary operator "fixed" we can either write
        // a custom functor using lambda manually...
        std::transform(v.begin(),v.end(),ret.begin(),[&](const T& x){
            return x*s;
        });
        return ret;
    }

    template<typename T,typename U,
             typename = can_reuse_for_t<T,std::multiplies<>,T,const U&>>
    vec_result_t<std::multiplies<>,T,const U&> operator*(vector<T>&& v,const U& s)
    {
        // ... or use std::bind.
        // The names of unbound parameters _1,_2,... are in a separate namespace.
        // bind normally captures by decay, to capture by reference, a std::reference_wrapper
        // is to be used through one of its make-like functions std::ref/cref.
        using namespace std::placeholders;
        std::transform(std::move_iterator{v.begin()},std::move_iterator{v.end()},
                       v.begin(),std::bind(std::multiplies{},_1,std::cref(s)));
        return v;
    }

    template<typename T,typename U>
    vec_result_t<std::multiplies<>,const T&,const U&> operator*(const T& s,const vector<U>& v)
    {
        vec_result_t<std::multiplies<>,const T&,const U&> ret(v.size(),boost::container::default_init);
        std::transform(v.begin(),v.end(),ret.begin(),[&](const T& x){
            return s*x;
        });
        return ret;
    }

    template<typename T,typename U,
             typename = can_reuse_for_t<U,std::multiplies<>,const T&,U>>
    vec_result_t<std::multiplies<>,const T&,U> operator*(const T& s,vector<U>&& v)
    {
        vec_result_t<std::multiplies<>,const T&,U> ret(v.size(),boost::container::default_init);
        std::transform(v.begin(),v.end(),ret.begin(),[&](T& x){
            return s*std::move(x);
        });
        return ret;
    }

    // When we have a multiplicative_invese available, delegate to it.
    // We use a generic decltype-based return type in case some
    // implementation of multiplicative_inverse actually changes type of value.

    template<typename T,typename U,
             typename = std::enable_if_t<has_multiplicative_inverse_v<U>>>
    auto operator/(const vector<T>& v,const U& s) ->
        decltype(v*multiplicative_inverse(s))
    {
        return v*multiplicative_inverse(s);
    }

    template<typename T,typename U,
             typename = std::enable_if_t<has_multiplicative_inverse_v<U>>>
    auto operator/(vector<T>&& v,U&& s) ->
        decltype(std::move(v)*multiplicative_inverse(s))
    {
        return std::move(v)*multiplicative_inverse(s);
    }

    // Otherwise do the usual. This also needs a complementary SFINAE check to the above.
    template<typename T,typename U,
             typename = std::enable_if_t<!has_multiplicative_inverse_v<U>>>
    vec_result_t<std::divides<>,const T&,const U&> operator/(const vector<T>& v,const U& s)
    {
        vec_result_t<std::divides<>,const T&,const U&> ret(v.size(),boost::container::default_init);
        std::transform(v.begin(),v.end(),v.begin(),[&](const T& x){
            return x/s;
        });
        return v;
    }

    template<typename T,typename U,
             typename = std::enable_if_t<!has_multiplicative_inverse_v<U>>,
             typename = vec_result_t<std::divides<>,T,const U&>>
    vector<std::common_type_t<T,U>> operator/(vector<T>&& v,const U& s)
    {
        std::transform(v.begin(),v.end(),v.begin(),[&](T& x){
            return std::move(x)/s;
        });
        return v;
    }
}

#endif

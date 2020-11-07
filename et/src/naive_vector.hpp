#ifndef UUID_09C08FF6_B107_4EEF_8DE8_BEE30A207CD4
#define UUID_09C08FF6_B107_4EEF_8DE8_BEE30A207CD4

#include "vector_base.hpp"

#include <algorithm>
#include <functional>

namespace naive
{
    template<typename T = double>
    class vector : public vector_base<T>
    {
        template<typename Vector>
        using is_vector_t = std::enable_if_t<std::is_same_v<std::decay_t<Vector>,vector>>;
    public:
        using vector_base<T>::vector_base;

        // None cone stolen from.
        friend vector operator+(const vector& v1,const vector& v2)
        {
            assert(v1.size()==v2.size());
            vector ret(v1.size(),boost::container::default_init);
            std::transform(v1.begin(),v1.end(),v2.begin(),ret.begin(),std::plus{});
        }

        // When left can be stolen from, reuse it.
        friend vector operator+(vector&& v1,const vector& v2)
        {
            assert(v1.size()==v2.size());
            vector ret{std::move(v1)};
            std::transform(ret.begin(),ret.end(),v2.begin(),ret.begin(),std::plus{});
            return ret;
        }

        // When right can be stolen from, delegate to previous implementation
        // because additions is commutative.
        friend vector operator+(const vector& v1,vector&& v2)
        {
            return std::move(v2)+v1;
        }

        // Same as previous, needed to resolve ambiguity between to
        // previous overloads when both arguments are rvalues.
        friend vector operator+(vector&& v1,vector&& v2)
        {
            return std::move(v1)+v2;
        }

        // Same for subtraction...
        friend vector operator-(const vector& v1,const vector& v2)
        {
            assert(v1.size()==v2.size());
            vector ret{v1.size(),boost::container::default_init};
            std::transform(v1.begin(),v1.end(),v2.begin(),ret.begin(),std::minus{});
        }

        friend vector operator-(vector&& v1,const vector& v2)
        {
            assert(v1.size()==v2.size());
            vector ret{std::move(v1)};
            std::transform(ret.begin(),ret.end(),v2.begin(),ret.begin(),std::minus{});
            return ret;
        }

        // ... except it is not commutative, so we have to write another version.
        friend vector operator-(const vector& v1,vector&& v2)
        {
            assert(v1.size()==v2.size());
            vector ret{std::move(v2)};
            std::transform(ret.begin(),ret.end(),v1.begin(),ret.begin(),
                           [](T y,T x){ return x-y; });
            return ret;
        }

        friend vector operator-(vector&& v1,vector&& v2)
        {
            return std::move(v1)-v2;
        }

        friend vector operator*(const vector& v,T coeff)
        {
            vector ret{v.size(),boost::container::default_init};
            std::transform(v.begin(),v.end(),ret.begin(),
                           [=](T x){ return x*coeff; });
            return ret;
        }

        friend vector operator*(vector&& v,T coeff)
        {
            vector ret{std::move(v)};
            std::transform(ret.begin(),ret.end(),ret.begin(),
                           [=](T x){ return x*coeff; });
            return ret;
        }

        // This overload delegates to either of two previous
        // by forwarding any kind of reference, provided
        // that it is to our vector.
        template<typename Vector,typename = is_vector_t<Vector>>
        friend vector operator*(T coeff,Vector&& v)
        {
            return std::forward<Vector>(v)*coeff;
        }

        template<typename Vector,typename = is_vector_t<Vector>>
        friend vector operator/(Vector&& v,T coeff)
        {
            return std::forward<Vector>(v)*(T(1)/coeff);
        }
    };
}

#endif

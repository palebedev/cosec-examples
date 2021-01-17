#ifndef UUID_762A2B41_DA34_443A_8C5C_09C3734E80DE
#define UUID_762A2B41_DA34_443A_8C5C_09C3734E80DE

#include "common.hpp"

namespace et
{
    // We assume the following concept for vector expression template nodes:
    // - nested type vector_et_elem_t is element type (used to enable et operations)
    // - size() returns number of elements (1 for scalars)
    // - operator[] provides per-element access.

    template<typename T>
    class vector : public boost::container::vector<T>
    {
    public:
        // Presence of vector_et_elem_t nested type
        // marks our expression template enabled classes,
        // including the vector itself.
        using vector_et_elem_t = T;

        using boost::container::vector<T>::vector;
    };

    namespace detail
    {
        template<typename Op,typename T,typename U>
        class vector_et_node;

        template<typename T>
        struct is_vector_et_node : std::false_type {};

        template<typename Op,typename T,typename U>
        struct is_vector_et_node<vector_et_node<Op,T,U>> : std::true_type {};

        template<typename T>
        constexpr inline bool is_vector_et_node_v = is_vector_et_node<T>{};

        template<typename Op,typename T,typename U>
        class vector_et_node
        {
        public:
            using vector_et_elem_t = op_result_t<Op,
                typename std::remove_cvref_t<T>::vector_et_elem_t,
                typename std::remove_cvref_t<U>::vector_et_elem_t>;

            vector_et_node(T&& x,U&& y)
                : x_{std::forward<T>(x)},
                  y_{std::forward<U>(y)}
            {
                // Either both have same size, or one is a scalar.
                assert(x_.size()==y_.size()||x_.size()==1||y_.size()==1);
            }

            std::size_t size() const noexcept
            {
                // One of the two args could be a scalar with size 1,
                // if neither is, we've already asserted both have same size
                // at construction, so just take the max of sizes.
                return std::max(x_.size(),y_.size());
            }

            vector_et_elem_t operator[](std::size_t i) &&
            {
                return Op{}(std::forward<T>(x_)[i],std::forward<U>(y_)[i]);
            }

            operator vector<vector_et_elem_t>() &&
            {
                auto calc = [this](vector<vector_et_elem_t>& ret) mutable {
                    std::generate(ret.begin(),ret.end(),[this,i=std::size_t{}]() mutable {
                        return std::move(*this)[i++];
                    });
                };
                auto&& stolen = std::move(*this).steal();
                if constexpr(std::is_same_v<decltype(stolen),vector<vector_et_elem_t>&&>){
                    calc(stolen);
                    return std::move(stolen);
                }else{
                    vector<vector_et_elem_t> ret{size(),boost::container::default_init};
                    calc(ret);
                    return ret;
                }
            }

            template<typename Res = vector_et_elem_t>
            decltype(auto) steal() &&
            {
                if constexpr(std::is_same_v<T,vector<Res>&&>)
                    return std::move(x_);
                else if constexpr(std::is_same_v<U,vector<Res>&&>)
                    return std::move(y_);
                else if constexpr(is_vector_et_node_v<T>){
                    auto&& ret = std::move(x_).template steal<Res>();
                    if constexpr(std::is_same_v<decltype(ret),vector<Res>&&>)
                        return std::move(ret);
                }
                if constexpr(is_vector_et_node_v<U>)
                    return std::move(y_).template steal<Res>();
                else
                    return nullptr;
            }
        private:
            T x_;
            U y_;
        };

        template<typename T>
        class scalar_wrapper
        {
        public:
            using vector_et_elem_t = const T&;

            scalar_wrapper(T x) noexcept
                : x_{std::move(x)}
            {}

            constexpr static std::size_t size() noexcept
            {
                return 1;
            }

            const T& operator[](std::size_t /*i*/) const noexcept
            {
                return x_;
            }
        private:
            T x_;
        };

        template<typename Op,typename T,typename U>
        using op_result_vv_t = op_result_t<Op,typename std::remove_cvref_t<T>::vector_et_elem_t,
                                              typename std::remove_cvref_t<U>::vector_et_elem_t>;

        template<typename Op,typename T,typename U>
        using op_result_vs_t = op_result_t<Op,typename std::remove_cvref_t<T>::vector_et_elem_t,U>;

        template<typename Op,typename T,typename U>
        using op_result_sv_t = op_result_t<Op,T,typename std::remove_cvref_t<U>::vector_et_elem_t>;
    }

    template<typename T,typename U,
             typename = detail::op_result_vv_t<std::plus<>,T,U>>
    detail::vector_et_node<std::plus<>,T&&,U&&> operator+(T&& x,U&& y) noexcept
    {
        return {std::forward<T>(x),std::forward<U>(y)};
    }

    template<typename T,typename U,
             typename = detail::op_result_vv_t<std::minus<>,T,U>>
    detail::vector_et_node<std::minus<>,T&&,U&&> operator-(T&& x,U&& y) noexcept
    {
        return {std::forward<T>(x),std::forward<U>(y)};
    }

    template<typename T,typename U,
             typename = detail::op_result_vs_t<std::multiplies<>,T,const U&>>
    detail::vector_et_node<std::multiplies<>,T&&,detail::scalar_wrapper<const U&>>
        operator*(T&& x,const U& y) noexcept
    {
        return {std::forward<T>(x),{y}};
    }

    template<typename T,typename U,
             typename = detail::op_result_sv_t<std::multiplies<>,const T&,U>>
    detail::vector_et_node<std::multiplies<>,detail::scalar_wrapper<const T&>,U&&>
        operator*(const T& x,U&& y) noexcept
    {
        return {{x},std::forward<U>(y)};
    }

    template<typename T,typename U,
             typename = detail::op_result_vs_t<std::multiplies<>,T,
                                               multiplicative_inverse_result_t<U>>>
    detail::vector_et_node<std::multiplies<>,T&&,
                           detail::scalar_wrapper<multiplicative_inverse_result_t<U>>>
        operator/(T&& x,const U& y)
    {
        return {std::forward<T>(x),{multiplicative_inverse(y)}};
    }

    template<typename T,typename U,
             typename = detail::op_result_vs_t<std::multiplies<>,T,const U&>,
             typename = std::enable_if_t<!has_multiplicative_inverse_v<U>>>
    detail::vector_et_node<std::divides<>,T&&,detail::scalar_wrapper<const U&>>
        operator/(T&& x,const U& y) noexcept
    {
        return {std::forward<T>(x),{y}};
    }
}

#endif

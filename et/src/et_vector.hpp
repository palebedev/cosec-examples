#ifndef UUID_762A2B41_DA34_443A_8C5C_09C3734E80DE
#define UUID_762A2B41_DA34_443A_8C5C_09C3734E80DE

#include "vector_base.hpp"

#include <iterator>
#include <type_traits>

namespace et
{
    template<typename T = double>
    class vector : public vector_base<T>
    {
    public:
        // Presence of vector_et_elem_t nested type
        // marks our expression template enabled classes,
        // including the vector itself.
        using vector_et_elem_t = T;

        using vector_base<T>::vector_base;
    };

    namespace detail
    {
        template<typename Derived,typename T>
        class vector_et_node_base
        {
        public:
            // Nodes are also et-enabled
            using vector_et_elem_t = T;

            // When a node is to be converted to a real vector,
            // calculate its elements in a single loop.
            operator vector<T>() const
            {
                const Derived& this_ = static_cast<const Derived&>(*this);
                vector<T> ret{this_.size(),boost::container::default_init};
                for(std::size_t i=0;i<ret.size();++i)
                    ret[i] = this_[i];
                return ret;
            }
        };

        // A type of element for two et-enabled classes is a common
        // type of its elements.
        template<typename Node1,typename Node2>
        using vector_et_binary_node_elem_t = std::common_type_t<
            typename Node1::vector_et_elem_t,
            typename Node2::vector_et_elem_t
        >;

        // Base classes for binary operator et nodes.
        // It stores both subnodes and provides size.
        template<typename Derived,typename Node1,typename Node2>
        class vector_et_binary_node_base :
            public vector_et_node_base<
                Derived,
                vector_et_binary_node_elem_t<Node1,Node2>
            >
        {
        public:
            vector_et_binary_node_base(const Node1& n1,const Node2& n2) noexcept
                : n1_{n1},
                  n2_{n2}
            {
                assert(n1.size()==n2.size());
            }

            std::size_t size() const noexcept
            {
                return n1_.size();
            }
        protected:
            const Node1& n1_;
            const Node2& n2_;
        };

        // Node for addition provides implementation of operator[].
        template<typename Node1,typename Node2>
        class vector_et_add_node :
            public vector_et_binary_node_base<
                vector_et_add_node<Node1,Node2>,Node1,Node2>
        {
        public:
            using vector_et_binary_node_base<
                vector_et_add_node<Node1,Node2>,Node1,Node2>::vector_et_binary_node_base;

            auto operator[](std::size_t i) const
            {
                return this->n1_[i]+this->n2_[i];
            }
        };

        // Same for subtraction.
        template<typename Node1,typename Node2>
        class vector_et_sub_node :
            public vector_et_binary_node_base<
                vector_et_sub_node<Node1,Node2>,Node1,Node2>
        {
        public:
            using vector_et_binary_node_base<
                vector_et_sub_node<Node1,Node2>,Node1,Node2>::vector_et_binary_node_base;

            auto operator[](std::size_t i) const
            {
                return this->n1_[i]-this->n2_[i];
            }
        };

        template<typename Node,typename Coeff>
        using vector_et_mul_node_elem_t = std::common_type_t<
            typename Node::vector_et_elem_t,Coeff>;

        template<typename Node,typename Coeff>
        class vector_et_mul_node :
            public vector_et_node_base<
                vector_et_mul_node<Node,Coeff>,
                vector_et_mul_node_elem_t<Node,Coeff>
            >
        {
        public:
            vector_et_mul_node(const Node& n,Coeff coeff)
                : n_{n},
                  coeff_{coeff}
            {}

            auto operator[](std::size_t i) const
            {
                return this->n_[i]*coeff_;
            }

            std::size_t size() const noexcept
            {
                return n_.size();
            }
        protected:
            const Node& n_;
            Coeff coeff_;
        };
    }

    // Operators are SFINAED on whether arguments are et-enabled vectors and have
    // a common element type.
    template<typename Node1,typename Node2,
             typename = detail::vector_et_binary_node_elem_t<Node1,Node2>>
    auto operator+(const Node1& n1,const Node2& n2)
    {
        // CTAD doesn't work with inherited constructors.
        return detail::vector_et_add_node<Node1,Node2>{n1,n2};
    }

    template<typename Node1,typename Node2,
             typename = detail::vector_et_binary_node_elem_t<Node1,Node2>>
    auto operator-(const Node1& n1,const Node2& n2)
    {
        return detail::vector_et_sub_node<Node1,Node2>{n1,n2};
    }

    template<typename Node,typename Coeff,
             typename = detail::vector_et_mul_node_elem_t<Node,Coeff>>
    auto operator*(const Node& n,Coeff coeff)
    {
        return detail::vector_et_mul_node{n,coeff};
    }

    template<typename Node,typename Coeff,
             typename = detail::vector_et_mul_node_elem_t<Node,Coeff>>
    auto operator*(Coeff coeff,const Node& n)
    {
        return detail::vector_et_mul_node{n,coeff};
    }

    template<typename Node,typename Coeff,
             typename = detail::vector_et_mul_node_elem_t<Node,Coeff>>
    auto operator/(const Node& n,Coeff coeff)
    {
        return detail::vector_et_mul_node{n,Coeff(1)/coeff};
    }
}

#endif

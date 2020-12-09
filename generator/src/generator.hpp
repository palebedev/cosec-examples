#ifndef UUID_E15F4C61_C7C9_468F_B22C_A1702FB119C8
#define UUID_E15F4C61_C7C9_468F_B22C_A1702FB119C8

#include <ce/stdcoro.hpp>

#include <boost/iterator/iterator_facade.hpp>

#include <optional>
#include <utility>

namespace ce
{
    template <typename T>
    class generator
    {
    public:
        class iterator;

        class promise_type
        {
        public:
            generator get_return_object() noexcept
            {
                return generator{*this};
            }

            stdcoro::suspend_always initial_suspend() noexcept
            {
                return {};
            }

            stdcoro::suspend_always yield_value(T value) noexcept
            {
                current_value_ = std::move(value);
                return {};
            }

            stdcoro::suspend_always final_suspend() noexcept
            {
                return {};
            }

            void return_void() noexcept {}

            [[noreturn]] void unhandled_exception()
            {
                throw;
            }

            template<typename Awaitable>
            void await_transform(Awaitable a) = delete;
        private:
            friend iterator;

            std::optional<T> current_value_;
        };

        class iterator : public boost::iterator_facade<iterator,T,boost::single_pass_traversal_tag>
        {
        public:
            iterator() noexcept = default;
        private:
            friend boost::iterator_core_access;
            friend generator;

            stdcoro::coroutine_handle<promise_type>* handle_;

            iterator(stdcoro::coroutine_handle<promise_type>& handle) noexcept
                : handle_{&handle}
            {}

            void increment()
            {
                handle_->resume();
                if(handle_->done())
                    handle_ = {};
            }

            bool equal(const iterator& other) const noexcept
            {
                return handle_==other.handle_;
            }

            T& dereference() const noexcept
            {
                return *handle_->promise().current_value_;
            }
        };

        generator(generator&& other) noexcept
            : handle_{std::exchange(other.handle_,{})}
        {}

        generator& operator=(generator&& other) noexcept
        {
            if(this!=&other){
                clear();
                handle_ = std::exchange(other.handle_,{});
            }
            return *this;
        }

        ~generator()
        {
            clear();
        }

        iterator begin() noexcept
        {
            handle_.resume();
            return {handle_};
        }

        iterator end() noexcept
        {
            return {};
        }
    private:
        stdcoro::coroutine_handle<promise_type> handle_;

        generator(promise_type& promise) noexcept
        : handle_{stdcoro::coroutine_handle<promise_type>::from_promise(promise)}
        {}

        void clear() noexcept
        {
            if(handle_)
                handle_.destroy();
        }
    };
}

#endif

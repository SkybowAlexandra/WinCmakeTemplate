#include <iostream>
#include <functional>
#include <type_traits>
#include <future>
#include <memory>

namespace func
{
    template <typename ReturnType = void>
    class Callback
    {
    public:
        template <typename Func, typename... Args>
        void register_func(Func function, Args... args)
        {
            auto task_pkg = std::make_shared<std::packaged_task<std::invoke_result_t<Func, Args...>()>>(
                std::bind(function, args...));
            if (!task_pkg)
            {
                throw std::runtime_error("Failed to create packaged_task");
                return;
            }
            this->f = std::async(std::launch::deferred, [task_pkg]() -> ReturnType
            {
           
                    (*task_pkg)();
                    return (*task_pkg).get_future().get(); 
            });
        }
        ReturnType call()
        {
            return f.get();
        }
        ~Callback()
        {
            if (f.valid())
            {
                f.wait();
            }
        }

    private:
        std::future<ReturnType> f;
    };
};
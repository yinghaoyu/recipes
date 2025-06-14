#include <coroutine>
#include <iostream>

template <bool READY> struct Awaiter {
    bool await_ready() noexcept {
        std::cout << "await_ready: " << READY << std::endl;
        return READY;
    }
    void await_resume() noexcept { std::cout << "await_resume" << std::endl; }
    void await_suspend(std::coroutine_handle<>) noexcept {
        std::cout << "await_suspend" << std::endl;
    }
};

struct TaskPromise {
    struct promise_type {
        TaskPromise get_return_object() {
            std::cout << "get_return_object" << std::endl;
            return TaskPromise{
                std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        Awaiter<true> initial_suspend() noexcept {
            std::cout << "initial_suspend" << std::endl;
            return {};
        }
        Awaiter<true> final_suspend() noexcept {
            std::cout << "final_suspend" << std::endl;
            return {};
        }
        void unhandled_exception() {
            std::cout << "unhandled_exception" << std::endl;
        }
        void return_void() noexcept { std::cout << "return_void" << std::endl; }
    };

    void resume() {
        std::cout << "resume" << std::endl;
        handle.resume();
    }

    std::coroutine_handle<promise_type> handle;
};

TaskPromise task_func() {
    std::cout << "task first run" << std::endl;
    co_await Awaiter<false>{};
    std::cout << "task resume" << std::endl;
}

int main() {
    auto promise = task_func();
    promise.resume();

    return 0;
}
// Output:
// get_return_object
// initial_suspend
// await_ready: 1
// await_resume
// task first run
// await_ready: 0
// await_suspend
// resume
// await_resume
// task resume
// return_void
// final_suspend
// await_ready: 1
// await_resume

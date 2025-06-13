#include <coroutine>
#include <iostream>

// C++20的协程是无栈协程

// co_await
// 用于暂停协程的执行，直到某个异步操作完成
// 就好像你在等快递上门，没等到的时候就先去干别的事儿，等快递员打电话了，你再回来签收

// co_yield 用于从协程中产生一个值，并暂停协程的执行，它常用于生成器模式
// 想象一个生产数字序列的协程，每产生一个数字就暂停一下，等待外界取用

// co_return 则用于从协程中返回一个值，并结束协程的执行，类似普通函数的 return
// 语句，只不过协程的返回还有一些额外的 “善后工作” 要做

struct Generator {
    struct promise_type {
        int current_value;
        // 用于生成协程函数的返回对象
        Generator get_return_object() {
            return Generator{
                // 静态函数，获取这个协程的句柄
                std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // 注意这几个函数,返回的类型是awaiter
        // 协程创建时挂起
        std::suspend_always initial_suspend() { return {}; }

        // 协程退出时调用的接口
        // 返回std::suspend_never，自动销毁 coroutine state
        // 对象
        // 返回std::suspend_always，则需要用户自行调用
        // handle.destroy()进行销毁
        // 但值得注意的是返回std::suspend_always并不会挂起协程，因为这个时候协程已经退出了
        std::suspend_always final_suspend() noexcept { return {}; }

        void unhandled_exception() { std::terminate(); }

        // co_yield
        std::suspend_always yield_value(int value) {
            current_value = value;
            return {};
        }

        // co_return
        void return_void() {}
    };
    std::coroutine_handle<promise_type> coro_handle;
    explicit Generator(std::coroutine_handle<promise_type> handle)
        : coro_handle(handle) {}
    ~Generator() {
        if (coro_handle)
            coro_handle.destroy();
    }
    Generator(const Generator &) = delete;
    Generator &operator=(const Generator &) = delete;
    Generator(Generator &&other) noexcept : coro_handle(other.coro_handle) {
        other.coro_handle = nullptr;
    }
    Generator &operator=(Generator &&other) noexcept {
        if (this != &other) {
            coro_handle = other.coro_handle;
            other.coro_handle = nullptr;
        }
        return *this;
    }
    bool move_next() {
        if (!coro_handle.done()) {
            coro_handle.resume();
            return true;
        }
        return false;
    }
    int current_value() const { return coro_handle.promise().current_value; }
};

Generator counter() {
    for (int i = 0; i < 5; ++i) {
        co_yield i;
    }
    co_return;
}

int main() {
    Generator gen = counter();
    while (gen.move_next()) {
        std::cout << gen.current_value() << std::endl;
    }
    return 0;
}

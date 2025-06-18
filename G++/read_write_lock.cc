#include <assert.h>
#include <condition_variable>
#include <mutex>

class RWMutex {
    // 基于 Howard Hinnant 的参考实现
    // @sa https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2406.html
  public:
    RWMutex() {}

    ~RWMutex() {
        if (m_state != 0) {
            assert(!"RWMutex destroyed while locked");
        }
    }

    void lock_shared() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond1.wait(lock, [this]() { return m_state < _S_max_readers; });
        ++m_state;
    }

    void unlock_shared() {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto pre = m_state--;
        if (write_entered()) {
            if (readers() == 0) {
                m_cond2.notify_one();
            }
        } else {
            if (pre == _S_max_readers) {
                m_cond1.notify_one();
            }
        }
    }

    bool try_lock_shared() {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!lock.owns_lock()) {
            return false;
        }
        if (m_state >= _S_max_readers) {
            return false;
        }
        ++m_state;
        return true;
    }

    void lock() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond1.wait(lock, [this]() { return !write_entered(); });
        m_state |= _S_write_entered;
        m_cond2.wait(lock, [this]() { return readers() == 0; });
    }

    void unlock() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_state = 0;
        m_cond1.notify_all();
    }

    bool try_lock() {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!lock.owns_lock()) {
            return false;
        }
        if (m_state != 0) {
            return false;
        }
        m_state |= _S_write_entered;
        return true;
    }

  private:
    static constexpr unsigned _S_write_entered =
        1u << (sizeof(unsigned) * __CHAR_BIT__ - 1);

    static constexpr unsigned _S_max_readers = ~_S_write_entered;

    bool write_entered() const { return m_state & _S_write_entered; }

    bool readers() const { return m_state & _S_max_readers; }

    std::mutex m_mutex;
    // 1. 写者等待写锁释放
    // 2. 读者等待读锁数量未达到上限
    std::condition_variable m_cond1;
    // 1. 写者等待所有读锁释放
    std::condition_variable m_cond2;
    unsigned m_state = 0;
};

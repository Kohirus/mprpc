#ifndef __LOCK_QUEUE_H__
#define __LOCK_QUEUE_H__

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

// 异步写日志的日志队列
template <typename T>
class LockQueue {
public:
    void Push(const T& data) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(data);
        _cond.notify_one();
    }

    T Pop() {
        std::unique_lock<std::mutex> lock(_mutex);
        while (_queue.empty()) {
            // 日志队列为空 线程进入wait状态
            _cond.wait(lock);
        }

        T data = _queue.front();
        _queue.pop();
        return data;
    }

private:
    std::queue<T>           _queue;
    std::mutex              _mutex;
    std::condition_variable _cond;
};

#endif // !__LOCK_QUEUE_H__

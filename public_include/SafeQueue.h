#pragma once

#include <queue>
#include <mutex>

template <typename T>
class SafeQueue
{
private:
    std::queue<T> _queue; 
    std::mutex _mutex;
public:
    SafeQueue() {}
    SafeQueue(SafeQueue &&other) {}
    ~SafeQueue() {}
    bool empty()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _queue.empty();
    }
    int size()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _queue.size();
    }
    // 队列添加元素
    void enqueue(T &t)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.emplace(t);
    }
    // 队列取出元素
    bool dequeue(T &t)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_queue.empty())
            return false;
        t = std::move(_queue.front());
        _queue.pop();
        return true;
    }
};
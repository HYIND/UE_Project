#include "RateLimiter.h"
#include <map>
#include <shared_mutex>

class RateLimiter_Manager
{
public:
    static RateLimiter_Manager *Instance()
    {
        static RateLimiter_Manager *m_Instance = new RateLimiter_Manager();
        return m_Instance;
    }

private:
    RateLimiter_Manager() {}

public:
    bool Push(int fd);
    bool Pop(int fd);
    bool TryPass(int fd);
    void Pass(int fd);

private:
    std::map<int, RateLimiter *> RateLimiter_Map;
    mutable shared_mutex Map_mutex;
};
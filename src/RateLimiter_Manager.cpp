#include "RateLimiter_Manager.h"
#include "Log.h"

bool RateLimiter_Manager::Push(int fd)
{
    Map_mutex.lock();
    if (RateLimiter_Map.find(fd) != RateLimiter_Map.end())
    {
        LOGINFO("RateLimiter_Manager::Push ,Fail! fd {} has been Push!", fd);
        Map_mutex.unlock();
        return false;
    }
    RateLimiter_Map.emplace(fd, new RateLimiter(RATE_LIMIT));
    Map_mutex.unlock();
    return true;
}
bool RateLimiter_Manager::Pop(int fd)
{
    Map_mutex.lock();
    if (RateLimiter_Map.find(fd) == RateLimiter_Map.end())
    {
        LOGINFO("RateLimiter_Manager::Pop ,Fail! fd {} not exist!", fd);
        Map_mutex.unlock();
        return false;
    }
    RateLimiter_Map.erase(fd);
    Map_mutex.unlock();
    return true;
}
bool RateLimiter_Manager::TryPass(int fd)
{
    Map_mutex.lock_shared();
    auto it = RateLimiter_Map.find(fd);
    if (it == RateLimiter_Map.end())
        RateLimiter_Map.emplace(fd, new RateLimiter(RATE_LIMIT));

    bool result = RateLimiter_Map[fd]->tryGetToken();
    Map_mutex.unlock_shared();
    return result;
}

void RateLimiter_Manager::Pass(int fd)
{
    Map_mutex.lock_shared();
    auto it = RateLimiter_Map.find(fd);
    if (it == RateLimiter_Map.end())
        RateLimiter_Map.emplace(fd, new RateLimiter(RATE_LIMIT));

    RateLimiter_Map[fd]->pass();
    Map_mutex.unlock_shared();
}
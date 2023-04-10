// #include <iostream>
// #include <sys/types.h>
// #include <sys/time.h>
// #include <unistd.h>
// #include <time.h>
#include "RateLimiter.h"

RateLimiter::RateLimiter(int qps)
{
    supplyUnitTime = 1000000000L / qps;
    bucketSize = 50;
    tokenLeft = 0;
}

long RateLimiter::now()
{
    struct timeval tv;
    long seconds;

    gettimeofday(&tv, 0);

    seconds = tv.tv_sec;

    return seconds * 1000000000L + tv.tv_usec * 1000L;
}

void RateLimiter::supplyTokens()
{
    long cur = now();

    if (cur - lastAddTokenTime < supplyUnitTime)
    {
        return;
    }

    long newTokens = (cur - lastAddTokenTime) / supplyUnitTime;

    if (newTokens <= 0)
    {
        return;
    }

    mtx.lock();

    // 更新补充时间
    lastAddTokenTime += (newTokens * supplyUnitTime);

    int freeRoom = bucketSize - tokenLeft;

    if (newTokens > freeRoom || freeRoom > bucketSize)
    {
        newTokens = freeRoom > bucketSize ? bucketSize : freeRoom;
    }

    tokenLeft += newTokens;

    mtx.unlock();
}

bool RateLimiter::tryGetToken()
{
    int token;

    supplyTokens();

    mtx.lock();

    token = tokenLeft;

    tokenLeft--;

    mtx.unlock();

    if (token <= 0)
    {
        mtx.lock();
        tokenLeft++;
        mtx.unlock();

        return false;
    }

    return true;
}

void RateLimiter::mustGetToken()
{
    bool isGetToken = false;

    for (int i = 0; i < RETRY_IMEDIATELY_TIMES; i++)
    {
        isGetToken = tryGetToken();

        if (isGetToken)
        {
            return;
        }
    }

    while (1)
    {
        isGetToken = tryGetToken();

        if (isGetToken)
        {
            return;
        }
        else
        {
            sleep(0);
        }
    }
}

void RateLimiter::pass()
{
    return mustGetToken();
}

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

// int main()
// {
//     RateLimiter r(50);
//     r.pass();
//     return 0;
// }
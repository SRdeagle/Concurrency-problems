#pragma once

#include <mutex>
#include <condition_variable>

using std::condition_variable;
using std::mutex;
using std::unique_lock;

class semaphore
{
private:
    mutex m;
    condition_variable cv;
    int n;

public:
    explicit semaphore(int nn) : n(nn) {}

    // wait / P / down
    void wait()
    {
        unique_lock<mutex> lock(m);
        while (n == 0)
        {
            cv.wait(lock);
        }
        n--;
    }

    // signal / V / up
    void signal()
    {
        unique_lock<mutex> lock(m);
        n++;
        cv.notify_one();
    }
};
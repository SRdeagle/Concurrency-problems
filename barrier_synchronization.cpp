// clang++ -std=c++17 barrier_synchronization.cpp -pthread -o app
// ^ komanda za kompilaciju, posle samo ./app

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "sem.cpp"
#include <random>
#include <mutex>
using namespace std;

const int N = 10;
int blocked = 0;
semaphore arrived(1);
semaphore completed(0);
semaphore mutexWorkers(1);

void random_sleep(int min_ms = 50, int max_ms = 300)
{
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(min_ms, max_ms);
    std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
}

void work1(int id)
{
    random_sleep(100, 1000);
    mutexWorkers.wait();
    cout << "Worker " << id << " is working job 1" << endl;
    mutexWorkers.signal();
};
void work2(int id)
{
    mutexWorkers.wait();
    cout << "Worker " << id << " is working job 2" << endl;
    mutexWorkers.signal();
};

void worker(int id)
{
    work1(id);

    arrived.wait();
    blocked++;
    if (blocked != N)
    {
        arrived.signal();
        completed.wait();
    }
    else if (blocked == N)
    {
        mutexWorkers.wait();
        cout << "barrier passed!" << endl;
        mutexWorkers.signal();
    }
    blocked--;
    if (blocked > 0)
    {
        completed.signal();
    }
    else
    {
        arrived.signal();
    }

    work2(id);
}

int main()
{
    vector<thread> t;
    for (int i = 0; i < N; i++)
    {
        t.emplace_back(worker, i);
    }
    for (thread &tt : t)
    {
        tt.join();
    }
    cout << "blocked " << blocked << endl;
    return 0;
}
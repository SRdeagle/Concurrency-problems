// clang++ -std=c++17 h2o_problem.cpp -pthread -o app
// ^ komanda za kompilaciju, posle samo ./app

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "sem.cpp"
#include <random>
#include <mutex>
using namespace std;

int numH = 0;
int numO = 0;
semaphore mutexH(1);
semaphore mutexO(1);
semaphore coord(0);
semaphore H(0);
semaphore O(0);

void coordinator()
{
    while (true)
    {
        coord.wait();
        mutexH.wait();
        mutexO.wait();

        if (numH >= 2 && numO >= 1)
        {
            numH -= 2;
            numO -= 1;
            H.signal();
            H.signal();
            O.signal();
        }

        mutexH.signal();
        mutexO.signal();
    }
}

void hydrogen()
{
    mutexH.wait();
    numH++;
    mutexH.signal();
    coord.signal();
    H.wait();
}

void oxygen()
{
    mutexO.wait();
    numO++;
    mutexO.signal();
    coord.signal();
    O.wait();
}

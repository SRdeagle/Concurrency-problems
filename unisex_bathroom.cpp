// clang++ -std=c++17 unisex_bathroom.cpp -pthread -o app
// ^ komanda za kompilaciju, posle samo ./app

// ne radi

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "sem.cpp"
using namespace std;
const int capacity = 3, MAX_CYCLE = 4;
int womenCount = 0;
int menCount = 0;
int cycleCount = 0;
int waitingMen = 0, waitingWomen = 0;
bool turn = false;
semaphore mutexToilet(1);
semaphore womenSemaphore(3);
semaphore menSemaphore(3);
semaphore mutexWomen(1);
semaphore mutexMen(1);
semaphore mutexOutput(1);
void use(int id)
{
    string who = (id < 100) ? "WOMAN " + to_string(id)
                            : "MAN " + to_string(id - 100);

    mutexOutput.wait();
    cout << who << " ENTER bathroom" << endl;
    mutexOutput.signal();

    // simulate usage time
    this_thread::sleep_for(chrono::milliseconds(1000 + rand() % 2000));

    mutexOutput.wait();
    cout << who << " EXIT bathroom" << endl;
    mutexOutput.signal();
}

void woman(int id)
{
    mutexWomen.wait();
    if (womenCount == 0)
    {

        mutexToilet.wait();
    }
    womenCount++;
    mutexWomen.signal();

    use(id);

    return;
}

void man(int id)
{
    id = id + 100;

    use(id);

    return;
}

int main()
{
    srand(time(nullptr));

    vector<thread> t;

    const int N = 20; // total number of people

    for (int i = 0; i < N; i++)
    {
        if (rand() % 2 == 0)
        {
            int id = (i % 10) + 1;
            t.emplace_back(woman, id);
        }
        else
        {
            int id = (i % 10) + 1;
            t.emplace_back(man, id);
        }

        // random arrival time
        this_thread::sleep_for(chrono::milliseconds(100 + rand() % 400));
    }

    for (thread &tt : t)
    {
        tt.join();
    }

    return 0;
}
// clang++ -std=c++17 hungry_birds.cpp -pthread -o app
// ^ komanda za kompilaciju, posle samo ./app

// posto nisam dobro zakljucao food kad ispisujem u pomocnim metodama, ispis mozda nekad izgleda netacno
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "sem.cpp"
#include <random>
#include <mutex>
using namespace std;

const int N = 10;
const int F = 5;
const int MAX_LOOP = 50;
int food = F;

semaphore mutexFood(1);
semaphore birdSem(0);
semaphore chickSem(0);
semaphore birdAway(1);

mutex io_mutex;

void random_sleep(int min_ms = 50, int max_ms = 300)
{
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(min_ms, max_ms);
    std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
}

void sleep()
{
    random_sleep(100, 400);
}

void getFood()
{
    {
        lock_guard<mutex> lg(io_mutex);
        cout << "[BIRD " << this_thread::get_id() << "] fetching food..." << endl;
    }
    random_sleep(300, 700);
    {
        lock_guard<mutex> lg(io_mutex);
        cout << "[BIRD " << this_thread::get_id() << "] food refilled" << endl;
    }
}

void goEat(bool isChick)
{
    {
        lock_guard<mutex> lg(io_mutex);
        cout << (isChick ? "[CHICK " : "[BIRD ") << this_thread::get_id() << "] eating, food left = " << (!isChick ? "" : to_string(food)) << endl;
    }
    random_sleep(100, 300);
}

void chick()
{
    for (int h = 0; h < MAX_LOOP; h++)
    {
        sleep();
        mutexFood.wait();
        food--;
        if (food == 0)
        {
            birdSem.signal();
            goEat(true);
            chickSem.wait();
        }
        else
        {
            mutexFood.signal();
            goEat(true);
        }
    }
}

void bird()
{
    // pretpostavka da ptica tek kad je probude ptici i donese hranu odlucuje da li ce ici da jede
    for (int h = 0; h < MAX_LOOP; h++)
    {
        birdSem.wait();
        birdAway.wait();
        getFood();
        food = F;
        chickSem.signal();
        mutexFood.signal();

        if (rand() % 3 == 0)
        {
            goEat(false);
        }

        birdAway.signal();
    }
}

int main()
{
    srand(time(nullptr));
    {
        lock_guard<mutex> lg(io_mutex);
        cout << "=== Hungry Birds simulation started ===" << endl;
    }
    vector<thread> t;
    for (int i = 0; i < N; i++)
    {
        t.emplace_back(chick);
    }
    for (int i = 0; i < 2; i++)
    {
        t.emplace_back(bird);
    }
    for (thread &tt : t)
    {
        tt.join();
    }
    {
        lock_guard<mutex> lg(io_mutex);
        cout << "=== Simulation finished ===" << endl;
    }
    return 0;
}
// clang++ -std=c++17 child_care.cpp -pthread -o app
// ^ komanda za kompilaciju, posle samo ./app

// super odradjeno

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "sem.cpp"
#include <random>
#include <mutex>
using namespace std;
struct node
{
    semaphore sem;
    node *next = nullptr;
    node() : sem(0) {}
};
int children = 0;
int ctakers = 0;
node *head = nullptr;
node *tail = nullptr;
semaphore mutexChildren(1);
semaphore mutexCtakers(1);
semaphore mutexList(1);

void random_sleep(int min_ms = 50, int max_ms = 300)
{
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(min_ms, max_ms);
    std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
}

void parent(int kids)
{

    if (kids < 0)
    {
        int available;
        mutexChildren.wait();

        children += kids;
        if (children < 0)
        {
            children -= kids;
            mutexChildren.signal();
            return;
        }

        available = ctakers - (children / 3 + (children % 3) ? 1 : 0);

        mutexChildren.signal();
        mutexList.wait();
        while (available > 0 && head != nullptr)
        {
            semaphore *temp = &head->sem;
            head = head->next;
            available--;
            if (head == nullptr)
            {
                tail = nullptr;
                temp->signal();
                break;
            }
            temp->signal();
        }
        mutexList.signal();
    }
    else
    {
        if (head != nullptr) // caretaker wants to leave, we dont want to block them
            return;
        mutexChildren.wait();
        if (ctakers * 3 - children < kids) // not enough caretakers
        {
            mutexChildren.signal();
            return;
        }
        children += kids;
        mutexChildren.signal();
    }
}

void caretaker()
{
    mutexList.wait();
    if (head != nullptr)
    {
        semaphore *temp = &head->sem;
        head = head->next;
        if (head == nullptr)
            tail = nullptr;
        temp->signal();
    }
    mutexList.signal();
    mutexCtakers.wait();
    ctakers++;
    mutexCtakers.signal();

    // sleep random time
    random_sleep(100, 1000);

    // try to leave
    mutexChildren.wait();
    mutexCtakers.wait();
    if (ctakers * 3 - children > 0)
    {
        ctakers--;
        mutexCtakers.signal();
        mutexChildren.signal();
        return;
    }
    mutexChildren.signal();
    mutexCtakers.signal();

    // make a request to leave, not allowing any more kids until fulfilled
    mutexList.wait();
    node request;
    if (head != nullptr)
    {
        tail->next = &request;
        tail = &request;
    }
    else
    {
        head = &request;
        tail = &request;
    }
    mutexList.signal();
    request.sem.wait();

    // leave
    mutexCtakers.wait();
    bool valid = ctakers * 3 >= children;
    if (!valid)
        cout << "[ERROR] ctaker cant leave" << endl;
    ctakers--;
    mutexCtakers.signal();
}

int main()
{
    cout << "=== START CHILD CARE SIMULATION ===" << endl;

    vector<thread> threads;

    // Pokretanje negovatelja
    for (int i = 0; i < 3; i++)
    {
        threads.emplace_back([]
                             {
            cout << "[CTAKER] Arrived" << endl;
            caretaker();
            cout << "[CTAKER] Left" << endl; });
        random_sleep(50, 150);
    }

    // Pokretanje roditelja koji dovode decu
    for (int i = 0; i < 5; i++)
    {
        threads.emplace_back([i]
                             {
            cout << "[PARENT] Bringing 2 kids" << endl;
            parent(2);
            cout << "[PARENT] Done bringing kids" << endl; });
        random_sleep(50, 150);
    }

    // Roditelji koji odvode decu
    for (int i = 0; i < 5; i++)
    {
        threads.emplace_back([i]
                             {
            cout << "[PARENT] Taking 1 kid away" << endl;
            parent(-1);
            cout << "[PARENT] Done taking kid" << endl; });
        random_sleep(50, 150);
    }

    for (auto &t : threads)
        t.join();

    cout << "=== END SIMULATION ===" << endl;
    cout << "Final children count: " << children << endl;
    cout << "Final caretaker count: " << ctakers << endl;

    return 0;
}
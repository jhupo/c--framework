#include <iostream>
#include <chrono>

#include <thread>
#include <thread/threadpool.h>

void test(int id)
{
    std::cout << "Thread ID: " << std::this_thread::get_id() << " ID:" << id << std::endl;
}

int main(int argc, char* argv[])
{
    int as = 1;
    std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;  
    COMMON_NAMESPACE::ThreadPool::ptr pool(new COMMON_NAMESPACE::ThreadPool);

    for(int i = 0; i < 10; ++i)
    {
        pool->addTask(std::bind(test, i));
    }
    pool->waitForDone();
    getchar();

    return 0;
}
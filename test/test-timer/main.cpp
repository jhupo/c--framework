#include <iostream>

#include <thread>
#include <timer/timermanager.h>

void test(int i)
{
    std::cout << "ID: " << i << std::endl;
}

int main(int argc, char* argv[])
{

    COMMON_NAMESPACE::TimerManager::ptr timer(new COMMON_NAMESPACE::TimerManager);

    for(int i = 1; i < 4; ++i)
    {
        timer->addTimer(1000 * i, std::bind(test,i * 10000));
    }

    // timer->addTimer(1000,std::bind(test,1000));
    // timer->addTimer(2000,std::bind(test,2000));
    // timer->addTimer(3000,std::bind(test,3000));

    getchar();
    return 0;
}
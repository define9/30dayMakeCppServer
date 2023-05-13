#include <iostream>
#include <string>
#include "ThreadPool.h"

void print(int a, double b, const char *c, std::string d){
    std::cout << a << b << c << d << std::endl;
}

void test(){
    std::cout << "hellp" << std::endl;
}

int main()
{
    ThreadPool *pool = new ThreadPool();
    pool->start(5);
    pool->addTask(test);
    pool->addTask(test);
    pool->addTask(test);
    pool->addTask(test);
    pool->addTask(test);
    pool->addTask(test);
    pool->stop();
    delete pool;
    return 0;
}

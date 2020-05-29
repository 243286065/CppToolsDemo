#include "libgo.h"
#include "coroutine.h"

#include <iostream>

void f1() {
    std::cout << "---------------" << __FUNCTION__ << "--------1" << std::endl;
    co_yield;
    std::cout << "---------------" << __FUNCTION__ << "--------2" << std::endl;
    co_yield;
    std::cout << "---------------" << __FUNCTION__ << "--------3" << std::endl;
}

void f2() {
    go f1;
    std::cout << "---------------" << __FUNCTION__ << "--------1" << std::endl;
    co_yield;
    std::cout << "---------------" << __FUNCTION__ << "--------2" << std::endl;
    co_yield;
    std::cout << "---------------" << __FUNCTION__ << "--------3" << std::endl;
    co_yield;
    g_Scheduler.Stop();
}

int main() {
    go f2;
    std::cout << "---------------" << __FUNCTION__ << "--------go---------" << std::thread::hardware_concurrency() << std::endl;
    g_Scheduler.Start();
    std::cout << "end" << std::endl;
    return 0;
}
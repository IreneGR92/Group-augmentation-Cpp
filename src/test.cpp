#include <iostream>
#include "model/container/Container.h"

/* MAIN PROGRAM */
int main(int count, char **argv) {

    auto *test = new Container<int>();

//    auto *test = new Container<Parameters>();
//
//    std::cout << test.size();

    int four = 4;

    test->add(four);


    std::cout << "hl";

    return 0;
}
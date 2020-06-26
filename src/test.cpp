#include <iostream>
#include "model/container/Container.h"

/* MAIN PROGRAM */
int main(int count, char **argv) {

    auto *test = new Container<int>();

//    auto *test = new Container<Parameters>();

    int four = 4;
    int two = 2;

    test->add(four);
    test->add(two);

    test->remove(1); //by pointer instead of index if possible


    std::cout << "hl";

    return 0;
}
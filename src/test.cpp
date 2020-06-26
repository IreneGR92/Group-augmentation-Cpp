#include <iostream>
#include "model/container/Container.h"

/* MAIN PROGRAM */
int main(int count, char **argv) {

    auto *test = new Container<int>();
    auto *test2 = new Container<int>();

//    auto *test = new Container<Parameters>();

    int four = 4;
    int two = 2;
    int one = 1;

    if (test->isEmpty()){std::cout << "vector empty" << std::endl;} else{std::cout << "vector NOT empty"<< std::endl;};
    test->add(four);
    test->add(two);
    test->add(one);
    test->remove(1); //by pointer instead of index if possible
    test->add(two);
    if (test->isEmpty()){std::cout << "vector empty"<< std::endl;} else{std::cout << "vector NOT empty"<< std::endl;};
    std::cout << test->accessElement(0)<< std::endl;

    test2->add(four);
    test2->add(four);
    test2->add(four);
    test2->add(four);
    test2->add(four);
    test->merge(*test2);

    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;
    std::cout << test->getRandomElement() << std::endl;

    int i = test->getRandomElement();

    std::cout << "all seems good!";

    delete test;
    delete test2;

    return 0;
}
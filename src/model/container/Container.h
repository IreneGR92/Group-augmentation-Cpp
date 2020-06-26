#ifndef GROUP_AUGMENTATION_CONTAINER_H
#define GROUP_AUGMENTATION_CONTAINER_H


#include <vector>
#include <algorithm>
#include <iostream>

template<class T>

class Container {

private:
    std::vector<T> *vector;
public:
    virtual ~Container();

public:
    Container();

    void test();

//
    void add(T &element);
//
//    const unsigned int size() const;
//
//    const bool isEmpty();
//
//    const T &accessElement(int index);
//
//    void merge(Container<T> container);
//
//    T &getRandomElement() const;
//
//    T remove(int index);

//    void shuffle();
//
//    void removeElement(size_t itemIndex);

};

template<class T>
Container<T>::Container() {
    vector = new std::vector<T>;
}

template<class T>
Container<T>::~Container() {
    delete vector;
}

template<class T>
void Container<T>::test() {
    std::cout << "gekdsfs";
}

template<class T>
void Container<T>::add(T &element) {
    vector->push_back(element);
}

#endif //GROUP_AUGMENTATION_CONTAINER_H

#ifndef GROUP_AUGMENTATION_CONTAINER_H
#define GROUP_AUGMENTATION_CONTAINER_H


#include <vector>
#include <algorithm>
#include <iostream>
#include "../../Parameters.h"

template<class T>

class Container {

private:
    std::vector<T> *vector;
public:
    virtual ~Container();

public:
    Container();

    const unsigned int size() const;

    const bool isEmpty();

    const T &accessElement(int index);

    //Modifiers
    void add(T &element);

    T remove(int index);

    void merge(Container<T> &container);

    T &getRandomElement() const;

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
const unsigned int Container<T>::size() const {
    vector->size();
}

template<class T>
const bool Container<T>::isEmpty() {
    vector->empty();
}

template<class T>
const T &Container<T>::accessElement(int index) {
    vector->at(index);
}


template<class T>
void Container<T>::add(T &element) {
    vector->push_back(element);
}

template<class T>
T Container<T>::remove(int index) {
    vector->erase(vector->begin() + index);
}

template<class T>
void Container<T>::merge(Container<T> &container) {
    vector->insert(vector->end(), container.vector->begin(), container.vector->end());
}

template<class T>
T &Container<T>::getRandomElement() const {
    Parameters *parameters = Parameters::instance();
    std::uniform_int_distribution<int> uniform(0, vector->size() - 1);
    int index = uniform(*parameters->getGenerator()); // selects a random index the noRelatednessGroupsID vector
    vector[index];

}


#endif //GROUP_AUGMENTATION_CONTAINER_H

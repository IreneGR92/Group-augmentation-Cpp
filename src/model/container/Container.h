#ifndef GROUP_AUGMENTATION_CONTAINER_H
#define GROUP_AUGMENTATION_CONTAINER_H


#include <vector>
#include <algorithm>

template<class T>

class Container {

private:
    std::vector<T> *vector;
public:
    virtual ~Container();

public:
    Container();


    void add(T &element);

    const unsigned int size() const;

    const bool isEmpty();

    const T &accessElement(int index);

    void merge(Container<T> container);

    T &getRandomElement() const;

    T remove(int index);

//    void shuffle();
//
//    void removeElement(size_t itemIndex);

};

#endif //GROUP_AUGMENTATION_CONTAINER_H

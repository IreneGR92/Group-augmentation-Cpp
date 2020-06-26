#include "Container.h"


#include <iostream>
#include "Container.h"



//

//
//
//template<class T>
//void Container<T>::add(T &element) {
//    vector->push_back(element);
//}
//
//template<class T>
//void Container<T>::merge(Container<T> container) {
//    this->insert(vector->end(), container.begin(), container.end());
//}
//
//
//template<class T>
//const unsigned int Container<T>::size() const {
//    return vector->size();
//}
//
//template<class T>
//const bool Container<T>::isEmpty() {
//    return vector->empty();
//}
//
//template<class T>
//const T &Container<T>::accessElement(int index) {
//    return vector->at(index);
//}
//
//template<class T>
//T Container<T>::remove(int index) {
//    return vector->erase(vector->begin() + index);
//}
//
//template<class T>
//T &Container<T>::getRandomElement() const {
//    Parameters *parameters = Parameters::instance();
//    std::uniform_int_distribution<int> uniform(0, vector->size() - 1);
//    int index = uniform(*parameters->getGenerator()); // selects a random index the noRelatednessGroupsID vector
//    return vector[index];
//
//}
//
//
//

#ifndef GROUP_AUGMENTATION_INDIVIDUALVECTOR_H
#define GROUP_AUGMENTATION_INDIVIDUALVECTOR_H


#include <vector>
#include <algorithm>
#include "../Individual.h"

struct IndividualVector : std::vector<Individual> {
    std::vector<double> get(Attribute) const;

    void merge(std::vector<Individual> vector);

    void removeIndividual(size_t itemIndex);

};
#endif //GROUP_AUGMENTATION_INDIVIDUALVECTOR_H

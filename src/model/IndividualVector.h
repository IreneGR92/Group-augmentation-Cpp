#ifndef GROUP_AUGMENTATION_INDIVIDUALVECTOR_H
#define GROUP_AUGMENTATION_INDIVIDUALVECTOR_H


#include <vector>
#include "Individual.h"

struct IndividualVector : std::vector<Individual> {

    std::vector<double> get(GeneType) const;

    std::vector<Individual> getRandomSample();


    IndividualVector();

private:
    Parameters *parameters;
};


#endif //GROUP_AUGMENTATION_INDIVIDUALVECTOR_H

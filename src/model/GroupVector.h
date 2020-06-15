//
// Created by odin on 6/15/20.
//

#ifndef GROUP_AUGMENTATION_GROUPVECTOR_H
#define GROUP_AUGMENTATION_GROUPVECTOR_H


#include "Individual.h"

class GroupVector : std::vector<Individual> {
    std::vector<double> get(Attribute) const;
};


#endif //GROUP_AUGMENTATION_GROUPVECTOR_H

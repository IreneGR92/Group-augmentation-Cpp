
#ifndef GROUP_AUGMENTATION_INDIVIDUALCONTAINER_H
#define GROUP_AUGMENTATION_INDIVIDUALCONTAINER_H


#include "../Individual.h"
#include "Container.h"

class IndividualContainer : public Container<Individual> {
public:
    Container<double> get(Attribute type) const;
};


#endif //GROUP_AUGMENTATION_INDIVIDUALCONTAINER_H

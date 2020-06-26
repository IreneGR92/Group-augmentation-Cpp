
#include "IndividualContainer.h"

Container<double> IndividualContainer::get(Attribute type) const {
    Container<double> result;
    for (const Individual individual : this->vector) {
        result.add(individual.get(type));
    }
    return result;
}

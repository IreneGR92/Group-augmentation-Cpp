#include <algorithm>
#include "IndividualVector.h"

using namespace std;

std::vector<double> IndividualVector::get(Attribute type) const {
    std::vector<double> result;
    for (const auto &it : *this) {
        result.push_back(it.get(type));
    }
    return result;
}


void IndividualVector::merge(vector<Individual> vector) {
    this->insert(end(), vector.begin(), vector.end());
}

void IndividualVector::removeIndividual(size_t itemIndex) {
    std::swap(this->at(itemIndex), back());
    pop_back();
}

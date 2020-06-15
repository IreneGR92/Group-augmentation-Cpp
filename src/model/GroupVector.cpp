#include "GroupVector.h"

std::vector<double> GroupVector::get(Attribute attribute) const {
    std::vector<double> result;
    for (const auto &it : *this) {
        result.push_back(it.get(attribute));
    }
    return result;
}

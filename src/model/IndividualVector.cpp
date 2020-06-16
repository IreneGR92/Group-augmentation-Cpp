
#include "IndividualVector.h"

using namespace std;

std::vector<double> IndividualVector::get(Attribute type) const {
    std::vector<double> result;
    for (const auto &it : *this) {
        result.push_back(it.get(type));
    }
    return result;
}




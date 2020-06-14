
#include "IndividualVector.h"

using namespace std;

std::vector<double> IndividualVector::get(GeneType type) const {
    std::vector<double> result;
    vector<Individual>::iterator it;
    for (std::vector<int>::size_type i = 0; i != size(); i++) {
        result.push_back(it->get(type));
    }
    return result;
}




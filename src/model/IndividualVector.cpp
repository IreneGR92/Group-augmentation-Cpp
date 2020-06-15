
#include <set>
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

std::vector<Individual> IndividualVector::getRandomSample() {
    int sampleSizeFloater = round(
            size() * Parameters::instance()->getBiasFloatBreeder() / Parameters::instance()->getMaxColonies());

    std::vector<Individual> candidates;
    if (empty()) {
        return candidates;
    }


    //set is a container with each element being unique
    std::set<int> indexSet;


    if (size() > sampleSizeFloater) {
        while (indexSet.size() < sampleSizeFloater) {
            uniform_int_distribution<int> uniformFloat(0, size() - 1); //random floater ID taken in the sample
            indexSet.insert(uniformFloat(*parameters->getGenerator()));
        }
    } else {
        for (int i = 0; i < size(); i++) {
            indexSet.insert(i);
        }
    }

    candidates.reserve(indexSet.size());
    for (int index: indexSet) {
        candidates.push_back(at(index)); //add references of the floaters sampled to a vector
        erase(begin() + index);
    }
    return candidates;
}


IndividualVector::IndividualVector() {
    this->parameters = Parameters::instance();

}




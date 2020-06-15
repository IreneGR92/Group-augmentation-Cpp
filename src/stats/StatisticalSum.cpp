
#include "StatisticalSum.h"
#include <cmath>
#include <numeric>

double StatisticalSum::calculateStandardDeviation() {

    double mean = calculateMean();
    double value = 0;

    for (double attribute : individualValues) {
        value += pow(attribute - mean, 2);
    }

    return sqrt(value / individualValues.size());;
}

void StatisticalSum::addValue(double toAdd) {
    this->individualValues.push_back(toAdd);
}

double StatisticalSum::calculateMean() {
    double sum = std::accumulate(individualValues.begin(), individualValues.end(), 0.0);

    return sum / individualValues.size();
}

void StatisticalSum::merge(StatisticalSum statisticalSum) {
    this->individualValues.insert(individualValues.end(), statisticalSum.individualValues.begin(),
                                  statisticalSum.individualValues.end());
}

int StatisticalSum::size() {
    return this->individualValues.size();
}


void StatisticalSum::addValues(const std::vector<double> &values) {
    for (double value: values) {
        this->addValue(value);
    }
}


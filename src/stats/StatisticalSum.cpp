
#include "StatisticalSum.h"
#include <cmath>
#include <numeric>
#include <algorithm>
#include <functional>
#include <assert.h>


void StatisticalSum::addValue(double toAdd) {
    this->individualValues.push_back(toAdd);
}

double StatisticalSum::calculateMean() {
    double sum = std::accumulate(individualValues.begin(), individualValues.end(), 0.0);
    double counter = individualValues.size();

    if (counter > 0) {
        return sum / counter;
    } else {
        return -1; //TODO: or 0?
    }

}

double StatisticalSum::calculateSD() {
    double mean = this->calculateMean();
    double stdev;

    std::vector<double> diff(size());
    std::transform(individualValues.begin(), individualValues.end(), diff.begin(),
                   std::bind2nd(std::minus<double>(), mean));
    double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    if (individualValues.size() > 0) {
        stdev = std::sqrt(sq_sum / individualValues.size());
    } else {
        stdev = 0;
    }

    return stdev;
}

///Alternative implementation for SD, not so efficient
//double StatisticalSum::calculateStandardDeviation() {
//
//    double mean = calculateMean();
//    double value = 0;
//
//    for (double attribute : individualValues) {
//        value += pow(attribute - mean, 2);
//    }
//
//    return sqrt(value / individualValues.size());;
//}

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

std::vector<double> StatisticalSum::getValues() const {
    return this->individualValues;
}

double StatisticalSum::correlation(StatisticalSum y) {

    double meanY = y.calculateMean();
    double meanX = this->calculateMean();
    for (double singleY: y.getValues()) {


    }


    return 0;
}


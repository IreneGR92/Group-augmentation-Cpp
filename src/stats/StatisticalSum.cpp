
#include "StatisticalSum.h"
#include <cmath>
#include <numeric>
#include <algorithm>
#include <functional>
#include <assert.h>




double StatisticalSum::calculateMean() {

//    double sum =0;
//    double counter =0;
//
//    for (double attribute : individualValues) {
//        if(!std::isnan(attribute)){
//            sum += attribute;
//            counter++;
//        }
//    }
    double sum = std::accumulate(individualValues.begin(), individualValues.end(), 0.0);
    double counter = individualValues.size();

    if (counter > 0) {
        return sum / counter;
    } else {
        return 0; //TODO: or -1?
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

double StatisticalSum::correlation(StatisticalSum y) {

    double meanX = this->calculateMean();
    double meanY = y.calculateMean();
    double SD_X = this->calculateSD();
    double SD_Y = y.calculateSD();
    double Xi, Yi, X, Y;
    double sumProductXY = 0.0;
    double counter = this->size();
    assert(size() == y.size());
    double correlation;

    for (int i = 0; i < counter; i++) {
        Xi = this->getValues().at(i);
        Yi = y.getValues().at(i);
        X = (Xi - meanX);
        Y = (Yi - meanY);
        sumProductXY += X * Y;
    }

    if (SD_X * SD_Y * counter == 0) {
        correlation = 0;
    } else {
        correlation = sumProductXY / (SD_X * SD_Y * counter);
    }

    assert(abs(correlation) <= 1 && "[ERROR] correlation out of range");

    return correlation;
}


void StatisticalSum::addValue(double toAdd) {
    this->individualValues.push_back(toAdd);
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

std::vector<double> StatisticalSum::getValues() const {
    return this->individualValues;
}




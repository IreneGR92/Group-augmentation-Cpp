
#ifndef GROUP_AUGMENTATION_STATISTICALSUM_H
#define GROUP_AUGMENTATION_STATISTICALSUM_H


#include <vector>

class StatisticalSum {

    std::vector<double> individualValues;

public:
    double calculateStandardDeviation();

    void addValue(double toAdd);

    double calculateMean();

    void merge(StatisticalSum statisticalSum);

    int size();

};


#endif //GROUP_AUGMENTATION_STATISTICALSUM_H

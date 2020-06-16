
#ifndef GROUP_AUGMENTATION_STATISTICALSUM_H
#define GROUP_AUGMENTATION_STATISTICALSUM_H


#include <vector>

class StatisticalSum {

    std::vector<double> individualValues;

public:

    void addValue(double toAdd);

    void addValues(const std::vector<double> &values);

    double calculateMean();

    double calculateSD();

    double correlation(StatisticalSum y);

    int getMaxValue();

//    double calculateStandardDeviation();

    void merge(StatisticalSum statisticalSum);

    int size();

    std::vector<double> getValues() const;

};


#endif //GROUP_AUGMENTATION_STATISTICALSUM_H


#ifndef GROUP_AUGMENTATION_STATISTICALFORMULAS_H
#define GROUP_AUGMENTATION_STATISTICALFORMULAS_H

#include<vector>

class StatisticalFormulas {

    std::vector<double> individualValues;

public:

    void addValue(double toAdd);

    void addValues(const std::vector<double> &values);

    double calculateMean();

    double calculateSD();

    double correlation(StatisticalFormulas y);

    int getMaxValue();

//    double calculateStandardDeviation();

    void merge(StatisticalFormulas statisticalFormulas);

    int size();

    std::vector<double> getValues() const;

};


#endif //GROUP_AUGMENTATION_STATISTICALFORMULAS_H


#ifndef GROUP_AUGMENTATION_STATISTICALFORMULAS_H
#define GROUP_AUGMENTATION_STATISTICALFORMULAS_H


#include "../model/container/Container.h"

class StatisticalFormulas {

    Container<double> individualValues;

public:

    void addValue(double toAdd);

    void addValues(const Container<double> &values);

    double calculateMean();

    double calculateSD();

    double correlation(StatisticalFormulas y);

    int getMaxValue();

//    double calculateStandardDeviation();

    void merge(StatisticalFormulas statisticalSum);

    int size();

    Container<double> getValues() const;

};


#endif //GROUP_AUGMENTATION_STATISTICALFORMULAS_H

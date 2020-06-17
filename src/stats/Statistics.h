
#ifndef GROUP_AUGMENTATION_STATISTICS_H
#define GROUP_AUGMENTATION_STATISTICS_H


#include "../model/Group.h"
#include "../model/Individual.h"
#include "../Simulation.h"
#include "StatisticalSum.h"

class Statistics {

    Parameters *parameters;

    //Population parameters and Statistics

    int population, totalFloaters, totalHelpers, totalBreeders; //Counters

    //Relatedness
    double relatedness;

    StatisticalSum groupSize;

    StatisticalSum age, ageBreeders, ageFloaters, ageHelpers;

    StatisticalSum alpha, alphaAge;

    StatisticalSum beta, betaAge;

    StatisticalSum help, cumulativeHelp;

    StatisticalSum dispersal, dispersalHelpers;

    StatisticalSum survival, survivalBreeders, survivalFloaters, survivalHelpers;



    void printIndividual(Individual individual, int generation, int groupID, int replica);

public:
    Statistics();

    void calculateStatistics(std::vector<Group> groups, IndividualVector floaters);

    void printHeadersToConsole();

    void printHeadersToFile();

    double calculateRelatedness(std::vector<Group> groups);

    void printToConsole(int generation, int deaths);

    void printToFile(int replica, int generation, int deaths, int newBreederFloater,
                     int newBreederHelper, int inheritance);

    void printToFileLastGeneration(Simulation *simulation);
};


#endif //GROUP_AUGMENTATION_STATISTICS_H

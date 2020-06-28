
#ifndef GROUP_AUGMENTATION_STATISTICS_H
#define GROUP_AUGMENTATION_STATISTICS_H


#include "../model/Group.h"
#include "../model/Individual.h"
#include "../Simulation.h"
#include "StatisticalFormulas.h"
#include "../model/container/IndividualVector.h"

class Statistics {

    Parameters *parameters;

    //Population parameters and Statistics

    int population, totalFloaters, totalHelpers, totalBreeders; //Counters

    //Relatedness
    int driftGroupSize;
    double relatedness, corrHelpGroupSize;

    StatisticalFormulas groupSize;

    StatisticalFormulas age, ageBreeders, ageFloaters, ageHelpers;

    StatisticalFormulas alpha, alphaAge;

    StatisticalFormulas beta, betaAge;

    StatisticalFormulas help, cumulativeHelp;

    StatisticalFormulas dispersal, dispersalHelpers;

    StatisticalFormulas survival, survivalBreeders, survivalFloaters, survivalHelpers;

    StatisticalFormulas driftB, driftH, driftBB, driftHB;

    double meanDriftB, sumDriftB, meanDriftH, sumDriftH, meanDriftBH, meanDriftBB, sumDriftBH, sumDriftBB;          //relatedness related

    void printIndividual(Individual individual, int generation, int groupID, int replica);

public:
    Statistics();

    void calculateStatistics(const Population &populationObj);

    double calculateRelatedness(const std::vector<Group> &groups);

    double correlationHelpGroupSize(const std::vector<Group> &groups);

    void printHeadersToConsole();

    void printHeadersToFile();

    void printToConsole(int generation, int deaths);

    void printToFile(int replica, int generation, int deaths, int newBreederFloater,
                     int newBreederHelper, int inheritance);

    void printToFileLastGeneration(Simulation *simulation, const Population &populationObj);

};


#endif //GROUP_AUGMENTATION_STATISTICS_H

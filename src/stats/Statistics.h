
#ifndef GROUP_AUGMENTATION_STATISTICS_H
#define GROUP_AUGMENTATION_STATISTICS_H


#include "../model/Group.h"
#include "../model/Individual.h"
#include "../Simulation.h"

class Statistics {

    Parameters *parameters;

    //Population parameters and Statistics

    int population;

    int totalFloaters;

    int driftGroupSize, maxGroupSize, countGroupWithHelpers, countHelpers, countBreeders; //counters
    double relatedness;
    double meanGroupSize, stdevGroupSize, sumGroupSize, sumsqGroupSize, varGroupSize,
            meanGroupSizeHelp, stdevGroupSizeHelp, sumGroupSizeHelp, sumsqGroupSizeHelp, varGroupSizeHelp,
            meanAge, stdevAge, sumAge, sumsqAge, varAge,
            meanAgeHelper, stdevAgeHelper, sumAgeHelper, sumsqAgeHelper, varAgeHelper,
            meanAgeFloater, stdevAgeFloater, sumAgeFloater, sumsqAgeFloater, varAgeFloater,
            meanAgeBreeder, stdevAgeBreeder, sumAgeBreeder, sumsqAgeBreeder, varAgeBreeder,
            meanAlpha, stdevAlpha, sumAlpha, sumsqAlpha, varAlpha,
            meanAlphaAge, stdevAlphaAge, sumAlphaAge, sumsqAlphaAge, varAlphaAge,
            meanBeta, stdevBeta, sumBeta, sumsqBeta, varBeta,
            meanBetaAge, stdevBetaAge, sumBetaAge, sumsqBetaAge, varBetaAge,
            meanHelp, stdevHelp, sumHelp, sumsqHelp, varHelp,
            meanCumHelp, stdevCumHelp, sumCumHelp, sumsqCumHelp, varCumHelp,
            meanDispersal, stdevDispersal, sumDispersal, sumsqDispersal, varDispersal,
            meanSurvival, stdevSurvival, sumSurvival, sumsqSurvival, varSurvival,
            meanSurvivalBreeder, stdevSurvivalBreeder, sumSurvivalBreeder, sumsqSurvivalBreeder, varSurvivalBreeder,
            meanSurvivalHelper, stdevSurvivalHelper, sumSurvivalHelper, sumsqSurvivalHelper, varSurvivalHelper,
            meanSurvivalFloater, stdevSurvivalFloater, sumSurvivalFloater, sumsqSurvivalFloater, varSurvivalFloater,
            meanDriftB, sumDriftB, meanDriftH, sumDriftH,            //relatedness related
    meanDriftBH, meanDriftBB, sumDriftBH, sumDriftBB,        //relatedness related
    corr_HelpDispersal, sumprodHelpDispersal,
            corr_HelpGroup, sumprodHelpGroup;

    void printIndividual(Individual individual, int generation, int groupID, int replica);

public:
    Statistics();

    void calculateStatistics(std::vector<Group> groups, IndividualVector floaters);

    void printHeadersToConsole();

    void printHeadersToFile();

    void printToConsole(int generation, int deaths);

    void printToFile(int replica, int generation, int deaths, int newBreederFloater,
                     int newBreederHelper, int inheritance);

    void printToFileLastGeneration(Simulation *simulation);
};


#endif //GROUP_AUGMENTATION_STATISTICS_H

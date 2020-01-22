
#include <iostream>

#include "Statistics.h"
#include "model/Group.h"
#include "model/Individual.h"

using namespace std;

/* CALCULATE STATISTICS */
void Statistics::calculateStatistics(vector<Group> groups, vector<Individual> floaters) {

    population = 0, totalFloaters = 0, countGroupWithHelpers = 0, countHelpers = 0, countBreeders = 0,
    relatedness = 0.0, driftGroupSize = 0,
    meanGroupSize = 0.0, stdevGroupSize = 0.0, maxGroupSize = 0, sumGroupSize = 0.0, sumsqGroupSize = 0.0, varGroupSize = 0.0,
    meanGroupSizeHelp = 0.0, stdevGroupSizeHelp = 0.0, sumGroupSizeHelp = 0.0, sumsqGroupSizeHelp = 0.0, varGroupSizeHelp = 0.0,
    meanAge = 0.0, stdevAge = 0.0, sumAge = 0.0, sumsqAge = 0.0, varAge = 0.0,
    meanAgeHelper = 0.0, stdevAgeHelper = 0.0, sumAgeHelper = 0.0, sumsqAgeHelper = 0.0, varAgeHelper = 0.0,
    meanAgeFloater = 0.0, stdevAgeFloater = 0.0, sumAgeFloater = 0.0, sumsqAgeFloater = 0.0, varAgeFloater = 0.0,
    meanAgeBreeder = 0.0, stdevAgeBreeder = 0.0, sumAgeBreeder = 0.0, sumsqAgeBreeder = 0.0, varAgeBreeder = 0.0,
    meanAlpha = 0.0, stdevAlpha = 0.0, sumAlpha = 0.0, sumsqAlpha = 0.0, varAlpha = 0.0,
    meanAlphaAge = 0.0, stdevAlphaAge = 0.0, sumAlphaAge = 0.0, sumsqAlphaAge = 0.0, varAlphaAge = 0.0,
    meanBeta = 0.0, stdevBeta = 0.0, sumBeta = 0.0, sumsqBeta = 0.0, varBeta = 0.0,
    meanBetaAge = 0.0, stdevBetaAge = 0.0, sumBetaAge = 0.0, sumsqBetaAge = 0.0, varBetaAge = 0.0,
    meanHelp = 0.0, stdevHelp = 0.0, sumHelp = 0.0, sumsqHelp = 0.0, varHelp = 0.0,
    meanCumHelp = 0.0, stdevCumHelp = 0.0, sumCumHelp = 0.0, sumsqCumHelp = 0.0, varCumHelp = 0.0,
    meanDispersal = 0.0, stdevDispersal = 0.0, sumDispersal = 0.0, sumsqDispersal = 0.0, varDispersal = 0.0,
    meanSurvival = 0.0, stdevSurvival = 0.0, sumSurvival = 0.0, sumsqSurvival = 0.0, varSurvival = 0.0,
    meanSurvivalBreeder = 0.0, stdevSurvivalBreeder = 0.0, sumSurvivalBreeder = 0.0, sumsqSurvivalBreeder = 0.0, varSurvivalBreeder = 0.0,
    meanSurvivalHelper = 0.0, stdevSurvivalHelper = 0.0, sumSurvivalHelper = 0.0, sumsqSurvivalHelper = 0.0, varSurvivalHelper = 0.0,
    meanSurvivalFloater = 0.0, stdevSurvivalFloater = 0.0, sumSurvivalFloater = 0.0, sumsqSurvivalFloater = 0.0, varSurvivalFloater = 0.0,
    meanDriftB = 0.0, sumDriftB = 0.0, meanDriftH = 0.0, sumDriftH = 0.0,
    meanDriftBH = 0.0, meanDriftBB = 0.0, sumDriftBH = 0.0, sumDriftBB = 0.0,
    corr_HelpDispersal = 0.0, sumprodHelpDispersal = 0.0,
    corr_HelpGroup = 0.0, sumprodHelpGroup = 0.0;


    vector<Group, std::allocator<Group >>::iterator
            groupStatsIt;
    for (groupStatsIt = groups.begin(); groupStatsIt < groups.end(); ++groupStatsIt) {

        // HELPERS
        vector<Individual, std::allocator<Individual >>::iterator
                helperStatsIt; //helpers
        for (helperStatsIt = groupStatsIt->helpers.begin();
             helperStatsIt < groupStatsIt->helpers.end(); ++helperStatsIt) {

            // Genes
            sumAlpha += helperStatsIt->getAlpha();
            sumsqAlpha += helperStatsIt->getAlpha() * helperStatsIt->getAlpha();

            sumAlphaAge += helperStatsIt->getAlphaAge();
            sumsqAlphaAge += helperStatsIt->getAlphaAge() * helperStatsIt->getAlphaAge();

            sumBeta += helperStatsIt->getBeta();
            sumsqBeta += helperStatsIt->getBeta() * helperStatsIt->getBeta();

            sumBetaAge += helperStatsIt->getBetaAge();
            sumsqBetaAge += helperStatsIt->getBetaAge() * helperStatsIt->getBetaAge();

            //Relatedness
            if (groupStatsIt->isBreederAlive()) {
                sumDriftB += groupStatsIt->breeder.getDrift();
                sumDriftH += helperStatsIt->getDrift();
                sumDriftBH += helperStatsIt->getDrift() * groupStatsIt->breeder.getDrift();
                sumDriftBB += groupStatsIt->breeder.getDrift() * groupStatsIt->breeder.getDrift();
                ++driftGroupSize;
            }

            //Phenotypes
            sumAgeHelper += helperStatsIt->getAge();
            sumsqAgeHelper += helperStatsIt->getAge() * helperStatsIt->getAge();

            if (!isnan(helperStatsIt->getHelp())) {
                sumHelp += helperStatsIt->getHelp();
                sumsqHelp += helperStatsIt->getHelp() * helperStatsIt->getHelp();
            }

            if (!isnan(helperStatsIt->getDispersal())) {
                sumDispersal += helperStatsIt->getDispersal();
                sumsqDispersal += helperStatsIt->getDispersal() * helperStatsIt->getDispersal();

                sumprodHelpDispersal += helperStatsIt->getHelp() * helperStatsIt->getDispersal();
            }

            if (!isnan(helperStatsIt->getSurvival())) {
                sumSurvivalHelper += helperStatsIt->getSurvival();
                sumsqSurvivalHelper += helperStatsIt->getSurvival() * helperStatsIt->getSurvival();
            }
        }

        countHelpers += groupStatsIt->helpers.size();

        //BREEDERS
        if (groupStatsIt->isBreederAlive()) {

            //Genes
            sumAlpha += groupStatsIt->breeder.getAlpha();
            sumsqAlpha += groupStatsIt->breeder.getAlpha() * groupStatsIt->breeder.getAlpha();

            sumAlphaAge += groupStatsIt->breeder.getAlphaAge();
            sumsqAlphaAge += groupStatsIt->breeder.getAlphaAge() * groupStatsIt->breeder.getAlphaAge();

            sumBeta += groupStatsIt->breeder.getBeta();
            sumsqBeta += groupStatsIt->breeder.getBeta() * groupStatsIt->breeder.getBeta();

            sumBetaAge += groupStatsIt->breeder.getBetaAge();
            sumsqBetaAge += groupStatsIt->breeder.getBetaAge() * groupStatsIt->breeder.getBetaAge();

            //Phenotypes
            sumAgeBreeder += groupStatsIt->breeder.getAge();
            sumsqAgeBreeder += groupStatsIt->breeder.getAge() * groupStatsIt->breeder.getAge();

            sumSurvivalBreeder += groupStatsIt->breeder.getSurvival();
            sumsqSurvivalBreeder += groupStatsIt->breeder.getSurvival() * groupStatsIt->breeder.getSurvival();

            countBreeders++;
        }

        //Group
        groupStatsIt->calcGroupSize();
        sumGroupSize += groupStatsIt->getGroupSize();
        sumsqGroupSize += groupStatsIt->getGroupSize() * groupStatsIt->getGroupSize();
        if (groupStatsIt->getGroupSize() > maxGroupSize) maxGroupSize = groupStatsIt->getGroupSize();


        if (groupStatsIt->isHelpersPresent()) {
            sumCumHelp += groupStatsIt->getCumHelp();
            sumsqCumHelp += groupStatsIt->getCumHelp() * groupStatsIt->getCumHelp();
            sumGroupSizeHelp += groupStatsIt->getGroupSize();
            sumsqGroupSizeHelp += groupStatsIt->getGroupSize() * groupStatsIt->getGroupSize();
            sumprodHelpGroup += groupStatsIt->getCumHelp() * groupStatsIt->getGroupSize();

            countGroupWithHelpers++;
        }
    }

    //FLOATERS

    totalFloaters = floaters.size();

    vector<Individual, std::allocator<Individual >>::iterator
            floatStatsIt;
    for (floatStatsIt = floaters.begin(); floatStatsIt < floaters.end(); ++floatStatsIt) {
        // Genes
        sumAlpha += floatStatsIt->getAlpha();
        sumsqAlpha += floatStatsIt->getAlpha() * floatStatsIt->getAlpha();

        sumAlphaAge += floatStatsIt->getAlphaAge();
        sumsqAlphaAge += floatStatsIt->getAlphaAge() * floatStatsIt->getAlphaAge();

        sumBeta += floatStatsIt->getBeta();
        sumsqBeta += floatStatsIt->getBeta() * floatStatsIt->getBeta();

        sumBetaAge += floatStatsIt->getBetaAge();
        sumsqBetaAge += floatStatsIt->getBetaAge() * floatStatsIt->getBetaAge();

        //Phenotypes
        sumAgeFloater += floatStatsIt->getAge();
        sumsqAgeFloater += floatStatsIt->getAge() * floatStatsIt->getAge();

        if (!isnan(floatStatsIt->getDispersal())) {
            sumDispersal += floatStatsIt->getDispersal();
            sumsqDispersal += floatStatsIt->getDispersal() * floatStatsIt->getDispersal();
        }

        if (!isnan(floatStatsIt->getSurvival())) {
            sumSurvivalFloater += floatStatsIt->getSurvival();
            sumsqSurvivalFloater += floatStatsIt->getSurvival() * floatStatsIt->getSurvival();
        }
    }

    sumAge = sumAgeHelper + sumAgeFloater + sumAgeBreeder;
    sumsqAge = sumsqAgeHelper + sumsqAgeFloater + sumsqAgeBreeder;

    sumSurvival = sumSurvivalHelper + sumSurvivalFloater + sumSurvivalBreeder;
    sumsqSurvival = sumsqSurvivalHelper + sumsqSurvivalFloater + sumsqSurvivalBreeder;


    //MEANS
    population = countBreeders + countHelpers + totalFloaters;

    meanGroupSize = sumGroupSize / parameters.getMaxColonies();
    countGroupWithHelpers == 0 ? meanGroupSizeHelp = 0 : meanGroupSizeHelp = sumGroupSizeHelp / countGroupWithHelpers;

    meanAlpha = sumAlpha / population;
    meanAlphaAge = sumAlphaAge / population;
    meanBeta = sumBeta / population;
    meanBetaAge = sumBetaAge / population;
    if (driftGroupSize != 0) {
        meanDriftB = sumDriftB / driftGroupSize;
        meanDriftH = sumDriftH / driftGroupSize;
        meanDriftBH = sumDriftBH / driftGroupSize;
        meanDriftBB = sumDriftBB / driftGroupSize;
    }

    meanAge = sumAge / population;
    meanAgeHelper = sumAgeHelper / countHelpers;
    meanAgeFloater = sumAgeFloater / totalFloaters;
    meanAgeBreeder = sumAgeBreeder / countBreeders;

    countHelpers == 0 ? meanHelp = 0 : meanHelp = sumHelp / countHelpers;
    countGroupWithHelpers == 0 ? meanCumHelp = 0 : meanCumHelp = sumCumHelp / countGroupWithHelpers;
    meanDispersal = sumDispersal / (countHelpers + totalFloaters);

    meanSurvival = sumSurvival / population;
    meanSurvivalHelper = sumSurvivalHelper / countHelpers;
    meanSurvivalFloater = sumSurvivalFloater / totalFloaters;
    meanSurvivalBreeder = sumSurvivalBreeder / countBreeders;

    relatedness = (meanDriftBH - meanDriftB * meanDriftH) /
                  (meanDriftBB - meanDriftB * meanDriftB); //covariate of a neutral selected gene
    if ((meanDriftBB - meanDriftB * meanDriftB) == 0 ||
        driftGroupSize == 0) { relatedness = Parameters::NO_VALUE; } //prevent to divide by 0


    //Variance
    varGroupSize = sumsqGroupSize / parameters.getMaxColonies() - meanGroupSize * meanGroupSize;
    varGroupSizeHelp = sumsqGroupSizeHelp / countGroupWithHelpers - meanGroupSizeHelp * meanGroupSizeHelp;

    varAlpha = sumsqAlpha / population - meanAlpha * meanAlpha;
    varAlphaAge = sumsqAlphaAge / population - meanAlphaAge * meanAlphaAge;
    varBeta = sumsqBeta / population - meanBeta * meanBeta;
    varBetaAge = sumsqBetaAge / population - meanBetaAge * meanBetaAge;

    varAge = sumsqAge / population - meanAge * meanAge;
    varAgeHelper = sumsqAgeHelper / countHelpers - meanAgeHelper * meanAgeHelper;
    varAgeFloater = sumsqAgeFloater / totalFloaters - meanAgeFloater * meanAgeFloater;
    varAgeBreeder = sumsqAgeBreeder / countBreeders - meanAgeBreeder * meanAgeBreeder;

    varHelp = sumsqHelp / countHelpers - meanHelp * meanHelp;
    varCumHelp = sumsqCumHelp / countGroupWithHelpers - meanCumHelp * meanCumHelp;
    varDispersal = sumsqDispersal / (countHelpers + totalFloaters) - meanDispersal * meanDispersal;

    varSurvival = sumsqSurvival / population - meanSurvival * meanSurvival;
    varSurvivalHelper = sumsqSurvivalHelper / countHelpers - meanSurvivalHelper * meanSurvivalHelper;
    varSurvivalFloater = sumsqSurvivalFloater / totalFloaters - meanSurvivalFloater * meanSurvivalFloater;
    varSurvivalBreeder = sumsqSurvivalBreeder / countBreeders - meanSurvivalBreeder * meanSurvivalBreeder;

    // SD
    /*if (varGroupSize < 0 || varAlpha < 0 || varBeta < 0 || varAge < 0 ||
        varDispersal < 0 | varHelp < 0 || varCumHelp < 0 || varSurvival < 0) {
        cout << "error variance negative" << endl;
    }*/

    varGroupSize > 0 ? stdevGroupSize = sqrt(varGroupSize) : stdevGroupSize = 0;
    varGroupSizeHelp > 0 ? stdevGroupSizeHelp = sqrt(varGroupSizeHelp) : stdevGroupSizeHelp = 0;

    varAlpha > 0 ? stdevAlpha = sqrt(varAlpha) : stdevAlpha = 0;
    varAlphaAge > 0 ? stdevAlphaAge = sqrt(varAlphaAge) : stdevAlphaAge = 0;
    varBeta > 0 ? stdevBeta = sqrt(varBeta) : stdevBeta = 0;
    varBetaAge > 0 ? stdevBetaAge = sqrt(varBetaAge) : stdevBetaAge = 0;

    varAge > 0 ? stdevAge = sqrt(varAge) : stdevAge = 0;
    varAgeHelper > 0 ? stdevAgeHelper = sqrt(varAgeHelper) : stdevAgeHelper = 0;
    varAgeFloater > 0 ? stdevAgeFloater = sqrt(varAgeFloater) : stdevAgeFloater = 0;
    varAgeBreeder > 0 ? stdevAgeBreeder = sqrt(varAgeBreeder) : stdevAgeBreeder = 0;

    varHelp > 0 ? stdevHelp = sqrt(varHelp) : stdevHelp = 0;
    varCumHelp > 0 ? stdevCumHelp = sqrt(varCumHelp) : stdevCumHelp = 0;
    varDispersal > 0 ? stdevDispersal = sqrt(varDispersal) : stdevDispersal = 0;

    varSurvival > 0 ? stdevSurvival = sqrt(varSurvival) : stdevSurvival = 0;
    varSurvivalHelper > 0 ? stdevSurvivalHelper = sqrt(varSurvivalHelper) : stdevSurvivalHelper = 0;
    varSurvivalFloater > 0 ? stdevSurvivalFloater = sqrt(varSurvivalFloater) : stdevSurvivalFloater = 0;
    varSurvivalBreeder > 0 ? stdevSurvivalBreeder = sqrt(varSurvivalBreeder) : stdevSurvivalBreeder = 0;

    //Correlations
    (countHelpers > 0 && stdevHelp > 0 && stdevDispersal > 0) ? corr_HelpDispersal =
                                                                        (sumprodHelpDispersal / countHelpers -
                                                                         meanHelp * meanDispersal) /
                                                                        (stdevHelp * stdevDispersal)
                                                              : corr_HelpDispersal = 0;

    (countGroupWithHelpers > 0 && stdevCumHelp > 0 && stdevGroupSize > 0) ? corr_HelpGroup =
                                                                                    (sumprodHelpGroup /
                                                                                     countGroupWithHelpers -
                                                                                     meanCumHelp * meanGroupSizeHelp) /
                                                                                    (stdevCumHelp * stdevGroupSizeHelp)
                                                                          : corr_HelpGroup = 0;

}

void Statistics::printHeadersToConsole() {
    // column headings on screen
    cout << setw(6) << "gen" << setw(9) << "pop" << setw(9) << "deaths" << setw(9)
         << "float" << setw(9) << "group" << setw(9) << "maxGroup" << setw(9) << "age" << setw(9)
         << "alpha" << setw(9) << "alphaAge" << setw(9) << "beta" << setw(9) << "betaAge" << setw(9)
         << "help" << setw(9) << "disper" << setw(9) << "surv" << setw(9) << "relat" << endl;
}


void Statistics::printToConsole(int generation, int deaths) {
    // show values on screen
    std::cout << fixed << showpoint
              << setw(6) << generation
              << setw(9) << population
              << setw(9) << deaths
              << setw(9) << totalFloaters
              << setw(9) << setprecision(2) << meanGroupSize
              << setw(9) << maxGroupSize
              << setw(9) << setprecision(2) << meanAge
              << setw(9) << setprecision(4) << meanAlpha
              << setw(9) << setprecision(4) << meanAlphaAge
              << setw(9) << setprecision(4) << meanBeta
              << setw(9) << setprecision(4) << meanBetaAge
              << setw(9) << setprecision(4) << meanHelp
              << setw(9) << setprecision(4) << meanDispersal
              << setw(9) << setprecision(2) << meanSurvival
              << setw(9) << setprecision(2) << relatedness
              << endl;
}

void Statistics::printHeadersToFile() {
    // column headings in output file main
    *parameters.getMainWriter() << "Replica" << "\t" << "Generation" << "\t" << "Population" << "\t"
                                << "Deaths" << "\t" << "Floaters" << "\t"
                                << "Group_size" << "\t" << "Age" << "\t" << "Age_H" << "\t" << "Age_F"
                                << "\t" << "Age_B" << "\t"
                                << "meanAlpha" << "\t" << "meanAlphaAge" << "\t" << "meanBeta" << "\t"
                                << "meanBetaAge" << "\t"
                                << "meanHelp" << "\t" << "meanCumHelp" << "\t" << "meanDispersal" << "\t"
                                << "meanSurvival" << "\t" << "meanSurvival_H" << "\t" << "meanSurvival_F" << "\t"
                                << "meanSurvival_B" << "\t" << "Relatedness" << "\t"
                                << "SD_GroupSize" << "\t" << "SD_Age" << "\t" << "SD_Age_H" << "\t"
                                << "SD_Age_F" << "\t" << "SD_Age_B" << "\t"
                                << "SD_Alpha" << "\t" << "SD_AlphaAge" << "\t" << "SD_Beta" << "\t"
                                << "SD_BetaAge" << "\t"
                                << "SD_Help" << "\t" << "SD_CumHelp" << "\t" << "SD_Dispersal" << "\t"
                                << "SD_Survival" << "\t" << "SD_Survival_H" << "\t" << "SD_Survival_F"
                                << "\t" << "SD_Survival_B" << "\t"
                                << "corr_Help_Disp" << "\t" << "corr_Help_Group" << "\t"
                                << "newBreederFloater" << "\t" << "newBreederHelper" << "\t"
                                << "inheritance" << endl;

    // column headings in output file last generation
    *parameters.getLastGenerationWriter() << "replica" << "\t" << "generation" << "\t" << "groupID"
                                          << "\t" << "type" << "\t" << "age" << "\t"
                                          << "alpha" << "\t" << "alphaAge" << "\t" << "beta" << "\t"
                                          << "betaAge" << "\t" << "drift"
                                          << "\t" << "help" << "\t" << "dispersal" << "\t" << "survival"
                                          << endl;
}

void Statistics::printToFile(int replica, int generation, int deaths, int newBreederFloater,
                             int newBreederHelper, int inheritance) {

    // Output file

    // write values to output file
    *parameters.getMainWriter() << fixed << showpoint
                                << replica << 1
                                << "\t" << generation
                                << "\t" << population
                                << "\t" << deaths
                                << "\t" << totalFloaters
                                << "\t" << setprecision(4) << meanGroupSize
                                << "\t" << setprecision(4) << meanAge
                                << "\t" << setprecision(4) << meanAgeHelper
                                << "\t" << setprecision(4) << meanAgeFloater
                                << "\t" << setprecision(4) << meanAgeBreeder
                                << "\t" << setprecision(4) << meanAlpha
                                << "\t" << setprecision(4) << meanAlphaAge
                                << "\t" << setprecision(4) << meanBeta
                                << "\t" << setprecision(4) << meanBetaAge
                                << "\t" << setprecision(4) << meanHelp
                                << "\t" << setprecision(4) << meanCumHelp
                                << "\t" << setprecision(4) << meanDispersal
                                << "\t" << setprecision(4) << meanSurvival
                                << "\t" << setprecision(4) << meanSurvivalHelper
                                << "\t" << setprecision(4) << meanSurvivalFloater
                                << "\t" << setprecision(4) << meanSurvivalBreeder
                                << "\t" << setprecision(4) << relatedness
                                << "\t" << setprecision(4) << stdevGroupSize
                                << "\t" << setprecision(4) << stdevAge
                                << "\t" << setprecision(4) << stdevAgeHelper
                                << "\t" << setprecision(4) << stdevAgeFloater
                                << "\t" << setprecision(4) << stdevAgeBreeder
                                << "\t" << setprecision(4) << stdevAlpha
                                << "\t" << setprecision(4) << stdevAlphaAge
                                << "\t" << setprecision(4) << stdevBeta
                                << "\t" << setprecision(4) << stdevBetaAge
                                << "\t" << setprecision(4) << stdevHelp
                                << "\t" << setprecision(4) << stdevCumHelp
                                << "\t" << setprecision(4) << stdevDispersal
                                << "\t" << setprecision(4) << stdevSurvival
                                << "\t" << setprecision(4) << stdevSurvivalHelper
                                << "\t" << setprecision(4) << stdevSurvivalFloater
                                << "\t" << setprecision(4) << stdevSurvivalBreeder
                                << "\t" << setprecision(4) << corr_HelpDispersal
                                << "\t" << setprecision(4) << corr_HelpGroup
                                << "\t" << newBreederFloater
                                << "\t" << newBreederHelper
                                << "\t" << inheritance
                                << endl;
}


void Statistics::printToFileLastGeneration(const std::vector<Group> &groups, int replica, int generation) {

    int groupID = 0;
    int counter = 0;

    for (auto const &group: groups) {
        if (counter < 100) {
            *parameters.getLastGenerationWriter() << fixed << showpoint
                                                  << replica + 1
                                                  << "\t" << generation
                                                  << "\t" << groupID
                                                  << "\t" << group.breeder.getFishType()
                                                  << "\t" << setprecision(4) << group.breeder.getAge()
                                                  << "\t" << setprecision(4) << group.breeder.getAlpha()
                                                  << "\t" << setprecision(4)
                                                  << group.breeder.getAlphaAge()
                                                  << "\t" << setprecision(4) << group.breeder.getBeta()
                                                  << "\t" << setprecision(4) << group.breeder.getBetaAge()
                                                  << "\t" << setprecision(4) << group.breeder.getDrift()
                                                  << "\t" << setprecision(4) << "NA"
                                                  << "\t" << setprecision(4) << "NA"
                                                  << "\t" << setprecision(4)
                                                  << group.breeder.getSurvival()
                                                  << endl;

            for (auto const &helper: group.helpers) { //TODO: floaters are missing in the output

                *parameters.getLastGenerationWriter() << fixed << showpoint
                                                      << replica + 1
                                                      << "\t" << generation
                                                      << "\t" << groupID
                                                      << "\t" << helper.getFishType()
                                                      << "\t" << setprecision(4) << helper.getAge()
                                                      << "\t" << setprecision(4) << helper.getAlpha()
                                                      << "\t" << setprecision(4) << helper.getAlphaAge()
                                                      << "\t" << setprecision(4) << helper.getBeta()
                                                      << "\t" << setprecision(4) << helper.getBetaAge()
                                                      << "\t" << setprecision(4) << helper.getDrift()
                                                      << "\t" << setprecision(4)
                                                      << helper.getHelp() // TODO: for floaters is 0 no NA!
                                                      << "\t" << setprecision(4) << helper.getDispersal()
                                                      << "\t" << setprecision(4) << helper.getSurvival()
                                                      << endl;
            }
            counter++;
        }
        groupID++;
    }
}

Statistics::Statistics(Parameters &parameters) : parameters(parameters) {}





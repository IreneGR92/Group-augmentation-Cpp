
#include <iostream>
#include <assert.h>

#include "Statistics.h"
#include "../model/Group.h"
#include "../model/Individual.h"

using namespace std;

/* CALCULATE STATISTICS */
void Statistics::calculateStatistics(vector<Group> groups, IndividualVector floaters) {

    IndividualVector breeders;
    IndividualVector helpers;
    IndividualVector individualsAll;
    std::vector<double> groupSizes;
    std::vector<double> cumHelps;

    vector<Group, std::allocator<Group >>::iterator groupsIt;
    for (groupsIt = groups.begin(); groupsIt < groups.end(); ++groupsIt) {
        if (groupsIt->isBreederAlive()) {
            breeders.push_back(groupsIt->getBreeder());
        }
        helpers.insert(helpers.end(), groupsIt->getHelpers().begin(),
                       groupsIt->getHelpers().end());

        groupSizes.push_back(groupsIt->getGroupSize());
        cumHelps.push_back(groupsIt->getCumHelp());
    }

    individualsAll.insert(individualsAll.end(), helpers.begin(), helpers.end());
    individualsAll.insert(individualsAll.end(), floaters.begin(), floaters.end());
    individualsAll.insert(individualsAll.end(), breeders.begin(), breeders.end());

    //Counters
    totalHelpers = helpers.size();
    totalFloaters = floaters.size();
    totalBreeders = breeders.size();
    population = totalBreeders + totalHelpers + totalFloaters;
    assert(population > 0);

    //Initialize the stats

    //Genes
    alpha.addValues(individualsAll.get(ALPHA));
    alphaAge.addValues(individualsAll.get(ALPHA_AGE));
    beta.addValues(individualsAll.get(BETA));
    betaAge.addValues(individualsAll.get(BETA_AGE));

    //Phenotypes
    age.addValues(individualsAll.get(AGE));
    ageHelpers.addValues(helpers.get(AGE));
    ageBreeders.addValues(breeders.get(AGE));
    ageFloaters.addValues(floaters.get(AGE));

    help.addValues(helpers.get(HELP));

    dispersal.addValues(helpers.get(DISPERSAL));
    dispersal.addValues(floaters.get(DISPERSAL));
    dispersalHelpers.addValues(helpers.get(DISPERSAL));

    survival.addValues(individualsAll.get(SURVIVAL));
    survivalHelpers.addValues(helpers.get(SURVIVAL));
    survivalBreeders.addValues(breeders.get(SURVIVAL));
    survivalFloaters.addValues(floaters.get(SURVIVAL));

    //Group attributes
    groupSize.addValues(groupSizes);
    cumulativeHelp.addValues(cumHelps);


    //Correlations in different levels
    relatedness = calculateRelatedness (groups);
    corrHelpGroupSize = correlationHelpGroupSize (groups);

}

double Statistics::calculateRelatedness(std::vector<Group> groups) { //TODO: optimise formula, make abstract version

    //Relatedness
    double correlation;          //relatedness related
    int counter = 0;
    double meanX, meanY, sumX = 0.0, sumY = 0.0;
    double sumProductXY = 0, sumProductXX = 0, sumProductYY = 0;

    vector<Group, std::allocator<Group >>::iterator groupsIt;
    for (groupsIt = groups.begin(); groupsIt < groups.end(); ++groupsIt) {
        vector<Individual, std::allocator<Individual >>::iterator helperIt;
        for (helperIt = groupsIt->helpers.begin();
             helperIt < groupsIt->helpers.end(); ++helperIt) {

            if (groupsIt->isBreederAlive() && !groupsIt->helpers.empty()) {
                sumX += helperIt->getDrift();
                sumY += groupsIt->breeder.getDrift();
                counter++;
            }
        }
    }

    if (counter != 0) {
        meanX = sumX / counter;
        meanY = sumY / counter;
    }

    for (groupsIt = groups.begin(); groupsIt < groups.end(); ++groupsIt) {

        // HELPERS
        vector<Individual, std::allocator<Individual >>::iterator helperIt; //helpers
        for (helperIt = groupsIt->helpers.begin();
             helperIt < groupsIt->helpers.end(); ++helperIt) {
            if (!isnan(helperIt->getDispersal()) || !isnan(helperIt->getHelp())) {
                double X = (helperIt->getDrift() - meanX);
                double Y = (groupsIt->breeder.getDrift() - meanY);

                sumProductXY += X * Y;
                sumProductXX += X * X;
                sumProductYY += Y * Y;
            }
        }
    }
    double stdevX = sqrt(sumProductXX / counter);
    double stdevY = sqrt(sumProductYY / counter);

    if (stdevX * stdevY * counter == 0) {
        correlation = 0;
    } else {
        correlation = sumProductXY / (stdevX * stdevY * counter);
    }
    assert (abs(correlation) >= 0);
    return correlation;


    //Old version
//    vector<Group, std::allocator<Group >>::iterator groupsIt;
//    for (groupsIt = groups.begin(); groupsIt < groups.end(); ++groupsIt) {
//        vector<Individual, std::allocator<Individual >>::iterator helperIt;
//        for (helperIt = groupsIt->helpers.begin();
//             helperIt < groupsIt->helpers.end(); ++helperIt) {
//
//            if (groupsIt->isBreederAlive() && !groupsIt->helpers.empty()) {
//                sumDriftB += groupsIt->breeder.getDrift();
//                sumX += helperIt->getDrift();
//                sumDriftBH += helperIt->getDrift() * groupsIt->breeder.getDrift();
//                sumDriftBB += groupsIt->breeder.getDrift() * groupsIt->breeder.getDrift();
//                driftGroupSize++;
//            }
//        }
//    }
//
//    if (driftGroupSize != 0) {
//        meanDriftB = sumDriftB / driftGroupSize;
//        meanX = sumX / driftGroupSize;
//        meanDriftBH = sumDriftBH / driftGroupSize;
//        meanDriftBB = sumDriftBB / driftGroupSize;
//    }
//
//    relatedness = (meanDriftBH - meanDriftB * meanX) /
//                  (meanDriftBB - meanDriftB * meanDriftB); //covariate of a neutral selected gene
//    if ((meanDriftBB - meanDriftB * meanDriftB) == 0 || driftGroupSize == 0) {
//        relatedness = Parameters::NO_VALUE; //prevent to divide by 0
//    }
}

double Statistics::correlationHelpGroupSize(std::vector<Group> groups) { //TODO: optimise formula, make abstract version

    double correlation;
    int counter = 0;
    double meanX, meanY, sumX = 0.0, sumY = 0.0;
    double sumProductXY = 0, sumProductXX = 0, sumProductYY = 0;

    vector<Group, std::allocator<Group >>::iterator groupsIt;
    for (groupsIt = groups.begin(); groupsIt < groups.end(); ++groupsIt) {
        vector<Individual, std::allocator<Individual >>::iterator helperIt;
        for (helperIt = groupsIt->helpers.begin();
             helperIt < groupsIt->helpers.end(); ++helperIt) {

            if (!groupsIt->helpers.empty()) {
                sumX += helperIt->getHelp();
                sumY += groupsIt->getGroupSize();
                counter++;
            }
        }
    }

    if (counter != 0) {
        meanX = sumX / counter;
        meanY = sumY / counter;
    }

    for (groupsIt = groups.begin(); groupsIt < groups.end(); ++groupsIt) {

        // HELPERS
        vector<Individual, std::allocator<Individual >>::iterator helperIt; //helpers
        for (helperIt = groupsIt->helpers.begin();
             helperIt < groupsIt->helpers.end(); ++helperIt) {
            if (!isnan(helperIt->getDispersal()) || !isnan(helperIt->getHelp())) {
                double X = (helperIt->getHelp() - meanX);
                double Y = (groupsIt->getGroupSize() - meanY);

                sumProductXY += X * Y;
                sumProductXX += X * X;
                sumProductYY += Y * Y;
            }
        }
    }
    double stdevX = sqrt(sumProductXX / counter);
    double stdevY = sqrt(sumProductYY / counter);

    if (stdevX * stdevY * counter == 0) {
        correlation = 0;
    } else {
        correlation = sumProductXY / (stdevX * stdevY * counter);
    }

    assert (abs(correlation) >= 0);
    return correlation;
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
              << setw(9) << setprecision(2) << groupSize.calculateMean()
              << setw(9) << groupSize.getMaxValue()
              << setw(9) << setprecision(2) << age.calculateMean()
              << setw(9) << setprecision(4) << alpha.calculateMean()
              << setw(9) << setprecision(4) << alphaAge.calculateMean()
              << setw(9) << setprecision(4) << beta.calculateMean()
              << setw(9) << setprecision(4) << betaAge.calculateMean()
              << setw(9) << setprecision(4) << help.calculateMean()
              << setw(9) << setprecision(4) << dispersal.calculateMean()
              << setw(9) << setprecision(2) << survival.calculateMean()
              << setw(9) << setprecision(2) << relatedness
              << endl;
}

void Statistics::printHeadersToFile() {
    // column headings in output file main
    *parameters->getMainWriter() << "Replica" << "\t" << "Generation" << "\t" << "Population" << "\t"
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
    *parameters->getLastGenerationWriter() << "replica" << "\t" << "generation" << "\t" << "groupID"
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
    *parameters->getMainWriter() << fixed << showpoint
                                 << replica
                                 << "\t" << generation
                                 << "\t" << population
                                 << "\t" << deaths
                                 << "\t" << totalFloaters
                                 << "\t" << setprecision(4) << groupSize.calculateMean()
                                 << "\t" << setprecision(4) << age.calculateMean()
                                 << "\t" << setprecision(4) << ageHelpers.calculateMean()
                                 << "\t" << setprecision(4) << ageFloaters.calculateMean()
                                 << "\t" << setprecision(4) << ageBreeders.calculateMean()
                                 << "\t" << setprecision(4) << alpha.calculateMean()
                                 << "\t" << setprecision(4) << alphaAge.calculateMean()
                                 << "\t" << setprecision(4) << beta.calculateMean()
                                 << "\t" << setprecision(4) << betaAge.calculateMean()
                                 << "\t" << setprecision(4) << help.calculateMean()
                                 << "\t" << setprecision(4) << cumulativeHelp.calculateMean()
                                 << "\t" << setprecision(4) << dispersal.calculateMean()
                                 << "\t" << setprecision(4) << survival.calculateMean()
                                 << "\t" << setprecision(4) << survivalHelpers.calculateMean()
                                 << "\t" << setprecision(4) << survivalFloaters.calculateMean()
                                 << "\t" << setprecision(4) << survivalBreeders.calculateMean()
                                 << "\t" << setprecision(4) << relatedness
                                 << "\t" << setprecision(4) << groupSize.calculateSD()
                                 << "\t" << setprecision(4) << age.calculateSD()
                                 << "\t" << setprecision(4) << ageHelpers.calculateSD()
                                 << "\t" << setprecision(4) << ageFloaters.calculateSD()
                                 << "\t" << setprecision(4) << ageBreeders.calculateSD()
                                 << "\t" << setprecision(4) << alpha.calculateSD()
                                 << "\t" << setprecision(4) << alphaAge.calculateSD()
                                 << "\t" << setprecision(4) << beta.calculateSD()
                                 << "\t" << setprecision(4) << betaAge.calculateSD()
                                 << "\t" << setprecision(4) << help.calculateSD()
                                 << "\t" << setprecision(4) << cumulativeHelp.calculateSD()
                                 << "\t" << setprecision(4) << dispersal.calculateSD()
                                 << "\t" << setprecision(4) << survival.calculateSD()
                                 << "\t" << setprecision(4) << survivalHelpers.calculateSD()
                                 << "\t" << setprecision(4) << survivalFloaters.calculateSD()
                                 << "\t" << setprecision(4) << survivalBreeders.calculateSD()
                                 << "\t" << setprecision(4) << help.correlation(dispersalHelpers)
                                 << "\t" << setprecision(4) << corrHelpGroupSize
                                 << "\t" << newBreederFloater
                                 << "\t" << newBreederHelper
                                 << "\t" << inheritance
                                 << endl;
}


void Statistics::printToFileLastGeneration(Simulation *simulation) {

    int groupID = 0;
    int counter = 0;

    for (auto const &group: simulation->getGroups()) {
        if (counter < 100) {
            this->printIndividual(group.getBreeder(), simulation->getGeneration(), groupID, simulation->getReplica());

            for (auto const &helper: group.helpers) {
                this->printIndividual(helper, simulation->getGeneration(), groupID, simulation->getReplica());
            }
            counter++;
        }
        groupID++;
    }
    for (auto const &floater: simulation->getFloaters()) {
        this->printIndividual(floater, simulation->getGeneration(), groupID, simulation->getReplica());
    }


}

Statistics::Statistics() {
    this->parameters = Parameters::instance();

}

void Statistics::printIndividual(Individual individual, int generation, int groupID, int replica) {
    *parameters->getLastGenerationWriter() << fixed << showpoint
                                           << replica + 1
                                           << "\t" << generation
                                           << "\t" << groupID
                                           << "\t" << individual.getFishType()
                                           << "\t" << setprecision(4) << individual.getAge()
                                           << "\t" << setprecision(4) << individual.getAlpha()
                                           << "\t" << setprecision(4) << individual.getAlphaAge()
                                           << "\t" << setprecision(4) << individual.getBeta()
                                           << "\t" << setprecision(4) << individual.getBetaAge()
                                           << "\t" << setprecision(4) << individual.getDrift()
                                           << "\t" << setprecision(4) << individual.getHelp()
                                           << "\t" << setprecision(4) << individual.getDispersal()
                                           << "\t" << setprecision(4) << individual.getSurvival()
                                           << endl;
}







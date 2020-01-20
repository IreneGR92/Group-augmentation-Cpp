/***********************************************
 GROUP AUGMENTATION MODEL
 - Passive group augmentation: individuals help in order to increase group size which in turn increases survival
 - Active group augmentation or delayed reciprocity: if inherit the breeding position, individuals benefit of the help given by the recruits in the group.


 Stochastic individual based model.
 Limited breeding spots, unlimited population size; one breeder per group.
 Population overlap, turn over when breeder dies. 
 Older/more dominant individual higher probability of becoming new breeder.
 Evolution of level of help and dispersal. Inclusion of reaction norm to age.
 Dispersal produces dispersers/floaters that can reproduce in another group or join as helpers.
 Survival dependent on group size, level of help and predation/environment.
 Fecundity dependent on cumulative level of help within group.
 Relatedness as an emergent property.

***********************************************/

/*HEADER FILES*/

#include <iomanip>
#include <iostream>
#include "Parameters.h"
#include "model/Individual.h"
#include "model/Group.h"
//#include <chrono> //if we use the system clock as seed

using namespace std;

Parameters parameters;



/*CONSTANTS AND STRUCTURES*/

// Random numbers
//mt19937 mt(time(0)); // random number generator
//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // if we don't want to obtain the same simulation under the same parameters
unsigned seed = 0; //in the same run takes different random values, but different runs same values unless we change the seed
default_random_engine generator(seed);


//Population parameters and Statistics
int replica, population, deaths, totalFloaters, newbreederFloater, newbreederHelper, inheritance;
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


/* INITIALISE POPULATION */
vector<Group> initializeGroups(int &generation) {

    vector<Group> groups(parameters.getMaxColonies(),Group(parameters, generator, generation));

    return groups;
}


void survivalFloaters(vector<Individual> &floaters, int &deaths) //Calculate the survival of the floaters
{
    vector<Individual, std::allocator<Individual>>::iterator survFIt;
    survFIt = floaters.begin();
    int sizevec = floaters.size();
    int counting = 0;
    while (!floaters.empty() && sizevec > counting) {

        //Mortality floaters
        if (parameters.uniform(generator) > survFIt->survival) {
            *survFIt = floaters[floaters.size() - 1];
            floaters.pop_back();
            ++counting;
            deaths++;
        } else
            ++survFIt, ++counting; //go to next individual
    }
}


/*REASSIGN FLOATERS*/

void reassignFloaters(vector<Individual> &floaters, vector<Group> &groups) {
    if (!parameters.isNoRelatedness()) {
        uniform_int_distribution<int> UniformMaxCol(0, parameters.getMaxColonies() - 1);
        int selectGroup;
        vector<Individual>::iterator indIt;
        while (!floaters.empty()) {
            indIt = floaters.end() - 1;
            indIt->help = 0;
            selectGroup = UniformMaxCol(generator);
            //indIt->fishType = HELPER; //modify the class
            groups[selectGroup].helpers.push_back(
                    *indIt); //add the floater to the helper vector in a randomly selected group
            floaters.pop_back(); //remove the floater from its vector
        }
    } else {

        double sumcumHelp = 0;
        double currentposition = 0;
        double RandP = parameters.uniform(generator);
        int allNoHelp = 0;

        vector<double> position; //vector of cumHelp to choose with higher likelihood the ind with higher age

        vector<Group, std::allocator<Group>>::iterator groupIt;
        for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
            sumcumHelp += 1 + groupIt->cumHelp; //add all the cumHelp from the vector Groups
            if (groupIt->cumHelp !=
                parameters.getMaxColonies()) { allNoHelp++; } //to check if all groups display cumhelp 0

            //if (sumcumHelp != 0) { cout << "sumcumHelp =" << sumcumHelp << '\t' << "allNoHelp =" << allNoHelp << endl; } //track
        }

        if (allNoHelp != 0) {

            for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
                position.push_back(static_cast<double>(1 + (groupIt)->cumHelp) / static_cast<double>(sumcumHelp) +
                                   currentposition); //creates a vector with proportional segments to the cumHelp of each group
                currentposition = position[position.size() - 1];
            }

            /*vector<Individual, std::allocator<Individual>>::iterator floatIt;
            for (floatIt = floaters.begin(); floatIt < floaters.end(); ++floatIt) {
                floatIt->fishType = HELPER; //all floaters pass to be helpers
            }*/

            while (!floaters.empty()) {
                for (int i = 0; i < groups.size(); i++) {
                    if (!floaters.empty()) {
                        if (RandP < position[i]) //to access the same ind in the candidates vector
                        {
                            groups[i].helpers.push_back(floaters[floaters.size() -
                                                                 1]); //add the floater to the helper vector in a randomly selected group
                            floaters.pop_back();
                        }
                    }
                }
            }

        } else {
            uniform_int_distribution<int> UniformMaxCol(0, parameters.getMaxColonies() - 1);
            int selectGroup;
            vector<Individual>::iterator floatIt;
            while (!floaters.empty()) {
                floatIt = floaters.end() - 1;
                selectGroup = UniformMaxCol(generator);
                //floatIt->fishType = HELPER; //modify the class
                groups[selectGroup].helpers.push_back(
                        *floatIt); //add the floater to the helper vector in a randomly selected group
                floaters.pop_back(); //remove the floater from its vector
            }
        }
    }
}


/* CALCULATE STATISTICS */
void calculateStatistics(vector<Group> groups, vector<Individual> floaters) {

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


    vector<Group, std::allocator<Group>>::iterator groupStatsIt;
    for (groupStatsIt = groups.begin(); groupStatsIt < groups.end(); ++groupStatsIt) {

        // HELPERS
        vector<Individual, std::allocator<Individual>>::iterator helperStatsIt; //helpers
        for (helperStatsIt = groupStatsIt->helpers.begin();
             helperStatsIt < groupStatsIt->helpers.end(); ++helperStatsIt) {

            // Genes
            sumAlpha += helperStatsIt->alpha;
            sumsqAlpha += helperStatsIt->alpha * helperStatsIt->alpha;

            sumAlphaAge += helperStatsIt->alphaAge;
            sumsqAlphaAge += helperStatsIt->alphaAge * helperStatsIt->alphaAge;

            sumBeta += helperStatsIt->beta;
            sumsqBeta += helperStatsIt->beta * helperStatsIt->beta;

            sumBetaAge += helperStatsIt->betaAge;
            sumsqBetaAge += helperStatsIt->betaAge * helperStatsIt->betaAge;

            //Relatedness
            if (groupStatsIt->breederAlive) {
                sumDriftB += groupStatsIt->breeder.drift;
                sumDriftH += helperStatsIt->drift;
                sumDriftBH += helperStatsIt->drift * groupStatsIt->breeder.drift;
                sumDriftBB += groupStatsIt->breeder.drift * groupStatsIt->breeder.drift;
                ++driftGroupSize;
            }

            //Phenotypes
            sumAgeHelper += helperStatsIt->age;
            sumsqAgeHelper += helperStatsIt->age * helperStatsIt->age;

            if (!isnan(helperStatsIt->help)) {
                sumHelp += helperStatsIt->help;
                sumsqHelp += helperStatsIt->help * helperStatsIt->help;
            }

            if (!isnan(helperStatsIt->dispersal)) {
                sumDispersal += helperStatsIt->dispersal;
                sumsqDispersal += helperStatsIt->dispersal * helperStatsIt->dispersal;

                sumprodHelpDispersal += helperStatsIt->help * helperStatsIt->dispersal;
            }

            if (!isnan(helperStatsIt->survival)) {
                sumSurvivalHelper += helperStatsIt->survival;
                sumsqSurvivalHelper += helperStatsIt->survival * helperStatsIt->survival;
            }
        }

        countHelpers += groupStatsIt->helpers.size();

        //BREEDERS
        if (groupStatsIt->breederAlive) {

            //Genes
            sumAlpha += groupStatsIt->breeder.alpha;
            sumsqAlpha += groupStatsIt->breeder.alpha * groupStatsIt->breeder.alpha;

            sumAlphaAge += groupStatsIt->breeder.alphaAge;
            sumsqAlphaAge += groupStatsIt->breeder.alphaAge * groupStatsIt->breeder.alphaAge;

            sumBeta += groupStatsIt->breeder.beta;
            sumsqBeta += groupStatsIt->breeder.beta * groupStatsIt->breeder.beta;

            sumBetaAge += groupStatsIt->breeder.betaAge;
            sumsqBetaAge += groupStatsIt->breeder.betaAge * groupStatsIt->breeder.betaAge;

            //Phenotypes
            sumAgeBreeder += groupStatsIt->breeder.age;
            sumsqAgeBreeder += groupStatsIt->breeder.age * groupStatsIt->breeder.age;

            sumSurvivalBreeder += groupStatsIt->breeder.survival;
            sumsqSurvivalBreeder += groupStatsIt->breeder.survival * groupStatsIt->breeder.survival;

            countBreeders++;
        }

        //Group
        groupStatsIt->GroupSize();
        sumGroupSize += groupStatsIt->groupSize;
        sumsqGroupSize += groupStatsIt->groupSize * groupStatsIt->groupSize;
        if (groupStatsIt->groupSize > maxGroupSize) maxGroupSize = groupStatsIt->groupSize;


        if (groupStatsIt->helpersPresent) {
            sumCumHelp += groupStatsIt->cumHelp;
            sumsqCumHelp += groupStatsIt->cumHelp * groupStatsIt->cumHelp;
            sumGroupSizeHelp += groupStatsIt->groupSize;
            sumsqGroupSizeHelp += groupStatsIt->groupSize * groupStatsIt->groupSize;
            sumprodHelpGroup += groupStatsIt->cumHelp * groupStatsIt->groupSize;

            countGroupWithHelpers++;
        }
    }

    //FLOATERS

    totalFloaters = floaters.size();

    vector<Individual, std::allocator<Individual>>::iterator floatStatsIt;
    for (floatStatsIt = floaters.begin(); floatStatsIt < floaters.end(); ++floatStatsIt) {
        // Genes
        sumAlpha += floatStatsIt->alpha;
        sumsqAlpha += floatStatsIt->alpha * floatStatsIt->alpha;

        sumAlphaAge += floatStatsIt->alphaAge;
        sumsqAlphaAge += floatStatsIt->alphaAge * floatStatsIt->alphaAge;

        sumBeta += floatStatsIt->beta;
        sumsqBeta += floatStatsIt->beta * floatStatsIt->beta;

        sumBetaAge += floatStatsIt->betaAge;
        sumsqBetaAge += floatStatsIt->betaAge * floatStatsIt->betaAge;

        //Phenotypes
        sumAgeFloater += floatStatsIt->age;
        sumsqAgeFloater += floatStatsIt->age * floatStatsIt->age;

        if (!isnan(floatStatsIt->dispersal)) {
            sumDispersal += floatStatsIt->dispersal;
            sumsqDispersal += floatStatsIt->dispersal * floatStatsIt->dispersal;
        }

        if (!isnan(floatStatsIt->survival)) {
            sumSurvivalFloater += floatStatsIt->survival;
            sumsqSurvivalFloater += floatStatsIt->survival * floatStatsIt->survival;
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

    (countGroupWithHelpers > 0 && stdevCumHelp > 0 && stdevGroupSize > 0) ? corr_HelpGroup = (sumprodHelpGroup /
                                                                                              countGroupWithHelpers -
                                                                                              meanCumHelp *
                                                                                              meanGroupSizeHelp) /
                                                                                             (stdevCumHelp *
                                                                                              stdevGroupSizeHelp)
                                                                          : corr_HelpGroup = 0;

}


/* MAIN PROGRAM */
int main(int count, char **argv) {

    cout << "reading file " << argv[1] << "\n";

    parameters = Parameters(argv[1]);


    // Output file
    ofstream fout("main_" + parameters.getName() + ".txt");
    ofstream fout2("last_generation_" + parameters.getName() + ".txt");


    fout << "PARAMETER VALUES" << endl

         << "Reaction_norm_help?: " << "\t" << parameters.isReactionNormHelp() << endl
         << "Reaction_norm_dispersal?: " << "\t" << parameters.isReactionNormDispersal() << endl
         << "Evolution_help_after_dispersal?: " << "\t" << parameters.isEvolutionHelpAfterDispersal() << endl
         << "Low_survival_breeder?: " << "\t" << parameters.isLowSurvivalBreeder() << endl
         << "Low_survival_floater?: " << "\t" << parameters.isLowSurvivalFloater() << endl
         << "No_group_augmentation?: " << "\t" << parameters.isNoGroupAugmentation() << endl
         << "No_effect_relatedness?: " << "\t" << parameters.isNoRelatedness() << endl
         << "Logistic_survival?: " << "\t" << parameters.isLogisticSurvival() << endl
         << "Initial_population: " << "\t" << parameters.getMaxColonies() * (parameters.getInitNumHelpers() + 1) << endl
         << "Number_of_colonies: " << "\t" << parameters.getMaxColonies() << endl
         << "Number_generations: " << "\t" << parameters.getNumGenerations() << endl
         << "Number_replicates: " << "\t" << parameters.getMaxNumReplicates() << endl
         << "Bias_float_breeder: " << "\t" << parameters.getBiasFloatBreeder() << endl
         << "m(predation): " << "\t" << parameters.getM() << endl
         << "n(effect_size_mortality_dispersal): " << "\t" << parameters.getN() << endl
         << "X0(intercept): " << "\t" << parameters.getX0() << endl
         << "Xh(Cost_help_survival): " << "\t" << parameters.getXsh() << endl
         << "Xn(Benefit_group_size_survival): " << "\t" << parameters.getXsn() << endl
         << "K0(Base_fecundity): " << "\t" << parameters.getK0() << endl
         << "K1(Benefit_help_fecundity): " << "\t" << parameters.getK1() << endl
         << "initAlpha: " << "\t" << parameters.getInitAlpha() << endl
         << "initAlphaAge: " << "\t" << parameters.getInitAlphaAge() << endl
         << "initBeta: " << "\t" << parameters.getInitBeta() << endl
         << "initBetaAge: " << "\t" << parameters.getInitBetaAge() << endl
         << "mutAlpha: " << "\t" << parameters.getMutationAlpha() << endl
         << "mutAlphaAge: " << "\t" << parameters.getMutationAlphaAge() << endl
         << "mutBeta: " << "\t" << parameters.getMutationBeta() << endl
         << "mutBetaAge: " << "\t" << parameters.getMutationBetaAge() << endl
         << "mutDrift: " << "\t" << parameters.getMutationDrift() << endl
         << "stepAlpha: " << "\t" << parameters.getStepAlpha() << endl
         << "stepBeta: " << "\t" << parameters.getStepBeta() << endl
         << "stepDrift: " << "\t" << parameters.getStepDrift() << endl << endl;


    fout2 << "PARAMETER VALUES" << endl

          << "Reaction_norm_help?: " << "\t" << parameters.isReactionNormHelp() << endl
          << "Reaction_norm_dispersal?: " << "\t" << parameters.isReactionNormDispersal() << endl
          << "Evolution_help_after_dispersal?: " << "\t" << parameters.isEvolutionHelpAfterDispersal() << endl
          << "Low_survival_breeder?: " << "\t" << parameters.isLowSurvivalBreeder() << endl
          << "Low_survival_floater?: " << "\t" << parameters.isLowSurvivalFloater() << endl
          << "No_group_augmentation?: " << "\t" << parameters.isNoGroupAugmentation() << endl
          << "No_effect_relatedness?: " << "\t" << parameters.isNoRelatedness() << endl
          << "Logistic_survival?: " << "\t" << parameters.isLogisticSurvival() << endl
          << "Initial_population: " << "\t" << parameters.getMaxColonies() * (parameters.getInitNumHelpers() + 1)
          << endl
          << "Number_of_colonies: " << "\t" << parameters.getMaxColonies() << endl
          << "Number_generations: " << "\t" << parameters.getNumGenerations() << endl
          << "Number_replicates: " << "\t" << parameters.getMaxNumReplicates() << endl
          << "Bias_float_breeder: " << "\t" << parameters.getBiasFloatBreeder() << endl
          << "m(predation): " << "\t" << parameters.getM() << endl
          << "n(effect_size_mortality_dispersal): " << "\t" << parameters.getN() << endl
          << "X0(intercept): " << "\t" << parameters.getX0() << endl
          << "Xh(Cost_help_survival): " << "\t" << parameters.getXsh() << endl
          << "Xn(Benefit_group_size_survival): " << "\t" << parameters.getXsn() << endl
          << "K0(Base_fecundity): " << "\t" << parameters.getK0() << endl
          << "K1(Benefit_help_fecundity): " << "\t" << parameters.getK1() << endl
          << "initAlpha: " << "\t" << parameters.getInitAlpha() << endl
          << "initAlphaAge: " << "\t" << parameters.getInitAlphaAge() << endl
          << "initBeta: " << "\t" << parameters.getInitBeta() << endl
          << "initBetaAge: " << "\t" << parameters.getInitBetaAge() << endl
          << "mutAlpha: " << "\t" << parameters.getMutationAlpha() << endl
          << "mutAlphaAge: " << "\t" << parameters.getMutationAlphaAge() << endl
          << "mutBeta: " << "\t" << parameters.getMutationBeta() << endl
          << "mutBetaAge: " << "\t" << parameters.getMutationBetaAge() << endl
          << "mutDrift: " << "\t" << parameters.getMutationDrift() << endl
          << "stepAlpha: " << "\t" << parameters.getStepAlpha() << endl
          << "stepBeta: " << "\t" << parameters.getStepBeta() << endl
          << "stepDrift: " << "\t" << parameters.getStepDrift() << endl << endl;

    // column headings in output file main
    fout << "Replica" << "\t" << "Generation" << "\t" << "Population" << "\t" << "Deaths" << "\t" << "Floaters" << "\t"
         << "Group_size" << "\t" << "Age" << "\t" << "Age_H" << "\t" << "Age_F" << "\t" << "Age_B" << "\t"
         << "meanAlpha" << "\t" << "meanAlphaAge" << "\t" << "meanBeta" << "\t" << "meanBetaAge" << "\t"
         << "meanHelp" << "\t" << "meanCumHelp" << "\t" << "meanDispersal" << "\t"
         << "meanSurvival" << "\t" << "meanSurvival_H" << "\t" << "meanSurvival_F" << "\t" << "meanSurvival_B" << "\t"
         << "Relatedness" << "\t"
         << "SD_GroupSize" << "\t" << "SD_Age" << "\t" << "SD_Age_H" << "\t" << "SD_Age_F" << "\t" << "SD_Age_B" << "\t"
         << "SD_Alpha" << "\t" << "SD_AlphaAge" << "\t" << "SD_Beta" << "\t" << "SD_BetaAge" << "\t"
         << "SD_Help" << "\t" << "SD_CumHelp" << "\t" << "SD_Dispersal" << "\t"
         << "SD_Survival" << "\t" << "SD_Survival_H" << "\t" << "SD_Survival_F" << "\t" << "SD_Survival_B" << "\t"
         << "corr_Help_Disp" << "\t" << "corr_Help_Group" << "\t"
         << "newBreederFloater" << "\t" << "newBreederHelper" << "\t" << "inheritance" << endl;

    // column headings in output file last generation
    fout2 << "replica" << "\t" << "generation" << "\t" << "groupID" << "\t" << "type" << "\t" << "age" << "\t"
          << "alpha" << "\t" << "alphaAge" << "\t" << "beta" << "\t" << "betaAge" << "\t" << "drift"
          << "\t" << "help" << "\t" << "dispersal" << "\t" << "survival" << endl;
//
//
    for (replica = 0; replica < parameters.getMaxNumReplicates(); replica++) {

        int generation = 0;
        deaths = 0; // to keep track of how many individuals die each generation
        newbreederFloater = 0, newbreederHelper = 0, inheritance = 0; //to know if the new breeder was a helper or a floater

        // column headings on screen
        cout << setw(6) << "gen" << setw(9) << "pop" << setw(9) << "deaths" << setw(9)
             << "float" << setw(9) << "group" << setw(9) << "maxGroup" << setw(9) << "age" << setw(9)
             << "alpha" << setw(9) << "alphaAge" << setw(9) << "beta" << setw(9) << "betaAge" << setw(9)
             << "help" << setw(9) << "disper" << setw(9) << "surv" << setw(9) << "relat" << endl;


        vector<Individual> floaters;
        vector<Group> groups = initializeGroups(generation);


        calculateStatistics(groups, floaters);

        // show values on screen
        cout << fixed << showpoint
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


        // write values to output file
        fout << fixed << showpoint
             << replica + 1
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
             << "\t" << newbreederFloater
             << "\t" << newbreederHelper
             << "\t" << inheritance
             << endl;

        for (generation = 1; generation <= parameters.getNumGenerations(); generation++) {

            deaths = 0; // to keep track of how many individuals die each generation
            newbreederFloater = 0, newbreederHelper = 0, inheritance = 0;

            //Dispersal
            vector<Group, std::allocator<Group>>::iterator itDispersal;
            for (itDispersal = groups.begin(); itDispersal < groups.end(); ++itDispersal) {
                itDispersal->disperse(floaters);
            }

            //Help & Survival
            vector<Group, std::allocator<Group>>::iterator itHelpSurvival;
            for (itHelpSurvival = groups.begin(); itHelpSurvival < groups.end(); ++itHelpSurvival) {
                //Calculate help
                itHelpSurvival->CumHelp();

                //Calculate survival for the helpers
                itHelpSurvival->GroupSize();
                vector<Individual, std::allocator<Individual>>::iterator helperStatsIt; //helpers
                for (helperStatsIt = itHelpSurvival->helpers.begin();
                     helperStatsIt < itHelpSurvival->helpers.end(); ++helperStatsIt) {
                    helperStatsIt->calcSurvival(itHelpSurvival->groupSize);
                }

                //Calculate the survival of the breeder
                if (parameters.isLowSurvivalBreeder()) {
                    itHelpSurvival->breeder.calcSurvival(0); //survival for breeder does not include group size benefits
                } // TODO:Change to 1?
                else {
                    itHelpSurvival->breeder.calcSurvival(itHelpSurvival->groupSize);
                }
            }

            //Calculate survival for floaters
            vector<Individual, std::allocator<Individual>>::iterator floatIt;
            for (floatIt = floaters.begin(); floatIt < floaters.end(); ++floatIt) {
                floatIt->calcSurvival(0); // TODO:Change to 1?
            }

            //Calculate stats
            if (generation % parameters.getSkip() == 0) {
                calculateStatistics(groups, floaters); // Statistics calculated before survival
            }

            //Mortality of helpers and breeders
            vector<Group, std::allocator<Group>>::iterator itSurvival;
            for (itSurvival = groups.begin(); itSurvival < groups.end(); ++itSurvival) {
                itSurvival->survival(deaths);
            }

            //Mortality of floaters
            survivalFloaters(floaters, deaths);

            //Become a breeder
            vector<Group, std::allocator<Group>>::iterator itBreeder;
            for (itBreeder = groups.begin(); itBreeder < groups.end(); ++itBreeder) {
                if (!itBreeder->breederAlive) {
                    itBreeder->newBreeder(floaters, newbreederFloater, newbreederHelper, inheritance);
                }
            }

            //reassignFloaters floaters to groups for later philopatry vs dispersal
            reassignFloaters(floaters, groups);
            /*if (!floaters.empty()) {
                cout << "Not all floaters were reassigned!" << endl;
            }*/

            //Increase age
            vector<Group, std::allocator<Group>>::iterator itAge;
            for (itAge = groups.begin(); itAge < groups.end(); ++itAge) {
                itAge->increaseAge(); //add 1 rank or age to all individuals alive
            }

            //Print stats
            if (generation % parameters.getSkip() == 0) {

                // show values on screen
                cout << fixed << showpoint
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


                // write values to output file
                fout << fixed << showpoint
                     << replica + 1
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
                     << "\t" << newbreederFloater
                     << "\t" << newbreederHelper
                     << "\t" << inheritance
                     << endl;
            }

            //Print last generation
            if (generation == parameters.getNumGenerations() / 10 || generation == parameters.getNumGenerations() / 4 ||
                generation == parameters.getNumGenerations() / 2 || generation == parameters.getNumGenerations()) {

                int groupID = 0;
                int counter = 0;

                vector<Group, std::allocator<Group>>::iterator itGroups;
                for (itGroups = groups.begin(); itGroups < groups.end(); ++itGroups) {
                    if (counter < 100) {
                        fout2 << fixed << showpoint
                              << replica + 1
                              << "\t" << generation
                              << "\t" << groupID
                              << "\t" << itGroups->breeder.fishType
                              << "\t" << setprecision(4) << itGroups->breeder.age
                              << "\t" << setprecision(4) << itGroups->breeder.alpha
                              << "\t" << setprecision(4) << itGroups->breeder.alphaAge
                              << "\t" << setprecision(4) << itGroups->breeder.beta
                              << "\t" << setprecision(4) << itGroups->breeder.betaAge
                              << "\t" << setprecision(4) << itGroups->breeder.drift
                              << "\t" << setprecision(4) << "NA"
                              << "\t" << setprecision(4) << "NA"
                              << "\t" << setprecision(4) << itGroups->breeder.survival
                              << endl;

                        for (vector<Individual>::iterator itHelpers = itGroups->helpers.begin();
                             itHelpers < itGroups->helpers.end(); ++itHelpers) {
                            fout2 << fixed << showpoint
                                  << replica + 1
                                  << "\t" << generation
                                  << "\t" << groupID
                                  << "\t" << itHelpers->fishType
                                  << "\t" << setprecision(4) << itHelpers->age
                                  << "\t" << setprecision(4) << itHelpers->alpha
                                  << "\t" << setprecision(4) << itHelpers->alphaAge
                                  << "\t" << setprecision(4) << itHelpers->beta
                                  << "\t" << setprecision(4) << itHelpers->betaAge
                                  << "\t" << setprecision(4) << itHelpers->drift
                                  << "\t" << setprecision(4) << itHelpers->help // TODO: for floaters is 0 no NA!
                                  << "\t" << setprecision(4) << itHelpers->dispersal
                                  << "\t" << setprecision(4) << itHelpers->survival
                                  << endl;
                        }
                        counter++;
                    }
                    groupID++;
                }
            }

            //Reproduction
            vector<Group, std::allocator<Group>>::iterator itReproduction;
            for (itReproduction = groups.begin(); itReproduction < groups.end(); ++itReproduction) {
                itReproduction->reproduce();
            }
        }
    }

    return 0;
}

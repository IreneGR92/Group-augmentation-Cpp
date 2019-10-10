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

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <random>
#include "Parameters.h"
//#include <chrono> //if we use the system clock as seed

using namespace std;

Parameters parameters;



/*CONSTANTS AND STRUCTURES*/

// Random numbers
//mt19937 mt(time(0)); // random number generator
//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // if we don't want to obtain the same simulation under the same parameters
unsigned seed = 0; //in the same run takes different random values, but different runs same values unless we change the seed
default_random_engine generator(seed);
uniform_real_distribution<double> DriftUniform(0, 100);
uniform_real_distribution<double> Uniform(0, 1);


enum classes {
    BREEDER, HELPER, FLOATER
};


const int NO_VALUE = -1;

//Population parameters and Statistics
int replica, generation, population, deaths, totalFloaters, newbreederFloater, newbreederHelper, inheritance;
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


//Structures

struct Individual // define individual traits
{
    explicit Individual(double alpha_ = 0, double alphaAge_ = 0, double beta_ = 0,
                        double betaAge_ = 0, double drift_ = 0, classes fishType_ = HELPER);

    Individual(const Individual &mother);

    double alpha, alphaAge, beta, betaAge, drift,        // genetic values
            dispersal, help, survival;                                    // phenotypic values
    classes fishType;                                                // possible classes: breeder, helper, floater
    int age;
    bool inherit;                                                    //did the new breeder inherit the territory or did it disperse?

    //Functions inside Individual
    double calcDispersal();

    void calcHelp();

    double calcSurvival(int groupSize);

    void Mutate();
};


Individual::Individual(double alpha_, double alphaAge_, double beta_, double betaAge_, double drift_,
                       classes fishType_) {
    alpha = alpha_;
    alphaAge = alphaAge_;
    beta = beta_;
    betaAge = betaAge_;
    drift = drift_;
    if(generation != 0) {
        Mutate();
    }
    fishType = fishType_;
    age = 1;
    inherit = true;
    survival = NO_VALUE;
    help = 0;
    dispersal = NO_VALUE;
}

Individual::Individual(const Individual &copy) {
    alpha = copy.alpha;
    alphaAge = copy.alphaAge;
    beta = copy.beta;
    betaAge = copy.betaAge;
    drift = copy.drift;
    fishType = copy.fishType;
    age = copy.age;
    inherit = copy.inherit;
    survival = copy.survival;
    help = copy.help;
    dispersal = copy.dispersal;
}


struct Group // define group traits
{
    Group(double alpha_, double alphaAge_, double beta_, double betaAge_, int numhelp_);

    double cumHelp;
    bool breederAlive;                                     // for the breeder: 1 alive, 0 dead
    bool helpersPresent;                                   //helpers present in group before reassign?
    int groupSize;
    double fecundity;
    int realFecundity;

    Individual breeder; // create breeder inside group
    vector<Individual> helpers; // create a vector of helpers inside each group

//Functions inside Group
    void Disperse(vector<Individual> &floaters);

    void CumHelp();

    void SurvivalGroup(int &deaths);

    void NewBreeder(vector<Individual> &floaters, int &newBreederFloater, int &newBreederHelper, int &inheritance);

    void IncreaseAge();

    void GroupSize();

    void Fecundity();

    void Reproduction();
};


Group::Group(double alpha_ = parameters.getInitAlpha(), double alphaAge_ = parameters.getInitAlphaAge(),
             double beta_ = parameters.getInitBeta(), double betaAge_ = parameters.getInitBetaAge(),
             int numhelp_ = parameters.getInitNumHelpers()) {
    breeder = Individual(alpha_, alphaAge_, beta_, betaAge_, DriftUniform(generator), BREEDER);
    breederAlive = true;
    helpersPresent = false;
    cumHelp = NO_VALUE;
    fecundity = NO_VALUE;
    realFecundity = NO_VALUE;

    for (int i = 0; i < numhelp_; ++i) {
        helpers.emplace_back(Individual(alpha_, alphaAge_, beta_, betaAge_, DriftUniform(generator), HELPER));
    }

    GroupSize();
}

/* TOTAL NUMBER OF INDIVIDUALS PER GROUP*/

void Group::GroupSize() {
    if (breederAlive) {
        groupSize = helpers.size() + 1;
    } else {
        groupSize = helpers.size();
    }
}

/* INITIALISE POPULATION */
void InitGroup(vector<Group> &groups) {
    int i;
    for (i = 0; i < parameters.getMaxColonies(); i++) {
        groups[i] = Group(parameters.getInitAlpha(), parameters.getInitAlphaAge(),
                          parameters.getInitBeta(), parameters.getInitBetaAge(), parameters.getInitNumHelpers());
    }
}


/* BECOME FLOATER (STAY VS DISPERSE) */

double Individual::calcDispersal() {
    if (parameters.isNoRelatedness() && age == 1) {
        dispersal = 1;

    } else {
        if (!parameters.isReactionNormDispersal()) {

            dispersal = beta; // Range from 0 to 1 to compare to a Uniform distribution
        } else {
            dispersal = 1 / (1 + exp(betaAge * age - beta));
        }

        /*if (dispersal < 0 || dispersal > 1) {
            cout << "error in dispersal: " << dispersal << endl;
        }*/
    }

    return dispersal;
}


void Group::Disperse(vector<Individual> &floaters) {
    vector<Individual, std::allocator<Individual>>::iterator dispersalIt;
    dispersalIt = helpers.begin();
    int sizevec = helpers.size();
    int counting = 0;

    while (!helpers.empty() && sizevec > counting) {
        dispersalIt->calcDispersal();

        if (Uniform(generator) < dispersalIt->dispersal) {
            dispersalIt->inherit = false; //the location of the individual is not the natal territory
            floaters.push_back(*dispersalIt); //add the individual to the vector floaters in the last position
            floaters[floaters.size() - 1].fishType = FLOATER;
            *dispersalIt = helpers[helpers.size() -
                                   1]; // this and next line removes the individual from the helpers vector
            helpers.pop_back();
            ++counting;
        } else {
            dispersalIt->fishType = HELPER; //individuals that stay or disperse to this group become floaters
            ++dispersalIt, ++counting;
        }
    }
}


/*DISPLAY LEVEL OF HELP*/
void Individual::calcHelp() {
    if (fishType == HELPER) {
        if (!parameters.isReactionNormHelp()) {
            help = alpha;
        } else {
            help = alpha + alphaAge * age;
            if (help < 0) { help = 0; }
        }
    } else {
        help = NAN;
        cout << "Error: floaters get a help value" << endl;
    }
}


void Group::CumHelp() //Calculate accumulative help of all individuals inside of each group.
{
    cumHelp = 0;

    helpersPresent = false;
    if (!helpers.empty()) {
        helpersPresent = true;
    }

    //Level of help for helpers
    vector<Individual, std::allocator<Individual>>::iterator helpIt;
    for (helpIt = helpers.begin(); helpIt < helpers.end(); ++helpIt) {
        helpIt->calcHelp();
        cumHelp += helpIt->help;
    }
    //Level of help for breeders
    //breeder.calcHelp();
    //cumHelp += breeder.help;
}

/*SURVIVAL*/

double Individual::calcSurvival(int groupSize) {

    if (parameters.isNoGroupAugmentation()){
        if (parameters.isLowSurvialFloater()) {
            survival = (1 - parameters.getX0()) /
                       (1 + exp(parameters.getXsh() * help - parameters.getXsn() * (parameters.getFixedGroupSize()))); // +1 to know group size (1 breeder + helpers)
        } else {
            survival = parameters.getX0() + parameters.getXsn() / (1 + exp(-(parameters.getFixedGroupSize()))) -
                       parameters.getXsh() / (1 + exp(-help)); //alternative implementation of survival, if Xsn=Xsh, equivalent size effect of help and group size in survival

            if (survival > 0.95) {
                survival = 0.95;
               // cout << "survival greater than 1" << endl;
            }
        }
    } else {
        if (parameters.isLowSurvialFloater() && fishType == FLOATER ) {
            survival = parameters.getX0();  // +1 to know group size (1 breeder + helpers)
        } else {
            survival = parameters.getX0() + parameters.getXsn() / (1 + exp(-(groupSize))) -
                       parameters.getXsh() / (1 + exp(-help)); //alternative implementation of survival, if Xsn=Xsh, equivalent size effect of help and group size in survival

            if (survival > 0.95) {
                survival = 0.95;
                //cout << "survival greater than 1" << endl;
            }
        }
    }
    return survival;
}


void Group::SurvivalGroup(int &deaths) {
    GroupSize(); //update group size after dispersal

    //Calculate the survival of the helpers

    vector<Individual, std::allocator<Individual>>::iterator survHIt;
    survHIt = helpers.begin();
    int sizevec = helpers.size();
    int counting = 0;
    while (!helpers.empty() && sizevec > counting) {

        //Mortality helpers
        if (Uniform(generator) > survHIt->survival) {
            *survHIt = helpers[helpers.size() - 1];
            helpers.pop_back();
            ++counting;
            deaths++;
        } else
            ++survHIt, ++counting; //go to next individual
    }

    //Mortality breeder
	if (Uniform(generator) > breeder.survival) {
        breederAlive = false;
        deaths++;
    }
}

void SurvivalFloaters(vector<Individual> &floaters, int &deaths) //Calculate the survival of the floaters
{
    vector<Individual, std::allocator<Individual>>::iterator survFIt;
    survFIt = floaters.begin();
    int sizevec = floaters.size();
    int counting = 0;
    while (!floaters.empty() && sizevec > counting) {

        //Mortality floaters
        if (Uniform(generator) > survFIt->survival) {
            *survFIt = floaters[floaters.size() - 1];
            floaters.pop_back();
            ++counting;
            deaths++;
        } else
            ++survFIt, ++counting; //go to next individual
    }
}

/* BECOME BREEDER */

void Group::NewBreeder(vector<Individual> &floaters, int &newBreederFloater, int &newBreederHelper, int &inheritance) {
    //    Select a random sample from the floaters
    int i = 0;
    int sumage = 0;
    double currentposition = 0; //age of the previous ind taken from Candidates
    int UniformFloatNum = 0;
    double RandP = Uniform(generator);
    int proportFloaters;
    proportFloaters = round(floaters.size() * parameters.getBiasFloatBreeder() / parameters.getMaxColonies());

    vector<Individual *> Candidates;
    vector<double> position; //vector of age to choose with higher likelihood the ind with higher age
    vector<int> TemporaryCandidates; // to prevent taking the same ind several times in the sample

    if (!floaters.empty() && floaters.size() > proportFloaters) {
        while (i < proportFloaters) {
            uniform_int_distribution<int> UniformFloat(0, floaters.size() - 1); //random floater ID taken in the sample
            UniformFloatNum = UniformFloat(generator);
            TemporaryCandidates.push_back(UniformFloatNum); //add references of the floaters sampled to a vector
            sort(TemporaryCandidates.begin(), TemporaryCandidates.end()); //sort vector
            i++;
        }

        int temp = 0;
        vector<int, std::allocator<int>>::iterator itTempCandidates;
        for (itTempCandidates = TemporaryCandidates.begin();
             itTempCandidates < TemporaryCandidates.end(); ++itTempCandidates) {
            if (*itTempCandidates != temp) //to make sure the same ind is not taken more than ones
            {
                Candidates.push_back(&floaters[UniformFloatNum]);
                temp = *itTempCandidates;
            }
        }
    } else if (!floaters.empty() && floaters.size() <
                                    proportFloaters) { //TODO:When less floaters available than the sample size, takes all of them. Change to a proportion?
        vector<Individual, std::allocator<Individual>>::iterator floatIt;
        for (floatIt = floaters.begin(); floatIt < floaters.end(); ++floatIt) {
            Candidates.push_back(&(*floatIt));
        }
    }


    //    Join the helpers in the group to the sample of floaters

    vector<Individual, std::allocator<Individual>>::iterator helpIt;
    for (helpIt = helpers.begin(); helpIt < helpers.end(); ++helpIt) {
        Candidates.push_back(&(*helpIt));
    }


//     Choose breeder with higher likelihood for the highest age
    vector<Individual *, std::allocator<Individual *>>::iterator ageIt;
    for (ageIt = Candidates.begin();
         ageIt < Candidates.end(); ++ageIt) //ageIt creates a vector of pointers to an individual
    {
        sumage += (*ageIt)->age; //add all the age from the vector Candidates
    }

    vector<Individual *, std::allocator<Individual *>>::iterator age2It;
    for (age2It = Candidates.begin(); age2It < Candidates.end(); ++age2It) {
        position.push_back(static_cast<double>((*age2It)->age) / static_cast<double>(sumage) +
                           currentposition); //creates a vector with proportional segments to the age of each individual
        currentposition = position[position.size() - 1];
    }

    if (floaters.empty() && Candidates.size() != helpers.size()) {
        //cout << "Error assigning empty floaters to Breeder" << endl;
    }

    vector<Individual *, std::allocator<Individual *>>::iterator age3It;
    age3It = Candidates.begin();
    int counting = 0;
    while (counting < Candidates.size()) {
        if (RandP < position[age3It - Candidates.begin()]) //to access the same ind in the candidates vector
        {
            breeder = **age3It; //substitute the previous dead breeder
            breederAlive = true;

            if ((*age3It)->fishType == FLOATER) //delete the ind from the vector floaters
            {
                **age3It = floaters[floaters.size() - 1];
                floaters.pop_back();
                newBreederFloater++;
//                if ((*age3It)->inherit == 1) {
//                    cout << "error in inheritance" << endl;
//                }
            } else {
                **age3It = helpers[helpers.size() - 1]; //delete the ind from the vector helpers
                helpers.pop_back();
                newBreederHelper++;
                if ((*age3It)->inherit == 1) {
                    inheritance++;                    //calculates how many individuals that become breeders are natal to the territory
                }
            }

            breeder.fishType = BREEDER; //modify the class
            counting = Candidates.size();//end loop
        } else
            ++age3It, ++counting;
    }
}


/*REASSIGN FLOATERS*/

void Reassign(vector<Individual> &floaters, vector<Group> &groups) {
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
        double RandP = Uniform(generator);
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


/* INCREASE AGE*/
void Group::IncreaseAge() {
    vector<Individual, std::allocator<Individual>>::iterator ageIt;
    for (ageIt = helpers.begin(); ageIt < helpers.end(); ++ageIt) {
        ageIt->age++;
    }
    if (breederAlive) {
        breeder.age++;
    } else {
        breeder.age = NO_VALUE; //to check for dead breeders still existing in print last generation
    }
}


/* REPRODUCTION */

void Group::Fecundity() {
    if (!parameters.isNoRelatedness()) {
        fecundity = parameters.getK0() + parameters.getK1() * cumHelp / (1 + cumHelp *
                                                                             parameters.getK1()); //fecundity function of cummulative help in the group. If cumHelp bigger than 2, no effect on fecundity
    } else {
        fecundity = parameters.getK0();
    }


    poisson_distribution<int> PoissonFec(fecundity);
    realFecundity = PoissonFec(generator); //integer number
}

void Group::Reproduction() // populate offspring generation
{
    if (breederAlive) {
        for (int i = 0; i < realFecundity; i++) //number of offspring dependent on real fecundity
        {
            helpers.emplace_back(breeder.alpha, breeder.alphaAge, breeder.beta, breeder.betaAge,
                                 breeder.drift); //create a new individual as helper in the group. Call construct to assign the mother genetic values to the offspring, construct calls Mutate function.
        }
    }
}


void Individual::Mutate() // mutate genome of offspring 
{
    normal_distribution<double> NormalA(0,
                                        parameters.getStepAlpha()); //TODO: could be simplified if I decide to have all the steps size with the same magnitude
    normal_distribution<double> NormalB(0, parameters.getStepBeta());
    normal_distribution<double> NormalD(0, parameters.getStepDrift());

    if (parameters.isEvolutionHelpAfterDispersal()) {
        if (generation < parameters.getNumGenerations() / 4) {
            parameters.setMutationAlpha(0);
            parameters.setMutationAlphaAge(0);
        } else {
            parameters.setMutationAlpha(0.05);
            parameters.setMutationAlphaAge(0.05);
        }
    }

    if (Uniform(generator) < parameters.getMutationAlpha()) {
        alpha += NormalA(generator);
        if (!parameters.isReactionNormHelp()) {
            if (alpha < 0) { alpha = 0; }
            else if (alpha > 4) { alpha = 4; } //else camouflaged to selection
        }
    }
    if (parameters.isReactionNormHelp()) {
        if (Uniform(generator) < parameters.getMutationAlphaAge()) {
            alphaAge += NormalA(generator);
        }
    }

    if (Uniform(generator) < parameters.getMutationBeta()) {
        beta += NormalB(generator);
        if (!parameters.isReactionNormDispersal()) {
            if (beta < 0) { beta = 0; }
            else if (beta > 1) { beta = 1; }
        }
    }
    if (parameters.isReactionNormDispersal()) {
        if (Uniform(generator) < parameters.getMutationBetaAge()) {
            betaAge += NormalB(generator);
        }
    }

    if (Uniform(generator) < parameters.getMutationDrift()) {
        drift += NormalD(generator);
    }
}


/* CALCULATE STATISTICS */
void Statistics(vector<Group> groups, vector<Individual> floaters) {

    population = 0, totalFloaters = 0, countGroupWithHelpers = 0, countHelpers = 0, countBreeders= 0,
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
        for (helperStatsIt = groupStatsIt->helpers.begin(); helperStatsIt < groupStatsIt->helpers.end(); ++helperStatsIt) {

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
        if (groupStatsIt->breederAlive){

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
    if (driftGroupSize != 0){
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
    if ((meanDriftBB - meanDriftB * meanDriftB) == 0 || driftGroupSize == 0) { relatedness = NO_VALUE; } //prevent to divide by 0


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
    (countHelpers > 0 && stdevHelp > 0 && stdevDispersal > 0) ? corr_HelpDispersal = (sumprodHelpDispersal / countHelpers -
                                                                  meanHelp * meanDispersal) /
                                                                 (stdevHelp * stdevDispersal) : corr_HelpDispersal = 0;

    (countGroupWithHelpers > 0 && stdevCumHelp > 0 && stdevGroupSize > 0) ? corr_HelpGroup = (sumprodHelpGroup / countGroupWithHelpers -
                                                                 meanCumHelp * meanGroupSizeHelp) /
                                                                (stdevCumHelp * stdevGroupSizeHelp) : corr_HelpGroup = 0;

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
         << "Low_survival_floater?: " << "\t" << parameters.isLowSurvialFloater() << endl
         << "No_group_augmentation?: " << "\t" << parameters.isNoGroupAugmentation() << endl
         << "No_effect_relatedness?: " << "\t" << parameters.isNoRelatedness() << endl
         << "Initial_population: " << "\t" << parameters.getMaxColonies() * (parameters.getInitNumHelpers() + 1) << endl
         << "Number_of_colonies: " << "\t" << parameters.getMaxColonies() << endl
         << "Number_generations: " << "\t" << parameters.getNumGenerations() << endl
         << "Number_replicates: " << "\t" << parameters.getMaxNumReplicates() << endl
         << "Bias_float_breeder: " << "\t" << parameters.getBiasFloatBreeder() << endl
         << "X0(Baseline_survival): " << "\t" << parameters.getX0() << endl
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
        << "Low_survival_floater?: " << "\t" << parameters.isLowSurvialFloater() << endl
        << "No_group_augmentation?: " << "\t" << parameters.isNoGroupAugmentation() << endl
        << "No_effect_relatedness?: " << "\t" << parameters.isNoRelatedness() << endl
        << "Initial_population: " << "\t" << parameters.getMaxColonies() * (parameters.getInitNumHelpers() + 1) << endl
        << "Number_of_colonies: " << "\t" << parameters.getMaxColonies() << endl
        << "Number_generations: " << "\t" << parameters.getNumGenerations() << endl
        << "Number_replicates: " << "\t" << parameters.getMaxNumReplicates() << endl
        << "Bias_float_breeder: " << "\t" << parameters.getBiasFloatBreeder() << endl
        << "X0(Baseline_survival): " << "\t" << parameters.getX0() << endl
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


    for (replica = 0; replica < parameters.getMaxNumReplicates(); replica++) {

        generation = 0;
        deaths = 0; // to keep track of how many individuals die each generation
        newbreederFloater = 0, newbreederHelper = 0, inheritance = 0; //to know if the new breeder was a helper or a floater

        // column headings on screen
        cout << setw(6) << "gen" << setw(9) << "pop" << setw(9) << "deaths" << setw(9)
             << "float" << setw(9) << "group" << setw(9) << "maxGroup" << setw(9) << "age" << setw(9)
             << "alpha" << setw(9) << "alphaAge" << setw(9) << "beta" << setw(9) << "betaAge" << setw(9)
             << "help" << setw(9) << "disper" << setw(9) << "surv" << setw(9) << "relat" << endl;


        vector<Individual> floaters;
        vector<Group> groups(parameters.getMaxColonies());

        InitGroup(groups);

        Statistics(groups, floaters);

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

            vector<Group, std::allocator<Group>>::iterator itDispersal;
            for (itDispersal = groups.begin(); itDispersal < groups.end(); ++itDispersal) {
                itDispersal->Disperse(floaters);
            }

            vector<Group, std::allocator<Group>>::iterator itHelpSurvival;
            for (itHelpSurvival = groups.begin(); itHelpSurvival < groups.end(); ++itHelpSurvival) {
                itHelpSurvival->CumHelp();
                itHelpSurvival->GroupSize();

                //Calculate survival for the helpers
                vector<Individual, std::allocator<Individual>>::iterator helperStatsIt; //helpers
                for (helperStatsIt = itHelpSurvival->helpers.begin(); helperStatsIt < itHelpSurvival->helpers.end(); ++helperStatsIt) {
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

            vector<Individual, std::allocator<Individual>>::iterator floatIt;
            for (floatIt = floaters.begin(); floatIt < floaters.end(); ++floatIt) {
                floatIt->calcSurvival(0); // TODO:Change to 1?
            }

            if (generation % parameters.getSkip() == 0) {
                Statistics(groups, floaters); // Statistics calculated before survival
            }

            vector<Group, std::allocator<Group>>::iterator itSurvival;
            for (itSurvival = groups.begin(); itSurvival < groups.end(); ++itSurvival) {
                itSurvival->SurvivalGroup(deaths);
            }

            SurvivalFloaters(floaters, deaths);

            vector<Group, std::allocator<Group>>::iterator itBreeder;
            for (itBreeder = groups.begin(); itBreeder < groups.end(); ++itBreeder) {
                if (!itBreeder->breederAlive) {
                    itBreeder->NewBreeder(floaters, newbreederFloater, newbreederHelper, inheritance);
                }
            }

            vector<Group, std::allocator<Group>>::iterator itAge;
            for (itAge = groups.begin(); itAge < groups.end(); ++itAge) {
                itAge->IncreaseAge(); //add 1 rank or age to all individuals alive
            }

            Reassign(floaters, groups);
            /*if (!floaters.empty()) {
                cout << "Not all floaters were reassigned!" << endl;
            }*/

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
            if (generation == parameters.getNumGenerations()/10 || generation == parameters.getNumGenerations() / 4 || generation == parameters.getNumGenerations() / 2 || generation == parameters.getNumGenerations()) {

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

            vector<Group, std::allocator<Group>>::iterator itReproduction;
            for (itReproduction = groups.begin(); itReproduction < groups.end(); ++itReproduction) {
                itReproduction->Fecundity();
                itReproduction->Reproduction();
            }
        }
    }

    return 0;
}

/***********************************************
 GROUP AUGMENTATION MODEL
 - Passive group augmentation: individuals help in order to increase group size which in turn increases survival
 - Active group augmentation or delayed reciprocity: if inherit the breeding possition, individuals benefit of the help given by the recruits in the group.


 Stochastic individual based model.
 Limited breeding spots, unlimited population size; one breeder per group.
 Population overlap, turn over when breeder dies. 
 Older/more dominant individual higher probability of becoming new breeder.
 Evolution of level of help and dispersal. Inclusion of reaction norm to age.
 Dispersal produces temporal dispersers/floaters that can reproduce in another group or join as helpers. 
 Survival dependent on group size, level of help and predation/environment.
 Fecundity dependent on cummulative level of help within group.
 Relatedness as an emergent property.

***********************************************/

/*HEADER FILES*/

#include <cstdlib>
#include <stdio.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <random>
#include "parameters.h"
//#include <chrono> //if we use the system clock as seed

using namespace std;

// Output file
ofstream fout("group_augmentation_.txt");
ofstream fout2("group_augmentation_last_generation_.txt");


/*CONSTANTS AND STRUCTURES*/

// Random numbers
//mt19937 mt(time(0)); // random number generator
//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // if we don't want to obtain the same simulation under the same parameters
unsigned seed = 0; //in the same run takes different random values, but different runs same values unless we change the seed
default_random_engine generator(seed);
uniform_real_distribution<double> DriftUniform(0, 100);
uniform_real_distribution<double> Uniform(0, 1);

// mutation step size in the neutral genetic value to track level of relatedness


enum classes {
    BREEDER, HELPER, FLOATER
};

const int NO_VALUE = -1;

//Population parameters and Statistics
int replica, gen, population, driftGroupSize, maxGroupSize, populationHelpers;
int populationBeforeSurv, deaths, floatersgenerated, newbreederFloater, newbreederHelper, inheritance; //counters
double relatedness;
double meanGroupSize, stdevGroupSize, sumGroupSize, sumsqGroupSize, varGroupSize,
        meanAge, stdevAge, sumAge, sumsqAge, varAge,
        meanAlpha, stdevAlpha, sumAlpha, sumsqAlpha, varAlpha,
        meanAlphaAge, stdevAlphaAge, sumAlphaAge, sumsqAlphaAge, varAlphaAge,
        meanAlphaAge2, stdevAlphaAge2, sumAlphaAge2, sumsqAlphaAge2, varAlphaAge2,
        meanBeta, stdevBeta, sumBeta, sumsqBeta, varBeta,
        meanBetaAge, stdevBetaAge, sumBetaAge, sumsqBetaAge, varBetaAge,
        meanHelp, stdevHelp, sumHelp, sumsqHelp, varHelp,
        meanCumHelp, stdevCumHelp, sumCumHelp, sumsqCumHelp, varCumHelp,
        meanDispersal, stdevDispersal, sumDispersal, sumsqDispersal, varDispersal,
        meanSurvival, stdevSurvival, sumSurvival, sumsqSurvival, varSurvival,
        meanDriftB, sumDriftB, meanDriftH, sumDriftH,            //relatedness related
        meanDriftBH, meanDriftBB, sumDriftBH, sumDriftBB,        //relatedness related
        corr_HelpDispersal, sumprodHelpDispersal,
        corr_HelpGroup, sumprodHelpGroup;


//Structures

class Individual // define individual traits
{


    double alpha;
public:
    void increaseAge();

    double getAlpha() const;

    void setAlpha(double alpha);

    double getAlphaAge() const;

    void setAlphaAge(double alphaAge);

    double getAlphaAge2() const;

    void setAlphaAge2(double alphaAge2);

    double getBeta() const;

    void setBeta(double beta);

    double getBetaAge() const;

    void setBetaAge(double betaAge);

    double getDrift() const;

    void setDrift(double drift);

    double getDispersal() const;

    void setDispersal(double dispersal);

    double getHelp() const;

    void setHelp(double help);

    double getSurvival() const;

    void setSurvival(double survival);

    classes getFishType() const;

    void setFishType(classes fishType);

    int getAge() const;

    void setAge(int age);

    bool isInherit() const;

    void setInherit(bool inherit);

private:

    double alphaAge, alphaAge2, beta, betaAge, drift,               // genetic values
            dispersal, help, survival;                                    // phenotypic values
    classes fishType;                                                // possible classes: breeder, helper, floater
    int age;
    bool inherit;                                                    //did the new breeder inherit the territory or did it disperse?

    //Functions inside Individual

public:
    Individual(double alpha_ = 0, double alphaAge_ = 0, double alphaAge2_ = 0, double beta_ = 0, double betaAge_ = 0,
               double drift_ = 0, classes own_ = HELPER);

    Individual(const Individual &mother);

    double calcDispersal();

    double calcHelp();

    double calcSurvival(int totalHelpers);

    void Mutate();
};


Individual::Individual(double alpha, double alphaAge, double alphaAge2_, double beta_, double betaAge_, double drift_,
                       classes fishType_) {
    this->alpha = alpha;
    this->alphaAge = alphaAge;
    alphaAge2 = alphaAge2_;
    beta = beta_;
    betaAge = betaAge_;
    drift = drift_;
    Mutate();
    fishType = fishType_;
    age = 1;
    inherit = 1;
    survival = NO_VALUE;
    help = 0;
    dispersal = NO_VALUE;
}

Individual::Individual(const Individual &copy) {
    alpha = copy.alpha;
    alphaAge = copy.alphaAge;
    alphaAge2 = copy.alphaAge2;
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
    Group(double alpha_, double alphaAge_, double alphaAge2_, double beta_, double betaAge_, int numhelp_);

    double cumhelp;
    int totalHelpers;
    bool breederalive;                                     // for the breeder: 1 alive, 0 dead
    int groupSize;
    double fecundity;
    int realfecundity;

    Individual vbreeder; // create breeder inside group
    vector<Individual> vhelpers; // create a vector of helpers inside each group

//Functions inside Group
    void Dispersal(vector<Individual> &vfloaters);

    void Help();

    void SurvivalGroup(int &deaths);

    void NewBreeder(vector<Individual> &vfloaters, int &newbreederFloater, int &newbreederHelper, int &inheritance);

    void IncreaseAge();

    double TotalPopulation();

    void Fecundity();

    void Reproduction();
};


Group::Group(double alpha_ = INIT_ALPHA, double alphaAge_ = INIT_ALPHA_AGE, double alphaAge2_ = INIT_ALPHA_AGE2, double beta_ = INIT_BETA, double betaAge_ = INIT_BETA_AGE, int numhelp_ = INIT_NUM_HELPERS)
{
	vbreeder = Individual(alpha_, alphaAge_, alphaAge2_, beta_, betaAge_, DriftUniform(generator), BREEDER);
	breederalive = 1;
	fecundity = NO_VALUE;
	realfecundity = NO_VALUE;

	for (int i = 0; i < numhelp_; ++i)
	{
		vhelpers.push_back(Individual(alpha_, alphaAge_, alphaAge2_, beta_, betaAge_, DriftUniform(generator), HELPER));
	}

	TotalPopulation();
}


/* INITIALISE POPULATION */
void InitGroup(vector<Group> &vgroups) {
    int i;
    for (i = 0; i < MAX_COLONIES; i++) {
        vgroups[i] = Group(INIT_ALPHA, INIT_ALPHA_AGE, INIT_ALPHA_AGE2, INIT_BETA, INIT_BETA_AGE, INIT_NUM_HELPERS);
    }
}


/* BECOME FLOATER (STAY VS DISPERSE) */

double Individual::calcDispersal() {
    if (!REACTION_NORM_DISPERSAL) {

        dispersal = beta; // Range from 0 to 1 to compare to a Uniform distribution

        if (dispersal < 0 || dispersal > 1) {
            cout << "error in dispersal: " << dispersal << endl;
        }

    } else {
        dispersal = 1 / (1 + exp(betaAge * age - beta));
    }

    return dispersal;
}


void Group::Dispersal(vector<Individual> &vfloaters) {
    vector<Individual>::iterator dispersalIt = vhelpers.begin();
    int sizevec = vhelpers.size();
    int counting = 0;

    while (!vhelpers.empty() && sizevec > counting) {
        dispersalIt->calcDispersal();
        if (dispersalIt->getDispersal() < 0) {
            cout << "error in dispersal: " << dispersalIt->getDispersal() << endl;
        }

        if (Uniform(generator) < dispersalIt->getDispersal()) {
            dispersalIt->setInherit(0); //the location of the individual is not the natal territory
            vfloaters.push_back(*dispersalIt); //add the individual to the vector floaters in the last position
            vfloaters[vfloaters.size() - 1].setFishType(FLOATER);
            *dispersalIt = vhelpers[vhelpers.size() -
                                    1]; // this and next line removes the individual from the helpers vector
            vhelpers.pop_back();
            ++counting;
        } else
            ++dispersalIt, ++counting;
    }
    //cout << "floater size " << vfloaters.size() << endl;
}


/*DISPLAY LEVEL OF HELP*/
double Individual::calcHelp() {
    if (!REACTION_NORM_HELP) {
        help = alpha;
    } else {
        help = alpha + alphaAge * age + alphaAge2 * age * age;
        if (help < 0) { help = 0; }
    }

    return help;
}


void Group::Help() //Calculate accumulative help of all individuals inside of each group.
{
    cumhelp = 0;
    //Level of help for helpers
    for (vector<Individual>::iterator helpIt = vhelpers.begin(); helpIt < vhelpers.end(); ++helpIt) {
        helpIt->calcHelp();
        cumhelp += helpIt->getHelp();
    }
    //Level of help for breeders
    //vbreeder.calcHelp();
    //cumhelp += vbreeder.help;
}

/*SURVIVAL*/

double Individual::calcSurvival(int totalHelpers) {
    //survival = (1 - X0) / (1 + exp(Xsh*help - Xsn * (totalHelpers + 1))); // +1 to know group size (1 breeder + helpers)

    survival = X0 + Xsn / (1 + exp(-(totalHelpers + 1))) - Xsh / (1 +
                                                                  exp(-help)); //alternative implementation of survival, if Xsn=Xsh, equivalent size effect of help and group size in survival

    if (survival > 0.95) {
        survival = 0.95;
        cout << "survival greater than 1" << endl;
    }

    return survival;
}

void Individual::increaseAge() {
    this->age++;
}


void Group::SurvivalGroup(int &deaths) {
    totalHelpers = vhelpers.size();

    //Calculate the survival of the helpers

    vector<Individual>::iterator survHIt = vhelpers.begin();
    int sizevec = vhelpers.size();
    int counting = 0;
    while (!vhelpers.empty() && sizevec > counting) {
        //Calculate value of survival
        survHIt->calcSurvival(totalHelpers);

        //Mortality
        if (Uniform(generator) > survHIt->getSurvival()) {
            *survHIt = vhelpers[vhelpers.size() - 1];
            vhelpers.pop_back();
            ++counting;
            deaths++;
        } else
            ++survHIt, ++counting; //go to next individual
    }

    //Calculate the survival of the breeder
    vbreeder.calcSurvival(totalHelpers);
    if (Uniform(generator) > vbreeder.getSurvival()) {
        breederalive = 0;
        vbreeder.setAge(NO_VALUE);
        deaths++;
    }
}

void SurvivalFloaters(vector<Individual> &vfloaters, int &deaths) //Calculate the survival of the floaters
{
    vector<Individual>::iterator survFIt = vfloaters.begin();
    int sizevec = vfloaters.size();
    int counting = 0;
    while (!vfloaters.empty() && sizevec > counting) {
        //Calculate value of survival
        survFIt->calcSurvival(0);

        //Mortality
        if (Uniform(generator) > survFIt->getSurvival()) {
            *survFIt = vfloaters[vfloaters.size() - 1];
            vfloaters.pop_back();
            ++counting;
            deaths++;
        } else
            ++survFIt, ++counting; //go to next individual
    }
}

/* BECOME BREEDER */

void Group::NewBreeder(vector<Individual> &vfloaters, int &newbreederFloater, int &newbreederHelper, int &inheritance) {
    //    Select a random sample from the floaters
    int i = 0;
    int sumage = 0;
    double currentposition = 0; //age of the previous ind taken from Candidates
    int UniformFloatNum;
    double RandP = Uniform(generator);
    int proportFloaters = round(vfloaters.size() * BIAS_FLOAT_BREEDER / MAX_COLONIES);

    vector<Individual *> Candidates;
    vector<double> position; //vector of age to choose with higher likelihood the ind with higher age
    vector<int> TemporaryCandidates; // to prevent taking the same ind several times in the sample

    if (!vfloaters.empty() && vfloaters.size() > proportFloaters) {
        while (i < proportFloaters) {
            uniform_int_distribution<int> UniformFloat(0, vfloaters.size() - 1); //random floater ID taken in the sample
            UniformFloatNum = UniformFloat(generator);
            TemporaryCandidates.push_back(UniformFloatNum); //add references of the floaters sampled to a vector
            sort(TemporaryCandidates.begin(), TemporaryCandidates.end()); //sort vector
            i++;
        }

        int temp = 0;
        for (vector<int>::iterator itTempCandidates = TemporaryCandidates.begin();
             itTempCandidates < TemporaryCandidates.end(); ++itTempCandidates) {
            if (!(*itTempCandidates == temp)) //to make sure the same ind is not taken more than ones
            {
                Candidates.push_back(&vfloaters[UniformFloatNum]);
                temp = *itTempCandidates;
            }
        }
    } else if (!vfloaters.empty() && vfloaters.size() <
                                     proportFloaters) { ///When less floaters available than the sample size, takes all of them. Change to a proportion?
        for (vector<Individual>::iterator floatIt = vfloaters.begin(); floatIt < vfloaters.end(); ++floatIt) {
            Candidates.push_back(&(*floatIt));
        }
    }


    //    Join the helpers in the group to the sample of floaters

    for (vector<Individual>::iterator helpIt = vhelpers.begin(); helpIt < vhelpers.end(); ++helpIt) {
        Candidates.push_back(&(*helpIt));
    }


//     Choose breeder with higher likelihood for the highest age
    for (vector<Individual *>::iterator ageIt = Candidates.begin();
         ageIt < Candidates.end(); ++ageIt) //ageIt creates a vector of pointers to an individual
    {
        sumage += (*ageIt)->getAge(); //add all the age from the vector Candidates
    }

    for (vector<Individual *>::iterator age2It = Candidates.begin(); age2It < Candidates.end(); ++age2It) {
        position.push_back(static_cast<double>((*age2It)->getAge()) / static_cast<double>(sumage) +
                           currentposition); //creates a vector with proportional segments to the age of each individual
        currentposition = position[position.size() - 1];
    }

    if (vfloaters.empty() && Candidates.size() != vhelpers.size()) {
        cout << "Error assigning empty floaters to Breeder" << endl;
    }

    vector<Individual *>::iterator age3It = Candidates.begin();
    int counting = 0;
    while (counting < Candidates.size()) {
        if (RandP < position[age3It - Candidates.begin()]) //to access the same ind in the candidates vector
        {
            vbreeder = **age3It; //substitute the previous dead breeder
            breederalive = 1;

            if ((*age3It)->getFishType() == FLOATER) //delete the ind from the vector floaters
            {
                **age3It = vfloaters[vfloaters.size() - 1];
                vfloaters.pop_back();
                newbreederFloater++;
                if ((*age3It)->isInherit() == true) {
                    cout << "error in inheritance" << endl;
                }
            } else {
                **age3It = vhelpers[vhelpers.size() - 1]; //delete the ind from the vector helpers
                vhelpers.pop_back();
                newbreederHelper++;
                if ((*age3It)->isInherit() == true) {
                    inheritance++;                    //calculates how many individuals that become breeders are natal to the territory
                }
            }

            vbreeder.setFishType(BREEDER); //modify the class
            counting = Candidates.size();//end loop
        } else
            ++age3It, ++counting;
    }
}


/*REASSIGN FLOATERS*/

void Reassign(vector<Individual> &vfloaters, vector<Group> &vgroups) {
    uniform_int_distribution<int> UniformMaxCol(0, MAX_COLONIES - 1);
    int selectGroup;
    vector<Individual>::iterator indIt;
    while (!vfloaters.empty()) {
        indIt = vfloaters.end() - 1;
        selectGroup = UniformMaxCol(generator);
        indIt->setFishType(HELPER); //modify the class
        vgroups[selectGroup].vhelpers.push_back(
                *indIt); //add the floater to the helper vector in a randomly selected group
        vfloaters.pop_back(); //remove the floater from its vector
    }
}


/* INCREASE AGE*/
void Group::IncreaseAge() {
    for (vector<Individual>::iterator ageIt = vhelpers.begin(); ageIt < vhelpers.end(); ++ageIt) {
        ageIt->increaseAge();
    }
    if (breederalive == 1) {
        vbreeder.increaseAge();
    } else {
        vbreeder.setAge(NO_VALUE); //to check for dead breeders still existing in print last generation
    }
}


/* TOTAL NUMBER OF INDIVIDUALS PER GROUP*/

double Group::TotalPopulation() {
    if (breederalive == 1) {
        groupSize = vhelpers.size() + 1;
    } else {
        groupSize = vhelpers.size();
    }

    return groupSize;
}


/* REPRODUCTION */

void Group::Fecundity() {
    fecundity = K0 + K1 * cumhelp / (1 + cumhelp * K1); //fecundity function of cummulative help in the group

    poisson_distribution<int> PoissonFec(fecundity);
    realfecundity = PoissonFec(generator); //integer number
}

void Group::Reproduction() // populate offspring generation
{
    if (breederalive == 1) {
        for (int i = 0; i < realfecundity; i++) //number of offspring dependent on real fecundity
        {
            vhelpers.push_back(
                    Individual(vbreeder.getAlpha(), vbreeder.getAlphaAge(), vbreeder.getAlphaAge2(), vbreeder.getBeta(),
                               vbreeder.getBetaAge(),
                               vbreeder.getDrift())); //create a new individual as helper in the group. Call construct to assign the mother genetic values to the offspring, construct calls Mutate function.
        }
    }
}


void Individual::Mutate() // mutate genome of offspring 
{
    normal_distribution<double> NormalA(0,
                                        STEP_ALPHA); ///could be simplified if I decide to have all the steps size with the same magnitude
    normal_distribution<double> NormalB(0, STEP_BETA);
    normal_distribution<double> NormalD(0, STEP_DRIFT);

    if (Uniform(generator) < MUTATION_ALPHA) {
        alpha += NormalA(generator);
        if (!REACTION_NORM_HELP) {
            if (alpha < 0) { alpha = 0; }
        }
    }
    if (REACTION_NORM_HELP) {
        if (Uniform(generator) < MUTATION_ALPHA_AGE) {
            alphaAge += NormalA(generator);
        }

        if (Uniform(generator) < MUTATION_ALPHA_AGE2) {
            alphaAge2 += NormalA(generator);
        }
    }

    if (Uniform(generator) < MUTATION_BETA) {
        beta += NormalB(generator);
        if (!REACTION_NORM_DISPERSAL) {
            if (beta < 0) { beta = 0; }
            if (beta > 1) { beta = 1; }
        }
    }
    if (REACTION_NORM_DISPERSAL) {
        if (Uniform(generator) < MUTATION_BETA_AGE) {
            betaAge += NormalD(generator);
        }
    }

    if (Uniform(generator) < MUTATION_DRIFT) {
        drift += NormalD(generator);
    }
}

double Individual::getAlpha() const {
    return alpha;
}

void Individual::setAlpha(double alpha) {
    Individual::alpha = alpha;
}

double Individual::getAlphaAge() const {
    return alphaAge;
}

void Individual::setAlphaAge(double alphaAge) {
    Individual::alphaAge = alphaAge;
}

double Individual::getAlphaAge2() const {
    return alphaAge2;
}

void Individual::setAlphaAge2(double alphaAge2) {
    Individual::alphaAge2 = alphaAge2;
}

double Individual::getBeta() const {
    return beta;
}

void Individual::setBeta(double beta) {
    Individual::beta = beta;
}

double Individual::getBetaAge() const {
    return betaAge;
}

void Individual::setBetaAge(double betaAge) {
    Individual::betaAge = betaAge;
}

double Individual::getDrift() const {
    return drift;
}

void Individual::setDrift(double drift) {
    Individual::drift = drift;
}

double Individual::getDispersal() const {
    return dispersal;
}

void Individual::setDispersal(double dispersal) {
    Individual::dispersal = dispersal;
}

double Individual::getHelp() const {
    return help;
}

void Individual::setHelp(double help) {
    Individual::help = help;
}

double Individual::getSurvival() const {
    return survival;
}

void Individual::setSurvival(double survival) {
    Individual::survival = survival;
}

classes Individual::getFishType() const {
    return fishType;
}

void Individual::setFishType(classes fishType) {
    Individual::fishType = fishType;
}

int Individual::getAge() const {
    return age;
}

void Individual::setAge(int age) {
    Individual::age = age;
}

bool Individual::isInherit() const {
    return inherit;
}

void Individual::setInherit(bool inherit) {
    Individual::inherit = inherit;
}


/* CALCULATE STATISTICS */
void Statistics(vector<Group> vgroups) {

    relatedness = 0.0, driftGroupSize = 0, populationHelpers = 0,
    meanGroupSize = 0.0, stdevGroupSize = 0.0, maxGroupSize = 0, sumGroupSize = 0.0, sumsqGroupSize = 0.0, varGroupSize = 0.0,
    meanAge = 0.0, stdevAge = 0.0, sumAge = 0.0, sumsqAge = 0.0, varAge = 0.0,
    meanAlpha = 0.0, stdevAlpha = 0.0, sumAlpha = 0.0, sumsqAlpha = 0.0, varAlpha = 0.0,
    meanAlphaAge = 0.0, stdevAlphaAge = 0.0, sumAlphaAge = 0.0, sumsqAlphaAge = 0.0, varAlphaAge = 0.0,
    meanAlphaAge2 = 0.0, stdevAlphaAge2 = 0.0, sumAlphaAge2 = 0.0, sumsqAlphaAge2 = 0.0, varAlphaAge2 = 0.0,
    meanBeta = 0.0, stdevBeta = 0.0, sumBeta = 0.0, sumsqBeta = 0.0, varBeta = 0.0,
    meanBetaAge = 0.0, stdevBetaAge = 0.0, sumBetaAge = 0.0, sumsqBetaAge = 0.0, varBetaAge = 0.0,
    meanHelp = 0.0, stdevHelp = 0.0, sumHelp = 0.0, sumsqHelp = 0.0, varHelp = 0.0,
    meanCumHelp = 0.0, stdevCumHelp = 0.0, sumCumHelp = 0.0, sumsqCumHelp = 0.0, varCumHelp = 0.0,
    meanDispersal = 0.0, stdevDispersal = 0.0, sumDispersal = 0.0, sumsqDispersal = 0.0, varDispersal = 0.0,
    meanSurvival = 0.0, stdevSurvival = 0.0, sumSurvival = 0.0, sumsqSurvival = 0.0, varSurvival = 0.0,
    meanDriftB = 0.0, sumDriftB = 0.0, meanDriftH = 0.0, sumDriftH = 0.0,
    meanDriftBH = 0.0, meanDriftBB = 0.0, sumDriftBH = 0.0, sumDriftBB = 0.0,
    corr_HelpDispersal = 0.0, sumprodHelpDispersal = 0.0,
    corr_HelpGroup = 0.0, sumprodHelpGroup = 0.0;

    for (vector<Group>::iterator groupStatsIt = vgroups.begin(); groupStatsIt < vgroups.end(); ++groupStatsIt) {

        for (vector<Individual>::iterator indStatsIt = groupStatsIt->vhelpers.begin();
             indStatsIt < groupStatsIt->vhelpers.end(); ++indStatsIt) {

            sumAge += indStatsIt->getAge();
            sumsqAge += indStatsIt->getAge() * indStatsIt->getAge();

            sumAlpha += indStatsIt->getAlpha();
            sumsqAlpha += indStatsIt->getAlpha() * indStatsIt->getAlpha();

            sumAlphaAge += indStatsIt->getAlphaAge();
            sumsqAlphaAge += indStatsIt->getAlphaAge() * indStatsIt->getAlphaAge();

            sumAlphaAge2 += indStatsIt->getAlphaAge2();
            sumsqAlphaAge2 += indStatsIt->getAlphaAge2() * indStatsIt->getAlphaAge2();

            sumBeta += indStatsIt->getBeta();
            sumsqBeta += indStatsIt->getBeta() * indStatsIt->getBeta();

            sumBetaAge += indStatsIt->getBetaAge();
            sumsqBetaAge += indStatsIt->getBetaAge() * indStatsIt->getBetaAge();

            sumHelp += indStatsIt->getHelp();
            sumsqHelp += indStatsIt->getHelp() * indStatsIt->getHelp();

            sumDispersal += indStatsIt->getDispersal();
            sumsqDispersal += indStatsIt->getDispersal() * indStatsIt->getDispersal();

            sumSurvival += indStatsIt->getSurvival();
            sumsqSurvival += indStatsIt->getSurvival() * indStatsIt->getSurvival();

            sumprodHelpDispersal += indStatsIt->getHelp() * indStatsIt->getDispersal();

            if (groupStatsIt->breederalive) {
                sumDriftB += groupStatsIt->vbreeder.getDrift();
                sumDriftH += indStatsIt->getDrift();
                sumDriftBH += indStatsIt->getDrift() * groupStatsIt->vbreeder.getDrift();
                sumDriftBB += groupStatsIt->vbreeder.getDrift() * groupStatsIt->vbreeder.getDrift();
                ++driftGroupSize;
            }
        }

        populationHelpers += groupStatsIt->vhelpers.size();

        sumGroupSize += groupStatsIt->groupSize;
        sumsqGroupSize += groupStatsIt->groupSize * groupStatsIt->groupSize;
        if (groupStatsIt->groupSize > maxGroupSize) maxGroupSize = groupStatsIt->groupSize;

        if (groupStatsIt->breederalive == 1) sumAge += groupStatsIt->vbreeder.getAge();
        if (groupStatsIt->breederalive == 1)
            sumsqAge += groupStatsIt->vbreeder.getAge() * groupStatsIt->vbreeder.getAge();

        if (groupStatsIt->breederalive == 1) sumAlpha += groupStatsIt->vbreeder.getAlpha();
        if (groupStatsIt->breederalive == 1)
            sumsqAlpha += groupStatsIt->vbreeder.getAlpha() * groupStatsIt->vbreeder.getAlpha();

        if (groupStatsIt->breederalive == 1) sumAlphaAge += groupStatsIt->vbreeder.getAlphaAge();
        if (groupStatsIt->breederalive == 1)
            sumsqAlphaAge += groupStatsIt->vbreeder.getAlphaAge() * groupStatsIt->vbreeder.getAlphaAge();

        if (groupStatsIt->breederalive == 1) sumAlphaAge2 += groupStatsIt->vbreeder.getAlphaAge2();
        if (groupStatsIt->breederalive == 1)
            sumsqAlphaAge2 += groupStatsIt->vbreeder.getAlphaAge2() * groupStatsIt->vbreeder.getAlphaAge2();

        if (groupStatsIt->breederalive == 1) sumBeta += groupStatsIt->vbreeder.getBeta();
        if (groupStatsIt->breederalive == 1)
            sumsqBeta += groupStatsIt->vbreeder.getBeta() * groupStatsIt->vbreeder.getBeta();

        if (groupStatsIt->breederalive == 1) sumBetaAge += groupStatsIt->vbreeder.getBetaAge();
        if (groupStatsIt->breederalive == 1)
            sumsqBetaAge += groupStatsIt->vbreeder.getBetaAge() * groupStatsIt->vbreeder.getBetaAge();

        if (groupStatsIt->breederalive == 1) sumSurvival += groupStatsIt->vbreeder.getSurvival();
        if (groupStatsIt->breederalive == 1)
            sumsqSurvival += groupStatsIt->vbreeder.getSurvival() * groupStatsIt->vbreeder.getSurvival();

        sumCumHelp += groupStatsIt->cumhelp;
        sumsqCumHelp += groupStatsIt->cumhelp * groupStatsIt->cumhelp;

        sumprodHelpGroup += groupStatsIt->cumhelp * groupStatsIt->groupSize;
    }

    meanGroupSize = sumGroupSize / MAX_COLONIES;
    meanAge = sumAge / population;
    meanAlpha = sumAlpha / population; ///population=sumGroupSize so simplify!
    meanAlphaAge = sumAlphaAge / population;
    meanAlphaAge2 = sumAlphaAge2 / population;
    meanBeta = sumBeta / population;
    meanBetaAge = sumBetaAge / population;
    meanDriftB = sumDriftB / driftGroupSize;
    meanDriftH = sumDriftH / driftGroupSize;
    meanDriftBH = sumDriftBH / driftGroupSize;
    meanDriftBB = sumDriftBB / driftGroupSize;
    meanHelp = sumHelp / populationHelpers;
    meanCumHelp = sumCumHelp / MAX_COLONIES;
    meanDispersal = sumDispersal / populationHelpers;
    meanSurvival = sumSurvival / population;

    relatedness = (meanDriftBH - meanDriftB * meanDriftH) /
                  (meanDriftBB - meanDriftB * meanDriftB); //covariate of a neutral selected gene
    if ((meanDriftBB - meanDriftB * meanDriftB) == 0) { relatedness = 2; } //prevent to divide by 0

    varGroupSize = sumsqGroupSize / MAX_COLONIES - meanGroupSize * meanGroupSize;
    varAge = sumsqAge / population - meanAge * meanAge;
    varAlpha = sumsqAlpha / population - meanAlpha * meanAlpha;
    varAlphaAge = sumsqAlphaAge / population - meanAlphaAge * meanAlphaAge;
    varAlphaAge2 = sumsqAlphaAge2 / population - meanAlphaAge2 * meanAlphaAge2;
    varBeta = sumsqBeta / population - meanBeta * meanBeta;
    varBetaAge = sumsqBetaAge / population - meanBetaAge * meanBetaAge;
    varHelp = sumsqHelp / populationHelpers - meanHelp * meanHelp;
    varCumHelp = sumsqCumHelp / MAX_COLONIES - meanCumHelp * meanCumHelp;
    varDispersal = sumsqDispersal / populationHelpers - meanDispersal * meanDispersal;
    varSurvival = sumsqSurvival / population - meanSurvival * meanSurvival;

    // to know if there is a problem (variance cannot be negative)
    varGroupSize > 0 ? stdevGroupSize = sqrt(varGroupSize) : stdevGroupSize = 0;
    varAge > 0 ? stdevAge = sqrt(varAge) : stdevAge = 0;
    varAlpha > 0 ? stdevAlpha = sqrt(varAlpha) : stdevAlpha = 0;
    varAlphaAge > 0 ? stdevAlphaAge = sqrt(varAlphaAge) : stdevAlphaAge = 0;
    varAlphaAge2 > 0 ? stdevAlphaAge2 = sqrt(varAlphaAge2) : stdevAlphaAge2 = 0;
    varBeta > 0 ? stdevBeta = sqrt(varBeta) : stdevBeta = 0;
    varBetaAge > 0 ? stdevBetaAge = sqrt(varBetaAge) : stdevBetaAge = 0;
    varHelp > 0 ? stdevHelp = sqrt(varHelp) : stdevHelp = 0;
    varCumHelp > 0 ? stdevCumHelp = sqrt(varCumHelp) : stdevCumHelp = 0;
    varDispersal > 0 ? stdevDispersal = sqrt(varDispersal) : stdevDispersal = 0;
    varSurvival > 0 ? stdevSurvival = sqrt(varBeta) : stdevSurvival = 0;

    //Correlations
    (stdevHelp > 0 && stdevDispersal > 0) ? corr_HelpDispersal = (sumprodHelpDispersal / populationHelpers -
                                                                  meanHelp * meanDispersal) /
                                                                 (stdevHelp * stdevDispersal) : corr_HelpDispersal = 0;
    (stdevCumHelp > 0 && stdevGroupSize > 0) ? corr_HelpGroup =
                                                       (sumprodHelpGroup / MAX_COLONIES - meanCumHelp * meanGroupSize) /
                                                       (stdevCumHelp * stdevGroupSize) : corr_HelpGroup = 0;

}


/* WRITE PARAMETER SETTINGS TO OUTPUT FILE */
void Printparams() {
    fout << "PARAMETER VALUES" << endl

         << "Reaction_norm_help: " << "\t" << REACTION_NORM_HELP << endl
         << "Reaction_norm_dispersal: " << "\t" << REACTION_NORM_DISPERSAL << endl
         << "Initial_population: " << "\t" << MAX_COLONIES * (INIT_NUM_HELPERS + 1) << endl
         << "Number_of_colonies: " << "\t" << MAX_COLONIES << endl
         << "Number_generations: " << "\t" << NUM_GENERATIONS << endl
         << "Number_replicates: " << "\t" << MAX_NUM_REPLICATES << endl
         << "Bias_float_breeder: " << "\t" << BIAS_FLOAT_BREEDER << endl
         << "Base_survival: " << "\t" << X0 << endl
         << "Cost_help: " << "\t" << Xsh << endl
         << "Benefit_group_size: " << "\t" << Xsn << endl
         << "K0: " << "\t" << K0 << endl
         << "K1: " << "\t" << K1 << endl
         << "initAlpha: " << "\t" << INIT_ALPHA << endl
         << "initAlphaAge: " << "\t" << INIT_ALPHA_AGE << endl
         << "initAlphaAge2: " << "\t" << INIT_ALPHA_AGE2 << endl
         << "initBeta: " << "\t" << INIT_BETA << endl
         << "initBetaAge: " << "\t" << INIT_BETA_AGE << endl
         << "mutAlpha: " << "\t" << MUTATION_ALPHA << endl
         << "mutAlphaAge: " << "\t" << MUTATION_ALPHA_AGE << endl
         << "mutAlphaAge2: " << "\t" << MUTATION_ALPHA_AGE2 << endl
         << "mutBeta: " << "\t" << MUTATION_BETA << endl
         << "mutBetaAge: " << "\t" << MUTATION_BETA_AGE << endl
         << "mutDrift: " << "\t" << MUTATION_DRIFT << endl
         << "stepAlpha: " << "\t" << STEP_ALPHA << endl
         << "stepBeta: " << "\t" << STEP_BETA << endl
         << "stepDrift: " << "\t" << STEP_DRIFT << endl << endl;


    fout2 << "PARAMETER VALUES" << endl

          << "Reaction_norm_help: " << "\t" << REACTION_NORM_HELP << endl
          << "Reaction_norm_dispersal: " << "\t" << REACTION_NORM_DISPERSAL << endl
          << "Initial_population: " << "\t" << MAX_COLONIES * (INIT_NUM_HELPERS + 1) << endl
          << "Number_of_colonies: " << "\t" << MAX_COLONIES << endl
          << "Number_generations: " << "\t" << NUM_GENERATIONS << endl
          << "Number_replicates: " << "\t" << MAX_NUM_REPLICATES << endl
          << "Bias_float_breeder: " << "\t" << BIAS_FLOAT_BREEDER << endl
          << "Base_survival: " << "\t" << X0 << endl
          << "Cost_help: " << "\t" << Xsh << endl
          << "Benefit_group_size: " << "\t" << Xsn << endl
          << "K0: " << "\t" << K0 << endl
          << "K1: " << "\t" << K1 << endl
          << "initAlpha: " << "\t" << INIT_ALPHA << endl
          << "initAlphaAge: " << "\t" << INIT_ALPHA_AGE << endl
          << "initAlphaAge2: " << "\t" << INIT_ALPHA_AGE2 << endl
          << "initBeta: " << "\t" << INIT_BETA << endl
          << "initBetaAge: " << "\t" << INIT_BETA_AGE << endl
          << "mutAlpha: " << "\t" << MUTATION_ALPHA << endl
          << "mutAlphaAge: " << "\t" << MUTATION_ALPHA_AGE << endl
          << "mutAlphaAge2: " << "\t" << MUTATION_ALPHA_AGE2 << endl
          << "mutBeta: " << "\t" << MUTATION_BETA << endl
          << "mutBetaAge: " << "\t" << MUTATION_BETA_AGE << endl
          << "mutDrift: " << "\t" << MUTATION_DRIFT << endl
          << "stepAlpha: " << "\t" << STEP_ALPHA << endl
          << "stepBeta: " << "\t" << STEP_BETA << endl
          << "stepDrift: " << "\t" << STEP_DRIFT << endl << endl;

}


/* WRITE GENETIC TRAIT VALUES TO OUTPUT FILE */
void WriteMeans() {

    // show values on screen
    cout << fixed << showpoint
         << setw(6) << gen
         << setw(9) << population
         << setw(9) << deaths
         << setw(9) << floatersgenerated
         << setw(9) << setprecision(2) << meanGroupSize
         << setw(9) << maxGroupSize
         << setw(9) << setprecision(2) << meanAge
         << setw(9) << setprecision(4) << meanAlpha
         << setw(9) << setprecision(4) << meanAlphaAge
         << setw(9) << setprecision(4) << meanAlphaAge2
         << setw(9) << setprecision(4) << meanBeta
         << setw(9) << setprecision(4) << meanBetaAge
         << setw(9) << setprecision(4) << meanHelp
         << setw(9) << setprecision(4) << meanDispersal
         << setw(9) << setprecision(2) << meanSurvival
         << setw(9) << setprecision(2) << relatedness
         << endl;


    // write values to output file
    fout << fixed << showpoint
         << gen
         << "\t" << population
         << "\t" << deaths
         << "\t" << floatersgenerated
         << "\t" << setprecision(4) << meanGroupSize
         << "\t" << setprecision(4) << meanAge
         << "\t" << setprecision(4) << meanAlpha
         << "\t" << setprecision(4) << meanAlphaAge
         << "\t" << setprecision(4) << meanAlphaAge2
         << "\t" << setprecision(4) << meanBeta
         << "\t" << setprecision(4) << meanBetaAge
         << "\t" << setprecision(4) << meanHelp
         << "\t" << setprecision(4) << meanCumHelp
         << "\t" << setprecision(4) << meanDispersal
         << "\t" << setprecision(4) << meanSurvival
         << "\t" << setprecision(4) << relatedness
         << "\t" << setprecision(4) << stdevGroupSize
         << "\t" << setprecision(4) << stdevAge
         << "\t" << setprecision(4) << stdevAlpha
         << "\t" << setprecision(4) << stdevAlphaAge
         << "\t" << setprecision(4) << stdevAlphaAge2
         << "\t" << setprecision(4) << stdevBeta
         << "\t" << setprecision(4) << stdevBetaAge
         << "\t" << setprecision(4) << stdevHelp
         << "\t" << setprecision(4) << stdevCumHelp
         << "\t" << setprecision(4) << stdevDispersal
         << "\t" << setprecision(4) << stdevSurvival
         << "\t" << setprecision(4) << corr_HelpDispersal
         << "\t" << setprecision(4) << corr_HelpGroup
         << "\t" << setprecision(4) << newbreederFloater
         << "\t" << setprecision(4) << newbreederHelper
         << "\t" << setprecision(4) << inheritance
         << endl;
}


/* MAIN PROGRAM */
int main() {

    Printparams();

    // column headings in output file 1
    fout << "Generation" << "\t" << "Population" << "\t" << "Deaths" << "\t" << "Floaters" << "\t"
         << "Group_size" << "\t" << "Age" << "\t" << "meanAlpha" << "\t" << "meanAlphaAge" << "\t" << "meanAlphaAge2"
         << "\t" << "meanBeta" << "\t" << "meanBetaAge" << "\t"
         << "meanHelp" << "\t" << "meanCumHelp" << "\t" << "meanDispersal" << "\t" << "meanSurvival" << "\t"
         << "Relatedness" << "\t"
         << "SD_GroupSize" << "\t" << "SD_Age" << "\t" << "SD_Alpha" << "\t" << "SD_AlphaAge" << "\t" << "SD_AlphaAge2"
         << "\t" << "SD_Beta" << "\t" << "SD_BetaAge" << "\t"
         << "SD_Help" << "\t" << "SD_CumHelp" << "\t" << "SD_Dispersal" << "\t" << "SD_Survival" << "\t"
         << "corr_Help_Disp" << "\t" << "corr_Help_Group" << "\t"
         << "newbreederFloater" << "\t" << "newbreederHelper" << "\t" << "inheritance" << endl;

    // column headings in output file 2
    fout2 << "replica" << "\t" << "groupID" << "\t" << "type" << "\t" << "age" << "\t"
          << "alpha" << "\t" << "alphaAge" << "\t" << "alphaAge2" << "\t"
          << "beta" << "\t" << "betaAge" << "\t" << "drift"
          << "\t" << "help" << "\t" << "dispersal" << "\t" << "survival" << endl;


    for (replica = 0; replica < MAX_NUM_REPLICATES; replica++) {

        gen = 0;
        deaths = 0; // to keep track of how many individuals die each generation
        population = 0; //total of ind in the whole simulation for the expecific generation
        populationBeforeSurv = 0;
        floatersgenerated = 0;
        newbreederFloater = 0, newbreederHelper = 0, inheritance = 0; //to know if the new breeder was a helper or a floater

        // column headings on screen
        cout << setw(6) << "gen" << setw(9) << "pop" << setw(9) << "deaths" << setw(9)
             << "float" << setw(9) << "group" << setw(9) << "maxGroup" << setw(9) << "age" << setw(9)
             << "alpha" << setw(9) << "alphaAge" << setw(9) << "alphaAge2" << setw(9)
             << "beta" << setw(9) << "betaAge" << setw(9)
             << "help" << setw(9) << "disper" << setw(9) << "surv" << setw(9) << "relat" << endl;


        vector<Individual> vfloaters;
        vector<Group> vgroups(MAX_COLONIES);

        InitGroup(vgroups);

        for (vector<Group>::iterator itPopulation = vgroups.begin(); itPopulation < vgroups.end(); ++itPopulation) {
            population += itPopulation->TotalPopulation(); //calculate number of ind in the whole population
        }

        Statistics(vgroups);
        WriteMeans();

        for (gen = 1; gen <= NUM_GENERATIONS; gen++) {
            //cout << "\t" << "\t" << "\t" << "\t" << "\t" << "GENERATION "<<gen<< " STARTS NOW!!!" <<endl;

            deaths = 0; // to keep track of how many individuals die each generation
            population = 0; //total of ind in the whole simulation for the expecific generation
            populationBeforeSurv = 0;
            floatersgenerated = 0;
            newbreederFloater = 0, newbreederHelper = 0, inheritance = 0;

            //cout << "Floaters before dispersal: " << vfloaters.size() << endl;
            for (vector<Group>::iterator itDispersal = vgroups.begin(); itDispersal < vgroups.end(); ++itDispersal) {
                populationBeforeSurv += itDispersal->TotalPopulation();
                itDispersal->Dispersal(vfloaters);
                floatersgenerated = vfloaters.size();
                //    cout << "Floaters after dispersal: " << vfloaters.size() << endl;
            }

            for (vector<Group>::iterator itHelpSurvival = vgroups.begin();
                 itHelpSurvival < vgroups.end(); ++itHelpSurvival) {
                itHelpSurvival->Help();
                itHelpSurvival->SurvivalGroup(deaths);
            }

            SurvivalFloaters(vfloaters, deaths);
            //           cout << "total deaths: " << deaths << endl;

            for (vector<Group>::iterator itBreeder = vgroups.begin(); itBreeder < vgroups.end(); ++itBreeder) {
                if (itBreeder->breederalive == 0) {
                    //cout << vgroups.begin() - breederIt << endl;
                    itBreeder->NewBreeder(vfloaters, newbreederFloater, newbreederHelper, inheritance);
                }
            }

            Reassign(vfloaters, vgroups);
            if (!vfloaters.empty()) {
                cout << "Not all floaters were reassigned!" << endl;
            }

            for (vector<Group>::iterator itAge = vgroups.begin(); itAge < vgroups.end(); ++itAge) {
                itAge->IncreaseAge(); //add 1 rank or age to all individuals alive

                population += itAge->TotalPopulation(); //calculate number of ind in the whole population
            }

            if (gen % SKIP == 0) {   // write output every 'skip' generations
                Statistics(vgroups);
                WriteMeans();
            }

            //Print last generation
            if (gen == NUM_GENERATIONS) {

                int groupID = 0;

                for (vector<Group>::iterator itGroups = vgroups.begin(); itGroups < vgroups.end(); ++itGroups) {
                    fout2 << fixed << showpoint
                          << replica + 1
                          << "\t" << groupID
                          << "\t" << itGroups->vbreeder.getFishType()
                          << "\t" << setprecision(4) << itGroups->vbreeder.getAge()
                          << "\t" << setprecision(4) << itGroups->vbreeder.getAlpha()
                          << "\t" << setprecision(4) << itGroups->vbreeder.getAlphaAge()
                          << "\t" << setprecision(4) << itGroups->vbreeder.getAlphaAge2()
                          << "\t" << setprecision(4) << itGroups->vbreeder.getBeta()
                          << "\t" << setprecision(4) << itGroups->vbreeder.getBetaAge()
                          << "\t" << setprecision(4) << itGroups->vbreeder.getDrift()
                          << "\t" << setprecision(4) << "NA"
                          << "\t" << setprecision(4) << "NA"
                          << "\t" << setprecision(4) << itGroups->vbreeder.getSurvival()
                          << endl;

                    for (vector<Individual>::iterator itHelpers = itGroups->vhelpers.begin();
                         itHelpers < itGroups->vhelpers.end(); ++itHelpers) {
                        fout2 << fixed << showpoint
                              << replica + 1
                              << "\t" << groupID
                              << "\t" << itHelpers->getFishType()
                              << "\t" << setprecision(4) << itHelpers->getAge()
                              << "\t" << setprecision(4) << itHelpers->getAlpha()
                              << "\t" << setprecision(4) << itHelpers->getAlphaAge()
                              << "\t" << setprecision(4) << itHelpers->getAlphaAge2()
                              << "\t" << setprecision(4) << itHelpers->getBeta()
                              << "\t" << setprecision(4) << itHelpers->getBetaAge()
                              << "\t" << setprecision(4) << itHelpers->getDrift()
                              << "\t" << setprecision(4) << itHelpers->getHelp()
                              << "\t" << setprecision(4) << itHelpers->getDispersal()
                              << "\t" << setprecision(4) << itHelpers->getSurvival()
                              << endl;
                    }

                    groupID++;
                }
            }

            for (vector<Group>::iterator itReproduction = vgroups.begin();
                 itReproduction < vgroups.end(); ++itReproduction) {
                itReproduction->Fecundity();
                itReproduction->Reproduction();
            }

        }

    }


    return 0;
}

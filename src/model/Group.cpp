
#include <iomanip>
#include <algorithm>
#include "Group.h"
#include "Classes.h"
#include "../Parameters.h"

using namespace std;


Group::Group(Parameters &parameters, std::default_random_engine &generator, int &generation) : breeder(
        parameters.driftUniform(generator), BREEDER, parameters,
        generator, generation) {
    this->generator = generator;
    this->parameters = parameters;
    this->generation = generation;


    breederAlive = true;
    helpersPresent = false;
    cumHelp = Parameters::NO_VALUE;
    fecundity = Parameters::NO_VALUE;
    realFecundity = Parameters::NO_VALUE;

    for (int i = 0; i < parameters.getInitNumHelpers(); ++i) {

        helpers.emplace_back(
                Individual(parameters.driftUniform(generator), HELPER, parameters, generator, generation));
    }

    GroupSize();
}

void Group::disperse(vector<Individual> &floaters) {
    vector<Individual, std::allocator<Individual >>::iterator
            dispersalIt;
    dispersalIt = helpers.begin();
    int sizevec = helpers.size();
    int counting = 0;

    while (!helpers.empty() && sizevec > counting) {
        dispersalIt->calcDispersal();

        if (parameters.uniform(generator) < dispersalIt->dispersal) {
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

/* BECOME BREEDER */

void Group::newBreeder(vector<Individual> &floaters, int &newBreederFloater, int &newBreederHelper, int &inheritance) {
    //    Select a random sample from the floaters
    int i = 0;
    int sumage = 0;
    double currentposition = 0; //age of the previous ind taken from Candidates
    int UniformFloatNum = 0;
    double RandP = parameters.uniform(generator);
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


/* INCREASE AGE*/
void Group::increaseAge() {
    vector<Individual, std::allocator<Individual>>::iterator ageIt;
    for (ageIt = helpers.begin(); ageIt < helpers.end(); ++ageIt) {
        ageIt->age++;
    }
    if (breederAlive) {
        breeder.age++;
    } else {
        breeder.age = parameters.NO_VALUE; //to check for dead breeders still existing in print last generation
    }
}


/* REPRODUCTION */

void Group::reproduce() // populate offspring generation
{
    //Calculate fecundity
    if (!parameters.isNoRelatedness()) {
        fecundity = parameters.getK0() + parameters.getK1() * cumHelp / (1 + cumHelp *
                                                                             parameters.getK1()); //fecundity function of cummulative help in the group. If cumHelp bigger than 2, no effect on fecundity
    } else {
        fecundity = parameters.getK0();
    }


    poisson_distribution<int> PoissonFec(fecundity);
    realFecundity = PoissonFec(generator); //integer number

    //Reproduction
    if (breederAlive) {
        for (int i = 0; i < realFecundity; i++) //number of offspring dependent on real fecundity
        {
            helpers.emplace_back(breeder, HELPER, parameters,
                                 generator,
                                 generation); //create a new individual as helper in the group. Call construct to assign the mother genetic values to the offspring, construct calls Mutate function.
        }
    }
}


/* TOTAL NUMBER OF INDIVIDUALS PER GROUP*/

void Group::GroupSize() {
    if (breederAlive) {
        groupSize = helpers.size() + 1;
    } else {
        groupSize = helpers.size();
    }
}

void Group::survival(int &deaths) {
    GroupSize(); //update group size after dispersal

    //Calculate the survival of the helpers

    vector<Individual, std::allocator<Individual>>::iterator survHIt;
    survHIt = helpers.begin();
    int sizevec = helpers.size();
    int counting = 0;
    while (!helpers.empty() && sizevec > counting) {

        //Mortality helpers
        if (parameters.uniform(generator) > survHIt->survival) {
            *survHIt = helpers[helpers.size() - 1];
            helpers.pop_back();
            ++counting;
            deaths++;
        } else
            ++survHIt, ++counting; //go to next individual
    }

    //Mortality breeder
    if (parameters.uniform(generator) > breeder.survival) {
        breederAlive = false;
        deaths++;
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
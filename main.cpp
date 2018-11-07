/***********************************************
 GROUP AUGMENTATION BASIC MODEL

 Level of help and dispersal with genetic bases. No reaction norms.
***********************************************/

/*HEADER FILES*/

#include <cstdlib>
#include <stdio.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <random>
//#include <chrono> //if we use the system clock as seed

using namespace std;

ofstream fout("group augmentation.txt");     // output file

/*CONSTANTS AND STRUCTURES*/


// Random numbers
//mt19937 mt(time(0)); // random number generator
//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // if we don't want to obtain the same simulation under the same parameters
unsigned seed = 0; ///change value for every generation ////in the same run takes different random values, but different runs same values unless we change the seed
default_random_engine generator (seed);
normal_distribution<double> DriftNormal(0, 10);
uniform_real_distribution<double> Uniform(0, 1);


//Run parameters
const int maxcolon     = 50;     // max number of groups or colonies --> breeding spots. Whole population size = maxcolon * (numhelp + 1)
const int numhelp      = 2;       //initial number of helpers per group when initializing group

const int NumGen       = 10000;   // number of generations
const int numrep       = 1;     // number of replicates
const int skip         = 50;   // interval between print-outs

//Fix values
const double m         = 0.7;       // predation pressure

// Modifiers
//const double X0r    = 1; // inflexion point in the level of help formula for the influence of rank/age
//const double X0n    = 1; // inflexion point in the level of help formula for the influence of group size
const double K0     = 1; // min fecundity, fecundity when no help provided.
const double K1     = 1; // benefit of cumhelp in the fecundity
const double Xsh    = 1 ; // cost of help in survival
const double Xsn    = 1; // benefit of group size in survival


//Genetic values
const double initAlpha    = 0.0;     // starting value of alpha (in gen 0)
const double mutAlpha     = 0.05;    // mutation rate in alpha for level of help
const double stepAlpha    = 0.4;     // mutation step size in alpha for level of help
const double initBeta     = 0.0;     // starting value of beta (in gen 0)
const double mutBeta      = 0.05;    // mutation rate in beta for the propensity to disperse
const double stepBeta     = 0.4;     // mutation step size in beta for the propensity to disperse
const double mutDrift     = 0.05;    // mutation rate in the neutral selected value to track level of relatedness
const double stepDrift    = 0.4;     // mutation step size in the neutral genetic value to track level of relatedness

//const int minsurv     = 50;     // min number of individual that survive

const double avFloatersSample = 10; ///average number of floaters sampled from the total ///Check first if there are enough floaters, take a proportion instead??

enum classes {breeder, helper, floater};

//Stats
int gen;
int population, deaths;
double  meanAlpha,stdevAlpha,sumAlpha,sumsqAlpha,varAlpha,
            meanBeta,stdevBeta, sumBeta,sumsqBeta,varBeta,
            meanDrift, stdevDrift, sumDrift,sumsqDrift,varDrift,
            corr_AlphaBeta,sumprodAlphaBeta,
            meanGroupsize, stdevGroupsize;         // population statistics


//Structures

struct Individual // define individual traits
{
	Individual(double alpha_,double beta_, classes own_);
	Individual(const Individual &mother);
    double alpha, beta, drift,                        // genetic values
            dispersal, help, survival;                // phenotypic values
    classes own;                                      // possible classes: breeder, helper, floater
    int age;

    //Functions inside Individual
    void calcDispersal();
    void calcHelp ();
    void calcSurvival(int totalHelpers);
    void Mutate();

};

Individual::Individual(double alpha_=0,double beta_=0, classes own_=helper){
	alpha=alpha_;
	beta=beta_;
	drift=DriftNormal(generator);
	own=own_;
	age = 1;
	survival = -1;      //out of range, so check later for errors
	help = -1;          //out of range, so check later for errors
}

Individual::Individual(const Individual &mother){
	alpha=mother.alpha;
	beta=mother.beta;
	drift=mother.drift;
	Mutate();
	own=helper;
	age = 1;
	survival = -1;      //out of range, so check later for errors
	help = -1;          //out of range, so check later for errors
}


struct Group // define group traits
{
    Group(double alpha_,double beta_,int numhelp_);
    double cumhelp;
    int totalHelpers;
    bool breederalive;                                     // for the breeder: 1 alive, 0 dead
    int totalIndividualGroup;
    double fecundity;
    int realfecundity;

    Individual vbreeder; // create breeder inside group
    vector<Individual> vhelpers; // create a vector of helpers inside each group

//Functions inside Individual
    void Dispersal(vector<Individual> &vfloaters);
    void Help();
    void SurvivalGroup(int &deaths);
    void Breeder(vector<Individual> &vfloaters);
    void increaseAge();
    void TotalPopulation();
    void Fecundity();
    void Reproduction();
//    void Statistics(vector<Individual>vhelpers);
};

Group::	Group(double alpha_=initAlpha,double beta_=initBeta,int numhelp_=2)
{
   	vbreeder = Individual(alpha_,beta_,breeder);
    breederalive = 1;
    fecundity = -1;         //out of range, so check later for errors
    realfecundity = -1;     //out of range, so check later for errors

   	for(int i=0;i<numhelp_;++i)
    {
    	vhelpers.push_back(Individual(alpha_,beta_,helper));
	}
}


/* INITIALISE POPULATION */
void InitGroup(vector<Group> &vgroups)
{
    int i;
    for (i=0; i<maxcolon; i++)
    {
		vgroups[i]=Group(initAlpha,initBeta,numhelp);
    }
}


/* BECOME FLOATER (STAY VS DISPERSE) */
void Individual::calcDispersal()
{
    dispersal = beta; // Range from 0 to 1 to compare to a Uniform distribution
    if (beta<0 || beta>1){
        cout <<"error in beta: " << beta << endl;
    }
}


void Group::Dispersal(vector<Individual> &vfloaters)
{
	vector<Individual>::iterator dispersalIt = vhelpers.begin();
	int sizevec = vhelpers.size();
	int counting = 0;

	while (!vhelpers.empty() && sizevec>counting)
	{
	    dispersalIt->calcDispersal();
		if (Uniform(generator) < dispersalIt->dispersal) // beta is equal to dispersal propensity
		{
			vfloaters.push_back(*dispersalIt); //add the individual to the vector floaters in the last position
			vfloaters[vfloaters.size()-1].own = floater;
			*dispersalIt = vhelpers[vhelpers.size() - 1]; // this and next line removes the individual from the helpers vector
			vhelpers.pop_back();
			++counting;
		}
		else
			++dispersalIt, ++counting;
	}
    //cout << "floater size " << vfloaters.size() << endl;
}

/*DISPLAY LEVEL OF HELP*/
void Individual::calcHelp ()
{
    help = alpha; //absolute value, help cannot be negative
    if (alpha<0)
    {
        cout << "error in alpha: " << alpha <<endl;
    }
    //help = 1 / (1 + exp (alpha * (age - X0r) - beta * (Group.size - X0n)); //later on it will need to be inside group
}


void Group::Help () //Calculate accumulative help of all individuals inside of each group.
{
	cumhelp = 0;
//Level of help for helpers
    for (vector<Individual>::iterator helpIt = vhelpers.begin(); helpIt < vhelpers.end(); ++helpIt)
    {
		helpIt->calcHelp();
		cumhelp+= helpIt->help;
    }
//Level of help for breeders
	vbreeder.calcHelp();
	cumhelp += vbreeder.help;
}

/*SURVIVAL*/

void Individual::calcSurvival(int totalHelpers)
{
        survival = m / (1 + exp (Xsh*help - Xsn*(totalHelpers + 1))); // +1 to know group size (1 breeder + helpers)
}


void Group::SurvivalGroup(int &deaths)
{
    totalHelpers=vhelpers.size();

    //Calculate the survival of the helpers

    vector<Individual>::iterator survHIt = vhelpers.begin();
    int sizevec = vhelpers.size();
    int counting = 0;
    while (!vhelpers.empty() && sizevec>counting)
    {
        //Calculate value of survival
        survHIt->calcSurvival(totalHelpers);
        //Mortality
        if (Uniform(generator) > survHIt->survival)
        {
            *survHIt = vhelpers[vhelpers.size() - 1];
            vhelpers.pop_back();
            ++counting;
            deaths++; //set to 0 for each generation in main
        }
        else
        ++survHIt, ++counting; //go to next individual
    }

        //Calculate the survival of the breeder
    vbreeder.calcSurvival(vhelpers.size());
    if (Uniform(generator) > vbreeder.survival)
    {
        breederalive=0;
        vbreeder.age=-1;
        deaths++;
    }
}

void SurvivalFloaters(vector<Individual> &vfloaters, int &deaths) //Calculate the survival of the floaters
{
    vector<Individual>::iterator survFIt = vfloaters.begin();
    int sizevec = vfloaters.size();
    int counting = 0;
    while (!vfloaters.empty() && sizevec>counting)
    {
//        cout << survFIt->own << "   " <<survFIt->age << endl;
        //Calculate value of survival
        survFIt->calcSurvival(0);
        //Mortality
        if (Uniform(generator) > survFIt->survival)
        {
            *survFIt = vfloaters[vfloaters.size() - 1];
            vfloaters.pop_back();
            ++counting;
            deaths++;
        }
        else
        ++survFIt, ++counting; //go to next individual
    }
}

/* BECOME BREEDER */

void Group::Breeder(vector<Individual> &vfloaters)
{
//    Select a random sample from the floaters
        int i=0;
        int sumage=0;
        double currentposition=0; //age of the previous ind taken from Candidates
        int UniformFloatNum;
        poisson_distribution<int> PoissonFloat(avFloatersSample); //random sample size of floaters taken to compete for breeding spot
        uniform_int_distribution<int> UniformFloat(0, vfloaters.size()); //random floater ID taken in the sample
        uniform_real_distribution<double> Randomposition (0, 1);
        double RandP = Randomposition (generator);
        int RandN = PoissonFloat (generator);

        vector<Individual*> Candidates;
        vector<double>position; //vector of age to chose with higher likelihood the ind with higher age

//        cout << "vfloaters: " << vfloaters.size() <<endl;
        if (vfloaters.size()>0 && vfloaters.size() > RandN){
            while (i < RandN) ///Change to a proportion instead
            {
                UniformFloatNum = UniformFloat(generator);
                Candidates.push_back(&vfloaters[UniformFloatNum]); ///PROBLEM: IT COULD PICK THE SAME IND SEVERAL TIMES (see commented code below)
                i++;
            }
        }

		else { //When less floaters available than the sample size, take all of them. Change to a proportion 
			for (vector<Individual>::iterator floatIt = vfloaters.begin(); floatIt < vfloaters.end(); ++floatIt)
			{
				Candidates.push_back(&(*floatIt));
			}
		}

		//vector<int>TemporaryCandidates;
		//UniformFloatNum = UniformFloat(generator);
		//TemporaryCandidates.push_back(UniformFloatNum);
		//sort(TemporaryCandidates.begin(), TemporaryCandidates.end());
		//int temp = 0;
		//for (vector<int>::iterator itTempCandidates = TemporaryCandidates.begin(); itTempCandidates < TemporaryCandidates.end(); ++itTempCandidates)
		//{
		//	if (!itTempCandidates == temp)
		//	{
		//		Candidates.push_back(&vfloaters[UniformFloatNum]);
		//		temp = itTempCandidates;
		//	}
		//}


//    Join the helpers in the group to the sample of floaters

        for (vector<Individual>::iterator helpIt = vhelpers.begin(); helpIt < vhelpers.end(); ++helpIt)
        {
            Candidates.push_back(&(*helpIt));
        }

        //cout << "Helpers size: " << vhelpers.size() << endl;
        //cout << "Candidates size: " << Candidates.size() <<endl;

//     Choose breeder with higher likelihood for the highest
        for (vector<Individual*>::iterator ageIt = Candidates.begin(); ageIt < Candidates.end(); ++ageIt) //ageIt creates a vector of pointers to an individual
        {
            sumage += (*ageIt)->age; //add all the age from the vector Candidates
//            cout << (*ageIt)->own << "  " << (*ageIt)->age << endl;

        }
//        cout << "sumage: " << sumage <<endl;
//        cout << "size candidates: " << Candidates.size() <<endl;

        for (vector<Individual*>::iterator age2It = Candidates.begin(); age2It < Candidates.end(); ++age2It)
        {
            position.push_back((*age2It)->age / sumage + currentposition); //create a vector with proportional segments to the age of each individual
            currentposition = position[position.size()-1];
        }

		vector<Individual*>::iterator age3It = Candidates.begin();
		int counting = 0;
		while (counting < Candidates.size()){
			if (RandP < position[age3It - Candidates.begin()]) //to access the same ind in the candidates vector
			{
				vbreeder = **age3It; //substitute the previous dead breeder
				breederalive = 1;

				if ((*age3It)->own == floater) //delete the ind from the vector floaters
				{
					**age3It = vfloaters[vfloaters.size() - 1];
					vfloaters.pop_back();
				}
				else
				{
					**age3It = vhelpers[vhelpers.size() - 1]; //delete the ind from the vector helpers
					vhelpers.pop_back();
				}

				vbreeder.own = breeder; //modify the class
				counting = Candidates.size();//end loop
			}
			else
				++age3It, ++counting;
        }
}


/*REASSIGN FLOATERS*/

void Reassign(vector<Individual> &vfloaters, vector<Group> &vgroups)
{
    uniform_int_distribution<int> UniformMaxCol(0, maxcolon);
    int selecGroup;
    vector<Individual>::iterator indIt;
    while (!vfloaters.empty())
    {
        indIt = vfloaters.end();
        selecGroup = UniformMaxCol(generator);
        indIt->own=helper; //modify the class
        vgroups[selecGroup].vhelpers.push_back(*indIt); //add the floater to the helper vector in a randomly selected group
        vfloaters.pop_back(); //remove the floater from its vector
    }
}


/* INCREASE AGE*/
void Group::increaseAge()
{
    for (vector<Individual>::iterator ageIt = vhelpers.begin(); ageIt < vhelpers.end(); ++ageIt)
    {
        ageIt->age++;
    }
    if (breederalive==1)
    {
        vbreeder.age++;
//        cout << "Age breeder: " << vbreeder.age << endl;
    }
    else
    {
//        cout << "Age breeder: BREEDER DEAD!" << endl;
    }

}

/*CALCULATE TOTAL NUMBER OF INDIVIDUALS PER GROUP*/

void Group::TotalPopulation()
{
    totalIndividualGroup=vhelpers.size()+1;
}


/* REPRODUCTION */

void Group::Fecundity()
{
    fecundity = K0 + K1*cumhelp;
    poisson_distribution<int> PoissonFec(fecundity);
    realfecundity = PoissonFec(generator);
//    cout << "Fecundity: " << fecundity <<"\t"<< "Real Fecundity: " << realfecundity << endl;
}

void Group::Reproduction() // populate offspring generation
{
    if (breederalive==1)
    {
        for (int i=0;i<realfecundity;i++) //number of offspring dependent on real fecundity
        {
            vhelpers.push_back(Individual(vbreeder)); //create a new individual as helper in the group. Call construct to assign the mother genetic values to the offspring, construct calls Mutate function.
        }
    }
}


void Individual::Mutate() /// mutate genome of offspring // PROBLEM: All alpha and beta should be 0 in gen=0, they also get negative values!
{
    normal_distribution <double> NormalA(0, stepAlpha); ///could be simplified if I decide to have all the steps size with the same magnitude
    normal_distribution <double> NormalB(0, stepBeta);
    normal_distribution <double> NormalD(0, stepDrift);

    if (Uniform(generator)< mutAlpha){
        alpha += NormalA(generator);
        if (alpha < 0){alpha=0;}
    }

    if (Uniform(generator)< mutBeta){
        beta += NormalB(generator);
        if (beta < 0){beta=0;}
        if (beta > 1){beta=1;}
    }

    if (Uniform(generator)< mutDrift){
        drift += NormalD(generator);
    }
 }


/* CALCULATE STATISTICS */
void Statistics(vector<Group>vgroups){

	for (vector<Group>::iterator groupStatsIt = vgroups.begin(); groupStatsIt < vgroups.end(); ++groupStatsIt){

        for (vector<Individual>::iterator indStatsIt = groupStatsIt->vhelpers.begin(); indStatsIt < groupStatsIt->vhelpers.end(); ++indStatsIt){

            sumAlpha+=indStatsIt->alpha;
            sumsqAlpha+=indStatsIt->alpha*indStatsIt->alpha;

            sumBeta+=indStatsIt->beta;
            sumsqBeta+=indStatsIt->beta*indStatsIt->beta;

            sumDrift+=indStatsIt->drift;
            sumsqDrift+=indStatsIt->drift*indStatsIt->drift;

        }

        sumAlpha+=groupStatsIt->vbreeder.alpha;
		sumsqAlpha+=groupStatsIt->vbreeder.alpha*groupStatsIt->vbreeder.alpha;

		sumBeta+=groupStatsIt->vbreeder.beta;
		sumsqBeta+=groupStatsIt->vbreeder.beta*groupStatsIt->vbreeder.beta;

		sumDrift+=groupStatsIt->vbreeder.drift;
		sumsqDrift+=groupStatsIt->vbreeder.drift*groupStatsIt->vbreeder.drift;

		sumprodAlphaBeta+=groupStatsIt->vbreeder.alpha*groupStatsIt->vbreeder.beta;
    }

    meanAlpha=sumAlpha/population;
    meanBeta=sumBeta/population;
    meanDrift=sumDrift/population; ///differentiate between within group and global

	varAlpha=sumsqAlpha/population-meanAlpha*meanAlpha;
	varBeta=sumsqBeta/population-meanBeta*meanBeta;
	varDrift=sumsqDrift/population-meanDrift*meanDrift; ///differentiate between within group and global

// to know if there is a problem, variance cannot be negative
	varAlpha>0? stdevAlpha=sqrt(varAlpha):stdevAlpha=0;
	varBeta>0? stdevBeta=sqrt(varBeta):stdevBeta=0;
	varDrift>0? stdevDrift=sqrt(varDrift):stdevDrift=0; ///differentiate between within group and global

	(stdevAlpha>0 && stdevBeta>0)? corr_AlphaBeta=(sumprodAlphaBeta/population-meanAlpha*meanBeta)/(stdevAlpha*stdevBeta):corr_AlphaBeta=0; ///add correlations between alpha and beta to withing group relatedness

}



/* WRITE PARAMETER SETTINGS TO OUTPUT FILE */
void Printparams()
{
  fout << endl << "PARAMETER VALUES" << endl

       << "Initial population: " << "\t" << maxcolon*(numhelp+1) << endl
       << "Number of colonies: " << "\t" << maxcolon << endl
       << "Number generations: " << "\t" << NumGen << endl
       << "Predation: " << "\t" << m << endl
       << "initAlpha: " << "\t" << setprecision(4) << initAlpha << endl
       << "initBeta: " << "\t" << setprecision(4) << initBeta << endl
       << "mutAlpha: " << "\t" << setprecision(4) << mutAlpha << endl
       << "stepAlpha: " << "\t" << setprecision(4) << stepAlpha << endl
       << "mutBeta: " << "\t" << setprecision(4) << mutBeta << endl
       << "stepBeta: " << "\t" << setprecision(4) << stepBeta << endl
       << "mutDrift: " << "\t" << setprecision(4) << mutDrift << endl
       << "stepDrift: " << "\t" << setprecision(4) << stepDrift << endl
       << "K0: " << "\t" << K0 << endl
       << "Xsh: " << "\t" << Xsh << endl
       << "Xsn: " << "\t" << Xsn << endl;

}



/* WRITE GENETIC TRAIT VALUES TO OUTPUT FILE */
void WriteMeans()
{

  // show values on screen
  cout << setw(6) << gen
       << setw(9) << population
       << setw(9) << deaths
	   << setw(9) << setprecision(4) << meanAlpha
	   << setw(9) << setprecision(4) << meanBeta
	   << setw(9) << setprecision(4) << meanDrift
	   << endl;


  // write values to output file
  fout << gen
       << "\t" << population
       << "\t" << deaths
	   << "\t" << setprecision(4) << meanAlpha
	   << "\t" << setprecision(4) << meanBeta
	   << "\t" << setprecision(4) << meanDrift
	   << "\t" << setprecision(4) << stdevAlpha
	   << "\t" << setprecision(4) << stdevBeta
	   << "\t" << setprecision(4) << stdevDrift
	   << "\t" << setprecision(4) << corr_AlphaBeta
	   << endl;
}


/* MAIN PROGRAM */
int main(){

for(int rep=0;rep<numrep;rep++){

    gen=0;

// Population statistics
    meanAlpha=0.0,stdevAlpha=0.0,sumAlpha=0.0,sumsqAlpha=0.0,varAlpha=0.0,
    meanBeta=0.0,stdevBeta=0.0, sumBeta=0.0,sumsqBeta=0.0,varBeta=0.0,
    meanDrift=0.0, stdevDrift=0.0, sumDrift=0.0,sumsqDrift=0.0,varDrift=0.0,
    corr_AlphaBeta=0.0,sumprodAlphaBeta=0.0,
    meanGroupsize=0.0, stdevGroupsize=0.0;

    // column headings on screen
    cout << setw(6) << "gen" << setw(9) << "population" << setw(9)<< "deaths" << setw(9) << "alpha" << setw(9) << "beta" << setw(9) << "drift" << endl;

	// column headings in output file
    fout << "Generation" << "\t" << "Population" << "\t" << "Deaths" << "\t"  <<  "meanAlpha" << "\t" << "meanBeta" << "\t" << "meanDrift" << "\t"
         << "SD_Alpha" << "\t" << "SD_Beta" << "\t" << "SD_Drift" << "\t" << "corr_AB" << "\t" << endl;

    vector<Individual> vfloaters;
	vector<Group> vgroups (maxcolon);

	InitGroup(vgroups);
	population = maxcolon*(numhelp+1);

	Statistics(vgroups);
	WriteMeans();

    for (gen=1;gen<=NumGen;gen++)
    {
//        cout << "\t" << "\t" << "\t" << "\t" << "\t" << "GENERATION "<<gen<< " STARTS NOW!!!" <<endl;

        deaths=0; // to keep track of how many individuals die each generation
        population=0;

// Population statistics
        meanAlpha=0.0,stdevAlpha=0.0,sumAlpha=0.0,sumsqAlpha=0.0,varAlpha=0.0,
        meanBeta=0.0,stdevBeta=0.0, sumBeta=0.0,sumsqBeta=0.0,varBeta=0.0,
        meanDrift=0.0, stdevDrift=0.0, sumDrift=0.0,sumsqDrift=0.0,varDrift=0.0,
        corr_AlphaBeta=0.0,sumprodAlphaBeta=0.0,
        meanGroupsize=0.0, stdevGroupsize=0.0;


        for (vector<Group>::iterator dispersalIt = vgroups.begin(); dispersalIt < vgroups.end(); ++dispersalIt)
        {
            dispersalIt->Dispersal(vfloaters);
//            cout << "Floaters after dispersal: " << vfloaters.size() << endl;
        }

        for (vector<Group>::iterator helpsurvIt = vgroups.begin(); helpsurvIt < vgroups.end(); ++helpsurvIt)
        {
            helpsurvIt->Help();
            helpsurvIt->SurvivalGroup(deaths);
        }

        SurvivalFloaters(vfloaters,deaths);
 //           cout << "total deaths: " << deaths << endl;

        for (vector<Group>::iterator breederIt = vgroups.begin(); breederIt < vgroups.end(); ++breederIt)
        {
            if (breederIt->breederalive == 0)
            {
                //cout << vgroups.begin() - breederIt << endl;
                breederIt->Breeder(vfloaters);
            }
        }

//        cout << "Floaters before reassign: " << vfloaters.size() << endl;
        Reassign(vfloaters, vgroups);
//        cout << "Floaters after reassign: " << vfloaters.size() << endl;

        for (vector<Group>::iterator increaseAgeIt = vgroups.begin(); increaseAgeIt < vgroups.end(); ++increaseAgeIt)
        {
            increaseAgeIt->increaseAge(); //add 1 rank or age to all individuals alive
            increaseAgeIt->TotalPopulation();
            population += increaseAgeIt->totalIndividualGroup;
        }
 //       cout << "Population: " << population << endl;

        for (vector<Group>::iterator reprodIt = vgroups.begin(); reprodIt < vgroups.end(); ++reprodIt)
        {
            reprodIt->Fecundity();
            reprodIt->Reproduction();
        }

		if (gen%skip==0){   // write output every 'skip' generations
            Statistics(vgroups);
            WriteMeans();
        }
    }
    fout << endl << endl << endl;

}
    Printparams();

	cin.get();

    return 0;
}

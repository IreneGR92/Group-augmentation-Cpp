/***********************************************
 GROUP AUGMENTATION BASIC MODEL

 Level of help and dispersal with genetic bases. No reaction norms.
***********************************************/

//HEADER FILES

#include <cstdlib>
#include <stdio.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
//#include <chrono> //if we use the system clock as seed
#include <vector>
//#include "H:\PhD\CODE\random.h" ///decide for one method

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

//Fix values
const double m         = 0.8;       // predation pressure

// Modifiers
//const double X0r    = 1; // inflexion point in the level of help formula for the influence of rank/age
//const double X0n    = 1; // inflexion point in the level of help formula for the influence of group size
const double K0     = 1; // min fecundity, fecundity when no help provided.
const double Xsh    = 1; // cost of help in survival
const double Xsr    = 1; // benefit of age in survival
const double Xsn    = 1; // benefit of group size in survival


//Genetic values
const double initAlpha    = 0.0;     // starting value of alpha (in gen 0)
const double mutAlpha     = 0.05;    // mutation rate in alpha for level of help
const double stepAlpha    = 0.4;     // mutation step size in alpha for level of help
const double initBeta     = 0.0;     // starting value of beta (in gen 0)
const double mutBeta      = 0.05;    // mutation rate in beta for the propensity to disperse
const double stepBeta     = 0.4;     // mutation step size in beta for the propensity to disperse
const double mutDrift     = 0.05;    // mutation rate in the neutral genetic value to track level of relatedness
const double stepDrift    = 0.4;     // mutation step size in the neutral genetic value to track level of relatedness

const int minsurv     = 50;     // min number of individual that survive

const double avFloatersSample = 10; ///average number of floaters sampled from the total ///Check first if there are enough floaters, take a proportion instead??

enum classes {breeder, helper, floater};

const int maxcolon     = 50;     // max number of groups or colonies --> breeding spots. Whole population size = maxcolon * (numhelp + 1)
const int numhelp      = 2;       //initial number of helpers per group when initializing group

const int NumGen       = 1000;   // number of generations
const int numrep       = 20;     // number of replicates
const int skip         = 50;     // interval between print-outs


//Structures

struct Individual // define individual traits
{
	Individual(double alpha_,double beta_, classes own_);
//	Individual(const Individual &mother);
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
    void Fecundity();
    void Reproduction();
};

Group::	Group(double alpha_=initAlpha,double beta_=initBeta,int numhelp_=2)
{
   	vbreeder = Individual(alpha_,beta_,breeder);
    breederalive=1;
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
}


void Group::Dispersal(vector<Individual> &vfloaters)
{
	vector<Individual>::iterator dispersalIt = vhelpers.begin();
	int size = vhelpers.size();
	int count = 0;

	while (!vhelpers.empty() && size>count)
	{
	    dispersalIt->calcDispersal();
		if (Uniform(generator) < dispersalIt->dispersal) // beta is equal to dispersal propensity
		{
			dispersalIt->own=floater; //modify the class
			vfloaters.push_back(*dispersalIt); //add the individual to the vector floaters in the last position
			*dispersalIt = vhelpers[vhelpers.size() - 1]; // this and next line removes the individual from the helpers vector
			vhelpers.pop_back();
			dispersalIt->help=0;
			++count;
		}
		else
			++dispersalIt, ++count;
	}
}

/*DISPLAY LEVEL OF HELP*/
void Individual::calcHelp ()
{
    help = alpha; //absolute value, help cannot be negative
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
        survival = m / (1 + exp (Xsh*help - Xsr*age - Xsn*(totalHelpers + 1))); // +1 to know group size (1 breeder + helpers)
}


void Group::SurvivalGroup(int &deaths)
{
    totalHelpers=vhelpers.size();

    //Calculate the survival of the helpers

    vector<Individual>::iterator survHIt = vhelpers.begin();
    int size = vhelpers.size();
    int count = 0;
    while (!vhelpers.empty() && size>count)
    {
        //Calculate value of survival
        survHIt->calcSurvival(totalHelpers);
        //Mortality
        if (Uniform(generator) > survHIt->survival)
        {
            *survHIt = vhelpers[vhelpers.size() - 1];
            vhelpers.pop_back();
            ++count;
            deaths++; //set to 0 for each generation in main
        }
        else
        ++survHIt, ++count; //go to next individual
    }

        //Calculate the survival of the breeder
    vbreeder.calcSurvival(vhelpers.size());
    if (Uniform(generator) > vbreeder.survival)
    {
        breederalive=0;
        deaths++;
    }
}

void SurvivalFloaters(vector<Individual> &vfloaters, int &deaths) //Calculate the survival of the floaters
{
    vector<Individual>::iterator survFIt = vfloaters.begin();
    int size = vfloaters.size();
    int count = 0;
    while (!vfloaters.empty() && size>count)
    {
        //Calculate value of survival
        survFIt->calcSurvival(0);
        //Mortality
        if (Uniform(generator) > survFIt->survival)
        {
            *survFIt = vfloaters[vfloaters.size() - 1];
            vfloaters.pop_back();
            ++count;
            deaths++;
        }
        else
        ++survFIt, ++count; //go to next individual
    }
}

/* BECOME BREEDER */

Void group::breeder(vector<individual> &vfloaters)
{
   //Select a random sample from the floaters
       int i=0;
       int sumage=0;
       int currentposition=0; //age of the previous ind taken from candidates
       int uniformfloatnum;
       poisson_distribution<int> poissonfloat(avfloaterssample); //random sample size of floaters taken to compete for breeding spot
       uniform_real_distribution<float> uniformfloat(0, vfloaters.size()); //random floater id taken in the sample
       uniformfloatnum = uniformfloat(generator);
       uniform_real_distribution<double> randomposition;
       int randp = randomposition (generator);
       int randn = poissonfloat (generator);

       Vector<individual*> candidates(randn);
       vector<double>position; //vector of age to chose with higher likelihood the ind with higher age
       while (I < randn) ///change to a proportion instead
       {
           candidates[i] = &vfloaters[uniformfloatnum]; ///problem: it could pick the same ind several times
           i++;
       }
       cout << "breeder1" << endl;

   //Join the helpers in the group to the sample of floaters

       for (vector<individual>::iterator helpit = vhelpers.begin(); helpit < vhelpers.end(); ++helpit)
       {
           candidates.push_back(&(*helpit));
       }
       cout << "breeder2" << endl;

   //Choose breeder with higher likelihood for the highest age
       for (vector<individual*>::iterator ageit = candidates.begin(); ageit < candidates.end(); ++ageit) //ageit creates a vector of pointers to an individual
       {
           sumage += (*ageit)->age; //add all the age from the vector candidates
       }
       cout << "breeder3" << endl;
       for (vector<individual*>::iterator age2it = candidates.begin(); age2it < candidates.end(); ++age2it)
       {
           position.push_back((*age2it)->age / sumage + currentposition); //create a vector with proportional segments to the age of each individual
           currentposition = *position.end();
       }
       cout << "breeder4" << endl;
       for (vector<individual*>::iterator age3it = candidates.begin(); age3it < candidates.end(); ++age3it)
       {
           if (randp < position[age3it-candidates.begin()]) //to access the same ind in the candidates vector
           {
               vbreeder = **age3it; //substitute the previous dead breeder
               breederalive = 1;

               if ((*age3it)->own == floater) //delete the ind from the vector floaters
               {
                   **age3it = vfloaters[vfloaters.size() - 1];
                   vfloaters.pop_back();
               }
               else
               {
                   **age3it = vhelpers[vhelpers.size() - 1]; //delete the ind from the vector helpers
                   vhelpers.pop_back();
               }

               vbreeder.own = breeder; //modify the class
               age3it = candidates.end();//end loop
           }
       }
       cout << "breeder5" << endl;
} 


/*REASSIGN FLOATERS*/

void Reassign(vector<Individual> &vfloaters, vector<Group> &vgroups)
{
    uniform_real_distribution<float> UniformMaxCol(0, maxcolon);
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
    vbreeder.age++;
    cout << "Age breeder" << vbreeder.age << endl;
}


/* REPRODUCTION */

void Group::Fecundity()
{
    fecundity = K0 + cumhelp;
    cout << "Fecundity: " << fecundity << endl;
    poisson_distribution<int> PoissonFec(fecundity);
    realfecundity = PoissonFec(generator);
}

void Group::Reproduction() // populate offspring generation
{
    for (int i=0;i<realfecundity;i++) //number of offspring dependent on real fecundity
    {
        vhelpers.push_back(Individual(vbreeder)); //create a new individual as helper in the group. Call construct to assign the mother genetic values to the offspring, construct calls Mutate function.
    }
}

void Individual::Mutate() // mutate genome of offspring
{
    normal_distribution <double> NormalA(0, stepAlpha); ///could be simplified if I decide to have all the steps size with the same magnitude
    normal_distribution <double> NormalB(0, stepBeta);
    normal_distribution <double> NormalD(0, stepDrift);

    if (Uniform(generator)<mutAlpha)
        alpha += NormalA(generator);
        if (alpha < 0){alpha==0;}
 //       Uniform(generator)<0.5? Individual.alpha-=Uniform(generator)*stepAlpha : Individual.alpha+=Uniform(generator)*stepAlpha;

    if (Uniform(generator)<mutBeta)
        beta += NormalB(generator);
        if (beta < 0){beta==0;}
        if (beta > 1){beta==1;}
 //       Uniform(generator)<0.5? Individual.beta-=Uniform(generator)*stepBeta : Individual.beta+=Uniform(generator)*stepBeta;

    if (Uniform(generator)<mutDrift)
        drift += NormalD(generator);
 //       Uniform(generator)<0.5? Individual.drift-=Uniform(generator)*stepDrift : Individual.drift+=Uniform(generator)*stepDrift;
}


/* CALCULATE STATISTICS */
//void Statistics(vector<Group>vgroups){
//
//    double  meanAlpha,stdevAlpha,meanBeta,stdevBeta, meanDrift, stdevDrift, corr_AlphaBeta,
//        meanGroupsize, stdevGroupsize;         // population means, standard deviations and correlations
//
//
//	double alpha,beta,drift,sumAlpha=0.0,sumBeta=0.0,sumDrift=0.0, sumsqAlpha=0.0,sumsqBeta=0.0,sumsqDrift=0.0,
//        sumprodAlphaBeta=0.0,varAlpha,varBeta,varDrift;
//
//	for (int  i=0;i<vgroups.size;i++)
//	{
//		alpha=Individual[i].alpha;
//		beta=Individual[i].beta;
//		drift=Individual[i].drift;
//
//        sumAlpha+=alpha;
//		sumsqAlpha+=alpha*alpha;
//
//		sumBeta+=beta;
//		sumsqBeta+=beta*beta;
//
//		sumDrift+=drift;
//		sumsqDrift+=drift*drift;
//
//		sumprodAlphaBeta+=alpha*beta;
//
//   	}
//
//    meanAlpha=sumAlpha/double(vgroups.size);
//    meanBeta=sumBeta/double(vgroups.size);
//    meanDrift=sumDrift/double(vgroups.size); ///differentiate between within group and global
//
//	varAlpha=sumsqAlpha/double(vgroups.size)-meanAlpha*meanAlpha;
//	varBeta=sumsqBeta/double(vgroups.size)-meanBeta*meanBeta;
//	varDrift=sumsqDrift/double(vgroups.size)-meanDrift*meanDrift; ///differentiate between within group and global
//
//// to know if there is a problem, variance cannot be negative
//	varAlpha>0? stdevAlpha=sqrt(varAlpha):stdevAlpha=0;
//	varBeta>0? stdevBeta=sqrt(varBeta):stdevBeta=0;
//	varDrift>0? stdevDrift=sqrt(varDrift):stdevDrift=0; ///differentiate between within group and global
//
//	(stdevAlpha>0 && stdevBeta>0)? corr_AlphaBeta=(sumprodAlphaBeta/double(popsize)-meanAlpha*meanBeta)/(stdevAlpha*stdevBeta):corr_AlphaBeta=0; ///add correlations between alpha and beta to withing group relatedness
//
//
//}



/* WRITE PARAMETER SETTINGS TO OUTPUT FILE */
//void Printparams()
//{
//  fout << endl << "PARAMETER VALUES" << endl
//
//       << "initAlpha: " << "\t" << setprecision(4) << initAlpha << endl
//       << "initBeta: " << "\t" << setprecision(4) << initBeta << endl
//       << "initDrift: " << "\t" << setprecision(4) << initDrift << endl
//       << "mutAlpha: " << "\t" << setprecision(4) << mutAlpha << endl
//       << "stepAlpha: " << "\t" << setprecision(4) << stepAlpha << endl
//       << "mutBeta: " << "\t" << setprecision(4) << mutBeta << endl
//       << "stepBeta: " << "\t" << setprecision(4) << stepBeta << endl
//       << "mutDrift: " << "\t" << setprecision(4) << mutDrift << endl
//       << "stepDrift: " << "\t" << setprecision(4) << stepDrift << endl
//       << "m: " << "\t" << m << endl
//       << "K0: " << "\t" << K0 << endl
//       << "Xsh: " << "\t" << Xsh << endl
//       << "Xsr: " << "\t" << Xsr << endl
//       << "Xsn: " << "\t" << Xsn << endl
//       << "maxcolon: " << "\t" << maxcolon << endl
//       << "initN: " << "\t" << initN << endl
//       << "NumGen: " << "\t" << NumGen << endl;
//}



/* WRITE GENETIC TRAIT VALUES TO OUTPUT FILE */
//void WriteMeans()
//{
//
//  // show values on screen
//  cout << setw(6) << gen
//       << setw(9) << vgroups.size            ///correct??
//	   << setw(9) << setprecision(4) << meanAlpha
//	   << setw(9) << setprecision(4) << meanBeta
//	   << setw(9) << setprecision(4) << meanDrift
//	   << endl;
//
//
//  // write values to output file
//  fout << gen
//       << "\t" << vgroups.size
//	   << "\t" << setprecision(4) << meanAlpha
//	   << "\t" << setprecision(4) << meanBeta
//	   << "\t" << setprecision(4) << meanDrift
//	   << "\t" << setprecision(4) << stdevAlpha
//	   << "\t" << setprecision(4) << stdevBeta
//	   << "\t" << setprecision(4) << stdevDrift
//	   << "\t" << setprecision(4) << corr_AB
//	   << endl;
//}


/* MAIN PROGRAM */
int main()
{
    int gen=0;

    vector<Individual> vfloaters;
	vector<Group> vgroups (maxcolon);

	InitGroup(vgroups);

        int deaths=0; // to keep track of how many individuals die each generation

        for (vector<Group>::iterator popdispersal = vgroups.begin(); popdispersal < vgroups.end(); ++popdispersal)
        {
            popdispersal->Dispersal(vfloaters);
            cout << "Floaters after dispersal: " << vfloaters.size() << endl;
        }

        for (vector<Group>::iterator pophelpsurv = vgroups.begin(); pophelpsurv < vgroups.end(); ++pophelpsurv)
        {
            pophelpsurv->Help();
            pophelpsurv->SurvivalGroup(deaths);
        }

        SurvivalFloaters(vfloaters,deaths);
            cout << "dead + floaters: " << deaths << endl;

//        for (vector<Group>::iterator popbreeder = vgroups.begin(); popbreeder < vgroups.end(); ++popbreeder)
//        {
//            if (popbreeder->breederalive == 0)
//            {
//                //cout << vgroups.begin() - popbreeder << endl;
//                popbreeder->Breeder(vfloaters);
//            }
//        }

        cout << "Floaters before reassign: " << vfloaters.size() << endl;
        Reassign(vfloaters, vgroups);
        cout << "Floaters after reassign: " << vfloaters.size() << endl;

        for (vector<Group>::iterator popincreaseAge = vgroups.begin(); popincreaseAge < vgroups.end(); ++popincreaseAge)
        {
            popincreaseAge->increaseAge(); //add 1 rank or age to all individuals alive
        }

        for (vector<Group>::iterator popreprod = vgroups.begin(); popreprod < vgroups.end(); ++popreprod)
        {
            popreprod->Fecundity();
            popreprod->Reproduction();
        }





//    for(int rep=0;rep<numrep;rep++)
//    {
//
//    int gen=0; // generation zero
//
//
//    // column headings on screen
//    cout << setw(6) << "gen" << setw(9) << "popsize" << setw(9) << "alpha" << setw(9) << "beta" << setw(9) << "drift" << endl;
//
//	// column headings in output file
//	fout << "generation" << "\t" << "popsize" << "\t"  <<  "meanAlpha" << "\t" << "meanBeta" << "\t" << "meanDrift" << "\t"
//         << "stdevAlpha" << "\t" << "stdevBeta" << "\t" << "stdevDrift" << "\t" << "corr_AB" << "\t" << endl;
//
//
//	InitGroup(vector<Group> &vgroups)
//	Statistics();
//	WriteMeans();
//
//	for (gen=1;gen<=NumGen;gen++)
//	{
//
//		if (gen%skip==0){ Statistics(); WriteMeans();} // write output every 'skip' generations
//	}
//
//    fout << endl << endl << endl;
//
//    }
//    Printparams();

	return 0;
}

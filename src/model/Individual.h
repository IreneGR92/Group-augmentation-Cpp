
#ifndef GROUP_AUGMENTATION_INDIVIDUAL_H
#define GROUP_AUGMENTATION_INDIVIDUAL_H


#include "FishType.h"
#include "../Parameters.h"

class Individual {

    Individual(double alpha, double alphaAge, double beta, double betaAge, double drift, FishType fishType,
               std::default_random_engine &generator, Parameters &parameters, int &generation);

    Parameters parameters;

    std::default_random_engine generator;

    double alpha;
    double alphaAge;
    double beta;
    double betaAge;
    double drift;

    double dispersal;
    double help;
    double survival;

    FishType fishType;                                                // possible classes: breeder, helper, floater
    int age;
    bool inherit;                                                    //did the new breeder inherit the territory or did it disperse?

    void mutate(int generation);

public:
    Individual(Individual &individual, FishType fishType, Parameters &parameters, std::default_random_engine &generator,
               int &generation);

    Individual(double drift, FishType fishType, Parameters &parameters, std::default_random_engine &generator,
               int &generation);


    //Functions inside Individual
    double calcDispersal();
    void calcHelp();
    double calcSurvival(int groupSize);

    //Getters and setters
    double getAlpha() const;
    double getAlphaAge() const;
    double getBeta() const;
    double getBetaAge() const;
    double getDrift() const;

    double getDispersal() const;
    double getHelp() const;
    void setHelp(double help);
    double getSurvival() const;

    FishType getFishType() const;
    void setFishType(FishType fishType);

    int getAge() const;
    void increaseAge(bool alive); //TODO: for the breeder?
    void increaseAge(); // TODO: For helpers and floaters?

    bool isInherit() const;
    void setInherit(bool inherit);
};


#endif //GROUP_AUGMENTATION_INDIVIDUAL_H

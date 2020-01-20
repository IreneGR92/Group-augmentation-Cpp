
#ifndef GROUP_AUGMENTATION_INDIVIDUAL_H
#define GROUP_AUGMENTATION_INDIVIDUAL_H


#include "Classes.h"
#include "../Parameters.h"

class Individual {

    Parameters parameters;

    std::default_random_engine generator;

    double survival;
    double help;
    double dispersal;
    double drift;
    double betaAge;
    double beta;
    double alphaAge;
    double alpha;


    Classes fishType;                                                // possible classes: breeder, helper, floater
    int age;
    bool inherit;                                                    //did the new breeder inherit the territory or did it disperse?




    void mutate(int generation);

public:
    Individual(Individual &individual, Classes fishType, Parameters &parameters, std::default_random_engine &generator,
               int &generation);

    Individual(double drift_, Classes fishType_, Parameters &parameters, std::default_random_engine &generator,
               int &generation);

    void calculateHelp();

    //Functions inside Individual
    double calcDispersal();

    double calcSurvival(int groupSize);

    double getSurvival() const;

    double getHelp() const;

    double getDispersal() const;

    double getDrift() const;

    double getBetaAge() const;

    double getBeta() const;

    double getAlphaAge() const;

    double getAlpha() const;

    Classes getFishType() const;

    int getAge() const;

    bool isInherit() const;

    void setHelp(double help);

    void setInherit(bool inherit);

    void setFishType(Classes fishType);

    void increaseAge(bool alive);

    void increaseAge();
};


#endif //GROUP_AUGMENTATION_INDIVIDUAL_H


#include "Individual.h"
#include "Classes.h"

Individual::Individual(double alpha_, double alphaAge_, double beta_, double betaAge_, double drift_,
                       Classes fishType_) {
    alpha = alpha_;
    alphaAge = alphaAge_;
    beta = beta_;
    betaAge = betaAge_;
    drift = drift_;

    //TODO
    //    if (generation != 0) {
//        Mutate();
//    }
    fishType = fishType_;
    age = 1;
    inherit = true;

    //TODO
//    survival = NO_VALUE;
    help = 0;

    //TODO
//    dispersal = NO_VALUE;
}

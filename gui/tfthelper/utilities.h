#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>


namespace TFTHelper {

// Returns the Probability Density/mass Function array of the given binomial distribution described by input parameters:
// n  -  number of independent Bernoulli trials
// p  -  the probability of a single success
std::vector<double> BinomPDF(int n, double p);


// Returns the Probability Density/mass Function array of the given hypergeometric distribution described by input parameters:
// M  -  total number of objects in the pool
// n  -  number of objects drawn
// N  -  total number of objects in the pool counted by the random variable
// trimSize - the return vector size should be
std::vector<double> HyperGeoPDF(int M, int n, int N, int trimSize);

// Returns the Probability Density/mass Function array of "how many the cards you get" for given rolling times.
// lv - level of the little legend
// targetTier - the tier of your target card
// numRolling - number of rolling
// targetExist - the number of your target card that has been drawn
// sameTierExist - the number of cards in the same tier with the target that has been drawn(exclude "targetExist")
std::vector<double> GetCardDrawnPDFArray(int lv, int targetTier, int numRolling, int targetExist, int sameTierExist);

// Returns the Probability Density/mass Function array of the "rolling" you need for "roll until reach the min number target card" policy.
// lv - level of the little legend
// targetTier - the tier of your target card
// MinNum - the minium number of target card to stop rolling
// targetExist - the number of your target card that has been drawn
// sameTierExist - the number of cards in the same tier with the target that has been drawn(exclude "targetExist")
std::vector<double> GetStopTimePDFArray(int lv, int targetTier, int MinNum, int targetExist, int sameTierExist);


void PrintDistribution(std::vector<double> PDFArray);

void PrintRollingProbTable();

}




#endif // UTILITIES_H

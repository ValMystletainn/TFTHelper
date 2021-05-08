#include "utilities.h"


namespace TFTHelper {

const std::vector<std::vector<double>> probMatrix = {
     {1.0,  0,    0,    0,    0},
     {1.0,  0,    0,    0,    0},
     {0.75, 0.25, 0,    0,    0},
     {0.55, 0.30, 0.15, 0,    0},
     {0.45, 0.33, 0.20, 0.02, 0},
     {0.35, 0.35, 0.25, 0.05, 0},
     {0.19, 0.35, 0.30, 0.15, 0.01},
     {0.10, 0.25, 0.35, 0.25, 0.05},
     {0.10, 0.15, 0.30, 0.30, 0.15}
};

const int poolSize[] = { 29, 22, 18, 12, 10 };
const int tierSpeciesNum[] = { 13, 13, 13, 11, 8 };


// Returns the Probability Density/mass Function array of the given binomial distribution described by input parameters:
// n  -  number of independent Bernoulli trials
// p  -  the probability of a single success
std::vector<double> BinomPDF(int n, double p) {
    std::vector<double> pdf(n + 1, 0);

    // corner case
    if (p == 1) {
        pdf[n] = 1;
        return pdf;
    }

    double pi = 1;
    for (int i = 0; i < n; i++) {
        pi *= 1 - p;
    }
    for (int i = 0; i < n + 1; i++) {
        pdf[i] = pi;
        pi *= p / (1 - p) * (n - i) / (i + 1);
    }

    return pdf;
}

// Returns the Probability Density/mass Function array of the given hypergeometric distribution described by input parameters:
// M  -  total number of objects in the pool
// n  -  number of objects drawn
// N  -  total number of objects in the pool counted by the random variable
// trimSize - the return vector size should be
// !use logrithm to avoid overflow for getting each probability
std::vector<double> HyperGeoPDF(int M, int n, int N, int trimSize) {
    // p(k, M, n, N) = CNk * C(M -N, n-k) / C(M,n) = Cnk * C (M - n, N - k)  / C(M, N)

    int maxGetNum = n < N ? n : N;
    std::vector<double> pdf(trimSize, 0);
    double logck = 0;
    for (int i = 1; i < N + 1; i++) {
        logck += log(M - i + 1) - log(i); // count denominator with log to avoid overflow
    }
    logck = -logck;
    int kmin = 0 > N - M + n ? 0 : N - M + n;
    for (int i = 1; i <= N - kmin; i++) {
        logck += log(M - n - i + 1) - log(i); // count C(M - n, N - kmin)
    }

    for (int k = kmin; k < maxGetNum + 1; k++) {
        //pdf[k] = double((cnkNum / cnkDeNum) * (cMnNkNum / cMnNkDeNum)) / CMN;
        pdf[k] = exp(logck);
        logck += log(n - k) - log(k + 1) + log(N - k) - log(M - n - N + k + 1);
    }
    return pdf;
}


// Returns the Probability Density/mass Function array of "how many the cards you get" for given rolling times.
// lv - level of the little legend
// targetTier - the tier of your target card
// numRolling - number of rolling
// targetExist - the number of your target card that has been drawn
// sameTierExist - the number of cards in the same tier with the target that has been drawn(exclude "targetExist")
// ! It's a three stages process.
// Stage1, get pdf of #targetTier you get(variable 'n', binom distribution)
// Stage2, condition to fixed n, draw cards in the target pool, get pdf of #targetCard(conditional distribution, HyperGeo)
// Stage3, synthesize those conditional distribution by Law of total expectation / weighted sum
std::vector<double> GetCardDrawnPDFArray(int lv, int targetTier, int numRolling, int targetExist = 0, int sameTierExist = 0) {
    double singleDrawProb = probMatrix[lv - 1][targetTier - 1];
    auto tierDrawPDFArray = BinomPDF(numRolling * 5, singleDrawProb);

    int N = poolSize[targetTier - 1] - targetExist;
    int M = poolSize[targetTier - 1] * tierSpeciesNum[targetTier - 1] - targetExist - sameTierExist;
    int maxGetNum = numRolling * 5 < N ? numRolling * 5 : N;
    std::vector<double> targetDrawPDFArray(maxGetNum + 1, 0);
    int maxTierDraw = numRolling * 5 < M ? numRolling * 5 : M;
    maxTierDraw++;

    for (int n = 0; n < maxTierDraw; n++) {
        std::vector<double> targetDrawPDFArrayn = HyperGeoPDF(M, n, N, maxGetNum + 1);
        for (int i = 0; i < maxGetNum + 1; i++) {
            targetDrawPDFArray[i] += targetDrawPDFArrayn[i] * tierDrawPDFArray[n];
        }
    }

    return targetDrawPDFArray;
}

// Returns the Probability Density/mass Function array of the "rolling" you need for "roll until reach the min number target card" policy.
// lv - level of the little legend
// targetTier - the tier of your target card
// MinNum - the minium number of target card to stop rolling
// targetExist - the number of your target card that has been drawn
// sameTierExist - the number of cards in the same tier with the target that has been drawn(exclude "targetExist")
// Calculates by complementary of A."stop at after t rolling" and B. "rolling t times but get less than minNum" or "stop after 1,2,..,t-1 rolling"
std::vector<double> GetStopTimePDFArray(int lv, int targetTier, int MinNum, int targetExist = 0, int sameTierExist = 0) {
    std::vector<double> stopTimePDFArray(1, 0);

    double breakThreshold = 1e-2; // cut if the 1 - cumulative prob less than it
    int stopTime = 1;
    double PrSum = 0;
    double PrStopTime = 0;
    while(true) {
        auto targetDrawPDFArray = GetCardDrawnPDFArray(lv, targetTier, stopTime, targetExist, sameTierExist);
        PrStopTime = 1 - PrSum;
        for (int i = 0; (i < 5 * stopTime + 1) && (i < MinNum); ++i) {
            PrStopTime -= targetDrawPDFArray[i];
        }
        stopTimePDFArray.push_back(PrStopTime);

        if ((PrStopTime * stopTime < breakThreshold) && (1 - PrSum < breakThreshold / 100)) {
            break;
        }
        stopTime++;
        PrSum += PrStopTime;
    }
    return stopTimePDFArray;
}


void PrintDistribution(std::vector<double> PDFArray) {
    int n = PDFArray.size();
    double mean = 0; // sum up when print
    double std = 0;
    printf("num\tprob\n");
    bool isDescending = false;
    for (int i = 0; i < n; i++) {
        if (i == n - 1 || PDFArray[i + 1] < PDFArray[i]) {
            isDescending = true;
        }

        if (PDFArray[i] > 1e-4) {
            printf("%d  \t%.2f%%\n", i, PDFArray[i] * 100);
        }
        else if (isDescending) {
            // pass
        } else {
            printf("%d  \t%.2e\n", i, PDFArray[i]);
        }

        mean += i * PDFArray[i];
        std += i * i * PDFArray[i];
    }
    std -= mean * mean;
    std = sqrt(std);
    printf("mean: %.2f\n", mean);
    printf("std: %.2f\n", std);
    return;
}

void PrintRollingProbTable() {
    printf("Lv/Tier\t  1 \t  2 \t  3 \t  4 \t  5 \n");
    for (int lv = 2; lv <= 9; lv++) {
        printf("%d      \t", lv);
        for (int tier = 1; tier <= 5; tier++) {
            printf("%3d\t", int(probMatrix[lv - 1][tier - 1] * 100));
        }
        printf("\n");
    }
    printf("\n");
    printf("Poolsize");
    for (int tier = 1; tier <= 5; tier++) {
        printf("%3d\t", poolSize[tier - 1]);
    }
    printf("\n");
}

}



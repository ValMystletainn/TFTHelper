import sys
import numpy as np
from scipy.stats import binom
from scipy.stats import hypergeom


probMatrix = np.array(
[[1.0,  0,    0,    0,    0],
    [1.0,  0,    0,    0,    0],
    [0.75, 0.25, 0,    0,    0],
    [0.55, 0.30, 0.15, 0,    0],
    [0.45, 0.33, 0.20, 0.02, 0],
    [0.35, 0.35, 0.25, 0.05, 0],
    [0.19, 0.35, 0.30, 0.15, 0.01],
    [0.10, 0.25, 0.35, 0.25, 0.05],
    [0.10, 0.15, 0.30, 0.30, 0.15]]
)

poolSize = [29, 22, 18, 12, 10]
tierSpeciesNum = [13, 13, 13, 11, 8]



def GetCardDrawnPDFArray(Lv, targetTier, numRolling, targetExist = 0, sameTierExist = 0):
    '''
    Returns the Probability Density/mass Function array of "how many the cards you get" for given rolling times.
        lv - level of the little legend
        targetTier - the tier of your target card
        numRolling - number of rolling
        targetExist - the number of your target card that has been drawn
        sameTierExist - the number of cards in the same tier with the target that has been drawn(exclude "targetExist")
    ! It's a three stages process.
    Stage1, get pdf of #targetTier you get(variable 'n', binom distribution)
    Stage2, condition to fixed n, draw cards in the target pool, get pdf of #targetCard(conditional distribution, HyperGeo)
    Stage3, synthesize those conditional distribution by Law of total expectation / weighted sum
    '''
    singleDrawProb = probMatrix[Lv - 1, targetTier - 1]
    
    tierDrawPDFArray = binom.pmf(np.arange(0, numRolling * 5 + 1), numRolling * 5, singleDrawProb)


    N = poolSize[targetTier - 1] - targetExist
    M = poolSize[targetTier - 1] * tierSpeciesNum[targetTier - 1] - targetExist - sameTierExist

    maxGetNum = min(numRolling * 5, N)
    targetDrawPDFArray = np.zeros(maxGetNum + 1)
    for n in range(0, min(numRolling * 5, M) + 1):
        targetDrawPDFArrayn = hypergeom.pmf(np.arange(0, n + 1), M, n, N)
        if (targetDrawPDFArrayn.size == 1):
            targetDrawPDFArrayn = np.zeros(maxGetNum + 1)
            targetDrawPDFArrayn[0] = 1

        if (targetDrawPDFArrayn.size < maxGetNum + 1):
            targetDrawPDFArrayn = np.pad(targetDrawPDFArrayn, (0, maxGetNum - targetDrawPDFArrayn.size + 1), 'constant', constant_values = (0, 0))
        else:
            targetDrawPDFArrayn = targetDrawPDFArrayn[0 : maxGetNum + 1]

        targetDrawPDFArray += targetDrawPDFArrayn * tierDrawPDFArray[n]



    return targetDrawPDFArray
        

def GetStopTimePDFArray(Lv, targetTier, MinNum, targetExist = 0, sameTierExist = 0):
    '''
    Returns the Probability Density/mass Function array of the "rolling" you need for "roll until reach the min number target card" policy.
        lv - level of the little legend
        targetTier - the tier of your target card
        MinNum - the minium number of target card to stop rolling
        targetExist - the number of your target card that has been drawn
        sameTierExist - the number of cards in the same tier with the target that has been drawn(exclude "targetExist")
    Calculates by complementary of A."stop at after t rolling" and B. "rolling t times but get less than minNum" or "stop after 1,2,..,t-1 rolling"
    '''
    stopTimePDFArray = []
    stopTimePDFArray.append(0)

    breakThreshold = 1e-2
    stopTime = 1
    PrSum = 0
    while(True):
        targetDrawPDFArray = GetCardDrawnPDFArray(Lv, targetTier, stopTime, targetExist, sameTierExist)
        PrStopTime = 1 - PrSum - sum(targetDrawPDFArray[0:min(5 * stopTime + 1, MinNum)])
        stopTimePDFArray.append(PrStopTime)
        if PrStopTime * stopTime < breakThreshold and 1 - PrSum < breakThreshold / 100:
            break
        stopTime += 1
        PrSum += PrStopTime

    return np.array(stopTimePDFArray)


def PrintDistribution(PDFArray):
    getNumArray = np.arange(0, PDFArray.size)   
    mean = np.sum(getNumArray * PDFArray)
    std = np.sum(getNumArray ** 2 * PDFArray) - mean ** 2
    std = np.sqrt(std)
    probSum = 0
    print('num\tprob')
    for i in range(0, PDFArray.size):
        if (PDFArray[i] > 1e-4):
            print('%d  \t%.2f%%' %(i, PDFArray[i] * 100))
        else:
            print('%d  \t%.2e' %(i, PDFArray[i]))

        probSum += PDFArray[i]
        if (probSum > 1 - 1e-4):
            break

    print('mean: %.2f' %(mean))
    print('std: %.2f' %(std))


def PrintRollingProbTable():
    print('Lv/Tier\t  1 \t  2 \t  3 \t  4 \t  5 ')
    for lv in range(2, 10):
        print('%d      \t' %(lv), end='')
        for tier in range(1, 6):
            print('%3d\t' % (probMatrix[lv-1, tier - 1] * 100), end='')
        print('')
    
    print('')
    print('Poolsize', end='')
    for tier in range(1, 6):
        print('%3d\t' %(poolSize[tier - 1]), end='')

    print('')

if __name__ == '__main__':
    if(len(sys.argv) == 1):
        print('no more input arguments, please use -h or --help subcommand to get help')
        sys.exit(0)

    if (sys.argv[1] == '-h' or sys.argv[1] == '--help'):
        print('usage:\n1. tfthelper -r \t to show rolling prob table at each level\n2. tfthelper -d <Lv> <target tier> <numRolling> [#target drawn (Default 0)] [#other same tier cards drawn(Default 0)] \t return the distribution and statistics of the number of target you get(given rolling time)\n3.tfthelper -s <Lv> <target tier> <Count for Stopping> [#target drawn(Default 0)] [#other same tier cards drawn(Default 0)]\t return the distribution and statistics of the number of rolling(given the target counts of stopping)')
        sys.exit(0)

    if (sys.argv[1] == '-r' or sys.argv[1] == '--rtable'):
        PrintRollingProbTable()
        sys.exit(0)

    if (sys.argv[1] == '-d' or sys.argv[1] == '--drawout'):
        arg = [0, 0, 0, 0, 0]
        for i in range(len(sys.argv) - 2):
            arg[i] = int(sys.argv[i + 2])

        targetDrawPDFArray = GetCardDrawnPDFArray(arg[0], arg[1], arg[2], arg[3], arg[4])
        PrintDistribution(targetDrawPDFArray)
        sys.exit(0)

    if (sys.argv[1] == '-s' or sys.argv[1] == '--stoptime'):
        arg = [0, 0, 0, 0, 0]
        for i in range(len(sys.argv) - 2):
            arg[i] = int(sys.argv[i + 2])
        stoptimePDFArray = GetStopTimePDFArray(arg[0], arg[1], arg[2], arg[3], arg[4])
        PrintDistribution(stoptimePDFArray)
        sys.exit(0)


#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "angleCalculator.h"

using namespace std;

AngleCalculator::AngleCalculator(double new_lambda,
                                double new_a, double new_b, double new_c,
                                double new_alphaInDegrees, double new_betaInDegrees, double new_gammaInDegrees )
 : lambda(new_lambda),
   a(new_a), b(new_b), c(new_c),
   alphaInDegrees(new_alphaInDegrees),
   betaInDegrees(new_betaInDegrees),
   gammaInDegrees(new_gammaInDegrees),
   twoTetha(0.0),
   Xi(0.0),
   Fi(0.0)
{
    //empty
}

void AngleCalculator::buildPoints(double xLeft, double xRight,
                                 double yDown, double yUp,
                                 int resCount)
{
    coordsVector.resize(resCount);

    if (xLeft == 0) throw logic_error("XLeft can't be zero!");
    if (xRight == 0) throw logic_error("XRight can't be zero!");
    if (yUp == 0) throw logic_error("yUp can't be zero!");
    if (yDown == 0) throw logic_error("yDown can't be zero!");

    double kOfFirstLine = yUp / xLeft; //angle coefficient of first line (y = k*x + b)
    double kOfLastLine = yDown / xRight;
    double accuracy = (kOfFirstLine - kOfLastLine) / resCount;

    if (xLeft < accuracy) throw logic_error("XLeft OR resCount can't be such small");
    if (xRight < accuracy) throw logic_error("XRight OR resCount can't be such small");
    if (yUp < accuracy) throw logic_error("yUp OR resCount can't be such small");
    if (yDown < accuracy) throw logic_error("yDownOR resCount can't be such small");

    double k = kOfFirstLine;
    double kStep = accuracy;
    int counter = 0;

    while (k - kOfLastLine > accuracy)
    {
        coordsVector[counter] = calculatePairOfIntersectCoords(xLeft, xRight, yUp, yDown, k);
        k -= kStep;
        counter++;
    }

    //last pair of coords
    coordsVector[resCount-1].resize(4);
    coordsVector[resCount-1][xFirst] = xRight;
    coordsVector[resCount-1][yFirst] = yDown;
    coordsVector[resCount-1][xSecond] = xRight;
    coordsVector[resCount-1][ySecond] = yDown;
}

vector <double> &AngleCalculator::
    calculatePairOfIntersectCoords(double xLeft, double xRight,
                                   double yUp, double yDown,
                                   double k)
{
    static double okCoords[4][2];
    static vector< double > result(4);
    static int coordsCount;
    static double x, y;

    coordsCount = 0;

    /*
     1'st system
        y - k * x = 0
        y - yUp = 0
    */
    x = yUp / k;
    y = yUp;
    if (x >= xLeft && x <= xRight) {
        okCoords[coordsCount][xFirst] = x;
        okCoords[coordsCount][yFirst] = y;
        coordsCount++;
    }

    /*
     2'nd system
        y - k * x = 0
        y - yDown = 0
    */
    x = yDown / k;
    y = yDown;
    if (x >= xLeft && x <= xRight) {
        okCoords[coordsCount][xFirst] = x;
        okCoords[coordsCount][yFirst] = y;
        coordsCount++;
    }

    /*
     3'rd system
        y - k * x = 0
        x - xLeft = 0
    */
    x = xLeft;
    y = k * xLeft;
    if (y >= yDown && y <= yUp) {
        okCoords[coordsCount][xFirst] = x;
        okCoords[coordsCount][yFirst] = y;
        coordsCount++;
    }

    /*
     4'th system
        y - k * x = 0
        x - xRight = 0
    */
    x = xRight;
    y = k * xRight;
    if (y >= yDown && y <= yUp) {
        okCoords[coordsCount][xFirst] = x;
        okCoords[coordsCount][yFirst] = y;
        coordsCount++;
    }

    if (coordsCount > 3)
        throw logic_error("Line intersects 3 lines at the same time (only 2 allowed)");

    //fill vector
    result[xFirst] = okCoords[0][xFirst];
    result[yFirst] = okCoords[0][yFirst];
    result[xSecond] = okCoords[1][xFirst];
    result[ySecond] = okCoords[1][yFirst];
    return result;
}

void AngleCalculator::calculateResultingAngles(double H, double K, double L)
{
    //angles in direct space
    double alpha = alphaInDegrees * (M_PI / 180),
        beta = betaInDegrees * (M_PI / 180),
        gamma = gammaInDegrees * (M_PI / 180);
    double
        cosAlpha = cos(alpha),
        cosBeta = cos(beta),
        cosGamma = cos(gamma),
        sinAlpha = sin(alpha),
        sinBeta = sin(beta),
        sinGamma = sin(gamma);

    //volume of an unit cell
    double
        volume = (a * b * c) * sqrt( 1 - cosAlpha*cosAlpha - cosBeta*cosBeta - cosGamma*cosGamma
                               + 2 * cosAlpha * cosBeta * cosGamma );

    //a, b, c in reciprocal lattice (b1, b2, b3)
    double
        aR = (b * c * sinAlpha) / volume,
        bR = (c * a * sinBeta) / volume,
        cR = (a * b * sinGamma) / volume;

    //angles in reciprocal lattice
    double
        cosAlphaR = (cosBeta * cosGamma - cosAlpha) / (sinBeta * sinGamma),
        cosBetaR = (cosAlpha * cosGamma - cosBeta) / (sinAlpha * sinGamma),
        cosGammaR = (cosAlpha * cosBeta - cosGamma) / (sinAlpha * sinBeta);

    //d is interplanar distance
    //dd = 1 / d^2

    //triclinic formula for reciprocal interplanar length
    double
        dR = sqrt( (H*H)*(aR*aR) + (K*K)*(bR*bR) + (L*L)*(cR*cR) + (2*H)*(K*aR)*(bR*cosGammaR)
                                                           + (2*H)*(L*aR)*(cR*cosBetaR)
                                                           + (2*K)*(L*bR)*(cR*cosAlphaR) );
	double sinTetha = lambda * dR / 2;
	//strange rounding arror occures
	double maxSinTetha = 1.0,
			minSinTetha = -1.0;
	if (sinTetha > maxSinTetha)
		sinTetha = maxSinTetha;
	else if (sinTetha < minSinTetha)
		sinTetha = minSinTetha;

	twoTetha = 2 * (180 / M_PI) * asin(sinTetha);

    double NxS = L*H*aR*cR*cosBetaR + L*K*bR*cR*cosAlphaR + L*L*cR*cR,
        absN = dR,
        absS = L*cR;
	double cosXi = NxS / (absN * absS);
	Xi = (180 / M_PI) * acos(cosXi);


    //sinFi = |D| / (|F| * |C|)
    double coef_aR = K*L*H * cR*bR * cosAlphaR,
           coef_bR = L*K*K * cR*bR * cosAlphaR,
           coef_cR = L*K*K * bR*bR - K*L*H * aR*bR * cosGammaR;
    double absD = sqrt(  coef_aR*coef_aR * aR*aR
                       + coef_bR*coef_bR * bR*bR
                       + coef_cR*coef_cR * cR*cR
                       + 2 * coef_aR * coef_bR * aR * bR * cosGammaR
                       + 2 * coef_aR * coef_cR * aR * cR * cosBetaR
                       + 2 * coef_bR * coef_cR * bR * cR * cosAlphaR );
    double absF = sqrt(  L*L*H*H * (cR*cR*aR*aR - cR*cR*aR*aR*cosBetaR*cosBetaR)
                       + L*L*H*K * (cR*cR*aR*bR*cosGammaR - cR*bR*cosAlphaR*aR*cR*cosBetaR)
                       + L*L*H*K * (cR*cR*aR*bR*cosGammaR - cR*aR*cosBetaR*bR*cR*cosAlphaR)
                       + L*L*K*K * (cR*cR*bR*bR - cR*cR*bR*bR*cosAlphaR*cosAlphaR) );
	double absC = K * bR;
    double sinFi = absD / (absF * absC);
    Fi = (180 / M_PI) * asin(sinFi);
}

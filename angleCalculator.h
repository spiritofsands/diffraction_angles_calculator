#ifndef ANGLECALCULATOR_H
#define ANGLECALCULATOR_H

#include <vector>

using namespace std;

enum {xFirst, yFirst, xSecond, ySecond};
typedef vector< vector<double> > my2dVector;

class AngleCalculator
{
public:
    AngleCalculator(double lambda,
                    double a, double b, double c,
                    double alphaInDegrees, double betaInDegrees, double gammaInDegrees );

    void buildPoints(double xLeft, double xRight,
                     double yDown, double yUp,
                     int resCount);
    void calculateResultingAngles(double H, double K, double L);

    double getTwoTetha(){ return twoTetha; }
    double getXi(){ return Xi; }
    double getFi(){ return Fi; }
    my2dVector &getVectorOfCoords(){ return coordsVector; }

private:
    const double lambda;
    const double a, b, c;
    const double alphaInDegrees, betaInDegrees, gammaInDegrees;

    double twoTetha, Xi, Fi;
    my2dVector coordsVector;

    vector<double> &calculatePairOfIntersectCoords(double xLeft, double xRight,
                                                double yUp, double yDown,
                                                double k);
};

#endif // ANGLECALCULATOR_H


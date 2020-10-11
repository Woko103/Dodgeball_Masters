#include "FuzzySet.h"

class FuzzySet_Triangle : public FuzzySet
{
private:

    //Valores que definen la forma
    double m_dPeakPoint;
    double m_dLeftOffset;
    double m_dRightOffset;

public:
  
    FuzzySet_Triangle(double mid,
                        double lft,
                        double rgt):FuzzySet(mid), 
                                m_dPeakPoint(mid),
                                m_dLeftOffset(lft),
                                m_dRightOffset(rgt)
    {}

    //Calcula el grado de pertenencia de un valor concreto
    double CalculateDOM(double val)const;
};
#include "FuzzySet.h"

class FuzzySet_RightShoulder : public FuzzySet
{
private:

    //Valores que definen la forma
    double m_dPeakPoint;
    double m_dLeftOffset;
    double m_dRightOffset;

public:
  
    FuzzySet_RightShoulder(double peak,
                            double LeftOffset,
                            double RightOffset):
    
                    FuzzySet( ((peak + RightOffset) + peak) / 2),
                    m_dPeakPoint(peak),
                    m_dLeftOffset(LeftOffset),
                    m_dRightOffset(RightOffset)
    {}

    //Calcula el grado de pertenencia de un valor concreto
    double CalculateDOM(double val)const;
};
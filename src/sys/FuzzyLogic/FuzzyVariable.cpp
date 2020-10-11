#include "FuzzyVariable.h"
#include "FuzzyOperators.h"
#include "FuzzySet_Triangle.h"
#include "FuzzySet_LeftShoulder.h"
#include "FuzzySet_RightShoulder.h"
#include "FzSet.h"
#include <cassert>
#include <iostream>
#include <algorithm>

FuzzyVariable::~FuzzyVariable()
{
    MemberSets::iterator it;
    for (it = m_MemberSets.begin(); it != m_MemberSets.end(); ++it)
        delete it->second;
}

//Coge un crisp value y calcula su DOM
void FuzzyVariable::Fuzzify(double val)
{    
    //Comprobamos que este dentro del rango
    assert ( (val >= m_dMinRange) && (val <= m_dMaxRange) && 
            "<FuzzyVariable::Fuzzify>: value out of range");

    //Calculamos el DOM para cada set
    MemberSets::const_iterator curSet;
    for (curSet = m_MemberSets.begin(); curSet != m_MemberSets.end(); ++curSet)
        curSet->second->SetDOM(curSet->second->CalculateDOM(val));
}

//Defuzzifica la variable con el valor maximo
double FuzzyVariable::DeFuzzifyMaxAv()const
{
    double bottom = 0.0;
    double top    = 0.0;

    MemberSets::const_iterator curSet;
    for (curSet = m_MemberSets.begin(); curSet != m_MemberSets.end(); ++curSet)
    {
        bottom += curSet->second->GetDOM();
        top += curSet->second->GetRepresentativeVal() * curSet->second->GetDOM();
    }

    //Nos aseguramos de que bottom no es 0
    if (0==bottom) return 0.0;

    return top/bottom;   
}

FzSet FuzzyVariable::AddTriangularSet(std::string name,
                                      double       minBound,
                                      double       peak,
                                      double       maxBound)
{
    m_MemberSets[name] = new FuzzySet_Triangle(peak,
                                                peak-minBound,
                                                maxBound-peak);
    //Ajusta el rango si es necesario
    AdjustRangeToFit(minBound, maxBound);

    return FzSet(*m_MemberSets[name]);
}

FzSet FuzzyVariable::AddLeftShoulderSet(std::string name,
                                        double       minBound,
                                        double       peak,
                                        double       maxBound)
{
    m_MemberSets[name] = new FuzzySet_LeftShoulder(peak, peak-minBound, maxBound-peak);

    //Ajusta el rango si es necesario
    AdjustRangeToFit(minBound, maxBound);

    return FzSet(*m_MemberSets[name]);
}

FzSet FuzzyVariable::AddRightShoulderSet(std::string name,
                                         double       minBound,
                                         double       peak,
                                         double       maxBound)
{
    m_MemberSets[name] = new FuzzySet_RightShoulder(peak, peak-minBound, maxBound-peak);

    //Ajusta el rango si es necesario
    AdjustRangeToFit(minBound, maxBound);

    return FzSet(*m_MemberSets[name]);
}

//Ajusta los valores minimo y maximo
void FuzzyVariable::AdjustRangeToFit(double minBound, double maxBound)
{
    if (minBound < m_dMinRange) m_dMinRange = minBound;
    if (maxBound > m_dMaxRange) m_dMaxRange = maxBound;
}
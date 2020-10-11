#pragma once

#include <string>
#include <cassert>

class FuzzySet
{
protected:
  
    //Grado de pertenencia de un valor
    double m_dDOM;

    //Valor maximo (el pico)
    double m_dRepresentativeValue;

public:

    FuzzySet(double RepVal):m_dDOM(0.0), m_dRepresentativeValue(RepVal){}

    //Devuelve el grado de pertenencia del valor dado, sin asignarle a m_dDOM ese valor
    virtual double CalculateDOM(double val)const = 0;

    //El DOM pasa a ser el valor mas alto
    void ORwithDOM(double val){if (val > m_dDOM) m_dDOM = val;}

    double GetRepresentativeVal()const{return m_dRepresentativeValue;}
    void ClearDOM(){m_dDOM = 0.0;}  
    double GetDOM()const{return m_dDOM;}
    void SetDOM(double val)
    {
        assert ((val <=1) && (val >= 0) && "<FuzzySet::SetDOM>: invalid value");
        m_dDOM = val;
  }
};
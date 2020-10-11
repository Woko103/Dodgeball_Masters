#include "FuzzySet_Triangle.h"
#include "FuzzySet_RightShoulder.h"
#include "FuzzySet_LeftShoulder.h"

//--------------------------TRIANGLE-------------------------------
double FuzzySet_Triangle::CalculateDOM(double val)const
{
    //Comprobamos si algun valor es cero, para evitar dividir entre cero
    if ( ( (m_dRightOffset==0.0) && (m_dPeakPoint==val) ) ||
        ( (m_dLeftOffset==0.0) && (m_dPeakPoint==val) ) )
    {
        return 1.0;
    }

    //Si esta a la izq del centro sin salirse del rango
    if ( (val <= m_dPeakPoint) && (val >= (m_dPeakPoint - m_dLeftOffset)) )
    {
        double grad = 1.0 / m_dLeftOffset;
        return grad * (val - (m_dPeakPoint - m_dLeftOffset));
    }
    //Si esta a la der del centro sin salirse del rango
    else if ( (val > m_dPeakPoint) && (val < (m_dPeakPoint + m_dRightOffset)) )
    {
        double grad = 1.0 / -m_dRightOffset;
        return grad * (val - m_dPeakPoint) + 1.0;
    }
    //Esta fuera del rango
    else
        return 0.0;
}

//--------------------------RIGHTSHOULDER-------------------------------
double FuzzySet_RightShoulder::CalculateDOM(double val)const
{
    //Comprobamos si algun valor es cero, para evitar dividir entre cero
    if ( ( (m_dRightOffset==0.0) && (m_dPeakPoint==val) ) ||
        ( (m_dLeftOffset==0.0) && (m_dPeakPoint==val) ) )
    {
        return 1.0;
    }
    
    //Si esta a la izq del centro sin salirse del rango
    else if ( (val <= m_dPeakPoint) && (val > (m_dPeakPoint - m_dLeftOffset)) )
    {
        double grad = 1.0 / m_dLeftOffset;
        return grad * (val - (m_dPeakPoint - m_dLeftOffset));
    }
    //Si esta a la der del centro sin salirse del rango
    else if ( (val > m_dPeakPoint) && (val <= m_dPeakPoint+m_dRightOffset) )
        return 1.0;

    //Esta fuera del rango
    else
        return 0;
}

//--------------------------LEFTSHOULDER-------------------------------
double FuzzySet_LeftShoulder::CalculateDOM(double val)const
{
    //Comprobamos si algun valor es cero, para evitar dividir entre cero
    if ( ( (m_dRightOffset==0.0) && (m_dPeakPoint==val) ) ||
        ( (m_dLeftOffset==0.0) && (m_dPeakPoint==val) ) )
    {
        return 1.0;
    }

    //Si esta a la der del centro sin salirse del rango
    else if ( (val >= m_dPeakPoint) && (val < (m_dPeakPoint + m_dRightOffset)) )
    {
        double grad = 1.0 / -m_dRightOffset;
        return grad * (val - m_dPeakPoint) + 1.0;
    }

    //Si esta a la izq del centro sin salirse del rango
    else if ( (val < m_dPeakPoint) && (val >= m_dPeakPoint-m_dLeftOffset) )
        return 1.0;

    //Esta fuera del rango
    else
        return 0.0;
}
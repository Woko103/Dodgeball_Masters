#pragma once

#include "FuzzyTerm.h"
#include "FuzzySet.h"

class FzSet : public FuzzyTerm
{
private:

    //Referencia al fuzzy set que representa
    FuzzySet& m_Set;

public:

    FzSet(FuzzySet& fs):m_Set(fs){}

    FuzzyTerm* Clone()const{return new FzSet(*this);}
    double     GetDOM()const {return m_Set.GetDOM();}
    void       ClearDOM(){m_Set.ClearDOM();}
    void       ORwithDOM(double val){m_Set.ORwithDOM(val);}
};
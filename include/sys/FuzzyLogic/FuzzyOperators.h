#pragma once

#include <vector>
#include <cassert>
#include "FuzzyTerm.h"

class FzAND : public FuzzyTerm
{
private:

    //Este vector puede tener hasta 4 terms
    std::vector<FuzzyTerm*> m_Terms;

    //Desactivamos el constructor de copia
    FzAND& operator=(const FzAND&);

public:

    ~FzAND();

    FzAND(const FzAND& fa);
    
    //Constructores
    FzAND(FuzzyTerm& op1, FuzzyTerm& op2);
    FzAND(FuzzyTerm& op1, FuzzyTerm& op2, FuzzyTerm& op3);

    //Constructor virtual
    FuzzyTerm* Clone()const{return new FzAND(*this);}
    
    double GetDOM()const;
    void  ClearDOM();
    void  ORwithDOM(double val);
};

class FzOR : public FuzzyTerm
{
private:

    //Este vector puede tener hasta 4 terms
    std::vector<FuzzyTerm*> m_Terms;

    //Desactivamos el constructor de copia
    FzOR& operator=(const FzOR&);

public:

    ~FzOR();

    //Constructor de copia
    FzOR(const FzOR& fa);
    
    //Constructores
    FzOR(FuzzyTerm& op1, FuzzyTerm& op2);
    FzOR(FuzzyTerm& op1, FuzzyTerm& op2, FuzzyTerm& op3);

    //Constructor virtual
    FuzzyTerm* Clone()const{return new FzOR(*this);}
    
    double GetDOM()const;

    //unused
    void ClearDOM(){assert(0 && "<FzOR::ClearDOM>: invalid context");}
    void ORwithDOM(double val){assert(0 && "<FzOR::ORwithDOM>: invalid context");}
};
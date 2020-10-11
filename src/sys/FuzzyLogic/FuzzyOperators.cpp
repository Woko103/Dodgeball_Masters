#include "FuzzyOperators.h"

//------------------------------FZAND--------------------------------------
FzAND::FzAND(const FzAND& fa)
{
    std::vector<FuzzyTerm*>::const_iterator curTerm;
    for (curTerm = fa.m_Terms.begin(); curTerm != fa.m_Terms.end(); ++curTerm)
        m_Terms.push_back((*curTerm)->Clone());
}
   
FzAND::FzAND(FuzzyTerm& op1, FuzzyTerm& op2)
{
    m_Terms.push_back(op1.Clone());
    m_Terms.push_back(op2.Clone());
}

FzAND::FzAND(FuzzyTerm& op1, FuzzyTerm& op2, FuzzyTerm& op3)
{
    m_Terms.push_back(op1.Clone());
    m_Terms.push_back(op2.Clone());
    m_Terms.push_back(op3.Clone());
}

FzAND::~FzAND()
{
    std::vector<FuzzyTerm*>::iterator curTerm;
    for (curTerm = m_Terms.begin(); curTerm != m_Terms.end(); ++curTerm)
        delete *curTerm;
}

//El AND devuelve el minimo DOM del set
double FzAND::GetDOM()const
{
    double smallest;

    std::vector<FuzzyTerm*>::const_iterator curTerm;
    for (curTerm = m_Terms.begin(); curTerm != m_Terms.end(); ++curTerm)
    {
        if (curTerm==m_Terms.begin())
            smallest = (*curTerm)->GetDOM();
        else if ((*curTerm)->GetDOM() < smallest)
            smallest = (*curTerm)->GetDOM();
    }
    return smallest;
}

void FzAND::ORwithDOM(double val)
{
    std::vector<FuzzyTerm*>::iterator curTerm;
    for (curTerm = m_Terms.begin(); curTerm != m_Terms.end(); ++curTerm)
        (*curTerm)->ORwithDOM(val);
}

void FzAND::ClearDOM()
{
    std::vector<FuzzyTerm*>::iterator curTerm;
    for (curTerm = m_Terms.begin(); curTerm != m_Terms.end(); ++curTerm)
        (*curTerm)->ClearDOM();
}

//------------------------------FZOR--------------------------------------
FzOR::FzOR(const FzOR& fa)
{
    std::vector<FuzzyTerm*>::const_iterator curTerm;
    for (curTerm = fa.m_Terms.begin(); curTerm != fa.m_Terms.end(); ++curTerm)
        m_Terms.push_back((*curTerm)->Clone());
}

FzOR::FzOR(FuzzyTerm& op1, FuzzyTerm& op2)
{
    m_Terms.push_back(op1.Clone());
    m_Terms.push_back(op2.Clone());
}

FzOR::FzOR(FuzzyTerm& op1, FuzzyTerm& op2, FuzzyTerm& op3)
{
    m_Terms.push_back(op1.Clone());
    m_Terms.push_back(op2.Clone());
    m_Terms.push_back(op3.Clone());
}

FzOR::~FzOR()
{
    std::vector<FuzzyTerm*>::iterator curTerm;
    for (curTerm = m_Terms.begin(); curTerm != m_Terms.end(); ++curTerm)
        delete *curTerm;
}

//El OR devuelve el maximo DOM del set
double FzOR::GetDOM()const
{
    double largest;

    std::vector<FuzzyTerm*>::const_iterator curTerm;
    for (curTerm = m_Terms.begin(); curTerm != m_Terms.end(); ++curTerm)
    {
        if (curTerm==m_Terms.begin())
            largest = (*curTerm)->GetDOM();
        else if ((*curTerm)->GetDOM() > largest)
            largest = (*curTerm)->GetDOM();
    }
    return largest;
}

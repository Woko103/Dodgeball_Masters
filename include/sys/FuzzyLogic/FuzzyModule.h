#pragma once

#include <vector>
#include <string>
#include <map>
#include <iosfwd>

#include "FuzzySet.h"
#include "FuzzyVariable.h"
#include "FuzzyRule.h"
#include "FuzzyOperators.h"
#include "FzSet.h"

class FuzzyModule
{
private:

    typedef std::map<std::string, FuzzyVariable*> VarMap;

public:

    //Uno de estos valores se le pasa al defuzzificador
    enum DefuzzifyType{max_av, centroid};

    //Se usa para calcular el centroid
    enum {NumSamples = 15};

private:

    //Mapa con las fuzzy variables que usa este modulo
    VarMap m_Variables;

    //Contiene todas las reglas
    std::vector<FuzzyRule*> m_Rules;

    //zeros the DOMs of the consequents of each rule. Used by Defuzzify()
    inline void SetConfidencesOfConsequentsToZero();

public:

    ~FuzzyModule();

    //Crea una fuzzy variable vacia y devuelve una referencia
    FuzzyVariable& CreateFLV(const std::string& VarName);

    //Anyade una regla al modulo
    void AddRule(FuzzyTerm& antecedent, FuzzyTerm& consequence);

    //Fuzzificar 
    inline void Fuzzify(const std::string& NameOfFLV, double val);

    //Defuzzifica (crisp value)
    inline double DeFuzzify(const std::string& key);
};

//-----------------------------FUZZIFY------------------------------------
inline void FuzzyModule::Fuzzify(const std::string& NameOfFLV, double val)
{
    //Comprobamos que existe
    assert ( (m_Variables.find(NameOfFLV) != m_Variables.end()) &&
          "<FuzzyModule::Fuzzify>:key not found");

    m_Variables[NameOfFLV]->Fuzzify(val);
}

//-----------------------------DEFUZZIFY------------------------------------
inline double
FuzzyModule::DeFuzzify(const std::string& NameOfFLV)
{
    //Comprobamos que existe
    assert ( (m_Variables.find(NameOfFLV) != m_Variables.end()) &&
            "<FuzzyModule::DeFuzzifyMaxAv>:key not found");

    //clear the DOMs of all the consequents of all the rules
    SetConfidencesOfConsequentsToZero();

    //Calculamos la regla
    std::vector<FuzzyRule*>::iterator curRule = m_Rules.begin();
    for (curRule; curRule != m_Rules.end(); ++curRule)
        (*curRule)->Calculate();

    //Defuzzificamos el resultado
    return m_Variables[NameOfFLV]->DeFuzzifyMaxAv();

    return 0;
}

//-----------------------------METHOD------------------------------------
inline void FuzzyModule::SetConfidencesOfConsequentsToZero()
{
    std::vector<FuzzyRule*>::iterator curRule = m_Rules.begin();
    for (curRule; curRule != m_Rules.end(); ++curRule)
        (*curRule)->SetConfidenceOfConsequentToZero();
}
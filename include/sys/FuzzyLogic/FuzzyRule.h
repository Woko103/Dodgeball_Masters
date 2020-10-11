#include <vector>
#include "FuzzySet.h"
#include "FuzzyOperators.h"

class FuzzyRule
{ 
private:

    //antecedent (usually a composite of several fuzzy sets and operators)
    const FuzzyTerm* m_pAntecedent;

    //consequence (usually a single fuzzy set, but can be several ANDed together)
    FuzzyTerm* m_pConsequence;

    //Prohibimos que se copien reglas haciendolo privado
    FuzzyRule(const FuzzyRule&);
    FuzzyRule& operator=(const FuzzyRule&);

public:

    FuzzyRule(const FuzzyTerm& ant,
              const FuzzyTerm& con):m_pAntecedent(ant.Clone()),
                                    m_pConsequence(con.Clone())
    {}

    ~FuzzyRule(){delete m_pAntecedent; delete m_pConsequence;}

    void SetConfidenceOfConsequentToZero(){m_pConsequence->ClearDOM();}

    //Actualiza el DOM del consiguiente con el DOM del antecedente
    void Calculate(){ m_pConsequence->ORwithDOM(m_pAntecedent->GetDOM()); }
};
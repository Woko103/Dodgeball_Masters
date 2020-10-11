#include <map>
#include <iosfwd>
#include <string>

class FuzzySet;
class FzSet;
class FuzzyModule;

class FuzzyVariable
{
private:
  
    typedef std::map<std::string, FuzzySet*>  MemberSets;

private:

    //Prohibimos copias
    FuzzyVariable(const FuzzyVariable&);
    FuzzyVariable& operator=(const FuzzyVariable&);

private:

    //Mapa con los fuzzy sets que comprenden esta variable
    MemberSets m_MemberSets;

    //El minimo y el maximo valor del rango de esta variable
    double m_dMinRange;
    double m_dMaxRange;
    
    //Actualiza los valores minimo y maximo
    void AdjustRangeToFit(double min, double max);

    //El destructor se hace privado para evitar que se destruya
    ~FuzzyVariable();

    friend class FuzzyModule;

public:

    FuzzyVariable():m_dMinRange(0.0),m_dMaxRange(0.0){}
    
    //Metodos para anyadir sets. Actualizan los valores m_dMinRange y m_dMaxRange
    FzSet AddLeftShoulderSet(std::string name, double minBound, double peak, double maxBound);
    FzSet AddRightShoulderSet(std::string name, double minBound, double peak, double maxBound);
    FzSet AddTriangularSet(std::string name, double minBound, double peak, double maxBound);
    
    //Fuzzifica un valor calculando su DOM en cada una de sus variables
    void Fuzzify(double val);

    //Defuzzifica un valor con el valor maximo
    double DeFuzzifyMaxAv()const;
};
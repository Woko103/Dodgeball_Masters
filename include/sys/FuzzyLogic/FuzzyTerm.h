#pragma once

class FuzzyTerm //Clase abstracta para las reglas
{  
public:

    virtual ~FuzzyTerm(){}

    //Constructor virtual
    virtual FuzzyTerm* Clone()const = 0;

    //Devuelve el DOM
    virtual double      GetDOM()const=0;

    //DOM = 0
    virtual void       ClearDOM()=0;

    //Actualiza el DOM
    virtual void       ORwithDOM(double val)=0;
};
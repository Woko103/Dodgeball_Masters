#pragma once

#include <DecisionTree.h>
#include <FuzzyModule.h>
#include <mechanics.h>
#include <facade2.h>

using namespace std;

class IA_system {
    private:
        //Variables del arbol de decision
        unique_ptr<IntDecision> rootDefensivo;
        unique_ptr<IntDecision> rootOfensivo;
        unique_ptr<IntDecision> rootLanzador;

        //Variables de la logica difusa en el caso de tener el balon
        FuzzyModule fmLanzar; //Distancia al rival y potencia propia (defensa rival)
        FuzzyModule fmPasar; //Potencia propia y potencia companero (posicion companero, posicion propia, posicion rival)
                             //(Estaria bien que se ejecute si estas lejos de la posicion de tiro o si tienes poca potencia o si la deseabilidad de tirar es baja)
                             //(Tambien podria pasarsela a quien tenga mayor Deseabilidad de lanzar)
                             //Si la deseabilidad para tirar es baja, es porque tu potencia es baja y los rivales ya se han aalejado. En ese caso lo mejor es pasarla
        //FuzzyModule fmAmagar;

        //Variables de la logica difusa en el caso de estar defendiendo
        FuzzyModule fmEsquivar; //Velocidad pelota y esquive propio (distancia pelota)
        FuzzyModule fmAgarrar; //Velocidad pelota y agarre propio (distancia pelota)
        //FuzzyModule fmProvocar; //Potencia rival y defensa propia

        Facade2* facade; // Reference to the facade
        Mechanics* mechanics; // Reference to the mechanics
        Entity_Manager* entity_manager; // Reference to the entity manager

        std::vector<IEntity*> targets;

    public:
        void initializeIA(Entity_Manager*, Facade2*,  Mechanics*);
        void updateIA();

        //Metodos para crear la estructura de los arboles que usaremos
        void createDecisionTreeDefensivo();
        void createDecisionTreeOfensivo();
        void createDecisionTreeLanzador();

        //Metodos de logica difusa para cuando la IA tiene el balon
        void createFuzzyLanzar();
        double updateFuzzyLanzar(std::map< const char*, unique_ptr<IEntity>, cmp_str >& _entities, IEntity*);
        void createFuzzyPasar();
        double updateFuzzyPasar(std::map< const char*, unique_ptr<IEntity>, cmp_str >& _entities, IEntity*);

        //Metodos de logica difusa para cuando la IA defiende
        void createFuzzyEsquivar();
        double updateFuzzyEsquivar(std::map< const char*, unique_ptr<IEntity>, cmp_str >& entities, IEntity*, IEntity*);
        void createFuzzyAgarrar();
        double updateFuzzyAgarrar(std::map< const char*, unique_ptr<IEntity>, cmp_str >& entities, IEntity*, IEntity*);
};

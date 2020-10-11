#pragma once

#include <entity_manager.h>
#include <memory>
#include <iostream>

using namespace std;

class DecisionTreeNode {
    public:
        virtual ~DecisionTreeNode() = default;
        virtual DecisionTreeNode *makeDecision(IEntity* entity)=0;
};

class Action : public DecisionTreeNode {
    private:
        uint32_t accion; //Valor de 0 a x, siendo x igual al numero de acciones totales, equivalente a la posicion en el vector de acciones de la accion en cuestion
    public:
        Action(uint32_t _accion);
        DecisionTreeNode *makeDecision(IEntity* entity) { return this; }
        uint32_t getAccion() { return accion; }
};

class Decision : public DecisionTreeNode {
    public:
        unique_ptr<DecisionTreeNode> trueNode;
        unique_ptr<DecisionTreeNode> falseNode;
        virtual DecisionTreeNode *getBranch(IEntity* entity)=0;
        DecisionTreeNode *makeDecision(IEntity* entity); //Devuelve la accion final
};

class IntDecision : public Decision {
    private:
        int value;
    public:
        IntDecision(int _value);
        DecisionTreeNode *getBranch(IEntity* entity); //Decide el nodo
};

class FloatDecision : public Decision {
    private:
        float value;
    public:
        FloatDecision(float _value);
        DecisionTreeNode *getBranch(IEntity* entity); //Decide el nodo
};
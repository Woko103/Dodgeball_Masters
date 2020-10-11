#include <DecisionTree.h>

Action::Action(uint32_t _accion){
    accion = _accion;
}

IntDecision::IntDecision(int _value){
    value = _value;
}

FloatDecision::FloatDecision(float _value){
    value = _value;
}

DecisionTreeNode* Decision::makeDecision(IEntity* entity){
        return getBranch(entity)->makeDecision(entity);
}

DecisionTreeNode* IntDecision::getBranch(IEntity* entity){
    if (value>=10)
        return trueNode.get();
    else
        return falseNode.get();
}

DecisionTreeNode* FloatDecision::getBranch(IEntity* entity){
    if (value>=entity->position.X)
        return trueNode.get();
    else
        return falseNode.get();
}
#include<BehaviorTree.h>

bool Sequence::run()
{
    for (auto tarea = tareas.begin(); tarea != tareas.end(); tarea++)
    {
        if (!(*tarea)->run()) 
            return false;
    }
    return true;
}

bool Selector::run()
{
    for (auto tarea = tareas.begin(); tarea != tareas.end(); tarea++)
    {
        if ((*tarea)->run()) 
            return true;
    }

    return false;
}

bool FuzzySelector::run()
{
    for (auto tarea = tareas.begin(); tarea != tareas.end(); tarea++)
    {
        if ((*tarea)->run()) 
            return true;
    }

    return false;
}
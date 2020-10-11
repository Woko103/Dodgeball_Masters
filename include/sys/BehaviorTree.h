#pragma once

#include <vector>

class Task
{
protected:
    std::vector<Task*> tareas;

public:
    void addTask(Task* tarea) { tareas.push_back(tarea); }
    //void removeTask(Task*);
    void clearTasks() { tareas.clear(); };
    virtual bool run()=0;
};

class Selector : public Task
{
public:
    bool run();
};

class FuzzySelector : public Task
{
public:
    bool run();
};

class Sequence : public Task
{
public:
    bool run();
};
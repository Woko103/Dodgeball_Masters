#pragma once

#include <map>
#include <MyModel.h>

class Resource_Manager
{
/*
    Stocking class for the needed resources of the game.
*/

private:
    // Map that keeps references to all the models (and textures) of the game in case they need to be reused
    std::map< const char*, MyModel* > models;

public:
    // Constructor
    explicit Resource_Manager() = default;

    // Management methods
    bool modelExists(const char* name); // Check if a model already exits in the map
    MyModel* getModel(const char* name); // Gets a model from the map
    void addModel(const char* name, MyModel* model); // Adds a new model to the map
    void removeModel(const char* name); // Removes a model from the map
    void removeMapModel();
    void clearResources();
    void showModels();
};


#include <resource_manager.h>

// Management methods
bool Resource_Manager::modelExists(const char* name)
{
  for(auto itr = models.begin(); itr != models.end(); itr++)
    {
        if(strcmp(itr->first, name) == 0)
            return true;
    }

    return false;
}

MyModel* Resource_Manager::getModel(const char* name)
{
    for(auto itr = models.begin(); itr != models.end(); itr++)
    {
        if(strcmp(itr->first, name) == 0)
            return itr->second;
    }
}

void Resource_Manager::addModel(const char* name, MyModel* model)
{
    models[name] = model;
}

void Resource_Manager::removeModel(const char* name)
{
    for(auto itr = models.begin(); itr != models.end(); itr++)
    {
        if(strcmp(itr->first, name) == 0)
        {
            // return itr->second;
        }
    }
}

void Resource_Manager::removeMapModel(){
    for(auto itr = models.begin(); itr != models.end(); itr++)
    {
        if(strcmp(itr->first, "Luna") == 0 || strcmp(itr->first, "Estadio") == 0 || strcmp(itr->first, "Jungla") == 0)
        {
            delete itr->second;
            models.erase(itr->first);
        }
    }
}

void Resource_Manager::clearResources(){
  for(auto itr = models.begin(); itr != models.end(); itr++){
    delete itr->second;
  }
  models.clear();
}

void Resource_Manager::showModels()
{
    /*Creamos un iterador para ir iterando por las entidades
            y mostrando del tipo que son */

    for(auto itr = models.begin(); itr != models.end(); itr++)
    {
        //cout << "Key(name): " << itr->first << endl;
    }
}

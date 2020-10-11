#pragma once

#include <map>
#include <ientity.h>
#include <iscore.h>
#include <icharacter.h>
#include <imap.h>
#include <iguitexture.h>
#include <memory>
#include <iostream>

using namespace std;

struct cmp_str
{
   bool operator()(char const *a, char const *b) const
   {
      return std::strcmp(a, b) < 0;
   }
};

class Entity_Manager
{
  private:
        std::map< const char*, unique_ptr<IEntity>, cmp_str > entities;
        unique_ptr<IScore> score;

        std::map< const char*, unique_ptr<ICharacter> > characters;
        std::map< const char*, unique_ptr<IMap> > maps;
        IMap* selectedMap;

        std::map< const char*, unique_ptr<IGuiTexture> > guiTextures;
  public:

        ICharacter* selectedCharacters[3];
        ICharacter* selectedEnemyCharacters[3];
        std::string selectedIA;
        int teamMembers = 0;

        void addEntity(const char* name, const char* characterID, const char* meshPath,
                    float X, float Y, float Z, std::string team, bool esPlayer, bool esJugador, bool esMultiplayer,
                    bool esPelota, bool esPowerUp, float initialStrength, float dash, char *eventDesc, bool hasParameter, float hability,
                    float grabRange, char status, float speed, int stamina, std::string difficulty, int superThrowType);
        void removeEntity(const char* name);
        void removeAllEntities();
        void initializeEntities(uint16_t multiplayerID);
        void setTarget();
        void showEntities();
        void showCharacters();
        void initializeScore();
        void initializeCharacters();
        void addCharacter(const char* name, const char* meshPath, const char* imagePath,
                    float X, float Y, float Z, std::string team, bool esPlayer, bool esJugador,
                    float initialStrength, float dash, char *eventDesc, bool hasParameter, float hability,
                    float grabRange, float speed, int stamina, int superThrowType, bool selected);
        float randomPos();
        void initializeMaps();
        void addMap(const char* name, const char* meshPath, const char* imagePath,
                    char *eventDesc, bool hasParameter, bool selected);

        unique_ptr<IEntity>& getEntity(const char* id);
        unique_ptr<IEntity>& getEntityByCharacter(const char* name);
        unique_ptr<IEntity>& getEntityByName(const char* name);
        bool entityExist(const char* name);
        bool entityExistsByCharacter(const char* name);
        bool entityExistsMultiplayer(const char* name);
        std::map< const char*, unique_ptr<IEntity>, cmp_str >& getMapEntities();
        std::map< const char*, unique_ptr<ICharacter> >& getMapCharacters();
        unique_ptr<IScore>& getScore();
        void resetMatchVars();
        //void addComponent(C component);

      /* CHARACTER MEHTODS */
        unique_ptr<ICharacter>& getCharacter(const char* name);
        bool insertSelectedCharacter(int index, ICharacter* character);
        bool removeSelectedCharacter(ICharacter* character);
        int selectedCharactersLength();
        bool selectedCharactersInOrder();
        int selectedEnemiesLength();
        int balanceTeams();
        void selectEnemyCharacters();
        void insertEnemyCharacters(std::vector<std::string> enemiesID);

      /* MAP METHODS */
        unique_ptr<IMap>& getMap(const char* name);
        void insertSelectedMap(IMap* map);
        const char* getSelectedMapID();
        const char* getSelectedMapText();


      /* GUI TEXTURE METHODS */
      std::map< const char*, unique_ptr<IGuiTexture> >& getMapGuiTextures();
      void addGuiTexture(const char* name, const char* texturePath);
      void initializeGuiTextures();
      unique_ptr<IGuiTexture>& getGuiTexture(const char* name);
};

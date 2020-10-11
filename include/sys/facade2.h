#pragma once

#include <MyGraphicsEngine.h>
#include <resource_manager.h>
#include <entity_manager.h>
#include <iostream>
#include <memory>
// imgui h
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class Facade2
{
    private:
        // Graphics engine interface declaration
        unique_ptr<MyGraphicsEngine> graphicsEngine;

        // Graphics engine resource manager declaration
        unique_ptr<Resource_Manager> resourceManager;

        // Root scene node declaration
        unique_ptr<MyNode> rootNode;

        //Tiempo para los timers
        float gameTime = 0.0f;

        unsigned int diffuseMap;  // TODO Declarar aqui el vector de recursos

        //Multiplayer variables
        std::string inputs;
        int multiplayerID = 0;
        std::string multiplayerPowerup;
        std::string multiplayerMuerto = "";

        unique_ptr<MyCamera> camera;
        unique_ptr<MyLight> pointLight;
        Entity_Manager* entity_manager;
        void updateEntitiesPosition(Entity_Manager*);

        // SELECTION CHARACTER ROTATION INDEX
        int characterRotationIndex = 0;
        bool mapWasSelected = false;
        bool IAWasSelected = false;

        bool once = false;


    public:
        explicit Facade2 () = default;

        const char* elimination();

        const char* checkInput();
        char checkInputPlayer2();

        // Interface mehtods
        void InitGUIElements();
        int renderMainMenu();
        int renderCharacterSelectMenu();
        int renderMultiplayerCharacterSelectMenu();
        int renderMapSelectMenu();
        int renderCredits();
        int renderControls();
        int renderHUD();
        void renderLoadingScreen(int num);
        int renderPauseMenu();
        int renderEndingScreen();
        void resetGame();

        // Collisions methods
        void initColliders();
        void updateEntitiesCollisions();
        void addCubeColliderToEntity(char*, MyNode*, float, float, float, float, float, float);
        bool compareCollision(IEntity*, IEntity*);
        bool isEntityColliding(IEntity*);
        char* getCollisionName(IEntity*);

        bool compareParent(std::map< const char*, unique_ptr<IEntity> >&, IEntity*);
        bool compareParent(IEntity*, IEntity*);
        bool checkShutDown();
        bool checkStart1();
        bool checkStart2();
        int checkMando();
        bool joystickMoving();
        bool keyboardMoving();
        const unsigned char buttons();
        const float triggers();
        const float getYAxisRJ();
        const float getXAxisRJ();
        const float getCursorX();
        const float getCursorY();

        double getTime(double);
        void setTime(double);

        //Multiplayer methods
        void clearInputs();
        std::string getInputs();
        void setMultiplayerID(int id);
        int getMultiplayerID();
        std::string getMultiplayerPowerup();
        void setMultiplayerMuerto(std::string name);
        std::string getMultiplayerMuerto();

        void startGraphics(Entity_Manager*);
        void startHUD(Entity_Manager*);
        void initDevice(Entity_Manager*);
        void renderEntities();
        void updateGraphics(Entity_Manager*);
        void updateMenu();
        void updatePlayerMovement(IEntity* player);
        void updatePlayerMovement2(IEntity* player);
        void updatePlayerMovementJoystick(IEntity* player);
        void deleteCharacter(Entity_Manager*, const char*);
        void deleteAllCharacters();
        void createChild(IEntity*, IEntity*);
        void removeChild(IEntity*, IEntity*);
        void setPosition(IEntity*, vec3);
        void renderEnt(IEntity*);
        void setAnimator(IEntity*);
        void removeAnimator(IEntity*);
        void setRotation(IEntity*, vec3);
        void setAnim(IEntity*, int);
        void shutDown();
};

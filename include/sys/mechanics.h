#pragma once

#include <boost/algorithm/string.hpp>
#include <facade2.h>
#include <sound_system.h>

class Mechanics
{
    private:
        IEntity* lastPlayer = nullptr;
        bool missBall;
        bool pick;
        bool inAir;
        bool pass;
        bool fromPlayer;
        bool checker=true;
        bool playerIsDead;
        bool player2IsDead;
        float speedX;
        float speedZ;
        float multiplayerSpeedX;
        float multiplayerSpeedZ;
        double airTime;
        double missTime;
        double dropTime;
        double changeTime;
        double spawnTime;
        double launchTime;
        double angle;
        char direction;
        int step;


        Entity_Manager* entity_manager; // Reference to entity manager
        Facade2* facade; // Reference to facade
        Sound_System* sound_engine; // Reference to sound system

    public:
        void initialize (Entity_Manager*, Facade2*, Sound_System*);
        void update();
        void checkDelete(IEntity*, const char*, bool, bool);
        void catchBall(IEntity*, IEntity*);
        void throwBall(IEntity*, IEntity*, IEntity*);
        void superThrow(IEntity*, IEntity*, IEntity*);
        void checkSuperThrow(IEntity*, IEntity*);
        void superThrow1(IEntity*, IEntity*);
        void superThrow2(IEntity*, IEntity*);
        void superThrow3(IEntity*, IEntity*);
        void superThrow4(IEntity*, IEntity*);
        void superThrow5(IEntity*, IEntity*);
        void superThrow6(IEntity*, IEntity*);
        void moveSuperBall(IEntity*, IEntity*);
        void moveBall(IEntity*);
        void passBall(IEntity*, IEntity*, IEntity*);
        void dodgeBall(IEntity*, IEntity*, IEntity*);
        void goToBall(IEntity*, IEntity*);
        bool control2Ally(std::string);
        void pickUpPowerUp(IEntity*);
        bool getInAir();
        bool getPlayerIsDead();
        bool getPlayer2IsDead();
        double getSpawnTime();
        bool getPass();
        void move(IEntity*);
        void updateStamina(IEntity*);

        //Multiplayer methods
        std::string getPlayerNameAndPosition();
        void processMultiplayerInfo(std::string answer);
        void multiplayerAction(IEntity* player, IEntity* ball, const char* action);

        IEntity* closerTarget(IEntity*, char);
        IEntity* checkTarget(IEntity*);

        std::vector<std::string> split(const std::string &s, char delim);

        void resetMatch();

        // UPDATE INDICATORS METHOD
        void updateIndicators();
        float getAngle(float xPos, float yPos);
};

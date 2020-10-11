#pragma once

#include <IA_system.h>
#include <sound_system.h>
#include <multiplayer.h>
#include <facade2.h>

class Game_Manager
{
        private:
                unique_ptr<Facade2> facade;
                unique_ptr<Entity_Manager> entity_manager;
                unique_ptr<Mechanics> mechanics;
                unique_ptr<IA_system> IA;
                unique_ptr<Sound_System> sound_engine;
                unique_ptr<Multiplayer> multiplayer;
                bool finish;
                bool biggerFinish;
                bool isPaused;

        public:
                explicit Game_Manager();
                int startGame();
                void startMenus();
                bool getFinish();
                bool getBiggerFinish();
                bool startMultiplayer();
                void turnOff();
};

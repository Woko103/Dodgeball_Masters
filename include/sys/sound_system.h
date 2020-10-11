#pragma once

#include <audio.h>
#include <entity_manager.h>


class Sound_System{
private:
    FMOD::Studio::System *system = nullptr;

    FMOD::Studio::Bank* masterBank = nullptr;
    FMOD_RESULT result = FMOD_OK;
    FMOD::Studio::Bank* stringsBanks = nullptr;
    FMOD::Studio::Bank* ambiente = nullptr;
    FMOD::Studio::Bank* hardSfx = nullptr;
    FMOD::Studio::Bank* voces = nullptr;
    FMOD::System *lowlevel = nullptr;
    FMOD::Sound *sound = nullptr;
    FMOD::Channel *channel = 0;

public:
    explicit Sound_System () = default;
    void initialize();
    void setEvents2Characters(Entity_Manager*);
    void setEvents2Entities(Entity_Manager*);
    void playEventFromEntity(IEntity*);
    //NUEVO
    void stopMapEvent(Entity_Manager*);
    void stopMenuEvent(Entity_Manager*);
    void updateMenu(Entity_Manager*);
    void update(Entity_Manager*);
    void shutdown ();

    void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line)
{
    if (result != FMOD_OK)
    {
        std::cerr << file << "(" << line << "): FMOD error " << result << " - " << FMOD_ErrorString(result) << std::endl;
        exit(-1);
    }
}


#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)
};

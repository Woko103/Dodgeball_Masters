#include <sound_system.h>
#include <iostream>

using namespace std;

void Sound_System::initialize(){
    FMOD::System_Create(&lowlevel);
    lowlevel->init(32, FMOD_INIT_NORMAL, 0);

    FMOD::Studio::System::create(&system);
    system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    system->loadBankFile("fmod/banks/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);
    system->loadBankFile("fmod/banks/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBanks);
    system->loadBankFile("fmod/banks/Ambiente y Musica.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &ambiente);
    system->loadBankFile("fmod/banks/HardSFX.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &hardSfx);
    system->loadBankFile("fmod/banks/Voces.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &voces);
}

void Sound_System::setEvents2Characters(Entity_Manager *entities){
    auto& entitiesMap = entities->getMapCharacters();
    std::map < const char*, unique_ptr<ICharacter> >::iterator itr;
    for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
    {
        // Recogemos una reference a cada unique_ptr de Entidad
        auto& entity = itr->second;
        system->getEvent(entity->audio.cadEvent, &entity->audio.event);
        if(entity->audio.hasParameter==true)
            entity->audio.event->getParameterDescriptionByName("Voicelines", &entity->audio.param);
        entity->audio.event->loadSampleData();
    }
    entities->getCharacter("music")->audio.event->createInstance(&entities->getCharacter("music")->audio.eventInstance);
    entities->getCharacter("music")->audio.eventInstance->start();
}
//NUEVO
void Sound_System::stopMapEvent(Entity_Manager* entities){
    if(entities->getEntity("map_02") != nullptr)
        entities->getEntity("map_02")->audio.eventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
}

void Sound_System::stopMenuEvent(Entity_Manager* entities){
    if(entities->getCharacter("music") != nullptr)
        entities->getCharacter("music")->audio.eventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
}
    
void Sound_System::setEvents2Entities(Entity_Manager *entities){
    entities->getCharacter("music")->audio.eventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
    auto& entitiesMap = entities->getMapEntities();
    std::map < const char*, unique_ptr<IEntity> >::iterator itr;
    for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
    {
            // Recogemos una reference a cada unique_ptr de Entidad
            auto& entity = itr->second;
            system->getEvent(entity->audio.cadEvent, &entity->audio.event);
            if(entity->audio.hasParameter==true)
                entity->audio.event->getParameterDescriptionByName("Voicelines", &entity->audio.param);
            entity->audio.event->loadSampleData();
    }
    entities->getEntity("map_02")->audio.event->createInstance(&entities->getEntity("map_02")->audio.eventInstance);
    entities->getEntity("map_02")->audio.eventInstance->start();
}

void Sound_System::playEventFromEntity(IEntity* entity){
    if(entity->audio.suena==true){
        entity->audio.event->createInstance(&entity->audio.eventInstance);
        if(entity->audio.hasParameter==true)
            entity->audio.eventInstance->setParameterByName("Voicelines", entity->audio.paramValue);
        entity->audio.eventInstance->start();
        entity->audio.eventInstance->release();
        system->update();
    }
}

void Sound_System::updateMenu(Entity_Manager *entities){
    auto& entitiesMap = entities->getMapCharacters();
    std::map < const char*, unique_ptr<ICharacter> >::iterator itr;
    for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
    {
        // Recogemos una reference a cada unique_ptr de Entidad
        auto& entity = itr->second;
        if(entity->audio.suena==true){
            entity->audio.event->createInstance(&entity->audio.eventInstance);
            if(entity->audio.hasParameter==true)
                entity->audio.eventInstance->setParameterByName("Voicelines", entity->audio.paramValue);
            entity->audio.eventInstance->start();
            entity->audio.eventInstance->release();
            entity->audio.suena=false;
        }
            
    }
    system->update();
}

void Sound_System::update(Entity_Manager *entities){
    auto& entitiesMap = entities->getMapEntities();
    std::map < const char*, unique_ptr<IEntity> >::iterator itr;
    for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
    {
        // Recogemos una reference a cada unique_ptr de Entidad
        auto& entity = itr->second;
        if(entity->audio.suena==true){
            entity->audio.event->createInstance(&entity->audio.eventInstance);
            if(entity->audio.hasParameter==true)
                entity->audio.eventInstance->setParameterByName("Voicelines", entity->audio.paramValue);
            entity->audio.eventInstance->start();
            entity->audio.eventInstance->release();
            if(entity != entities->getEntity("map_02")){
                entity->audio.suena=false;
            }
        }
    }

    system->update();
}

void Sound_System::shutdown(){
    ambiente->unload();
    hardSfx->unload();
    voces->unload();
    stringsBanks->unload();
    masterBank->unload();
    system->release();
}


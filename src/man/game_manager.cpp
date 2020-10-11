#include <game_manager.h>

Game_Manager::Game_Manager()
{
    //Creamos unique_ptr de Facade, Sound_System, Entity_Manager, Render_Manager
    facade = make_unique<Facade2>();
    sound_engine = make_unique<Sound_System>();
    sound_engine->initialize();
    entity_manager = make_unique<Entity_Manager>();
    entity_manager->initializeScore();
    entity_manager->initializeCharacters();
    entity_manager->initializeMaps();
    entity_manager->initializeGuiTextures();
    mechanics = make_unique<Mechanics>();
    IA = make_unique<IA_system>();
    multiplayer = nullptr;

    // Initialize main window
    facade->initDevice(entity_manager.get());
    // Load gui textures in GPU
    facade->InitGUIElements();
}

void Game_Manager::startMenus(){

    sound_engine->setEvents2Characters(entity_manager.get());

    finish = false;
    biggerFinish = false;

    bool menusFinished = false;
    int currentMenu = 0;

    while(!menusFinished)
    {
        switch(currentMenu)
        {
            case 0:
                currentMenu = facade->renderMainMenu();
            break;

            case 1:
                if(multiplayer != nullptr)
                    multiplayer = nullptr;
                currentMenu = facade->renderCharacterSelectMenu();
            break;

            case 2:
                if(multiplayer == nullptr)
                    multiplayer = make_unique<Multiplayer>();
                 currentMenu = facade->renderMultiplayerCharacterSelectMenu();
            break;

            case 3:
                currentMenu = facade->renderMapSelectMenu();
            break;

            case 4:
                menusFinished = true;
            break;

            case 5:
                menusFinished = true;
                finish = true;
                biggerFinish = true;
            break;
            case 6:
                currentMenu = facade->renderCredits();
            break;
            case 7:
                currentMenu = facade->renderControls();
            break;
        }

        if(facade->checkShutDown())
            facade->shutDown();

        sound_engine->updateMenu(entity_manager.get());
    }
}

int Game_Manager::startGame()
{
    entity_manager->resetMatchVars();

    // Graphics initialization

    if (multiplayer != nullptr)
    {
        facade->renderLoadingScreen(1);
        if (!startMultiplayer())
        {
            bool end = false;
            while(end == false){
                facade->renderLoadingScreen(3);
                if(!strcmp(facade->checkInput(), "E"))
                    end = true;
            }

            sound_engine->stopMenuEvent(entity_manager.get());
            finish = true;
            multiplayer = nullptr;
        }
    }

    facade->renderLoadingScreen(2);

    if (!finish)
    {
        entity_manager->initializeEntities(facade->getMultiplayerID());

        facade->startGraphics(entity_manager.get());

        // Sound initialization
        sound_engine->setEvents2Entities(entity_manager.get());

        // IA initialization
        IA->initializeIA(entity_manager.get(), facade.get(), mechanics.get());

        // Mechanics initialization
        mechanics->initialize(entity_manager.get(), facade.get(), sound_engine.get());

        if (multiplayer != nullptr)
            multiplayer->run("");
    }

    while(!finish)
    {

        if(entity_manager->getScore()->match.finish == true)
        {
            if(entity_manager->getScore()->match.redPoints == 3 || entity_manager->getScore()->match.bluePoints == 3){
                while(!finish){
                    switch(facade->renderHUD()){
                    case 0:
                    break;

                    case 1:
                        finish = true;
                        sound_engine->stopMapEvent(entity_manager.get());
                        multiplayer = nullptr;
                        facade->resetGame();
                    break;

                    case 2:
                        finish = true;
                        biggerFinish = true;
                        sound_engine->stopMapEvent(entity_manager.get());
                        multiplayer = nullptr;
                        facade->resetGame();
                    break;
                    }
                }
            }
            if(!finish){
                sound_engine->stopMapEvent(entity_manager.get());
                mechanics->resetMatch();
                sound_engine->setEvents2Entities(entity_manager.get());
                entity_manager->getScore()->match.finish=false;
            }
        }

        // facade->updateGameHUD();
        if(multiplayer!=nullptr){
            facade->updateGraphics(entity_manager.get());
            facade->updateEntitiesCollisions();
            IA->updateIA();

            if(!entity_manager->getScore()->match.finish){
                mechanics->update();
            }
        }else{
            if((!strcmp(facade->checkInput(), "P") && facade->checkMando()==0 || facade->checkMando()==1 && facade->buttons()==7) && !finish)
            {
                if(!isPaused){
                    isPaused = true;
                }
            }
            if(!isPaused && !finish)
            {
                facade->updateGraphics(entity_manager.get());
                facade->updateEntitiesCollisions();
                entity_manager->setTarget();
                IA->updateIA();
                if(!entity_manager->getScore()->match.finish){
                    mechanics->update();
                }
            }else{
                if(!finish){
                    switch(facade->renderPauseMenu()){

                    case 0:
                    break;

                    case 1:
                        isPaused = false;
                    break;

                    case 2:
                        isPaused = false;
                        finish = true;
                        sound_engine->stopMapEvent(entity_manager.get());
                        multiplayer = nullptr;
                        facade->resetGame();
                    break;

                    case 3:
                        isPaused = false;
                        finish = true;
                        biggerFinish = true;
                        sound_engine->stopMapEvent(entity_manager.get());
                        multiplayer = nullptr;
                        facade->resetGame();
                    break;
                    }
                }
            }
        }
        if(!finish){
                sound_engine->update(entity_manager.get());
                sound_engine->updateMenu(entity_manager.get());
            }

        if (multiplayer != nullptr && !finish)
        {
            std::string answer = multiplayer->run(mechanics->getPlayerNameAndPosition() + "," + facade->getInputs() + "," + facade->getMultiplayerMuerto());
            facade->clearInputs();
            mechanics->processMultiplayerInfo(answer);
        }

        // Close window
        if(facade->checkShutDown()){
            finish = true;
            biggerFinish = true;
        }
    }
}

bool Game_Manager::getFinish(){
    return finish;
}

bool Game_Manager::getBiggerFinish(){
    return biggerFinish;
}

void Game_Manager::turnOff(){
    sound_engine->shutdown();
    facade->shutDown();
}

bool Game_Manager::startMultiplayer()
{
    std::string answerID = "";

    //Esperamos a que hayan dos jugadores conectados
    while (answerID != "2")
    {
        answerID = multiplayer->run("");

        if (answerID == "Error" || answerID == "No")
        {
            return false;
        }

        if (answerID.size() >= 3)
            answerID = answerID.substr(2,1);

        if (facade->getMultiplayerID() == 0 && (answerID == "1" || answerID == "2" ||
            answerID == "3" || answerID == "4" || answerID == "5" || answerID == "6"))
        {
            facade->setMultiplayerID(std::stoi(answerID));
        }
    }

    std::string infoInicial;

    //Una vez tenemos dos jugadores, comunicamos que personajes y que mapa (en el caso del primero) han cogido
    if (entity_manager->selectedCharactersLength() == 1)
    {
        infoInicial = std::string(entity_manager->selectedCharacters[0]->id);
    }
    else if (entity_manager->selectedCharactersLength() == 2)
    {
        infoInicial = std::string(entity_manager->selectedCharacters[0]->id) + " " + std::string(entity_manager->selectedCharacters[1]->id);
    }
    else if (entity_manager->selectedCharactersLength() == 3)
    {
        infoInicial = std::string(entity_manager->selectedCharacters[0]->id) + " " + std::string(entity_manager->selectedCharacters[1]->id)
                    + " " + std::string(entity_manager->selectedCharacters[2]->id);
    }

    if (facade->getMultiplayerID() == 1)
    {
        infoInicial = infoInicial + "," + std::string(entity_manager->getSelectedMapID());
    }

    while (answerID.size() < 7)
    {
        answerID = multiplayer->run(infoInicial);
    }

    std::vector<std::string> answer = mechanics->split(answerID, ',');

    entity_manager->insertEnemyCharacters(mechanics->split(answer[2], ' '));

    if (facade->getMultiplayerID() != 1 && answer.size() == 4)
    {
        entity_manager->insertSelectedMap(entity_manager->getMap(answer[3].c_str()).get());
    }

    entity_manager->selectedIA = "Medium";

    return true;
}

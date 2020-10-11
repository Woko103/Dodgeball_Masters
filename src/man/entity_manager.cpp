#include <entity_manager.h>

void Entity_Manager::addEntity(const char* name, const char* characterId, const char* meshPath,
    float X, float Y, float Z, std::string team, bool esPlayer, bool esJugador, bool esMultiplayer, bool esPelota, bool esPowerUp, float initialStrength,
    float dash, char* eventDesc, bool hasParameter, float hability, float grabRange, char status, float initialSpeed, int stamina,
    std::string difficulty, int superThrowType)
{
    /*Le pasamos un name como key al mapa y un puntero único que
    será la entidad en cuestión.*/

    bool entityExistant = false;
    std::map< const char*, unique_ptr<IEntity> >::iterator itr;
    // Comprobamos si existe la clave para no sustituir existentes punteros.
    for(itr = entities.begin(); itr != entities.end(); itr++)
    {
        if(strcmp(itr->first, name) == 0)
            entityExistant = true;
    }

    /* Si la clave no existe en el mapa, metemos el puntero de la entidad
        junto con la clave */
    if(!entityExistant)
    {
        entities[name] = make_unique<IEntity>();
        entities[name].get()->meshPath = meshPath;
        entities[name].get()->id = name;
        entities[name].get()->characterId = characterId;
        entities[name].get()->position.X = X;
        entities[name].get()->position.Y = Y;
        entities[name].get()->position.Z = Z;
        entities[name].get()->team.team = team;
        entities[name].get()->control.esPlayer = esPlayer;
        entities[name].get()->control.esJugador = esJugador;
        entities[name].get()->control.esMultiplayer = esMultiplayer;
        entities[name].get()->control.esPelota = esPelota;
        entities[name].get()->control.esPowerUp = esPowerUp;
        entities[name].get()->t.initialStrength = initialStrength;
        entities[name].get()->t.strengthMax = initialStrength;
        entities[name].get()->t.strength = initialStrength;
        entities[name].get()->t.checker = 0;
        entities[name].get()->t.superShot = 0;
        entities[name].get()->t.superThrowType = superThrowType;
        entities[name].get()->dash.valueMax = dash;
        entities[name].get()->dash.value = dash;
        entities[name].get()->dash.count = true;
        entities[name].get()->dash.direction = '-';
        entities[name].get()->audio.cadEvent = eventDesc;
        entities[name].get()->audio.hasParameter = hasParameter;
        entities[name].get()->c.habilityMax = hability;
        entities[name].get()->c.hability = hability;
        entities[name].get()->c.grabRange = grabRange;
        entities[name].get()->c.maxRange = grabRange*2;
        entities[name].get()->status.est = status;
        entities[name].get()->speed.initialPower = initialSpeed;
        entities[name].get()->speed.powerMax = initialSpeed;
        entities[name].get()->speed.power = initialSpeed;
        entities[name].get()->speed.X = 0;
        entities[name].get()->speed.Z = 0;
        entities[name].get()->stamina.stamina = stamina;
        entities[name].get()->stamina.staminaMax = stamina;
        entities[name].get()->difficulty.difficulty = difficulty;
        entities[name].get()->difficulty.perception = 0;
        entities[name].get()->difficulty.actionTime = 0;
        entities[name].get()->difficulty.active = true;
        entities[name].get()->target = nullptr;
    }
    //else
        //cout << "La clave (" << name << ") existe en el mapa de entidades." << endl;
}

void Entity_Manager::removeEntity(const char* name)
{
 	/*Le pasamos el nombre que funciona como clave para que borre
 	            el elemento en cuestión.*/
 	for(auto itr = entities.begin(); itr != entities.end(); itr++)
  	{
        if (!strcmp(itr->first, name))
        {
 	        entities.erase(itr->first);
            break;
        }
  	}
}

void Entity_Manager::removeAllEntities()
{
 	entities.clear(); //Borramos todos los elementos
}

void Entity_Manager::initializeEntities(uint16_t multiplayerID)
{
    // Inicializamos todas las entidades de nuestro juego
    /*
        Key: Clave por la que reconoceremos a la entidad
        MeshPath: Ruta relativa del mesh de la entidad
        TexturePath: Ruta relativa de la textura del mesh de la entidad
        Position3D: X, Y, Z
        Team: Red/Blue/NoTeam
        5 bools de control: esPlayer, esJugador, esMultiplayer, esPelota, esPowerUp
        Throw.strength: Fuerza inicial Max 0.08 Min 0.03
        Dash: Dash inicial Max 0.08 Min discutible
        2 cosas de audio: Cadena del evento, bool hasParamter
        Catch hability: Del 1 al 10
        Grab Range: Max 0.3
        Status: En el estado.h esta divinamente explicado
        Speed: Velocidad inicial Max 0.05 Min 0.02
        Stamina: Stamina maxima 500 valor medio
    */
    //Se inicializan una variable con una posicion aleatoria para la aparicion del powerUp y
    // otra pàra asignarle una posicion aleatoria a la pelota
    float posPowerUp = randomPos();
    float posBall = 0;
    //Se inicializa una seed para hcaerlo lo mas aleatorio posible
    srand(time(NULL));
    int num = rand() % 2;
    switch (num) {
      case 0:
        posBall = 0.7;
      break;
      case 1:
        posBall = -0.85;
      break;
    }

    float iaStats = 0;
    if(selectedIA == "Easy"){
      iaStats = -0.02;
    }
    else if(selectedIA == "Hard"){
      iaStats = 0.02;
    }

    bool playerEsPlayer = true;
    bool playerEsJugador = false;

    //Number of players to be loaded
    int teamMembers = balanceTeams();

    // Init friend characters
    if (multiplayerID == 2)
    {
      addEntity("Aplayer", selectedEnemyCharacters[0]->id, selectedEnemyCharacters[0]->meshPath, 1, 0.52, 0.7, "Blue", false, false, false, false, false, selectedEnemyCharacters[0]->t.strength, selectedEnemyCharacters[0]->dash.value, selectedEnemyCharacters[0]->audio.cadEvent, selectedEnemyCharacters[0]->audio.hasParameter, selectedEnemyCharacters[0]->c.hability, selectedEnemyCharacters[0]->c.grabRange, 'N', selectedEnemyCharacters[0]->speed.initialPower, selectedEnemyCharacters[0]->stamina.stamina, "Medium", selectedEnemyCharacters[0]->t.superThrowType);
      if(teamMembers>1)
        addEntity("n_npc_01", selectedEnemyCharacters[1]->id, selectedEnemyCharacters[1]->meshPath, -1, 0.52, 0.7, "Blue", false, true, false, false, false, selectedEnemyCharacters[1]->t.strength + iaStats, selectedEnemyCharacters[1]->dash.value, selectedEnemyCharacters[1]->audio.cadEvent, selectedEnemyCharacters[1]->audio.hasParameter, selectedEnemyCharacters[1]->c.hability, selectedEnemyCharacters[1]->c.grabRange, 'N', selectedEnemyCharacters[1]->speed.initialPower, selectedEnemyCharacters[1]->stamina.stamina, "Medium", selectedEnemyCharacters[1]->t.superThrowType);
      if(teamMembers>2)
        addEntity("n_npc_02", selectedEnemyCharacters[2]->id, selectedEnemyCharacters[2]->meshPath, 0, 0.52, 1.8, "Blue", false, true, false, false, false, selectedEnemyCharacters[2]->t.strength + iaStats, selectedEnemyCharacters[2]->dash.value, selectedEnemyCharacters[2]->audio.cadEvent, selectedEnemyCharacters[2]->audio.hasParameter, selectedEnemyCharacters[2]->c.hability, selectedEnemyCharacters[2]->c.grabRange, 'N', selectedEnemyCharacters[2]->speed.initialPower, selectedEnemyCharacters[2]->stamina.stamina, "Medium", selectedEnemyCharacters[2]->t.superThrowType);
      // Init enemy characters
      addEntity("n_enemy_01", selectedCharacters[0]->id, selectedCharacters[0]->meshPath, 1, 0.52, -0.85, "Red", true, false, false, false, false, selectedCharacters[0]->t.strength + iaStats, selectedCharacters[0]->dash.value, selectedCharacters[0]->audio.cadEvent, selectedCharacters[0]->audio.hasParameter, selectedCharacters[0]->c.hability, selectedCharacters[0]->c.grabRange, 'N', selectedCharacters[0]->speed.initialPower, selectedCharacters[0]->stamina.stamina, selectedIA, selectedCharacters[0]->t.superThrowType);
      if(teamMembers>1)
        addEntity("n_enemy_02", selectedCharacters[1]->id, selectedCharacters[1]->meshPath, -1, 0.52, -0.85, "Red", false, true, false, false, false, selectedCharacters[1]->t.strength + iaStats, selectedCharacters[1]->dash.value, selectedCharacters[1]->audio.cadEvent, selectedCharacters[1]->audio.hasParameter, selectedCharacters[1]->c.hability, selectedCharacters[1]->c.grabRange, 'N', selectedCharacters[1]->speed.initialPower, selectedCharacters[1]->stamina.stamina, selectedIA, selectedCharacters[1]->t.superThrowType);
      if(teamMembers>2)
        addEntity("n_enemy_03", selectedCharacters[2]->id, selectedCharacters[2]->meshPath, 0, 0.52, -1.85, "Red", false, true, false, false, false, selectedCharacters[2]->t.strength + iaStats, selectedCharacters[2]->dash.value, selectedCharacters[2]->audio.cadEvent, selectedCharacters[2]->audio.hasParameter, selectedCharacters[2]->c.hability, selectedCharacters[2]->c.grabRange, 'N', selectedCharacters[2]->speed.initialPower, selectedCharacters[2]->stamina.stamina, selectedIA, selectedCharacters[2]->t.superThrowType);
    }
    else
    {
      addEntity("Aplayer", selectedCharacters[0]->id, selectedCharacters[0]->meshPath, 1, 0.52, 0.7, "Blue", true, false, false, false, false, selectedCharacters[0]->t.strength, selectedCharacters[0]->dash.value, selectedCharacters[0]->audio.cadEvent, selectedCharacters[0]->audio.hasParameter, selectedCharacters[0]->c.hability, selectedCharacters[0]->c.grabRange, 'N', selectedCharacters[0]->speed.initialPower, selectedCharacters[0]->stamina.stamina, "Medium", selectedCharacters[0]->t.superThrowType);
      if(teamMembers>1)
        addEntity("n_npc_01", selectedCharacters[1]->id, selectedCharacters[1]->meshPath, -1, 0.52, 0.7, "Blue", false, true, false, false, false, selectedCharacters[1]->t.strength + iaStats, selectedCharacters[1]->dash.value, selectedCharacters[1]->audio.cadEvent, selectedCharacters[1]->audio.hasParameter, selectedCharacters[1]->c.hability, selectedCharacters[1]->c.grabRange, 'N', selectedCharacters[1]->speed.initialPower, selectedCharacters[1]->stamina.stamina, "Medium", selectedCharacters[1]->t.superThrowType);
      if(teamMembers>2)
        addEntity("n_npc_02", selectedCharacters[2]->id, selectedCharacters[2]->meshPath, 0, 0.52, 1.8, "Blue", false, true, false, false, false, selectedCharacters[2]->t.strength + iaStats, selectedCharacters[2]->dash.value, selectedCharacters[2]->audio.cadEvent, selectedCharacters[2]->audio.hasParameter, selectedCharacters[2]->c.hability, selectedCharacters[2]->c.grabRange, 'N', selectedCharacters[2]->speed.initialPower, selectedCharacters[2]->stamina.stamina, "Medium", selectedCharacters[2]->t.superThrowType);

     // Init enemy characters
      if (multiplayerID == 0)
        addEntity("n_enemy_01", selectedEnemyCharacters[0]->id, selectedEnemyCharacters[0]->meshPath, 1, 0.52, -0.85, "Red", false, true, false, false, false, selectedEnemyCharacters[0]->t.strength + iaStats, selectedEnemyCharacters[0]->dash.value, selectedEnemyCharacters[0]->audio.cadEvent, selectedEnemyCharacters[0]->audio.hasParameter, selectedEnemyCharacters[0]->c.hability, selectedEnemyCharacters[0]->c.grabRange, 'N', selectedEnemyCharacters[0]->speed.initialPower, selectedEnemyCharacters[0]->stamina.stamina, selectedIA, selectedEnemyCharacters[0]->t.superThrowType);
      else
        addEntity("n_enemy_01", selectedEnemyCharacters[0]->id, selectedEnemyCharacters[0]->meshPath, 1, 0.52, -0.85, "Red", false, false, false, false, false, selectedEnemyCharacters[0]->t.strength + iaStats, selectedEnemyCharacters[0]->dash.value, selectedEnemyCharacters[0]->audio.cadEvent, selectedEnemyCharacters[0]->audio.hasParameter, selectedEnemyCharacters[0]->c.hability, selectedEnemyCharacters[0]->c.grabRange, 'N', selectedEnemyCharacters[0]->speed.initialPower, selectedEnemyCharacters[0]->stamina.stamina, selectedIA, selectedEnemyCharacters[0]->t.superThrowType);
      if(teamMembers>1)
        addEntity("n_enemy_02", selectedEnemyCharacters[1]->id, selectedEnemyCharacters[1]->meshPath, -1, 0.52, -0.85, "Red", false, true, false, false, false, selectedEnemyCharacters[1]->t.strength + iaStats, selectedEnemyCharacters[1]->dash.value, selectedEnemyCharacters[1]->audio.cadEvent, selectedEnemyCharacters[1]->audio.hasParameter, selectedEnemyCharacters[1]->c.hability, selectedEnemyCharacters[1]->c.grabRange, 'N', selectedEnemyCharacters[1]->speed.initialPower, selectedEnemyCharacters[1]->stamina.stamina, selectedIA, selectedEnemyCharacters[1]->t.superThrowType);
      if(teamMembers>2)
        addEntity("n_enemy_03", selectedEnemyCharacters[2]->id, selectedEnemyCharacters[2]->meshPath, 0, 0.52, -1.85, "Red", false, true, false, false, false, selectedEnemyCharacters[2]->t.strength + iaStats, selectedEnemyCharacters[2]->dash.value, selectedEnemyCharacters[2]->audio.cadEvent, selectedEnemyCharacters[2]->audio.hasParameter, selectedEnemyCharacters[2]->c.hability, selectedEnemyCharacters[2]->c.grabRange, 'N', selectedEnemyCharacters[2]->speed.initialPower, selectedEnemyCharacters[2]->stamina.stamina, selectedIA, selectedEnemyCharacters[2]->t.superThrowType);
    }

    // Init neutral entities
    addEntity("flecha_direccion_player", "flecha_direccion_player", "meshes/indicators/flecha_direccion_player.obj", 0, 0, 0, "NoTeam", false, false, false, false, false, 0, 0, "", false, 0, 0.04, 'N', 0, 0, "-", -1);
    addEntity("ball_icon", "ball_icon", "meshes/pelota/pelota.obj", 0, 0, 0, "NoTeam", false, false, false, false, false, 0, 0, "", false, 0, 0.04, 'N', 0, 0, "-", -1);
    addEntity("n_ball_01", "n_ball_01", "meshes/pelota/pelota.obj", 0, 0.2, 0.7, "NoTeam", false, false, false, true, false, 0, 0, "event:/Hard_SFX/Lanza", false, 0, 0.04, 'N', 0, 0, "-", -1);
    if (multiplayerID == 0)
      addEntity("n_powerup_01", "n_powerup_01", "meshes/powerUp/powerup.obj", posPowerUp, 0.25, 0, "NoTeam", false, false, false, false, true, 0, 0, "", false, 0, 0, 'N', 0, 0, "-", -1);
    addEntity("map_02", selectedMap->id, selectedMap->meshPath, 0, 0.5, 0, "NoTeam", false, false, false, false, false, 0, 0, selectedMap->audio.cadEvent, false, 0, 0.3, 'N', 0, 0, "-", -1);
    if(selectedMap->id == "Jungla"){
      addEntity("zcubo_mapa", "cubo_mapa", "meshes/jungle/Jungle_Transparent_Wall.obj", 0, 0.5, 0, "NoTeam", false, false, false, false, false, 0, 0, "", false, 0, 0.3, 'N', 0, 0, "-", -1);
    }
    else{
      addEntity("zcubo_mapa", "cubo_mapa2", "meshes/stadium/malaka_stadium_cubo.obj", 0, 0.5, 0, "NoTeam", false, false, false, false, false, 0, 0, "", false, 0, 0.3, 'N', 0, 0, "-", -1);
    }
}

void Entity_Manager::setTarget(){
  vector<IEntity*> redTeam;
  vector<IEntity*> blueTeam;
  for(auto itr = entities.begin(); itr != entities.end(); itr++){
    auto& entity = itr->second;
    if((entity->team.team == "Red" || entity->team.team == "Blue") && !entity->control.esPelota){
      if(entity->team.team == "Red"){
        redTeam.push_back(entity.get());
      }
      else{
        blueTeam.push_back(entity.get());
      }
    }
  }
  int pos1 = -1;
  int pos2 = -1;
  for(int i = 0; i < redTeam.size(); i++){
    auto& red = redTeam.at(i);
    float distMin = 99999;
    if(blueTeam.size() == 1){
      red->target = blueTeam.back();
    }
      for(int j = 0; j < blueTeam.size(); j++){
        auto& blue = blueTeam.at(j);
        if(redTeam.size() == 1){
          blue->target = red;
        }
        else{
          float dist = (red->position.Z - blue->position.Z) * (red->position.Z - blue->position.Z) +
          (red->position.X - blue->position.X) * (red->position.X - blue->position.X);
          dist = sqrtl(dist);
          if(dist < distMin && j != pos1 && j != pos2){
            red->target = blue;
            distMin = dist;
            if(i == 0){
              pos1 = j;
            }
            else if(i == 1){
              pos2 = j;
            }
          }
        }
      }
      if(i == redTeam.size() - 1 && blueTeam.size() < redTeam.size()){
        red->target = blueTeam.back();
      }
      if(redTeam.size() != 1){
        red->target->target = redTeam.at(i);
      }
  }
  if(redTeam.size() == 2 && blueTeam.size() > redTeam.size()){
    int cont = 0;
    if(cont == pos1 || cont == pos2){
      cont++;
    }
    if(cont == pos1 || cont == pos2){
      cont++;
    }
    auto& pesao = blueTeam.at(cont);
    pesao->target = redTeam.back();
  }
}

void Entity_Manager::initializeScore()
{
    score = make_unique<IScore>();
    score->match.bluePoints = 0;
    score->match.redPoints = 0;
    score->match.finish = false;
    score->match.ronda = 1;
}

void Entity_Manager::initializeCharacters()
{
  addCharacter("KingMalaka", "meshes/KingMalaka/KingMalaka", "Elementos_ingame/seleccion_personajes/kingmalaka.png",0, 0, 0, "NoTeam", false, false, 0.12, 0.082, "event:/Voces/KingMalaka", true, 7, 0.48, 0.035, 615, 1, false);
  addCharacter("BBH", "meshes/BeepBot/BeepBot", "Elementos_ingame/seleccion_personajes/bbh.png",0, 0, 0, "NoTeam", false, false, 0.13, 0.04, "event:/Voces/BBH", true, 10, 0.6, 0.0225, 660, 3, false);
  addCharacter("KStar", "meshes/KStar/KStar", "Elementos_ingame/seleccion_personajes/kstar.png",0, 0, 0, "NoTeam", false, false, 0.08, 0.094, "event:/Voces/KStar", true, 4, 0.36, 0.045, 570, 5, false);
  addCharacter("ElRisas", "meshes/ElRisas/ElRisas", "Elementos_ingame/seleccion_personajes/elrisas.png",0, 0, 0, "NoTeam", false, false, 0.09, 0.1, "event:/Voces/ElRisas", true, 1, 0.2, 0.0425, 660, 6, false);
  addCharacter("LilDumb", "meshes/LilDumb/LilDumb", "Elementos_ingame/seleccion_personajes/lil_dumb.png",0, 0, 0, "NoTeam", false, false, 0.11, 0.07, "event:/Voces/LilDumb", true, 5, 0.4, 0.03, 525, 4, false);
  addCharacter("MrBuck", "meshes/MrBuck/MrBuck", "Elementos_ingame/seleccion_personajes/mrbuck.png",0, 0, 0, "NoTeam", false, false, 0.14, 0.052, "event:/Voces/MrBuck", true, 7, 0.48, 0.02, 750, 2, false);
  addCharacter("ElAfilador", "meshes/ElAfilador/ElAfilador", "Elementos_ingame/seleccion_personajes/elafilador.png",0, 0, 0, "NoTeam", false, false, 0.1, 0.064, "event:/Voces/ElAfilador", true, 7, 0.48, 0.035, 750, 4, false);
  addCharacter("Cythria", "meshes/Cytrhia/Cytrhia", "Elementos_ingame/seleccion_personajes/cythria.png",0, 0, 0, "NoTeam", false, false, 0.13, 0.058, "event:/Voces/Cythria", true, 9, 0.56, 0.025, 570, 5, false);
  addCharacter("HardSFX", "", "",0, 0.07, 1.8, "NoTeam", false, false, 0.04, 0.08, "event:/Hard_SFX/HardSFX_Parameter", true, 1, 0.3, 0.02, 400, 1, false);
  addCharacter("music", "", "",0, 0.07, 1.8, "NoTeam", false, false, 0.04, 0.08, "event:/Ambiente/Menu", false, 1, 0.3, 0.02, 400, 1, false);
}

void Entity_Manager::addCharacter(const char* name, const char* meshPath, const char* imagePath,
            float X, float Y, float Z, std::string team, bool esPlayer, bool esJugador,
            float initialStrength, float dash, char *eventDesc, bool hasParameter, float hability,
            float grabRange, float initialSpeed, int stamina, int superThrowType, bool selected)
{
  bool characterExists = false;
  std::map< const char*, unique_ptr<ICharacter> >::iterator itr;
  // Comprobamos si existe la clave para no sustituir existentes punteros.
  for(itr = characters.begin(); itr != characters.end(); itr++)
  {
      if(strcmp(itr->first, name) == 0)
          characterExists = true;
  }
  /* Si la clave no existe en el mapa, metemos el puntero de la gui
      junto con la clave */
  if(!characterExists)
  {
    characters[name] = make_unique<ICharacter>();
    characters[name].get()->meshPath = meshPath;
    characters[name].get()->imagePath = imagePath;
    characters[name].get()->id = name;
    characters[name].get()->position.X = X;
    characters[name].get()->position.Y = Y;
    characters[name].get()->position.Z = Z;
    characters[name].get()->team.team = team;
    characters[name].get()->control.esPlayer = esPlayer;
    characters[name].get()->control.esJugador = esJugador;
    characters[name].get()->control.esMultiplayer = false;
    characters[name].get()->t.initialStrength = initialStrength;
    characters[name].get()->t.strengthMax = initialStrength;
    characters[name].get()->t.strength = initialStrength;
    characters[name].get()->t.superThrowType = superThrowType;
    characters[name].get()->dash.valueMax = dash;
    characters[name].get()->dash.value = dash;
    characters[name].get()->audio.cadEvent = eventDesc;
    characters[name].get()->audio.hasParameter = hasParameter;
    characters[name].get()->c.habilityMax = hability;
    characters[name].get()->c.hability = hability;
    characters[name].get()->c.grabRange = grabRange;
    characters[name].get()->c.maxRange = grabRange*2;
    characters[name].get()->speed.initialPower = initialSpeed;
    characters[name].get()->speed.powerMax = initialSpeed;
    characters[name].get()->speed.power = initialSpeed;
    characters[name].get()->stamina.stamina = stamina;
    characters[name].get()->stamina.staminaMax = stamina;
    characters[name].get()->selected = selected;
  }
}

void Entity_Manager::initializeMaps(){
    addMap("Estadio", "meshes/stadium/malaka_stadium.obj", "Estadio_text", "event:/Ambiente/Estadio", true, false);
    addMap("Jungla", "meshes/jungle/Jungle_Map.obj", "Jungla_text", "event:/Ambiente/Jungla", true, false);
    addMap("Luna", "meshes/moon/moon_stage.obj", "Luna_text", "event:/Ambiente/Luna", true, false);
}

void Entity_Manager::addMap(const char* name, const char* meshPath, const char* imagePath, char *eventDesc, bool hasParameter, bool selected)
{
  bool mapExists = false;
  std::map< const char*, unique_ptr<IMap> >::iterator itr;
  // Comprobamos si existe la clave para no sustituir existentes punteros.
  for(itr = maps.begin(); itr != maps.end(); itr++)
  {
      if(strcmp(itr->first, name) == 0)
          mapExists = true;
  }
  /* Si la clave no existe en el mapa, metemos el puntero de la gui
      junto con la clave */
  if(!mapExists)
  {
    maps[name] = make_unique<IMap>();
    maps[name].get()->meshPath = meshPath;
    maps[name].get()->imagePath = imagePath;
    maps[name].get()->id = name;
    maps[name].get()->audio.cadEvent = eventDesc;
    maps[name].get()->audio.hasParameter = hasParameter;
    maps[name].get()->selected = selected;
  }
}

const char* Entity_Manager::getSelectedMapID(){
    return selectedMap->id;
}

const char* Entity_Manager::getSelectedMapText(){
    return selectedMap->imagePath;
}

//Metodo para elegir una posicion aleatoria para el powerUp
float Entity_Manager::randomPos(){
  //Se inicializa una seed para hcaerlo lo mas aleatorio posible
  srand(time(NULL));
  int num = rand() % 4;
  float pos = 0;
  switch (num) {
    case 0:
      pos = -1.5;
    break;
    case 1:
      pos = -0.75;
    break;
    case 2:
      pos = 0.75;
    break;
    case 3:
      pos = 1.5;
    break;
  }
  return pos;
}

void Entity_Manager::showEntities()
{
    /*Creamos un iterador para ir iterando por las entidades
            y mostrando del tipo que son */
  	std::map< const char*, unique_ptr<IEntity> >::iterator itr;

  	for(itr = entities.begin(); itr != entities.end(); itr++)
  	{
		cout << "Key(name): " << itr->first << endl;
  	  	cout << "Value(Unique Pointer) " << itr->second.get() << endl;
  	  	cout << "Entity: " << typeid(itr->second.get()).name() << endl;
  	  	cout << "=========================" << endl;
  	}
}

void Entity_Manager::showCharacters()
{
    /*Creamos un iterador para ir iterando por las entidades
            y mostrando del tipo que son */
  	std::map< const char*, unique_ptr<ICharacter> >::iterator itr;

  	for(itr = characters.begin(); itr != characters.end(); itr++)
  	{
		cout << "Key(name): " << itr->first << endl;
  	  	cout << "Value(Unique Pointer) " << itr->second.get() << endl;
  	  	cout << "Entity: " << typeid(itr->second.get()).name() << endl;
  	  	cout << "=========================" << endl;
  	}
}

/* GET ENTITY METHODS*/
unique_ptr<IEntity>& Entity_Manager::getEntity(const char* id)
{
    for(auto itr = entities.begin(); itr != entities.end(); itr++)
    {
        if(strcmp(itr->first, id) == 0)
            return itr->second;
    }
}

unique_ptr<IEntity>& Entity_Manager::getEntityByName(const char* name)
{
    for(auto itr = entities.begin(); itr != entities.end(); itr++)
    {
        if(strcmp(itr->second->id, name) == 0)
            return itr->second;
    }
}

unique_ptr<IEntity>& Entity_Manager::getEntityByCharacter(const char* name)
{
    for(auto itr = entities.begin(); itr != entities.end(); itr++)
    {
        if(strcmp(itr->second->characterId, name) == 0)
            return itr->second;
    }
}

bool Entity_Manager::entityExist(const char* name)
{
  for(auto itr = entities.begin(); itr != entities.end(); itr++)
  {
    if(strcmp(itr->second->id, name) == 0)
        return true;
  }

  return false;
}

bool Entity_Manager::entityExistsByCharacter(const char* name)
{
  for(auto itr = entities.begin(); itr != entities.end(); itr++)
  {
    if(strcmp(itr->second->characterId, name) == 0)
        return true;
  }

  return false;
}

bool Entity_Manager::entityExistsMultiplayer(const char* name)
{
  for(auto itr = entities.begin(); itr != entities.end(); itr++)
  {
    if(strcmp(itr->first, name) == 0)
        return true;
  }

  return false;
}

unique_ptr<IScore>& Entity_Manager::getScore()
{
    return score;
}

void Entity_Manager::resetMatchVars(){
  score->match.bluePoints = 0;
  score->match.redPoints = 0;
  score->match.finish = false;
}

std::map< const char*, unique_ptr<IEntity>, cmp_str >& Entity_Manager::getMapEntities()
{
    return entities;
}

std::map< const char*, unique_ptr<ICharacter> >& Entity_Manager::getMapCharacters()
{
    return characters;
}

/* CHARACTER METODS */
// Get character by name
unique_ptr<ICharacter>& Entity_Manager::getCharacter(const char* name)
{
    for(auto itr = characters.begin(); itr != characters.end(); itr++)
    {
        if(strcmp(itr->first, name) == 0)
            return itr->second;
    }
}

unique_ptr<IMap>& Entity_Manager::getMap(const char* name)
{
    for(auto itr = maps.begin(); itr != maps.end(); itr++)
    {
        if(strcmp(itr->first, name) == 0)
            return itr->second;
    }
}

void Entity_Manager::insertSelectedMap(IMap* map)
{
  if(selectedMap){
    selectedMap->selected = false;
  }
  selectedMap = (IMap*)map;
}

bool Entity_Manager::insertSelectedCharacter(int index, ICharacter* character)
{
  // Insert character in the first empty position
  for(int i = 0; i < 3; i++)
  {
    if(!selectedCharacters[i])
    {
      selectedCharacters[i] = character;
      return true;
    }
  }

  // If there is no empty position replace the earlier selected character
  if(selectedCharacters[index])
    selectedCharacters[index]->selected = false;

  selectedCharacters[index] = (ICharacter*)character;
}

bool Entity_Manager::removeSelectedCharacter(ICharacter* character)
{
  // Insert character in the first empty position
  for(int i = 0; i < 3; i++)
  {
    if(selectedCharacters[i] == character)
    {
      selectedCharacters[i] = nullptr;
      return true;
    }
  }
}

int Entity_Manager::balanceTeams(){

  int devol = selectedCharactersLength();

  if(selectedCharactersLength() != selectedEnemiesLength()){
    if(selectedCharactersLength() < selectedEnemiesLength()){
      devol = selectedCharactersLength();
    }
    else if(selectedCharactersLength() > selectedEnemiesLength()){
      devol = selectedEnemiesLength();
    }
  }



  return devol;
}

int Entity_Manager::selectedCharactersLength()
{
  int devol = 3;
  for(int i = 0; i < 3; i++)
  {
    if(!selectedCharacters[i]){
      devol--;
    }
  }

  return devol;
}

bool Entity_Manager::selectedCharactersInOrder(){
  bool devol = false;
  if(selectedCharactersLength()>0){
    if(selectedCharacters[0]){
      if((!selectedCharacters[2] && selectedCharacters[1] ) || (selectedCharacters[1] && selectedCharacters[2]) || (!selectedCharacters[1] && !selectedCharacters[2])){
        devol = true;
      }
    }
  }
  return devol;
}

int Entity_Manager::selectedEnemiesLength()
{
  int devol = 3;
  for(int i = 0; i < 3; i++)
  {
    if(!selectedEnemyCharacters[i])
      devol--;
  }

  return devol;
}

void Entity_Manager::selectEnemyCharacters()
{
  char* enemyCharacterPool[8] = {"KingMalaka", "BBH", "KStar", "ElRisas", "LilDumb", "MrBuck", "ElAfilador", "Cythria"};

  // Shuffle pool array
  char* temp = "";
  int randomIndex = 0;

  srand (time(NULL));

  for(int i = 0; i < 8; i++)
  {
    randomIndex = rand() % 8;
    temp = enemyCharacterPool[i];
    enemyCharacterPool[i] = enemyCharacterPool[randomIndex];
    enemyCharacterPool[randomIndex] = temp;
  }

  // Select enemy characters for the game
  // Creates the same number of enemies as selectedCharacters
  int j = 0;
  for(int i = 0; i < 8 && j < selectedCharactersLength(); i++)
  {
    ICharacter* enemy = getCharacter(enemyCharacterPool[i]).get();

    if(!enemy->selected)
    {
      selectedEnemyCharacters[j] = enemy;
      j++;
    }
  }

  // Reset selected variables for the next game
  std::map< const char*, unique_ptr<ICharacter> >::iterator itr;

  for(itr = characters.begin(); itr != characters.end(); itr++)
  {
      if(itr->second->selected)
        itr->second->selected = false;
  }
}

void Entity_Manager::insertEnemyCharacters(std::vector<std::string> enemiesID)
{
  for(int i = 0; i < enemiesID.size(); i++)
  {
    if (enemiesID[i] != "")
      selectedEnemyCharacters[i] = getCharacter(enemiesID[i].c_str()).get();
  }
}

/* GUI TEXTURE METHODS */
std::map< const char*, unique_ptr<IGuiTexture> >& Entity_Manager::getMapGuiTextures()
{
  return guiTextures;
}

void Entity_Manager::addGuiTexture(const char* name, const char* texturePath)
{
  bool guiTextureExists = false;

  // Comprobamos si existe la clave para no sustituir existentes punteros.
  for(auto itr = guiTextures.begin(); itr != guiTextures.end(); itr++)
  {
      if(strcmp(itr->first, name) == 0)
          guiTextureExists = true;
  }
  /* Si la clave no existe en el mapa, metemos el puntero de la gui
      junto con la clave */
  if(!guiTextureExists)
  {
    guiTextures[name] = make_unique<IGuiTexture>();
    guiTextures[name].get()->texturePath = texturePath;
    guiTextures[name].get()->width = 0;
    guiTextures[name].get()->height = 0;
    guiTextures[name].get()->texture = 0;
  }
}

void Entity_Manager::initializeGuiTextures()
{
  addGuiTexture("DodgeballMastersTitle", "Elementos_ingame/menu_principal/DodgeballMasters.png");
  addGuiTexture("DodgeballMastersTitle2", "Elementos_ingame/menu_principal/titulo_dodgeball_masters_splashart.png");
  addGuiTexture("PlayText", "Elementos_ingame/menu_principal/PLAY.png");
  addGuiTexture("PlayButton", "Elementos_ingame/menu_principal/opcion_roja_noselect.png");
  addGuiTexture("MastersCup", "Elementos_ingame/menu_principal/Modos_juego/MASTERS_CUP.png");
  addGuiTexture("Multiplayer", "Elementos_ingame/menu_principal/Modos_juego/MULTIPLAYER.png");
  addGuiTexture("Back", "Elementos_ingame/menu_principal/Modos_juego/BACK.png");
  addGuiTexture("Exit", "Elementos_ingame/menu_principal/EXIT.png");
  addGuiTexture("Choose", "Elementos_ingame/seleccion_lado/choose.png");
  addGuiTexture("SelectCharacter", "Elementos_ingame/seleccion_personajes/select_character.png");
  addGuiTexture("SelectMap", "Elementos_ingame/seleccion_escenarios/select_stage.png");
  addGuiTexture("EnemiesLvl", "Elementos_ingame/match_options/enemies_lvl.png");
  addGuiTexture("Easy", "Elementos_ingame/match_options/Easy.png");
  addGuiTexture("Medium", "Elementos_ingame/match_options/medium.png");
  addGuiTexture("Hard", "Elementos_ingame/match_options/hard.png");
  addGuiTexture("Resume", "Elementos_ingame/pause_options/resume_selected.png");
  addGuiTexture("Quit", "Elementos_ingame/pause_options/quit_selected.png");
  addGuiTexture("Controls", "Elementos_ingame/pause_options/controls.png");
  addGuiTexture("FondoDesenfocado", "Elementos_ingame/menu_principal/fondoDesenfocado.png");

  addGuiTexture("grafVerde", "Elementos_ingame/menu_principal/Grafiti_Verde.png");
  addGuiTexture("grafAzul", "Elementos_ingame/menu_principal/Grafiti_Azul.png");
  addGuiTexture("grafRojo", "Elementos_ingame/menu_principal/Grafiti_Rojo.png");
  addGuiTexture("grafAmarillo", "Elementos_ingame/menu_principal/Fondo.png");
  addGuiTexture("PauseScreen", "Elementos_ingame/menu_principal/menuPausa.png");
  addGuiTexture("DodgeballBGNoTitle", "Elementos_ingame/menu_principal/Splashart_menu_principal_sin_titulo.png");
  addGuiTexture("DodgeballBG", "Elementos_ingame/menu_principal/Splashart_menu_principal.png");
  addGuiTexture("Creditos", "Elementos_ingame/menu_principal/creditos.png");
  addGuiTexture("Credits", "Elementos_ingame/menu_principal/CREDITS.png");
  addGuiTexture("Controles", "Elementos_ingame/menu_principal/controles.png");
  addGuiTexture("LoadingScreen", "Elementos_ingame/menu_principal/pantalla_de_carga.png");
  addGuiTexture("LoadingScreenSingle", "Elementos_ingame/menu_principal/pantallaCarga.png");
  addGuiTexture("LoadingScreenMultiplayer", "Elementos_ingame/menu_principal/pantallaCargaMultiplayer.png");
  addGuiTexture("ErrorScreen", "Elementos_ingame/menu_principal/pantallaErrorServer.png");
  addGuiTexture("Victory", "Elementos_ingame/match_options/victory.png");
  addGuiTexture("Defeat", "Elementos_ingame/match_options/defeat.png");
  addGuiTexture("Deathcam", "Elementos_ingame/match_options/Deathcam.png");

  // Characters
  addGuiTexture("KingMalaka", "Elementos_ingame/seleccion_personajes/KingMalaka.png");
  addGuiTexture("KStar", "Elementos_ingame/seleccion_personajes/KStar.png");
  addGuiTexture("BBH", "Elementos_ingame/seleccion_personajes/BBH.png");
  addGuiTexture("LilDumb", "Elementos_ingame/seleccion_personajes/LilDumb.png");
  addGuiTexture("MrBuck", "Elementos_ingame/seleccion_personajes/MrBuck.png");
  addGuiTexture("ElAfilador", "Elementos_ingame/seleccion_personajes/ElAfilador.png");
  addGuiTexture("Cythria", "Elementos_ingame/seleccion_personajes/Cytrhia.png");
  addGuiTexture("ElRisas", "Elementos_ingame/seleccion_personajes/ElRisas.png");

  addGuiTexture("KingMalaka_border", "Elementos_ingame/seleccion_personajes/KingMalaka_selected_home.png");
  addGuiTexture("KStar_border", "Elementos_ingame/seleccion_personajes/KStar_selected_home.png");
  addGuiTexture("BBH_border", "Elementos_ingame/seleccion_personajes/BBH_selected_home.png");
  addGuiTexture("LilDumb_border", "Elementos_ingame/seleccion_personajes/LilDumb_selected_home.png");
  addGuiTexture("MrBuck_border", "Elementos_ingame/seleccion_personajes/MrBuck_selected_home.png");
  addGuiTexture("ElAfilador_border", "Elementos_ingame/seleccion_personajes/ElAfilador_selected_home.png");
  addGuiTexture("Cythria_border", "Elementos_ingame/seleccion_personajes/Cytrhia_selected_home.png");
  addGuiTexture("ElRisas_border", "Elementos_ingame/seleccion_personajes/ElRisas_selected_home.png");

  addGuiTexture("KingMalaka_selected", "Elementos_ingame/seleccion_personajes/paneles_seleccion_kingmalaka_definitive.png");
  addGuiTexture("KStar_selected", "Elementos_ingame/seleccion_personajes/paneles_seleccion_kstar_definitive.png");
  addGuiTexture("BBH_selected", "Elementos_ingame/seleccion_personajes/paneles_seleccion_BBH_definitive.png");
  addGuiTexture("LilDumb_selected", "Elementos_ingame/seleccion_personajes/paneles_seleccion_lildumb_definitive.png");
  addGuiTexture("MrBuck_selected", "Elementos_ingame/seleccion_personajes/paneles_seleccion_mrbuck_definitive.png");
  addGuiTexture("ElAfilador_selected", "Elementos_ingame/seleccion_personajes/paneles_seleccion_elafilador_definitive.png");
  addGuiTexture("Cythria_selected", "Elementos_ingame/seleccion_personajes/paneles_seleccion_cytrhia_definitive.png");
  addGuiTexture("ElRisas_selected", "Elementos_ingame/seleccion_personajes/paneles_seleccion_elrisas_definitive.png");

  // HUD Elements
  addGuiTexture("poder_local_0", "Elementos_ingame/hud/barra_de_poder_local_vacia.png");
  addGuiTexture("poder_local_1", "Elementos_ingame/hud/barra_de_poder_local_1.png");
  addGuiTexture("poder_local_2", "Elementos_ingame/hud/barra_de_poder_local_2.png");
  addGuiTexture("poder_local_3", "Elementos_ingame/hud/barra_de_poder_local_full.png");
  addGuiTexture("poder_visitante_0", "Elementos_ingame/hud/barra_de_poder_visitante_vacia.png");
  addGuiTexture("poder_visitante_1", "Elementos_ingame/hud/barra_de_poder_visitante_1.png");
  addGuiTexture("poder_visitante_2", "Elementos_ingame/hud/barra_de_poder_visitante_2.png");
  addGuiTexture("poder_visitante_3", "Elementos_ingame/hud/barra_de_poder_visitante_full.png");
  addGuiTexture("powerUpFuerza", "Elementos_ingame/hud/powerUp-fuerza.png");
  addGuiTexture("powerUpResistencia", "Elementos_ingame/hud/powerUp-resistencia.png");
  addGuiTexture("powerUpSuperTiro", "Elementos_ingame/hud/powerUp-superTiro.png");
  addGuiTexture("powerUpVelocidad", "Elementos_ingame/hud/powerUp-velocidad.png");

  // Score Elements
  addGuiTexture("score_vs", "Elementos_ingame/menu_principal/Modos_juego/VS.png");
  addGuiTexture("score_number_0", "Elementos_ingame/match_options/0.png");
  addGuiTexture("score_number_1", "Elementos_ingame/match_options/1.png");
  addGuiTexture("score_number_2", "Elementos_ingame/match_options/2.png");
  addGuiTexture("score_number_3", "Elementos_ingame/match_options/3.png");
  addGuiTexture("marcador", "Elementos_ingame/match_options/marcador.png");
  addGuiTexture("round1", "Elementos_ingame/match_options/round1.png");
  addGuiTexture("round2", "Elementos_ingame/match_options/round2.png");
  addGuiTexture("round3", "Elementos_ingame/match_options/round3.png");
  addGuiTexture("round4", "Elementos_ingame/match_options/round4.png");
  addGuiTexture("round5", "Elementos_ingame/match_options/round5.png");

  //Maps
  addGuiTexture("Volcan", "Elementos_ingame/seleccion_escenarios/escenario_inferno_volcano.png");
  addGuiTexture("Volcan_pre", "Elementos_ingame/seleccion_escenarios/escenario_inferno_volcano_derecha.png");
  addGuiTexture("Volcan_text", "Elementos_ingame/seleccion_escenarios/inferno_volcano.png");
  addGuiTexture("Estadio", "Elementos_ingame/seleccion_escenarios/escenario_malaka_stadium.png");
  addGuiTexture("Estadio_pre", "Elementos_ingame/seleccion_escenarios/escenario_malaka_stadium_derecha.png");
  addGuiTexture("Estadio_text", "Elementos_ingame/seleccion_escenarios/malaka_stadium.png");
  addGuiTexture("Jungla", "Elementos_ingame/seleccion_escenarios/escenario_jungle_dumped.png");
  addGuiTexture("Jungla_pre", "Elementos_ingame/seleccion_escenarios/escenario_jungle_dumped_derecha.png");
  addGuiTexture("Jungla_text", "Elementos_ingame/seleccion_escenarios/jungle_dumped.png");
  addGuiTexture("Luna", "Elementos_ingame/seleccion_escenarios/escenario_space_bum.png");
  addGuiTexture("Luna_pre", "Elementos_ingame/seleccion_escenarios/escenario_space_bum_derecha.png");
  addGuiTexture("Luna_text", "Elementos_ingame/seleccion_escenarios/space_bum.png");

}

unique_ptr<IGuiTexture>& Entity_Manager::getGuiTexture(const char* name)
{
    for(auto itr = guiTextures.begin(); itr != guiTextures.end(); itr++)
    {
        if(strcmp(itr->first, name) == 0)
            return itr->second;
    }
}

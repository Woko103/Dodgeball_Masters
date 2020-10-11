#include <mechanics.h>

void Mechanics::initialize(Entity_Manager* entity_manager, Facade2* facade, Sound_System* sound_engine) {
    // Establishing references
    if(!this->entity_manager)
      this->entity_manager = entity_manager;
    if(!this->facade)
      this->facade = facade;
    if(!this->sound_engine)
      this->sound_engine = sound_engine;
    multiplayerSpeedX = 0;
}

void Mechanics::update(){

    //Se inicializan una entidad pelota y un jugador
    IEntity* player = nullptr;
    IEntity* player2 = nullptr;
    IEntity* multiplayer = nullptr;
    IEntity* ball = nullptr;
    bool playerNumber = false;
    bool multiplayerIsDead = true;
    playerIsDead = true;
    player2IsDead = true;
    //Se inicializa una variable de puntuacion para controlar los puntos del juego
    auto& score = entity_manager->getScore();
    auto& entitiesMap = entity_manager->getMapEntities();
    bool stop = false;
    std::map < const char*, unique_ptr<IEntity> >::iterator itr;
    //Se recorre el bucle para saber que entidad del mapa es el jugador y cual es la pelota
    //Se le asignan estas entidades a las variables anteriores
    for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
    {
        auto& entity = itr->second;

        if(entity->control.esPlayer)
        {
            if (!playerNumber)
            {
                player = entity.get();
                playerNumber = true;
                player->control.esMultiplayer = false;
                playerIsDead = false;
            }
            else
            {
                player2 = entity.get();
                player2IsDead = false;
            }
        }

        if(entity->control.esMultiplayer)
        {
            multiplayer = entity.get();
            multiplayer->control.esJugador = false;
            multiplayerIsDead = false;
        }

        else if (entity->control.esPelota){
            if(ball == nullptr){
                ball = entity.get();
            }
            else{
              if(!inAir){
                facade->deleteCharacter(entity_manager, entity->id);
                checker = true;
                ball->t.checker = 0;
                stop = true;
              }
            }
            // Gravity simulation
            if(!pick && ball && !pass)
            {
              if(ball->position.Y > 0.2f)
                ball->position.Y -= 1.0f / 60.0f;
              else
                ball->position.Y = 0.2f;
            }
        }

        else if (entity->control.esJugador)
            move(entity.get());
    }

    //Metodos para actualizar al player
    if (!playerIsDead)
    {
      if(facade->checkMando()==0)
      {
        facade->updatePlayerMovement(player);
      }
      else
      {
        facade->updatePlayerMovementJoystick(player);
      }

      if (facade->compareParent(ball, player))
      {
          if(!strcmp(facade->checkInput(), "LEFTMOUSE")  && facade->checkMando()==0 || !strcmp(facade->checkInput(), "SP")  && facade->checkMando()==0 || !strcmp(facade->checkInput(), "SPA")  && facade->checkMando()==0 || !strcmp(facade->checkInput(), "SPD") && facade->checkMando()==0 || facade->checkMando()==1 && facade->triggers()==5)
          {
              throwBall(player, ball, player);
          }
          else if(!strcmp(facade->checkInput(), "Y") && facade->checkMando()==0 || facade->checkMando()==1 && facade->buttons()==5)
          {
            superThrow(player, ball, player);
          }
          else if(!strcmp(facade->checkInput(), "R") && facade->checkMando()==0 || !strcmp(facade->checkInput(), "RAW") && facade->checkMando()==0 || !strcmp(facade->checkInput(), "RAS") && facade->checkMando()==0 || !strcmp(facade->checkInput(), "RA")
                && facade->checkMando()==0 || !strcmp(facade->checkInput(), "RDW") && facade->checkMando()==0 || !strcmp(facade->checkInput(), "RDS") && facade->checkMando()==0 || !strcmp(facade->checkInput(), "RD") && facade->checkMando()==0 || !strcmp(facade->checkInput(), "RS")
                ||facade->checkMando()==1 && facade->triggers()==2){
                  passBall(player, ball, player);
                  facade->setAnim(player, 2);
                }
      }
      else
      {
        catchBall(player, ball);

        if (facade->compareParent(ball, player))
        {
          facade->setMultiplayerMuerto("catch");
        }
      }
    }

    if (!player2IsDead)
    {
        facade->updatePlayerMovement2(player2);
        if (facade->compareParent(ball, player2))
        {
            if(facade->checkInputPlayer2() == 'N' || facade->checkInputPlayer2() == 'V')
            {
                throwBall(player2, ball, player2);
            }
            else if(facade->checkInputPlayer2() == 'B'){
              passBall(player2, ball, player2);
              facade->setAnim(player, 2);
            }
        }
        else
            catchBall(player2, ball);
    }
    //Se comprueba si el balon esta en el aire para comprobar las eliminaciones, mover el balon y supertiros
    if(inAir){
        const char* del = facade->elimination();

        if(del != nullptr)
        {
          //cout << "Elimination name: " << del << endl;
        }
        //TODO, ver elimination asignacion
        if(ball->t.checker==0){
            moveBall(ball);
        }
        if(!playerIsDead && ball->t.checker > 0 && player->t.checker > 0){
            checkSuperThrow(ball, player);
        }
        if(!multiplayerIsDead && ball->t.checker > 0 && multiplayer->t.checker > 0){
            checkSuperThrow(ball, multiplayer);
        }

        checkDelete(player, del, playerIsDead, player2IsDead);
    }
    //Si el spawnTime es distinto de 0 y menor que el tiempo actual, se crea y se renderiza el nuevo powerUp
    if(spawnTime > 0 && spawnTime <= facade->getTime(0) && facade->getMultiplayerID() < 1){
      spawnTime = 0;
      //Se le asigna una posicion aleatoria al powerUp
      float pos = entity_manager->randomPos();
      entity_manager->addEntity("n_powerup_01", "", "meshes/powerUp/powerup.obj", pos, 0.25, 0, "NoTeam", false, false, false, false, true, 0, 0, "", false, 0, 0, 'N', 0, 0, "-", -1);
      IEntity* power = entity_manager->getEntity("n_powerup_01").get();
      facade->renderEnt(power);
      power->node.myNode->setRotation(vec3(90, 0, -90));
      facade->addCubeColliderToEntity((char *) power->id, power->node.myNode, 0.14, 1, 0.14, 0, 0, 0);
    }

    //Al pulsar la C pasamos a controlar a un companyero
    if ((!strcmp(facade->checkInput(), "C") && !playerIsDead && facade->getTime(0) >= changeTime && facade->checkMando()==0 && !facade->compareParent(ball, player)) || (facade->checkMando()==1 && facade->buttons()==4 && !playerIsDead && facade->getTime(0) >= changeTime && !facade->compareParent(ball, player)))
    {
        changeTime = facade->getTime(1);
        if (control2Ally(player->team.team))
        {
            player->control.esJugador = true;
            player->control.esPlayer = false;
        }
    }

    if (!player2IsDead && facade->checkInputPlayer2()==',' && facade->getTime(0) >= changeTime)
    {
        changeTime = facade->getTime(1);
        if (control2Ally(player2->team.team))
        {
            player2->control.esJugador = true;
            player2->control.esPlayer = false;
        }
    }

    int redMembers=0;
    int blueMembers=0;
    //Se recorre el bucle para contar las entidades de cada equipo
    for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++){
        auto& entity = itr->second;
        if(entity->team.team=="Red")
            redMembers++;
        if(entity->team.team=="Blue")
            blueMembers++;
        updateStamina(entity.get());
    }

    if(ball->position.Z < -3.4){
      ball->position.Z = -3.4;
    }
    else if(ball->position.Z > 3.4){
      ball->position.Z = 3.4;
    }

    if(!inAir && ball->position.Z < 0.15 && ball->position.Z > -0.15){
      if(ball->team.team == "Blue"){
        ball->position.Z = -0.15;
      }
      else{
        ball->position.Z = 0.15;
      }
    }

    if(ball->t.checker != 0){
      if(ball->position.X < -2.7){
        ball->position.X = -2.7;
      }
      else if(ball->position.X > 2.7){
        ball->position.X = 2.7;
      }
    }

    //Se comprueba que equipo no tiene jugadores para establecer la vitoria/derrota y resetear el partido
    if(redMembers==0){
        score->match.bluePoints++;
        score->match.finish=true;
        spawnTime = 0;
    }else if(blueMembers==0){
        score->match.redPoints++;
        score->match.finish=true;
        spawnTime = 0;
    }

    // Update indicators
    updateIndicators();
}


//--------------------------------UPDATESTAMINA----------------------------------------
void Mechanics::updateStamina(IEntity* entity)
{
    if (entity->control.esPlayer==true || entity->control.esJugador==true)
    {
        if(entity->stamina.stamina>=entity->stamina.staminaMax)
            entity->stamina.stamina=entity->stamina.staminaMax;
        else{
            //PONER TIMER
            if(entity->control.esPlayer && !facade->keyboardMoving() && !facade->joystickMoving()){
              entity->stamina.stamina += 2;
            }
            else{
              if(entity->control.esJugador && entity->speed.X == 0 && entity->speed.Z == 0){
                entity->stamina.stamina += 2;
              }
            }
        }
    }
    if (entity->stamina.stamina<=0)
        entity->stamina.stamina=0;

    float stamina = 1-((entity->stamina.staminaMax - entity->stamina.stamina) * 0.0004);
    entity->speed.power = entity->speed.powerMax * stamina;
    entity->dash.value = entity->dash.valueMax * stamina;
    entity->t.strength = entity->t.strengthMax * stamina;
    entity->c.hability = entity->c.habilityMax * stamina;

    if(entity->control.powered && entity->control.powerType == 2){
      if(entity->control.powerTime <= facade->getTime(0)){
        entity->control.powered = false;
      }
    }
}

void Mechanics::checkDelete(IEntity* player, const char* del, bool playerIsDead, bool player2IsDead){
  //Se comprueba si el nombre de la entidad no es nulo para eliminarla
  if(del != nullptr){
      //Se inicializa una entidad para asignarle el valor del powerUp, si hay
      IEntity* powerUp = nullptr;
      //Se recorre el mapa
      auto& entitiesMap = entity_manager->getMapEntities();
      std::map < const char*, unique_ptr<IEntity> >::iterator itr;
      for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++){
          auto& entityB = itr->second;
          //Si la entidad es la seleccionada para eliminar, se activa el audio de muerte
          if(itr->first == del)
          {
              facade->setMultiplayerMuerto(itr->first);
              itr->second->audio.paramValue=1;
              itr->second->audio.suena = true;
              sound_engine->playEventFromEntity(itr->second.get());
              entity_manager->getCharacter("HardSFX")->audio.paramValue = 2;
              entity_manager->getCharacter("HardSFX")->audio.suena = true;

          }
          //Se comprueba si la entidad a eliminar es el jugador para pasarle el control de un compañero
          if (entityB->control.esPlayer && itr->first==del)
          {
              if (entityB.get() == player)
              {
                playerIsDead = true;
              }
              else
              {
                player2IsDead = true;
              }

              control2Ally(entityB->team.team);
          }
          //Se comprueba si la entidad es un powerUp
          if(entityB->control.esPowerUp){
              powerUp = entityB.get();
          }
      }
      //Si la entidad a eliminar es un powerUp, se coge y se le pone un tiempo de aparicion
      if(powerUp != nullptr && del == powerUp->id && !pass){
          pickUpPowerUp(powerUp);
          spawnTime = facade->getTime(15);
      }
      if(!pass){
        facade->deleteCharacter(entity_manager, del);
      }
  }
}

//--------------------------------CATCHBALL----------------------------------------
void Mechanics::catchBall(IEntity* ent, IEntity* ball){
  //Se inicializan variables de equipo
  std::string& tBall = ball->team.team;
  std::string tPlayer = ent->team.team;

  if (!facade->compareParent(ent, ball))
  {
    //Se comprueba si la pelota no esta en el aire
    if(!inAir){
      //Si esta en el suelo, se comprueba si la entidad colisiona con la pelota y viceversa para cogerla automaticamente
      if(facade->compareCollision(ent, ball) || (ent->control.esMultiplayer && multiplayerSpeedX == 100)){
          pick = true;
          tBall = tPlayer;
          facade->createChild(ent, ball);
          if(lastPlayer != nullptr){
            lastPlayer->t.checker = 0;
          }
          lastPlayer = ent;
          if(ball->t.checker != 0){
          ball->t.checker = 0;
          ent->t.checker = 0;
          checker = true;
        }
        if(ent->control.esJugador){
          if(ent->difficulty.difficulty == "Easy"){
            ent->difficulty.actionTime = facade->getTime(2.5);
          }
          else if(ent->difficulty.difficulty == "Medium"){
            ent->difficulty.actionTime = facade->getTime(1.75);
          }
          else{
            ent->difficulty.actionTime = facade->getTime(1);
          }
        }
      }
    }
    else{
      //Comprueba si la pelota ha sido cogida, si no se ha fallado al cogerla, si no es un pase,
      // si no son del mismo equipo y si existe la pelota
      if(!pick && !missBall && !pass && ball != NULL && ent->team.team.compare(ball->team.team)){
        float factor = ent->c.grabRange - (ent->stamina.staminaMax-ent->stamina.stamina)*0.0004;
        bool enters = false;
        //Se comprueba si la entidad es el jugador
        if(ent->control.esPlayer){
          //Si lo es, se inicializan las variables de distancia y posicion
          float pBallZ = ball->position.Z;
          float pEntZ = ent->position.Z;
          float pBallX = ball->position.X;
          float pEntX = ent->position.X;
          //Comprueba si el balon se ha intentado coger en el area de fallo
          if((pBallX + ent->c.maxRange >= pEntX && pBallX + factor <= pEntX && pBallZ + ent->c.maxRange >= pEntZ && pBallZ - ent->c.maxRange <= pEntZ) ||
        (pBallX - ent->c.maxRange <= pEntX && pBallX - factor >= pEntX && pBallZ + ent->c.maxRange >= pEntZ && pBallZ - ent->c.maxRange <= pEntZ) ||
      (pBallZ + ent->c.maxRange >= pEntZ && pBallZ + factor <= pEntZ && pBallX + ent->c.maxRange >= pEntX && pBallX - ent->c.maxRange <= pEntX) ||
      (pBallZ - ent->c.maxRange <= pEntZ && pBallZ - factor >= pEntZ && pBallX + ent->c.maxRange >= pEntX && pBallX - ent->c.maxRange <= pEntX)){
            if(((!strcmp(facade->checkInput(), "E") && facade->checkMando() == 0) || (facade->checkMando() == 1 && facade->buttons() == 0)
              || facade->checkInputPlayer2() == 'M') && ent->control.esPlayer){
              //Se pone la variable de fallo a true y se pone el timer a 0
              missBall = true;
              missTime = facade->getTime(1);
            }
          }
          //Si no falla, se comprueba si se ha intentado coger en el area permitida
          if(!missBall){
            if((pBallZ - factor < pEntZ && pBallZ >= pEntZ && pBallX + factor > pEntX && pBallX <= pEntX) ||
            (pBallZ + factor> pEntZ && pBallZ <= pEntZ && pBallX - factor < pEntX && pBallX >= pEntX) ||
          (pBallZ - factor < pEntZ && pBallZ >= pEntZ && pBallX - factor < pEntX && pBallX >= pEntX) ||
        (pBallZ + factor > pEntZ && pBallZ <= pEntZ && pBallX + factor > pEntX && pBallX <= pEntX)){
              if(((!strcmp(facade->checkInput(), "E") && facade->checkMando() == 0) || (facade->checkMando() == 1 && facade->buttons() == 5)
              || facade->checkInputPlayer2() == 'M') && ent->control.esPlayer){
                  std::cerr << "yes" << '\n';
                enters = true;
              }
            }
          }
        }
        if(ent->control.esJugador || enters || (ent->control.esMultiplayer && multiplayerSpeedX == 100)){
          if(ball->position.Y < 1){
            //Se pone la variable de coger a true, el balon deja de estar en el aire y se aumenta la variable de supertiro
            pick = true;
            inAir = false;
            if(ent->t.superShot < 3){
              ent->t.superShot++;
            }
            ent->audio.paramValue = 0;
            ent->audio.suena=true;
            ent->stamina.stamina += 150;
            facade->setAnim(ent, 3);
            if(pass){
              pass = false;
            }
            if(ball->t.checker != 0){
              ball->t.checker = 0;
              ent->t.checker = 0;
              checker = true;
            }
            //Se cambia el equipo al balon, se le quita el animator y se pone al jugador como padre para que le siga
            tBall = tPlayer;
            facade->createChild(ent, ball);
            if(lastPlayer != nullptr){
              lastPlayer->t.checker = 0;
            }
            lastPlayer = ent;
          //Se comprueba si la comprobacion anterior ha fallado por ser un enemigo/npc
            if(ent->control.esJugador){
              if(ent->difficulty.difficulty == "Easy"){
                ent->difficulty.actionTime = facade->getTime(2.5);
              }
              else if(ent->difficulty.difficulty == "Medium"){
                ent->difficulty.actionTime = facade->getTime(1.75);
              }
              else{
                ent->difficulty.actionTime = facade->getTime(1);
              }
            }
          }
        }
      }
      else{
        //Se comprueba si el tiempo de fallo ha pasado para volver a hacer las comprobaciones de las areas
        if(missBall && facade->getTime(0) >= missTime){
          missBall = false;
        }
      }
    }
  }
}

//--------------------------------THROWBALL----------------------------------------
void Mechanics::throwBall(IEntity* ent, IEntity* ball, IEntity* player){

    //Se comprueba si al balon ha sido cogido y si la tecla T se pulsa

    if(pick && (ent == player || (ent != player && ent->team.team != "NoTeam" && facade->getTime(0) >= ent->difficulty.actionTime && ent->t.superShot < 3))){
        //El balon deja de cogerse, se pone en el aire, se le asigna la fuerza del jugador y se inicializa el tiempo de vuelo
        //El jugador deja de ser su padre y se le agregan de nuevo el animator y el triangle selector
        ent->audio.suena = true;
        ent->audio.paramValue = 4;
        ent->stamina.stamina -= 50;
        facade->setAnim(ent, 2);
        if(!player->control.powered || player->control.powerType != 2){
            player->stamina.stamina -= 50;
        }
        ball->audio.suena = true;

        pick = false;
        inAir = true;
        ball->t.strength = ent->t.strength;
        airTime = facade->getTime(1);
        //Esto elimina al nodo por completo, asi que hay que volver a crearlo
        facade->removeChild(ent, ball);
        fromPlayer = true;
        if(ent != player){
            if (player->control.esPowerUp){
              ent->target = player;
            }
            else{
              ent->target = closerTarget(ent, 'E');
            }
            ball->target = ent->target;

            speedX = (abs((ball->position.X - ent->target->position.X)));
            speedZ = (abs((ball->position.Z - ent->target->position.Z)));
            float distTot = speedX + speedZ;
            speedX = speedX / distTot;
            speedZ = speedZ / distTot;
            speedX = speedX * ball->t.strength;
            speedZ = speedZ * ball->t.strength;
            vec3 rotation;
            if(ent->target->position.X < ent->position.X){
                direction = 'L';
            }
            else{
                direction = 'R';
            }
            facade->setRotation(ent, rotation);
            fromPlayer = false;
            if(ent->control.esJugador){
              if(ent->difficulty.difficulty == "Easy"){
                ent->difficulty.actionTime = facade->getTime(2.5);
              }
              else if(ent->difficulty.difficulty == "Medium"){
                ent->difficulty.actionTime = facade->getTime(1.75);
              }
              else{
                ent->difficulty.actionTime = facade->getTime(1);
              }
            }
        }
        else{
          if (ent->control.esPlayer)
          {
            if(facade->checkMando()==0)
            {
                //EL ALGORITMO DE LAS 4:30
                float xProv = ball->t.strength * facade->getCursorX();
                float zProv = ball->t.strength * facade->getCursorY();
                float totalProv = abs(xProv) + abs(zProv);
                float factor = ball->t.strength/totalProv;
                speedX = xProv * factor;
                speedZ = zProv * factor;
            }
            else if(facade->checkMando()==1)
            {
              speedX = facade->getXAxisRJ() * ball->t.strength;
              speedZ = -(facade->getYAxisRJ()) * ball->t.strength;
            }
          }
          else if (ent->control.esMultiplayer)
          {
            speedX = ball->t.strength * multiplayerSpeedX;
            speedZ = ball->t.strength * multiplayerSpeedZ;
          }
        }
        if(ent->control.powered && ent->control.powerType == 0){
            if(ent->control.powerUses > 0){
                ent->control.powerUses --;
            }
            if(ent->control.powerUses == 0){
                ent->t.strengthMax = ent->t.initialStrength;
                ent->control.powered = false;
            }
        }
    }
}

void Mechanics::superThrow(IEntity* ent, IEntity* ball, IEntity* player)
{
  //Si la tecla Y se ha pulsado teniendo el balon y siendo un jugador, se comprueba el superlanzamiento
  if (pick && ent->t.superShot >= 3 && (ent == player ||
    (ent != player && ent->team.team != "NoTeam" && facade->getTime(0) >= ent->difficulty.actionTime)))
  {
    facade->setAnim(ent, 2);
    ent->target = closerTarget(ent, 'E');
    ball->target = ent->target;
    //Se compruba ha superado la variable de supertiro
    ent->audio.suena = true;
    ent->audio.paramValue = 7;
    ball->audio.suena = true;

    //Se resetea la varaible de contador de supertiro, el balon deja de estar cogido y se pone en el aire
    ent->t.superShot = 0;
    pick = false;
    inAir = true;

    //Se le pone la fuerza del que lanza al balon
    ball->t.strength = ent->t.strength;
    //El jugador deja de ser su padre y se le agregan de nuevo el animator y el triangle selector
    facade->removeChild(ent, ball);
    //Se almacena la ultima posicion del balon
    ball->position.lastX = ball->position.X;
    ball->position.lastY = ball->position.Y;
    ball->position.lastZ = ball->position.Z;
    ball->t.checker = 1;
    ent->t.checker = 1;
    if(ent->control.powered && ent->control.powerType == 3){
      ent->control.powered = false;
    }
    fromPlayer = true;
    if(ent != player){
      ent->status.est = 'N';
      speedX = ball->t.strength+0.1;
      speedZ = ball->t.strength+0.1;
    }
    else{
      speedX = ball->t.strength+0.1;
      speedZ = ball->t.strength+0.1;
    }
    if(ent->control.esJugador){
      if(ent->difficulty.difficulty == "Easy"){
        ent->difficulty.actionTime = facade->getTime(2.5);
      }
      else if(ent->difficulty.difficulty == "Medium"){
        ent->difficulty.actionTime = facade->getTime(1.75);
      }
      else{
        ent->difficulty.actionTime = facade->getTime(1);
      }
    }
  }
}

//--------------------------------MOVEBALL----------------------------------------
void Mechanics::moveBall(IEntity* ball){

  if(!pass) // The ball is being thrown
  {
    if(facade->isEntityColliding(ball)) // The ball hits something
    {
      char* collisionName = facade->getCollisionName(ball);

      if(strcmp(collisionName, "LateralWall") == 0)
      {
        speedX = -speedX;
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 6;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
      }
      
      else if(strcmp(collisionName, "CentralWall") == 0)
      {
        inAir = false;
        ball->team.team = "NoTeam";
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 6;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
      }
      
    }
    else if(airTime <= facade->getTime(0)) // The ball is NOT hitting anything but
    {                                   // air time is over and the ball falls to the ground
      inAir = false;
      ball->team.team = "NoTeam";
    }

    if(inAir) // The balls does NOT hit anything and it is still in the air
    {
      if(ball->team.team.compare("Blue") == 0) // The ball was thrown by the ally team
        ball->position.Z -= speedZ;
      else // The ball was thrown by the enemy team
        ball->position.Z += speedZ;

      if(fromPlayer) //
      {
        if(ball->team.team.compare("Blue") == 0)
          ball->position.X += speedX;
        else
          ball->position.X -= speedX;
      }
      else
      {
        if(direction == 'R')
          ball->position.X += speedX;
        else
          ball->position.X -= speedX;
      }
    }
  }
  else // The ball is being passed
  {
    // Move ball in X towards the target position
    if(ball->target->position.X > ball->position.X)
      ball->position.X += speedX;
    else
      ball->position.X -= speedX;

    // Move ball in Z towards the target position
    if(ball->target->position.Z > ball->position.Z)
      ball->position.Z += speedZ;
    else
      ball->position.Z -= speedZ;

    if(facade->compareCollision(ball, ball->target)) // Ball hits the target of the pass
    {
      inAir = false; // Ball is not in air anymore
      pass = false; // Ball is not being passed anymore
      pick = true; // Ball is picked by the target

      facade->createChild(ball->target, ball); // Set ball as child of the target

      lastPlayer = ball->target;

      dropTime = facade->getTime(1);

      // Set time for the next throw based on the difficulty of the AI Entity (Player will be AI sometimes)
      if(ball->target->difficulty.difficulty == "Easy"){
        ball->target->difficulty.actionTime = facade->getTime(2.5);
      }
      else if(ball->target->difficulty.difficulty == "Medium"){
        ball->target->difficulty.actionTime = facade->getTime(1.75);
      }
      else{
        ball->target->difficulty.actionTime = facade->getTime(1);
      }
    }
  }
}

//--------------------------------CHECKSUPERTHROW----------------------------------------
void Mechanics::checkSuperThrow(IEntity* ball, IEntity* ent){
  switch (ent->t.superThrowType) {
    case 1:
    superThrow1(ball, ent);
    break;
    case 2:
    superThrow2(ball, ent);
    break;
    case 3:
    superThrow3(ball, ent);
    break;
    case 4:
    superThrow4(ball, ent);
    break;
    case 5:
    superThrow5(ball, ent);
    break;
    case 6: superThrow6(ball, ent);
    break;
  }
}

//--------------------------------SUPERTHROW1----------------------------------------
void Mechanics::superThrow1(IEntity* ball, IEntity* player){
  //Se comprueba si se trata de un pase
  if(facade->isEntityColliding(ball)){
    inAir = false;
    ball->team.team = "NoTeam";
    ball->t.checker = 0;
    player->t.checker = 0;
    checker = true;
  }
  //Se comprueba que el balon este en el aire
  else{
    //Se comprueba si el tiempo de vuelo ha sido superado
      if(player->team.team == "Blue"){
        //Si sigue el vuelo, se actualiza su posicion en base a la fuerza de lanzamiento y se le quita el animator
        if(ball->position.lastZ <= ball->position.Z + 0.44){
          ball->position.Z -= speedZ;
        }
        else{
          if(ball->position.lastX >= ball->position.X - 0.44 && checker){
            ball->position.X += speedX;
          }
          else if(ball->position.lastX <= ball->position.X + 0.44 && !checker){
            ball->position.X -= speedX;
          }
          else{
            if(checker){
              checker = false;
            }
            else{
              checker = true;
            }
            ball->position.lastX=ball->position.X;
            ball->position.lastZ=ball->position.Z;
          }
        }
      }
      else{
        if(ball->position.lastZ >= ball->position.Z - 0.2){
          ball->position.Z += speedZ;
        }
        else{
          if(ball->position.lastX >= ball->position.X - 0.2 && checker){
            ball->position.X += speedX;
          }
          else if(ball->position.lastX <= ball->position.X + 0.2 && !checker){
            ball->position.X -= speedX;
          }
          else{
            if(checker){
              checker = false;
            }
            else{
              checker = true;
            }
            ball->position.lastX=ball->position.X;
            ball->position.lastZ=ball->position.Z;
          }
        }
      }
    }
}

//--------------------------------SUPERTHROW2----------------------------------------
void Mechanics::superThrow2(IEntity* ball, IEntity* ent){
      //Se comprueba si ha llegado al tope de altura para aumentar su Y
      if(ball->position.Y < 1.52 && checker){
        ball->position.Y += ent->t.strength / 4;
        //Se comprueba el equipo de la pelota para saber en que direccion moverse
        if(ent->team.team == "Red"){
          ball->position.Z += ent->t.strength;
        }
        else{
          ball->position.Z -= ent->t.strength;
        }
      }
      //Cuando ya ha llegado al tope, se comprueba si ha llegado al suelo para dejar de disminuir la Y
      else{
        checker = false;
        if(ball->position.Y >= 0.52){
          ball->position.Y -= 0.1;
        }
        //Cuando ya ha llegado, se recorre el mapa de entidades
        else{
          auto& entitiesMap = entity_manager->getMapEntities();
          std::map < const char*, unique_ptr<IEntity> >::iterator itr;
          //Se recorre el bucle para saber que entidad del mapa es el jugador y cual es la pelota
          //Se le asignan estas entidades a las variables anteriores
          //Se crea un vector para almacenar las entidades a eliminar, porque si borramos directamente del mapa
          // en el bucle, se producen errores al seguir recorriendolo
          vector<const char*> deletes;
          for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
          {
              auto& entity = itr->second;

              //Se crea un vector para almacenar la posicion de la entidad
              vec3 entPos = vec3(entity->position.X, entity->position.Y, entity->position.Z);

              //Se comprueba si hay alguna entidad dentro del radio de eliminacion del supertiro
              if(entity->team.team != ent->team.team && ent->team.team != "NoTeam" &&
              ((ball->position.Z - 0.6 < entPos.z && ball->position.Z >= entPos.z && ball->position.X + 0.6 > entPos.x && ball->position.X <= entPos.x) ||
              (ball->position.Z + 0.6 > entPos.z && ball->position.Z <= entPos.z && ball->position.X - 0.6 < entPos.x && ball->position.X >= entPos.x) ||
            (ball->position.Z - 0.6 < entPos.z && ball->position.Z >= entPos.z && ball->position.X - 0.6 < entPos.x && ball->position.X >= entPos.x) ||
          (ball->position.Z + 0.6 > entPos.z && ball->position.Z <= entPos.z && ball->position.X + 0.6 > entPos.x && ball->position.X <= entPos.x)) &&
        entity.get() != ball){
                deletes.push_back(entity->id);
                //Si el jugador va a ser eliminado, se hace un cambio de control
                if(entity->control.esPlayer){
                  control2Ally(entity->team.team);
                }
              }
          }
          if(deletes.size() > 0){
            for(int i = 0; i < deletes.size(); i++){
              //Se elimina a los jugadores dentro del area
              entity_manager->getEntityByName(deletes[i])->audio.paramValue=1;
              entity_manager->getEntityByName(deletes[i])->audio.suena = true;
              sound_engine->playEventFromEntity(entity_manager->getEntityByName(deletes[i]).get());
              facade->deleteCharacter(entity_manager, deletes[i]);
            }
          }
          //El balon deja de estar en el aire, se le devuelve el animator, se le quita el equipo y se cambia la variable de supertiro
          inAir = false;
          entity_manager->getCharacter("HardSFX")->audio.paramValue = 2;
          entity_manager->getCharacter("HardSFX")->audio.suena = true;
          facade->setAnimator(ball);
          ball->team.team = "NoTeam";
          ball->t.checker = 0;
          ent->t.checker = 0;
          checker = true;
        }
      }
}

//--------------------------------SUPERTHROW3----------------------------------------
void Mechanics::superThrow3(IEntity* ball, IEntity* ent){
  //Se comprueba el equipo de la pelota para darle la direccion y se comprueba el limite del campo para saber
  //hasta donde se tiene que mover con cierta velocidad
  if(ball->team.team == "Blue" && ball->position.Z > 0.25 && checker){
    ball->position.Z -= ent->t.strength / 2;
  }
  else if(ball->team.team == "Red" && ball->position.Z < -0.25 && checker){
    ball->position.Z += ent->t.strength / 2;
  }
  //Se comprueba si se ha alcanzado el limite para asignarle un tiempo de espera
  if(launchTime <= facade->getTime(0) && checker && ((ball->team.team == "Blue" && ball->position.Z <= 0.25) ||
  (ball->team.team == "Red" && ball->position.Z >= -0.25))){
    launchTime = facade->getTime(1);
    checker = false;
  }
  if(!checker && launchTime <= facade->getTime(0)){
    //Se comprueba si la pelota ha colisionado para detenerla
    if(facade->isEntityColliding(ball)){
      inAir = false;
      facade->setAnimator(ball);
      ball->team.team = "NoTeam";
      ball->t.checker = 0;
      ent->t.checker = 0;
      checker = true;
    }
    else{
      //Una vez pasado el tiempo, se le asigna un objetivo la primera vez que entra
      if(ball->t.checker == 1){
        ent->target = closerTarget(ent, 'E');
        speedX = (abs((ball->position.X - ent->target->position.X)));
        speedZ = (abs((ball->position.Z - ent->target->position.Z)));
        float distTot = speedX + speedZ;
        speedX = speedX / distTot;
        speedZ = speedZ / distTot;
        speedX *= 0.4;
        speedZ *= 0.4;
        if(ent->target->position.X < ent->position.X){
          direction = 'L';
        }
        else{
          direction = 'R';
        }
        ball->t.checker = 2;
      }
      //Se le asigna velocidad en la X segun el lado en el que estuviese el objetivo y se le asigna velocidad en Z
      if(direction == 'R'){
        ball->position.X += speedX;
      }
      else{
        ball->position.X -= speedX;
      }
      if(ball->team.team == "Blue"){
        ball->position.Z -= speedZ;
      }
      else{
        ball->position.Z += speedZ;
      }
    }
  }
}

//--------------------------------SUPERTHROW4----------------------------------------
void Mechanics::superThrow4(IEntity* ball, IEntity* ent){
    //Se compruena si ha habio una colision para detener la pelota y destruir las adyacentes
    if(facade->isEntityColliding(ball)){
      inAir = false;
      ball->team.team = "NoTeam";
      ball->t.checker = 0;
      ent->t.checker = 0;
      checker = true;
    }
    else{
      //se compureba si es la primera vez que se entra para generar la 2 pelotas adicionales
      if(checker){
        entity_manager->addEntity("zzBall2", "zzBall2", "meshes/pelota/pelota.obj", ball->position.X + 0.1, ball->position.Y, ball->position.Z, ball->team.team, false, false, false, true, false, 0, 0, "event:/Hard_SFX/Lanza", false, 0, 0.04, 'N', 0, 0, "-", -1);
        IEntity* ball2 = entity_manager->getEntity("zzBall2").get();
        facade->renderEnt(ball2);
        facade->addCubeColliderToEntity((char *) ball2->id, ball2->node.myNode, 0.07, 0.07, 0.07, 0, 0, 0);
        entity_manager->addEntity("zzBall3", "zzBall3", "meshes/pelota/pelota.obj", ball->position.X - 0.1, ball->position.Y, ball->position.Z, ball->team.team, false, false, false, true, false, 0, 0, "event:/Hard_SFX/Lanza", false, 0, 0.04, 'N', 0, 0, "-", -1);
        IEntity* ball3 = entity_manager->getEntity("zzBall3").get();
        facade->renderEnt(ball3);
        facade->addCubeColliderToEntity((char *) ball3->id, ball3->node.myNode, 0.07, 0.07, 0.07, 0, 0, 0);
        checker = false;
      }
      ////Se crean variables para las entidades nuevas y otras dos con la posicion de las pelotas
      auto& entitiesMap = entity_manager->getMapEntities();
      std::map < const char*, unique_ptr<IEntity> >::iterator itr;
      //Se recorre el bucle para saber que entidad del mapa es el jugador y cual es la pelota
      //Se le asignan estas entidades a las variables anteriores
      bool stop = false;
      for(itr = entitiesMap.begin(); itr != entitiesMap.end() && !stop; itr++){
          auto& entity = itr->second;

          if(entity->control.esPelota){
              if(facade->isEntityColliding(entity.get()) && entity.get() != ball){
                const char* del = facade->elimination();
                stop = true;
                if(del != NULL){
                  bool stop2 = false;
                  for(itr = entitiesMap.begin(); itr != entitiesMap.end() && !stop2; itr++)
                  {
                      auto& entityB = itr->second;

                      if(!entityB->control.esPelota && entityB->id == del){
                        stop2 = true;
                      }
                  }
                }
                facade->deleteCharacter(entity_manager, entity->id);
              }
            }
          }
          for(itr = entitiesMap.begin(); itr != entitiesMap.end() && !stop; itr++){
              auto& entity = itr->second;
              if(entity->control.esPelota){
                if(ball->team.team == "Blue" ){
                  entity->position.Z -= 0.12;
                }
                else{
                  entity->position.Z += 0.12;
                }
                if(entity.get() != ball && entity->position.X < ball->position.X){
                  entity->position.X -= 0.01;
                }
                else if(entity.get() != ball && entity->position.X > ball->position.X){
                  entity->position.X += 0.01;
                }
              }
          }
    }
}

//--------------------------------SUPERTHROW5----------------------------------------
void Mechanics::superThrow5(IEntity* ball, IEntity* ent){
  //Si la pelota y el lanzador colisionan, entonces el balon se para y coge automaticamente al caer al suelo
  if(facade->isEntityColliding(ball) && facade->isEntityColliding(ent)){
    inAir = false;
    facade->setAnimator(ball);
    ball->team.team = "NoTeam";
    ball->t.checker = 0;
    ent->t.checker = 0;
    checker = true;
  }
  else{
    //Dependiendo del equipo del lanzador, se le asigna un rango de ida y vuelta y el signo de la velocidad
    if(ent->team.team == "Blue"){
      if(ball->position.Z > -3.4 && checker){
        ball->position.Z -= 0.16;
      }
      else{
        //Se le cambia el valor del checker para que la pelota vaya en la otra direccion
        if(checker){
          checker = false;
        }
        ball->position.Z += 0.16;
        //Se comprueba si el el checker de la entidad ha llegado al numero de vueltas para repetir el ciclo
        if(ball->position.Z > ent->position.Z - 0.3 && ent->t.checker < 2){
          checker = true;
          ent->t.checker ++;
        }
      }
    }
    //Lo mismo para el otro equipo
    else{
      if(ball->position.Z < 3.4 && checker){
        ball->position.Z += 0.16;
      }
      else{
        if(checker){
          checker = false;
        }
        ball->position.Z -= 0.16;
        if(ball->position.Z < ent->position.Z + 0.3 && ent->t.checker < 2){
          checker = true;
          ent->t.checker ++;
        }
      }
    }
    ball->position.X = ent->position.X;
  }
}

//--------------------------------SUPERTHROW5----------------------------------------
void Mechanics::superThrow6(IEntity* ball, IEntity* ent){
  //Se comprueba que el checker es true para la primera parte del tiro
  //Se comprueba que el checker es true para la primera parte del tiro
  if(!checker){
    if(facade->isEntityColliding(ball)){
      auto& entitiesMap = entity_manager->getMapEntities();
      std::map < const char*, unique_ptr<IEntity> >::iterator itr;
      bool stop = false;
      for(itr = entitiesMap.begin(); itr != entitiesMap.end() && !stop; itr++){
          auto& entity = itr->second;
          if(strcmp(facade->getCollisionName(ball), entity->id) == 0 && entity->team.team != ball->team.team){
            stop = true;
            inAir = false;
            ball->team.team = "NoTeam";
            ball->t.checker = 0;
            ent->t.checker = 0;
            checker = true;
          }
      }
    }
    else{
      auto& entitiesMap = entity_manager->getMapEntities();
      std::map < const char*, unique_ptr<IEntity> >::iterator itr;
      //Se recorre el bucle para saber que entidad del mapa es el jugador y cual es la pelota
      //Se le asignan estas entidades a las variables anteriores
      bool stop = false;
      //Se comprueba el equipo y si la pelota ha llegado al limite del campo en cuestion para aumentar la Z
      for(itr = entitiesMap.begin(); itr != entitiesMap.end() && !stop; itr++){
          auto& entity = itr->second;

          if(entity->control.esPelota){
              if(facade->isEntityColliding(entity.get()) && entity.get() != ball){
                const char* del = facade->elimination();
                stop = true;
                bool good = false;
                if(del != NULL){
                  for(itr = entitiesMap.begin(); itr != entitiesMap.end() && !good; itr++)
                  {
                      auto& entityB = itr->second;

                      if(!entityB->control.esPelota && entityB->team.team != ball->team.team && entityB->id == del){
                        good = true;
                      }
                  }
                }
                if(good){
                  facade->deleteCharacter(entity_manager, entity->id);
                }
              }
            }
          }
        for(itr = entitiesMap.begin(); itr != entitiesMap.end() && !stop; itr++){
            auto& entity = itr->second;
            if(entity->control.esPelota){
              if(ball->team.team == "Blue" ){
                entity->position.Z -= 0.12;
              }
              else{
                entity->position.Z += 0.12;
              }
              if(entity.get() != ball && entity->position.X < ball->position.X){
                entity->position.X = ent->position.X + cos(angle * 0.3) * 0.85;
              }
              else if(entity.get() != ball && entity->position.X > ball->position.X){
                entity->position.X = ent->position.X - cos(angle * 0.3) * 0.85;
              }
            }
        }
        angle ++;
    }
}
else{
  checker = false;
    entity_manager->addEntity("zzBall2", "zzBall2", "meshes/pelota/pelota.obj", ball->position.X + 0.1, ball->position.Y, ball->position.Z, "NoTeam", false, false, false, true, false, 0, 0, "event:/Hard_SFX/Lanza", false, 0, 0.04, 'N', 0, 0, "-", -1);
    IEntity* ball2 = entity_manager->getEntity("zzBall2").get();
    facade->renderEnt(ball2);
    facade->addCubeColliderToEntity((char *) ball2->id, ball2->node.myNode, 0.07, 0.07, 0.07, 0, 0, 0);
    entity_manager->addEntity("zzBall3", "zzBall3", "meshes/pelota/pelota.obj", ball->position.X - 0.1, ball->position.Y, ball->position.Z, "NoTeam", false, false, false, true, false, 0, 0, "event:/Hard_SFX/Lanza", false, 0, 0.04, 'N', 0, 0, "-", -1);
    IEntity* ball3 = entity_manager->getEntity("zzBall3").get();
    facade->renderEnt(ball3);
    facade->addCubeColliderToEntity((char *) ball3->id, ball3->node.myNode, 0.07, 0.07, 0.07, 0, 0, 0);
    if(ball->team.team == "Blue"){
      angle = 180;
    }
    else{
      angle = 0;
    }
  }
}

//--------------------------------PASSBALL----------------------------------------
void Mechanics::passBall(IEntity* ent, IEntity* ball, IEntity* compa){
    if(!pass && pick && ent == compa)
    {
        ent->target = checkTarget(ent);

        if (ent->target==nullptr)
            ent->target = closerTarget(ent, 'A');

        if (ent->target!=nullptr)
        {
            ball->audio.suena=true;
            pick = false;
            inAir = true;
            pass = true;
            facade->removeChild(ent, ball);
            speedX = (abs(ball->position.X - ent->target->position.X))/50;
            speedZ = (abs(ball->position.Z - ent->target->position.Z))/50;
        }
    }
    else if(pick && ent->control.esJugador && facade->getTime(0) >= ent->difficulty.actionTime){
        if(facade->getTime(0) > dropTime){
            ball->audio.suena=true;
            pick = false;
            inAir = true;
            pass = true;
            ent->target = compa;
            facade->removeChild(ent, ball);
            speedX = (abs(ball->position.X - ent->target->position.X))/50;
            speedZ = (abs(ball->position.Z - ent->target->position.Z))/50;
            if(ent->difficulty.difficulty == "Easy"){
              ent->difficulty.actionTime = facade->getTime(2.5);
            }
            else if(ent->difficulty.difficulty == "Medium"){
              ent->difficulty.actionTime = facade->getTime(1.75);
            }
            else{
              ent->difficulty.actionTime = facade->getTime(1);
            }
        }
    }
    ball->target = ent->target;
}

//--------------------------------CHECKTARGET---------------------------------------
IEntity* Mechanics::checkTarget(IEntity* player)
{
    IEntity* entBuena = nullptr;
    float x = 9999;
    float xTemp;

    //Si pulsas pasar o pasar y W
    if (!strcmp(facade->checkInput(), "R") && facade->checkMando()==0 || facade->checkMando() == 1 && facade->triggers() == 2)
    {
        auto& entitiesMap = entity_manager->getMapEntities();
        for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
        {
            auto& entity = itr->second;
            if (entity->team.team == player->team.team && entity->control.esJugador && entity->position.Z > player->position.Z)
            {
                xTemp = abs(abs(player->position.X) - abs(entity->position.X));
                if (xTemp < x)
                {
                    x = xTemp;
                    entBuena = entity.get();
                }
            }
        }
    }

    //Si pulsas S al pasar
    else if (!strcmp(facade->checkInput(), "RS"))
    {
        auto& entitiesMap = entity_manager->getMapEntities();
        for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
        {
            auto& entity = itr->second;
            if (entity->team.team == player->team.team && entity->control.esJugador && entity->position.Z < player->position.Z)
            {
                xTemp = abs(abs(entity->position.X) - abs(player->position.X));
                if (xTemp < x)
                {
                    x = xTemp;
                    entBuena = entity.get();
                }
            }
        }
    }

    //Si pulsas A al pasar
    else if (!strcmp(facade->checkInput(), "RA"))
    {
        auto& entitiesMap = entity_manager->getMapEntities();
        for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
        {
            auto& entity = itr->second;
            if (entity->team.team == player->team.team && entity->control.esJugador && entity->position.X < player->position.X)
            {
                xTemp = abs(abs(player->position.Z) - abs(entity->position.Z));
                if (xTemp < x)
                {
                    x = xTemp;
                    entBuena = entity.get();
                }
            }
        }
    }

    //Si pulsas D al pasar
    else if (!strcmp(facade->checkInput(), "RD"))
    {
        auto& entitiesMap = entity_manager->getMapEntities();
        for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
        {
            auto& entity = itr->second;
            if (entity->team.team == player->team.team && entity->control.esJugador && entity->position.X > player->position.X)
            {
                xTemp = abs(abs(player->position.Z) - abs(entity->position.Z));
                if (xTemp < x)
                {
                    x = xTemp;
                    entBuena = entity.get();
                }
            }
        }
    }

    //Si pulsas A y W al pasar
    else if (!strcmp(facade->checkInput(), "RAW"))
    {
        auto& entitiesMap = entity_manager->getMapEntities();
        for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
        {
            auto& entity = itr->second;
            if (entity->team.team == player->team.team && entity->control.esJugador && entity->position.X < player->position.X && entity->position.Z > player->position.Z)
            {
                xTemp = abs(abs(abs(player->position.Z) - abs(entity->position.Z)) - abs(abs(player->position.X) - abs(entity->position.X)));
                if (xTemp < x)
                {
                    x = xTemp;
                    entBuena = entity.get();
                }
            }
        }
    }

    //Si pulsas D y W al pasar
    else if (!strcmp(facade->checkInput(), "RDW"))
    {
        auto& entitiesMap = entity_manager->getMapEntities();
        for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
        {
            auto& entity = itr->second;
            if (entity->team.team == player->team.team && entity->control.esJugador && entity->position.X > player->position.X && entity->position.Z > player->position.Z)
            {
                xTemp = abs(abs(abs(player->position.Z) - abs(entity->position.Z)) - abs(abs(player->position.X) - abs(entity->position.X)));
                if (xTemp < x)
                {
                    x = xTemp;
                    entBuena = entity.get();
                }
            }
        }
    }

    //Si pulsas A y S al pasar
    else if (!strcmp(facade->checkInput(), "RAS"))
    {
        auto& entitiesMap = entity_manager->getMapEntities();
        for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
        {
            auto& entity = itr->second;
            if (entity->team.team == player->team.team && entity->control.esJugador && entity->position.X < player->position.X && entity->position.Z < player->position.Z)
            {
                xTemp = abs(abs(abs(player->position.Z) - abs(entity->position.Z)) - abs(abs(player->position.X) - abs(entity->position.X)));
                if (xTemp < x)
                {
                    x = xTemp;
                    entBuena = entity.get();
                }
            }
        }
    }

    //Si pulsas D y S al pasar
    else if (!strcmp(facade->checkInput(), "RDS"))
    {
        auto& entitiesMap = entity_manager->getMapEntities();
        for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
        {
            auto& entity = itr->second;
            if (entity->team.team == player->team.team && entity->control.esJugador && entity->position.X > player->position.X && entity->position.Z < player->position.Z)
            {
                xTemp = abs(abs(abs(player->position.Z) - abs(entity->position.Z)) - abs(abs(player->position.X) - abs(entity->position.X)));
                if (xTemp < x)
                {
                    x = xTemp;
                    entBuena = entity.get();
                }
            }
        }
    }

    return entBuena;
}

//--------------------------------DODGEBALL----------------------------------------
void Mechanics::dodgeBall(IEntity* ent, IEntity* ball, IEntity* player){
  if(ent->dash.direction == '-'){
    if(ball->position.X > ent->position.X){
      ent->dash.direction = 'L';
      ent->dash.posFin = ent->position.X - ent->dash.value * 28;
    }
    else{
      ent->dash.direction = 'R';
      ent->dash.posFin = ent->position.X + ent->dash.value * 28;
    }
  }
  if(ent->dash.dodgeTime > facade->getTime(0)){
      if(ent->dash.direction == 'L'){
        if(ent->dash.posFin > -2.6){
          if(ent->position.X > -2.55){
            ent->position.X -= ent->dash.value;
            ent->node.myNode->setRotation(vec3 (0, -90, 0));
          }
        }
        else{
          if(ent->position.X < 2.55){
            ent->position.X += ent->dash.value;
            ent->node.myNode->setRotation(vec3 (0, 90, 0));
          }
        }
      }
      else if(ent->dash.direction == 'R'){
        if(ent->dash.posFin > 2.55){
          if(ent->position.X > -2.55){
            ent->position.X -= ent->dash.value;
            ent->node.myNode->setRotation(vec3 (0, -90, 0));
          }
        }
        else{
          if(ent->position.X < 2.55){
            ent->position.X += ent->dash.value;
            ent->node.myNode->setRotation(vec3 (0, 90, 0));
          }
        }
      }
  }
}

//--------------------------------GOTOBALL----------------------------------------
void Mechanics::goToBall(IEntity* ent, IEntity* ball)
{
    if(!inAir && !pick)
    {
        //Calculamos la distancia en X, en Z y en total hasta la pelota
        float speedX = abs(abs(ent->position.X) - abs(ball->position.X));
        float speedZ = abs(abs(ent->position.Z) - abs(ball->position.Z));
        float dist = speedX + speedZ;

        //Calculamos el porcentaje de distancia en X y en Z para ajustar la velocidad en esos ejes
        speedX = speedX/dist;
        speedZ = speedZ/dist;

        //Asignamos las velocidades en X
        if(ball->position.X < ent->position.X)
            ent->speed.X = -(speedX * ent->speed.power);
        else if(ball->position.X > ent->position.X)
            ent->speed.X = speedX * ent->speed.power;
        else ent->speed.X = 0;

        //Asignamos las velocidades en Z
        if(ball->position.Z < ent->position.Z)
            ent->speed.Z = -(speedZ * ent->speed.power);
        else if(ball->position.Z > ent->position.Z)
            ent->speed.Z = speedZ * ent->speed.power;
        else ent->speed.Z = 0;
    }
}

bool Mechanics::getInAir(){
  return inAir;
}

bool Mechanics::getPlayerIsDead(){
  return playerIsDead;
}

bool Mechanics::getPlayer2IsDead(){
  return player2IsDead;
}

double Mechanics::getSpawnTime(){
  return spawnTime;
}

bool Mechanics::getPass(){
  return pass;
}

//--------------------------------CONTROL2ALLY----------------------------------------
bool Mechanics::control2Ally(std::string team)
{
    auto& entitiesMap = entity_manager->getMapEntities();
    bool playercico = false;

    for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
    {
        auto& entity = itr->second;
        if (entity->control.esPlayer && entity->team.team == team)
            playercico = true;
        if(entity->control.esJugador && !entity->control.esPlayer && !entity->control.esMultiplayer && entity->team.team==team && playercico)
        {
            entity->control.esJugador = false;
            entity->control.esPlayer = true;
            return true;
        }
    }

    for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
    {
        auto& entity = itr->second;
        if(entity->control.esJugador && !entity->control.esPlayer && !entity->control.esMultiplayer && entity->team.team==team)
        {
            entity->control.esJugador = false;
            entity->control.esPlayer = true;
            return true;
        }
    }

    return false;
}

//--------------------------------PICKUPPOWERUP----------------------------------------
void Mechanics::pickUpPowerUp(IEntity* powerUp){
  int tipo = 0;
  srand (time(NULL));
  tipo = rand() % 4;
  switch (tipo) {
    case 0:
    lastPlayer->t.strengthMax = lastPlayer->t.strengthMax + 0.05;
    if(lastPlayer->t.strengthMax >= 0.2){
      lastPlayer->t.strengthMax = 0.2;
    }
    lastPlayer->control.powerUses = 3;
    break;
    case 1:
    lastPlayer->speed.powerMax = lastPlayer->speed.powerMax + 0.05;
    if(lastPlayer->speed.powerMax >= 0.10){
      lastPlayer->speed.powerMax = 0.1;
    }
    lastPlayer->control.powerTime = facade->getTime(10);
    break;
    case 2:
    lastPlayer->stamina.stamina = lastPlayer->stamina.staminaMax;
    lastPlayer->control.powerTime = facade->getTime(20);
    break;
    case 3:
    lastPlayer->t.superShot = 3;
    break;
  }
  lastPlayer->control.powered = true;
  lastPlayer->control.powerType = tipo;
}

//--------------------------------CLOSERTARGET----------------------------------------
IEntity* Mechanics::closerTarget(IEntity* ent, char targ){
  IEntity* closer = nullptr;
  float distX = 0;
  float distZ = 0;
  float distTot = 0;
  float distCloserX = 0;
  float distCloserZ = 0;
  float distCloserTot = 0;
  auto& entitiesMap = entity_manager->getMapEntities();
  auto& ball = entity_manager->getEntity("n_ball_01");
  std::map < const char*, unique_ptr<IEntity> >::iterator itr;
  for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++){
    auto& entity = itr->second;
    if(targ == 'E'){
      if(entity->team.team.compare(ent->team.team) && entity->team.team.compare("NoTeam") && entity != ball){
        if(closer == nullptr){
          closer = entity.get();
          distCloserX = abs(ent->position.X - entity->position.X);
          distCloserZ = abs(ent->position.Z - entity->position.Z);
          distCloserTot = distCloserX + distCloserZ;
        }
        else{
          distX = abs(ent->position.X - entity->position.X);
          distZ = abs(ent->position.Z - entity->position.Z);
          distTot = distX + distZ;
          if(distTot < distCloserTot){
            closer = entity.get();
            distCloserX = distX;
            distCloserZ = distZ;
            distCloserTot = distTot;
          }
        }
      }
    }
    else{
      if(!entity->team.team.compare(ent->team.team) && !entity->control.esPelota && entity.get() != ent  && entity != ball){
        if(closer == nullptr){
          closer = entity.get();
          distCloserX = abs(ball->position.X - entity->position.X);
          distCloserZ = abs(ball->position.Z - entity->position.Z);
          distCloserTot = distCloserX + distCloserZ;
        }
        else{
          distX = abs(ball->position.X - entity->position.X);
          distZ = abs(ball->position.Z - entity->position.Z);
          distTot = distX + distZ;
          if(distTot < distCloserTot){
            closer = entity.get();
            distCloserX = distX;
            distCloserZ = distZ;
            distCloserTot = distTot;
          }
        }
      }
    }
  }
  return closer;
}

//--------------------------------MOVE----------------------------------------
void Mechanics::move(IEntity* entity)
{
    vec3  rotation;
    vec3  entPos;
    bool check = false;

    if(entity->control.powered && entity->control.powerType == 1){
      if(entity->control.powerTime <= facade->getTime(0)){
        entity->control.powered = false;
        entity->speed.powerMax = entity->speed.initialPower;
      }
    }

    if (entity->speed.Z != 0)
    {
      if (((entity->position.Z+entity->speed.Z/2 > 0.15 && entity->position.Z+entity->speed.Z/2 < 3.4 && entity->team.team == "Blue") ||
      (entity->position.Z+entity->speed.Z/2 > -3.4 & entity->position.Z+entity->speed.Z/2 < -0.15 && entity->team.team == "Red")))
      {
          check = true;
          entity->position.Z += entity->speed.Z/2;
      }
    }

    if (entity->speed.X != 0)
    {
      if (entity->position.X+entity->speed.X/2 > -2.6 && entity->position.X+entity->speed.X/2 < 2.6 && entity->speed.X!=0)
      {
          check = true;
          entity->position.X += entity->speed.X/2;
      }
    }
    if(check == true)
    {
        float rotX = entity->speed.X / entity->speed.power;
        float rotZ = 180;

        if (entity->speed.Z > 0)
        {
            rotX *= 90;
            rotZ += rotX;
        }
        else if (entity->speed.Z < 0)
        {
            rotX *= -90;
            rotZ = rotX;
        }
        else
        {
          rotZ = rotX * -90;
        }

        entity->node.myNode->setRotation(vec3 (0, rotZ, 0));

        entPos = vec3 (entity->position.X, entity->node.myNode->getPosition().y, entity->position.Z);
        if(entity->stamina.stamina < 0){
          entity->stamina.stamina = 0;
        }
        if(entity->dash.dodgeTime > facade->getTime(0)){
          facade->setAnim(entity, 4);
        }
        else{
            facade->setAnim(entity, 1);
        }
    }
    else{
      entity->speed.X = 0;
      entity->speed.Z = 0;
      facade->setAnim(entity, 0);
      if(entity->team.team == "Red"){
        entity->node.myNode->setRotation(vec3 (0, 180, 0));
      }
      else{
          entity->node.myNode->setRotation(vec3 (0, 0, 0));
      }
    }
}

// Reset the game to start playing the next round
void Mechanics::resetMatch(){
    // Delete all characters

    if(entity_manager->getScore()->match.ronda < 5)
        entity_manager->getScore()->match.ronda++;

    if(airTime >= facade->getTime(0)){
      airTime = facade->getTime(0);
    }

    facade->deleteAllCharacters();

    entity_manager->initializeEntities(facade->getMultiplayerID());
    facade->renderEntities();
}

//======================================================================================================
//                                    MULTIPLAYER METHODS                                              |
//======================================================================================================

//--------------------------------GETPLAYERNAMEANDPOSITION----------------------------------------
std::string Mechanics::getPlayerNameAndPosition()
{
  auto& entitiesMap = entity_manager->getMapEntities();

  for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
  {
    if(itr->second->control.esPlayer)
    {
      std::string playerPosition = "," + std::to_string(itr->second->position.X) + " " + std::to_string(itr->second->position.Z);

      return itr->first + playerPosition;
    }
  }
  return ",";
}

//--------------------------------PROCESSMULTIPLAYERINFO----------------------------------------
void Mechanics::processMultiplayerInfo(std::string answer)
{
  std::vector<std::string> datos = split(answer, ',');
  IEntity* player;

  if (datos.size() > 4 && datos[2] != "" && entity_manager->entityExistsMultiplayer(datos[2].c_str()))
  {
    player = entity_manager->getEntity(datos[2].c_str()).get();

    if (player->control.esMultiplayer == false)
    {
      //Si hay algun otro multiplayer lo pasamos a IA (ojo si hay mas de dos players)
      auto& entitiesMap = entity_manager->getMapEntities();

      for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
      {
        if(itr->second->control.esMultiplayer)
        {
          itr->second->control.esMultiplayer = false;

          if(!itr->second->control.esPlayer)
            itr->second->control.esJugador = true;
        }
      }

      player->control.esMultiplayer = true;
      player->control.esJugador = false;
    }

    if (datos[3] != "")
    {
      std::vector<std::string> position = split(datos[3], ' ');

      float playerX = std::stof(position[0]);
      float playerZ = std::stof(position[1]);

      if (playerX < player->position.X)
      {
        if(playerZ < player->position.Z)
        {
          player->node.myNode->setRotation(vec3 (0, 45, 0));
        }
        else if(playerZ > player->position.Z)
        {
          player->node.myNode->setRotation(vec3 (0, 135, 0));
        }
        else
        {
          player->node.myNode->setRotation(vec3 (0, 90, 0));
        }

        facade->setAnim(player, 1);
      }
      else if (playerX > player->position.X)
      {
        if(playerZ < player->position.Z)
        {
          player->node.myNode->setRotation(vec3 (0, -45, 0));
        }
        else if(playerZ > player->position.Z)
        {
          player->node.myNode->setRotation(vec3 (0, -135, 0));
        }
        else
        {
          player->node.myNode->setRotation(vec3 (0, -90, 0));
        }

        facade->setAnim(player, 1);
      }
      else if (playerZ < player->position.Z)
      {
        player->node.myNode->setRotation(vec3 (0, 0, 0));
        facade->setAnim(player, 1);
      }
      else if (playerZ > player->position.Z)
      {
        player->node.myNode->setRotation(vec3 (0, 180, 0));
        facade->setAnim(player, 1);
      }
      else
      {
        if (player->team.team == "Blue")
          player->node.myNode->setRotation(vec3 (0, 0, 0));
        else if (player->team.team == "Red")
          player->node.myNode->setRotation(vec3 (0, 180, 0));

        facade->setAnim(player, 0);
      }

      player->position.X = playerX;
      player->position.Z = playerZ;
    }

    if (datos.size() > 4 && datos[4] != "" && datos[4].size() < 5)
    {
      std::vector<std::string> actions = split(datos[4], ' ');

      if (datos.size() > 5 && datos[5] != "")
      {
        std::vector<std::string> direction = split(datos[5], ' ');

        multiplayerSpeedX = std::stof(direction[0]);
        multiplayerSpeedZ = std::stof(direction[1]);
      }

      for (int i = 0; i < actions.size(); ++i)
      {
        multiplayerAction(player, entity_manager->getEntity("n_ball_01").get(), actions[i].c_str());
      }
    }
  }
  if (datos.size() > 4 && datos[datos.size()-1] != "")
  {
    if (entity_manager->entityExistsMultiplayer(datos[datos.size()-1].c_str()))
    {
      auto* player = entity_manager->getEntity(datos[datos.size()-1].c_str()).get();

      checkDelete(player, datos[datos.size()-1].c_str(), playerIsDead, player2IsDead);
    }
    else if (datos[2] != "" && datos[datos.size()-1] == "catch")
    {
      multiplayerSpeedX = 100;
      catchBall(player, entity_manager->getEntity("n_ball_01").get());
      multiplayerSpeedX = 0;
    }
  }
}

//--------------------------------MULTIPLAYERACTION----------------------------------------
void Mechanics::multiplayerAction(IEntity* player, IEntity* ball, const char* action)
{
  if (facade->compareParent(ball, player))
  {
    if(!strcmp(action, "SP"))
    {
      throwBall(player, ball, player);
    }
    else if(!strcmp(action, "Y"))
    {
      superThrow(player, ball, player);
    }
    else if(!strcmp(action, "R") || !strcmp(action, "RAW") || !strcmp(action, "RAS") || !strcmp(action, "RA") ||
            !strcmp(action, "RDW") || !strcmp(action, "RDS") || !strcmp(action, "RD") || !strcmp(action, "RS"))
    {
      passBall(player, ball, player);
      facade->setAnim(player, 2);
    }
  }
}

//--------------------------------SPLIT----------------------------------------
std::vector<std::string> Mechanics::split(const std::string &s, char delim)
{
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;

  while (std::getline(ss, item, delim))
  {
    elems.push_back(item);
  }

  return elems;
}

/* UPDATE INDICATORS METHOD */
void Mechanics::updateIndicators()
{
    auto& entitiesMap = entity_manager->getMapEntities();

    for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
    {
        // Add circle to the bottom of the player
        if(itr->second->control.esPlayer)
        {
            // Get current player node and direction arrow entity
            auto playerNode = itr->second->node.myNode;
            auto& flecha = entity_manager->getEntity("flecha_direccion_player");

            if(!flecha || !playerNode) { return; }

            // Get node of the direction arrow
            auto flechaNode = flecha->node.myNode;

            if(!flechaNode) { return; }

            // Update rotation of the direction arrow
            float angle;
            if(facade->checkMando())
                angle = getAngle(facade->getXAxisRJ(), -facade->getYAxisRJ());
            else
                angle = getAngle(facade->getCursorX(), facade->getCursorY());
            facade->setRotation(flecha.get(), -playerNode->getRotation() + vec3(0, angle + 180.0f, 0));

            if(playerNode != flechaNode->getParent()) // Player character has recently changed
            {
                // Remove direction arrow from last parent child list
                if(flechaNode->getParent())
                  flechaNode->getParent()->removeChild(flechaNode);

                // Set new scale of the direction arrow
                flechaNode->setScale(vec3(0.08f));
                auto newScale = (vec3(1) / playerNode->getScale()) * flechaNode->getScale();
                flechaNode->setScale(newScale);

                // Set new position of the direction arrow (new player's feet)
                flecha->position.Y = -6 * 0.05 / playerNode->getScale().y;

                // Set new parent of the direction arrow
                playerNode->addChild(flechaNode);
            }
        }

        // Add ball icon to the top of the character that has the ball
        if(itr->second->control.esPelota)
        {
            // Get ball icon entity
            auto& ballIcon = entity_manager->getEntity("ball_icon");

            if(!ballIcon) { return; }

            // Get ball icon node and current ball parent node
            auto ballIconNode = ballIcon->node.myNode;
            auto ballParentNode = itr->second->node.myNode->getParent();

            if(!ballIconNode) { return; }

            if(!ballParentNode || !pick) // If ball is not picked or has no parent
            {
                // Set ball icon scale to 0
                ballIconNode->setScale(vec3(0));
            }
            else // Otherwise
            {
                // Remove ball icon from last parent child list
                if(ballIconNode->getParent())
                  ballIconNode->getParent()->removeChild(ballIconNode);

                // Set ball icon new scale
                ballIconNode->setScale(vec3(0.6f));
                auto newScale = (vec3(1) / ballParentNode->getScale() * ballIconNode->getScale());
                ballIconNode->setScale(newScale);

                // Set ball icon new position
                ballIcon->position.Y = 7.5 * 0.05 / ballParentNode->getScale().y;

                // Set ball icon new parent
                ballParentNode->addChild(ballIconNode);
            }
        }
    }
}

float Mechanics::getAngle(float xPos, float yPos)
{
    float angle = atan2(yPos, xPos) * 180.0f / M_PI;

    if(angle > 360.0f)
      angle = mod(angle, 360.0f);

    return angle;
}

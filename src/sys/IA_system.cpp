#include <IA_system.h>

void IA_system::initializeIA(Entity_Manager* entity_manager, Facade2* facade, Mechanics* mechanics) {
    createFuzzyLanzar();
    createFuzzyPasar();
    createFuzzyEsquivar();
    createFuzzyAgarrar();

    // Establishing references
    if(!this->entity_manager)
      this->entity_manager = entity_manager;
    if(!this->facade)
      this->facade = facade;
    if(!this->mechanics)
      this->mechanics = mechanics;
}

void IA_system::updateIA()
{
    IEntity* ball = nullptr;
    IEntity* player = nullptr;
    IEntity* player2 = nullptr;
    IEntity* multiplayer = nullptr;
    IEntity* powerUp = nullptr;
    IEntity* cercaDelPowerUp = nullptr;
    bool playerNumber = false;
    bool multiplayerMode = false;
    uint16_t multiplayers = 0;
    float powerDist = 9999;

    bool checkCerca = false;

    auto& entitiesMap = entity_manager->getMapEntities();

    for(auto itrA = entitiesMap.begin(); itrA != entitiesMap.end(); itrA++)
    {
        auto& entityB = itrA->second;
        if (entityB->control.esPlayer)
        {
            if (!playerNumber)
            {
                player = entityB.get();
                playerNumber = true;
            }
            else
            {
                player2 = entityB.get();
            }

            entityB->status.est = 'N';
        }
        else if (entityB->control.esPowerUp)
            powerUp = entityB.get();
        else if (entityB->control.esPelota && ball == nullptr)
            ball = entityB.get();

        if(entityB->status.est == 'D'){
          float range = 0;

            range = 1.8;

          int dir = 0;
          for(auto itrC = entitiesMap.begin(); itrC != entitiesMap.end(); itrC++){
            auto& entityR = itrC->second;
            if(entityR->id != entityB->id && entityR->team.team == entityB->team.team && entityR.get() != player){
              if((entityB->position.Z - range < entityR->position.Z && entityB->position.Z >= entityR->position.Z && entityB->position.X + range > entityR->position.X && entityB->position.X <= entityR->position.X) ||
              (entityB->position.Z + range > entityR->position.Z && entityB->position.Z <= entityR->position.Z && entityB->position.X - range < entityR->position.X && entityB->position.X >= entityR->position.X) ||
            (entityB->position.Z - range < entityR->position.Z && entityB->position.Z >= entityR->position.Z && entityB->position.X - range < entityR->position.X && entityB->position.X >= entityR->position.X) ||
          (entityB->position.Z + range > entityR->position.Z && entityB->position.Z <= entityR->position.Z && entityB->position.X + range > entityR->position.X && entityB->position.X <= entityR->position.X)){
                if(entityB->position.X < entityR->position.X){
                  if(dir!= -1){
                    dir --;
                  }
                }
                else{
                  if(dir!= 1){
                /* message */    dir ++;
                  }
                }
              }
            }
          }
          switch (dir) {
            case -1:
              if(entityB->position.X > -2){
                entityB->position.X -= entityB->speed.power;
              }
              else{
                dir = 0;
              }
            break;
            case 1:
            if(entityB->position.X < 2){
              entityB->position.X += entityB->speed.power;
            }
            else{
              dir = 0;
            }
            break;
          }
        }
        if(facade->getMultiplayerID() == 0 && entityB->team.team == "Blue" && !entityB->control.esPelota){
          if(player != nullptr && player != entityB.get() && player->position.X < entityB->target->position.X + 0.3 &&
          player->position.X > entityB->target->position.X - 0.3){
            auto iCercana = entityB->target;
            entityB->target = player->target;
            player->target = iCercana;
          }
        }
    }

    for(auto itrA = entitiesMap.begin(); itrA != entitiesMap.end(); itrA++)
    {
        auto& entityB = itrA->second;
        if (player != nullptr && entityB->control.esMultiplayer && entityB->team.team == player->team.team)
        {
            ++multiplayers;
            multiplayer = entityB.get();
            multiplayer->control.esJugador = false;
        }
        if (entityB->control.esMultiplayer)
        {
            multiplayerMode = true;
        }
    }

    float pBallZ = ball->position.Z;
    float pBallX = ball->position.X;

    bool stop = false;

    if (multiplayerMode || player != nullptr)
    {
      for(auto itrA = entitiesMap.begin(); itrA != entitiesMap.end() && !stop; itrA++)
      {
          auto& entity = itrA->second;

          if (multiplayerMode)
        {
            entity->stamina.stamina = entity->stamina.staminaMax;
        }

          if((entity->control.esJugador && !entity->control.esMultiplayer) || entity->control.esPlayer)
          {

              float pEntZ = entity->position.Z;
              float pEntX = entity->position.X;
              float factor = entity->c.grabRange - (entity->stamina.staminaMax-entity->stamina.stamina)*0.0004;

              //-------------------------------------UPDATESTATUS----------------------------------------

                if (entity->dash.dodgeTime <= facade->getTime(0) && entity->status.est != 'P')
              {
                if(!entity->dash.count){
                  entity->dash.count = true;
                  entity->dash.direction = '-';
                  entity->dash.posFin = 0;
                }
                  //Gestiona el cambio de jugador y de estado al coger la pelota la IA
                  if (facade->compareParent(ball, entity.get()))
                  {
                      entity->status.est = 'L';
                      if (player != nullptr && entity->team.team == player->team.team && entity->control.esMultiplayer == false)
                      {
                        //Si no hay multijugadores en tu equipo
                        if (multiplayers == 0)
                        {
                          player->control.esPlayer = false;
                          player->control.esJugador = true;

                          player = entity.get();
                          entity->control.esJugador = false;
                          entity->control.esPlayer = true;
                        }

                        //Si hay 1 multijugador en tu equipo
                        else if (multiplayers == 1)
                        {
                          float distPlayer = (player->position.Z - entity->position.Z) * (player->position.Z - entity->position.Z) +
                                              (player->position.Z - entity->position.X) * (player->position.Z - entity->position.X);
                          distPlayer = sqrtl(distPlayer);

                          float distMultiplayer = (multiplayer->position.Z - entity->position.Z) * (multiplayer->position.Z - entity->position.Z) +
                                                  (multiplayer->position.Z - entity->position.X) * (multiplayer->position.Z - entity->position.X);
                          distMultiplayer = sqrtl(distMultiplayer);

                          if(distPlayer < distMultiplayer)
                          {
                              player->control.esPlayer = false;
                              player->control.esJugador = true;

                              player = entity.get();
                              entity->control.esJugador = false;
                              entity->control.esPlayer = true;
                          }
                          else
                          {
                              multiplayer->control.esMultiplayer = false;
                              multiplayer->control.esJugador = true;

                              multiplayer = entity.get();
                              entity->control.esJugador = false;
                              entity->control.esMultiplayer = true;

                          }
                        }
                      }
                      else if (player2 != nullptr && entity->team.team == player2->team.team && entity->control.esMultiplayer == false)
                      {
                          if (multiplayers < 2)
                          {
                            player2->control.esPlayer = false;
                            player2->control.esJugador = true;

                            player2 = entity.get();
                            entity->control.esJugador = false;
                            entity->control.esPlayer = true;
                          }
                          else
                          {
                            entity->control.esJugador = false;
                            entity->control.esMultiplayer = true;
                            entity->status.est = 'N';
                          }
                      }
                    }
                    else if (ball->team.team == entity->team.team && !mechanics->getInAir())
                        entity->status.est = 'O';
                    else if (((pBallZ - factor < pEntZ && pBallZ >= pEntZ && pBallX + factor > pEntX && pBallX <= pEntX) ||
                    (pBallZ + factor> pEntZ && pBallZ <= pEntZ && pBallX - factor < pEntX && pBallX >= pEntX) ||
                (pBallZ - factor < pEntZ && pBallZ >= pEntZ && pBallX - factor < pEntX && pBallX >= pEntX) ||
                (pBallZ + factor > pEntZ && pBallZ <= pEntZ && pBallX + factor > pEntX && pBallX <= pEntX))
                            && ball->team.team != entity->team.team && ball->team.team!="NoTeam" && mechanics->getInAir())
                        entity->status.est = 'A';
                    else if (ball->team.team != entity->team.team && ball->team.team!="NoTeam")
                        entity->status.est = 'D';
                    else if (checkCerca == false && ball->team.team=="NoTeam" && !mechanics->getInAir() &&
                    ((ball->position.Z <= 0 && entity->team.team == "Red")
                    || (ball->position.Z > 0 && entity->team.team == "Blue"))){
                                std::map < const char*, unique_ptr<IEntity> >::iterator itrB;
                                float distMin = 99999;
                                IEntity* iCercana;
                                for(itrB = entitiesMap.begin(); itrB != entitiesMap.end(); itrB++)
                                {
                                    auto& entityCerca = itrB->second;
                                    if(entityCerca->team.team == entity->team.team &&
                                      entityCerca->dash.dodgeTime <= facade->getTime(0))
                                    {
                                        float dist = (pBallZ - entityCerca->position.Z) * (pBallZ - entityCerca->position.Z) +
                                        (pBallX - entityCerca->position.X) * (pBallX - entityCerca->position.X);
                                        dist = sqrtl(dist);

                                        if(dist < distMin)
                                        {
                                            distMin = dist;
                                            iCercana = entityCerca.get();
                                        }
                                    }
                                }
                                iCercana->status.est = 'G';
                                checkCerca = true;
                            }
                    else
                    {
                        if(entity->status.est != 'G')
                        {
                            entity->status.est = 'N';
                            entity->speed.Z = 0;
                            entity->speed.X = 0;
                        }
                    }
                  }

              //-------------------------------------IA SYSTEM----------------------------------------
              if (entity->control.esJugador)
              {
                  //Si la IA esta en ESTADO LANZADOR
                  if(entity->status.est == 'L')
                  {
                      entity->speed.X = 0;
                      if (entity->team.team=="Blue")
                          entity->speed.Z = -entity->speed.power;
                      else if (entity->team.team=="Red")
                          entity->speed.Z = entity->speed.power;

                      //Se calculan las deseabilidades
                      double lanzar = updateFuzzyLanzar(entitiesMap, entity.get());

                      std::map < const char*, unique_ptr<IEntity> >::iterator itrB;
                      double lanzaComp = 0;
                      double mejorLanzar = 0;
                      IEntity* compa;
                      for(itrB = entitiesMap.begin(); itrB != entitiesMap.end(); itrB++){
                          auto& entityComp = itrB->second;
                          if(entityComp->team.team == entity->team.team && !entityComp->control.esPelota)
                          {
                              lanzaComp = updateFuzzyLanzar(entitiesMap, entityComp.get());
                              if(mejorLanzar < lanzaComp)
                              {
                                  mejorLanzar = lanzaComp;
                                  compa = entityComp.get();
                              }
                          }
                      }

                      entity->target = mechanics->closerTarget(entity.get(), 'E');
                        //Se comprueba si la entidad poseedora del balon prefiere lanzar o pasar
                        if (compa == entity.get() || (lanzar > mejorLanzar && lanzar > 5) || entity->t.superShot > 2){
                          if(entity->position.X < entity->target->position.X+0.5 && entity->position.X > entity->target->position.X-0.5){
                            if (entity->t.superShot > 2)
                            {
                                mechanics->superThrow(entity.get(), ball, entity->target);
                            }
                            else
                            {
                                mechanics->throwBall(entity.get(), ball, entity->target);
                            }
                          }
                          else
                          {
                              if (entity->target->position.X > entity->position.X)
                                  entity->speed.X = entity->speed.power;
                              else if (entity->target->position.X < entity->position.X)
                                  entity->speed.X = -(entity->speed.power);
                          }
                        }
                        else if (mejorLanzar > 6){
                          mechanics->passBall(entity.get(), ball, compa);
                        }
                        else if (powerUp != nullptr)
                        {
                            float distMin = 9999;
                            for(auto itrB = entitiesMap.begin(); itrB != entitiesMap.end(); itrB++)
                            {
                                auto& entityB = itrB->second;
                                if (entityB->team.team == entity->team.team && entityB.get() != ball)
                                {
                                    float dist = (powerUp->position.Z - entityB->position.Z) * (powerUp->position.Z - entityB->position.Z) +
                                                (powerUp->position.X - entityB->position.X) * (powerUp->position.X - entityB->position.X);
                                    dist = sqrtl(dist);
                                    if (dist < distMin)
                                    {
                                        distMin = dist;
                                        entity->target = entityB.get();
                                    }
                                }
                            }
                            if (entity.get() == entity->target)
                            {
                                mechanics->throwBall(entity.get(), ball, powerUp);
                            }
                            else
                            {
                                mechanics->passBall(entity.get(), ball, entity->target);
                                entity->target->status.est = 'P';
                            }
                        }
                        else{
                          if (compa == entity.get() || (lanzar > mejorLanzar && lanzar > 5) || entity->t.superShot > 2){
                            if (entity->t.superShot > 2)
                            {
                                mechanics->superThrow(entity.get(), ball, entity->target);
                            }
                            else
                            {
                                mechanics->throwBall(entity.get(), ball, entity->target);
                            }
                          }
                          else{
                            mechanics->passBall(entity.get(), ball, compa);
                          }
                        }
                  }

                  //Si la IA debe lanzar al power up
                  else if (entity->status.est == 'P')
                  {
                      if (powerUp != nullptr)
                      {
                          if (facade->compareParent(ball, entity.get()))
                          {
                              if (powerUp->position.Z - 0.6 < entity->position.Z)
                              {
                                  entity->speed.Z = -entity->speed.power;
                              }
                              else
                              {
                                  entity->speed.Z = 0;
                                  mechanics->throwBall(entity.get(), ball, powerUp);
                              }
                          }
                          if(ball->team.team == "Blue"){
                            entity->status.est = 'D';
                          }
                      }
                      else
                      {
                        entity->status.est = 'O';
                      }
                  }

                  //Si la IA esta en ESTADO OFENSIVO
                  else if (entity->status.est == 'O')
                  {
                      entity->speed.X = 0;
                      if(entity->team.team=="Blue" && entity->position.Z <= 0.15)
                          entity->speed.Z = 0;
                      else if (entity->team.team=="Blue" && entity->position.Z > 0.15)
                          entity->speed.Z = -entity->speed.power;
                      else if(entity->team.team == "Red" && entity->position.Z >= -0.15)
                          entity->speed.Z = 0;
                      else if (entity->team.team=="Red" && entity->position.Z < -0.15)
                          entity->speed.Z = entity->speed.power;
                      if(mechanics->getInAir()){
                        entity->speed.X = 0;
                        entity->speed.Z = 0;
                      }
                  }
                  else if(entity->status.est == 'L' && mechanics->getInAir()){
                    entity->speed.X = 0;
                    entity->speed.Z = 0;
                  }

                  //Si la IA esta en ESTADO ALERTA
                  else if(entity->status.est == 'A')
                  {
                          double esquivar = updateFuzzyEsquivar(entitiesMap, entity.get(), ball);
                          double agarrar = updateFuzzyAgarrar(entitiesMap, entity.get(), ball);

                          if(entity->dash.dodgeTime <= facade->getTime(0) && entity->difficulty.active){

                            if (agarrar>8.5 || agarrar>esquivar || (entity->dash.dodgeCoolDown > facade->getTime(0) &&
                            entity->dash.dodgeTime <= facade->getTime(0)) || (entity->dash.dodgeCoolDown > facade->getTime(0) &&
                            entity->dash.dodgeTime <= facade->getTime(0))){
                                mechanics->catchBall(entity.get(), ball);
                            }

                            else
                            {
                              if(entity->dash.dodgeCoolDown <= facade->getTime(0))
                              {
                                  entity->dash.count = false;
                                  entity->dash.dodgeCoolDown = facade->getTime(5);
                                  entity->dash.dodgeTime = facade->getTime(0.4);
                                  entity->audio.suena = true;
                                  entity->audio.paramValue = 2;
                                  facade->setAnim(entity.get(), 4);
                                  mechanics->dodgeBall(entity.get(), ball, player);
                                  if(entity->difficulty.difficulty == "Easy"){
                                    entity->difficulty.actionTime = facade->getTime(2.5);
                                  }
                                  else if(entity->difficulty.difficulty == "Medium"){
                                    entity->difficulty.actionTime = facade->getTime(1.5);
                                  }
                                  else{
                                    entity->difficulty.actionTime = facade->getTime(1);
                                  }
                              }
                            }
                          }
                          if(entity->dash.dodgeTime > facade->getTime(0) && entity->dash.direction != '-'){
                            mechanics->dodgeBall(entity.get(), ball, player);
                          }
                  }

                  //Si la IA esta en ESTADO DEFENSIVO
                  else if(entity->status.est == 'D')
                  {
                      entity->speed.X = 0;
                      if (entity->team.team=="Blue")
                          entity->speed.Z = entity->speed.power;
                      else if (entity->team.team=="Red")
                          entity->speed.Z = -(entity->speed.power);
                  }

                      //Si la IA esta en ESTADO SIN PELOTA
                  else if (entity->status.est == 'G')
                  {
                      entity->speed.X = 0;
                      entity->speed.Z = 0;
                      mechanics->goToBall(entity.get(), ball);
                      mechanics->catchBall(entity.get(), ball);
                  }

                  else if(entity->status.est == 'N' && entity->t.checker > 0){
                    if(mechanics->getInAir()){
                        const char* del = facade->elimination();
                        mechanics->checkSuperThrow(ball, entity.get());
                        mechanics->checkDelete(player, del, mechanics->getPlayerIsDead(), mechanics->getPlayer2IsDead());
                        int redMembers=0;
                        int blueMembers=0;
                        auto& score = entity_manager->getScore();
                        std::map < const char*, unique_ptr<IEntity> >::iterator itrC;
                        //Se recorre el bucle para contar las entidades de cada equipo
                        for(itrC = entitiesMap.begin(); itrC != entitiesMap.end(); itrC++){
                            auto& entity = itrC->second;
                            if(entity->team.team=="Red")
                                redMembers++;
                            if(entity->team.team=="Blue")
                                blueMembers++;
                        }

                        double spawnTime = mechanics->getSpawnTime();
                        //Se comprueba que equipo no tiene jugadores para establecer la vitoria/derrota y resetear el partido
                        if(redMembers==0){
                            score->match.bluePoints++;
                            score->match.ronda++;
                            score->match.finish=true;
                            spawnTime = 0;
                        }else if(blueMembers==0){
                            score->match.redPoints++;
                            score->match.ronda++;
                            score->match.finish=true;
                            spawnTime = 0;
                        }
                    }
                  }

                  targets.clear();
                  if(entity->status.est == 'O' || entity->status.est == 'L'){
                    if(facade->getMultiplayerID() == 0 && entity->target != nullptr){
                      if(entity->position.X < entity->target->position.X - 0.1){
                          entity->speed.X += entity->speed.power;
                      }
                      else if(entity->position.X > entity->target->position.X + 0.1){
                        entity->speed.X -= entity->speed.power;
                      }
                      else{
                        facade->setRotation(entity.get(), vec3(0, 0, 0));
                      }
                    }
                  }
                  if(mechanics->getInAir() && entity->team.team == ball->team.team){
                    entity->speed.X = 0;
                    entity->speed.Z = 0;
                  }
                  entity->target = nullptr;
                  player->target = nullptr;

                  if(entity->difficulty.active && entity->difficulty.perception <= facade->getTime(0)){
                    entity->difficulty.active = false;
                    if(entity->difficulty.difficulty == "Easy"){
                      entity->difficulty.perception = facade->getTime(0.6);
                    }
                    else if(entity->difficulty.difficulty == "Medium"){
                      entity->difficulty.perception = facade->getTime(0.35);
                    }
                    else{
                      entity->difficulty.perception = facade->getTime(0.2);
                    }
                  }
                  else if(!entity->difficulty.active && entity->difficulty.perception <= facade->getTime(0)){
                    entity->difficulty.active = true;
                    if(entity->difficulty.difficulty == "Easy"){
                      entity->difficulty.perception = facade->getTime(0.4);
                    }
                    else if(entity->difficulty.difficulty == "Medium"){
                      entity->difficulty.perception = facade->getTime(0.65);
                    }
                    else{
                      entity->difficulty.perception = facade->getTime(0.8);
                    }
                  }
              }
          }
      }
    }
}

//-------------------------------------fUZZYLANZAR-----------------------------------------------
void IA_system::createFuzzyLanzar(){
    FuzzyVariable& Distancia = fmLanzar.CreateFLV("Distancia");
    FzSet DistanciaCerca = Distancia.AddLeftShoulderSet("Cerca", 0, 1, 3);
    FzSet DistanciaMedia = Distancia.AddTriangularSet("Media", 1, 3, 5);
    FzSet DistanciaLejos = Distancia.AddRightShoulderSet("Lejos", 3, 5, 400);

    FuzzyVariable& Potencia = fmLanzar.CreateFLV("Potencia");
    FzSet PotenciaFlojo = Potencia.AddLeftShoulderSet("Flojo", 0, 6, 10);
    FzSet PotenciaMedio = Potencia.AddTriangularSet("Medio", 6, 10, 16);
    FzSet PotenciaFuerte = Potencia.AddRightShoulderSet("Fuerte", 10, 16, 100);

    FuzzyVariable& Deseabilidad = fmLanzar.CreateFLV("Deseabilidad");
    FzSet Indeseable = Deseabilidad.AddLeftShoulderSet("Indeseable", 0, 2.5, 5);
    FzSet Deseable = Deseabilidad.AddTriangularSet("Deseable", 2.5, 5, 7.5);
    FzSet MuyDeseable = Deseabilidad.AddRightShoulderSet("MuyDeseable", 5, 7.5, 10);

    FzAND f11 = FzAND(DistanciaCerca, PotenciaFlojo);
    fmLanzar.AddRule(f11, Deseable);
    FzAND f12 = FzAND(DistanciaCerca, PotenciaMedio);
    fmLanzar.AddRule(f12, MuyDeseable);
    FzAND f13 = FzAND(DistanciaCerca, PotenciaFuerte);
    fmLanzar.AddRule(f13, MuyDeseable);
    FzAND f21 = FzAND(DistanciaMedia, PotenciaFlojo);
    fmLanzar.AddRule(f21, Indeseable);
    FzAND f22 = FzAND(DistanciaMedia, PotenciaMedio);
    fmLanzar.AddRule(f22, Deseable);
    FzAND f23 = FzAND(DistanciaMedia, PotenciaFuerte);
    fmLanzar.AddRule(f23, MuyDeseable);
    FzAND f31 = FzAND(DistanciaLejos, PotenciaFlojo);
    fmLanzar.AddRule(f31, Indeseable);
    FzAND f32 = FzAND(DistanciaLejos, PotenciaMedio);
    fmLanzar.AddRule(f32, Indeseable);
    FzAND f33 = FzAND(DistanciaLejos, PotenciaFuerte);
    fmLanzar.AddRule(f33, Deseable);
}

double IA_system::updateFuzzyLanzar(std::map< const char*, unique_ptr<IEntity>, cmp_str >& entities, IEntity* ent)
{
  //Se crean una variable de distancia auxiliar y otra de distancia mas cercana al jugador o a un npc
  double dist = 0;
  double distMin = 9999;

  std::map < const char*, unique_ptr<IEntity> >::iterator itr;
  for(itr = entities.begin(); itr != entities.end(); itr++)
  {
    auto& entity = itr->second;
    //Se comprueba si la entidad es el jugador o si es un npc
    if(ent->team.team != entity->team.team && ent->team.team != "NoTeam")
    {
      //La distancia se calcula con la suma de la distancia en X y en Z
      dist = (ent->position.Z - entity->position.Z) * (ent->position.Z - entity->position.Z) +
      (ent->position.X - entity->position.X) * (ent->position.X - entity->position.X);
      dist = sqrtl(dist);

      //Si la distancia es menor que la distancia mini anterior, se sustituye el valor
      if(dist < distMin)
        distMin = dist;
    }
  }
  fmLanzar.Fuzzify("Distancia", distMin);
  fmLanzar.Fuzzify("Potencia", ent->t.strength * 100);

  return fmLanzar.DeFuzzify("Deseabilidad");
}

//-------------------------------------fUZZYPASAR-----------------------------------------------
void IA_system::createFuzzyPasar(){
    FuzzyVariable& Potencia = fmPasar.CreateFLV("Potencia");
    FzSet PotenciaFlojo = Potencia.AddLeftShoulderSet("Flojo", 0, 6, 10);
    FzSet PotenciaMedio = Potencia.AddTriangularSet("Medio", 6, 10, 16);
    FzSet PotenciaFuerte = Potencia.AddRightShoulderSet("Fuerte", 10, 16, 100);

    FuzzyVariable& PotenciaCompa = fmPasar.CreateFLV("PotenciaCompa");
    FzSet PotenciaCompaFlojo = PotenciaCompa.AddLeftShoulderSet("Flojo", 0, 6, 10);
    FzSet PotenciaCompaMedio = PotenciaCompa.AddTriangularSet("Medio", 6, 10, 16);
    FzSet PotenciaCompaFuerte = PotenciaCompa.AddRightShoulderSet("Fuerte", 10, 16, 100);

    FuzzyVariable& Deseabilidad = fmPasar.CreateFLV("Deseabilidad");
    FzSet Indeseable = Deseabilidad.AddLeftShoulderSet("Indeseable", 0, 2.5, 5);
    FzSet Deseable = Deseabilidad.AddTriangularSet("Deseable", 2.5, 5, 7.5);
    FzSet MuyDeseable = Deseabilidad.AddRightShoulderSet("MuyDeseable", 5, 7.5, 10);

    FzAND fp11 = FzAND(PotenciaFlojo, PotenciaCompaFlojo);
    fmPasar.AddRule(fp11, Indeseable);
    FzAND fp12 = FzAND(PotenciaFlojo, PotenciaCompaMedio);
    fmPasar.AddRule(fp12, Deseable);
    FzAND fp13 = FzAND(PotenciaFlojo, PotenciaCompaFuerte);
    fmPasar.AddRule(fp13, MuyDeseable);
    FzAND fp21 = FzAND(PotenciaMedio, PotenciaCompaFlojo);
    fmPasar.AddRule(fp21, Indeseable);
    FzAND fp22 = FzAND(PotenciaMedio, PotenciaCompaMedio);
    fmPasar.AddRule(fp22, Indeseable);
    FzAND fp23 = FzAND(PotenciaMedio, PotenciaCompaFuerte);
    fmPasar.AddRule(fp23, Deseable);
    FzAND fp31 = FzAND(PotenciaFuerte, PotenciaCompaFlojo);
    fmPasar.AddRule(fp31, Indeseable);
    FzAND fp32 = FzAND(PotenciaFuerte, PotenciaCompaMedio);
    fmPasar.AddRule(fp32, Indeseable);
    FzAND fp33 = FzAND(PotenciaFuerte, PotenciaCompaFuerte);
    fmPasar.AddRule(fp33, Indeseable);
}

double IA_system::updateFuzzyPasar(std::map< const char*, unique_ptr<IEntity>, cmp_str >& entities, IEntity* ent)
{
    //double dist = 0;
    //double distMin = 9999;
    //IEntity* compa;

    //std::map < const char*, unique_ptr<IEntity> >::iterator itr;
    //double lanzar = updateFuzzyLanzar(entities, ent);
    //double lanzaComp = 0;
    //double mejorLanzar = 0;

    //for(itr = entities.begin(); itr != entities.end(); itr++)
    //{
    //    auto& entityComp = itr->second;
    //    if(entityComp->team.team == ent->team.team && ent != entityComp.get() && !entityComp->control.esPelota)
    //    {
    //      lanzaComp = updateFuzzyLanzar(entities, entityComp.get());
    //      if(mejorLanzar < lanzaComp){
    //        mejorLanzar = lanzaComp;
    //      }
    //    }
    //}
    //fmPasar.Fuzzify("Potencia", lanzar);
    //fmPasar.Fuzzify("PotenciaCompa", mejorLanzar);

    //return fmPasar.DeFuzzify("Deseabilidad");
    return 0;
}

//-------------------------------------fUZZYESQUIVAR-----------------------------------------------
void IA_system::createFuzzyEsquivar(){
    FuzzyVariable& VelocidadBall = fmEsquivar.CreateFLV("VelocidadBall");
    FzSet VelocidadBallLento = VelocidadBall.AddLeftShoulderSet("Lento", 0, 0.06, 0.1);
    FzSet VelocidadBallMedio = VelocidadBall.AddTriangularSet("Medio", 0.06, 0.1, 0.16);
    FzSet VelocidadBallRapido = VelocidadBall.AddRightShoulderSet("Rapido", 0.1, 0.16, 6);

    FuzzyVariable& Esquive = fmEsquivar.CreateFLV("Esquive");
    FzSet EsquiveLento = Esquive.AddLeftShoulderSet("Lento", 0, 0.04, 0.1);
    FzSet EsquiveMedio = Esquive.AddTriangularSet("Medio", 0.04, 0.1, 0.16);
    FzSet EsquiveRapido = Esquive.AddRightShoulderSet("Rapido", 0.1, 0.16, 6);

    FuzzyVariable& Deseabilidad = fmEsquivar.CreateFLV("Deseabilidad");
    FzSet Indeseable = Deseabilidad.AddLeftShoulderSet("Indeseable", 0, 2.5, 5);
    FzSet Deseable = Deseabilidad.AddTriangularSet("Deseable", 2.5, 5, 7.5);
    FzSet MuyDeseable = Deseabilidad.AddRightShoulderSet("MuyDeseable", 5, 7.5, 10);

    FzAND fe11 = FzAND(VelocidadBallLento, EsquiveLento);
    fmEsquivar.AddRule(fe11, Deseable);
    FzAND fe12 = FzAND(VelocidadBallLento, EsquiveMedio);
    fmEsquivar.AddRule(fe12, MuyDeseable);
    FzAND fe13 = FzAND(VelocidadBallLento, EsquiveRapido);
    fmEsquivar.AddRule(fe13, MuyDeseable);
    FzAND fe21 = FzAND(VelocidadBallMedio, EsquiveLento);
    fmEsquivar.AddRule(fe21, Indeseable);
    FzAND fe22 = FzAND(VelocidadBallMedio, EsquiveMedio);
    fmEsquivar.AddRule(fe22, Deseable);
    FzAND fe23 = FzAND(VelocidadBallMedio, EsquiveRapido);
    fmEsquivar.AddRule(fe23, MuyDeseable);
    FzAND fe31 = FzAND(VelocidadBallRapido, EsquiveLento);
    fmEsquivar.AddRule(fe31, Indeseable);
    FzAND fe32 = FzAND(VelocidadBallRapido, EsquiveMedio);
    fmEsquivar.AddRule(fe32, Indeseable);
    FzAND fe33 = FzAND(VelocidadBallRapido, EsquiveRapido);
    fmEsquivar.AddRule(fe33, Deseable);
}

double IA_system::updateFuzzyEsquivar(std::map< const char*, unique_ptr<IEntity>, cmp_str >& entities, IEntity* ent, IEntity* ball){
    fmEsquivar.Fuzzify("VelocidadBall", ball->t.strength);
    fmEsquivar.Fuzzify("Esquive", ent->dash.value);

    return fmEsquivar.DeFuzzify("Deseabilidad");
}

//-------------------------------------fUZZYAGARRAR-----------------------------------------------
void IA_system::createFuzzyAgarrar(){
    FuzzyVariable& VelocidadBall = fmAgarrar.CreateFLV("VelocidadBall");
    FzSet VelocidadBallLento = VelocidadBall.AddLeftShoulderSet("Lento", 0, 0.06, 0.1);
    FzSet VelocidadBallMedio = VelocidadBall.AddTriangularSet("Medio", 0.06, 0.1, 0.16);
    FzSet VelocidadBallRapido = VelocidadBall.AddRightShoulderSet("Rapido", 0.1, 0.16, 6);

    FuzzyVariable& Agarre = fmAgarrar.CreateFLV("Agarre");
    FzSet AgarreLento = Agarre.AddLeftShoulderSet("Lento", 0, 1, 5);
    FzSet AgarreMedio = Agarre.AddTriangularSet("Medio", 1, 5, 9);
    FzSet AgarreRapido = Agarre.AddRightShoulderSet("Rapido", 5, 9, 10);

    FuzzyVariable& Deseabilidad = fmAgarrar.CreateFLV("Deseabilidad");
    FzSet Indeseable = Deseabilidad.AddLeftShoulderSet("Indeseable", 0, 2.5, 5);
    FzSet Deseable = Deseabilidad.AddTriangularSet("Deseable", 2.5, 5, 7.5);
    FzSet MuyDeseable = Deseabilidad.AddRightShoulderSet("MuyDeseable", 5, 7.5, 10);

    FzAND fa11 = FzAND(VelocidadBallLento, AgarreLento);
    fmAgarrar.AddRule(fa11, Deseable);
    FzAND fa12 = FzAND(VelocidadBallLento, AgarreMedio);
    fmAgarrar.AddRule(fa12, MuyDeseable);
    FzAND fa13 = FzAND(VelocidadBallLento, AgarreRapido);
    fmAgarrar.AddRule(fa13, MuyDeseable);
    FzAND fa21 = FzAND(VelocidadBallMedio, AgarreLento);
    fmAgarrar.AddRule(fa21, Indeseable);
    FzAND fa22 = FzAND(VelocidadBallMedio, AgarreMedio);
    fmAgarrar.AddRule(fa22, Deseable);
    FzAND fa23 = FzAND(VelocidadBallMedio, AgarreRapido);
    fmAgarrar.AddRule(fa23, MuyDeseable);
    FzAND fa31 = FzAND(VelocidadBallRapido, AgarreLento);
    fmAgarrar.AddRule(fa31, Indeseable);
    FzAND fa32 = FzAND(VelocidadBallRapido, AgarreMedio);
    fmAgarrar.AddRule(fa32, Indeseable);
    FzAND fa33 = FzAND(VelocidadBallRapido, AgarreRapido);
    fmAgarrar.AddRule(fa33, Deseable);
}

double IA_system::updateFuzzyAgarrar(std::map< const char*, unique_ptr<IEntity>, cmp_str >& entities, IEntity* ent, IEntity* ball){
    fmAgarrar.Fuzzify("VelocidadBall", ball->t.strength);
    fmAgarrar.Fuzzify("Agarre", ent->c.hability);

    return fmAgarrar.DeFuzzify("Deseabilidad");
}

//-----------------------------------DECISION TREES------------------------------------------

//DECISION TREE

    //std::map< const char*, unique_ptr<IEntity> >::iterator itr;
    //for(itr = entities.begin(); itr != entities.end(); itr++)
    //{
    //    DecisionTreeNode* nodo = rootDefensivo->makeDecision(itr->second.get());
    //
    //    if (Action* a = dynamic_cast<Action*>(nodo)){
    //        uint32_t numAction = a->getAccion();
    //        //if (numAction==2){
    //        //    itr->second.get()->position.X = 0;
    //        //}
    //    }
    //    else { cout << "No hay accion :(" << endl; }
    //}

void IA_system::createDecisionTreeDefensivo(){
    rootDefensivo = make_unique<IntDecision>(10);
    rootDefensivo->falseNode = make_unique<Action>(0);
    rootDefensivo->trueNode = make_unique<FloatDecision>(9.5);
    FloatDecision* d = dynamic_cast<FloatDecision*>(rootDefensivo->trueNode.get());
    d->trueNode = make_unique<Action>(2);
    d->falseNode = make_unique<Action>(3);
}

void IA_system::createDecisionTreeOfensivo(){
    rootOfensivo = make_unique<IntDecision>(10);
    rootOfensivo->falseNode = make_unique<Action>(0);
    rootOfensivo->trueNode = make_unique<IntDecision>(9);
    IntDecision* d = dynamic_cast<IntDecision*>(rootOfensivo->trueNode.get());
    d->trueNode = make_unique<Action>(2);
    d->falseNode = make_unique<Action>(3);
}

void IA_system::createDecisionTreeLanzador(){
    rootLanzador = make_unique<IntDecision>(10);
    rootLanzador->falseNode = make_unique<Action>(0);
    rootLanzador->trueNode = make_unique<IntDecision>(9);
    IntDecision* d = dynamic_cast<IntDecision*>(rootLanzador->trueNode.get());
    d->trueNode = make_unique<Action>(2);
    d->falseNode = make_unique<Action>(3);
}

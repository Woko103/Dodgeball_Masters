#include <facade2.h>

void Facade2::startGraphics(Entity_Manager* entity_manager)
{
    this->entity_manager = entity_manager;
    renderEntities();
    inputs = "";
}

void Facade2::initDevice(Entity_Manager* entity_manager)
{
    // Initialize entity manager reference
    this->entity_manager = entity_manager;

    // Initialize graphics engine
    graphicsEngine = make_unique<MyGraphicsEngine>();
    graphicsEngine->initDevice("Dodgeball Masters");

    // Initialize graphics engine resource manager
    resourceManager = make_unique<Resource_Manager>();

    // Initialize camera
    camera = make_unique<MyCamera>(graphicsEngine->device->window, graphicsEngine->lightingShader, graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight());

    // Initialize lights
    vec3 lightPosition(0.0f, 5.0f, 0.0f);
    pointLight = make_unique<MyLight>(graphicsEngine->lightingShader, camera.get(), graphicsEngine->device->window);
    pointLight->createPointLight(lightPosition);

    // Initialize IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(graphicsEngine->device->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Facade2::renderEntities()
{
    rootNode = make_unique <MyNode>(nullptr);

    auto& entitiesMap = entity_manager->getMapEntities();

    std::map < const char*, unique_ptr<IEntity> >::iterator itr;

    for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
    {
        //cout << itr->first << endl;

        if(!resourceManager->modelExists(itr->second->characterId))
        {
            resourceManager->addModel(itr->second->characterId, new MyModel(graphicsEngine->device->window, graphicsEngine->lightingShader, itr->second->meshPath));
        }

        MyModel* entity = resourceManager->getModel(itr->second->characterId);
        MyNode* child = itr->second->node.myNode = new MyNode(entity);


        if(strcmp(itr->first, "zcubo_mapa") == 0) // Set map cube node properties
        {
            child->setRotation(vec3(0, 90, 0));
            child->setScale(vec3(0.05f));
            if(strcmp(entity_manager->getSelectedMapID(), "Jungla") == 0){
              entity->setTransparency(0.3f);
            }
            else{
              entity->setTransparency(0.8f);
            }
        }
        else if(strncasecmp(itr->first, "map", 3) == 0) // Set map node properties
        {
            child->setRotation(vec3(0, 90, 0));
            child->setScale(vec3(0.05f));
        }
        else if(strncasecmp(itr->first, "n_p", 3) == 0)
        {
            child->setRotation(vec3(90, 0, -90));
            entity->setTransparency(0.6f);
        }

        if(!strncasecmp(itr->first, "n_b", 3) == 0 && !strncasecmp(itr->first, "n_p", 3) == 0){

            child->setScale(vec3(0.05f));
        }

        if(strcmp(itr->second->meshPath, "meshes/KStar/KStar") == 0 || strcmp(itr->second->meshPath, "meshes/Cytrhia/Cytrhia") == 0)
        {
            child->setScale(vec3(0.17f));
        }

        rootNode->addChild(child);
    }

    initColliders(); // Initialize the nodes colliders
}

void Facade2::updateGraphics(Entity_Manager* entity_manager)
{
    graphicsEngine->clear();

    graphicsEngine->pollEvents();

    this->updateEntitiesPosition(entity_manager);

    // Render entities
    rootNode->travel(mat4(1.0f));

    // Render interface IMGUI after rendering etities
    renderHUD();

    // Updating light
    pointLight->updateLight();

    // Updating camera
    camera->update(multiplayerID);
    auto& entitiesMap = entity_manager->getMapEntities();
    bool stop = false;
    std::map < const char*, unique_ptr<IEntity> >::iterator itr;
    for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++){
        auto& entity = itr->second;
        if(entity->control.esPlayer){
            camera->follow(entity->position.X, entity->position.Y, entity->position.Z, multiplayerID);
        }
    }

    graphicsEngine->swapBuffers();
}

void Facade2::updatePlayerMovement(IEntity* player)
{
     double dash;

    MyModel* m = (MyModel*)player->node.myNode->getMyEnt();

    if(graphicsEngine->getKey(GLFW_KEY_LEFT_SHIFT) && player->stamina.stamina > 249)
    {
        if(player->dash.count == true && player->dash.dodgeCoolDown <= getTime(0))
        {
            player->audio.paramValue = 2;
            player->audio.suena=true;
            if(!player->control.powered || player->control.powerType != 2){
                player->stamina.stamina -= 250;
            }
            player->dash.dodgeCoolDown = getTime(5);
            player->dash.dodgeTime = getTime(0.4);
            player->dash.count = false;
            if(player->dash.dodgeTime > getTime(0)){
              setAnim(player, 4);
            }
        }
    }
    if(player->dash.dodgeTime > getTime(0)){
        dash = player->dash.value;
    }
    else
    {
        dash = 0;
        if(player->dash.dodgeCoolDown <= getTime(0))
            player->dash.count = true;
    }

    if(player->control.powered && player->control.powerType == 1){
      if(player->control.powerTime <= getTime(0)){
        player->control.powered = false;
        player->speed.powerMax = player->speed.initialPower;
      }
    }

    float speed;

    if (dash == 0){
      speed = player->speed.power;
    }
    else
        speed = 0.02 + dash;

    if(graphicsEngine->getKey(GLFW_KEY_W) && (player->position.Z > 0.15 && player->team.team == "Blue") || graphicsEngine->getKey(GLFW_KEY_S) && (player->position.Z > -3.4 && player->team.team == "Red"))
    {
        if(player->dash.dodgeTime <= getTime(0)){
            setAnim(player, 1);
        }

        if(graphicsEngine->getKey(GLFW_KEY_A) && player->position.X > -2.5 && player->team.team == "Blue" || graphicsEngine->getKey(GLFW_KEY_D) && player->position.X > -2.5 && player->team.team == "Red")
        {
            player->position.Z -= speed * 0.5;
            player->position.X -= speed * 0.5;
            player->speed.Z = speed * 0.5;
            player->speed.X = speed * 0.5;
            player->node.myNode->setRotation(vec3(0, 45, 0));
        }
        else if(graphicsEngine->getKey(GLFW_KEY_D) && player->position.X < 2.5 && player->team.team =="Blue" || graphicsEngine->getKey(GLFW_KEY_A) && player->position.X < 2.5 && player->team.team =="Red")
        {
            player->position.Z -= speed * 0.5;
            player->position.X += speed * 0.5;
            player->speed.Z = speed * 0.5;
            player->speed.X = -speed * 0.5;
            player->node.myNode->setRotation(vec3(0, -45, 0));
        }
        else
        {
            player->position.Z -= speed;
            player->speed.Z = speed;
            player->speed.X = 0;
            player->node.myNode->setRotation(vec3(0, 0, 0));
        }
    }
    else if(graphicsEngine->getKey(GLFW_KEY_S) && (player->position.Z < 3.4 && player->team.team == "Blue") || graphicsEngine->getKey(GLFW_KEY_W) && (player->position.Z < -0.15 && player->team.team == "Red"))
    {
        if(player->dash.dodgeTime <= getTime(0)){
            setAnim(player, 1);
        }

        if(graphicsEngine->getKey(GLFW_KEY_A) && player->position.X > -2.5 && player->team.team == "Blue" || graphicsEngine->getKey(GLFW_KEY_D) && player->position.X > -2.5 && player->team.team == "Red")
        {
            player->position.Z += speed * 0.5;
            player->position.X -= speed * 0.5;
            player->speed.Z = -speed * 0.5;
            player->speed.X = speed * 0.5;
            player->node.myNode->setRotation(vec3(0, 135, 0));
        }
        else if(graphicsEngine->getKey(GLFW_KEY_D) && player->position.X < 2.5 && player->team.team == "Blue" || graphicsEngine->getKey(GLFW_KEY_A) && player->position.X < 2.5 && player->team.team == "Red" )
        {
            player->position.Z += speed * 0.5;
            player->position.X += speed * 0.5;
            player->speed.Z = -speed * 0.5;
            player->speed.X = -speed * 0.5;
            player->node.myNode->setRotation(vec3(0, -135, 0));
        }
        else
        {
            player->position.Z += speed;
            player->speed.Z = -speed;
            player->speed.X = 0;
            player->node.myNode->setRotation(vec3(0, 180, 0));
        }
    }
    else if(graphicsEngine->getKey(GLFW_KEY_A) && player->position.X > -2.5 && player->team.team == "Blue" || graphicsEngine->getKey(GLFW_KEY_D) && player->position.X > -2.5 && player->team.team == "Red")
    {
        if(player->dash.dodgeTime <= getTime(0)){
            setAnim(player, 1);
        }
        player->position.X -= speed;
        player->speed.X = speed;
        player->speed.Z = 0;
        player->node.myNode->setRotation(vec3(0, 90, 0));
    }
    else if(graphicsEngine->getKey(GLFW_KEY_D) && player->position.X < 2.5 && player->team.team == "Blue" || graphicsEngine->getKey(GLFW_KEY_A) && player->position.X < 2.5 && player->team.team == "Red" )
    {
        if(player->dash.dodgeTime <= getTime(0)){
            setAnim(player, 1);
        }
        player->position.X += speed;
        player->speed.X = -speed;
        player->speed.Z = 0;
        player->node.myNode->setRotation(vec3(0, -90, 0));
    }
    else
    {
        player->speed.X = 0;
        player->speed.Z = 0;
        setAnim(player, 0);
    }
    if(player->stamina.stamina < 0){
      player->stamina.stamina = 0;
    }

    if(player->speed.X == 0 && player->speed.Z == 0){
      if(player->team.team == "Blue"){
        player->node.myNode->setRotation(vec3(0, 0, 0));
      }
      else{
        player->node.myNode->setRotation(vec3(0, 180, 0));
      }
    }

    if(player->position.X < -2.6){
      player->position.X = -2.6;
    }
    else if(player->position.X > 2.6){
      player->position.X = 2.6;
    }
}

void Facade2::updateEntitiesPosition(Entity_Manager* entity_manager){
    auto& entitiesMap = entity_manager->getMapEntities();
    std::map < const char*, unique_ptr<IEntity> >::iterator itr;
    for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
    {
        auto& entity = itr->second;
        vec3 compa = vec3(entity->position.X, entity->position.Y, entity->position.Z);
        if(entity->node.myNode->getPosition() != compa){
            entity->node.myNode->setPosition(compa);
        }
    }
}

void Facade2::updatePlayerMovementJoystick(IEntity* player){
    double dash;
    //Mando y joystick
    int count;
    //Lei por ahi que este puntero se lo carga al final GLFW asi que se puede quedar
    const float* axes = graphicsEngine->getJoystickAxes(GLFW_JOYSTICK_1, &count);
    MyModel* m = (MyModel*)player->node.myNode->getMyEnt();

    //Boton del mando
    if(buttons()==2 && player->stamina.stamina > 249)
    {
        if(player->dash.count == true && player->dash.dodgeCoolDown <= getTime(0))
        {
            player->audio.paramValue = 2;
            player->audio.suena=true;
            if(!player->control.powered || player->control.powerType != 2){
                player->stamina.stamina -= 250;
            }
            player->dash.dodgeCoolDown = getTime(5);
            player->dash.dodgeTime = getTime(0.4);
            player->dash.count = false;
            if(player->dash.dodgeTime > getTime(0)){
            setAnim(player, 4);
        }
        }
    }
    if(player->dash.dodgeTime > getTime(0)){
        dash = player->dash.value;
    }
    else
    {
        dash = 0;
        if(player->dash.dodgeCoolDown <= getTime(0))
            player->dash.count = true;
    }

    if(player->control.powered && player->control.powerType == 1){
      if(player->control.powerTime <= getTime(0)){
        player->control.powered = false;
        player->speed.powerMax = player->speed.initialPower;
      }
    }

    float speed;

    if (dash == 0){
      speed = player->speed.power;
    }
    else
        speed = 0.02 + dash;

    //La comprobacion se hace para que si el joystick esta muy poco desplazado
    //no se mueva directamente
    if(abs(axes[1])<0.1 || ((player->position.Z <= 0.15 && axes[1]<0 || player->position.Z >=3.4 && axes[1]>0) && player->team.team == "Blue") || ((player->position.Z <= -3.4 && axes[1]>0 || player->position.Z >= -0.15 && axes[1]<0 ) && player->team.team == "Red")){
        player->speed.Z = 0;
    }else{
        if(player->dash.dodgeTime <= getTime(0)){
            setAnim(player, 1);
        }
        if(player->team.team == "Blue")
            player->speed.Z = speed * axes[1];
        else if(player->team.team == "Red")
            player->speed.Z = -(speed * axes[1]);


    }
    if(abs(axes[0])<0.1 || ((player->position.X <= -2.5 && axes[0]<0 || player->position.X >= 2.5 && axes[0]>0) && player->team.team == "Blue") || ((player->position.X <= -2.5 && axes[0]>0 || player->position.X >= 2.5 && axes[0]<0) && player->team.team == "Red")){
        player->speed.X = 0;
    }
    else{
        if(player->dash.dodgeTime <= getTime(0)){
            setAnim(player, 1);
        }
        if(player->team.team == "Blue")
            player->speed.X = speed * axes[0];
        else if(player->team.team == "Red")
            player->speed.X = -(speed * axes[0]);
    }

    if(player->speed.X == 0 && player->speed.Z == 0){
        setAnim(player, 0);
    }

    float rotX = player->speed.X / player->speed.power;
    float rotZ = 180;

    if (player->speed.Z > 0)
    {
        rotX *= 90;
        rotZ += rotX;
    }
    else if (player->speed.Z < 0)
    {
        rotX *= -90;
        rotZ = rotX;
    }
    if(player->speed.Z == 0){
        if(rotX >0){
            rotX *= 90;
            rotZ += rotX;
        }
        else if(rotX<0){
            rotX *= -90;
            rotZ = rotX;
        }
    }

    player->node.myNode->setRotation(vec3 (0, rotZ, 0));

    player->position.Z += player->speed.Z;
    player->position.X += player->speed.X;

    if(player->stamina.stamina < 0){
      player->stamina.stamina = 0;
    }

    if(player->speed.X == 0 && player->speed.Z == 0){
      if(player->team.team == "Blue"){
        player->node.myNode->setRotation(vec3(0, 0, 0));
      }
      else{
        player->node.myNode->setRotation(vec3(0, 180, 0));
      }
    }

    if(player->position.X < -2.6){
      player->position.X = -2.6;
    }
    else if(player->position.X > 2.6){
      player->position.X = 2.6;
    }
}

void Facade2::updatePlayerMovement2(IEntity* player)
{
    {
    double dash;

    if(graphicsEngine->getKey(GLFW_KEY_PERIOD) && player->stamina.stamina > 249)
    {
        if(player->dash.count == true)
        {
            player->audio.paramValue = 2;
            player->audio.suena=true;
            if(!player->control.powered || player->control.powerType != 2){
                player->stamina.stamina -= 250;
            }
            player->dash.dodgeCoolDown = getTime(5);
            player->dash.dodgeTime = getTime(0.4);
            player->dash.count = false;
        }
    }
    if(player->dash.dodgeTime > getTime(0))
        dash = player->dash.value;
    else
    {
        dash = 0;
        if(player->dash.dodgeCoolDown <= getTime(0))
            player->dash.count = true;
    }

    if(player->control.powered && player->control.powerType == 1){
      if(player->control.powerTime <= getTime(0)){
        player->control.powered = false;
        player->speed.powerMax = player->speed.initialPower;
      }
    }

    float speed;

    if (dash == 0){
      speed = player->speed.power;
    }
    else
        speed = 0.02 + dash;

    if(graphicsEngine->getKey(GLFW_KEY_UP))
    {
        setAnim(player, 1);
        if(graphicsEngine->getKey(GLFW_KEY_LEFT))
        {
            player->position.Z -= speed * 0.5;
            player->position.X -= speed * 0.5;
            player->speed.Z = speed * 0.5;
            player->speed.X = speed * 0.5;
            player->node.myNode->setRotation(vec3(0, 45, 0));
        }
        else if(graphicsEngine->getKey(GLFW_KEY_RIGHT))
        {
            player->position.Z -= speed * 0.5;
            player->position.X += speed * 0.5;
            player->speed.Z = speed * 0.5;
            player->speed.X = -speed * 0.5;
            player->node.myNode->setRotation(vec3(0, -45, 0));
        }
        else
        {
            player->position.Z -= speed;
            player->speed.Z = speed;
            player->speed.X = 0;
            player->node.myNode->setRotation(vec3(0, 0, 0));
        }
    }
    else if(graphicsEngine->getKey(GLFW_KEY_DOWN))
    {
        setAnim(player, 1);
        if(graphicsEngine->getKey(GLFW_KEY_LEFT))
        {
            player->position.Z += speed * 0.5;
            player->position.X -= speed * 0.5;
            player->speed.Z = -speed * 0.5;
            player->speed.X = speed * 0.5;
            player->node.myNode->setRotation(vec3(0, 135, 0));
        }
        else if(graphicsEngine->getKey(GLFW_KEY_RIGHT))
        {
            player->position.Z += speed * 0.5;
            player->position.X += speed * 0.5;
            player->speed.Z = -speed * 0.5;
            player->speed.X = -speed * 0.5;
            player->node.myNode->setRotation(vec3(0, -135, 0));
        }
        else
        {
            player->position.Z += speed;
            player->speed.Z = -speed;
            player->speed.X = 0;
            player->node.myNode->setRotation(vec3(0, 180, 0));
        }

        if(!player->control.powered || player->control.powerType != 2){
            player->stamina.stamina -= 1;
        }
    }
    else if(graphicsEngine->getKey(GLFW_KEY_LEFT))
    {
        setAnim(player, 1);
        player->position.X -= speed;
        if(!player->control.powered || player->control.powerType != 2){
            player->stamina.stamina -= 1;
        }
        player->speed.X = speed;
        player->speed.Z = 0;
        player->node.myNode->setRotation(vec3(0, 90, 0));
    }
    else if(graphicsEngine->getKey(GLFW_KEY_RIGHT))
    {
        setAnim(player, 1);
        player->position.X += speed;
        if(!player->control.powered || player->control.powerType != 2){
            player->stamina.stamina -= 1;
        }
        player->speed.X = -speed;
        player->speed.Z = 0;
        player->node.myNode->setRotation(vec3(0, -90, 0));
    }
    else
    {
        player->speed.X = 0;
        player->speed.Z = 0;
        setAnim(player, 0);
    }
    if(player->stamina.stamina < 0){
      player->stamina.stamina = 0;
    }
}
}

//Devuelve el nombre de la entidad a eliminar, o null si no elimina nada
const char* Facade2::elimination(){

    auto& entitiesMap = entity_manager->getMapEntities();
    std::map < const char*, unique_ptr<IEntity> >::iterator itr;

    bool stop = false;

    for(itr = entitiesMap.begin(); itr != entitiesMap.end() && !stop; itr++)
    {
      auto& entity = itr->second;

      if(entity->control.esPelota && isEntityColliding(entity.get())){
        stop = true;
        const char* collisionID = entity->node.myNode->getCollisions()->at(0)->getName();
        std::string ballTeam = entity->team.team;

        for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
        {
          if(itr->first == collisionID && itr->second->control.esMultiplayer)
          {
              return nullptr;
          }
        }

        if(strcmp(collisionID, "Aplayer") == 0 || strncasecmp(collisionID, "n_n", 3) == 0)
        {
            if(ballTeam.compare("Red") == 0)
            {
                return collisionID;
            }
        }

        else if(strncasecmp(collisionID, "n_e", 3) == 0)
        {
            if(ballTeam.compare("Blue") == 0)
            {
                return collisionID;
            }
        }

        else if(strcmp(collisionID, "n_powerup_01") == 0)
        {
            return collisionID;
        }
      }
    }

    return nullptr;
}

//Metodo para eliminar una entidad al haberla golpeado
void Facade2::deleteCharacter(Entity_Manager* entity_manager, const char* del){
  bool stop = false;

  auto& entity = entity_manager->getEntity(del);
  entity->node.myNode->getParent()->removeChild(entity->node.myNode);
  entity_manager->removeEntity(del);

  auto& ball = entity_manager->getEntity("n_ball_01");
  auto ball1Collisions = ball->node.myNode->getCollisions();
  ball1Collisions->clear();
}

//Metodo para eliminar todas las entidades
void Facade2::deleteAllCharacters()
{
    auto& entitiesMap = entity_manager->getMapEntities();
    for(auto itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
    {
        auto& entity = itr->second;
        //Revisar
        if(strcmp(entity->id, "n_ball_01") == 0)
        {
            rootNode->addChild(entity->node.myNode);
        }

        if(entity->node.myNode !=nullptr){
            entity->node.myNode->getParent()->removeChild(entity->node.myNode);
        }
    }
    entity_manager->removeAllEntities();

}

//Metodo para obtener el tiempo de espera para realizar una accion o para obtener el tiempo actual del device
double Facade2::getTime(double plus){
    if(plus != 0){
    //Se le suma al tiempo actual la cantidad de tiempo que se quiere retrasar una accion
    return graphicsEngine->getTime() + plus;
    }
    else{
    //Devuelve el tiempo actual
    return graphicsEngine->getTime();
  }
}

void Facade2::setTime(double plus){
    graphicsEngine->setTime(plus);
}

//Metodo que comprueba cuando se pulsa una tecla
const char* Facade2::checkInput(){
    if(graphicsEngine->getKey(GLFW_KEY_SPACE)) //CATCH
    {
        inputs = "E";
        return "E";
    }
    else if(graphicsEngine->getMouseButton(GLFW_MOUSE_BUTTON_LEFT))
    {
        inputs = "SP";
        return "LEFTMOUSE";
    }
    else if(graphicsEngine->getKey(GLFW_KEY_R) || graphicsEngine->getMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) //PASS
    {
        if(graphicsEngine->getKey(GLFW_KEY_A))
        {
            if(graphicsEngine->getKey(GLFW_KEY_W))
            {
                inputs = "RAW";
                return "RAW";
            }
            if(graphicsEngine->getKey(GLFW_KEY_S))
            {
                inputs = "RAS";
                return "RAS";
            }
            inputs = "RA";
            return "RA";
        }
        if(graphicsEngine->getKey(GLFW_KEY_D))
        {
            if(graphicsEngine->getKey(GLFW_KEY_W))
            {
                inputs = "RDW";
                return "RDW";
            }
            if(graphicsEngine->getKey(GLFW_KEY_S))
            {
                inputs = "RDS";
                return "RDS";
            }
            inputs = "RD";
            return "RD";
        }
        if(graphicsEngine->getKey(GLFW_KEY_S))
        {
            inputs = "RS";
            return "RS";
        }
        inputs = "R";
        return "R";
    }
    else if(graphicsEngine->getKey(GLFW_KEY_Q)){ //SUPER THROW
        inputs = "Y";
        return "Y";
    }
    else if(graphicsEngine->getKey(GLFW_KEY_C)){ //CAMBIAR PLAYER
        inputs = "C";
        return "C";
    }
    else if(graphicsEngine->getKey(GLFW_KEY_W)){
        inputs = "W";
        return "W";
    }
    else if(graphicsEngine->getKey(GLFW_KEY_A)){
        inputs = "A";
        return "A";
    }
    else if(graphicsEngine->getKey(GLFW_KEY_S)){
        inputs = "S";
        return "S";
    }
    else if(graphicsEngine->getKey(GLFW_KEY_D)){
        inputs = "D";
        return "D";
    }
    else if(graphicsEngine->getKey(GLFW_KEY_1)){
        inputs = "1";
        return "1";
    }
    else if(graphicsEngine->getKey(GLFW_KEY_2)){
        inputs = "2";
        return "2";
    }
    else if(graphicsEngine->getKey(GLFW_KEY_ESCAPE)){
        inputs = "P";
        return "P";
    }
    return "z";
}

char Facade2::checkInputPlayer2()
{
    if(graphicsEngine->getKey(GLFW_KEY_M)) //CATCH
    {
        return 'M';
    }
    else if(graphicsEngine->getKey(GLFW_KEY_N)){ //THROW
        return 'N';
    }
    else if(graphicsEngine->getKey(GLFW_KEY_B)) //PASS
    {
        return 'B';
    }
    else if(graphicsEngine->getKey(GLFW_KEY_V)){ //SUPER THROW
        return 'V';
    }
    else if(graphicsEngine->getKey(GLFW_KEY_COMMA)){ //CAMBIAR PLAYER
        return ',';
    }
    return 'x';
}

// INTERFACE METHODS IMGUI
void Facade2::InitGUIElements()
{
    auto& guiTexturesMap = entity_manager->getMapGuiTextures();

    std::map < const char*, unique_ptr<IGuiTexture> >::iterator itr;
    for(itr = guiTexturesMap.begin(); itr != guiTexturesMap.end(); itr++)
    {
        bool ret = graphicsEngine->loadTextureFromFile(itr->second->texturePath, &itr->second->texture, &itr->second->width, &itr->second->height);
        IM_ASSERT(ret);
    }
}

int Facade2::renderMainMenu()
{
    int devol = 0;

    graphicsEngine->clear();

    graphicsEngine->pollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Set window full screen mode and full transparency
    ImGui::SetNextWindowSize(ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0);

    ImGui::Begin("Hello, world!", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    int windowWidth = graphicsEngine->device->getWindowWidth();
    int windowHeight = graphicsEngine->device->getWindowHeight();

    float windowRelWidth = (float)windowWidth / 1920.0f;
    float windowRelHeight = (float)windowHeight / 1080.0f;

    ImGui::SetCursorPos(ImVec2(0, 0));

    ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("DodgeballBGNoTitle")->texture, ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    ImGui::SetCursorPos(ImVec2((windowWidth/2)- (entity_manager->getGuiTexture("DodgeballMastersTitle2")->width*0.9f)/2 * windowRelWidth, 0));
    ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("DodgeballMastersTitle2")->texture, ImVec2(entity_manager->getGuiTexture("DodgeballMastersTitle2")->width*0.9f * windowRelWidth, entity_manager->getGuiTexture("DodgeballMastersTitle2")->height*0.9f * windowRelHeight));

    int pops = 0;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
    pops++;

    ImGui::SetCursorPos(ImVec2((windowWidth/2)- (entity_manager->getGuiTexture("MastersCup")->width*0.8)/2 * windowRelWidth, entity_manager->getGuiTexture("DodgeballMastersTitle2")->height * windowRelHeight /*/ 1.1*/));
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("MastersCup")->texture, ImVec2(entity_manager->getGuiTexture("MastersCup")->width * 0.8 * windowRelWidth, entity_manager->getGuiTexture("MastersCup")->height * 0.8 * windowRelHeight))){
        devol = 1;
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }
    ImGui::SetCursorPos(ImVec2((windowWidth/2)- (entity_manager->getGuiTexture("Multiplayer")->width*0.8 )/2 * windowRelWidth, entity_manager->getGuiTexture("DodgeballMastersTitle2")->height  * windowRelHeight+ entity_manager->getGuiTexture("MastersCup")->height * windowRelHeight));
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Multiplayer")->texture, ImVec2(entity_manager->getGuiTexture("Multiplayer")->width * 0.8 * windowRelWidth, entity_manager->getGuiTexture("Multiplayer")->height * 0.8 * windowRelHeight))){
        devol =  2;
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }
    ImGui::SetCursorPos(ImVec2((windowWidth/2)- (entity_manager->getGuiTexture("Credits")->width*0.8)/2 * windowRelWidth, entity_manager->getGuiTexture("DodgeballMastersTitle2")->height * windowRelHeight + entity_manager->getGuiTexture("MastersCup")->height + entity_manager->getGuiTexture("Multiplayer")->height * windowRelHeight));
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Credits")->texture, ImVec2(entity_manager->getGuiTexture("Credits")->width * 0.8 * windowRelWidth, entity_manager->getGuiTexture("Credits")->height * 0.8 * windowRelHeight))){
        devol = 6;
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }
    ImGui::SetCursorPos(ImVec2((windowWidth/2)- (entity_manager->getGuiTexture("Controls")->width)*1.2/2 * windowRelWidth, entity_manager->getGuiTexture("DodgeballMastersTitle2")->height * windowRelHeight + entity_manager->getGuiTexture("MastersCup")->height + entity_manager->getGuiTexture("Multiplayer")->height + entity_manager->getGuiTexture("Credits")->height * windowRelHeight));
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Controls")->texture, ImVec2(entity_manager->getGuiTexture("Controls")->width *1.2 * windowRelWidth, entity_manager->getGuiTexture("Controls")->height*1.2 * windowRelHeight))){
        devol = 7;
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }
    ImGui::SetCursorPos(ImVec2((windowWidth/2)- (entity_manager->getGuiTexture("Exit")->width*0.8)/2 * windowRelWidth, entity_manager->getGuiTexture("DodgeballMastersTitle2")->height * windowRelHeight + entity_manager->getGuiTexture("MastersCup")->height + entity_manager->getGuiTexture("Multiplayer")->height + entity_manager->getGuiTexture("Credits")->height + entity_manager->getGuiTexture("Controls")->height*1.4 *windowRelHeight));
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Exit")->texture, ImVec2(entity_manager->getGuiTexture("Exit")->width * 0.8 * windowRelWidth, entity_manager->getGuiTexture("MastersCup")->height * 0.8 * windowRelHeight))){
        devol =  5;
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 3;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }
    ImGui::PopStyleColor(pops);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    graphicsEngine->swapBuffers();

    return devol;
}

int Facade2::renderCharacterSelectMenu()
{
    int devol = 1;

    graphicsEngine->clear();

    graphicsEngine->pollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Get window size
    int windowWidth = graphicsEngine->device->getWindowWidth();
    int windowHeight = graphicsEngine->device->getWindowHeight();

    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0);

    ImGui::Begin("Menu de personajes (LOCAL)", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if(characterRotationIndex > 2)
        characterRotationIndex = 0;

    float windowRelWidth = (float)windowWidth / 1920.0f;
    float windowRelHeight = (float)windowHeight / 1080.0f;

    IGuiTexture* img = entity_manager->getGuiTexture("FondoDesenfocado").get();
    float imgWidth = img->width * windowRelWidth;
    float imgHeight = img->height * windowRelHeight;

    ImGui::SetCursorPos(ImVec2(windowWidth/2 - imgWidth/2, windowHeight/2 - imgHeight/2));
    ImGui::Image((void*)(intptr_t)img->texture, ImVec2(imgWidth, imgHeight));

    img = entity_manager->getGuiTexture("SelectCharacter").get();
    imgWidth = img->width * windowRelWidth;
    imgHeight = img->height * windowRelHeight;

    ImGui::SetCursorPos(ImVec2(windowWidth / 2.0f - imgWidth * 1.10f / 2.0f, 0));
    ImGui::Image((void*)(intptr_t)img->texture, ImVec2(imgWidth * 1.10f, imgHeight * 1.10f));

    // Image position references
    float xRelPos = 0;
    float yRelPos = 0.2f;

    int i = 0;
    // Iterate through the characters map
    auto& charactersMap = entity_manager->getMapCharacters();

    std::map < const char*, unique_ptr<ICharacter> >::iterator itr;
    for(itr = charactersMap.begin(); itr != charactersMap.end(); itr++)
    {
        i++;

        const char* characterID = itr->second->id;
        char characterTexture[25];
        strcpy(characterTexture, characterID);

        if(strcmp(characterID, "SilbatoMan") != 0 && strcmp(characterID, "music") != 0 && strcmp(characterID, "HardSFX") != 0)
        {
            // Display character image with orange border if selected
            if(entity_manager->getCharacter(characterID)->selected)
            {
                strcat(characterTexture, "_border");
            }

            //cout << characterTexture << endl;

            img = entity_manager->getGuiTexture(characterTexture).get();
            imgWidth = img->width * windowRelWidth;
            imgHeight = img->height * windowRelHeight;

            if(i == 5)
            {
                xRelPos = 0;
                yRelPos += 0.2f;
            }

            xRelPos += 0.2f;

            int xPos = (xRelPos * windowWidth) - (imgWidth / 2.0f);
            int yPos = (yRelPos * windowHeight) - (imgHeight / 2.0f);

            ImGui::SetCursorPos(ImVec2(xPos, yPos));
            if(ImGui::ImageButton((void*)(intptr_t)img->texture, ImVec2(imgWidth, imgHeight), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 1)))
            {
                entity_manager->getCharacter(characterID)->selected = !entity_manager->getCharacter(characterID)->selected;

                if(entity_manager->getCharacter(characterID)->selected) // Select character
                {
                    //cout << characterID << " Seleccionado" << endl;

                    entity_manager->getCharacter(characterID)->audio.suena = true;
                    entity_manager->insertSelectedCharacter(characterRotationIndex, entity_manager->getCharacter(characterID).get());

                    characterRotationIndex ++;
                }
                else // Unselect character
                {
                    entity_manager->removeSelectedCharacter(entity_manager->getCharacter(characterID).get());
                }
            }
        }
    }

    xRelPos = 0;
    yRelPos = 0.75f;

    // Display selected characters images
    for(int i = 0; i < 3; i++)
    {
        xRelPos += 0.25f;

        ICharacter* character = entity_manager->selectedCharacters[i];

        if(character)
        {
            char characterID[20];
            strcpy(characterID, character->id); // copy string one into the result.
            strcat(characterID, "_selected");

            IGuiTexture* characterImage = entity_manager->getGuiTexture(characterID).get();

            int xPos = (windowWidth * xRelPos) - (characterImage->width * windowRelWidth / 2.0f);
            int yPos = (windowHeight * yRelPos) - (characterImage->height * windowRelHeight / 2.0f);

            ImGui::SetCursorPos(ImVec2(xPos, yPos));
            ImGui::Image((void*)(intptr_t)characterImage->texture, ImVec2(characterImage->width * windowRelWidth, characterImage->height * windowRelHeight));
        }
    }

    if(entity_manager->selectedCharactersInOrder()) // Render play button if there are at least 3 selected characters (Now at least 1)
    {
        img = entity_manager->getGuiTexture("SelectMap").get();
        imgWidth = img->width * windowRelWidth;
        imgHeight = img->height * windowRelHeight;

        ImGui::SetCursorPos(ImVec2(windowWidth / 2.0f - imgWidth / 2.0f, windowHeight - imgHeight));
        if(ImGui::ImageButton((void*)(intptr_t)img->texture, ImVec2(imgWidth, imgHeight))){
            entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
            entity_manager->getCharacter("HardSFX")->audio.suena = true;
            devol =  3;
        }
    }

    img = entity_manager->getGuiTexture("Back").get();
    imgWidth = img->width * windowRelWidth;
    imgHeight = img->height * windowRelHeight;

    int pops = 0;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
    pops++;

    ImGui::SetCursorPos(ImVec2(0, windowHeight - imgHeight));
    if(ImGui::ImageButton((void*)(intptr_t)img->texture, ImVec2(imgWidth, imgHeight))){
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 3;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
        devol =  0;
    }

    ImGui::PopStyleColor(pops);


    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    graphicsEngine->swapBuffers();

    return devol;
}

int Facade2::renderMultiplayerCharacterSelectMenu()
{
    int devol = 2;

    graphicsEngine->clear();

    graphicsEngine->pollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0);
    ImGui::Begin("Menu de personajes (Multiplayer)", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if(characterRotationIndex > 2)
        characterRotationIndex = 0;

    // Get window size
    int windowWidth = graphicsEngine->device->getWindowWidth();
    int windowHeight = graphicsEngine->device->getWindowHeight();

    float windowRelWidth = (float)windowWidth / 1920.0f;
    float windowRelHeight = (float)windowHeight / 1080.0f;

    ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("FondoDesenfocado")->texture, ImVec2(windowWidth, windowHeight));

    IGuiTexture* img = entity_manager->getGuiTexture("SelectCharacter").get();
    float imgWidth = img->width * windowRelWidth;
    float imgHeight = img->height * windowRelHeight;

    ImGui::SetCursorPos(ImVec2(windowWidth / 2.0f - imgWidth * 1.10f / 2.0f, 0));
    ImGui::Image((void*)(intptr_t)img->texture, ImVec2(imgWidth * 1.10f, imgHeight * 1.10f));

    // Image position references
    float xRelPos = 0;
    float yRelPos = 0.2f;

    int i = 0;
    // Iterate through the characters map
    auto& charactersMap = entity_manager->getMapCharacters();

    std::map < const char*, unique_ptr<ICharacter> >::iterator itr;
    for(itr = charactersMap.begin(); itr != charactersMap.end(); itr++)
    {
        i++;

        const char* characterID = itr->second->id;

        if(strcmp(characterID, "SilbatoMan") != 0 && strcmp(characterID, "music") != 0 && strcmp(characterID, "HardSFX") != 0)
        {
            //cout << characterID << endl;

            if(i == 5)
            {
                xRelPos = 0;
                yRelPos += 0.2f;
            }

            xRelPos += 0.2f;

            int xPos = (xRelPos * windowWidth) - (entity_manager->getGuiTexture(characterID)->width * windowRelWidth / 2.0f);
            int yPos = (yRelPos * windowHeight) - (entity_manager->getGuiTexture(characterID)->height * windowRelHeight / 2.0f);

            ImGui::SetCursorPos(ImVec2(xPos, yPos));
            if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture(characterID)->texture, ImVec2(entity_manager->getGuiTexture(characterID)->width * windowRelWidth, entity_manager->getGuiTexture(characterID)->height * windowRelHeight)))
            {
                entity_manager->getCharacter(characterID)->selected = !entity_manager->getCharacter(characterID)->selected;

                if(entity_manager->getCharacter(characterID)->selected) // Select character
                {
                    //cout << characterID << " Seleccionado" << endl;

                    entity_manager->getCharacter(characterID)->audio.suena = true;
                    entity_manager->insertSelectedCharacter(characterRotationIndex, entity_manager->getCharacter(characterID).get());

                    characterRotationIndex ++;
                }
                else // Unselect character
                {
                    entity_manager->removeSelectedCharacter(entity_manager->getCharacter(characterID).get());
                }
            }
        }
    }

    xRelPos = 0;
    yRelPos = 0.75f;

    // Display selected characters images
    for(int i = 0; i < 3; i++)
    {
        xRelPos += 0.25f;

        ICharacter* character = entity_manager->selectedCharacters[i];

        if(character)
        {
            char characterID[20];
            strcpy(characterID, character->id); // copy string one into the result.
            strcat(characterID, "_selected");

            IGuiTexture* characterImage = entity_manager->getGuiTexture(characterID).get();

            int xPos = (windowWidth * xRelPos) - (characterImage->width * windowRelWidth / 2.0f);
            int yPos = (windowHeight * yRelPos) - (characterImage->height * windowRelHeight / 2.0f);

            ImGui::SetCursorPos(ImVec2(xPos, yPos));
            ImGui::Image((void*)(intptr_t)characterImage->texture, ImVec2(characterImage->width * windowRelWidth, characterImage->height * windowRelHeight));
        }
    }

    if(entity_manager->selectedCharactersLength()>0) // Render play button if there are at least 3 selected characters (Now at least 1)
    {
        img = entity_manager->getGuiTexture("SelectMap").get();
        imgWidth = img->width * windowRelWidth;
        imgHeight = img->height * windowRelHeight;

        ImGui::SetCursorPos(ImVec2(windowWidth / 2.0f - imgWidth / 2.0f, windowHeight - imgHeight));
        if(ImGui::ImageButton((void*)(intptr_t)img->texture, ImVec2(imgWidth, imgHeight)))
        {
            devol =  3;
        }
    }

    img = entity_manager->getGuiTexture("Back").get();
    imgWidth = img->width * windowRelWidth;
    imgHeight = img->height * windowRelHeight;

    int pops = 0;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
    pops++;

    ImGui::SetCursorPos(ImVec2(0, windowHeight - imgHeight));
    if(ImGui::ImageButton((void*)(intptr_t)img->texture, ImVec2(imgWidth, imgHeight)))
        devol =  0;

    ImGui::PopStyleColor(pops);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    graphicsEngine->swapBuffers();

    return devol;
}

int Facade2::renderMapSelectMenu()
{
    int devol = 3;

    graphicsEngine->clear();

    graphicsEngine->pollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize(ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0);
    ImGui::Begin("Menu de mapa", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("FondoDesenfocado")->texture, ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    int windowWidth = graphicsEngine->device->getWindowWidth();
    int windowHeight = graphicsEngine->device->getWindowHeight();
    ImGui::SetCursorPos(ImVec2((windowWidth/4)- (entity_manager->getGuiTexture("SelectMap")->width*1.10f)/2, 0));
    ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("SelectMap")->texture, ImVec2(entity_manager->getGuiTexture("SelectMap")->width*1.10f, entity_manager->getGuiTexture("SelectMap")->height*1.10f));
    ImGui::SetCursorPos(ImVec2(windowWidth * 0.165, 5 + entity_manager->getGuiTexture("SelectMap")->height*1.10f));
    
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Estadio_pre")->texture, ImVec2(entity_manager->getGuiTexture("Estadio_pre")->width * 0.5f, entity_manager->getGuiTexture("Estadio_pre")->height * 0.5f)))
    {
        entity_manager->insertSelectedMap(entity_manager->getMap("Estadio").get());
        //devol =  3;
        mapWasSelected = true;
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }
    
    ImGui::SameLine();
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Luna_pre")->texture, ImVec2(entity_manager->getGuiTexture("Luna_pre")->width * 0.5f, entity_manager->getGuiTexture("Luna_pre")->height * 0.5f)))
    {
        entity_manager->insertSelectedMap(entity_manager->getMap("Luna").get());
        //devol =  3;
        mapWasSelected = true;
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }
    
    ImGui::SameLine();
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Jungla_pre")->texture, ImVec2(entity_manager->getGuiTexture("Jungla_pre")->width * 0.5f, entity_manager->getGuiTexture("Jungla_pre")->height * 0.5f)))
    {
        entity_manager->insertSelectedMap(entity_manager->getMap("Jungla").get());
        
        mapWasSelected = true;
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }
    
    if(mapWasSelected){
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(windowWidth*0.6f - entity_manager->getGuiTexture(entity_manager->getSelectedMapText())->width*0.4/2, 20 + entity_manager->getGuiTexture("SelectMap")->height));
        ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture(entity_manager->getSelectedMapText())->texture, ImVec2(entity_manager->getGuiTexture(entity_manager->getSelectedMapText())->width*0.4, entity_manager->getGuiTexture(entity_manager->getSelectedMapText())->height*0.4));
        ImGui::SetCursorPos(ImVec2(windowWidth*0.6f - entity_manager->getGuiTexture(entity_manager->getSelectedMapID())->width*0.4/2, 20 + entity_manager->getGuiTexture("SelectMap")->height + entity_manager->getGuiTexture(entity_manager->getSelectedMapText())->height*0.4));
        ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture(entity_manager->getSelectedMapID())->texture, ImVec2(entity_manager->getGuiTexture(entity_manager->getSelectedMapID())->width*0.4, entity_manager->getGuiTexture(entity_manager->getSelectedMapID())->height*0.4));
    }
    int pops = 0;

    static bool pressEasy = false;
    static bool pressMedium = false;
    static bool pressHard = false;

    if(pressEasy && IAWasSelected){
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 150, 90)));
    }else{
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
    }

    ImGui::SetCursorPos(ImVec2((windowWidth/4)- (entity_manager->getGuiTexture("EnemiesLvl")->width)/2, windowHeight*0.15 + entity_manager->getGuiTexture("SelectMap")->height * 1.1f + entity_manager->getGuiTexture("Volcan_pre")->height * 0.5f));
    ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("EnemiesLvl")->texture, ImVec2(entity_manager->getGuiTexture("EnemiesLvl")->width, entity_manager->getGuiTexture("EnemiesLvl")->height));
    ImGui::SetCursorPos(ImVec2(windowWidth * 0.1, windowHeight*0.15 + 10 + entity_manager->getGuiTexture("SelectMap")->height * 1.1f + entity_manager->getGuiTexture("Volcan_pre")->height * 0.5f + entity_manager->getGuiTexture("EnemiesLvl")->height));
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Easy")->texture, ImVec2(entity_manager->getGuiTexture("PlayText")->width * 0.5f, entity_manager->getGuiTexture("PlayText")->height * 0.5f))){
        IAWasSelected = true;
        pressMedium = false;
        pressHard = false;
        pressEasy = true;
        entity_manager->selectedIA = "Easy";
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }
    if(pressMedium && IAWasSelected){
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 150, 90)));
    }else{
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
    }
    ImGui::SameLine();
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Medium")->texture, ImVec2(entity_manager->getGuiTexture("PlayText")->width * 0.5f, entity_manager->getGuiTexture("PlayText")->height * 0.5f))){
        IAWasSelected = true;
        pressMedium = true;
        pressHard = false;
        pressEasy = false;
        entity_manager->selectedIA = "Medium";
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }
    if(pressHard && IAWasSelected){
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 150, 90)));
    }else{
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
    }
    ImGui::SameLine();
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Hard")->texture, ImVec2(entity_manager->getGuiTexture("PlayText")->width * 0.5f, entity_manager->getGuiTexture("PlayText")->height * 0.5f))){
        IAWasSelected = true;
        pressMedium = false;
        pressHard = true;
        pressEasy = false;
        entity_manager->selectedIA = "Hard";
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));

    if(mapWasSelected && IAWasSelected){
        ImGui::SetCursorPos(ImVec2(windowWidth - entity_manager->getGuiTexture("PlayText")->width, windowHeight - entity_manager->getGuiTexture("PlayText")->height));
        ImGui::SetCursorPos(ImVec2(windowWidth*0.6f - entity_manager->getGuiTexture("PlayText")->width/2, 30 + entity_manager->getGuiTexture("SelectMap")->height * 1.1f + entity_manager->getGuiTexture("Volcan_pre")->height * 0.5f + entity_manager->getGuiTexture("EnemiesLvl")->height));
        if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("PlayText")->texture, ImVec2(entity_manager->getGuiTexture("PlayText")->width, entity_manager->getGuiTexture("PlayText")->height)))
        {
            devol =  4;
            entity_manager->selectEnemyCharacters();
            entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
            entity_manager->getCharacter("HardSFX")->audio.suena = true;
        }
    }
    ImGui::SetCursorPos(ImVec2(0, windowHeight - entity_manager->getGuiTexture("Back")->height));
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Back")->texture, ImVec2(entity_manager->getGuiTexture("Back")->width, entity_manager->getGuiTexture("Back")->height)))
    {
        devol =  1;
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 3;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }

    ImGui::PopStyleColor(4);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    graphicsEngine->swapBuffers();

    return devol;
}

int Facade2::renderCredits(){
    int devol = 6;

    graphicsEngine->clear();

    graphicsEngine->pollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize(ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0);
    ImGui::Begin("Credits", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    int windowWidth = graphicsEngine->device->getWindowWidth();
    int windowHeight = graphicsEngine->device->getWindowHeight();
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("Creditos")->texture, ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));

    int pops = 0;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
    pops++;

    ImGui::SetCursorPos(ImVec2(0, windowHeight - entity_manager->getGuiTexture("Back")->height*0.8));
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Back")->texture, ImVec2(entity_manager->getGuiTexture("Back")->width * 0.8f, entity_manager->getGuiTexture("Back")->height * 0.8f))){
        devol =  0;
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 3;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }
    ImGui::PopStyleColor(pops);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    graphicsEngine->swapBuffers();

    return devol;
}

int Facade2::renderControls(){
    int devol = 7;

    graphicsEngine->clear();

    graphicsEngine->pollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize(ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0);
    ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    int windowWidth = graphicsEngine->device->getWindowWidth();
    int windowHeight = graphicsEngine->device->getWindowHeight();
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("Controles")->texture, ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));

    int pops = 0;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
    pops++;

    ImGui::SetCursorPos(ImVec2(0, windowHeight - entity_manager->getGuiTexture("Back")->height*0.8));
    if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Back")->texture, ImVec2(entity_manager->getGuiTexture("Back")->width * 0.8f, entity_manager->getGuiTexture("Back")->height * 0.8f))){
        devol =  0;
        entity_manager->getCharacter("HardSFX")->audio.paramValue = 3;
        entity_manager->getCharacter("HardSFX")->audio.suena = true;
    }

    ImGui::PopStyleColor(pops);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    graphicsEngine->swapBuffers();

    return devol;
}

int Facade2::renderPauseMenu(){

    graphicsEngine->pollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    int devol = 0;

    // Set window full screen mode and full transparency
    ImGui::SetNextWindowSize(ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0);

    int pops = 0;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
    pops++;

    ImGui::Begin("Pause", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    {
        // Get the screen size
        int windowWidth = graphicsEngine->device->getWindowWidth();
        int windowHeight = graphicsEngine->device->getWindowHeight();

        float windowRelWidth = (float)windowWidth / 1920.0f;
        float windowRelHeight = (float)windowHeight / 1080.0f;

        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("PauseScreen")->texture, ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
        ImGui::SetCursorPos(ImVec2((windowWidth/2 - entity_manager->getGuiTexture("Resume")->width/2 * windowRelWidth)*1.57, windowHeight/2 * 1.1 - entity_manager->getGuiTexture("Quit")->height * windowRelHeight));
        if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Resume")->texture, ImVec2(entity_manager->getGuiTexture("Resume")->width * windowRelWidth, entity_manager->getGuiTexture("Resume")->height * windowRelHeight))){
            devol = 1;
            entity_manager->getCharacter("HardSFX")->audio.paramValue = 4;
            entity_manager->getCharacter("HardSFX")->audio.suena = true;
        }
        ImGui::SetCursorPos(ImVec2((windowWidth/2 - entity_manager->getGuiTexture("Quit")->width/2 * windowRelWidth) * 1.55, windowHeight/2 * 1.1));
        if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Quit")->texture, ImVec2(entity_manager->getGuiTexture("Quit")->width * windowRelWidth, entity_manager->getGuiTexture("Quit")->height * windowRelHeight))){
            devol = 2;
            entity_manager->getCharacter("HardSFX")->audio.paramValue = 3;
            entity_manager->getCharacter("HardSFX")->audio.suena = true;
        }
        ImGui::SetCursorPos(ImVec2((windowWidth/2 - entity_manager->getGuiTexture("Exit")->width/2 * windowRelWidth)*1.6, windowHeight/2 * 1.1 + entity_manager->getGuiTexture("Quit")->height * windowRelHeight));
        if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Exit")->texture, ImVec2(entity_manager->getGuiTexture("Exit")->width * windowRelWidth, entity_manager->getGuiTexture("Exit")->height * windowRelHeight))){
            devol = 3;
            entity_manager->getCharacter("HardSFX")->audio.paramValue = 3;
            entity_manager->getCharacter("HardSFX")->audio.suena = true;
        }
    }

    ImGui::PopStyleColor(pops);

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    graphicsEngine->swapBuffers();

    return devol;
}

int Facade2::renderEndingScreen(){

    //graphicsEngine->pollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    int devol = 0;

    // Set window full screen mode and full transparency
    ImGui::SetNextWindowSize(ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0);

    ImGui::Begin("Ending", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    {
        // Get the screen size
        int windowWidth = graphicsEngine->device->getWindowWidth();
        int windowHeight = graphicsEngine->device->getWindowHeight();

        float windowRelWidth = (float)windowWidth / 1920.0f;
        float windowRelHeight = (float)windowHeight / 1080.0f;
        if(entity_manager->getScore()->match.bluePoints == 3){
            ImGui::SetCursorPos(ImVec2((windowWidth/2)- (entity_manager->getGuiTexture("Victory")->width * 0.75)/2, 0));
            ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("Victory")->texture, ImVec2(entity_manager->getGuiTexture("Victory")->width * 0.75, entity_manager->getGuiTexture("Victory")->height * 0.75));
        }
        else if(entity_manager->getScore()->match.redPoints == 3){
            ImGui::SetCursorPos(ImVec2((windowWidth/2)- (entity_manager->getGuiTexture("Defeat")->width * 0.75)/2, 0));
            ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("Defeat")->texture, ImVec2(entity_manager->getGuiTexture("Defeat")->width * 0.75, entity_manager->getGuiTexture("Defeat")->height * 0.75));
        }
        ImGui::SetCursorPos(ImVec2(windowWidth/2 - entity_manager->getGuiTexture("Quit")->width/2, windowHeight/2 - entity_manager->getGuiTexture("Quit")->height/2));
        if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Quit")->texture, ImVec2(entity_manager->getGuiTexture("Quit")->width, entity_manager->getGuiTexture("Quit")->height))){
            devol = 1;
            entity_manager->getCharacter("HardSFX")->audio.paramValue = 3;
            entity_manager->getCharacter("HardSFX")->audio.suena = true;
        }
        ImGui::SetCursorPos(ImVec2(windowWidth/2 - entity_manager->getGuiTexture("Exit")->width/2, windowHeight/2 + entity_manager->getGuiTexture("Quit")->height /*- entity_manager->getGuiTexture("Exit")->height/2*/));
        if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Exit")->texture, ImVec2(entity_manager->getGuiTexture("Exit")->width, entity_manager->getGuiTexture("Exit")->height))){
            devol = 2;
            entity_manager->getCharacter("HardSFX")->audio.paramValue = 3;
            entity_manager->getCharacter("HardSFX")->audio.suena = true;
        }
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return devol;
}

void Facade2::renderLoadingScreen(int num){

    graphicsEngine->pollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize(ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0);
    ImGui::Begin("Loading", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    int windowWidth = graphicsEngine->device->getWindowWidth();
    int windowHeight = graphicsEngine->device->getWindowHeight();
    ImGui::SetCursorPos(ImVec2(0, 0));

    if(num==2)
        ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("LoadingScreenSingle")->texture, ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    else if(num==1)
        ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("LoadingScreenMultiplayer")->texture, ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    else if(num==3)
        ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("ErrorScreen")->texture, ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    graphicsEngine->swapBuffers();
}

int Facade2::renderHUD()
{
    int devol = 0;
    if(entity_manager->getScore()->match.finish==true)
        graphicsEngine->pollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Set window full screen mode and full transparency
    ImGui::SetNextWindowSize(ImVec2(graphicsEngine->device->getWindowWidth(), graphicsEngine->device->getWindowHeight()));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0);

    ImGui::Begin("HUD", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    {
        // Get the screen size
        int windowWidth = graphicsEngine->device->getWindowWidth();
        int windowHeight = graphicsEngine->device->getWindowHeight();

        float windowRelWidth = (float)windowWidth / 1920.0f;
        float windowRelHeight = (float)windowHeight / 1080.0f;

        // Display the score hud
        if(entity_manager->getScore()->match.finish==true){
            if(entity_manager->getScore()->match.bluePoints == 3){
                ImGui::SetCursorPos(ImVec2((windowWidth/2)- (entity_manager->getGuiTexture("Victory")->width * 0.75)/2, 0));
                ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("Victory")->texture, ImVec2(entity_manager->getGuiTexture("Victory")->width * 0.75, entity_manager->getGuiTexture("Victory")->height * 0.75));
            }
            else if(entity_manager->getScore()->match.redPoints == 3){
                ImGui::SetCursorPos(ImVec2((windowWidth/2)- (entity_manager->getGuiTexture("Defeat")->width * 0.75)/2, 0));
                ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture("Defeat")->texture, ImVec2(entity_manager->getGuiTexture("Defeat")->width * 0.75, entity_manager->getGuiTexture("Defeat")->height * 0.75));
            }
            ImGui::SetCursorPos(ImVec2(windowWidth/2 - entity_manager->getGuiTexture("Quit")->width/2 * windowRelWidth, windowHeight/2 - entity_manager->getGuiTexture("Quit")->height/2 * windowRelHeight));
            if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Quit")->texture, ImVec2(entity_manager->getGuiTexture("Quit")->width * windowRelWidth, entity_manager->getGuiTexture("Quit")->height * windowRelHeight))){
                devol = 1;
                entity_manager->getCharacter("HardSFX")->audio.paramValue = 3;
                entity_manager->getCharacter("HardSFX")->audio.suena = true;
            }
            ImGui::SetCursorPos(ImVec2(windowWidth/2 - entity_manager->getGuiTexture("Exit")->width/2 * windowRelWidth, windowHeight/2 + entity_manager->getGuiTexture("Quit")->height * windowRelHeight /*- entity_manager->getGuiTexture("Exit")->height/2*/));
            if(ImGui::ImageButton((void*)(intptr_t)entity_manager->getGuiTexture("Exit")->texture, ImVec2(entity_manager->getGuiTexture("Exit")->width * windowRelWidth, entity_manager->getGuiTexture("Exit")->height * windowRelHeight))){
                devol = 2;
                entity_manager->getCharacter("HardSFX")->audio.paramValue = 3;
                entity_manager->getCharacter("HardSFX")->audio.suena = true;
            }
        }

        IGuiTexture* vsImg = entity_manager->getGuiTexture("marcador").get();
        float vsImgWidth = vsImg->width * windowRelWidth;
        float vsImgHeight = vsImg->height * windowRelHeight;

        ImGui::SetCursorPos(ImVec2(windowWidth / 2 - vsImgWidth/2 * 2.5, windowHeight - vsImgHeight * 2.5));
        ImGui::Image((void*)(intptr_t)vsImg->texture, ImVec2(vsImgWidth * 2.5, vsImgHeight * 2.5));

        int bluePoints = entity_manager->getScore()->match.bluePoints;
        int redPoints = entity_manager->getScore()->match.redPoints;
        int round = entity_manager->getScore()->match.ronda;

        char* bluePointsImage = "";
        // Switch blue points image
        switch(bluePoints)
        {
            case 1:
                bluePointsImage = "score_number_1";
            break;

            case 2:
                bluePointsImage = "score_number_2";
            break;

            case 3:
                bluePointsImage = "score_number_3";
            break;

            case 0:
                bluePointsImage = "score_number_0";
            break;
        }

        char* redPointsImage = "";
        // Switch red points image
        switch(redPoints)
        {
            case 1:
                redPointsImage = "score_number_1";
            break;

            case 2:
                redPointsImage = "score_number_2";
            break;

            case 3:
                redPointsImage = "score_number_3";
            break;

            case 0:
                redPointsImage = "score_number_0";
            break;
        }
        char* roundImage = "";
        // Switch round image
        switch(round)
        {
            case 1:
                roundImage = "round1";
            break;

            case 2:
                roundImage = "round2";
            break;

            case 3:
                roundImage = "round3";
            break;

            case 4:
                roundImage = "round4";
            break;

            case 5:
                roundImage = "round5";
            break;
        }

        ImGui::SetCursorPos(ImVec2(windowWidth/2 - entity_manager->getGuiTexture(roundImage)->width/2 * 0.9 * windowRelWidth, windowHeight - entity_manager->getGuiTexture(roundImage)->height * 0.9 * windowRelHeight));
        ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture(roundImage)->texture, ImVec2(entity_manager->getGuiTexture(roundImage)->width * 0.9 * windowRelWidth, entity_manager->getGuiTexture(roundImage)->height * 0.8 * windowRelHeight));

        ImGui::SetCursorPos(ImVec2(windowWidth/2 - entity_manager->getGuiTexture(bluePointsImage)->width/2 - entity_manager->getGuiTexture("marcador")->width *  0.9 * windowRelWidth, windowHeight - entity_manager->getGuiTexture(bluePointsImage)->height * 2.45 * windowRelHeight));
        ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture(bluePointsImage)->texture, ImVec2(entity_manager->getGuiTexture(bluePointsImage)->width * 1.25 * windowRelWidth, entity_manager->getGuiTexture(bluePointsImage)->height * 1.25 * windowRelHeight));

        ImGui::SetCursorPos(ImVec2(windowWidth/2 - entity_manager->getGuiTexture(redPointsImage)->width/2 + entity_manager->getGuiTexture("marcador")->width * 0.9 * windowRelWidth, windowHeight - entity_manager->getGuiTexture(redPointsImage)->height * 2.45 * windowRelHeight));
        ImGui::Image((void*)(intptr_t)entity_manager->getGuiTexture(redPointsImage)->texture, ImVec2(entity_manager->getGuiTexture(redPointsImage)->width * 1.25 * windowRelWidth, entity_manager->getGuiTexture(redPointsImage)->height * 1.25 * windowRelHeight));

        // Recorrer mapa de entidades y dibujar sus imagenes
        auto &entitiesMap = entity_manager->getMapEntities();
        std::map < const char*, unique_ptr<IEntity> >::iterator itr;

        //cambiar a balanceTeams
        int teamMembers = entity_manager->balanceTeams();
        float xRelPos = 0.05f;
        float yRelPos = 0.15f;

        for(int i = 0; i < teamMembers*2; i++)// for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
        {
            if(i == teamMembers)
            {
                xRelPos = 0.95f;
                yRelPos = 0.15f;
            }

            ICharacter* character = nullptr;
            if(i < teamMembers)
                character = entity_manager->selectedCharacters[i];
            else
                character = entity_manager->selectedEnemyCharacters[i-teamMembers];

            if(!character) {cout << "Character is equal to nullptr (Render HUD)" << endl; return -1;}

            ImVec4 tint = ImVec4(1.0f, 0, 0, 0.5f);
            float staminaValue = 0;
            int superThrowValue = 0;
            bool poweredValue = false;
            int tipoValue = 0;
            if(entity_manager->entityExistsByCharacter(character->id))
            {
                tint = ImVec4(1, 1, 1, 1);
                staminaValue = (float)entity_manager->getEntityByCharacter(character->id)->stamina.stamina / (float)entity_manager->getEntityByCharacter(character->id)->stamina.staminaMax;
                superThrowValue = entity_manager->getEntityByCharacter(character->id)->t.superShot;
                poweredValue = entity_manager->getEntityByCharacter(character->id)->control.powered;
                tipoValue = entity_manager->getEntityByCharacter(character->id)->control.powerType;
            }

            IGuiTexture* image = entity_manager->getGuiTexture(character->id).get();

            int imageWidth = image->width * windowRelWidth;
            int imageHeight = image->height * windowRelHeight;

            int xPos = xRelPos * windowWidth - imageWidth/2;
            int yPos = yRelPos * windowHeight - imageHeight/2;

            // Display Hud character images
            ImGui::SetCursorPos(ImVec2(xPos, yPos));
            ImGui::Image((void*)(intptr_t)image->texture, ImVec2(imageWidth, imageHeight), ImVec2(0, 0), ImVec2(1, 1), tint);

            // Display Hud character stamina bars
            ImGui::SetCursorPos(ImVec2(xPos, yPos + imageHeight/2 + 70));
            ImGui::ProgressBar(staminaValue, ImVec2(imageWidth, imageWidth/6));

            // Display Hud character super throw bars
            char* superThrowImage = "";
            switch (superThrowValue)
            {
                case 0:
                    if(i < 3)
                        superThrowImage = "poder_local_0";
                    else
                        superThrowImage = "poder_visitante_0";
                break;

                case 1:
                    if(i < 3)
                        superThrowImage = "poder_local_1";
                    else
                        superThrowImage = "poder_visitante_1";
                break;

                case 2:
                    if(i < 3)
                        superThrowImage = "poder_local_2";
                    else
                        superThrowImage = "poder_visitante_2";
                break;

                case 3:
                    if(i < 3)
                        superThrowImage = "poder_local_3";
                    else
                        superThrowImage = "poder_visitante_3";
                break;
            }
            if(poweredValue == true){
                char* powerUpImage = "";
                switch (tipoValue)
                {
                case 0:
                    powerUpImage = "powerUpFuerza";
                break;

                case 1:
                    powerUpImage = "powerUpVelocidad";
                break;

                case 2:
                    powerUpImage = "powerUpResistencia";
                break;

                case 3:
                    powerUpImage = "powerUpSuperTiro";
                break;
                }

                IGuiTexture* powerUpImg = entity_manager->getGuiTexture(powerUpImage).get();
                if(i<3)
                    ImGui::SetCursorPos(ImVec2(xPos + 85 * windowRelWidth, yPos));
                else
                    ImGui::SetCursorPos(ImVec2(xPos - 5 * windowRelWidth, yPos));
                ImGui::Image((void*)(intptr_t)powerUpImg->texture, ImVec2(imageWidth * 0.25, imageHeight * 0.25));

            }

            IGuiTexture* superThrowImg = entity_manager->getGuiTexture(superThrowImage).get();

            ImGui::SetCursorPos(ImVec2(xPos, yPos + imageHeight/2 + 100));
            ImGui::Image((void*)(intptr_t)superThrowImg->texture, ImVec2(imageWidth, imageHeight/5), ImVec2(0, 0), ImVec2(1, 1), tint);

            yRelPos = yRelPos + 0.3f;
        }
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if(entity_manager->getScore()->match.finish==true)
        graphicsEngine->swapBuffers();

    return devol;
}

void Facade2::resetGame()
{
    // Delete all characters
    entity_manager->getScore()->match.ronda = 1;

    deleteAllCharacters();

    // Reset selected characters and enmies arrays
    for(int i = 0; i < 3; i++)
    {
        entity_manager->selectedCharacters[i] = nullptr;
        entity_manager->selectedEnemyCharacters[i] = nullptr;
    }

    // Reset selected map and IA
    mapWasSelected = false;
    IAWasSelected = false;
    entity_manager->teamMembers = 0;
}

// Collision methods
void Facade2::initColliders()
{
    auto &entitiesMap = entity_manager->getMapEntities();
    std::map < const char*, unique_ptr<IEntity> >::iterator itr;

    for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++)
    {
        if(itr->second->control.esPlayer || itr->second->control.esJugador || itr->second->control.esPowerUp){ // Initialise characters and powerups colliders
          if(itr->second->control.esPowerUp){
            itr->second->node.myNode->addCubeCollider((char *)itr->first, itr->second->node.myNode, 0.14, 1, 0.14, 0, 0, 0);
          }
          else{
            itr->second->node.myNode->addCubeCollider((char *)itr->first, itr->second->node.myNode, 0.14, 1, 0.14, 0, 0, 0);
          }
        }
        else if(itr->second->control.esPelota) // Initialise ball collider
            itr->second->node.myNode->addCubeCollider((char *)itr->first, itr->second->node.myNode, 0.07, 0.07, 0.07, 0, 0, 0);
        else if(strcmp(itr->first, "map_02") == 0) // Initialise map colliders
        {
          itr->second->node.myNode->addCubeCollider("LateralWall", itr->second->node.myNode, 0.14, 4, 7, 2.7, 0, 0);
          itr->second->node.myNode->addCubeCollider("LateralWall", itr->second->node.myNode, 0.14, 4, 7, -2.7, 0, 0);
          itr->second->node.myNode->addCubeCollider("CentralWall", itr->second->node.myNode, 7, 4, 0.14, 0, 0, 3.5);
          itr->second->node.myNode->addCubeCollider("CentralWall", itr->second->node.myNode, 7, 4, 0.14, 0, 0, -3.5);
        }
    }
}

void Facade2::updateEntitiesCollisions()
{
    auto &entitiesMap = entity_manager->getMapEntities();
    std::map < const char*, unique_ptr<IEntity> >::iterator itr;

    for(itr = entitiesMap.begin(); itr != entitiesMap.end(); itr++){

        std::map <const char*, unique_ptr<IEntity> >::iterator itr2;

        for(itr2 = entitiesMap.begin(); itr2 != entitiesMap.end(); itr2++)
        {
            itr->second->node.myNode->checkCollisions(itr2->second->node.myNode);
        }
    }
}

void Facade2::addCubeColliderToEntity(char* name, MyNode* parent, float xSize, float ySize, float zSize, float xOffset, float yOffset, float zOffset){
  parent->addCubeCollider(name, parent, xSize, ySize, zSize, xOffset, yOffset, zOffset);
}

bool Facade2::compareCollision(IEntity* entOne, IEntity* entTwo){

    int entOneCollisions = entOne->node.myNode->getCollisions()->size();
    int entTwoColliders = entTwo->node.myNode->getColliders()->size();

    if(entOneCollisions == 0|| entTwoColliders == 0) {return false;}

    for(int i = 0; i < entOneCollisions; i++){
      for(int j = 0; j < entTwoColliders; j++){
        if(entOne->node.myNode->getCollisions()->at(i) == entTwo->node.myNode->getColliders()->at(j)){
          return true;
        }
      }
    }

    return false;
}

bool Facade2::isEntityColliding(IEntity* entity)
{
    int nbCollisions = entity->node.myNode->getCollisions()->size();

    if(nbCollisions == 0) {return false;}

    return true;
}

char* Facade2::getCollisionName(IEntity* entity)
{
    if(!isEntityColliding(entity)) {return "There is no collision name.";}

    return entity->node.myNode->getCollisions()->at(0)->getName();
}

 // Other methods

//Metodo que devuelve true cuando la pelota tiene esta asociada a un nodo (parent)
bool Facade2::compareParent(std::map< const char*, unique_ptr<IEntity> >& entities, IEntity* ent){
    //Se inicializa una variable para parar el bucle, un mapa de entidades y un iterador para recorrerlo
  bool stop = false;
  std::map < const char*, unique_ptr<IEntity> >::iterator itr;
  for(itr = entities.begin(); itr != entities.end() && stop == false; itr++){
    //Se compruba que la entidad sea la pelota
    if(itr->second->control.esPelota){
      auto& ball = itr->second;
      stop = true;
      //Se comprueba si la pelota tiene esta asociada a un nodo (parent)
      if(ent->node.myNode == ball->node.myNode->getParent()){
        return true;
      }
    }
  }
  return false;
}

bool Facade2::compareParent(IEntity* ball, IEntity* ent)
{
    if(ent->node.myNode == ball->node.myNode->getParent())
    {
        return true;
    }

    return false;
}

bool Facade2::checkStart1(){
    if(!strcmp(checkInput(), "1"))
        return true;
    return false;
}
bool Facade2::checkStart2(){
    if(!strcmp(checkInput(), "2"))
        return true;
    return false;
}

int Facade2::checkMando(){
    return graphicsEngine->joystickPresent(GLFW_JOYSTICK_1);
}

const float Facade2::triggers(){
    int count;
    const float* axes = graphicsEngine->getJoystickAxes(GLFW_JOYSTICK_1, &count);

    if(axes[2]>0.1){
        //Left trigger
        inputs = "R";
        return 2;
    }
    if(axes[5]>0.1){
        //Right trigger
        inputs = "SP";
        return 5;
    }
    return 0;
}

const float Facade2::getYAxisRJ(){
    int count;
    double devol;
    const float* axes = graphicsEngine->getJoystickAxes(GLFW_JOYSTICK_1, &count);
    if(axes[4] < -0.75)
        devol = -0.75;
    else if(axes[4] > 0.75)
        devol = 0.75;
    else if(axes[4] < 0.08 && axes[4] > -0.08)
        devol = 0;
    else
        devol = axes[4];
    return devol;
}

const float Facade2::getXAxisRJ(){
    int count;
    double devol;
    const float* axes = graphicsEngine->getJoystickAxes(GLFW_JOYSTICK_1, &count);
    if(axes[3] < -0.75)
        devol = -0.75;
    else if (axes[3] > 0.75)
        devol = 0.75;
    else if(axes[3] < 0.08 && axes[3] > -0.08)
        devol = 0;
    else
        devol = axes[3];
    return devol;
}

const unsigned char Facade2::buttons(){
    int buttonsCount;
    const unsigned char *buttons = graphicsEngine->getJoystickButtons(GLFW_JOYSTICK_1, &buttonsCount);
    if(GLFW_PRESS == buttons[0]){
        //  X/A
        inputs = "E";
        return 0;
    }
    if(GLFW_PRESS == buttons[1]){
        //  Circulo/B
        return 1;
    }
    if(GLFW_PRESS == buttons[2]){
        //  Cuadrado/X
        return 2;
    }
    if(GLFW_PRESS == buttons[3]){
        //  Triangulo/Y
        return 3;
    }
    if(GLFW_PRESS == buttons[4]){
        //  L1/LB
        inputs = "C";
        return 4;
    }
    if(GLFW_PRESS == buttons[5]){
        //  R1/RB
        inputs = "Y";
        return 5;
    }
    if(GLFW_PRESS == buttons[6]){
        //  Select
        return 6;
    }
    if(GLFW_PRESS == buttons[7]){
        //  Start
        inputs = "P";
        return 7;
    }
    return 100;
}

bool Facade2::joystickMoving(){

    int count;
    const float* axes = graphicsEngine->getJoystickAxes(GLFW_JOYSTICK_1, &count);
    if(checkMando()==1){

        if(abs(axes[0])>=0.1 || abs(axes[1])>=0.1){
            return true;
        }
    }
    return false;
}

const float Facade2::getCursorX(){
    return camera->cursorXNorm;
}

const float Facade2::getCursorY(){
    return camera->cursorYNorm * -1.0f;
}

bool Facade2::keyboardMoving(){
    if(strcmp(checkInput(), "A") && strcmp(checkInput(), "S") && strcmp(checkInput(), "D") && strcmp(checkInput(), "W")){
        return false;
    }
    return true;
}

bool Facade2::checkShutDown()
{
    return graphicsEngine->windowShouldClose();
}

//Metodo que asocia al balon a otro nodo
void Facade2::createChild(IEntity* ent, IEntity* ball)
{
  //Se elimina en animator del balon y se establece como hijo de otro nodo, cambiando su posicion
  ent->node.myNode->addChild(ball->node.myNode);
  ball->node.myNode->setPosition(vec3(0.12, 0, 0));
  ball->position.X = ball->node.myNode->getPosition().x;
  ball->position.Y = ball->node.myNode->getPosition().y;
  ball->position.Z = ball->node.myNode->getPosition().z;
}

//Metodo que elimina el child de otro nodo y crea un nodo nuevo para el balon
void Facade2::removeChild(IEntity* ent, IEntity* ball)
{
  float pEntX = ent->node.myNode->getPosition().x;
  float pEntY = ent->node.myNode->getPosition().y;
  float pEntZ = ent->node.myNode->getPosition().z;

  ent->node.myNode->removeChild(ball->node.myNode);

  //AQUI ORIGINALMENTE LAS Z ERAN + Y -
  if(ent->team.team.compare("Red")){
    setPosition(ball, vec3(pEntX, pEntY, pEntZ - 0.14));
  }
  else{
    setPosition(ball, vec3(pEntX, pEntY, pEntZ + 0.19));
  }
}

//Metodo que cambia la posicion de un nodo
void Facade2::setPosition(IEntity* ent, vec3 pos)
{
    ent->position.X = pos.x;
    ent->position.Y = pos.y;
    ent->position.Z = pos.z;
}

//Metodo para renderizar entidades (usado para los powerups)
void Facade2::renderEnt(IEntity* ent)
{
    MyModel *entity = new MyModel(graphicsEngine->device->window, graphicsEngine->lightingShader, ent->meshPath);
    MyNode* child = ent->node.myNode = new MyNode(entity);

    if(ent->control.esPowerUp)
        entity->setTransparency(0.6f);

    rootNode->addChild(child);
}

//Metodo que crea un animator a un nodo
void Facade2::setAnimator(IEntity* ent){

}

//Metodo para quitar animators
void Facade2::removeAnimator(IEntity* ent){

}

void Facade2::setRotation(IEntity* ent, vec3 rot)
{
    ent->node.myNode->setRotation(rot);
}

void Facade2::setAnim(IEntity* ent, int state){
    MyModel* m = (MyModel*)ent->node.myNode->getMyEnt();
    if(state == 0 && m->animationState != state){
        if(strcmp(ent->meshPath, "meshes/BeepBot/BeepBot") == 0 && m->getFrame() == m->getAnimationSize(m->animationState) - 2){
            m->setState(state);
        }

        else if(strcmp(ent->meshPath, "meshes/KStar/KStar") == 0 && m->getFrame() == m->getAnimationSize(m->animationState) - 13){
            m->setState(state);
        }
        else if((strcmp(ent->meshPath, "meshes/KingMalaka/KingMalaka") == 0 || strcmp(ent->meshPath, "meshes/ElAfilador/ElAfilador") == 0) && m->getFrame() == m->getAnimationSize(m->animationState) - 9){
            m->setState(state);
        }
        else if(strcmp(ent->meshPath, "meshes/ElRisas/ElRisas") == 0 && m->getFrame() == m->getAnimationSize(m->animationState) - 6){
            m->setState(state);
        }
        else if(strcmp(ent->meshPath, "meshes/LilDumb/LilDumb") == 0 && m->getFrame() == m->getAnimationSize(m->animationState) - 8){
            m->setState(state);
        }
        else if(strcmp(ent->meshPath, "meshes/MrBuck/MrBuck") == 0 && m->getFrame() == m->getAnimationSize(m->animationState) - 10){
            m->setState(state);
        }
        else if(strcmp(ent->meshPath, "meshes/Cytrhia/Cytrhia") == 0 && m->getFrame() == m->getAnimationSize(m->animationState) - 14){
            m->setState(state);
        }
    }
    if(m->animationState != state && ((m->animationState == 1 && state == 0)|| state != 0)){
        m->setState(state);
    }
}

//------------------------MULTIPLAYER METHODS----------------------------------------------
std::string Facade2::getInputs()
{
    if (!strcmp(inputs.c_str(), "SP") || !strcmp(inputs.c_str(), "Y"))
    {
        if(checkMando()==0)
        {
            return inputs + "," + std::to_string(getCursorX()) + " " + std::to_string(getCursorY());
        }
        if(checkMando()==1)
        {
            return inputs + "," + std::to_string(getXAxisRJ()) + " " + std::to_string(getYAxisRJ());
        }
    }
    else
    {
        return inputs + ",";
    }
}

void Facade2::clearInputs()
{
    inputs = "";
}

void Facade2::setMultiplayerID(int id)
{
    multiplayerID = id;
}

int Facade2::getMultiplayerID()
{
    return multiplayerID;
}

std::string Facade2::getMultiplayerPowerup()
{
    return multiplayerPowerup;
}

void Facade2::setMultiplayerMuerto(std::string name)
{
    multiplayerMuerto = name;
}

std::string Facade2::getMultiplayerMuerto()
{
    std::string m = multiplayerMuerto;
        multiplayerMuerto = "";
    return m;
}

void Facade2::shutDown()
{
    // Cleanup IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    resourceManager->clearResources();

    // Closing the window
    graphicsEngine->terminate();
}
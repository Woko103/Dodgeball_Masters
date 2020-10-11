#include <game_manager.h>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main()
{
        /* Creamos un game manager desde el que controlaremos las partes generales del juego
        como la inicialización del juego, el renderizado etc...*/
        unique_ptr<Game_Manager> gm = make_unique<Game_Manager>();
        
        // Inicializamos el juego
        while(!gm->getBiggerFinish()){
        	gm->startMenus();
        	if(!gm->getFinish())
        		gm->startGame();
    	}
    	gm->turnOff();
}

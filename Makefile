######################
###MACROS
#######################
#$(1): Compiler
#$(2): Object file to generate
#$(3): Source file
#$(4): Additional Dependencies
#$(5): Compiler Flags
define COMPILE
$(2) : $(3)
	$(1) -c -o $(2) $(3) $(4) $(5)
endef

#$(1): Source file
#src/entity_manager.cpp
#obj/entity_manager.o
define CPP2OBJ
$(patsubst %.cpp, %.o, $(patsubst $(SRC)%, $(OBJ)%, $(1)))
endef

#######################
###CONFIG
#######################
APP	:= "DodgeballMasters"
DELIVERY_FOLDER	:= DodgeballMasters_D
CCFLAGS	:= -Wall -pedantic -std=c++17
CFLAGS	:= $(CCFLAGS)
CC	:= g++
C	:= gcc
MKDIR	:= mkdir -p
SRC	:= src
OBJ	:= obj

# Libraries
FMOD_LIBS	:= -Wl,-rpath,libs:fmod/lib -Lfmod/lib -lfmod -lfmodstudio
GLFW_LIBS := -Wl,-rpath,libs:opengl/glfw/lib -Lopengl/glfw/lib -lGL -lGLU -lglut -lglfw
ASSIMP_LIBS := -Wl,-rpath,libs:opengl/assimp/lib -Lopengl/assimp/lib -lassimp
GLAD_LIBS := -Wl,-rpath,libs:opengl/glad/lib -Lopengl/glad/lib -ldl
GLEW_LIBS := -Wl,-rpath,libs:opengl/glew/lib -Lopengl/glew/lib -lGLEW
BOOST_LIBS := -Wl,-rpath,libs:boost/lib -Lboost/lib -lboost_filesystem -lboost_system -lpthread

ALL_LIBS := $(FMOD_LIBS) $(GLFW_LIBS) $(GLAD_LIBS) $(ASSIMP_LIBS) $(BOOST_LIBS) $(GLEW_LIBS)

# Headers
OPENGL_HEADERS := -Iopengl -Iopengl/glfw/include -Iopengl/glm -Iopengl/glad/include -Iopengl/glew/include
FMOD_HEADERS := -Ifmod/inc
PROJECT_HEADERS := -Iinclude/cmp -Iinclude/ent -Iinclude/sys -Iinclude/sys/FuzzyLogic -Iinclude/man -Iinclude/physics -Iinclude/opengl -Iinclude/arbol -Iinclude/imgui
BOOST_HEADERS := -Iboost

ALL_HEADERS	:= $(PROJECT_HEADERS) $(FMOD_HEADERS) $(OPENGL_HEADERS) $(BOOST_HEADERS)

ALLCPPS	:= $(shell find $(SRC) -type f -iname *.cpp)
ALLCS	:= $(shell find $(SRC) -type f -iname *.c)
ALLOBJ	:= $(foreach F, $(ALLCPPS) $(ALLCS), $(call CPP2OBJ, $(F)))
# Encuentrame todos los directorios que tengan "src"
SUBDIRS	:= $(shell find $(SRC) -type d)
# Estando en SUBDIRS, me cambias todo lo que tenga src "lo que sea" por obj "lo que sea"
OBJSUBDIRS	:= $(patsubst $(SRC)%, $(OBJ)%, $(SUBDIRS))
# Con esto le decimos de que esto no genera ningún fichero al ejecutar la macro "dir" o cualquier otra
.PHONY: options info clean delivery r_delivery

$(APP) : $(OBJSUBDIRS) $(ALLOBJ)
	$(CC) -o $(APP) $(ALLOBJ) $(ALL_LIBS) 
	@echo -e "\e[1;32m $(APP) HAS BEEN SUCCESSFULLY CREATED \e[0m";

# Generate rules for all objects
$(foreach F, $(ALLCPPS), $(eval $(call COMPILE,$(CC),$(call CPP2OBJ,$(F)),$(F),$(ALL_HEADERS),$(CCFLAGS))))

info:
	$(info $(SUBDIRS))
	$(info $(OBJSUBDIRS))
	$(info $(ALLCPPS))
	$(info $(ALLCS))
	$(info $(ALLCSOBJ))
	$(info $(ALLCPPSOBJ))

$(OBJSUBDIRS) :
	$(MKDIR) $(OBJSUBDIRS)

options:
	@echo -e "\e[1;33m 1. make clean:\e[1;32m Cleans the obj and delivery folder \e[0m";
	@echo -e "\e[1;33m 2. make delivery:\e[1;32m Creates a delivery folder with the executable, meshes and all the necessary libs. \e[0m";
clean:
	@if [ -d $(OBJ) ]; then \
		echo -e "\e[1;32m OBJ folder has been removed successfully \e[0m"; \
		rm -r $(OBJ); \
	else \
		echo -e "\e[1;33m OBJ folder is not created \e[0m"; \
	fi

	@if [ -d $(DELIVERY_FOLDER) ]; then \
		echo -e "\e[1;32m Delivery folder has been removed successfully \e[0m"; \
		rm -r $(DELIVERY_FOLDER); \
	else \
		echo -e "\e[1;33m Delivery folder is not created \e[0m"; \
	fi
	
	@if [ -f $(APP) ]; then \
		rm $(APP); \
		echo -e "\e[1;32m $(APP) has been removed successfully \e[0m"; \
	fi

	@if [ -d .vscode ]; then \
		rm -r .vscode; \
	fi

	@if [ -f imgui.ini ]; then \
		rm imgui.ini; \
	fi 
	
delivery:
	@if [  !   -d $(DELIVERY_FOLDER) ]; then\
		mkdir $(DELIVERY_FOLDER) $(DELIVERY_FOLDER)/libs; \
		echo -e "\e[1;32m Delivery folder "$(DELIVERY_FOLDER)" has been created successfully \e[0m"; \
	fi

	@cp $(APP) $(DELIVERY_FOLDER)
	@cp -r opengl/shaders $(DELIVERY_FOLDER)/opengl
	@cp -r fmod/banks $(DELIVERY_FOLDER)/fmod
	@cp -r meshes $(DELIVERY_FOLDER)
	@cp -a opengl/assimp/lib/. $(DELIVERY_FOLDER)/libs
	@cp -a opengl/glad/lib/. $(DELIVERY_FOLDER)/libs
	@cp -a opengl/glfw/lib/. $(DELIVERY_FOLDER)/libs
	@cp -a fmod/lib/. $(DELIVERY_FOLDER)/libs
	@cp -a boost/lib/. $(DELIVERY_FOLDER)/libs
	@echo -e "\e[1;33m $(DELIVERY_FOLDER) has been updated \e[0m"

	
	
	
	
	

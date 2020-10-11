#ifndef AUDIO_H
#define AUDIO_H
#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <cstring>
#include <iostream>

struct Audio
{
  //evento asociado
  FMOD::Studio::EventDescription *event = nullptr;
  FMOD::Studio::EventInstance *eventInstance = nullptr;
  FMOD_STUDIO_PARAMETER_DESCRIPTION param;
  bool suena=false;
  char* cadEvent;
  //Correspondencia paramValue
  //0=Atrapa/Bocina
  //1=Derrota/Carga
  //2=Esquiva/Explosion
  //3=Golpe/Cancel
  //4=Lanza/Confirmation
  //5=Pick/Pasos
  //6=SuperDefensa/Rebote
  //7=SuperTiro/Silbato
  //8=Vacile/Lanza
  //9=Victoria/Lanza
  float paramValue = 5;
  bool hasParameter;
};

#endif

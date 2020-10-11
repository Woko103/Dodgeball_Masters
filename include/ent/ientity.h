#ifndef IENTITY_H
#define IENTITY_H

#include <position.h>
#include <node.h>
#include <team.h>
#include <throw.h>
#include <dash.h>
#include <audio.h>
#include <control.h>
#include <catch.h>
#include <estado.h>
#include <speed.h>
#include <stamina.h>
#include <difficulty.h>

struct IEntity
{
        Position position;
        Node node;
        const char* meshPath;
        const char* id;
        const char* characterId;
        Team team;
        Control control;
        Throw t;
        Dash dash;
        Audio audio;
        Catch c;
        Estado status;
        Speed speed;
        Stamina stamina;
        Difficulty difficulty;
        IEntity* target;
};

#endif

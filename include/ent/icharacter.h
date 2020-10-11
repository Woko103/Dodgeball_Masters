#ifndef ICHARACTER_H
#define ICHARACTER_H

#include <position.h>
#include <node.h>
#include <team.h>
#include <throw.h>
#include <dash.h>
#include <audio.h>
#include <control.h>
#include <catch.h>
#include <speed.h>
#include <stamina.h>

struct ICharacter
{
        Position position;
        Node node;
        const char* meshPath;
        const char* imagePath;
        const char* id;
        Team team;
        Control control;
        Throw t;
        Dash dash;
        Audio audio;
        Catch c;
        Speed speed;
        Stamina stamina;
        bool selected;
};

#endif

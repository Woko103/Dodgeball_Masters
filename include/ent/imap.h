#ifndef IMAP_H
#define IMAP_H

#include <position.h>
#include <node.h>
#include <audio.h>

struct IMap
{
        Position position;
        Node node;
        const char* meshPath;
        const char* imagePath;
        const char* id;
        Audio audio;
        bool selected;
};

#endif
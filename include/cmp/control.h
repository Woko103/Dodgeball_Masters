#pragma once

struct Control{
  //Player solo es el que es manejado, jugador son todos menos
  //el player, los dos a false es mapa, pelota, etc...
  bool esPlayer;
  //Player solo es el que es manejado, jugador son todos menos
  //el player, los dos a false es mapa, pelota, etc...
  bool esJugador;

  bool esMultiplayer;

  bool esPelota;

  bool esPowerUp;

  bool powered;

  int powerUses;

  int powerType;

  double powerTime;
};

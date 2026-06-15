#pragma once
#include <box2d.h>
#include "raylib.h"

class PhysicsObject;

class Mechanism {
public:
    int tipoEscena;
    Texture2D texturaBloque; // Variable para la imagen del bloque
    Texture2D texturaPuente;  //Puente

    PhysicsObject* platform;
    PhysicsObject* lever;

    Mechanism(b2World& world, b2Body* ground, int escena, Texture2D texLoco, Texture2D texPuente);
    ~Mechanism();
    void Draw();
};
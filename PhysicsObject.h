#pragma once
#include <box2d.h>
#include <raylib.h>

class PhysicsObject {
public:
    b2Body* body;
    float width;
    float height;
    Color color;

    // Solo declaración limpia para evitar el error de cuerpos duplicados
    PhysicsObject(b2World& world, float x, float y, float w, float h, b2BodyType type, Color col);
    void Draw();
};
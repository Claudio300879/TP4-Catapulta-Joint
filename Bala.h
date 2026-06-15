#pragma once
#include "raylib.h"
#include "box2d.h"

class Bala {
private:
    b2Body* body;
    float width;
    float height;
    Texture2D texturaBala; // Nueva variable para la imagen

public:
    Bala(b2World* world, b2Vec2 position, float angleInDegrees, Texture2D textura);
    ~Bala();

    void Draw();
    b2Body* GetBody() const { return body; }
};
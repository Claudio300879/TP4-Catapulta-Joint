#include "Bala.h"
#include <cmath>

Bala::Bala(b2World* world, b2Vec2 position, float angleInDegrees, Texture2D textura) {
    width = 30.0f;
    height = 30.0f;
    texturaBala = textura; // Asignamos la textura recibida

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.angle = -angleInDegrees * DEG2RAD;
    bodyDef.bullet = true;

    body = world->CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(width / 2.0f, height / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 3.5f;
    fixtureDef.friction = 0.6f;
    fixtureDef.restitution = 0.4f;
    body->CreateFixture(&fixtureDef);
}

Bala::~Bala() {
    if (body) body->GetWorld()->DestroyBody(body);
}

void Bala::Draw() {
    if (!body) return;

    b2Vec2 pos = body->GetPosition();
    float angle = body->GetAngle() * RAD2DEG;

    // Dibujamos la textura usando los mismos parámetros físicos
    DrawTexturePro(texturaBala,
        { 0, 0, (float)texturaBala.width, (float)texturaBala.height },
        { pos.x, pos.y, width, height },
        { width / 2.0f, height / 2.0f },
        -angle, WHITE);
}
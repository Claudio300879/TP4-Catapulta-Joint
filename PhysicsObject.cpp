#include "PhysicsObject.h"

PhysicsObject::PhysicsObject(b2World& world, float x, float y, float w, float h, b2BodyType type, Color col) {
    width = w;
    height = h;
    color = col;

    b2BodyDef bodyDef;
    bodyDef.type = type;
    bodyDef.position.Set(x, y);
    body = world.CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(w / 2.0f, h / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = (type == b2_dynamicBody) ? 1.0f : 0.0f;
    fixtureDef.friction = 0.5f;
    fixtureDef.restitution = 0.3f;

    fixtureDef.filter.categoryBits = 0x0001;
    fixtureDef.filter.maskBits = 0xFFFF;

    body->CreateFixture(&fixtureDef);
}

void PhysicsObject::Draw() {
    if (!body) return;

    b2Vec2 pos = body->GetPosition();
    float angle = body->GetAngle() * RAD2DEG;

    DrawRectanglePro(
        { pos.x, pos.y, width, height },
        { width / 2.0f, height / 2.0f },
        -angle,
        color
    );
}
#include "Mechanism.h"
#include "PhysicsObject.h"
#include "raylib.h"

Mechanism::Mechanism(b2World& world, b2Body* ground, int escena, Texture2D texLoco, Texture2D texPuente) {
    tipoEscena = escena;
    texturaBloque = texLoco;   //Textura del bloque
    texturaPuente = texPuente; // Textura del puente
    platform = nullptr;
    lever = nullptr;

    // =======================================================================
    // ESCENA 1: PUENTE RESORTE (Derecha) + BLOQUE PRISMATICO AJUSTADO (Suelo)
    // =======================================================================
    if (tipoEscena == 1) {
        // -------------------------------------------------------------------
        // PUENTE RESORTE (Intacto, sin modificar ninguna coordenada ni joint)
        // -------------------------------------------------------------------
        platform = new PhysicsObject(world, 680, 320, 250, 20, b2_dynamicBody, GRAY);

        b2DistanceJointDef djd;
        float freq = 1.8f;
        float damp = 0.15f;

        djd.Initialize(ground, platform->body, { 530.0f, 50.0f }, { 555.0f, 320.0f });
        b2LinearStiffness(djd.stiffness, djd.damping, freq, damp, ground, platform->body);
        world.CreateJoint(&djd);

        djd.Initialize(ground, platform->body, { 830.0f, 50.0f }, { 805.0f, 320.0f });
        b2LinearStiffness(djd.stiffness, djd.damping, freq, damp, ground, platform->body);
        world.CreateJoint(&djd);

        // -------------------------------------------------------------------
        // EXPERIMENTO PRISMATIC JOINT AL RAS DEL SUELO (Desplazado a la derecha)
        // -------------------------------------------------------------------
        lever = new PhysicsObject(world, 480, 535, 60, 40, b2_dynamicBody, RED);

        b2PrismaticJointDef pjd;

        // Eje { 1.0f, 0.0f } -> Restringe el movimiento estrictamente al eje X
        b2Vec2 ejeHorizontal(1.0f, 0.0f);

        // Inicializamos el joint prismático uniendo el suelo fijo (ground) con el bloque rojo
        pjd.Initialize(ground, lever->body, lever->body->GetPosition(), ejeHorizontal);

        // Habilitamos límites de carrera adaptados a su posición central
        pjd.lowerTranslation = -120.0f; // Límite izquierdo (evita que choque con la catapulta)
        pjd.upperTranslation = 150.0f;  // Límite derecho (le permite deslizarse libremente hacia el puente)
        pjd.enableLimit = true;

        // Creamos el joint en el mundo de Box2D
        world.CreateJoint(&pjd);
    }
}

Mechanism::~Mechanism() {
    if (platform) delete platform;
    if (lever) delete lever;
}

void Mechanism::Draw() {
    if (tipoEscena == 1) {
        // --- RENDERIZADO DEL PUENTE ---
        if (platform) {
            b2Vec2 pos = platform->body->GetPosition();
            float angle = platform->body->GetAngle() * RAD2DEG;

            // 1. Dibujamos la textura PRIMERO
            DrawTexturePro(texturaPuente,
                { 0, 0, (float)texturaPuente.width, (float)texturaPuente.height },
                { pos.x, pos.y, platform->width, platform->height },
                { platform->width / 2.0f, platform->height / 2.0f },
                -angle, WHITE);

            // 2. Quitamos platform->Draw() para que el rectángulo gris no tape la imagen
            // platform->Draw(); 

            // 3. Dibujamos los tirantes y el texto
            DrawLineEx({ 530, 50 }, { pos.x - 125, pos.y }, 2.0f, RAYWHITE);
            DrawLineEx({ 830, 50 }, { pos.x + 125, pos.y }, 2.0f, RAYWHITE);
            DrawText("DistanceJoint (Puente)", (int)pos.x - 60, (int)pos.y - 25, 12, GRAY);
        }

        // --- RENDERIZADO DEL BLOQUE (Locomotora) ---
        if (lever) {
            b2Vec2 pos = lever->body->GetPosition();
            float angle = lever->body->GetAngle() * RAD2DEG;

            DrawTexturePro(texturaBloque,
                { 0, 0, (float)texturaBloque.width, (float)texturaBloque.height },
                { pos.x, pos.y, lever->width, lever->height },
                { lever->width / 2.0f, lever->height / 2.0f },
                -angle, WHITE);

            DrawLineEx({ 330, 555 }, { 660, 555 }, 2.0f, GRAY);
            DrawText("PrismaticJoint (Riel Horizontal)", (int)pos.x - 75, (int)pos.y - 35, 12, RED);
        }
    }
}
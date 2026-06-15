#pragma once
#include "raylib.h"
#include "box2d.h"
#include "PhysicsObject.h"
#include "Mechanism.h"
#include "Bala.h"
#include <vector>

class Game {
private:
    const int screenWidth = 1000;
    const int screenHeight = 600;

    b2World* world;
    PhysicsObject* suelo;
    Mechanism* miSistema;

    std::vector<Bala*> projectiles;
    Bala* proyectilCargado;

    int estadoJuego;        // 0 = Menú, 1 a 4 = Escenas activas, 5 = En Construcción
    int estadoCatapulta;

    float anguloMavix;
    float anguloRotacionBala;
    float fuerzaMultiplicadora;

    Vector2 posicionMavix;

    void InitPhysicsForScene(int escena);
    void SpawnProyectilEnSuelo();
    void HandleInput();
    void Update();
    void Draw();
    void CleanUpScene();
    Texture2D texturaShellAmarillo;
    Texture2D texturaLocomotora;
    Texture2D texturaPuente;
    Sound sonidoReloj;
    float tiempoRestante;
    bool temporizadorActivo;

public:
    Game();
    ~Game();
    void Run(); // Declaración del bucle principal
};
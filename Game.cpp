#include "Game.h"
#include <cmath>

Game::Game() {
    InitWindow(screenWidth, screenHeight, "Catapulta y Joints");
    SetTargetFPS(60);
    InitAudioDevice();
    sonidoReloj = LoadSound("assets/Reloj.wav");
    texturaShellAmarillo = LoadTexture("assets/shell_amarillo.png");
    texturaLocomotora = LoadTexture("Assets/locomotora.png");
    texturaPuente = LoadTexture("Assets/puerta.png");


    estadoJuego = 0;
    estadoCatapulta = 1;
    anguloMavix = 40.0f;
    anguloRotacionBala = 45.0f;
    fuerzaMultiplicadora = 240.0f;
    posicionMavix = { 200.0f, (float)screenHeight - 40.0f };
    proyectilCargado = nullptr;
    world = nullptr;
    suelo = nullptr;
    miSistema = nullptr;
}

Game::~Game() {
    UnloadSound(sonidoReloj);
    CloseAudioDevice();
    UnloadTexture(texturaShellAmarillo);
    UnloadTexture(texturaLocomotora);
    UnloadTexture(texturaPuente);
    CleanUpScene();
    CloseWindow();
}

void Game::CleanUpScene() {
    if (proyectilCargado) { delete proyectilCargado; proyectilCargado = nullptr; }
    for (auto p : projectiles) delete p;
    projectiles.clear();

    if (miSistema) { delete miSistema; miSistema = nullptr; }
    if (suelo) { delete suelo; suelo = nullptr; }
    if (world) { delete world; world = nullptr; }
}

void Game::InitPhysicsForScene(int escena) {
    CleanUpScene();

    b2Vec2 gravity(0.0f, 18.5f);
    world = new b2World(gravity);

    // Suelo
    suelo = new PhysicsObject(*world, screenWidth / 2.0f, screenHeight - 20.0f, (float)screenWidth, 40.0f, b2_staticBody, DARKGREEN);

    // Paredes invisibles
    new PhysicsObject(*world, -10.0f, screenHeight / 2.0f, 20.0f, (float)screenHeight, b2_staticBody, BLANK);
    new PhysicsObject(*world, (float)screenWidth + 10.0f, screenHeight / 2.0f, 20.0f, (float)screenHeight, b2_staticBody, BLANK);
    new PhysicsObject(*world, screenWidth / 2.0f, -10.0f, (float)screenWidth, 20.0f, b2_staticBody, BLANK);

   // miSistema = new Mechanism(*world, suelo->body, escena);
   // miSistema = new Mechanism(*world, suelo->body, escena, texturaLocomotora);
    miSistema = new Mechanism(*world, suelo->body, escena, texturaLocomotora, texturaPuente);

    estadoCatapulta = 1;
    SpawnProyectilEnSuelo();
    tiempoRestante = 30.0f;
    temporizadorActivo = true;
    PlaySoundMulti(sonidoReloj); // Reproduce en bucle (o usa PlaySound)
}

void Game::SpawnProyectilEnSuelo() {
    if (proyectilCargado == nullptr && world != nullptr) {
        float angleRad = anguloMavix * DEG2RAD;
        b2Vec2 posicionPunta = {
            posicionMavix.x + cosf(angleRad) * 50.0f,
            posicionMavix.y - sinf(angleRad) * 50.0f
        };
        proyectilCargado = new Bala(world, posicionPunta, anguloRotacionBala, texturaShellAmarillo);
        proyectilCargado->GetBody()->SetGravityScale(0.0f);
       // proyectilCargado = new Bala(world, posicionPunta, anguloRotacionBala);
       // proyectilCargado->GetBody()->SetGravityScale(0.0f);
    }
}

void Game::HandleInput() {
    if (estadoJuego == 0) {
        //  Solo la opción 1 permanece válida e inicializa las físicas.
        if (IsKeyPressed(KEY_ONE)) { estadoJuego = 1; InitPhysicsForScene(1); }

        // Las opciones de la 2 a la 5 quedan explícitamente vacías y bloqueadas.
        if (IsKeyPressed(KEY_TWO)) { /* Bloqueado - No hace nada */ }
        if (IsKeyPressed(KEY_THREE)) { /* Bloqueado - No hace nada */ }
        if (IsKeyPressed(KEY_FOUR)) { /* Bloqueado - No hace nada */ }
        if (IsKeyPressed(KEY_FIVE)) { /* Bloqueado - No hace nada */ }
    }
    else {
        if (IsKeyPressed(KEY_M)) { estadoJuego = 0; CleanUpScene(); return; }

        if (estadoCatapulta == 1 && proyectilCargado != nullptr) {
            if (IsKeyDown(KEY_LEFT))   anguloRotacionBala += 3.0f;
            if (IsKeyDown(KEY_RIGHT)) anguloRotacionBala -= 3.0f;

            if (IsKeyDown(KEY_UP))   anguloMavix += 1.5f;
            if (IsKeyDown(KEY_DOWN)) anguloMavix -= 1.5f;

            if (anguloMavix > 85.0f) anguloMavix = 85.0f;
            if (anguloMavix < 5.0f)  anguloMavix = 5.0f;

            float angleRad = anguloMavix * DEG2RAD;
            b2Vec2 nuevaPosicionPunta = {
                posicionMavix.x + cosf(angleRad) * 50.0f,
                posicionMavix.y - sinf(angleRad) * 50.0f
            };

            proyectilCargado->GetBody()->SetTransform(nuevaPosicionPunta, -anguloRotacionBala * DEG2RAD);

            if (IsKeyPressed(KEY_SPACE)) {
                proyectilCargado->GetBody()->SetGravityScale(1.0f);

                float vx = cosf(angleRad) * fuerzaMultiplicadora;
                float vy = -sinf(angleRad) * fuerzaMultiplicadora;

                float masa = proyectilCargado->GetBody()->GetMass();
                b2Vec2 impulse(vx * masa, vy * masa);

                proyectilCargado->GetBody()->ApplyLinearImpulseToCenter(impulse, true);

                projectiles.push_back(proyectilCargado);
                proyectilCargado = nullptr;

                SpawnProyectilEnSuelo();
            }
        }
    }
}

void Game::Update() {
    if (estadoJuego != 0 && world != nullptr) {
        world->Step(1.0f / 60.0f, 8, 3);

        if (temporizadorActivo) {
            if (!IsSoundPlaying(sonidoReloj)) {
                PlaySound(sonidoReloj);
            }
            tiempoRestante -= GetFrameTime();
            if (tiempoRestante <= 0) {
                tiempoRestante = 0;
                temporizadorActivo = false;
                StopSound(sonidoReloj);
            }
        }
        else if (tiempoRestante <= 0) {
            // Lógica para reiniciar el juego al presionar ESPACIO cuando se agotó el tiempo
            if (IsKeyPressed(KEY_SPACE)) {
                InitPhysicsForScene(1);
            }
        }
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground({ 30, 30, 40, 255 });

    if (estadoJuego == 0) {
        // --- MENÚ PRINCIPAL ---
        DrawText("           UNL - PROGRAMACION DE VIDEOJUEGOS", 120, 50, 28, YELLOW);
        DrawText("Joints de Box2D", 250, 100, 20, LIGHTGRAY);
        DrawRectangleLines(150, 160, 700, 370, GRAY);
        DrawText("PRESIONE UNA TECLA PARA SELECCIONAR LA ESCENA FISICA:", 180, 180, 16, RAYWHITE);

        DrawText("[ 1 ] - Escena 1: DistanceJoint & PrismaticJoint (Puente y Riel)", 200, 220, 18, SKYBLUE);
        DrawText("[ 2 ] - Escena 2: PrismaticJoint & WheelJoint (En Construccion)", 200, 260, 18, GOLD);
        DrawText("[ 3 ] - Escena 3: PulleyJoint & WeldJoint (En Construccion)", 200, 300, 18, LIME);
        DrawText("[ 4 ] - Escena 4: GearJoint (En Construccion)", 200, 340, 18, PINK);
        DrawText("[ 5 ] - Escena 5: Friction & Motor Joint (En Construccion)", 200, 380, 18, RED);

        DrawText("Desarrollado por Claudio Fernando Navarro", 320, 550, 14, GRAY);
    }
    else {
        // --- ESCENAS ACTIVAS ---

        // 1. Lógica del texto de tiempo / pantalla de fin
        if (tiempoRestante > 0) {
            DrawText(TextFormat("Tiempo: %.1fs", tiempoRestante), 850, 10, 20, (tiempoRestante < 5) ? RED : RAYWHITE);
        }
        else {
            DrawRectangle(250, 200, 500, 150, Fade(BLACK, 0.8f));
            DrawText("TIEMPO AGOTADO", 350, 230, 30, RED);
            DrawText("Presione ESPACIO para volver a jugar", 310, 280, 20, WHITE);
        }

        // 2. Dibujo de los objetos físicos
        if (suelo) suelo->Draw();
        if (miSistema) miSistema->Draw();
        if (proyectilCargado != nullptr) proyectilCargado->Draw();
        for (Bala* p : projectiles) {
            p->Draw();
        }

        // 3. Dibujo de la Catapulta (Mavix)
        DrawCircleV(posicionMavix, 20, DARKGRAY);
        float angleRad = anguloMavix * DEG2RAD;
        Vector2 puntaBrazo = { posicionMavix.x + cosf(angleRad) * 50, posicionMavix.y - sinf(angleRad) * 50 };
        DrawLineEx(posicionMavix, puntaBrazo, 6.0f, RED);

        // 4. Interfaz de texto del juego
        DrawText("Mecanismo de Disparo [Mavix Catapulta - UNL]", 10, 10, 16, YELLOW);
        DrawText("Controles: [ARRIBA/ABAJO] Ángulo Brazo  --  [IZQ/DER] Rotar ", 10, 35, 14, GREEN);
        DrawText(TextFormat("Ángulo Catapulta: %.0f° | Rotación propia del Proyectil: %.0f°", anguloMavix, anguloRotacionBala), 10, 55, 14, RAYWHITE);
        DrawText("Presione [ ESPACIO ] para disparar  --  Presione [ M ] para ir al Menú", 10, 80, 14, ORANGE);

        // 5. Indicadores de estado de escenas
        //if (estadoJuego == 1) DrawText("ESCENA 1: SOPORTE ELASTICO Y LINEAL", 600, 15, 18, SKYBLUE);
        if (estadoJuego == 2) DrawText("ESCENA 2: EN CONSTRUCCION", 680, 15, 18, GOLD);
        if (estadoJuego == 3) DrawText("ESCENA 3: EN CONSTRUCCION", 680, 15, 18, LIME);
        if (estadoJuego == 4) DrawText("ESCENA 4: EN CONSTRUCCION", 680, 15, 18, PINK);

        if (estadoJuego == 5) {
            DrawText("ESCENA 5: EN CONSTRUCCION", 680, 15, 18, RED);
            DrawRectangle(450, 250, 420, 120, { 50, 50, 60, 230 });
            DrawRectangleLines(450, 250, 420, 120, RED);
            DrawText("ESCENA EN CONSTRUCCIÓN", 520, 280, 20, RED);
            DrawText("Próximamente: MotorJoint & FrictionJoint", 480, 320, 16, LIGHTGRAY);
        }
    }
    EndDrawing();
}

void Game::Run() {
    while (!WindowShouldClose()) {
        HandleInput();
        Update();
        Draw();
    }
}
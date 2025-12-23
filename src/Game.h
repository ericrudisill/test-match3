#pragma once

#include "Grid.h"
#include "Renderer.h"
#include "InputHandler.h"
#include <SDL3/SDL.h>
#include <memory>

enum class GameState {
    PLAYING,
    CHECKING_MATCHES,
    REMOVING_MATCHES,
    NO_MOVES
};

class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();
    void cleanup();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::unique_ptr<Grid> grid;
    std::unique_ptr<Renderer> gameRenderer;
    std::unique_ptr<InputHandler> inputHandler;

    bool running;
    GameState state;
    Uint64 lastTime;

    void handleEvents();
    void update(float deltaTime);
    void render();
    void processInput();
    void updateGameLogic(float deltaTime);
};

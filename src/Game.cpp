#include "Game.h"
#include "MathUtils.h"
#include <SDL3_ttf/SDL_ttf.h>

Game::Game()
    : window(nullptr)
    , renderer(nullptr)
    , running(false)
    , state(GameState::PLAYING)
    , lastTime(0)
{
}

Game::~Game() {
    cleanup();
}

bool Game::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return false;
    }

    if (!TTF_Init()) {
        SDL_Log("SDL_ttf initialization failed: %s", SDL_GetError());
        return false;
    }

    // Create window with appropriate size for mobile/desktop
    int windowWidth = 720;
    int windowHeight = 1280;

#ifdef PLATFORM_DESKTOP
    windowWidth = 1080;
    windowHeight = 1920;
#endif

    window = SDL_CreateWindow(
        "Match 3 Game",
        windowWidth,
        windowHeight,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        return false;
    }

    // Enable VSync
    SDL_SetRenderVSync(renderer, 1);

    // Get actual window size (may differ on high-DPI displays)
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Initialize game objects
    grid = std::make_unique<Grid>();
    gameRenderer = std::make_unique<Renderer>(renderer, windowWidth, windowHeight);
    inputHandler = std::make_unique<InputHandler>(
        gameRenderer->getGemSize(),
        gameRenderer->getGridOffsetX(),
        gameRenderer->getGridOffsetY()
    );

    lastTime = SDL_GetTicks();
    running = true;

    return true;
}

void Game::run() {
    while (running) {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        deltaTime = MathUtils::clampDeltaTime(deltaTime);

        handleEvents();
        update(deltaTime);
        render();
    }
}

void Game::cleanup() {
    inputHandler.reset();
    gameRenderer.reset();
    grid.reset();

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    TTF_Quit();
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }
        else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            int width, height;
            SDL_GetWindowSize(window, &width, &height);
            gameRenderer->setWindowSize(width, height);
            inputHandler->update(
                gameRenderer->getGemSize(),
                gameRenderer->getGridOffsetX(),
                gameRenderer->getGridOffsetY()
            );
        }
        else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            running = false;
        }
        else {
            inputHandler->handleEvent(event);
        }
    }
}

void Game::update(float deltaTime) {
    grid->update(deltaTime);

    if (state == GameState::PLAYING && !grid->isAnimating()) {
        processInput();
    }

    updateGameLogic(deltaTime);
}

void Game::render() {
    gameRenderer->render(*grid);
}

void Game::processInput() {
    if (inputHandler->hasPendingSwap()) {
        int row1, col1, row2, col2;
        inputHandler->getSwap(row1, col1, row2, col2);

        if (grid->swapGems(row1, col1, row2, col2)) {
            state = GameState::CHECKING_MATCHES;
        }

        inputHandler->clearSwap();
        inputHandler->clearSelection();
    }
}

void Game::updateGameLogic(float deltaTime) {
    if (grid->isAnimating()) {
        return;
    }

    switch (state) {
        case GameState::CHECKING_MATCHES:
            grid->checkMatches();
            state = GameState::REMOVING_MATCHES;
            break;

        case GameState::REMOVING_MATCHES:
            grid->removeMatches();
            grid->applyGravity();

            // Check for cascade matches
            grid->checkMatches();
            if (!grid->isAnimating()) {
                state = GameState::PLAYING;

                // Check if there are valid moves
                if (!grid->hasValidMoves()) {
                    SDL_Log("No more valid moves! Game over. Score: %d", grid->getScore());
                    state = GameState::NO_MOVES;
                }
            }
            break;

        case GameState::NO_MOVES:
            // Could reset or show game over screen
            break;

        case GameState::PLAYING:
            // Waiting for input
            break;
    }
}

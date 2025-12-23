#pragma once

#include "Grid.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class Renderer {
public:
    Renderer(SDL_Renderer* renderer, int windowWidth, int windowHeight);
    ~Renderer();

    void render(const Grid& grid);
    void setWindowSize(int width, int height);

    int getGemSize() const { return gemSize; }
    int getGridOffsetX() const { return gridOffsetX; }
    int getGridOffsetY() const { return gridOffsetY; }

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int windowWidth;
    int windowHeight;
    int gemSize;
    int gridOffsetX;
    int gridOffsetY;

    void calculateLayout();
    void drawGem(const Gem* gem, float alpha = 1.0f);
    void drawBackground();
    void drawScore(int score);
    SDL_Color getGemColor(GemType type) const;
};

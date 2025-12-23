#pragma once

#include "Gem.h"
#include <vector>
#include <memory>

class Grid {
public:
    static const int ROWS = 8;
    static const int COLS = 8;

    Grid();

    void update(float deltaTime);
    bool isAnimating() const;

    Gem* getGem(int row, int col) const;
    bool swapGems(int row1, int col1, int row2, int col2);
    void checkMatches();
    void removeMatches();
    void applyGravity();
    void fillEmpty();

    int getScore() const { return score; }
    bool hasValidMoves() const;

private:
    std::vector<std::vector<std::unique_ptr<Gem>>> gems;
    std::vector<std::pair<int, int>> matchedPositions;
    int score;

    void createGem(int row, int col);
    GemType getRandomGemType() const;
    bool isValidPosition(int row, int col) const;
    bool wouldCreateMatch(int row, int col, GemType type) const;
    void findMatches(int row, int col, std::vector<std::pair<int, int>>& matches);
    bool areAdjacent(int row1, int col1, int row2, int col2) const;
};

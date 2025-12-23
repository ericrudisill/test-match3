#pragma once

#include "Gem.h"
#include "BoardLogic.h"
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

    int getScore() const { return boardState.score; }
    bool hasValidMoves() const;

    const BoardState& getBoardState() const { return boardState; }

private:
    std::vector<std::vector<std::unique_ptr<Gem>>> gems;
    std::vector<std::pair<int, int>> matchedPositions;
    BoardState boardState;
    BoardLogic boardLogic;

    void createGem(int row, int col);
    void syncGemToBoard(int row, int col);
    void syncBoardToGem(int row, int col);
    bool isValidPosition(int row, int col) const;
    bool areAdjacent(int row1, int col1, int row2, int col2) const;
};

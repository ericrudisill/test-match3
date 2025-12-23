#include "Grid.h"
#include <algorithm>

Grid::Grid() {
    gems.resize(ROWS);
    for (int row = 0; row < ROWS; ++row) {
        gems[row].resize(COLS);
    }

    // Initialize board state using BoardLogic (avoids initial matches)
    boardLogic.initializeBoard(boardState);

    // Create Gem objects to match board state
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            syncBoardToGem(row, col);
        }
    }
}

void Grid::update(float deltaTime) {
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (gems[row][col]) {
                gems[row][col]->update(deltaTime);
            }
        }
    }
}

bool Grid::isAnimating() const {
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (gems[row][col] && gems[row][col]->isAnimating()) {
                return true;
            }
        }
    }
    return false;
}

Gem* Grid::getGem(int row, int col) const {
    if (!isValidPosition(row, col)) return nullptr;
    return gems[row][col].get();
}

bool Grid::swapGems(int row1, int col1, int row2, int col2) {
    if (!isValidPosition(row1, col1) || !isValidPosition(row2, col2)) {
        return false;
    }

    if (!areAdjacent(row1, col1, row2, col2)) {
        return false;
    }

    Gem* gem1 = gems[row1][col1].get();
    Gem* gem2 = gems[row2][col2].get();

    if (!gem1 || !gem2) return false;

    // Swap in grid and board state
    std::swap(gems[row1][col1], gems[row2][col2]);
    std::swap(boardState.at(row1, col1), boardState.at(row2, col2));

    // Update gem positions and trigger animation
    gem1->setRow(row2);
    gem1->setCol(col2);
    gem1->setTarget(row2, col2);
    gem1->setState(GemState::SWAPPING);

    gem2->setRow(row1);
    gem2->setCol(col1);
    gem2->setTarget(row1, col1);
    gem2->setState(GemState::SWAPPING);

    return true;
}

void Grid::checkMatches() {
    auto result = boardLogic.checkMatches(boardState);

    matchedPositions.clear();
    for (const auto& pos : result.matchedPositions) {
        matchedPositions.push_back({pos.row, pos.col});
    }
}

void Grid::removeMatches() {
    if (matchedPositions.empty()) return;

    // Convert to Position vector for BoardLogic
    std::vector<Position> positions;
    for (const auto& pos : matchedPositions) {
        positions.push_back({pos.first, pos.second});
    }

    // Update score before removing
    boardState.score += static_cast<int>(positions.size()) * 10;

    // Set gems to exploding state (for animation)
    for (const auto& pos : matchedPositions) {
        int row = pos.first;
        int col = pos.second;
        if (gems[row][col]) {
            gems[row][col]->setState(GemState::EXPLODING);
        }
    }

    // Update board state
    boardLogic.removeMatches(boardState, positions);
}

void Grid::applyGravity() {
    // Remove gems that are ready for removal (finished exploding)
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (gems[row][col] && gems[row][col]->getState() == GemState::READY_FOR_REMOVAL) {
                gems[row][col].reset();
            }
        }
    }

    // Compute gravity moves using BoardLogic
    auto result = boardLogic.applyGravity(boardState);

    // Apply moves to Gem objects with animation
    for (const auto& move : result.moves) {
        int fromRow = move.from.row, fromCol = move.from.col;
        int toRow = move.to.row, toCol = move.to.col;

        gems[toRow][toCol] = std::move(gems[fromRow][fromCol]);
        gems[toRow][toCol]->setRow(toRow);
        gems[toRow][toCol]->setCol(toCol);
        gems[toRow][toCol]->setTarget(toRow, toCol);
        gems[toRow][toCol]->setState(GemState::FALLING);
    }
}

void Grid::fillEmpty() {
    // Collect empty positions
    std::vector<Position> emptyPositions;
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (!gems[row][col]) {
                emptyPositions.push_back({row, col});
            }
        }
    }

    // Fill empty positions in board state
    boardLogic.fillEmpty(boardState, emptyPositions);

    // Create Gem objects for filled positions
    for (const auto& pos : emptyPositions) {
        syncBoardToGem(pos.row, pos.col);
        gems[pos.row][pos.col]->setY(-1.0f);
        gems[pos.row][pos.col]->setTarget(pos.row, pos.col);
        gems[pos.row][pos.col]->setState(GemState::FALLING);
    }
}

void Grid::createGem(int row, int col) {
    GemType type = boardState.at(row, col);
    gems[row][col] = std::make_unique<Gem>(row, col, type);
}

void Grid::syncGemToBoard(int row, int col) {
    if (gems[row][col]) {
        boardState.at(row, col) = gems[row][col]->getType();
    } else {
        boardState.at(row, col) = GemType::EMPTY;
    }
}

void Grid::syncBoardToGem(int row, int col) {
    GemType type = boardState.at(row, col);
    if (type != GemType::EMPTY) {
        gems[row][col] = std::make_unique<Gem>(row, col, type);
    } else {
        gems[row][col].reset();
    }
}

bool Grid::isValidPosition(int row, int col) const {
    return row >= 0 && row < ROWS && col >= 0 && col < COLS;
}

bool Grid::areAdjacent(int row1, int col1, int row2, int col2) const {
    int rowDiff = std::abs(row1 - row2);
    int colDiff = std::abs(col1 - col2);
    return (rowDiff == 1 && colDiff == 0) || (rowDiff == 0 && colDiff == 1);
}

bool Grid::hasValidMoves() const {
    return boardLogic.hasValidMoves(boardState);
}

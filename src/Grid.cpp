#include "Grid.h"
#include <algorithm>
#include <random>
#include <set>

Grid::Grid() : score(0) {
    gems.resize(ROWS);
    for (int row = 0; row < ROWS; ++row) {
        gems[row].resize(COLS);
        for (int col = 0; col < COLS; ++col) {
            createGem(row, col);
        }
    }

    // Remove any initial matches
    while (true) {
        checkMatches();
        if (matchedPositions.empty()) break;
        removeMatches();
        applyGravity();
        fillEmpty();
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

    // Swap in grid
    std::swap(gems[row1][col1], gems[row2][col2]);

    // Update gem positions
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
    matchedPositions.clear();
    std::set<std::pair<int, int>> uniqueMatches;

    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (!gems[row][col]) continue;

            std::vector<std::pair<int, int>> matches;
            findMatches(row, col, matches);

            if (matches.size() >= 3) {
                for (const auto& pos : matches) {
                    uniqueMatches.insert(pos);
                }
            }
        }
    }

    matchedPositions.assign(uniqueMatches.begin(), uniqueMatches.end());
}

void Grid::removeMatches() {
    if (matchedPositions.empty()) return;

    for (const auto& pos : matchedPositions) {
        int row = pos.first;
        int col = pos.second;
        if (gems[row][col]) {
            gems[row][col]->setState(GemState::EXPLODING);
            score += 10;
        }
    }
}

void Grid::applyGravity() {
    // Remove exploded gems first
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (gems[row][col] && gems[row][col]->getState() == GemState::MATCHED) {
                gems[row][col].reset();
            }
        }
    }

    // Apply gravity
    for (int col = 0; col < COLS; ++col) {
        int writeRow = ROWS - 1;
        for (int row = ROWS - 1; row >= 0; --row) {
            if (gems[row][col]) {
                if (row != writeRow) {
                    gems[writeRow][col] = std::move(gems[row][col]);
                    gems[writeRow][col]->setRow(writeRow);
                    gems[writeRow][col]->setCol(col);
                    gems[writeRow][col]->setTarget(writeRow, col);
                    gems[writeRow][col]->setState(GemState::FALLING);
                }
                writeRow--;
            }
        }
    }
}

void Grid::fillEmpty() {
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (!gems[row][col]) {
                createGem(row, col);
                gems[row][col]->setY(-1.0f);
                gems[row][col]->setTarget(row, col);
                gems[row][col]->setState(GemState::FALLING);
            }
        }
    }
}

void Grid::createGem(int row, int col) {
    GemType type;
    do {
        type = getRandomGemType();
    } while (wouldCreateMatch(row, col, type));

    gems[row][col] = std::make_unique<Gem>(row, col, type);
}

GemType Grid::getRandomGemType() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(GemType::COUNT) - 1);
    return static_cast<GemType>(dis(gen));
}

bool Grid::isValidPosition(int row, int col) const {
    return row >= 0 && row < ROWS && col >= 0 && col < COLS;
}

bool Grid::wouldCreateMatch(int row, int col, GemType type) const {
    // Check horizontal
    int horizontalCount = 1;
    for (int c = col - 1; c >= 0; --c) {
        if (c >= static_cast<int>(gems[row].size()) || !gems[row][c]) break;
        if (gems[row][c]->getType() != type) break;
        horizontalCount++;
    }
    for (int c = col + 1; c < COLS; ++c) {
        if (c >= static_cast<int>(gems[row].size()) || !gems[row][c]) break;
        if (gems[row][c]->getType() != type) break;
        horizontalCount++;
    }
    if (horizontalCount >= 3) return true;

    // Check vertical
    int verticalCount = 1;
    for (int r = row - 1; r >= 0; --r) {
        if (r >= static_cast<int>(gems.size()) || col >= static_cast<int>(gems[r].size()) || !gems[r][col]) break;
        if (gems[r][col]->getType() != type) break;
        verticalCount++;
    }
    for (int r = row + 1; r < ROWS; ++r) {
        if (r >= static_cast<int>(gems.size()) || col >= static_cast<int>(gems[r].size()) || !gems[r][col]) break;
        if (gems[r][col]->getType() != type) break;
        verticalCount++;
    }
    if (verticalCount >= 3) return true;

    return false;
}

void Grid::findMatches(int row, int col, std::vector<std::pair<int, int>>& matches) {
    if (!gems[row][col]) return;

    GemType type = gems[row][col]->getType();
    matches.clear();

    // Check horizontal
    std::vector<std::pair<int, int>> horizontal;
    horizontal.push_back({row, col});
    for (int c = col - 1; c >= 0 && gems[row][c] && gems[row][c]->getType() == type; --c) {
        horizontal.push_back({row, c});
    }
    for (int c = col + 1; c < COLS && gems[row][c] && gems[row][c]->getType() == type; ++c) {
        horizontal.push_back({row, c});
    }

    // Check vertical
    std::vector<std::pair<int, int>> vertical;
    vertical.push_back({row, col});
    for (int r = row - 1; r >= 0 && gems[r][col] && gems[r][col]->getType() == type; --r) {
        vertical.push_back({r, col});
    }
    for (int r = row + 1; r < ROWS && gems[r][col] && gems[r][col]->getType() == type; ++r) {
        vertical.push_back({r, col});
    }

    // Add matches
    if (horizontal.size() >= 3) {
        matches.insert(matches.end(), horizontal.begin(), horizontal.end());
    }
    if (vertical.size() >= 3) {
        matches.insert(matches.end(), vertical.begin(), vertical.end());
    }
}

bool Grid::areAdjacent(int row1, int col1, int row2, int col2) const {
    int rowDiff = std::abs(row1 - row2);
    int colDiff = std::abs(col1 - col2);
    return (rowDiff == 1 && colDiff == 0) || (rowDiff == 0 && colDiff == 1);
}

bool Grid::hasValidMoves() const {
    // Simple check: try all possible swaps
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (!gems[row][col]) continue;

            // Check right swap
            if (col + 1 < COLS && gems[row][col + 1]) {
                GemType type1 = gems[row][col]->getType();
                GemType type2 = gems[row][col + 1]->getType();

                // Simulate swap
                if (wouldCreateMatch(row, col, type2) || wouldCreateMatch(row, col + 1, type1)) {
                    return true;
                }
            }

            // Check down swap
            if (row + 1 < ROWS && gems[row + 1][col]) {
                GemType type1 = gems[row][col]->getType();
                GemType type2 = gems[row + 1][col]->getType();

                // Simulate swap
                if (wouldCreateMatch(row, col, type2) || wouldCreateMatch(row + 1, col, type1)) {
                    return true;
                }
            }
        }
    }
    return false;
}

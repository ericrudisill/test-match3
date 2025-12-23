#include "BoardLogic.h"
#include <random>
#include <set>
#include <algorithm>

BoardLogic::BoardLogic(GemFactory factory) : gemFactory(factory) {
    if (!gemFactory) {
        gemFactory = [this](int, int) {
            return getRandomGemType();
        };
    }
}

GemType BoardLogic::getRandomGemType() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(GemType::COUNT) - 1);
    return static_cast<GemType>(dis(gen));
}

void BoardLogic::initializeBoard(BoardState& state) const {
    for (int row = 0; row < BoardState::ROWS; ++row) {
        for (int col = 0; col < BoardState::COLS; ++col) {
            GemType type;
            do {
                type = gemFactory(row, col);
            } while (wouldCreateMatch(state, row, col, type));
            state.at(row, col) = type;
        }
    }
}

MatchResult BoardLogic::checkMatches(const BoardState& state) const {
    MatchResult result;
    std::set<Position> uniqueMatches;

    for (int row = 0; row < BoardState::ROWS; ++row) {
        for (int col = 0; col < BoardState::COLS; ++col) {
            if (state.at(row, col) == GemType::EMPTY) continue;

            std::vector<Position> horizontal, vertical;
            findHorizontalMatches(state, row, col, horizontal);
            findVerticalMatches(state, row, col, vertical);

            if (horizontal.size() >= 3) {
                for (const auto& pos : horizontal) {
                    uniqueMatches.insert(pos);
                }
            }
            if (vertical.size() >= 3) {
                for (const auto& pos : vertical) {
                    uniqueMatches.insert(pos);
                }
            }
        }
    }

    for (const auto& pos : uniqueMatches) {
        result.matchedPositions.push_back(pos);
        result.score += 10;
    }

    return result;
}

void BoardLogic::findHorizontalMatches(const BoardState& state, int row, int col,
                                       std::vector<Position>& matches) const {
    if (state.at(row, col) == GemType::EMPTY) return;

    GemType type = state.at(row, col);
    matches.clear();
    matches.push_back({row, col});

    // Check left
    for (int c = col - 1; c >= 0 && state.at(row, c) == type; --c) {
        matches.push_back({row, c});
    }
    // Check right
    for (int c = col + 1; c < BoardState::COLS && state.at(row, c) == type; ++c) {
        matches.push_back({row, c});
    }
}

void BoardLogic::findVerticalMatches(const BoardState& state, int row, int col,
                                     std::vector<Position>& matches) const {
    if (state.at(row, col) == GemType::EMPTY) return;

    GemType type = state.at(row, col);
    matches.clear();
    matches.push_back({row, col});

    // Check up
    for (int r = row - 1; r >= 0 && state.at(r, col) == type; --r) {
        matches.push_back({r, col});
    }
    // Check down
    for (int r = row + 1; r < BoardState::ROWS && state.at(r, col) == type; ++r) {
        matches.push_back({r, col});
    }
}

void BoardLogic::removeMatches(BoardState& state, const std::vector<Position>& positions) const {
    for (const auto& pos : positions) {
        if (state.isValid(pos.row, pos.col)) {
            state.at(pos.row, pos.col) = GemType::EMPTY;
        }
    }
}

GravityResult BoardLogic::applyGravity(BoardState& state) const {
    GravityResult result;

    for (int col = 0; col < BoardState::COLS; ++col) {
        int writeRow = BoardState::ROWS - 1;

        // Move existing gems down
        for (int row = BoardState::ROWS - 1; row >= 0; --row) {
            if (state.at(row, col) != GemType::EMPTY) {
                if (row != writeRow) {
                    result.moves.push_back({{row, col}, {writeRow, col}});
                    state.at(writeRow, col) = state.at(row, col);
                    state.at(row, col) = GemType::EMPTY;
                }
                writeRow--;
            }
        }

        // Record empty positions that need filling
        for (int row = 0; row <= writeRow; ++row) {
            result.emptyPositions.push_back({row, col});
        }
    }

    return result;
}

void BoardLogic::fillEmpty(BoardState& state, const std::vector<Position>& positions) const {
    for (const auto& pos : positions) {
        if (state.isValid(pos.row, pos.col) && state.at(pos.row, pos.col) == GemType::EMPTY) {
            state.at(pos.row, pos.col) = gemFactory(pos.row, pos.col);
        }
    }
}

bool BoardLogic::isValidSwap(const BoardState& state, const Move& move) const {
    if (!state.isValid(move.from.row, move.from.col)) return false;
    if (!state.isValid(move.to.row, move.to.col)) return false;
    if (state.at(move.from.row, move.from.col) == GemType::EMPTY) return false;
    if (state.at(move.to.row, move.to.col) == GemType::EMPTY) return false;
    return areAdjacent(move.from, move.to);
}

bool BoardLogic::areAdjacent(const Position& a, const Position& b) const {
    int rowDiff = std::abs(a.row - b.row);
    int colDiff = std::abs(a.col - b.col);
    return (rowDiff == 1 && colDiff == 0) || (rowDiff == 0 && colDiff == 1);
}

bool BoardLogic::wouldCreateMatch(const BoardState& state, int row, int col, GemType type) const {
    if (type == GemType::EMPTY) return false;

    // Check horizontal
    int horizontalCount = 1;
    for (int c = col - 1; c >= 0 && state.at(row, c) == type; --c) {
        horizontalCount++;
    }
    for (int c = col + 1; c < BoardState::COLS && state.at(row, c) == type; ++c) {
        horizontalCount++;
    }
    if (horizontalCount >= 3) return true;

    // Check vertical
    int verticalCount = 1;
    for (int r = row - 1; r >= 0 && state.at(r, col) == type; --r) {
        verticalCount++;
    }
    for (int r = row + 1; r < BoardState::ROWS && state.at(r, col) == type; ++r) {
        verticalCount++;
    }
    if (verticalCount >= 3) return true;

    return false;
}

void BoardLogic::executeSwap(BoardState& state, const Move& move) const {
    std::swap(state.at(move.from.row, move.from.col),
              state.at(move.to.row, move.to.col));
}

bool BoardLogic::hasValidMoves(const BoardState& state) const {
    for (int row = 0; row < BoardState::ROWS; ++row) {
        for (int col = 0; col < BoardState::COLS; ++col) {
            if (state.at(row, col) == GemType::EMPTY) continue;

            GemType type1 = state.at(row, col);

            // Check right swap
            if (col + 1 < BoardState::COLS && state.at(row, col + 1) != GemType::EMPTY) {
                GemType type2 = state.at(row, col + 1);

                // Temporarily swap and check
                BoardState temp = state;
                temp.at(row, col) = type2;
                temp.at(row, col + 1) = type1;

                if (wouldCreateMatch(temp, row, col, type2) ||
                    wouldCreateMatch(temp, row, col + 1, type1)) {
                    return true;
                }
            }

            // Check down swap
            if (row + 1 < BoardState::ROWS && state.at(row + 1, col) != GemType::EMPTY) {
                GemType type2 = state.at(row + 1, col);

                // Temporarily swap and check
                BoardState temp = state;
                temp.at(row, col) = type2;
                temp.at(row + 1, col) = type1;

                if (wouldCreateMatch(temp, row, col, type2) ||
                    wouldCreateMatch(temp, row + 1, col, type1)) {
                    return true;
                }
            }
        }
    }
    return false;
}

BoardLogic::SequenceResult BoardLogic::executeSequence(BoardState& state, const Move& move) const {
    SequenceResult result;

    if (!isValidSwap(state, move)) {
        return result;
    }

    // Execute swap
    executeSwap(state, move);

    // Check if swap creates a match
    auto matchResult = checkMatches(state);
    if (matchResult.matchedPositions.empty()) {
        // Invalid swap - reverse it
        executeSwap(state, move);
        return result;
    }

    result.swapValid = true;

    // Process cascades
    while (!matchResult.matchedPositions.empty()) {
        result.matches.push_back(matchResult);
        result.totalScore += matchResult.score;

        removeMatches(state, matchResult.matchedPositions);
        auto gravityResult = applyGravity(state);
        result.gravities.push_back(gravityResult);

        fillEmpty(state, gravityResult.emptyPositions);

        matchResult = checkMatches(state);
    }

    state.score += result.totalScore;
    return result;
}

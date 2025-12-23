#pragma once

#include <vector>
#include <utility>
#include <functional>

enum class GemType {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    ORANGE,
    COUNT,
    EMPTY
};

struct Position {
    int row;
    int col;

    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }

    bool operator<(const Position& other) const {
        if (row != other.row) return row < other.row;
        return col < other.col;
    }
};

struct Move {
    Position from;
    Position to;
};

struct GravityMove {
    Position from;
    Position to;
};

struct MatchResult {
    std::vector<Position> matchedPositions;
    int score = 0;
};

struct GravityResult {
    std::vector<GravityMove> moves;
    std::vector<Position> emptyPositions;
};

class BoardState {
public:
    static const int ROWS = 8;
    static const int COLS = 8;

    BoardState() {
        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                gems[row][col] = GemType::EMPTY;
            }
        }
    }

    GemType& at(int row, int col) { return gems[row][col]; }
    GemType at(int row, int col) const { return gems[row][col]; }

    bool isValid(int row, int col) const {
        return row >= 0 && row < ROWS && col >= 0 && col < COLS;
    }

    int score = 0;

private:
    GemType gems[ROWS][COLS];
};

using GemFactory = std::function<GemType(int row, int col)>;

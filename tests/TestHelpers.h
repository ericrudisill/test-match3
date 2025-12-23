#pragma once

#include "BoardTypes.h"
#include "BoardLogic.h"
#include <string>
#include <vector>
#include <memory>

// Parse board state from ASCII art for readable tests
// Characters: R=Red, G=Green, B=Blue, Y=Yellow, P=Purple, O=Orange, .=Empty
// Example:
//   parseBoard({
//       "RRRGGBBY",
//       "GBYGPOGR",
//       ...
//   });
inline BoardState parseBoard(const std::vector<std::string>& rows) {
    BoardState state;
    for (int row = 0; row < BoardState::ROWS; ++row) {
        for (int col = 0; col < BoardState::COLS; ++col) {
            char c = '.';
            if (row < static_cast<int>(rows.size()) && col < static_cast<int>(rows[row].size())) {
                c = rows[row][col];
            }
            switch (c) {
                case 'R': state.at(row, col) = GemType::RED; break;
                case 'G': state.at(row, col) = GemType::GREEN; break;
                case 'B': state.at(row, col) = GemType::BLUE; break;
                case 'Y': state.at(row, col) = GemType::YELLOW; break;
                case 'P': state.at(row, col) = GemType::PURPLE; break;
                case 'O': state.at(row, col) = GemType::ORANGE; break;
                case '.':
                default:  state.at(row, col) = GemType::EMPTY; break;
            }
        }
    }
    return state;
}

// Convert board state back to string for debugging
inline std::vector<std::string> boardToString(const BoardState& state) {
    std::vector<std::string> rows;
    for (int row = 0; row < BoardState::ROWS; ++row) {
        std::string line;
        for (int col = 0; col < BoardState::COLS; ++col) {
            switch (state.at(row, col)) {
                case GemType::RED:    line += 'R'; break;
                case GemType::GREEN:  line += 'G'; break;
                case GemType::BLUE:   line += 'B'; break;
                case GemType::YELLOW: line += 'Y'; break;
                case GemType::PURPLE: line += 'P'; break;
                case GemType::ORANGE: line += 'O'; break;
                case GemType::EMPTY:  line += '.'; break;
                default:              line += '?'; break;
            }
        }
        rows.push_back(line);
    }
    return rows;
}

// Create a deterministic gem factory from a sequence
// Cycles through the sequence repeatedly
inline GemFactory sequenceFactory(const std::vector<GemType>& sequence) {
    auto seq = std::make_shared<std::vector<GemType>>(sequence);
    auto index = std::make_shared<size_t>(0);
    return [seq, index](int, int) {
        GemType type = (*seq)[*index % seq->size()];
        (*index)++;
        return type;
    };
}

// Create a factory that returns a single type
inline GemFactory constantFactory(GemType type) {
    return [type](int, int) { return type; };
}

// Check if a position is in a list of positions
inline bool containsPosition(const std::vector<Position>& positions, int row, int col) {
    for (const auto& pos : positions) {
        if (pos.row == row && pos.col == col) {
            return true;
        }
    }
    return false;
}

// Create a board with no matches using a repeating pattern
// Pattern ensures no 3-in-a-row horizontally or vertically
inline BoardState noMatchBoard() {
    BoardState state;
    // Use pattern: RGBY repeating, offset each row
    GemType types[] = {GemType::RED, GemType::GREEN, GemType::BLUE, GemType::YELLOW};
    for (int row = 0; row < BoardState::ROWS; ++row) {
        for (int col = 0; col < BoardState::COLS; ++col) {
            // Offset by row to prevent vertical matches
            int idx = (col + row) % 4;
            state.at(row, col) = types[idx];
        }
    }
    return state;
}

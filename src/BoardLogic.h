#pragma once

#include "BoardTypes.h"
#include <vector>
#include <optional>

class BoardLogic {
public:
    explicit BoardLogic(GemFactory factory = nullptr);

    // Initialize board, avoiding initial matches
    void initializeBoard(BoardState& state) const;

    // Core game rules - pure functions operating on BoardState
    MatchResult checkMatches(const BoardState& state) const;
    GravityResult applyGravity(BoardState& state) const;
    void removeMatches(BoardState& state, const std::vector<Position>& positions) const;
    void fillEmpty(BoardState& state, const std::vector<Position>& positions) const;

    // Swap validation and execution
    bool isValidSwap(const BoardState& state, const Move& move) const;
    bool wouldCreateMatch(const BoardState& state, int row, int col, GemType type) const;
    void executeSwap(BoardState& state, const Move& move) const;

    // Check for valid moves remaining
    bool hasValidMoves(const BoardState& state) const;

    // Execute a complete sequence (swap -> matches -> gravity -> cascades)
    struct SequenceResult {
        bool swapValid = false;
        std::vector<MatchResult> matches;
        std::vector<GravityResult> gravities;
        int totalScore = 0;
    };
    SequenceResult executeSequence(BoardState& state, const Move& move) const;

private:
    GemFactory gemFactory;

    void findHorizontalMatches(const BoardState& state, int row, int col,
                               std::vector<Position>& matches) const;
    void findVerticalMatches(const BoardState& state, int row, int col,
                             std::vector<Position>& matches) const;
    bool areAdjacent(const Position& a, const Position& b) const;
    GemType getRandomGemType() const;
};

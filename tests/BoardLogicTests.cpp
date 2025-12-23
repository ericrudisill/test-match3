#include <catch2/catch_test_macros.hpp>
#include "BoardLogic.h"
#include "TestHelpers.h"

// ============================================================================
// Match Detection Tests
// ============================================================================

TEST_CASE("Horizontal match detection", "[matches]") {
    BoardLogic logic;

    SECTION("Detects 3-in-a-row horizontal") {
        auto state = noMatchBoard();
        // Add RRR at row 0, cols 0-2
        state.at(0, 0) = GemType::RED;
        state.at(0, 1) = GemType::RED;
        state.at(0, 2) = GemType::RED;

        auto result = logic.checkMatches(state);

        CHECK(result.matchedPositions.size() == 3);
        CHECK(containsPosition(result.matchedPositions, 0, 0));
        CHECK(containsPosition(result.matchedPositions, 0, 1));
        CHECK(containsPosition(result.matchedPositions, 0, 2));
        CHECK(result.score == 30);
    }

    SECTION("Detects 4-in-a-row horizontal") {
        auto state = noMatchBoard();
        // Use PURPLE which won't accidentally create vertical matches
        state.at(0, 0) = GemType::PURPLE;
        state.at(0, 1) = GemType::PURPLE;
        state.at(0, 2) = GemType::PURPLE;
        state.at(0, 3) = GemType::PURPLE;

        auto result = logic.checkMatches(state);

        CHECK(result.matchedPositions.size() == 4);
        CHECK(result.score == 40);
    }

    SECTION("No match for 2-in-a-row") {
        auto state = noMatchBoard();
        // Just 2 reds in a row - no match
        state.at(0, 0) = GemType::RED;
        state.at(0, 1) = GemType::RED;
        // Make sure col 2 is different
        state.at(0, 2) = GemType::PURPLE;

        auto result = logic.checkMatches(state);

        CHECK(result.matchedPositions.empty());
        CHECK(result.score == 0);
    }
}

TEST_CASE("Vertical match detection", "[matches]") {
    BoardLogic logic;

    SECTION("Detects 3-in-a-column vertical") {
        auto state = noMatchBoard();
        state.at(0, 0) = GemType::PURPLE;
        state.at(1, 0) = GemType::PURPLE;
        state.at(2, 0) = GemType::PURPLE;

        auto result = logic.checkMatches(state);

        CHECK(result.matchedPositions.size() == 3);
        CHECK(containsPosition(result.matchedPositions, 0, 0));
        CHECK(containsPosition(result.matchedPositions, 1, 0));
        CHECK(containsPosition(result.matchedPositions, 2, 0));
    }

    SECTION("Detects 4-in-a-column vertical") {
        auto state = noMatchBoard();
        state.at(0, 0) = GemType::PURPLE;
        state.at(1, 0) = GemType::PURPLE;
        state.at(2, 0) = GemType::PURPLE;
        state.at(3, 0) = GemType::PURPLE;

        auto result = logic.checkMatches(state);

        CHECK(result.matchedPositions.size() == 4);
    }
}

TEST_CASE("Combined matches", "[matches]") {
    BoardLogic logic;

    SECTION("L-shaped match counts all unique gems") {
        auto state = noMatchBoard();
        // Horizontal: row 0, cols 0-2
        state.at(0, 0) = GemType::PURPLE;
        state.at(0, 1) = GemType::PURPLE;
        state.at(0, 2) = GemType::PURPLE;
        // Vertical: rows 1-2, col 0 (extending from corner)
        state.at(1, 0) = GemType::PURPLE;
        state.at(2, 0) = GemType::PURPLE;

        auto result = logic.checkMatches(state);

        // 5 unique positions form L-shape
        CHECK(result.matchedPositions.size() == 5);
    }
}

// ============================================================================
// Gravity Tests
// ============================================================================

TEST_CASE("Gravity fills gaps correctly", "[gravity]") {
    BoardLogic logic;

    SECTION("Gems fall into empty spaces") {
        auto state = noMatchBoard();
        // Create a gap in column 0
        GemType top = state.at(0, 0);
        state.at(1, 0) = GemType::EMPTY;

        auto result = logic.applyGravity(state);

        CHECK(result.moves.size() == 1);
        CHECK(state.at(0, 0) == GemType::EMPTY);
        CHECK(state.at(1, 0) == top);
    }

    SECTION("No gaps - no movement") {
        auto state = noMatchBoard();

        auto result = logic.applyGravity(state);

        CHECK(result.moves.empty());
        CHECK(result.emptyPositions.empty());
    }

    SECTION("Multiple gaps in column consolidate") {
        BoardState state; // All empty
        state.at(0, 0) = GemType::RED;
        state.at(2, 0) = GemType::GREEN;
        state.at(4, 0) = GemType::BLUE;
        state.at(6, 0) = GemType::YELLOW;

        logic.applyGravity(state);

        CHECK(state.at(7, 0) == GemType::YELLOW);
        CHECK(state.at(6, 0) == GemType::BLUE);
        CHECK(state.at(5, 0) == GemType::GREEN);
        CHECK(state.at(4, 0) == GemType::RED);
        CHECK(state.at(3, 0) == GemType::EMPTY);
    }
}

// ============================================================================
// Remove Matches Tests
// ============================================================================

TEST_CASE("Remove matches clears gems", "[remove]") {
    BoardLogic logic;

    auto state = noMatchBoard();
    GemType preserved = state.at(0, 3);

    std::vector<Position> toRemove = {{0, 0}, {0, 1}, {0, 2}};
    logic.removeMatches(state, toRemove);

    CHECK(state.at(0, 0) == GemType::EMPTY);
    CHECK(state.at(0, 1) == GemType::EMPTY);
    CHECK(state.at(0, 2) == GemType::EMPTY);
    CHECK(state.at(0, 3) == preserved); // Unchanged
}

// ============================================================================
// Swap Validation Tests
// ============================================================================

TEST_CASE("Swap validation", "[swap]") {
    BoardLogic logic;
    auto state = noMatchBoard();

    SECTION("Adjacent horizontal swap is valid") {
        CHECK(logic.isValidSwap(state, {{0, 0}, {0, 1}}));
    }

    SECTION("Adjacent vertical swap is valid") {
        CHECK(logic.isValidSwap(state, {{0, 0}, {1, 0}}));
    }

    SECTION("Non-adjacent swap is invalid") {
        CHECK_FALSE(logic.isValidSwap(state, {{0, 0}, {0, 2}}));
        CHECK_FALSE(logic.isValidSwap(state, {{0, 0}, {2, 0}}));
        CHECK_FALSE(logic.isValidSwap(state, {{0, 0}, {1, 1}})); // Diagonal
    }

    SECTION("Out of bounds swap is invalid") {
        CHECK_FALSE(logic.isValidSwap(state, {{-1, 0}, {0, 0}}));
        CHECK_FALSE(logic.isValidSwap(state, {{0, 0}, {0, 8}}));
    }

    SECTION("Swap with empty cell is invalid") {
        state.at(0, 1) = GemType::EMPTY;
        CHECK_FALSE(logic.isValidSwap(state, {{0, 0}, {0, 1}}));
    }
}

TEST_CASE("Execute swap", "[swap]") {
    BoardLogic logic;
    auto state = noMatchBoard();

    GemType before00 = state.at(0, 0);
    GemType before01 = state.at(0, 1);

    logic.executeSwap(state, {{0, 0}, {0, 1}});

    CHECK(state.at(0, 0) == before01);
    CHECK(state.at(0, 1) == before00);
}

// ============================================================================
// Valid Moves Detection Tests
// ============================================================================

TEST_CASE("Valid moves detection", "[moves]") {
    BoardLogic logic;

    SECTION("Board with valid moves returns true") {
        auto state = noMatchBoard();
        // Set up so swapping creates a match: RR at (0,0-1), R at (1,2)
        // Swapping (0,2) with (1,2) creates RRR
        state.at(0, 0) = GemType::RED;
        state.at(0, 1) = GemType::RED;
        state.at(0, 2) = GemType::BLUE;
        state.at(1, 2) = GemType::RED;

        CHECK(logic.hasValidMoves(state));
    }

    SECTION("Alternating 3-color pattern with no valid moves") {
        // Use 3 colors in a pattern that prevents any swaps from creating matches
        // Pattern: RGB RGB RGB... offset each row
        BoardState state;
        GemType colors[] = {GemType::RED, GemType::GREEN, GemType::BLUE};
        for (int row = 0; row < BoardState::ROWS; ++row) {
            for (int col = 0; col < BoardState::COLS; ++col) {
                state.at(row, col) = colors[(col + row) % 3];
            }
        }

        CHECK_FALSE(logic.hasValidMoves(state));
    }
}

// ============================================================================
// Would Create Match Tests
// ============================================================================

TEST_CASE("Would create match detection", "[matches]") {
    BoardLogic logic;

    SECTION("Detects potential horizontal match") {
        auto state = noMatchBoard();
        state.at(0, 0) = GemType::EMPTY;
        state.at(0, 1) = GemType::RED;
        state.at(0, 2) = GemType::RED;

        CHECK(logic.wouldCreateMatch(state, 0, 0, GemType::RED));
        CHECK_FALSE(logic.wouldCreateMatch(state, 0, 0, GemType::GREEN));
    }

    SECTION("Detects potential vertical match") {
        auto state = noMatchBoard();
        state.at(0, 0) = GemType::EMPTY;
        state.at(1, 0) = GemType::RED;
        state.at(2, 0) = GemType::RED;

        CHECK(logic.wouldCreateMatch(state, 0, 0, GemType::RED));
    }
}

// ============================================================================
// Full Sequence Tests
// ============================================================================

TEST_CASE("Execute sequence", "[sequence]") {
    auto factory = sequenceFactory({
        GemType::PURPLE, GemType::ORANGE, GemType::YELLOW,
        GemType::GREEN, GemType::BLUE, GemType::PURPLE,
        GemType::ORANGE, GemType::YELLOW
    });
    BoardLogic logic(factory);

    SECTION("Valid swap creates match") {
        auto state = noMatchBoard();
        // Set up: RR at (1,0) and (2,0), G at (0,0)
        // Swapping (0,0)G with (1,0)R won't help
        // Instead: Set up RRR-1 vertically, swap to complete
        state.at(0, 0) = GemType::GREEN;
        state.at(1, 0) = GemType::RED;
        state.at(2, 0) = GemType::RED;
        state.at(0, 1) = GemType::RED; // Swap this down

        // Swap (0,0) with (0,1) then (0,1)R down to (1,1)?
        // Simpler: put RED at (0,1), (1,1), swap (2,1) to get 3
        state.at(0, 1) = GemType::PURPLE;
        state.at(1, 1) = GemType::PURPLE;
        state.at(2, 0) = GemType::PURPLE; // Now swap (2,0) with (2,1) to get PPP at col 1
        state.at(2, 1) = GemType::BLUE;

        Move move{{2, 0}, {2, 1}};
        auto result = logic.executeSequence(state, move);

        CHECK(result.swapValid == true);
        CHECK(result.matches.size() >= 1);
        CHECK(result.totalScore >= 30);
    }

    SECTION("Invalid swap reverts board") {
        auto state = noMatchBoard();
        GemType orig00 = state.at(0, 0);
        GemType orig01 = state.at(0, 1);

        Move move{{0, 0}, {0, 1}};
        auto result = logic.executeSequence(state, move);

        CHECK(result.swapValid == false);
        CHECK(result.totalScore == 0);
        CHECK(state.at(0, 0) == orig00);
        CHECK(state.at(0, 1) == orig01);
    }
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_CASE("Edge cases", "[edge]") {
    BoardLogic logic;

    SECTION("Empty board has no matches") {
        BoardState state; // All EMPTY by default
        auto result = logic.checkMatches(state);
        CHECK(result.matchedPositions.empty());
    }

    SECTION("Single gem type fills entire board - many matches") {
        BoardState state;
        for (int row = 0; row < BoardState::ROWS; ++row) {
            for (int col = 0; col < BoardState::COLS; ++col) {
                state.at(row, col) = GemType::RED;
            }
        }

        auto result = logic.checkMatches(state);
        CHECK(result.matchedPositions.size() == 64);
    }
}

// ============================================================================
// Board Initialization Tests
// ============================================================================

TEST_CASE("Board initialization avoids matches", "[init]") {
    BoardLogic logic;
    BoardState state;

    logic.initializeBoard(state);

    auto result = logic.checkMatches(state);
    CHECK(result.matchedPositions.empty());
}

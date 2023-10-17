#pragma once

#include <vector>
#include "Board.h"
#include "Player.h"
#include "Piece.h"

class Game {
    /// STRUCTS / ENUM
    enum GameState {IN_PROGRESS, DRAW, STALEMATE, WHITE_WIN, BLACK_WIN};
    struct castingAvailability {
        bool kingSide = true;
        bool queenSide = true;
    };

    /// DATA MEMBERS
    Board board{};
    GameState gameState {IN_PROGRESS};
    const Player player1 {Piece::Colour::WHITE};
    const Player player2 {Piece::Colour::BLACK};
    Location<> enPassantTargetSquare{};
    castingAvailability whiteCastingAvailability {true, true};
    castingAvailability blackCastingAvailability {true, true};
    Player activePlayer = player1;

    /// FRIENDS
    friend class GameController;

    /// CONSTRUCTORS and related
public:
    Game() = default;

    // Copy operator
    Game(const Game& other);

    // Copy assignment operator
    Game& operator=(const Game& other);
};


#pragma once

#include <vector>
#include "Board.h"
#include "Player.h"
#include "Piece.h"

class Game {
    /// STRUCTS / ENUM
public:
    enum GameState {IN_PROGRESS, DRAW, STALEMATE, WHITE_WIN, BLACK_WIN};
private:
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
    Game(const Game& other);
    Game& operator=(const Game& other);

    /// MISC.
    static std::string gameStateAsString(GameState gs);
    [[nodiscard]] Player getPlayerWithColour(Piece::Colour colour) const;
private:
    [[nodiscard]] static bool isValidPromotionPiece(const Piece*& promotionPiece, const Player& player);
};


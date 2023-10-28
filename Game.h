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
    struct castlingAvailability {
        bool kingSide = true;
        bool queenSide = true;
    };
    struct MoveInfo {
        const Location source;
        const Location destination;
        std::unique_ptr<Piece> promotionPiece;
    };
    /// DATA MEMBERS
    Board board{};
    GameState gameState {IN_PROGRESS};
    const Player whitePlayer {Piece::Colour::WHITE}; // TODO: const std::array<Player> players
    const Player blackPlayer {Piece::Colour::BLACK};
    Location enPassantTargetSquare{};
    castlingAvailability whiteCastlingAvailability {.kingSide = true, .queenSide = true}; // TODO: player has castling Availability
    castlingAvailability blackCastlingAvailability {.kingSide = true, .queenSide = true};
    Player activePlayer = whitePlayer;

    /// FRIENDS
    friend class GameController;

    /// CONSTRUCTORS and related
public:
    Game() = default;
    Game(const Game& other);
    Game& operator=(const Game& other);

    /// MISC.
    static std::string gameStateAsString(GameState gs);
};


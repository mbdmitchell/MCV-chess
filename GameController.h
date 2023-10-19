#pragma once

#include "Game.h"
#include "GameView.h"

using PieceFactory = std::function<std::unique_ptr<Piece>(Piece::Colour)>;

class GameController {

    /// DATA MEMBERS
    Game game;
    std::unique_ptr<GameView> gameView = std::make_unique<GameViewCLI>(); // Change if implementing other `GameView`s
    static const std::map<char, PieceFactory> pieceFactories;

    /// CONSTRUCTORS / OVERLOADS
public:
    GameController() = default;
    GameController(const GameController& rhs); // Change if implementing other `GameView`s
    GameController& operator=(const GameController& rhs);

    /// SETUP
    void setup();
    void manualSetup();
    void setupSimple(); // TODO: remove from public API

    /// MISC.
    void submitMove(const Location<> &source, const Location<> &destination);
    void initGameLoop();

private:

    /// VALIDATION
    [[nodiscard]] bool isValidMove(Piece::Colour playerColour, const Location<> &source, const Location<> &destination) const;
    [[nodiscard]] bool isValidCastling(const Location<> &source, const Location<> &destination) const;

    /// MOVE TYPE
    [[nodiscard]] bool isCastlingAttempt(const Location<> &source, const Location<> &destination) const;
    [[nodiscard]] bool isEnPassant(const Location<> &source, const Location<> &destination) const;

    /// CHECK
    [[nodiscard]] bool inCheck(const Player& player) const;
    [[nodiscard]] bool moveLeavesMoverInCheck(const Location<> &source, const Location<> &destination) const;

    /// MANIPULATE GAME / BOARD
    void makeMove(const Location<> &source, const Location<> &destination);

    void setEnPassantTargetSquare(const Location<> &source, const Location<> &destination);
    void updateCastingAvailability(gsl::not_null<Piece*> pieceMoved, const Location<> &source);
    void handleRookCastlingMove(const Location<> &destination);

    void swapActivePlayer();

    /// GET / CALCULATE
    [[nodiscard]] Location<> getLocationOfKing(Piece::Colour kingColour) const;
    [[nodiscard]] Game::GameState calculateGameState() const;
    [[nodiscard]] bool isUnderAttackBy(Location<> location, const Piece::Colour& opponentsColour) const;
    [[nodiscard]] bool thereExistsValidMove(const Player& activePlayer) const;

    /// MISC.
    static std::map<char, PieceFactory> createPieceFactories();
};



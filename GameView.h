#pragma once

#include "Board.h"
#include "ErrorLogger.h"

class GameView {
public:
    virtual void viewBoard(const Board& b) const = 0;
    virtual void viewPiece(gsl::not_null<const Piece*> piece) const = 0;
    virtual ~GameView() = default;
    [[nodiscard]] virtual std::string readInput(std::string_view message) const = 0;
    virtual void logError(const Error::Type& error) const = 0;
    virtual void logException(const std::exception& e) const = 0;
};

class GameViewCLI : public GameView {
public:
    void viewBoard(const Board &b) const override;
    void viewPiece(const gsl::not_null<const Piece*> piece) const override;
    [[nodiscard]] std::string readInput(std::string_view message) const override;
    void logError(const Error::Type& error) const override;

    void logException(const std::exception& e) const override;
};


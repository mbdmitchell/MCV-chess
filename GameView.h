#pragma once

#include "Board.h"

class GameView {
public:
    virtual void viewBoard(const Board& b) const = 0;
    virtual void viewPiece(gsl::not_null<const Piece*> piece) const = 0;
    virtual ~GameView() = default;
};

class GameViewCLI : public GameView {
public:
    void viewBoard(const Board &b) const override {

        const auto& board = b.board;

        for (gsl::index row = Location<>::getMaxRowIndex(); row != std::numeric_limits<size_t>::max(); --row) {
            for (Location location {row,0}; location <= Location{row,Location<>::getMaxColumnIndex()}; ++location) {

                if (board.contains(location)) {
                    const gsl::not_null<Piece*> piece = board.at(location).get();
                    viewPiece(piece);
                } else {
                    std::cout << '.';
                }

                std::cout << ' ';
                if (Location<>::getMaxColumnIndex() == location.getBoardColumnIndex()) {
                    std::cout << '\n';
                }

            }
        }

    }
    void viewPiece(const gsl::not_null<const Piece*> piece) const override {
        std::cout << static_cast<char>(*piece);
    }
};
#pragma once

#include "Piece.h"

class Player {
    Piece::Colour colour;
public:
    explicit Player(Piece::Colour colour);

    Player(const Player& other);

    [[nodiscard]] Piece::Colour getColour() const;
};

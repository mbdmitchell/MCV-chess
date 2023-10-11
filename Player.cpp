#include "Player.h"

Player::Player(Piece::Colour colour) : colour{colour} {}

Player::Player(const Player &other)
        : colour{other.colour} { }

Piece::Colour Player::getColour() const { return colour; }




#include "Player.h"

Player::Player(Piece::Colour colour) : colour{colour} {}
Player::Player(const Player &other) = default;

Piece::Colour Player::getColour() const { return colour; }




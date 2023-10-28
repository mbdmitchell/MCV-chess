#include "Piece.h"

/// PIECE

Piece::Piece(Piece::Colour colour) : colour{colour} {}

Piece::Colour Piece::getColour() const noexcept {return colour; }

/// PAWN

Pawn::Pawn(Piece::Colour colour) : Piece(colour) { }

Pawn::operator char() const noexcept {
    char sprite = 'p';
    return ((getColour() == Piece::Colour::WHITE) ? toupper(sprite, std::locale()) : sprite);
}

bool Pawn::isValidMovePath(const Location &source,
                           const Location &destination,
                           const Location &enPassantTargetSquare,
                           const bool isCapture) const noexcept
{
    const bool isMoveForward = (getColour() == Piece::Colour::WHITE);
    const bool isMovingInRightDirection = (isMoveForward == (getColour() == Piece::Colour::WHITE));

    if (source == destination || !isMovingInRightDirection) return false;

    if (!(Location::isVertical(source, destination) || Location::isDiagonal(source, destination))){
        return false;
    }

    auto& [row, col] = source;
    const auto [deltaRow, deltaColumn] = Location::calculateRowColumnDifferences(source, destination);

    if (Location::isVertical(source, destination)) {
        if (isCapture) {
            return false;
        }
        if (abs(deltaRow) == 1) {
            return true;
        }
        else if (abs(deltaRow) == 2) {
            const gsl::index startingRowIndexWhite = 1;
            const gsl::index startingRowIndexBlack = 6;
            return ((row == startingRowIndexWhite && getColour() == Piece::Colour::WHITE) || (row == startingRowIndexBlack && getColour() == Piece::Colour::BLACK));
        }
        return false;
    }

    // Location::isDiagonal(source, destination)) == true
    const bool isEnPassant = (enPassantTargetSquare == Location{source.getBoardRowIndex().value(), destination.getBoardColumnIndex().value()});
    return isEnPassant || (isCapture && abs(deltaRow) == 1);
}

std::unique_ptr<Piece> Pawn::clone() const noexcept {
    return std::make_unique<Pawn>(*this);
}

/// BISHOP

Bishop::Bishop(Piece::Colour colour) : Piece(colour) { }

Bishop::operator char() const noexcept {
    char sprite = 'b';
    return ((getColour() == Piece::Colour::WHITE) ? toupper(sprite, std::locale()) : sprite);
}

bool Bishop::isValidMovePath(const Location &source,
                             const Location &destination,
                             const Location &/*enPassantTargetSquare*/,
                             bool /*isCapture*/) const noexcept {
    return Location::isDiagonal(source, destination);
}

std::unique_ptr<Piece> Bishop::clone() const noexcept {
    return std::make_unique<Bishop>(*this);
}

/// KNIGHT

Knight::Knight(Piece::Colour colour) : Piece(colour) { }

Knight::operator char() const noexcept {
    char sprite = 'n';
    return ((getColour() == Piece::Colour::WHITE) ? toupper(sprite, std::locale()) : sprite);
}

bool Knight::isValidMovePath(const Location &source,
                             const Location &destination,
                             const Location &/*enPassantTargetSquare*/,
                             bool /*isCapture*/) const noexcept {
    return Location::isKnightMove(source, destination);
}

std::unique_ptr<Piece> Knight::clone() const noexcept {
    return std::make_unique<Knight>(*this);
}

Rook::Rook(Piece::Colour colour) : Piece(colour) { }

Rook::operator char() const noexcept {
    char sprite = 'r';
    return ((getColour() == Piece::Colour::WHITE) ? toupper(sprite, std::locale()) : sprite);
}

bool Rook::isValidMovePath(const Location &source,
                           const Location &destination,
                           const Location &/*enPassantTargetSquare*/,
                           bool /*isCapture*/) const noexcept {
    return (Location::isHorizontal(source, destination)
        || Location::isVertical(source, destination));
}

std::unique_ptr<Piece> Rook::clone() const noexcept {
    return std::make_unique<Rook>(*this);
}

/// QUEEN

Queen::Queen(Piece::Colour colour) : Piece(colour) { }

Queen::operator char() const noexcept {
    char sprite = 'q';
    return ((getColour() == Piece::Colour::WHITE) ? toupper(sprite, std::locale()) : sprite);
}

bool Queen::isValidMovePath(const Location &source,
                            const Location &destination,
                            const Location &/*enPassantTargetSquare*/,
                            bool /*isCapture*/) const noexcept {
    return (Location::isVertical(source, destination)
        ||  Location::isDiagonal(source, destination)
        ||  Location::isHorizontal(source, destination)
    );
}

std::unique_ptr<Piece> Queen::clone() const noexcept {
    return std::make_unique<Queen>(*this);
}

/// KING

King::King(Piece::Colour colour) : Piece(colour) { }

King::operator char() const noexcept {
    char sprite = 'k';
    return ((getColour() == Piece::Colour::WHITE) ? toupper(sprite, std::locale()) : sprite);
}

[[nodiscard]] bool King::isValidCastlingPath(const Location& source, const Location& destination) noexcept {
    const auto [rowDifference, columnDifference] { Location::calculateRowColumnDifferences(source, destination) };
    return ((rowDifference == 0
             && abs(columnDifference) == 2
             && (source == Location{"E1"} || source == Location{"E8"})));
}
bool King::isValidMovePath(const Location &source,
                           const Location &destination,
                           const Location &/*enPassantTargetSquare*/,
                           bool /*isCapture*/) const noexcept {

    const auto& [rowDiff, colDiff] = Location::calculateRowColumnDifferences(source, destination);
    return std::max(abs(rowDiff), abs(colDiff)) == 1 || isValidCastlingPath(source, destination);
}

std::unique_ptr<Piece> King::clone() const noexcept {
    return std::make_unique<King>(*this);
}

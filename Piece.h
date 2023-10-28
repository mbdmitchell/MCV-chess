#pragma once

#include <stdexcept>
#include <locale>
#include "Location.h"

// NB: Included derived Piece classes here to speed up build-time on the *incredibly* slow machine I'm currently using
class Piece {
    /// ENUMS / STRUCTS
public:
    enum class Colour {WHITE, BLACK};
    /// DATA MEMBERS
protected:
    Colour colour;
public:
    /// CONSTRUCTORS / DESTRUCTORS
    explicit Piece(Colour colour);
    virtual ~Piece() = default;

    /// GETTERS
    [[nodiscard]] Colour getColour() const noexcept;

    /// OPERATORS
    [[nodiscard]] virtual explicit operator char() const noexcept = 0;

    /// VALIDATION.
    [[nodiscard]] virtual bool isValidMovePath(const Location &source,
                                               const Location &destination,
                                               const Location &enPassantTargetSquare,
                                               bool isCapture) const noexcept = 0;

    /// MISC.
    [[nodiscard]] virtual std::unique_ptr<Piece> clone() const noexcept = 0;
};

class Pawn : public Piece {
public:
    /// CONSTRUCTOR
    explicit Pawn(Colour colour);

    /// OPERATORS
    [[nodiscard]] explicit operator char() const noexcept override;

    /// VALIDATION
    [[nodiscard]] bool isValidMovePath(const Location &source,
                                       const Location &destination,
                                       const Location &enPassantTargetSquare,
                                       bool isCapture) const noexcept override;

    /// MISC.
    [[nodiscard]] std::unique_ptr<Piece> clone() const noexcept override;
};


class Bishop : public Piece {
public:
    /// CONSTRUCTOR
    explicit Bishop(Colour colour);

    /// OPERATORS
    [[nodiscard]] explicit operator char() const noexcept override;
    /// VALIDATION
    [[nodiscard]] bool isValidMovePath(const Location &source,
                                       const Location &destination,
                                       const Location &/*enPassantTargetSquare*/,
                                       bool /*isCapture*/) const noexcept override;

    /// MISC.
    [[nodiscard]] std::unique_ptr<Piece> clone() const noexcept override;
};


class Knight : public Piece {
public:
    /// CONSTRUCTOR
    explicit Knight(Colour colour);

    /// OPERATORS
    [[nodiscard]] explicit operator char() const noexcept override;
    /// VALIDATION
    [[nodiscard]] bool isValidMovePath(const Location &source,
                                       const Location &destination,
                                       const Location &/*enPassantTargetSquare*/,
                                       bool /*isCapture*/) const noexcept override;

    /// MISC.
    [[nodiscard]] std::unique_ptr<Piece> clone() const noexcept override;
};

class Rook : public Piece {
public:
    /// CONSTRUCTOR
    explicit Rook(Colour colour);

    /// OPERATORS
    [[nodiscard]] explicit operator char() const noexcept override;
    /// VALIDATION
    [[nodiscard]] bool isValidMovePath(const Location &source,
                                       const Location &destination,
                                       const Location &/*enPassantTargetSquare*/,
                                       bool /*isCapture*/) const noexcept override;

    /// MISC.
    [[nodiscard]] std::unique_ptr<Piece> clone() const noexcept override;
};


class Queen : public Piece {
public:
    /// CONSTRUCTOR
    explicit Queen(Colour colour);

    /// OPERATORS
    [[nodiscard]] explicit operator char() const noexcept override;
    /// VALIDATION
    [[nodiscard]] bool isValidMovePath(const Location &source,
                                       const Location &destination,
                                       const Location &/*enPassantTargetSquare*/,
                                       bool /*isCapture*/) const noexcept override;

    /// MISC.
    [[nodiscard]] std::unique_ptr<Piece> clone() const noexcept override;
};

class King : public Piece {
public:
    /// CONSTRUCTOR
    explicit King(Colour colour);

    /// OPERATORS
    [[nodiscard]] explicit operator char() const noexcept override;
    /// VALIDATION
    [[nodiscard]] bool isValidMovePath(const Location &source,
                                       const Location &destination,
                                       const Location &/*enPassantTargetSquare*/,
                                       bool /*isCapture*/) const noexcept override;
    [[nodiscard]] static bool isValidCastlingPath(const Location &source, const Location &destination) noexcept;
    /// MISC.
    [[nodiscard]] std::unique_ptr<Piece> clone() const noexcept override;
};
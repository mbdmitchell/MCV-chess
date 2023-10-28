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
    [[nodiscard]] Colour getColour() const;

    /// OPERATORS
    [[nodiscard]] virtual explicit operator char() const = 0;

    /// VALIDATION.
    [[nodiscard]] virtual bool isValidMovePath(const Location &source,
                                               const Location &destination,
                                               const Location &enPassantTargetSquare,
                                               bool isCapture) const = 0;

    /// MISC.
    [[nodiscard]] virtual std::unique_ptr<Piece> clone() const = 0;
};

class Pawn : public Piece {
public:
    /// CONSTRUCTOR
    explicit Pawn(Colour colour);

    /// OPERATORS
    [[nodiscard]] explicit operator char() const override;

    /// VALIDATION
    [[nodiscard]] bool isValidMovePath(const Location &source,
                                       const Location &destination,
                                       const Location &enPassantTargetSquare,
                                       bool isCapture) const override;

    /// MISC.
    [[nodiscard]] std::unique_ptr<Piece> clone() const override;
};


class Bishop : public Piece {
public:
    /// CONSTRUCTOR
    explicit Bishop(Colour colour);

    /// OPERATORS
    [[nodiscard]] explicit operator char() const override;
    /// VALIDATION
    [[nodiscard]] bool isValidMovePath(const Location &source,
                                       const Location &destination,
                                       const Location &enPassantTargetSquare,
                                       bool isCapture) const override;

    /// MISC.
    [[nodiscard]] std::unique_ptr<Piece> clone() const override;
};


class Knight : public Piece {
public:
    /// CONSTRUCTOR
    explicit Knight(Colour colour);

    /// OPERATORS
    [[nodiscard]] explicit operator char() const override;
    /// VALIDATION
    [[nodiscard]] bool isValidMovePath(const Location &source,
                                       const Location &destination,
                                       const Location &enPassantTargetSquare,
                                       bool isCapture) const override;

    /// MISC.
    [[nodiscard]] std::unique_ptr<Piece> clone() const override;
};

class Rook : public Piece {
public:
    /// CONSTRUCTOR
    explicit Rook(Colour colour);

    /// OPERATORS
    [[nodiscard]] explicit operator char() const override;
    /// VALIDATION
    [[nodiscard]] bool isValidMovePath(const Location &source,
                                       const Location &destination,
                                       const Location &enPassantTargetSquare,
                                       bool isCapture) const override;

    /// MISC.
    [[nodiscard]] std::unique_ptr<Piece> clone() const override;
};


class Queen : public Piece {
public:
    /// CONSTRUCTOR
    explicit Queen(Colour colour);

    /// OPERATORS
    [[nodiscard]] explicit operator char() const override;
    /// VALIDATION
    [[nodiscard]] bool isValidMovePath(const Location &source,
                                       const Location &destination,
                                       const Location &enPassantTargetSquare,
                                       bool isCapture) const override;

    /// MISC.
    [[nodiscard]] std::unique_ptr<Piece> clone() const override;
};

class King : public Piece {
public:
    /// CONSTRUCTOR
    explicit King(Colour colour);

    /// OPERATORS
    [[nodiscard]] explicit operator char() const override;
    /// VALIDATION
    [[nodiscard]] bool isValidMovePath(const Location &source,
                                       const Location &destination,
                                       const Location &enPassantTargetSquare,
                                       bool isCapture) const override;
    [[nodiscard]] static bool isValidCastlingPath(const Location &source, const Location &destination);
    /// MISC.
    [[nodiscard]] std::unique_ptr<Piece> clone() const override;
};
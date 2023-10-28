#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection" // silence clang warnings about unused functions as it mislabels many `is move shape` functions 
#pragma once

#include <string>
#include <compare>
#include "format"
#include <iostream>
#include "gsl/gsl"

class Location {

    const static gsl::index maxRowIndex = 7;
    const static gsl::index maxColumnIndex = 7;

    /// DATA MEMBERS
    /* Imagine like board[row][col], even if board is actually a map)
        0 1 2 3 4 5 6 7
      0 . . . . . . . .
      1 . . . . . . . .
      2 . . . . . . . .
      ...
      7 . . . . . . . .

     (0,7) = A1; (1,7) = B1
    */
    std::optional<gsl::index> boardRowIndex; // std::optional to allow for null locations
    std::optional<gsl::index> boardColumnIndex;

    /// STRUCTS
public:
    struct RowColumnDifferences {
        gsl::index rowDifference, columnDifference;
    };

private:
    struct Indices {
        gsl::index destColumn, destRow, sourceColumn, sourceRow;
    };

    /// CONSTRUCTORS
public:

    Location() = default; // Null location
    Location(gsl::index row, gsl::index col);
    explicit Location(std::string_view str); // Chess notation -> Coordinates (eg. "A2" -> 1,0 )

    /// GETTERS

    [[nodiscard]] static constexpr gsl::index getMaxColumnIndex() noexcept { return maxColumnIndex; }
    [[nodiscard]] static constexpr gsl::index getMaxRowIndex() noexcept { return maxRowIndex; }
    [[nodiscard]] auto getBoardRowIndex() const noexcept { return boardRowIndex; }
    [[nodiscard]] auto getBoardColumnIndex() const noexcept { return boardColumnIndex; }

    /// ... For structured bindings
    template<size_t I>
    [[nodiscard]] auto get() const;

    /// OPERATORS

    explicit operator std::string() const noexcept;

    std::strong_ordering operator<=>(const Location& other) const;
    bool operator==(const Location& other) const;

    Location& operator++();
    const Location operator++(int);

    /// VALIDATION
private:
    [[nodiscard]] bool isValid() const noexcept;
    /// LOCATION-to-LOCATION RELATIONSHIP FUNCTIONS
private:
    [[nodiscard]] static RowColumnDifferences calculateRowColumnDifferences(const Indices& indices) noexcept;
public:
    [[nodiscard]] static RowColumnDifferences calculateRowColumnDifferences(const Location& source, const Location& destination) noexcept;
    [[nodiscard]] static gsl::index maxAbsoluteRowColumnDifference(const Location& source, const Location& destination) noexcept;

    ///... `is move shape` functions
    [[nodiscard]] static bool isDiagonal(const Location& source, const Location& destination) noexcept;
    [[nodiscard]] static bool isHorizontal(const Location& source, const Location& destination) noexcept;
    [[nodiscard]] static bool isVertical(const Location& source, const Location& destination) noexcept;
    [[nodiscard]] static bool isKnightMove(const Location& source, const Location& destination) noexcept;
    [[nodiscard]] static bool isForwardMove(const Location& source, const Location& destination) noexcept;
    
    /// MISC.
private:
    [[nodiscard]] static Indices calculateIndices(const Location& source, const Location& destination) noexcept;
};

/// ... For structured bindings
namespace std {
    // tells the compiler that Location should be treated as if it were a tuple-like structure with two elements.
    template<>
    struct tuple_size<Location>
            : std::integral_constant<std::size_t, 2> { };

    template<std::size_t N>
    struct tuple_element<N, Location> {
        using type = decltype(std::declval<Location>().get<N>());
    };
}

template<size_t I>
auto Location::get() const {
    if constexpr (I == 0) return boardRowIndex;
    else if constexpr (I == 1) return boardColumnIndex;
}

#pragma clang diagnostic pop

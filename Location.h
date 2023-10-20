#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma once

#include <string>
#include <compare>
#include "format"
#include <iostream>
#include "gsl/gsl"

// This is a template to make class more generic and remove magic
// numbers regarding board size from prefix/postfix operator++
template<gsl::index maxRowIndex = 7, gsl::index maxColumnIndex = 7>
class Location {

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

    [[nodiscard]] static constexpr gsl::index getMaxColumnIndex() { return maxColumnIndex; }
    [[nodiscard]] static constexpr gsl::index getMaxRowIndex() { return maxRowIndex; }
    [[nodiscard]] auto getBoardRowIndex() const { return boardRowIndex; }
    [[nodiscard]] auto getBoardColumnIndex() const { return boardColumnIndex; }

    /// ... For structured bindings
    template<size_t I>
    [[nodiscard]] auto get() const;

    /// OPERATORS

    explicit operator std::string() const;

    auto operator<=>(const Location& other) const;
    bool operator==(const Location& other) const;

    Location& operator++();
    const Location operator++(int);

    /// VALIDATION
private:
    [[nodiscard]] bool isValid() const;
    /// LOCATION-to-LOCATION RELATIONSHIP FUNCTIONS
private:
    [[nodiscard]] static RowColumnDifferences calculateRowColumnDifferences(const Indices& indices);
public:
    [[nodiscard]] static RowColumnDifferences calculateRowColumnDifferences(const Location& source, const Location& destination);

    [[nodiscard]] static bool isDiagonal(const Location& source, const Location& destination);
    [[nodiscard]] static bool isHorizontal(const Location& source, const Location& destination);
    [[nodiscard]] static bool isVertical(const Location& source, const Location& destination);
    [[nodiscard]] static bool isKnightMove(const Location& source, const Location& destination);
    [[nodiscard]] static bool isForwardMove(const Location& source, const Location& destination);
    [[nodiscard]] static long long maxAbsoluteRowColumnDifference(const Location& source, const Location& destination);

    /// MISC.
private:
    [[nodiscard]] static Indices calculateIndices(const Location& source, const Location& destination);
};

    /// ... For structured bindings
namespace std {
    // tells the compiler that Location should be treated as if it were a tuple-like structure with two elements.
    template<>
    struct tuple_size<Location<>>
            : std::integral_constant<std::size_t, 2> { };

    template<std::size_t N>
    struct tuple_element<N, Location<>> {
        using type = decltype(std::declval<Location<>>().get<N>());
    };
}


/// DEFINITIONS
template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
Location<maxRowIndex, maxColumnIndex>::Indices
Location<maxRowIndex, maxColumnIndex>::calculateIndices(const Location &source, const Location &destination) {
    return {destination.boardColumnIndex.value(),
            destination.boardRowIndex.value(),
            source.boardColumnIndex.value(),
            source.boardRowIndex.value()};
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
long long Location<maxRowIndex, maxColumnIndex>::maxAbsoluteRowColumnDifference(const Location &source,
                                                                                const Location &destination) {
    const auto differences = Location::calculateRowColumnDifferences(source, destination);
    return (std::max(abs(differences.columnDifference), abs(differences.rowDifference)));
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
bool Location<maxRowIndex, maxColumnIndex>::isForwardMove(const Location &source, const Location &destination) {
    return destination.boardRowIndex.value() > source.boardRowIndex.value();
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
bool Location<maxRowIndex, maxColumnIndex>::isKnightMove(const Location &source, const Location &destination) {
    const auto differences = Location::calculateRowColumnDifferences(source, destination);
    return (abs(differences.columnDifference) == 2 && abs(differences.rowDifference) == 1)
           || (abs(differences.columnDifference) == 1 && abs(differences.rowDifference) == 2);
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
bool Location<maxRowIndex, maxColumnIndex>::isVertical(const Location &source, const Location &destination) {
    const auto differences = Location::calculateRowColumnDifferences(source, destination);
    return differences.rowDifference != 0 &&  differences.columnDifference == 0;
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
bool Location<maxRowIndex, maxColumnIndex>::isHorizontal(const Location &source, const Location &destination) {
    const auto differences = Location::calculateRowColumnDifferences(source, destination);
    return differences.rowDifference == 0 &&  differences.columnDifference != 0;
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
bool Location<maxRowIndex, maxColumnIndex>::isDiagonal(const Location &source, const Location &destination) {
    const auto differences = Location::calculateRowColumnDifferences(source, destination);
    return abs(differences.columnDifference) == abs(differences.rowDifference);
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
Location<maxRowIndex, maxColumnIndex>::RowColumnDifferences
Location<maxRowIndex, maxColumnIndex>::calculateRowColumnDifferences(const Location &source,
                                                                     const Location &destination) {
    Indices indices = calculateIndices(source, destination);
    return calculateRowColumnDifferences(indices);
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
Location<maxRowIndex, maxColumnIndex>::RowColumnDifferences
Location<maxRowIndex, maxColumnIndex>::calculateRowColumnDifferences(const Location::Indices &indices) {
    return {indices.destRow - indices.sourceRow, indices.destColumn - indices.sourceColumn};
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
bool Location<maxRowIndex, maxColumnIndex>::isValid() const {
    return boardRowIndex <= maxRowIndex || boardColumnIndex <= maxColumnIndex;
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
const Location<maxRowIndex, maxColumnIndex> Location<maxRowIndex, maxColumnIndex>::operator++(int) { // (obj++)
    Location copy {*this};
    ++(*this);
    return copy;
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
Location<maxRowIndex, maxColumnIndex> &Location<maxRowIndex, maxColumnIndex>::operator++() { // (++obj)
    if (boardColumnIndex < maxColumnIndex) {
        *this = Location{this->getBoardRowIndex().value(), this->getBoardColumnIndex().value() + 1};
    } else {
        *this = Location{this->getBoardRowIndex().value() + 1, 0};
    }
    return *this;
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
bool Location<maxRowIndex, maxColumnIndex>::operator==(const Location &other) const {
    return (boardRowIndex == other.boardRowIndex
            && boardColumnIndex == other.boardColumnIndex);
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
auto Location<maxRowIndex, maxColumnIndex>::operator<=>(const Location &other) const { // TODO: try getting it working as `auto operator<=>(const Location& other) = default;` instead
    if (boardRowIndex < other.boardRowIndex){
        return std::strong_ordering::less;
    }
    else if (boardRowIndex > other.boardRowIndex) {
        return std::strong_ordering::greater;
    }
    else {
        if (boardColumnIndex < other.boardColumnIndex){
            return std::strong_ordering::less;
        }
        else if (boardColumnIndex > other.boardColumnIndex) {
            return std::strong_ordering::greater;
        }
        else {
            return std::strong_ordering::equal;
        }
    }
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
Location<maxRowIndex, maxColumnIndex>::operator std::string() const {
    if (boardRowIndex.has_value() && boardColumnIndex.has_value()) {
        return std::format("({}, {})", boardRowIndex.value(), boardColumnIndex.value());
    }
    else if (boardRowIndex.has_value()) {
        return std::format("({}, {})", boardRowIndex.value(), "NULL");
    }
    else if (boardColumnIndex.has_value()) {
        return std::format("({}, {})", "NULL", boardColumnIndex.value());
    } else {
        return "(NULL, NULL)";
    }
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
template<size_t I>
auto Location<maxRowIndex, maxColumnIndex>::get() const {
    if constexpr (I == 0) return boardRowIndex;
    else if constexpr (I == 1) return boardColumnIndex;
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
Location<maxRowIndex, maxColumnIndex>::Location(std::string_view str) // TODO: exception to handle str too short
        : Location(static_cast<gsl::index>((str[1] - '0' - 1)), static_cast<gsl::index>(str[0] - 'A')) {
    if (str[1] - '0' - 1 < 0) {
        throw std::invalid_argument("Invalid Argument passed into Location(std::string_view str))");
    }
}

template<gsl::index maxRowIndex, gsl::index maxColumnIndex>
Location<maxRowIndex, maxColumnIndex>::Location(gsl::index row, gsl::index col) : boardRowIndex{row}, boardColumnIndex{col} {
    if (!isValid()) {
        throw std::invalid_argument("Invalid Argument passed into Location(gsl::index row, gsl::index col)");
    }
}
#pragma clang diagnostic pop
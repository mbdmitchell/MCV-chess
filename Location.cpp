#include "Location.h"

const gsl::index Location::maxRowIndex;
const gsl::index Location::maxColumnIndex;

Location::Indices Location::calculateIndices(const Location &source, const Location &destination) {
    return {destination.boardColumnIndex.value(),
            destination.boardRowIndex.value(),
            source.boardColumnIndex.value(),
            source.boardRowIndex.value()};
}

gsl::index Location::maxAbsoluteRowColumnDifference(const Location &source, const Location &destination) {
    const auto differences = Location::calculateRowColumnDifferences(source, destination);
    return (std::max(abs(differences.columnDifference), abs(differences.rowDifference)));
}

bool Location::isForwardMove(const Location &source, const Location &destination) {
    return destination.boardRowIndex.value() > source.boardRowIndex.value();
}

bool Location::isKnightMove(const Location &source, const Location &destination) {
    const auto differences = Location::calculateRowColumnDifferences(source, destination);
    return (abs(differences.columnDifference) == 2 && abs(differences.rowDifference) == 1)
           || (abs(differences.columnDifference) == 1 && abs(differences.rowDifference) == 2);
}

bool Location::isVertical(const Location &source, const Location &destination) {
    const auto differences = Location::calculateRowColumnDifferences(source, destination);
    return differences.rowDifference != 0 &&  differences.columnDifference == 0;
}

bool Location::isHorizontal(const Location &source, const Location &destination) {
    const auto differences = Location::calculateRowColumnDifferences(source, destination);
    return differences.rowDifference == 0 &&  differences.columnDifference != 0;
}

bool Location::isDiagonal(const Location &source, const Location &destination) {
    const auto differences = Location::calculateRowColumnDifferences(source, destination);
    return abs(differences.columnDifference) == abs(differences.rowDifference);
}

Location::RowColumnDifferences Location::calculateRowColumnDifferences(const Location &source, const Location &destination) {
    Indices indices = calculateIndices(source, destination);
    return calculateRowColumnDifferences(indices);
}

Location::RowColumnDifferences Location::calculateRowColumnDifferences(const Location::Indices &indices) {
    return {indices.destRow - indices.sourceRow, indices.destColumn - indices.sourceColumn};
}

bool Location::isValid() const {
    return boardRowIndex <= maxRowIndex || boardColumnIndex <= maxColumnIndex;
}

const Location Location::operator++(int) { // (obj++)
    Location copy {*this};
    ++(*this);
    return copy;
}

Location& Location::operator++() { // (++obj)
    if (boardColumnIndex < maxColumnIndex) {
        *this = Location{this->getBoardRowIndex().value(), this->getBoardColumnIndex().value() + 1};
    } else {
        *this = Location{this->getBoardRowIndex().value() + 1, 0};
    }
    return *this;
}

bool Location::operator==(const Location &other) const {
    return (boardRowIndex == other.boardRowIndex
            && boardColumnIndex == other.boardColumnIndex);
}

Location::operator std::string() const {
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

Location::Location(std::string_view str) // TODO: exception to handle str too short
        : Location(static_cast<gsl::index>((str[1] - '0' - 1)), static_cast<gsl::index>(str[0] - 'A')) {
    if (str[1] - '0' - 1 < 0) {
        throw std::invalid_argument("Invalid Argument passed into Location(std::string_view str))");
    }
}

Location::Location(gsl::index row, gsl::index col) : boardRowIndex{row}, boardColumnIndex{col} {
    if (!isValid()) {
        throw std::invalid_argument("Invalid Argument passed into Location(gsl::index row, gsl::index col)");
    }
}

std::strong_ordering Location::operator<=>(const Location &other) const { // TODO: try getting it working as `auto operator<=>(const Location& other) = default;` instead
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
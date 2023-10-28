#include "GameController.h"

// TODO: `noexcept` all the things!
// TODO: `final` things that should be `final`

/* -----------------------------------------------------------------------------

IDEA - probably won't implement but thought it was fun

struct SimpleMove {Location source, Location destination};
struct CaptureMove {Location source, Location destination, bool enPassant};
struct CastleMove {bool isKingside};

class Move {
    std::variant<SimpleMove, CaptureMove, CastleMove> details;
public:
    bool isSimpleMove() const {return details.index == 0;}
    bool isCapturingMove() const {return details.index == 1;}
    bool isCastlingMove() const {return details.index == 2;}
};

----------------------------------------------------------------------------- */

int main() {
    GameController g{};
    g.setup();
    g.initGameLoop();
    return EXIT_SUCCESS;
}

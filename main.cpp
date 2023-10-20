#include "GameController.h"

// TODO: Idea: instead of unique_ptr's, construct needed objects in main() and classes have non-owning const references
int main() {
    GameController g{};
    g.setupSimple();
    g.initGameLoop();
    return EXIT_SUCCESS;
}

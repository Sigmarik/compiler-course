#include "ast.hpp"

Sequence::~Sequence() {
    for (Action* action : actions) delete action;
}

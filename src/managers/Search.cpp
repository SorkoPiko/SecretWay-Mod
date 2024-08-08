#include "Search.hpp"

bool Search::on = false;

bool Search::getOn() {
    return on;
}

void Search::setOn(const bool value) {
    on = value;
}
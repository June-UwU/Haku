#include "haku.hpp"

int main() {
    if (false == initialize_engine()) {
        return 1;
    }

    run_engine();
    return 0;
}
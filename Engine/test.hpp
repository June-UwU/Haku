#pragma once

#include "defines.hpp"
// routine to not expose internal system used for testing purposes
HAPI void prepare_subsystem_for_test(void);

HAPI void shutdown_test_subsystem(void);

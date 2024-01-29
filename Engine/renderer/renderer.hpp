#pragma once
#include "types.hpp"
#include "defines.hpp"

[[nodiscard]] Status initializeRenderer();

void render();

void clearScreen();

void shutdownRenderer();
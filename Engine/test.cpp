/*****************************************************************//**
 * \file   test.cpp
 * \brief  implementations of the test routines
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/

#include "test.hpp"
#include "platform/platform.hpp"


void prepare_subsystem_for_test(void)
{
	platform_memory_initialize();
}

void shutdown_test_subsystem(void)
{
	platform_memory_shutdown();
}

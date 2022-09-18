/*****************************************************************//**
 * \file   test.cpp
 * \brief  implementations of the test routines
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/

#include "test.hpp"
#include "platform/platform.hpp"
#include "memory/hmemory.hpp"

void prepare_subsystem_for_test(void)
{
	hmemory_initialize();
}

void shutdown_test_subsystem(void)
{
	hlog_memory_report();
	hmemory_shutdown();
}

#include "test.hpp"
#include <gtest/gtest.h>
#include "platform/platform.hpp"
#include "memory/linear_allocator.hpp"

TEST(linear_allocator_test, linear_allocator_test)
{
	linear_allocator* allocator = create_linear_allocator(512);

	EXPECT_EQ(allocator->offset, 0);

	void* ret_ptr = linear_allocate(allocator, allocator->size_in_bytes - 1 );

	EXPECT_TRUE(ret_ptr);
	
	ret_ptr       = linear_allocate(allocator, allocator->size_in_bytes);

	EXPECT_TRUE(nullptr == ret_ptr);

	linear_allocator_reset(allocator);

	EXPECT_TRUE(allocator->offset == 0);

}

int main(int argc, char** argv) 
{
	prepare_subsystem_for_test();

	::testing::InitGoogleTest(&argc, argv);
	int  ret_val = RUN_ALL_TESTS();
	
	shutdown_test_subsystem();
	return ret_val;
}
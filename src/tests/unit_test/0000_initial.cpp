//
// file:			0000_initial.cpp
// created on:		2021 Feb 02
//

#include <cinternal/unit_test.h>
#include <stdio.h>

static int s_handler_execution_number = 2;

TEST(f_0000_initial, t0000_first)
{
	printf("Tests to test CPP utils started!\n");
	EXPECT_EQ(s_handler_execution_number, 2);

}

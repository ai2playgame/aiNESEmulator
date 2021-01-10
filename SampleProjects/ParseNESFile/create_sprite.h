#pragma once
#include <array>

void merge_row(uint8_t lhs, uint8_t rhs, uint8_t* buf) {
	for (int i = 7; i >= 0; --i) {
		uint8_t lhs_mask = lhs & (0x1 << i);
		uint8_t rhs_mask = rhs & (0x1 << i);
		
		uint8_t pixVal;
		if (!(lhs_mask) && !(rhs_mask)) // 00
		{
			pixVal = 0;
		}
		else if (lhs_mask && !(rhs_mask)) // 01
		{
			pixVal = 0x55;
		}
		else if (!(lhs_mask) && rhs_mask) // 10
		{
			pixVal = 0xaa;
		}
		else { // 11
			pixVal = 0xff;
		}
		buf[7 - i] = pixVal;
	}
}

#ifndef __CONVOLUTION_FILTER_H__
#define __CONVOLUTION_FILTER_H__

namespace Edge_Detect {
	// Main function for edge detection, All memory (for input AND output) should be allocated before calling this, and bBPP must be 1, 2, 3 or 4 (not multiples of 8).
	void EdgeDetect_Filter(unsigned char* pOut, const unsigned char* pSourceData, const unsigned char bBPP, const long ulWidth, const long ulHeight, const float fConstant = 8.0f);
}

#endif	//	__CONVOLUTION_FILTER_H__

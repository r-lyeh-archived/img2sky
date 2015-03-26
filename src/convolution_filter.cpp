#include <stdlib.h>
#include <algorithm>

#include "convolution_filter.h"

namespace Edge_Detect {
	const float c = -1/8.f;

	const unsigned long ulKernelSize = 3;
	const float fKernel[] = {
				c, c, c,
				c, 1, c,
				c, c, c,
	};

	// Convolution filter (returns a signle channel, by averaging all given channels).
	void EdgeDetect_Filter(unsigned char* pOut, const unsigned char* pSourceData, const unsigned char bBPP, const long ulWidth, const long ulHeight, const float fConstant) {
		// All the memory must be allocated, and the data should be valid, before calling this.
		// Otherwise, we bail OUT.
		if ((pOut == NULL) || (pSourceData == NULL) || (bBPP == 0) || (ulWidth == 0) || (ulHeight == 0))
			return;

		float *pTMP = new float[ulWidth * ulHeight];
		float *pTMP2 = new float[ulWidth * ulHeight];

		// First, copy the channel data (without discarding any channel) to a floating point array.
		for (long i = 0, j = 0; i < ulWidth * ulHeight * bBPP; i += bBPP, j++) {
			pTMP[j] = 0; //pSourceData[i];

			for (long k = 0; k < bBPP; k++)
				pTMP[j] += pSourceData[i + k];

			pTMP[j] /= bBPP;
			pTMP2[j] = pTMP[j];
		}

		// Now apply the filter
		int i = 0;

		for (long y = 0; y < ulHeight; y++) {
			for (long x = 0; x < ulWidth; x++) {
				float fNew = 0.0f;
				
				for (long cy = 0, oy = -1; cy < ulKernelSize; cy++, oy++) {
					for (long cx = 0, ox = -1; cx < ulKernelSize; cx++, ox++) {
						float fVal = 0.0f;

						// If given co-ordinates are outside 0, give a value of zero (which is already
						// present.) Else get the value from input image.
						if (!((x + ox < 0) || (x + ox >= ulWidth) || (y + oy >= ulHeight) || (y + oy < 0)))
							fVal = pTMP2[(y + oy) * ulWidth + (x + ox)];

						fNew += fKernel[cy * ulKernelSize + cx] * fVal;
					}
				}

				pTMP[i] = fNew * fConstant;

				i++;
			}
		}

		// Finally, copy to destination
		for (int i = 0, j = 0; i < ulWidth * ulHeight * bBPP; i += bBPP, j++) {
			for (long k = 0; k < bBPP; k++)
				pOut[i + k] = (unsigned char)std::max(0.0f, std::min(pTMP[j], 255.0f));
		}

		// Free memory.
		delete [] pTMP;
		delete [] pTMP2;
	}
}

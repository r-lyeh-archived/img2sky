#ifndef __TGAIMAGE_H__
#define __TGAIMAGE_H__

/*
**	Dependencies
*/
#include <memory.h>
#include <math.h>
#include <algorithm>
#include "convolution_filter.h"

/*
**	Macros
*/
// To clamp 'i' between [l, u]
template<typename T, typename L, typename U>
T clamp( const T &i, const L &l, const U& u ) {
	return ((i < l) ? (l) : ((u < i) ? (u) : (i)));
}

// Generic 4 char to long converter
#define TGAI_MAKE_COL(a, b, c, d)		((a) | (b << 8) | (c << 16) | (d << 24))

// To make different colours
#define TGAI_MAKE_RGBA(r, g, b, a)		TGAI_MAKE_COL(r, g, b, a)
#define TGAI_MAKE_BGRA(r, g, b, a)		TGAI_MAKE_COL(b, g, r, a)
#define TGAI_MAKE_ARGB(r, g, b, a)		TGAI_MAKE_COL(a, r, g, b)
#define TGAI_MAKE_ABGR(r, g, b, a)		TGAI_MAKE_COL(a, b, g, r)

// To make colour of the order 'TGAPixel32'
#define TGAI_MAKE						TGAI_MAKE_BGRA

// Pixel colour order for TGA. MUST match as defined in the pixel struct below.
#define TGA_R							2
#define TGA_G							1
#define TGA_B							0
#define TGA_A							3

/*
**	TGA Structs\Classes
*/
struct TGAHeader {
	TGAHeader() {
		memset(this, 0, sizeof(TGAHeader));

		bUnknown[2]	= 2;
		bBPP		= 32;
		bUnknown2	= 8;
	}

	~TGAHeader() { }

	unsigned char bUnknown[8];

	signed short ssXOffset;
	signed short ssYOffset;

	unsigned short usWidth;
	unsigned short usHeight;

	unsigned char bBPP;
	unsigned char bUnknown2;
};

struct TGAPixel32 {
	// Constructor(s)
	TGAPixel32() {
		R = G = B = A = 0;
	}

	TGAPixel32(const unsigned char _R, const unsigned char _G, const unsigned char _B, const unsigned char _A) {
		R = _R; G = _G; B = _B; A = _A;
	}

	TGAPixel32(const unsigned long ulABGR) {
		ulColour = ulABGR;
	}

	TGAPixel32(const TGAPixel32 &p) {
		ulColour = p.ulColour;
	}

	// Destructor
	~TGAPixel32() { }

	// Pixel Values.
	unsigned char GrayValue(void) {
		return unsigned char(0.3f * R + 0.59f * G + 0.11f * B);
	}

	unsigned char RMSValue(void) {
		return unsigned char(sqrtf((powf(R, 2) + powf(G, 2) + powf(B, 2)) / 3));
	}

	unsigned char AvgValue(void) {
		return unsigned char(((short)R + (short)G + (short)B) / 3);
	}

	// Conversion operator(s)
	operator long() const {
		return ulColour;
	}

	// Comparision operator(s)
	bool operator==(const TGAPixel32 &p) const {
		return (p.R == R) && (p.G == G) && (p.B == B) && (p.A == A);
	}

	bool operator!=(const TGAPixel32 &p) const {
		return !(p == *this);
	}

	bool operator< (const TGAPixel32 &p) const {
		const float fA1 = A / 255.0f;
		const float fA2 = p.A / 255.0f;

		const float f1 = sqrtf(powf(R * fA1, 2) + powf(G * fA1, 2) + powf(B * fA1, 2));
		const float f2 = sqrtf(powf(p.R * fA2, 2) + powf(p.G * fA2, 2) + powf(p.B * fA2, 2));
		
		return (f1 < f2);
	}

	bool operator<= (const TGAPixel32 &p) const {
		return (*this < p) || (*this == p);
	}

	bool operator> (const TGAPixel32 &p) const {
		return !(*this <= p);
	}

	bool operator>= (const TGAPixel32 &p) const {
		return !(*this < p);
	}

	// Logical operators
	// OR
	TGAPixel32 operator | (const TGAPixel32 &p) const {
		return ulColour | p.ulColour; 
	}

	// OR, in place
	TGAPixel32& operator |=(const TGAPixel32 &p) {
		ulColour |= p.ulColour;
		return *this;
	}

	// AND
	TGAPixel32 operator & (const TGAPixel32 &p) const {
		return ulColour & p.ulColour; 
	}

	// AND, in place
	TGAPixel32& operator &=(const TGAPixel32 &p) {
		ulColour &= p.ulColour;
		return *this;
	}

	// XOR
	TGAPixel32 operator ^ (const TGAPixel32 &p) const {
		return ulColour ^ p.ulColour; 
	}

	// XOR, in place
	TGAPixel32& operator ^=(const TGAPixel32 &p) {
		ulColour ^= p.ulColour;
		return *this;
	}

	// NOT
	TGAPixel32& operator ~() {
		ulColour = ~ulColour;
		return *this;
	}

	// Data
	union {
		unsigned long ulColour;
		unsigned char bColour[4];
		struct {
			unsigned char B;
			unsigned char G;
			unsigned char R;
			unsigned char A;
		};
	};
};

class TGAFile {
public:
	// Default contructor and the destructor
	TGAFile() : hdr() {
		pixels = NULL;
	}

	TGAFile(const TGAFile &t) { 
		memset(this, 0, sizeof(TGAFile));
		CopyTGA(t);
	}

	~TGAFile()	{ CleanUp(); }

	TGAFile& operator =(const TGAFile &t) {
		CopyTGA(t);
		return *this;
	}

	void CopyTGA(const TGAFile &t) {
		// Free Original Image
		FreeImage();

		// Copy header (width, height, etc)
		hdr = t.hdr;

		// Allocate space for new image
		ReallocateImage();
		
		// Copy data.
		memcpy(pixels, t.pixels, t.hdr.usWidth * t.hdr.usHeight * sizeof(TGAPixel32));
	}

	// Performs cleanup on the TGA (release of allocated mem, etc)
	void CleanUp(void);

	// Reads the given TGA. 'true' is returned if all went fine.
	bool ReadTGA(const char *szFileName);

	// Dumps the pixel data of given TGA as a RAW file. 'true' is returned if all went fine.
	bool DumpTGA(const char *szFileName, unsigned char bR = 0xFF, unsigned char bG = 0xFF, unsigned char bB = 0xFF, unsigned char bA = 0xFF) const;

	// Write the given TGA. 'true' is returned if all went fine.
	bool WriteTGA(const char *szFileName) const;

	// To get 32-bit read-only image data.
	const void * const GetImageStream() const							{ return pixels; }

	// To get channel data (bR\bG\bB\bA = place of R\G\B\A channel in the interval [0, 3); -1 = exclude channel)
	void *GetImageData(unsigned char bR = 0xFF, unsigned char bG = 0xFF, unsigned char bB = 0xFF, unsigned char bA = 0xFF) const;

	// To set 32-bit image data.
	void SetImageData(void *pNewData);

	// To retrieve a pixel (function for 'normalized' position also)
	TGAPixel32 GetPixel(unsigned short x, unsigned short y) const;
	TGAPixel32 GetPixel(float x, float y) const							{ return GetPixel(unsigned short(x * hdr.usWidth), unsigned short(y * hdr.usHeight)); }
	template <class T> TGAPixel32 GetPixelA(T x, T y) const				{ return GetPixel(unsigned short(x), unsigned short(y)); }

	// To set a pixel (function for 'normalized' position also
	void SetPixel(unsigned short x, unsigned short y, TGAPixel32 p);
	void SetPixel(float x, float y, TGAPixel32 p)						{ SetPixel(unsigned short(x * hdr.usWidth), unsigned short(y * hdr.usHeight), p); }
	template <class T> void SetPixelA(T x, T y, TGAPixel32 p)			{ SetPixel(unsigned short(x), unsigned short(y), p); }

	// To modify some basic info.
	unsigned short GetWidth() const				{ return hdr.usWidth; }
	unsigned short GetHeight() const			{ return hdr.usHeight; }
	unsigned char GetBPP() const				{ return hdr.bBPP; }

	void SetWidth(unsigned short usNewWidth)	{ hdr.usWidth = usNewWidth; }
	void SetHeight(unsigned short usNewHeight)	{ hdr.usHeight = usNewHeight; }
	void SetBPP(unsigned char bNewBPP)			{ hdr.bBPP = bNewBPP; hdr.bUnknown2 = 8; }

	// Blit function (to copy a square rect into part of a TGA and bR\bG\bB\bA = place of R\G\B\A channel in the interval 
	// [0, 3); -1 = exclude channel). Note that the pointer must *NOT* be a null.
	bool RectBlit(const unsigned short ulSX, const unsigned short ulSY, const unsigned short ulEX, const unsigned short ulEY, unsigned char bR = 0xFF, unsigned char bG = 0xFF, unsigned char bB = 0xFF, unsigned char bA = 0xFF, const void *pData = NULL);

	// Blit function, using normalized co-ordinates.
	bool RectBlit(const float fSX, const float fSY, const float fEX, const float fEY, unsigned char bR = 0xFF, unsigned char bG = 0xFF, unsigned char bB = 0xFF, unsigned char bA = 0xFF, const void *pData = NULL)  {
		if ((fSX < 0.0f) || (fSY < 0.0f) || (fEX > 1.0f) || (fEY > 1.0f))
			return false;

		const unsigned short ulSX = (unsigned short)std::min(fSX * GetWidth(), float(GetWidth() - 1));
		const unsigned short ulSY = (unsigned short)std::min(fSY * GetHeight(), float(GetHeight() - 1));
		const unsigned short ulEX = (unsigned short)std::min(fEX * GetWidth(), float(GetWidth() - 1));
		const unsigned short ulEY = (unsigned short)std::min(fEY * GetHeight(), float(GetHeight() - 1));

		return RectBlit(ulSX, ulSY, ulEX, ulEY, bR, bG, bB, bA, pData);
	}

	// Blit function, for another TGA.
	bool RectBlit(const unsigned short ulSX, const unsigned short ulSY, const unsigned short ulEX, const unsigned short ulEY, const TGAFile &pTGA) {
		return RectBlit(ulSX, ulSY, ulEX, ulEY, TGA_R, TGA_G, TGA_B, TGA_A, pTGA.GetImageStream());
	}

	// Blit function, using normalized co-ordinates, for another TGA.
	bool RectBlit(const float fSX, const float fSY, const float fEX, const float fEY, const TGAFile &pTGA) {
		return RectBlit(fSX, fSY, fEX, fEY, TGA_R, TGA_G, TGA_B, TGA_A, (unsigned char *)pTGA.GetImageStream());
	}

	enum TGA_Fill_OP {
		// No extra inputs.
		OP_NONE,
		OP_NOT,

		// Uses 'ulCol' only.
		OP_OR,
		OP_NOR,
		OP_XOR,
		OP_NXOR,
		OP_AND,
		OP_NAND,

		// Uses 'ulCol' and 'cmpPixel'
		OP_EQUAL,
		OP_NEQUAL,
		OP_LESSTHAN,
		OP_NLESSTHAN,
		OP_LESSTHANEQ,
		OP_NLESSTHANEQ,
		OP_MORETHAN,
		OP_NMORETHAN,
		OP_MORETHANEQ,
		OP_NMORETHANEQ,

		// Last OP.
		OP_LAST
	};

	// To make basic modifications to the image.
	void RectFill(const long x1, const long x2, const long y1, const long y2, const unsigned long ulCol = 0, const TGA_Fill_OP OP = OP_NONE, const TGAPixel32 cmpPixel = 0);
	void RectFill(const float x1, const float x2, const float y1, const float y2, const unsigned long ulCol = 0, const TGA_Fill_OP OP = OP_NONE, const TGAPixel32 cmpPixel = 0) {
		const long ulX1 = long(x1 * GetWidth());
		const long ulY1 = long(y1 * GetHeight());
		const long ulX2 = long(x2 * GetWidth());
		const long ulY2 = long(y2 * GetHeight());

		RectFill(ulX1, ulX2, ulY1, ulY2, ulCol, OP, cmpPixel);
	}

	// Alt function for absolute co-ordinates, no matter what form they are (double, single, long, short, or even byte)..
	template <class T> void RectFillA(const T x1, const T x2, const T y1, const T y2, const unsigned long ulCol = 0, const TGA_Fill_OP OP = OP_NONE, const TGAPixel32 cmpPixel = 0) {
		RectFill((long)x1, (long)x2, (long)y1, (long)y2, ulCol, OP, cmpPixel);
	}

	// To apply the convolution (edge-detect) filter to this image.
	void Convolute(const bool bColoured = false, const float fConvConstant = 8.0f);

	// To apply the edge detect filter to a single channel (returns the channel).
	void *ConvoluteChannel(const unsigned char bInd, const float fConvConstant = 8.0f) const;

	// To apply a simple gaussian filter
	void GaussianBlur(const unsigned short usRadius = 1, const float fAlpha = 0.125f);

	// Multiplies (and clamps) the colour with a given factor. Negative values not accepted (0 is considered OK).
	void ColourMult(float fFactor);

	// Greys the image.
	void GrayImage(void) {
		// Make sure that our image is present.
		if (pixels == NULL)
			return;

		for (unsigned short y = 0; y < hdr.usHeight; y++) {
			for (unsigned short x = 0; x < hdr.usWidth; x++) {
				float fPixel = 0;
				TGAPixel32 P = GetPixel(x, y);

				fPixel = P.R * 0.3f + P.G * 0.59f + P.B * 0.11f;

				P.R = unsigned char(fPixel);
				P.G = unsigned char(fPixel);
				P.B = unsigned char(fPixel);
				
				SetPixel(x, y, P);
			}
		}
	}

	// Returns the pixel difference for the pixel at the specified position, and pixels in the given radius.
	unsigned char PixelDiff(const unsigned short x, const unsigned short y, const unsigned short r) {
		// Pixel radius must be valid (greater than 0).
		if (r < 0)
			return 0;

		// Check for invalid co-ordinates.
		if ((x < 0) || (y < 0) || (x >= hdr.usWidth) || (y >= hdr.usHeight))
			return 0;

		unsigned char bRetVal = 0;

		// Get the mentioned pixel.
		const TGAPixel32 p = GetPixel(x, y);

		if (r > 0) {
			// Compare with previous and next pixels.
			for (long j = y - r; j <= y + r; j++) {
				for (long i = x - r; i <= x + r; i++) {
					// If they are outside range, go for next pixel.
					if ((i < 0) || (j < 0) || (i >= hdr.usWidth) || (j >= hdr.usHeight))
						continue;
					
					// Don't compare with self.
					if ((i == x) && (j == y))
						continue;

					// Get the pixel and compare.
					TGAPixel32 p1 = GetPixelA(i, j);

					unsigned char R = (unsigned char)labs((long)p1.R - (long)p.R);
					unsigned char G = (unsigned char)labs((long)p1.G - (long)p.G);
					unsigned char B = (unsigned char)labs((long)p1.B - (long)p.B);
					unsigned char A = (unsigned char)labs((long)p1.A - (long)p.A);

					bRetVal = std::max(bRetVal, std::max(R, std::max(G, std::max(B, A))));
				}
			}
		} else {
			// Compare with immediately next pixels.
			unsigned long i = x, j = y;

			for (unsigned long a = 0; a < 3; a++) {
				switch (a) {
					case 0: i = x + 1; j = y; break;
					case 1: i = x; j = y + 1; break;
					case 2: i = x + 1; j = y + 1; break;
				}

				// If out of range then exit.
				if ((i < 0) || (j < 0) || (i >= hdr.usWidth) || (j >= hdr.usHeight))
					continue;

				// Get the pixel and compare.
				const TGAPixel32 p1 = GetPixelA(i, j);

				unsigned char R = (unsigned char)labs((long)p1.R - (long)p.R);
				unsigned char G = (unsigned char)labs((long)p1.G - (long)p.G);
				unsigned char B = (unsigned char)labs((long)p1.B - (long)p.B);
				unsigned char A = (unsigned char)labs((long)p1.A - (long)p.A);

				bRetVal = std::max(bRetVal, std::max(R, std::max(G, std::max(B, A))));
			}

			
		}

		return bRetVal;
	}

	// Does a min\max filter on given pixel with given radius.
	TGAPixel32 MinMaxFilterPixel(const unsigned short x, const unsigned short y, const unsigned short r, const bool bMax) {
		// Pixel radius must be valid (greater than 0).
		if (r < 0)
			return 0;

		// Check for invalid co-ordinates.
		if ((x < 0) || (y < 0) || (x >= hdr.usWidth) || (y >= hdr.usHeight))
			return 0;

		TGAPixel32 pRet = GetPixelA(x, y);

		if (r) {
			for (long j = y - r; j <= y + r; j++) {
				for (long i = x - r; i <= x + r; i++) {
					// If they are outside range, go for next pixel.
					if ((i < 0) || (j < 0) || (i >= hdr.usWidth) || (j >= hdr.usHeight))
						continue;

					TGAPixel32 pTmp = GetPixelA(i, j);

					if (bMax) {
						pRet.R = std::max(pRet.R, pTmp.R);
						pRet.G = std::max(pRet.G, pTmp.G);
						pRet.B = std::max(pRet.B, pTmp.B);
						pRet.A = std::max(pRet.A, pTmp.A);
					} else {
						pRet.R = std::min(pRet.R, pTmp.R);
						pRet.G = std::min(pRet.G, pTmp.G);
						pRet.B = std::min(pRet.B, pTmp.B);
						pRet.A = std::min(pRet.A, pTmp.A);
					}
				}
			}
		}

		return pRet;
	}	

	// Does a min\max filter on complete image.
	void MinMaxFilter(const unsigned short r, const bool bMax) {
		// Pixel radius must be valid (not 0 or 1).
		if (r < 1)
			return;

		TGAFile TGA = *this;

		for (unsigned short y = 0; y < hdr.usHeight; y++) {
			for (unsigned short x = 0; x < hdr.usWidth; x++) {
				TGAPixel32 p = TGA.MinMaxFilterPixel(x, y, r, bMax);
				SetPixel(x, y, p);
			}
		}

		TGA.CleanUp();
	}

	// To free the image.
	void FreeImage() {
		if (pixels)
			delete [] pixels;

		pixels = NULL;

		SetWidth(0);
		SetHeight(0);
		SetBPP(32);
	}

	// Re-allocates the image data. Note that, all existing data is lost.
	void ReallocateImage(bool bNullify = false) {
		unsigned long ulWidth = GetWidth();
		unsigned long ulHeight = GetHeight();
		unsigned char bBPP = GetBPP();

		FreeImage();

		SetWidth((unsigned short)ulWidth);
		SetHeight((unsigned short)ulHeight);
		SetBPP(bBPP);

		pixels = new TGAPixel32[ulWidth * ulHeight];

		if (bNullify)
			memset(pixels, 0, sizeof(TGAPixel32) * ulWidth * ulHeight);
	}
private:
	// Storage of TGA
	TGAHeader	hdr;
	TGAPixel32*	pixels;
};

inline TGAPixel32 TGAFile::GetPixel(unsigned short x, unsigned short y) const {
	signed long _x = clamp(x, 0, hdr.usWidth);
	signed long _y = clamp(y, 0, hdr.usHeight);

	if (_x == hdr.usWidth)	{ _x--; }
	if (_y == hdr.usHeight)	{ _y--; }

	return pixels[_y * hdr.usWidth + _x];
}

inline void TGAFile::SetPixel(unsigned short x, unsigned short y, TGAPixel32 p) {
	signed long _x = clamp(x, 0, hdr.usWidth);
	signed long _y = clamp(y, 0, hdr.usHeight);

	if (_x == hdr.usWidth)	{ _x--; }
	if (_y == hdr.usHeight)	{ _y--; }

	pixels[_y * hdr.usWidth + _x] = p;
}

#endif	//	__TGAIMAGE_H__

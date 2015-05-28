#include <stdio.h>
#include <memory>
#include <algorithm>
#include <iostream>

#include "spot.hpp"
#include "image.h"

using namespace std;

void TGAFile::CleanUp(void) {
	FreeImage();
	memset(this, 0, sizeof(TGAFile));
}

bool TGAFile::ReadTGA(const char *szFileName) {
	// Read if it exists.
	//spot::image original = spot::image(szFileName);
	//spot::image halftoned = original * spot::hsla( 1, 1, 0.5f, 1 );
	//spot::texture img = halftoned;
	spot::texture img( szFileName );
	
	if (!img.loaded())
		goto Failure;

	img = img.flip_h(); // original implementation expects a flipped image, so we flip it here

	CleanUp();

	hdr.usWidth = img.w;
	hdr.usHeight = img.h;
	hdr.bBPP = 32;

	// We don't accept zero sized files.
	if ((hdr.usWidth * hdr.usHeight * hdr.bBPP) == 0)
		goto Failure;

	// Read in the pixels.
	{
		const unsigned char bNumChannels = hdr.bBPP / 8;

		ReallocateImage();

		for (unsigned long y = 0; y < hdr.usHeight; y++) {
			for (unsigned long x = 0; x < hdr.usWidth; x++) {
				const unsigned long ulPixel = y * hdr.usWidth + x;

				pixels[ulPixel].ulColour = TGAI_MAKE_BGRA(
					img[ulPixel].r,img[ulPixel].g,img[ulPixel].b,img[ulPixel].a);
			}
		}
	}

	// GaussianBlur(const unsigned short usRadius, const float fAlpha) {

	return true;

Failure:

	// Reset TGA data; we're in a big problem.
	FreeImage();
	CleanUp();

	return false;
}

bool TGAFile::DumpTGA(const char *szFileName, unsigned char bR, unsigned char bG, unsigned char bB, unsigned char bA) const {
	void *pOut = NULL;
	unsigned char bNumChannels = 0;

	// Make sure that our image is present.
	if (pixels == NULL)
		return false;

	// Open file
	FILE *pFile = fopen(szFileName, "wb");

	if (!pFile)
		goto Failure;

	// Get data to be written
	pOut = (unsigned char *)GetImageData(bR, bG, bB, bA);	

	if (!pOut)
		goto Failure;

	// Guess the number of channels
	if (bR != 0xFF)	{ bNumChannels++; }
	if (bG != 0xFF)	{ bNumChannels++; }
	if (bB != 0xFF)	{ bNumChannels++; }
	if (bA != 0xFF)	{ bNumChannels++; }	

	// Write
	fwrite(pOut, sizeof(char), hdr.usWidth * hdr.usHeight * bNumChannels, pFile);
	fclose(pFile);

	// Clean-up
	delete [] pOut;

	return true;
Failure:
	// Close the file if it was ever opened.
	if (pFile)
		fclose(pFile);

	// And free this if it was ever allocated.
	if (pOut)
		delete [] pOut;

	return false;
}

bool TGAFile::WriteTGA(const char *szFileName) const {
	unsigned long x, y;
	FILE *pFile = fopen(szFileName, "wb");

	// Make sure that our image is present.
	if (pixels == NULL)
		goto Failure;

	if (!pFile)
		goto Failure;

	fwrite(&hdr, sizeof(TGAHeader), 1, pFile);

	// Write the pixels.
	for (y = GetHeight(); y > 0; y--) {
		for (x = 0; x < GetWidth(); x++)
			fwrite(&GetPixelA(x, y - 1), sizeof(char), GetBPP() / 8, pFile);
	}

	fclose(pFile);

	return true;

Failure:
	// Close the file if it was ever opened.
	if (pFile)
		fclose(pFile);

	return false;
}

bool TGAFile::RectBlit(const unsigned short ulSX, const unsigned short ulSY, const unsigned short ulEX, const unsigned short ulEY, unsigned char bR, unsigned char bG, unsigned char bB, unsigned char bA, const void *pData) {
	// Make sure the co-ordinates are valid.
	if ((ulSX > ulEX) || (ulSY > ulEY))
		return false;

	// Make sure the data is present.
	if (pData == NULL)
		return false;

	// Make sure that our image is present.
	if (pixels == NULL)
		return false;

	const unsigned char bNumChannels = 
		((bR != 0xFF) ? (1) : (0)) + 
		((bG != 0xFF) ? (1) : (0)) + 
		((bB != 0xFF) ? (1) : (0)) + 
		((bA != 0xFF) ? (1) : (0));

	// Make sure we have atleast one channel.
	if (bNumChannels == 0)
		return false;

	// Calculate the min\max channels.
	const unsigned char bMin = min(bR, min(bG, min(bB, bA)));
	const unsigned char bMax = max(
		(bR != 0xFF) ? (bR) : (0), max(
		(bG != 0xFF) ? (bG) : (0), max(
		(bB != 0xFF) ? (bB) : (0), 
		(bA != 0xFF) ? (bA) : (0)))) - bMin;

	// Calculate the actual channel position (starting from 0).
	const unsigned char bER = bR - bMin;
	const unsigned char bEG = bG - bMin;
	const unsigned char bEB = bB - bMin;
	const unsigned char bEA = bA - bMin;

	// Make sure that the channel indices are valid.
	if (bMax > 3)
		return false;

	const unsigned char *pPix = (unsigned char *)pData;

	for (unsigned short y = ulSY; y < ulEY; y++) {
		for (unsigned short x = ulSX; x < ulEX; x++) {
			TGAPixel32 p = 0xFF000000;

			for (unsigned char i = 0; i < bNumChannels; i++, pPix++) {
				if (bER == i) {
					p.R = *pPix;
					continue;
				}

				if (bEG == i) {
					p.G = *pPix;
					continue;
				}

				if (bEB == i) {
					p.B = *pPix;
					continue;
				}

				if (bEA == i) {
					p.A = *pPix;
					continue;
				}
			}

			SetPixel(x, y, p);
		}
	}

	return true;
}

void TGAFile::RectFill(const long x1, const long x2, const long y1, const long y2, const unsigned long ulCol, const TGA_Fill_OP OP, const TGAPixel32 cmpPixel) {
	TGAPixel32 p(ulCol);

	// Make sure that our image is present.
	if (pixels == NULL)
		return;

	// Check for bad co-ordinates
	if ((x1 < 0) || (y1 < 0) || (x2 < 0) || (y2 < 0))
		return;

	if ((x1 >= GetWidth()) || (y1 >= GetHeight()) || (x2 >= GetWidth()) || (y2 >= GetHeight()))
		return;

	// Check for invalid co-ordinates
	if ((x2 < x1) || (y2 < y1))
		return;

	for (unsigned short y = (unsigned short)y1; y <= y2; y++) {
		for (unsigned short x = (unsigned short)x1; x <= x2; x++) {
			const unsigned long d = x + y * hdr.usWidth;

			// On the basis of the option given, decide what we've to o.
			switch (OP) {
				default:
				case OP_NONE:			// destination = source
					pixels[d].ulColour = ulCol;
					break;
				case OP_NOT:			// NOT destination
					pixels[d].ulColour = ~pixels[d].ulColour;
					break;

				case OP_EQUAL:			// destination == source
					if (pixels[d] == cmpPixel)
						pixels[d].ulColour = ulCol;

					break;
				case OP_NEQUAL:			// destination != source
					if (pixels[d] != cmpPixel)
						pixels[d].ulColour = ulCol;

					break;
				case OP_NMORETHANEQ:	// NOT (destination >= source)
				case OP_LESSTHAN:		// destination < source
					if (pixels[d] < cmpPixel)
						pixels[d].ulColour = ulCol;

					break;
				case OP_NMORETHAN:		// NOT (destination > source)
				case OP_LESSTHANEQ:		// destination <= source
					if (pixels[d] < cmpPixel)
						pixels[d].ulColour = ulCol;

					break;
				case OP_NLESSTHANEQ:	// NOT (destination <= source)
				case OP_MORETHAN:		// destination > source
					if (pixels[d] > cmpPixel)
						pixels[d].ulColour = ulCol;

					break;
				case OP_NLESSTHAN:		// NOT (destination < source)
				case OP_MORETHANEQ:		// destination >= source
					if (pixels[d] >= cmpPixel)
						pixels[d].ulColour = ulCol;

					break;
				case OP_OR:				// destination OR source
					pixels[d] |= ulCol;

					break;
				case OP_NOR:			// NOT (destination OR source)
					pixels[d] |= ulCol;
					~pixels[d];

					break;
				case OP_XOR:			// destination XOR source
					pixels[d] ^= ulCol;

					break;

				case OP_NXOR:			// NOT (destination XOR source)
					pixels[d] ^= ulCol;
					~pixels[d];

					break;
				case OP_AND:			// destination AND source
					pixels[d] &= ulCol;

					break;
				case OP_NAND:			// NOT (destination AND source)
					pixels[d] &= ulCol;
					~pixels[d];

					break;
			}
		}
	}
}

void TGAFile::Convolute(const bool bColoured, const float fConvConstant) {
	// Make sure that our image is present.
	if (pixels == NULL)
		return;

	unsigned char *pOut1 = (unsigned char *)ConvoluteChannel(0, fConvConstant);	/* A */
	unsigned char *pOut2 = (unsigned char *)ConvoluteChannel(1, fConvConstant);	/* B */
	unsigned char *pOut3 = (unsigned char *)ConvoluteChannel(2, fConvConstant);	/* G */
	unsigned char *pOut4 = (unsigned char *)ConvoluteChannel(3, fConvConstant);	/* R */

	unsigned long i = 0;

	for (unsigned short y = 0; y < hdr.usHeight; y++) {
		for (unsigned short x = 0; x < hdr.usWidth; x++) {
			TGAPixel32 p;

			p.A = pOut1[i];		/* A */

			if (!bColoured) {
				// Neglect the effect of alpha. We convolute alpha seperately.
				p.R = p.G = p.B = (/* B */pOut2[i] + /* G */pOut3[i] + /* R */pOut4[i]) / 3;
			} else {
				p.B = pOut2[i]; /* B */
				p.G = pOut3[i]; /* G */
				p.R = pOut4[i]; /* R */
			}

			pixels[i] = p;

			i++;
		}
	}

	delete [] pOut1;	/* A */
	delete [] pOut2;	/* B */
	delete [] pOut3;	/* G */
	delete [] pOut4;	/* R */	
}

void *TGAFile::ConvoluteChannel(const unsigned char bInd, const float fConvConstant) const {
	// Make sure that our image is present.
	if (pixels == NULL)
		return false;

	const unsigned long ulNumPixels = (unsigned long)hdr.usWidth * (unsigned long)hdr.usHeight;

	// Store input data
	unsigned char *pIn = NULL;
	unsigned char *pOut = new unsigned char[ulNumPixels];

	switch (bInd) {
		case 0:	// Alpha
			pIn = (unsigned char *)GetImageData(0xFF, 0xFF, 0xFF, 0);
			break;
		case 1:	// Blue
			pIn = (unsigned char *)GetImageData(0xFF, 0xFF, 0);
			break;
		case 2:	// Green
			pIn = (unsigned char *)GetImageData(0xFF, 0);
			break;
		case 3:	// Red
			pIn = (unsigned char *)GetImageData(0);
			break;
	}
	// Convolute
	Edge_Detect::EdgeDetect_Filter(pOut, pIn, 1, hdr.usWidth, hdr.usHeight, fConvConstant);

	delete [] pIn;
	return pOut;
}

// To apply a simple gaussian filter
void TGAFile::GaussianBlur(const unsigned short usRadius, const float fAlpha) {
	// Make sure that our image is present.
	if (pixels == NULL)
		return;

	// Radius must not be zero; alpha must not be 0
	if ((usRadius == 0) || (fAlpha == 0.0f))
		return;

	unsigned char *pImage = (unsigned char *)GetImageData(3, 2, 1, 0);

	for (long y = 0; y < GetHeight(); y++) {
		for (long x = 0; x < GetWidth(); x++) {
			float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
			float fDiv = 0.0f;

			// Acculmulate colour
			for (short oy = -usRadius; oy <= usRadius; oy++) {
				for (short ox = -usRadius; ox <= usRadius; ox++) {
					const float fMult = ((ox == 0) && (oy == 0) ? (1.0f) : (fAlpha));
					const TGAPixel32 p = GetPixelA(x + ox, y + oy);

					r += p.R * fMult; g += p.G * fMult;
					b += p.B * fMult; a += p.A * fMult;

					fDiv++;
				}
			}

			// Average colour
			if (fDiv) {
				r /= fDiv; g /= fDiv;
				b /= fDiv; a /= fDiv;
			}

			// Clamp
			r = clamp(r, 0.0f, 255.0f);
			g = clamp(g, 0.0f, 255.0f);
			b = clamp(b, 0.0f, 255.0f);
			a = clamp(a, 0.0f, 255.0f);

			// copy,
			TGAPixel32 p((unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);
			SetPixelA(x, y, p);
		}
	}

	// Don't forget to free the temporary copy!
	delete [] pImage;
}

// Multiplies (and clamps) the colour with a given factor. Negative values not accepted (0 is considered OK).
void TGAFile::ColourMult(float fFactor) {
	// Make sure that our image is present.
	if (pixels == NULL)
		return;

	// Factor cannot be zero.
	if (fFactor < 0)
		return;

	// No point wasting our time.
	if (fFactor = 1.0f)
		return;

	for (long y = 0; y < GetHeight(); y++) {
		for (long x = 0; x < GetWidth(); x++) {
			TGAPixel32 p = GetPixelA(x, y);

			p.R = (unsigned char)clamp(p.R * fFactor, 0.0f, 255.0f);
			p.G = (unsigned char)clamp(p.G * fFactor, 0.0f, 255.0f);
			p.B = (unsigned char)clamp(p.B * fFactor, 0.0f, 255.0f);
			p.A = (unsigned char)clamp(p.A * fFactor, 0.0f, 255.0f);

			SetPixelA(x, y, p);
		}
	}
}

void *TGAFile::GetImageData(unsigned char bR, unsigned char bG, unsigned char bB, unsigned char bA) const {
	// Make sure that our image is present.
	if (pixels == NULL)
		return false;

	// Get the number of channels
	unsigned char bNumChannels = 0;

	// Get the number of channels
	if (bR != 0xFF)	{ bNumChannels++; }
	if (bG != 0xFF)	{ bNumChannels++; }
	if (bB != 0xFF)	{ bNumChannels++; }
	if (bA != 0xFF)	{ bNumChannels++; }

	if (bNumChannels == 0)
		return NULL;

	// Shift channels, if we start from any other indice.
	{
		unsigned char bLowest = min(bR, min(bG, min(bB, bA)));

		if (bLowest) {
			bR -= bLowest;
			bG -= bLowest;
			bB -= bLowest;
			bA -= bLowest;
		}
	}

	// Allocate return
	unsigned char *pRet = new unsigned char[hdr.usWidth * hdr.usHeight * bNumChannels];

	// Write the pixels (force 32 bpp).
	{
		unsigned long ulPixelPos = 0;

		for (unsigned long y = 0; y < hdr.usHeight; y++) {
			for (unsigned long x = 0; x < hdr.usWidth; x++) {
				unsigned long ulPixel = 0;

				if (bR != 0xFF)
					ulPixel |= (pixels[ulPixelPos / bNumChannels].R << (bR * 8));

				if (bG != 0xFF)
					ulPixel |= (pixels[ulPixelPos / bNumChannels].G << (bG * 8));

				if (bB != 0xFF)
					ulPixel |= (pixels[ulPixelPos / bNumChannels].B << (bB * 8));

				if (bA != 0xFF)
					ulPixel |= (pixels[ulPixelPos / bNumChannels].A << (bA * 8));

				memcpy(&pRet[ulPixelPos], &ulPixel, sizeof(char) * bNumChannels);
				ulPixelPos += bNumChannels;
			}
		}
	}

	return pRet;
}

void TGAFile::SetImageData(void *pNewData) {
	unsigned short width = GetWidth();
	unsigned short height = GetHeight();
	unsigned char bpp = GetBPP();

	FreeImage();
	SetWidth(width);
	SetHeight(height);
	SetBPP(bpp);
	ReallocateImage();

	memcpy(pixels, pNewData, hdr.usWidth * hdr.usHeight * sizeof(char) * 4);
}

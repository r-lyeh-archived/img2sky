#pragma once

#include <string>
#include "greedy_insert.h"
#include "map.h"
#include "mask.h"
#include "image.h"

extern GreedySubdivision	*mesh;
extern Map					*DEM;
extern ImportMask			*MASK;
extern TGAFile				*TGA;

extern bool					strip_hod;
extern unsigned long		point_limit;
extern unsigned long		partition_count_hod;
extern double				error_threshold;

// greedy.cpp
extern bool	greedy_insertion();
extern void	scripted_preinsertion(std::istream&);
extern void	subsample_insertion(int target_width);

// output.cpp
extern bool	output_ply(const char *filename);

enum geometry {
	plane,
	curve,
	sphere,
	hemisphere,
};

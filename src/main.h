#ifndef TERRA_INCLUDED // -*- C++ -*-
#define TERRA_INCLUDED

#include <string>
#include "greedy_insert.h"
#include "map.h"
#include "mask.h"
#include "image.h"

enum FileFormat {
	NULLfile, 
	OBJfile, 
	HODfile,
	PLYfile
};

extern GreedySubdivision	*mesh;
extern Map					*DEM;
extern ImportMask			*MASK;
extern TGAFile				*TGA;

extern bool					strip_hod;
extern unsigned long		point_limit;
extern unsigned long		partition_count_hod;
extern real					error_threshold;

//extern std::string   		output_filename_p;

// greedy.cpp
extern int	goal_not_met();
extern void	greedy_insertion();
extern void	scripted_preinsertion(std::istream&);
extern void	subsample_insertion(int target_width);

// output.cpp
extern void	generate_output(const char *filename=NULL, FileFormat format=NULLfile);
extern void	output_obj(const char *filename);
extern void	output_ply(const char *filename);
extern void	output_hod(const char *filename);

// cmdline.cpp
extern bool	process_cmdline(int nNumArgs, const char *pszArgs[]);

#endif

#include "Main.h"
#include <iostream>

void scripted_preinsertion(std::istream& script)
{
    char op[4];
    int x, y;

    while(script.peek() != EOF)
    {
		script >> op >> x >> y;

		switch(op[0]) {
			case 's':
				if(!mesh->is_used(x, y)) {
					mesh->select(x, y);
					mesh->is_used(x, y) = DATA_POINT_USED;
				}

				break;
			default:
				break;
		}
    }
}

void subsample_insertion(int target_width)
{
    int width = DEM->width;
    int height = DEM->height;

    // 'i' is the target width and 'j' is the target height

    real i = (real)target_width;
    real j = real((i*height) / width);

    real dx = (width-1)/(i-1);
    real dy = (height-1)/(j-1);

    real u, v;
    int x, y;
    for(u=0; u<i; u++)
	for(v=0; v<j; v++)
	{
	    x =  (int)real(u*dx);
	    y =  (int)real(v*dy);

	    if( !mesh->is_used(x,y) )
		mesh->select(x, y);
	}
}

inline int goal_not_met()
{
    return mesh->maxError() > error_threshold &&
	   mesh->pointCount() < point_limit;
}

#include <sstream>
extern std::stringstream used_options;

static void announce_goal()
{
	std::cout << used_options.str();
	std::cout << " (points: " << mesh->pointCount() << " points, max-error: " << mesh->maxError() << ")";
	std::cout << "\r[ OK ]\n";

/*
    std::cerr << std::endl << "Goal conditions met:" << std::endl;
    std::cerr << "     error=" << mesh->maxError()
	 << " [thresh="<< error_threshold << "]" << std::endl;
    std::cerr << "     points=" << mesh->pointCount()
	 << " [limit=" << point_limit << "]" << std::endl;
*/
}

void greedy_insertion()
{

    while( goal_not_met() )
    {
	if( !mesh->greedyInsert() )
	    break;
    }

    announce_goal();
}

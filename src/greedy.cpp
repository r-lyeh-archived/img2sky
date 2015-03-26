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

    double i = (double)target_width;
    double j = double((i*height) / width);

    double dx = (width-1)/(i-1);
    double dy = (height-1)/(j-1);

    double u, v;
    int x, y;
    for(u=0; u<i; u++)
    for(v=0; v<j; v++)
    {
        x =  (int)double(u*dx);
        y =  (int)double(v*dy);

        if( !mesh->is_used(x,y) )
        mesh->select(x, y);
    }
}

inline int goal_not_met() {
    return mesh->maxError() > error_threshold && mesh->pointCount() < point_limit;
}

bool greedy_insertion() {
    while( goal_not_met() ) {
        if( !mesh->greedyInsert() )
            break;
    }
    return true;
}

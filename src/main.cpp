#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <sstream>
#include <vector>

#include "spot.hpp"
#include "Main.h"

#define IMG2SKY_BUILD "DEBUG"
#define IMG2SKY_URL "https://github.com/r-lyeh/img2sky"
#define IMG2SKY_TEXT "img2sky " IMG2SKY_VERSION " (" IMG2SKY_BUILD ")"
#define IMG2SKY_VERSION "0.0.3" /* options for plane and custom curved planes and hemispheres (@perfectforwarding); Bugfixed vertically flipped images (@r-lyeh)
#define IMG2SKY_VERSION "0.0.2" // customizable radius sphere
#define IMG2SKY_VERSION "0.0.1" // initial revision */

#if defined(NDEBUG) || defined(_NDEBUG)
#undef  IMG2SKY_BUILD
#define IMG2SKY_BUILD "RELEASE"
#endif

GreedySubdivision   *mesh;
Map                 *DEM;
ImportMask          *MASK;
TGAFile             *TGA;

static ImportMask   default_mask;

// quality [0..100] as error treshold [0..INF]
double Q( double q ) {
    return abs(-log(q*q/10000.0));
}

unsigned        make_geometry       = plane;
bool            strip_hod           = true;
unsigned long   point_limit         = -1;
double          error_threshold     = Q(50.00); // defaults to quality 50%
double          radius              = 10;       // default radius size (or planar displacement when make_curved is selected)

std::vector<std::string> to_process;
std::stringstream used_options;

std::string lowercase( std::string input ) {
    for( auto &ch : input ) {
        if( ch >= 'A' && ch <= 'Z' ) ch = ch - 'A' + 'a';
    }
    return input;
}

void help( const std::string &arg0 ) {
    std::stringstream cout;

    std::cout << arg0 << ": " << IMG2SKY_TEXT ". Compiled on " __DATE__ " - " IMG2SKY_URL << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "    img2sky [options] input.img [...]" << std::endl;
    std::cout << "        -q,--quality float       Quality for vertex density [0..100] (lowest..highest) (default: 50.00)" << std::endl;
    std::cout << "        -v,--vertices integer    Specify maximum number of vertices [4..N] (default: disabled)" << std::endl;
    std::cout << "        -p,--plane               Create plane mesh of given (default geometry)" << std::endl;
    std::cout << "        -s,--sphere float        Create sphere mesh of given float radius (default: 10.00)" << std::endl;
    std::cout << "        -h,--hemisphere float    Create hemisphere mesh of given float radius (default: 10.00)" << std::endl;
    std::cout << "        -c,--curve float         Create a curved plane, displacement along z can be specified next (default: 10.00)" << std::endl;
    std::cout << "        -f,--fast                Disable triangle stripification (default: slow)" << std::endl;
    std::cout << std::endl;

    std::string sep;
    std::cout << "    img2sky reads";
    for( auto &fmt : spot::list_supported_inputs() ) {
        std::cout << sep << " ." << fmt;
        sep = ',';
    }
    std::cout << " texture files." << std::endl;
    std::cout << "    img2sky writes .ply mesh files." << std::endl << std::endl;

    std::cout << "Quality and number of vertices should be mutually exclusive options. You can specify both at same time, but does not make much sense at all." << std::endl;
    std::cout << "And if you are the expensive/smooth meshes guy kind, then use larger textures, increase quality and/or number of vertices." << std::endl;

    std::cout << std::endl;
}

bool parse_cmdline( int argc, const char **argv ) {

    // Print message
    if (argc == 1) {
        help(argv[0]);
        return true; 
    }

    // Process command-line options.
    for (int i = 1; i < argc; i++) {
        std::string sarg = lowercase(argv[i]);
        const char *arg = sarg.c_str();

        // Help
        if (!strcmp(arg, "-?") || !strcmp(arg, "--help") || !strcmp(arg, "/?")) {
            help(argv[0]);
            return true;
        }

        // Set error threshold (~vertex density; ~quality)
        if (i + 1 < argc)
        if (!strcmp(arg, "-q") || !strcmp(arg, "--quality") || !strcmp(arg, "/quality")) {
            float quality = fabsf(atof(argv[++i]));
            error_threshold = Q(quality > 100 ? 100 : quality);

            used_options << " (error-threshold: " << float(error_threshold) << ")";
            continue;
        }

        // Set vertex limit
        if (!strcmp(arg, "-v") || !strcmp(arg, "--vertices") || !strcmp(arg, "/vertices")) {
            if(i < argc - 1) {
                auto d = strtoul(argv[++i], 0, 0);
                point_limit = ( d < 4 ? 4 : d );
            }
            used_options << " (point-limit: " << point_limit << ")";
            continue;
        }

        // Set mesh type
        if (!strcmp(arg, "-p") || !strcmp(arg, "--plane") || !strcmp(arg, "/plane")) {
            make_geometry = plane;
            used_options << " (plane)";
            continue;
        }
        if (!strcmp(arg, "-s") || !strcmp(arg, "--sphere") || !strcmp(arg, "/sphere")) {
            make_geometry = sphere;
            if(i < argc - 1) {
                double d = atof(argv[++i]);
                radius = ( d < 1 ? 1 : d );
            }
            used_options << " (sphere-radius: " << radius << ")";
            continue;
        }
        if (!strcmp(arg, "-h") || !strcmp(arg, "--hemisphere") || !strcmp(arg, "/hemisphere")) {
            make_geometry = hemisphere;
            if(i < argc - 1) {
                double d = atof(argv[++i]);
                radius = ( d < 1 ? 1 : d );
            }
            used_options << " (hemisphere-radius: " << radius << ")";
            continue;
        }
        if (!strcmp(arg, "-c") || !strcmp(arg, "--curve") || !strcmp(arg, "/curve")) {
            make_geometry = curve;
            if(i < argc - 1) {
                double d = atof(argv[++i]);
                radius = ( d < 1 ? 1 : d );
            }
            used_options << " (curve-displacement: " << radius << ")";
            continue;
        }

        // Disable Stripification
        if (!strcmp(arg, "-f") || !strcmp(arg, "--fast") || !strcmp(arg, "/fast")) {
            strip_hod = false;
            used_options << " (stripification: disabled)";
            continue;
        }

        // Nothing else. Must be a filename.
        to_process.push_back( argv[i] );
    }

    return true;
}

int main(int argc, const char **argv) {

    if( !parse_cmdline( argc, argv ) ) {
        return -1;
    }

    if( to_process.empty() && argc > 1 ) {
        return -1;
    }

    unsigned errors = 0;
    for( auto &file : to_process ) {
        // Set the output filename.
        auto ply = std::string() + file;
        /**/ if( make_geometry == curve )      ply += ".curve";
        else if( make_geometry == sphere )     ply += ".sphere";
        else if( make_geometry == plane )      ply += ".plane";
        else if( make_geometry == hemisphere ) ply += ".hemisphere";
        else /* ? */;
        ply += ".ply";

        ////////////////////////////////////////////////////////////////
        std::cout << "[    ] " << file << " -> " << ply;
        DEM = readTGA(file.c_str());

        if(!DEM) {
            std::cout << "(unable to load image)\r[FAIL]\n";
            ++errors;
            continue;
        }

        MASK = &default_mask;

        mesh = new GreedySubdivision(DEM);

        ////////////////////////////////////////////////////////////////
        if (point_limit == -1)
            point_limit = DEM->width * DEM->height;

        bool local_error = true;
        if( greedy_insertion() ) {
            std::cout << used_options.str();
            std::cout << " (max-error: " << mesh->maxError() << ")";
            std::cout << " (vertices: " << mesh->pointCount() << ")";
            /*
            std::cout << std::endl << "Goal conditions met:" << std::endl;
            std::cout << "     error=" << mesh->maxError()
             << " [thresh="<< error_threshold << "]" << std::endl;
            std::cout << "     points=" << mesh->pointCount()
             << " [limit=" << point_limit << "]" << std::endl;
            */
            if( output_ply(ply.c_str()) ) {
                local_error = false;
            }
        }

        std::cout << ( local_error ? "\r[FAIL]\n" : "\r[ OK ]\n" );
        errors += local_error;

        if( TGA ) {
            TGA->CleanUp();     
        } 
    }

    return errors ? -1 : 0;
}

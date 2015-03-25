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
#define IMG2SKY_VERSION "0.0.0"
#define IMG2SKY_TEXT "img2sky " IMG2SKY_VERSION " (" IMG2SKY_BUILD ")"

#if defined(NDEBUG) || defined(_NDEBUG)
#undef  IMG2SKY_BUILD
#define IMG2SKY_BUILD "RELEASE"
#endif

std::string head( const std::string &appname ) {
    std::stringstream cout;
    cout << appname << ": " << IMG2SKY_BUILD ". Compiled on " __DATE__ " - " IMG2SKY_URL;
    return cout.str();
}

GreedySubdivision   *mesh;
Map                 *DEM;
ImportMask          *MASK;
TGAFile             *TGA;

static ImportMask   default_mask;

bool            make_sphere         = false;
bool            strip_hod           = true;
unsigned long   point_limit         = -1;
unsigned long   partition_count_hod = 2;
real            error_threshold     = 2.f;

std::vector<std::string> to_process;
std::stringstream used_options;

using namespace std;

std::string lowercase( std::string input ) {
    for( auto &ch : input ) {
        if( ch >= 'A' && ch <= 'Z' ) ch = ch - 'A' + 'a';
    }
    return input;
}

void help() {
    std::cout << head("img2sky") << std::endl;

    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "    img2sky [options] input.img [...]" << std::endl;
    std::cout << "        -q,--quality integer   Quality polygon setting [0..100] (lowest..highest) (default: 90)" << std::endl;
    std::cout << "        -v,--vertices integer  Specify maximum number of vertices (default: disabled)" << std::endl;
    std::cout << "        -f,--fast              Disable stripfication (default: enabled)" << std::endl;
    std::cout << "        -s,--sphere            Create sphere mesh (default: plane)" << std::endl;
    std::cout << std::endl;

    std::string sep;
    std::cout << "    img2sky reads";
    for( auto &fmt : spot::list_supported_inputs() ) {
        std::cout << sep << " ." << fmt;
        sep = ',';
    }
    std::cout << " texture files." << std::endl;
    std::cout << "    img2sky writes .ply mesh files." << std::endl << std::endl;

    std::cout << "Quality and number of vertices should be mutually exclusive options. Ie, you cannot specify both at same time." << std::endl;
    std::cout << "If you are the big/expensive meshes guy, use any of { large textures, high quality setting and/or high number of vertex settings };" << std::endl;
    std::cout << "else use any of { small textures, low quality setting and/or low number of vertex settings }." << std::endl;

    std::cout << std::endl;
}

bool process_cmdline(int nNumArgs, const char *pszArgs[]) {

    // Print message
    if (nNumArgs == 1) {
        help();
        return true; 
    }

    // Retrive the filename;
    static char szFilename[_MAX_PATH];
    static char szFilenameO[_MAX_PATH]; // OBJ
    static char szFilenameH[_MAX_PATH]; // HOD
    static char szFilenameP[_MAX_PATH]; // PLY

    static char szFilenameM[_MAX_PATH]; // Mask
    static char szFilenameS[_MAX_PATH]; // Script

    bool bOptionSet = false;

    szFilename[0] = 
    szFilenameO[0] = 
    szFilenameH[0] = 
    szFilenameP[0] = 
    szFilenameM[0] = 
    szFilenameS[0] = 0;

    // Process command-line options.
    for (int i = 1; i < nNumArgs; i++) {
        std::string sarg = lowercase(pszArgs[i]);
        const char *arg = sarg.c_str();

        // Help
        if (!strcmp(arg, "/?") || !strcmp(arg, "-h") || !strcmp(arg, "--help") ) {
            help();
            return 0;
        }

        // Set error threshold.
        if (strstr(arg, "-e") || strstr(arg, "/e")) {
            string szTmp = arg;
            szTmp.erase(0, 3);
            error_threshold = fabsf(atof(szTmp.c_str()));
            bOptionSet = true;

            used_options << " (error-threshold: " << (float)error_threshold << ")";
            continue;
        }

        // Set point limit.
        if (strstr(arg, "-v") || strstr(arg, "/v")) {
            string szTmp = arg;
            szTmp.erase(0, 3);
            point_limit = (unsigned long)abs(atol(szTmp.c_str()));
            bOptionSet = true;

            used_options << " (point-limit: " << point_limit << ")";
            continue;
        }

        // Set mesh type
        if (strstr(arg, "-p") || strstr(arg, "--plane") || strstr(arg, "/plane")) {
            make_sphere = false;
            used_options << " (plane)";
            continue;
        }
        if (strstr(arg, "-s") || strstr(arg, "--sphere") || strstr(arg, "/sphere")) {
            make_sphere = true;
            used_options << " (sphere)";
            continue;
        }

        // Disable Stripification
        if (strstr(arg, "-f") || strstr(arg, "--fast") || strstr(arg, "/fast")) {
            strip_hod = false;
            used_options << " (stripification: disabled)";
            continue;
        }

        // Nothing else. Must be a filename.
        to_process.push_back( pszArgs[i] );
    }

    return true;
}

int main(int argc, const char **argv) {
    unsigned errors = 0;

    if( process_cmdline(argc, argv) ) {
        return -1;
    }

    for( auto &file : to_process ) {
        // Set the output filename.
        auto output_filename_p = std::string() + file + ".ply";

        ////////////////////////////////////////////////////////////////
        DEM = readTGA(file.c_str());

        if(!DEM) {
            cout << "(unable to load image)\r[FAIL]\n";
            ++errors;
            continue;
        }

        MASK = &default_mask;

        mesh = new GreedySubdivision(DEM);

        ////////////////////////////////////////////////////////////////
        if (point_limit == -1)
            point_limit = DEM->width * DEM->height;

        greedy_insertion();
        generate_output(output_filename_p.c_str(), PLYfile);

        if( TGA ) {
            TGA->CleanUp();     
        } 
    }

    return errors ? -1 : 0;
}

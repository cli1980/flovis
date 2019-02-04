/**
 * @file main
 *       main entrance of flo visualizer
 */
#include <iostream>
#include <string>
#include "flow_visualizer.h"

using namespace std;

static void help(const string& prog)
{
    cout << "Usage: " << prog << " path the .flo file" << endl;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        help(argv[0]);
        return 0;
    }

    FlowVisualizer flovis;
    if (!flovis.load(argv[1]))
    {
        cout << "Failed to load file " << argv[1] << endl;
        return -1;
    }

    flovis.show();
    
    return 0;
}

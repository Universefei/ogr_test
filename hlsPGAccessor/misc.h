#ifndef MISC_H
#define MISC_H

#include <vector>
#include <list>
#include <utility>
using namespace std;

typedef vector<string> fieldVec;

typedef list< pair< char*,int>* > BufList;

struct BBOX
{
    double xmin;
    double ymin;
    double xmax;
    double ymax;
};

#endif

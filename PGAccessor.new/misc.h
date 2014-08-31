#ifndef MISC_H
#define MISC_H

#include <vector>
#include <list>
#include <string>
#include <pthread.h>
#include <utility>

using namespace std;

struct BBOX
{
    double xmin;
    double ymin;
    double xmax;
    double ymax;
};

typedef pair< char*, int> JsonSeg;

typedef vector<string> fieldVec;

//typedef list< pair< char*,int>* > BufList;
typedef list< JsonSeg* > BufList;


class RsltNode
{
public:
    pthread_mutex_t mutex_;
    BufList*        pcontent;
    //list< pair<char*,int> * >* pcontent;
    RsltNode();
    ~RsltNode();
};

typedef list< RsltNode* > RsltList;

#endif // MISC_H

#ifndef UTILITIES_H
#define UTILITIES_H

#include "misc.h"

bool IsQueEmpty(RsltNode rsltNode);

/**
 * @brief:  pop a node from result list.
 *
 * @return: 1 if reach the end and returned pair is unuseable, else return 0.
 * @ppair:  a BufList node get from the front.
 */
int GetNextJsonSeg(RsltNode rsltNode,pair<char*,int>** ppair);

#endif // UTILITIES_H

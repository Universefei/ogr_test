#include "misc.h"


RsltNode::RsltNode()
{
    pcontent = new BufList();
}

RsltNode::~RsltNode()
{
    delete pcontent;
}

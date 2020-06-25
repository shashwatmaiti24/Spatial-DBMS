#include "node.hpp"

Node::Node(int _d,int _maxCap,bool leaf){
    this->d=d;
    this->maxCap=_maxCap;
    this->leaf=leaf;
}
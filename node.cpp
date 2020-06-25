#include "node.hpp"

Node::Node(){
    id=-1;
    parentId=-1;
}
Node::Node(int maxcap,int d){
    id=-1;
    parentId=-1;
    mbr.resize(2*d,INT32_MIN);
    for(int i=0;i<d;i++) mbr[i]=INT32_MAX;
    childId.resize(maxcap,INT32_MIN);
    childMbr.resize(maxcap);
    for(int i=0;i<maxcap;i++){
        childMbr[i].resize(2*d,INT32_MIN);
        for(int j=0;j<d;j++) childMbr[i][j]=INT32_MAX;
    }
}
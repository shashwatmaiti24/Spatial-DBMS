#ifndef RTREE_HPP_INCLUDED__
#define RTREE_HPP_INCLUDED__

#include "node.hpp"
class RTree{
public:
	Node *root;
	int d;
	int maxCap;

	RTree(int _d, int _maxCap);
    void insert(std::vector<int> P); //Vector of D dimension provided
    void inserrt(int xNode,int d,int maxCap,FileHandler& fh,int nodeSize,std::vector<int> P,int& nodeIDCtr);//Helper Function for insert
    void bulkLoad(char* filename); 
    Node ChooseLeaf(Node RN,std::vector<int>P);
    Node SplitNode(std::vector<int> P, int newentryid, Node L);
};
RTree::RTree(int _d,int _maxCap){
    d=_d;
    maxCap=_maxCap;
}
// 	Node Insert(int P[d], int n){
// //		get the root node from the page
// 		Node RN;
// 		Node Leaf = ChooseLeaf(RN, P);
// 		if(maxCap>Leaf.nchildren){
// 			for(int i=0; i<d; i++){
// 				Leaf.children[nchildren][i+d] = P[i];
// 			}
// 			return RN;
// 		}
// //      Code from step 10
// 	}
	

#endif
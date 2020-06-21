#include <iostream>
#include <bits/stdc++.h>
using namespace std;
class Node{
	int d;
	int maxCap;
	bool leaf;
	int id;
	int *mbr;
	int parent;
	int *children;
	int **childrenmbr;
	int nchildren;
public:
	Node(int _d, int _maxCap, bool _leaf);
//	Node *search(int k);
friend class Rtree;
}

class RTree{
	Node *root;
	int d;
	int maxCap;
public:
	RTree(int _d, int _maxCap){
		root = NULL; d=_d; maxCap=_maxCap;
	}
	Node Insert(int P[d], int n){
//		get the root node from the page
		Node RN;
		Node Leaf = ChooseLeaf(RN, P);
		if(maxCap>Leaf.nchildren){
			for(int i=0; i<d; i++){
				Leaf.children[nchildren][i+d] = P[i];
			}
			return RN;
		}
//      Code from step 10
	}
	Node ChooseLeaf(Node N, int[d] P){
		for(int i=0; i<d; i++){
			N.mbr[i] = min(N.mbr[i],N.mbr[i+d],P[i]);
			N.mbr[i+d] = max(N.mbr[i],N.mbr[i+d],P[i]);
		}
		if(N.leaf){
			return N;
		}
		int minarea;
		int area=1;
		int newmbr[2*d];	
		for(int i=0; i<d; i++){
			area*=(childrenmbr[0][i]-childrenmbr[0][i+d]);
			newmbr[i] = min(childrenmbr[0][i],childrenmbr[0][i+d],P[i]);
			newmbr[i+d] = max(childrenmbr[0][i],childrenmbr[0][i+d],P[i]);
		}
		newarea=1;
		for(int i=0; i<d; j++){
			newarea*=(newmbr[i]-newmbr[i+d]);
		}
		minarea=area;
		int mindiff = 0;
		int mindiffarea = newarea-area;
		int i = 1;
		while(i<maxCap and children[i]!=-1){
			area=1;
			int newmbr[2*d];
			for(int j=0; j<d; j++){
				area*=(childrenmbr[i][j]-childrenmbr[i][j+d]);
				newmbr[j] = min(childrenmbr[i][j],childrenmbr[i][j+d],P[j]);
				newmbr[j+d] = max(childrenmbr[i][j],childrenmbr[i][j+d],P[j]);
			}
			newarea=1;
			for(int j=0; j<d; j++){
				newarea*=(newmbr[j]-newmbr[j+d]);
			}
			if(mindiffarea>newarea-area){
				mindiffarea=newarea-area;
				mindiff=i;
				minarea = area;
			}else if (mindiffarea==newarea-area){
				if(minarea>area){
					minarea = area;
					mindiff = i;
				}
			}
			i++
		}
		// Get the node corresponding to mindiff from page
		N;
		ChooseLeaf(N, P)
	}
}

Node::Node(int _d, int _maxCap, bool _leaf){
	d = _d;
	maxCap = _maxCap;
	leaf = _leaf;
	mbr = new int[2*d];
	children = new int[maxCap];
	childrenmbr = new int[maxCap][2*d];
	for(int i = 0; i<maxCap; i++){
		children[i]=INT_MIN;
	}
	if(leaf){
		for (int i = 0; i < maxCap; i++){
			for(int j = 0; j<d; i++){
				childrenmbr[i][j]=INT_MIN;
			}
		}
		for(int i = 0; i<maxCap; i++){
			children[i]=-1;
		}
	}
}
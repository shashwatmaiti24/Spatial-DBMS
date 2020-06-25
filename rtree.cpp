#include "rtree.hpp"
#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>
using namespace std;

void RTree::inserrt(int xNode, std::vector<int> P, const char* filename){
//		get the root node from the page
    FileManager fm1;
    FileHandler fh1=fm1.OpenFile(filename);
	int nodeSize = sizeof(Node);
    int nodePerPage=PAGE_SIZE/nodeSize;
	int pageNum = xNode/nodePerPage;
	PageHandler ph = fh1.PageAt(pageNum);
	char *data = ph.GetData ();
    Node RN;
	memcpy (&RN, &data[xNode%nodePerPage], sizeof(Node));
	Node Leaf = ChooseLeaf(RN, P);
	if(maxCap>Leaf.nchild){
		for(int i=0; i<d; i++){
			Leaf.childMbr[Leaf.nchild][i+d] = P[i];
		}
		Leaf.childId[Leaf.nchild] = 0;
		Leaf.nchild+=1;
		return;
	}
	Node NN = SplitNode(P, -1, Leaf);
	while(Leaf.parentId!=RN.id){
		Node parent;
		memcpy (&parent, &data[0], sizeof(Node));
		if(maxCap>parent.nchild){
			for(int i=0; i<d; i++){
				parent.childMbr[parent.nchild][i+d] = P[i];
			}
			parent.childId[parent.nchild] = NN.id;
			parent.nchild+=1;
		}
		Node NN = SplitNode(NN.mbr, NN.id, parent);
	}
//      Code from step 10
}
Node RTree::ChooseLeaf(Node N, std::vector<int>P){
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
		area*=(N.childMbr[0][i]-N.childMbr[0][i+d]);
		newmbr[i] = min(N.childMbr[0][i],N.childMbr[0][i+d],P[i]);
		newmbr[i+d] = max(N.childMbr[0][i],N.childMbr[0][i+d],P[i]);
	}
	int newarea=1;
	for(int i=0; i<d; i++){
		newarea*=(newmbr[i]-newmbr[i+d]);
	}
	minarea=area;
	int mindiff = 0;
	int mindiffarea = newarea-area;
	int i = 1;
	while(i<maxCap and N.childId[i]!=-1){
		area=1;
		int newmbr[2*d];
		for(int j=0; j<d; j++){
			area*=(N.childMbr[i][j]-N.childMbr[i][j+d]);
			newmbr[j] = min(N.childMbr[i][j],N.childMbr[i][j+d],P[j]);
			newmbr[j+d] = max(N.childMbr[i][j],N.childMbr[i][j+d],P[j]);
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
		i++;
	}
	// Get the node corresponding to mindiff from page
	N;
	ChooseLeaf(N, P);
}
Node RTree::SplitNode(std::vector<int> P, int newentryid, Node L){
	Node LL;
	if (L.leaf)
	{
		int maxdist = 0;
		int s1 = 0;
		int s2 = 1;
		for (int i = 0; i < maxCap-1; ++i)
		{
			for (int j = i+1; j < maxCap; ++j)
			{
				int dist = 0;
				for(int k = d; k<2*d; ++k){
					int t = (L.childMbr[i][k]-L.childMbr[j][k]);
					dist+=t*t;
				}
				if(dist>maxdist){
					dist = maxdist;
					s1 = i;
					s2 = j;
				}
			}
		}
		int dist = 0;
		for(int k = d; k<2*d; ++k){
			int t = (L.childMbr[s1][k]-P[k]);
			dist+=t*t;
		}
		if(dist>maxdist){
			dist = maxdist;
			s2 = -1;
		}
		dist = 0;
		for(int k = d; k<2*d; ++k){
			int t = (L.childMbr[s2][k]-P[k]);
			dist+=t*t;
		}
		if(dist>maxdist){
			dist = maxdist;
			s1 = -1;
		}
		for(int k = d; k<2*d; ++k){
			if(s1!=-1){
				L.childMbr[0][k] = L.childMbr[s1][k];
				L.mbr[k-d] = L.childMbr[s1][k];
				L.mbr[k] = L.childMbr[s1][k];
			}else{
				L.childMbr[0][k] = P[k];
				L.mbr[k-d] = P[k];
				L.mbr[k] = P[k];
			}
		}
		for(int k = d; k<2*d; ++k){
			if(s2!=-1){
				LL.childMbr[0][k] = L.childMbr[s2][k];
				LL.mbr[k-d] = L.childMbr[s2][k];
				LL.mbr[k] = L.childMbr[s2][k];
			}else{
				LL.childMbr[0][k] = P[k];
				LL.mbr[k-d] = P[k];
				LL.mbr[k] = P[k];
			}
		}
		int area1=0;
		int area2=0;
		int e1 = 1;
		int e2 = 1;
		for(int i = 0; i < maxCap; ++i){
			if(i!=s1 and i!=s2){
				std::vector<int> newmbr1;
				std::vector<int> newmbr2;	
				for(int j=0; j<d; j++){
					newmbr1[j] = min(L.mbr[j],L.childMbr[i][j+d],L.mbr[j+d]);
					newmbr1[j+d] = max(L.mbr[j],L.childMbr[i][j+d],L.mbr[j+d]);
					newmbr2[j] = min(L.mbr[j],L.childMbr[i][j+d],L.mbr[j+d]);
					newmbr2[j+d] = max(L.mbr[j],L.childMbr[i][j+d],L.mbr[j+d]);
				}
				int newarea1=1;
				int newarea2=1;
				for(int i=0; i<d; i++){
					newarea1*=(newmbr1[i]-newmbr1[i+d]);
					newarea2*=(newmbr2[i]-newmbr2[i+d]);
				}
				if(newarea1==newarea2){
					if(area1==area2){
						if(e1<e2){
							L.mbr = newmbr1;
							area1 = newarea1;
							for(int k = d; k<2*d; ++k){
								L.childMbr[e1][k] = L.childMbr[i][k];
							}
							L.childId[e1] = 0;
						}else{
							LL.mbr = newmbr2;
							area2 = newarea2;
							for(int k = d; k<2*d; ++k){
								LL.childMbr[e2][k] = L.childMbr[i][k];
							}
							LL.childId[e2] = 0;

						}
					}else if(area1<area2){
						L.mbr = newmbr1;
						area1 = newarea1;
						for(int k = d; k<2*d; ++k){
							L.childMbr[e1][k] = L.childMbr[i][k];
							
						}
						L.childId[e1] = 0;
					}else{
						LL.mbr = newmbr2;
						area2 = newarea2;
						for(int k = d; k<2*d; ++k){
							LL.childMbr[e2][k] = L.childMbr[i][k];
							
						}
						LL.childId[e2] = 0;
					}
				}else if(newarea1<newarea2){
					L.mbr = newmbr1;
					area1 = newarea1;
					for(int k = d; k<2*d; ++k){
						L.childMbr[e1][k] = L.childMbr[i][k];
						
					}
					L.childId[e1] = 0;
				}else{
					LL.mbr = newmbr2;
					area2 = newarea2;
					for(int k = d; k<2*d; ++k){
						LL.childMbr[e2][k] = L.childMbr[i][k];
						
					}
					LL.childId[e2] = 0;
				}
			}
		}
		if(s1!=-1 and s2!=-1){
			std::vector<int> newmbr1;
			std::vector<int> newmbr2;	
			for(int j=0; j<d; j++){
				newmbr1[j] = min(L.mbr[j],P[d],L.mbr[j+d]);
				newmbr1[j+d] = max(L.mbr[j],P[d],L.mbr[j+d]);
				newmbr2[j] = min(L.mbr[j],P[d],L.mbr[j+d]);
				newmbr2[j+d] = max(L.mbr[j],P[d],L.mbr[j+d]);
			}
			int newarea1=1;
			int newarea2=1;
			for(int i=0; i<d; i++){
				newarea1*=(newmbr1[i]-newmbr1[i+d]);
				newarea2*=(newmbr2[i]-newmbr2[i+d]);
			}
			if(newarea1==newarea2){
				if(area1==area2){
					if(e1<e2){
						L.mbr = newmbr1;
						area1 = newarea1;
						for(int k = d; k<2*d; ++k){
							L.childMbr[e1][k] = P[k];
							
						}
						L.childId[e1] = 0;
					}else{
						LL.mbr = newmbr2;
						area2 = newarea2;
						for(int k = d; k<2*d; ++k){
							LL.childMbr[e2][k] = P[k];
							
						}
						LL.childId[e2] = 0;
					}
				}else if(area1<area2){
					L.mbr = newmbr1;
					area1 = newarea1;
					for(int k = d; k<2*d; ++k){
						L.childMbr[e1][k] = P[k];
						
					}
					L.childId[e1] = 0;
				}else{
					LL.mbr = newmbr2;
					area2 = newarea2;
					for(int k = d; k<2*d; ++k){
						LL.childMbr[e2][k] = P[k];
						
					}
					LL.childId[e2] = 0;
				}
			}else if(newarea1<newarea2){
				L.mbr = newmbr1;
				area1 = newarea1;
				for(int k = d; k<2*d; ++k){
					L.childMbr[e1][k] = P[k];
					
				}
				L.childId[e1] = 0;
			}else{
				LL.mbr = newmbr2;
				area2 = newarea2;
				for(int k = d; k<2*d; ++k){
					LL.childMbr[e2][k] = P[k];
					
				}
				LL.childId[e2] = 0;
			}
		}
	}
	else{
		int s1 = 0;
		int s2 = 1;
		int maxdiff = 0;
		for (int i = 0; i < maxCap-1; ++i)
		{
			for (int j = i+1; j < maxCap; ++j)
			{
				int jarea = 1;
				int area1 = 1;
				int area2 = 1;
				int jmbr[2*d];
				for(int k = 0; k<d; ++k){
					area1*=(L.childMbr[i][k+d]-L.childMbr[i][k]);
					area2*=(L.childMbr[j][k+d]-L.childMbr[j][k]);
					jmbr[k] = min(L.childMbr[i][k], L.childMbr[j][k]);
					jmbr[k+d] = max(L.childMbr[i][k+d], L.childMbr[j][k+d]);
				}
				for(int k = 0; k<d; ++k){
					jarea*=(jmbr[k+d]-jmbr[k]);
				}
				if(jarea-area1-area2>maxdiff){
					maxdiff = jarea-area1-area2;
					s1 = i;
					s2 = j;
				}
			}
		}
		int jarea1 = 1;
		int jarea2 = 1;
		int area1 = 1;
		int area2 = 1;
		int areap = 1;
		int jmbr1[2*d];
		int jmbr2[2*d];
		for(int k = 0; k<d; ++k){
			area1*=(L.childMbr[s1][k+d]-L.childMbr[s1][k]);
			area2*=(L.childMbr[s2][k+d]-L.childMbr[s2][k]);
			areap*=(P[k+d]-P[k]);
			jmbr1[k] = min(L.childMbr[s1][k], P[k]);
			jmbr1[k+d] = max(L.childMbr[s1][k+d], P[k+d]);
		}
		for(int k = 0; k<d; ++k){
			jarea1*=(jmbr1[k+d]-jmbr1[k]);
		}
		if(jarea1-area1-areap>maxdiff){
			maxdiff = jarea1-area1-areap;
			s2 = -1;
		}
		for(int k = 0; k<d; ++k){
			jarea1*=(jmbr1[k+d]-jmbr1[k]);
		}
		if(jarea1-area2-areap>maxdiff){
			maxdiff = jarea2-area2-areap;
			s1 = -1;
		}
		for(int k = 0; k<2*d; ++k){
			if(s1!=-1){
				L.childMbr[0][k] = L.childMbr[s1][k];
				L.mbr[k-d] = L.childMbr[s1][k];
				L.mbr[k] = L.childMbr[s1][k];
			}else{
				L.childMbr[0][k] = P[k];
				L.mbr[k-d] = P[k];
				L.mbr[k] = P[k];
			}
		}
		for(int k = 0; k<2*d; ++k){
			if(s2!=-1){
				LL.childMbr[0][k] = L.childMbr[s2][k];
				LL.mbr[k-d] = L.childMbr[s2][k];
				LL.mbr[k] = L.childMbr[s2][k];
			}else{
				LL.childMbr[0][k] = P[k];
				LL.mbr[k-d] = P[k];
				LL.mbr[k] = P[k];
			}
		}
		if(s1!=-1){
			L.childId[0] = L.childId[s1];
		}else{
			L.childId[0] = newentryid;
		}
		if(s2!=-1){
			LL.childId[0] = L.childId[s2];
		}else{
			LL.childId[0] = newentryid;
		}
		area1=0;
		area2=0;
		int e1 = 1;
		int e2 = 1;
		for(int i = 0; i < maxCap; ++i){
			if(i!=s1 and i!=s2){
				std::vector<int> newmbr1;
				std::vector<int> newmbr2;	
				for(int j=0; j<d; i++){
					newmbr1[j] = min(L.mbr[j],L.childMbr[i][j]);
					newmbr1[j+d] = max(L.mbr[j+d],L.childMbr[i][j+d]);
					newmbr2[j] = min(L.mbr[j],L.childMbr[i][j]);
					newmbr2[j+d] = max(L.mbr[j+d],L.childMbr[i][j+d]);
				}
				int newarea1=1;
				int newarea2=1;
				for(int j=0; j<d; j++){
					newarea1*=(newmbr1[j]-newmbr1[j+d]);
					newarea2*=(newmbr2[j]-newmbr2[j+d]);
				}
				if(newarea1==newarea2){
					if(area1==area2){
						if(e1<e2){
							L.mbr = newmbr1;
							area1 = newarea1;
							for(int k=0; k<2*d; ++k){
								L.childMbr[e1][k] = L.childMbr[i][k];
							}
							L.childId[e1] = L.childId[i];
						}else{
							LL.mbr = newmbr2;
							area2 = newarea2;
							for(int k=0; k<2*d; ++k){
								LL.childMbr[e2][k] = L.childMbr[i][k];
							}
							LL.childId[e2] = L.childId[i];
						}
					}else if(area1<area2){
						L.mbr = newmbr1;
						area1 = newarea1;
						for(int k=0; k<2*d; ++k){
							L.childMbr[e1][k] = L.childMbr[i][k];
						}
						L.childId[e1] = L.childId[i];
					}else{
						LL.mbr = newmbr2;
						area2 = newarea2;
						for(int k=0; k<2*d; ++k){
							LL.childMbr[e2][k] = L.childMbr[i][k];
						}
						LL.childId[e2] = L.childId[i];
					}
				}else if(newarea1<newarea2){
					L.mbr = newmbr1;
					area1 = newarea1;
					for(int k=0; k<2*d; ++k){
						L.childMbr[e1][k] = L.childMbr[i][k];
					}
					L.childId[e1] = L.childId[i];
				}else{
					LL.mbr = newmbr2;
					area2 = newarea2;
					for(int k=0; k<2*d; ++k){
						LL.childMbr[e2][k] = L.childMbr[i][k];
					}
					LL.childId[e2] = L.childId[i];
				}
			}
		}
		if(s1!=-1 and s2!=-1){
			std::vector<int> newmbr1;
			std::vector<int> newmbr2;	
			for(int j=0; j<d; j++){
				newmbr1[j] = min(L.mbr[j],P[j]);
				newmbr1[j+d] = max(L.mbr[j+d],P[j+d]);
				newmbr2[j] = min(L.mbr[j],P[j]);
				newmbr2[j+d] = max(L.mbr[j+d],P[j+d]);
			}
			int newarea1=1;
			int newarea2=1;
			for(int i=0; i<d; i++){
				newarea1*=(newmbr1[i]-newmbr1[i+d]);
				newarea2*=(newmbr2[i]-newmbr2[i+d]);
			}
			if(newarea1==newarea2){
				if(area1==area2){
					if(e1<e2){
						L.mbr = newmbr1;
						area1 = newarea1;
						for(int k=0; k<2*d; ++k){
							L.childMbr[e1][k] = P[k];
						}
						L.childId[e1] = newentryid;
					}else{
						LL.mbr = newmbr2;
						area2 = newarea2;
						for(int k=0; k<2*d; ++k){
							LL.childMbr[e2][k] = P[k];
						}
						LL.childId[e2] = newentryid;
					}
				}else if(area1<area2){
					L.mbr = newmbr1;
					area1 = newarea1;
					for(int k=0; k<2*d; ++k){
						L.childMbr[e1][k] = P[k];
					}
					L.childId[e1] = newentryid;
				}else{
					LL.mbr = newmbr2;
					area2 = newarea2;
					for(int k=0; k<2*d; ++k){
						LL.childMbr[e2][k] = P[k];
					}
					LL.childId[e2] = newentryid;
				}
			}else if(newarea1<newarea2){
				L.mbr = newmbr1;
				area1 = newarea1;
				for(int k=0; k<2*d; ++k){
					L.childMbr[e1][k] = P[k];
				}
				L.childId[e1] = newentryid;
			}else{
				LL.mbr = newmbr2;
				area2 = newarea2;
				for(int k=0; k<2*d; ++k){
					LL.childMbr[e2][k] = P[k];
				}
				LL.childId[e2] = newentryid;
			}
		}
	}
	return LL;
}
// Rtree::Node PickNext(int P[d], int childMbr[][], bool leaf, Node L){
// 	int area1=0;
// 	int area2=0;
// 	int e1 = 1;
// 	int e2 = 1;
// 	for(int i = 0; i < maxCap; ++i){
// 		if(i!=s1 and i!=s2){
// 			int newmbr1[2*d];
// 			int newmbr2[2*d];	
// 			for(int j=0; j<d; i++){
// 				newmbr1[j] = min(L.mbr[j],childMbr[i][j+d],L.mbr[j+d]);
// 				newmbr1[j+d] = max(L.mbr[j],childMbr[i][j+d],L.mbr[j+d]);
// 				newmbr2[j] = min(L.mbr[j],childMbr[i][j+d],L.mbr[j+d]);
// 				newmbr2[j+d] = max(L.mbr[j],childMbr[i][j+d],L.mbr[j+d]);
// 			}
// 			newarea1=1;
// 			newarea2=1;
// 			for(int i=0; i<d; j++){
// 				newarea1*=(newmbr1[i]-newmbr1[i+d]);
// 				newarea2*=(newmbr2[i]-newmbr2[i+d]);
// 			}
// 			if(newarea1==newarea2){
// 				if(area1==area2){
// 					if(e1<e2){
// 						L.mbr = newmbr1;
// 						area1 = newarea1;
// 					}else{
// 						LL.mbr = newmbr2;
// 						area2 = newarea2;
// 					}
// 				}else if(area1<area2){
// 					L.mbr = newmbr1;
// 					area1 = newarea1;
// 				}else{
// 					LL.mbr = newmbr2;
// 					area2 = newarea2;
// 				}
// 			}else if(newarea1<newarea2){
// 				L.mbr = newmbr1;
// 				area1 = newarea1;
// 			}else{
// 				LL.mbr = newmbr2;
// 				area2 = newarea2;
// 			}
// 		}
// 	}
// 	int newmbr1[2*d];
// 	int newmbr2[2*d];	
// 	for(int j=0; j<d; i++){
// 		newmbr1[j] = min(L.mbr[j],P[d],L.mbr[j+d]);
// 		newmbr1[j+d] = max(L.mbr[j],P[d],L.mbr[j+d]);
// 		newmbr2[j] = min(L.mbr[j],P[d],L.mbr[j+d]);
// 		newmbr2[j+d] = max(L.mbr[j],P[d],L.mbr[j+d]);
// 	}
// 	newarea1=1;
// 	newarea2=1;
// 	for(int i=0; i<d; j++){
// 		newarea1*=(newmbr1[i]-newmbr1[i+d]);
// 		newarea2*=(newmbr2[i]-newmbr2[i+d]);
// 	}
// 	if(newarea1==newarea2){
// 		if(area1==area2){
// 			if(e1<e2){
// 				L.mbr = newmbr1;
// 				area1 = newarea1;
// 			}else{
// 				LL.mbr = newmbr2;
// 				area2 = newarea2;
// 			}
// 		}else if(area1<area2){
// 			L.mbr = newmbr1;
// 			area1 = newarea1;
// 		}else{
// 			LL.mbr = newmbr2;
// 			area2 = newarea2;
// 		}
// 	}else if(newarea1<newarea2){
// 		L.mbr = newmbr1;
// 		area1 = newarea1;
// 	}else{
// 		LL.mbr = newmbr2;
// 		area2 = newarea2;
// 	}
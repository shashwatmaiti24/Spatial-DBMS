#include "rtree.hpp"

Rtree::Node Insert(int P[d], int n){
//		get the root node from the page
	Node RN;
	Node Leaf = ChooseLeaf(RN, P);
	if(maxCap>Leaf.nchildren){
		for(int i=0; i<d; i++){
			Leaf.children[nchildren][i+d] = P[i];
		}
		return RN;
	}
	SplitNode(Leaf, P);
//      Code from step 10
}
Rtree::Node ChooseLeaf(Node N, int[d] P){
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
Rtree::Node SplitNode(Node L, int P[d]){
	if (L.leaf)
	{
		PickSeeds(P, L.childrenmbr, L);
	}
}
Rtree::Node PickSeeds(int P[2*d], int childrenmbr[maxCap][2*d], Node L){
	Node LL = new Node;
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
				for(int k = d; k<2d; ++k){
					int t = (childrenmbr[i][k]-childrenmbr[j][k]);
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
		for(int k = d; k<2d; ++k){
			int t = (childrenmbr[i][k]-P[k]);
			dist+=t*t;
		}
		if(dist>maxdist){
			dist = maxdist;
			s2 = -1;
		}
		dist = 0;
		for(int k = d; k<2d; ++k){
			int t = (childrenmbr[j][k]-P[k]);
			dist+=t*t;
		}
		if(dist>maxdist){
			dist = maxdist;
			s1 = -1;
		}
		for(int k = d; k<2d; ++k){
			if(s1!=-1){
				L.childrenmbr[0][k] = childrenmbr[s1][k];
				L.mbr[k-d] = childrenmbr[s1][k];
				L.mbr[k] = childrenmbr[s1][k];
			}else{
				L.childrenmbr[0][k] = P[k];
				L.mbr[k-d] = P[k];
				L.mbr[k] = P[k];
			}
		}
		for(int k = d; k<2d; ++k){
			if(s2!=-1){
				LL.childrenmbr[0][k] = childrenmbr[s2][k];
				LL.mbr[k-d] = childrenmbr[s2][k];
				LL.mbr[k] = childrenmbr[s2][k];
			}else{
				LL.childrenmbr[0][k] = P[k];
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
				int newmbr1[2*d];
				int newmbr2[2*d];	
				for(int j=0; j<d; i++){
					newmbr1[j] = min(L.mbr[j],childrenmbr[i][j+d],L.mbr[j+d]);
					newmbr1[j+d] = max(L.mbr[j],childrenmbr[i][j+d],L.mbr[j+d]);
					newmbr2[j] = min(L.mbr[j],childrenmbr[i][j+d],L.mbr[j+d]);
					newmbr2[j+d] = max(L.mbr[j],childrenmbr[i][j+d],L.mbr[j+d]);
				}
				newarea1=1;
				newarea2=1;
				for(int i=0; i<d; j++){
					newarea1*=(newmbr1[i]-newmbr1[i+d]);
					newarea2*=(newmbr2[i]-newmbr2[i+d]);
				}
				if(newarea1==newarea2){
					if(area1==area2){
						if(e1<e2){
							L.mbr = newmbr1;
							area1 = newarea1;
						}else{
							LL.mbr = newmbr2;
							area2 = newarea2;
						}
					}else if(area1<area2){
						L.mbr = newmbr1;
						area1 = newarea1;
					}else{
						LL.mbr = newmbr2;
						area2 = newarea2;
					}
				}else if(newarea1<newarea2){
					L.mbr = newmbr1;
					area1 = newarea1;
				}else{
					LL.mbr = newmbr2;
					area2 = newarea2;
				}
			}
		}
		int newmbr1[2*d];
		int newmbr2[2*d];	
		for(int j=0; j<d; i++){
			newmbr1[j] = min(L.mbr[j],P[d],L.mbr[j+d]);
			newmbr1[j+d] = max(L.mbr[j],P[d],L.mbr[j+d]);
			newmbr2[j] = min(L.mbr[j],P[d],L.mbr[j+d]);
			newmbr2[j+d] = max(L.mbr[j],P[d],L.mbr[j+d]);
		}
		newarea1=1;
		newarea2=1;
		for(int i=0; i<d; j++){
			newarea1*=(newmbr1[i]-newmbr1[i+d]);
			newarea2*=(newmbr2[i]-newmbr2[i+d]);
		}
		if(newarea1==newarea2){
			if(area1==area2){
				if(e1<e2){
					L.mbr = newmbr1;
					area1 = newarea1;
				}else{
					LL.mbr = newmbr2;
					area2 = newarea2;
				}
			}else if(area1<area2){
				L.mbr = newmbr1;
				area1 = newarea1;
			}else{
				LL.mbr = newmbr2;
				area2 = newarea2;
			}
		}else if(newarea1<newarea2){
			L.mbr = newmbr1;
			area1 = newarea1;
		}else{
			LL.mbr = newmbr2;
			area2 = newarea2;
		}
	}
	else{
		int s1 = 0;
		int s2 = 1;
		int maxdiff = 0;
		int jprevmbr[2*d];
		for (int i = 0; i < maxCap-1; ++i)
		{
			for (int j = i+1; j < maxCap; ++j)
			{
				int jarea = 1;
				int area1 = 1;
				int area2 = 1;
				int jmbr[2*d];
				for(int k = 0; k<d; ++k){
					area1*=(childrenmbr[i][k+d]-childrenmbr[i][k]);
					area2*=(childrenmbr[j][k+d]-childrenmbr[j][k]);
					jmbr[k] = min(childrenmbr[i][k], childrenmbr[j][k]);
					jmbr[k+d] = max(childrenmbr[i][k+d], childrenmbr[j][k+d]);
				}
				for(int k = 0; k<d; ++k){
					jarea*=(jmbr[k+d]-jmbr[k]);
				}
				if(jarea-area1-area2>maxdiff){
					maxdiff = jarea-area1-area2;
					s1 = i;
					s2 = j;
					jprevmbr = jmbr;
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
			area1*=(childrenmbr[s1][k+d]-childrenmbr[s1][k]);
			area2*=(childrenmbr[s2][k+d]-childrenmbr[s2][k]);
			areap*=(P[j][k+d]-P[j][k]);
			jmbr1[k] = min(childrenmbr[s1][k], P[k]);
			jmbr1[k+d] = max(childrenmbr[s1][k+d], P[k+d]);
		}
		for(int k = 0; k<d; ++k){
			jarea1*=(jmbr1[k+d]-jmbr1[k]);
		}
		if(jarea1-area1-areap>maxdiff){
			maxdiff = jarea-area1-areap;
			s2 = -1;
		}
		for(int k = 0; k<d; ++k){
			jarea1*=(jmbr1[k+d]-jmbr1[k]);
		}
		if(jarea1-area2-areap>maxdiff){
			maxdiff = jarea-area2-areap;
			s1 = -1;
		}
		for(int k = 0; k<2d; ++k){
			if(s1!=-1){
				L.childrenmbr[0][k] = childrenmbr[s1][k];
				L.mbr[k-d] = childrenmbr[s1][k];
				L.mbr[k] = childrenmbr[s1][k];
			}else{
				L.childrenmbr[0][k] = P[k];
				L.mbr[k-d] = P[k];
				L.mbr[k] = P[k];
			}
		}
		for(int k = 0; k<2d; ++k){
			if(s2!=-1){
				LL.childrenmbr[0][k] = childrenmbr[s2][k];
				LL.mbr[k-d] = childrenmbr[s2][k];
				LL.mbr[k] = childrenmbr[s2][k];
			}else{
				LL.childrenmbr[0][k] = P[k];
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
				int newmbr1[2*d];
				int newmbr2[2*d];	
				for(int j=0; j<d; i++){
					newmbr1[j] = min(L.mbr[j],childrenmbr[i][j]);
					newmbr1[j+d] = max(L.mbr[j+d],childrenmbr[i][j+d]);
					newmbr2[j] = min(L.mbr[j],childrenmbr[i][j]);
					newmbr2[j+d] = max(L.mbr[j+d],childrenmbr[i][j+d]);
				}
				newarea1=1;
				newarea2=1;
				for(int i=0; i<d; j++){
					newarea1*=(newmbr1[i]-newmbr1[i+d]);
					newarea2*=(newmbr2[i]-newmbr2[i+d]);
				}
				if(newarea1==newarea2){
					if(area1==area2){
						if(e1<e2){
							L.mbr = newmbr1;
							area1 = newarea1;
						}else{
							LL.mbr = newmbr2;
							area2 = newarea2;
						}
					}else if(area1<area2){
						L.mbr = newmbr1;
						area1 = newarea1;
					}else{
						LL.mbr = newmbr2;
						area2 = newarea2;
					}
				}else if(newarea1<newarea2){
					L.mbr = newmbr1;
					area1 = newarea1;
				}else{
					LL.mbr = newmbr2;
					area2 = newarea2;
				}
			}
		}
		int newmbr1[2*d];
		int newmbr2[2*d];	
		for(int j=0; j<d; i++){
			newmbr1[j] = min(L.mbr[j],P[j]);
			newmbr1[j+d] = max(L.mbr[j+d],P[j+d]);
			newmbr2[j] = min(L.mbr[j],P[j]);
			newmbr2[j+d] = max(L.mbr[j+d],P[j+d]);
		}
		newarea1=1;
		newarea2=1;
		for(int i=0; i<d; j++){
			newarea1*=(newmbr1[i]-newmbr1[i+d]);
			newarea2*=(newmbr2[i]-newmbr2[i+d]);
		}
		if(newarea1==newarea2){
			if(area1==area2){
				if(e1<e2){
					L.mbr = newmbr1;
					area1 = newarea1;
				}else{
					LL.mbr = newmbr2;
					area2 = newarea2;
				}
			}else if(area1<area2){
				L.mbr = newmbr1;
				area1 = newarea1;
			}else{
				LL.mbr = newmbr2;
				area2 = newarea2;
			}
		}else if(newarea1<newarea2){
			L.mbr = newmbr1;
			area1 = newarea1;
		}else{
			LL.mbr = newmbr2;
			area2 = newarea2;
		}
	}
	return LL;
}
// Rtree::Node PickNext(int P[d], int childrenmbr[][], bool leaf, Node L){
// 	int area1=0;
// 	int area2=0;
// 	int e1 = 1;
// 	int e2 = 1;
// 	for(int i = 0; i < maxCap; ++i){
// 		if(i!=s1 and i!=s2){
// 			int newmbr1[2*d];
// 			int newmbr2[2*d];	
// 			for(int j=0; j<d; i++){
// 				newmbr1[j] = min(L.mbr[j],childrenmbr[i][j+d],L.mbr[j+d]);
// 				newmbr1[j+d] = max(L.mbr[j],childrenmbr[i][j+d],L.mbr[j+d]);
// 				newmbr2[j] = min(L.mbr[j],childrenmbr[i][j+d],L.mbr[j+d]);
// 				newmbr2[j+d] = max(L.mbr[j],childrenmbr[i][j+d],L.mbr[j+d]);
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
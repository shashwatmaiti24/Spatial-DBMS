#include "rtree.hpp"

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
}
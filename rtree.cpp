#include "rtree.hpp"
#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>
using namespace std;

void RTree::inserrt(int xNode, std::vector<int> P){
//		get the root node from the page
    FileManager fm1;
    FileHandler fh1=fm1.OpenFile(filename);   
    PageHandler ph=fh1.FirstPage();
    PageHandler ph2=fh1.LastPage();
    int p0=ph.GetPageNum();
    int p1=ph2.GetPageNum();
  

    int entryPerPage=PAGE_SIZE/(sizeof(int)*d);
    int numPage=N/entryPerPage;
    if (N%entryPerPage!=0){
        numPage++;
    }

    nodeIDCtr=0;
    int offset=10000;
    cout<<"Num Pages 1"<<p1-p0+1<<" == "<<numPage<<endl;
    while(p0<=p1){
        for(int i=0;i<entryPerPage;i++){
            char* data=ph.GetData();
            vector<int> v(d,-1);
            for (int j=0;j<d;j++){
                memcpy(&v[j],&data[4*i*d+4*j],sizeof(int));
            }

            if ((PAGE_SIZE-offset)<=nodeSize){
                if (offset<9000){
                    fh.MarkDirty(rph.GetPageNum());
                    fh.UnpinPage(rph.GetPageNum());
                }
                offset=0;
                rph=fh.NewPage();                
            }

            Node N;
            N.id=nodeIDCtr;
            nodeIDCtr++;
            N.parentId=-1;
            N.mbr.resize(2*d);
            for (int k=0;k<d;k++){
                N.mbr[k]=v[k];
                N.mbr[k+d]=v[k];//Should Be InTMin--Check
            }
            N.childId.resize(d,-1);
            N.childMbr.resize(maxCap);
            for(int ctr=0;ctr<maxCap;ctr++){
                N.childMbr[ctr].resize(2*d,INT32_MIN);
            }
            //Flush Node
            char* rphData=rph.GetData();
            int additive=0;
            memcpy(&rphData[offset+additive],&N.id,sizeof(int));
            additive+=4;
            memcpy(&rphData[offset+additive],&N.parentId,sizeof(int));
            additive+=4;
            for (int i=0;i<N.mbr.size();i++){
                memcpy(&rphData[offset+additive],&N.mbr[i],sizeof(int));
                additive+=4;
            }
            for (int i=0;i<N.childId.size();i++){
                memcpy(&rphData[offset+additive],&N.childId[i],sizeof(int));
                additive+=4;
            }
            for (int i=0;i<N.childMbr.size();i++){
                memcpy(&rphData[offset+additive],&N.mbr[i],sizeof(int));
                additive+=4;
            }
            offset=offset+additive;

        }
    }
    ph=fh1.NextPage(ph.GetPageNum());
    p0=ph.GetPageNum();
    fm.CloseFile(fh);
    fm1.CloseFile(fh1);
	Node RN;
	memcpy (&RN, &data[0], sizeof(Node));
	Node Leaf = ChooseLeaf(RN, P);
	if(maxCap>Leaf.nchildren){
		for(int i=0; i<d; i++){
			Leaf.childrenmbr[nchildren][i+d] = P[i];
		}
		Leaf.children[nchildren] = 0;
		Leaf.nchildren+=1;
		return RN;
	}
	Node NN = SplitNode(Leaf, -1, P);
	while(Leaf.parent!=RN.id){
		Node parent;
		memcpy (&parent, &data[0], sizeof(Node));
		if(maxCap>parent.nchildren){
			for(int i=0; i<d; i++){
				parent.childrenmbr[nchildren][i+d] = P[i];
			}
			parent.children[nchildren] = NN.id;
			parent.nchildren+=1;
		}
		Node NN = SplitNode(parent,NN.id, NN.mbr);
	}
//      Code from step 10
}
RTree::Node ChooseLeaf(Node N, int[d] P){
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
	int newarea=1;
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
RTree::Node SplitNode(int P[2*d], int newentryid, Node L){
	Node LL = new Node;
	int childrenmbr[maxCap][2*d] = L.childrenmbr;
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
			int t = (childrenmbr[s1][k]-P[k]);
			dist+=t*t;
		}
		if(dist>maxdist){
			dist = maxdist;
			s2 = -1;
		}
		dist = 0;
		for(int k = d; k<2d; ++k){
			int t = (childrenmbr[s2][k]-P[k]);
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
							for(int k = d; k<2d; ++k){
								L.childrenmbr[e1][k] = childrenmbr[i][k];
							}
							L.children[e1] = 0;
						}else{
							LL.mbr = newmbr2;
							area2 = newarea2;
							for(int k = d; k<2d; ++k){
								LL.childrenmbr[e2][k] = childrenmbr[i][k];
							}
							LL.children[e2] = 0;

						}
					}else if(area1<area2){
						L.mbr = newmbr1;
						area1 = newarea1;
						for(int k = d; k<2d; ++k){
							L.childrenmbr[e1][k] = childrenmbr[i][k];
							
						}
						L.children[e1] = 0;
					}else{
						LL.mbr = newmbr2;
						area2 = newarea2;
						for(int k = d; k<2d; ++k){
							LL.childrenmbr[e2][k] = childrenmbr[i][k];
							
						}
						LL.children[e2] = 0;
					}
				}else if(newarea1<newarea2){
					L.mbr = newmbr1;
					area1 = newarea1;
					for(int k = d; k<2d; ++k){
						L.childrenmbr[e1][k] = childrenmbr[i][k];
						
					}
					L.children[e1] = 0;
				}else{
					LL.mbr = newmbr2;
					area2 = newarea2;
					for(int k = d; k<2d; ++k){
						LL.childrenmbr[e2][k] = childrenmbr[i][k];
						
					}
					LL.children[e2] = 0;
				}
			}
		}
		if(s1!=-1 and s2!=-1){
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
						for(int k = d; k<2d; ++k){
							L.childrenmbr[e1][k] = P[k];
							
						}
						L.children[e1] = 0;
					}else{
						LL.mbr = newmbr2;
						area2 = newarea2;
						for(int k = d; k<2d; ++k){
							LL.childrenmbr[e2][k] = P[k];
							
						}
						LL.children[e2] = 0;
					}
				}else if(area1<area2){
					L.mbr = newmbr1;
					area1 = newarea1;
					for(int k = d; k<2d; ++k){
						L.childrenmbr[e1][k] = P[k];
						
					}
					L.children[e1] = 0;
				}else{
					LL.mbr = newmbr2;
					area2 = newarea2;
					for(int k = d; k<2d; ++k){
						LL.childrenmbr[e2][k] = P[k];
						
					}
					LL.children[e2] = 0;
				}
			}else if(newarea1<newarea2){
				L.mbr = newmbr1;
				area1 = newarea1;
				for(int k = d; k<2d; ++k){
					L.childrenmbr[e1][k] = P[k];
					
				}
				L.children[e1] = 0;
			}else{
				LL.mbr = newmbr2;
				area2 = newarea2;
				for(int k = d; k<2d; ++k){
					LL.childrenmbr[e2][k] = P[k];
					
				}
				LL.children[e2] = 0;
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
		if(s1!=-1){
			L.children[0] = children[s1];
		}else{
			L.children[0] = newentryid;
		}
		if(s2!=-1){
			LL.children[0] = children[s2];
		}else{
			LL.children[0] = newentryid;
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
				for(int j=0; j<d; j++){
					newarea1*=(newmbr1[j]-newmbr1[j+d]);
					newarea2*=(newmbr2[j]-newmbr2[j+d]);
				}
				if(newarea1==newarea2){
					if(area1==area2){
						if(e1<e2){
							L.mbr = newmbr1;
							area1 = newarea1;
							for(int i=0; i<2*d; ++i){
								L.childrenmbr[e1][k] = childrenmbr[i][k];
							}
							L.children[e1] = children[i];
						}else{
							LL.mbr = newmbr2;
							area2 = newarea2;
							for(int i=0; i<2*d; ++i){
								LL.childrenmbr[e2][k] = childrenmbr[i][k];
							}
							LL.children[e2] = children[i];
						}
					}else if(area1<area2){
						L.mbr = newmbr1;
						area1 = newarea1;
						for(int i=0; i<2*d; ++i){
							L.childrenmbr[e1][k] = childrenmbr[i][k];
						}
						L.children[e1] = children[i];
					}else{
						LL.mbr = newmbr2;
						area2 = newarea2;
						for(int i=0; i<2*d; ++i){
							LL.childrenmbr[e2][k] = childrenmbr[i][k];
						}
						LL.children[e2] = children[i];
					}
				}else if(newarea1<newarea2){
					L.mbr = newmbr1;
					area1 = newarea1;
					for(int i=0; i<2*d; ++i){
						L.childrenmbr[e1][k] = childrenmbr[i][k];
					}
					L.children[e1] = children[i];
				}else{
					LL.mbr = newmbr2;
					area2 = newarea2;
					for(int i=0; i<2*d; ++i){
						LL.childrenmbr[e2][k] = childrenmbr[i][k];
					}
					LL.children[e2] = children[i];
				}
			}
		}
		if(s1!=-1 and s2!=-1){
			int newmbr1[2*d];
			int newmbr2[2*d];	
			for(int j=0; j<d; i++){
				newmbr1[j] = min(L.mbr[j],P[j]);
				newmbr1[j+d] = max(L.mbr[j+d],P[j+d]);
				newmbr2[j] = min(L.mbr[j],P[j]);
				newmbr2[j+d] = max(L.mbr[j+d],P[j+d]);
			}
			int newarea1=1;
			int newarea2=1;
			for(int i=0; i<d; j++){
				newarea1*=(newmbr1[i]-newmbr1[i+d]);
				newarea2*=(newmbr2[i]-newmbr2[i+d]);
			}
			if(newarea1==newarea2){
				if(area1==area2){
					if(e1<e2){
						L.mbr = newmbr1;
						area1 = newarea1;
						for(int i=0; i<2*d; ++i){
							L.childrenmbr[e1][k] = childrenmbr[i][k];
						}
						L.children[e1] = children[i];
					}else{
						LL.mbr = newmbr2;
						area2 = newarea2;
						for(int i=0; i<2*d; ++i){
							LL.childrenmbr[e2][k] = childrenmbr[i][k];
						}
						LL.children[e2] = children[i];
					}
				}else if(area1<area2){
					L.mbr = newmbr1;
					area1 = newarea1;
					for(int i=0; i<2*d; ++i){
						L.childrenmbr[e1][k] = childrenmbr[i][k];
					}
					L.children[e1] = children[i];
				}else{
					LL.mbr = newmbr2;
					area2 = newarea2;
					for(int i=0; i<2*d; ++i){
						LL.childrenmbr[e2][k] = childrenmbr[i][k];
					}
					LL.children[e2] = children[i];
				}
			}else if(newarea1<newarea2){
				L.mbr = newmbr1;
				area1 = newarea1;
				for(int i=0; i<2*d; ++i){
					L.childrenmbr[e1][k] = childrenmbr[i][k];
				}
				L.children[e1] = children[i];
			}else{
				LL.mbr = newmbr2;
				area2 = newarea2;
				for(int i=0; i<2*d; ++i){
					LL.childrenmbr[e2][k] = childrenmbr[i][k];
				}
				LL.children[e2] = children[i];
			}
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
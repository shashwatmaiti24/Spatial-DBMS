
#include<iostream>
#include <fstream>
#include "file_manager.h"
#include "errors.h"
#include <vector>
#include <cstring>
#include<bits/stdc++.h> 
using namespace std;

// const int INT_MIN=-1;
void flushNode(){

}
class Node{
public:
	int id;
	vector<int> mbr;
	int parentId;
	vector<int>childId;
	vector<vector<int>> childMbr;
	//No need of constructor
	Node();
	Node(int maxcap,int d);
    vector<int> vectorify();
    void devtorify(vector<int>& v,int nodeSize);
    void print();
};
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
vector<int> Node::vectorify(){
    vector<int> v;
    v.push_back(id);
    v.push_back(parentId);
    for (auto x : mbr){
        v.push_back(x);
    }
    for(auto x:childId) v.push_back(x);
    for(auto x:childMbr){
        for(auto y:x)v.push_back(y);
    }
    return v;
}
void Node::devtorify(vector<int>& v,int nodeSize){
    if(nodeSize==v.size()){
        int i=0;
        id=v[i++];
        parentId=v[i++];
        for(int j=0;j<mbr.size();j++){
            mbr[j]=v[i++];
        }
        for(int j=0;j<childId.size();j++){
            childId[j]=v[i++];
        }
        for(int j=0;j<childMbr.size();j++){
            for(int k=0;k<childMbr[j].size();k++){
                childMbr[j][k]=v[i++];
            }
        }
    }else{
        // cout<<"Problem in Devectorify";
    }
}
void Node::print(){
    cout<<"ID "<<id<<" ParentId "<<parentId<<endl;;
    cout<<"MBR ";
    for (auto x: mbr) cout<<x<<" " ;
    cout<<endl;
    cout<<"CHILD ID ";
    for (auto x: childId) cout<<x<<" " ;
    cout<<endl;
    cout<<"CHILD MBR ";
    // for (auto x: childMbr){ 
	// 	for (auto y: x) cout<<y<<" " ;
    // 	cout<<endl;
	// }    
}
void assignParent(int startIdx,int endidx,int maxCap,int& nodeIDCtr,int nodeSize,int d,FileManager& fm, FileHandler& fh);
int BulkLoad(FileManager& fm,FileHandler& fh,FileHandler& fh1,int d,int maxCap,int N,int& nodeIDCtr,int nodeSize){
    
    PageHandler rph;
	// std::cout << "Rtree File created " << endl;     
    PageHandler ph=fh1.FirstPage();
    //PageHandler ph2=fh1.LastPage();
    int p0=ph.GetPageNum();   

    int entryPerPage=PAGE_CONTENT_SIZE/(sizeof(int)*d);
    int numPage=N/entryPerPage;
    if (N%entryPerPage!=0){
        numPage++;
    }
    ofstream outfile("points_new.txt", ios::out);

    nodeIDCtr=0;
    int offset=0;
    // std::cout<<"Num Pages "<<p0+1<<" == "<<numPage<<" And N is"<<N<<endl;
    rph=fh.NewPage();
    char* rData=rph.GetData();
    char* data=ph.GetData();
    int entryCtr=0;
    //int numPoints=N;
    while(entryCtr<N){
        int upperLimit=min(entryPerPage,N-entryCtr); 
        //N-=upperLimit; 
        //cout<<N<<entryPerPage;      
        for (int i=0;i<upperLimit;i++){
            vector<int> v(d,-1);
            outfile<<"SEARCH ";
            for(int j=0;j<d;j++){

                std::memcpy(&v[j],&data[4*i*d+4*j],sizeof(int));
                outfile<<v[j]<<" ";
                //std::cout<<"v["<<i<<"]"<<"["<<j<<"] = "<<v[j]<<" "<<4*i*d+4*j;  
                            
            }
            outfile<<endl;
            //std::cout<<" Out of Loop "<<entryCtr<<endl;
            entryCtr++;
            
            //std::cout<<PAGE_CONTENT_SIZE<<" "<<offset<<" "<<nodeSize<<" ";
            //Flushing page
            if ((PAGE_CONTENT_SIZE-offset)<nodeSize){
                // std::cout<<"flush page started"<<endl;
                fh.MarkDirty(rph.GetPageNum() );
                fh.UnpinPage(rph.GetPageNum());
                offset=0;
                if (entryCtr<N){
                    rph=fh.NewPage();
                    rData=rph.GetData();
                }
                else {
                    // cout<<"Break1 "<<N<<endl;
                    break;
                }
            }

            //Making Node
            Node n(maxCap,d);
            n.id=nodeIDCtr;
            nodeIDCtr++;            
            for(int i=0;i<d;i++){
                n.mbr[i]=v[i];
                n.mbr[i+d]=v[i];
            }

            //Writing Node of RTree Page            
            /*std::memcpy(&rData[offset],&n.id,sizeof(int));
            offset+=4;
            std::memcpy(&rData[offset],&n.parentId,sizeof(int));
            offset+=4;
            for (int i=0;i<n.mbr.size();i++){
                std::memcpy(&rData[offset],&n.mbr[i],sizeof(int));
                offset+=4;
            }
            for (int i=0;i<n.childId.size();i++){
                std::memcpy(&rData[offset],&n.childId[i],sizeof(int));
                offset+=4;
            }
            for (int i=0;i<n.childMbr.size();i++){
                for(int j=0;j<2*d;j++){
                    std::memcpy(&rData[offset],&n.mbr[i],sizeof(int));
                    offset+=4;
                }
            }*/ 
            vector<int> v1=n.vectorify();
            if (v1.size()*sizeof(int)==nodeSize){
                std::memcpy(&rData[offset],&v1[0],nodeSize); 
                int temp=-1;
                std::memcpy(&temp,&rData[offset+8],sizeof(int));
                //std::cout<<"Created Node with id "<<n.id<<" "<<n.mbr[0]<<" "<<temp<<endl;               
            }
            else{
                // std::cout<<"Problem in vectorify "<<v1.size()<<" "<<nodeSize<<endl;
            }    
            offset+=nodeSize;      
        }        
        if (entryCtr<N){
            // std::cout<<"getting next page"<<endl;            
            fh1.UnpinPage(p0);
            p0++;
            ph=fh1.PageAt(p0);
            data=ph.GetData();                        
        } else {
            // std::cout<<"break 2 "<<N<<endl;
            break;
        }
    }
    //outfile.close();
    // cout<<N<<" Going To Assignparents "<<nodeIDCtr<<endl;    
    assignParent(0,nodeIDCtr,maxCap,nodeIDCtr,nodeSize,d,fm,fh);
    // fm.CloseFile(fh);
    // fm.CloseFile(fh1);    
    return nodeIDCtr-1;
}

void assignParent(int startIdx,int endidx,int maxCap,int& nodeIDCtr,int nodeSize,int d,FileManager& fm, FileHandler& fh){
    int numNode=endidx-startIdx;    
    
    int block=numNode/maxCap;
    if (numNode%maxCap!=0)block++;
    int blockRem=numNode%maxCap;
    int numNodeCreated=0;

    int nodePerPage=PAGE_CONTENT_SIZE/nodeSize;
    int pageNum=(endidx)/nodePerPage;    
    int offset=((endidx)%nodePerPage)*nodeSize;
    //std::cout<<"endidx "<<endidx<<" "<<startIdx << endl;
    PageHandler rph;
    if (offset==0){
        rph=fh.NewPage();
    }
    else{
        rph=fh.PageAt(pageNum);
    }
    char* rData=rph.GetData();
    while( startIdx<endidx){
        Node p(maxCap,d);
        p.id=nodeIDCtr++;
        p.parentId=-1;
        int upperLimit=min(maxCap,endidx-startIdx);
        //std::cout<<"UpperLimit "<<upperLimit<<endl;
        for(int i=0;i<upperLimit;i++){
            int cPage=startIdx/nodePerPage;
            int coffset=(startIdx%nodePerPage)*nodeSize;
            PageHandler cph=fh.PageAt(cPage);
            char* cData=cph.GetData();
            vector<int> nodeVector(nodeSize/sizeof(int),-1);
            Node cNode(maxCap,d); 
            int temp=0;           
            std::memcpy(&cData[coffset+4],&p.id,sizeof(int));            
            std::memcpy(&nodeVector[0],&cData[coffset],nodeSize);
            cNode.devtorify(nodeVector,nodeSize/sizeof(int));

            //cNode.print();
            p.childId[i]=cNode.id;
            for(int j=0;j<2*d;j++){
                if (j<d)p.mbr[j]=min(p.mbr[j],cNode.mbr[j]);
                else p.mbr[j]=max(p.mbr[j],cNode.mbr[j]);                
                p.childMbr[i][j]=cNode.mbr[j];
            }
            startIdx++;
            fh.MarkDirty(cPage);
            fh.UnpinPage(cPage);
            
        }
        vector<int> parentVector=p.vectorify();
        std::memcpy(&rData[offset],&parentVector[0],nodeSize);
        //cout<<"Created Node with "<<parentVector[0]<<" "<<p.id<<endl;
        offset+=nodeSize;
        if (startIdx==endidx){
            // cout<<"ASSIGN  breaking loop"<<endl;
            break;
        }
        if ((PAGE_CONTENT_SIZE-offset)<nodeSize){
            fh.MarkDirty(rph.GetPageNum());
            fh.UnpinPage(rph.GetPageNum());                      
            offset=0;
            rph=fh.NewPage();
            rData=rph.GetData();
        }
        
    }
    // cout<<"rph pagenum "<<rph.GetPageNum()<<endl;
    fh.MarkDirty(rph.GetPageNum());
    fh.UnpinPage(rph.GetPageNum());
    //fh.FlushPage(rph.GetPageNum());
    /*int tempPage=0;
    int lp=fh.LastPage().GetPageNum();
    fh.UnpinPage(lp);

    while(true){
        rph=fh.PageAt(tempPage);
        rData=rph.GetData();
        for (int i=0;i<18;i++){
            int temp=0;
            vector<int> sample_v(nodeSize/sizeof(int));
            std::memcpy(&sample_v[0],&rData[i*nodeSize],nodeSize);
            Node kk(maxCap,d);
            kk.devtorify(sample_v,nodeSize/sizeof(int));
            kk.print();            
        }
        fh.UnpinPage(tempPage);
        tempPage++;
        if (tempPage>lp) break;
    }*/
    
    
    if (numNode>maxCap){
        // cout<<"Calling Assign Parent Again"<<numNode<<endl;
        assignParent(endidx,nodeIDCtr,maxCap,nodeIDCtr,nodeSize,d,fm,fh);
    }
}

bool isInside(vector<int>& rectangle,vector<int>& point,int d){
    for (int i=0;i<d;i++){
        if(!(rectangle[i]<=point[i] && rectangle[i+d]>=point[i])) return false;
    }
    return true;
}
bool search(int xnode,int d,int maxCap,int nodeSize,vector<int>& point,FileManager& fm,FileHandler& fh){
    int nodePerPage=PAGE_CONTENT_SIZE/nodeSize;
    int pageNum=(xnode)/nodePerPage;
    int offset=((xnode)%nodePerPage)*nodeSize;

    int maxChildId=INT32_MIN;
    vector<int> xVector(nodeSize/sizeof(int));
    PageHandler rph=fh.PageAt(pageNum);
    char* rData=rph.GetData();
    std::memcpy(&xVector[0],&rData[offset],nodeSize);
    Node P(maxCap,d);
    P.devtorify(xVector,nodeSize/sizeof(int));
	fh.UnpinPage(rph.GetPageNum());
    // P.print();
    // std::cout<<"STARTING POINT ";
    // for (auto x:point){
    //     std::cout<<x<<" ";
    // }
    // std::cout<<"END POINT"<<endl;
    // for(int i=0;i<P.childId.size();i++){
    //     if (maxChildId<P.childId[i]) maxChildId=P.childId[i];
    // }    
    if (P.childId[0]<0){
        // std::cout<<"AT LEAF NODE"<<endl;
        for (int i=0;i<d;i++){
            if (point[i]!=P.mbr[i]){
                return false;
            }
        }
        //cout<<"Returning True";
		// fh.UnpinPage(rph.GetPageNum());
        return true;
    }
    else{
        bool res=false;
        // cout<<"Looking Inside "<<endl;;
        for (int i=0;i<maxCap;i++){
            // cout<<P.childId[i]<<endl;;
            if (P.childId[i]>=0){
                for (int j=0;j<2*d;j++){
                    // cout<<P.childMbr[i][j]<<" ";
                }
                // cout<<endl;
                if (isInside(P.childMbr[i],point,d)){
                    // cout<<"THE POINT MAY BE INSIDE THIS CHILD "<<P.childId[i]<<endl;
                    if(search(P.childId[i],d,maxCap,nodeSize,point,fm,fh)){
						// fh.UnpinPage(rph.GetPageNum());
						return true;
					}
                }
            }
        }
		// fh.UnpinPage(rph.GetPageNum());
        return res;        
    }   
    
}
Node GetNode(int id,FileHandler& fh, int nodeSize, int maxCap, int d, FileManager& fm);
void CreateNode(Node node,FileHandler& fh, int nodeSize, int maxCap, int d, FileManager& fm);
int insert(int xNode,int d,int maxCap,FileHandler& fh,int nodeSize,std::vector<int> P,int& nodeIDCtr, FileManager& fm);
Node ChooseLeaf(Node N, std::vector<int>P, int nodeSize, FileHandler& fh,int maxCap,int d, FileManager& fm);
Node SplitNode(Node Pnode, Node L, FileHandler& fh, int nodeSize, int& nodeIDCtr, int maxCap, int d, FileManager& fm);
int AdjustTree(Node L, Node LL, FileHandler& fh, int nodeSize, int& nodeIDCtr, bool split, int maxCap, int d, FileManager& fm);
Node GetNode(int id,FileHandler& fh, int nodeSize, int maxCap, int d, FileManager& fm){
	if(id==1557){
		cout<<"Getting 1557"<<endl;
	}
	int nodePerPage=PAGE_CONTENT_SIZE/nodeSize;
	Node N(maxCap, d);
	int NPnum=id/nodePerPage;
	PageHandler NPage=fh.PageAt(NPnum);
    char* NData=NPage.GetData();
	int NOffset=(id%nodePerPage)*nodeSize;
	NPage=fh.PageAt(NPnum);
    vector<int> xVector(nodeSize/sizeof(int));
    std::memcpy(&xVector[0],&NData[NOffset],nodeSize);
    N.devtorify(xVector,nodeSize/sizeof(int));
	// int Nid;
	// memcpy(&Nid,&NData[NOffset],sizeof(int)); 
	// N.id = Nid;
	// int NparentId;
	// memcpy(&NparentId,&NData[NOffset+4],sizeof(int)); 
	// N.parentId = NparentId;
	// N.mbr.resize(2*d);
	// for (int i=0;i<2*d;i++){
	// 	int mbrCoordinate=-1;
	// 	memcpy(&mbrCoordinate,&NData[NOffset+8+4*i],sizeof(int));
	// 	N.mbr[i]=mbrCoordinate;
	// }
	// N.childId.resize(maxCap);
	// for(int j=0;j<maxCap;j++){
	// 	int NchildId;
	// 	memcpy(&NchildId,&NData[NOffset+8+8*d+4*j],sizeof(int));
	// 	N.childId[j] = NchildId;
	// 	N.childMbr[j].resize(2*d);
	// 	for (int i=0;i<2*d;i++){
	// 		int mbrCordinate=-1;
	// 		memcpy(&mbrCordinate,&NData[NOffset+8+8*d+4*maxCap+8*d*j+4*i],sizeof(int));
	// 		N.childMbr[j][i]=mbrCordinate;
	// 	}
	// }
	fh.UnpinPage(NPage.GetPageNum());
	return N;
}
void CreateNode(Node node,FileHandler& fh, int nodeSize, int maxCap, int d, FileManager& fm){
	int nodePerPage=PAGE_CONTENT_SIZE/nodeSize;
	int NOffset=(node.id%nodePerPage)*nodeSize;
	int NPnum=node.id/nodePerPage;
		if(node.id==1557){
		cout<<"Node 1557"<<endl;
		cout<<nodePerPage<<endl;
		cout<<NOffset<<endl;
		cout<<NPnum<<endl;
	}
	PageHandler NPage;

    // cout<<NPnum<<" node created "<<node.id<<endl;
    cout<<" creating node"<<endl;        

    // cout<<PAGE_CONTENT_SIZE<<" "<<NOffset<<" "<<nodeSize<<endl;
	if (NPnum>fh.LastPage().GetPageNum()){      
        cout<<"newpage is created "<<node.id<<endl;  
		// fh.UnpinPage(NPage.GetPageNum());           
		NOffset=0;
		NPage=fh.NewPage();
		NPnum = NPage.GetPageNum();
	}
	else{
		NPage = fh.PageAt(NPnum);
	}
    vector<int> v1=node.vectorify();
    char* NData=NPage.GetData();
    std::memcpy(&NData[NOffset],&v1[0],nodeSize);
    // fh.FlushPage(NPage.GetPageNum());
	fh.MarkDirty(NPage.GetPageNum());
	fh.UnpinPage(NPage.GetPageNum()); 
    // fh.FlushPage(NPage.GetPageNum());
	// Node blah = GetNode(node.id, fh, nodeSize, maxCap, d, fm);
	// blah.print();
		// }
	// int Nid = node.id;
	// memcpy(&NData[NOffset],&Nid,sizeof(int)); 
	// int NparentId = node.parentId;
	// memcpy(&NData[NOffset],&NparentId,sizeof(int)); 
	// node.mbr.resize(2*d);
	// for (int i=0;i<2*d;i++){
	// 	int mbrCoordinate=node.mbr[i];
	// 	memcpy(&NData[NOffset+8+4*i],&mbrCoordinate,sizeof(int));
	// }
	// node.childId.resize(maxCap);
	// for(int j=0;j<maxCap;j++){
	// 	int NchildId = node.childId[j];
	// 	memcpy(&NData[NOffset+8+8*d+4*j],&NchildId,sizeof(int));
	// 	node.childMbr[j].resize(2*d);
	// 	for (int i=0;i<2*d;i++){
	// 		int mbrCordinate=node.childMbr[j][i];
	// 		memcpy(&NData[NOffset+8+8*d+4*maxCap+8*d*j+4*i],&mbrCordinate,sizeof(int));
	// 	}
	// }
// 	fh.MarkDirty(NPage.GetPageNum());
// 	fh.UnpinPage(NPage.GetPageNum());
}


int insert(int xNode,int d,int maxCap,FileHandler& fh,int nodeSize,std::vector<int> P,int& nodeIDCtr,FileManager& fm){
	int tempPage=0;
    int lp=fh.LastPage().GetPageNum();
    fh.UnpinPage(lp);

	
	cout<<"********************************************************"<<endl;
	PageHandler rph;
	char* rData;
    while(true){
        rph=fh.PageAt(tempPage);
        rData=rph.GetData();
        for (int i=0;i<PAGE_CONTENT_SIZE/nodeSize;i++){
            int temp=0;
            vector<int> sample_v(nodeSize/sizeof(int));
            std::memcpy(&sample_v[0],&rData[i*nodeSize],nodeSize);
            Node kk(maxCap,d);
            kk.devtorify(sample_v,nodeSize/sizeof(int));
            kk.print();            
        }
        fh.UnpinPage(tempPage);
        tempPage++;
        if (tempPage>lp) break;
    }
	
	cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
	
	
	cout<<"******************INSERTING***********************"<<endl;

    for (auto x: P) cout<<x<<" " ;
    // cout<<endl;
    // cout<<"This is the root ID "<<xNode<<endl;
	if(nodeIDCtr>1557){
		GetNode(1557, fh, nodeSize, maxCap, d, fm).print();
	}
	cout << "1_________________" << endl;
	cout<< xNode <<endl;
	Node RN = GetNode(xNode, fh, nodeSize, maxCap, d, fm);
    // RN.print();
    // cout<<"RN print done"<<endl;
	cout << "2_________________" << endl;
    Node L = ChooseLeaf(RN, P, nodeSize,fh, maxCap, d, fm);
	cout << "3_________________" << endl;
	Node Pnode(maxCap,d);
	Pnode.id=nodeIDCtr;
	nodeIDCtr++;
	P.resize(d);     
	for(int i=0;i<d;i++){
		Pnode.mbr[i] = P[i];
 		Pnode.mbr[i+d]=P[i];
	}
	cout << "4_________________" << endl;
	Pnode.print();
	// put new data in the leaf node
	int nchild=maxCap;
	for(int i=0; i<maxCap; ++i){
        // cout<<L.childId[i]<<endl;
		if(L.childId[i]<0){
			nchild = i;
			break;
		}
	}
	cout << "5_________________" << endl;

	// cout<<"______________Pnode________________"<<endl;
	// Pnode.print();
    // if(nodeIDCtr>137){
    //     // cout<<"Printing BTNode start"<<endl;
    //     Node BTNode = GetNode(137, fh, nodeSize, maxCap, d);
    //     BTNode.print();
	// 	while(BTNode.parentId>0){
    //     	BTNode = GetNode(BTNode.parentId, fh, nodeSize, maxCap, d);
    //    		BTNode.print();
	// 	}
    //     cout<<"Printing BTNode end"<<endl;
    // }
    // cout<<maxCap<<"   "<<nchild<<";  "<<P[0]<<";  "<<L.childId[7]<<endl;
	if(maxCap>nchild){
        // cout<<L.childMbr[0][0]<<" "<<L.childMbr[0][1];
        //  L.mbr.resize(2*d);
        //  L.childMbr.resize(maxCap);
        //  L.childMbr[nchild].resize(2*d,INT32_MIN);
        // L.childMbr.resize(maxCap);
		for(int i=0; i<d; i++){
			L.childMbr[nchild][i] = P[i];
			L.childMbr[nchild][i+d] = P[i];
			L.mbr[i] = min(L.mbr[i], P[i]);
			L.mbr[i+d] = max(L.mbr[i+d], P[i]);
		}
		L.childId[nchild] = Pnode.id;
		Pnode.parentId = L.id;
	cout << "6_________________" << endl;
        CreateNode(Pnode, fh, nodeSize, maxCap, d, fm);
	cout << "7_________________" << endl;
        CreateNode(L, fh, nodeSize, maxCap, d, fm);
	cout << "8_________________" << endl;
        
	//	nchild+=1;
		return AdjustTree(L, L, fh, nodeSize, nodeIDCtr, false, maxCap, d, fm);
	}
	// put new data in the leaf node
	cout << "9_________________" << endl;
    // cout<<"1________________________ "<<Pnode.id<<endl;
	Node NN = SplitNode(Pnode, L, fh, nodeSize, nodeIDCtr, maxCap, d, fm);
	cout << "10_________________" << endl;
	L.print();
	NN.print();
    // NN.print();
    // cout<<"2________________________ "<<Pnode.id<<endl;
	return AdjustTree(L, NN, fh, nodeSize, nodeIDCtr, true, maxCap, d, fm);
	// while(Leaf.parentId!=RN.id){
	// 	Node parent;
	// 	memcpy (parent, data[0], sizeof(Node));
	// 	if(maxCap>nchild){
	// 		for(int i=0; i<d; i++){
	// 			parent.childMbr[nchild][i+d] = P[i];
	// 		}
	// 		parent.childId[nchild] = NN.id;
	// 		nchild+=1;
	// 	}
	// 	Node NN = SplitNode(NN.mbr, NN.id, parent);
	// }
//      Code from step 10
}
Node ChooseLeaf(Node N, std::vector<int>P, int nodeSize, FileHandler& fh,int maxCap,int d, FileManager& fm){
        N.print();
        cout<<"printed N"<<endl;
		cout<<N.childId[0]<<endl;
	Node leaf = GetNode(N.childId[0], fh, nodeSize, maxCap, d, fm);
	leaf.print();
	if(leaf.childId[0]<0){
        // cout<<"------------------Returned----------------"<<endl;
		return N;

	}
	// for(int i=0; i<d; i++){
	// 	N.mbr[i] = min(N.mbr[i],min(N.mbr[i+d],P[i]));
	// 	N.mbr[i+d] = max(N.mbr[i],max(N.mbr[i+d],P[i]));
	// }
	double minarea;
	double area=1;
	int newmbr[2*d];	
	// P.resize(d);
	for(int i=0; i<d; i++){
		area*=(N.childMbr[0][i+d]-N.childMbr[0][i]);
		newmbr[i] = min(N.childMbr[0][i],P[i]);
		newmbr[i+d] = max(N.childMbr[0][i+d],P[i]);
	}
	double newarea=1;
	for(int i=0; i<d; i++){
		newarea*=(newmbr[i+d]-newmbr[i]);
	}
	minarea=area;
	int mindiff = 0;
	double mindiffarea = newarea-area;
	int i = 1;
	while(i<maxCap and N.childId[i]>0){
		area=1;
		int newmbr[2*d];
		for(int j=0; j<d; j++){
			area*=(N.childMbr[i][j]-N.childMbr[i][j+d]);
			newmbr[j] = min(N.childMbr[i][j],P[j]);
			newmbr[j+d] = max(N.childMbr[i][j+d],P[j]);
		}
		newarea=1;
		for(int j=0; j<d; j++){
			newarea*=(newmbr[j+d]-newmbr[j]);
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
    cout<<"What's happening "<<N.childId[mindiff]<<endl;
	// Get the node corresponding to mindiff from page to N
	Node newN = GetNode(N.childId[mindiff], fh, nodeSize, maxCap, d, fm);

        // newN.print();
        // cout<<"printed new N"<<endl;
	return ChooseLeaf(newN, P, nodeSize, fh, maxCap, d, fm);
}
Node SplitNode(Node Pnode, Node L, FileHandler& fh, int nodeSize, int& nodeIDCtr, int maxCap, int d, FileManager& fm){

	cout<<"Inside Split"<<endl;
	if(nodeIDCtr>1557){
		GetNode(1557, fh, nodeSize, maxCap, d, fm).print();
	}
	Node LL(maxCap, d);
	LL.id = nodeIDCtr;
	nodeIDCtr++;
	Node newL(maxCap, d);
	newL.id = L.id;
	newL.parentId = L.parentId;
	bool isleaf;
	Node leaf = GetNode(L.childId[0], fh, nodeSize, maxCap, d, fm);
	if(leaf.childId[0]<0){
		isleaf = true;
	}
	int s1 = 0;
	int s2 = 1;
	int maxdiff = 0;
	for (int i = 0; i < maxCap-1; ++i)
	{
		for (int j = i+1; j < maxCap; ++j)
		{
			double jarea = 1;
			double area1 = 1;
			double area2 = 1;
			int jmbr[2*d];
			if(!isleaf){
				for(int k = 0; k<d; ++k){
					area1*=(L.childMbr[i][k+d]-L.childMbr[i][k]);
					area2*=(L.childMbr[j][k+d]-L.childMbr[j][k]);
				}
			}
			else{
				area1 = 0;
				area2 = 0;
			}
			for(int k = 0; k<d; ++k){
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
	double jarea1 = 1;
	double jarea2 = 1;
	double area1 = 1;
	double area2 = 1;
	double areap = 1;
	int jmbr1[2*d];
	int jmbr2[2*d];
	if(!isleaf){
		for(int k = 0; k<d; ++k){
			area1*=(L.childMbr[s1][k+d]-L.childMbr[s1][k]);
			area2*=(L.childMbr[s2][k+d]-L.childMbr[s2][k]);
			areap*=(Pnode.mbr[k+d]-Pnode.mbr[k]);
		}
	}
	else{
		area1 = 0;
		area2 = 0;
		areap = 0;
	}
	for(int k = 0; k<d; ++k){
		jmbr1[k] = min(L.childMbr[s1][k], Pnode.mbr[k]);
		jmbr1[k+d] = max(L.childMbr[s1][k+d], Pnode.mbr[k+d]);
		jmbr2[k] = min(L.childMbr[s2][k], Pnode.mbr[k]);
		jmbr2[k+d] = max(L.childMbr[s2][k+d], Pnode.mbr[k+d]);
	}
	for(int k = 0; k<d; ++k){
		jarea1*=(jmbr1[k+d]-jmbr1[k]);
		jarea2*=(jmbr2[k+d]-jmbr2[k]);
	}
	if(jarea1-area1-areap>maxdiff){
		maxdiff = jarea1-area1-areap;
		s2 = -1;
	}
	else if(jarea2-area2-areap>maxdiff){
		maxdiff = jarea2-area2-areap;
		s1 = -1;
	}
	if(s1!=-1){
		newL.childId[0] = L.childId[s1];
		for(int k = 0; k<2*d; ++k){
			newL.childMbr[0][k] = L.childMbr[s1][k];
			newL.mbr[k] = L.childMbr[s1][k];
		}

	}else{
		newL.childId[0] = Pnode.id;
        Pnode.parentId = newL.id;
		for(int k = 0; k<2*d; ++k){
			newL.childMbr[0][k] = Pnode.mbr[k];
			newL.mbr[k] = Pnode.mbr[k];
		}
	}
	if(s2!=-1){
		LL.childId[0] = L.childId[s2];
        Node LLchild = GetNode(L.childId[s2], fh, nodeSize, maxCap, d, fm);
        LLchild.parentId = LL.id;
        CreateNode(LLchild, fh, nodeSize, maxCap, d, fm);
		for(int k = 0; k<2*d; ++k){
			LL.childMbr[0][k] = L.childMbr[s2][k];
			LL.mbr[k] = L.childMbr[s2][k];
		}
	}else{
		LL.childId[0] = Pnode.id;
        Pnode.parentId = LL.id;
		for(int k = 0; k<2*d; ++k){
			LL.childMbr[0][k] = Pnode.mbr[k];
			LL.mbr[k] = Pnode.mbr[k];
		}
	}
	int e1 = 1;
	int e2 = 1;
	if(s1!=-1 and s2!=-1){
		vector<int> newmbrL;
		vector<int> newmbrLL;	
		newmbrL.resize(2*d);
		newmbrLL.resize(2*d);
		for(int j=0; j<d; j++){
			newmbrL[j] = min(newL.mbr[j],Pnode.mbr[j]);
			newmbrL[j+d] = max(newL.mbr[j+d],Pnode.mbr[j+d]);
			newmbrLL[j] = min(LL.mbr[j],Pnode.mbr[j]);
			newmbrLL[j+d] = max(LL.mbr[j+d],Pnode.mbr[j+d]);
		}
		double newareaL=1;
		double newareaLL=1;
		
		for(int j=0; j<d; j++){
			newareaL*=(newmbrL[j]-newmbrL[j+d]);
			newareaLL*=(newmbrLL[j]-newmbrLL[j+d]);
		}
		if(newareaL<=newareaLL){
			for(int j=0; j<2*d; j++){
				newL.mbr[j] = newmbrL[j];
				newL.childMbr[e1][j] = Pnode.mbr[j];
			}
			newL.childId[e1] = Pnode.id;
			Pnode.parentId = newL.id;
			e1++;
		}
		else{
			for(int j=0; j<2*d; j++){
				LL.mbr[j] = newmbrLL[j];
				LL.childMbr[e2][j] = Pnode.mbr[j];
			}
			LL.childId[e2] = Pnode.id;
			Pnode.parentId = LL.id;
			e2++;
		}
	}
    CreateNode(Pnode, fh, nodeSize, maxCap, d, fm);
	int cur;
	int filled;
	double areaL=1;
	double areaLL=1;
	for(int i = 0; i < maxCap; ++i){
		Node Enode = GetNode(L.childId[i], fh, nodeSize, maxCap, d, fm);
		if(i!=s1 and i!=s2){
			vector<int> newmbrL;
			vector<int> newmbrLL;	
			newmbrL.resize(2*d);
			newmbrLL.resize(2*d);
			for(int j=0; j<d; j++){
				areaL*=newL.mbr[j+d]-newL.mbr[j];
				areaLL*=LL.mbr[j+d]-LL.mbr[j];
				newmbrL[j] = min(newL.mbr[j],Enode.mbr[j]);
				newmbrL[j+d] = max(newL.mbr[j+d],Enode.mbr[j+d]);
				newmbrLL[j] = min(LL.mbr[j],Enode.mbr[j]);
				newmbrLL[j+d] = max(LL.mbr[j+d],Enode.mbr[j+d]);
			}
			double newareaL=1;
			double newareaLL=1;
			
			for(int j=0; j<d; j++){
				newareaL*=(newmbrL[j+d]-newmbrL[j]);
				newareaLL*=(newmbrLL[j+d]-newmbrLL[j]);
			}
			if(newareaL-areaL==newareaLL-areaLL){
				if(areaL<=areaLL){
					for(int j=0; j<2*d; j++){
						newL.mbr[j] = newmbrL[j];
						newL.childMbr[e1][j] = L.childMbr[i][j];
					}
					areaL = newareaL;
					newL.childId[e1] = Enode.id;
					Enode.parentId = newL.id;
					e1++;
				}else{
					for(int j=0; j<2*d; j++){
						LL.mbr[j] = newmbrLL[j];
						LL.childMbr[e2][j] = L.childMbr[i][j];
					}
					areaLL = newareaLL;
					LL.childId[e2] = Enode.id;
					Enode.parentId = LL.id;
					e2++;
				}
			}else if(newareaL-areaL<newareaLL-areaLL){
				for(int j=0; j<2*d; j++){
					newL.mbr[j] = newmbrL[j];
					newL.childMbr[e1][j] = L.childMbr[i][j];
				}
				areaL = newareaL;
				newL.childId[e1] = Enode.id;
				Enode.parentId = newL.id;
				e1++;
			}else{
				for(int j=0; j<2*d; j++){
					LL.mbr[j] = newmbrLL[j];
					LL.childMbr[e2][j] = L.childMbr[i][j];
				}
				areaLL = newareaLL;
				LL.childId[e2] = Enode.id;
				Enode.parentId = LL.id;
				e2++;
			}
		}
		CreateNode(Enode, fh, nodeSize, maxCap, d, fm);
		if(e1 == ceil(maxCap/2)){
			cur = i+1;
			filled = 0;
			break;
		}
		else if(e2 == ceil(maxCap/2)){
			cur = i+1;
			filled = 1;
			break;
		}
	}
  //  cout<<"_________________FILLED____________"<<endl;
  //  newL.print();
    // LL.print();
  //  L.print();
	if(filled == 0){
		for(int i = cur; i < maxCap; ++i){
		    if(i!=s1 and i!=s2){
                Node Enode = GetNode(L.childId[i], fh, nodeSize, maxCap, d, fm);
                for(int j=0; j<d; j++){
                    LL.mbr[j] = min(LL.mbr[j],L.childMbr[i][j]);
                    LL.mbr[j+d] = max(LL.mbr[j+d],L.childMbr[i][j+d]);
                    LL.childMbr[e2][j] = L.childMbr[i][j];
                    LL.childMbr[e2][j+d] = L.childMbr[i][j+d];
                }
                // int newareaLL=1;
                // for(int j=0; j<d; j++){
                // 	newareaLL*=(newmbrLL[j+d]-newmbrLL[j]);
                // }
                // for(int j=0; j<2*d; j++){
                // 	LL.mbr[j] = newmbrLL[j];
                // }
                // areaLL = newareaLL;
                // for(int k = d; k<2*d; ++k){
                // 	LL.childMbr[e2][k] = L.childMbr[i][k];
                // }
                LL.childId[e2] = Enode.id;
                Enode.parentId = LL.id;
                CreateNode(Enode, fh, nodeSize, maxCap, d, fm);
                e2++;

                // cout<<"Ye wala ho rha"<<endl;
            }
		}
	}
	else if(filled == 1){
		for(int i = cur; i < maxCap; ++i){
		    if(i!=s1 and i!=s2){
                Node Enode = GetNode(L.childId[i], fh, nodeSize, maxCap, d, fm);
                for(int j=0; j<d; j++){
                    newL.mbr[j] = min(newL.mbr[j],L.childMbr[i][j]);
                    newL.mbr[j+d] = max(newL.mbr[j+d],L.childMbr[i][j+d]);
                    newL.childMbr[e1][j] = L.childMbr[i][j];
                    newL.childMbr[e1][j+d] = L.childMbr[i][j+d];
                }
                // int newareaLL=1;
                // for(int j=0; j<d; j++){
                // 	newareaLL*=(newmbrLL[j+d]-newmbrLL[j]);
                // }
                // for(int j=0; j<2*d; j++){
                // 	LL.mbr[j] = newmbrLL[j];
                // }
                // areaLL = newareaLL;
                // for(int k = d; k<2*d; ++k){
                // 	LL.childMbr[e2][k] = L.childMbr[i][k];
                // }
                newL.childId[e1] = Enode.id;
                Enode.parentId = newL.id;
                CreateNode(Enode, fh, nodeSize, maxCap, d, fm);
                e1++;
                // cout<<"Nahi Ye wala ho rha"<<endl;
            }
		}
	}
 //   LL.print();
	CreateNode(newL, fh, nodeSize, maxCap, d, fm);
	CreateNode(LL, fh, nodeSize, maxCap, d, fm);
    // cout<<endl;
    // cout<<"Split Here"<<Pnode.id<<endl;
    // cout<<endl;
    // newL.print();
    // LL.print();
	return LL;
}

int AdjustTree(Node L, Node LL, FileHandler& fh, int nodeSize, int& nodeIDCtr, bool split, int maxCap, int d, FileManager& fm){
    // L.print();
    // cout<<"_________"<<endl;
    // LL.print();
    // cout<<"_"<<endl;
	cout<<"Inside AT"<<endl;
	if(nodeIDCtr>1557){
		GetNode(1557, fh, nodeSize, maxCap, d, fm).print();
	}
    cout<<"AT1__________"<<endl;
	if(!split){
		if(L.parentId<0){
            cout<<"AT2__________"<<endl;
			return L.id;
		}
		else{
        cout<<"AT3__________"<<endl;
		Node LParent = GetNode(L.parentId, fh, nodeSize, maxCap, d, fm);
		int posL;
		for(int i=0; i<maxCap; ++i){
			if(LParent.childId[i]== L.id){
                cout<<"AT4__________"<<endl;
				posL = i;
				break;
			}
		}
        cout<<"AT5__________"<<endl;
        // LParent.print();
        // L.print();
		for(int i=0; i<d; i++){
			LParent.childMbr[posL][i] = L.mbr[i];
			LParent.childMbr[posL][i+d] = L.mbr[i+d];
			LParent.mbr[i] = min(LParent.mbr[i], L.mbr[i]);
			LParent.mbr[i+d] = max(LParent.mbr[i+d], L.mbr[i+d]);
		}
        cout<<"AT6__________"<<endl;
		CreateNode(LParent, fh, nodeSize, maxCap, d, fm);
		LParent.print();
        cout<<"AT7__________"<<endl;
		return AdjustTree(LParent, LParent, fh, nodeSize, nodeIDCtr, false, maxCap, d, fm);
		}
	}
	else{
		if(L.parentId<0){
			Node newRN(maxCap, d);
			newRN.id = nodeIDCtr;
			nodeIDCtr++;
			L.parentId = newRN.id;
			LL.parentId = newRN.id;
			newRN.childId[0] = L.id;
			newRN.childId[1] = LL.id;
			cout<<"AT8_______________"<<endl;
			newRN.id;
			// L.print();
			// LL.print();
			for(int i=0; i<d; i++){
				newRN.childMbr[0][i] = L.mbr[i];
				newRN.childMbr[0][i+d] = L.mbr[i+d];
				newRN.childMbr[1][i] = LL.mbr[i];
				newRN.childMbr[1][i+d] = LL.mbr[i+d];
				newRN.mbr[i] = min(LL.mbr[i], L.mbr[i]);
				newRN.mbr[i+d] = max(LL.mbr[i+d], L.mbr[i+d]);
			}

			cout<<"AT9_______________"<<endl;
			CreateNode(newRN, fh, nodeSize, maxCap, d, fm);
			CreateNode(L, fh, nodeSize, maxCap, d, fm);
			CreateNode(LL, fh, nodeSize, maxCap, d, fm);
			cout<<"AT10_______________"<<endl;
			return newRN.id;
		}
		else{
			cout<<"AT11__________"<<endl;
			Node LParent = GetNode(L.parentId, fh, nodeSize, maxCap, d, fm);
			int nchild=maxCap;
			int posL;
			for(int i=0; i<maxCap; ++i){
				if(LParent.childId[i]== L.id){
						cout<<"AT12__________"<<endl;
					posL = i;
				}
				if(LParent.childId[i]<0){
					nchild = i;
					break;
				}
			}
			L.print();
			LL.print();
			LParent.print();
			cout<<"AT13__________"<<endl;
			if(maxCap>nchild){
				LL.parentId = LParent.id;
				LParent.childId[nchild] = LL.id;
				for(int i=0; i<d; i++){
					LParent.childMbr[posL][i] = L.mbr[i];
					LParent.childMbr[posL][i+d] = L.mbr[i+d];
					LParent.childMbr[nchild][i] = LL.mbr[i];
					LParent.childMbr[nchild][i+d] = LL.mbr[i+d];
					LParent.mbr[i] = min(min(LL.mbr[i], L.mbr[i]), LParent.mbr[i]);
					LParent.mbr[i+d] = max(max(LL.mbr[i+d], L.mbr[i+d]), LParent.mbr[i+d]);
				}
				CreateNode(LParent, fh, nodeSize, maxCap, d, fm);
				CreateNode(LL, fh, nodeSize, maxCap, d, fm);
				return AdjustTree(LParent, LParent, fh, nodeSize, nodeIDCtr, false, maxCap, d, fm);
			}
			else{
				for(int i=0; i<d; i++){
					LParent.childMbr[posL][i] = L.mbr[i];
					LParent.childMbr[posL][i+d] = L.mbr[i+d];
					LParent.mbr[i] = min(LParent.mbr[i], L.mbr[i]);
					LParent.mbr[i+d] = max(LParent.mbr[i+d], L.mbr[i+d]);
				}
				CreateNode(LParent, fh, nodeSize, maxCap, d, fm);
				cout<<"AT14__________"<<endl;
				Node newLParent = SplitNode(LL, LParent, fh, nodeSize, nodeIDCtr, maxCap, d, fm);
				// CreateNode(newLParent, fh, nodeSize, maxCap, d);
				return AdjustTree(LParent, newLParent, fh, nodeSize, nodeIDCtr, true, maxCap, d, fm);
			}
		}
	}
}

int main(int argc, char** argv){
    // ofstream cout("output.txt");
	int maxCap = atoi(argv[2]);
	int d = atoi(argv[3]);
	FileManager fm;
	fm.DestroyFile ("temp.txt");
	fm.ClearBuffer();
    FileHandler fh;
	try
    {
        fh=fm.CreateFile("temp.txt");
    }
    catch(InvalidFileException e)   
    {
        std::cerr << "temp creation failed" << '\n';
    }
	ifstream fi(argv[1], ios::in);	
	ofstream fo(argv[4], ios::out);
	string word;
	fi>>word;
	fo<<word<<endl;
	fo<<"\n";
	fi>>word;

	const char * filename = word.c_str();
    FileHandler fh1;
    try
    {
        fh1=fm.OpenFile(filename);
    }
    catch(InvalidFileException e)
    {
        std::cerr << "Opening of Bulkload File failed"<< '\n';
    }

	fi>>word;

	int N=stoi(word);
    int nodeSize=(2+2*d+maxCap+maxCap*2*d)*sizeof(int);
    int nodeIDCtr=0;
	int rootId=BulkLoad(fm, fh, fh1, d,maxCap,N,nodeIDCtr,nodeSize);
	
    int nodesPerPage=PAGE_CONTENT_SIZE/nodeSize;
    int rootpage=rootId/nodesPerPage;
    int offs=(rootId%nodesPerPage)*nodeSize;
    int temp=-2;
    char* t1=fh.PageAt(rootpage).GetData();
    std::memcpy(&temp,&t1[offs],sizeof(int));
    
    cout<<"ROOT DETAIL "<<temp<<endl;
	int arr[2];
	arr[0]=rootId;
	// arr[1]=rootId+1;
	arr[1]=rootId+1;
	// int pnt[d];
	int count=0;
	int count2=0;
	while(fi>>word){
		vector<int> pnt(d,0);
        for(int i=0;i<d;i++)pnt[i]=d;
		if(word.compare("INSERT")==0){
			for(int k=0;k<d;k++){
				fi>>word;
				pnt[k]=stoi(word);
				//pnt[k] = stoi(word);  
				// pnt.push_back(stoi(word));
			}	

            // for (int i=0; i<2; i++) cout<<pnt[i]<<" " ;
            // cout<<endl;
			 cout<<"INSERT started\n";
			 cout<<rootId<<endl;
        //		get the root node from the page
        //    Node RN = GetNode(rootId, fh, nodeSize, maxCap, d);
        //    Node L = ChooseLeaf(RN, pnt, nodeSize,fh, maxCap, d);
            rootId = insert(rootId,d,maxCap,fh,nodeSize,pnt,nodeIDCtr, fm);
            arr[0] = rootId;
            // cout<<rootId<<" this is the rootid___________"<<endl;
			fo<<"INSERT\n";
			fo<<"\n";
			
		}
		else if(word.compare("QUERY")==0){
			for(int k=0;k<d;k++){
				fi>>word;
				pnt[k]=stoi(word);
                // cout<<word<<" ";                
				//pnt.push_back(stoi(word));
			}  
            // cout<<"Search Started"<<endl;          
			if(search(arr[0],d,maxCap,nodeSize,pnt,fm,fh) ){
				fo<<"TRUE\n";
				fo<<"\n";
			}
			else{
				fo<<"FALSE\n";
				fo<<"\n";                
			}

		}
	}
	
	fi.close();
	fo.close();

	
	fm.CloseFile(fh);
	//fm.CloseFile(fh2);

	fm.DestroyFile ("temp.txt");
	return 0;

}


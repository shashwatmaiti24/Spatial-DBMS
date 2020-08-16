
#include<iostream>
#include <fstream>
#include "file_manager.h"
#include "errors.h"
#include <vector>
#include <cstring>
using namespace std;

const int INT_MIN=-1;
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
        cout<<"Problem in Devectorify";
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
}
void assignParent(int startIdx,int endidx,int maxCap,int& nodeIDCtr,int nodeSize,int d,FileManager& fm, FileHandler& fh);
int BulkLoad(FileManager& fm,FileHandler& fh,FileHandler& fh1,int d,int maxCap,int N,int& nodeIDCtr,int nodeSize){
    
    PageHandler rph;
	std::cout << "Rtree File created " << endl;     
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
    std::cout<<"Num Pages "<<p0+1<<" == "<<numPage<<" And N is"<<N<<endl;
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
                std::cout<<"flush page started"<<endl;
                fh.MarkDirty(rph.GetPageNum() );
                fh.UnpinPage(rph.GetPageNum());
                offset=0;
                if (entryCtr<N){
                    rph=fh.NewPage();
                    rData=rph.GetData();
                }
                else {
                    cout<<"Break1 "<<N<<endl;
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
                std::cout<<"Problem in vectorify "<<v1.size()<<" "<<nodeSize<<endl;
            }    
            offset+=nodeSize;      
        }        
        if (entryCtr<N){
            std::cout<<"getting next page"<<endl;            
            fh1.UnpinPage(p0);
            p0++;
            ph=fh1.PageAt(p0);
            data=ph.GetData();                        
        } else {
            std::cout<<"break 2 "<<N<<endl;
            break;
        }
    }
    //outfile.close();
    cout<<N<<" Going To Assignparents "<<nodeIDCtr<<endl;    
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
            cout<<"ASSIGN  breaking loop"<<endl;
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
    cout<<"rph pagenum "<<rph.GetPageNum()<<endl;
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
        cout<<"Calling Assign Parent Again"<<numNode<<endl;
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
    P.print();
    std::cout<<"STARTING POINT ";
    for (auto x:point){
        std::cout<<x<<" ";
    }
    std::cout<<"END POINT"<<endl;
    for(int i=0;i<P.childId.size();i++){
        if (maxChildId<P.childId[i]) maxChildId=P.childId[i];
    }    
    if (maxChildId<0){
        std::cout<<"AT LEAF NODE"<<endl;
        for (int i=0;i<d;i++){
            if (point[i]!=P.mbr[i]){
                return false;
            }
        }
        //cout<<"Returning True";
        return true;
    }
    else{
        bool res=false;
        cout<<"Looking Inside "<<endl;;
        for (int i=0;i<maxCap;i++){
            cout<<P.childId[i]<<endl;;
            if (P.childId[i]>=0){
                for (int j=0;j<2*d;j++){
                    cout<<P.childMbr[i][j]<<" ";
                }
                cout<<endl;
                if (isInside(P.childMbr[i],point,d)){
                    cout<<"THE POINT MAY BE ISIDE THIS CHILD "<<P.childId[i]<<endl;
                    if(search(P.childId[i],d,maxCap,nodeSize,point,fm,fh)) return true;
                }
            }
        }
        return res;        
    }   
    
}


int main(int argc, char** argv){
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
				// pnt[k]=stoi(word);
				pnt.push_back(stoi(word));
			}			
			fo<<"INSERT\n";
			fo<<"\n";
			
		}
		else if(word.compare("QUERY")==0){
			for(int k=0;k<d;k++){
				fi>>word;
				pnt[k]=stoi(word);
                cout<<word<<" ";                
				//pnt.push_back(stoi(word));
			}  
            cout<<endl;          
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


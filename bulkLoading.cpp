
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
void assignParent(int startIdx,int endidx,int maxCap,int& nodeIDCtr,int nodeSize,int d,FileHandler& fh);
int BulkLoad(int d,int maxCap,int N,char* filename, char* rtreefilename,int& nodeIDCtr,int nodeSize){
    FileManager fm;
    FileHandler fh = fm.CreateFile(rtreefilename);
    PageHandler rph;
	cout << "Rtree File created " << endl;
    FileManager fm1;
    FileHandler fh1=fm1.OpenFile(filename);   
    PageHandler ph=fh1.FirstPage();
    //PageHandler ph2=fh1.LastPage();
    int p0=ph.GetPageNum();
    int p1=fh1.LastPage().GetPageNum();
  

    int entryPerPage=PAGE_SIZE/(sizeof(int)*d);
    int numPage=N/entryPerPage;
    if (N%entryPerPage!=0){
        numPage++;
    }

    nodeIDCtr=0;
    int offset=0;
    cout<<"Num Pages "<<p1-p0+1<<" == "<<numPage<<endl;
    rph=fh.NewPage();
    char* rData;
    char* data=ph.GetData();
    int entryCtr=0;
    while(p0<=p1){
        int upperLimit=min(entryPerPage,N-entryCtr);        
        for (int i=0;i<upperLimit;i++){
            vector<int> v(d,-1);
            for(int j=0;j<d;j++){
                memcpy(&v[j],&data[4*i*d+4*j],sizeof(int));
                cout<<"v["<<i<<"]"<<"["<<j<<"] = "<<v[j];                
            }
            entryCtr++;

            //Flushing page
            if ((PAGE_SIZE-offset)<nodeSize){
                fh.MarkDirty(rph.GetPageNum() );
                fh.UnpinPage(rph.GetPageNum());
                offset=0;
                if (entryCtr<N){
                    rph=fh.NewPage();
                }
                else break;
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
            memcpy(&rData[offset],&n.id,sizeof(int));
            offset+=4;
            memcpy(&rData[offset],&n.parentId,sizeof(int));
            offset+=4;
            for (int i=0;i<n.mbr.size();i++){
                memcpy(&rData[offset],&n.mbr[i],sizeof(int));
                offset+=4;
            }
            for (int i=0;i<n.childId.size();i++){
                memcpy(&rData[offset],&n.childId[i],sizeof(int));
                offset+=4;
            }
            for (int i=0;i<n.childMbr.size();i++){
                for(int j=0;j<2*d;j++){
                    memcpy(&rData[offset],&n.mbr[i],sizeof(int));
                    offset+=4;
                }
            }           
        }
        if (entryCtr<N){
            p0++;
            fh1.UnpinPage(p0);
            ph=fh1.PageAt(p0);
            data=ph.GetData();                        
        } else break;
    }    
    assignParent(0,nodeIDCtr,maxCap,nodeIDCtr,nodeSize,d,fh);
    fm.CloseFile(fh);
    fm1.CloseFile(fh1);
    
}

void assignParent(int startIdx,int endidx,int maxCap,int& nodeIDCtr,int nodeSize,int d,FileHandler& fh){
    int numNode=endidx-startIdx;
    int numPNode=0;
    int startPidx=endidx;
    int block=numNode/maxCap;
    int blockRem=numNode%maxCap;

    int nodePerPage=PAGE_SIZE/nodeSize;
    int pageNum=(endidx)/nodePerPage;
    int offset=((endidx)%nodePerPage)*nodeSize;
    PageHandler rph;
    if (offset=0){
        rph=fh.NewPage();
    }
    for (int i=0;i<block;i++){
        if ((PAGE_SIZE-offset)<nodeSize){
            fh.MarkDirty(pageNum);
            fh.UnpinPage(pageNum);
            fh.FlushPages();
            rph=fh.NewPage();
            pageNum=rph.GetPageNum();
        }
        Node P;
        P.id=nodeIDCtr;
        nodeIDCtr++;
        P.parentId=-1;
        P.mbr.resize(2*d);
        for(int i=0;i<d;i++){
            P.mbr[i]=INT32_MAX;
            P.mbr[i+d]=INT32_MIN;
        }
        P.childId.resize(maxCap,INT32_MIN);
        P.childMbr.resize(maxCap);
        for(int i=0;i<maxCap;i++){
            for(int j=0;j<d;j++){
                P.childMbr[i][j]=INT32_MAX;
                P.childMbr[i][j+d]=INT32_MIN;
            }
        }
        for(int j=0;j<maxCap;j++){
            int childPnum=startIdx/nodePerPage;
            P.childId[j]=startIdx;
            startIdx++;

            PageHandler childPage=fh.PageAt(childPnum);
            char* childData=childPage.GetData();
            int childOffset=startIdx%nodePerPage*nodeSize;
            memcpy(&childData[offset+4],&P.id,sizeof(int));
            vector<int> childmbr(2*d);
            P.childMbr[j].resize(2*d);
            for (int i=0;i<2*d;i++){
                int mbrCordinate=-1;
                memcpy(&mbrCordinate,&childData[offset+8+4*i],sizeof(int));
                P.childMbr[j][i]=mbrCordinate;
                if (i<d){
                    P.mbr[i]=min(P.mbr[i],mbrCordinate);
                }
                else{
                    P.mbr[i]=max(P.mbr[i],mbrCordinate);
                }
            }
            fh.MarkDirty(childPnum);
            fh.UnpinPage(childPnum);
            fh.FlushPages();
        }
        //Flush Node
            char* rphData=rph.GetData();
            int additive=0;
            memcpy(&rphData[offset+additive],&P.id,sizeof(int));
            additive+=4;
            memcpy(&rphData[offset+additive],&P.parentId,sizeof(int));
            additive+=4;
            for (int i=0;i<P.mbr.size();i++){
                memcpy(&rphData[offset+additive],&P.mbr[i],sizeof(int));
                additive+=4;
            }
            for (int i=0;i<P.childId.size();i++){
                memcpy(&rphData[offset+additive],&P.childId[i],sizeof(int));
                additive+=4;
            }
            for (int i=0;i<P.childMbr.size();i++){
                for(int j=0;j<2*d;j++){
                    memcpy(&rphData[offset+additive],&P.childMbr[i][j],sizeof(int));
                    additive+=4;
                }
            }
            offset=offset+additive;           

    }
    if (numNode%maxCap!=0){
        if ((PAGE_SIZE-offset)<nodeSize){
            fh.MarkDirty(pageNum);
            fh.UnpinPage(pageNum);
            fh.FlushPages();
            rph=fh.NewPage();
            pageNum=rph.GetPageNum();
        }
        Node P;
        P.id=nodeIDCtr;
        nodeIDCtr++;
        P.parentId=-1;
        P.mbr.resize(2*d);
        for(int i=0;i<d;i++){
            P.mbr[i]=INT32_MAX;
            P.mbr[i+d]=INT32_MIN;
        }
        P.childId.resize(maxCap,INT32_MIN);
        P.childMbr.resize(maxCap);
        for(int i=0;i<maxCap;i++){
            for(int j=0;j<d;j++){
                P.childMbr[i][j]=INT32_MAX;
                P.childMbr[i][j+d]=INT32_MIN;
            }
        }
        for(int j=0;j<numNode%maxCap;j++){
            int childPnum=startIdx/nodePerPage;
            P.childId[j]=startIdx;
            startIdx++;

            PageHandler childPage=fh.PageAt(childPnum);
            char* childData=childPage.GetData();
            int childOffset=startIdx%nodePerPage*nodeSize;
            memcpy(&childData[offset+4],&P.id,sizeof(int));
            vector<int> childmbr(2*d);
            P.childMbr[j].resize(2*d);
            
            for (int i=0;i<2*d;i++){
                int mbrCordinate=-1;
                memcpy(&mbrCordinate,&childData[offset+8+4*i],sizeof(int));
                P.childMbr[j][i]=mbrCordinate;
                if (i<d){
                    P.mbr[i]=min(P.mbr[i],mbrCordinate);
                }
                else{
                    P.mbr[i]=max(P.mbr[i],mbrCordinate);
                }
            }
            fh.MarkDirty(childPnum);
            fh.UnpinPage(childPnum);
            fh.FlushPages();
        }
        //Flush Node
            char* rphData=rph.GetData();
            int additive=0;
            memcpy(&rphData[offset+additive],&P.id,sizeof(int));
            additive+=4;
            memcpy(&rphData[offset+additive],&P.parentId,sizeof(int));
            additive+=4;
            for (int i=0;i<P.mbr.size();i++){
                memcpy(&rphData[offset+additive],&P.mbr[i],sizeof(int));
                additive+=4;
            }
            for (int i=0;i<P.childId.size();i++){
                memcpy(&rphData[offset+additive],&P.childId[i],sizeof(int));
                additive+=4;
            }
            for (int i=0;i<P.childMbr.size();i++){
                for(int j=0;j<2*d;j++){
                    memcpy(&rphData[offset+additive],&P.childMbr[i][j],sizeof(int));
                    additive+=4;
                }
            }
            offset=offset+additive;         


    }
    if (numNode>maxCap){
        assignParent(endidx,nodeIDCtr,maxCap,nodeIDCtr,nodeSize,d,fh);
    }
}


bool search(int xnode,int d,int maxCap,int nodeSize,vector<int>& point,FileHandler& fh){
    int nodePerPage=PAGE_SIZE/nodeSize;
    int pageNum=(xnode)/nodePerPage;
    int offset=((xnode)%nodePerPage)*nodeSize;

    int maxChildId=INT32_MIN;

    PageHandler ph=fh.PageAt(pageNum);
    char* Data=ph.GetData();  
    Node P;
    int additive=0;  
    memcpy(&P.id,&Data[offset+additive],sizeof(int));
    additive+=4;
    memcpy(&P.parentId,&Data[offset+additive],sizeof(int));
    additive+=4;
    P.mbr.resize(2*d);
    for (int i=0;i<2*d;i++){
        memcpy(&P.mbr[i],&Data[offset+additive],sizeof(int));
        additive+=4;
    }
    P.childId.resize(maxCap,INT32_MIN);
    for(int i=0;i<maxCap;i++){
        memcpy(&P.childId[i],&Data[offset+additive],sizeof(int));
        additive+=4;
        maxChildId=max(maxChildId,P.childId[i]);
    }
    P.childMbr.resize(maxCap);
    for(int i=0;i<maxCap;i++){
        for(int j=0;j<d;j++){
            P.childMbr[i][j]=INT32_MAX;
            P.childMbr[i][j+d]=INT32_MIN;
        }
    }
    for(int i=0;i<maxCap;i++){
        for(int j=0;j<2*d;j++){
            memcpy(&P.childMbr[i][j],&Data[offset+additive],sizeof(int));
            additive+=4;
        }
    }
    fh.UnpinPage(pageNum);
    if (maxChildId<0){
        for (int i=0;i<d;i++){
            if (point[i]!=P.mbr[i]){
                return false;
            }
        }
        return true;
    }
    else{
        for(int i=0;i<d;i++){
            if ((point[i]<P.mbr[i])||(point[i]>P.mbr[i+d])){
                return false;
            }            
        }
        bool res=false;
        for(int i=0;i<maxCap;i++){
            if(P.childId[i]>=0){
                bool var=true;
                for(int j=0;j<d;j++){
                    if ((point[j]<P.childMbr[i][j])||(point[j]>P.childMbr[i][j+d])){
                        var=false;
                        break;
                    }
                }
                if (var){
                    res=res||search(P.childId[i],d,maxCap,nodeSize,point,fh);
                    if (res==true){
                        return true;
                    }
                }
            }
        }
        return false;
    }   
    
}


int main(int argc, char *argv[])
{
    const char* query=argv[1];
    int maxCap=atoi(argv[2]);
    int d=atoi(argv[3]);
    char* output=argv[4];
    ofstream outfile;
    outfile.open(output);
    ifstream infile;
    infile.open(query);
    char* rtreefilename="rtree.txt";
    int nodeIDctr=0;
    int nodeSize=32*(2+2*d+maxCap+maxCap*2*d);
    int rootNode;
    vector<int> point(d);
    while (true) {
        char* type;
        int x;
        infile >> type;
        if (type[0]=='B'){
            char* filename;
            int N;
            infile>>filename>>N;
            BulkLoad(d,maxCap,N,filename,rtreefilename,nodeIDctr,nodeSize);
            rootNode=nodeIDctr;
            outfile<<"BULKLOAD"<<'\n'<<'\n';
        }
        else if(type[0]=='I'){
            //INSERT FUNCTION
            for(int i=0;i<d;i++)infile>>point[i];
            outfile<<"INSERT"<<'\n'<<'\n';

        }
        else{
            FileManager fm;
            FileHandler fh=fm.OpenFile(rtreefilename);
            for(int i=0;i<d;i++)infile>>point[i];
            bool res=search(rootNode,d,maxCap,nodeSize,point,fh);
            if(res) outfile<<"TRUE"<<'\n'<<'\n';
            else outfile<<"FALSE"<<'\n'<<'\n';
            fm.CloseFile(fh);
        }

        if( infile.eof() ) break;
    
    } 


    return 0;
}



#include "rtree.hpp"
#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>
using namespace std;

const int INT_MIN=-1;
void flushNode(){

}

int BulkLoad(int d,int maxCap,int N,const char* filename,const char* rtreefilename,int& nodeIDCtr,int nodeSize){
    FileManager fm;
    FileHandler fh = fm.CreateFile(rtreefilename);
    PageHandler rph;
	cout << "Rtree File created " << endl;
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
        Node P;
        P.id=nodeIDCtr;
        nodeIDCtr++;
        P.parentId=-1;
        P.mbr.resize(2*d);
        P.childId.resize(maxCap);
        P.childMbr.resize(maxCap);
        for(int j=0;j<maxCap;j++){

        }       
    }
    if (numPNode>1){

    }
}



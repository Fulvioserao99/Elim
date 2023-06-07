#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

double smthreshold; //35.8
int tsize; //4

class TNode{
    private:
        Rect region;
        TNode *UL,*UR,*LL,*LR;
        vector<TNode *> merged;
        vector<bool> mergedB=vector<bool>(4,false);
        double stddev, mean;
    public:
        TNode(Rect R){region=R;UL=UR=LL=LR=nullptr;};
        
        TNode *getUL(){return UL;};
        TNode *getUR(){return UR;};
        TNode *getLL(){return LL;};
        TNode *getLR(){return LR;};
        
        void setUL(TNode *N){UL=N;};
        void setUR(TNode *N){UR=N;};
        void setLL(TNode *N){LL=N;};
        void setLR(TNode *N){LR=N;};
        
        double getStdDev(){return stddev;};
        double getMean(){return mean;};
        
        void setStdDev(double stddev){this->stddev=stddev;};
        void setMean(double mean){this->mean=mean;};

        void addRegion(TNode *R){merged.push_back(R);};
        vector<TNode *> &getMerged(){return merged;};

        Rect &getRegion(){return region;};

        void setMergedB(int i){mergedB[i]=true;};
        bool getMergedB(int i){return mergedB[i];};
};

TNode *split(Mat &img,Rect R){

    TNode *root=new TNode(R);

    Scalar stddev, mean;
    meanStdDev(img(R), mean, stddev);

    root->setMean(mean[0]);
    root->setStdDev(stddev[0]);

    if(R.width>tsize && root->getStdDev()>smthreshold){

        Rect ul(R.x,R.y,R.height/2,R.width/2);
        root->setUL(split(img,ul));

        Rect ur(R.x,R.y+R.width/2,R.height/2,R.width/2);
        root->setUR(split(img,ur));

        Rect ll(R.x+R.height/2,R.y,R.height/2,R.width/2);
        root->setLL(split(img,ll));

        Rect lr(R.x+R.height/2,R.y+R.width/2,R.height/2,R.width/2);
        root->setLR(split(img,lr));
    }

    rectangle(img, R, Scalar(0));
    return root;
}

/*
 _____   ____
|UL|UR| |0|1|
------  -----
|LL|LR| |3|2|
------- -----
*/

void merge(TNode *root){

    if(root->getRegion().width>tsize && root->getStdDev()>smthreshold){
        if(root->getUL()->getStdDev()<=smthreshold && root->getUR()->getStdDev()<=smthreshold){//UL-UR
            root->addRegion(root->getUL()); root->setMergedB(0);
            root->addRegion(root->getUR()); root->setMergedB(1);
            if(root->getLL()->getStdDev()<=smthreshold && root->getLR()->getStdDev()<=smthreshold){
                root->addRegion(root->getLL()); root->setMergedB(3);
                root->addRegion(root->getLR()); root->setMergedB(2);
            }else{
                merge(root->getLL());
                merge(root->getLR());
            }
        }else if(root->getUR()->getStdDev()<=smthreshold && root->getLR()->getStdDev()<=smthreshold){//UR-LR
            root->addRegion(root->getUR()); root->setMergedB(1);
            root->addRegion(root->getLR()); root->setMergedB(2);
            if(root->getUL()->getStdDev()<=smthreshold && root->getLL()->getStdDev()<=smthreshold){
                root->addRegion(root->getUL()); root->setMergedB(0);
                root->addRegion(root->getLL()); root->setMergedB(3);
            }else{
                merge(root->getUL());
                merge(root->getLL());
            }
        }else if(root->getLL()->getStdDev()<=smthreshold && root->getLR()->getStdDev()<=smthreshold){//LL-LR
            root->addRegion(root->getLL()); root->setMergedB(3);
            root->addRegion(root->getLR()); root->setMergedB(2);
            if(root->getUL()->getStdDev()<=smthreshold && root->getUR()->getStdDev()<=smthreshold){
                root->addRegion(root->getUL()); root->setMergedB(0);
                root->addRegion(root->getUR()); root->setMergedB(1);
            }else{
                merge(root->getUL());
                merge(root->getUR());
            }
        }else if(root->getUL()->getStdDev()<=smthreshold && root->getLL()->getStdDev()<=smthreshold){//UL-LL
            root->addRegion(root->getUL()); root->setMergedB(0);
            root->addRegion(root->getLL()); root->setMergedB(3);
            if(root->getUR()->getStdDev()<=smthreshold && root->getLR()->getStdDev()<=smthreshold){
                root->addRegion(root->getUR()); root->setMergedB(1);
                root->addRegion(root->getLR()); root->setMergedB(2);
            }else{
                merge(root->getUR());
                merge(root->getLR());
            }
        }else{
            merge(root->getUL());
            merge(root->getUR());
            merge(root->getLR());
            merge(root->getLL());
        }
    }else{
        root->addRegion(root); root->setMergedB(0); root->setMergedB(1); root->setMergedB(2); root->setMergedB(3);
    }
}

void segment(TNode *root,Mat &img){

    vector<TNode *> tmp=root->getMerged();

    if(!tmp.size()){
        segment(root->getUL(),img);
        segment(root->getUR(),img);
        segment(root->getLR(),img);
        segment(root->getLL(),img);
    }else{
        double val=0;
        for(auto x:tmp){
            val+=(int)x->getMean();
        }
        val/=tmp.size();

        for(auto x:tmp){
            img(x->getRegion())=(int)val;
        }

        if(tmp.size()>1){
            if(!root->getMergedB(0))
                segment(root->getUL(),img);
            if(!root->getMergedB(1))
                segment(root->getUR(),img);
            if(!root->getMergedB(2))
                segment(root->getLR(),img);
            if(!root->getMergedB(3))
                segment(root->getLL(),img);
        }
    }
}

void drawMerged(TNode *root,Mat &img){
    vector<TNode *> tmp=root->getMerged();

    if(!tmp.size()){
        drawMerged(root->getUL(),img);
        drawMerged(root->getUR(),img);
        drawMerged(root->getLR(),img);
        drawMerged(root->getLL(),img);
    }else if(tmp.size()==1){
        rectangle(img, tmp[0]->getRegion(), Scalar(0));
    }else{
        if((root->getMergedB(0)&&root->getMergedB(1)) || (root->getMergedB(2)&&root->getMergedB(3)))
            rectangle(img, Rect(tmp[0]->getRegion().x,tmp[0]->getRegion().y,tmp[0]->getRegion().height,tmp[0]->getRegion().width*2), Scalar(0));
        else if((root->getMergedB(0)&&root->getMergedB(3)) || (root->getMergedB(1)&&root->getMergedB(2)))
            rectangle(img, Rect(tmp[0]->getRegion().x,tmp[0]->getRegion().y,tmp[0]->getRegion().height*2,tmp[0]->getRegion().width), Scalar(0));
        if(!root->getMergedB(0))
            drawMerged(root->getUL(),img);
        if(!root->getMergedB(1))
            drawMerged(root->getUR(),img);
        if(!root->getMergedB(2))
            drawMerged(root->getLR(),img);
        if(!root->getMergedB(3))
            drawMerged(root->getLL(),img);
    }  
}

int main( int argc, char** argv ){

    Mat src = imread( argv[1], IMREAD_GRAYSCALE );
    
    if(src.empty())
        exit(-1);

    smthreshold=stod(argv[2]);
    tsize=stoi(argv[3]);

    GaussianBlur(src, src, Size(5,5), 0, 0);

    int exponent = log(min(src.cols, src.rows)) / log (2);
    int s = pow(2.0, (double)exponent);
    Rect square = Rect(0,0, s,s);
    src = src(square).clone();
    Mat srcMer=src.clone();
    Mat srcSeg=src.clone();

    TNode *root=split(src,Rect(0,0,src.rows,src.cols));

    merge(root);

    drawMerged(root,srcMer);

    segment(root,srcSeg);

    imshow("Quad Tree", src);
    imshow("Merged", srcMer);
    imshow("Segmented", srcSeg);
    waitKey(0);

    return 0;
}
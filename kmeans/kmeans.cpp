//
//  main.cpp
//  Segmentation KMeans
//
//  Created on 2/5/18.
//  Copyright © 2018. All rights reserved.
//

#include <iostream>
#include <ctime>
#include <cmath>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;
//using namespace Spectra;

struct Center{
    double blueValue;
    double greenValue;
    double redValue;
};

void createClustersInfo(Mat imgInput, int clusters_number, vector<Scalar> & clustersCenters, vector<vector<Point>> & ptInClusters){
    
    RNG random(cv::getTickCount());
    
    for(int k =0; k<clusters_number; k++){
        
        //get random pixel in image to initialize cluster center
        Point centerKPoint;
        centerKPoint.x = random.uniform(0, imgInput.cols);
        centerKPoint.y = random.uniform(0, imgInput.rows);
        Scalar centerPixel = imgInput.at<Vec3b>(centerKPoint.y, centerKPoint.x);
        
        //get color value of pixel and save it as a center
        Scalar centerK(centerPixel.val[0], centerPixel.val[1], centerPixel.val[2]);
        clustersCenters.push_back(centerK);
        
        //create vector to store future associated pixel to each center
        vector<Point> ptInClusterK;
        ptInClusters.push_back(ptInClusterK);
    }
}

double computeColorDistance(Scalar pixel, Scalar clusterPixel){
    
    //use color difference to get distance to cluster
    double diffBlue = pixel.val[0] - clusterPixel[0];
    double diffGreen = pixel.val[1] - clusterPixel[1];
    double diffRed = pixel.val[2] - clusterPixel[2];
    
    //use euclidian distance to get distance
    double distance = sqrt(pow(diffBlue, 2) + pow(diffGreen,2) + pow(diffRed,2));
    
    return distance;
}

void findAssociatedCluster(Mat imgInput, int clusters_number, vector<Scalar> clustersCenters, vector<vector<Point>> & ptInClusters){
    
    // For each pixel, find closest cluster
    for(int r = 0 ; r<imgInput.rows; r++){
        for(int c = 0; c<imgInput.cols; c++){
            
            double minDistance = INFINITY;
            int closestClusterIndex = 0;
            Scalar pixel = imgInput.at<Vec3b>(r,c);
            
            for(int k = 0; k<clusters_number; k++){
                
                Scalar clusterPixel = clustersCenters[k];
                
                //use color difference to get distance to cluster
                double distance = computeColorDistance(pixel, clusterPixel);
               
                //update to closest cluster center
                if(distance < minDistance){
                    minDistance = distance;
                    closestClusterIndex = k;
                }
            }
            
            //save pixel into associated cluster
            ptInClusters[closestClusterIndex].push_back(Point(c,r));
        }
    }
}

double adjustClusterCenters(Mat imgInput, int clusters_number, vector<Scalar> & clustersCenters, vector<vector<Point>> ptInClusters, double & oldCenter, double newCenter){
    
    double diffChange;
    
    //adjust cluster center to mean of associated pixels
    for(int k =0; k<clusters_number; k++){
        
        vector<Point> ptInCluster = ptInClusters[k];
        double newBlue = 0;
        double newGreen = 0;
        double newRed = 0;
        
        //compute mean values for 3 channels
        for(int i=0; i<ptInCluster.size(); i++){
            Scalar pixel = imgInput.at<Vec3b>(ptInCluster[i].y,ptInCluster[i].x);
            newBlue += pixel.val[0];
            newGreen += pixel.val[1];
            newRed += pixel.val[2];
        }
        
        newBlue /= ptInCluster.size();
        newGreen /= ptInCluster.size();
        newRed /= ptInCluster.size();
        
        //assign new color value to cluster center
        Scalar newPixel(newBlue, newGreen, newRed);

        //compute distance between the old and new values
        newCenter += computeColorDistance(newPixel,clustersCenters[k]);

        clustersCenters[k] = newPixel;
        
    }
    
    newCenter /= clusters_number;
    
    //get difference between previous iteration change
    diffChange = abs(oldCenter - newCenter);
    cout << "diffChange is: " << diffChange << endl;
    oldCenter = newCenter;
    
    return diffChange;
}

Mat applyFinalClusterToImage(Mat & imgOutput, int clusters_number, vector<vector<Point>> ptInClusters, vector<Scalar> & clustersCenters){
    
    srand(time(NULL));
    
    //assign random color to each cluster
    for(int k =0; k<clusters_number; k++){
        vector<Point> ptInCluster = ptInClusters[k];
        
        //Scalar randomColor(rand() % 255,rand() % 255,rand() % 255);
        
        //for each pixel in cluster change color to fit cluster
        for(int i=0; i<ptInCluster.size(); i++){
            
            Scalar pixelColor = imgOutput.at<Vec3b>(ptInCluster[i]);
            pixelColor = clustersCenters[k];//randomColor;
            
            imgOutput.at<Vec3b>(ptInCluster[i])[0] = pixelColor.val[0];
            imgOutput.at<Vec3b>(ptInCluster[i])[1] = pixelColor.val[1];
            imgOutput.at<Vec3b>(ptInCluster[i])[2] = pixelColor.val[2];
        }
    }
    
    return imgOutput;
}

int main(int argc, const char * argv[]) {
    
    Mat imgInput = imread(argv[1],IMREAD_COLOR);
    
    if(imgInput.empty()){
        printf("Error opening image.\n");
        return -1;
    }
    
    //---------------------- K-MEANS -----------------------------
    
    //The number of cluster is the only parameter to choose
    int clusters_number = stoi(argv[2]);
    
    //set up cluster center, cluster vector, and parameter to stop the iterations
    vector<Scalar> clustersCenters;
    vector< vector<Point> > ptInClusters;
    double threshold = 0.1;
    double oldCenter=INFINITY;
    double newCenter=0;
    double diffChange = oldCenter - newCenter;
    
    //create ramdom clusters centers and clusters vectors
    createClustersInfo(imgInput, clusters_number, clustersCenters, ptInClusters);
    
    //iterate until cluster centers nearly stop moving (using threshold)
    while( diffChange > threshold){
        
        //reset change
        newCenter = 0;
        
        //clear associated pixels for each cluster
        for(int k=0; k<clusters_number; k++){
            ptInClusters[k].clear();
        }
        
        //find all closest pixel to cluster centers
        findAssociatedCluster(imgInput, clusters_number, clustersCenters, ptInClusters);
        
        //recompute cluster centers values
        diffChange = adjustClusterCenters(imgInput, clusters_number, clustersCenters, ptInClusters, oldCenter, newCenter);
    }
    
    Mat imgOutputKNN = imgInput.clone();
    imgOutputKNN = applyFinalClusterToImage(imgOutputKNN, clusters_number, ptInClusters, clustersCenters);
    imshow("Segmentation", imgOutputKNN);
    
    waitKey(0);
    return 0;
}


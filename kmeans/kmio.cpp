#include <iostream>
#include <time.h>
#include <cmath>
#include <vector>
#include "opencv2/opencv.hpp"



using namespace std;
using namespace cv;

int clusters_number;
vector<Scalar> centroids;
vector<vector<Point>> clusters;


void create_clusters(Mat image){

    int rand_i,rand_j;
    for (int i=0; i<clusters_number; ++i){

        rand_i = rand()%image.rows;
        rand_j = rand()%image.cols;

        Scalar centroid = image.at<Vec3b>(rand_i,rand_j);

        centroids.push_back(centroid);

        vector<Point> inCluster;
        clusters.push_back(inCluster);

    }

}

double squaredSumDist(Scalar a, Scalar b){

    double distance;

    distance = pow(a.val[0] - b.val[0],2)+
               pow(a.val[1] - b.val[1],2)+
               pow(a.val[2] - b.val[2],2);

    return distance;

}


void find_cluster(Mat image){

    for(int i=0; i<image.rows; ++i){

        for (int j=0; j<image.cols; ++j){

            double min_dist = INFINITY;
            double dist = 0;
            int index = 0;

            Scalar pixel = image.at<Vec3b>(i,j);

            for (int k=0; k<clusters_number; ++k){

                dist = squaredSumDist(pixel,centroids.at(k));

                if(dist < min_dist){

                    min_dist = dist;
                    index = k;
                }
            }

            clusters.at(index).push_back(Point(j,i));
        }
    }

}


double calc_centroids(Mat image, double &new_center, double &old_center){

    int blue=0,green=0,red=0;

    double diff=0;

    for(int i=0; i<clusters_number; ++i){



        for (int j=0; j<clusters.at(i).size(); ++j){

            Scalar pixel = image.at<Vec3b>(clusters.at(i).at(j).y, clusters.at(i).at(j).x);

            blue+=pixel[0];
            green+=pixel[1];
            red+=pixel[2];


        }

        blue/=clusters.at(i).size();
        green/=clusters.at(i).size();
        red/=clusters.at(i).size();



        centroids.at(i) = Scalar(blue,green,red);


    }


    new_center/=clusters_number;

    diff = abs(old_center-new_center);
    old_center = new_center;

    return diff;

}

Mat see_output(Mat image){

    Mat output(image.size(),image.type());

    for (int i=0; i<clusters_number; ++i){

        Scalar random(rand()%255,rand()%255,rand()%255);

        for(int j=0; j<clusters.at(i).size(); ++j){

            output.at<Vec3b>(clusters.at(i).at(j))[0] = random.val[0];
            output.at<Vec3b>(clusters.at(i).at(j))[1] = random.val[1];
            output.at<Vec3b>(clusters.at(i).at(j))[2] = random.val[2];
        }

    }
    return output;
}

int main (int argc, char **argv){

    srand(time(NULL));

    Mat image = imread(argv[1],IMREAD_COLOR);

    clusters_number = stoi(argv[2]);


    if(image.empty()){
        printf("Error opening image.\n");
        return -1;
    }

    create_clusters(image);

    double threshold = 0.1;
    double new_center = 0;
    double old_center = INFINITY;
    double diff = old_center - new_center;

    while(diff > threshold){

        for(int k=0; k<clusters_number; k++)
            clusters.at(k).clear();

        new_center = 0;

        find_cluster(image);
        diff = calc_centroids(image,new_center,old_center);

    }

    Mat output = see_output(image);

    imshow("Input", image);
    imshow("Output", output);

    waitKey(0);

    return 0;




}

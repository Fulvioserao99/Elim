#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

void otsu (Mat image, int &th1, int &th2);




int main( int argc, char** argv )
{
    string imageName;


    if( argc < 2){
        cout<<"usage: "<<argv[0]<<" image_name"<<endl;
        exit(0);
    }
    imageName = argv[1];


    Mat image = imread(imageName, IMREAD_GRAYSCALE);


    if( image.empty() ){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    Mat outputmio(image.size(), CV_8UC1);

    imshow("Immagine originale",image);


    int th1,th2;
    otsu(image,th1,th2);
    cout << th1 << " " << th2 << endl;



    for (int i=0; i<image.rows; ++i){

        for (int j=0; j<image.cols; ++j){

            if (image.at<uchar>(i,j) >= th2)

                outputmio.at<uchar>(i,j) = 255;

           else if(image.at<uchar>(i,j) >= th1 && image.at<uchar>(i,j) < th2)

                outputmio.at<uchar>(i,j) = 127;

            else
                outputmio.at<uchar>(i,j) = 0;

        }
    }

    imshow("Output", outputmio);



    waitKey(0);



    return 0;

}




void otsu (Mat image, int &th1, int &th2){


    GaussianBlur(image,image,Size(5,5),0);
    float sigma[256][256]={0.0};
    float sigma_max=0.0;
    int MN = image.rows*image.cols;
    vector<float> histogram(MN,0.0);
    int output = 0;
    float mG=0.0;
    int temp1, temp2;

    for (int i=0; i<image.rows; i++)
        for (int j=0; j<image.cols; j++){
            histogram.at(image.at<uchar>(i,j))++;

        }

    for (int i=0; i<MN; i++)
            histogram.at(i)/=MN;

    for (int i=1; i<256; i++)
        mG+=histogram.at(i)*i;

    for(int k=0; k<256; k++){

        for (int l=k+1; l<256; l++){

            float P1=0.0,P2=0.0,P3=0.0;

            for (int i=1; i<=k; i++)
                P1+=histogram.at(i);

            for (int i=k+1; i<=l; i++)
                P2+=histogram.at(i);

            for (int i=l+1; i<256; i++)
                P3+=histogram.at(i);


            float med_cum_A=0.0,med_cum_B=0.0, med_cum_C=0.0;


            for (int i=1; i<=k; i++)
                med_cum_A+=(histogram.at(i)*i)/P1;

            for (int i=k+1; i<=l; i++)
                med_cum_B+=(histogram.at(i)*i)/P2;

            for (int i=l+1; i<256; i++)
                med_cum_C+=(histogram.at(i)*i)/P3;

            sigma[k][l] = P1*((med_cum_A-mG)*(med_cum_A-mG)) + P2*((med_cum_B-mG)*(med_cum_B-mG)) + P3*((med_cum_C-mG)*(med_cum_C-mG));

            if(sigma[k][l] > sigma_max){

                sigma_max = sigma[k][l];
                temp1 = k;
                temp2 = l;
            }

        }
    }

    th1 = temp1;
    th2 = temp2;

}


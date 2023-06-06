#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

int otsu (Mat image);




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

    Mat outputmio;

    imshow("Immagine originale",image);


    int th = otsu(image);

    threshold(image,outputmio,th,255,THRESH_BINARY);

    imshow("Output", outputmio);



    waitKey(0);



    return 0;

}




int otsu (Mat image){

    float sigma_max=0.0;
    int MN = image.rows*image.cols;
    vector<float> histogram(MN,0.0);
    int output = 0;

    for (int i=0; i<image.rows; i++)
        for (int j=0; j<image.cols; j++){
            histogram.at(image.at<uchar>(i,j))++;

        }



    for (int i=0; i<MN; i++)
            histogram.at(i)/=MN;

    for(int k=0; k<256; k++){

        float P1=0.0,P2=0.0;

        for (int i=1; i<=k; i++)
            P1+=histogram.at(i);

        for (int i=k+1; i<MN; i++)
            P2+=histogram.at(i);


        float med_cum_A=0.0,med_cum_B=0.0;


        for (int i=1; i<=k; i++)
            med_cum_A+=histogram.at(i)*i;

        for (int i=k+1; i<256; i++)
            med_cum_B+=histogram.at(i)*i;



        float sigma = P1*P2*((med_cum_A-med_cum_B)*(med_cum_A-med_cum_B));

        if(sigma > sigma_max){
            sigma_max = sigma;
            output = k;
        }


    }
    cout << output << endl;
    return output;
}


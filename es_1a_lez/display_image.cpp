#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>

using namespace cv;
using namespace std;

Mat padding(Mat image);
Mat blur(Mat image);


int main( int argc, char** argv )
{
    string imageName;

    if( argc > 1)
    imageName = argv[1];

    else{

        puts("Usage -- ./p_name img_name\n");
        exit(-1);
    }



    Mat image;



    image = imread( samples::findFile( imageName ), IMREAD_GRAYSCALE );

    cout << image.type() << endl;

    Mat padded = padding(image);

    Mat blurred = blur(padded);


    if( image.empty() ){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }



    imshow( "ared", image );
    waitKey(0);

    imshow( "ared2", padded );
    waitKey(0);

    imshow( "ared3", blurred );
    waitKey(0);

    return 0;
}

Mat padding(Mat image){ //padding di una immagine aggiungendo 2 righe e 2 colonne // padding per maschere standard 3x3

    Mat padded;
    Scalar def = 0;
    padded.create(image.rows+2,image.cols+2,image.type());
    padded.setTo(def); //settami i pixel della Mat a 0
    image.copyTo(padded(Rect(1,1,image.cols,image.rows))); //copiami la Mat padded nel rettangolo che parte dall'elemento (1;1) e arriva all'elemento (cols-1;rows-1)

    return padded;

}

Mat blur(Mat image){ //sfoca un'immagine che ha già subito un processo di padding per maschere standard 3x3

    Mat blurred;
    Scalar def = 0;
    blurred.create(image.rows,image.cols,image.type());
    blurred.setTo(def);

    //poichè il metodo at è un template, dobbiamo distinguere i vari casi a seconda dell'immagine che gli passiamo

    //se l'immagine è a 8bit monocanale
    if (image.type() == CV_8UC1) {
    for (int i=1; i<image.rows; i++)
        for(int j=1; j<image.cols; j++){//somma i 9 pixel di una zona e metti il valore medio all'interno della nuova Mat
            blurred.at<uchar>(i, j) = (image.at<uchar>(i, j) + image.at<uchar>(i-1, j-1) + image.at<uchar>(i-1, j) + image.at<uchar>(i-1, j+1) + image.at<uchar>(i, j-1) + image.at<uchar>(i, j+1) + image.at<uchar>(i+1, j-1) + image.at<uchar>(i+1, j) + image.at<uchar>(i+1, j+1))/9;

    }

    //se l'immagine è a 8bit rgb
    } else if (image.type() == CV_8UC3) {
    for (int i=1; i<image.rows; i++)
        for(int j=1; j<image.cols; j++){//somma i 9 pixel di una zona e metti il valore medio all'interno della nuova Mat
            blurred.at<Vec3b>(i, j) = (image.at<Vec3b>(i, j) + image.at<Vec3b>(i-1, j-1) + image.at<Vec3b>(i-1, j) + image.at<Vec3b>(i-1, j+1) + image.at<Vec3b>(i, j-1) + image.at<Vec3b>(i, j+1) + image.at<Vec3b>(i+1, j-1) + image.at<Vec3b>(i+1, j) + image.at<Vec3b>(i+1, j+1))/9;

    }

    //se l'immagine è a 16bit monocanale
    } else if (image.type() == CV_16UC1) {
    for (int i=1; i<image.rows; i++)
        for(int j=1; j<image.cols; j++){//somma i 9 pixel di una zona e metti il valore medio all'interno della nuova Mat
            blurred.at<ushort>(i, j) = (image.at<ushort>(i, j) + image.at<ushort>(i-1, j-1) + image.at<ushort>(i-1, j) + image.at<ushort>(i-1, j+1) + image.at<ushort>(i, j-1) + image.at<ushort>(i, j+1) + image.at<ushort>(i+1, j-1) + image.at<ushort>(i+1, j) + image.at<ushort>(i+1, j+1))/9;

    }

    //se l'immagine è a 16bit rgb
    } else if (image.type() == CV_16UC3) {
    for (int i=1; i<image.rows; i++)
        for(int j=1; j<image.cols; j++){//somma i 9 pixel di una zona e metti il valore medio all'interno della nuova Mat
            blurred.at<Vec3w>(i, j) = (image.at<Vec3w>(i, j) + image.at<Vec3w>(i-1, j-1) + image.at<Vec3w>(i-1, j) + image.at<Vec3w>(i-1, j+1) + image.at<Vec3w>(i, j-1) + image.at<Vec3w>(i, j+1) + image.at<Vec3w>(i+1, j-1) + image.at<Vec3w>(i+1, j) + image.at<Vec3w>(i+1, j+1))/9;

    }
    } else {
    cout << "Unknown img type" << endl;
    exit(-1);
    }

    return blurred;

}



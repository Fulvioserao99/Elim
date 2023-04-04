#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>

using namespace cv;
using namespace std;

Mat padding(Mat image, int n);
Mat blur(Mat image);


int main( int argc, char** argv )
{
    string imageName;

    if( argc > 1)
    imageName = argv[1];

    else{

        puts("Usage -- ./p_name img_name \n");
        exit(-1);
    }


    Mat image;

    image = imread( samples::findFile( imageName ), IMREAD_COLOR);

    Mat padded = padding(image,1);

    Mat blurred = blur(image);


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

Mat padding(Mat image, int n){ //padding di una immagine aggiungendo 2*n righe e 2*n colonne

    Mat padded;
    Scalar def = 0;
    padded.create(image.rows+2*n,image.cols+2*n,image.type());
    padded.setTo(def); //settami i pixel della Mat a 0
    image.copyTo(padded(Rect(n,n,image.cols,image.rows))); //copiami la Mat padded nel rettangolo che parte dall'elemento (n;n) e arriva all'elemento (cols-1;rows-1)

    return padded;

}

Mat blur(Mat image){ //effettua un padding standard e sfoca l'immagine

    Mat blurred = padding(image,1);

    if (image.type() == CV_8UC1) //se l'immagine è a 8 bit, e in scala di grigi
    for (int i=1; i<image.rows; i++)
        for(int j=1; j<image.cols; j++){//somma i 9 pixel di una zona e metti il valore medio all'interno della nuova Mat
            blurred.at<uchar>(i, j) = (image.at<uchar>(i, j) + image.at<uchar>(i-1, j-1) + image.at<uchar>(i-1, j) + image.at<uchar>(i-1, j+1) + image.at<uchar>(i, j-1) + image.at<uchar>(i, j+1) + image.at<uchar>(i+1, j-1) + image.at<uchar>(i+1, j) + image.at<uchar>(i+1, j+1))/9;

    }

    else if (image.type() == CV_8UC3){ //se l'immagine è a 8 bit, rgb

    for (int i=1; i<image.rows; i++)
        for(int j=1; j<image.cols; j++){//somma i 9 pixel di una zona e metti il valore medio all'interno della nuova Mat // per ogni pixel
            blurred.at<Vec3b>(i, j)[0] = (image.at<Vec3b>(i, j)[0] + image.at<Vec3b>(i-1, j-1)[0] + image.at<Vec3b>(i-1, j)[0] + image.at<Vec3b>(i-1, j+1)[0] + image.at<Vec3b>(i, j-1)[0] + image.at<Vec3b>(i, j+1)[0] + image.at<Vec3b>(i+1, j-1)[0] + image.at<Vec3b>(i+1, j)[0] + image.at<Vec3b>(i+1, j+1)[0])/9;

            blurred.at<Vec3b>(i, j)[1] = (image.at<Vec3b>(i, j)[1] + image.at<Vec3b>(i-1, j-1)[1] + image.at<Vec3b>(i-1, j)[1] + image.at<Vec3b>(i-1, j+1)[1] + image.at<Vec3b>(i, j-1)[1] + image.at<Vec3b>(i, j+1)[1] + image.at<Vec3b>(i+1, j-1)[1] + image.at<Vec3b>(i+1, j)[1] + image.at<Vec3b>(i+1, j+1)[1])/9;

            blurred.at<Vec3b>(i, j)[2] = (image.at<Vec3b>(i, j)[2] + image.at<Vec3b>(i-1, j-1)[2] + image.at<Vec3b>(i-1, j)[2] + image.at<Vec3b>(i-1, j+1)[2] + image.at<Vec3b>(i, j-1)[2] + image.at<Vec3b>(i, j+1)[2] + image.at<Vec3b>(i+1, j-1)[2] + image.at<Vec3b>(i+1, j)[2] + image.at<Vec3b>(i+1, j+1)[2])/9;
        }
    }
    else {
        cout << "Can't blur this img" << endl;
        exit(-1);
    }

    return blurred;

}



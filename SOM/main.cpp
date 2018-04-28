#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <random>
#include <math.h>

using namespace cv;
using namespace std;

const int n = 512;
const int n1 = 86;
const int n2 = 3;
const int n3 = 258;
const int vec_n = 86*86;
const int dim = 3*3;


int main(int argc, char *argv[])
{
    unsigned char im[n3][n3] = {0};
    float codebook[n][dim] = {0};
    int epoch = 200;
    float weight[n][dim];
    float vec[vec_n][dim];
    float norm2[n] = {0};
    float a = 0.025; ///learning rate
    float err = 0.01;
    random_device rd;
    default_random_engine engine(rd());
    uniform_int_distribution<> dis(0, 10000);
    auto dice = std::bind(dis, engine);

    Mat img = imread("LENA.BMP",IMREAD_GRAYSCALE);
    if(img.empty())
       return -1;
    //namedWindow( "lena", CV_WINDOW_AUTOSIZE );
    //imshow("lena", img);

    //Scalar intensity=img.at<uchar>(1,3);

    ///read training image
    for(int i = 0; i < img.rows; ++i)
        for(int j = 0; j < img.cols; ++j)
            im[i][j] = img.at<uchar>(i, j);
    for(int i = 0; i < n1; ++i)
        for(int j = 0; j < n1; ++j)
            for(int k1 = 0; k1 < n2; ++k1)
                for(int k2 = 0; k2 < n2; ++k2)
                    vec[i*n1+j][k1*n2+k2] = im[i*n2+k1][j*n2+k2];
    ///random initialize weight
    for(int i = 0; i < n; ++i)
        for(int j = 0; j < dim; ++j)
        {
            weight[i][j] = dice()/10000.0*255;
            cout << weight[i][j] << endl;
        }
    /*
    for(int i = 0; i < 86*86; ++i)
    {
        for(int j = 0; j < 9; ++j)
            printf("%f ", vec[i][j]);
        printf("\n");
    }
    */

    ///norm of the dk
    int cnt = 0;
    float er;
    do
    {
        er = 0;
        float dk[dim];
        for(int k1 = 0; k1 < vec_n; ++k1)
        {
            for(int k2 = 0; k2 < n; ++k2)
            {
                norm2[k2] = 0;
                for(int i = 0; i < dim; ++i)
                {
                    dk[i] = vec[k1][i] - weight[k2][i];
                    //norm2[k2] += (weight[k2][i] - vec[k1][i])*(weight[k2][i] - vec[k1][i]);
                    norm2[k2] += dk[i] * dk[i];
                }
            }
            ///find the min of dk
            float min_norm2 = norm2[0];
            int k3 = 0;
            for(int i = 1; i < n; ++i)
                if(min_norm2 > norm2[i])
                {
                    min_norm2 = norm2[i];
                    k3 = i;
                }
            ///update weight
            for(int i = 0; i < dim; ++i)
                weight[k3][i] += a*(vec[k3][i] - weight[k3][i]);
            //cout << min_norm2 << endl;
            er += sqrt(min_norm2);
        }
        cout << cnt+1 <<" "<< er << endl;
    }while(++cnt < epoch);

    namedWindow( "lena", CV_WINDOW_AUTOSIZE );
    imshow("lena", img);
    remove("../codebook.bin");
    ofstream file("../codebook.bin", ios::out | ios::binary);
    if(!file)
    {
        cerr << "file open error!" << endl;
        exit(-1);
    }
    for(int i = 0; i < n; ++i)
    {
        for(int j = 0; j < dim; ++j)
        {
            //cout << weight[i][j] << " ";
            file.write((char*)&weight[i][j], sizeof(weight[i][j]));
        }
        //cout << i <<endl;
    }
    file.close();
    /*
    ifstream file1("../codebook.bin", ios::in | ios::binary);
    for(int i = 0; i < n; ++i)
    {
        for(int j = 0; j < dim; ++j)
        {
            file1.read((char*)&codebook[i][j], sizeof(codebook[i][j]));
            cout << codebook[i][j] << " ";
        }
        cout << i << " " <<endl;
    }
    file1.close();
    */
    waitKey(0);
    return 0;
}




#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <cmath>

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
    float codebook[n][dim];
    int codebook_con[n][dim];
    int code[vec_n];
    int im[n3][n3];
    Mat raw = imread("../CR.BMP",IMREAD_GRAYSCALE);
    if(raw.empty())
       return -1;
    Mat img(256, 256, CV_8UC1, Scalar(0));
    ifstream file;
    file.open("../codebook.bin", ios::in | ios::binary);
    if(!file)
    {
        cerr << "file open error!" << endl;
        exit(-1);
    }
    for(int i = 0; i < n; ++i)
        for(int j = 0; j < dim; ++j)
        {
            file.read((char*)&codebook[i][j], sizeof(codebook[i][j]));
            codebook_con[i][j] = int(codebook[i][j]);
            //cout << codebook_con[i][j] << endl;
        }
    file.close();

    ifstream file1;
    file1.open("../coding.bin", ios::in | ios::binary);
    if(!file1)
    {
        cerr << "file open error!" << endl;
        exit(-1);
    }
    for(int i = 0; i < vec_n; ++i)
    {
        file1.read((char*)&code[i], sizeof(code[i]));
        //cout << i <<" " <<code[i] << endl;
    }
    file1.close();
    int cnt = 0;
    for(int i = 0; i < n1; ++i)
        for(int j = 0; j < n1; ++j, ++cnt)
            for(int k1 = 0; k1 < n2; ++k1)
                for(int k2 = 0; k2 < n2; ++k2)
                {
                    im[i*n2+k1][j*n2+k2] = (codebook_con[code[cnt]])[k1*n2+k2];
                    //cout << cnt << " " << im[i*n2+k1][j*n2+k2] << endl;
                }

    for(int i = 0; i < img.rows; ++i)
            for(int j = 0; j < img.cols; ++j)
                img.at<uchar>(i, j) = im[i][j];
    float MSR;
    for(int i = 0; i < img.rows; ++i)
        for(int j = 0; j < img.cols; ++j)
            MSR += (img.at<uchar>(i, j) - raw.at<uchar>(i, j))*(img.at<uchar>(i, j) - raw.at<uchar>(i, j));
    MSR /= (256.0*256.0);
    MSR = 10*log10(255*255/MSR);
    cout << "MSR=" << MSR << endl;
    namedWindow( "decoding_image", CV_WINDOW_AUTOSIZE );
    imshow("decoding_image", img);
    waitKey(0);
    return 0;
}




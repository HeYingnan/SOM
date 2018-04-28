#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>

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
    float im[n3][n3];
    float weight[n][dim];
    float norm2[n] = {0};
    float vec[vec_n][dim];
    float codebook[n][dim];
    int code[vec_n];
    Mat img = imread("../CR.BMP", IMREAD_GRAYSCALE);
    if(img.empty())
       return -1;

    for(int i = 0; i < img.rows; ++i)
        for(int j = 0; j < img.cols; ++j)
            im[i][j] = img.at<uchar>(i, j);
    for(int i = 0; i < n1; ++i)
        for(int j = 0; j < n1; ++j)
            for(int k1 = 0; k1 < n2; ++k1)
                for(int k2 = 0; k2 < n2; ++k2)
                    vec[i*n1+j][k1*n2+k2] = im[i*n2+k1][j*n2+k2];
    ifstream file;
    file.open("../codebook.bin", ios::in | ios::binary);
    if(!file)
    {
        cerr << "file open error!" << endl;
        exit(-1);
    }
    for(int i = 0; i < n; ++i)
        for(int j = 0; j < dim; ++j)
            file.read((char*)&codebook[i][j], sizeof(codebook[i][j]));
    file.close();

    float dk[dim];
    for(int k1 = 0; k1 < vec_n; ++k1)
    {
        for(int k2 = 0; k2 < n; ++k2)
        {
            norm2[k2] = 0;
            for(int i = 0; i < dim; ++i)
            {
                dk[i] = vec[k1][i] - codebook[k2][i];
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
        code[k1] = k3;
        cout << code[k1] << endl;
    }
    remove("../coding.bin");
    ofstream file_out("../coding.bin", ios::out | ios::binary);
    if(!file_out)
    {
        cerr << "file open error!" << endl;
        exit(-1);
    }
    for(int i = 0; i < vec_n; ++i)
    {
        //cout << i << " " << code[i] << endl;
        file_out.write((char*)&code[i], sizeof(int));
    }
    file_out.close();

    namedWindow( "raw_image", CV_WINDOW_AUTOSIZE );
    imshow("raw_image", img);
    waitKey(0);
    return 0;
}




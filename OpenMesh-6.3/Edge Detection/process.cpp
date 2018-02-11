//
//  process.cpp
//  
//
//  Created by Terri on 4/18/17.
//
//


#include "opencv2/core.hpp"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;

/// Global variables


//this is the source imafe
Mat src, src_gray;

//dst is the black image with the white lines
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold = 75;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";
vector<Vec4i> heirarcy;
vector< vector<Point> > contours;

void CannyThreshold(int, void*)
{
    /// Reduce noise with a kernel 3x3
    blur( src_gray, detected_edges, Size(3,3) );
    
    /// Canny detector
    Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
    
    /// Using Canny's output as a mask, we display our result
    dst = Scalar::all(0);
    
    src.copyTo( dst, detected_edges);

}

//g++ $(pkg-config --cflags --libs opencv) process.cpp -o process
//use .tiff files
int main( int argc, char** argv )
{
    /// Load an image
    src = imread( argv[1] );
    
    if( !src.data )
    { return -1; }
    
    /// Create a matrix of the same type and size as src (for dst)
    dst.create( src.size(), src.type() );
    
    /// Convert the image to grayscale
    cvtColor( src, src_gray, CV_BGR2GRAY );
    
    /// Create a window
    namedWindow( window_name, CV_WINDOW_AUTOSIZE );
    
    /// Create a Trackbar for user to enter threshold
    //createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );
    
    /// Show the image
    CannyThreshold(0, 0);
    
    findContours(detected_edges, contours, heirarcy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0,0));
    
    dst = Scalar::all(0);
    
    src.copyTo( dst, detected_edges);
    
    imshow( window_name, dst );
    
    for(int i =0; i< contours.size(); i++){
        for (int j = 0; j < contours[i].size(); j++){
        
            std::cout<<"EDGES "<<contours[i][j]<<" "<<i<<" "<<j<<std::endl;
        }
    }
    //contours is a 2D array, the outer layer is ennumerating the edges,
    //and the inner layer specifies the points in those edges
    
    
    
    /// Wait until user exit program by pressing a key
    waitKey(0);
    
    return 0;
}

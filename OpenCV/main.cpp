//
//  main.cpp
//  OpenCV
//
//  Created by Aneesh Bhansali on 1/31/16.
//

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

vector<Point2f> findCorners(Mat input) {
    // Find the corners
    vector<Point2f> corners;
    int maxCorners = 100;
    double qualityLevel = 0.3;
    double minDistance = 7;
    int blockSize = 7;
    goodFeaturesToTrack(input, corners, maxCorners, qualityLevel, minDistance, noArray(), blockSize);
    
    // Return the corners
    return corners;
}

int main(int argc, const char * argv[]) {
    VideoCapture webcam(0); // Webcam video feed
    
    // If unable to open webcam, terminate
    if (!webcam.isOpened()) {
        cerr << "Unable to open webcam." << endl;
        return -1;
    }
    
    // Motion tracking variables
    vector<Point2f> corners;    // Corners to track
    Mat previousFrame;  // Previous frame for comparison for motion tracking
    Mat grayPreviousFrame;  // Need grayscale for motion analysis
    Mat currentFrame;   // Current frame to compare to previousFrame for motion tracking
    Mat grayCurrentFrame;   // Need grayscale for motion analysis
    
    // Read a frame and find the corners
    Mat initFrame;  // Initial frame from webcam
    webcam.read(initFrame);
    flip(initFrame, currentFrame, 1);
    cvtColor(currentFrame, grayCurrentFrame, COLOR_BGR2GRAY);
    corners = findCorners(grayCurrentFrame);
    
    // Initialize the line mask
    Mat lineMask(currentFrame.rows, currentFrame.cols, CV_8UC3);
    
    // Loop until a keystroke is detected
    while (waitKey(30) < 0) {
        Mat inFrame;    // Used for reading from webcam
        
        // Increment frames
        previousFrame = currentFrame.clone();
        grayPreviousFrame = grayCurrentFrame.clone();
        webcam.read(inFrame);
        flip(inFrame, currentFrame, 1);
        cvtColor(currentFrame, grayCurrentFrame, COLOR_BGR2GRAY);
        
        // Find the new location of the initial corners by calling calcOpticalFlowPyrLK
        vector<Point2f> currentCorners;    // Corners which calcOpticalFlowPyrLK finds
        vector<uchar> status;   // Which features a flow has been found for
        Mat err;    //  Errors of features
        Size winSize(200, 200);   // Search window size at each level
        int maxLevel = 2;   // Maximum pyramid levels to use
        TermCriteria criteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 0.03);   // Criteria for search termination
        calcOpticalFlowPyrLK(previousFrame, currentFrame, corners, currentCorners, status, err, winSize, maxLevel, criteria);
        
        // Draw the difference between the two sets of locations of corners
        for (unsigned int i = 0; i < currentCorners.size(); i++) {
            // If the feature was found, draw the line in red and a circle indicating the current location of said feature
            if (status[i] != 0  ) {
                line(lineMask, corners[i], currentCorners[i], Scalar(0, 0, 255));
                circle(currentFrame, currentCorners[i], 5, Scalar(0, 0, 255));
            }
        }
        
        // Draw the mask onto the frame
        Mat outputFrame;
        add(currentFrame, lineMask, outputFrame);
    
        // Show the current frame
        imshow("Webcam", outputFrame);
    }
    
    // Release the camera
    webcam.release();
    
    // Finished without error
    return 0;
}

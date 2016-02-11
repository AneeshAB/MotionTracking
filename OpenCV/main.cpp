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

int main() {
    // Open webcam
    VideoCapture webcam;    // Webcam video feed
    webcam.open(0);
    if (!webcam.isOpened()) {
        cerr << "Unable to open webcam" << endl;
        return -1;
    }
    
    bool drawLines = false;     // Whether to draw lines tracking each corner
    bool findCorners = false;   // Whether to find new corners
    Mat grayFrames[2];          // Current(1) and previous(0) frames in grayscale
    Mat lines;                  // Where to draw the lines
    vector<Point2f> corners[2]; // Current(1) and previous(0) corners
    
    // Loop until q is pressed
    while ((char)waitKey(10) != 'q') {
        // Shift frames over
        if (!grayFrames[1].empty()) {
            grayFrames[1].copyTo(grayFrames[0]);
        }
        if (!corners[1].empty()) {
            swap(corners[1], corners[0]);
        }
        Mat frame;  // Current frame from webcam
        webcam >> frame;
        cvtColor(frame, grayFrames[1], COLOR_BGR2GRAY);
        
        // If necessary, read another frame
        if (grayFrames[0].empty()) {
            grayFrames[1].copyTo(grayFrames[0]);
            webcam >> frame;
            cvtColor(frame, grayFrames[1], COLOR_BGR2GRAY);
        }
        
        // Erase lines if necessary
        if (!drawLines && !lines.empty()) {
            frame.copyTo(lines);
            lines = Scalar(0, 0, 0, 0);
        }
        
        // Find corners if necessary
        if (findCorners) {
            // Erase all lines
            frame.copyTo(lines);
            lines = Scalar(0, 0, 0, 0);
            
            // Delete old corners
            corners[0].clear();
            corners[1].clear();
            
            // Shi-Tomasi Corner Detection
            goodFeaturesToTrack(grayFrames[1], corners[1], 200, 0.01, 7, Mat());
            
            // No longer need to find corners
            findCorners = false;
        }
        
        // Track motion if corners have been found
        if (!corners[0].empty()) {
            // Lucas-Kanade Pyramidal
            vector<uchar> status;   // Contains whether each corner was found in next frame
            Mat err;                // Contains error values for each corner's flow
            calcOpticalFlowPyrLK(grayFrames[0], grayFrames[1], corners[0], corners[1], status, err);
            
            // Draw the corners and lines
            for (int i = 0; i < corners[1].size(); i++) {
                // Proceed if the corner was found on the new frame
                if (status[i]) {
                    // Draw lines if necessary
                    if (drawLines) {
                        line(lines, corners[0][i], corners[1][i], Scalar(0, 0, 255), 1, CV_AA);
                    }
                    
                    // Draw corners
                    circle(frame, corners[1][i], 3, Scalar(0, 0, 255 ), -1, CV_AA );
                }
            }
        }
        
        // Add the lines to the frame to display if necessary
        if (drawLines && !lines.empty()) {
            Mat in;
            frame.copyTo(in);
            add(in, lines, frame);
        }
        
        // Display the frame
        imshow("Motion tracking", frame);
        
        // Keystroke handling
        char keyStroke = (char)waitKey(10);
        switch (keyStroke) {
                // space finds new corners
                case ' ':
                    findCorners = true;
                    break;
                // l toggles line drawing
                case 'l':
                    drawLines = !drawLines;
                    break;
        }
    }
    
    // Close webcam
    webcam.release();
    
    // Program completed successfully
    return 0;
}
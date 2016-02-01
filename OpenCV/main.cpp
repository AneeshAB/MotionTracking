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

int main(int argc, const char * argv[]) {
    VideoCapture webcam(0); // Webcam video feed
    
    // If unable to open webcam, terminate
    if (!webcam.isOpened()) {
        cerr << "Unable to open webcam." << endl;
        return -1;
    }
    
    // Loop until a keystroke is detected
    while (waitKey(30) < 0) {
        // Read in the current frame
        Mat inFrame;
        webcam.read(inFrame);
        
        // Horizontally flip the frame
        Mat outFrame;
        flip(inFrame, outFrame, 1);
        
        // Show the frame
        imshow("Webcam", outFrame);
    }
    
    // Finished without error
    return 0;
}

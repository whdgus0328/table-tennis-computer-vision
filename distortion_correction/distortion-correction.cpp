// g++ distortion-correction.cpp -o distortion-correction `pkg-config --cflags --libs opencv`

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    // video filename and calibration settings (XML) filename should be given as an argument
    if (argc < 3) {
        cerr << "Please give the video filename and calibration settings filename as arguments" << endl;
        exit(1);
    }

    const string video_filename       = argv[1];
    const string calibration_filename = argv[2];

    // we open the video file
    VideoCapture capture(video_filename);

    if (!capture.isOpened()) {
        cerr << "Error when reading video file" << endl;
        exit(1);
    }

    // we open the calibration settings XML file
    FileStorage fs;
    fs.open(calibration_filename, FileStorage::READ);
    Mat cameraMatrix, distortionCoeffs;
    fs["Camera_Matrix"] >> cameraMatrix;
    fs["Distortion_Coefficients"] >> distortionCoeffs;
    double image_width, image_height;
    fs["image_Width"] >> image_width;
    fs["image_Height"] >> image_height;
    Size image_size(image_width, image_height);

    // we get the optimal new camera matrix
    // needed to prevent the undistorted picture from being cropped
    Mat newCameraMatrix = getOptimalNewCameraMatrix(cameraMatrix, distortionCoeffs, image_size, 0.5);
    // Nota bene: it is possible to change the last parameter (alpha) of the previous function call
    // between 0 and 1 depending on if we want the resulting image to be cropped or not

    // we compute the frame duration
    int FPS = capture.get(CV_CAP_PROP_FPS);
    cout << "FPS: " << FPS << endl;

    int frame_duration = 1000 / FPS;  // frame duration in milliseconds
    cout << "frame duration: " << frame_duration << " ms" << endl;

    // we read and display the video file, image after image
    Mat frame, frame_undistorted;
    namedWindow(video_filename, WINDOW_AUTOSIZE);

    while(true) {
        // we get a new image
        capture >> frame;

        // check for end of the video
        if(frame.empty())
            break;

        // we undistort the frame
        undistort(frame, frame_undistorted, cameraMatrix, distortionCoeffs, newCameraMatrix);

        // we display the image
        imshow(video_filename, frame_undistorted);

        // press 'q' to quit
        char key = waitKey(1); // waits to display frame
        if (key == 'q')
            break;
    }

    return 0;
}
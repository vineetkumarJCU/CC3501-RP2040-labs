#include <opencv2/opencv.hpp>
#include <sys/time.h>

#include <cstdio>
#include <iostream>
#include <string>

int main()
{
    std::string pipeline = "libcamerasrc"
        " ! video/x-raw, width=800, height=600"
        " ! videoconvert"
        " ! videoscale"
        " ! video/x-raw, width=400, height=300"
        " ! videoflip method=rotate-180"
        " ! appsink drop=true max-buffers=2";

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);

    if (!cap.isOpened())
    {
        std::printf("Could not open camera.\n");
        return 1;
    }

    // HSV threshold values
    int lowH = 0;
    int highH = 179;

    int lowS = 0;
    int highS = 255;

    int lowV = 0;
    int highV = 255;

    // Morphological structuring-element sizes
    int openSize = 0;
    int closeSize = 0;

    // Create OpenCV windows
    cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Thresholded", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Controls", cv::WINDOW_AUTOSIZE);

    // HSV threshold sliders
    cv::createTrackbar("Low H", "Controls", &lowH, 179);
    cv::createTrackbar("High H", "Controls", &highH, 179);

    cv::createTrackbar("Low S", "Controls", &lowS, 255);
    cv::createTrackbar("High S", "Controls", &highS, 255);

    cv::createTrackbar("Low V", "Controls", &lowV, 255);
    cv::createTrackbar("High V", "Controls", &highV, 255);

    // Morphological operation sliders
    cv::createTrackbar("Open Size", "Controls", &openSize, 20);
    cv::createTrackbar("Close Size", "Controls", &closeSize, 20);

    cv::Mat frame;
    cv::Mat hsvImage;
    cv::Mat thresholdImage;

    // Frame-rate measurement variables
    int frameId = 0;
    timeval start;
    timeval end;

    gettimeofday(&start, nullptr);

    while (true)
    {
        // Capture one live frame
        if (!cap.read(frame) || frame.empty())
        {
            std::printf("Could not read a frame.\n");
            break;
        }

        // Prevent invalid threshold ranges
        if (lowH > highH)
        {
            lowH = highH;
            cv::setTrackbarPos("Low H", "Controls", lowH);
        }

        if (lowS > highS)
        {
            lowS = highS;
            cv::setTrackbarPos("Low S", "Controls", lowS);
        }

        if (lowV > highV)
        {
            lowV = highV;
            cv::setTrackbarPos("Low V", "Controls", lowV);
        }

        // Convert the camera frame from BGR to HSV
        cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

        // Apply colour thresholding
        cv::inRange(
            hsvImage,
            cv::Scalar(lowH, lowS, lowV),
            cv::Scalar(highH, highS, highV),
            thresholdImage
        );

        // Morphological opening
        if (openSize > 0)
        {
            const int kernelSize = (2 * openSize) + 1;

            cv::Mat openKernel = cv::getStructuringElement(
                cv::MORPH_ELLIPSE,
                cv::Size(kernelSize, kernelSize)
            );

            cv::morphologyEx(
                thresholdImage,
                thresholdImage,
                cv::MORPH_OPEN,
                openKernel
            );
        }

        // Morphological closing fills small black holes and gaps
        if (closeSize > 0)
        {
            const int kernelSize = (2 * closeSize) + 1;

            cv::Mat closeKernel = cv::getStructuringElement(
                cv::MORPH_ELLIPSE,
                cv::Size(kernelSize, kernelSize)
            );

            cv::morphologyEx(
                thresholdImage,
                thresholdImage,
                cv::MORPH_CLOSE,
                closeKernel
            );
        }

        // Calculate centre of mass using image moments
        cv::Moments moments = cv::moments(thresholdImage, true);

        if (moments.m00 > 0.0)
        {
            const int centreX =
                static_cast<int>(moments.m10 / moments.m00);

            const int centreY =
                static_cast<int>(moments.m01 / moments.m00);

            std::cout << "Centre of mass: ("
                      << centreX
                      << ", "
                      << centreY
                      << ")"
                      << std::endl;
        }
        else
        {
            std::cout << "No object detected" << std::endl;
        }

        // Show the live camera image and thresholded result
        cv::imshow("Camera", frame);
        cv::imshow("Thresholded", thresholdImage);

        // Press Q or Escape to stop
        const int key = cv::waitKey(1);

        if (key == 'q' || key == 'Q' || key == 27)
        {
            break;
        }

        // Measure frame rate
        frameId++;

        if (frameId >= 30)
        {
            gettimeofday(&end, nullptr);

            const double difference =
                end.tv_sec - start.tv_sec +
                (end.tv_usec - start.tv_usec) / 1000000.0;

            std::printf(
                "30 frames in %f seconds = %f FPS\n",
                difference,
                30.0 / difference
            );

            frameId = 0;
            gettimeofday(&start, nullptr);
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}
#include <sw/redis++/redis++.h>
#include <iostream>
#include <libserial/SerialPort.h>
#include <libserial/SerialStream.h>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

#include "mng_lib.h"

using namespace sw::redis;

auto redis = Redis("tcp://127.0.0.1:6379");
std::thread thread_A, thread_B;
std::string state_connection_base = "POWER_ON"; 
std::string screen = "POWER_ON";
std::chrono::high_resolution_clock::time_point timer_power_on, timer_new_base_detected, timer_event;
int power_on_waiting_sreen = 3000;
int new_base_waiting_sreen = 2000;
int event_waiting_sreen = 3000;

void overlayImage(Mat* src, Mat* overlay, const Point& location)
{
    for (int y = max(location.y, 0); y < src->rows; ++y)
    {
        int fY = y - location.y;

        if (fY >= overlay->rows)
            break;

        for (int x = max(location.x, 0); x < src->cols; ++x)
        {
            int fX = x - location.x;

            if (fX >= overlay->cols)
                break;

            double opacity = ((double)overlay->data[fY * overlay->step + fX * overlay->channels() + 3]) / 255;

            for (int c = 0; opacity > 0 && c < src->channels(); ++c)
            {
                unsigned char overlayPx = overlay->data[fY * overlay->step + fX * overlay->channels() + c];
                unsigned char srcPx = src->data[y * src->step + x * src->channels() + c];
                src->data[y * src->step + src->channels() * x + c] = srcPx * (1. - opacity) + overlayPx * opacity;
            }
        }
    }
}

void show_function()
{
    cv::Mat main_screen_original(600, 1024, CV_8UC3, cv::Scalar(239, 242, 237));

    //////////////////////////////////////////////////////////////////////////////////
    cv::Mat img1 = cv::imread("../image/power_on.png", -1);
    cv::resize(img1, img1, cv::Size(200,200));
    //////////////////////////////////////////////////////////////////////////////////
    cv::Mat img2 = cv::imread("../image/img2.png", -1);
    cv::resize(img2, img2, cv::Size(200,200));
    //////////////////////////////////////////////////////////////////////////////////
    cv::Mat img3 = cv::imread("../image/img3.png", -1);
    cv::resize(img3, img3, cv::Size(200,200));
    //////////////////////////////////////////////////////////////////////////////////
    cv::Mat img4 = cv::imread("../image/time.png", -1);
    cv::resize(img4, img4, cv::Size(200,200));
    //////////////////////////////////////////////////////////////////////////////////
    cv::Mat img5 = cv::imread("../image/fast-delivery.png", -1);
    cv::resize(img5, img5, cv::Size(200,200));
    //////////////////////////////////////////////////////////////////////////////////
    cv::Mat img6 = cv::imread("../image/pin-code.png", -1);
    cv::resize(img6, img6, cv::Size(200,200));
    //////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////
    cv::Mat imgE1 = cv::imread("../image/lock.png", -1);
    cv::resize(imgE1, imgE1, cv::Size(200,200));
    //////////////////////////////////////////////////////////////////////////////////
    cv::Mat imgE2 = cv::imread("../image/unlock.png", -1);
    cv::resize(imgE2, imgE2, cv::Size(200,200));
    //////////////////////////////////////////////////////////////////////////////////
    cv::Mat imgE3 = cv::imread("../image/enter.png", -1);
    cv::resize(imgE3, imgE3, cv::Size(200,200));
    //////////////////////////////////////////////////////////////////////////////////
    cv::Mat imgE4 = cv::imread("../image/close-doors.png", -1);
    cv::resize(imgE4, imgE4, cv::Size(200,200));
    //////////////////////////////////////////////////////////////////////////////////

    img1.convertTo(img1, CV_8UC3);
    img2.convertTo(img2, CV_8UC3);
    img3.convertTo(img3, CV_8UC3);
    img4.convertTo(img4, CV_8UC3);
    img5.convertTo(img5, CV_8UC3);
    img6.convertTo(img6, CV_8UC3);
    imgE1.convertTo(imgE1, CV_8UC3);
    imgE2.convertTo(imgE2, CV_8UC3);
    imgE3.convertTo(imgE3, CV_8UC3);
    imgE4.convertTo(imgE4, CV_8UC3);

    //////////////////////////////////////////////////////////////////////////////////

    //! CHRONO TIMER VARIABLE
    int frequency       = 15;                              // en Hz.
    double time_of_loop = 1000/frequency;                  // en milliseconde.
    std::chrono::high_resolution_clock::time_point last_loop_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point x              = std::chrono::high_resolution_clock::now();
    auto next = std::chrono::high_resolution_clock::now();
    //! CHRONO TIMER VARIABLE

    std::chrono::duration<double, std::milli> time_span;

    while(true)
    {
        //! CHRONO TIMER VARIABLE
        x                          = std::chrono::high_resolution_clock::now();         
        last_loop_time             = x;
        next                       += std::chrono::milliseconds((int)time_of_loop);
        std::this_thread::sleep_until(next);
        //! CHRONO TIMER VARIABLE

        if(state_connection_base.compare("POWER_ON") == 0)
        {
            auto now  = std::chrono::high_resolution_clock::now();
			time_span = now - timer_power_on;

			if((int)time_span.count() > power_on_waiting_sreen)
            {
                state_connection_base = *(redis.get("State_connection_base"));
                screen = "NO_MODULE_CONNECTED";
            }
        }
        ///////////////////////////////////


        cv::Mat current_visual = main_screen_original.clone();

        if(screen.compare("POWER_ON") == 0)
        {
            overlayImage( &current_visual, &img1, Point((current_visual.cols/2)-(img1.cols/2),300) );

            cv::putText(current_visual, "DELIVERY MODULE IS STARTING.", cv::Point(50,200), cv::FONT_HERSHEY_DUPLEX, 1.7, CV_RGB(0,0,0), 4);
        }
        if(screen.compare("NO_MODULE_CONNECTED") == 0)
        {
            auto fond_tracking_status = cv::Scalar(104,112,249);

            cv::rectangle(current_visual, cv::Point(0, 0), cv::Point(1024, 600), fond_tracking_status, -1, cv::LINE_8);

            cv::putText(current_visual, "NO BASE DETECTED.", cv::Point(200,200), cv::FONT_HERSHEY_DUPLEX, 2.0, CV_RGB(255,255,255), 4);

            overlayImage( &current_visual, &img2, Point((current_visual.cols/2)-(img2.cols/2),300) );
        }
        if(screen.compare("NEW_MODULE_CONNECTED") == 0)
        {
            auto fond_tracking_status = cv::Scalar(70,214,209);

            cv::rectangle(current_visual, cv::Point(0, 0), cv::Point(1024, 600), fond_tracking_status, -1, cv::LINE_8);

            cv::putText(current_visual, "NEW BASE DETECTED.", cv::Point(200,200), cv::FONT_HERSHEY_DUPLEX, 2.0, CV_RGB(255,255,255), 4);

            std::string msg = "BASE ID : " + *(redis.get("State_base_identifiant"));
            cv::putText(current_visual, msg, cv::Point(600,50), cv::FONT_HERSHEY_DUPLEX, 0.7, CV_RGB(255,255,255), 1);

            overlayImage( &current_visual, &img3, Point((current_visual.cols/2)-(img3.cols/2),300) );
        }
        if(screen.compare("WAITING") == 0)
        {
            auto fond_tracking_status = cv::Scalar(229,196,87);

            cv::rectangle(current_visual, cv::Point(0, 0), cv::Point(1024, 600), fond_tracking_status, -1, cv::LINE_8);

            cv::putText(current_visual, "WAITING FOR MISSIONS.", cv::Point(200,200), cv::FONT_HERSHEY_DUPLEX, 2.0, CV_RGB(255,255,255), 4);

            overlayImage( &current_visual, &img4, Point((current_visual.cols/2)-(img3.cols/2),300) );
        }
        if(screen.compare("IN_DELIVERY") == 0)
        {
            auto fond_tracking_status = cv::Scalar(56,39,33);

            cv::rectangle(current_visual, cv::Point(0, 0), cv::Point(1024, 600), fond_tracking_status, -1, cv::LINE_8);

            cv::putText(current_visual, "IN DELIVERY.", cv::Point(200,200), cv::FONT_HERSHEY_DUPLEX, 2.0, CV_RGB(255,255,255), 4);

            overlayImage( &current_visual, &img5, Point((current_visual.cols/2)-(img5.cols/2),300) );
        }
        if(screen.compare("WAITING_FOR_CODE") == 0)
        {
            auto fond_tracking_status = cv::Scalar(70,214,209);

            cv::rectangle(current_visual, cv::Point(0, 0), cv::Point(1024, 600), fond_tracking_status, -1, cv::LINE_8);

            cv::putText(current_visual, "CODE : C8QTL7", cv::Point(200,200), cv::FONT_HERSHEY_DUPLEX, 2.0, CV_RGB(255,255,255), 4);

            overlayImage( &current_visual, &img6, Point((current_visual.cols/2)-(img6.cols/2),300) );
        }

        ///////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////
        if(screen.compare("LOCK") == 0)
        {
            auto fond_tracking_status = cv::Scalar(39,1,209);

            cv::rectangle(current_visual, cv::Point(0, 0), cv::Point(1024, 600), fond_tracking_status, -1, cv::LINE_8);

            cv::putText(current_visual, "MODULE IS LOCK.", cv::Point(200,200), cv::FONT_HERSHEY_DUPLEX, 2.0, CV_RGB(255,255,255), 4);

            overlayImage( &current_visual, &imgE1, Point((current_visual.cols/2)-(imgE1.cols/2),300) );
        }
        if(screen.compare("UNLOCK") == 0)
        {
            auto fond_tracking_status = cv::Scalar(67,209,1);

            cv::rectangle(current_visual, cv::Point(0, 0), cv::Point(1024, 600), fond_tracking_status, -1, cv::LINE_8);

            cv::putText(current_visual, "MODULE IS UNLOCK", cv::Point(200,200), cv::FONT_HERSHEY_DUPLEX, 2.0, CV_RGB(255,255,255), 4);

            overlayImage( &current_visual, &imgE2, Point((current_visual.cols/2)-(imgE2.cols/2),300) );
        }
        if(screen.compare("OPEN") == 0)
        {
            auto fond_tracking_status = cv::Scalar(67,209,1);

            cv::rectangle(current_visual, cv::Point(0, 0), cv::Point(1024, 600), fond_tracking_status, -1, cv::LINE_8);

            cv::putText(current_visual, "LOCKER IS OPEN.", cv::Point(200,200), cv::FONT_HERSHEY_DUPLEX, 2.0, CV_RGB(255,255,255), 4);

            overlayImage( &current_visual, &imgE3, Point((current_visual.cols/2)-(imgE3.cols/2),300) );
        }
        if(screen.compare("CLOSE") == 0)
        {
            auto fond_tracking_status = cv::Scalar(39,1,209);

            cv::rectangle(current_visual, cv::Point(0, 0), cv::Point(1024, 600), fond_tracking_status, -1, cv::LINE_8);

            cv::putText(current_visual, "LOCKER IS CLOSE", cv::Point(200,200), cv::FONT_HERSHEY_DUPLEX, 2.0, CV_RGB(255,255,255), 4);

            overlayImage( &current_visual, &imgE4, Point((current_visual.cols/2)-(imgE4.cols/2),300) );
        }

        ///////////////////////////////////
        cv::namedWindow("Interface module",cv::WINDOW_AUTOSIZE);
        cv::imshow("Interface module", current_visual);

        char d=(char)cv::waitKey(25);
	    if(d==27)
	      break;

    }
}

void listen_function()
{
    bool changement = false;
    std::string lock = "LOCK";
    std::string door = "CLOSE";

    std::chrono::duration<double, std::milli> time_span, time_span2;
    timer_event = std::chrono::high_resolution_clock::now();

    //! CHRONO TIMER VARIABLE
    int frequency       = 15;                              // en Hz.
    double time_of_loop = 1000/frequency;                  // en milliseconde.
    std::chrono::high_resolution_clock::time_point last_loop_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point x              = std::chrono::high_resolution_clock::now();
    auto next = std::chrono::high_resolution_clock::now();
    //! CHRONO TIMER VARIABLE

    while(true)
    {
        //! CHRONO TIMER VARIABLE
        x                          = std::chrono::high_resolution_clock::now();         
        last_loop_time             = x;
        next                       += std::chrono::milliseconds((int)time_of_loop);
        std::this_thread::sleep_until(next);
        //! CHRONO TIMER VARIABLE

        if(screen.compare("POWER_ON") != 0)
        {
            state_connection_base = *(redis.get("State_connection_base"));
            if(state_connection_base.compare("NO_CONNECTION") == 0) { screen = "NO_MODULE_CONNECTED";}
            if(state_connection_base.compare("CONNECTED") == 0) 
            {
                if(screen.compare("NO_MODULE_CONNECTED") == 0) {screen = "NEW_MODULE_CONNECTED"; timer_new_base_detected = std::chrono::high_resolution_clock::now();}

                auto now  = std::chrono::high_resolution_clock::now();
                time_span = now - timer_new_base_detected;
                if((int)time_span.count() > new_base_waiting_sreen)
                {
                    if(lock.compare("LOCK") == 0 && (*(redis.get("State_module"))).compare("UNLOCK") == 0 && !changement)
                    {
                        lock = "UNLOCK";
                        screen = lock;
                        changement = true;
                    }
                    if(lock.compare("UNLOCK") == 0 && (*(redis.get("State_module"))).compare("LOCK") == 0 && !changement)
                    {
                        lock = "LOCK";
                        screen = lock;
                        changement = true;
                    }
                    if(door.compare("CLOSE") == 0 && (*(redis.get("State_door"))).compare("OPEN") == 0 && !changement)
                    {
                        door = "OPEN";
                        screen = door;
                        changement = true;
                    }
                    if(door.compare("OPEN") == 0 && (*(redis.get("State_door"))).compare("CLOSE") == 0 && !changement)
                    {
                        door = "CLOSE";
                        screen = door;
                        changement = true;
                    }
                    
                    if(changement)
                    {
                        timer_event = std::chrono::high_resolution_clock::now();
                        changement = false;
                    }

                    auto now  = std::chrono::high_resolution_clock::now();
                    time_span2 = now - timer_event;
                    
                    if((int)time_span2.count() > event_waiting_sreen)
                    {
                        changement = false;
                        std::string state_robot = *(redis.get("State_robot"));
                        if(state_robot.compare("WAITING") == 0) screen = "WAITING";
                        if(state_robot.compare("IN_DELIVERY") == 0) screen = "IN_DELIVERY";
                        if(state_robot.compare("WAITING_FOR_CODE") == 0) screen = "WAITING_FOR_CODE";
                    }
                }


                
            }
        }
    }
}

int main()
{
    timer_power_on = std::chrono::high_resolution_clock::now();

    thread_A = std::thread(&show_function);
    thread_B = std::thread(&listen_function);

    thread_A.join();
    thread_B.join();

    return 0;
}
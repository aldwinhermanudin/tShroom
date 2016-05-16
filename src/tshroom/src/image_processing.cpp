#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int32.h>

#include "cmt/CMT.h"
#include "cmt/gui.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#ifdef __GNUC__
#include <getopt.h>
#else
#include "getopt/getopt.h"
#endif

using cmt::CMT;
using cv::imread;
using cv::namedWindow;
using cv::Scalar;
using cv::VideoCapture;
using cv::waitKey;
using std::cerr;
using std::istream;
using std::ifstream;
using std::stringstream;
using std::ofstream;
using std::cout;
using std::min_element;
using std::max_element;
using std::endl;
using ::atof;

static string WIN_NAME = "tShroom";

int display					(Mat im, CMT & cmt);
string write_rotated_rect	(RotatedRect rect);
double rect_size			(RotatedRect rect);
void imageReceiveCB			(const sensor_msgs::ImageConstPtr& msg);
void readImage				(Mat &output_image);

Mat				image_received;
bool 			from_file		= false;
VideoCapture 	cap;



int main(int argc, char **argv)
{
	
	ros::init(argc, argv, "image_processing");
	ros::NodeHandle nh("~");
		
	image_transport::ImageTransport it(nh);
	image_transport::Subscriber sub_video;
	
	ros::Publisher pub_detection_size = nh.advertise<std_msgs::Float64>("/tshroom/cv/size", 10);
	ros::Publisher pub_active_point = nh.advertise<std_msgs::Int32>("/tshroom/cv/active_point", 10);
	
				
    //Create a CMT object
    CMT cmt;

    //Initialization bounding box
    Rect rect;

    //Parse args
    int bbox_flag = 0;
    string input_path;
    int display_video = 1;
    
    nh.getParam("input_path", input_path);
    nh.getParam("display_video", display_video);
        
    //Set up logging
    FILELog::ReportingLevel() = logINFO;
    
    //Create window
    namedWindow(WIN_NAME,CV_WINDOW_NORMAL);

    bool show_preview	= true;

    //If no input was specified
    if (input_path.length() == 0){
		sub_video 	 = it.subscribe("/tshroom/video", 1, imageReceiveCB);
		
		ROS_INFO_STREAM("Waiting for video feed");
		while (ros::ok() && image_received.empty()){
			ros::spinOnce();
		}
        from_file = false;
    }

    //Else open the video specified by input_path
    else
    {
        cap.open(input_path);
        show_preview = false;
        from_file = true;
        
        //If it doesn't work, stop
		if(!cap.isOpened())
		{
			cerr << "Unable to open video capture." << endl;
			return -1;
		}

    }

    //Show preview until key is pressed
    while (show_preview && ros::ok())
    {
        Mat preview;
        readImage(preview);

        screenLog(preview, "Press a key to start selecting an object.");
        imshow(WIN_NAME, preview);

        char k = waitKey(10);
        if (k != -1) {
            show_preview = false;
        }
    } 
    
    //Get initial image
    Mat im0;
    readImage(im0);

    //If no bounding was specified, get it from user
    if (!bbox_flag)
    {
        rect = getRect(im0, WIN_NAME);
    }

    ROS_INFO_STREAM("Using " << rect.x << "," << rect.y << "," << rect.width << "," << rect.height
        << " as initial bounding box.");

    //Convert im0 to grayscale
    Mat im0_gray;
    if (im0.channels() > 1) {
        cvtColor(im0, im0_gray, CV_BGR2GRAY);
    } else {
        im0_gray = im0;
    }

    //Initialize CMT
    cmt.initialize(im0_gray, rect);

    int frame = 0;

    //Main loop
    while (true && ros::ok())
    {
        frame++;

        Mat im;
        readImage(im);

        if (im.empty()) break; //Exit at end of video stream

        Mat im_gray;
        if (im.channels() > 1) {
            cvtColor(im, im_gray, CV_BGR2GRAY);
        } else {
            im_gray = im;
        }

        //Let CMT process the frame
        cmt.processFrame(im_gray);

        //TODO: Provide meaningful output
        std_msgs::Float64 blob_size;
        blob_size.data =  rect_size(cmt.bb_rot);
        pub_detection_size.publish(blob_size);
        
        std_msgs::Int32 active_point;
        active_point.data =  cmt.points_active.size();
        pub_active_point.publish(active_point);
        
        ROS_INFO_STREAM("#" << frame << " active: " << cmt.points_active.size() << " | " << rect_size(cmt.bb_rot)) ;

        //Display image and then quit if requested.
        if(display_video == 1){
			char key = display(im, cmt);
			if(key == 'q') break;
		}	
		else if (display_video == 0){
			cv::destroyWindow(WIN_NAME);
			display_video = 2; // for flagging
		}
		// for any other condition
		else if ( display_video != 0 && display_video != 1 && display_video != 2){
			char key = display(im, cmt);
			if(key == 'q') break;
		}
    }

    return 0;
}

int display(Mat im, CMT & cmt)
{
    //Visualize the output
    //It is ok to draw on im itself, as CMT only uses the grayscale image
    for(size_t i = 0; i < cmt.points_active.size(); i++)
    {
        circle(im, cmt.points_active[i], 2, Scalar(255,0,0));
    }

    Point2f vertices[4];
    cmt.bb_rot.points(vertices);
    for (int i = 0; i < 4; i++)
    {
        line(im, vertices[i], vertices[(i+1)%4], Scalar(255,0,0));
    }

    imshow(WIN_NAME, im);

    return waitKey(5);
}

string write_rotated_rect(RotatedRect rect)
{
    Point2f verts[4];
    rect.points(verts);
    stringstream coords;

    coords << rect.center.x << "," << rect.center.y << ",";
    coords << rect.size.width << "," << rect.size.height << ",";
    coords << rect.angle << ",";

    for (int i = 0; i < 4; i++)
    {
        coords << verts[i].x << "," << verts[i].y;
        if (i != 3) coords << ",";
    }

    return coords.str();
}

double rect_size(RotatedRect rect){
   
    return (rect.size.width)*(rect.size.height);
}

void imageReceiveCB(const sensor_msgs::ImageConstPtr& msg){
	
	try {
		image_received  = cv_bridge::toCvCopy(msg, "bgr8")->image;		// use cv_bridge::toCvCopy instead of cv_bridge::toCvShare to process a mat data
	}
	
	catch (cv_bridge::Exception& e){
		ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
	}
}

void readImage(Mat &output_image){
	
	if(!from_file){
		ros::spinOnce();
		if(!image_received.empty()){
			output_image = image_received.clone();
		}
	}
	
	else {
		cap >> output_image;
	}
	
}

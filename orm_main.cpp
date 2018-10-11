/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 * 2018 Gal Zaidenstein.
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <time.h>
#include <sys/time.h>

//OpenCV Headers
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>

#define OPENCV  //Enable OpenCV for yolo
#include <yolo_v2_class.hpp> //yolo cpp wrapper

using namespace std;
using namespace cv;

struct timeval t1, t2;
int fps = 0;
unsigned int cone_num = 0;

VideoWriter video_out;

//draw aמ isosceles triangle on a using  x,y,w,h values
void triangle(Mat mat_img,int x, int y, int w, int h , Scalar color){

	line(mat_img,Point((x+w/2),(y)),Point((x),(y+h)),color,1,8,0);
	line(mat_img,Point((x+w/2),(y)),Point((x+w),(y+h)),color,1,8,0);
	line(mat_img,Point((x),(y+h)),Point((x+w),(y+h)),color,1,8,0);
}

void draw_cones(Mat mat_img, vector<bbox_t> result_vec, vector<string> obj_names, unsigned int wait_msec = 0) {

	Scalar color(60, 160, 260);
	Scalar fpscolor(215, 20, 107);

	for (auto &i : result_vec) {

		//basically counts cones
		if(i.track_id>cone_num){
			cone_num= i.track_id;
		}

		triangle(mat_img,i.x,i.y,i.w,i.h,color);
		//		rectangle(mat_img, Rect(i.x, i.y, i.w, i.h), color, 3);
		//		if(obj_names.size() > i.obj_id)
		//			putText(mat_img, obj_names[i.obj_id], Point2f(i.x, i.y - 10), FONT_HERSHEY_COMPLEX_SMALL, 1, color);

		//tracking id
		if(i.track_id > 0)
			putText(mat_img, to_string(i.track_id), Point2f(i.x+i.w+5, i.y + i.h/2), FONT_HERSHEY_COMPLEX_SMALL, 0.7, color);

		//only print up to 2 digits for probability
		stringstream stream;
		stream << fixed << setprecision(2) << i.prob;
		string probString = stream.str();
		putText(mat_img, probString, Point2f(i.x+5, (i.y + 15)+i.h), FONT_HERSHEY_COMPLEX_SMALL, 0.7, color);




	}
	putText(mat_img, "Cones: "+to_string(cone_num), Point2f(mat_img.cols-220, 22), FONT_HERSHEY_PLAIN, 2, fpscolor,2);
	putText(mat_img, "FPS: "+to_string(fps), Point2f(0,22), FONT_HERSHEY_PLAIN , 2, fpscolor,2);
	imshow("window name", mat_img);
	waitKey(wait_msec);
}

//Print the results
void show_result(vector<bbox_t> const result_vec, vector<string> const obj_names) {
	for (auto &i : result_vec) {
		if (obj_names.size() > i.obj_id) cout << obj_names[i.obj_id] << " - ";
		cout <<"FPS: "<<fps<< ", obj_id = " << i.obj_id<< ", tracking id = "<< i.track_id<< ",  x = " << i.x << ", y = " << i.y
				<< ", w = " << i.w << ", h = " << i.h
				<< setprecision(3) << ", prob = " << i.prob << endl;
	}
}

//parse object names
vector<string> objects_names_from_file(string const filename) {
	ifstream file(filename);
	vector<string> file_lines;
	if (!file.is_open()) return file_lines;
	for(string line; file >> line;) file_lines.push_back(line);
	cout << "object names loaded \n";
	return file_lines;
}

int main(void){

	//Paths
	string cfg_path = "yolov3-tiny-obj.cfg";
	string weights_path = "yolov3-tiny-obj_3100.weights";
	string names_path = "cones.names";
	string file_path = "Test3.mp4";

	//Confidence threshold
	float confThreshold = 0.4;

	//Setup output video
	string output_file_path = file_path;
	output_file_path.replace(output_file_path.end()-4, output_file_path.end(), "-yolo.avi");

	//Initialize the detector
	Detector detector(cfg_path,weights_path);
	auto obj_names = objects_names_from_file(names_path);
	int wait_ms = 1; //How long to wait before previewing each frame in [ms]

	//Create a capture session
	Mat frame;
	Mat resized_frame;
	detector.nms = 0.02;

	//Capture each frame until we finish the video
	VideoCapture cap(file_path);

	//Open the output video
	video_out.open(output_file_path, VideoWriter::fourcc('M','J','P','G'), 28, Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)));

	//Start processing the frames
	while(cap.isOpened()) {
		cap >> frame;
		if (frame.empty()){
			cout << "Video completed !" << endl;
			break;
		}

		//start timer
		gettimeofday(&t1, NULL);

		//detect objects in frames with threshold over confThreshold
		vector<bbox_t> result_vec = detector.detect(frame, confThreshold);
		result_vec = detector.tracking_id(result_vec);	// comment it - if track_id is not required

		//stop timer
		gettimeofday(&t2, NULL);

		//calculate fps
		fps =1/((t2.tv_usec - t1.tv_usec)/pow(10,6)) ;

		//draw the cones
		draw_cones(frame, result_vec, obj_names, wait_ms);
		// print results
		show_result(result_vec, obj_names);

		//write out to file
		video_out.write(frame);

	}
	video_out.release();


	return 0;

}

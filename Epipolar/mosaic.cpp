#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <stdio.h>
#include <vector>
#include <cmath>

#define PNUM 8

//parameters of mouse inputs
struct mouseParam {
	int x;
	int y;
	int event;
	int flags;
};


//callback function
void CallBackFunc(int eventType, int x, int y, int flags, void* userdata)
{
	mouseParam *ptr = static_cast<mouseParam*> (userdata);
	ptr->x = x;
	ptr->y = y;
	ptr->event = eventType;
	ptr->flags = flags;
}

//クリック座標を取得
std::vector<cv::Point> click_event(cv::Mat img, cv::String showing_name) {
	mouseParam mouseEvent;
	//表示するウィンドウ名
	//cv::String showing_name = "input";
	//画像の表示
	cv::namedWindow(showing_name, cv::WINDOW_AUTOSIZE);
	cv::imshow(showing_name, img);
	cv::waitKey(3);
	//コールバックの設定
	cv::setMouseCallback(showing_name, CallBackFunc, &mouseEvent);
	//クリック点の取得
	std::vector<cv::Point> click_vec;
	int count = 0;

	while (count < PNUM) {
		//左クリックがあったら表示
		cv::waitKey(30);
		if (mouseEvent.event == cv::EVENT_LBUTTONUP) {
			//クリック後のマウスの座標を出力
			cv::Point p;
			p.x = mouseEvent.x;
			p.y = mouseEvent.y;
			click_vec.push_back(p);
			std::cout << p.x << " , " << p.y << std::endl;
			cv::circle(img, p, 2, cv::Scalar(255, 255, 0), 3);
			count++;
		}
	}
	cv::imwrite("../../Output/" + showing_name + ".jpg", img);
	return click_vec;
}

cv::Mat make_b(std::vector<cv::Point>& r_cp, std::vector<cv::Point>& l_cp, cv::Mat b_mat) {
	for (int i = 0; i < PNUM; i++) {
		b_mat.at<float>(i * 2, 0) = l_cp.at(i).x;//xi'
		b_mat.at<float>(i * 2 + 1, 0) = l_cp.at(i).y;//y1'
	}
	return b_mat;

}


cv::Mat make_arr(std::vector<cv::Point>& r_cp, std::vector<cv::Point>& l_cp, cv::Mat p_matrix) {
	//    std::vector<std::vector<int>> p_matrix(PNUM * 2, std::vector<int>(8));

	for (int i = 0; i < PNUM; i++) {
		p_matrix.at<float>(i * 2, 0) = r_cp.at(i).x;//x1
		p_matrix.at<float>(i * 2, 1) = r_cp.at(i).y;//y1
		p_matrix.at<float>(i * 2, 2) = 1;
		p_matrix.at<float>(i * 2, 3) = 0;
		p_matrix.at<float>(i * 2, 4) = 0;
		p_matrix.at<float>(i * 2, 5) = 0;
		p_matrix.at<float>(i * 2, 6) = -r_cp.at(i).x *l_cp.at(i).x;//-x1 x1'
		p_matrix.at<float>(i * 2, 7) = -l_cp.at(i).x *r_cp.at(i).y;//-x1' y1

		p_matrix.at<float>(i * 2 + 1, 0) = 0;
		p_matrix.at<float>(i * 2 + 1, 1) = 0;
		p_matrix.at<float>(i * 2 + 1, 2) = 0;
		p_matrix.at<float>(i * 2 + 1, 3) = r_cp.at(i).x;//x1;
		p_matrix.at<float>(i * 2 + 1, 4) = r_cp.at(i).y;//y1;
		p_matrix.at<float>(i * 2 + 1, 5) = 1;
		p_matrix.at<float>(i * 2 + 1, 6) = -r_cp.at(i).x *l_cp.at(i).y;//-x1 y1'
		p_matrix.at<float>(i * 2 + 1, 7) = -r_cp.at(i).y *l_cp.at(i).y;//-y1 y1'

	}
	return p_matrix;
}



int main() {
	cv::Mat left_img_x, right_img_x, all_img_x, left_img, right_img, all_img;
	cv::imread("../../Input/mosaic_l.jpg").copyTo(left_img);
	cv::imread("../../Input/mosaic_r.jpg").copyTo(right_img);

	//    cv::cvtColor(left_img_x, left_img, cv::COLOR_BGR2RGB);
	//    cv::cvtColor(right_img_x, right_img, cv::COLOR_BGR2RGB);
	//    cv::cvtColor(all_img_x, all_img, cv::COLOR_BGR2RGB);
	std::vector<cv::Point> right_clicked, left_clicked;
	right_clicked = click_event(right_img, "right_image");
	std::cout << "right finished" << std::endl;
	left_clicked = click_event(left_img, "left_image");
	std::cout << "left finished" << std::endl;

	//    //to check the coordinates
	//    for(int i = 0; i < PNUM; i++){
	//        std::cout << i << "right: " << right_clicked.at(i).x << " , " << right_clicked.at(i).y << std::endl;
	//        std::cout << i << "left: " << left_clicked.at(i).x << " , " << left_clicked.at(i).y << std::endl;
	//    }

		//click points
	cv::Mat b_mat(PNUM * 2, 1, CV_32FC1);
	//p_matrix
	cv::Mat p_matrix(PNUM * 2, 8, CV_32FC1);
	//unknown val
	cv::Mat h_mat(8, 1, CV_32FC1);

	//create b according to clicked points
	b_mat = make_b(right_clicked, left_clicked, b_mat);
	//create p_matrix according to clicked points
	p_matrix = make_arr(right_clicked, left_clicked, p_matrix);
	// calculate least squared solution
	h_mat = (p_matrix.t() * p_matrix).inv() * p_matrix.t() * b_mat;

	//to check the matrix
	std::cout << b_mat << std::endl;
	std::cout << p_matrix << std::endl;
	std::cout << h_mat << std::endl;

	//change to 3*3 mat from 8*1(9*1)
	cv::Mat h_mat_2 = (cv::Mat_<float>(3, 3, CV_32FC1) << h_mat.at<float>(0, 0), h_mat.at<float>(1, 0), h_mat.at<float>(2, 0),
		h_mat.at<float>(3, 0), h_mat.at<float>(4, 0), h_mat.at<float>(5, 0), h_mat.at<float>(6, 0), h_mat.at<float>(7, 0), 1);
	std::cout << h_mat_2 << std::endl;

	//projective transformation
	/*
	 * void warpPerspective(const Mat& src, Mat& dst, const Mat& M, Size dsize, int flags=INTER_LINEAR,
	 * int borderMode=BORDER_CONSTANT, const Scalar& borderValue=Scalar())¶
	 */

	cv::Mat affine_dst = cv::Mat::zeros(right_img.size() * 2, CV_8UC3);
	std::cout << "after affine transformation" << std::endl;
	//transform using mat_2
	cv::warpPerspective(right_img, affine_dst, h_mat_2, affine_dst.size());
	//show image after affine transformation
	cv::String win_name = "after affine";
	cv::imwrite("../../Output/affine.jpg", affine_dst);
	//    //show
	//    cv::namedWindow(win_name, cv::WINDOW_AUTOSIZE);
	//    cv::imshow(win_name, affine_dst);
	//    cv::waitKey(3);

		// combine images
	cv::Mat combined_img = cv::Mat::zeros(right_img.size() * 2, CV_8UC3);
	std::cout << left_img.size() << std::endl;
	std::cout << right_img.size() << std::endl;
	std::cout << affine_dst.size() << std::endl;
	std::cout << affine_dst.rows << std::endl;
	std::cout << combined_img.size() << std::endl;

	for (int i = 0; i < left_img.rows; ++i) {//width
		for (int j = 0; j < left_img.cols; ++j) {//height
			combined_img.at<cv::Vec3b>(i, j) = left_img.at<cv::Vec3b>(i, j);
		}
	}
	//show image after affine transformation
	cv::imwrite("../../Output/left_fin.jpg", combined_img);
	cv::imwrite("../../Output/left_img.jpg", left_img);

	for (int i = 0; i < combined_img.rows; ++i) {
		for (int j = 0; j < combined_img.cols; ++j) {
			for (int k = 0; k < combined_img.channels(); ++k) {
				if (combined_img.at<cv::Vec3b>(i, j)[k] > 0 && affine_dst.at<cv::Vec3b>(i, j)[k] > 0) {
					//combined_img.at<cv::Vec3b>(i, j)[k] = (combined_img.at<cv::Vec3b>(i, j)[k] + affine_dst.at<cv::Vec3b>(i, j)[k]) * 0.5;
					combined_img.at<cv::Vec3b>(i, j)[k] = affine_dst.at<cv::Vec3b>(i, j)[k];
				}
				else if (affine_dst.at<cv::Vec3b>(i, j)[k] > 0) {
					combined_img.at<cv::Vec3b>(i, j)[k] = affine_dst.at<cv::Vec3b>(i, j)[k];
				}
			}
		}
	}
	std::cout << "finished copying right" << std::endl;

	//show final image
	cv::String final_win_name = "combined image";
	cv::imwrite("../../Output/combined.jpg", combined_img);
	//show
	cv::namedWindow(final_win_name, cv::WINDOW_AUTOSIZE);
	cv::imshow(final_win_name, combined_img);
	cv::waitKey(0);


	return 0;
}

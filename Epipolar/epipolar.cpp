
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <stdio.h>
#include <exception>

#define PNUM 8

class point_lack_exception : public std::exception
{
	virtual const char* what() const throw()
	{
		return "two points are needed";
	}
} p_lack_ex;

//parameters of mouse inputs
struct mouseParam {
	int x;
	int y;
	int event;
	int flags;
};

struct epi_point {
	cv::Point cp;
	float a;
	float b;
	float c;
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

//�N���b�N���W���擾
std::vector<cv::Point> click_event(cv::Mat img, cv::String showing_name) {
	mouseParam mouseEvent;
	//�\������E�B���h�E��
	//cv::String showing_name = "input";
	//�摜�̕\��
	cv::namedWindow(showing_name, cv::WINDOW_AUTOSIZE);
	cv::imshow(showing_name, img);
	cv::waitKey(3);
	//�R�[���o�b�N�̐ݒ�
	cv::setMouseCallback(showing_name, CallBackFunc, &mouseEvent);
	//�N���b�N�_�̎擾
	std::vector<cv::Point> click_vec;
	int count = 0;

	while (count < PNUM) {
		//���N���b�N����������\��
		cv::waitKey(30);
		if (mouseEvent.event == cv::EVENT_LBUTTONUP) {
			//�N���b�N��̃}�E�X�̍��W���o��
			cv::Point p;
			p.x = mouseEvent.x;
			p.y = mouseEvent.y;
			click_vec.push_back(p);
			std::cout << count << ": " << p.x << " , " << p.y << std::endl;
			cv::circle(img, p, 1.5, cv::Scalar(255, 255, 0), 10);
			count++;
		}
	}
	cv::imwrite("../../Output/" + showing_name + ".jpg", img);
	return click_vec;
}


//create mat to calculate Essential Matrix or Fundamental Matrix
cv::Mat create_mat_func(std::vector<cv::Point>& pa, std::vector<cv::Point>& pb, cv::Mat mat) {
	for (int i = 0; i < PNUM; i++) {
		mat.at<float>(i, 0) = pa.at(i).x * pb.at(i).x;  //x * x'
		mat.at<float>(i, 1) = pa.at(i).x * pa.at(i).y;  //x * y
		mat.at<float>(i, 2) = pa.at(i).x;               //x
		mat.at<float>(i, 3) = pa.at(i).y * pb.at(i).x;  //y * x'
		mat.at<float>(i, 4) = pa.at(i).y * pb.at(i).y;  //y * y'
		mat.at<float>(i, 5) = pa.at(i).y;               //y
		mat.at<float>(i, 6) = pb.at(i).x;               //x'
		mat.at<float>(i, 7) = pb.at(i).y;               //y'
	}

	return mat;
}


/*�[���t�s��ɂ����Essential Matrix or Fundamental Matrix�̗v�f�����߂�
 * A����8�̍쐬�����s��
 * EorF�����߂�8*1�s��
 * b���e�v�f��1��8*1�s��*/
cv::Mat pseudo_inverse_mat(cv::Mat A, cv::Mat B, cv::Mat EorF) {
	std::cout << "in inverse_mat" << std::endl;
	EorF = (A.t() * A).inv() * A.t() * B;
	return EorF;
}

void draw_line(cv::Mat img, std::vector<epi_point> &ep, std::string s) {
	try {
		int h = img.rows;
		int w = img.cols;

		int count = 0;

		std::vector<epi_point>::iterator ptr;

		for (ptr = ep.begin(); ptr < ep.end(); ++ptr) {
			std::cout << count++ << std::endl;
			std::vector<cv::Point> Point_vec;
			cv::Point p;
			if ((*ptr).b == 0) {//y���ɕ��s(a��0�̎��͍l����̂�߂�)
				//y=0(x��)�Ƃ̌�_ y���Ƃ̌�_�Ȃ�
				p.x = -(*ptr).c / (*ptr).a; //-c/a
				p.y = 0;
				Point_vec.push_back(p);
				//y=h�Ƃ̌�_
				p.x = -(*ptr).c / (*ptr).a;
				p.y = h;
				Point_vec.push_back(p);
				std::cout << "b parallel" << p << std::endl;
			}
			else if ((*ptr).a == 0) {//x���ɕ��s
				//x=0�Ƃ̌�_
				p.x = 0;
				p.y = -(*ptr).c / (*ptr).b;
				Point_vec.push_back(p);
				//x=w�Ƃ̌�_
				p.x = w;
				p.y = -(*ptr).c / (*ptr).b;
				Point_vec.push_back(p);
				std::cout << "a parallel" << p << std::endl;
			}
			else {//���ɂ͕��s�łȂ�
				float tmp;
				//y=0�Ƃ̌�_
				tmp = -(*ptr).c / (*ptr).a;
				std::cout << "1 " << tmp << std::endl;
				if (tmp > 0 && tmp <= w) {
					p.x = tmp;
					p.y = 0;
					Point_vec.push_back(p);
					std::cout << "not parallel 1" << p << std::endl;
				}
				//y=h�Ƃ̌�_
				tmp = -((*ptr).b  * h + (*ptr).c) / (*ptr).a;
				std::cout << "2 " << tmp << std::endl;
				if (tmp > 0 && tmp <= w) {
					p.x = tmp;
					p.y = h;
					Point_vec.push_back(p);
					std::cout << "not parallel 2" << p << std::endl;
				}
				//x=0�Ƃ̌�_
				tmp = -(*ptr).c / (*ptr).b;
				std::cout << "3 " << tmp << std::endl;
				if (tmp >= 0 && tmp <= h) {
					p.x = 0;
					p.y = tmp;
					Point_vec.push_back(p);
					std::cout << "not parallel 3" << p << std::endl;
				}
				//x=w�Ƃ̌�_
				tmp = -((*ptr).a  * w + (*ptr).c) / (*ptr).b;
				std::cout << "4 " << tmp << std::endl;
				if (tmp > 0 && tmp < h) {
					p.x = w;
					p.y = tmp;
					Point_vec.push_back(p);
					std::cout << "not parallel 4" << p << std::endl;
				}
			}

			if (Point_vec.size() < 2) {
				std::cout << "����Ȃ�" << std::endl;
				throw p_lack_ex;//�G���[
			}
			if (Point_vec.size() > 2) {//�_����_�Ȃ������璸�_�ɂ��Ă���\������
				std::cout << "����" << std::endl;
				throw p_lack_ex;//�G���[
			}

			cv::line(img, Point_vec.at(0), Point_vec.at(1), cv::Scalar(0, 50, 255), 3);
		}

		cv::imwrite("../../Output/" + s + ".jpg", img);
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}

}

void calc_coefficient(cv::Mat F, std::vector<cv::Point>& p, std::vector<epi_point> &ep) {
	//p vector�ɑΉ�����W��abc�����߂�
	std::vector<cv::Point>::iterator ptr;

	for (ptr = p.begin(); ptr < p.end(); ptr++) {
		epi_point e;
		e.cp = *ptr;
		e.a = F.at<float>(0, 0) * (*ptr).x + F.at<float>(0, 1) * (*ptr).y + F.at<float>(0, 2);
		e.b = F.at<float>(1, 0) * (*ptr).x + F.at<float>(1, 1) * (*ptr).y + F.at<float>(1, 2);
		e.c = F.at<float>(2, 0) * (*ptr).x + F.at<float>(2, 1) * (*ptr).y + F.at<float>(2, 2);
		ep.push_back(e);
	}
}

int main() {
	cv::Mat img1, img2;
	cv::imread("../../Input/epipolar_l.jpg").copyTo(img1);
	cv::imread("../../Input/epipolar_r.jpg").copyTo(img2);

	//click points(PNUM)
	std::vector<cv::Point> clicked1, clicked2;
	clicked1 = click_event(img1, "image1");
	std::cout << "1 finished" << std::endl;
	clicked2 = click_event(img2, "image2");
	std::cout << "2 finished" << std::endl;

	/*Estimating Essential Fundamental matrix F*/
	cv::Mat matB = cv::Mat::ones(PNUM, 1, CV_32FC1) * -1;
	cv::Mat matF = cv::Mat::zeros(8, 1, CV_32FC1);
	cv::Mat matA = cv::Mat::zeros(PNUM, 8, CV_32FC1);

	matA = create_mat_func(clicked1, clicked2, matA);

	//calculate F
	matF = pseudo_inverse_mat(matA, matB, matF);
	cv::Mat reshaped_F = (cv::Mat_<float>(3, 3, CV_32FC1) << matF.at<float>(0, 0),
		matF.at<float>(1, 0), matF.at<float>(2, 0), matF.at<float>(3, 0),
		matF.at<float>(4, 0), matF.at<float>(5, 0), matF.at<float>(6, 0),
		matF.at<float>(7, 0), 1);


	std::cout << " matA " << matA << std::endl;
	std::cout << " matB " << matB << std::endl;
	std::cout << " matF " << matF << std::endl;
	std::cout << " reshaped_F " << reshaped_F << std::endl;
	//epipolar line  ax + by + c = 0 �̌W�������߂�
	std::vector<epi_point> ep1, ep2;
	calc_coefficient(reshaped_F, clicked2, ep1);
	calc_coefficient(reshaped_F.t(), clicked1, ep2);

	//draw lines on pictures and save the picture
	draw_line(img1, ep1, "img1_epi");
	draw_line(img2, ep2, "img2_epi");

	return 0;

}
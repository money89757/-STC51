#pragma once

#include "common.h"
#include "math_functions.hpp"

class Blob
{
public:
	Blob();
	~Blob();
	void reshape(int num, int channel, int height, int width);
	void release();

	int shape(int index);
	void set_data(float* data);
	float* cpu_data();
	float data_at(int n, int c, int h, int w);
	int offset(int n, int c = 0, int h = 0, int w = 0);
	int count(int start_axis, int end_axis);
	int num_;
	int channel_;
	int height_;
	int width_;
	int counts_;
private:
	
	float* data_;
};

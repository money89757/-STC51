#pragma once

#include "blob.h"
#include "cnnParam.h"

class InnerProductLayer
{
public:
	InnerProductLayer();
	~InnerProductLayer();


	InnerProductParam  inner_product_param_;

	void layer_set_up(vector<Blob>& bottom_data_, vector<Blob>& top_data_);

	void forward_cpu(vector<Blob>& bottom_data_, vector<Blob>& top_data_);

	Blob weights_;
	Blob bias_;
	Blob bias_multiplier_;
private:
	int M_;
	int K_;
	int N_;
};


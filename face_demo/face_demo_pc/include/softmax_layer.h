#pragma once

#include "blob.h"
#include "cnnParam.h"

class SoftmaxLayer
{
public:
	SoftmaxLayer();
	~SoftmaxLayer();

	

	void layer_set_up(vector<Blob>& bottom_data_, vector<Blob>& top_data_);

	void forward_cpu(vector<Blob>& bottom_data_, vector<Blob>& top_data_);

private:

	int outer_num_;
	int inner_num_;
	int softmax_axis_;
	Blob sum_multiplier_;
	Blob scale_;
};


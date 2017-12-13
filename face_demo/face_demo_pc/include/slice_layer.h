#pragma once

#include "blob.h"
#include "cnnParam.h"

class SliceLayer
{
public:
	SliceLayer();
	~SliceLayer();


	SliceParam  slice_param_;

	void layer_set_up(vector<Blob>& bottom_data_, vector<Blob>& top_data_);

	void forward_cpu(vector<Blob>& bottom_data_, vector<Blob>& top_data_);

private:
	int slice_axis_;
	int num_slices_;
	int slice_size_;
};


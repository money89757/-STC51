#pragma once

#include "blob.h"
#include "cnnParam.h"

//注意：只实现了max操作

class EltwiseLayerss
{
public:
	EltwiseLayerss();
	~EltwiseLayerss();

	void layer_set_up(vector<Blob>& bottom_data_, vector<Blob>& top_data_);

	void forward_cpu(vector<Blob>& bottom_data_, vector<Blob>& top_data_);

private:
	int* max_idx_;
};


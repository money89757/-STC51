#pragma once

#include "blob.h"
#include "cnnParam.h"

//ע�⣺ֻʵ����max����

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


#pragma once

#include "blob.h"
#include "cnnParam.h"

//layer_set_up的作用是预先开辟各层输入输出内存

class InputLayer
{
public:
	InputLayer();
	~InputLayer();

	InputParam input_params_;

	void layer_set_up(vector<Blob>& top_data);

	void forward_cpu(Mat inputImg, vector<Blob>& top_data);
};
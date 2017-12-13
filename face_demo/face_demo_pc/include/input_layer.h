#pragma once

#include "blob.h"
#include "cnnParam.h"

//layer_set_up��������Ԥ�ȿ��ٸ�����������ڴ�

class InputLayer
{
public:
	InputLayer();
	~InputLayer();

	InputParam input_params_;

	void layer_set_up(vector<Blob>& top_data);

	void forward_cpu(Mat inputImg, vector<Blob>& top_data);
};
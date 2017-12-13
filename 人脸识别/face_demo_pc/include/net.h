#pragma once
#include "input_layer.h"
#include "conv_layer.hpp"
#include "pooling_layer.h"
#include "innerproduct_layer.h"
#include "softmax_layer.h"
#include "slice_layer.h"
#include "eltwise_layer.h"

struct CnnNet
{
	vector<Blob> input_datas_;
	vector<Blob> conv_1_datas_;
	vector<Blob> slice_1_datas_;
	vector<Blob> eltwise_1_datas_;
	vector<Blob> pool_1_datas_;

	vector<Blob> conv_2_a_datas_;
	vector<Blob> slice_2_a_datas_;
	vector<Blob> eltwise_2_a_datas_;
	vector<Blob> conv_2_datas_;
	vector<Blob> slice_2_datas_;
	vector<Blob> eltwise_2_datas_;
	vector<Blob> pool_2_datas_;

	vector<Blob> conv_3_a_datas_;
	vector<Blob> slice_3_a_datas_;
	vector<Blob> eltwise_3_a_datas_;
	vector<Blob> conv_3_datas_;
	vector<Blob> slice_3_datas_;
	vector<Blob> eltwise_3_datas_;
	vector<Blob> pool_3_datas_;

	vector<Blob> conv_4_a_datas_;
	vector<Blob> slice_4_a_datas_;
	vector<Blob> eltwise_4_a_datas_;
	vector<Blob> conv_4_datas_;
	vector<Blob> slice_4_datas_;
	vector<Blob> eltwise_4_datas_;

	vector<Blob> conv_5_a_datas_;
	vector<Blob> slice_5_a_datas_;
	vector<Blob> eltwise_5_a_datas_;
	vector<Blob> conv_5_datas_;
	vector<Blob> slice_5_datas_;
	vector<Blob> eltwise_5_datas_;
	vector<Blob> pool_4_datas_;

	vector<Blob> fc_1_datas_;
	vector<Blob> slice_6_datas_;
	vector<Blob> eltwise_6_datas_;
	/*vector<Blob> pool_1_datas_;
	vector<Blob> conv_2_datas_;
	vector<Blob> pool_2_datas_;
	vector<Blob> ip_1_datas_;
	vector<Blob> ip_2_datas_;
	vector<Blob> softmax_datas_;*/

	InputLayer input;
	ConvolutionLayer conv_1;
	SliceLayer slice_1;
	EltwiseLayerss eltwise_1;
	PoolingLayer pool_1;

	ConvolutionLayer conv_2_a;
	SliceLayer slice_2_a;
	EltwiseLayerss eltwise_2_a;
	ConvolutionLayer conv_2;
	SliceLayer slice_2;
	EltwiseLayerss eltwise_2;
	PoolingLayer pool_2;

	ConvolutionLayer conv_3_a;
	SliceLayer slice_3_a;
	EltwiseLayerss eltwise_3_a;
	ConvolutionLayer conv_3;
	SliceLayer slice_3;
	EltwiseLayerss eltwise_3;
	PoolingLayer pool_3;

	ConvolutionLayer conv_4_a;
	SliceLayer slice_4_a;
	EltwiseLayerss eltwise_4_a;
	ConvolutionLayer conv_4;
	SliceLayer slice_4;
	EltwiseLayerss eltwise_4;

	ConvolutionLayer conv_5_a;
	SliceLayer slice_5_a;
	EltwiseLayerss eltwise_5_a;
	ConvolutionLayer conv_5;
	SliceLayer slice_5;
	EltwiseLayerss eltwise_5;
	PoolingLayer pool_4;

	InnerProductLayer fc_1;
	SliceLayer slice_6;
	EltwiseLayerss eltwise_6;
	/*PoolingLayer pool_1;
	ConvolutionLayer conv_2;
	PoolingLayer pool_2;
	InnerProductLayer ip_1;
	InnerProductLayer ip_2;
	SoftmaxLayer softmax;*/
};


vector<float> Cnn_Forward(CnnNet& net, Mat inImg);

float computeSimilarScore(CnnNet& net, Mat inImg_first, Mat inImg_second);





//RELU层是原位运算，因此不写层
void relu_forward_cpu(vector<Blob>& data_blob);

void readAllParamFromdatFile(CnnNet& net, FILE* fp);
void readToInputLayer(InputLayer& input_layer, FILE* fp);
void readToConvLayer(ConvolutionLayer& conv_layer, FILE* fp);
void readToPoolingLayer(PoolingLayer& conv_layer, FILE* fp);
void readToInnerproductLayer(InnerProductLayer& innerproduct_layer, FILE* fp);

void readToSliceLayer(SliceLayer& slice_layer, FILE* fp);





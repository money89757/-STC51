#include "net.h"
#include "pthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <curl/curl.h>
using namespace std;
#define BYTE unsigned char

#define POSTURL "http://192.168.0.123:8081/index1.html"

#define FILENAME "curlposttest.log"
    struct stuff{  
            char *sno;   
            float score_; 
    };  
string *strss;
size_t write_data(void* buffer,size_t size,size_t nmemb,void* stream)
{
	//FILE *fptr = (FILE* )stream;
	string *str = dynamic_cast<string*>((string*)stream);
	char* pdata = (char*)buffer;
	str->append(pdata,size*nmemb);
	strss = str;
	//fwrite(buffer,size,nmemb,fptr);
	return size*nmemb;
}

void face_curl(char *curl_buff)
{
	string curl_fanhui;
	CURL *curl;
	CURLcode res;
	FILE* fptr;
	char curl_buffer[25600];
	struct curl_slist *http_header = NULL;
	
	if((fptr = fopen(FILENAME,"w"))==NULL)
	{
		fprintf(stderr,"fopen file error:%s\n",FILENAME);
	}
	curl = curl_easy_init();
	if(!curl)
	{
		fprintf(stderr,"curl init failed\n");
	}
	curl_easy_setopt(curl,CURLOPT_URL,POSTURL);
	curl_easy_setopt(curl,CURLOPT_POSTFIELDS,curl_buff);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,(void*)&curl_fanhui);
	curl_easy_setopt(curl,CURLOPT_POST,1);
	curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1);
	curl_easy_setopt(curl,CURLOPT_COOKIEFILE,"curlposttest.cookie");
	
	res = curl_easy_perform(curl);
	
	curl_easy_cleanup(curl);
}

//多线程计算相似度
float computeSimilarScore(CnnNet& net, Mat inImg_first, Mat inImg_second)
{
	int num =0 ;
	stuff * face_temp = new stuff;
	string asd;
	string *str;
	char* temmp = {"id="};
	char *temp = new char;
	char *snos= new char;
	
	
	float *fea22 = (float*)malloc(256*sizeof(float));
	BYTE *fea11 = (BYTE*)malloc(256*sizeof(float));
	vector<float> res_fea2 = Cnn_Forward(net, inImg_second);
	Mat fea_2 = Mat(res_fea2);
	face_temp->score_ = 0.;
	for(int i=1;i<9;i++)
	{
		sprintf(temp, "%d ",i);
		asd = asd + "id=";
		asd += *temp;
		asd += "&flag=0";
		face_curl(asd.data());
		asd.clear();
		memcpy(fea11,strss->data(),256*sizeof(float));
		
		memcpy(fea22,fea11,256*sizeof(float));
		vector<float> res_fea02(fea22,fea22+256);
		Mat face_ff1 = Mat(res_fea02);

		Mat score_mat = (face_ff1.t() * fea_2) / norm(face_ff1) / norm(fea_2);
		float score = score_mat.at<float>(0, 0);

		if(score > face_temp->score_)
		{
			face_temp->score_ = score;
			sprintf(snos, "%d ",i);
			//printf("%s\n",face_temp->sno);
		}
		
	}
	asd = asd + "id=";
	asd += *snos;
	asd += "&flag=1";

	face_curl(asd.data());
	face_temp->sno = strss->data();
	asd.clear();	
	printf("name:%s",face_temp->sno);

	asd = asd + "id=";
	asd += *snos;
	asd += "&flag=2";
printf("asd:%s\n",asd.data());
	face_curl(asd.data());
	
	return face_temp->score_;
}


vector<float> Cnn_Forward(CnnNet& net, Mat inImg)
{
	net.input.forward_cpu(inImg, net.input_datas_);
	
	//输入层测试正确
	/*for (int i = 0; i < net.input_datas_[0].counts_; i++)
	{
	cout << net.input_datas_[0].cpu_data()[i] << endl;
	outFea << net.input_datas_[0].cpu_data()[i] << std::endl;
	}*/
	net.conv_1.forward_cpu(net.input_datas_, net.conv_1_datas_);

	net.slice_1.forward_cpu(net.conv_1_datas_, net.slice_1_datas_);
	net.eltwise_1.forward_cpu(net.slice_1_datas_, net.eltwise_1_datas_);
	net.pool_1.forward_cpu(net.eltwise_1_datas_, net.pool_1_datas_);

	net.conv_2_a.forward_cpu(net.pool_1_datas_, net.conv_2_a_datas_);
	net.slice_2_a.forward_cpu(net.conv_2_a_datas_, net.slice_2_a_datas_);
	net.eltwise_2_a.forward_cpu(net.slice_2_a_datas_, net.eltwise_2_a_datas_);
	net.conv_2.forward_cpu(net.eltwise_2_a_datas_, net.conv_2_datas_);
	net.slice_2.forward_cpu(net.conv_2_datas_, net.slice_2_datas_);
	net.eltwise_2.forward_cpu(net.slice_2_datas_, net.eltwise_2_datas_);
	net.pool_2.forward_cpu(net.eltwise_2_datas_, net.pool_2_datas_);

	net.conv_3_a.forward_cpu(net.pool_2_datas_, net.conv_3_a_datas_);
	net.slice_3_a.forward_cpu(net.conv_3_a_datas_, net.slice_3_a_datas_);
	net.eltwise_3_a.forward_cpu(net.slice_3_a_datas_, net.eltwise_3_a_datas_);
	net.conv_3.forward_cpu(net.eltwise_3_a_datas_, net.conv_3_datas_);
	net.slice_3.forward_cpu(net.conv_3_datas_, net.slice_3_datas_);
	net.eltwise_3.forward_cpu(net.slice_3_datas_, net.eltwise_3_datas_);
	net.pool_3.forward_cpu(net.eltwise_3_datas_, net.pool_3_datas_);

	net.conv_4_a.forward_cpu(net.pool_3_datas_, net.conv_4_a_datas_);
	net.slice_4_a.forward_cpu(net.conv_4_a_datas_, net.slice_4_a_datas_);
	net.eltwise_4_a.forward_cpu(net.slice_4_a_datas_, net.eltwise_4_a_datas_);
	net.conv_4.forward_cpu(net.eltwise_4_a_datas_, net.conv_4_datas_);
	net.slice_4.forward_cpu(net.conv_4_datas_, net.slice_4_datas_);
	net.eltwise_4.forward_cpu(net.slice_4_datas_, net.eltwise_4_datas_);

	net.conv_5_a.forward_cpu(net.eltwise_4_datas_, net.conv_5_a_datas_);
	net.slice_5_a.forward_cpu(net.conv_5_a_datas_, net.slice_5_a_datas_);
	net.eltwise_5_a.forward_cpu(net.slice_5_a_datas_, net.eltwise_5_a_datas_);
	net.conv_5.forward_cpu(net.eltwise_5_a_datas_, net.conv_5_datas_);
	net.slice_5.forward_cpu(net.conv_5_datas_, net.slice_5_datas_);
	net.eltwise_5.forward_cpu(net.slice_5_datas_, net.eltwise_5_datas_);
	net.pool_4.forward_cpu(net.eltwise_5_datas_, net.pool_4_datas_);

	net.fc_1.forward_cpu(net.pool_4_datas_, net.fc_1_datas_);
	net.slice_6.forward_cpu(net.fc_1_datas_, net.slice_6_datas_);
	net.eltwise_6.forward_cpu(net.slice_6_datas_, net.eltwise_6_datas_);


	vector<float> result_fea;
	result_fea.clear();

	for (int i = 0; i < net.eltwise_6_datas_[0].counts_; i++)
	{
		result_fea.push_back(net.eltwise_6_datas_[0].cpu_data()[i]);	
	}
	return result_fea;
	//卷积层测试正确
	/*for (int i = 0; i < net.conv_1_datas_[0].counts_; i++)
	{
		cout << net.conv_1_datas_[0].cpu_data()[i] << endl;
		outFea << net.conv_1_datas_[0].cpu_data()[i] << std::endl;
	}*/

	//net.pool_1.forward_cpu(net.conv_1_datas_, net.pool_1_datas_);

	////测试pooling层正确
	///*for (int i = 0; i < net.pool_1_datas_[0].counts_; i++)
	//{
	//	cout << net.pool_1_datas_[0].cpu_data()[i] << endl;
	//}*/
	//net.conv_2.forward_cpu(net.pool_1_datas_, net.conv_2_datas_);

	///*for (int i = 0; i < net.conv_2_datas_[0].counts_; i++)
	//{
	//cout << net.conv_2_datas_[0].cpu_data()[i] << endl;
	//}*/
	//net.pool_2.forward_cpu(net.conv_2_datas_, net.pool_2_datas_);

	///*for (int i = 0; i < net.pool_2_datas_[0].counts_; i++)
	//{
	//cout << net.pool_2_datas_[0].cpu_data()[i] << endl;
	//}*/

	//net.ip_1.forward_cpu(net.pool_2_datas_, net.ip_1_datas_);

	////全链接层测试正确
	///*for (int i = 0; i < net.ip_1_datas_[0].counts_; i++)
	//{
	//cout << net.ip_1_datas_[0].cpu_data()[i] << endl;
	//}*/

	////原位运算
	//relu_forward_cpu(net.ip_1_datas_);

	////relu测试正确
	///*for (int i = 0; i < net.ip_1_datas_[0].counts_; i++)
	//{
	//cout << net.ip_1_datas_[0].cpu_data()[i] << endl;
	//}*/
	//net.ip_2.forward_cpu(net.ip_1_datas_, net.ip_2_datas_);
	///*for (int i = 0; i < net.ip_2_datas_[0].counts_; i++)
	//{
	//cout << net.ip_2_datas_[0].cpu_data()[i] << endl;
	//}*/

	//net.softmax.forward_cpu(net.ip_2_datas_, net.softmax_datas_);
	//for (int i = 0; i < net.softmax_datas_[0].counts_; i++)
	//{
	//	cout << net.softmax_datas_[0].cpu_data()[i] << endl;
	//}
}


void readAllParamFromdatFile(CnnNet& net, FILE* fp)
{
	readToInputLayer(net.input, fp);
	readToConvLayer(net.conv_1, fp);
	readToSliceLayer(net.slice_1, fp);
	readToPoolingLayer(net.pool_1, fp);
	
	readToConvLayer(net.conv_2_a, fp);
	readToSliceLayer(net.slice_2_a, fp);
	readToConvLayer(net.conv_2, fp);
	readToSliceLayer(net.slice_2, fp);
	readToPoolingLayer(net.pool_2, fp);

	readToConvLayer(net.conv_3_a, fp);
	readToSliceLayer(net.slice_3_a, fp);
	readToConvLayer(net.conv_3, fp);
	readToSliceLayer(net.slice_3, fp);
	readToPoolingLayer(net.pool_3, fp);

	readToConvLayer(net.conv_4_a, fp);
	readToSliceLayer(net.slice_4_a, fp);
	readToConvLayer(net.conv_4, fp);
	readToSliceLayer(net.slice_4, fp);

	readToConvLayer(net.conv_5_a, fp);
	readToSliceLayer(net.slice_5_a, fp);
	readToConvLayer(net.conv_5, fp);
	readToSliceLayer(net.slice_5, fp);
	readToPoolingLayer(net.pool_4, fp);

	readToInnerproductLayer(net.fc_1, fp);
	readToSliceLayer(net.slice_6, fp);
	//readToPoolingLayer(net.pool_1, fp);
	//readToConvLayer(net.conv_2, fp);
	//readToPoolingLayer(net.pool_2, fp);
	//readToInnerproductLayer(net.ip_1, fp);
	//readToInnerproductLayer(net.ip_2, fp);

	net.input.layer_set_up(net.input_datas_);
	net.conv_1.layer_set_up(net.input_datas_, net.conv_1_datas_);
	net.slice_1.layer_set_up(net.conv_1_datas_, net.slice_1_datas_);
	net.eltwise_1.layer_set_up(net.slice_1_datas_, net.eltwise_1_datas_);
	net.pool_1.layer_set_up(net.eltwise_1_datas_, net.pool_1_datas_);

	net.conv_2_a.layer_set_up(net.pool_1_datas_, net.conv_2_a_datas_);
	net.slice_2_a.layer_set_up(net.conv_2_a_datas_, net.slice_2_a_datas_);
	net.eltwise_2_a.layer_set_up(net.slice_2_a_datas_, net.eltwise_2_a_datas_);
	net.conv_2.layer_set_up(net.eltwise_2_a_datas_, net.conv_2_datas_);
	net.slice_2.layer_set_up(net.conv_2_datas_, net.slice_2_datas_);
	net.eltwise_2.layer_set_up(net.slice_2_datas_, net.eltwise_2_datas_);
	net.pool_2.layer_set_up(net.eltwise_2_datas_, net.pool_2_datas_);

	net.conv_3_a.layer_set_up(net.pool_2_datas_, net.conv_3_a_datas_);
	net.slice_3_a.layer_set_up(net.conv_3_a_datas_, net.slice_3_a_datas_);
	net.eltwise_3_a.layer_set_up(net.slice_3_a_datas_, net.eltwise_3_a_datas_);
	net.conv_3.layer_set_up(net.eltwise_3_a_datas_, net.conv_3_datas_);
	net.slice_3.layer_set_up(net.conv_3_datas_, net.slice_3_datas_);
	net.eltwise_3.layer_set_up(net.slice_3_datas_, net.eltwise_3_datas_);
	net.pool_3.layer_set_up(net.eltwise_3_datas_, net.pool_3_datas_);

	net.conv_4_a.layer_set_up(net.pool_3_datas_, net.conv_4_a_datas_);
	net.slice_4_a.layer_set_up(net.conv_4_a_datas_, net.slice_4_a_datas_);
	net.eltwise_4_a.layer_set_up(net.slice_4_a_datas_, net.eltwise_4_a_datas_);
	net.conv_4.layer_set_up(net.eltwise_4_a_datas_, net.conv_4_datas_);
	net.slice_4.layer_set_up(net.conv_4_datas_, net.slice_4_datas_);
	net.eltwise_4.layer_set_up(net.slice_4_datas_, net.eltwise_4_datas_);

	net.conv_5_a.layer_set_up(net.eltwise_4_datas_, net.conv_5_a_datas_);
	net.slice_5_a.layer_set_up(net.conv_5_a_datas_, net.slice_5_a_datas_);
	net.eltwise_5_a.layer_set_up(net.slice_5_a_datas_, net.eltwise_5_a_datas_);
	net.conv_5.layer_set_up(net.eltwise_5_a_datas_, net.conv_5_datas_);
	net.slice_5.layer_set_up(net.conv_5_datas_, net.slice_5_datas_);
	net.eltwise_5.layer_set_up(net.slice_5_datas_, net.eltwise_5_datas_);
	net.pool_4.layer_set_up(net.eltwise_5_datas_, net.pool_4_datas_);

	net.fc_1.layer_set_up(net.pool_4_datas_, net.fc_1_datas_);
	net.slice_6.layer_set_up(net.fc_1_datas_, net.slice_6_datas_);
	net.eltwise_6.layer_set_up(net.slice_6_datas_, net.eltwise_6_datas_);

	//net.pool_1.layer_set_up(net.conv_1_datas_, net.pool_1_datas_);
	//net.conv_2.layer_set_up(net.pool_1_datas_, net.conv_2_datas_);
	//net.pool_2.layer_set_up(net.conv_2_datas_, net.pool_2_datas_);
	//net.ip_1.layer_set_up(net.pool_2_datas_, net.ip_1_datas_);
	//net.ip_2.layer_set_up(net.ip_1_datas_, net.ip_2_datas_);
	//net.softmax.layer_set_up(net.ip_2_datas_, net.softmax_datas_);
}


void readToInputLayer(InputLayer& input_layer, FILE* fp)
{
	fread(&input_layer.input_params_.channel_, sizeof(int), 1, fp);// 写stride
	fread(&input_layer.input_params_.height_, sizeof(int), 1, fp);// 写stride
	fread(&input_layer.input_params_.width_, sizeof(int), 1, fp);// 写stride
}
void readToConvLayer(ConvolutionLayer& conv_layer, FILE* fp)
{

	int* pad_ = NULL;
	int* stride_ = NULL;
	int* kernel_size_ = NULL;
	int* dilation_ = NULL;
	fread(&conv_layer.convolution_param_.pad_size_, sizeof(int), 1, fp);
	if (conv_layer.convolution_param_.pad_size_ > 0)
	{
		pad_ = new int[conv_layer.convolution_param_.pad_size_];
		fread(pad_, sizeof(int), conv_layer.convolution_param_.pad_size_, fp);
		for (int i = 0; i < conv_layer.convolution_param_.pad_size_; i++)
		{
			conv_layer.convolution_param_.m_pad_.push_back(pad_[i]);
		}
	}

	fread(&conv_layer.convolution_param_.stride_size_, sizeof(int), 1, fp);
	if (conv_layer.convolution_param_.stride_size_ > 0)
	{
		stride_ = new int[conv_layer.convolution_param_.stride_size_];
		fread(stride_, sizeof(int), conv_layer.convolution_param_.stride_size_, fp);
		for (int i = 0; i < conv_layer.convolution_param_.stride_size_; i++)
		{
			conv_layer.convolution_param_.m_stride_.push_back(stride_[i]);
		}
	}

	fread(&conv_layer.convolution_param_.kernel_size_size_, sizeof(int), 1, fp);
	if (conv_layer.convolution_param_.kernel_size_size_ > 0)
	{
		kernel_size_ = new int[conv_layer.convolution_param_.kernel_size_size_];
		fread(kernel_size_, sizeof(int), conv_layer.convolution_param_.kernel_size_size_, fp);
		for (int i = 0; i < conv_layer.convolution_param_.kernel_size_size_; i++)
		{
			conv_layer.convolution_param_.m_kernel_size_.push_back(kernel_size_[i]);
		}
	}

	fread(&conv_layer.convolution_param_.dilation_size_, sizeof(int), 1, fp);
	if (conv_layer.convolution_param_.dilation_size_ > 0)
	{
		dilation_ = new int[conv_layer.convolution_param_.dilation_size_];
		fread(dilation_, sizeof(int), conv_layer.convolution_param_.dilation_size_, fp);
		for (int i = 0; i < conv_layer.convolution_param_.dilation_size_; i++)
		{
			conv_layer.convolution_param_.m_dilation_.push_back(dilation_[i]);
		}
	}

	fread(&conv_layer.convolution_param_.has_pad_h_, sizeof(bool), 1, fp);
	fread(&conv_layer.convolution_param_.has_pad_w_, sizeof(bool), 1, fp);
	fread(&conv_layer.convolution_param_.pad_h_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.pad_w_, sizeof(int), 1, fp);

	fread(&conv_layer.convolution_param_.has_kernel_h_, sizeof(bool), 1, fp);
	fread(&conv_layer.convolution_param_.has_kernel_w_, sizeof(bool), 1, fp);
	fread(&conv_layer.convolution_param_.kernel_h_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.kernel_w_, sizeof(int), 1, fp);

	fread(&conv_layer.convolution_param_.has_stride_h_, sizeof(bool), 1, fp);
	fread(&conv_layer.convolution_param_.has_stride_w_, sizeof(bool), 1, fp);
	fread(&conv_layer.convolution_param_.stride_h_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.stride_w_, sizeof(int), 1, fp);

	fread(&conv_layer.convolution_param_.group_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.axis_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.num_output_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.force_nd_im2col_, sizeof(bool), 1, fp);


	fread(&conv_layer.convolution_param_.weight_num_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.weight_channels_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.weight_height_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.weight_width_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.bias_num_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.bias_channels_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.bias_height_, sizeof(int), 1, fp);
	fread(&conv_layer.convolution_param_.bias_width_, sizeof(int), 1, fp);

	int weight_count = conv_layer.convolution_param_.weight_num_ * conv_layer.convolution_param_.weight_channels_ * conv_layer.convolution_param_.weight_height_ * conv_layer.convolution_param_.weight_width_;
	int bias_count = conv_layer.convolution_param_.bias_num_ * conv_layer.convolution_param_.bias_channels_ * conv_layer.convolution_param_.bias_height_ * conv_layer.convolution_param_.bias_width_;
	float* weight = (float*)malloc(sizeof(float) * weight_count);
	float* bias = (float*)malloc(sizeof(float) * bias_count);
	fread(weight, sizeof(float), weight_count, fp);

	fread(bias, sizeof(float), bias_count, fp);



	//将权重和偏置矩阵赋值给blobs
	conv_layer.weights_.reshape(conv_layer.convolution_param_.weight_num_, conv_layer.convolution_param_.weight_channels_, conv_layer.convolution_param_.weight_height_, conv_layer.convolution_param_.weight_width_);
	conv_layer.weights_.set_data(weight);

	conv_layer.bias_.reshape(conv_layer.convolution_param_.bias_num_, conv_layer.convolution_param_.bias_channels_, conv_layer.convolution_param_.bias_height_, conv_layer.convolution_param_.bias_width_);
	conv_layer.bias_.set_data(bias);
	
	if (pad_ != NULL)
	{
		delete[] pad_;
	}
	if (stride_ != NULL)
	{
		delete[] stride_;
	}
	if (kernel_size_ != NULL)
	{
		delete[] kernel_size_;
	}
	if (dilation_ != NULL)
	{
		delete[] dilation_;
	}
	free(weight);
	free(bias);


	//测试bias是否读正确
	/*for (int i = 0; i < weight_count; i++)
	{
	std::cout << conv_layer.weights_.cpu_data()[i] << std::endl;
	outFea << conv_layer.weights_.cpu_data()[i] << std::endl;
	}*/
}

void readToPoolingLayer(PoolingLayer& conv_layer, FILE* fp)
{
	fread(&conv_layer.pool_param_.global_pooling_, sizeof(bool), 1, fp);
	fread(&conv_layer.pool_param_.pool_method_, sizeof(int), 1, fp);

	fread(&conv_layer.pool_param_.has_kernel_size_, sizeof(bool), 1, fp);
	fread(&conv_layer.pool_param_.kernel_size_, sizeof(int), 1, fp);
	fread(&conv_layer.pool_param_.kernel_h_, sizeof(int), 1, fp);
	fread(&conv_layer.pool_param_.kernel_w_, sizeof(int), 1, fp);

	fread(&conv_layer.pool_param_.has_stride_h_, sizeof(bool), 1, fp);
	fread(&conv_layer.pool_param_.stride_, sizeof(int), 1, fp);
	fread(&conv_layer.pool_param_.stride_h_, sizeof(int), 1, fp);
	fread(&conv_layer.pool_param_.stride_w_, sizeof(int), 1, fp);

	fread(&conv_layer.pool_param_.has_pad_h_, sizeof(bool), 1, fp);
	fread(&conv_layer.pool_param_.pad_, sizeof(int), 1, fp);
	fread(&conv_layer.pool_param_.pad_h_, sizeof(int), 1, fp);
	fread(&conv_layer.pool_param_.pad_w_, sizeof(int), 1, fp);
}

void readToInnerproductLayer(InnerProductLayer& innerproduct_layer, FILE* fp)
{

	fread(&innerproduct_layer.inner_product_param_.num_output_, sizeof(int), 1, fp);
	fread(&innerproduct_layer.inner_product_param_.axis_, sizeof(int), 1, fp);
	fread(&innerproduct_layer.inner_product_param_.weight_num_, sizeof(int), 1, fp);
	fread(&innerproduct_layer.inner_product_param_.weight_channels_, sizeof(int), 1, fp);
	fread(&innerproduct_layer.inner_product_param_.weight_height_, sizeof(int), 1, fp);
	fread(&innerproduct_layer.inner_product_param_.weight_width_, sizeof(int), 1, fp);
	fread(&innerproduct_layer.inner_product_param_.bias_num_, sizeof(int), 1, fp);
	fread(&innerproduct_layer.inner_product_param_.bias_channels_, sizeof(int), 1, fp);
	fread(&innerproduct_layer.inner_product_param_.bias_height_, sizeof(int), 1, fp);
	fread(&innerproduct_layer.inner_product_param_.bias_width_, sizeof(int), 1, fp);

	int weight_count = innerproduct_layer.inner_product_param_.weight_num_ * innerproduct_layer.inner_product_param_.weight_channels_ * innerproduct_layer.inner_product_param_.weight_height_ * innerproduct_layer.inner_product_param_.weight_width_;
	int bias_count = innerproduct_layer.inner_product_param_.bias_num_ * innerproduct_layer.inner_product_param_.bias_channels_ * innerproduct_layer.inner_product_param_.bias_height_ * innerproduct_layer.inner_product_param_.bias_width_;
	float* weight = (float*)malloc(sizeof(float) * weight_count);
	float* bias = (float*)malloc(sizeof(float) * bias_count);
	fread(weight, sizeof(float), weight_count, fp);

	fread(bias, sizeof(float), bias_count, fp);

	//将权重和偏置矩阵赋值给blobs
	innerproduct_layer.weights_.reshape(innerproduct_layer.inner_product_param_.weight_num_, innerproduct_layer.inner_product_param_.weight_channels_, innerproduct_layer.inner_product_param_.weight_height_, innerproduct_layer.inner_product_param_.weight_width_);
	innerproduct_layer.weights_.set_data(weight);

	innerproduct_layer.bias_.reshape(innerproduct_layer.inner_product_param_.bias_num_, innerproduct_layer.inner_product_param_.bias_channels_, innerproduct_layer.inner_product_param_.bias_height_, innerproduct_layer.inner_product_param_.bias_width_);
	innerproduct_layer.bias_.set_data(bias);
	
	free(weight);
	free(bias);
}

void relu_forward_cpu(vector<Blob>& data_blob)
{
	//只接受1个blob作为输入
	float* bottom_data = data_blob[0].cpu_data();
	int count = data_blob[0].counts_;
	for (int i = 0; i < count; ++i) {
		bottom_data[i] = std::max(bottom_data[i], float(0));
	}
}

void readToSliceLayer(SliceLayer& slice_layer, FILE* fp)
{
	fread(&slice_layer.slice_param_.axis_, sizeof(int), 1, fp);
	fread(&slice_layer.slice_param_.slice_dim_, sizeof(int), 1, fp);
}

#pragma once
#include "chnsPyramid.h"
#include <fstream>
#include <vector>

class _bbox
{
public:
	double rootcoord[4];
	double componentid;
	double score;
	_bbox(){}
	~_bbox()
	{}
};

template<class T>
class _matrix
{
public:

	T *value;
	int tsize[2];

	_matrix()
	{
		value=NULL;
	}
	
	~_matrix()
	{
		if (value)
		{
			delete []value;
			value=NULL;
		}
	}

	_matrix(std::ifstream& filein)
	{
		if (value==NULL)
		{
			filein.read((char*)tsize,sizeof(int)*2);
			value = new T[tsize[0]*tsize[1]];
			filein.read((char*)value,sizeof(T)*tsize[0]*tsize[1]);
		}
	}

	void init(const std::vector<T>&data, const int row, const int col)
	{
		if (value)
		{
			delete []value;
			value=NULL;
		}
		tsize[0] = row;
		tsize[1] = col;
		if (data.size()<row*col)
			return;
		int num = row * col;
		value = new T[num];
		for(int i=0; i<num; i++)
		{
			value[i] = data[i];
		}
	}

	void init(std::ifstream& filein)
	{
		if (value==NULL)
		{
			filein.read((char*)tsize,sizeof(int)*2);

			value = new T[tsize[0]*tsize[1]];
			//printf("tsize %d %d \n", tsize[0],tsize[1]);
			filein.read((char*)value,sizeof(T)*tsize[0]*tsize[1]);
		}
	}
};

//	.modelDs	- [] model height + width without padding(eg[100 41])
//	.modelDsPad - [] model height + width with padding(eg[128 64])
//	.stride		- [4] spatial stride between detection windows
//	.cascThr	- [-1] constant cascade threshold(affects speed / accuracy)
//	.cascCal	- [.005] cascade calibration(affects speed / accuracy)
//	.nWeakNum	- [128] vector defining number weak clfs per stage
//
//  model - learned boosted tree classifier w the following fields
//	  .fids		- [K x nWeak] feature ids for each node
//	  .thrs		- [K x nWeak] threshold corresponding to each fid
//	  .child	- [K x nWeak] index of child for each node(1 - indexed)
//	  .hs		- [K x nWeak] log ratio(.5*log(p / (1 - p)) at each node
//	  .weights	- [K x nWeak] total sample weight at each node
//	  .depth	- [K x nWeak] depth of each node
//	  .errs		- [1 x nWeak] error for each tree(for debugging)
//	  .losses	- [1 x nWeak] loss after every iteration(for debugging)
//	  .treeDepth- depth of all leaf nodes(or 0 if leaf depth varies)
class _detector
{
public:
	float lambdas[3];
	int modelDs[2];
	int modelDsPad[2];
	int stride;
	float cascThr;
	float cascCal;
	int nWeakNum;
	std::vector<int>nWeak;
	int shrink;

	// clfs
	_matrix<int>	fids;
	_matrix<float>	thrs;
	_matrix<float>	child;
	_matrix<float>	hs;
	_matrix<float>	weights;
	_matrix<int>	depth;
	std::vector<float> errors;
	std::vector<float> losses;
	int weakNum;
	int treeDepth;

	_detector(){}
	bool readbinary(const std::string& filename);
};

class _valindex
{
public:
	//float value;
	int index;
	/*float area;
	float x1;
	float x2;
	float y1;
	float y2;*/
	double value, area, x1, x2, y1, y2;
	_valindex(){;}
	~_valindex(){;}
};

// used in ObjDetects
void detection(std::vector<_detector>& detectors, byte *img, int width, int height, std::vector<_bbox>& bbox2);

// classify
void decisionTree(_detector& detector, _pPyramid& pPyramid, _Pyramid_output* outPyamind, std::vector<_bbox>& bbox);

// non-max suppression
bool nms3(std::vector<_bbox>&src, std::vector<_bbox>&dst, float overlap);

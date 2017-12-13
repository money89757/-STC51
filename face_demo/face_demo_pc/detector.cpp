//#pragma once
#include "detector.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "dltype.h"
#include <omp.h>

using namespace std;

int prepos[70] = { 0, -1, 0, -2, 0, -3, 0, -4, 0, -5, -1, 0, -1, -1, -1, -2, -1, -3, -1, -4, -2, 0, -2, -1, -2, -2, -2, -3, -3, 0, -3, -1,
-3, -2, -4, 0, -4, -1, -5, 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 1, 1, 1, 2, 1, 3, 1, 4, 2, 1, 2, 2, 2, 3, 3, 1, 3, 2, 4, 1 };

float iou2[35] = { 0.0f };

bool _detector::readbinary(const string& filename)
{
	ifstream filein(filename.c_str(), ios::in | ios::binary);
	if(filein.fail())
	{
		cout << "reading binary fail" << endl;
		return false;
	}
	filein.read((char*)&stride,sizeof(int));
	int temp1;
	filein.read((char*)&temp1,sizeof(int));
	filein.read((char*)lambdas,sizeof(float)*3);
	filein.read((char*)&temp1,sizeof(int));
	filein.read((char*)modelDs,sizeof(int)*2);
	filein.read((char*)&temp1,sizeof(int));
	filein.read((char*)modelDsPad,sizeof(int)*2);
	filein.read((char*)&cascThr,sizeof(float));
	filein.read((char*)&cascCal,sizeof(float));
	int nWeakNum;
	filein.read((char*)&nWeakNum,sizeof(int));
	nWeak.clear();
	for (int i=0;i<nWeakNum;i++)
	{	
		filein.read((char*)&temp1,sizeof(int));
		nWeak.push_back(temp1);
	}
	filein.read((char*)&treeDepth,sizeof(int));
	filein.read((char*)&shrink,sizeof(int));
	fids.init(filein);
	thrs.init(filein);
	child.init(filein);
	hs.init(filein);
	weights.init(filein);
	depth.init(filein);
	filein.close();
	return true;
}

bool rindcom(const _valindex& t1,const _valindex& t2)
{
	return t1.value>=t2.value;
}

bool nms3(vector<_bbox>&src, vector<_bbox>&dst, double overlap)
{
	if (src.empty() == 1)
		return false;

	int srcnum = src.size();
	vector<_valindex>s(srcnum);
	for (int i = 0; i < srcnum; i++)
	{
		s[i].x1 = src[i].rootcoord[0];
		s[i].y1 = src[i].rootcoord[1];
		s[i].x2 = src[i].rootcoord[2];
		s[i].y2 = src[i].rootcoord[3];
		s[i].value = src[i].score;
		s[i].index = i;
		s[i].area = (src[i].rootcoord[2] - src[i].rootcoord[0] + 1 )*(src[i].rootcoord[3] - src[i].rootcoord[1] +1);
	}
	// sort by score
	sort(s.begin(), s.end(), rindcom);
	vector<int>pick;

	int bsize = s.size();
	bool *kp = new bool[bsize];
	memset(kp, 1, bsize);
	for (int i = 0; i < bsize; i++)
	{
		if (!kp[i])
			continue;
		for (int j = i+1; j<bsize; j++)
		{
			if (!kp[j])
				continue;

			double xx1 = max(s[i].x1, s[j].x1);
			double yy1 = max(s[i].y1, s[j].y1);
			double xx2 = min(s[i].x2, s[j].x2);
			double yy2 = min(s[i].y2, s[j].y2);
			double w = xx2 - xx1 + 1;
			double h = yy2 - yy1 + 1;
			if (w>0 && h > 0)
			{
				double u = w*h / min(s[i].area, s[j].area);
				if (u > overlap)
					kp[j] = 0;
			}
		}
	}
	for (int i = 0; i < bsize; i++)
	{
		if (kp[i])
			dst.push_back(src[s[i].index]);
	}

	delete[] kp;
	return true;
}

inline void getChild( float *chns1, int *cids, int *fids, float *thrs, int offset, int &k0, int &k)
{
	float ftr = chns1[cids[fids[k]]];
	k = (ftr<thrs[k]) ? 1 : 2;
	k0=k+=k0*2;
	k+=offset;
}

inline void getChild(FILE* fp, float *chns1, int *cids, int *fids, float *thrs, int offset, int &k0, int &k)
{
	float ftr = chns1[cids[fids[k]]];
	fprintf(fp, "ftr %f %d thrs %f k %d\n", ftr, cids[fids[k]], thrs[k], k);

	k = (ftr<thrs[k]) ? 1 : 2;
	k0=k+=k0*2; 
	k+=offset;
}

void detection(vector<_detector>& detectors, byte *img, int width, int height, vector<_bbox>& bbox2)
{
	_pPyramid pPyramid;
	pPyramid.minDs[0] = detectors[0].modelDs[0];
	pPyramid.minDs[1] = detectors[0].modelDs[1];

	//pPyramid.pad[0] = detectors[0].modelDsPad[0]-detectors[0].modelDs[0];
	//pPyramid.pad[1] = detectors[0].modelDsPad[1]-detectors[0].modelDs[1];

	pPyramid.pad[0] = 8 * pPyramid.shrink;
	pPyramid.pad[1] = 8 * pPyramid.shrink;

	pPyramid.nApprox = 1;
	pPyramid.perOct = 2;

//	pPyramid.lambdas[0] = detectors[0].lambdas[0];
//	pPyramid.lambdas[1] = detectors[0].lambdas[1];
//	pPyramid.lambdas[2] = detectors[0].lambdas[2];

	pPyramid.lambdas[0] = detectors[0].lambdas[2];
	pPyramid.lambdas[1] = detectors[0].lambdas[1];
	pPyramid.lambdas[2] = detectors[0].lambdas[0];

	pPyramid.shrink = detectors[0].shrink;

	// plate detection
	pPyramid.nOctUp = 0;
	pPyramid.pChns.pColor.colorSpace = (_colortype)2;//_colortype::luv;

	_Pyramid_output* outPyamind = new _Pyramid_output;
	vector<_bbox> bboxes;

	chnsPyramid(img, outPyamind, height, width, pPyramid);

	//QueryTime qt;
	//qt.start();
	for (size_t k=0;k<detectors.size();k++)
	{
		vector<_bbox> bbox;
		decisionTree(detectors[k], pPyramid, outPyamind, bbox);
		bboxes.insert(bboxes.end(),bbox.begin(),bbox.end());
	}
	//qt.end("Section::detection");
	nms3(bboxes, bbox2, 0.65);
	release_Pyramid(outPyamind);
	delete outPyamind;
}

void decisionTree(_detector& detector, _pPyramid& pPyramid, _Pyramid_output* outPyamind, vector<_bbox>& bbox)
{
	int pad[2];//={4,4};
	pad[0] = pPyramid.pad[0]; pad[1] = pPyramid.pad[1];
	int shrink = pPyramid.shrink;
	int modelHt = detector.modelDsPad[0];
	int modelWd = detector.modelDsPad[1];
	int stride = detector.stride;
	float cascThr = detector.cascThr;
	const int treeDepth = detector.treeDepth;

	int nTreeNodes = detector.fids.tsize[0];
	int nTrees = detector.fids.tsize[1];

omp_set_num_threads(2);
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < outPyamind->nScales; i++)
	{
     	float *chns = outPyamind->data[i].data;

		//int chnsSize[3];
		int height = outPyamind->data[i].height;
		int width = outPyamind->data[i].width;

		int nChns = 10;

		int height1 = (int)ceil(float(height*shrink - modelHt + 1) / stride);
		int width1 = (int)ceil(float(width*shrink - modelWd + 1) / stride);

		// construct cids array
		// m -	feature index
		// cids -	real location in the feature pool
		int nFtrs = modelHt / shrink*modelWd / shrink*nChns;
		int *cids = new int[nFtrs]; int m = 0;
		for (int z = 0; z < nChns; z++)
		for (int c = 0; c < modelWd / shrink; c++)
		for (int r = 0; r < modelHt / shrink; r++)
			cids[m++] = z*width*height + c*height + r;

		// apply classifier to each patch
		vector<int> rs, cs; vector<float> hs1;
		for (int c = 0; c < width1; c++) for (int r = 0; r < height1; r++) {
			float h = 0, *chns1 = chns + (r*stride / shrink) + (c*stride / shrink)*height;
            for (int t = 0; t < nTrees; t++) {
                int offset = t*nTreeNodes, k = offset, k0 = k;
                while (detector.child.value[k]) {
                    float ftr = chns1[cids[detector.fids.value[k]]];
                    k = (ftr < detector.thrs.value[k]) ? 1 : 0;
                    k0 = k = static_cast<int>(detector.child.value[k0] - k + offset);
                }
                h += detector.hs.value[k]; if (h <= cascThr) break;
            }
			if (h > cascThr) { cs.push_back(c); rs.push_back(r); hs1.push_back(h); }
		}
		delete[] cids; m = cs.size();
		//shift=(modelDsPad-modelDs)/2-pad;

		double shift[2];
		shift[0] = (detector.modelDsPad[0] - detector.modelDs[0]) / 2.0 - pad[0];
		shift[1] = (detector.modelDsPad[1] - detector.modelDs[1]) / 2.0 - pad[1];
#pragma omp critical
        {
            for (int j = 0; j < m; j++)
		    {
			    _bbox bbs;
			    bbs.rootcoord[0] = (cs[j] * stride + shift[1]) / outPyamind->scaleshw[2 * i + 1];
			    bbs.rootcoord[2] = bbs.rootcoord[0] + (double)(detector.modelDs[1]) / outPyamind->scales[i];
			    bbs.rootcoord[1] = (rs[j] * stride + shift[0]) / outPyamind->scaleshw[2 * i];
			    bbs.rootcoord[3] = bbs.rootcoord[1] + (double)(detector.modelDs[0]) / outPyamind->scales[i];
			    bbs.score = hs1[j];
			    bbs.componentid = 0;
			    bbox.push_back(bbs);
		    }
        }
    }
}

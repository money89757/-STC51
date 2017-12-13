//#pragma once
#ifndef _CHNSFEATURE_H
#define _CHNSFEATURE_H

#include <stdlib.h>
#include <string.h>
#include "dltype.h"

enum _colortype{gray, rgb, luv, hsv, orig};

//	 .pColor	 - parameters for color space :
//	   .enabled		- [1] if true enable color channels
//	   .smooth		- [1] radius for image smoothing(using convTri)
//	   .colorSpace	- ['luv'] choices are : 'gray', 'rgb', 'hsv', 'orig'
struct _pColor
{
	bool enabled;
	_colortype colorSpace;
	void init()
	{
		enabled=true;
		colorSpace = luv;
	}
	_pColor()
	{
		enabled=1;
		colorSpace = luv;
	}
	~_pColor(){;}
};

//	 .pGradMag	 - parameters for gradient magnitude :
//	   .enabled		- [1] if true enable gradient magnitude channel
//	   .colorChn	- [0] if > 0 color channel to use for grad computation
//	   .normRad		- [5] normalization radius for gradient
//	   .normConst	- [.005] normalization constant for gradient
//	   .full		- [0] if true compute angles in[0, 2 * pi) else in[0, pi)
struct _pGradMag
{
	bool enabled;
	int colorChn;
	int normRad;
	float normConst;

	void init()
	{
		enabled=true;
		colorChn=0;
		normRad=5;
		normConst=0.0050f;
	}
	_pGradMag()
	{
		enabled=true;
		colorChn=0;
		normRad=5;
		normConst=0.0050f;
	}
	~_pGradMag(){;}
};

//	 .pGradHist	 - parameters for gradient histograms :
//	   .enabled		- [1] if true enable gradient histogram channels
//	   .binSize		- [shrink] spatial bin size(defaults to shrink)
//	   .nOrients	- [6] number of orientation channels
//	   .softBin		- [0] if true use "soft" bilinear spatial binning
//	   .useHog		- [0] if true perform 4 - way hog normalization / clipping
//	   .clipHog		- [.2] value at which to clip hog histogram bins
struct _PGradHist
{
	bool enabled;
	int binSize;
	int nOrients;
	bool softBin;
	bool useHog;
	float clipHog;

	void init()
	{
		enabled=true;
		binSize=4;          // equal to thrink factor
		nOrients=6;
		softBin=0;
		useHog=false;
		clipHog=0.2000f;
	}
	_PGradHist()
	{
		enabled=true;
		binSize=4;
		nOrients=6;
		softBin=0;
		useHog=false;
		clipHog=0.2000f;
	}
	~_PGradHist(){;}

};
struct _pConstom
{
	void init(){;}
	_pConstom(){;}
	~_pConstom(){;}
};

//  pChns - parameters(struct or name / value pairs)
//	 .shrink	- [4] integer down-sampling amount for channels
//	 .pColor	- parameters for color space :
//	 .pGradMag	- parameters for gradient magnitude :
//	 .pGradHist - parameters for gradient histograms :
//	 .pCustom	- parameters for custom channels(optional struct array) :
//	 .complete	- [] if true does not check / set default vals in pChns
struct _pChns
{
	_pColor pColor;
	_pGradMag pGradMag;
	_PGradHist PGradHist;
	_pConstom pConstom;
	bool complete;

	void init(){;}
	_pChns()
	{
		pColor.init();
		pGradMag.init();
		PGradHist.init();
		pConstom.init();
		complete=true;
	}

	~_pChns(){;}
};

//	 pPyramid - parameters(struct or name / value pairs)
//	  .pChns	- parameters for creating channels(see chnsCompute.m)
//	  .nPerOct	- [8] number of scales per octave
//	  .nOctUp	- [0] number of upsampled octaves to compute
//	  .nApprox	- [-1] number of approx.scales(if - 1 nApprox = nPerOct - 1)
//	  .lambdas	- [] coefficients for power law scaling(see BMVC10)
//	  .pad		- [0 0] amount to pad channels(along T / B and L / R)
//	  .minDs	- [16 16] minimum image size for channel computation
//	  .smooth	- [1] radius for channel smoothing(using convTri)
//	  .concat	- [1] if true concatenate channels
//	  .complete	- [] if true does not check / set default vals in pPyramid
struct _pPyramid
{
	_pChns	pChns;
	int		perOct;
	int		nOctUp;
	int		nApprox;
	//float* lambdas;
	float	lambdas[3];
	int		pad[2];
	int		minDs[2];
	bool	smoothIm;
	int		smoothChns;
	bool	concat;
	bool	complete;

	bool	calclambdas;
	int		shrink;

	void init()
	{
		pChns.init();
		perOct=8;
		nOctUp=0;
		nApprox=7;//-1;//
		shrink=4;
		pad[0]=4;//0;//
		pad[1]=4;//0;//
		minDs[0]=72;//16;//
		minDs[1]=72;//16;//
		//lambdas = NULL;
		
		lambdas[0]=0.0f;
		lambdas[1]=0.11579f;
		lambdas[2]=0.11579f;
		calclambdas = false;

		smoothIm=true;
		smoothChns=1;
		concat=true;
		complete=true;
	}
	_pPyramid()
	{
		pChns.init();
		perOct=8;
		nOctUp=0;
		nApprox=7;//-1;//
		shrink=4;
		pad[0]=4;//0;//
		pad[1]=4;//0;//
		minDs[0]=72;//16;//
		minDs[1]=72;//16;//
		//lambdas = NULL;
		
		lambdas[0]=0.0f;
		lambdas[1]=0.11579f;
		lambdas[2]=0.11579f;
		calclambdas = false;

		smoothIm=true;
		smoothChns=1;
		concat=true;
		complete=true;
	}
	~_pPyramid()
	{
		;
// 		if (lambdas!=NULL)
// 		{
// 			delete []lambdas;
// 		}
	}
};

//	 .info - [nTypes x 1] struct array
//	   .name	- channel type name
//	   .pChn	- exact input parameters for given channel type
//	   .nChns	- number of channels for given channel type
//	   .padWith - how channel should be padded(0, 'replicate')
struct _info
{
	std::string name;
	_pChns pChn;
	int nchns;
	std::string padWith;
	
	// parameter for the data
	int width;
	int height;
	int depth;
};

//	.pChns - exact input parameters used
//	  .nTypes	- number of channel types
//	  .data		- [nTypes x 1] cell[h / shrink x w / shrink x nChns] channels
//	  .info		- [nTypes x 1] struct array
struct _chns
{
	_pChns pChns;
	int nTypes;
	float** data;
	_info* info;	// contains width, height and depth used for data
	_chns()
	{
		data = new float*[4];
		info = new _info[4];
	}
	void init()
	{
		delete []data[0];
		delete []data[1];
		delete []data[2];
	}
	~_chns()
	{
		delete []info;
		delete []data;
	}
};

struct _Pyramid_Data
{
	int width;
	int height;
	int depth;
	float* data;
	_Pyramid_Data()
	{
		data = NULL;
	}
	~_Pyramid_Data()
	{
		if (data!=NULL)
		{
			delete []data;
		}
	}
};
//	 pyramid - output struct
//	.pPyramid - exact input parameters used(may change from input)
//	  .nTypes	- number of channel types
//	  .nScales	- number of scales computed
//	  .data		- [nScales x nTypes] cell array of computed channels
//	  .info		- [nTypes x 1] struct array(mirrored from chnsCompute)
//	  .lambdas	- [nTypes x 1] scaling coefficients actually used
//	  .scales	- [nScales x 1] relative scales(approximate)
//	  .scaleshw - [nScales x 2] exact scales for re-sampling h and w
struct _Pyramid_output
{
	_pPyramid pPyramid;
	int ntypes;
	int nScales;
	int ndata;	//nTypes * nScales
	_Pyramid_Data* data;
	_info info;
	//float* lambdas;

	float lambdas[3];	// same as in _pPyramid

	//float* scales;
	double* scales;

	// scaleshw has saved in _Pyramid_Data [WRONG!]
	double* scaleshw;

	bool concat;

	_Pyramid_output()
	{
		data = NULL;
		//lambdas = NULL;
		
		lambdas[0] = 0.0f;
		lambdas[1] = 0.11579f;
		lambdas[2] = 0.11579f;

		scales = NULL;
		scaleshw = NULL;
		concat = true;
		ntypes = 3;
	}
	~_Pyramid_output()
	{
// 		if (lambdas != NULL)
// 		{
// 			delete []lambdas;
// 			lambdas = NULL;
// 		}
		if (scales != NULL)
		{
			delete []scales;
			scales = NULL;
		}
		if (scaleshw != NULL)
		{
			delete[] scaleshw;
			scaleshw = NULL;
		}
	}
};

#endif // _CHNSFEATURE_H
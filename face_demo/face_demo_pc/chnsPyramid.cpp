#include "chnsfeature.h"
#include "chnsPyramid.h"
#include "chnsPyramid_function_neon_instrinsics.h"
#include "dltype.h"

using namespace std;

//template<class T> void imPad( T *A, T *B, int h, int w, int d, int pt, int pb,int pl, int pr);

#define min1(a,b) (((a) < (b)) ? (a) : (b))

// pad A by [pt,pb,pl,pr] and store result in B
template<class T> void imPad2(T *A, T *B, int h, int w, int d, int pt, int pb,
	int pl, int pr, int flag)
{
	int h1 = h + pt, hb = h1 + pb, w1 = w + pl, wb = w1 + pr, x, y, z, mPad;
	int ct = 0, cb = 0, cl = 0, cr = 0;
	if (pt<0) { ct = -pt; pt = 0; } if (pb<0) { h1 += pb; cb = -pb; pb = 0; }
	if (pl<0) { cl = -pl; pl = 0; } if (pr<0) { w1 += pr; cr = -pr; pr = 0; }
	int *xs = NULL, *ys = NULL; x = pr>pl ? pr : pl; y = pt>pb ? pt : pb; mPad = x>y ? x : y;
	bool useLookup = ((flag == 2 || flag == 3) && (mPad>h || mPad > w))
		|| (flag == 3 && (ct || cb || cl || cr));
	// helper macro for padding
#define PAD(XL,XM,XR,YT,YM,YB) \
	for (x = 0; x < pl; x++) for (y = 0; y < pt; y++) B[x*hb + y] = A[(XL + cl)*h + YT + ct]; \
	for (x = 0; x < pl; x++) for (y = pt; y < h1; y++) B[x*hb + y] = A[(XL + cl)*h + YM + ct]; \
	for (x = 0; x < pl; x++) for (y = h1; y < hb; y++) B[x*hb + y] = A[(XL + cl)*h + YB - cb]; \
	for (x = pl; x < w1; x++) for (y = 0; y < pt; y++) B[x*hb + y] = A[(XM + cl)*h + YT + ct]; \
	for (x = pl; x < w1; x++) for (y = h1; y < hb; y++) B[x*hb + y] = A[(XM + cl)*h + YB - cb]; \
	for (x = w1; x < wb; x++) for (y = 0; y < pt; y++) B[x*hb + y] = A[(XR - cr)*h + YT + ct]; \
	for (x = w1; x < wb; x++) for (y = pt; y < h1; y++) B[x*hb + y] = A[(XR - cr)*h + YM + ct]; \
	for (x = w1; x < wb; x++) for (y = h1; y < hb; y++) B[x*hb + y] = A[(XR - cr)*h + YB - cb];

	// pad by appropriate value
	for (z = 0; z < d; z++) {
		// copy over A to relevant region in B
		for (x = 0; x < w - cr - cl; x++)
			memcpy(B + (x + pl)*hb + pt, A + (x + cl)*h + ct, sizeof(T)*(h - ct - cb));
		
		if (flag == 1) {  // "replicate"
			PAD(0, x - pl, w - 1, 0, y - pt, h - 1);
		}
		A += h*w;  B += hb*wb;
	}
#undef PAD
}

double* getScales3(int perOct, int nOctUp, int* minDs, int shrink, int* sz, int& nScales)
{
	double* scales;
	double* dscales;
	nScales = (int)floor((double)perOct*((double)nOctUp + log((double)min1((double)sz[0] / minDs[0], (double)sz[1] / minDs[1])) / log(2.0)) + 1);

	scales = new double[nScales];
	dscales = new double[nScales];

	for (int i = 0; i < nScales; ++i)
	{
		double index = (double)(-1)*i / perOct + nOctUp;
		double temp_pow = pow(2, index);
		dscales[i] = temp_pow;
	}

	int d0, d1;
	if (sz[0] < sz[1])
	{
		d0 = sz[0]; d1 = sz[1];
	}
	else
	{
		d0 = sz[1]; d1 = sz[0];
	}

	//const double shrinkInv = 1 / shrink;

	// acfOpt
	for (int i = 0; i < nScales; ++i)
	{
		double s = dscales[i];

		//double s0 = (((int)(d0*s*shrinkInv + 0.5))*shrink - 0.25*shrink) / d0;
		//double s1 = (((int)(d0*s*shrinkInv + 0.5))*shrink + 0.25*shrink) / d0;

		double s0 = (((int)(d0*s / shrink + 0.5))*shrink - 0.25*shrink) / d0;
		double s1 = (((int)(d0*s / shrink + 0.5))*shrink + 0.25*shrink) / d0;
		double ss[101] = { 0.0 };
		double es0[101] = { 0.0 };
		double es1[101] = { 0.0 };
		int k = 0;
		for (double j = 0; j <= 1; j += 0.01, ++k)
		{
			ss[k] = j*(s1 - s0) + s0;
			es0[k] = d0*ss[k];
			//es0[k] = abs(es0[k] - ((int)(es0[k] * shrinkInv + 0.5))*shrink);
			es0[k] = abs(es0[k] - ((int)(es0[k] / shrink + 0.5))*shrink);
			es1[k] = d1*ss[k];
			//es1[k] = abs(es1[k] - ((int)(es1[k] * shrinkInv + 0.5))*shrink);
			es1[k] = abs(es1[k] - ((int)(es1[k] / shrink + 0.5))*shrink);
		}

		double maxValueofTow[101];
		for (int n = 0; n<101; ++n)
		{
			maxValueofTow[n] = es0[n]>es1[n] ? es0[n] : es1[n];
		}
		int minValueIndex = 0;
		double m_minValue = maxValueofTow[0];
		for (int n = 1; n<100; ++n)
		{
			if (m_minValue > maxValueofTow[n])
			{
				m_minValue = maxValueofTow[n];
				minValueIndex = n;
			}
		}
		double temp_ss = ss[minValueIndex];
		dscales[i] = temp_ss;
	}

	int* kp = new int[nScales];
	int minusNum = 0;
	for (int i = 0; i < nScales - 1; ++i)
	{
		if (dscales[i] != dscales[i + 1])
		{
			kp[i] = 1;
		}
		else
		{
			kp[i] = 0;
			minusNum++;
		}
	}
	kp[nScales - 1] = 1;
	double* tempScales = dscales;
	double* tempScalesIndex = tempScales;
	dscales = new double[nScales - minusNum];
	double* scalesIndex = dscales;
	for (int i = 0; i < nScales; ++i, ++tempScalesIndex)
	{
		if (kp[i] == 1)
		{
			scalesIndex[0] = *tempScalesIndex;
			++scalesIndex;
		}
	}
	nScales -= minusNum;
	delete[]kp;
	delete[]tempScales;
	tempScalesIndex = NULL;
	scalesIndex = NULL;
	for (int i = 0; i < nScales; ++i)
		scales[i] = dscales[i]; // memory is not fully used
	delete[]dscales;
	return scales;
}

template<class T>
void copyMax(T* input, T* output, int width, int height, int depth)
{
	memcpy(output,input,sizeof(T)*width*height*depth);
}

// matrix transpose
template<class T>
void transMax(T* input, T* output, int width, int height, int depth)
{
	for (int i=0; i<depth; ++i)
	{
		T* input_Bin = input+width*height*i;
		T* output_Bin = output+width*height*i;
		int IndexTrans;
		int IndexOut;
		for (int y=0; y<height; ++y)
		{
			for (int x=0; x<width; ++x)
			{
				IndexOut = y*width+x;
				IndexTrans = x*height+y;
				output_Bin[IndexTrans] = input_Bin[IndexOut];
			}
		}
		input_Bin = NULL;
		output_Bin = NULL;
	}
}

void chnsCompute(_chns* chns, float* I1, _pChns pChns, int imgWidth, int imgHeight)
{
	//% extract parameters from pChns
	int binSize = pChns.PGradHist.binSize;
	int nOrients = pChns.PGradHist.nOrients;
	float* H = NULL;
	float* G = NULL;
	float* rgbCon = NULL;
	rgbCon = (float*)alMalloc(imgWidth*imgHeight * sizeof(float), 16);
	//% compute color channels
	//	I = rgbConvert(I,colorSpace); M=[]; H=[];
	memcpy(rgbCon, I1, sizeof(float)*imgWidth*imgHeight);

	float* M = (float*)alMalloc(imgWidth*imgHeight*sizeof(float), 16);
	float* O = (float*)alMalloc(imgWidth*imgHeight*sizeof(float), 16);
	float* S = (float*)alMalloc(imgWidth*imgHeight*sizeof(float), 16);

	//% compute gradient magnitude channel
	int full = 0;
	if (pChns.pGradMag.enabled || pChns.PGradHist.enabled)
	{
		gradMag(rgbCon, M, O, imgHeight, imgWidth, 1, full > 0);
		convTri(M, S, imgHeight, imgWidth, 1, pChns.pGradMag.normRad, 1);
		gradMagNorm(M, S, imgHeight, imgWidth, pChns.pGradMag.normConst);
	}
	float* M_trans = (float*)alMalloc(imgWidth*imgHeight*sizeof(float), 16);
	transMax(M, M_trans, imgWidth, imgHeight, 1);
	//% compute gradient histogram channels
	int hb = imgHeight / binSize;
	int wb = imgWidth / binSize;
	if (pChns.PGradHist.enabled)
	{
		if (pChns.PGradHist.useHog == false)
		{
			H = (float*)alMalloc(hb*wb*nOrients*sizeof(float), 16);
			gradHist(M, O, H, imgHeight, imgWidth, binSize, nOrients, pChns.PGradHist.softBin, full > 0);
		}
	}
	alFree(O);
	alFree(S);
	alFree(M_trans);

	////% compute custom channels

	//% construct extra info for output struct
	//    chns->info[0].name = "color channels";
	//    chns->info[0].pChn.pColor = pChns.pColor;
	//    //chns->info[0].nchns = 3;
	//    chns->info[0].nchns = 1;
	//    chns->info[0].padWith = "replicate";
	//    chns->info[0].width = imgWidth;
	//    chns->info[0].height = imgHeight;
	//    //chns->info[0].depth = 3;
	//    chns->info[0].depth = 1;
	//
	//    chns->info[1].name = "gradient magnitude";
	//    chns->info[1].pChn.pGradMag = pChns.pGradMag;
	//    chns->info[1].nchns = 1; chns->info[1].padWith = "0";
	//    chns->info[1].width = imgWidth;
	//    chns->info[1].height = imgHeight;
	//    chns->info[1].depth = 1;
	//
	//    chns->info[2].name = "gradient histogram";
	//    chns->info[2].pChn.PGradHist = pChns.PGradHist;
	//    chns->info[2].nchns = nOrients;
	//    chns->info[2].padWith = "0";
	//    chns->info[2].width = wb;
	//    chns->info[2].height = hb;
	//    chns->info[2].depth = 6;
	//    chns->data[0] = rgbCon; chns->data[1] = M; chns->data[2] = H;

	chns->info[1].name = "gradient magnitude";
	chns->info[1].pChn.pGradMag = pChns.pGradMag;
	chns->info[1].nchns = 1;
	chns->info[1].padWith = "0";
	chns->info[1].width = imgWidth;
	chns->info[1].height = imgHeight;
	chns->info[1].depth = 1;

	chns->info[0].name = "gradient histogram";
	chns->info[0].pChn.PGradHist = pChns.PGradHist;
	chns->info[0].nchns = nOrients;
	chns->info[0].padWith = "0";
	chns->info[0].width = wb;
	chns->info[0].height = hb;
	chns->info[0].depth = 6;

	chns->info[2].name = "color channels";
	chns->info[2].pChn.pColor = pChns.pColor;
	//chns->info[2].nchns = 3;
	chns->info[2].nchns = 1;
	chns->info[2].padWith = "replicate";
	chns->info[2].width = imgWidth;
	chns->info[2].height = imgHeight;
	//chns->info[2].depth = 3;
	chns->info[2].depth = 1;
	chns->data[2] = rgbCon; chns->data[1] = M; chns->data[0] = H;
	// detection needs to recover the channel 
	//% create output struct
	chns->nTypes = 3;
}

int chnsPyramid(byte* inputData, _Pyramid_output* output, int width, int height, _pPyramid pPyramid)
{
	// get default parameters pPyramid
	pPyramid.pChns.PGradHist.binSize = pPyramid.shrink;
	if (pPyramid.nApprox<0)	//(if -1 nApprox=nPerOct-1)
	{
		pPyramid.nApprox = pPyramid.perOct-1;
	}
	pPyramid.minDs[0] = pPyramid.minDs[0]>(pPyramid.shrink*4)?pPyramid.minDs[0]:(pPyramid.shrink*4);
	pPyramid.minDs[1] = pPyramid.minDs[1]>(pPyramid.shrink*4)?pPyramid.minDs[1]:(pPyramid.shrink*4);

	if (height<pPyramid.minDs[0] || width<pPyramid.minDs[1])
	{
		output->pPyramid = pPyramid;
		//output->lambdas = NULL;
		output->ntypes = 0;
		output->nScales = 0;
		output->scales = NULL;
		output->ndata = 0;
		output->data = NULL;
		return 0;
	}

	float *I2 = NULL;
	int d=1;	// [3] default image depth
	//float* lambdas = NULL;
	float lambdas[3];

	//QueryTime qt;
	/************************************************************************/
	//% convert I to appropriate color space (or simply normalize)
	/************************************************************************/
	//qt.start();

	I2 = (float*)alMalloc(width*height*sizeof(float), 16);
	int wh = width*height;
	for (int i = 0; i < wh; i++)
		I2[i] = (inputData[i] / 255.0f);
	d = 1;

	pPyramid.pChns.pColor.colorSpace=orig;
	int sz[2];
	sz[0] = height; sz[1] = width;
	//qt.end("Section::Convert");
	/************************************************************************/
	//% get scales at which to compute features and list of real/approx scales
	/************************************************************************/
	//qt.start();
	double* scales = NULL;
	int nScales;

	scales = getScales3(pPyramid.perOct, pPyramid.nOctUp, pPyramid.minDs, pPyramid.shrink, sz, nScales);
	//scaleshw = [round(sz(1)*scales / shrink)*shrink / sz(1);
	//round(sz(2)*scales / shrink)*shrink / sz(2)]';
	output->scaleshw = new double[nScales * 2];
	for (int i = 0; i < nScales; i++)
	{
		output->scaleshw[2 * i] = round(height * scales[i] / pPyramid.shrink) * pPyramid.shrink / height;
		output->scaleshw[2 * i + 1] = round(width * scales[i] / pPyramid.shrink) * pPyramid.shrink / width;
	}

	if ( (nScales-1-pPyramid.nOctUp*pPyramid.perOct)<(pPyramid.nApprox+1) )
	{
		pPyramid.nApprox = (nScales-1-pPyramid.nOctUp*pPyramid.perOct)-1;
		if (pPyramid.nApprox<0)
		{
			pPyramid.nApprox = 0;
		}
	}

	///////////////////////////////////////////////////////////////
	int isR_num = (int)( (nScales-1)/(pPyramid.nApprox+1) )+1;/*{*/
	int* isR = new int[isR_num];
	memset(isR, 0, sizeof(int)*isR_num);
	for (int i=0, j=0; i<nScales; i+=pPyramid.nApprox+1, ++j) 
	{
		isR[j] = i;
	}
	int* isA = NULL;
	int isA_num = nScales-isR_num;
	if (isA_num==0)
	{
		isA = NULL;
	}
	else
	{
		isA = new int[isA_num];
		memset(isA, 0, sizeof(int)*isA_num);
		for (int i=0, j=0; i<nScales; ++i)
		{
			bool load = true;
			for (int n=0; n<isR_num; ++n)
			{
				if (i==isR[n])
				{
					load = false;
					break;
				}
			}
			if (load)
			{
				isA[j] = i;
				++j;
			}
		}
	}

	int* m_j = new int[isR_num+1];
	m_j[0] = 0;
	m_j[isR_num]=nScales-1;
	for (int i=1; i<isR_num; ++i)
	{
		m_j[i] = (int)floor(((float)(isR[i-1]+isR[i])/2));
	}
	int* isN = new int[nScales];
	for (int i=0; i<nScales; ++i)
	{
		isN[i] = i;
	}
	isN[0] = isR[0];
	for (int i=0; i<isR_num; ++i)
	{
		for (int j=m_j[i]+1; j<=m_j[i+1]; ++j)
		{
			isN[j] = isR[i];
		}
	}
	delete []m_j;
	int nTypes = 0;
	//qt.end("Section::getScales");/*}*/
	/************************************************************************/
	// compute image pyramid [real scales]
	/************************************************************************/
	//qt.start();
	_Pyramid_Data* data = NULL;
	float* shr = NULL;
	int I2_Width = sz[1]; int I2_Height = sz[0];

	{
        double s = scales[0];
		int sz1[2];
		sz1[0] = ((int)(sz[0] * s / pPyramid.shrink + 0.5))*pPyramid.shrink;
		sz1[1] = ((int)(sz[1] * s / pPyramid.shrink + 0.5))*pPyramid.shrink;
		nTypes = 3;
		shr = (float*) wrCalloc(nTypes,sizeof(float));
		data = new _Pyramid_Data[nScales*nTypes];
		//shr=zeros(1,nTypes); for j=1:nTypes, shr(j)=size(chns->data{j},1); end

        // put bin channel in the first for the openmp
        shr[2] = (float)sz1[0];
		shr[1] = (float)sz1[0];
		shr[0] = (float)(sz1[0] / pPyramid.pChns.PGradHist.binSize);

		//shr=sz1(1)./shr; assert(all(shr<=shrink) && all(mod(shr,1)==0));
		for (int j=0; j<nTypes; ++j)
		{
			shr[j] = sz1[0]/shr[j];
			int inshr = (int)shr[j];
			if (shr[j]<=pPyramid.shrink && shr[j]==inshr)
			{
				shr[j] = shr[j]/pPyramid.shrink;
			}
			else
			{
				cout<<"ERROR! WACACACACACACACA"<<endl;
				system("pause");
			}
		}
	}
omp_set_num_threads(2);
#pragma omp parallel for schedule(dynamic)
    for (int n = 0; n<isR_num; ++n)
	{
        float *I1,*I3;I1 = NULL; I3 = NULL;
    	I1 = (float*) alMalloc(sz[0] * sz[1] * d * 2 * sizeof(float), 16);
	    I3 = (float*) alMalloc(sz[0] * sz[1] * d * 2 * sizeof(float), 16);
	    _chns* chns = new _chns;
        int m_i = isR[n];
		//s=scales(i); sz1=round(sz*s/shrink)*shrink;
		if (m_i>nScales)
		{
			cout<<"ERROR! m_i_1 > nScales!"<<endl;
			system("pause");
		}
		double s = scales[m_i];
		int sz1[2];
		sz1[0] = ((int)(sz[0]*s/pPyramid.shrink+0.5))*pPyramid.shrink;
		sz1[1] = ((int)(sz[1]*s/pPyramid.shrink+0.5))*pPyramid.shrink;
		if (sz1[0] > sz[0] || sz1[1] > sz[1])
		{
			alFree(I1);
			alFree(I3);
			I1 = (float*)alMalloc(sz1[0] * sz1[1] * d * 2 * sizeof(float), 16);
			I3 = (float*)alMalloc(sz1[0] * sz1[1] * d * 2 * sizeof(float), 16);
		}

		//if(all(sz==sz1)), I1=I; else I1=imResampleMex(I,sz1(1),sz1(2),1); end
        if (sz1[0]==I2_Height && sz1[1]==I2_Width)
            memcpy(I1, I2, sizeof(float)*(sz1[0]*sz1[1]*d));
		else
			resample(I2, I1, I2_Height, sz1[0], I2_Width, sz1[1], 1, float(1.0));
		//if(s==.5 && (nApprox>0 || nPerOct==1)), I=I1; end

		// should be contained in chsCompute
		convTri1( I1, I3, sz1[0], sz1[1], d, 2.0f, 1 );
		chnsCompute(chns, I3, pPyramid.pChns, sz1[1], sz1[0]);
		float ** data1 = chns->data;
		//if( i==isR(1) )
		// shrink = down-sampling-rate = binSize = window steps = stride
		// decrease computation
		for (int j=0; j<nTypes; ++j)
		{
			// m_i index of nScales
			data[nTypes*m_i + j].data = (float*) alMalloc((size_t)((chns->info[j].width)*shr[j] * (chns->info[j].height)*shr[j] * (chns->info[j].nchns))*sizeof(float), 16);
			if (shr[j]==1)
			{
				memcpy(data[nTypes*m_i+j].data, data1[j], sizeof(float)*(chns->info[j].height*chns->info[j].width*chns->info[j].nchns));
				data[nTypes*m_i+j].height = chns->info[j].height;
				data[nTypes*m_i+j].width = chns->info[j].width;
				data[nTypes*m_i+j].depth = chns->info[j].nchns;
				alFree(data1[j]);
				continue;
			}

			resample(data1[j], data[nTypes*m_i+j].data, chns->info[j].height, (int)(chns->info[j].height*shr[j]+0.5), chns->info[j].width, (int)(chns->info[j].width*shr[j]+0.5), chns->info[j].nchns, float(1.0));
			data[nTypes*m_i+j].height = (int)chns->info[j].height*shr[j];
			data[nTypes*m_i+j].width = (int)chns->info[j].width*shr[j];
			data[nTypes*m_i+j].depth = chns->info[j].nchns;
			alFree(data1[j]);
		}

		data1 = NULL;
    	alFree(I1);
	    alFree(I3);
    	delete chns;
	}

	if (shr!=NULL)
	{
		wrFree((float*)shr);
	}
	alFree(I2);

	//qt.end("Section::RealPyramid");
	/************************************************************************/
	//% if lambdas not specified compute image specific lambdas
	/************************************************************************/
	//qt.start();
	if (pPyramid.nApprox >0 && pPyramid.calclambdas==true)
	{
		//is=1+nOctUp*nPerOct:nApprox+1:nScales;
		//assert(length(is)>=2); if(length(is)>2), is=is(2:3); end
		int is[2];
		if ( (nScales-1-pPyramid.nOctUp*pPyramid.perOct)<(pPyramid.nApprox+1) )
		{
			cout<<"ERROR! FUCK! ERROR AGAIN!"<<endl;
			system("pause");
		}
		if((nScales-1-pPyramid.nOctUp*pPyramid.perOct)>=( (pPyramid.nApprox+1)*2 ))
		{
			is[0] = 1+pPyramid.nOctUp*pPyramid.perOct+(pPyramid.nApprox+1)-1;
			is[1] = is[0]+(pPyramid.nApprox+1);
		}
		else
		{
			is[0] = 1+pPyramid.nOctUp*pPyramid.perOct-1;
			is[1] = is[0]+(pPyramid.nApprox+1);
		}
		// f0=zeros(1,nTypes); f1=f0; d0=data(is(1),:); d1=data(is(2),:);
		float* f0 = (float*)wrCalloc(nTypes, sizeof(float));
		float* f1 = (float*)wrCalloc(nTypes, sizeof(float));

		// nType = 3
		_Pyramid_Data* d0[3];
		d0[0] = &data[is[0]*nTypes]; d0[1] = &data[is[0]*nTypes+1]; d0[2] = &data[is[0]*nTypes+2];
		_Pyramid_Data* d1[3];
		d1[0] = &data[is[1]*nTypes]; d1[1] = &data[is[1]*nTypes+1]; d1[2] = &data[is[1]*nTypes+2];

		// for j=1:nTypes, d=d0{j}; f0(j)=sum(d(:))/numel(d); end
		float* pData;
		for (int j=0; j<nTypes; ++j)
		{
			double sum = 0;
			int num = d0[j]->height*d0[j]->width*d0[j]->depth;
			pData = d0[j]->data;
			for (int n=0; n<num; ++n, ++pData)
			{
				sum+= pData[0];
			}
			f0[j] = sum/num;
		}
		for (int j=0; j<nTypes; ++j)
		{
			float sum = 0;
			int num = d1[j]->height*d1[j]->width*d1[j]->depth;
			float* pData = d1[j]->data;
			for (int n=0; n<num; ++n, ++pData)
			{
				sum+= pData[0];
			}
			f1[j] = (float)sum/num;
		}

		//lambdas = - log2(f0./f1) / log2(scales(is(1))/scales(is(2)));
		//lambdas = new float[nTypes];
		for (int n=0; n<nTypes; ++n)
		{
			if (is[0]>nScales || is[1]>nScales)
			{
				cout<<"ERROR! is_[] > nScales"<<endl;
				system("pause");
			}
			lambdas[n] = (-1)* (log(f0[n]/f1[n])/log(2.0f)) / (log(scales[is[0]]/scales[is[1]])/log(2.0));
		}
		d0[0] = NULL; d0[1] = NULL; d0[2] = NULL;
		d1[0] = NULL; d1[1] = NULL; d1[2] = NULL;
		pData = NULL;
		wrFree((float*)f0);
		wrFree((float*)f1);
	}
	else
	{
		//lambdas = new float[nTypes];
		memcpy(lambdas,pPyramid.lambdas,nTypes*sizeof(float));
	}
	//qt.end("Section::lambda");
	/************************************************************************/
	//% compute image pyramid [approximated scales]
	/************************************************************************/
	//qt.start();
	for (int i=0; i<isA_num; ++i)
	{
		int m_i = isA[i];
		int iR = isN[m_i];
		int sz1[2];
		if (m_i>nScales)
		{
			cout<<"isA ERROR! m_i[] > nScales1 m_i/nScales="<<m_i<<"/"<<nScales<<endl;
			continue;
            system("pause");
		}
		sz1[0] = ((int)(sz[0]*scales[m_i]/pPyramid.shrink+0.5));
		sz1[1] = ((int)(sz[1]*scales[m_i]/pPyramid.shrink+0.5));

		_Pyramid_Data* data1[3];
		data1[0] = &data[iR*3]; data1[1] = &data[iR*3+1]; data1[2] = &data[iR*3+2];
		//rs=(scales(i)/scales(iR)).^-lambdas;
		double rs[3];
		for (int n=0; n<nTypes; ++n)
		{
			rs[n] = pow((scales[m_i]/scales[iR]), (-1.0f)*lambdas[n]);
			//rs[n] = rstable[i * 3 + n];
		}
		//for j=1:nTypes, data1{j}=imResampleMex(data1{j},sz1(1),sz1(2),rs(j)); end
		//data(i,:) = data1;
		for (int j=0; j<nTypes; ++j)
		{
			data[m_i * 3 + j].data = (float*) alMalloc(sz1[0] * sz1[1] * data1[j]->depth*sizeof(float), 16);
			resample(data1[j]->data, data[m_i*3+j].data, data1[j]->height, sz1[0],
				data1[j]->width, sz1[1], data1[j]->depth, float(rs[j]));
			data[m_i*3+j].height = sz1[0];
			data[m_i*3+j].width = sz1[1];
			data[m_i*3+j].depth = data1[j]->depth;
		}
	}

	if (isA != NULL)
	{
		delete []isA;
		isA = NULL;
	}
	if (isR != NULL)
	{
		delete[]isR;
		isR = NULL;
	}
	if (isN != NULL)
	{
		delete []isN;
		isN = NULL;
	}
	//qt.end("Section::approxPyramid");

	/************************************************************************/
	//% smooth channels, optionally pad and concatenate channels
	/************************************************************************/
	//qt.start();
	int ndata_num = nScales*nTypes;

	omp_set_num_threads(2);
    int pad0 = pPyramid.pad[0]/pPyramid.shrink;
    int pad1 = pPyramid.pad[1]/pPyramid.shrink;


    #pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < nTypes * nScales; i++)
    {
        float* tempData = (float*)alMalloc(data[i].height*data[i].width*data[i].depth*sizeof(float), 16);
        float ss = 12.0f / (float)pPyramid.smoothChns / (pPyramid.smoothChns + 2) - 2;
        convTri1(data[i].data, tempData, data[i].height, data[i].width, data[i].depth, ss, 1);
        float *tempData2 = (float*)alMalloc((data[i].height + pad0 * 2)*(data[i].width + pad1 * 2)*data[i].depth*sizeof(float), 16);
        if ( i % 3 == 0)
            imPad2<float>(tempData, tempData2, data[i].height, data[i].width, data[i].depth, pad0, pad1, pad0, pad1, 1);
        else
            imPad2<float>(tempData, tempData2, data[i].height, data[i].width, data[i].depth, pad0, pad1, pad0, pad1, 0);
        alFree(data[i].data);
        alFree(tempData);
        data[i].data = tempData2;
        data[i].height = data[i].height + pad0 * 2;
        data[i].width = data[i].width + pad1 * 2;
    }

    //if(concat && nTypes), data0=data; data=cell(nScales,1); end
	//if(concat && nTypes), for i=1:nScales, data{i}=cat(3,data0{i,:}); end; end

    if (pPyramid.concat== true && nTypes>0)
	{
        ndata_num = nScales;
        _Pyramid_Data* data0 = data;
        data = new _Pyramid_Data[nScales];

#pragma parallel omp for schedule(dynamic)
        for (int i=0; i<nScales; ++i)
        {
            // combine the upon conTri1 if pad is not needed
            int index = 0;
            data[i].data = (float*)alMalloc(data0[i*nTypes].height*data0[i*nTypes].width*(data0[i*nTypes].depth + data0[i*nTypes + 1].depth + data0[i*nTypes + 2].depth)*sizeof(float), 16);

            // change channel orders
            copyMax(data0[i*nTypes+2].data, data[i].data+index, data0[i*nTypes+2].height, data0[i*nTypes+2].width, data0[i*nTypes+2].depth);
            alFree(data0[i*nTypes + 2].data);
            data0[i*nTypes+2].data = NULL;
            index += data0[i*nTypes+2].width*data0[i*nTypes+2].height*data0[i*nTypes+2].depth;

            copyMax(data0[i*nTypes+1].data, data[i].data+index, data0[i*nTypes+1].height, data0[i*nTypes+1].width, data0[i*nTypes+1].depth);
            alFree(data0[i*nTypes + 1].data);
            data0[i*nTypes+1].data = NULL;
            index += data0[i*nTypes+1].width*data0[i*nTypes+1].height*data0[i*nTypes+1].depth;

            copyMax(data0[i*nTypes].data, data[i].data+index, data0[i*nTypes].height, data0[i*nTypes].width, data0[i*nTypes].depth);
            alFree(data0[i*nTypes].data);
            data0[i*nTypes].data = NULL;
            index += (data0[i*nTypes].width*data0[i*nTypes].height*data0[i*nTypes].depth);

            data[i].width = data0[i*nTypes].width;
            data[i].height = data0[i*nTypes].height;
            data[i].depth = data0[i*nTypes].depth+data0[i*nTypes+1].depth+data0[i*nTypes+2].depth;
        }
        delete []data0;
        data0 = NULL;
    }
	else
	{
		cout<<"WARING! the output is not arrange successful!"<<endl;
		system("pause");
	}

	//qt.end("Section::smooth");
	/************************************************************************/
	//% create output struct
	/************************************************************************/
	output->pPyramid = pPyramid;
	//output->lambdas = lambdas;
	memcpy(output->lambdas,lambdas,sizeof(float)*3);
	output->ntypes = nTypes;
	output->nScales = nScales;
	output->scales = scales;
	output->ndata = ndata_num;
	output->data = data;

	return 1;

}

void release_Pyramid(_Pyramid_output* output)
{
	if (output != NULL)
	{
// 		if (output->lambdas != NULL)
// 		{
// 			delete []output->lambdas;
// 			output->lambdas = NULL;
// 		}
		if (output->scales != NULL)
		{
			delete []output->scales;
			output->scales = NULL;
		}

		for (int i=0; i<output->nScales; ++i)
		{
			if (output->data[i].data != NULL)
			{
				alFree(output->data[i].data);
				//delete []output->data[i].data;
				output->data[i].data = NULL;
			}
		}

		if (output->data != NULL)
		{
			delete []output->data;
			output->data = NULL;
		}
	}
}

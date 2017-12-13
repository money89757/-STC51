#include "dltype.h"
#include "ObjDetector.h"
#include <algorithm>
#include <iostream>
#include <fstream>
//#include "JniLog.h"

using namespace std;

const char MODEL_LIST_FILE_NAME[] = "model.txt";

const string joinPath(const string& parentPath, const string& subPath) {
	string fullPath = parentPath;
	if (fullPath[fullPath.size()-1] != '/') {
		fullPath.push_back('/');
	}
	return fullPath + subPath;
}

qdetector *qdetector::createInstance(const std::string path)
{
	ObjDetector *p = new ObjDetector();
	if (!p->init(path))
		return NULL;
	return (qdetector*)p;
}

ObjDetector::ObjDetector()
{

}

ObjDetector::~ObjDetector()
{

}

bool ObjDetector::init(const string path)
{
	_modelDirPath = path;
	string modelListFilePath = joinPath(_modelDirPath, MODEL_LIST_FILE_NAME);
	vector<string> modelFileList;
	if(!readModelList(modelFileList, modelListFilePath)) {
		return false;
	}
	if (modelFileList.size() == 0) {
		return false;
	}
	if(!readModels(modelFileList)) {
		return false;
	}
	return true;
}


// detect mat image
vector<ObjRect> ObjDetector::detectObjs(byte *img, int width, int height, float threshold, int maxCount)
{
	vector<_bbox> bboxs, bboxs_mir;

	// _tdetectors has been loaded in function readModels
	detection(_tdetectors, img, width, height, bboxs);

	// select objects by threshold and maxCounts
	vector<ObjRect> vecResults;
	vecResults.reserve(maxCount);
	int objsCount = 0;
	for (int i = 0; i < bboxs.size(); i++) {
		_bbox box = bboxs[i];
		// Because the list is sorted, we can break loop when we the first image whose score less than threshold occurs.
		if (objsCount >= maxCount || box.score < threshold) {
			break;
		}
		ObjRect objRect;// = results + facesCount;
		objRect.left = box.rootcoord[0];
		objRect.top = box.rootcoord[1];
		objRect.right = box.rootcoord[2];
		objRect.bottom = box.rootcoord[3];
		objRect.score = box.score;
		vecResults.push_back(objRect);
		++objsCount;
	}

	return vecResults;
}

bool ObjDetector::readModelList(vector<string>& modelFileList, const string& modelListFilePath){
	ifstream inputFile(modelListFilePath.c_str());
	if (!inputFile.is_open())
		return false;
	string imageFileName;
	while (inputFile >> imageFileName){
		modelFileList.push_back(joinPath(_modelDirPath, imageFileName));
	}
	inputFile.close();
	return true;
}

bool ObjDetector::readModels(const vector<string>& modelFileList){
	_tdetectors.resize(modelFileList.size());
	for (int i = 0; i < modelFileList.size(); i++)
	{
		if(!_tdetectors[i].readbinary(modelFileList[i]))
			return false;
	}
	return true;
}

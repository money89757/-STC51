#pragma once

#include <string>
#include <vector>
#include "dltype.h"
#include "detector.h"

class ObjDetector : public qdetector
{
public:
	ObjDetector();
	~ObjDetector();

	bool init(const std::string path);
	std::vector<ObjRect> detectObjs(byte *img, int width, int height, float threshold, int maxCount);
private:
	bool readModelList(std::vector<std::string>& modelFileList, const std::string& modelListFilePath);
	bool readModels(const std::vector<std::string>& modelFileList);

	std::string _modelDirPath;
	std::vector<_detector> _tdetectors;
};


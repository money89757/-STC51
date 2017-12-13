#pragma once
#include <stdlib.h>
#include <string>
#include <vector>

#ifndef TYPE_UCHAR
#define TYPE_UCHAR
typedef unsigned char byte;
#endif

// int left, top, right, bottom	- position of object
// float score	-	probability
typedef struct obj_RECT{
	double left, top, right, bottom;
	float score;
	bool operator==(obj_RECT const& t) const
	{
		return t.left==this->left &&t.top==this->top &&t.right==this->right &&t.bottom==this->bottom && t.score==this->score ;
	}
	bool operator!=(obj_RECT const& t) const
	{ 
		return t.left!=this->left || t.top!=this->top ||t.right!=this->right ||t.bottom!=this->bottom || t.score!=this->score;
	}
} ObjRect;

struct qdetector
{
	static qdetector* createInstance(const std::string path);
	virtual std::vector<ObjRect> detectObjs(byte *img, int width, int height, float threshold, int maxCount) = 0;
};

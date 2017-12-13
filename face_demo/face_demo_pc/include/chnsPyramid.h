#pragma once
#include "chnsfeature.h"

int chnsPyramid(byte* inputData, _Pyramid_output* output, int width, int height, _pPyramid pPyramid);
void release_Pyramid(_Pyramid_output* output);

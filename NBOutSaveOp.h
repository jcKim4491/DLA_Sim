#pragma once
#include "dian_nbout.h"

class dian_nbout;

struct NBoutSaveOp
{
	dian_nbout * nbout_p = NULL;
	bool saving = false;		// needs to be saved
	int start_index = 0;		// start index of saving
	int size = 0;				// target size
	int output_num = 0;
	int output_size = 0;
};
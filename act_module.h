#pragma once
#ifndef __ACT_MODULE_H__
#define __ACT_MODULE_H__

#include <iostream>
#include <string>
#include <cmath>
#include "register_module.h"
#include "typedef_dla.h"
#include "dian_stats.h"

class act_module : public m_reg
{
protected:
	// Register access counter
	_ACCESS_CNT _act_access_cnt = 0;
	// type of activation function
	ACTFUNC _cur_actfunc = ACTFUNC::NOACT;

public:
	// Constructor
	act_module();
	act_module(const char* name);
	// Destructor
	~act_module();

	void setActFunc(ACTFUNC actf)
	{
		_cur_actfunc = actf;
	}

	// tick
	void virtual tick(_TICK curTick);
};

#endif //__ACT_MODULE_H__
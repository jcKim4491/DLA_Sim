#pragma once
#ifndef __ADDER_MODULE_H__
#define __ADDER_MODULE_H__
#include <iostream>
#include <string>
#include "timingObject.h"
#include "typedef_dla.h"
#include "register_module.h"
#include "dian_stats.h"

using namespace std;

class m_add : public timingObject
{
protected:
	// Adder bit size
	_BITSIZE _add_bitsize = 16;
	// Input-connected object 1
	timingObject* _in_port1 = NULL;
	// Input-connected object 2
	timingObject* _in_port2 = NULL;
	// Output-connected object
	timingObject* _out_port = NULL;
	// Multiplier Access counter
	_ACCESS_CNT _add_access_cnt = 0;
	// Data in this register
	_DATA_STORAGE _data = 0;
	// Data buffer to be out
	_DATA_STORAGE _data_buffer = 0;

	/* flags */
	// checking overhead
	bool _check_overflow = false;
public:
	// Constructors
	m_add();
	m_add(const char* name, _BITSIZE add_bitsize = 16);
	// Copty Constructor
	m_add(const m_add& p);

	// destructors
	~m_add();

	// tick
	void virtual tick(_TICK curTick);

	// Set Ports
	void setInPort1(timingObject * tobj);
	void setInPort2(timingObject * tobj);
	void setOutPort(timingObject * tobj);

	// Get Ports
	timingObject* getInPort1();
	timingObject* getInPort2();
	timingObject* getOutPort();

	// Get data from buffer
	_DATA_STORAGE getData();

	// Set overflow
	void setOverChecking(bool check)
	{
		_check_overflow = check;
	}
};

#endif //__ADDER_MODULE_H__
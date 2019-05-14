#pragma once
#ifndef __MULT_MODULE_H__
#define __MULT_MODULE_H__
#include <iostream>
#include <string>
#include "timingObject.h"
#include "typedef_dla.h"
#include "register_module.h"
#include "dian_stats.h"

class m_mult : public timingObject
{
protected:
	// Multiplier bit size
	_BITSIZE _mul_bitsize = 16;
	// Input-connected object 1
	timingObject* _in_port1 = NULL;
	// Input-connected object 2
	timingObject* _in_port2 = NULL;
	// Output-connected object
	timingObject* _out_port = NULL;
	// Multiplier Access counter
	_ACCESS_CNT _mul_access_cnt = 0;
	// Data to be forwared
	_DATA_STORAGE _data = 0;
	// Data buffer to be out
	_DATA_STORAGE _data_buffer = 0;

public:
	// Constructor
	m_mult();
	m_mult(const char* name, _BITSIZE mult_bitsize = 16);
	// Copy Constructor
	m_mult(const m_mult& p);

	// Destructor
	~m_mult();

	// tick
	void virtual tick(_TICK curTick);
	void virtual tick(_TICK curTick, bool write);

	// Set Ports
	void setInPort1(timingObject * tobj);
	void setInPort2(timingObject * tobj);
	void setOutPort(timingObject * tobj);

	// Get Ports
	timingObject* getInPort1();
	timingObject* getInPort2();
	timingObject* getOutPort();

	// Get data from buffer
	_DATA_STORAGE virtual getData();

	// calculation
	bool calculation();

private:

};

#endif // __MULT_MODULE_H__
#pragma once
#ifndef __REGISTER_MODULE_H__
#define __REGISTER_MODULE_H__
#include "typedef_dla.h"
#include "timingObject.h"
#include "dian_stats.h"


class m_reg : public timingObject
{
protected:
	// Reg bit size
	_BITSIZE _reg_bitsize = 32;
	// Input-conntected object
	timingObject* _in_port = NULL;
	// Output-connected object
	timingObject* _out_port = NULL;
	// Register access counter
	_ACCESS_CNT _reg_access_cnt = 0;
	// Data in this register
	_DATA_STORAGE _data = 0;
	// Data buffer to be written
	_DATA_STORAGE _data_buffer = 0;

public:
	// Constructor
	m_reg() : timingObject()
	{
		_type = TIMINGOBJ_TYPE::REGISTER;
		_reg_bitsize = 32;
		_reg_access_cnt = 0;
		_in_port = NULL;
		_out_port = NULL;
		_data = 0;
		_data_buffer = 0;
	}
	// Constructor
	m_reg(const char* name, _BITSIZE reg_bitsize = 32) : timingObject(name)
	{
		_type = TIMINGOBJ_TYPE::REGISTER;
		_reg_bitsize = reg_bitsize;
		_reg_access_cnt = 0;
		_in_port = NULL;
		_out_port = NULL;
		_data = 0;
		_data_buffer = 0;
	}

	// Copy Constructor
	m_reg(const m_reg& p) : timingObject(p)
	{
		_type = TIMINGOBJ_TYPE::REGISTER;
		_reg_bitsize = p._reg_bitsize;
		_reg_access_cnt = p._reg_access_cnt;
		_in_port = p._in_port;
		_out_port = p._out_port;
		_data = p._data;
		_data_buffer = p._data_buffer;
	}

	// Destructor
	~m_reg()
	{

	}

	// tick
	void virtual tick(_TICK curTick);
	void virtual tick(_TICK curTick, bool write);

	// Set Ports
	void setInPort(timingObject* tobj);
	void setOutPort(timingObject* tobj);

	// Get Ports
	timingObject* getInPort();
	timingObject* getOutPort();

	// access to data
	_DATA_STORAGE virtual getData();			//getter
	void setData(_DATA_STORAGE data);	//setter

	// Forced feed
	void foreced_feed(_DATA_STORAGE feed_data);

	// input feed
	void input_feed();
};

#endif //__REGISTER_MODULE_H__
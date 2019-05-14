#pragma once
#ifndef __DIANNAO_SB_H__
#define __DIANNAO_SB_H__
#include <iostream>
#include <string>
#include "typedef_dla.h"
#include "optionparser.h"
#include "diannao_system.h"
#include "register_module.h"
#include "dian_nfu1.h"

using namespace std;

class Diannao_system;
class memory_interface;

class dian_sb
{
private:
	// DIANNAO DL use 16 bit fixed point number
	const _BITSIZE _diannao_bitsize = 16;
	// SB Register Array
	m_reg **_sb_regs = NULL;
	// connected NFU1
	dian_nfu1* conn_nfu1 = NULL;
	// data availability
	int _data_avail = 0;
	// current row index to be filled.
	int _cur_row = 0;
	// Diannao system pointer
	Diannao_system *const _dian_sys = NULL;
	// Current opcode in sb
	Opcode _curOp;

public:
	// constructors
	dian_sb();
	dian_sb(Diannao_system *const dian_sys, int SB_REG_CNT = 1024);

	// destructor
	~dian_sb();

	// port connect
	void connect_to_nfu1(dian_nfu1* target_nfu1);
	void switch_port_to_nfu1();

	// feed from memory_interface
	void feed_from_mem(int idx, _DATA_STORAGE data);

	// get current row index to be filled.
	int getCurrentRow();
	// increase current row index by 1.
	void incCurrentRow();

	// getter/setter for data_avail
	int get_data_avail()
	{
		return _data_avail;
	}
	void set_data_avail(int data_avail)
	{
		_data_avail = data_avail;
	}

	void move_to_next_kernel();

	// tick
	void tick(_TICK curTick);
	// feed nfu1
	void feed_nfu1();

	// number of registers
	const int _SB_REG_CNT = 1024;
	// number of entry
	int _SB_ENTRY_CNT = 64;
	// number of Tn
	const int _SB_WIDTH = 256;
};

#endif __DIANNAO_SB_H__


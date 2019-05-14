#pragma once
#ifndef __DIANNAO_NBIN_H__
#define __DIANNAO_NBIN_H__

#include <iostream>
#include <string>
#include <algorithm>
#include "typedef_dla.h"
#include "optionparser.h"
#include "diannao_system.h"
#include "register_module.h"
#include "dian_nfu1.h"
#include "memory_interface.h"

using namespace std;

class Diannao_system;
class memory_interface;
class dian_nfu1;

class dian_nbin
{
private:
	// DIANNAO DL use 16 bit fixed point number
	const _BITSIZE _diannao_bitsize = 16;
	// NB_In Register Array
	m_reg **_nbin_regs = NULL;
	// connected NFU1
	dian_nfu1* conn_nfu1 = NULL;
	// data availability
	int _data_avail = 0;
	// calculation availability
	bool _calc_avail = false;
	// current row index to be filled.
	int _cur_row = 0;
	// Diannao system pointer
	Diannao_system *const _dian_sys = NULL;
	// Current opcode in nbin
	Opcode _curOp;
	// number of calculable kernel set.
	int _calc_set = 0;
	// stop when one layer is end.
	bool _one_layer_end = false;
	// weight for writing back done
	bool _wait_next_output = false;
	// if data is first incomings.
	bool _re_porting = true;
	// wait for filling nbout
	bool _wait_nbout_filling = false;
public:
	// constructors
	dian_nbin();
	dian_nbin(Diannao_system *const dian_sys, int NBIN_REG_CNT = 32);

	// destructor
	~dian_nbin();

	// port connect
	void connect_to_nfu1(dian_nfu1* target_nfu1);
	// port switching
	void switch_port_to_nfu1(bool mult_using, int sb_data_avail);

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
	bool get_calc_avail()
	{
		return _calc_avail;
	}
	int get_calc_set()
	{
		return _calc_set;
	}

	// tick
	void tick(_TICK curTick, bool mult_using, bool nbout_stall, bool nbout_readdone, int sb_data_avail);
	// feed nfu1
	void feed_nfu1();
	// one layer ends
	bool get_one_layer_end()
	{
		return _one_layer_end;
	}
	void set_one_layer_end(bool ole)
	{
		_one_layer_end = ole;
	}

	// get wait for next output
	bool get_wait_next_output()
	{
		return _wait_next_output;
	}
	void set_wait_next_output(bool wno)
	{
		_wait_next_output = wno;
	}
	// get wait NBOut re-filling
	bool get_wait_nbout_filling()
	{
		return _wait_nbout_filling;
	}

	// continue process
	void continue_process();
	// move to next kernel
	void move_to_next_kernel();

	// print
	void print();

	// number of registers
	int _NBIN_REG_CNT = 1024;
	// number of entry
	int _NBIN_ENTRY_CNT = 64;
	// number of Tn
	const int _NBIN_WIDTH = 16;
	// position in input feature map at starting position.
	int _pos_input_offset = 0;
};

#endif //__DIANNAO_NBIN_H__
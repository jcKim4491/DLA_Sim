#pragma once
#ifndef __DIAN_NBOUT_H__
#define __DIAN_NBOUT_H__
#include <iostream>
#include <string>
#include "typedef_dla.h"
#include "optionparser.h"
#include "register_module.h"
#include "diannao_system.h"
#include "memory_interface.h"
#include "NBOutSaveOp.h"

class Diannao_system;
class dian_nbout;
struct NBoutSaveOp;

class dian_nbout
{
public:
	// constructors
	dian_nbout();
	dian_nbout(Diannao_system *const dian_sys, int NBOUT_REG_CNT = 32);

	// destructor
	~dian_nbout();

	// tick
	void tick(_TICK curTick, int calcSet, bool write_done);

	// get current row
	int getCurrentRow()
	{
		return _cur_row;
	}
	// set current row
	void setCurrentRow(int row)
	{
		_cur_row = row;
	}
	// move to next row.
	void incCurrentRow()
	{
		_cur_row = (_cur_row + 1) % _NBOUT_ENTRY_CNT;
	}

	void feed_from_mem(int idx, _DATA_STORAGE data);

	// get reg
	m_reg* getReg(int index);
	// get stall info
	bool getStalled() { return _stalled; }
	// get saveOp
	NBoutSaveOp getSaveOp() { return _saveOp; }

	// number of regs in nbout
	int _NBOUT_REG_CNT = 1024;
	// number of entry
	int _NBOUT_ENTRY_CNT = 64;
	// number of Tn(=width)
	const int _NBOUT_WIDHT = 16;

	// print register
	void print();
private:
	// DIANNAO DL use 16 bit fixed point number
	const _BITSIZE _diannao_bitsize = 16;
	// Diannao system pointer
	Diannao_system * const _dian_sys = NULL;
	// Current opcode in nfu1
	Opcode _curOp;
	// stalled this stage
	bool _stalled = false;

	// Buffer needs to be saved
	NBoutSaveOp _saveOp;

	// current row
	int _cur_row = 0;
	// start index
	int _start_index = 0;
	// NBOut Register array
	m_reg ** _nbout_regs = NULL;
};

#endif //__DIAN_NBOUT_H__
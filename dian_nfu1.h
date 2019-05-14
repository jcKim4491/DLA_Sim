#pragma once
#ifndef __DIANNAO_NFU_1__
#define __DIANNAO_NFU_1__
#include <iostream>
#include <string>
#include "mult_module.h"
#include "dian_nfu2.h"
#include "diannao_system.h"
#include "typedef_dla.h"
#include "dian_nbout.h"

using namespace std;

class Diannao_system;
class dian_nfu2;
class dian_nbout;

class dian_nfu1
{
public:
	// constructors
	dian_nfu1();
	dian_nfu1(Diannao_system *const dian_sys, int MULT_MODULE_CNT = 16);

	// destructor
	~dian_nfu1();

	// input feed

	// component info
	int getMULT_MODDULE_CNT()
	{
		return _MULT_MODULE_CNT;
	}

	// each module info
	m_mult* getM_MUlT(int index);

	// tick
	void tick(_TICK curTick, bool calcAvail, int calcSet, bool stall_from_nbout, bool nbout_filling, bool nbin_stall);
	// do calculation
	void calculating(_TICK curTick);

	// feed from previos modules
	void feed_from_nbin();
	void feed_from_sb();

	// block to set
	void block_from_nbin();
	void block_from_sb();

	// connect to nfu-2
	void connect_to_nfu2(dian_nfu2* target_nfu2);

	// switch connection
	void switch_port_to_nfu2(int calcSet);
	// move to next kernel and update opcode
	void move_to_next_kernel();

	bool get_calculating()
	{
		return _calculating;
	}
	// calc done as mult done
	bool get_mult_done()
	{
		return _calc_done;
	}

	// get current calc Set
	int get_calc_set()
	{
		if (_calc_done)
			return _cur_calcSet;
		else
			return 0;
	}

private:
	// diannao_system pointer
	Diannao_system * _dian_sys = NULL;
	// Current opcode in nfu1
	Opcode _curOp;

	int _MULT_MODULE_CNT = 16;
	// connected nfu2
	dian_nfu2 *conn_nfu2 = NULL;
	// NFU-1's multiplier modules array
	m_mult** nfu1_mults = NULL;
	// feed confirmation
	bool _confirm_nbin = false;
	bool _confirm_sb = false;
	// doing calculation, may be done in several ticks.
	bool _calculating = false;
	bool _data_avail = false;
	bool _calc_done = false;
	int _cur_calcSet = 0;

	int _calcSet_buffer = 0;

	// clock variables for timing simulation.
	int _clk_s = 0;
	int _clk_end = 1;
};

#endif // __DIANNAO_NFU_1__


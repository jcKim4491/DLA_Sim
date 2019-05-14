#pragma once
#ifndef __DIAN_NFU2_H__
#define __DIAN_NFU2_H__

#include <iostream>
#include <string>
#include "typedef_dla.h"
#include "optionparser.h"
#include "register_module.h"
#include "diannao_system.h"
#include "dian_addertree.h"
#include "dian_nfu3.h"
#include "dian_nbout.h"

using namespace std;

class Diannao_system;
class dian_nfu3;
class dian_nbout;

class dian_nfu2
{
public:
	// constructor
	dian_nfu2();
	dian_nfu2(Diannao_system *const dian_sys);

	// destructors
	~dian_nfu2();

	// set input ports
	void setInPorts(timingObject* src, int index);

	// tick
	void tick(_TICK curTick, int calcSet, bool stall_NBOut);

	// connect to nfu3
	void connect_to_nfu3(dian_nfu3 *target_nfu3);
	// connect to nbout
	void connect_to_nbout(dian_nbout* target_nbout);

	// switch backward connection from nbout
	void switch_port_from_nbout(int calcSet);

	// move to next kernel and update opcode
	void move_to_next_kernel();

	int get_calc_set()
	{
		return _calcSet;
	}

private:
	Diannao_system * _dian_sys = NULL;
	dian_addertree ** _dian_adtrees = NULL;
	// connected nbout
	dian_nbout *conn_nbout = NULL;
	// Current opcode in nfu1
	Opcode _s1_Op, _s2_Op, _s3_Op, _s4_Op;

	// data inserted
	bool _data_inserted = false;
	// data available
	bool _data_available = false;

	// calculated set number
	int _calcSet = 0;
	int _calcSet_buffer1 = 0;
	int _calcSet_buffer2 = 0;
};

#endif //__DIAN_NFU2_H__
#pragma once
#ifndef __DIAN_NFU3_H__
#define __DIAN_NFU3_H__
#include <iostream>
#include <string>
#include "typedef_dla.h"
#include "optionparser.h"
#include "register_module.h"
#include "diannao_system.h"
#include "act_module.h"
#include "dian_nbout.h"

using namespace std;

class Diannao_system;
class dian_nbout;

class dian_nfu3
{
public:
	// constructor
	dian_nfu3();
	dian_nfu3(Diannao_system *const dian_sys);

	// destructor
	~dian_nfu3();

	// set input ports
	void setInPorts(timingObject* src, int index);

	// tick
	void tick(_TICK curTick, int calcSet);

	// connection to nbout
	void connection_to_nbout(dian_nbout *target_nbout);
	// switch port
	void switch_port_to_nbout();
	void switch_port_to_nbout(int start_index);

	// move to next kernel and update opcode
	void move_to_next_kernel();

	void set_activation_all(ACTFUNC actfunc);

	// print
	void print();

	int get_calc_set()
	{
		return _calcSet;
	}

private:
	Diannao_system * _dian_sys = NULL;
	act_module ** _actfunc_modules = NULL;
	dian_nbout * conn_nbout = NULL;

	// Current opcode in nfu1
	Opcode _curOp;
	Opcode _delayedOp;
	int _calcSet = 0;
	int _calcSet_buffer1 = 0;

};

#endif //__DIAN_NFU3_H__

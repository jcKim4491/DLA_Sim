#pragma once
#ifndef __DIANNAO_SYSTEM_H__
#define __DIANNAO_SYSTEM_H__
#include <iostream>
#include <string>
#include "typedef_dla.h"
#include "dla_system.h"
#include "memory_interface.h"
#include "dian_nbin.h"
#include "dian_sb.h"
#include "dian_nfu1.h"
#include "dian_nfu2.h"
#include "dian_nfu3.h"
#include "dian_nbout.h"

using namespace std;

class dian_nbin;
class dian_nfu2;
class dian_nfu3;

class Diannao_system : public DLA_System
{
	friend class dian_nbin;
	friend class memory_interface;

public:
	Diannao_system();

	~Diannao_system();

	// Initialize
	void initialize();

	// get components's pointers
	memory_interface* getMemInterface()
	{
		return _mem_inter;
	}

	dian_nbin* getDianNBin()
	{
		return _nbin;
	}

	dian_sb* getDianSB()
	{
		return _sb;
	}

	// tick
	void tick(_TICK curTick);

	// print
	void print_nbin();
	void print_nbout();

	// save output
	void save_output(int size);
	
	// print stats
	void print_stats();

	Opcode _curOp;

	m_reg* _zero_reg;

private:
	memory_interface * _mem_inter = NULL;
	dian_nbin * _nbin = NULL;
	dian_sb * _sb = NULL;
	dian_nfu1 * _nfu1 = NULL;
	dian_nfu2 * _nfu2 = NULL;
	dian_nfu3 * _nfu3 = NULL;
	dian_nbout * _nbout = NULL;

	// last writeback when ending process.
	bool last_writeback = false;


	// Maximum addr 0x400000
	long _feature_addr = 0x000000;
	long _weight_addr = 0x200000;
};

#endif //__DIANNAO_SYSTEM_H__
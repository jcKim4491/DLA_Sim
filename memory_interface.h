#pragma once
#ifndef __MEMORY_INTERFACE_H__
#define __MEMORY_INTERFACE_H__

#include <iostream>
#include <fstream>
#include <string>
#include "typedef_dla.h"
#include "diannao_system.h"
#include "dian_nbin.h"
#include "dian_sb.h"
#include "dian_nbout.h"
#include "NBOutSaveOp.h"
#include "dian_stats.h"

using namespace std;

class Diannao_system;
class dian_nbin;
class dian_sb;
class dian_nbout;

struct NBOutSaveOp;

class memory_interface
{
	// inner memory size
#define INNER_MEM_SIZE 0x400000 //4MB

public:
	// Constructor
	memory_interface();
	memory_interface(Diannao_system *const dian_sys);

	// Destructor
	~memory_interface();

	// Set mode
	void setControl(MEM_CONTROL newCTRL)
	{
		_cur_CONTROL = newCTRL;

		// switch context
		switch (newCTRL)
		{
		case MEM_CONTROL::NBIN_CTRL:
			_cur_addr = 0;
			break;
		case MEM_CONTROL::SB_CTRL:
			_cur_addr = 10000; //for test
			break;
		default:
			break;
		}
	}

	// set input file
	void setInputFile(const char* inputfile)
	{
		_inputfile = string(inputfile);
		_input_set = true;
		cout << "Input file is set to " << inputfile << endl;
	}

	// fill memory
	void fillMemory(long src_addr, long dst_addr, long size, bool overflow_ignore = false);

	// read from memory
	char* readMemory(long addr, long size);
	void setReadArea(long addr, long size);

	void set_read_nbin(long addr, long size);
	void set_read_sb(long addr, long size);
	void set_read_nbout(long addr, long size);

	// set feed parameters
	void set_feed_nbin();
	void set_feed_sb();
	void set_feed_nbout();
	void set_fed_nbout();

	// feed to next stage
	void feed_nbin();
	void feed_sb();
	void feed_nbout();
	// feed from nbout
	void feed_from_nbout();

	// port connect
	void connect_to_nbin(dian_nbin* target_nbin);
	void connect_to_sb(dian_sb* target_sb);
	void connect_to_nbout(dian_nbout* target_nbout);

	// tick
	void tick(_TICK curTick, NBoutSaveOp saveOp);

	// save output
	void save_output(int size);

	// nbout read done flag
	bool get_nbout_readdone()
	{
		return _nbout_readdone;
	}

	bool get_nbout_reading()
	{
		if (_nbout_readdone == false && _nbout_saved_size > 0)
			return true;
		else
			return false;
	}

	// nbout write done flags
	bool get_write_done()
	{
		return _nbout_writedone;
	}
	void set_write_done(bool wd)
	{
		_nbout_writedone = wd;
	}


private:
	Diannao_system * _dian_sys = NULL;

	MEM_CONTROL _cur_CONTROL = MEM_CONTROL::NBIN_CTRL;
	bool _input_set = false;
	string _inputfile;

	// inner memory space
	char* _inMem = NULL;

	// connected modules
	dian_nbin* conn_nbin = NULL;
	dian_sb* conn_sb = NULL;
	dian_nbout* conn_nbout = NULL;

	// nbin read saved
	long _nbin_saved_addr = 0;
	long _nbin_saved_size = 0;
	// sb read saved
	long _sb_saved_addr = 0;
	long _sb_saved_size = 0;
	// nbout read save
	long _nbout_saved_addr = 0;
	long _nbout_saved_size = 0;

	// nbin reg cnt
	int _nbin_width = 0;
	// sb reg cnt
	int _sb_reg_cnt = 0;
	// nbout size param
	int _nbout_width = 0;
	int _nbout_size = 0;

	// nbin current row index
	int _nbin_cur_row_idx = 0;
	// sb current row index
	int _sb_cur_row_idx = 0;
	// nbout current row index
	int _nbout_cur_row_idx = 0;

	// read control
	const int MAXIMUM_READ_NUM = 64;	// maximum number within single cycle.
	int _allowed_max_cycle = 0;			//allowed maximum cycle being scheduled.

	/* use for continuous reading from memory */
	long _cur_addr = 0;
	long _cur_read_size = 0;			// summation of reading bytes per cycle (not clock)
	long _cur_read_length = 0;
	long _cur_cnt = 1;					// for split reading
	long _cur_max_cnt = 1;
	long _tar_addr = 0;					// target address.

	bool _init_feeding = false;			// feed initializtion
	bool _done_feeding = false;			// is feeding done?
	bool _over_loaded = false;

	// feed to nbout
	bool _nbout_readdone = false;

	// feed from nbout
	NBoutSaveOp _saveOp;

	long _cur_write_pos = 0;
	long _cur_write_length = 0;
	bool _nbout_writedone = false;

	// clock counter for timing simulation
	int _clk_s = 0;
	const int _clk_end = 2;
};

#endif //__MEMORY_INTERFACE_H__


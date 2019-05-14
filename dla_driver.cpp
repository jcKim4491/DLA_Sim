#include <iostream>
#include <string>
#include <stdio.h>
#include "dla_system.h"
#include "typedef_dla.h"
#include "diannao_system.h"
#include "optionparser.h"
#include "memory_interface.h"
#include "dian_stats.h"

//variable defs
DLA_System *_current_dla_system = NULL;
static _TICK _curTick = 0;

// stats initialize
unsigned long REGISTER_ACCESS = 0;
unsigned long ADDER_ACCESS = 0;
unsigned long MULT_ACCESS = 0;
unsigned long ACTFUNC_ACCESS = 0;
unsigned long MEMREAD_ACCESS = 0;
unsigned long MEMWRITE_ACCESS = 0;

bool _quite_all = true;

// for test
m_reg *reg_0, *reg_1, *reg_2;
bool _quite_timingobj = true;
bool _quite_moduleconnection = true;
bool _quite_add = true;
bool _quite_dataflow = true;
bool _quite_mult = true;
bool _quite_actfunc = true;
bool _quite_calcSet = false;
bool _quite_nbin = false;
bool _quite_writeback = false;
bool _quite_nbout = false;
bool _quite_nfu2 = false;
bool _quite_nfu1 = false;
bool _quite_sb = false;
bool _quite_nfu3 = false;
bool _quite_dma = false;

//function defs
void runSystem();
void runSystem(_TICK runTick);
void tick();

void runSystem()
{
	//loop clock
}

void runSystem(_TICK runTick)
{
	freopen("out.txt", "w", stdout);

	_TICK temp_tick = 0;
	if (_quite_all)
	{
		_quite_timingobj = true;
		_quite_moduleconnection = true;
		_quite_add = true;
		_quite_dataflow = true;
		_quite_mult = true;
		_quite_actfunc = true;
		_quite_calcSet = true;
		_quite_nbin = true;
		_quite_writeback = true;
		_quite_nbout = true;
		_quite_nfu1 = true;
		_quite_nfu2 = true;
		_quite_nfu1 = true;
		_quite_sb = true;
		_quite_nfu3 = true;
		_quite_dma = true;
	}

	for (temp_tick; temp_tick < runTick; ++temp_tick)
	{
		tick();

		//std::cout << "MULT_ACCESS = " << std::dec << MULT_ACCESS << std::endl;
		//MULT_ACCESS = 0;

		// end process
		if (_current_dla_system->_process_end)
			break;
	}

	cout << "end time : " << dec << temp_tick << endl;
}

void tick()
{
	if(!_quite_all)
	std::cout << "===================================\tTICK : " << dec << _curTick << "\t===================================" << std::endl;
	_current_dla_system->tick(_curTick);

	// time elapsed.
	++_curTick;
}

int main(int argc, char* argv[])
{
	/*
	std::cout << "Initialize DLA-Sim" << std::endl;
	_current_dla_system = new DLA_System("SMALL_DLA");

	_current_dla_system->place_register("REG_0");
	_current_dla_system->place_register("REG_1");
	_current_dla_system->place_register("REG_2");

	_current_dla_system->connect_objects("REG_0", "REG_1");
	_current_dla_system->connect_objects("REG_1", "REG_2");

	reg_0 = (m_reg*)_current_dla_system->findTimingObj_byName("REG_0");
	reg_1 = (m_reg*)_current_dla_system->findTimingObj_byName("REG_1");
	reg_2 = (m_reg*)_current_dla_system->findTimingObj_byName("REG_2");

	reg_0->foreced_feed(950712);

	runSystem(3);

	delete _current_dla_system;
	*/


	// DianNao System.
	Diannao_system *diannao_sys = new Diannao_system();
	memory_interface *_mem_inter = diannao_sys->getMemInterface();
	_current_dla_system = diannao_sys;

	// initialize
	diannao_sys->initialize();

	runSystem(30000000);

	diannao_sys->print_nbin();
	diannao_sys->print_nbout();

	diannao_sys->save_output(64 * 256);
	diannao_sys->print_stats();

	delete diannao_sys;
	//system("pause");
	return 0;
}
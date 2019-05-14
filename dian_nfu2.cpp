#include "dian_nfu2.h"


// Constructors
dian_nfu2::dian_nfu2()
{
}

dian_nfu2::dian_nfu2(Diannao_system *const dian_sys)
	: _dian_sys(dian_sys)
{
	_dian_adtrees = new dian_addertree*[ADTREE_CNT];
	int i;
	for (i = 0; i < ADTREE_CNT; ++i)
	{
		_dian_adtrees[i] = new dian_addertree(i);
	}

	// Copy Opcode
	_s1_Op = dian_sys->_curOp;

	cout << "NFU-2 module is created." << endl;
}

// Destructor
dian_nfu2::~dian_nfu2()
{
	int i;
	for (i = 0; i < ADTREE_CNT; ++i)
		delete _dian_adtrees[i];
	delete[] _dian_adtrees;
	cout << "NFU-2 module is deleted." << endl;
}

// set input ports
void
dian_nfu2::setInPorts(timingObject* src, int index)
{
	// port to level 1 connections
	_dian_adtrees[index / ADTREE_CNT]->setInPort(src, index % ADTREE_CNT);
}

// tick
void
dian_nfu2::tick(_TICK curTick, int calcSet, bool stall_NBOut)
{
	// Opcode propagation
	_s4_Op = _s3_Op;
	_s3_Op = _s2_Op;
	_s2_Op = _s1_Op;

	// calcSet buffer propagation
	_calcSet_buffer2 = _calcSet_buffer1;
	_calcSet_buffer1 = calcSet;

	// switch ports
	if (_calcSet_buffer2 > 0)
	{
		switch_port_from_nbout(_calcSet_buffer2);
	}

	// Update stage 1 Opcode.
	_s1_Op._nfu2_setnum = _calcSet_buffer2;

	if (!stall_NBOut)
	{
		// Adder tree tick
		int i;
		for (i = 0; i < ADTREE_CNT; ++i)
			_dian_adtrees[i]->tick(curTick);

	}

	// Update stage 4 Opcode
	_calcSet = _s4_Op._nfu2_setnum;

	if (!_quite_calcSet)
		cout << "[NFU-2] " << dec << _calcSet_buffer2 << " sets are inserted." << endl;
	if (!_quite_nfu2)
		cout << "[NFU-2] current Position(In: " << _s1_Op._cur_input_cnt << " ,Out:" << _s1_Op._cur_output_cnt
		<< " ,X:" << _s1_Op._cur_X << " ,Y:" << _s1_Op._cur_Y << ")" << endl;
}

// connect to nfu3
void
dian_nfu2::connect_to_nfu3(dian_nfu3 *target_nfu3)
{
	// validation check
	if (target_nfu3 == NULL)
	{
		cout << "ERROR:[NFU-2] No NFU-3 is connected." << endl;
		return;
	}
	if (NFU3_WIDTH != ADTREE_CNT)
	{
		cout << "ERROR: [NFU-2] Size of adder tree is not match with NFU-3 width." << endl;
		return;
	}

	int i;
	for (i = 0; i < NFU3_WIDTH; ++i)
	{
		target_nfu3->setInPorts(_dian_adtrees[i]->getExitModule(), i);
	}

	cout << "[NFU-2] Connected with NFU-3 done." << endl;
}

// connect to nbout
void
dian_nfu2::connect_to_nbout(dian_nbout* target_nbout)
{
	if (target_nbout == NULL)
	{
		cout << "ERROR:[NFU-2] No NBOut is connected." << endl;
		return;
	}

	// connect
	conn_nbout = target_nbout;

	// do no set ports
}

// switch backward connection from nbout
void
dian_nfu2::switch_port_from_nbout(int calcSet)
{
	int i;
	for (i = 0; i < calcSet; ++i)
	{
		// get backward connection
		if (_s1_Op._cur_input_cnt > 0)
		{
			setInPorts(conn_nbout->getReg(_s1_Op._cur_Y * _s1_Op._output_width + _s1_Op._cur_X)
				, (i * ADTREE_CNT) + (_s1_Op._kernel_size));
		}

		// move each position in output feature map
		_s1_Op._cur_X++;
		if (_s1_Op._cur_X == _s1_Op._input_width)
		{
			_s1_Op._cur_X = 0;
			_s1_Op._cur_Y++;
			if (_s1_Op._cur_Y == _s1_Op._input_width)
			{
				_s1_Op._cur_Y = 0;
				// move to next propagation feature map.
				move_to_next_kernel();
			}
		}
	}

	if (!_quite_nfu2)
		cout << "[NFU-2] connecting with NFU-3 is done." << endl;
}

// increase kernel count by one and update opcode.
void
dian_nfu2::move_to_next_kernel()
{
	_s1_Op._cur_kernel_cnt++;
	_s1_Op._cur_output_cnt++;
	// move to next input
	if (_s1_Op._cur_output_cnt == _s1_Op._max_output_cnt)
	{
		_s1_Op._cur_output_cnt = 0;
		_s1_Op._cur_input_cnt++;

		if (_s1_Op._cur_input_cnt == _s1_Op._max_input_cnt)
		{
			//TODO: end
		}
	}
}
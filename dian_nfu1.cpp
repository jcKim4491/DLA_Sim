#include "dian_nfu1.h"

// Constructors
dian_nfu1::dian_nfu1()
{
	// REG size 16bit
	nfu1_mults = new m_mult*[_MULT_MODULE_CNT];
	int i = 0;
	string m_mult_name;
	for (i = 0; i < _MULT_MODULE_CNT; ++i)
	{
		m_mult_name = string("NFU1_MULT_");
		m_mult_name.append(to_string(i));
		nfu1_mults[i] = new m_mult(m_mult_name.c_str(), 16);
	}

	cout << "NFU-1 module is created." << endl;
}

dian_nfu1::dian_nfu1(Diannao_system *const dian_sys, int MULT_MODULE_CNT)
	: _dian_sys(dian_sys), _MULT_MODULE_CNT(MULT_MODULE_CNT)
{
	// REG size 16bit
	nfu1_mults = new m_mult*[_MULT_MODULE_CNT];
	int i = 0;
	string m_mult_name;
	for (i = 0; i < _MULT_MODULE_CNT; ++i)
	{
		m_mult_name = string("NFU1_MULT_");
		m_mult_name.append(to_string(i));
		nfu1_mults[i] = new m_mult(m_mult_name.c_str(), 16);
	}

	// Copy Opcode
	_curOp = dian_sys->_curOp;

	cout << "NFU-1 module is created." << endl;
}

// Destructor
dian_nfu1::~dian_nfu1()
{
	int i;
	for (i = 0; i < _MULT_MODULE_CNT; ++i)
	{
		delete nfu1_mults[i];
	}
	delete[] nfu1_mults;

	cout << "NFU-1 module is deleted." << endl;
}

m_mult*
dian_nfu1::getM_MUlT(int index)
{
	// Assume that nfu1_mults is not NULL

	if (_MULT_MODULE_CNT < index || index < 0)
	{
		cout << "ERROR: Invalid m_mult index(" << index << ") in NFU-1." << endl;
		return NULL;
	}

	return nfu1_mults[index];
}

// tick
void
dian_nfu1::tick(_TICK curTick, bool calcAvail, int calcSet, bool stall_from_nbout, bool nbout_filling, bool nbin_stall)
{
	if (!_quite_nfu1)
		cout << "calcAvail: " << calcAvail << ", calcSet: " << calcSet << ", stall_from_nbout: " << stall_from_nbout << ", nbout_filling: " << nbout_filling << ", nbin_stall: " << nbin_stall << endl;

	if (_confirm_nbin && _confirm_sb)
	{
		_data_avail = true;
	}
	if (nbout_filling)
	{
		if (!_quite_nfu1)
			cout << "[NFU-1] Stalled by NBOut. (Loading previous data)" << endl;
		return;
	}

	// calculaton only available
	if (_data_avail == true)
	{
		if ((calcAvail == false || calcSet == 0) && !_calculating)
		{
			_calc_done = false;
			if (!_quite_nfu1)
				cout << "[NFU-1] Calculation is not available." << endl;
			return;
		}

		// stall by nbout
		if (stall_from_nbout)
		{
			if (!_quite_nfu1)
				cout << "[NFU-1] Stalled by NBOut." << endl;
			return;
		}

		// not calculating right now.
		if (_calculating == false)
		{
			// if nbin is stalled, then wait for next calculation.
			if (nbin_stall)
			{
				_calc_done = false;
				_clk_s = 0;
				_cur_calcSet = calcSet;
				_data_avail = false;
			}
			else
			{
				_calculating = true;
				_calc_done = false;
				_clk_s = 0;

				_cur_calcSet = calcSet;
			}
		}
		else
		{
			_clk_s++;
			if (_clk_s == _clk_end)
			{
				_clk_s = 0;
				_calc_done = true;
				switch_port_to_nfu2(_calcSet_buffer);
			}
		}

		//Do your work
		calculating(curTick);

		// calculating done
		if (_calc_done == true)
		{
			// if calculable stuff left
			if (_calcSet_buffer == 0)
			{
				// check availability again.
				if (_confirm_nbin && _confirm_sb)
					_data_avail = true;
				else
					_data_avail = false;

				// reset
				_confirm_nbin = false;
				_confirm_sb = false;
			}

			if (!_quite_nfu1)
			{
				cout << "[NFU-1] current Position(In: " << _curOp._cur_input_cnt
					<< ", Out: " << _curOp._cur_output_cnt << ", X: " << _curOp._cur_X << ", Y: "
					<< _curOp._cur_Y << ")" << endl;
				cout << "[NFU-1] Multiplies done." << endl;
			}
			_calculating = false;
		}
	}
	else
	{
		// reset cur_calcSet
		_cur_calcSet = 0;
		if (!_quite_nfu1)
			cout << "[NFU-1] Stalled." << endl;
	}

	_calcSet_buffer = calcSet;
}

// Feed from NBIN
void
dian_nfu1::feed_from_nbin()
{
	_confirm_nbin = true;
}

// Feed from SB
void
dian_nfu1::feed_from_sb()
{
	_confirm_sb = true;
}

// Blocking from NBIn
void
dian_nfu1::block_from_nbin()
{
	_confirm_nbin = false;
}

// Blocking from SB
void
dian_nfu1::block_from_sb()
{
	_confirm_sb = false;
}

// calculating
void
dian_nfu1::calculating(_TICK curTick)
{
	// ticking every mult_modules in NFU-1
	int i;
	for (i = 0; i < _MULT_MODULE_CNT; ++i)
	{
		if (_clk_s == 0)
			nfu1_mults[i]->tick(curTick);
		else
			nfu1_mults[i]->tick(curTick, false);
	}

	if (!_quite_moduleconnection)
		cout << "[NFU-1] Connected with NFU-2 done." << endl;
}

// connect to nfu-2
void
dian_nfu1::connect_to_nfu2(dian_nfu2* target_nfu2)
{
	if (target_nfu2 == NULL)
	{
		cout << "ERROR:[NFU-1] No NFU-2 is connected." << endl;
		return;
	}

	// connect
	conn_nfu2 = target_nfu2;

	// set porting
	int i;
	for (i = 0; i < _MULT_MODULE_CNT; ++i)
	{
		target_nfu2->setInPorts(nfu1_mults[i], i);
	}
}

// switch connection
void
dian_nfu1::switch_port_to_nfu2(int calcSet)
{
	// Porting only available set.

	int set = 0;
	int i = 0, k = 0;
	int kern_size = _curOp._kernel_width * _curOp._kernel_width;

	// Set ports
	for (i = 0; i < _MULT_MODULE_CNT; ++i)
	{
		// port with NFU1 mult_modules
		if (set < calcSet)
		{
			// NBOut merge needed.
			if (_curOp._cur_input_cnt > 0)
			{
				// TODO: handle with oversized kernels.
				if (i % ADTREE_CNT < kern_size)
				{
					conn_nfu2->setInPorts(nfu1_mults[k], i);
					++k;
				}
				else
					conn_nfu2->setInPorts(_dian_sys->_zero_reg, i);
			}
			// Non NBOut merging.
			else
			{
				if (i % ADTREE_CNT < kern_size)
				{
					conn_nfu2->setInPorts(nfu1_mults[k], i);
					++k;
				}
				else
					conn_nfu2->setInPorts(_dian_sys->_zero_reg, i);
			}
		}
		else
			conn_nfu2->setInPorts(_dian_sys->_zero_reg, i);


		if ((i + 1) % ADTREE_CNT == 0)
		{
			// update Opcode
			if (set < calcSet)
			{
				_curOp._cur_X++;
				if (_curOp._cur_X == _curOp._input_width)
				{
					_curOp._cur_X = 0;
					_curOp._cur_Y++;

					if (_curOp._cur_Y == _curOp._input_width)
					{
						_curOp._cur_Y = 0;
						//move to the next input feature map.
						move_to_next_kernel();
					}
				}
			}

			++set;
		}
	}

	if (!_quite_nfu1)
		cout << "[NFU-1] Connecting with NFU-2 is done. " << endl;

	return;
}

// increase kernel count by one and update opcode.
void
dian_nfu1::move_to_next_kernel()
{
	_curOp._cur_kernel_cnt++;
	_curOp._cur_output_cnt++;
	// move to next input
	if (_curOp._cur_output_cnt == _curOp._max_output_cnt)
	{
		_curOp._cur_output_cnt = 0;
		_curOp._cur_input_cnt++;

		if (_curOp._cur_input_cnt == _curOp._max_input_cnt)
		{
			//TODO: end
		}
	}
}
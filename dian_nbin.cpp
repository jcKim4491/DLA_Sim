#include "dian_nbin.h"


// Constructors
dian_nbin::dian_nbin()
{
	// allocate nbin regs.
	_nbin_regs = new m_reg*[_NBIN_REG_CNT];

	int i;
	string reg_name;

	for (i = 0; i < _NBIN_REG_CNT; ++i)
	{
		reg_name = string("NBIN_");
		reg_name.append(to_string(i));

		_nbin_regs[i] = new	m_reg(reg_name.c_str(), _diannao_bitsize);

		reg_name.clear();
	}

	cout << "DIANNAO NB_In is created." << endl;
}

dian_nbin::dian_nbin(Diannao_system *const dian_sys, int NBIN_REG_CNT)
	: _dian_sys(dian_sys), _NBIN_REG_CNT(NBIN_REG_CNT)
{
	// allocate nbin regs.
	_nbin_regs = new m_reg*[NBIN_REG_CNT];

	int i;
	string reg_name;

	for (i = 0; i < NBIN_REG_CNT; ++i)
	{
		reg_name = string("NBIN_");
		reg_name.append(to_string(i));

		_nbin_regs[i] = new	m_reg(reg_name.c_str(), _diannao_bitsize);

		reg_name.clear();
	}

	// Copy Opcode
	_curOp = dian_sys->_curOp;

	cout << "DIANNAO NB_In is created." << endl;
}

// destructor
dian_nbin::~dian_nbin()
{
	//remove registers
	int i;
	for (i = 0; i < _NBIN_REG_CNT; ++i)
	{
		delete _nbin_regs[i];
	}
	delete[] _nbin_regs;
}


// port connect
void dian_nbin::connect_to_nfu1(dian_nfu1 * target_nfu1)
{
	// check port numbers are same.
	if (_NBIN_REG_CNT % target_nfu1->getMULT_MODDULE_CNT() != 0)
	{
		cout << "ERROR:[NBIN]The number of ports in NBIN and NFU-1 are matched.(nbin: "
			<< _NBIN_REG_CNT << ", nfu-1: " << target_nfu1->getMULT_MODDULE_CNT()
			<< ")" << endl;
		return;
	}

	// connect module
	conn_nfu1 = target_nfu1;

	switch_port_to_nfu1(false, 0);

	cout << "[NBIN] Connected with NFU-1 done." << endl;
}

// port switching
void
dian_nbin::switch_port_to_nfu1(bool mult_using, int sb_data_avail)
{
	if (conn_nfu1 == NULL)
	{
		cout << "ERROR:[NBIN] No NFU-1 is connected." << endl;
		return;
	}
	if (mult_using)
	{
		if (!_quite_nbin)
			cout << "[NBIN] Stalled by NFU-1 stage." << endl;
		return;
	}

	int max_mult_cnt = conn_nfu1->getMULT_MODDULE_CNT();

	// find right ports
	int tar_port = 0;
	int sb_tar_port = 0;

	int k = 0;
	_calc_set = 0;
	int kx = _curOp._cur_kX;
	int ky = _curOp._cur_kY;
	int cx = _curOp._cur_X;	// current position X in output feature map
	int cy = _curOp._cur_Y;	// current position Y in output feature map
	int padding_marginX = _curOp._padding_num;
	int padding_marginY = _curOp._padding_num;
	int padding_num = _curOp._padding_num;

	bool zp_left, zp_right, zp_upper, zp_lower;

	_calc_avail = false;
NEXT_POS:


	// check data availability
	if (_curOp._zero_padding == false)
	{
		tar_port = (cy + _curOp._kernel_width) * _curOp._input_width + (cx + _curOp._kernel_width);
		if (tar_port > _data_avail)
		{
			// not available.
			if (!_quite_nbin)
				cout << "[NBIn] No more available data. " << endl;
			goto QUIT_PORTING;
		}
		else
		{
			if (!_quite_nbin)
				cout << "[NBIn] current output pos(X: " << dec << cx << ", Y: " << cy
				<< ", kX: " << kx << ", kY: " << ky << ")" << endl;
		}
	}
	// zero padding check for a fixed output position.
	else
	{
		zp_left = false;
		zp_right = false;
		zp_upper = false;
		zp_lower = false;

		// find padding margin
		// left side
		if (cx < padding_num)
		{
			padding_marginX = padding_num - cx;
			zp_left = true;
		}
		// right side
		else if (cx >= _curOp._input_width - padding_num)
		{
			padding_marginX = cx - (_curOp._input_width - padding_num) + 1;
			zp_right = true;
		}
		else
			padding_marginX = 0;

		// upper side
		if (cy < padding_num)
		{
			padding_marginY = padding_num - cy;
			zp_upper = true;
		}
		// lower side
		else if (cy >= _curOp._input_width - padding_num)
		{
			padding_marginY = cy - (_curOp._input_width - padding_num) + 1;
			zp_lower = true;
		}
		else
			padding_marginY = 0;

		// check data availability
		tar_port = min((cy + _curOp._kernel_width - padding_marginY), _curOp._input_width - 1) * _curOp._input_width
			+ min((cx + _curOp._kernel_width - padding_marginX), _curOp._input_width - 1);

		// kernel data availability
		sb_tar_port = (((_curOp._cur_input_cnt * _curOp._max_output_cnt) + _curOp._cur_output_cnt) + 1) * _curOp._kernel_size;

		if (tar_port > _data_avail)
		{
			// not available.
			if (!_quite_nbin)
				cout << "[NBIn] No more available data. " << endl;
			goto QUIT_PORTING;
		}
		else if (sb_tar_port > sb_data_avail)
		{
			// not available.
			if (!_quite_sb)
				cout << "[SB] Kernel data is not ready. " << endl;
			goto QUIT_PORTING;
		}
		else
		{
			if (!_quite_nbin)
				cout << "[NBIn] current output pos(X: " << dec << cx << ", Y: " << cy
				<< ", kX: " << kx << ", kY: " << ky << ")" << endl;
		}
	}

	// whole kernel can be calculated.
	if (_curOp._kernel_width * _curOp._kernel_width > (max_mult_cnt - k))
	{

		goto QUIT_PORTING;
	}

	// calculation avail.
	_calc_avail = true;

	// Porting
	for (ky = _curOp._cur_kY; ky < _curOp._kernel_width; ++ky)
	{
		for (kx = _curOp._cur_kX; kx < _curOp._kernel_width; ++kx)
		{
			// zero padding?
			if (_curOp._zero_padding)
			{
				if (zp_left && (kx < padding_marginX))
					conn_nfu1->getM_MUlT(k)->setInPort1(_dian_sys->_zero_reg);
				else if (zp_right && (kx >= _curOp._kernel_width - padding_marginX))
					conn_nfu1->getM_MUlT(k)->setInPort1(_dian_sys->_zero_reg);
				else if (zp_upper && (ky < padding_marginY))
					conn_nfu1->getM_MUlT(k)->setInPort1(_dian_sys->_zero_reg);
				else if (zp_lower && (ky >= _curOp._kernel_width - padding_marginY))
					conn_nfu1->getM_MUlT(k)->setInPort1(_dian_sys->_zero_reg);
				else
				{
					// normal port with padding
					tar_port = (_curOp._cur_input_cnt * (_curOp._input_width * _curOp._input_width)) % BUFFER_SIZE;
					tar_port += (cy + ky - padding_num) * _curOp._input_width
						+ (cx + kx - padding_num);
					conn_nfu1->getM_MUlT(k)->setInPort1(_nbin_regs[tar_port]);
					_nbin_regs[tar_port]->setOutPort(conn_nfu1->getM_MUlT(k));
				}

			}
			else
			{
				// normal port without padding
				tar_port = (_curOp._cur_input_cnt * (_curOp._input_width * _curOp._input_width)) % BUFFER_SIZE;
				tar_port += (cy + ky) * _curOp._input_width
					+ (cx + kx);
				conn_nfu1->getM_MUlT(k)->setInPort1(_nbin_regs[tar_port]);
				_nbin_regs[tar_port]->setOutPort(conn_nfu1->getM_MUlT(k));
			}

			_re_porting = false;

			++k;
			// if maximum mult cnt reach,
			if (k == max_mult_cnt)
				goto QUIT_PORTING;
		}
	}

	// number of calculable kernel set
	_calc_set++;
	// have more room to port.
	// moved to next output position
	cx++;
	if (cx == _curOp._input_width)
	{
		cx = 0;
		cy++;
		if (cy == _curOp._input_width)
		{
			cy = 0;
			kx = 0;
			ky = 0;

			// end this output feature map.
			_one_layer_end = true;

			goto QUIT_PORTING;
		}
	}

	_curOp._cur_kX = kx = 0;
	_curOp._cur_kY = ky = 0;

	// maximum set limit
	if (_calc_set >= ADTREE_CNT)
	{
		goto QUIT_PORTING;
	}
	goto NEXT_POS;

QUIT_PORTING:
	if (!_quite_nbin) {
		cout << "[NBIn] (end)current output pos(X: " << dec << cx << ", Y: " << cy
			<< ", kX: " << kx << ", kY: " << ky << ")" << endl;
	}

	// update parameters
	_curOp._cur_X = cx;
	_curOp._cur_Y = cy;
	_curOp._cur_kX = kx;
	_curOp._cur_kY = ky;
	return;
}

// feed from memory
void
dian_nbin::feed_from_mem(int idx, _DATA_STORAGE data)
{
	if (_nbin_regs == NULL)
	{
		return;
	}

	// force feeding
	_nbin_regs[idx]->foreced_feed(data);
	_data_avail = idx; //assume that input is iterative.

	if (!_quite_dataflow)
	{
		cout << "[NBIN] Reg_" << dec << idx << " is fed with " << hex << data << endl;
	}
}

// get current row to be filled.
int
dian_nbin::getCurrentRow()
{
	// should not over _NBIN_ENTRY_CNT.
	_cur_row = _cur_row % _NBIN_ENTRY_CNT;
	// return the value and increase.
	return _cur_row;
}

// Increase current row index by one.
void
dian_nbin::incCurrentRow()
{
	_cur_row = (_cur_row + 1) % _NBIN_ENTRY_CNT;
}

// tick
void
dian_nbin::tick(_TICK curTick, bool mult_done, bool nbout_stall, bool nbout_readdone, int sb_data_avail)
{
	if (_wait_nbout_filling)
	{
		_calc_avail = false;

		if (nbout_readdone == false)
		{
			//waiting
			if (!_quite_nbin)
				cout << "[NBIN] Stalled by NBout re-filling" << endl;
			return;
		}
		else
		{
			_wait_nbout_filling = false;
			// no more waiting
			_re_porting = true;
		}
	}

	if (conn_nfu1->get_mult_done())
		_re_porting = true;

	if (nbout_stall)
	{
		if (!_quite_nbin)
			cout << "[NBIN] Stalled by NBout writeback." << endl;
	}
	else if (_wait_next_output)
	{
		_calc_avail = false;
		_calc_set = 0;
		// block nfu1
		conn_nfu1->block_from_nbin();
		if (!_quite_nbin)
			cout << "[NBIN] Blocked for next output." << endl;
	}
	else if (_data_avail)
	{
		if (_re_porting)
		{
			switch_port_to_nfu1(mult_done, sb_data_avail);
		}

		feed_nfu1();
	}

	if (!_quite_nbin)
		cout << "[NBIN] reporting :" << _re_porting << ", data_avail: " << _data_avail << endl;
}

// feed NFU-1
void
dian_nbin::feed_nfu1()
{
	conn_nfu1->feed_from_nbin();
}

// continue process
void
dian_nbin::continue_process()
{
	_wait_next_output = false;
}

// increase kernel count by one and update opcode.
void
dian_nbin::move_to_next_kernel()
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

	if (_curOp._cur_input_cnt > 0)
	{
		// wait for filling nbout with previous data
		_wait_nbout_filling = true;
	}
}

// print
void
dian_nbin::print()
{
	if (_nbin_regs == NULL)
		return;

	cout << "<NBIN " << _NBIN_WIDTH << "X" << _NBIN_ENTRY_CNT << ">" << endl;

	for (int i = 0; i < _NBIN_ENTRY_CNT; ++i)
	{
		for (int j = 0; j < _NBIN_WIDTH; ++j)
		{
			cout << hex << (_nbin_regs[i * BUFFER_WIDTH + j]->getData() & 0xFFFF) << "\t";
		}
		cout << endl;
	}
}
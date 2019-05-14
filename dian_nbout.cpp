#include "dian_nbout.h"


// Constructors
dian_nbout::dian_nbout()
{
	// allocate nbin regs.
	_nbout_regs = new m_reg*[_NBOUT_REG_CNT];

	int i;
	string reg_name;

	for (i = 0; i < _NBOUT_REG_CNT; ++i)
	{
		reg_name = string("NBIN_");
		reg_name.append(to_string(i));

		_nbout_regs[i] = new m_reg(reg_name.c_str(), _diannao_bitsize);

		reg_name.clear();
	}

	cout << "DIANNAO NB_In is created." << endl;
}

dian_nbout::dian_nbout(Diannao_system *const dian_sys, int NBOUT_REG_CNT)
	: _dian_sys(dian_sys), _NBOUT_REG_CNT(NBOUT_REG_CNT)
{
	// allocate nbin regs.
	_nbout_regs = new m_reg*[NBOUT_REG_CNT];

	int i;
	string reg_name;

	for (i = 0; i < NBOUT_REG_CNT; ++i)
	{
		reg_name = string("NBIN_");
		reg_name.append(to_string(i));

		_nbout_regs[i] = new m_reg(reg_name.c_str(), _diannao_bitsize);

		reg_name.clear();
	}

	// Copy Opcode
	_curOp = dian_sys->_curOp;
	_saveOp.nbout_p = this;
	_saveOp.output_size = _curOp._output_width  *_curOp._output_width;

	cout << "DIANNAO NBOut is created." << endl;
}

// Destructor
dian_nbout::~dian_nbout()
{
	int i;
	for (i = 0; i < _NBOUT_REG_CNT; ++i)
		delete _nbout_regs[i];

	delete[] _nbout_regs;

	cout << "DIANNAO NBOut is deleted." << endl;
}

// tick
void
dian_nbout::tick(_TICK curTick, int calcSet, bool write_done)
{
	// Save in Memory
	if (_saveOp.saving)
	{
		// write back is done.
		if (write_done)
		{
			// end stall
			_stalled = false;

			// reset
			_saveOp.saving = false;
			_saveOp.size = 0;
			_saveOp.start_index = 0;

			if (!_quite_writeback)
				cout << "[NBOut] Write back is done, now continue the pipe-line." << endl;
		}
	}
	// Save in Buffer
	else
	{
		int i;
		if (_curOp._nbout_setnum > 0)
		{
			for (i = 0; i < _curOp._nbout_setnum; ++i)
			{
				_nbout_regs[_start_index + i]->tick(curTick, false);
			}

			// update start_index
			_start_index = _start_index + _curOp._nbout_setnum;

			// reset
			_curOp._nbout_setnum = 0;
		}
		else
			_curOp._nbout_setnum = 0;

		if (!_quite_nbout)
			cout << "[NBOut] start index is " << dec << _start_index << ", calcSet: " << calcSet << endl;


		// when one output layer completed,
		// it needs to be saved in memory
		if (_start_index >= _curOp._output_width * _curOp._output_width)
		{
			if (!_quite_nbout)
				cout << "[NBOut] Buffer is full. now start to writeback to memory." << endl;
			// stall the pipelines
			_stalled = true;

			// save
			_saveOp.saving = true;
			_saveOp.start_index = 0;
			_saveOp.size = _curOp._output_width * _curOp._output_width;
			_saveOp.output_num = _curOp._cur_output_cnt;

			// move to next output
			_curOp._cur_output_cnt++;
			if (_curOp._cur_output_cnt == _curOp._max_output_cnt)
			{
				_curOp._cur_output_cnt = 0;
			}

			// reset
			_start_index = 0;
		}

		if (calcSet > 0)
		{
			_curOp._nbout_setnum = calcSet;

			for (i = 0; i < calcSet; ++i)
			{
				_nbout_regs[_start_index + i]->tick(curTick);
			}
		}
	}
}

// get Register
m_reg *
dian_nbout::getReg(int index)
{
	if (_nbout_regs == NULL)
		return NULL;
	if (index < 0 || index >= _NBOUT_REG_CNT)
		return NULL;

	return _nbout_regs[index];
}

// feed from memory
void
dian_nbout::feed_from_mem(int idx, _DATA_STORAGE data)
{
	if (_nbout_regs == NULL)
	{
		return;
	}

	// force feeding
	_nbout_regs[idx]->foreced_feed(data);

	if (!_quite_dataflow)
	{
		cout << "[NBOUT] Reg_" << dec << idx << " is fed with " << hex << data << endl;
	}
}

void
dian_nbout::print()
{
	if (_nbout_regs == NULL)
		return;

	cout << "<NBOUT " << NBOUT_WIDTH << "X" << _NBOUT_ENTRY_CNT << ">" << endl;

	for (int i = 0; i < _NBOUT_ENTRY_CNT; ++i)
	{
		for (int j = 0; j < NBOUT_WIDTH; ++j)
		{
			cout << hex << (_nbout_regs[i * NBOUT_WIDTH + j]->getData() & 0xFFFF) << "\t";
		}
		cout << endl;
	}
}
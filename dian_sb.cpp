#include "dian_sb.h"

dian_sb::dian_sb()
{
	// allocate nbin regs.
	_sb_regs = new m_reg*[_SB_REG_CNT];

	int i;
	string reg_name;

	for (i = 0; i < _SB_REG_CNT; ++i)
	{
		reg_name = string("SB_");
		reg_name.append(to_string(i));

		_sb_regs[i] = new m_reg(reg_name.c_str(), _diannao_bitsize);

		reg_name.clear();
	}

	cout << "DIANNAO SB is created." << endl;
}

dian_sb::dian_sb(Diannao_system *const dian_sys, int SB_REG_CNT)
	: _dian_sys(dian_sys), _SB_REG_CNT(SB_REG_CNT)
{
	// allocate nbin regs.
	_sb_regs = new m_reg*[_SB_REG_CNT];

	int i;
	string reg_name;

	for (i = 0; i < _SB_REG_CNT; ++i)
	{
		reg_name = string("SB_");
		reg_name.append(to_string(i));

		_sb_regs[i] = new m_reg(reg_name.c_str(), _diannao_bitsize);

		reg_name.clear();
	}

	// Copy Opcode
	_curOp = dian_sys->_curOp;

	cout << "DIANNAO SB is created." << endl;
}

// destructor
dian_sb::~dian_sb()
{
	//remove registers
	int i;
	for (i = 0; i < _SB_REG_CNT; ++i)
	{
		delete _sb_regs[i];
	}
	delete[] _sb_regs;
}

// port connect
void dian_sb::connect_to_nfu1(dian_nfu1 * target_nfu1)
{
	int mult_module_cnt = target_nfu1->getMULT_MODDULE_CNT();
	int nto1 = _SB_REG_CNT / mult_module_cnt;
	// check port numbers are same.
	if (_SB_REG_CNT % mult_module_cnt != 0)
	{
		cout << "ERROR: The number of ports in SB should be some multiple of the number of ports in nfu-1.(sb: "
			<< _SB_REG_CNT << ", nfu-1: " << mult_module_cnt
			<< ")" << endl;
		return;
	}

	// connect modules
	conn_nfu1 = target_nfu1;

	switch_port_to_nfu1();
}

void
dian_sb::switch_port_to_nfu1()
{
	int i, k;
	int max_mult_cnt = conn_nfu1->getMULT_MODDULE_CNT();
	int kern_base = _curOp._cur_kernel_cnt * _curOp._kernel_size;

	// local kernel index k
	k = (_curOp._cur_kY * _curOp._kernel_width) + _curOp._cur_kX;
	for (i = 0; i < max_mult_cnt; ++i)
	{
		conn_nfu1->getM_MUlT(i)->setInPort2(_sb_regs[(kern_base + k) % _SB_REG_CNT]);
		_sb_regs[(kern_base + k) % _SB_REG_CNT]->setOutPort(conn_nfu1->getM_MUlT(i));

		// kernel weight reuse
		++k;
		if (k >= _curOp._kernel_size)
			k = 0;
	}
}

// feed from memory
void
dian_sb::feed_from_mem(int idx, _DATA_STORAGE data)
{
	if (_sb_regs == NULL)
	{
		return;
	}

	// force feeding
	_sb_regs[idx]->foreced_feed(data);
	_data_avail++;

	if (!_quite_dataflow)
	{
		cout << "[SB] Reg_" << dec << idx << " is fed with " << hex << data << endl;
	}
}

// get current row to be filled.
int
dian_sb::getCurrentRow()
{
	// should not over _SB_ENTRY_CNT.
	_cur_row = _cur_row % (_SB_ENTRY_CNT * BUFFER_WIDTH);
	// return the value and increase.
	return _cur_row;
}

// Increase current row index by one.
void
dian_sb::incCurrentRow()
{
	_cur_row = (_cur_row + 1) % (_SB_ENTRY_CNT * BUFFER_WIDTH);
}

// increase kernel count by one and update opcode.
void
dian_sb::move_to_next_kernel()
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

	// switch ports
	switch_port_to_nfu1();
}

// tick
void
dian_sb::tick(_TICK curTick)
{
	if (_data_avail)
	{
		feed_nfu1();
	}
}

// feed NFU-1
void
dian_sb::feed_nfu1()
{
	conn_nfu1->feed_from_sb();
}
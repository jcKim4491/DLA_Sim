#include "dian_nfu3.h"


// Constructors
dian_nfu3::dian_nfu3()
{
}

dian_nfu3::dian_nfu3(Diannao_system *const dian_sys)
	: _dian_sys(dian_sys)
{
	_actfunc_modules = new act_module*[NFU3_WIDTH];
	int i;
	for (i = 0; i < NFU3_WIDTH; ++i)
	{
		_actfunc_modules[i] = new act_module((string("NFU3_ACTM_") + to_string(i)).c_str());
	}

	// Copy Opcode
	_curOp = dian_sys->_curOp;
	_delayedOp = _curOp;

	cout << "NFU-3 module is created." << endl;
}

// Destructor
dian_nfu3::~dian_nfu3()
{
	int i;
	for (i = 0; i < NFU3_WIDTH; ++i)
		delete _actfunc_modules[i];
	delete[] _actfunc_modules;
	cout << "NFU-3 module is deleted." << endl;
}

// Set input ports
void
dian_nfu3::setInPorts(timingObject* src, int index)
{
	// validation check
	if (src == NULL)
		return;
	if (index < 0 || index >= NFU3_WIDTH)
		return;

	_actfunc_modules[index]->setInPort(src);
}

// tick
void
dian_nfu3::tick(_TICK curTick, int calcSet)
{
	if (_actfunc_modules == NULL)
		return;

	// update
	if (_delayedOp._nfu3_setnum > 0)
	{
		// opcode update
		int i;
		for (i = 0; i < _delayedOp._nfu3_setnum; ++i)
		{
			_delayedOp._cur_X++;
			if (_delayedOp._cur_X == _delayedOp._output_width)
			{
				_delayedOp._cur_X = 0;
				_delayedOp._cur_Y++;
				if (_delayedOp._cur_Y == _delayedOp._output_width)
				{
					_delayedOp._cur_Y = 0;

					//move to next propagation feature map.
					move_to_next_kernel();
				}
			}
		}

		_calcSet = _delayedOp._nfu3_setnum;
		if (!_quite_calcSet)
			cout << "[NFU-3] " << dec << _delayedOp._nfu3_setnum << " sets are now propagating." << endl;

		// reset
		_delayedOp._nfu3_setnum = 0;
	}
	else
		_calcSet = 0;

	// switch port
	if (_curOp._nfu3_setnum > 0)
	{
		//print();
		// switch port
		switch_port_to_nbout(_delayedOp._cur_Y * _delayedOp._input_width + _delayedOp._cur_X);

		int i;
		for (i = 0; i < NFU3_WIDTH; ++i)
			_actfunc_modules[i]->tick(curTick);
	}

	// propagate
	_delayedOp._nfu3_setnum = _curOp._nfu3_setnum;

	//if (_delayedOp._cur_input_cnt > 0 && _delayedOp._cur_output_cnt == 0)
	//{
	//	_curOp._nfu3_setnum = _calcSet_buffer1;
	//	_calcSet_buffer1 = calcSet;
	//}
	//else
	//{
	_curOp._nfu3_setnum = calcSet;
	//_calcSet_buffer1 = 0;
	//}

	if (calcSet > 0)
	{
		int i;
		for (i = 0; i < NFU3_WIDTH; ++i)
			_actfunc_modules[i]->tick(curTick);
	}

	if (!_quite_calcSet)
		cout << "[NFU-3] " << dec << calcSet << " sets are inserted." << endl;
}

// connection to nbout
void
dian_nfu3::connection_to_nbout(dian_nbout *target_nbout)
{
	// valdiation check
	if (target_nbout == NULL)
	{
		cout << "ERROR:[NFU-3] No NBOut is connected." << endl;
		return;
	}
	if (NFU3_WIDTH != NBOUT_WIDTH)
	{
		cout << "ERROR:[NFU-3] The number of ports in NFU-3 and NBOut are matched.(nfu-3: "
			<< NFU3_WIDTH << ", nbout: " << NBOUT_WIDTH << ")" << endl;
		return;
	}

	// connect
	conn_nbout = target_nbout;

	// switch port
	switch_port_to_nbout();

	if (!_quite_moduleconnection)
		cout << "[NFU-3] Connected with NBOut done." << endl;
}

// switch port
void
dian_nfu3::switch_port_to_nbout()
{
	int tar_row = conn_nbout->getCurrentRow();

	int i;
	for (i = 0; i < NFU3_WIDTH; ++i)
	{
		// connect iteratively
		_actfunc_modules[i]->setOutPort(conn_nbout->getReg((tar_row * NFU3_WIDTH) + i));
		conn_nbout->getReg((tar_row * NFU3_WIDTH) + i)->setInPort(_actfunc_modules[i]);
	}
}

void
dian_nfu3::switch_port_to_nbout(int start_index)
{
	int i;
	for (i = 0; i < NFU3_WIDTH; ++i)
	{
		if (start_index + i >= BUFFER_SIZE)
			return;

		// connect iteratively
		_actfunc_modules[i]->setOutPort(conn_nbout->getReg(start_index + i));
		conn_nbout->getReg(start_index + i)->setInPort(_actfunc_modules[i]);
	}
}

// increase kernel count by one and update opcode.
void
dian_nfu3::move_to_next_kernel()
{
	_delayedOp._cur_kernel_cnt++;
	_delayedOp._cur_output_cnt++;
	// move to next input
	if (_delayedOp._cur_output_cnt == _delayedOp._max_output_cnt)
	{
		_delayedOp._cur_output_cnt = 0;
		_delayedOp._cur_input_cnt++;

		// Apply activation function only last input.
		if (_delayedOp._cur_input_cnt == _delayedOp._max_input_cnt - 1)
		{
			set_activation_all(_delayedOp._actfunc);
		}

		if (_delayedOp._cur_input_cnt == _delayedOp._max_input_cnt)
		{
			//TODO: end
		}
	}
}

// set activation for all modules
void
dian_nfu3::set_activation_all(ACTFUNC actfunc)
{
	int i;
	for (i = 0; i < NFU3_WIDTH; ++i)
	{
		_actfunc_modules[i]->setActFunc(actfunc);
	}

	cout << "[NFU-3] Activation Function is now on." << endl;
}

// print
void
dian_nfu3::print()
{
	int i;
	cout << "<NFU-3> PRINT" << endl;
	for (i = 0; i < NFU3_WIDTH; ++i)
	{
		cout << hex << _actfunc_modules[i]->getData() << "\t";
	}

	cout << endl;
}
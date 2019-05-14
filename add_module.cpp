#include "add_module.h"
#include "typedef_dla.h"


// Constructor
m_add::m_add()
{
	_type = TIMINGOBJ_TYPE::ADD;
	_add_bitsize = 16;
	_add_access_cnt = 0;
	_in_port1 = NULL;
	_in_port2 = NULL;
	_out_port = NULL;
	_data_buffer = 0;
}

m_add::m_add(const char* name, _BITSIZE add_bitsize) : timingObject(name)
{
	_type = TIMINGOBJ_TYPE::ADD;
	_add_bitsize = add_bitsize;
	_add_access_cnt = 0;
	_in_port1 = NULL;
	_in_port2 = NULL;
	_out_port = NULL;
	_data_buffer = 0;
}

// Copy Constructor
m_add::m_add(const m_add& p) : timingObject(p)
{
	_type = TIMINGOBJ_TYPE::ADD;
	_add_bitsize = p._add_bitsize;
	_add_access_cnt = p._add_access_cnt;
	_in_port1 = p._in_port1;
	_in_port2 = p._in_port2;
	_out_port = p._out_port;
	_data_buffer = p._data_buffer;
}

// Destructor
m_add::~m_add()
{
}

// Tick
void
m_add::tick(_TICK curTick)
{
	// stats
	if (_data != _data_buffer)
		ADDER_ACCESS++;

	// forward previous result
	_data = _data_buffer;

	// do calculation
	if (_in_port1 == NULL || _in_port2 == NULL)
		return;

	_data_buffer = (_in_port1)->getData() + (_in_port2)->getData();

	// check only in final adder
	if (_check_overflow)
	{
		// check sign & overflow
		if (_data_buffer < NEG_MAX)
		{
			// negative number
			_data_buffer = NEG_MAX;
		}
		else if (_data_buffer > POS_MAX)
		{
			// positive number
			_data_buffer = POS_MAX;
		}
	}

	if (!_quite_add)
		cout << "[ADDER:" << this->getName() << "] calc " << hex << (_in_port1)->getData()
		<< " + " << (_in_port2)->getData() << " = " << _data_buffer << dec << endl;
}


// Set ports
void
m_add::setInPort1(timingObject* tobj)
{
	_in_port1 = tobj;
	if (!_quite_moduleconnection)
		std::cout << tobj->getName() << " is connected to " << this->getName() << std::endl;
}

void
m_add::setInPort2(timingObject* tobj)
{
	_in_port2 = tobj;
	if (!_quite_moduleconnection)
		std::cout << tobj->getName() << " is connected to " << this->getName() << std::endl;
}

void
m_add::setOutPort(timingObject* tobj)
{
	_out_port = tobj;
}

// Get ports
timingObject*
m_add::getInPort1()
{
	return _in_port1;
}

timingObject*
m_add::getInPort2()
{
	return _in_port2;
}

timingObject*
m_add::getOutPort()
{
	return _out_port;
}

// Input feed
_DATA_STORAGE
m_add::getData()
{
	return _data;
}

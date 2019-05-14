#include "dla_system.h"

DLA_System::DLA_System()
{
	// set name to default
	_name = (char*)("default_system");

	// make timingObjectList
	_tobjList.clear();

	std::cout << "\'" << _name << "\' is created." << std::endl;
}

DLA_System::DLA_System(const char *name)
{
	// set name
	_name = new char[(strlen(name) + 1) * sizeof(char)];
	strcpy_s(_name, strlen(name) + 1, name);

	// make timingObjectList
	_tobjList.clear();

	std::cout << "\'" << _name << "\' is created." << std::endl;
}

DLA_System::~DLA_System()
{
	// tobjList 정리하기
	for (auto it = _tobjList.begin(); it != _tobjList.end(); ++it)
	{
		timingObject* tempTOBJ = *it;

		//TODO : timingObject Type별로 delete 적용할 것.
		switch (tempTOBJ->_type)
		{
		case NONE:
			delete (tempTOBJ);
			break;
		case REGISTER:
			delete ((m_reg*)tempTOBJ);
			break;
		case WIRE:
			break;
		case MUX:
			break;
		case DEMUX:
			break;
		case ALU:
			break;
		default:
			break;
		}
	}

	std::cout << "\'" << _name << "\' is deleted." << std::endl;
}
/*---------------------------------- Tick --------------------------------*/
void
DLA_System::tick(_TICK curTick)
{
	// for all modules in the system.
	// tick first
	for (auto it = _tobjList.begin(); it != _tobjList.end(); ++it)
	{
		(*it)->tick(curTick);
	}
	// and update to it.
	for (auto it = _tobjList.begin(); it != _tobjList.end(); ++it)
	{
		(*it)->tick_update(curTick);
	}
}


/*---------------------------------- Place an Object --------------------------------*/
bool
DLA_System::place_object(timingObject*& tobj)
{
	_tobjList.push_back(tobj);

	std::cout << _name << ":\t";
	std::cout << "\'" << tobj->getName() << "\' is placed." << std::endl;

	return true;
}

bool
DLA_System::place_register(const char* name, _BITSIZE reg_bitsize)
{
	// make new register
	timingObject* temp_reg = new m_reg(name, reg_bitsize);
	return place_object(temp_reg);
}

bool DLA_System::place_register(_BITSIZE reg_bitsize)
{
	return place_register("REG_0");
}

/* Find Objects */
timingObject*
DLA_System::findTimingObj_byName(const char* name)
{
	auto it = _tobjList.begin();

	for (it; it != _tobjList.end(); ++it)
	{
		if (!strcmp(name, (*it)->getName()))
		{
			return *it;
		}
	}

	return NULL;
}

/*---------------------------------- Connect Objects --------------------------------*/
bool DLA_System::connect_objects(timingObject *& from_tobj, timingObject *& to_tobj)
{
	// check validation
	if (from_tobj == NULL || to_tobj == NULL)
	{
		return false;
	}

	// TODO : Check both objects are I/O port acceptable.

	switch (from_tobj->_type)
	{
	case TIMINGOBJ_TYPE::REGISTER:
		((m_reg*)from_tobj)->setOutPort(to_tobj);
		break;
	default:
		return false;
	}

	switch (to_tobj->_type)
	{
	case TIMINGOBJ_TYPE::REGISTER:
		((m_reg*)to_tobj)->setInPort(from_tobj);
		break;
	default:
		break;
	}

	std::cout << _name << ":\t";
	std::cout << "\'" << from_tobj->getName() << "\' => \'" << to_tobj->getName() << "\' connected." << std::endl;

	return true;
}

bool DLA_System::connect_objects(const char * from_name, const char * to_name)
{
	timingObject* from_tobj = findTimingObj_byName(from_name);
	timingObject* to_tobj = findTimingObj_byName(to_name);

	if (from_tobj == NULL)
		std::cout << "\'" << from_name << "\' object could not be found." << std::endl;
	if (to_tobj == NULL)
		std::cout << "\'" << to_name << "\' object could not be found." << std::endl;

	return connect_objects(from_tobj, to_tobj);
}

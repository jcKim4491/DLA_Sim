#pragma once
#ifndef __DLA_SYSTEM_H__
#define __DLA_SYSTEM_H__

#include <iostream>
#include <string>
#include <list>
#include "timingObject.h"
#include "register_module.h"

using namespace std;

class DLA_System
{
public:
	// Constructor
	DLA_System();
	DLA_System(const char *dla_name);
	// Copy Constructor
	DLA_System(const DLA_System& p)
	{
		strcpy_s(_name, strlen(p._name), p._name);
	}
	// Destructor
	~DLA_System();

	// tick
	void virtual tick(_TICK curTick);

	// Place an object
	bool place_object(timingObject*& tobj);
	bool place_register(const char* name, _BITSIZE reg_bitsize = 32);
	bool place_register(_BITSIZE reg_bitsize = 32);

	// Find object by its name
	timingObject* findTimingObj_byName(const char* name);

	// Connect objects
	bool connect_objects(timingObject*& from_tobj, timingObject*& to_tobj);
	bool connect_objects(const char* from_name, const char* to_name);

	bool _process_end = false;

protected:
	char* _name;	//system name
	list<timingObject*> _tobjList;
	
};

#endif // __DLA_SYSTEM_H__


#pragma once
#ifndef __DIAN_ADDER_TREE_H__
#define __DIAN_ADDER_TREE_H__

#include <iostream>
#include <string>
#include "typedef_dla.h"
#include "optionparser.h"
#include "register_module.h"
#include "add_module.h"

using namespace std;

class dian_addertree
{
public:
	// Constructor
	dian_addertree();
	dian_addertree(int tree_id);

	// Destructor
	~dian_addertree();

	// tick
	void tick(_TICK curTick);

	// set input Ports
	void setInPort(timingObject* src, int target_idx);

	// get exit module
	timingObject* getExitModule()
	{
		if (_tree_regs == NULL)
			return NULL;

		return _tree_regs[TREE_LENGTH - 1];
	}

	// get result
	_DATA_STORAGE getResult();

private:
	m_add ** _tree_regs;
	int _tree_id;			// Id of this tree.
};

#endif // __DIAN_ADDER_TREE_H__
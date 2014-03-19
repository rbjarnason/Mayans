/** \file inventory.cpp
 * 
 *
 * $Id: inventory.cpp,v 1.2 2002/06/06 05:51:01 robofly Exp $
 */

/* Copyright, 2001, 2002 InOrbit Entertainment, Inc.
 *
 * This file is part of Bang.
 * Bang is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the 
 * Free Software Foundation; either version 2, or (at your option) any later 
 * version.
 *
 * Bang is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.

 * You should have received a copy of the GNU General Public License along 
 * with Bang; see the file COPYING. If not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */


#include "inventory.h"

#include "nel/misc/common.h"
#include "nel/misc/command.h"
#include "nel/misc/debug.h" 
#include "nel/misc/path.h"
#include "nel/misc/i18n.h"
#include <iostream.h>
#include "nel/misc/types_nl.h"
#include "nel/misc/file.h" 

#include "network.h"
#include "client.h"
// contains the config class
#include "nel/misc/config_file.h"


using namespace NLMISC;
using namespace std;

// ***************************************************************************
void	CInventory::load(std::string configFileName)
{
	CIFile f;
	try
	{
		if (f.open(configFileName+".inv"))
		{
			printf("reading inventory for %s\n", configFileName.c_str());
			serial(f);
			f.close();
		}
		else
		{
			save (configFileName);
		}
		}
	catch(exception &e)
	{
		printf ("My T3 Error %s\n", e.what ());
	}


}

void	CInventory::save(std::string configFileName)
{
	COFile f(configFileName+".inv");
	try
	{
		serial(f);
		f.close();
	}
	catch(exception &e)
	{
		printf ("HMM is this MY Error %s\n", e.what ());
	}
//	f.close();
	printf("We have save the serialized file called %s.\n", configFileName);
}


void CInventory::addItem(uint32 item, uint8 classId)
{

	if (ShowDebugInChat) nlinfo("invmap...+ ");
	_Inventory.insert( make_pair(item, classId));
	select(item);
}

void CInventory::dropItem(uint32 item)
{
	if (ShowDebugInChat) nlinfo("dropped %d",item);
	if(_Inventory.find(item) != _Inventory.end())
	{
			_Inventory.erase(item);
	}
	selectNext();
}

uint32 CInventory::selectNext()
{
	std::map<uint32, uint8>::iterator ite=_Inventory.find(_Selected);
	
	if (ite==_Inventory.end())
	{
		ite=_Inventory.begin();
		if (ite!=_Inventory.end())
		{
			select((*ite).first);
		}
		else select( 0 );
	}
	else 
	{
		ite++;
		if (ite!=_Inventory.end())
		{
			select((*ite).first);
		}
		else
		{
			ite=_Inventory.begin();
			if (ite!=_Inventory.end())
			{
				select((*ite).first);
			}
		}
	}

//	nlinfo("selected %d", _Selected);
	return _Selected;
}

void CInventory::select(uint32 item)
{
//	nlinfo("selected %d",item);
	if(_Inventory.find(item) != _Inventory.end())
	{
		_Selected=item;
		_RightHand=item;
		if (ShowDebugInChat) nlinfo("item = %d", item);
		sendHoldDynamicObject( getSelected(), false );
		setHand( getSelected(), false );
	}
	else
	{
		if (ShowDebugInChat) nlinfo("player does not have item");
	}
}

void CInventory::hold(bool leftHand, uint32 item)
{
	if(_Inventory.find(item) != _Inventory.end())
	{
		if (leftHand) _LeftHand=item;
		else _RightHand=item;
	}
	else if (ShowDebugInChat) nlinfo("player does not have item");
}

uint8 CInventory::getClassId(uint32 item)
{
	if (ShowDebugInChat) nlinfo("class of %d",item);
	if(_Inventory.find(item) != _Inventory.end())
	{
		return _Inventory[item];
	}
	return 0;
}

// ***************************************************************************
void CInventory::serial(NLMISC::IStream &f) throw(NLMISC::EStream)
{
	
	uint	ver= f.serialVersion(1);
	// First, serial an header or checking if it is correct
	f.serialCheck ((uint32)'VNI');

	// Then, serial the shape
	f.serialCont (_Inventory);

	// Ok, it's done
}

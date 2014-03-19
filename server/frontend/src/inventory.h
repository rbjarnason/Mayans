/** \file inventory.h
 * 
 *
 * $Id: inventory.h,v 1.1 2002/03/20 18:10:21 robofly Exp $
 */

/* Copyright, 2001 In-Orbit Entertainment Inc.
 *
 * This file is part of NEVRAX NEL.
 * NEVRAX NEL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * NEVRAX NEL is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with NEVRAX NEL; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef NL_INVENTORY_H
#define NL_INVENTORY_H

#include <vector>
#include <string> 
#include <nel/misc/types_nl.h>
#include <nel/misc/vector.h>
#include <map>


/**
 * The inventory class
 * \author Michael Warnock
 * \author Róbert Bjarnason
 * \author In-Orbit Entertainment SF
 * \date 2001
 */
class CInventory //: public NLMISC::IStreamable
{

public:

	std::map< uint32,uint8 > _Inventory;

	uint32 _Selected;
	uint32 _LeftHand;
	uint32 _RightHand;

	CInventory()
	{
		_Selected=99999999;
	}

	void addItem(uint32 item, uint8 classId);

	void dropItem(uint32 item);

	void select(uint32 item);
	void hold(bool leftHand, uint32 item);

	uint32 selectNext();

	uint32 getSelected() { return _Selected; }
	uint32 getHand(bool leftHand ) { if (leftHand) return _LeftHand; else return _RightHand; }
	uint32 getLeftHand() { return _LeftHand; }

	uint8 getClassId(uint32 item);

	std::map<uint32,uint8 > getInventory() { return _Inventory; }
	
	void				load( std::string configFileName );
	
	void				save( std::string configFileName );
	/// serial the inventory.
	void serial(NLMISC::IStream &f) throw(NLMISC::EStream);

};

#endif // NL_INVENTORY_H
/** \world_objects.h
 * <File description>
 *
 * $Id: world_objects.h,v 1.1 2002/03/20 18:05:25 robofly Exp $
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

#ifndef NL_WORLD_OBJECTS_H
#define NL_WORLD_OBJECTS_H


#include "nel/misc/types_nl.h"
#include "nel/misc/smart_ptr.h"
#include "nel/misc/vector.h"
#include "nel/misc/matrix.h"
#include "nel/misc/stream.h"
#include "nel/misc/debug.h"
#include "nel/misc/config_file.h"

#include <stdio.h>
#include <vector>
#include <map>

using namespace std;

/*
 * Dynamic World object
 *
 * \author Róbert Bjarnason
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class CDynamicObject
{

public:
 
	enum ClassId
	{
		None,		
		DynamicObject,	
		Firearm,	
		BoxOfBullets,	
		Nutrition
	};

	uint32					Id;
	uint8					ClassId;
	uint32					Owner;
	uint32					TypeId;
	std::string				Name;
	std::string				MeshName;
	NLMISC::CVector			Position;
	NLMISC::CVector			Heading;
	NLMISC::CVector			TrajVector;
	NLMISC::CVector			MassCenter;
	NLMISC::CVector			Dimensions;
	
	float					AngVelocity;

	bool					Visable;
	
	float					Radius;
	float					Height;
	float					Scale;
	float					Weight;
	uint16				Durability;

	/// Constructor
	CDynamicObject() {};

	void				load( std::string fileName );
	void				save();

	virtual void		createFromTextFile( NLMISC::CConfigFile configFile );
	virtual void		loadFromTextFile( std::string textFileName );

	void				serial(NLMISC::IStream &f);

private:
};

/*
 * Firearm World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class CFirearm : public CDynamicObject
{


public:

	uint16			Quality;
	uint16			Maintenance;
	uint16			Reliability;
	bool			RoundPresent;
	uint8			BulletsLeft;
	bool			Safety;
	float			Firepower;
	uint32			TypeId;
	bool			Jammed;
	bool			Chambered;

	/// Constructor
	CFirearm() {};

	void				createFromTextFile( NLMISC::CConfigFile configFile );

	void				serial( NLMISC::IStream &f );

private:
};

/*
 * BoxOfBullets World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class CBoxOfBullets : public CDynamicObject
{


public:

	uint8			BulletsLeft;

	/// Constructor
	CBoxOfBullets() {};

	void				createFromTextFile( NLMISC::CConfigFile configFile );

	void				serial( NLMISC::IStream &f );

private:
};
		
/*
 * CNutrition World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class CNutrition : public CDynamicObject
{
	

public:	
	
	uint16			Quality;
	uint16			Maintenance;

	/// Constructor
	CNutrition() {};

	void				createFromTextFile( NLMISC::CConfigFile configFile );

	void				serial( NLMISC::IStream &f );

private:
};
#endif // NL_WORLD_OBJECTS_H

/* End of worldsobject.h */


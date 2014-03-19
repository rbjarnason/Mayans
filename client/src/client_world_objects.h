/** \client_world_objects.h
 * <File description>
 *
 * $Id: client_world_objects.h,v 1.1 2002/03/20 18:05:25 robofly Exp $
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


#ifndef NL_CLIENT_WORLD_OBJECTS_H
#define NL_CLIENT_WORLD_OBJECTS_H


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
 * Dynamic Client World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class CClientDynamicObject
{


public:

	uint32					Id;
	uint8					ClassId;
	uint32					Owner;
	uint32					TypeId;
	std::string				Name;
	std::string				MeshName;
	NLMISC::CVector			Position;
	NLMISC::CVector			Heading;
	NLMISC::CVector			TrajVector; 
	float					AngVelocity;

	bool					Visable;
	
	float					Radius;
	float					Height;
	float					Scale;
	float					Weight;

	std::string				SoundName;

	/// Constructor 
	CClientDynamicObject() {};

	void				serial(NLMISC::IStream &f);

private:
};

/*
 * Client Firearm World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class CClientFirearm : public CClientDynamicObject
{


public:

	uint16			Maintenance;
	bool			RoundPresent;
	uint8			BulletsLeft;
	bool			Safety;
	bool			Jammed;
	bool			Chambered;

	uint32			BulletsTypeId;
	uint8			MaxBullets;
	float			FireRate1Round;
	float			FireRateAuto;
	float			Loudness;
	float			RecoilMod;
	std::string		ParticleSystem;
	NLMISC::CVector	ParticleSystemScale;
	std::string		FireSound;


	/// Constructor
	CClientFirearm() {};

	void				serial( NLMISC::IStream &f );

private:
};

/*
 * Client BoxOfBullets World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class CClientBoxOfBullets : public CClientDynamicObject
{


public:

	uint32			BulletsType;
	uint8			MaxBullets;
	uint8			BulletsLeft;

	/// Constructor
	CClientBoxOfBullets() {};

	void				serial( NLMISC::IStream &f );

private:
};

/*
 * Client Nutrition World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class CClientNutrition : public CClientDynamicObject
{


public:

	uint8			Carbos;
	uint8			Proteins;
	uint8			VitaminA;
	uint8			VitaminB;
	uint8			VitaminC;
	uint8			Fats;
	uint8			Water;
	uint16			Maintenance;
	uint16			Quality;

	/// Constructor
	CClientNutrition() {};

	void				serial( NLMISC::IStream &f );

private:
};
 

#endif // NL_CLIENT_WORLD_OBJECTS_H

/* End of client_world_objects.h */


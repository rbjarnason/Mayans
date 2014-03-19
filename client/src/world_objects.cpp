/** \world_objects.cpp
 * <File description>
 *
 * $Id: world_objects.cpp,v 1.2 2002/06/06 05:51:01 robofly Exp $
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

#include "world_objects.h"
#include "nel/misc/common.h"
#include "nel/misc/command.h"
#include "nel/misc/debug.h"
#include "nel/misc/path.h"
#include "nel/misc/i18n.h"
#include <iostream.h>
#include "nel/misc/types_nl.h"
#include "nel/misc/file.h" 

#include "client.h"
// contains the config class
#include "nel/misc/config_file.h"

using namespace std;
using namespace NLMISC;

/*
 * Dynamic World object
 *
 * \author Róbert Bjarnason
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

void	CDynamicObject::load(std::string fileName)
{
	CIFile f;
	try
	{
		if (f.open(fileName+".dynobj"))
		{
			if (ShowDebugInChat) nlinfo("reading dynamic object for %s\n", fileName.c_str());
			serial(f);
			f.close();
		}
		else
		{
			loadFromTextFile( fileName );
		}
	}
	catch(exception &e)
	{
		if (ShowDebugInChat) nlinfo ("My Error %s\n", e.what ());
	}


}

void			CDynamicObject::save()
{
	string fileName;
	char pBuf[20];
	sprintf(pBuf, "%d", Id);
	fileName = pBuf;

	COFile f(fileName+".dynobj");
	try
	{
		serial(f);
		f.close();
	}
	catch(exception &e)
	{
		if (ShowDebugInChat) nlinfo ("HMM is this MY Error %s\n", e.what ());
	}

	if (ShowDebugInChat) nlinfo("We have save the serialized file called %s.\n", fileName);
}


void		CDynamicObject::loadFromTextFile( std::string textFileName )
{
	if (ShowDebugInChat) nlinfo("Entering loadFromTextFile file.");
	
	CConfigFile configFile;

	configFile.load(textFileName+".dodef");

	createFromTextFile( configFile );
}

void		CDynamicObject::createFromTextFile( CConfigFile configFile )
{
	if (ShowDebugInChat) nlinfo("Entering createFromTextFile file.");
	
	Id = configFile.getVar("DynamicObjectId").asInt();
	ClassId = configFile.getVar("ClassId").asInt();
	Owner = configFile.getVar("Owner").asInt();
	TypeId = configFile.getVar("TypeId").asInt();

	Name = configFile.getVar("Name").asString();
	MeshName = configFile.getVar("MeshName").asString();

	Position.x = configFile.getVar("Position").asFloat(0);
	Position.y = configFile.getVar("Position").asFloat(1);
	Position.z = configFile.getVar("Position").asFloat(2);

	Heading.x = configFile.getVar("Heading").asFloat(0);
	Heading.y = configFile.getVar("Heading").asFloat(1);
	Heading.z = configFile.getVar("Heading").asFloat(2);

	TrajVector.x = configFile.getVar("TrajVector").asFloat(0);
	TrajVector.y = configFile.getVar("TrajVector").asFloat(1);
	TrajVector.z = configFile.getVar("TrajVector").asFloat(2);

	MassCenter.x = configFile.getVar("MassCenter").asFloat(0);
	MassCenter.y = configFile.getVar("MassCenter").asFloat(1);
	MassCenter.z = configFile.getVar("MassCenter").asFloat(2);

	Dimensions.x = configFile.getVar("Dimensions").asFloat(0);
	Dimensions.y = configFile.getVar("Dimensions").asFloat(1);
	Dimensions.z = configFile.getVar("Dimensions").asFloat(2);

	AngVelocity = configFile.getVar("AngVelocity").asFloat();
	
	Visable = configFile.getVar("Visable").asInt();

	Radius = configFile.getVar("Radius").asFloat();
	Height = configFile.getVar("Height").asFloat();
	Scale = configFile.getVar("Scale").asFloat();

	Weight = configFile.getVar("Weight").asFloat();
	Durability = configFile.getVar("Durability").asInt();
}

// ***************************************************************************
void			CDynamicObject::serial(NLMISC::IStream &f)
{
	uint	ver= f.serialVersion(0);
	f.serial(Id, ClassId, Owner, TypeId);
	f.serial( Name );
	f.serial( MeshName );
	f.serial(Position, Heading);
	f.serial(TrajVector, MassCenter, Dimensions);
	f.serial(AngVelocity);
	f.serial(Visable);
	f.serial(Radius, Height, Scale);
}

/*
 * Firearm World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

void	CFirearm::createFromTextFile( CConfigFile configFile )
{
	Id = configFile.getVar("DynamicObjectId").asInt();
	ClassId = configFile.getVar("ClassId").asInt();
	Owner = configFile.getVar("Owner").asInt();
	TypeId = configFile.getVar("TypeId").asInt();

	Name = configFile.getVar("Name").asString();
	MeshName = configFile.getVar("MeshName").asString();

	Position.x = configFile.getVar("Position").asFloat(0);
	Position.y = configFile.getVar("Position").asFloat(1);
	Position.z = configFile.getVar("Position").asFloat(2);

	Heading.x = configFile.getVar("Heading").asFloat(0);
	Heading.y = configFile.getVar("Heading").asFloat(1);
	Heading.z = configFile.getVar("Heading").asFloat(2);

	TrajVector.x = configFile.getVar("TrajVector").asFloat(0);
	TrajVector.y = configFile.getVar("TrajVector").asFloat(1);
	TrajVector.z = configFile.getVar("TrajVector").asFloat(2);

	MassCenter.x = configFile.getVar("MassCenter").asFloat(0);
	MassCenter.y = configFile.getVar("MassCenter").asFloat(1);
	MassCenter.z = configFile.getVar("MassCenter").asFloat(2);

	Dimensions.x = configFile.getVar("Dimensions").asFloat(0);
	Dimensions.y = configFile.getVar("Dimensions").asFloat(1);
	Dimensions.z = configFile.getVar("Dimensions").asFloat(2);

	AngVelocity = configFile.getVar("AngVelocity").asFloat();
	
	Visable = configFile.getVar("Visable").asInt();

	Radius = configFile.getVar("Radius").asFloat();
	Height = configFile.getVar("Height").asFloat();
	Scale = configFile.getVar("Scale").asFloat();

	Weight = configFile.getVar("Weight").asFloat();
	Durability = configFile.getVar("Durability").asInt();

	Quality = configFile.getVar("Quality").asInt();
	Maintenance = configFile.getVar("Maintenance").asInt();
	Reliability = configFile.getVar("Reliability").asInt();
	RoundPresent = configFile.getVar("RoundPresent").asInt();
	BulletsLeft = configFile.getVar("BulletsLeft").asInt();
	Safety = configFile.getVar("Safety").asInt();
	RoundPresent = configFile.getVar("RoundPresent").asInt();
	Firepower = configFile.getVar("Firepower").asFloat();
	Jammed = configFile.getVar("Jammed").asInt();
	Chambered = configFile.getVar("chambered").asInt();
}

// ***************************************************************************
void			CFirearm::serial(NLMISC::IStream &f)
{
	uint	ver= f.serialVersion(0);
	f.serial(Id, ClassId, Owner, TypeId);
	f.serial( Name );
	f.serial( MeshName );
	f.serial(Position, Heading);
	f.serial(TrajVector, MassCenter, Dimensions);
	f.serial(AngVelocity);
	f.serial(Visable);
	f.serial(Radius, Height, Scale, Weight, Durability);

	f.serial( Quality, Maintenance, Reliability );
	f.serial( RoundPresent );
	f.serial( BulletsLeft );
	f.serial( Safety );
	f.serial( Firepower );
	f.serial( Jammed );
	f.serial( Chambered );
}

/*
 * BoxOfBullets World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

void	CBoxOfBullets::createFromTextFile( CConfigFile configFile )
{

	Id = configFile.getVar("DynamicObjectId").asInt();
	ClassId = configFile.getVar("ClassId").asInt();
	Owner = configFile.getVar("Owner").asInt();
	TypeId = configFile.getVar("TypeId").asInt();

	Name = configFile.getVar("Name").asString();
	MeshName = configFile.getVar("MeshName").asString();

	Position.x = configFile.getVar("Position").asFloat(0);
	Position.y = configFile.getVar("Position").asFloat(1);
	Position.z = configFile.getVar("Position").asFloat(2);

	Heading.x = configFile.getVar("Heading").asFloat(0);
	Heading.y = configFile.getVar("Heading").asFloat(1);
	Heading.z = configFile.getVar("Heading").asFloat(2);

	TrajVector.x = configFile.getVar("TrajVector").asFloat(0);
	TrajVector.y = configFile.getVar("TrajVector").asFloat(1);
	TrajVector.z = configFile.getVar("TrajVector").asFloat(2);

	MassCenter.x = configFile.getVar("MassCenter").asFloat(0);
	MassCenter.y = configFile.getVar("MassCenter").asFloat(1);
	MassCenter.z = configFile.getVar("MassCenter").asFloat(2);

	Dimensions.x = configFile.getVar("Dimensions").asFloat(0);
	Dimensions.y = configFile.getVar("Dimensions").asFloat(1);
	Dimensions.z = configFile.getVar("Dimensions").asFloat(2);

	AngVelocity = configFile.getVar("AngVelocity").asFloat();
	
	Visable = configFile.getVar("Visable").asInt();

	Radius = configFile.getVar("Radius").asFloat();
	Height = configFile.getVar("Height").asFloat();
	Scale = configFile.getVar("Scale").asFloat();

	Weight = configFile.getVar("Weight").asFloat();
	Durability = configFile.getVar("Durability").asInt();

	BulletsLeft = configFile.getVar("BulletsLeft").asInt();
}

// ***************************************************************************
void	CBoxOfBullets::serial(NLMISC::IStream &f)
{
	uint	ver= f.serialVersion(0);
	f.serial(Id, ClassId, Owner, TypeId);
	f.serial( Name );
	f.serial( MeshName );
	f.serial(Position, Heading);
	f.serial(TrajVector, MassCenter, Dimensions);
	f.serial(AngVelocity);
	f.serial(Visable);
	f.serial(Radius, Height, Scale, Weight, Durability);

	f.serial( BulletsLeft );
}
		
/*
 * CNutrition World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

void	CNutrition::createFromTextFile( CConfigFile configFile )
{
	Id = configFile.getVar("DynamicObjectId").asInt();
	ClassId = configFile.getVar("ClassId").asInt();
	Owner = configFile.getVar("Owner").asInt();
	TypeId = configFile.getVar("TypeId").asInt();

	Name = configFile.getVar("Name").asString();
	MeshName = configFile.getVar("MeshName").asString();

	Position.x = configFile.getVar("Position").asFloat(0);
	Position.y = configFile.getVar("Position").asFloat(1);
	Position.z = configFile.getVar("Position").asFloat(2);

	Heading.x = configFile.getVar("Heading").asFloat(0);
	Heading.y = configFile.getVar("Heading").asFloat(1);
	Heading.z = configFile.getVar("Heading").asFloat(2);

	TrajVector.x = configFile.getVar("TrajVector").asFloat(0);
	TrajVector.y = configFile.getVar("TrajVector").asFloat(1);
	TrajVector.z = configFile.getVar("TrajVector").asFloat(2);

	MassCenter.x = configFile.getVar("MassCenter").asFloat(0);
	MassCenter.y = configFile.getVar("MassCenter").asFloat(1);
	MassCenter.z = configFile.getVar("MassCenter").asFloat(2);

	Dimensions.x = configFile.getVar("Dimensions").asFloat(0);
	Dimensions.y = configFile.getVar("Dimensions").asFloat(1);
	Dimensions.z = configFile.getVar("Dimensions").asFloat(2);

	AngVelocity = configFile.getVar("AngVelocity").asFloat();
	
	Visable = configFile.getVar("Visable").asInt();

	Radius = configFile.getVar("Radius").asFloat();
	Height = configFile.getVar("Height").asFloat();
	Scale = configFile.getVar("Scale").asFloat();

	Weight = configFile.getVar("Weight").asFloat();
	Durability = configFile.getVar("Durability").asInt();

	Quality = configFile.getVar("Quality").asInt();
	Maintenance = configFile.getVar("Maintenance").asInt();
}

// ***************************************************************************
void	CNutrition::serial(NLMISC::IStream &f)
{
	uint	ver= f.serialVersion(0);
	f.serial(Id, ClassId, Owner, TypeId);
	f.serial( Name );
	f.serial( MeshName );
	f.serial(Position, Heading);
	f.serial(TrajVector, MassCenter, Dimensions);
	f.serial(AngVelocity);
	f.serial(Visable);
	f.serial(Radius, Height, Scale, Weight, Durability);
	f.serial(Quality, Maintenance);
}
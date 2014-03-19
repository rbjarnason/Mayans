/** \world_types.cpp
 * <File description>
 *
 * $Id: world_types.cpp,v 1.1 2002/03/20 18:10:21 robofly Exp $
 */

/* Copyright, 2001 InOrbit Entertainment, Inc.
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

#include "world_types.h"
#include "nel/misc/common.h" 
#include "nel/misc/command.h"
#include "nel/misc/debug.h"
#include "nel/misc/path.h"
#include "nel/misc/i18n.h"
#include <iostream.h>
#include "nel/misc/types_nl.h"
#include "nel/misc/file.h" 

// contains the config class
#include "nel/misc/config_file.h"

using namespace std;
using namespace NLMISC;


/*
 * World Type
 *
 * \author Róbert Bjarnason
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

void	CWorldType::load(std::string fileName)
{
	CIFile f;
	try
	{
/*		if (f.open(fileName+".type"))
		{
			nlinfo("reading world type for %s\n", fileName.c_str());
			serial(f);
			this->serial(f);
			f.close();
		}
		else
		{*/
			loadFromTextFile( fileName );
//		}
		}
	catch(exception &e)
	{
		nlinfo ("My Error %s\n", e.what ());
	}


}

void	CWorldType::save(std::string fileName)
{
	COFile f(fileName+".type");
	try
	{
		serial(f);
		this->serial(f);
		f.close();
	}
	catch(exception &e)
	{
		nlinfo ("HMM is this MY Error %s\n", e.what ());
	}
//	f.close();
	nlinfo("We have save the serialized file called %s.\n", fileName);
}


void		CWorldType::loadFromTextFile( std::string textFileName )
{
	nlinfo("Entering CWorldType loadFromTextFile file.");
	
	CConfigFile configFile;

	configFile.load(textFileName+".typedef");

	createFromTextFile( configFile );
	this->createFromTextFile( configFile );
}

void		CWorldType::createFromTextFile( CConfigFile configFile )
{
	nlinfo("Entering createFromTextFile CWorldType. %s", configFile.getFilename());

	TypeId = configFile.getVar("TypeId").asInt();
	ClassId = configFile.getVar("ClassId").asInt();
	Name = configFile.getVar("Name").asString();
	Shape = configFile.getVar("Shape").asString();
	Weight = configFile.getVar("Weight").asFloat();
	Material = configFile.getVar("Material").asString();
	SoundName = configFile.getVar("SoundName").asString();
}

void		CWorldType::serial(NLMISC::IStream &f)
{
	uint	ver= f.serialVersion(0);
	f.serial( TypeId );
	f.serial( ClassId );
	f.serial( Name );
	f.serial( Shape );
	f.serial( Weight );
	f.serial( Material );
	f.serial( SoundName );
}

/*
 * Firearm World Type
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

void	CFirearmType::createFromTextFile( CConfigFile configFile )
{
	nlinfo("Entering createFromTextFile CFirearmType. %s", configFile.getFilename());

	TypeId = configFile.getVar("TypeId").asInt();
	ClassId = configFile.getVar("ClassId").asInt();
	Name = configFile.getVar("Name").asString();
	Shape = configFile.getVar("Shape").asString();
	Weight = configFile.getVar("Weight").asFloat();
	Material = configFile.getVar("Material").asString();
	SoundName = configFile.getVar("SoundName").asString();

	BulletsTypeId = configFile.getVar("BulletsTypeId").asInt();
	Accuracy = configFile.getVar("Accuracy").asInt();
	MaxBullets = configFile.getVar("MaxBullets").asInt();

	FireRate1Round = configFile.getVar("FireRate1Round").asFloat();
	FireRateAuto = configFile.getVar("FireRateAuto").asFloat();
	Loudness = configFile.getVar("Loudness").asFloat();
	RecoilMod = configFile.getVar("RecoilMod").asFloat();

	ParticleSystem = configFile.getVar("ParticleSystem").asString();

	ParticleSystemScale.x = configFile.getVar("ParticleSystemScale").asFloat(0);
	ParticleSystemScale.y = configFile.getVar("ParticleSystemScale").asFloat(1);
	ParticleSystemScale.z = configFile.getVar("ParticleSystemScale").asFloat(2);

	DurabilityThreshold = configFile.getVar("DurabilityThreshold").asInt();

	FireSound = configFile.getVar("FireSound").asString();

}

void	CFirearmType::serial(NLMISC::IStream &f)
{
	uint	ver= f.serialVersion(0);
	f.serial( TypeId );
	f.serial( ClassId );
	f.serial( Name );
	f.serial( Shape );
	f.serial( Weight );
	f.serial( Material );
	f.serial( SoundName );
	f.serial( BulletsTypeId, Accuracy, MaxBullets );
	f.serial( FireRate1Round, FireRateAuto, Loudness, RecoilMod );
	f.serial( ParticleSystem );
	f.serial( ParticleSystemScale );
	f.serial( DurabilityThreshold );
	f.serial( FireSound );
}

/*
 * Bullet World Type
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

void	CBulletType::createFromTextFile( CConfigFile configFile )
{
	nlinfo("Entering createFromTextFile CBulletType. %s", configFile.getFilename());

	TypeId = configFile.getVar("TypeId").asInt();
	ClassId = configFile.getVar("ClassId").asInt();
	Name = configFile.getVar("Name").asString();
	Shape = configFile.getVar("Shape").asString();
	Weight = configFile.getVar("Weight").asFloat();
	Material = configFile.getVar("Material").asString();
	SoundName = configFile.getVar("SoundName").asString();

	PsHard = configFile.getVar("PsHard").asString();
	PsSoft = configFile.getVar("PsSoft").asString();

	PsScale.x = configFile.getVar("PsScale").asFloat(0);
	PsScale.y = configFile.getVar("PsScale").asFloat(1);
	PsScale.z = configFile.getVar("PsScale").asFloat(2);

	ExploisionRadius = configFile.getVar("ExploisionRadius").asFloat();
	PropRating = configFile.getVar("PropRating").asFloat();

	HitSoundSoft = configFile.getVar("HitSoundSoft").asString();
	HitSoundHard = configFile.getVar("HitSoundHard").asString();


}

void	CBulletType::serial(NLMISC::IStream &f)
{
	uint	ver= f.serialVersion(0);
	f.serial( TypeId );
	f.serial( ClassId );
	f.serial( Name );
	f.serial( Shape );
	f.serial( Weight );
	f.serial( Material );
	f.serial( SoundName );

	f.serial( PsHard, PsSoft );
	f.serial( PsScale );
	f.serial( ExploisionRadius );
	f.serial( PropRating );
	f.serial( HitSoundSoft, HitSoundHard );
}

/*
 * CBoxOfBulletsType World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */		

void	CBoxOfBulletsType::createFromTextFile( CConfigFile configFile )
{

	TypeId = configFile.getVar("TypeId").asInt();
	ClassId = configFile.getVar("ClassId").asInt();
	Name = configFile.getVar("Name").asString();
	Shape = configFile.getVar("Shape").asString();
	Weight = configFile.getVar("Weight").asFloat();
	Material = configFile.getVar("Material").asString();
	SoundName = configFile.getVar("SoundName").asString();

	MaxBullets = configFile.getVar("MaxBullets").asInt();
}

void	CBoxOfBulletsType::serial(NLMISC::IStream &f)
{	
	uint	ver= f.serialVersion(0);
	f.serial( TypeId );
	f.serial( ClassId );
	f.serial( Name );
	f.serial( Shape );
	f.serial( Weight );
	f.serial( Material );
	f.serial( SoundName );

	f.serial( BulletsType );
	f.serial( MaxBullets );
}

/*
 * CNutritionType World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */		


void	CNutritionType::createFromTextFile( CConfigFile configFile )
{

	TypeId = configFile.getVar("TypeId").asInt();
	ClassId = configFile.getVar("ClassId").asInt();
	Name = configFile.getVar("Name").asString();
	Shape = configFile.getVar("Shape").asString();
	Weight = configFile.getVar("Weight").asFloat();
	Material = configFile.getVar("Material").asString();
	SoundName = configFile.getVar("SoundName").asString();

	Carbos = configFile.getVar("Carbos").asInt();
	Proteins = configFile.getVar("Proteins").asInt();
	VitaminA = configFile.getVar("VitaminA").asInt();
	VitaminB = configFile.getVar("VitaminB").asInt();
	VitaminC = configFile.getVar("VitaminC").asInt();
	Fats = configFile.getVar("Fats").asInt();
	Water = configFile.getVar("Water").asInt();
}

void	CNutritionType::serial(NLMISC::IStream &f)
{	
	uint	ver= f.serialVersion(0);
	f.serial( TypeId );
	f.serial( ClassId );
	f.serial( Name );
	f.serial( Shape );
	f.serial( Weight );
	f.serial( Material );
	f.serial( SoundName );

	f.serial( Carbos, Proteins, VitaminA, VitaminB, VitaminC );
	f.serial( Fats, Water );
}
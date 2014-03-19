/** \world_types.h
 * <File description>
 *
 * $Id: world_types.h,v 1.1 2002/03/20 18:05:25 robofly Exp $
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

#ifndef NL_WORLD_TYPES_H
#define NL_WORLD_TYPES_H


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
 * Abstract World Type
 *
 * \author Róbert Bjarnason
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class AWorldType
{


public:

	virtual void				load( std::string fileName )=0;
	virtual void				save( std::string fileName )=0;

	virtual void				loadFromTextFile( std::string textFileName )=0;
	virtual void				createFromTextFile( NLMISC::CConfigFile configFile )=0;
	virtual void				serial(NLMISC::IStream &f)=0;

private:
};

/*
 * World Type
 *
 * \author Róbert Bjarnason
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class CWorldType
{


public:

	uint32					TypeId;
	uint8					ClassId;
	std::string				Name;
	std::string				Shape;
	float					Weight;
	std::string				Material;
	std::string				SoundName;

	CWorldType() {};

	void				load( std::string fileName );
	void				save( std::string fileName );

	void						loadFromTextFile( std::string textFileName );

private:
	virtual void				createFromTextFile( NLMISC::CConfigFile configFile );
	virtual void				serial(NLMISC::IStream &f);

private:
};



/*
 * Firearm World Type
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class CFirearmType : public CWorldType
{


public:

	uint32			BulletsTypeId;
	uint8			Accuracy;
	uint8			MaxBullets;
	float			FireRate1Round;
	float			FireRateAuto;
	float			Loudness;
	float			RecoilMod;
	std::string		ParticleSystem;
	NLMISC::CVector	ParticleSystemScale;
	uint16			DurabilityThreshold;
	std::string		FireSound;

	CFirearmType() {};

private:
	void				createFromTextFile( NLMISC::CConfigFile configFile );

	void				serial( NLMISC::IStream &f );

private:
};

class CBulletType : public CWorldType
{

/*
 * Bullet World Type
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

public:

	std::string			PsHard;
	std::string			PsSoft; 
	NLMISC::CVector		PsScale;
	float				ExploisionRadius;
	float				PropRating;
	std::string			HitSoundSoft;
	std::string			HitSoundHard;

	CBulletType() {};

private:
	void				createFromTextFile( NLMISC::CConfigFile configFile );

	void				serial( NLMISC::IStream &f );

};

/*
 * CBoxOfBulletsType World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class CBoxOfBulletsType : public CWorldType
{


public:

	uint32			BulletsType;
	uint8			MaxBullets;

	CBoxOfBulletsType() {};

private:

	void				createFromTextFile( NLMISC::CConfigFile configFile );

	void				serial( NLMISC::IStream &f );

};
		

/*
 * CNutritionType World object
 *
 * \author Róbert Bjarnason
 * \author Michael Warnock
 * \author Charles Folk
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 */

class CNutritionType : public CWorldType
{


public:

	uint8			Carbos;
	uint8			Proteins;
	uint8			VitaminA;
	uint8			VitaminB;
	uint8			VitaminC;
	uint8			Fats;
	uint8			Water;

	CNutritionType() {};

private:

	void				createFromTextFile( NLMISC::CConfigFile configFile );

	void				serial( NLMISC::IStream &f );

};

#endif // NL_WORLD_TYPES_H

/* End of worldsobject.h */


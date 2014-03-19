/** \client_world_objects.cpp
 * <File description>
 *
 * $Id: client_world_objects.cpp,v 1.1 2002/03/20 18:05:25 robofly Exp $
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

 
#include "client_world_objects.h"
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


// ***************************************************************************
void	CClientDynamicObject::serial(NLMISC::IStream &f)
{
	uint	ver= f.serialVersion(0);
	f.serial(Id, ClassId, Owner, TypeId );
	f.serial( Name );
	f.serial( MeshName );
	f.serial( Position, Heading );
	f.serial( TrajVector );
	f.serial( AngVelocity );
	f.serial( Visable );
	f.serial( Radius, Height, Scale, Weight);
	f.serial( SoundName );
}

// ***************************************************************************
void	CClientFirearm::serial(NLMISC::IStream &f)
{
	uint	ver= f.serialVersion(0);
	f.serial( Id, ClassId, Owner, TypeId );
	f.serial( Name );
	f.serial( MeshName );
	f.serial( Position, Heading );
	f.serial( TrajVector );
	f.serial( AngVelocity );
	f.serial( Visable );
	f.serial( Radius, Height, Scale, Weight);
	f.serial( SoundName );

	f.serial( Maintenance );
	f.serial( RoundPresent );
	f.serial( BulletsLeft );
	f.serial( Safety );
	f.serial( Jammed );
	f.serial( Chambered );
	f.serial( BulletsTypeId, MaxBullets  );
	f.serial( FireRate1Round, FireRateAuto, Loudness, RecoilMod );
	f.serial( ParticleSystem );
	f.serial( ParticleSystemScale );
	f.serial( FireSound );
}

// ***************************************************************************
void	CClientBoxOfBullets::serial(NLMISC::IStream &f)
{
	uint	ver= f.serialVersion(0);
	f.serial( Id, ClassId, Owner, TypeId );
	f.serial( Name );
	f.serial( MeshName );
	f.serial( Position, Heading );
	f.serial( TrajVector );
	f.serial( AngVelocity );
	f.serial( Visable );
	f.serial( Radius, Height, Scale, Weight);
	f.serial( SoundName );

	f.serial( BulletsType );
	f.serial( MaxBullets, BulletsLeft );
}

// ***************************************************************************
void	CClientNutrition::serial(NLMISC::IStream &f)
{
	uint	ver= f.serialVersion(0);
	f.serial( Id, ClassId, Owner, TypeId );
	f.serial( Name );
	f.serial( MeshName );
	f.serial( Position, Heading );
	f.serial( TrajVector );
	f.serial( AngVelocity );
	f.serial( Visable );
	f.serial( Radius, Height, Scale, Weight);
	f.serial( SoundName );

	f.serial( Carbos, Proteins, VitaminA, VitaminB, VitaminC );
	f.serial( Fats, Water );

	f.serial( Quality, Maintenance );
}




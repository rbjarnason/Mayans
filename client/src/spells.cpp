/** \file spells.cpp
 * Spell Manager
 *
 * $Id: spells.cpp,v 1.6 2002/04/12 07:34:00 robofly Exp $
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

//
// Includes
//

#include <math.h>
#include <nel/misc/command.h>
#include <nel/misc/vectord.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/misc/time_nl.h>

#include "client.h"
#include "sound.h"
#include "entities.h"
#include "camera.h"
#include "spells.h"
#include "envfx.h"
#include "network.h"
#include "landscape.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NL3D;


//
// Variables
//

NLMISC::TTime		_FireSpellTime;
NL3D::UInstance*	_ReadySpell;
NL3D::UInstance*	_GoSpell;
bool				_SpellState;
bool				_SpellPrepare;
//
// Functions
//

void	initSpells()
{
	_ReadySpell = NULL;
	_GoSpell = NULL;
	_SpellState = false;
	_SpellPrepare = false;
}

void rainSpell()
{
//	if (!_SpellPrepare) nlinfo("_SpellPrepare = false");
//	if (!_SpellState) nlinfo("_SpellState = false");

	if (!_SpellPrepare && !_SpellState)
		prepareRainSpell();
	else if (!_SpellState)
		fireRainSpell();
}

void prepareRainSpell()
{
	_SpellPrepare = true;

	Self->setAnimState(CEntity::RainSpell);
	if (isOnline ())
		sendAddSpellFX(Self->Id, "ReadySpell01.ps", "");

	if (_ReadySpell==NULL) _ReadySpell = Scene->createInstance("ReadySpell01.ps");
	_ReadySpell->setClusterSystem(TempleInstanceGroup);

	_ReadySpell->setPos (Self->Position);
	_ReadySpell->show();
}

void fireRainSpell()
{
	_SpellPrepare = false;
	if (isOnline ())
		sendAddSpellFX(Self->Id, "GoSpell01.ps", "");
	
	_FireSpellTime = CTime::getLocalTime();				
	if (_ReadySpell!=NULL) Scene->deleteInstance(_ReadySpell);
	_ReadySpell=NULL;
	if (_GoSpell==NULL) _GoSpell = Scene->createInstance("GoSpell01.ps");
	_GoSpell->setPos (Self->Position);
	_GoSpell->setClusterSystem(TempleInstanceGroup);
	CVector jdir = CVector(-(float)cos(Self->Angle+1.42f), -(float)sin(Self->Angle+1.42f), 0.0f);
	_GoSpell->setRotQuat(jdir);
	_SpellState=true;
}

void	updateSpells()
{
	if (_SpellState)
	{
		TTime	newTime = CTime::getLocalTime();
		float	delta = (float)(newTime-_FireSpellTime)/1000.0f;
		if (delta>9)
		{
			if (isOnline ())
			{
				sendRemoveSpellFX(Self->Id);
				if (isRaining ())
				{
					sendRemoveEnviromentalFX(Self->Id);
					rainOff();
				}
				else
				{
					sendAddEnviromentalFX(Self->Id,"rain","rain.ps","rain-loop");
					rainOn("rain.ps", "rain-loop");
				}
			}
			else
			{
				toggleRain();
			}

			_SpellState=false;
			Self->setAnimState(CEntity::Idle);

			if (_GoSpell!=NULL) Scene->deleteInstance(_GoSpell);
			_GoSpell=NULL;
		}
	}
}
		
void	releaseSpells()
{
}

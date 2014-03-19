/** \file envfx.cpp
 * Environmental FX Manager
 *
 * $Id: envfx.cpp,v 1.3 2002/03/30 07:24:45 robofly Exp $
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
#include <nel/3d/viewport.h>
#include <nel/misc/time_nl.h>

#include <nel/pacs/u_move_container.h>
#include <nel/pacs/u_move_primitive.h>
#include <nel/pacs/u_global_retriever.h>
#include <nel/pacs/u_global_position.h>


#include "client.h"
#include "sound.h"
#include "entities.h"
#include "mouse_listener.h"
#include "pacs.h"
#include "camera.h"
#include "landscape.h"


//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NL3D;
using namespace NLPACS;
using namespace NLSOUND;

//
// Variables
//


bool				raining = false;

// The particle system for the rain effect
static UInstance	*Rain = NULL;
static UInstance	*RainE = NULL;
static UInstance	*RainW = NULL;
static UInstance	*RainS = NULL;

std::string			_rainName = "rain.ps";

NLMISC::TTime		_ThunderStartTime;

bool _Inside = false;
bool _OnTop = false;

//
// Functions
//

void	initEnvFX()
{
	_ThunderStartTime = 0;
}

void	updateEnvFX()
{
	if (raining) 
	{
	
		UGlobalPosition	gPos;
		Self->MovePrimitive->getGlobalPosition(gPos, 0);
		
		uint32 theMaterial = GlobalRetriever->getMaterial(gPos);


		if (theMaterial==71)
		{
			if (!_Inside)
			{
				Rain->setPos (CVector(2320.0f, -1278.5f, 2.0f));
				_Inside=true;
				setEnvFXGain(0, *Self, 0.1f);
			}
		}
		else if (theMaterial==54)
		{
			if (!_OnTop)
			{
				_OnTop=true;
				RainE = Scene->createInstance(_rainName);
				RainE->setPos(2319.0f, -1233.0f, 45.5f);

				RainW = Scene->createInstance(_rainName);
				RainW->setPos(2319.0f, -1199.0f, 45.5f);

				RainS = Scene->createInstance(_rainName);
				RainS->setPos(2338.0f, -1215.0f, 45.5f);

				Rain->setPos(CVector(2302.0f, -1216.0f, 45.5f));
			} 
		}
		else
		{
			Rain->setPos (Camera->getMatrix().getPos()/*+CVector (0.0f, 0.0f, -10.0f)*/);

//			nlinfo("rain set pos %f %f %f", Camera->getMatrix().getPos().x, Camera->getMatrix().getPos().y, Camera->getMatrix().getPos().z); 
			if (_OnTop)
			{
				_OnTop=false;
				Scene->deleteInstance(RainE);
				RainE=NULL;
				Scene->deleteInstance(RainW);
				RainW=NULL;
				Scene->deleteInstance(RainS);
				RainS=NULL;
			}

			if (_Inside)
			{
				_Inside=false;
				setEnvFXGain(1, *Self, 0.2f);
			}
		}

//		Rain->setMatrix(mat);
	}
		
	if (_ThunderStartTime!=0)
	{
		TTime	newTime = CTime::getLocalTime();
		float	delta = (float)(newTime-_ThunderStartTime)/1000.0f;
		if (delta>0.62f)
		{
			_ThunderStartTime=0;
//			deleteSoundEnvFX(0, *Self);
			playSoundEnvFX(1,*Self, "rain-loop", true, 0.2f );
		}
	}
}
		
void rainOn(string psName, string soundName)
{
	if (Rain!=NULL)
	{
		Scene->deleteInstance(Rain);
		Rain=NULL;
	}

	_rainName = psName;
	Rain = Scene->createInstance(psName);
	
//	Rain->setTransformMode (UTransformable::DirectMatrix);
	raining = true;

	if (Self->SourceEnvFX0!=NULL)
	{
		deleteSoundEnvFX(0, *Self);
	}

	playSoundEnvFX(0, *Self, "farlightning02", false, 1.0f );
	_ThunderStartTime = CTime::getLocalTime();
}

void rainOff()
{
	if (Rain!=NULL)
	{
		Scene->deleteInstance(Rain);
		Rain=NULL;
	}
	raining = false;
	deleteSoundEnvFX(1, *Self);
}

void toggleRain()
{
	if (!raining)
	{
		rainOn("rain.ps", "rain-loop");
	}
	else
	{
		rainOff();
	}
}

bool	isRaining()
{
	return raining;
}

void	releaseEnvFX()
{
	if (Rain!=NULL)
	{
		Scene->deleteInstance(Rain);
		Rain=NULL;
	}
}

NLMISC_COMMAND(rain,"Toggle rain","")
{
	// check args, if there s not the right number of parameter, return bad
	if (args.size() != 0) return false;

	toggleRain();

	return true;
}
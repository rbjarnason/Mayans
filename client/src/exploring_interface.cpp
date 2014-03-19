/** \exploring_interface.cpp
 * <File description>
 *
 * $Id: exploring_interface.cpp,v 1.3 2002/06/06 05:51:01 robofly Exp $
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

#include <vector>

#include <nel/misc/types_nl.h>
#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>
#include <nel/misc/command.h>

#include <nel/3d/u_material.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_texture.h>

#include "camera.h"
#include "client.h"
#include "commands.h"
#include "mouse_listener.h"
#include "entities.h"
#include "stats.h"

//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;
using namespace std;

//
// Variables
//
uint ExploringInterfaceState=0;
uint32 SelectedDynamicObject=0;
bool OtherPlayerSelection=false;


// ***************************************************************************
struct		CPotentialTarget
{
	uint32		objectId;
	float		distanceFromPlayer;
	float		angleOffset;
	bool		angleMode;

	// For find().
	bool	operator==(const CPotentialTarget &o) const
	{
		return objectId==o.objectId;
	}
	// For sort().
	bool	operator<(const CPotentialTarget &o) const
	{
		if (angleMode) return angleOffset>o.angleOffset;
		else return distanceFromPlayer>o.distanceFromPlayer;
	}
};

static vector<CPotentialTarget> potentialTargets;

//
// Functions
//


/*********************************************************\
					initSelectableTargets ()
\*********************************************************/
void initSelectableTargets (bool angleMode)
{
	potentialTargets.clear();

	CEntity::TType	Type;

	if (!OtherPlayerSelection)
		Type=CEntity::Dynamic;
	else
		Type=CEntity::Other;

	for(EIT eit=Entities.begin(); eit!=Entities.end(); eit++)
	{
		if((*eit).second.Type == Type)
		{
			CVector objectPos = (*eit).second.Position;

			// position of neighbour
			float posx = objectPos.x;
			float posy = objectPos.y;

			// relative position
			posx = (posx-Self->Position.x)*0.4f/1000;
			posy = (posy-Self->Position.y)*0.4f/1000;

			float dist = (float) sqrt((posx*posx)+(posy*posy));

			CPotentialTarget	pt;
			pt.objectId = (*eit).second.Id;
			pt.distanceFromPlayer = dist;
			pt.angleMode = angleMode;

			potentialTargets.push_back(pt);
		}
	}

	if (!potentialTargets.empty())
	{
		vector<CPotentialTarget>::iterator	it= potentialTargets.begin();

		while(it != potentialTargets.end())
		{
//				nlinfo("before sort target id id=%d", it->objectId);
				*it++;
		}

		std::sort( potentialTargets.begin(), potentialTargets.end() );

		it= potentialTargets.begin();

		while(it != potentialTargets.end())
		{
//				nlinfo("after sort target id id=%d", it->objectId);
				*it++;
		}
	}
}

void selectClosestExploringTarget()
{
	if (ShowDebugInChat) nlinfo("in sel closest tar: selected Object is now=%d", SelectedDynamicObject);
	initSelectableTargets(false);

	if (!potentialTargets.empty())
	{
		vector<CPotentialTarget>::iterator	it= potentialTargets.begin();
		if (it != potentialTargets.end())
		{
			SelectedDynamicObject= it->objectId;
//			nlinfo("Now I have changed DO to %d", SelectedDynamicObject);
			setSelectedEntity(SelectedDynamicObject, "selected.ps");
		}
	} else SelectedDynamicObject = 0;
}

void selectNextExploringTarget()
{
	initSelectableTargets(false);
//	CPotentialTarget	pt;
//	pt.objectId = selectedDynamicObject;

//	vector<CPotentialTarget>::iterator	it= find(potentialTargets.begin(), potentialTargets.end(), pt);
	
	bool chFlag=false;
	if (!potentialTargets.empty())
	{
		vector<CPotentialTarget>::iterator	it= potentialTargets.begin();
		while(it != potentialTargets.end())
		{
			if(it->objectId== SelectedDynamicObject)
			{
//				nlinfo("Last target id=%d", it->objectId);
				*it++;
//				nlinfo("New target id=%d", it->objectId);
				if(it!=potentialTargets.end())
				{
					SelectedDynamicObject= it->objectId;
//					nlinfo("Now I have changed DO to %d", SelectedDynamicObject);
					it=potentialTargets.end();
					chFlag=true;
					setSelectedEntity(SelectedDynamicObject, "selected.ps");
				}
			}
			if (it!=potentialTargets.end()) *it++;
		}

		if (!chFlag)
		{
			it=potentialTargets.begin();
			if (it!=potentialTargets.end())
				SelectedDynamicObject=it->objectId;
				setSelectedEntity(SelectedDynamicObject, "selected.ps");
		}
	} else SelectedDynamicObject=0;

	if (ShowDebugInChat) nlinfo("in sel next tar: selected Object is now=%d", SelectedDynamicObject);
}

/*********************************************************\
					displayExploringInterface()
\*********************************************************/
void displayExploringInterface()
{
static	float xLeft = 0.2f;
static	float xRight = 0.8f;
static	float yTop = 0.92f;
static	float yBottom = 0.22f;
static	float xCenter = 0.4f;
static	float yCenter = 0.92f/2.0f;
static  float row1 = 0.01f;
static  float row2 = 0.15f;

	if (SelectedDynamicObject!=0)
	{
//		Driver->setMatrixMode2D11 ();

		// Print selected object
//		TextContext->setHotSpot(UTextContext::TopLeft);
//		TextContext->setColor(CRGBA(0,20,100,120));
//		TextContext->setFontSize( 17 );
//		TextContext->printfAt(xLeft-0.15f,yTop+0.03f,"Selected Object: %d", SelectedDynamicObject );
	}
}

void cbExploringInterface (CConfigFile::CVar &var)
{
	if (var.Name == "ExploringInterfaceState") ExploringInterfaceState = var.asInt ();
}

void initExploringInterface ()
{
	ConfigFile.setCallback ("ExploringInterfaceState", cbExploringInterface);

	if (ShowDebugInChat) nlinfo("Init ExploringInterfaceState (state)=%d", ExploringInterfaceState);
}

void updateExploringInterface ()
{
	switch (ExploringInterfaceState)
	{
	case 0:
		break;
	case 1:
		displayExploringInterface();
		break;
	case 2:
//		displayLargeStats ();
		break;
	}
}

void releaseExploringInterface ()
{
}


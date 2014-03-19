/** \avatar_interface.cpp
 * <File description>
 *
 * $Id: avatar_interface.cpp,v 1.5 2002/06/06 05:51:01 robofly Exp $
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
#include <time.h>

#include <nel/misc/types_nl.h>
#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>
#include <nel/misc/command.h>

#include <nel/3d/u_material.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_shape_bank.h>
#include <nel/3d/u_skeleton.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_texture.h>
#include <nel/misc/time_nl.h>

#include "client.h"
#include "commands.h"
#include "avatar_interface.h"
#include "entities.h"
#include "mouse_listener.h"
#include "landscape.h"

//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;
using namespace std;

//
// Variables
//
uint AvatarInterfaceState=1;

// window width
uint32 _Width;

// window height
uint32 _Height;

// true if user validated his choice
bool _Validated;

// index of selected character
uint _CharacterIndex;

/// charcater's string
std::vector<std::string> _CharactersName;

UInstance					*MaleInstance = NULL;
USkeleton					*MaleSkeleton = NULL;
UInstance					*FemaleInstance = NULL;

/*********************************************************\
					displayAvatarInterface()
\*********************************************************/
void displayAvatarInterface()
{
/*	if (SelectedDynamicObject!=0)
	{
		Driver->setMatrixMode2D11 ();

		// Print selected object
		TextContext->setHotSpot(UTextContext::TopLeft);
		TextContext->setColor(CRGBA(0,20,100,120));
		TextContext->setFontSize( 17 );
		TextContext->printfAt(xLeft-0.15f,yTop+0.03f,"Selected Object: %d", SelectedDynamicObject );
	}
*/
	// time
//	Driver->drawQuad (0.5f-DisplayWidth/2.0f, 0.5f-DisplayHeight/2.0f, 0.5f+DisplayWidth/2.0f, 0.5f+DisplayHeight/2.0f, DisplayColor);
/*	Driver->drawQuad(backX-backW/2,  
					backY-backH/2,
					backX+backW/2,
					backY+backH/2,
					CRGBA(0,0,255,128));
*/

	Driver->setMatrixMode2D11 ();
	Driver->drawQuad (0.1f, 0.62f, 0.9f, 0.68f, CRGBA(0,0,64,128));
	// title
	TextContext->setFontSize(22);
	TextContext->setHotSpot(UTextContext::MiddleMiddle);
	ucstring cString = "Choose your Avatar with \"Tab\" and then use \"Return\" to select";
	TextContext->printAt( 0.5f,0.65f,cString);
	

/*	TextContext->setFontSize(20);
	TextContext->setHotSpot(UTextContext::MiddleTop);
	ucstring s;
	float x,y,w,h;

//	UTexture *charMaleTexture = Driver->createTextureFile ("60001Icon.tga");
//	UTexture *charFemaleTexture = Driver->createTextureFile ("60002Icon.tga");

	x = 0.48f-128/(float)_Width;
	y = 0.45f-64/(float)_Height;
	w = 128/(float)_Width;
	h = 128/(float)_Height;
	Driver->drawBitmap (x, y, w, h, *charMaleTexture );
	
	if(_CharacterIndex==0)
	{
		s = "> ";
		s += _CharactersName[0];
		s += ' ';
		s += '<';
	}
	else
	{ 
		s = _CharactersName[0];
	}
//	TextContext->printAt(x+64/(float)_Width,y-0.03f,s);

	x = 0.52f;
	y = 0.45f-64/(float)_Height;
	w = 128/(float)_Width;
	h = 128/(float)_Height;
//	Driver->drawBitmap (x, y, w, h, *charFemaleTexture);
	if(_CharacterIndex==1)
	{
		s = "> ";
		s += _CharactersName[1];
		s += ' ';
		s += '<';
	}
	else
	{
		s = _CharactersName[1];
	}
//	TextContext->printAt(x+64/(float)_Width,y-0.03f,s);
*/	

}

void cbAvatarInterface (CConfigFile::CVar &var)
{
	if (var.Name == "AvatarInterfaceState") AvatarInterfaceState = var.asInt ();
}

void initAvatarInterface ()
{
	ConfigFile.setCallback ("AvatarInterfaceState", cbAvatarInterface);

	if (ShowDebugInChat) nlinfo("Init AvatarInterfaceState (state)=%d", AvatarInterfaceState);

	vector<string> shapes;

	shapes.push_back("Male");
	shapes.push_back("Female");

	_CharactersName = shapes;
	_CharacterIndex = 0;
	
	Driver->getWindowSize(_Width, _Height);

	_Validated = false;

/*	NL3D::UShapeBank *theBank = Driver->getShapeBank();
	theBank->addShapeCache("shaman.shape");
	theBank->addShapeCache("IndianPrincess.shape");
	theBank->linkShapeToShapeCache("shaman.shape", "shaman.shape");
	theBank->linkShapeToShapeCache("IndianPrincess.shape", "IndianPrincess.shape");
*/
	MaleInstance = Scene->createInstance ("shaman.shape");
	MaleSkeleton = Scene->createSkeleton ("shaman.skel");
	MaleSkeleton->bindSkin (MaleInstance);
	MaleSkeleton->setClusterSystem(TempleInstanceGroup);

	FemaleInstance = Scene->createInstance ("IndianPrincess.shape");
	FemaleInstance->setClusterSystem(TempleInstanceGroup);
}

void nextAvatar()
{
	if(_CharacterIndex!=_CharactersName.size()-1)
	{
		_CharacterIndex++;
	}
	else _CharacterIndex=0;

	deleteEntity( *Self );
	Self = NULL;

	string AvatarShape;

	srand (time(NULL));
	uint32 id = rand();
	float startX = ConfigFile.getVar("StartPoint").asFloat(0)+frand(3.0f);
	float startY = ConfigFile.getVar("StartPoint").asFloat(1)+frand(3.0f);

	if (_CharacterIndex==0)
	{
		AvatarShape = "shaman.shape";
		MouseListener->setViewTargetHeight(MouseListener->getViewTargetHeight());
		addEntity(id, "Entity"+toString(id), AvatarShape, CEntity::Self, CVector(startX,
												 startY,
												 ConfigFile.getVar("StartPoint").asFloat(2)),
										 CVector(startX,
												 startY,
												 ConfigFile.getVar("StartPoint").asFloat(2)), 1.0f,  CVector(1.0, 1.0,1.0), 1.0f, "", NULL);

	} 
	else
	{
		AvatarShape = "IndianPrincess.shape";
		MouseListener->setViewTargetHeight(MouseListener->getViewTargetHeight());
		addEntity(id, "Entity"+toString(id), AvatarShape, CEntity::Self, CVector(startX,
												 startY,
												 ConfigFile.getVar("StartPoint").asFloat(2)),
										 CVector(startX,
												 startY,
												 ConfigFile.getVar("StartPoint").asFloat(2)), 1.0f,  CVector(1.0, 1.0,1.0), 1.0f, "", NULL);
	}


}

void selectCurrentAvatar()
{
	_Validated = true;
}

void updateAvatarInterface ()
{
	switch (AvatarInterfaceState)
	{
	case 0:
		break;
	case 1:
		displayAvatarInterface();
		break;
	}
}

void releaseAvatarInterface ()
{
}


/** \inventory_display.cpp
 * <File description>
 *
 * $Id: inventory_display.cpp,v 1.2 2002/06/06 05:51:01 robofly Exp $
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
#include <nel/3d/u_instance.h>
#include <nel/3d/u_scene.h>
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
#include "inventory.h"
#include "inventory_display.h"
#include "network.h"

//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;
using namespace std;

//
// Variables
//
static NL3D::UTexture *PsychTexture = NULL;

// The inventory Scene
UScene				*InventoryScene = NULL;


CInventory CharacterInventory;
uint InventoryState=1;

uint8	placeInList;
//static NL3D::UInstance* invSelectPS;

uint32 _Clicked=0;
// static UInstance			*Inventory = NULL;

std::map<uint32, NL3D::UInstance*>			InventoryItems;
std::map<uint32, uint8>						PlaceInInventoryList;

static UTexture *SelectedTexture = NULL;

static float     ScreenWidth, ScreenHeight;


void initInventoryScene()
{
	map<uint32, NL3D::UInstance*>::iterator	itu;
	for(itu = InventoryItems.begin(); itu!=InventoryItems.end(); itu++)
	{
		InventoryScene->deleteInstance( (*itu).second);
	}
	InventoryItems.clear();
//	invSelectPS->setPos (0.0f, 0.0f, 0.0f);
//	invSelectPS->setScale( 1.0f, 1.0f, 1.0f );
	if (ShowDebugInChat) nlinfo("InvScene cleared");
	placeInList=0;
	map<uint32,uint8>::iterator	it;
	for(it = CharacterInventory._Inventory.begin(); it!=CharacterInventory._Inventory.end(); it++)
	{
	if (ShowDebugInChat) nlinfo(". %d",placeInList);
		placeInList++;
		if ((*it).second==1)
		{
//			DIT dynamicObject = DynamicObjects.find ((*it).first);
//			CClientDynamicObject	&dynamicObjecta = dynamicObject->second; // hack
//			if (dynamicObjecta.MeshName=="") dynamicObjecta.MeshName = "MagicWand01.shape";
//			UInstance* dynamicInventoryItem = InventoryScene->createInstance( dynamicObjecta.MeshName );
//			dynamicInventoryItem->setPos (-1.0f, 3.5f, placeInList*-0.5f);
//			dynamicInventoryItem->setScale( CVector(0.2f, 0.2f, 0.2f	 ));
//			InventoryItems.insert(make_pair (dynamicObjecta.Id, dynamicInventoryItem));
//			PlaceInInventoryList.insert(make_pair (dynamicObjecta.Id, placeInList));
		}
		else if ((*it).second==2)
		{
			FIT dynamicFirearmObject = Firearms.find ((*it).first);
			CClientFirearm &dynamicFirearmObjecta = dynamicFirearmObject->second; // hack
			if (dynamicFirearmObjecta.MeshName=="") dynamicFirearmObjecta.MeshName = "handgun.shape";
			NL3D::UInstance* dynamicInventoryItem = InventoryScene->createInstance( dynamicFirearmObjecta.MeshName );
			dynamicInventoryItem->setPos (-2.0f, 4.5f, placeInList*-0.4f);
			InventoryItems.insert(make_pair (dynamicFirearmObjecta.Id, dynamicInventoryItem));
			PlaceInInventoryList.insert(make_pair (dynamicFirearmObjecta.Id, placeInList));
		}
		if ((*it).first==CharacterInventory.getSelected())
		{
//			nlinfo("setting select particle system");
//			invSelectPS->show();
//			invSelectPS->setPos (-2.0f, 4.5f, (placeInList*-1.0f)+0.7f);
//			invSelectPS->setScale( 0.3f, 0.3f, 0.3f );
		}

	}
	sendSelectedInInventory( CharacterInventory.getSelected() );
	
}

void updateSelected()
{
	
	std::map<uint32, NL3D::UInstance*>::iterator	itInvItems;
	std::map<uint32, uint8>::iterator				itInvSelItems;
	itInvItems = InventoryItems.find(CharacterInventory.getSelected());
	if(itInvItems!=InventoryItems.end())
	{
		itInvSelItems = PlaceInInventoryList.find(CharacterInventory.getSelected());
		if(itInvSelItems!=PlaceInInventoryList.end())
		{	
//			invSelectPS->setPos (-2.0f, 4.5f, ((*itInvSelItems).second*-1.0f)+0.7f);
//			nlinfo("Have set inv ps pos %d", (*itInvSelItems).second);
		}
	}
	
}

// nuiloa

			
void displayInventory()
{

	float	x= 8/800.0f;
	float	y= 8/600.0f;
	float	w=128/800.0f;
	float	h=128/600.0f;	
	_Clicked=0;

	h=(128-42)/600.0f;	

	y=y+.03f;

	uint8 whereInOrder = 0;

	map<uint32,uint8>::iterator	it;
	for(it = CharacterInventory._Inventory.begin(); it!=CharacterInventory._Inventory.end(); it++)
	{
		whereInOrder++;
		string itemname;
		char Buf[20];
		sprintf(Buf, "%d", (*it).first);
		itemname = Buf;

		uint8 itemquantity = (*it).second;

		//nlinfo("displayinv: %s %d",itemname, itemquantity);
		w = 64/800.0f;
		h = 64/600.0f;
		string filename = itemname+"Icon.tga";
//		nlinfo("loading texture: %s",filename);
		

		NL3D::UTexture *iconTexture = Driver->createTextureFile (filename);
//		nlinfo("placing at %f",(1-(h+y)));
		if (iconTexture!=NULL)
		{
	        Driver->setMatrixMode2D ( CFrustum (0.0f, ScreenWidth, 0.0f, ScreenHeight, 0.0f, 1.0f, false));
//			Driver->drawBitmap (10, ScreenHeight-128-10, 128, 128, *iconTexture);
//			Driver->drawBitmap (ScreenWidth-128-10, ScreenHeight-16-10, 128, 16, *iconTexture);
			if ((*it).first == CharacterInventory.getSelected())
			{
//				nlinfo("drawing quad");
//				Driver->setMatrixMode2D11 ();
//				Driver->drawQuad( 0.0149f, 0.96f-(whereInOrder*0.08f), 0.077f, 1.04f-(whereInOrder*0.08f) ,CRGBA(100,0,0,100));
				Driver->drawBitmap (15, ScreenHeight-24-(whereInOrder*72), 64, 64, *SelectedTexture);				
			//	nlinfo("xLeft %f yBottom %f xRight %f yTop %f", xLeft,yBottom,xRight,yTop);
//				0.1 0.8, 0.2, 0.9
			}
			Driver->drawBitmap (15, ScreenHeight-24-(whereInOrder*72), 64, 64, *iconTexture);
//			nlinfo("drawBitmap at x %f y %f", x, y);
		//_ClickMap[(1-(h+y))]=(*it).first;
		}
//		if ((*it).first == CharacterInventory.getSelected()) TextContext->setColor(CRGBA(255,0,255,255));
//		else TextContext->setColor(CRGBA(255,255,255,255));
//		TextContext->printfAt(x+.03f+w/2.0f,1.0f-h-y+.01f,"%d %s", itemquantity, itemname);
		y=y+.07f;		
//		nlinfo("outof loop");
	}

	if(_Clicked)
	{
		float	x= 8/800.0f;
		float	y= 8/600.0f;
		float	w=128/800.0f;
		float	h=128/600.0f;
		
		y=y+.03f;
		w = 32/800.0f;
		h = 32/600.0f;

		string itemname = "";
		string filename = itemname+"Icon.tga";
//		uint32 itemquantity = CharacterInventory.getInventory()[_Clicked];
//		NL3D::UTexture *iconTexture = Driver->createTextureFile (filename);
//		Driver->drawBitmap (x, 1-(h+y), w, h, *iconTexture);
//		TextContext->printfAt(x+.02f+w/2.0f,(1.0f-h-y+.01f)+0.03f,"%d %s", itemquantity, itemname);
	}

}

/*********************************************************\
						click()
\*********************************************************/
void selectInventoryItem(float xm, float ym)
{

		float	tx= 8/800.0f;
		float	ty= 8/600.0f;
		float	wm=128/800.0f;
		float	hm=128/600.0f;

		hm=(128-42)/600.0f;	

		ty=ty+.03f;

		map<uint32,uint8>::iterator	it;
		for(it = CharacterInventory._Inventory.begin(); it!=CharacterInventory._Inventory.end(); it++)
		{
			string itemname = "";
			uint32 itemquantity = (*it).second;
			wm = 32/800.0f;
			hm = 32/600.0f;
			
			if (ShowDebugInChat) nlinfo("Invclicked %f %f %f",ym,xm, (1-(hm+ty)-.48));
			
			if(fabs(ym-(1-(hm+ty)-.48))<.02)
			{
				_Clicked = (*it).first;
				if (ShowDebugInChat) nlinfo("clicked %s",itemname);
			}
			ty=ty+.07f;
		}
}

void	updateInventoryScene ()
{
	Driver->clearZBuffer();

	static float angle=0.0;
	angle+=0.05f;
	InventoryScene->animate (float(NewTime)/1000);
	InventoryScene->render ();

}

void cbUpdateInventoryDisplay (CConfigFile::CVar &var)
{
	if (var.Name == "InventoryState") InventoryState = var.asInt ();
}

void initInventoryDisplay ()
{
	ConfigFile.setCallback ("InventoryDisplayState", cbUpdateInventoryDisplay);
	if (ShowDebugInChat) nlinfo("Init Inventory Display (state)=%d", InventoryState);
	
	//
	// Setup the inventory scene
	//

	InventoryScene = Driver->createScene();

	CViewport v;
	v.init (0.0f, 0.0f, 1.0f, 1.0f);
	InventoryScene->setViewport (v);

	uint32 width, height;
	Driver->getWindowSize (width, height);
	ScreenWidth = (float) width;
	ScreenHeight = (float) height;
	SelectedTexture = Driver->createTextureFile ("selectedIcon.tga");
//	invSelectPS = InventoryScene->createInstance( "invselect.ps" );
//	invSelectPS->hide();

}

void updateInventoryDisplay ()
{
	switch (InventoryState)
	{
	case 0:
		break;
	case 1:
		displayInventory ();
		updateInventoryScene();
		break;
	}
}

void releaseInventoryDisplay ()
{
		Driver->deleteScene (InventoryScene);
}


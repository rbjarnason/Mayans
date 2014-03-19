/** \file client.cpp
 * Bang Client
 *
 * $Id: client.cpp,v 1.13 2002/06/06 05:51:01 robofly Exp $
 */

/* Copyright, 2001 Nevrax Ltd.
 * Copyright, 2001, 2002 InOrbit Entertainment, Inc.
 *
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

#include <nel/misc/types_nl.h>

#if defined(NL_OS_WINDOWS) && defined (NL_RELEASE)
#include <windows.h>
#  ifdef min 
#    undef min 
#  endif 
#  ifdef max 
#    undef max 
#  endif 
#endif

#include <time.h>
#include <string>
#include <vector>

#include <nel/misc/config_file.h>
#include <nel/misc/path.h>
#include <nel/misc/vectord.h>
#include <nel/misc/time_nl.h>
#include <nel/misc/command.h>
#include <nel/misc/file.h>

#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_transform.h>
#include <nel/3d/u_material.h>
#include <nel/3d/u_texture.h>
#include <nel/net/login_client.h>
#include "3d/transform_user.h"
#include <nel/pacs/u_move_container.h>
#include <nel/pacs/u_move_primitive.h>
#include <nel/pacs/u_global_retriever.h>
#include <nel/pacs/u_global_position.h>
#include <nel/3d/u_visual_collision_manager.h>
#include <nel/3d/u_visual_collision_entity.h>

#include "commands.h"
#include "landscape.h"
#include "entities.h"
#include "camera.h"
#include "pacs.h"
#include "animation.h"
#include "network.h"
#ifdef NL_OS_WINDOWS
#include "sound.h"
#endif
#include "interface.h"
#include "lens_flare.h"
#include "mouse_listener.h"
#include "compass.h"
#include "graph.h"
#include "inventory_display.h"
#include "exploring_interface.h"
#include "radar.h"
#include "envfx.h"
//#include "cluster_display.h"
#include "spells.h"
#include "avatar_interface.h"

#include <3d/register_3d.h>
#include <3d/init_3d.h>

//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NL3D;
using namespace NLNET;
using namespace NLPACS;

//
// Globals
//

// This class contains all variables that are in the client.cfg
CConfigFile			 ConfigFile;

// The 3d driver
UDriver				*Driver = NULL;

// This is the main scene
UScene				*Scene = NULL;

// This class is used to handle mouse/keyboard input for camera movement
C3dMouseListener	*MouseListener = NULL;

// This variable is used to display text on the screen
UTextContext		*TextContext = NULL;

// The previous and current frame dates
TTime				LastTime, NewTime;

// true if you want to exit the main loop
static bool			NeedExit = false;

// true if the commands (chat) interface must be display. This variable is set automatically with the config file
bool				ShowCommands = false;

bool				ShowDebugInChat = false;

// if true, the mouse can't go out the client window (work only on Windows)
static bool			CaptureState = false;

// Game mode defintions
enum GameModes	{ Exploring, Communication, Combat };

// The current active game mode
GameModes			GameMode=Communication;

bool				Falling = false;
bool				LoginComplete = false;

bool				SpeedUp = false;

bool				FullScreen;


uint8				ViewLagHeightEntry=0;
//
// Prototypes
//

void startLoginInterface ();
void updateLoginInterface ();

void initLoadingState ();
void displayLoadingState (char *state);

//
// Functions
//

//
// Main
//

#if defined(NL_OS_WINDOWS) && defined (NL_RELEASE)
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdline, int nCmdShow)
#else
int main(int argc, char **argv)
#endif
{
	nlinfo ("Starting Chichén Itzá");

	// Load config file
	ConfigFile.load ("client.cfg");

	// Set the ShowCommands with the value set in the clietn config file
	ShowCommands = ConfigFile.getVar("ShowCommands").asInt () == 1;

	// Set the ShowCommands with the value set in the clietn config file
	ShowDebugInChat = ConfigFile.getVar("ShowDebugInChat").asInt () == 1;

	// Add different path for automatic file lookup
	string dataPath = ConfigFile.getVar("DataPath").asString ();
	if (dataPath[dataPath.size()-1] != '/') dataPath += '/';
	CPath::addSearchPath (dataPath);
	CPath::addSearchPath (dataPath + "zones/");
	CPath::addSearchPath (dataPath + "tiles/");
	CPath::addSearchPath (dataPath + "shapes/");
	CPath::addSearchPath (dataPath + "maps/");
	CPath::addSearchPath (dataPath + "pacs/");
	CPath::addSearchPath (dataPath + "anims/");
	CPath::addSearchPath (dataPath + "sounds/");

	// Falling or not
	Falling = ConfigFile.getVar("Falling").asInt() == 1;

//	nlinfo("Falling = %d", Falling);
	// Create a driver
	Driver = UDriver::createDriver();

	Driver->enableLowLevelMouse(true);
	Driver->enableLowLevelKeyboard(true);
 
	// Create the window with config file values
	Driver->setDisplay (UDriver::CMode(ConfigFile.getVar("ScreenWidth").asInt(), 
									   ConfigFile.getVar("ScreenHeight").asInt(),
									   ConfigFile.getVar("ScreenDepth").asInt(),
									   ConfigFile.getVar("ScreenFull").asInt()==0));

	FullScreen = ConfigFile.getVar("ScreenFull").asInt()==0;

	// Set the cache size for the font manager (in bytes)
	Driver->setFontManagerMaxMemory (2000000);

	// Init the mouse so it's trapped by the main window.
	Driver->showCursor(false);
	Driver->setCapture(true);
	Driver->setMousePos(0.5f, 0.5f);

	// Create a Text context for later text rendering
	TextContext = Driver->createTextContext (CPath::lookup(ConfigFile.getVar("FontName").asString ()));

	// You can't call displayLoadingState() before init the loading state system
	initLoadingState ();

	// Create a scene
	Scene = Driver->createScene();

	/// Set the PolygonBalancingMode
//	Scene->setPolygonBalancingMode(NL3D::UScene::PolygonBalancingOn);	
	Scene->enableLightingSystem(true);

	// Init cluster display
//	displayLoadingState ("Initialize Cluster Display");
//	initClusterDisplay();

	// Init the landscape using the previously created UScene
	displayLoadingState ("Initialize Landscape");
	initLandscape();

	// Init the pacs
	displayLoadingState ("Initialize PACS");
	initPACS();

	// Init the aiming system
	displayLoadingState ("Initialize Aiming");
	initAiming();

	// Init the user camera
	displayLoadingState ("Initialize Camera");
	initCamera();

	// Create a 3D mouse listener
	displayLoadingState ("Initialize MouseListener");
	MouseListener = new C3dMouseListener();
	MouseListener->addToServer(Driver->EventServer);
	MouseListener->setCamera(Camera);
	MouseListener->setHotSpot (CVectorD (0,0,0));
	MouseListener->setFrustrum (Camera->getFrustum());
	MouseListener->setMatrix (Camera->getMatrix());
	MouseListener->setSpeed(PlayerSpeed);
	initMouseListenerConfig();

	// Init interface
	displayLoadingState ("Initialize Interface");
	initInterface();

	// Init Inventory Display
	displayLoadingState ("Initialize Inventory Display");
	initInventoryDisplay();

	// Init Exploring Interface
	displayLoadingState ("Initialize Exploring Interface");
	initExploringInterface();

	// Init Radar Interface
	displayLoadingState ("Initialize Radar");
	initRadar();
	
	// Init compass
	displayLoadingState ("Initialize Compass");
	initCompass();

	// Init spell manager
	displayLoadingState ("Initialize Spells");
	initSpells();

	// Init graph
	displayLoadingState ("Initialize Graph");
	initGraph();

#ifdef NL_OS_WINDOWS
	// Init sound control
	displayLoadingState ("Initialize Sound");
	initSound();
#endif

	// Init the command control
	displayLoadingState ("Initialize Command/Chat Console");
	initCommands ();

	// Init the Environamental Manager
	displayLoadingState ("Initialize Environmental FX");
	initEnvFX ();

	// Init the entities prefs
	displayLoadingState ("Initialize Entities");
	initEntities();

	// Init animation system
	displayLoadingState ("Initialize Animation");
	initAnimation ();

	// Init the lens flare
	displayLoadingState ("Initialize LensFlare");
	initLensFlare ();

	// Init avatar interface selection
	displayLoadingState ("Initialize Avatar Interface");
	initAvatarInterface ();

	// Creates the self entity
	displayLoadingState ("Adding your entity");
	srand (time(NULL));
	uint32 id = rand();
	float startX = ConfigFile.getVar("StartPoint").asFloat(0)+frand(3.0f);
	float startY = ConfigFile.getVar("StartPoint").asFloat(1)+frand(3.0f);
	addEntity(id, "Entity"+toString(id), "shaman.shape", CEntity::Self, CVector(startX,
												 startY,
												 ConfigFile.getVar("StartPoint").asFloat(2)),
										 CVector(startX,
												 startY,
												 ConfigFile.getVar("StartPoint").asFloat(2)), 1.0f,  CVector(1.0, 1.0,1.0), 1.0f, "", NULL);

	Self->AuxiliaryAngle = (float)Pi;

	// Init the network structure
	displayLoadingState ("Initialize Network");
	initNetwork();

	displayLoadingState ("Ready !!!");

	// Display the first line
	nlinfo ("Welcome to Chichen Itza");
	nlinfo ("");
//	nlinfo ("Press SHIFT-ESC to exit the simulation");

	// Get the current time
	NewTime = CTime::getLocalTime();

	// If auto login, we launch the login request interface
//	if (ConfigFile.getVar("AutoLogin").asInt() == 1)
//		startLoginInterface ();
//	else
//		LoginComplete=true;

	while ((!NeedExit) && Driver->isActive())
	{
		// Update the login request interface
		updateLoginInterface ();
		
		// Clear all buffers
		Driver->clearBuffers (CRGBA (0, 0, 0));

		// Update the time counters
		LastTime = NewTime;
		NewTime = /*CTime::ticksToSecond (CTime::getPerformanceTime())*1000;*/CTime::getLocalTime();

		// Update animation
//		updateAnimation ();

		// Update all entities positions
		MouseListener->update();
		updateEntities();

		// setup the camera
		// -> first update camera position directly from the mouselistener
		// -> then update stuffs linked to the camera (snow, sky, lens flare etc.)
		MouseListener->updateCamera();
		updateCamera();

		// Update the landscape
		updateLandscape ();

		// Update environmental FX
		updateEnvFX();

#ifdef NL_OS_WINDOWS
		// Update the sound
		updateSound ();
#endif
		// Set new animation date
		Scene->animate (float(NewTime)/1000);

		// Render the sky scene before the main scene
		updateSky ();

		// Render
		Scene->render ();

		// Render the lens flare
		updateLensFlare ();

		updateExploringInterface();

		// Update the compass
		updateCompass	 ();

		// Update spell manager
		updateSpells ();

		// Update the radar
		updateRadar ();

		// Update the graph
		updateGraph ();

		// Update the commands panel
		if (ShowCommands) updateCommands ();

		updateAnimation ();

		// Render the name on top of the other players
		renderEntitiesNames();

		// Update interface
		updateInterface ();

		// Update Avatar interface
		updateAvatarInterface ();

		// Display if we are online or offline
		TextContext->setHotSpot (UTextContext::TopRight);
		TextContext->setColor (isOnline()?CRGBA(0, 255, 0):CRGBA(255, 0, 0));
		TextContext->setFontSize (16);
//		TextContext->printfAt (0.99f, 0.99f, isOnline()?"Online":"Offline");
		std::string gmText="";
		if (GameMode==Exploring) gmText="Exploring";
		else if (GameMode==Communication) gmText="Communication";
		else if (GameMode==Combat) gmText="Combat";

		TextContext->printfAt (0.99f, 0.99f, "%s", gmText);

		// Display the frame rate
		uint32 dt = (uint32)(NewTime-LastTime);
		float fps = 1000.0f/(float)dt;
		TextContext->setHotSpot (UTextContext::TopLeft);
		TextContext->setColor (CRGBA(255, 255, 255, 255));
		TextContext->setFontSize (14);
		TextContext->printfAt (0.01f, 0.99f, "%.2ffps %ums", fps, dt);
		// one more frame
		FpsGraph.addValue (1.0f);
		SpfGraph.addOneValue ((float)dt);

//		updateClusterDisplay();

		updateInventoryDisplay();
		// Update network messages
		updateNetwork ();

		// Swap 3d buffers
		Driver->swapBuffers ();

		// Pump user input messages
		Driver->EventServer.pump();

		// Manage the keyboard

		if (Driver->AsyncListener.isKeyDown (KeySHIFT) && Driver->AsyncListener.isKeyDown (KeyESCAPE))
		{
			// Shift Escape -> quit
			NeedExit = true;
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyESCAPE))
		{
			srand (time(NULL));
			uint32 id = rand();
			float x = ConfigFile.getVar("StartPoint").asFloat(0)+frand(3.0f);
			float y = ConfigFile.getVar("StartPoint").asFloat(1)+frand(3.0f);

			if (Self->MovePrimitive != NULL && Self->VisualCollisionEntity != NULL)
			{
				UGlobalPosition	gPos;
				Self->MovePrimitive->setGlobalPosition(CVectorD(x, y, 0.0f), 0);
				// get the global position in pacs coordinates system
				Self->MovePrimitive->getGlobalPosition(gPos, 0);
				// convert it in a vector 3d
					Self->Position = GlobalRetriever->getGlobalPosition(gPos);
				// get the good z position
					gPos.LocalPosition.Estimation.z = 0.0f;
				Self->Position.z = GlobalRetriever->getMeanHeight(gPos);
				// snap to the ground
					if (!GlobalRetriever->isInterior(gPos))
					Self->VisualCollisionEntity->snapToGround( Self->Position );
				MouseListener->setPosition(Self->Position);

			}

			MouseListener->setViewLagBehind(ConfigFile.getVar("ViewLagBehind0").asFloat ());
			MouseListener->setViewHeight(ConfigFile.getVar("ViewHeight0").asFloat ());
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyTAB))
		{
			if (AvatarInterfaceState!=0)
			{
				nextAvatar();
				Self->AuxiliaryAngle = (float)Pi;
			}
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyNUMLOCK))
		{	
			ViewLagHeightEntry++;
			if (ViewLagHeightEntry>3)
			{
				ViewLagHeightEntry=0;
			}
			
			MouseListener->setViewLagBehind(ConfigFile.getVar("ViewLagBehind"+toString(ViewLagHeightEntry)).asFloat ());
			MouseListener->setViewHeight(ConfigFile.getVar("ViewHeight"+toString(ViewLagHeightEntry)).asFloat ());
		}
		else if (Driver->AsyncListener.isKeyDown (KeySHIFT) && Driver->AsyncListener.isKeyPushed (KeyRETURN))
		{
/*			// Create the window with config file values
			Driver->setDisplay (UDriver::CMode(ConfigFile.getVar("ScreenWidth").asInt(), 
									   ConfigFile.getVar("ScreenHeight").asInt(),
									   ConfigFile.getVar("ScreenDepth").asInt(),
									   !FullScreen));
*/
			FullScreen=!FullScreen;

		}
		else if (Driver->AsyncListener.isKeyPushed (KeyRETURN))
		{
			if (AvatarInterfaceState!=0)
			{
				AvatarInterfaceState=0;
				startLoginInterface ();
			}
		}
		else if (Driver->AsyncListener.isKeyDown (KeySHIFT) && Driver->AsyncListener.isKeyPushed (KeyF1))
		{
			// F1 -> Display the login request interface (to go online)
			if (!isOnline())
				startLoginInterface ();
		}
		else if (Driver->AsyncListener.isKeyDown (KeySHIFT) && Driver->AsyncListener.isKeyPushed (KeyF2))
		{
			// F2 -> disconnect if online (to go offline)
//			if (isOnline())
//				Connection->disconnect ();
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyF1))
		{
			GameMode=Exploring;
			ExploringInterfaceState=1;
			ShowCommands=0;
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyF2))
		{
			// F2 -> disconnect if online (to go offline)
//			if (isOnline())
//				Connection->disconnect ();
			ShowCommands = 1;
			GameMode=Communication;
			ExploringInterfaceState=1;
		}
		else if (Driver->AsyncListener.isKeyDown (KeySHIFT) && Driver->AsyncListener.isKeyPushed (KeyF3))
		{
			// F9 -> release/capture the mouse cursor
			if (!CaptureState)
			{
				Driver->setCapture(false);
				Driver->showCursor(true);
				MouseListener->removeFromServer(Driver->EventServer);
			}
			else
			{
				Driver->setCapture(true);
				Driver->showCursor(false);
				MouseListener->addToServer(Driver->EventServer);
			}
			CaptureState = !CaptureState;
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyF3))
		{
			if (InventoryState!=0)
			{
				uint32 selectedObject = CharacterInventory.selectNext();
				sendSelectedInInventory(selectedObject);
				updateSelected();
			}
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyF4))
		{
			// select next dynamic object
			OtherPlayerSelection=false;
			selectNextExploringTarget();
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyF5))
		{
			// select next player object
			OtherPlayerSelection=true;
			selectNextExploringTarget();
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyF4))
		{
			// F4 -> clear the command (chat) output
//			clearCommands ();
//			nlinfo("IS = %d", InventoryState);
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyF6))
		{
			// F6 -> display/hide the radar interface
			RadarState = (RadarState+1)%3;
		}
		else if(Driver->AsyncListener.isKeyDown(KeyF7))
		{
			// F7 -> radar zoom out
			RadarDistance += 50;
		}
		else if (Driver->AsyncListener.isKeyDown (KeySHIFT) && Driver->AsyncListener.isKeyPushed (KeyF8))
		{
			// F11 -> reset the PACS global position of the self entity (in case of a pacs failure :-\)
			if (Self != NULL)
				resetEntityPosition(Self->Id);
		}
		else if(Driver->AsyncListener.isKeyDown(KeyF8))
		{
			// F8 -> radar zoom in
			RadarDistance -= 50;
		}
		else if (Driver->AsyncListener.isKeyDown (KeySHIFT) && Driver->AsyncListener.isKeyPushed (KeyF9))
		{
			UDriver::TPolygonMode p = Driver->getPolygonMode ();
			p = UDriver::TPolygonMode(((int)p+1)%3);
			Driver->setPolygonMode (p);
		}
		else if(Driver->AsyncListener.isKeyPushed(KeyF9))
		{
			rainSpell();
		}
		else if (Driver->AsyncListener.isKeyDown (KeySHIFT) && Driver->AsyncListener.isKeyPushed (KeyF11))
		{
			if (SelectedURL!="") 
				::ShellExecute(NULL, NULL, SelectedURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyF11))
		{
			selectNextURL ();
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyF12))
		{
			// F12 -> take a screenshot
			CBitmap btm;
			Driver->getBuffer (btm);
			string filename = CFile::findNewFile ("screenshot.tga");
			COFile fs (filename);
			btm.writeTGA (fs,24,true);
			if (ShowDebugInChat) nlinfo("Screenshot '%s' saved", filename.c_str());
		}
//		else if (Driver->AsyncListener.isKeyDown (KeyCONTROL) && Driver->AsyncListener.isKeyPushed (KeyP))
		else if (Driver->AsyncListener.isKeyPushed (KeyDELETE))
		{
			if (ShowDebugInChat) nlinfo("Picking %d", SelectedDynamicObject);
			if (SelectedDynamicObject!=0)
			{
				sendPickDynamicObject( Self->Id, SelectedDynamicObject );
				selectNextExploringTarget();
			}
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyEND))
		{
			if (ShowDebugInChat) nlinfo("Giving selected inventory object to %d", SelectedDynamicObject);
			if (SelectedDynamicObject!=0 && SelectedDynamicObject < 7200)
			{
				sendGive( SelectedDynamicObject );
			}
		}
		else if (Driver->AsyncListener.isKeyDown (KeyCONTROL) && Driver->AsyncListener.isKeyPushed (KeyL))
		{
			if (SelectedDynamicObject!=0)
			{
				EIT eit = findEntity (SelectedDynamicObject);
				CEntity	&entity = (*eit).second;
				MouseListener->lookAt( entity.Position );
			}
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyS) && Driver->AsyncListener.isKeyDown (KeyCONTROL))
		{
			sendSummonShaman(2013);
		}
//		else if (Driver->AsyncListener.isKeyDown (KeyCONTROL) && Driver->AsyncListener.isKeyPushed (KeyD))
		else if (Driver->AsyncListener.isKeyPushed (KeyINSERT))
		{
			if (ShowDebugInChat) nlinfo("Dropping selected inventory object: %d",CharacterInventory.getSelected() );
			if ( CharacterInventory.getSelected()!=0)
			{
//				if (CharacterInventory.getHand( false ) == CharacterInventory.getSelected())
//					sendHoldDynamicObject( CharacterInventory.getSelected(), false );
				setEntityHand(Self->Id, false, 0, "");
				sendDropDynamicObject( Self->Id, CharacterInventory.getSelected() );
			}
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyHOME)  && 
				InventoryState!=0 && CharacterInventory.getSelected()!=0)
		{
				if (ShowDebugInChat) nlinfo("sending held");
				sendHoldDynamicObject( CharacterInventory.getSelected(), false );
				CharacterInventory.setHand( CharacterInventory.getSelected(), false );
		}
		else if (Driver->AsyncListener.isKeyPushed (KeyCONTROL))
		{
			if (SpeedUp)
			{
				MouseListener->setSpeed( PlayerSpeed );
				SpeedUp=false;
			} else
			{
				MouseListener->setSpeed( PlayerSpeed * 4 );
				SpeedUp=true;
			}
		}
/*		else if (Driver->AsyncListener.isKeyPushed (KeyH))
		{
			playAnimation (*Self, HitAnim, true);

			// todo get isWalking in the entity
			if (Driver->AsyncListener.isKeyDown (KeyUP) || Driver->AsyncListener.isKeyDown (KeyDOWN) ||
				Driver->AsyncListener.isKeyDown (KeyLEFT) || Driver->AsyncListener.isKeyDown (KeyRIGHT))
				playAnimation (*Self, WalkAnim);
			else
				playAnimation (*Self, IdleAnim);
		}
*/
		// Check if the config file was modified by another program
		CConfigFile::checkConfigFiles ();
	}

	// Release the mouse cursor
	if (CaptureState)
	{
		Driver->setCapture(false);
		Driver->showCursor(true);
	}

	// Release all before quit

	releaseLensFlare ();
	releaseExploringInterface ();
	releaseGraph ();
	releaseCompass ();
	releaseSpells ();
	releaseRadar ();
	releaseInterface ();
	releaseAvatarInterface ();
	releaseInventoryDisplay();
//	releaseClusterDisplay();

	releaseNetwork ();
	releaseAnimation ();
	releaseAiming();
	releasePACS();
	releaseLandscape();
	releaseEnvFX();

#ifdef NL_OS_WINDOWS	
	releaseSound ();
#endif

	// Release the mouse listener
	MouseListener->removeFromServer(Driver->EventServer);
	delete MouseListener;
	Scene->enableLightingSystem(false);

	Driver->deleteScene(Scene);
	// Release the 3d driver
	delete Driver;

	// Exit
	return EXIT_SUCCESS;
}

//
// Loading state procedure
//

static float	 ScreenWidth, ScreenHeight;

void initLoadingState ()
{
	uint32 width, height;
	Driver->getWindowSize (width, height);
	ScreenWidth = (float) width;
	ScreenHeight = (float) height;
}


void displayLoadingState (char *state)
{
	Driver->clearBuffers (CRGBA(0,0,0));

	Driver->setMatrixMode2D (CFrustum (0.0f, ScreenWidth, 0.0f, ScreenHeight, 0.0f, 1.0f, false));

	TextContext->setColor (CRGBA (255, 255, 255));
	TextContext->setHotSpot (UTextContext::MiddleMiddle);

	TextContext->setFontSize (40);
	TextContext->printAt (0.5f, 0.5f, ucstring("Welcome to the Mayans"));
	
	TextContext->setFontSize (30);
	TextContext->printAt (0.5f, 0.2f, ucstring(state));
	
	Driver->swapBuffers ();
}


//
// Login procedure
//
static uint loginState = 0;
static string login;

void startLoginInterface ()
{
	loginState = 1;
	askString ("Please enter your login:", ConfigFile.getVar("Login").asString ());
	login = "";
}

void updateLoginInterface ()
{
	string str;
	if (haveAnswer (str))
	{
//		nlinfo ("result %s", str.c_str());

		// esc pressed, stop all only before the shard selection
		if (str.empty () && loginState < 3)
		{
			loginState = 0;
			LoginComplete = true;
			return;
		}

		switch (loginState)
		{
		case 1:
//			nlinfo ("login entered");
			login = str;
			Self->Name = login;
			askString ("Please enter your password:", ConfigFile.getVar("Password").asString (), 1);
			loginState++;
			break;
		case 2:
			{
//				nlinfo ("password entered");
				string password = str;

				string LoginSystemAddress = ConfigFile.getVar("LoginSystemAddress").asString ();
				string res = CLoginClient::authenticate (LoginSystemAddress+":49999", login, password, 1);

				if (!res.empty ())
				{
					string err = string ("Authentification failed: ") + res;
					askString (err, "", 2, CRGBA(64,0,0,128));
					loginState = 0;
					LoginComplete = true;
				}
				else
				{
					string list = "Please enter the shard number you want to connected to\n\n";
					for (uint32 i = 0; i < CLoginClient::ShardList.size (); i++)
					{
						list += "Shard "+toString (i)+": "+CLoginClient::ShardList[i].ShardName+" ("+CLoginClient::ShardList[i].WSAddr+")\n";
					}
					askString (list, toString(ConfigFile.getVar("ShardNumber").asInt()));
					loginState++;
				}
			}
			break;
		case 3:
			{
//				nlinfo ("shard selected");
				uint32 shardNum = atoi (str.c_str());

				string res = CLoginClient::connectToShard (shardNum, *Connection);
				if (!res.empty ())
				{
					string err = string ("Connection to shard failed: ") + res;
					askString (err, "", 2, CRGBA(64,0,0,128));
					loginState = 0;
					LoginComplete = true;
				}
				else
				{
					// we remove all offline entities
					removeAllEntitiesExceptUs ();
					
//					askString ("You are online!!!", "", 2, CRGBA(0,64,0,128));
					loginState = 0;
					LoginComplete = true;

					// now we have to wait the identification message to know my id
				}
			}
			break;
		}
	}
}


// Command to quit the client
NLMISC_COMMAND(sb_quit,"quit the client","")
{
	// check args, if there s not the right number of parameter, return bad
	if(args.size() != 0) return false;

	log.displayNL("Exit requested");

	NeedExit = true;

	return true;
}

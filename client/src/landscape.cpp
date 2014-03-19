/** \file landscape.cpp
 * Landscape interface between the game and NeL
 *
 * $Id: landscape.cpp,v 1.5 2002/06/06 05:51:01 robofly Exp $
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

#include "nel/misc/types_nl.h"

#include <string>
#include <deque>
#include <vector>

#include "nel/misc/command.h"
#include "nel/misc/debug.h"
#include "nel/misc/path.h"
#include "nel/misc/i18n.h"
#include "nel/misc/config_file.h"

#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_material.h>
#include <nel/3d/u_landscape.h>
#include <nel/3d/u_water.h>
#include <../src/3d/landscape.h>
#include <../src/3d/water_pool_manager.h>
#include "3d/scene_group.h"

#include <nel/3d/u_instance_group.h>
#include <nel/3d/u_light.h>
#include <nel/3d/u_point_light.h>


#include <nel/3d/u_visual_collision_entity.h>
#include <nel/3d/u_visual_collision_manager.h>

#include "client.h"
#include "pacs.h"
#include "commands.h"
#include "mouse_listener.h"
#include "physics.h"
// #include "cluster_display.h"

// Landscape Tile Painter module
//#include "painter.h"
#include "3d/zone_search.h"
#include "../src/3d/landscape_user.h"
#include <../src/3d/visual_collision_entity.h>
#include <../src/3d/visual_collision_manager.h>


//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NL3D;

//
// Variables
//

ULandscape				*Landscape = NULL;
UVisualCollisionEntity	*AimingEntity = NULL;
vector<UInstanceGroup*>	 InstanceGroups;
UInstanceGroup			*PlanetInstance = NULL;
UInstanceGroup			*IslandsInstance = NULL;
UInstanceGroup			*TempleInstanceGroup = NULL;
uint8					templeInstanceIndex=0;

CWaterPoolManager						*WaterPoolManager;

ULight					*Sun = NULL;
//UPointLight					*PointL1 = NULL;
UPointLight					*PointL2 = NULL;
//UPointLight					*PointL3 = NULL;
//UPointLight					*PointL4 = NULL;



NLMISC::CVector			 SunDirection;
NLMISC::CVector			 PointDirection;

NLMISC::CVector			 PointPosition;
NLMISC::CVector			 PointPosition2;
NLMISC::CVector			 PointPosition3;
NLMISC::CVector			 PointPosition4;
NLMISC::CVector			 SunDir;
//CPainter				 TilePainter;
//bool					 paint = false;
//bool					 fillMode = false;
//int					     selectedTile = 6;
//CZoneSearch				 ZoneSearch;

//
// Functions
//

void cbUpdateLandscape (CConfigFile::CVar &var)
{
	if (var.Name == "LandscapeTileNear") Landscape->setTileNear (var.asFloat ());
	else if (var.Name == "LandscapeThresold") Landscape->setThreshold (var.asFloat ());
	else if (var.Name == "FogStart") Driver->setupFog (var.asFloat (), ConfigFile.getVar ("FogEnd").asFloat (), CRGBA(ConfigFile.getVar ("FogColor").asInt (0), ConfigFile.getVar ("FogColor").asInt (1), ConfigFile.getVar ("FogColor").asInt (2)));
	else if (var.Name == "FogEnd") Driver->setupFog (ConfigFile.getVar ("FogStart").asFloat (), var.asFloat (), CRGBA(ConfigFile.getVar ("FogColor").asInt (0), ConfigFile.getVar ("FogColor").asInt (1), ConfigFile.getVar ("FogColor").asInt (2)));
	else if (var.Name == "FogColor") Driver->setupFog (ConfigFile.getVar ("FogStart").asFloat (), ConfigFile.getVar ("FogEnd").asFloat (), CRGBA(var.asInt (0), var.asInt (1), var.asInt (2)));
	else if (var.Name == "FogEnable")
	{
		Driver->enableFog (var.asInt () == 1);
		Driver->setupFog (ConfigFile.getVar ("FogStart").asFloat (), ConfigFile.getVar ("FogStart").asFloat (), CRGBA(ConfigFile.getVar ("FogColor").asInt (0), ConfigFile.getVar ("FogColor").asInt (1), ConfigFile.getVar ("FogColor").asInt (2)));
	}
	else if (var.Name == "SunAmbientColor")
	{
		Scene->setSunAmbient(CRGBA (var.asInt(0), var.asInt(1), var.asInt(2)));
	}
	else if (var.Name == "SunDiffuseColor")
	{
		Scene->setSunDiffuse(CRGBA (var.asInt(0), var.asInt(1), var.asInt(2)));
	}
	else if (var.Name == "SunSpecularColor")
	{
		Scene->setSunSpecular(CRGBA (var.asInt(0), var.asInt(1), var.asInt(2)));
	}
	else if (var.Name == "SunDirection")
	{
		SunDirection.set(var.asFloat(0), var.asFloat(1), var.asFloat(2));
		SunDirection.normalize();
		Scene->setSunDirection(SunDirection);
	}
	else if (var.Name == "AmbientGlobal")
	{
		Scene->setAmbientGlobal(CRGBA (var.asInt(0), var.asInt(1), var.asInt(2)));
	}

	else nlwarning ("Unknown variable update %s", var.Name.c_str());
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CInstanceGroup* LoadInstanceGroup(const char* sFilename)
{
	CIFile file;
	CInstanceGroup *newIG = new CInstanceGroup;

	if( file.open( CPath::lookup( string(sFilename) ) ) )
	{
		try
		{
			// Serial the skeleton
			newIG->serial (file);
			// All is good
		}
		catch (Exception &)
		{
			if (ShowDebugInChat) nlinfo("Cant load file!");
			// Cannot save the file
			delete newIG;
			return NULL;
		}
	}
	return newIG;
}

void	initLandscape()
{
	// create the landscape
	Landscape = Scene->createLandscape();

	// load the bank files
	Landscape->loadBankFiles (CPath::lookup(ConfigFile.getVar("BankName").asString()), 
							  CPath::lookup(ConfigFile.getVar("FarBankName").asString()));

	// setup the zone path
	Landscape->setZonePath (ConfigFile.getVar("DataPath").asString() + "zones/");

	// and eventually, load the zones around the starting point.
	Landscape->loadAllZonesAround (CVector(ConfigFile.getVar("StartPoint").asFloat(0),
										   ConfigFile.getVar("StartPoint").asFloat(1),
										   ConfigFile.getVar("StartPoint").asFloat(2)), 
								   1000.0f);

	// color of the landscape shadow
	CRGBA diffuse (ConfigFile.getVar("LandscapeDiffuseColor").asInt(0), ConfigFile.getVar("LandscapeDiffuseColor").asInt(1), ConfigFile.getVar("LandscapeDiffuseColor").asInt(2));
	
	Landscape->setupStaticLight(
		diffuse,
		CRGBA(ConfigFile.getVar("LandscapeAmbiantColor").asInt(0), ConfigFile.getVar("LandscapeAmbiantColor").asInt(1), ConfigFile.getVar("LandscapeAmbiantColor").asInt(2)),
		ConfigFile.getVar("LandscapeMultiplyFactor").asFloat());

	Landscape->enableVegetable(true);
	Landscape->loadVegetableTexture("micro-veget.tga");
	Landscape->setVegetableWind(CVector(-2935.0f, 0.107f, -1.22f), 0.45f, 0.1f, 0.5f);
//	Landscape->setupVegetableLighting( CRGBA(90,90,90), CRGBA(190,190,190), CVector(-2935.0f, 0.107f, -1.22f));
//	Scene->setDynamicCoarseMeshManagerColor (diffuse);
//	Scene->setStaticCoarseMeshManagerColor (diffuse);

	Scene->setGlobalWindPower(0.8f);
	Scene->setGlobalWindDirection(CVector(-2935.0f, 0.107f, -1.22f));

	CConfigFile::CVar igv = ConfigFile.getVar("InstanceGroups");
	for (sint32 i = 0; i < igv.size (); i++)
	{
		UInstanceGroup *inst = UInstanceGroup::createInstanceGroup (igv.asString (i));
		if (inst == NULL)
		{
			nlwarning ("Instance group '%s' not found", igv.asString (i).c_str ());
		}
		else
		{
			if (igv.asString(i)=="8_AO.ig")
			{
				inst->createRoot (*Scene);
				TempleInstanceGroup = inst;
//				DispCS_N.push_back (igv.asString(i));
//				DispCS_I.push_back (inst);
			}
			inst->addToScene (*Scene, Driver);
			InstanceGroups.push_back (inst);
		}
	}

	if (Driver->getNbTextureStages()>3)
	{
		UInstanceGroup *inst = UInstanceGroup::createInstanceGroup ("9_AP.ig");
		inst->addToScene (*Scene, Driver);
		InstanceGroups.push_back (inst);
		UInstanceGroup *inst2 = UInstanceGroup::createInstanceGroup ("8_AO-PoolG3.ig");
		inst2->addToScene (*Scene, Driver);
		InstanceGroups.push_back (inst2);
		string w = "Water01";
		string wc = "Wave Maker01";
		UInstance* WaterInstance = inst2->getByName(w);
		UInstance* WaterInstanceC = inst2->getByName(wc);
		WaterInstance->setClusterSystem(TempleInstanceGroup);
		WaterInstanceC->setClusterSystem(TempleInstanceGroup);
	}
	else
	{
		UInstanceGroup *inst = UInstanceGroup::createInstanceGroup ("9_AP-G2.ig");
		inst->addToScene (*Scene, Driver);
		InstanceGroups.push_back (inst);
		UInstanceGroup *inst2 = UInstanceGroup::createInstanceGroup ("8_AO-PoolG2.ig");
		inst2->addToScene (*Scene, Driver);
		InstanceGroups.push_back (inst2);
		string w = "Water01-G2";
		UInstance* WaterInstance = inst2->getByName(w);
		WaterInstance->setClusterSystem(TempleInstanceGroup);
	}

	if (Falling)
	{
		PlanetInstance = UInstanceGroup::createInstanceGroup ("10_AO.ig");
		PlanetInstance->addToScene (*Scene);
		IslandsInstance = UInstanceGroup::createInstanceGroup ("10_AM.ig");
		IslandsInstance->addToScene (*Scene);

	}

//	Sun = ULight::createLight ();
//	nlassert (Sun != NULL);
//	Sun->setMode (ULight::DirectionalLight);
//	Driver->enableLight (0);

//	PointL1 = ULight::createLight();
//	PointL1->setMode(ULight::PointLight);
//	Driver->enableLight (1);

//	Sun->setDiffuse (CRGBA (var.asInt(0), var.asInt(1), var.asInt(2)));
//	PointL1->setDirection (PointDirection);
//	Driver->setLight(1, *PointL1);

/*	PointPosition.set (2290.815f, -1221.23f, 4.709f);
	PointL1 = Scene->createPointLight();
	PointL1->setColor(CRGBA (157, 74, 0));
	PointL1->setAmbient(CRGBA (255, 191, 91));
	PointL1->setPos(PointPosition);
	PointL1->setupAttenuation(4.0f, 9.0f);
	PointL1->setDiffuse(CRGBA (255, 191, 91));
	PointL1->show();
*/
	PointPosition2.set (2322.55f, -1207.0f, 3.5f);
	PointL2 = Scene->createPointLight();
	PointL2->setColor(CRGBA (255, 191, 91));
	PointL2->setAmbient(CRGBA (255, 191, 91));
	PointL2->setPos(PointPosition2);
	PointL2->setupAttenuation(1.0f, 6.2f);
	PointL2->setDiffuse(CRGBA (50, 50, 50));
	PointL2->show();
/*

	PointPosition3.set (2308.161f, -1235.231f, 4.467f);
	PointL3 = Scene->createPointLight();
	PointL3->setColor(CRGBA (255, 191, 91));
	PointL3->setAmbient(CRGBA (255, 191, 91));
	PointL3->setPos(PointPosition3);
	PointL3->setupAttenuation(3.0f, 5.0f);
	PointL3->setDiffuse(CRGBA (255, 191, 91));
	PointL3->show();

	PointPosition4.set (2273.312f, -1235.23f, 6.467f);
	PointL4 = Scene->createPointLight();
	PointL4->setColor(CRGBA (255, 191, 91));
	PointL4->setAmbient(CRGBA (255, 191, 91));
	PointL4->setPos(PointPosition4);
	PointL4->setupAttenuation(3.0f, 5.0f);
	PointL4->setDiffuse(CRGBA (255, 191, 91));
	PointL4->show();
*/

//	PointL1->setupPointLight(
//		CRGBA (255, 191, 91), CRGBA (255, 191, 91), CRGBA (255, 191, 91),
//		CVector(2288.486f, -1213.701f, 4.709f), CVector(0,0,0));


	ConfigFile.setCallback ("LandscapeTileNear", cbUpdateLandscape);
	ConfigFile.setCallback ("LandscapeThresold", cbUpdateLandscape);
	ConfigFile.setCallback ("FogStart", cbUpdateLandscape);
	ConfigFile.setCallback ("FogEnd", cbUpdateLandscape);
	ConfigFile.setCallback ("FogColor", cbUpdateLandscape);
	ConfigFile.setCallback ("FogEnable", cbUpdateLandscape);

	ConfigFile.setCallback ("SunAmbientColor", cbUpdateLandscape);
	ConfigFile.setCallback ("SunDiffuseColor", cbUpdateLandscape);
	ConfigFile.setCallback ("SunSpecularColor", cbUpdateLandscape);
	ConfigFile.setCallback ("SunDirection", cbUpdateLandscape);

	cbUpdateLandscape (ConfigFile.getVar ("LandscapeTileNear"));
	cbUpdateLandscape (ConfigFile.getVar ("LandscapeThresold"));
	cbUpdateLandscape (ConfigFile.getVar ("FogStart"));
	cbUpdateLandscape (ConfigFile.getVar ("FogEnd"));
	cbUpdateLandscape (ConfigFile.getVar ("FogColor"));
	cbUpdateLandscape (ConfigFile.getVar ("FogEnable"));

	cbUpdateLandscape (ConfigFile.getVar ("SunAmbientColor"));
	cbUpdateLandscape (ConfigFile.getVar ("SunDiffuseColor"));
	cbUpdateLandscape (ConfigFile.getVar ("SunSpecularColor"));
	cbUpdateLandscape (ConfigFile.getVar ("SunDirection"));

	// Init the landscape painter
//	TilePainter.init( &(dynamic_cast<CLandscapeUser*>(Landscape)->getLandscape()->Landscape) );

//	UWaterInstance *theWaterInstance = dynamic_cast<UWaterInstance*> (theWaterPool);

//	if (theWaterInstance=NULL) nlinfo("watershape is NULL");
//	else nlinfo("WaterHeightMapId %f .",theWaterInstance->getHeight(CVector2f(1.0f, 1.0f)));

//		UInstanceGroup  *waterIg = UInstanceGroup::createInstanceGroup(std::string("9_AO.ig"));
//		waterIg->addToScene(*Scene);

	if (Driver->getNbTextureStages()>3)
	{

		WaterPoolManager = &NL3D::GetWaterPoolManager();
		NL3D::CWaterPoolManager::CWaterHeightMapBuild whmb;
		whmb.ID   = 0;
		whmb.Name = "Pool00";
		whmb.Size = 256;
		whmb.FilterWeight = 2.0f;
		whmb.UnitSize = 0.3f;
		whmb.WavesEnabled = true;
		whmb.WaveIntensity = 1.5f;
		whmb.WavePeriod = 0.3f;
		whmb.WaveRadius = 2;
		whmb.BorderWaves = false;
		whmb.Damping = 0.99f;

		WaterPoolManager->createWaterPool(whmb);

		NL3D::CWaterPoolManager::CWaterHeightMapBuild whmb2;
		whmb2.ID   = 1;
		whmb2.Name = "Pool01";
		whmb2.Size = 256;
		whmb2.FilterWeight = 2.0f;
		whmb2.UnitSize = 0.3f;
		whmb2.WavesEnabled = true;
		whmb2.WaveIntensity = 1.5f;
		whmb2.WavePeriod = 0.3f;
		whmb2.WaveRadius = 2;
		whmb2.BorderWaves = false;
		whmb2.Damping = 0.99f;

		WaterPoolManager->createWaterPool(whmb2);

	//	CWaterModel::registerBasics();

		if (ShowDebugInChat) nlinfo("Nr of waterpools %d nr.",WaterPoolManager->getNumPools());
		WaterPoolManager->setBlendFactor(dynamic_cast<IDriver*> (Driver), 0.0f);

	}
//		Scene->loadLightmapAutoAnim("Omni1.anim");
//	*WaterPoolManager->createWaterPool(0);

}


void			unloadPlanetInstance()
{
	if (PlanetInstance!=NULL) PlanetInstance->removeFromScene(*Scene);
//	Scene->deleteInstance(*PlanetInstance);
//	nlinfo("Planet removed");
}

void	updateLandscape()
{
	// load the zones around the viewpoint
	Landscape->refreshZonesAround (MouseListener->getViewMatrix().getPos(), 1000.0f);
}

void	releaseLandscape()
{

	// Remove added/loaded igs and their instances.
	for(uint igId=0; igId<InstanceGroups.size(); igId++)
	{
		// remove instances.
		InstanceGroups[igId]->removeFromScene(*Scene);
		// free up the ig.
		delete InstanceGroups[igId];
	}
	InstanceGroups.clear();

	Scene->deleteLandscape(Landscape);
	Landscape = NULL;

//	Scene->deletePointLight(PointL1);
	Scene->deletePointLight(PointL2);
//	Scene->deletePointLight(PointL3);
//	Scene->deletePointLight(PointL4);
//	PointL1=NULL;
	PointL2=NULL;
//	PointL3=NULL;
//	PointL4=NULL;

}

void	initAiming()
{
	// Create an aiming entity
	AimingEntity = VisualCollisionManager->createEntity();
	AimingEntity->setCeilMode(true);
}

void	releaseAiming()
{
	VisualCollisionManager->deleteEntity(AimingEntity);
}


CVector	getTarget(const CVector &start, const CVector &step, uint numSteps)
{
	CVector	testPos = start;
	CVector thePos = MouseListener->getViewMatrix(). getPos();
//	pair<string, uint32> theNames;
//	theNames = ZoneSearch.getZoneName(thePos.x, thePos.y, 2, 2);
//	nlinfo(" startx= %f starty= %f", thePos.x, thePos.y);
//	nlinfo(" the name= %s and id= %d", theNames.first, theNames.second);	
//	nlinfo("hmm11");
//	CVisualTileDescNode *theNode = AimingEntity->getLandscapeCollisionGrid().select(testPos);
//	nlinfo("hmm2");

//	nlinfo("the PatchQuadBlockId=%d", theNode->PatchQuadBlocId);
//	nlinfo("hmm3");
//	nlinfo("the QuadId=%d", theNode->QuadId);
	

	uint	i;
	for (i=0; i<numSteps; ++i)
	{
		CVector	snapped = testPos, 
				normal;

		// here use normal to check if we have collision
		if (AimingEntity->snapToGround(snapped, normal) && (testPos.z-snapped.z)*normal.z < 0.0f)
		{
			testPos -= step*0.5f;
			break;
		}
		testPos += step;
	}
	return testPos;
}

CVector	getTarget(CTrajectory &trajectory, TTime dtSteps, uint numSteps)
{
	TTime	t = trajectory.getStartTime();
	CVector	testPos;

	uint	i;
	for (i=0; i<numSteps; ++i)
	{
		testPos = trajectory.eval(t);
		CVector	snapped = testPos, 
				normal;

		// here use normal to check if we have collision
		if (AimingEntity->snapToGround(snapped, normal) && (testPos.z-snapped.z)*normal.z < 0.0f)
		{
			t -= (dtSteps/2);
			testPos = trajectory.eval(t);
			break;
		}
		t += dtSteps;
	}
	return testPos;
}

/*
CVector	getTarget(const CVector &start, const CVector &step, uint numSteps)
{
	CVector	testPos = start;

	uint	i;
	for (i=0; i<numSteps; ++i)
	{
		// For each step, check if the snapped position is backward the normal
		CVector	snapped = testPos;
		CVector	normal;
		// here use normal to check if we have collision
		if (AimingEntity->snapToGround(snapped, normal) && (testPos.z-snapped.z)*normal.z < 0.0f)
		{
			testPos -= step*0.5f;
			break;
		}
		testPos += step;
	}

	return testPos;
}
*/

/*******************************************************************\
						getZoneCoordByName()
\*******************************************************************/
/*bool getZoneCoordByName(const char * name, uint16& x, uint16& y)
{
	uint i;
	
	std::string zoneName(name);

	// y
	uint ind1 = zoneName.find("_");
	if(ind1>=zoneName.length())
	{
		nlwarning("bad file name");
		return false;
	}
	std::string ystr = zoneName.substr(0,ind1);
	for(i=0; i<ystr.length(); i++)
	{
		if(!isdigit(ystr[i]))
		{
			nlwarning("y code size is not a 2 characters code");
			return false;
		}
	}
	y = atoi(ystr.c_str());

	// x
	x = 0;
	uint ind2 = zoneName.length();
	if((ind2-ind1-1)!=2)
	{
		nlwarning("x code size is not a 2 characters code");
		return false;
	}
	std::string xstr = zoneName.substr(ind1+1,ind2-ind1-1);
	for(i=0; i<xstr.length(); i++)
	{
		if (isalpha(xstr[i]))
		{
			x *= 26;
			x += (tolower(xstr[i])-'a');
		}
		else
		{
			nlwarning("invalid");
			return false;
		}
	}
	return true;
}
*/
/*******************************************************************\
						getDir()
\*******************************************************************/
std::string getDir (const std::string& path)
{
	char tmpPath[512];
	strcpy (tmpPath, path.c_str());
	char* slash=strrchr (tmpPath, '/');
	if (!slash)
	{
		slash=strrchr (tmpPath, '\\');
	}

	if (!slash)
		return "";

	slash++;
	*slash=0;
	return tmpPath;
}

/*******************************************************************\
						getName()
\*******************************************************************/
std::string getName (const std::string& path)
{
	std::string dir=getDir (path);

	char tmpPath[512];
	strcpy (tmpPath, path.c_str());

	char *name=tmpPath;
	nlassert (dir.length()<=strlen(tmpPath));
	name+=dir.length();

	char* point=strrchr (name, '.');
	if (point)
		*point=0;

	return name;
}

NLMISC_DYNVARIABLE(float,tilenear,"landscape tile near")
{
	if (get)
		*pointer = Landscape->getTileNear();
	else
		Landscape->setTileNear(*pointer);
}

NLMISC_DYNVARIABLE(float,threshold,"landscape threshold")
{
	if (get)
		*pointer = Landscape->getThreshold();
	else
		Landscape->setThreshold(*pointer);
}

// boost to 
NLMISC_COMMAND(boost,"switch landscape parameters between high speed and high quality","0|1")
{
	if (args.size() != 1 ) return false;
	if ( args[0]=="1" )
	{
		ICommand::execute( "tilenear 5", CommandsLog);
		ICommand::execute( "threshold 1", CommandsLog);
	}
	else
	{
		ICommand::execute( "tilenear 100", CommandsLog);
		ICommand::execute( "threshold 0.01", CommandsLog);
	}
	return true;
}

// boost to 
NLMISC_COMMAND(storm,"set storm","wind")
{
	if (args.size() == 0 )
	{
		Landscape->setVegetableWind(CVector(-2935.0f, 0.107f, -1.22f), 1.0f, 0.3f, 0.6f);
	}

	if (args.size() == 1 )
	{
		Landscape->setVegetableWind(CVector(-2935.0f, 0.107f, -1.22f), (float)atof(args[0].c_str()), 0.3f, 0.6f);
	}

	if (args.size() == 2 )
	{
		Landscape->setVegetableWind(CVector(-2935.0f, 0.107f, -1.22f), (float)atof(args[0].c_str()), (float)atof(args[1].c_str()), 0.6f);
	}

	if (args.size() == 3 )
	{
		Landscape->setVegetableWind(CVector(-2935.0f, 0.107f, -1.22f), (float)atof(args[0].c_str()), (float)atof(args[1].c_str()), (float)atof(args[2].c_str()));
	}

	return true;
}

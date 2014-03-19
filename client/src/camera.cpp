/** \file camera.cpp
 * Camera interface between the game and NeL
 *
 * $Id: camera.cpp,v 1.2 2002/03/27 03:39:04 robofly Exp $
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

#include <math.h>
#include <nel/misc/command.h>
#include <nel/misc/vectord.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_skeleton.h>
#include <nel/3d/u_visual_collision_entity.h>
#include <nel/3d/u_visual_collision_manager.h>
#include <nel/3d/viewport.h>
#include <nel/misc/time_nl.h>
#include "3d/scene_group.h"

#include <nel/pacs/u_move_container.h>
#include <nel/pacs/u_move_primitive.h>
#include <nel/pacs/u_global_retriever.h>
#include <nel/pacs/u_global_position.h>


#include "client.h"
#include "sound.h"
#include "entities.h"
#include "mouse_listener.h"
#include "pacs.h"


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

// The camera for the whole scene
UCamera					*Camera = NULL;
// The collision entity use to snap the camera on the ground
UVisualCollisionEntity	*CamCollisionEntity = NULL;

UMovePrimitive			*CamMovePrimitive = NULL;

// The sky 3D objects
static UScene				*SkyScene = NULL;
static UCamera				*SkyCamera = NULL;
static UInstance			*Sky = NULL;

//
// Functions
//

void	initSky()
{
	if (Sky==NULL) 
	{
		SkyScene = Driver->createScene();

		SkyCamera = SkyScene->getCam ();
		SkyCamera->setTransformMode (UTransformable::DirectMatrix);
		// Set the very same frustum as the main camera
		SkyCamera->setFrustum (Camera->getFrustum ());

		Sky = SkyScene->createInstance("sky.shape");
		//	Trees = SkyScene->createInstance("Trees.shape");

		Sky->setTransformMode (UTransformable::DirectMatrix);
		Sky->setMatrix(CMatrix::Identity);
	}
}

void	initCamera()
{
	// Set up directly the camera
	Camera = Scene->getCam();
	Camera->setTransformMode (UTransformable::DirectMatrix);
	Camera->setPerspective ((float)Pi/2.f, 1.33f, 0.1f, 90000);
	Camera->lookAt (CVector(ConfigFile.getVar("StartPoint").asFloat(0),
							ConfigFile.getVar("StartPoint").asFloat(1),
							ConfigFile.getVar("StartPoint").asFloat(2)),
							CVectorD (0,0,0));

	Camera->setClusterSystem((UInstanceGroup*)-1);

	CamCollisionEntity = VisualCollisionManager->createEntity();
	CamCollisionEntity->setCeilMode(true);
	CamMovePrimitive = MoveContainer->addCollisionablePrimitive(0, 1);
	CamMovePrimitive->setReactionType(UMovePrimitive::Slide);
	CamMovePrimitive->setTriggerType(UMovePrimitive::NotATrigger);
	CamMovePrimitive->setCollisionMask(OtherCollisionBit+SelfCollisionBit+StaticCollisionBit);
	CamMovePrimitive->setOcclusionMask(SelfCollisionBit);
	CamMovePrimitive->setObstacle(false);

	float rad;
	CamMovePrimitive->setPrimitiveType(UMovePrimitive::_2DOrientedCylinder);
	rad = 0.2f;
	CamMovePrimitive->setRadius(rad);

	CamMovePrimitive->setHeight(2.0f);
	CamMovePrimitive->insertInWorldImage(0);

	CamMovePrimitive->setGlobalPosition(CVectorD(ConfigFile.getVar("StartPoint").asFloat(0),
							ConfigFile.getVar("StartPoint").asFloat(1),
							ConfigFile.getVar("StartPoint").asFloat(2)), 0);

	initSky();
}

void	updateCamera()
{
}
		

void	updateSky ()
{
	if (Sky!=NULL)
	{
		CMatrix skyCameraMatrix;
		skyCameraMatrix.identity();
		// 
		skyCameraMatrix= Camera->getMatrix();
		skyCameraMatrix.setPos(CVector::Null);
		SkyCamera->setMatrix(skyCameraMatrix);	

		SkyScene->animate (float(NewTime)/1000);
		SkyScene->render ();

		// Must clear ZBuffer For incoming rendering.
		Driver->clearZBuffer();
	}
}

void	releaseCamera()
{
	Driver->deleteScene (SkyScene);
}

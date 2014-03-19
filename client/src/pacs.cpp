/** \file pacs.cpp
 * pacs management
 *
 * $Id: pacs.cpp,v 1.4 2002/06/06 05:51:01 robofly Exp $
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

#include <vector>

#include <nel/pacs/u_retriever_bank.h>
#include <nel/pacs/u_global_retriever.h>
#include <nel/pacs/u_move_container.h>
#include <nel/pacs/u_move_primitive.h>
#include <nel/misc/command.h>

#include "../src/pacs/global_retriever.h"

#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance_group.h>
#include <nel/3d/u_visual_collision_manager.h>

#include <nel/3d/u_instance.h>

#include <nel/misc/vectord.h>

#include "client.h"
#include "landscape.h"
#include "pacs.h"
#include "entities.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NL3D;
using namespace NLPACS;

//
// Variables
//

// The retriever bank used in the world
URetrieverBank				*RetrieverBank;
// The global retriever used for pacs
UGlobalRetriever			*GlobalRetriever;
// The move container used for dynamic collisions
UMoveContainer				*MoveContainer;

// The collision manager for ground snappping
UVisualCollisionManager		*VisualCollisionManager;

// The collision primitive for the instances in the landscape
vector<UMovePrimitive *>	InstancesMovePrimitives;

//
// Functions
//

void	initPACS()
{
	// init the global retriever and the retriever bank
	RetrieverBank = URetrieverBank::createRetrieverBank(ConfigFile.getVar("RetrieverBankName").asString().c_str());
	GlobalRetriever = UGlobalRetriever::createGlobalRetriever(ConfigFile.getVar("GlobalRetrieverName").asString().c_str(), RetrieverBank);

	// create the move primitive
	MoveContainer = UMoveContainer::createMoveContainer(GlobalRetriever, 100, 100, 6.0);

//	GlobalRetriever->testCylenderMove( NULL, NULL);

	// create a visual collision manager
	// this should not be in pacs, but this is too close to pacs to be put elsewhere
	VisualCollisionManager = Scene->createVisualCollisionManager();
	VisualCollisionManager->setLandscape(Landscape);

	// create a move primite for each instance in the instance group
	uint	i, j;
	for (j=0; j<InstanceGroups.size(); ++j)
	{
		for (i=0; i<InstanceGroups[j]->getNumInstance(); ++i)
		{
			string	name = InstanceGroups[j]->getShapeName(i);
			if (name != "Water01" && name != "Wave Maker01")
			{
				UMovePrimitive	*primitive = MoveContainer->addCollisionablePrimitive(0, 1);
				primitive->setReactionType(UMovePrimitive::DoNothing);
				primitive->setTriggerType(UMovePrimitive::NotATrigger);
				primitive->setCollisionMask(OtherCollisionBit+SelfCollisionBit+SnowballCollisionBit);
				primitive->setOcclusionMask(StaticCollisionBit);
				primitive->setObstacle(true);


				float rad;

//				nlinfo("ShapeName = %s", name);
				if (name == "ChamberColum02")
				{
					if (ShowDebugInChat) nlinfo("Found one!");
					primitive->setPrimitiveType (UMovePrimitive::_2DOrientedBox);
					CQuat tempQuad = InstanceGroups[j]->getInstanceRot(i);
					primitive->setOrientation (tempQuad.getAngle(), 0);
					primitive->setSize(3.85f, 3.85f);
				}
				else if (name == "pool")
				{
//					nlinfo("Found pool one!");
//					primitive->setPrimitiveType (UMovePrimitive::_2DOrientedBox);
//					CQuat tempQuad = InstanceGroups[j]->getInstanceRot(i);
//					primitive->setOrientation (tempQuad.getAngle(), 0);
//					primitive->setSize(20.0f, 10.0f);
				}
				else
				{
					primitive->setPrimitiveType(UMovePrimitive::_2DOrientedCylinder);
					rad = 0.2f;
					primitive->setRadius(rad);
//				nlwarning ("Instance name '%s' doesn't have a good radius for collision", name.c_str());
				}

				primitive->setHeight(6.0f);

				primitive->insertInWorldImage(0);

				CVector	pos = InstanceGroups[j]->getInstancePos(i);
				primitive->setGlobalPosition(CVectorD(pos.x, pos.y, pos.z-1.5f), 0);
				InstancesMovePrimitives.push_back(primitive);
			}
		}
	}
}

void	releasePACS()
{
	// create a move primite for each instance in the instance group
	uint	i;
	for (i=0; i<InstancesMovePrimitives.size(); ++i)
		MoveContainer->removePrimitive(InstancesMovePrimitives[i]);

	InstancesMovePrimitives.clear();

	// delete all allocated objects
	UGlobalRetriever::deleteGlobalRetriever(GlobalRetriever);
	URetrieverBank::deleteRetrieverBank(RetrieverBank);
	UMoveContainer::deleteMoveContainer(MoveContainer);

	// delete the visual collision manager
	Scene->deleteVisualCollisionManager(VisualCollisionManager);

}

NLMISC_COMMAND(getpath,"find a path to location","<x> <y> <z>")
{
	float toX, toY, toZ;
	// check args, if there s not the right number of parameter, return bad
	if (args.size() < 2) return false;

	toX = (float) atoi (args[0].c_str());
	toY = (float) atoi (args[1].c_str());
	if (args.size() == 3)
		toZ = (float) atoi (args[2].c_str());
	else
		toZ = Self->Position.z;

	CGlobalRetriever* GlobalRetrieverCasted = NULL;
	GlobalRetrieverCasted = dynamic_cast<NLPACS::CGlobalRetriever*> (GlobalRetriever);

	NLPACS::UGlobalPosition startPoint;
	ULocalPosition fromLocalPosition;
	fromLocalPosition.Estimation= Self->Position;
	startPoint.LocalPosition= fromLocalPosition;
	startPoint.InstanceId = 1;

	NLPACS::UGlobalPosition endPoint;
	ULocalPosition toLocalPosition;
	toLocalPosition.Estimation= CVector(toX, toY, toZ);
	endPoint.LocalPosition= toLocalPosition;
	endPoint.InstanceId = 1;

//	CLocalPath ourLocalPath;
	NLPACS::CGlobalRetriever::CGlobalPath ourGlobalPaths;

	GlobalRetrieverCasted->findPath(startPoint, endPoint, ourGlobalPaths);

	NLPACS::CGlobalRetriever::CLocalPath ourGlobalPath0 = ourGlobalPaths[0];
	vector<CVector2s>	path;

	path = ourGlobalPath0.Path;
	if (ShowDebugInChat) nlinfo("Path X=%f Y=%f", path[0].x, path[0].y);
	return true;
}


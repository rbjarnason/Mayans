/** \cluster_display.cpp
 * <File description>
 *
 * $Id: cluster_display.cpp,v 1.2 2002/03/30 04:05:10 robofly Exp $
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
#include <nel/3d/u_skeleton.h>

#include "3d/driver.h"
#include "3d/nelu.h"
#include "3d/scene_group.h"
#include "3d/transform_shape.h"

#include "camera.h"
#include "client.h"
#include "commands.h"
#include "mouse_listener.h"
#include "entities.h"
#include "cluster_display.h"
#include "network.h"
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

vector<std::string> DispCS_N;
vector<NL3D::UInstanceGroup*> DispCS_I;

uint ClusterState=1;
CClipTrav *pClipTrav;

// CTransformShape *pDynObj_InRoot;

void initClusterDisplay()
{

//	CClipTrav *pClipTrav2 = (CClipTrav*) Scene->getClipTrav();
//	pClipTrav = pClipTrav2;
//	DispCS_N.push_back ("Root");
//	DispCS_I.push_back ((UInstanceGroup*) pClipTrav->RootCluster->Group);
	// Put a dynamic object in the root

}

void displayCluster()
{

	vector<CCluster*> vCluster;

	Driver->setMatrixMode2D11 ();

//	TextContext->setColor (CRGBA(255,255,255,255));

	pClipTrav->fullSearch (vCluster, pClipTrav->RootCluster->Group, pClipTrav->CamPos);

	for (uint32 i = 0; i < DispCS_N.size(); ++i)
	{
		TextContext->setColor (CRGBA(255,255,255,255));
		for( uint32 j = 0; j < vCluster.size(); ++j)
		{
			if (DispCS_I[i] == (UInstanceGroup*) vCluster[j]->Group)
			{
//				pDynObj_InRoot->setClusterSystem(DispCS[i].pIG);
				TextContext->setColor (CRGBA(255,0,0,255));
				break;
			} else
			{
//				nlinfo("setting XyXyX %d", vCluster[j]->Group->getNumInstance());
//				Self->Instance->setClusterSystem(TempleInstanceGroup);
				Self->Skeleton->setClusterSystem(TempleInstanceGroup);
//				pDynObj_InRoot->setClusterSystem((UInstanceGroup*) vCluster[j]->Group);
//				pDynObj_InRoot->setClusterSystem(TempleInstanceGroup);
			}
		}

		TextContext->printfAt (0, 1-(i+2)*0.028f, DispCS_N[i].c_str());

		TextContext->setColor (CRGBA(255,255,255,255));

		string sAllClusters = "";
		for( uint32 jj = 0; jj < vCluster.size(); ++jj)
		{
			sAllClusters += vCluster[jj]->Name;
			if (jj < (vCluster.size()-1))
				sAllClusters += ",  ";
		}
		TextContext->printfAt (0, 1-0.028f, sAllClusters.c_str());				
	}
}

void cbUpdateClusterDisplay (CConfigFile::CVar &var)
{
	if (var.Name == "ClusterDisplayState") ClusterState = var.asInt ();
}


void updateClusterDisplay ()
{
	switch (ClusterState)
	{
	case 0:
		break;
	case 1:
		displayCluster ();
		break;
	}
}

void releaseClusterDisplay ()
{
}


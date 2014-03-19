/** \file tracers.cpp
 * 
 * 
 *
 * $Id: tracers.cpp,v 1.1 2002/03/20 18:05:25 robofly Exp $
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

#include <nel/misc/types_nl.h>
#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>

#include <nel/3d/u_material.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>

#include <nel/3d/u_driver.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_texture.h>

#include "mouse_listener.h"
#include "camera.h"
#include "client.h"
#include "entities.h"

//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;
using namespace std;

//
// Variables
//
// std::map<uint32, NL3D::UInstance*>			TracerEntities;
std::map<string, CVector>						StringTracers;
//
// Functions
//

void initTracers ()
{
}

void addTextTracer (std::string tracerString, CVector position)
{
	NL3D::UInstance* snowBallMesh = Scene->createInstance ("snowball.shape");
	snowBallMesh->setPos(position);
	StringTracers.insert(make_pair (tracerString, position));
}


void updateTracers ()
{
	// Setup the driver in matrix mode
	Driver->setMatrixMode3D (*Camera);
	// Setup the drawing context
	TextContext->setHotSpot (UTextContext::MiddleTop);
	TextContext->setColor (CRGBA(255, 255,0,255));
	TextContext->setFontSize (10);
	//
	for (std::map<string, CVector>::iterator tit = StringTracers.begin (); tit != StringTracers.end (); tit++)
	{
			CMatrix		mat;// = Camera->getMatrix();
			mat.setPos((*tit).second);
//			mat.scale(10.0f);
			TextContext->render3D(mat, (*tit).first);
	}
}

void releaseTracers ()
{
}

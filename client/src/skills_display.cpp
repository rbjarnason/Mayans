/** \skillsdisplay.cpp
 * <File description>
 *
 * $Id: skills_display.cpp,v 1.2 2002/06/06 05:51:01 robofly Exp $
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
#include "skills.h"

//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;
using namespace std;

//
// Variables
//

CSkills CharacterSkills;
uint SkillsDisplayState=0;

//
// Functions
//

/*********************************************************\
					displayLargeSkills()
\*********************************************************/
void displayLargeSkills ()
{

static	float xLeft = 0.2f;
static	float xRight = 0.8f;
static	float yTop = 0.92f;
static	float yBottom = 0.08f;
static	float xCenter = 0.4f;
static	float yCenter = 0.92f/2.0f;
static  float row1 = 0.01f;
static  float row2 = 0.20f;
static  float row3 = 0.31f;
static  float row4 = 0.50f;

	Driver->setMatrixMode2D11 ();

	// Background
	Driver->drawQuad (xLeft,yBottom,xRight,yTop,CRGBA(200, 255, 0, 46));

	// Print radar's range
	TextContext->setHotSpot(UTextContext::TopLeft);
	TextContext->setColor(CRGBA(0,0,0,255));
	TextContext->setFontSize( 12 );
	TextContext->printfAt(xLeft+row1,yTop-0.01f,"Luck:");
	TextContext->printfAt(xLeft+row1,yTop-0.035f,"Western Secular:");
	TextContext->printfAt(xLeft+row1,yTop-0.06f,"Pragmatism:");
	TextContext->printfAt(xLeft+row1,yTop-0.085f,"Humanism:");
	TextContext->printfAt(xLeft+row1,yTop-0.11f,"Transcendentalism:");
	TextContext->printfAt(xLeft+row1,yTop-0.135f,"Existentialism:");
	TextContext->printfAt(xLeft+row1,yTop-0.16f,"Ecstatic Nihilism:");
	TextContext->printfAt(xLeft+row1,yTop-0.185f,"Taoism:");
	TextContext->printfAt(xLeft+row1,yTop-0.21f,"Confucianism:");
	TextContext->printfAt(xLeft+row1,yTop-0.235f,"Paganism:");
	TextContext->printfAt(xLeft+row1,yTop-0.26f,"Judaism:");
	TextContext->printfAt(xLeft+row1,yTop-0.285f,"Christianity:");
	TextContext->printfAt(xLeft+row1,yTop-0.31f,"Islam:");
	TextContext->printfAt(xLeft+row1,yTop-0.335f,"Buddhism:");
	TextContext->printfAt(xLeft+row1,yTop-0.36f,"Metaversalism:");
	TextContext->printfAt(xLeft+row1,yTop-0.385f,"Ancient History:");
	TextContext->printfAt(xLeft+row1,yTop-0.41f,"Modern History:");
	TextContext->printfAt(xLeft+row1,yTop-0.435f,"Great Dark Sagas:");
	TextContext->printfAt(xLeft+row1,yTop-0.46f,"Metaverse History:");
	TextContext->printfAt(xLeft+row1,yTop-0.485f,"Survival:");
	TextContext->printfAt(xLeft+row1,yTop-0.51f,"Foraging:");
	TextContext->printfAt(xLeft+row1,yTop-0.535f,"Hunting:");
	TextContext->printfAt(xLeft+row1,yTop-0.56f,"Directional:");
	TextContext->printfAt(xLeft+row1,yTop-0.585f,"Piety:");
	TextContext->printfAt(xLeft+row1,yTop-0.61f,"Hermeneutics:");
	TextContext->printfAt(xLeft+row1,yTop-0.635f,"Divining:");
	TextContext->printfAt(xLeft+row1,yTop-0.66f,"Solid:");
	TextContext->printfAt(xLeft+row1,yTop-0.685f,"Liquid:");
	TextContext->printfAt(xLeft+row1,yTop-0.71f,"Gaseous:");
	TextContext->printfAt(xLeft+row1,yTop-0.735f,"Plasma:");
	TextContext->printfAt(xLeft+row1,yTop-0.76f,"Electricity:");
	TextContext->printfAt(xLeft+row1,yTop-0.785f,"Time:");


	TextContext->printfAt(xLeft+row3,yTop-0.01f,"Morphic:");
	TextContext->printfAt(xLeft+row3,yTop-0.035f,"Weak Strong Gamma:");
	TextContext->printfAt(xLeft+row3,yTop-0.06f,"Odin:");
	TextContext->printfAt(xLeft+row3,yTop-0.085f,"Thor:");
	TextContext->printfAt(xLeft+row3,yTop-0.11f,"Frey:");
	TextContext->printfAt(xLeft+row3,yTop-0.135f,"Loki:");
	TextContext->printfAt(xLeft+row3,yTop-0.16f,"Freya:");
	TextContext->printfAt(xLeft+row3,yTop-0.185f,"Balder:");
	TextContext->printfAt(xLeft+row3,yTop-0.21f,"Meditation:");
	TextContext->printfAt(xLeft+row3,yTop-0.235f,"Trancing:");
	TextContext->printfAt(xLeft+row3,yTop-0.26f,"Chanting:");
	TextContext->printfAt(xLeft+row3,yTop-0.285f,"Body Modification:");
	TextContext->printfAt(xLeft+row3,yTop-0.31f,"Jumping:");
	TextContext->printfAt(xLeft+row3,yTop-0.335f,"Running:");
	TextContext->printfAt(xLeft+row3,yTop-0.36f,"Throwing Blades:");
	TextContext->printfAt(xLeft+row3,yTop-0.385f,"Throwing Spherical:");
	TextContext->printfAt(xLeft+row3,yTop-0.41f,"Aimed Blowdart:");
	TextContext->printfAt(xLeft+row3,yTop-0.435f,"Aimed Bow:");
	TextContext->printfAt(xLeft+row3,yTop-0.46f,"Aimed Crossbow:");
	TextContext->printfAt(xLeft+row3,yTop-0.485f,"Aimed Pistol:");
	TextContext->printfAt(xLeft+row3,yTop-0.51f,"Aimed Projectile:");
	TextContext->printfAt(xLeft+row3,yTop-0.535f,"Aimed SMG:");
	TextContext->printfAt(xLeft+row3,yTop-0.56f,"Aimed Energy:");
	TextContext->printfAt(xLeft+row3,yTop-0.585f,"Aimed LongArms:");
	TextContext->printfAt(xLeft+row3,yTop-0.61f,"Aimed Rifle:");
	TextContext->printfAt(xLeft+row3,yTop-0.635f,"Aimed Auto:");
	TextContext->printfAt(xLeft+row3,yTop-0.66f,"Aimed Heavy:");
	TextContext->printfAt(xLeft+row3,yTop-0.685f,"Aimed Launcher:");
	TextContext->printfAt(xLeft+row3,yTop-0.71f,"Sneak:");
	TextContext->printfAt(xLeft+row3,yTop-0.735f,"Pickpocket:");
	TextContext->printfAt(xLeft+row3,yTop-0.76f,"Dancing:");

	TextContext->printfAt(xLeft+row2,yTop-0.01f,"%d", CharacterSkills.geti16()[0] );
	TextContext->printfAt(xLeft+row2,yTop-0.035f,"%d", CharacterSkills.geti16()[1] );
	TextContext->printfAt(xLeft+row2,yTop-0.06f,"%d", CharacterSkills.geti16()[2] );
	TextContext->printfAt(xLeft+row2,yTop-0.085f,"%d", CharacterSkills.geti16()[3] );
	TextContext->printfAt(xLeft+row2,yTop-0.11f,"%d", CharacterSkills.geti16()[4] );
	TextContext->printfAt(xLeft+row2,yTop-0.135f,"%d", CharacterSkills.geti16()[5] );
	TextContext->printfAt(xLeft+row2,yTop-0.16f,"%d", CharacterSkills.geti16()[6] );
	TextContext->printfAt(xLeft+row2,yTop-0.185f,"%d", CharacterSkills.geti16()[7] );
	TextContext->printfAt(xLeft+row2,yTop-0.21f,"%d", CharacterSkills.geti16()[8] );
	TextContext->printfAt(xLeft+row2,yTop-0.235f,"%d", CharacterSkills.geti16()[9] );
	TextContext->printfAt(xLeft+row2,yTop-0.26f,"%d", CharacterSkills.geti16()[10] );
	TextContext->printfAt(xLeft+row2,yTop-0.285f,"%d", CharacterSkills.geti16()[11] );
	TextContext->printfAt(xLeft+row2,yTop-0.31f,"%d", CharacterSkills.geti16()[12] );
	TextContext->printfAt(xLeft+row2,yTop-0.335f,"%d", CharacterSkills.geti16()[13] );
	TextContext->printfAt(xLeft+row2,yTop-0.36f,"%d", CharacterSkills.geti16()[14] );
	TextContext->printfAt(xLeft+row2,yTop-0.385f,"%d", CharacterSkills.geti16()[15] );
	TextContext->printfAt(xLeft+row2,yTop-0.41f,"%d", CharacterSkills.geti16()[16] );
	TextContext->printfAt(xLeft+row2,yTop-0.435f,"%d", CharacterSkills.geti16()[17] );
	TextContext->printfAt(xLeft+row2,yTop-0.46f,"%d", CharacterSkills.geti16()[18] );
	TextContext->printfAt(xLeft+row2,yTop-0.485f,"%d", CharacterSkills.geti16()[19] );
	TextContext->printfAt(xLeft+row2,yTop-0.51f,"%d", CharacterSkills.geti16()[20] );
	TextContext->printfAt(xLeft+row2,yTop-0.535f,"%d", CharacterSkills.geti16()[21] );
	TextContext->printfAt(xLeft+row2,yTop-0.56f,"%d", CharacterSkills.geti16()[22] );
	TextContext->printfAt(xLeft+row2,yTop-0.585f,"%d", CharacterSkills.geti16()[23] );
	TextContext->printfAt(xLeft+row2,yTop-0.61f,"%d", CharacterSkills.geti16()[24] );
	TextContext->printfAt(xLeft+row2,yTop-0.635f,"%d", CharacterSkills.geti16()[25] );
	TextContext->printfAt(xLeft+row2,yTop-0.66f,"%d", CharacterSkills.geti16()[26] );
	TextContext->printfAt(xLeft+row2,yTop-0.685f,"%d", CharacterSkills.geti16()[27] );
	TextContext->printfAt(xLeft+row2,yTop-0.71f,"%d", CharacterSkills.geti16()[28] );
	TextContext->printfAt(xLeft+row2,yTop-0.735f,"%d", CharacterSkills.geti16()[29] );
	TextContext->printfAt(xLeft+row2,yTop-0.76f,"%d", CharacterSkills.geti16()[30] );
	TextContext->printfAt(xLeft+row2,yTop-0.785f,"%d", CharacterSkills.geti16()[31] );

	TextContext->printfAt(xLeft+row4,yTop-0.01f,"%d", CharacterSkills.geti16()[32] );
	TextContext->printfAt(xLeft+row4,yTop-0.035f,"%d", CharacterSkills.geti16()[33] );
	TextContext->printfAt(xLeft+row4,yTop-0.06f,"%d", CharacterSkills.geti16()[34] );
	TextContext->printfAt(xLeft+row4,yTop-0.085f,"%d", CharacterSkills.geti16()[35] );
	TextContext->printfAt(xLeft+row4,yTop-0.11f,"%d", CharacterSkills.geti16()[36] );
	TextContext->printfAt(xLeft+row4,yTop-0.135f,"%d", CharacterSkills.geti16()[37] );
	TextContext->printfAt(xLeft+row4,yTop-0.16f,"%d", CharacterSkills.geti16()[38] );
	TextContext->printfAt(xLeft+row4,yTop-0.185f,"%d", CharacterSkills.geti16()[39] );
	TextContext->printfAt(xLeft+row4,yTop-0.21f,"%d", CharacterSkills.geti16()[40] );
	TextContext->printfAt(xLeft+row4,yTop-0.235f,"%d", CharacterSkills.geti16()[41] );
	TextContext->printfAt(xLeft+row4,yTop-0.26f,"%d", CharacterSkills.geti16()[42] );
	TextContext->printfAt(xLeft+row4,yTop-0.285f,"%d", CharacterSkills.geti16()[43] );
	TextContext->printfAt(xLeft+row4,yTop-0.31f,"%d", CharacterSkills.geti16()[44] );
	TextContext->printfAt(xLeft+row4,yTop-0.335f,"%d", CharacterSkills.geti16()[45] );
	TextContext->printfAt(xLeft+row4,yTop-0.36f,"%d", CharacterSkills.geti16()[46] );
	TextContext->printfAt(xLeft+row4,yTop-0.385f,"%d", CharacterSkills.geti16()[47] );
	TextContext->printfAt(xLeft+row4,yTop-0.41f,"%d", CharacterSkills.geti16()[48] );
	TextContext->printfAt(xLeft+row4,yTop-0.435f,"%d", CharacterSkills.geti16()[49] );
	TextContext->printfAt(xLeft+row4,yTop-0.46f,"%d", CharacterSkills.geti16()[50] );
	TextContext->printfAt(xLeft+row4,yTop-0.485f,"%d", CharacterSkills.geti16()[51] );
	TextContext->printfAt(xLeft+row4,yTop-0.51f,"%d", CharacterSkills.geti16()[52] );
	TextContext->printfAt(xLeft+row4,yTop-0.535f,"%d", CharacterSkills.geti16()[53] );
	TextContext->printfAt(xLeft+row4,yTop-0.56f,"%d", CharacterSkills.geti16()[54] );
	TextContext->printfAt(xLeft+row4,yTop-0.585f,"%d", CharacterSkills.geti16()[55] );
	TextContext->printfAt(xLeft+row4,yTop-0.61f,"%d", CharacterSkills.geti16()[56] );
	TextContext->printfAt(xLeft+row4,yTop-0.635f,"%d", CharacterSkills.geti16()[57] );
	TextContext->printfAt(xLeft+row4,yTop-0.66f,"%d", CharacterSkills.geti16()[58] );
	TextContext->printfAt(xLeft+row4,yTop-0.685f,"%d", CharacterSkills.geti16()[59] );
	TextContext->printfAt(xLeft+row4,yTop-0.71f,"%d", CharacterSkills.geti16()[60] );
	TextContext->printfAt(xLeft+row4,yTop-0.735f,"%d", CharacterSkills.geti16()[61] );
	TextContext->printfAt(xLeft+row4,yTop-0.76f,"%d", CharacterSkills.geti16()[62] );

}

/*********************************************************\
					displaySkills()
\*********************************************************/
void displaySmallSkills ()
{
/*	Driver->drawBitmap(x, 1-(h+y), w, h, *PsychTexture);

	TextContext->setHotSpot(UTextContext::TopRight);
	TextContext->setColor(CRGBA(255,255,255));
	TextContext->setFontSize(14);
	TextContext->printfAt(x+0.05f,1-(h+y+0.023f),"%d", CharacterSkills.geti16()[19] );
	TextContext->printfAt(x+0.156f,1-(h+y+0.023f),"%d", (sint) CharacterSkills.geti16()[17] );
	TextContext->printfAt(x+0.2695f,1-(h+y+0.023f),"%d", (sint) CharacterSkills.geti16()[21]);
*/
}


void cbUpdateSkillsDisplay (CConfigFile::CVar &var)
{
	if (var.Name == "SkillsDisplayState") SkillsDisplayState = var.asInt ();
}

void initSkillsDisplay ()
{
	ConfigFile.setCallback ("SkillsDisplayState", cbUpdateSkillsDisplay);

//	PsychTexture = Driver->createTextureFile ("psych.tga");
	if (ShowDebugInChat) nlinfo("Init Skills Display (state)=%d", SkillsDisplayState);
}

void updateSkillsDisplay ()
{
	switch (SkillsDisplayState)
	{
	case 0:
		break;
	case 1:
		displayLargeSkills ();
		break;
//	case 2:
//		displayLargeSkills ();
//		break;
	}
}

void releaseSkillsDisplay ()
{
}


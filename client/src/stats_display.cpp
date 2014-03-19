/** \statsdisplay.cpp
 * <File description>
 *
 * $Id: stats_display.cpp,v 1.2 2002/06/06 05:51:01 robofly Exp $
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
static NL3D::UTexture *PsychTexture = NULL;

CStats CharacterStats;
uint StatsDisplayState=1;

float	x = 300/800.0f;
float	y = 8/600.0f;
float	w = 256/800.0f;
float	h = 32/600.0f;

//
// Functions
//

/*********************************************************\
					displayLargeStats()
\*********************************************************/
void displayLargeStats ()
{

static	float xLeft = 0.2f;
static	float xRight = 0.8f;
static	float yTop = 0.92f;
static	float yBottom = 0.08f;
static	float xCenter = 0.4f;
static	float yCenter = 0.92f/2.0f;
static  float row1 = 0.01f;
static  float row2 = 0.15f;
static  float row3 = 0.31f;
static  float row4 = 0.48f;

	Driver->setMatrixMode2D11 ();

	// Background
	Driver->drawQuad (xLeft,yBottom,xRight,yTop,CRGBA(200, 255, 0, 46));

	// Print radar's range
	TextContext->setHotSpot(UTextContext::TopLeft);
	TextContext->setColor(CRGBA(0,0,0,255));
	TextContext->setFontSize( 12 );
	TextContext->printfAt(xLeft+row1,yTop-0.01f,"Name:" );
	TextContext->printfAt(xLeft+row1,yTop-0.035f,"Gender:");
	TextContext->printfAt(xLeft+row1,yTop-0.06f,"Handedness:" );
	TextContext->printfAt(xLeft+row1,yTop-0.085f,"Base Pigment:" );
	TextContext->printfAt(xLeft+row1,yTop-0.11f,"Hair Color:"  );
	TextContext->printfAt(xLeft+row1,yTop-0.135f,"Hair Type:");
	TextContext->printfAt(xLeft+row1,yTop-0.16f,"Eye Color:" );
	TextContext->printfAt(xLeft+row1,yTop-0.185f,"Weight:"  );
	TextContext->printfAt(xLeft+row1,yTop-0.21f,"Height:");
	TextContext->printfAt(xLeft+row1,yTop-0.235f,""  );
	TextContext->printfAt(xLeft+row1,yTop-0.26f,"Mental Fatigue:");
	TextContext->printfAt(xLeft+row1,yTop-0.285f,"Physical Fatigue:" );
	TextContext->printfAt(xLeft+row1,yTop-0.31f,"Combat Stance:"  );
	TextContext->printfAt(xLeft+row1,yTop-0.335f,"Hydration Level:");
	TextContext->printfAt(xLeft+row1,yTop-0.36f,"Karma:");

	TextContext->printfAt(xLeft+row1,yTop-0.385f,"Mood Base:"  );
	TextContext->printfAt(xLeft+row1,yTop-0.41f,"Mood Actual:");
	TextContext->printfAt(xLeft+row1,yTop-0.435f,"Calm Base"  );
	TextContext->printfAt(xLeft+row1,yTop-0.46f,"Calm Actual:");
	TextContext->printfAt(xLeft+row1,yTop-0.485f,"Resonance Base:" );
	TextContext->printfAt(xLeft+row1,yTop-0.51f,"Resonance Actual:"  );
	TextContext->printfAt(xLeft+row1,yTop-0.535f,"Science v. Magic:");
	TextContext->printfAt(xLeft+row1,yTop-0.56f,"Humanism v. Meta:");
// todo add rest

	TextContext->printfAt(xLeft+row3,yTop-0.01f,"");
	TextContext->printfAt(xLeft+row3,yTop-0.035f,"Faculties:");
	TextContext->printfAt(xLeft+row3,yTop-0.06f,"CognitiveFocus:");
	TextContext->printfAt(xLeft+row3,yTop-0.085f,"CognitiveSpeed:");
	TextContext->printfAt(xLeft+row3,yTop-0.11f,"Intentional Will:");
	TextContext->printfAt(xLeft+row3,yTop-0.135f,"Subconscious Will:");
	TextContext->printfAt(xLeft+row3,yTop-0.16f,"Mnemonics:");
	TextContext->printfAt(xLeft+row3,yTop-0.185f,"Multitasking:");
	TextContext->printfAt(xLeft+row3,yTop-0.21f,"Naturalistic:");
	TextContext->printfAt(xLeft+row3,yTop-0.235f,"Conceptual:");
	TextContext->printfAt(xLeft+row3,yTop-0.26f,"Intuitive:");
	TextContext->printfAt(xLeft+row3,yTop-0.285f,"Artistic:");
	TextContext->printfAt(xLeft+row3,yTop-0.31f,"Verbal:");
	TextContext->printfAt(xLeft+row3,yTop-0.335f,"Mechanistic:");
	TextContext->printfAt(xLeft+row3,yTop-0.36f,"Mathematical:");
	TextContext->printfAt(xLeft+row3,yTop-0.385f,"Physicality:");     
	TextContext->printfAt(xLeft+row3,yTop-0.41f,"UpperBodyFastTwitch:");
	TextContext->printfAt(xLeft+row3,yTop-0.435f,"UpperBodySlowTwitch:");
	TextContext->printfAt(xLeft+row3,yTop-0.46f,"LowerBodyfastTwitch:");
	TextContext->printfAt(xLeft+row3,yTop-0.485f,"LowerBodySlowTwitch:");
	TextContext->printfAt(xLeft+row3,yTop-0.51f,"BoneMass:");
	TextContext->printfAt(xLeft+row3,yTop-0.535f,"Circulatory:");
	TextContext->printfAt(xLeft+row3,yTop-0.56f,"Filtration:");
	TextContext->printfAt(xLeft+row3,yTop-0.585f,"Respiratory:");
	TextContext->printfAt(xLeft+row3,yTop-0.61f,"Thermoregulatory:");
	TextContext->printfAt(xLeft+row3,yTop-0.635f,"Metabolism:");
	TextContext->printfAt(xLeft+row3,yTop-0.66f,"Immuno Response:");
	TextContext->printfAt(xLeft+row3,yTop-0.685f,"Regeneratory:");
	TextContext->printfAt(xLeft+row3,yTop-0.71f,"Sexual Reproduction:");
	TextContext->printfAt(xLeft+row3,yTop-0.735f,"Reflex Speed:");
	TextContext->printfAt(xLeft+row3,yTop-0.76f,"HandEye:");
	TextContext->printfAt(xLeft+row3,yTop-0.785f,"Fine Manual:");
	TextContext->printfAt(xLeft+row3,yTop-0.81f,"Largem Muscle:");
//	TextContext->printfAt(xLeft+row3,yTop-0.835f,"Rhythmic:");
//	TextContext->printfAt(xLeft+row3,yTop-0.86f,"Visual:");
//	TextContext->printfAt(xLeft+row3,yTop-0.885f,"Aural:");
//	TextContext->printfAt(xLeft+row3,yTop-0.91f,"Tactile:");

/*
0.935f,"Height:");
0.96f,"Weight:");
"Proportion:");
Facialsymmetry:");
"Skinthickness:");
Pigmentation:");
"TopicalIntegrity:")
*/

	TextContext->printfAt(xLeft+row2,yTop-0.01f,"%s", CharacterStats.getstr()[0]+" "+CharacterStats.getstr()[1]  );
	TextContext->printfAt(xLeft+row2,yTop-0.035f,"%s", CharacterStats.getstr()[2] );
	TextContext->printfAt(xLeft+row2,yTop-0.06f,"%s", CharacterStats.getstr()[3] );
	TextContext->printfAt(xLeft+row2,yTop-0.085f,"%s", CharacterStats.getstr()[4] );
	TextContext->printfAt(xLeft+row2,yTop-0.11f,"%s", CharacterStats.getstr()[5] );
	TextContext->printfAt(xLeft+row2,yTop-0.135f,"%s", CharacterStats.getstr()[6] );
	TextContext->printfAt(xLeft+row2,yTop-0.16f,"%s", CharacterStats.getstr()[7] );
	TextContext->printfAt(xLeft+row2,yTop-0.185f,"%d kg", CharacterStats.geti16()[0] );
	TextContext->printfAt(xLeft+row2,yTop-0.21f,"%d cm", CharacterStats.geti16()[1] );
	TextContext->printfAt(xLeft+row2,yTop-0.26f,"%d", CharacterStats.geti16()[3] );
	TextContext->printfAt(xLeft+row2,yTop-0.285f,"%d", CharacterStats.geti16()[4] );
	TextContext->printfAt(xLeft+row2,yTop-0.31f,"%d", CharacterStats.geti16()[5] );
	TextContext->printfAt(xLeft+row2,yTop-0.335f,"%d", CharacterStats.geti16()[6] );
	TextContext->printfAt(xLeft+row2,yTop-0.36f,"%d", CharacterStats.geti16()[13] );

	TextContext->printfAt(xLeft+row2,yTop-0.385f,"%d", CharacterStats.geti16()[16] );
	TextContext->printfAt(xLeft+row2,yTop-0.41f,"%d", CharacterStats.geti16()[17] );
	TextContext->printfAt(xLeft+row2,yTop-0.435f,"%d", CharacterStats.geti16()[18] );
	TextContext->printfAt(xLeft+row2,yTop-0.46f,"%d", CharacterStats.geti16()[19] );
	TextContext->printfAt(xLeft+row2,yTop-0.485f,"%d", CharacterStats.geti16()[20] );
	TextContext->printfAt(xLeft+row2,yTop-0.51f,"%d", CharacterStats.geti16()[21] );
	TextContext->printfAt(xLeft+row2,yTop-0.535f,"%d", CharacterStats.geti16()[22] );
	TextContext->printfAt(xLeft+row2,yTop-0.56f,"%d", CharacterStats.geti16()[23] );

	float indent = 0.07f;
	TextContext->printfAt(xLeft+row4,yTop-0.01f,"Base");
	TextContext->printfAt(xLeft+row4+indent,yTop-0.01f,"Actual");
	TextContext->printfAt(xLeft+row4,yTop-0.035f,"%d", CharacterStats.geti16()[29] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.035f,"%d", CharacterStats.geti16()[30] );
	TextContext->printfAt(xLeft+row4,yTop-0.06f,"%d", CharacterStats.geti16()[31] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.06f,"%d", CharacterStats.geti16()[32] );
	TextContext->printfAt(xLeft+row4,yTop-0.085f,"%d", CharacterStats.geti16()[33] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.085f,"%d", CharacterStats.geti16()[34] );
	TextContext->printfAt(xLeft+row4,yTop-0.11f,"%d", CharacterStats.geti16()[35] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.11f,"%d", CharacterStats.geti16()[36] );
	TextContext->printfAt(xLeft+row4,yTop-0.135f,"%d", CharacterStats.geti16()[37] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.135f,"%d", CharacterStats.geti16()[38] );
	TextContext->printfAt(xLeft+row4,yTop-0.16f,"%d", CharacterStats.geti16()[39] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.16f,"%d", CharacterStats.geti16()[40] );
	TextContext->printfAt(xLeft+row4,yTop-0.185f,"%d", CharacterStats.geti16()[41] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.185f,"%d", CharacterStats.geti16()[42] );
	TextContext->printfAt(xLeft+row4,yTop-0.21f,"%d", CharacterStats.geti16()[43] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.21f,"%d", CharacterStats.geti16()[44] );
	TextContext->printfAt(xLeft+row4,yTop-0.26f,"%d", CharacterStats.geti16()[45] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.26f,"%d", CharacterStats.geti16()[46] );
	TextContext->printfAt(xLeft+row4,yTop-0.285f,"%d", CharacterStats.geti16()[47] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.285f,"%d", CharacterStats.geti16()[48] );
	TextContext->printfAt(xLeft+row4,yTop-0.31f,"%d", CharacterStats.geti16()[49] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.31f,"%d", CharacterStats.geti16()[50] );
	TextContext->printfAt(xLeft+row4,yTop-0.335f,"%d", CharacterStats.geti16()[51] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.335f,"%d", CharacterStats.geti16()[52] );
	TextContext->printfAt(xLeft+row4,yTop-0.36f,"%d", CharacterStats.geti16()[53] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.36f,"%d", CharacterStats.geti16()[54] );
	TextContext->printfAt(xLeft+row4,yTop-0.385f,"%d", CharacterStats.geti16()[55] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.385f,"%d", CharacterStats.geti16()[56] );
	TextContext->printfAt(xLeft+row4,yTop-0.41f,"%d", CharacterStats.geti16()[57] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.41f,"%d", CharacterStats.geti16()[58] );
	TextContext->printfAt(xLeft+row4,yTop-0.435f,"%d", CharacterStats.geti16()[59] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.435f,"%d", CharacterStats.geti16()[60] );
	TextContext->printfAt(xLeft+row4,yTop-0.46f,"%d", CharacterStats.geti16()[61] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.46f,"%d", CharacterStats.geti16()[62] );
	TextContext->printfAt(xLeft+row4,yTop-0.485f,"%d", CharacterStats.geti16()[63] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.485f,"%d", CharacterStats.geti16()[64] );
	TextContext->printfAt(xLeft+row4,yTop-0.51f,"%d", CharacterStats.geti16()[65] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.51f,"%d", CharacterStats.geti16()[66] );
	TextContext->printfAt(xLeft+row4,yTop-0.535f,"%d", CharacterStats.geti16()[67] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.535f,"%d", CharacterStats.geti16()[68] );
	TextContext->printfAt(xLeft+row4,yTop-0.56f,"%d", CharacterStats.geti16()[69] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.56f,"%d", CharacterStats.geti16()[70] );
	TextContext->printfAt(xLeft+row4,yTop-0.585f,"%d", CharacterStats.geti16()[71] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.585f,"%d", CharacterStats.geti16()[72] );
	TextContext->printfAt(xLeft+row4,yTop-0.61f,"%d", CharacterStats.geti16()[73] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.61f,"%d", CharacterStats.geti16()[74] );
	TextContext->printfAt(xLeft+row4,yTop-0.635f,"%d", CharacterStats.geti16()[75] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.635f,"%d", CharacterStats.geti16()[76] );
	TextContext->printfAt(xLeft+row4,yTop-0.66f,"%d", CharacterStats.geti16()[77] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.66f,"%d", CharacterStats.geti16()[78] );
	TextContext->printfAt(xLeft+row4,yTop-0.685f,"%d", CharacterStats.geti16()[79] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.685f,"%d", CharacterStats.geti16()[80] );
	TextContext->printfAt(xLeft+row4,yTop-0.71f,"%d", CharacterStats.geti16()[81] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.71f,"%d", CharacterStats.geti16()[82] );
	TextContext->printfAt(xLeft+row4,yTop-0.735f,"%d", CharacterStats.geti16()[83] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.735f,"%d", CharacterStats.geti16()[84] );
	TextContext->printfAt(xLeft+row4,yTop-0.76f,"%d", CharacterStats.geti16()[85] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.76f,"%d", CharacterStats.geti16()[86] );
	TextContext->printfAt(xLeft+row4,yTop-0.785f,"%d", CharacterStats.geti16()[87] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.785f,"%d", CharacterStats.geti16()[88] );
	TextContext->printfAt(xLeft+row4,yTop-0.81f,"%d", CharacterStats.geti16()[89] );
	TextContext->printfAt(xLeft+row4+indent,yTop-0.81f,"%d", CharacterStats.geti16()[90] );
//	TextContext->printfAt(xLeft+row4,yTop-0.835f,"%d", CharacterStats.geti16()[91] );
//	TextContext->printfAt(xLeft+row4+indent,yTop-0.835f,"%d", CharacterStats.geti16()[92] );
//	TextContext->printfAt(xLeft+row4,yTop-0.86f,"%d", CharacterStats.geti16()[93] );
//	TextContext->printfAt(xLeft+row4+indent,yTop-0.86f,"%d", CharacterStats.geti16()[94] );
//	TextContext->printfAt(xLeft+row4,yTop-0.885f,"%d", CharacterStats.geti16()[95] );
//	TextContext->printfAt(xLeft+row4+indent,yTop-0.885f,"%d", CharacterStats.geti16()[96] );
//	TextContext->printfAt(xLeft+row4,yTop-0.91f,"%d", CharacterStats.geti16()[97] );
//
	TextContext->printfAt(xLeft+row4+indent,yTop-0.91f,"%d", CharacterStats.geti16()[98] );

/*
	TextContext->printfAt(xLeft+row4,yTop-1.46f,"%d", CharacterStats.geti16()[99] );
	TextContext->printfAt(xLeft+row4+indent,yTop-1.46f,"%d", CharacterStats.geti16()[100] );

	TextContext->printfAt(xLeft+row4,yTop-1.485f,"%d", CharacterStats.geti16()[101] );
	TextContext->printfAt(xLeft+row4+indent,yTop-1.485f,"%d", CharacterStats.geti16()[102] );

	TextContext->printfAt(xLeft+row4,yTop-1.51f,"%d", CharacterStats.geti16()[103] );
	TextContext->printfAt(xLeft+row4+indent,yTop-1.51f,"%d", CharacterStats.geti16()[104] );

	TextContext->printfAt(xLeft+row4,yTop-1.535f,"%d", CharacterStats.geti16()[105] );
	TextContext->printfAt(xLeft+row4+indent,yTop-1.535f,"%d", CharacterStats.geti16()[106] );
	TextContext->printfAt(xLeft+row4,yTop-1.56f,"%d", CharacterStats.geti16()[107] );
	TextContext->printfAt(xLeft+row4+indent,yTop-1.56f,"%d", CharacterStats.geti16()[108] );
	TextContext->printfAt(xLeft+row4,yTop-1.585f,"%d", CharacterStats.geti16()[109] );
	TextContext->printfAt(xLeft+row4+indent,yTop-1.585f,"%d", CharacterStats.geti16()[110] );
	TextContext->printfAt(xLeft+row4,yTop-1.61f,"%d", CharacterStats.geti16()[111] );
	TextContext->printfAt(xLeft+row4+indent,yTop-1.61f,"%d", CharacterStats.geti16()[112] );
	TextContext->printfAt(xLeft+row4,yTop-1.635f,"%d", CharacterStats.geti16()[113] );
	TextContext->printfAt(xLeft+row4+indent,yTop-1.635f,"%d", CharacterStats.geti16()[114] );
	TextContext->printfAt(xLeft+row4,yTop-1.66f,"%d", CharacterStats.geti16()[115] );
	TextContext->printfAt(xLeft+row4+indent,yTop-1.66f,"%d", CharacterStats.geti16()[116] );
	TextContext->printfAt(xLeft+row4,yTop-1.685f,"%d", CharacterStats.geti16()[117] );
	TextContext->printfAt(xLeft+row4+indent,yTop-1.685f,"%d", CharacterStats.geti16()[118] );
*/
	

/*	CConfigFile::CVar &cvCharacterOrderChaosAlignment = cf.getVar("CharacterOrderChaosAlignment");
	_I16Stats[24] = cvCharacterOrderChaosAlignment.asInt();

	CConfigFile::CVar &cvCharacterAltrismSelfInterestScale = cf.getVar("CharacterAltrismSelfInterestScale");
	_I16Stats[25] = cvCharacterAltrismSelfInterestScale.asInt();

	CConfigFile::CVar &cvCharacterIntrovertExtrovertScale = cf.getVar("CharacterIntrovertExtrovertScale");
	_I16Stats[26] = cvCharacterIntrovertExtrovertScale.asInt();

	CConfigFile::CVar &cvCharacterHedonismIndustryScale = cf.getVar("CharacterHedonismIndustryScale");
	_I16Stats[27] = cvCharacterHedonismIndustryScale.asInt();

	CConfigFile::CVar &cvCharacterIntuitive = cf.getVar("CharacterIntuitive");
	_I16Stats[28] = cvCharacterIntuitive.asInt();
*/


/*		CharacterMoodBase,
		CharacterMoodActual,
		CharacterCalmBase,
		CharacterCalmActual,
		CharacterResonanceBase,
		CharacterResonanceActual,
		CharacterScienceMagicAlignment,
		CharacterHumanismMetaversalismAlignment,
		CharacterOrderChaosAlignment,
		CharacterAltrismSelfInterestScale,
		CharacterIntrovertExtrovertScale,
		CharacterHedonismIndustryScale
	};

	enum I32VectorEnum
	{
		CharacterId,
		CharacterHomelandId,
		CharacterBirthdate,	
		CWesternAstrologicalSignId,
		CChineeseAstrologicalSignId,
		CharacterDevelopmentPhaseId,
		CharacterSocialStandingId
	};

	enum StrVectorEnum
	{
	};

	
	TextContext->setHotSpot(UTextContext::TopRight);
	TextContext->setColor(CRGBA(255,255,255));
	TextContext->setFontSize(14);
	TextContext->printfAt(x+0.05f,1-(h+y+0.023f),"%d", CharacterStats.geti16()[19] );
	TextContext->printfAt(x+0.156f,1-(h+y+0.023f),"%d", (sint) CharacterStats.geti16()[17] );
	TextContext->printfAt(x+0.2695f,1-(h+y+0.023f),"%d", (sint) CharacterStats.geti16()[21]);
	*/
}

/*********************************************************\
					displayStats()
\*********************************************************/
void displaySmallStats ()
{
	Driver->drawBitmap(x, 1-(h+y), w, h, *PsychTexture);

	TextContext->setHotSpot(UTextContext::TopRight);
	TextContext->setColor(CRGBA(255,255,255));
	TextContext->setFontSize(14);
	TextContext->printfAt(x+0.05f,1-(h+y+0.023f),"%d", CharacterStats.geti16()[19] );
	TextContext->printfAt(x+0.156f,1-(h+y+0.023f),"%d", (sint) CharacterStats.geti16()[17] );
	TextContext->printfAt(x+0.2695f,1-(h+y+0.023f),"%d", (sint) CharacterStats.geti16()[21]);
}


void cbUpdateStatsDisplay (CConfigFile::CVar &var)
{
	if (var.Name == "StatsDisplayState") StatsDisplayState = var.asInt ();
}

void initStatsDisplay ()
{
	ConfigFile.setCallback ("StatsDisplayState", cbUpdateStatsDisplay);

	PsychTexture = Driver->createTextureFile ("psych.tga");
	if (ShowDebugInChat) nlinfo("Init Stats Display (state)=%d", StatsDisplayState);
}

void updateStatsDisplay ()
{
	switch (StatsDisplayState)
	{
	case 0:
		break;
	case 1:
		displaySmallStats ();
		break;
	case 2:
		displayLargeStats ();
		break;
	}
}

void releaseStatsDisplay ()
{
}


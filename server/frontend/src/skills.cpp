/** \file skills.cpp
 * <File description>
 *
 * $Id: skills.cpp,v 1.1 2002/03/20 18:10:21 robofly Exp $
 */

/* Copyright, 2001 InOrbit Entertainment, Inc.
 *
 * This file is part of NEVRAX NEL.
 * NEVRAX NEL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * NEVRAX NEL is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with NEVRAX NEL; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#include "skills.h"
#include "nel/misc/common.h"
#include "nel/misc/command.h"
#include "nel/misc/debug.h"
#include "nel/misc/path.h"
#include "nel/misc/i18n.h"
#include <iostream.h>
#include "nel/misc/types_nl.h"
#include "nel/misc/file.h" 

// contains the config class
#include "nel/misc/config_file.h"

using namespace std;
using namespace NLMISC;



// ***************************************************************************
void			CSkills::load(std::string configFileName)
{

	_Updated=false;
	CIFile f;
	try
	{
		if (f.open(configFileName+".skills"))
		{
			printf("reading skills for %s\n", configFileName.c_str());
			serial(f);
			f.close();
			_MyName=configFileName;
		}
		else
		{
			printf("Serialized file not found: %s.skills\n", configFileName.c_str());
			printf("Entering skills createFromConfigFile file.");
			createFromConfigFile(configFileName);
			printf("I have saved the file\n");
			printf("Serialized file is done and written");
			f.close();
			_MyName=configFileName;
		}
		}
	catch(exception &e)
	{
		printf ("My T3 Error %s\n", e.what ());
	}


}

void CSkills::createFromTemplate(std::string templateFileName, std::string newFile)
{
	createFromConfigFile(templateFileName);
	save(newFile);
}

void			CSkills::serializeIfUpdated()
{
	if (_Updated)
	{
		save(_MyName);
		_Updated=false;
	}
}

void			CSkills::serialize()
{
	save(_MyName);
	_Updated=false;
}



// ***************************************************************************
void			CSkills::save(std::string configFileName)
{
	COFile f(configFileName+".skills");
	try
	{
		serial(f);
		f.close();
	}
	catch(exception &e)
	{
		printf ("Error %s\n", e.what ());
	}
	printf("We have save the serialized file called %s.\n", configFileName);

}


// ***************************************************************************
/*
	Demo General Knowledge Skill Tree
			

*0-COMMON SENSE
0-Luck

*10-PHILOSOPHY (All philosophy skills allow the character to change alignments and goals)
10-Western Secular
11-Pragmatism
12-Humanism
13-Transcendentalism
14-Existentialism
15-Ecstatic Nihilism
*	Eastern Secular
16-		Taoism
17-		Confucianism
*	Theology
18-		Paganism
19-		Judaism 
20-		Christianity 
21-		Islam 
22-		Buddhism
23-		Metaversalism

*40-HISTORY (History Skills provide extra information when examining objects and people)
40-	Ancient (to 1000 AD)
41-	Modern (1000 AD to 2093)
42-	Sagas of the Great Dark
43-	History of the Metaverse

50-SURVIVAL (allows character to make fire)
51-	Foraging (allows the character to search for herbs, vegetation)
52-	Hunting (allows character to locate animals to hunt, track creatures)
53-	Directional Sense (allows character to assess cardinal directions)
*60-FAITH
60-	Piety
61-	Hermeneutics
62-	Divining

Mystic Practice Skill Tree 

*70-MAGIC
*70-	Sorcery
*70-		Material Realm
70-			Solid (Earth)
71			Liquid (Water)
72			Gaseous (Air)
73			Plasma (Fire)
*74		Immaterial Realm
74-			Electricity and Magnetism (Air) 
75-			Time and Gravity (Earth)
76-			Morphic Resonance (Water)
77-			Weak, Strong & Gamma(Fire)
*78	Invocation
*78		Asatru (each god requires specialization)
-78			Odin (2M, 2H, 4O)
-79			Thor (2M, 4H, 2C)
-80			Frey (4M, 2M, 2O)
-81			Loki (4M, 4M, 4C)
-82			Freya (2M, 2H, N)
-83			Balder (N, 1H, 3O)

*90-RITUALS
*90	Mind
-90		Meditation (Increases Calm & Resonance)
-91		Trancing 	(Increased Mood & Resonance)
*92Body
-92		Chanting (Increased Mood & Resonance, decreases calm)
-93		Body Modification (Increased Mood)

  
*Action Skill Tree

*100-LOCOMOTION
-100	Jumping
-101	Running

*120-COMBAT
*120-	Throwing
-120		Blades
-121		Spherical
*122	Aimed
-122		Blow dart
-123		Bow
-124		Crossbow
-125		Pistol
-126		Projectile
-127		SMG
-128		Energy
-129		Long Arms
-130			Rifle
-131			Auto
-132			Heavy 
-133			Launcher


*140-STEALTH
*140	BodyHide (
-140		Sneak
-141		Pickpocket 

*150-PHYSICAL ARTS 
-150	Dancing 

*/
void			CSkills::createFromConfigFile( std::string configFileName)
{
	
	printf("Entering createFromConfigFile file.");
	
	CConfigFile cf;

	cf.load(configFileName+".textskills");

	printf("About to resize\n");

	_I16Skills.resize(151);

	_I16Skills[0] = cf.getVar("Luck").asInt();
	_I16Skills[10] = cf.getVar("WesternSecular").asInt();
	_I16Skills[11] = cf.getVar("Pragmatism").asInt();
	_I16Skills[12] = cf.getVar("Humanism").asInt();
	_I16Skills[13] = cf.getVar("Transcendentalism").asInt();
	_I16Skills[14] = cf.getVar("Existentialism").asInt();
	_I16Skills[15] = cf.getVar("EcstaticNihilism").asInt();
	_I16Skills[16] = cf.getVar("Taoism").asInt();
	_I16Skills[17] = cf.getVar("Confucianism").asInt();
	_I16Skills[18] = cf.getVar("Paganism").asInt();
	_I16Skills[19] = cf.getVar("Judaism").asInt();
	_I16Skills[20] = cf.getVar("Christianity").asInt();
	_I16Skills[21] = cf.getVar("Islam").asInt();
	_I16Skills[22] = cf.getVar("Buddhism").asInt();
	_I16Skills[23] = cf.getVar("Metaversalism").asInt();
	_I16Skills[40] = cf.getVar("AncientHistory").asInt();
	_I16Skills[41] = cf.getVar("ModernHistory").asInt();
	_I16Skills[42] = cf.getVar("GreatDarkSagas").asInt();
	_I16Skills[43] = cf.getVar("MetaverseHistory").asInt();
	_I16Skills[50] = cf.getVar("Survival").asInt();
	_I16Skills[51] = cf.getVar("Foraging").asInt();
	_I16Skills[52] = cf.getVar("Hunting").asInt();
	_I16Skills[53] = cf.getVar("Directional").asInt();
	_I16Skills[60] = cf.getVar("Piety").asInt();
	_I16Skills[61] = cf.getVar("Hermeneutics").asInt();
	_I16Skills[62] = cf.getVar("Divining").asInt();
	_I16Skills[70] = cf.getVar("Solid").asInt();
	_I16Skills[71] = cf.getVar("Liquid").asInt();
	_I16Skills[72] = cf.getVar("Gaseous").asInt();
	_I16Skills[73] = cf.getVar("Plasma").asInt();
	_I16Skills[74] = cf.getVar("Electricity").asInt();
	_I16Skills[75] = cf.getVar("Time").asInt();
	_I16Skills[76] = cf.getVar("Morphic").asInt();
	_I16Skills[77] = cf.getVar("WeakStrongGamma").asInt();
	_I16Skills[78] = cf.getVar("Odin").asInt();
	_I16Skills[79] = cf.getVar("Thor").asInt();
	_I16Skills[80] = cf.getVar("Frey").asInt();
	_I16Skills[81] = cf.getVar("Loki").asInt();
	_I16Skills[82] = cf.getVar("Freya").asInt();
	_I16Skills[83] = cf.getVar("Balder").asInt();
	_I16Skills[90] = cf.getVar("Meditation").asInt();
	_I16Skills[91] = cf.getVar("Trancing").asInt();
	_I16Skills[93] = cf.getVar("Chanting").asInt();
	_I16Skills[94] = cf.getVar("BodyModification").asInt();
	_I16Skills[100] = cf.getVar("Jumping").asInt();
	_I16Skills[101] = cf.getVar("Running").asInt();
	_I16Skills[120] = cf.getVar("ThrowingBlades").asInt();
	_I16Skills[121] = cf.getVar("ThrowingSpherical").asInt();
	_I16Skills[122] = cf.getVar("AimedBlowdart").asInt();
	_I16Skills[123] = cf.getVar("AimedBow").asInt();
	_I16Skills[124] = cf.getVar("AimedCrossbow").asInt();
	_I16Skills[125] = cf.getVar("AimedPistol").asInt();
	_I16Skills[126] = cf.getVar("AimedProjectile").asInt();
	_I16Skills[127] = cf.getVar("AimedSMG").asInt();
	_I16Skills[128] = cf.getVar("AimedEnergy").asInt();
	_I16Skills[129] = cf.getVar("AimedLongArms").asInt();
	_I16Skills[130] = cf.getVar("AimedRifle").asInt();
	_I16Skills[131] = cf.getVar("AimedAuto").asInt();
	_I16Skills[132] = cf.getVar("AimedHeavy").asInt();
	_I16Skills[133] = cf.getVar("AimedLauncher").asInt();
	_I16Skills[140] = cf.getVar("Sneak").asInt();
	_I16Skills[141] = cf.getVar("Pickpocket").asInt();
	_I16Skills[150] = cf.getVar("Dancing").asInt();

	printf("I have created the internal structure from config file.\n");

	save(configFileName);
	printf("I have saved the file\n");

}

// ***************************************************************************
void			CSkills::update(uint8 skillId, uint16 value)
{
	_I16Skills[skillId]=value;
	_Updated=true;
}

// ***************************************************************************
void			CSkills::updateTally(uint8 skillId, uint16 value)
{
	_I16Tallies[skillId]=value;
	_Updated=true;
}

// ***************************************************************************
void			CSkills::serial(NLMISC::IStream &f)
{
	nlinfo("Skill are about to...");
	uint	ver= f.serialVersion(0);
	f.serialCont(_I16Skills);
	f.serialCont(_I16Tallies);
	nlinfo("Skills are in!");
}


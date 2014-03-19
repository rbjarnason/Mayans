/** \file stats.cpp
 * <File description>
 *
 * $Id: stats.cpp,v 1.2 2002/06/06 05:51:01 robofly Exp $
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

#include "stats.h"
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
#include "client.h"

using namespace std;
using namespace NLMISC;



// ***************************************************************************
void CStats::load(std::string configFileName)
{

	_Updated=false;
	CIFile f;
	try
	{
		if (f.open(configFileName+".stats"))
		{
			printf("reading stats for %s\n", configFileName.c_str());
			serial(f);
			f.close();
			_MyName=configFileName;
		}
		else
		{
			printf("Serialized file not found: %s.stats\n", configFileName.c_str());
			printf("Entering createFromConfigFile file.");
			createFromConfigFile(configFileName);
			save(configFileName);
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

void CStats::createFromTemplate(std::string templateFileName, std::string newFile)
{
	CIFile f;
	try
	{
		if (f.open(templateFileName+".stats"))
		{
			printf("reading template stats for %s\n", templateFileName.c_str());
			serial(f);
			f.close();
			_MyName=newFile;
		}
		else
		{
			printf("Serialized file not found: %s.stats\n", templateFileName.c_str());
			printf("Entering createFromConfigFile file.");
			createFromConfigFile(templateFileName);
			_MyName=newFile;
		}
		}
	catch(exception &e)
	{
		printf ("Error %s\n", e.what ());
	}

	_I32Stats[0]=(uint32) newFile.c_str();
	save(newFile);
}

void CStats::serializeIfUpdated()
{
	if (_Updated)
	{
		save(_MyName);
		_Updated=false;
	}
}

void CStats::serialize()
{
	save(_MyName);
	_Updated=false;
}

// ***************************************************************************
void CStats::save(std::string configFileName)
{
	COFile f(configFileName+".stats");
	try
	{

		serial(f);
		f.close();
	}
	catch(exception &e)
	{
		printf ("HMM is this MY Error %s\n", e.what ());
	}
//	f.close();
	printf("We have save the serialized file called %s.\n", configFileName);

}


// ***************************************************************************
void CStats::createFromConfigFile( std::string configFileName)
{
	
	printf("Entering createFromConfigFile file.");
	if (ShowDebugInChat) nlinfo("%s",configFileName);
	CConfigFile cf;

	cf.load(configFileName+".textstats");

	printf("About to resize\n");

	_I16Stats.resize(120);
	_I32Stats.resize(6);
	_StrStats.resize(9);
	_VecStats.resize(2);
	_FloatStats.resize(3);

	CConfigFile::CVar &cvCharacterId = cf.getVar("CharacterId");
	_I32Stats[0] = cvCharacterId.asInt();


	_StrStats[0] = cf.getVar("CharacterFirstName").asString();

	_StrStats[1] = cf.getVar("CharacterLastName").asString();

	CConfigFile::CVar &cvCharacterHomelandId = cf.getVar("CharacterHomelandId");
	_I32Stats[1] = cvCharacterHomelandId.asInt();

	CConfigFile::CVar &cvCharacterGender = cf.getVar("CharacterGender");
	_StrStats[2] = cvCharacterGender.asString();

	CConfigFile::CVar &cvCharacterHandedness = cf.getVar("CharacterHandedness");
	_StrStats[3] = cvCharacterHandedness.asString();

	CConfigFile::CVar &cvCharacterBasePigment = cf.getVar("CharacterBasePigment");
	_StrStats[4] = cvCharacterBasePigment.asString();

	CConfigFile::CVar &cvCharacterHairColor = cf.getVar("CharacterHairColor");
	_StrStats[5] = cvCharacterHairColor.asString();

	CConfigFile::CVar &cvCharacterHairType = cf.getVar("CharacterHairType");
	_StrStats[6] = cvCharacterHairType.asString();

	CConfigFile::CVar &cvCharacterEyeColr = cf.getVar("CharacterEyeColr");
	_StrStats[7] = cvCharacterEyeColr.asString();

	CConfigFile::CVar &cvCharacterWeight = cf.getVar("CharacterWeight");
	_I16Stats[0] = cvCharacterWeight.asInt();

	CConfigFile::CVar &cvCharacterHeight = cf.getVar("CharacterHeight");
	_I16Stats[1] = cvCharacterHeight.asInt();

	CConfigFile::CVar &cvCharacterBirthdate = cf.getVar("CharacterBirthdate");
	_I32Stats[2] = cvCharacterBirthdate.asInt();

	CConfigFile::CVar &cvCWesternAstrologicalSignId = cf.getVar("CWesternAstrologicalSignId");
	_I32Stats[3] = cvCWesternAstrologicalSignId.asInt();

	CConfigFile::CVar &cvCChineeseAstrologicalSignId = cf.getVar("CChineeseAstrologicalSignId");
	_I32Stats[4] = cvCChineeseAstrologicalSignId.asInt();

	CConfigFile::CVar &cvCharacterDevelopmentPhaseId = cf.getVar("CharacterDevelopmentPhaseId");
	_I32Stats[5] = cvCharacterDevelopmentPhaseId.asInt();

	CConfigFile::CVar &cvCharacterSocialStandingId = cf.getVar("CharacterSocialStandingId");
	_I32Stats[6] = cvCharacterSocialStandingId.asInt();

	CConfigFile::CVar &cvCharacterMentalFatigue = cf.getVar("CharacterMentalFatigue");
	_I16Stats[3] = cvCharacterMentalFatigue.asInt();

	CConfigFile::CVar &cvCharacterPhysicalFatigue = cf.getVar("CharacterPhysicalFatigue");
	_I16Stats[4] = cvCharacterPhysicalFatigue.asInt();

	CConfigFile::CVar &cvCharacterCombatStance = cf.getVar("CharacterCombatStance");
	_I16Stats[5] = cvCharacterCombatStance.asInt();

	CConfigFile::CVar &cvCharacterHydrationLevel = cf.getVar("CharacterHydrationLevel");
	_I16Stats[6] = cvCharacterHydrationLevel.asInt();

	CConfigFile::CVar &cvCharacterNutritionLevel1 = cf.getVar("CharacterNutritionLevel1");
	_I16Stats[7] = cvCharacterNutritionLevel1.asInt();

	CConfigFile::CVar &cvCharacterNutritionLevel2 = cf.getVar("CharacterNutritionLevel2");
	_I16Stats[8] = cvCharacterNutritionLevel2.asInt();

	CConfigFile::CVar &cvCharacterNutritionLevel3 = cf.getVar("CharacterNutritionLevel3");
	_I16Stats[9] = cvCharacterNutritionLevel3.asInt();

	CConfigFile::CVar &cvCharacterOpenCounts = cf.getVar("CharacterOpenCounts");
	_I16Stats[10] = cvCharacterOpenCounts.asInt();

	CConfigFile::CVar &cvCharacterFreeCounts = cf.getVar("CharacterFreeCounts");
	_I16Stats[11] = cvCharacterFreeCounts.asInt();

	CConfigFile::CVar &cvCharacterSpellCounts = cf.getVar("CharacterSpellCounts");
	_I16Stats[12] = cvCharacterSpellCounts.asInt();

	CConfigFile::CVar &cvCharacterKarma = cf.getVar("CharacterKarma");
	_I16Stats[13] = cvCharacterKarma.asInt();

	CConfigFile::CVar &cvCharacterRRPoints = cf.getVar("CharacterRRPoints");
	_I16Stats[14] = cvCharacterRRPoints.asInt();

	CConfigFile::CVar &cvCharacterErasedNegs = cf.getVar("CharacterErasedNegs");
	_I16Stats[15] = cvCharacterErasedNegs.asInt();

	printf("Near end of chardump\n");

	CConfigFile::CVar &cvCharacterMoodBase = cf.getVar("CharacterMoodBase");
	_I16Stats[16] = cvCharacterMoodBase.asInt();

	CConfigFile::CVar &cvCharacterMoodActual = cf.getVar("CharacterMoodActual");
	_I16Stats[17] = cvCharacterMoodActual.asInt();

	CConfigFile::CVar &cvCharacterCalmBase = cf.getVar("CharacterCalmBase");
	_I16Stats[18] = cvCharacterCalmBase.asInt();

	CConfigFile::CVar &cvCharacterCalmActual = cf.getVar("CharacterCalmActual");
	_I16Stats[19] = cvCharacterCalmActual.asInt();

	CConfigFile::CVar &cvCharacterResonanceBase = cf.getVar("CharacterResonanceBase");
	_I16Stats[20] = cvCharacterResonanceBase.asInt();

	CConfigFile::CVar &cvCharacterResonanceActual = cf.getVar("CharacterResonanceActual");
	_I16Stats[21] = cvCharacterResonanceActual.asInt();

	CConfigFile::CVar &cvCharacterScienceMagicAlignment = cf.getVar("CharacterScienceMagicAlignment");
	_I16Stats[22] = cvCharacterScienceMagicAlignment.asInt();

	CConfigFile::CVar &cvCharacterHumanismMetaversalismAlignment = cf.getVar("CharacterHumanismMetaversalismAlignment");
	_I16Stats[23] = cvCharacterHumanismMetaversalismAlignment.asInt();
 
	CConfigFile::CVar &cvCharacterOrderChaosAlignment = cf.getVar("CharacterOrderChaosAlignment");
	_I16Stats[24] = cvCharacterOrderChaosAlignment.asInt();

	CConfigFile::CVar &cvCharacterAltrismSelfInterestScale = cf.getVar("CharacterAltrismSelfInterestScale");
	_I16Stats[25] = cvCharacterAltrismSelfInterestScale.asInt();

	CConfigFile::CVar &cvCharacterIntrovertExtrovertScale = cf.getVar("CharacterIntrovertExtrovertScale");
	_I16Stats[26] = cvCharacterIntrovertExtrovertScale.asInt();

	CConfigFile::CVar &cvCharacterHedonismIndustryScale = cf.getVar("CharacterHedonismIndustryScale");
	_I16Stats[27] = cvCharacterHedonismIndustryScale.asInt();

	CConfigFile::CVar &cvCharacterIntuitive = cf.getVar("CharacterIntuitive");
	_I16Stats[28] = cvCharacterIntuitive.asInt();

	_I16Stats[29] = cf.getVar("FacultiesBase").asInt();
	_I16Stats[30] = cf.getVar("FacultiesActual").asInt();
	_I16Stats[31] = cf.getVar("CognitiveFocusBase").asInt();
	_I16Stats[32] = cf.getVar("CognitiveFocusActual").asInt();
	_I16Stats[33] = cf.getVar("CognitiveSpeedBase").asInt();
	_I16Stats[34] = cf.getVar("CognitiveSpeedActual").asInt();
	_I16Stats[35] = cf.getVar("IntentionalWillBase").asInt();
	_I16Stats[36] = cf.getVar("IntentionalWillActual").asInt();
	_I16Stats[37] = cf.getVar("SubconsciousWillBase").asInt();
	_I16Stats[38] = cf.getVar("SubconsciousWillActual").asInt();
	_I16Stats[39] = cf.getVar("MnemonicsBase").asInt();
	_I16Stats[40] = cf.getVar("MnemonicsActual").asInt();
	_I16Stats[41] = cf.getVar("MultitaskingBase").asInt();
	_I16Stats[42] = cf.getVar("MultitaskingActual").asInt();
	_I16Stats[43] = cf.getVar("NaturalisticBase").asInt();
	_I16Stats[44] = cf.getVar("NaturalisticActual").asInt();
	_I16Stats[45] = cf.getVar("ConceptualBase").asInt();
	_I16Stats[46] = cf.getVar("ConceptualActual").asInt();
	_I16Stats[47] = cf.getVar("IntuitiveBase").asInt();
	_I16Stats[48] = cf.getVar("IntuitiveActual").asInt();
	_I16Stats[49] = cf.getVar("ArtisticBase").asInt();
	_I16Stats[50] = cf.getVar("ArtisticActual").asInt();
	_I16Stats[51] = cf.getVar("VerbalBase").asInt();
	_I16Stats[52] = cf.getVar("VerbalActual").asInt();
	_I16Stats[53] = cf.getVar("MechanisticBase").asInt();
	_I16Stats[54] = cf.getVar("MechanisticActual").asInt();
	_I16Stats[55] = cf.getVar("MathematicalBase").asInt();
	_I16Stats[56] = cf.getVar("MathematicalActual").asInt();
	_I16Stats[57] = cf.getVar("PhysicalityBase").asInt();
	_I16Stats[58] = cf.getVar("PhysicalityActual").asInt();
	_I16Stats[59] = cf.getVar("UpperBodyFastTwitchBase").asInt();
	_I16Stats[60] = cf.getVar("UpperBodyFastTwitchActual").asInt();
	_I16Stats[61] = cf.getVar("UpperBodySlowTwitchBase").asInt();
	_I16Stats[62] = cf.getVar("UpperBodySlowTwitchActual").asInt();
	_I16Stats[63] = cf.getVar("LowerBodyfastTwitchBase").asInt();
	_I16Stats[64] = cf.getVar("LowerBodyfastTwitchActual").asInt();
	_I16Stats[65] = cf.getVar("LowerBodySlowTwitchBase").asInt();
	_I16Stats[66] = cf.getVar("LowerBodySlowTwitchActual").asInt();
	_I16Stats[67] = cf.getVar("BoneMassBase").asInt();
	_I16Stats[68] = cf.getVar("BoneMassActual").asInt();
	_I16Stats[69] = cf.getVar("CirculatoryBase").asInt();
	_I16Stats[70] = cf.getVar("CirculatoryActual").asInt();
	_I16Stats[71] = cf.getVar("FiltrationBase").asInt();
	_I16Stats[72] = cf.getVar("FiltrationActual").asInt();
	_I16Stats[73] = cf.getVar("RespiratoryBase").asInt();
	_I16Stats[74] = cf.getVar("RespiratoryActual").asInt();
	_I16Stats[75] = cf.getVar("ThermoregulatoryBase").asInt();
	_I16Stats[76] = cf.getVar("ThermoregulatoryActual").asInt();
	_I16Stats[77] = cf.getVar("MetabolismBase").asInt();
	_I16Stats[78] = cf.getVar("MetabolismActual").asInt();
	_I16Stats[79] = cf.getVar("ImmunoResponseBase").asInt();
	_I16Stats[80] = cf.getVar("ImmunoResponseActual").asInt();
	_I16Stats[81] = cf.getVar("RegeneratoryBase").asInt();
	_I16Stats[82] = cf.getVar("RegeneratoryActual").asInt();
	_I16Stats[83] = cf.getVar("SexualReproductionBase").asInt();
	_I16Stats[84] = cf.getVar("SexualReproductionActual").asInt();
	_I16Stats[85] = cf.getVar("ReflexSpeedBase").asInt();
	_I16Stats[86] = cf.getVar("ReflexSpeedActual").asInt();
	_I16Stats[87] = cf.getVar("HandEyeBase").asInt();
	_I16Stats[88] = cf.getVar("HandEyeActual").asInt();
	_I16Stats[89] = cf.getVar("FinemanualBase").asInt();
	_I16Stats[90] = cf.getVar("FinemanualActual").asInt();
	_I16Stats[91] = cf.getVar("LargemuscleBase").asInt();
	_I16Stats[92] = cf.getVar("LargemuscleActual").asInt();
	_I16Stats[93] = cf.getVar("RhythmicBase").asInt();
	_I16Stats[94] = cf.getVar("RhythmicActual").asInt();
	_I16Stats[95] = cf.getVar("VisualBase").asInt();
	_I16Stats[96] = cf.getVar("VisualActual").asInt();
	_I16Stats[97] = cf.getVar("AuralBase").asInt();
	_I16Stats[98] = cf.getVar("AuralActual").asInt();
	_I16Stats[99] = cf.getVar("TactileBase").asInt();
	_I16Stats[100] = cf.getVar("TactileActual").asInt();
	_I16Stats[101] = cf.getVar("OlfactoryBase").asInt();
	_I16Stats[102] = cf.getVar("OlfactoryActual").asInt();
	_I16Stats[103] = cf.getVar("GustatoryBase").asInt();
	_I16Stats[104] = cf.getVar("GustatoryActual").asInt();
	_I16Stats[105] = cf.getVar("HeightBase").asInt();
	_I16Stats[106] = cf.getVar("HeightActual").asInt();
	_I16Stats[107] = cf.getVar("WeightBase").asInt();
	_I16Stats[108] = cf.getVar("WeightActual").asInt();
	_I16Stats[109] = cf.getVar("ProportionBase").asInt();
	_I16Stats[110] = cf.getVar("ProportionActual").asInt();
	_I16Stats[111] = cf.getVar("FacialsymmetryBase").asInt();
	_I16Stats[112] = cf.getVar("FacialsymmetryActual").asInt();
	_I16Stats[113] = cf.getVar("SkinthicknessBase").asInt();
	_I16Stats[114] = cf.getVar("SkinthicknessActual").asInt();
	_I16Stats[115] = cf.getVar("PigmentationBase").asInt();
	_I16Stats[116] = cf.getVar("PigmentationActual").asInt();
	_I16Stats[117] = cf.getVar("TopicalIntegrityBase").asInt();
	_I16Stats[118] = cf.getVar("TopicalIntegrityActual").asInt();
	_I16Stats[119] = cf.getVar("ClassId").asInt();
	_StrStats[8] = cf.getVar("MeshName").asString(); 
	_FloatStats[0] = cf.getVar("Radius").asFloat();
	_FloatStats[1] = cf.getVar("Weight").asFloat();
	_FloatStats[2] = cf.getVar("Scale").asFloat();
		
	CConfigFile::CVar &cvPosition = cf.getVar("Position");
	nlassert(cvPosition.size()==3);

	_VecStats[0].x = cf.getVar("Position").asFloat(0);
	_VecStats[0].y = cf.getVar("Position").asFloat(1);
	_VecStats[0].z = cf.getVar("Position").asFloat(2);

	_VecStats[1].x = cf.getVar("Heading").asFloat(0);
	_VecStats[1].y = cf.getVar("Heading").asFloat(1);
	_VecStats[1].z = cf.getVar("Heading").asFloat(2);

	_VecStats[2].x = cf.getVar("MassCenter").asFloat(0);
	_VecStats[2].y = cf.getVar("MassCenter").asFloat(1);
	_VecStats[2].z = cf.getVar("MassCenter").asFloat(2);

	_VecStats[3].x = cf.getVar("Dimensions").asFloat(0);
	_VecStats[3].y = cf.getVar("Dimensions").asFloat(1);
	_VecStats[3].z = cf.getVar("Dimensions").asFloat(2);

	printf("I have created the internal structure from config file.\n");

}

// ***************************************************************************
void			CStats::update(uint8 statId, sint16 value)
{
	_I16Stats[statId]=value;
	_Updated=true;
}

// ***************************************************************************
void			CStats::update(uint8 statId, uint32 value)
{
	_I32Stats[statId]=value;
	_Updated=true;
}

// ***************************************************************************
void			CStats::update(uint8 statId, std::string value)
{
	_StrStats[statId]=value;
	_Updated=true;
}

// ***************************************************************************
void			CStats::update(uint8 statId, NLMISC::CVector value)
{
	_VecStats[statId]=value;
	_Updated=true;
}

// ***************************************************************************
void			CStats::serial(NLMISC::IStream &f)
{
	uint	ver= f.serialVersion(1);
	f.serialCont(_I16Stats);
	f.serialCont(_I32Stats);
	f.serialCont(_StrStats);
	f.serialCont(_VecStats);
	f.serialCont(_FloatStats);
}

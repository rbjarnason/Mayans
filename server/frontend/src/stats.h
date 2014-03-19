/** \file stats.h
 * <File description>
 *
 * $Id: stats.h,v 1.1 2002/03/20 18:10:21 robofly Exp $
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
 
#ifndef NL_STATS_H
#define NL_STATS_H

#include "nel/misc/types_nl.h"
#include "nel/misc/smart_ptr.h"
#include "nel/misc/matrix.h"
#include "nel/misc/stream.h"
#include "nel/misc/debug.h"
#include <stdio.h>
#include <vector>
#include <map>

using namespace std;

/*
*Characteristic
*Mental Statistics
Faculties
Cognitive Focus
Cognitive Speed
Intentional Will
Subconscious Will
Mnemonics
Multitasking
*Aptitudes
Naturalistic
Conceptual
Intuitive
Artistic
Verbal
Mechanistic
Mathematical
Physicality
*Physical Statistics
*Musculo-Skeletal System
Upper Body Fast Twitch
Upper Body Slow Twitch
Lower Body fast Twitch
Lower Body Slow Twitch
Bone Mass
*Internal Systems
Circulatory
Filtration
Respiratory
Thermoregulatory
Metabolism
Immuno Response
Regeneratory
Sexual Reproduction
*Motor Control Systems
Reflex Speed
Hand Eye
Fine manual
Large muscle
Rhythmic
*Sensory Acuity
Visual
Aural
Tactile
Olfactory
Gustatory
*Base Dimensions & Dermatology
Height
Weight
Proportion
Facial symmetry
Skin thickness
Pigmentation
Topical Integrity



/***************************************************************************
 * Keeps track of character statistic, is serialized into files
 * Called 
 * \author Róbert Bjarnason
 * \author InOrbit Entertainment, Inc.
 * \date 2000
 */
class CStats
{


public:

	/// Constructor
	CStats()
	{
		_I16Stats.resize(120);
		_I32Stats.resize(6);
		_StrStats.resize(9);
		_VecStats.resize(4);
		_FloatStats.resize(3);
	};

	enum I16VectorEnum
	{
		CharacterWeight,		
		CharacterHeight,	
		CharacterAge,	
		CharacterMentalFatigue,	
		CharacterPhysicalFatigue,
		CharacterCombatStance,	
		CharacterHydrationLevel,	
		CharacterNutritionLevel1,
		CharacterNutritionLevel2,
		CharacterNutritionLevel3,
		CharacterOpenCounts,		
		CharacterFreeCounts,	
		CharacterSpellCounts,	
		CharacterKarma,			
		CharacterRRPoints,		
		CharacterErasedNegs,		
		CharacterMoodBase,
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
		CharacterHedonismIndustryScale,
		CharacterFacultiesBase,
		CharacterFacultiesActual,
		CharacterCognitiveFocusBase,
		CharacterCognitiveFocusActual,
		CharacterCognitiveSpeedBase,
		CharacterCognitiveSpeedActual,
		CharacterIntentionalWillBase,
		CharacterIntentionalWillActual,
		CharacterSubconsciousWillBase,
		CharacterSubconsciousWillActual,
		CharacterMnemonicsBase,
		CharacterMnemonicsActual,
		CharacterMultitaskingBase,
		CharacterMultitaskingActual,
		CharacterNaturalisticBase,
		CharacterNaturalisticActual,
		CharacterConceptualBase,
		CharacterConceptualActual,
		CharacterIntuitiveBase,
		CharacterIntuitiveActual,
		CharacterArtisticBase,
		CharacterArtisticActual,
		CharacterVerbalBase,
		CharacterVerbalActual,
		CharacterMechanisticBase,
		CharacterMechanisticActual,
		CharacterMathematicalBase,
		CharacterMathematicalActual,
		CharacterPhysicalityBase,
		CharacterPhysicalityActual,
		CharacterUpperBodyFastTwitchBase,
		CharacterUpperBodyFastTwitchActual,
		CharacterUpperBodySlowTwitchBase,
		CharacterUpperBodySlowTwitchActual,
		CharacterLowerBodyfastTwitchBase,
		CharacterLowerBodyfastTwitchActual,
		CharacterLowerBodySlowTwitchBase,
		CharacterLowerBodySlowTwitchActual,
		CharacterBoneMassBase,
		CharacterBoneMassActual,
		CharacterCirculatoryBase,
		CharacterCirculatoryActual,
		CharacterFiltrationBase,
		CharacterFiltrationActual,
		CharacterRespiratoryBase,
		CharacterRespiratoryActual,
		CharacterThermoregulatoryBase,
		CharacterThermoregulatoryActual,
		CharacterMetabolismBase,
		CharacterMetabolismActual,
		CharacterImmunoResponseBase,
		CharacterImmunoResponseActual,
		CharacterRegeneratoryBase,
		CharacterRegeneratoryActual,
		CharacterSexualReproductionBase,
		CharacterSexualReproductionActual,
		CharacterReflexSpeedBase,
		CharacterReflexSpeedActual,
		CharacterHandEyeBase,
		CharacterHandEyeActual,
		CharacterFinemanualBase,
		CharacterFinemanualActual,
		CharacterLargemuscleBase,
		CharacterLargemuscleActual,
		CharacterRhythmicBase,
		CharacterRhythmicActual,
		CharacterVisualBase,
		CharacterVisualActual,
		CharacterAuralBase,
		CharacterAuralActual,
		CharacterTactileBase,
		CharacterTactileActual,
		CharacterOlfactoryBase,
		CharacterOlfactoryActual,
		CharacterGustatoryBase,
		CharacterGustatoryActual,
		CharacterHeightBase,
		CharacterHeightActual,
		CharacterWeightBase,
		CharacterWeightActual,
		CharacterProportionBase,
		CharacterProportionActual,
		CharacterFacialsymmetryBase,
		CharacterFacialsymmetryActual,
		CharacterSkinthicknessBase,
		CharacterSkinthicknessActual,
		CharacterPigmentationBase,
		CharacterPigmentationActual,
		CharacterTopicalIntegrityBase,
		CharacterTopicalIntegrityActual,
		ClassId
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
		CharacterFirstName,
		CharacterLastName,
		CharacterGender,	
		CharacterHandedness,
		CharacterBasePigment,
		CharacterHairColor,
		CharacterHairType,	
		CharacterEyeColr,
		MeshName
	};

	enum FloatVectorEnum
	{
		Radius,
		Weight,
		Scale
	};

	~CStats() {}

	void				createFromConfigFile( std::string configFileName );

	void				load( std::string configFileName );

	void				createFromTemplate( std::string templateFileName, std::string newFile );

	void				serializeIfUpdated();

	void				serialize();

	void				serial(NLMISC::IStream &f);

	void				update(uint8 statId, float value);
	void				update(uint8 statId, sint16 value);
	void				update(uint8 statId, uint32 value);
	void				update(uint8 statId, std::string value);
	void				update(uint8 statId, NLMISC::CVector value);

	vector<float>			getfloat() const { return _FloatStats; }
	vector<sint16>			geti16() const { return _I16Stats; }
	vector<uint32>			geti32() const { return _I32Stats; }
	vector<string>			getstr() const { return _StrStats; }
	vector<NLMISC::CVector>	getvec() const { return _VecStats; }

// 0	sint16			CharacterWeight;	
// 1	sint16			CharacterHeight;	
// 2	sint16			CharacterAge;	
// 3	sint16			CharacterMentalFatigue;	
// 4	sint16			CharacterPhysicalFatigue;
// 5	sint16			CharacterCombatStance;	
// 6	sint16			CharacterHydrationLevel;	
// 7	sint16			CharacterNutritionLevel1;
// 8	sint16			CharacterNutritionLevel2;
// 9	sint16			CharacterNutritionLevel3;
// 10	sint16			CharacterOpenCounts;		
// 11	sint16			CharacterFreeCounts;	
// 12	sint16			CharacterSpellCounts;	
// 13	sint16			CharacterKarma;			
// 14	sint16			CharacterRRPoints;		
// 15	sint16			CharacterErasedNegs;		
// 16	sint16			CharacterMoodBase;
// 17	sint16			CharacterMoodActual;
// 18	sint16			CharacterCalmBase;
// 19	sint16			CharacterCalmActual;
// 20	sint16			CharacterResonanceBase;
// 21	sint16			CharacterResonanceActual;

// 22	sint16			CharacterScienceMagicAlignment;
// 23	sint16			CharacterHumanismMetaversalismAlignment;
// 24	sint16			CharacterOrderChaosAlignment;

// 25	sint16			CharacterAltrismSelfInterestScale;
// 27	sint16			CharacterIntrovertExtrovertScale;
// 28	sint16			CharacterHedonismIndustryScale;
// 29	sint16			CharacterIntuitive;

// 0	uint32			CharacterId;
// 1	uint32			CharacterHomelandId;
// 2	uint32			CharacterBirthdate;	
// 3	uint32			CWesternAstrologicalSignId;
// 4	uint32			CChineeseAstrologicalSignId;
// 5	uint32			CharacterDevelopmentPhaseId;	
// 6	uint32			CharacterSocialStandingId;

// 0	std::string		CharacterFirstName;
// 1	std::string		CharacterLastName;
// 2	std::string		CharacterGender;	
// 3	std::string		CharacterHandedness;
// 4	std::string		CharacterBasePigment;
// 5	std::string		CharacterHairColor;
// 6	std::string		CharacterHairType;	
// 7	std::string		CharacterEyeColr;

// 0	NLMISC::CVector	Position;
// 1	NLMISC::CVector	Heading;




private:
	
	vector<sint16>				_I16Stats;
	vector<uint32>				_I32Stats;
	vector<string>				_StrStats;
	vector<NLMISC::CVector>		_VecStats;
	vector<float>		_FloatStats;

	void				save( std::string configFileName );
	std::string					_MyName;
	bool						_Updated;
};


#endif // NL_STATS_H

/* End of stats.h */


/** \file entities.h
 * Bang specific code for managing the entities
 *
 * $Id: entities.h,v 1.9 2002/04/17 03:37:10 robofly Exp $
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

#ifndef ENTITIES_H
#define ENTITIES_H

//
// Includes 
//

#include <string>
#include <map>
#include <queue>

#include <nel/misc/vector.h>
#include <nel/misc/time_nl.h>

#include <nel/3d/animation_time.h>

#include "physics.h"
#include "animation.h"
#include "client_world_objects.h"

//
// External definitions
//

namespace NLSOUND
{
	class USource;
}

namespace NLPACS
{
	class UMovePrimitive;
}

namespace NL3D
{
	class UInstance;
	class UInstanceGroup;
	class UVisualCollisionEntity;
	class UPlayList;
	class USkeleton;
}

//
// External classes
//

// An entity that will move through the landscape.
// The possible entities are the Self (the player's avatar), the Other and the Snowball
class CEntity
{
public:
	
	// Create a default entity
	CEntity () :
		Id(0xffffffff), Name("<Unknown>"), AutoMove(false), Instance(NULL), Skeleton(NULL),
		Particule(NULL), PlayList(NULL), /*CurrentAnim(NoAnim), */NextEmptySlot(0)/*, Source (NULL)*/,
		Angle(0.0f), AuxiliaryAngle(0.0f), InterpolatedAuxiliaryAngle(0.0f)
		{ }


	// The id of the entity
	uint32							Id;

	// The name of the entity
	std::string						Name;

	// The name of the entity
	std::string						ShapeName;

	// Contain the target position for this entity
	NLMISC::CVector					ServerPosition;

	// Contain the current position of the entity
	NLMISC::CVector					Position;

	// The immediate speed of the entity
	NLMISC::CVector					ImmediateSpeed;

	// The maximum speed of the entity
	float							Speed, 
	// The angle of the entity
									Angle,
	// Various angle controls for the interpolation
									AuxiliaryAngle, InterpolatedAuxiliaryAngle;

	// The trajectory (only for snowballs, defined in physics.h)
	CTrajectory						Trajectory;

	// Primary particle system name
	std::string						PsExplosionName;

	// The mesh instance associated to this entites left "hand"
	NL3D::UInstance					*LeftHandInstance;

	// The mesh instance associated to this entites right "hand"
	NL3D::UInstance					*RightHandInstance;

	// The id associated to this entites left "hand"
	uint32							LeftHandId;

	// The id associated to this entites right "hand"
	uint32							RightHandId;

	// The current running Spell Particle system FX
	NL3D::UInstance					*CurrentSpellFX;

	// The state enum of the entity
	enum TState	{ Appear, Normal, Disappear, AppearFast, DisappearFast, AppearFalling, Falling };

	// The state of this entity
	TState							State;
	// The date of the beginning of this state
	NLMISC::TTime					StateStartTime;

	// The state enum of the entity
	enum AState	{ None, Idle, Walk, LogIn, LogOff, RainSpell};

	// The animation state
	AState							AnimState;
	bool							HasAnimChanged;
	bool							AnimChangeTransmitted;

	// The date of the beginning of this animation state
	NLMISC::TTime					AnimStateStartTime;

	// The type enum of the entity
	enum TType	{ Self, Other, Dynamic, Snowball, Projectile };

	// The type of this entity
	TType							Type;

	// Is it an auto-moving entity
	bool							AutoMove;

	// The PACS move primitive
	NLPACS::UMovePrimitive			*MovePrimitive;
	// The collision entity (for ground snapping)
	NL3D::UVisualCollisionEntity	*VisualCollisionEntity;
	// The mesh instance associated to this entity
	NL3D::UInstance					*Instance;
	// The current running Spell Particle system FX
	NL3D::UInstanceGroup					*SelfInstanceGroup;
	// The skeleton binded to the instance
	NL3D::USkeleton					*Skeleton;
	// The particle system (for appear and disappear effects)
	NL3D::UInstance					*Particule;
	// A particle system (for selection)
	NL3D::UInstance					*SelectionParticle;
	// Headwear 
	NL3D::UInstance					*Headwear;
	// The sound source associated to the entity
	NLSOUND::USource				*Source;
	// The sound source associated with an environmentFX0
	NLSOUND::USource				*SourceEnvFX0;
	// The sound source associated with an environmentFX1
	NLSOUND::USource				*SourceEnvFX1;


	void	setState (TState state);
	void	setAnimState (AState state);

	// Playlist linked to this entity
//	EAnim							CurrentAnim;
	uint							NextEmptySlot;
	NL3D::UPlayList					*PlayList;
	std::queue<EAnim>				AnimQueue;
	NL3D::CAnimationTime			StartAnimationTime;

};

//
// Enums
//

// The collision bits used by pacs (dynamic collisions)
enum
{
	SelfCollisionBit = 1,
	OtherCollisionBit = 2,
	SnowballCollisionBit = 4,
	StaticCollisionBit = 8
};

//
// External variables
//

// The entity representing the player avatar
extern CEntity								*Self;

// The speed of the player
extern float								PlayerSpeed;
// The speed of the snowball
extern float								SnowballSpeed;

// The entities storage
extern std::map<uint32, CEntity>			Entities;

// The dynamicObject storage
extern std::map<uint32, CClientDynamicObject>			DynamicObjects;

// The firearmDynamicObject storage
extern std::map<uint32, CClientFirearm>			Firearms;

// The CClientBoxOfBullets storage
extern std::map<uint32, CClientBoxOfBullets>	BoxOfBullets;

// The CClientNutrition storage
extern std::map<uint32, CClientNutrition>		Nutrition;

typedef std::map<uint32, CEntity>::iterator	EIT;
typedef std::map<uint32, CClientDynamicObject>::iterator	DIT;
typedef std::map<uint32, CClientFirearm>::iterator	FIT;

typedef std::map<uint32, CClientBoxOfBullets>::iterator	BIT;
typedef std::map<uint32, CClientNutrition>::iterator	NIT;


extern uint32									CurrentPacsMaterial;
extern bool										DebugTranportBug;	
extern uint8									DTBugLevel;
//
// External functions
//

EIT findEntity (uint32 eid, bool needAssert = true);

void	addEntity (uint32 eid, std::string name, std::string mesh, CEntity::TType type, const NLMISC::CVector &startPosition, const NLMISC::CVector &serverPosition, float radius, const NLMISC::CVector dimensions, float speed, std::string psName, NL3D::UInstance *shapeInstance);
void	removeEntity (uint32 eid, bool fast);

void	setSelectedEntity(uint32 eid, std::string psName);
void	addSpotSoundToEntity (uint32 id, std::string soundName, float gain, float pitch);

void	addSpellFX(uint32 playerId, std::string psName, std::string soundName);
void	removeSpellFX(uint32 playerId);

// when we turn online, we need to clear all offline entities
void	removeAllEntitiesExceptUs ();

void	initEntities();
void	updateEntities ();

void deleteEntity (CEntity &entity);

// Reset the pacs position of an entity (in case pacs went wrong)
void	resetEntityPosition(uint32 eid);
// Process the event when an entity shoots a snowball
void	shotSnowball(uint32 sid, uint32 eid, const NLMISC::CVector &start, const NLMISC::CVector &target, float speed, float deflagRadius);

// Process the event when an entity shoots a projectile;
void	shotProjectile(uint32 projectileId, uint32 playerId, std::string shapeName, std::string psName, const NLMISC::CVector &start, const NLMISC::CVector &target, float speed);

void	setEntityHand(uint32 eid, bool leftHand, uint32 did, std::string meshName);

void	renderEntitiesNames ();


#endif // ENTITIES_H

/* End of entities.h */

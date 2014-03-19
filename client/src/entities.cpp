/** \file entities.cpp
 * Entity managment
 *
 * $Id: entities.cpp,v 1.21 2002/06/06 05:51:01 robofly Exp $
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
#include <map>

#include <nel/misc/types_nl.h>
#include <nel/misc/event_listener.h>
#include <nel/misc/command.h>
#include <nel/misc/log.h>
#include <nel/misc/time_nl.h>
#include <nel/misc/displayer.h>
#include <nel/misc/vector.h>
#include <nel/misc/vectord.h>
#include <nel/misc/time_nl.h>

#include "3d/scene_group.h"

#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_instance_group.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_material.h>
#include <nel/3d/u_landscape.h>
#include <nel/3d/u_skeleton.h>

#include <nel/3d/u_visual_collision_manager.h>
#include <nel/3d/u_visual_collision_entity.h>
#include <nel/3d/u_bone.h>

#include <nel/pacs/u_move_container.h>
#include <nel/pacs/u_move_primitive.h>
#include <nel/pacs/u_global_retriever.h>
#include <nel/pacs/u_global_position.h>

#include "client.h"
#include "entities.h"
#include "pacs.h"
#include "animation.h"
#include "camera.h"
#include "sound.h"
#include "mouse_listener.h"
#include "landscape.h"
#include "network.h"
#include "inventory_display.h"
#include "avatar_interface.h"

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

// A map of entities. All entities are later reffered by their unique id
map<uint32, CEntity>	Entities;

// The dynamicObject storage
map<uint32, CClientDynamicObject>		DynamicObjects;

// The firearmDynamicObject storage
map<uint32, CClientFirearm>				Firearms;

// The firearmDynamicObject storage
map<uint32, CClientBoxOfBullets>		BoxOfBullets;

// The firearmDynamicObject storage
map<uint32, CClientNutrition>			Nutrition;

CEntity					*Self = NULL;

// The size of the world, in meter
float					WorldWidth = 20*160;
float					WorldHeight = 6*160;

// Entity Id, only used offline
uint32					NextEID = 1000000;

// The speed settings
float					PlayerSpeed = 4.0f;	// 6.5 km/h
float					SnowballSpeed = 15.0f;	// 36 km/h
float				    ProjectileSpeed = 14.0f;

// falling hack
float				    FallingSpeed = 25.0f;
float					MinFallingSpeed = 2.0f;
float					FallingSpeedSlowdown = 1.0f;
float					SlowFallThreshold = 1800.0f;
bool					BreakAnimationDone = false;
float					FallingViewHeight = 8.0f;
float					MinFallingViewHeight;
// these variables are set with the config file
bool					PacsTest=false;
bool					DebugTranportBug=false;	
uint8					DTBugLevel=0;
uint32					CurrentPacsMaterial = 0;
// Setup for the name up the character
float					EntityNameSize;
CRGBA					EntityNameColor;

uint32					LastSelectedId=0;

CVector LastGoodPos=CVector(0,0,0);
// The move container used for dynamic collisions
UBone					*RightHandBone=NULL;

// The mesh instance associated to this entites left "hand"
//NL3D::UInstance			*LeftHandInstance=NULL;

// The mesh instance associated to this entites right "hand"
//NL3D::UInstance			*RightHandInstance=NULL;

// The current running Spell Particle system FX
//NL3D::UInstance			*CurrentSpellFX = NULL;

// Set the state of the entity (Appear, Normal, Disappear)
void CEntity::setState (TState state)
{
	State = state;
	StateStartTime = CTime::getLocalTime ();
}

// Set the animation state of the entity
void CEntity::setAnimState (AState state)
{
	if (AnimState==state) return;
	HasAnimChanged = true;
	AnimChangeTransmitted = false;
	AnimState = state;
}

// Get an map iterator on a entity, specified by its id
EIT findEntity (uint32 eid, bool needAssert)
{
	EIT entity = Entities.find (eid);
	if (entity == Entities.end () && needAssert)
	{
//		nlinfo ("Entity %u not found", eid);
	}
	return entity;
}


void updateAnimState(CEntity &entity)
{
	if (!entity.HasAnimChanged) return;

	entity.HasAnimChanged = false;

	switch (entity.AnimState)
	{
	case CEntity::Idle:
		if (entity.Name=="Shaman")
			playAnimation (entity, PrayAnim02, true);
		else
			playAnimation (entity, IdleAnim, true);
		break;
	case CEntity::Walk:
		if (entity.ShapeName=="IndianPrincess.shape")
			playAnimation (entity, WalkAnim02, true);
		else 
			playAnimation (entity, WalkAnim04, true);
		break;
	case CEntity::LogIn:
		playAnimation (entity, LogInAnim);
		break;
	case CEntity::LogOff:
		playAnimation (entity, LogOffAnim);
		break;
	case CEntity::RainSpell:
//		playAnimation (entity, PrepareRainSpellAnim, true);
		playAnimation (entity, RainSpellAnim, true);
		break;
	} 
}

// Creates an entity, given its id, its type (Self, Other, Snowball), its start and server positions.
void addEntity (uint32 eid, std::string name, std::string mesh, CEntity::TType type, const CVector &startPosition, const CVector &serverPosition, float radius,  const NLMISC::CVector dimensions, float speed, std::string psName, UInstance *shapeInstance)
{
//	nlinfo ("adding entity %u with name %s and mesh %s %d", eid, name, mesh, type);

	// Check that the entity doesn't exist yet
	EIT eit = findEntity (eid, false);
	if (eit != Entities.end ())
	{
		nlerror ("Entity %d already exist", eid);
		return;
	}

	// Create a new entity
	eit = (Entities.insert (make_pair (eid, CEntity()))).first;
	CEntity	&entity = (*eit).second;

	// Check that in the case the entity newly created is a Self, ther isn't a Self yet.
	if (type == CEntity::Self)
	{
		if (Self != NULL)
		{	
			if (ShowDebugInChat) nlinfo("Self entity already created");
			return;
		}
		else
		  Self = &entity;
	}

	// Set the entity up
	entity.Id = eid;
	entity.Type = type;
	entity.Name = name;
	entity.ShapeName = mesh;
	entity.Position = startPosition;
	entity.Angle = 0.0f;

	entity.ServerPosition = serverPosition;
	entity.VisualCollisionEntity = VisualCollisionManager->createEntity();
	entity.PsExplosionName = psName;
	entity.SelectionParticle = NULL;
	entity.LeftHandInstance = NULL;
	entity.RightHandInstance = NULL;
	entity.CurrentSpellFX = NULL;
	entity.Source = NULL;
	entity.SourceEnvFX0 = NULL;
	entity.SourceEnvFX1 = NULL;
	entity.Headwear = NULL;
	entity.SelfInstanceGroup = NULL;
//	nlinfo ("--------------------");
//	nlinfo ("ProjectileId = %d", eid );
//	nlinfo ("shapeName = %s", name );
//	nlinfo ("psName = %s", psName );

//	nlinfo ("startPosition x = %f", startPosition.x );
//	nlinfo ("startPosition y = %f", startPosition.y );
//	nlinfo ("startPosition z = %f", startPosition.z );
//	nlinfo ("speed = %f", speed );

	// setup the move primitive and the mesh instance depending on the type of entity
	switch (type)
	{
	case CEntity::Self:
		// create a move primitive associated to the entity
		entity.MovePrimitive = MoveContainer->addCollisionablePrimitive(0, 1);
		// it's a cylinder
		entity.MovePrimitive->setPrimitiveType(UMovePrimitive::_2DOrientedCylinder);
		// the entity should slide against obstacles
		entity.MovePrimitive->setReactionType(UMovePrimitive::Slide);
		// do not generate event if there is a collision
		entity.MovePrimitive->setTriggerType(UMovePrimitive::NotATrigger);
		// which entity should collide against me
		entity.MovePrimitive->setCollisionMask(OtherCollisionBit+SnowballCollisionBit+StaticCollisionBit);
		// the self collision bit
		entity.MovePrimitive->setOcclusionMask(SelfCollisionBit);
		// the self is an obstacle
		entity.MovePrimitive->setObstacle(true);
		// the size of the cylinder
		entity.MovePrimitive->setRadius(1.0f);
		entity.MovePrimitive->setHeight(1.8f);
		// only use one world image, so use insert in world image 0
		entity.MovePrimitive->insertInWorldImage(0);
		// retreive the start position of the entity
		entity.MovePrimitive->setGlobalPosition(CVectorD(startPosition.x, startPosition.y, startPosition.z+1.1f), 0);

//		entity.Instance = Scene->createInstance (mesh);
/*
		if (entity.Instance!=NULL)
		{
			entity.Instance->hide();
			entity.Instance=NULL;
		}

		nlinfo("have set instance");

		if (entity.Skeleton!=NULL)
		{
			entity.Skeleton->hide();
			entity.Skeleton=NULL;
		}

		nlinfo("have set skeleton");
		// create instance of the mesh character
		if (mesh=="shaman.shape") 
		{
			entity.Instance = MaleInstance;
			entity.Skeleton = MaleSkeleton;
		} 
		else if (mesh=="IndianPrincess.shape") 
		{
			entity.Instance = FemaleInstance;
			entity.Skeleton = NULL;
//			entity.Instance->setClusterSystem(TempleInstanceGroup);
		}
		else 
		{
			entity.Skeleton = NULL;
			entity.Instance->setClusterSystem(TempleInstanceGroup);
		}
*/
		entity.Instance = Scene->createInstance (mesh);
//		entity.Instance->setPos (CVector::Null);
//		entity.Instance->setRotQuat (CQuat::Identity);
//		entity.Instance->setScale (1, 1, 1);
		if (mesh=="shaman.shape") 
		{
			entity.Skeleton = Scene->createSkeleton ("shaman.skel");
			// use the instance on the skeleton	
			entity.Skeleton->bindSkin (entity.Instance);
//			entity.Skeleton->setPos (CVector::Null);
//			entity.Skeleton->setRotQuat (CQuat::Identity);
//			entity.Skeleton->setScale (1, 1, 1);
			entity.Skeleton->setClusterSystem(TempleInstanceGroup);
		} 
		else if (mesh=="IndianPrincess.shape") 
		{
			entity.Skeleton = Scene->createSkeleton ("IndianPrincess.skel");
			// use the instance on the skeleton	
//			entity.Skeleton->setScale(1.0f, 1.0f, 1.0f);
			entity.Skeleton->bindSkin (entity.Instance);
			entity.Skeleton->setClusterSystem(TempleInstanceGroup);
		} 
		else
		{
			entity.Skeleton = NULL;
			entity.Instance->setClusterSystem(TempleInstanceGroup);
		}



//		entity.Source = AudioMixer->createSource( "quetzal-loop" );
//		entity.Source->setLooping (true);
//		entity.Source->play ();

		entity.Instance->hide ();

/*		entity.Headwear = Scene->createInstance("hat.shape");
		entity.Headwear->setClusterSystem(TempleInstanceGroup);
		entity.Headwear->setPos (CVector::Null);
		entity.Headwear->setRotQuat (CQuat::Identity);
//		entity.Headwear->setScale (1, 1, 1);
		entity.Skeleton->stickObject(entity.Headwear, entity.Skeleton->getBoneIdByName("bip_ Head"));
*/
		entity.Angle = MouseListener->getOrientation();

		// setup final parameters
		entity.Speed = PlayerSpeed;
		entity.Particule = Scene->createInstance ("appear.ps");
		if (Falling)
		{
			entity.setState (CEntity::AppearFalling);
			MouseListener->setViewHeight(FallingViewHeight);
		}
		else entity.setState (CEntity::Appear);
		if (Falling) playAnimation (entity, LogInAnim);

		if (ShowDebugInChat) nlinfo("mesh name %s", mesh);
		playAnimation (entity, IdleAnim);

		break;
	case CEntity::Other:
		entity.MovePrimitive = MoveContainer->addCollisionablePrimitive(0, 1);
		entity.MovePrimitive->setPrimitiveType(UMovePrimitive::_2DOrientedCylinder);
		entity.MovePrimitive->setReactionType(UMovePrimitive::DoNothing);
		entity.MovePrimitive->setTriggerType(UMovePrimitive::NotATrigger);
		entity.MovePrimitive->setCollisionMask(OtherCollisionBit+SelfCollisionBit+SnowballCollisionBit);
		entity.MovePrimitive->setOcclusionMask(OtherCollisionBit);
		entity.MovePrimitive->setObstacle(true);
		entity.MovePrimitive->setRadius(radius);
		entity.MovePrimitive->setHeight(dimensions.x);
		entity.MovePrimitive->insertInWorldImage(0);
		entity.MovePrimitive->setGlobalPosition(CVectorD(startPosition.x, startPosition.y, startPosition.z+1.1f), 0);

		// create instance of the mesh character
		entity.Instance = Scene->createInstance (mesh);
		if (mesh=="shaman.shape") 
		{
			entity.Skeleton = Scene->createSkeleton ("shaman.skel");
			// use the instance on the skeleton	
			entity.Skeleton->bindSkin (entity.Instance);
			entity.Skeleton->setClusterSystem(TempleInstanceGroup);
		} 
		else if (mesh=="IndianPrincess.shape") 
		{
			entity.Skeleton = Scene->createSkeleton ("IndianPrincess.skel");
			// use the instance on the skeleton	
//			entity.Skeleton->setScale(1.0f, 1.0f, 1.0f);
			entity.Skeleton->bindSkin (entity.Instance);
			entity.Skeleton->setClusterSystem(TempleInstanceGroup);
		} 
		else
		{
			entity.Skeleton = NULL;
			entity.Instance->setClusterSystem(TempleInstanceGroup);
		}

		entity.Instance->hide ();
		if (ShowDebugInChat) nlinfo("Other name = %s", name);
		if (name=="Shaman")
		{
			entity.Headwear = Scene->createInstance("hat.shape");
			entity.Headwear->setClusterSystem(TempleInstanceGroup);
			entity.Headwear->setPos (CVector::Null);
			entity.Headwear->setRotQuat (CQuat::Identity);
			entity.Skeleton->stickObject(entity.Headwear, entity.Skeleton->getBoneIdByName("bip_ Head"));
		}

//		entity.Source = AudioMixer->createSource( "quetzal-loop" );
//		entity.Source->setLooping (true);
//		entity.Source->play ();

		entity.Speed = PlayerSpeed;
		entity.Particule = Scene->createInstance ("appear.ps");
		entity.setState (CEntity::Appear);
		playAnimation (entity, LogInAnim);
		playAnimation (entity, IdleAnim);

		break;
	case CEntity::Dynamic:
		entity.MovePrimitive = MoveContainer->addCollisionablePrimitive(0, 1);
		if (dimensions.y==0.0f)
		{
			entity.MovePrimitive->setPrimitiveType(UMovePrimitive::_2DOrientedCylinder);
			entity.MovePrimitive->setRadius(radius);
			entity.MovePrimitive->setHeight(dimensions.x);
		}
		else
		{
			// Setup the collision primitive
			entity.MovePrimitive->setPrimitiveType (UMovePrimitive::_2DOrientedBox);
			entity.MovePrimitive->setHeight (dimensions.x);
//			entity.MovePrimitive->setOrientation (orientation, insertWorldImage);
			entity.MovePrimitive->setSize (dimensions.y, dimensions.z);
		}

		// Setup reaction type
		entity.MovePrimitive->setReactionType(UMovePrimitive::Reflexion);
		entity.MovePrimitive->setAbsorbtion (0.9f);

		entity.MovePrimitive->setTriggerType(UMovePrimitive::NotATrigger);
		entity.MovePrimitive->setCollisionMask(OtherCollisionBit+SelfCollisionBit+SnowballCollisionBit);
		entity.MovePrimitive->setOcclusionMask(OtherCollisionBit);
		entity.MovePrimitive->insertInWorldImage(0);
		entity.MovePrimitive->setGlobalPosition(CVectorD(startPosition.x, startPosition.y, startPosition.z), 0);

//		entity.Source = AudioMixer->createSource( "wind04" );
//		entity.Source->setLooping (true);
//		entity.Source->play ();

		entity.Instance = Scene->createInstance (mesh);
//		entity.Skeleton = Scene->createSkeleton ("gnu.skel");
//		entity.Skeleton->bindSkin (entity.Instance);
		entity.Instance->hide ();
		entity.Instance->setClusterSystem(TempleInstanceGroup);

		entity.Speed = PlayerSpeed;
		entity.Particule = Scene->createInstance ("appear.ps");
		entity.Particule->setScale( 2.0f, 2.0f, 2.0f);
		entity.setState (CEntity::Appear);
//		playAnimation (entity, LogInAnim);
//		playAnimation (entity, IdleAnim);

		break;

	case CEntity::Snowball:
		entity.MovePrimitive = NULL;

		// allows collision snapping to the ceiling
		entity.VisualCollisionEntity->setCeilMode(true);

		entity.Instance = Scene->createInstance ("snowball.shape");
		entity.Skeleton = NULL;
		entity.Speed = SnowballSpeed;

//		playSound (entity, SoundId);

		entity.setState (CEntity::Normal);
		break;

	case CEntity::Projectile:
		entity.MovePrimitive = NULL;

		// allows collision snapping to the ceiling
		entity.VisualCollisionEntity->setCeilMode(true);

		if (mesh=="") mesh="snowball.shape";
		entity.Instance = Scene->createInstance (mesh);
		entity.Skeleton = NULL;
		entity.Speed = speed;

		if (ShowDebugInChat) nlinfo("GET SELECTED=%d", CharacterInventory.getSelected());

		FIT dynamicFirearmObject = Firearms.find (CharacterInventory.getSelected());
		CClientFirearm &dynamicFirearmObjecta = dynamicFirearmObject->second; // hack
		if (ShowDebugInChat) nlinfo( "Firearm name=%s",dynamicFirearmObjecta.Name );
		if (ShowDebugInChat) nlinfo("Firearm Sound =%s", dynamicFirearmObjecta.FireSound);
		if (dynamicFirearmObjecta.FireSound!="")
		{
/*			entity.Source = AudioMixer->createSource( dynamicFirearmObjecta.FireSound.c_str() );
			entity.Source->setLooping (false);
			entity.Source->play ();
*/		}
		if (ShowDebugInChat) nlinfo("PS =%s", dynamicFirearmObjecta.ParticleSystem);
		if (dynamicFirearmObjecta.ParticleSystem!="")
		{
			entity.Particule = Scene->createInstance (dynamicFirearmObjecta.ParticleSystem);
		}

		entity.setState (CEntity::AppearFast);
		if (ShowDebugInChat) nlinfo("out of case");
		break;
	}

	if (entity.Skeleton != NULL)
		entity.Skeleton->setPos (startPosition);

	entity.Instance->setPos (startPosition);

	if (entity.Source != NULL)
		entity.Source->setPos (startPosition);

	if (entity.SourceEnvFX0 != NULL)
		entity.SourceEnvFX0->setPos (startPosition);

	if (entity.SourceEnvFX1 != NULL)
		entity.SourceEnvFX1->setPos (startPosition);

	if (entity.Particule != NULL)
		entity.Particule->setPos (startPosition);

}

// effectively remove the entity (don't play animation)
void deleteEntity (CEntity &entity)
{
	if (entity.Particule != NULL)
	{
		Scene->deleteInstance (entity.Particule);
		entity.Particule = NULL;
	}

	if (entity.Type != CEntity::Dynamic)
	{	
		deleteAnimation (entity);

		if (entity.Skeleton != NULL)
		{
			entity.Skeleton->detachSkeletonSon (entity.Instance);
			Scene->deleteSkeleton (entity.Skeleton);
			entity.Skeleton = NULL;
		}
	}

	// if there is a selection particule system linked, delete it
	if (entity.LeftHandInstance != NULL)
	{
		Scene->deleteInstance (entity.LeftHandInstance);
		entity.LeftHandInstance = NULL;
	}

	// if there is a selection particule system linked, delete it
	if (entity.RightHandInstance != NULL)
	{
		Scene->deleteInstance (entity.RightHandInstance);
		entity.RightHandInstance = NULL;
	}

	if (entity.Headwear !=NULL)
	{
		Scene->deleteInstance( entity.Headwear );
		entity.Headwear = NULL;
	}

	if (entity.Instance != NULL)
	{
		Scene->deleteInstance (entity.Instance);
		entity.Instance = NULL;
	}

	if (entity.VisualCollisionEntity != NULL)
	{
		VisualCollisionManager->deleteEntity (entity.VisualCollisionEntity);
		entity.VisualCollisionEntity = NULL;
	}

	if (entity.MovePrimitive != NULL)
	{
		MoveContainer->removePrimitive(entity.MovePrimitive);
		entity.MovePrimitive = NULL;
	}

//	deleteSound (entity);

	if (ShowDebugInChat) nlinfo ("Remove the entity %u from the Entities list", entity.Id);
	EIT eit = findEntity (entity.Id);
	Entities.erase (eit);
}


void setSelectedEntity(uint32 eid, std::string psName)
{
	// look for the entity
	EIT eit = findEntity (eid);
	CEntity	&entity = (*eit).second;

	if (entity.SelectionParticle == NULL)
	{
		// look for the entity
		EIT eit2 = findEntity (LastSelectedId);
		CEntity	&entity2 = (*eit2).second;

		// if there is a particule system linked, delete it
		if (entity2.SelectionParticle != NULL)
		{
			Scene->deleteInstance (entity2.SelectionParticle);
			entity2.SelectionParticle = NULL;
		}
		if (ShowDebugInChat) nlinfo("Created PS for ID %d", eid);
		entity.SelectionParticle = Scene->createInstance (psName);
		entity.SelectionParticle->setPos (entity.Position);
		entity.SelectionParticle->setScale (7.0f, 7.0f, 7.0f);
		entity.SelectionParticle->setClusterSystem(TempleInstanceGroup);

		LastSelectedId=eid;
	}
}

void setEntityHand(uint32 eid, bool leftHand, uint32 did, std::string meshName)
{
	// look for the entity
	EIT eit = findEntity (eid);
	CEntity	&entity = (*eit).second;

	if (leftHand)
	{
		CharacterInventory.hold( true, did );
		// if there is another instance there, delete it
		if (entity.LeftHandInstance != NULL)
		{
			Scene->deleteInstance (entity.LeftHandInstance);
			entity.LeftHandInstance = NULL;
		}

		if (meshName!="") 
		{
			entity.LeftHandInstance = Scene->createInstance (meshName);
//			entity.LeftHandInstance->setPos (entity.Position+CVector(-1.0f, -0.2f, 1.0f));
//							uint16 theBoneId = entity.Skeleton->getBoneIdByName("Bip01 R Hand");
//				nlinfo("BoneId= %d",theBoneId);
			entity.Skeleton->stickObject(entity.LeftHandInstance, entity.Skeleton->getBoneIdByName("bip_ L Finger1"));
		}
	} 
	else
	{
		CharacterInventory.hold( false, did );
		// if there is another instance there, delete it
		if (entity.RightHandInstance != NULL)
		{
			Scene->deleteInstance (entity.RightHandInstance);
			entity.RightHandInstance = NULL;
		}

		if (meshName!="") 
		{
			entity.RightHandInstance = Scene->createInstance (meshName);
			entity.Skeleton->stickObject(entity.RightHandInstance, entity.Skeleton->getBoneIdByName("bip_ R Finger1"));
			entity.RightHandInstance->setPos (CVector::Null);
			entity.RightHandInstance->setRotQuat (CQuat::Identity);
//			entity.RightHandInstance->setScale (1, 1, 1);
			entity.RightHandInstance->setClusterSystem(TempleInstanceGroup);
//			entity.RightHandInstance->setPos (entity.Position+CVector(1.0f, -0.2f, 1.0f));
		}
	}
}


void	addSpotSoundToEntity (uint32 id, std::string soundName, float gain, float pitch)
{
	// look for the entity
	EIT eit = findEntity (id);
	CEntity	&entity = (*eit).second;

	if (entity.Source != NULL) deleteSound(entity);

	entity.Source = AudioMixer->createSource (soundName.c_str(), true);
	if (ShowDebugInChat) nlinfo("soundName = %s", soundName);

	if (entity.Source != NULL)
	{
		entity.Source->setLooping (false);
		entity.Source->setGain (gain);
		entity.Source->setGain (pitch);
		entity.Source->play ();
	}
}

void	addSpellFX(uint32 playerId, std::string psName, std::string soundName)
{
	// look for the entity
	EIT eit = findEntity (playerId);
	CEntity	&entity = (*eit).second;

	if (entity.CurrentSpellFX!=NULL)
	{
		Scene->deleteInstance(entity.CurrentSpellFX);
		entity.CurrentSpellFX=NULL;
	}

	entity.CurrentSpellFX = Scene->createInstance(psName);
	entity.CurrentSpellFX->setPos (entity.Position);

	CVector jdir = CVector(-(float)cos(entity.Angle+1.42f), -(float)sin(entity.Angle+1.42f), 0.0f);
	entity.CurrentSpellFX->setRotQuat (jdir);
}

void	removeSpellFX(uint32 playerId)
{
	// look for the entity
	EIT eit = findEntity (playerId);
	CEntity	&entity = (*eit).second;
	Scene->deleteInstance(entity.CurrentSpellFX);
	entity.CurrentSpellFX=NULL;

	entity.setAnimState(CEntity::Idle);
}



void removeEntity (uint32 eid, bool fast)
{

	// look for the entity
	EIT eit = findEntity (eid);
	CEntity	&entity = (*eit).second;

	// if there is a particule system linked, delete it
	if (entity.Particule != NULL)
	{
		Scene->deleteInstance (entity.Particule);
		entity.Particule = NULL;
	}

	// if there is a selection particule system linked, delete it
	if (entity.SelectionParticle != NULL)
	{
		Scene->deleteInstance (entity.SelectionParticle);
		entity.SelectionParticle = NULL;
	}

	// if there is a selection particule system linked, delete it
	if (entity.CurrentSpellFX != NULL)
	{
		Scene->deleteInstance (entity.CurrentSpellFX);
		entity.CurrentSpellFX = NULL;
	}

//	if (entity.Type == CEntity::Projectile) entity.Particule = Scene->createInstance(entity.PsExplosionName);
	entity.Particule = Scene->createInstance("appear.ps");
	entity.Particule->setPos (entity.Position);

	if (entity.Type != CEntity::Dynamic && entity.Type != CEntity::Projectile) playAnimation (entity, LogOffAnim, true);

	if (fast) entity.setState (CEntity::DisappearFast);
	else entity.setState (CEntity::Disappear);
}

void	removeAllEntitiesExceptUs ()
{
	EIT		eit, nexteit;

	// move entities
	for (eit = Entities.begin (); eit != Entities.end (); )
	{
		nexteit = eit; nexteit++;

		CEntity	&entity = (*eit).second;

		if (entity.Type != CEntity::Self)
		{
			// effectively remove the entity (don't play animation)
			deleteEntity (entity);
		}

		eit = nexteit;
	}
}


// What to do when the entity appears
void stateFalling (CEntity &entity)
{
	if (entity.Position.z<SlowFallThreshold && FallingSpeed > MinFallingSpeed)
	{
		if (!BreakAnimationDone)
		{
//			nlinfo("Breaking....");
			playAnimation (entity, LogInAnim, true);
			BreakAnimationDone=true;
		}
		FallingSpeed=FallingSpeed-FallingSpeedSlowdown;
	}

	if (entity.Position.z<SlowFallThreshold && FallingViewHeight>MinFallingViewHeight)
	{
//		FallingViewHeight=FallingViewHeight-0.017f;
//		nlinfo("Falling = %f", FallingViewHeight);
//		MouseListener->setViewHeight(FallingViewHeight);
	}

	entity.Position.z=entity.Position.z-FallingSpeed;

	if (entity.Position.z<1)
	{
		unloadPlanetInstance();
		playAnimation (entity, IdleAnim);
		nlinfo("");
		nlinfo("");
		nlinfo("");
		nlinfo("");
		nlinfo("Welcome to Chichen Itza");
		nlinfo("");
		ShowCommands = true; 
		entity.setState(CEntity::Normal);
	}
}

// What to do when the entity appears
void stateAppearFalling (CEntity &entity)
{
	entity.Position.z=20000.0f;
	// after 1 second, show the instance
	if (CTime::getLocalTime () > entity.StateStartTime + 1000)
	{
		if (entity.Instance->getVisibility () != UTransform::Show)
		{
			entity.Instance->show ();
		}
	}

	// after 5 seconds, delete the particle system (if any)
	// and passe the entity into the Normal state
	if (CTime::getLocalTime () > entity.StateStartTime + 3000 && LoginComplete)
	{
		if (entity.Particule != NULL)
		{
			Scene->deleteInstance (entity.Particule);
			entity.Particule = NULL;
		}

		entity.setState (CEntity::Falling);
	}

	if (entity.MovePrimitive != NULL)
		entity.MovePrimitive->move(CVector(0,0,20000), 0);
	
}

// What to do when the entity appears
void stateAppear (CEntity &entity)
{
	// after 1 second, show the instance
	if (CTime::getLocalTime () > entity.StateStartTime + 1000)
	{
		if (entity.Instance->getVisibility () != UTransform::Show)
		{
			entity.Instance->show ();
		}
	}

	// after 5 seconds, delete the particle system (if any)
	// and passe the entity into the Normal state
	if (CTime::getLocalTime () > entity.StateStartTime + 3000)
	{
		if (entity.Particule != NULL)
		{
			Scene->deleteInstance (entity.Particule);
			entity.Particule = NULL;
		}

		entity.setState (CEntity::Normal);
	}

	if (entity.MovePrimitive != NULL)
		entity.MovePrimitive->move(CVector(0,0,0), 0);
}

// What to do when the entity appears
void stateAppearFast (CEntity &entity)
{
	entity.Instance->show ();
	entity.setState (CEntity::Normal);

	// after 5 seconds, delete the particle system (if any)
	// and passe the entity into the Normal state
	if (CTime::getLocalTime () > entity.StateStartTime + 1000)
	{
		if (entity.Particule != NULL)
		{
			Scene->deleteInstance (entity.Particule);
			entity.Particule = NULL;
		}

	}

	if (entity.MovePrimitive != NULL)
		entity.MovePrimitive->move(CVector(0,0,0), 0);
}


// What to do when the entity disappears
void stateDisappear (CEntity &entity)
{
	// after 1 second, remove the mesh and all collision stuff
	if (CTime::getLocalTime () > entity.StateStartTime + 1000)
	{
		if (entity.Instance->getVisibility () != UTransform::Hide)
		{
//			nlinfo("hiding entity");
			if (entity.Type == CEntity::Self)
			{
				if (Self == NULL)
					nlerror("Self entity doesn't exist");
				Self = NULL;
			}

			entity.Instance->hide ();
		}
	}

	// after 5 seconds, remove the particule system and the entity entry
	if (CTime::getLocalTime () > entity.StateStartTime + 1500)
	{
		deleteEntity (entity);
	}
	else
	{
		if (entity.MovePrimitive != NULL)
			entity.MovePrimitive->move(CVector(0,0,0), 0);
	}
}

// What to do when the entity disappears
void stateDisappearFast (CEntity &entity)
{
	// after 1 second, remove the mesh and all collision stuff
	if (CTime::getLocalTime () > entity.StateStartTime)
	{
		if (entity.Instance->getVisibility () != UTransform::Hide)
		{
//			nlinfo("hiding entity");
			if (entity.Type == CEntity::Self)
			{
				if (Self == NULL)
					nlerror("Self entity doesn't exist");
				Self = NULL;
			}

			entity.Instance->hide ();
		}
	}

	// after 5 seconds, remove the particule system and the entity entry
	if (CTime::getLocalTime () > entity.StateStartTime + 1500)
	{
		deleteEntity (entity);
	}
	else
	{
		if (entity.MovePrimitive != NULL)
			entity.MovePrimitive->move(CVector(0,0,0), 0);
	}
}

void stateNormal (CEntity &entity)
{
	double	dt = (double)(NewTime-LastTime) / 1000.0;
	CVector	oldPos;
	CVector	newPos;

	oldPos = entity.Position;
	CVector	pDelta = entity.Position - entity.ServerPosition;
	CVector	pDeltaOri = pDelta;
	pDelta.z = 0.0f;


	if ((entity.Type == CEntity::Snowball || entity.Type == CEntity::Projectile) && NewTime >= entity.Trajectory.getStopTime())
	{
/*
		CVector	tp(1140,-833,30);
/		nlinfo("dist=%f", (entity.Position-tp).norm());
		if ((entity.Position-tp).norm()<30.0f)
		{
			static UInstance *t = NULL;
			if (t != NULL)
			{
				Scene->deleteInstance (t);
			}
			t = Scene->createInstance("pills.ps");
			t->setScale (10,10,10);
			CVector tp2 = tp;
			tp2.z+=20;
			t->setPos (tp2);
//			nlinfo("touche");
		}
*/
		removeEntity(entity.Id, true);
	}



	// control the character animation
	if (entity.Type != CEntity::Snowball || entity.Type != CEntity::Projectile)
	{
		updateAnimState(entity);
	}


	entity.ImmediateSpeed = CVector::Null;

	if (entity.Type == CEntity::Self)
	{
		// get new position
		newPos = MouseListener->getPosition();
		// get new orientation
		entity.Angle = MouseListener->getOrientation();

		if (DebugTranportBug)
		{
			if (DTBugLevel==7)
			{
				nlinfo("newPos=%f %f %f",newPos.x,newPos.y,newPos.z);
			}
		}

		// Interpolate the character orientation towards the server angle
		// for smoother movements
		float	sweepDistance = entity.AuxiliaryAngle-entity.InterpolatedAuxiliaryAngle;
		if (sweepDistance > (float)Pi)
		{
			sweepDistance -= (float)Pi*2.0f;
			entity.InterpolatedAuxiliaryAngle += (float)Pi*2.0f;
		}
		if (sweepDistance < -(float)Pi)
		{
			sweepDistance += (float)Pi*2.0f;
			entity.InterpolatedAuxiliaryAngle -= (float)Pi*2.0f;
		}
		float	sweepAngle = 4.0f*sweepDistance;
		entity.InterpolatedAuxiliaryAngle += (float)(sweepAngle*dt);
		if ((entity.AuxiliaryAngle-entity.InterpolatedAuxiliaryAngle)*sweepAngle <= 0.0)
			entity.InterpolatedAuxiliaryAngle = entity.AuxiliaryAngle;
		entity.Angle += entity.InterpolatedAuxiliaryAngle;

		// tell the move container how much the entity should move
		if (entity.AnimState == CEntity::Walk)
		{
			entity.ImmediateSpeed = (newPos-oldPos)/(float)dt;
			entity.MovePrimitive->move(entity.ImmediateSpeed, 0);
		}
	}
	else if (entity.Type == CEntity::Other)
	{
		// go to the server position with linear interpolation

		// Interpolate orientation for smoother motions
		// AuxiliaryAngle -> the server imposed angle
		// InterpolatedAuxiliaryAngle -> the angle showed by the entity
		float	sweepDistance = entity.AuxiliaryAngle-entity.InterpolatedAuxiliaryAngle;
		if (sweepDistance > (float)Pi)
		{
			sweepDistance -= (float)Pi*2.0f;
			entity.InterpolatedAuxiliaryAngle += (float)Pi*2.0f;
		}
		if (sweepDistance < -(float)Pi)
		{
			sweepDistance += (float)Pi*2.0f;
			entity.InterpolatedAuxiliaryAngle -= (float)Pi*2.0f;
		}
		float	sweepAngle = 4.0f*sweepDistance;
		entity.InterpolatedAuxiliaryAngle += (float)(sweepAngle*dt);
		if ((entity.AuxiliaryAngle-entity.InterpolatedAuxiliaryAngle)*sweepAngle <= 0.0)
			entity.InterpolatedAuxiliaryAngle = entity.AuxiliaryAngle;

		entity.Angle = entity.InterpolatedAuxiliaryAngle;

//		if (entity.IsWalking)
		if (pDelta.norm() > 0.1f)
		{
			pDelta.normalize();
			entity.ImmediateSpeed = pDelta*(-entity.Speed);
			entity.MovePrimitive->move(entity.ImmediateSpeed, 0);
			entity.setAnimState( CEntity::Walk );
		}
		else
		{
			entity.setAnimState( CEntity::Idle );
		}
	}
	else if (entity.Type == CEntity::Snowball || entity.Type == CEntity::Projectile)
	{
		// go to the server position using trajectory interpolation
		CVector newPos = entity.Trajectory.eval(NewTime);
		if (newPos != entity.Position)
		{
			entity.Position = entity.Trajectory.eval(NewTime);
			entity.Instance->show ();
		}
	}
	else
	{
		// automatic speed
		newPos = oldPos;
	}
}

void updateEntities ()
{
	// compute the delta t that has elapsed since the last update (in seconds)
	double	dt = (double)(NewTime-LastTime) / 1000.0;
	EIT		eit, nexteit;

	// move entities
	for (eit = Entities.begin (); eit != Entities.end (); )
	{
		nexteit = eit; nexteit++;

		CEntity	&entity = (*eit).second;

		switch (entity.State)
		{
		case CEntity::Appear:
			stateAppear (entity);
			break;
		case CEntity::AppearFast:
			stateAppearFast (entity);
			break;
		case CEntity::AppearFalling:
			stateAppearFalling (entity);
			break;
		case CEntity::Normal:
			stateNormal (entity);
			break;
		case CEntity::Falling:
			stateFalling (entity);
			break;
		case CEntity::Disappear:
			stateDisappear (entity);
			break;
		case CEntity::DisappearFast:
			stateDisappearFast (entity);
			break;
		default:
//			nlstop;
			break;
		}

		eit = nexteit;
	}

	// evaluate collisions
	MoveContainer->evalCollision(dt, 0);

	// snap entities to the ground
	for (eit = Entities.begin (); eit != Entities.end (); eit++)
	{
		CEntity	&entity = (*eit).second;
		UGlobalPosition	gPos;

		if (entity.MovePrimitive != NULL && entity.State!=CEntity::Falling && entity.State!=CEntity::AppearFalling)

		{
			// get the global position in pacs coordinates system
			entity.MovePrimitive->getGlobalPosition(gPos, 0);
			// convert it in a vector 3d
			entity.Position = GlobalRetriever->getGlobalPosition(gPos);
			if (DebugTranportBug)
			{
				if (DTBugLevel==7)
				{
					nlinfo("AfterPACS=%f %f %f",entity.Position.x,entity.Position.y,entity.Position.z);
				}
				if (DTBugLevel==6)
				{
					if (entity.Position.x>5000)
						entity.Position=LastGoodPos;
					else
						LastGoodPos=entity.Position;
				}
			}
		
			// get the good z position
			gPos.LocalPosition.Estimation.z = 0.0f;
			entity.Position.z = GlobalRetriever->getMeanHeight(gPos);

	
			// check position retrieving
/*
			UGlobalPosition gPosCheck;
			gPosCheck = GlobalRetriever->retrievePosition(entity.Position);
			if (gPos.InstanceId != gPosCheck.InstanceId ||
				gPos.LocalPosition.Surface != gPosCheck.LocalPosition.Surface)
			{
				nlwarning("Checked UGlobalPosition differs from store");
//				gPos.InstanceId = gPosCheck.InstanceId;
//				gPos.LocalPosition.Surface = gPosCheck.LocalPosition.Surface;
			}
*/
			// snap to the ground
//			entity.VisualCollisionEntity->snapToGround(entity.Position);
			if (!GlobalRetriever->isInterior( gPos ))
				entity.VisualCollisionEntity->snapToGround( entity.Position );

			if (PacsTest && entity.Type == CEntity::Other)
			{
				nlinfo("Local = x %f y %f sx %f %f sy", entity.Position.x, entity.Position.y, entity.ImmediateSpeed.x, entity.ImmediateSpeed.x);
				nlinfo("Server = x %f y %f", entity.ServerPosition.x, entity.ServerPosition.y);
				nlinfo("Delta Hack %f", (entity.ServerPosition-entity.Position)*entity.ImmediateSpeed);
			}

			if (entity.Type == CEntity::Other)
			{
				if ( (entity.ServerPosition-entity.Position)*entity.ImmediateSpeed < 0.0f )
				{
//				nlinfo("detected over entity %d", entity.Id);
					entity.ServerPosition.z = entity.Position.z;
					entity.Position = entity.ServerPosition;
					if (!GlobalRetriever->isInterior( gPos ))
						entity.VisualCollisionEntity->snapToGround( entity.Position );
					entity.MovePrimitive->setGlobalPosition(CVectorD(entity.Position.x, entity.Position.y, entity.Position.z), 0);
				}
				else if ( (entity.ServerPosition-entity.Position)*entity.ImmediateSpeed > 10.0f )
				{
					entity.ServerPosition.z = entity.Position.z;
					entity.Position = entity.ServerPosition;
					if (!GlobalRetriever->isInterior( gPos ))
						entity.VisualCollisionEntity->snapToGround( entity.Position );
					entity.MovePrimitive->setGlobalPosition(CVectorD(entity.Position.x, entity.Position.y, entity.Position.z), 0);
				}
			}

		}

		if (entity.Instance != NULL)
		{
			CVector	jdir;
			switch (entity.Type)
			{
			case CEntity::Self:
				if (entity.ShapeName == "IndianPrincess.shape")
					jdir = CVector(-(float)cos(entity.Angle-1.57f), -(float)sin(entity.Angle-1.57f), 0.0f);
				else
					jdir = CVector(-(float)cos(entity.Angle), -(float)sin(entity.Angle), 0.0f);
				break;
			case CEntity::Other:
				if (entity.ShapeName == "IndianPrincess.shape")
					jdir = CVector(-(float)cos(entity.Angle-1.57f), -(float)sin(entity.Angle-1.57f), 0.0f);
				else
					jdir = CVector(-(float)cos(entity.Angle), -(float)sin(entity.Angle), 0.0f);
				break;
			case CEntity::Dynamic:
				jdir = CVector(-(float)cos(entity.Angle), -(float)sin(entity.Angle), 0.0f);
				break;
			case CEntity::Snowball:
				jdir = entity.Trajectory.evalSpeed(NewTime).normed();
				break;
			case CEntity::Projectile:
				jdir = entity.Trajectory.evalSpeed(NewTime).normed();
				break;
			}

			if (entity.State!=CEntity::Falling && entity.Skeleton != NULL) entity.Position.z=entity.Position.z+1.0f;
			entity.Instance->setPos(entity.Position);
			entity.Instance->setRotQuat(jdir);

			if (entity.Skeleton != NULL)
			{
				entity.Skeleton->setPos(entity.Position);
				entity.Skeleton->setRotQuat(jdir);
			}

			if (entity.Headwear!=NULL)
			{
//				nlinfo("Seting headwear to pos %f %f %f", entity.Position.x, entity.Position.y, entity.Position.z); 
//				entity.Headwear->setPos(entity.Position+CVector(-0.07f,0.0f,-1.4f));
//				jdir = CVector(-(float)cos(entity.Angle), -(float)sin(entity.Angle), 0.0f);
//				entity.Headwear->setRotQuat(jdir);
			}

			if (entity.LeftHandInstance!= NULL)
			{
//				uint16 theBoneId = entity.Skeleton->getBoneIdByName("Bip01 R Hand");
//				nlinfo("BoneId= %d",theBoneId);
//				entity.LeftHandInstance->setPos(entity.Position+CVector(0.0f, 1.0f, 1.0f));
//				entity.LeftHandInstance->setRotQuat(jdir);
			}

			if (entity.RightHandInstance!= NULL)
			{
//				uint16 theBoneId = entity.Skeleton->getBoneIdByName("Bip01 R Finger21");
//				nlinfo("BoneId= %d",theBoneId);
//				UBone &RightHandBone2 = entity.Skeleton->getBone(theBoneId);
//				nlinfo("pos %f %f %f", RightHandBone2.getPos().x, RightHandBone2.getPos().y, RightHandBone2.getPos().z);  
//				nlinfo("rot %f %f %f %f", RightHandBone2.getRotQuat().w, RightHandBone2.getRotQuat().x, RightHandBone2.getRotQuat().y, RightHandBone2.getRotQuat().z );  
//				CMatrix theMatrix = RightHandBone2.getLastWorldMatrixComputed();
//				nlinfo("Matrix pos x %f y %f",theMatrix.getPos().x, theMatrix.getPos().y); 
//				entity.RightHandInstance->setPos(theMatrix.getPos());
//				entity.RightHandInstance->setRotQuat(theMatrix.getRot());
			}
		}

		if (entity.Source != NULL)
			entity.Source->setPos (entity.Position);

		if (entity.SourceEnvFX0 != NULL)
			entity.SourceEnvFX0->setPos (entity.Position);

		if (entity.SourceEnvFX1 != NULL)
			entity.SourceEnvFX1->setPos (entity.Position);
	
		if (entity.Particule != NULL)
		{
			entity.Particule->setPos(entity.Position);
		}

		if (entity.SelectionParticle!= NULL)
		{
			entity.SelectionParticle->setPos(entity.Position);
		}

		if (entity.Type == CEntity::Self)
		{
			MouseListener->setPosition(entity.Position);
		}
	}
}

// Draw entities names up the characters
void renderEntitiesNames ()
{
	// Setup the driver in matrix mode
	Driver->setMatrixMode3D (*Camera);
	// Setup the drawing context
	TextContext->setHotSpot (UTextContext::MiddleTop);
	TextContext->setColor (EntityNameColor);
	TextContext->setFontSize ((uint32)EntityNameSize);
	//
	for (EIT eit = Entities.begin (); eit != Entities.end (); eit++)
	{
		CEntity	&entity = (*eit).second;
		if (entity.Instance != NULL && entity.Type == CEntity::Other)
		{
			CMatrix		mat = Camera->getMatrix();
			mat.setPos(entity.Position + CVector(0.0f, 0.0f, 4.0f));
			mat.scale(10.0f);
			TextContext->render3D(mat, entity.Name);
		}
	}
}


// The entities preferences callback
void cbUpdateEntities (CConfigFile::CVar &var)
{
	if (var.Name == "EntityNameColor") EntityNameColor.set (var.asInt(0), var.asInt(1), var.asInt(2), var.asInt(3));
	else if (var.Name == "EntityNameSize") EntityNameSize = var.asFloat ();
	else if (var.Name == "ViewHeight") MinFallingViewHeight = var.asFloat ();
	else nlwarning ("Unknown variable update %s", var.Name.c_str());
}

void initEntities()
{
	ConfigFile.setCallback ("EntityNameColor", cbUpdateEntities);
	ConfigFile.setCallback ("EntityNameSize", cbUpdateEntities);
	ConfigFile.setCallback ("ViewHeight", cbUpdateEntities);

	cbUpdateEntities (ConfigFile.getVar ("EntityNameColor"));
	cbUpdateEntities (ConfigFile.getVar ("EntityNameSize"));
	cbUpdateEntities (ConfigFile.getVar ("ViewHeight"));
}

void releaseEntities()
{
}


// Reset the pacs position of an entity, in case pacs went wrong
void	resetEntityPosition(uint32 eid)
{
	uint32 sbid = NextEID++;
	EIT eit = findEntity (eid);

	CEntity	&entity = (*eit).second;

	UGlobalPosition	gPos;
	// get the global position
	gPos = GlobalRetriever->retrievePosition(entity.Position);
	// convert it in a vector 3d
	entity.Position = GlobalRetriever->getGlobalPosition(gPos);
	// get the good z position
	gPos.LocalPosition.Estimation.z = 0.0f;
	entity.Position.z = GlobalRetriever->getMeanHeight(gPos);

	// snap to the ground
	if (entity.VisualCollisionEntity != NULL && !GlobalRetriever->isInterior(gPos))
		entity.VisualCollisionEntity->snapToGround(entity.Position);

	if (entity.MovePrimitive != NULL)
		entity.MovePrimitive->setGlobalPosition(CVector(entity.Position.x, entity.Position.y, entity.Position.z+1.1f), 0);
}


void	shotSnowball(uint32 sid, uint32 eid, const CVector &start, const CVector &target, float speed, float deflagRadius)
{
	// get direction
//	CVector direction = (target-start).normed();

	// create a new snowball entity
	addEntity(sid,"", "", CEntity::Snowball, start, target, 0.0f,  CVector(0.1f, 0.1f, 0.1f), 0.0f, "", NULL);
	EIT sit = findEntity (sid);
	CEntity	&snowball = (*sit).second;

	snowball.AutoMove = 1;
	snowball.Trajectory.init(start, target, speed, CTime::getLocalTime ()+1000, 0);
	snowball.Instance->hide();

	EIT eit = findEntity (eid);
	CEntity	&entity = (*eit).second;
}

void	shotProjectile(uint32 projectileId, uint32 playerId, std::string shapeName, std::string psName, const CVector &start, const CVector &target, float speed)
{
	// get direction
	CVector direction = (target-start).normed();

	// create a new snowball entity
	addEntity(projectileId, "", shapeName, CEntity::Projectile, start, target, 0.0f,  CVector(0.1f, 0.1f, 0.1f),speed, psName, NULL);
	EIT sit = findEntity (projectileId);
	CEntity	&projectile = (*sit).second;

	projectile.AutoMove = 1;
	projectile.Trajectory.init(start, target, speed, CTime::getLocalTime (), 1);
	projectile.Instance->hide();
	projectile.PsExplosionName=psName;

	EIT eit = findEntity (playerId);
	CEntity	&entity = (*eit).second;
}


//
NLMISC_COMMAND(remove_entity,"remove a local entity","<eid>")
{
	// check args, if there s not the right number of parameter, return bad
	if(args.size() != 1) return false;

	uint32 eid = (uint32)atoi(args[0].c_str());
	removeEntity (eid, false);

	return true;
}

NLMISC_COMMAND(pacstest,"toggle pacs test","")
{
	if (PacsTest) PacsTest = false;
	else PacsTest = true;
	return true;
}

NLMISC_COMMAND(transportBug,"toggle transportBug test","<level>")
{
	if (DebugTranportBug) DebugTranportBug = false;
	else
	{
		DebugTranportBug = true;
		DTBugLevel = (uint8) atoi(args[0].c_str());
	}
	return true;
}


NLMISC_COMMAND(rightarm,"set right arm hold","<shape>")
{
	setEntityHand(Self->Id, false, 72000, args[0]);
		return true;
}


NLMISC_COMMAND(add_entity,"add a local entity","<nb_entities> <auto_update>")
{
	// check args, if there s not the right number of parameter, return bad
	if(args.size() != 2) return false;

	uint nb = (uint)atoi(args[0].c_str());

	for (uint i = 0; i < nb ; i++)
	{
		uint32 eid = NextEID++;
		CVector start(ConfigFile.getVar("StartPoint").asFloat(0), ConfigFile.getVar("StartPoint").asFloat(1), ConfigFile.getVar("StartPoint").asFloat(2));
		addEntity (eid, "Entity"+toString(eid), "", CEntity::Other, start, start, 1.0f,  CVector(0.1f, 0.1f, 0.1f),0.0f, "", NULL);
		EIT eit = findEntity (eid);
		(*eit).second.AutoMove = atoi(args[1].c_str()) == 1;
	}

	return true;
}

NLMISC_COMMAND(speed,"set the player speed ","[<entity_id>] <speed in km/h>")
{
	// check args, if there s not the right number of parameter, return bad
	if(args.size() == 1)
	{
		nlinfo("Setting speed");
		float speed = min( max( (float)atof(args[0].c_str()), 0.1f ), 200.0f ); // speed range in km/h
		if (Self != NULL)
		{
			// If online, send the chat line, otherwise, locally displays it
			if (isOnline ())
				sendChatLine (" has set his speed to "+args[0]+"\n");
//			else
//				addLine (string ("you said> " + "has set his speed to "+args[0]+"\n"));
			PlayerSpeed = speed / 3.6f;
			MouseListener->setSpeed( speed / 3.6f );
			Self->Speed = speed / 3.6f;
		}
	}
	else if(args.size() == 2)
	{
		float speed = min( max( (float)atof(args[1].c_str()), 0.1f ), 200.0f ); // speed range in km/h

		uint eid = (uint)atoi(args[0].c_str());
		EIT eit = findEntity (eid);
		CEntity	&entity = (*eit).second;

		entity.Speed = speed / 3.6f;
		if (entity.Type == CEntity::Self)
		{
			MouseListener->setSpeed(entity.Speed);
		}
	}
	return true;
}

NLMISC_COMMAND(goto, "go to a given position", "<x> <y>")
{
	// check args, if there s not the right number of parameter, return bad
	if(args.size() != 2) return false;

	if (Self == NULL) return false;

	CEntity	&entity = *Self;

	float	x, y;

	x = (float)atof(args[0].c_str());
	y = (float)atof(args[1].c_str());

	//
	if (entity.MovePrimitive != NULL && entity.VisualCollisionEntity != NULL)
	{
		UGlobalPosition	gPos;
		entity.MovePrimitive->setGlobalPosition(CVectorD(x, y, 0.0f), 0);
		// get the global position in pacs coordinates system
		entity.MovePrimitive->getGlobalPosition(gPos, 0);
		// convert it in a vector 3d
		entity.Position = GlobalRetriever->getGlobalPosition(gPos);
		// get the good z position
		gPos.LocalPosition.Estimation.z = 0.0f;
		entity.Position.z = GlobalRetriever->getMeanHeight(gPos);
		// snap to the ground
		if (!GlobalRetriever->isInterior(gPos))
			entity.VisualCollisionEntity->snapToGround( entity.Position );

		if (entity.Type == CEntity::Self)
		{
			MouseListener->setPosition(entity.Position);
		}
	}

	return true;
}

NLMISC_COMMAND(entities, "display all entities info", "")
{
	// check args, if there s not the right number of parameter, return bad
	if(args.size() != 0) return false;

	for (EIT eit = Entities.begin (); eit != Entities.end (); eit++)
	{
		CEntity	&e = (*eit).second;
		log.displayNL("%s %u (k%u) %s %d", (Self==&e)?"*":" ", e.Id, (*eit).first, e.Name.c_str(), e.Type);
	}
	return true;
}

NLMISC_COMMAND(drop,"drop an dynamic object from inventory","<do_id>")
{
	// check args, if there s not the right number of parameter, return bad
	if(args.size() == 1)
	{
		uint32 dynamicObjectId = (uint32)atof(args[0].c_str()); 
		sendDropDynamicObject( Self->Id, dynamicObjectId );
	}
	return true;
}

NLMISC_COMMAND(pick,"pick an dynamic object from the ground","<do_id>")
{
	// check args, if there s not the right number of parameter, return bad
	if(args.size() == 1)
	{
		uint32 dynamicObjectId = (uint32)atof(args[0].c_str()); 
		sendPickDynamicObject( Self->Id, dynamicObjectId );
	}
	return true;
}

NLMISC_COMMAND(give,"give your selected inventory item to the target id","<target_id>")
{
        if(args.size() == 1)
        {
                uint32 targetId = (uint32)atof(args[0].c_str()); 
                sendGive( targetId );
        }
        return true;
}

NLMISC_COMMAND(summon,"summon a mammoth from the beyond","<mammoth_id>")
{
	if(args.size() == 1)
	{
		uint32 mammothId = (uint32)atof(args[0].c_str()); 
		sendSummonMammoth( mammothId );
	}
	else
		return false;

	return true;

}

NLMISC_COMMAND(endpathtrace,"end and name the path trace","<string: pathname>")
{
	if(args.size() == 1)
	{
		string pathName = args[0].c_str(); 
		sendEndPathTrace( pathName );
	}
	else
		return false;

	return true;

}

NLMISC_COMMAND(walkpath,"tell shaman to walk path","<uint32: shamanId> <string pathname>")
{
	// check args, if there s not the right number of parameter, return bad
	if(args.size() != 2) return false;

	
	uint32 id = (uint32)atof(args[0].c_str());
	string name = args[1].c_str();
	sendWalkPath( id, name );
	
	return true;

}


NLMISC_COMMAND(startpathtrace,"tell the shaman service to start tracking your position","<none>")
{
	
	sendStartPathTrace();
	
	return true;
}
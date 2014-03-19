/** \file network.cpp
 * Animation interface between the game and NeL
 *
 * $Id: network.cpp,v 1.13 2002/06/06 05:51:01 robofly Exp $
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

#include <nel/misc/types_nl.h> 
#include <nel/misc/event_listener.h>
#include <nel/misc/command.h>
#include <nel/misc/log.h>
#include <nel/misc/displayer.h>

#include <nel/net/login_client.h>

#include <nel/3d/u_text_context.h>

#include "client.h"
#include "commands.h"
#include "camera.h"
#include "network.h"
#include "entities.h"
#include "interface.h"
#include "graph.h"
#include "stats_display.h"
#include "inventory_display.h"
#include "client_world_objects.h"
#include "skills_display.h"
//#include "mouse_listener.h"
#include "tracers.h"
#include "envfx.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;
using namespace NL3D;

//
// Variables
//

CCallbackClient *Connection = NULL;

//
// Functions
//

static void cbClientDisconnected (TSockId from, void *arg)
{
	nlwarning ("You lost the connection to the server");

	askString ("You are offline!!!", "", 2, CRGBA(64,0,0,128));
}

static void cbAddEntity (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32      id;
	uint8		classId;
	string		name;
	std::string meshName;
	CVector		position;
	CVector		heading;
	CVector		massCenter;
	CVector		dimensions;
	float		radius;
	float		weight;
	float		scale;

	msgin.serial(id);
	msgin.serial(classId);
	msgin.serial(name);
	msgin.serial(meshName);
	msgin.serial(position);
	msgin.serial(heading);
	msgin.serial(massCenter);
	msgin.serial(dimensions);
	msgin.serial(radius);
	msgin.serial(weight);
	msgin.serial(scale);

	//meshName="gnu.shape";

	if (ShowDebugInChat) nlinfo("id=%d", id);
	if (ShowDebugInChat) nlinfo("ENTITY Net name=%s", name);
	if (ShowDebugInChat) nlinfo("ENTITY Net mesh=%s", meshName);
	if (ShowDebugInChat) nlinfo("ENTITY Net classId=%d", classId);
	if (classId==42) classId=1;

	if (id	> 60000) classId = 2;

	EIT eit = findEntity (id, false);
	if (eit != Entities.end ())
	{
		if (ShowDebugInChat) nlinfo ("Entity %d alreaxdy exist", id);
	}
	else if (id != Self->Id)
	{	
		if (ShowDebugInChat) nlinfo("adding entity: meshName %s",meshName);
		if (ShowDebugInChat) nlinfo("at: %f %f %f",position.x,position.y,position.z);
		addEntity(id, name, meshName, (CEntity::TType) classId, position, position, radius, CVector(1.0f, 1.0f, 1.0f), 0.0f, "", NULL);
	}
	else
	{
		if (ShowDebugInChat) nlinfo ("Receive my add entity");
	} 
	nlinfo("cbAddEntitydone");
}

static void cbAddDynamicObject (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	CClientDynamicObject dynamicObject;

	if (ShowDebugInChat) nlinfo("in add dynamic object");
	msgin.serial( dynamicObject );

	DIT ItDynamicObjectMap = DynamicObjects.find( dynamicObject.Id );
	
	if ( ItDynamicObjectMap != DynamicObjects.end() )
	{
		DynamicObjects.erase( dynamicObject.Id );
	}
	if (ShowDebugInChat) nlinfo("inserting %d",dynamicObject.Id);
	DynamicObjects.insert (make_pair (dynamicObject.Id, dynamicObject));
}

static void cbAddFirearm (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	CClientFirearm dynamicFirearmObject;
	if (ShowDebugInChat) nlinfo("in add dynamic firearm object");

	msgin.serial( dynamicFirearmObject );

	FIT ItDynamicFireObjectMap = Firearms.find( dynamicFirearmObject.Id );
	
	if ( ItDynamicFireObjectMap != Firearms.end() )
	{
		Firearms.erase( dynamicFirearmObject.Id );
	}
	Firearms.insert (make_pair (dynamicFirearmObject.Id, dynamicFirearmObject));
}

static void cbAddBoxOfBullets (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	CClientBoxOfBullets boxOfBulletsObject;
	if (ShowDebugInChat) nlinfo("in add box of bullets");

	msgin.serial( boxOfBulletsObject );

	BIT ItBoxOfBulletsMap = BoxOfBullets.find( boxOfBulletsObject.Id );
	
	if ( ItBoxOfBulletsMap != BoxOfBullets.end() )
	{
		BoxOfBullets.erase( boxOfBulletsObject.Id );
	}
	BoxOfBullets.insert (make_pair (boxOfBulletsObject.Id, boxOfBulletsObject));
}

static void cbAddNutrition (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	CClientNutrition nutritionObject;
	if (ShowDebugInChat) nlinfo("in add nutrition");

	msgin.serial( nutritionObject );

	NIT ItNutritionMap = Nutrition.find( nutritionObject.Id );
	
	if ( ItNutritionMap != Nutrition.end() )
	{
		Nutrition.erase( nutritionObject.Id );
	}
	Nutrition.insert (make_pair (nutritionObject.Id, nutritionObject));
}

static void cbRemoveEntity (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32 id;
	if (ShowDebugInChat) nlinfo("remove entity");
	msgin.serial (id);

	if (ShowDebugInChat) nlinfo ("Receive remove entity %u", id);

	EIT eit = findEntity (id);
	CEntity	&entity = (*eit).second;

	if (ShowDebugInChat) nlinfo ("Player named '%s' goes offline", entity.Name.c_str());

	removeEntity (id, false);
}

/*static void cbAddSpotSound (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32 id;
	std::string soundName;
	float gain, pitch;

	msgin.serial (id);
	msgin.serial (soundName);
	msgin.serial (gain);
	msgin.serial (pitch);

	EIT eit = findEntity (id);
	CEntity	&entity = (*eit).second;

	addSpotSoundToEntity (id, soundName, gain, pitch);
}
*/
static void cbRemoveDynamicObject (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32 id;
//	nlinfo("remove do");
	msgin.serial (id);

//	nlinfo ("Receive remove entity %u", id);

	EIT eit = findEntity (id);
	CEntity	&entity = (*eit).second;

	removeEntity (id, false);
}			


static void cbEntityPos (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32 id;
	CVector position;
	float angle;
	uint32 state;

	msgin.serial (id, position, angle, state);

	if (DebugTranportBug)
	{
		if (DTBugLevel==7)
		{	
			if (ShowDebugInChat) nlinfo("from networkd id %i",id);
			if (ShowDebugInChat) nlinfo("pos at %f %f %f", position.x,position.y,position.z);
		}
	}


	//nlinfo ("(%5d) Receive entity pos %u (%f,%f,%f) %f, %u", msgin.length(), id, position.x, position.y, position.z, angle, state);

	if (Self->Id == id)
	{
		// receive my info, ignore them, i know where i am
		return;
	}

	EIT eit = findEntity (id, false);
	if (eit == Entities.end ())
	{
		nlwarning ("can't find entity %u", id);
		sendEntityMissing(id);
	}
	else
	{
		CEntity	&entity = (*eit).second;

		entity.ServerPosition = position;
		entity.AuxiliaryAngle = angle;
		entity.setAnimState( (CEntity::AState) state );
	}
}

static void cbHit(CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32 projectileId, playerId;

	msgin.serial (projectileId, playerId);

//	nlinfo ("Receive hit msg %u %u %d", sid, eid, direct);

	EIT eit = findEntity (playerId);
	CEntity	&entity = (*eit).second;

	EAnim a = entity.AnimQueue.front ();
//	playAnimation (entity, HitAnim, true);
	playAnimation (entity, a);

	removeEntity (projectileId, true);
}

static void cbTracer(CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	CVector			position;
	std::string		tracerText;

	msgin.serial (position);
	msgin.serial (tracerText);
	if (ShowDebugInChat) nlinfo(" Text=%s", tracerText);
	if (ShowDebugInChat) nlinfo(" pos=%f %f %f", position.x, position.y, position.z);
	addTextTracer(tracerText, position);
}


static void cbAddProjectile (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32		projectileId, playerId;
	std::string	shapeName, psName;
	CVector start,
			target,
			psScale;
	float   speed;

//	nlinfo("in add projectile");

	msgin.serial( projectileId );
	msgin.serial( playerId );
	msgin.serial( shapeName );
	msgin.serial( psName );
	msgin.serial( start );
	msgin.serial( target );
	msgin.serial( speed );
	if (ShowDebugInChat)	
	{
		nlinfo ("--------------------");
		nlinfo ("ProjectileId = %d", projectileId );
		nlinfo ("PlayerId = %d", playerId );
		nlinfo ("shapeName = %s", shapeName );
		nlinfo ("psName = %s", psName );
		nlinfo ("start = %d", start );
		nlinfo ("target x = %f", target.x );
		nlinfo ("target y = %f", target.y );
		nlinfo ("target z = %f", target.z );

		nlinfo ("start x = %f", start.x );
		nlinfo ("start y = %f", start.y );
		nlinfo ("start z = %f", start.z );
	}
	
	speed=200.0f;
	if (ShowDebugInChat) nlinfo ("speed = %f", speed );
	speed=200.0f;

	shotProjectile (projectileId, playerId, shapeName, psName, start, target, speed );
}

static void cbAddSpellFX(CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32	playerId;
	std::string psName, soundName;

	msgin.serial( playerId );
	msgin.serial( psName );
	msgin.serial( soundName );

	if (ShowDebugInChat) nlinfo("Add Spellfx FROM: %d", playerId);
	if (playerId!=Self->Id) addSpellFX( playerId, psName, soundName );
}

static void cbRemoveSpellFX(CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32	playerId;
	msgin.serial( playerId );

	if (ShowDebugInChat) nlinfo("GO RemoveSpellFX from %d", playerId);
	if (playerId!=Self->Id) removeSpellFX( playerId );
}

static void cbAddEnvFX(CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32	playerId;
	std::string name, psName, soundName;

	msgin.serial( playerId );
	msgin.serial( name );
	msgin.serial( psName );
	msgin.serial( soundName );

	if (ShowDebugInChat) nlinfo("GOt Add EnvFX from %d", playerId);

	if (playerId!=Self->Id && name=="rain") rainOn(psName,soundName);
}

static void cbRemoveEnvFX(CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32	playerId;
	string name;

	msgin.serial( playerId );

	if (ShowDebugInChat) nlinfo("GO RemoveEnvFX from %d", playerId);

	if (playerId!=Self->Id && name=="rain") rainOff();
}

static void cbChat (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	string line;
	msgin.serial (line);
	addLine (line);
}

/****************************************************************************
 * cbDBRawQuery
 *
 * Receive position messages from the Position Service to send it to all the
 * clients.
 ****************************************************************************/
void cbDBRawResult( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	string sql_out;
	vector<string> fieldnames;
	vector<string> queryrows;
	string tuples;
	string fields;

	msgin.serial( sql_out );
	if (sql_out=="OK")
	{
		msgin.serial( tuples );
		msgin.serial( fields );
		msgin.serialCont( fieldnames );
		msgin.serialCont( queryrows );

		int i, k, j;
		stringstream s;

		for (i=0; i < atoi(fields.c_str()); ++i)
		{
			s << fieldnames.at(i) << " \t|\t ";
		}

		s.str("");

		addLine(  s.str() );
	
		k=0;
		for (i=0; i < atoi(tuples.c_str()); ++i)
		{
			for (j=0; j < atoi(fields.c_str()); ++j)
			{
				s << queryrows.at(k) << " \t|\t "; 
				++k;
			}
			addLine( s.str() );
			s.str("");
		}

	}
	else
	{
		addLine( sql_out );
	}
}


static void cbInitStats (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	if (ShowDebugInChat) nlinfo("I have Created Stats");

	msgin.serial( CharacterStats );
	nlinfo("I have Created Stats");
	for ( int i=0; i!=6; ++i )
	{
		if (ShowDebugInChat) nlinfo("/n i32:%i has the value of %i \n", i, CharacterStats.geti32()[i]);		 
	}
	for ( i=0; i!=7; ++i )
	{
		if (ShowDebugInChat) nlinfo("/n str:%i has the value of %s", i, CharacterStats.getstr()[i]);		 
	}
	for ( i=0; i!=28; ++i )
	{
		if (ShowDebugInChat) nlinfo("/n i16:%i has the value of %i \n", i, CharacterStats.geti16()[i]);		 
		}
	if (ShowDebugInChat) nlinfo("And they seem ok");
}

static void cbInitSkills (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	if (ShowDebugInChat) nlinfo("I have Created Skills");

	msgin.serial(CharacterSkills );
	if (ShowDebugInChat) nlinfo("I have Created Skills");

	for ( int i=0; i!=28; ++i )
	{
		if (ShowDebugInChat) nlinfo("/n i16:%i has the value of %i \n", i, CharacterSkills.geti16()[i]);		 
	}
	if (ShowDebugInChat) nlinfo("And they seem ok");
}


static void cbInitInventory (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	msgin.serial( CharacterInventory );
	if (ShowDebugInChat) nlinfo("I have Created the Inventory");
	if (ShowDebugInChat) nlinfo("And they seem ok");
	initInventoryScene();
}

static void cbChangeInventory (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32 objectId;
	bool   add;
	uint8  classId;

	msgin.serial( objectId );
	msgin.serial( add );
	msgin.serial( classId );
	if (ShowDebugInChat) nlinfo("in change inventory classId=%d", classId);
	if (add) CharacterInventory.addItem( objectId, classId );
	else CharacterInventory.dropItem( objectId);

	if (ShowDebugInChat) nlinfo("Inventory ok added/removed %d", objectId);
	initInventoryScene();
}

static void cbChangeStat (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	if (ShowDebugInChat) nlinfo("CHSTAT");
	uint8 statId;
	sint16 value;
	msgin.serial( statId );
	msgin.serial( value );
	if (ShowDebugInChat) nlinfo("CHSTAT = %d id and %d value", statId, value );
	CharacterStats.update( statId, value );
}

static void cbChangeSkill (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	if (ShowDebugInChat) nlinfo("CHSKILL");
	uint8 skillId;
	uint16 value;
	msgin.serial( skillId );
	msgin.serial( value );
	if (ShowDebugInChat) nlinfo("CHSKILL = %d id and %d value", skillId, value );
	CharacterSkills.update( skillId, value );
}

static void	cbChangeEntityHand(CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32 eid, did;
	bool leftHand;
	std::string meshName;

	msgin.serial( eid );
	msgin.serial( leftHand );
	msgin.serial( did );
	msgin.serial( meshName );
	if (ShowDebugInChat) nlinfo("CH_ENTITY_HAND %d %s", did, meshName );

	setEntityHand(eid, leftHand, did, meshName);
}

static void cbIdentification (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	uint32 id;
	msgin.serial (id);
	
	if (ShowDebugInChat) nlinfo ("my online id is %u", id);

	if (Self == NULL)
		nlerror ("Self is NULL");

	if (Self->Id != id)
	{
		if (ShowDebugInChat) nlinfo ("remaping my entity from %u to %u", Self->Id, id);
		
		// copy my old entity
		CEntity me = *Self;
		
		// set my new online id
		me.Id = id;

		// add my new entity in the array
		EIT eit = (Entities.insert (make_pair (id, me))).first;

		// remove my old entity
		Entities.erase (Self->Id);

		// remap Self
		Self = &((*eit).second);
	}

	// send to the network my entity		
	if (ShowDebugInChat) nlinfo("about to send network entity");
	sendAddEntity (Self->Id, Self->Name, 1, Self->Position, Self->ShapeName);
}

/*static void cbDummy (CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
}*/

// Array that contains all callback that could comes from the server
static TCallbackItem ClientCallbackArray[] =
{
	{ "ADD_ENTITY", cbAddEntity },
	{ "ENTITY_POS", cbEntityPos },
//	{ "ENTITY_SCALE", cbEntityScale },
	{ "REMOVE_ENTITY", cbRemoveEntity },

	{ "ADD_DO", cbAddDynamicObject },
	{ "ADD_FIREARM", cbAddFirearm },
	{ "ADD_BOXOFBULLETS", cbAddBoxOfBullets },
	{ "ADD_NUTRITION", cbAddNutrition },

	{ "ADD_PROJECTILE", cbAddProjectile }, 
//	{ "ADD_SPOTSOUND", cbAddSpotSound },

	{ "ADD_SPELL_FX", cbAddSpellFX },
	{ "REMOVE_SPELL_FX", cbRemoveSpellFX },
	{ "ADD_ENV_FX", cbAddEnvFX },
	{ "REMOVE_ENV_FX", cbRemoveEnvFX },

	{ "INIT_STATS", cbInitStats },
	{ "INIT_INV", cbInitInventory },
	{ "INIT_SKILLS", cbInitSkills },
	
	{ "CH_STATS", cbChangeStat },
	{ "CH_SKILL", cbChangeSkill },
	{ "CH_INV", cbChangeInventory },
	{ "CH_ENTITY_HAND", cbChangeEntityHand },

	{ "IDENTIFICATION", cbIdentification },
	{ "HIT", cbHit },
	{ "CHAT", cbChat },
	{ "TRACER", cbTracer },
	{ "DB_RAW_RESULT", cbDBRawResult },
};


bool	isOnline ()
{
	return Connection != NULL && Connection->connected ();
}

void	sendAddEntity (uint32 id, string &name, uint8 race, CVector &startPosition, std::string shapeName)
{
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "POSSESS");//3.3 we do both
	msgout.serial( name );//4.4
	msgout.serial( shapeName );
	Connection->send (msgout);
	if (ShowDebugInChat) nlinfo("have sent add entity");
	
	CMessage msgout2 (Connection->getSIDA(), "ADD_ENTITY"); //2.23
	msgout2.serial( id );//2.23
	msgout2.serial( name );//2.23
	msgout2.serial( race );//2.23
	msgout2.serial( startPosition );//2.23
	Connection->send (msgout2);
}

void	sendPickDynamicObject (uint32 playerId, uint32 dynamicObjectId)
{
	if (!isOnline ()) return;

	if (dynamicObjectId<9000) return;

	if (ShowDebugInChat) nlinfo("sent pick");
	
	CMessage msgout (Connection->getSIDA(), "PICK_DO");
	msgout.serial ( playerId, dynamicObjectId);
	Connection->send (msgout);
}

void	sendDropDynamicObject (uint32 playerId, uint32 dynamicObjectId)
{
	if (!isOnline ()) return;
	if (dynamicObjectId<50000 || dynamicObjectId>90000) return;

	CMessage msgout (Connection->getSIDA(), "DROP_DO");
	msgout.serial ( playerId, dynamicObjectId);
	Connection->send (msgout);
	if (ShowDebugInChat) nlinfo("sent drop");
}

void	sendHoldDynamicObject (uint32 dynamicObjectId, bool leftHand)
{
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "HOLD_DO");
	msgout.serial ( dynamicObjectId );
	msgout.serial ( leftHand );

	Connection->send (msgout);
	if (ShowDebugInChat) nlinfo("sent hold dynamic object %d", dynamicObjectId);
} 

void	sendChatLine (string Line)
{
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "CHAT");
	string s;
	if (Self != NULL)
	{
		string line =  Self->Name + string("> ") + Line;
		msgout.serial (line);
	}
	else
	{
		string line = string("Unknown> ") + Line;
		msgout.serial (line);
	}

	Connection->send (msgout);
}


void	sendEntityMissing ( uint32 id )
{
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "ENTITY_MISSING");
	msgout.serial (id);

	Connection->send (msgout);

}

void    sendGive (uint32 targetId)
{
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "GIVE_DO");
	msgout.serial(targetId);
	Connection->send (msgout);
}

void    sendResetShaman ()
{
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "RESET_SHAMAN");
	Connection->send (msgout);
	if (ShowDebugInChat) nlinfo("have sent RESET_SHAMAN");
}

void	sendEntityPos (CEntity &entity)
{
	if (!isOnline ()) return;

	uint32 state = (uint32) entity.AnimState;

	CMessage msgout (Connection->getSIDA(), "ENTITY_POS");
	msgout.serial (entity.Id, entity.Position, entity.Angle, state);

	Connection->send (msgout);
	
//	nlinfo("(%5d) Sending pos to network (%f,%f,%f, %f), %u", msgout.length(), entity.Position.x, entity.Position.y, entity.Position.z, entity.Angle, state);
}

void	sendFire1 (const NLMISC::CVector &target, float angle, float pitch)
{
	if (!isOnline ()) return;
	if (ShowDebugInChat) nlinfo("Fire1");
	CMessage msgout (Connection->getSIDA(), "FIRE1");
	msgout.serial(const_cast<CVector &>(target));
	msgout.serial(angle);
	msgout.serial(pitch);
	Connection->send (msgout);
// nlinfo("sent fire1");
//	nlinfo("Sending snowball to network (%f,%f,%f) to (%f,%f,%f) with %f %f", position.x, position.y, position.z, target.x, target.y, target.z, speed, deflagRadius);
}

void	sendSelectedInInventory (uint32 did)
{
if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "INV_SELECT");
	msgout.serial( did );
	Connection->send (msgout);
//	nlinfo("Sending snowball to network (%f,%f,%f) to (%f,%f,%f) with %f %f", position.x, position.y, position.z, target.x, target.y, target.z, speed, deflagRadius);
}

// Send a request for SpellFX
void	sendAddSpellFX(uint32 playerId, std::string psName, std::string soundName)
{
	if (!isOnline ()) return;

	if (ShowDebugInChat) nlinfo("Sending spell %s", psName);
	CMessage msgout (Connection->getSIDA(), "ADD_SPELL_FX");
	msgout.serial(playerId);
	msgout.serial(psName);
	msgout.serial(soundName);
	Connection->send (msgout);
}

// Send a request for SpellFX removal
void	sendRemoveSpellFX(uint32 playerId)
{
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "REMOVE_SPELL_FX");
	msgout.serial(playerId);
	Connection->send (msgout);
}

// Send a request for Environmental FX
void	sendAddEnviromentalFX(uint32 playerId, std::string name, std::string psName, std::string soundName)
{
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "ADD_ENV_FX");
	msgout.serial(playerId);
	msgout.serial(name);
	msgout.serial(psName);
	msgout.serial(soundName);
	Connection->send (msgout);
}

// Send a request for removal of Environmental FX
void	sendRemoveEnviromentalFX(uint32 playerId)
{
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "REMOVE_ENV_FX");
	msgout.serial(playerId);
	Connection->send (msgout);
}


void	sendSummonMammoth (uint32 mammothId)
{
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "SUMMON_MAMMOTH");
	msgout.serial(mammothId);
	Connection->send (msgout);
//	nlinfo("Sending snowball to network (%f,%f,%f) to (%f,%f,%f) with %f %f", position.x, position.y, position.z, target.x, target.y, target.z, speed, deflagRadius);
}

void	sendSummonShaman (uint32 shamanId)
{
	if (ShowDebugInChat) nlinfo("Summoning a Shaman");
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "SUMMON_SHAMAN");
	msgout.serial(shamanId);
	Connection->send (msgout);
//	nlinfo("Sending snowball to network (%f,%f,%f) to (%f,%f,%f) with %f %f", position.x, position.y, position.z, target.x, target.y, target.z, speed, deflagRadius);
}

void	sendStartPathTrace ()
{
	if (ShowDebugInChat) nlinfo("Starting Path Trace");
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "START_PATH_TRACE");
	Connection->send (msgout);
}

void	sendEndPathTrace (string pathName)
{
	if (ShowDebugInChat) nlinfo("Ending Path Trace");
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "END_PATH_TRACE");
	
	msgout.serial( pathName );

	Connection->send (msgout);
}

void	sendWalkPath (uint32 walkerId, string pathName)
{
	if (ShowDebugInChat) nlinfo("Ending Path Trace");
	if (!isOnline ()) return;

	CMessage msgout (Connection->getSIDA(), "WALK_PATH");
	
	msgout.serial( walkerId );
	msgout.serial( pathName );

	Connection->send (msgout);
}

TTime LastPosSended;

void	initNetwork()
{
	Connection = new CCallbackClient;
	Connection->addCallbackArray (ClientCallbackArray, sizeof (ClientCallbackArray) / sizeof (ClientCallbackArray[0]));
	Connection->setDisconnectionCallback (cbClientDisconnected, NULL);

	LastPosSended = 0;
}	

void	updateNetwork()
{
	if (Connection != NULL)
	{
		Connection->update ();

		sint64 newBytesDownloaded = (sint64) Connection->newBytesDownloaded ();
		sint64 newBytesUploaded = (sint64) Connection->newBytesUploaded ();

//		TextContext->setHotSpot (UTextContext::MiddleTop);
//		TextContext->setColor (CRGBA(255, 255, 255, 255));
//		TextContext->setFontSize (14);
//		TextContext->printfAt (0.5f, 0.99f, "d:%"NL_I64"u u:%"NL_I64"u / d:%"NL_I64"u u:%"NL_I64"u / d:%"NL_I64"u u:%"NL_I64"u",
//		Connection->bytesDownloaded (), Connection->bytesUploaded (),
//		Connection->getBytesReceived (),Connection->getBytesSended (), 
//		newBytesDownloaded, newBytesUploaded);

		DownloadGraph.addValue ((float)newBytesDownloaded);
		UploadGraph.addValue ((float)newBytesUploaded);

		if (isOnline () && Self != NULL)
		{
			static float oldAngle = 0.0f;
			
			if ((Self->Angle != oldAngle || !Self->AnimChangeTransmitted || Self->AnimState == CEntity::Walk) && CTime::getLocalTime () > LastPosSended + 50)
			{
				sendEntityPos (*Self);
				LastPosSended = CTime::getLocalTime ();

				oldAngle = Self->Angle;
				Self->AnimChangeTransmitted = true;
			}
		}
	}
}

void	releaseNetwork()
{
	if (Connection != NULL)
	{
		if (Connection->connected ())
			Connection->disconnect ();

		delete Connection;
		Connection = NULL;
	}
}


NLMISC_COMMAND(connect,"connect to the login system","<login> <password>")
{
	// check args, if there s not the right number of parameter, return bad
	if (args.size() != 2) return false;

	string LoginSystemAddress = ConfigFile.getVar("LoginSystemAddress").asString ();

	string res = CLoginClient::authenticate (LoginSystemAddress+":49999", args[0], args[1], 1);
	if (!res.empty ()) log.displayNL ("Authentification failed: %s", res.c_str());

	log.displayNL ("Please select a shard in the list using \"/select <num>\" where <num> is the shard number");
	for (uint32 i = 0; i < CLoginClient::ShardList.size (); i++)
	{
		log.displayNL ("Shard %d: %s (%s)", i, CLoginClient::ShardList[i].ShardName.c_str(), CLoginClient::ShardList[i].WSAddr.c_str());
	}

	return true;
}

NLMISC_COMMAND(disconnect,"disconnect from the shard","")
{
	// check args, if there s not the right number of parameter, return bad
	if (args.size() != 0) return false;

	if (!isOnline()) log.displayNL ("You already are offline");

	Connection->disconnect ();

	return true;
}
 
NLMISC_COMMAND(resetshaman,"reset the shaman","")
{
	// check args, if there s not the right number of parameter, return bad
	if (args.size() != 0) return false;

	if (isOnline()) sendResetShaman();
	return true;
}

NLMISC_COMMAND(select,"select a shard using his number","<shard_number>")
{
	// check args, if there s not the right number of parameter, return bad
	if (args.size() != 1) return false;

	uint32 num = (uint32) atoi (args[0].c_str());

	string res = CLoginClient::connectToShard (num, *Connection);
	if (!res.empty ()) log.displayNL ("Connection failed: %s", res.c_str());

	log.displayNL ("You are online!!!");

	return true;
}

NLMISC_COMMAND(dbquery,"send a raw PSQL query","<the query string>")
{
	// check args, if there s not the right number of parameter, return bad
	if (args.size() == 0) return false;

	string msg_str;

	for(int i=0; i < args.size(); ++i)
	{
		msg_str += args[i].c_str();
		msg_str += " ";
	}

	if (ShowDebugInChat) nlinfo("Send DB_RAW_QUERY from Client");

	if (!isOnline ()) return false;

	CMessage msgout (Connection->getSIDA(), "DB_RAW_QUERY");
	msgout.serial (msg_str);
	Connection->send (msgout);
	
	return true;
}

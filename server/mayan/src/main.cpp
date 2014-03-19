/*
 * This file contain the Snowballs Mayan Service.
 *
 * $Id: main.cpp,v 1.1 2002/03/20 18:11:51 robofly Exp $
 */

/*
 * Copyright, 2000 - 2001 Nevrax Ltd.
 *
 * This file is part of NEVRAX SNOWBALLS.
 * NEVRAX SNOWBALLS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * NEVRAX SNOWBALLS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NEVRAX SNOWBALLS; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

// This include is mandatory to use NeL. It include NeL types.
#include <nel/misc/types_nl.h>
#include <nel/misc/vectord.h>
#include <nel/misc/config_file.h>
#include <nel/misc/path.h>

// We're using the NeL Service framework, and layer 4
#include <nel/net/service.h>
#include <nel/net/net_manager.h>

// For pacs

#include <vector>

#include <nel/pacs/u_retriever_bank.h>
#include <nel/pacs/u_global_retriever.h>
#include <nel/pacs/u_move_container.h>
#include <nel/pacs/u_move_primitive.h>
#include <nel/pacs/u_global_position.h>

#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance_group.h>
#include <nel/3d/u_visual_collision_manager.h>

#include <nel/3d/u_instance.h>

#include "../../frontend/src/stats.h"
#include "../../frontend/src/inventory.h"
#include "../../frontend/src/world_objects.h"
#include "../../frontend/src/world_types.h"
#include "../../frontend/src/skills.h"

using namespace NLNET;
using namespace NLMISC;
using namespace std;
using namespace NL3D;
using namespace NLPACS;

static	uint16 serviceId	= 2013;
//static	uint32	START_PROJECTILE_ID = 4200000;

// Define information used for all connected entities
struct _entity
{
	_entity( uint32 Id, uint8 ClassId, std::string Name, std::string MeshName, CVector Position, 
		CVector Heading, CVector MassCenter, CVector Dimensions, float Radius,
		float Weight, float Scale ) :
		id( Id ), classId( ClassId ), name( Name ), meshName( MeshName ), position( Position ), 
		heading( Heading ), massCenter( MassCenter), dimensions( Dimensions), radius( Radius ), 
		weight( Weight ), scale( Scale ) { }

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
};

struct _player
{
	_player( uint32 Id, uint8 ClassId, std::string Name, std::string MeshName, CVector Position, 
		CVector Heading, CVector MassCenter, CVector Dimensions, float Radius, 
		float Weight, float Scale ) :
		id( Id ), classId( ClassId ), name( Name ), meshName( MeshName ), position( Position ), 
		heading( Heading ), massCenter( MassCenter), dimensions( Dimensions), radius( Radius ),  
		weight( Weight ), scale( Scale ) { }

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

	float	speed;

	// The angle of the entity
	float	angle;

	// The PACS move primitive
	UMovePrimitive			*movePrimitive;

	uint					state;
	NLMISC::TTime			stateStart;

	bool		possessed;
};


// List of all the entities except players
typedef map<uint32, _entity> _emap;
_emap entityMap;

// List of all the players
typedef map<uint32, _player> _pmap;
_pmap playerMap;

typedef map<uint32,CInventory>			CInvMap;
typedef map<uint32,CStats>				CStatsMap;
typedef map<uint32,CSkills>				CSkillsMap;

typedef map<uint32,CDynamicObject>		CDynamicObjectMap;
typedef map<uint32,CFirearm>			CFirearmMap;
typedef map<uint32,CBoxOfBullets>		CBoxOfBulletsMap;
typedef map<uint32,CNutrition>			CNutritionMap;

typedef map<uint32,CFirearmType>		CFireTypesMap;
typedef map<uint32,CBulletType>			CBulletTypesMap;
typedef map<uint32,CBoxOfBulletsType>	CBoxOfBulletsTypeMap;
typedef map<uint32,CNutritionType>		CNutritionTypeMap;

CInvMap					InvMap;
CStatsMap				StatsMap;
CSkillsMap				SkillsMap;

CDynamicObjectMap		DynamicObjectMap;
CFirearmMap				FirearmMap;
CBoxOfBulletsMap		BoxOfBulletsMap;
CNutritionMap			NutritionMap;

CFireTypesMap			FireTypesMap;
CBulletTypesMap			BulletTypesMap;
CBoxOfBulletsTypeMap	BoxOfBulletsTypeMap;
CNutritionTypeMap		NutritionTypeMap;

//
// Pacs Variables
//

// The retriever bank used in the world
URetrieverBank				*RetrieverBank;
// The global retriever used for pacs
UGlobalRetriever			*GlobalRetriever;
// The move container used for dynamic collisions
UMoveContainer				*MoveContainer;

// The previous and current update dates
TTime				 LastTime, NewTime, LastSend;

// The collision primitive for the instances in the landscape
vector<UMovePrimitive *>	InstancesmovePrimitives;

// The collision bits used by pacs (dynamic collisions)
enum
{
	SelfCollisionBit = 1,
	OtherCollisionBit = 2,
	SnowballCollisionBit = 4,
	StaticCollisionBit = 8
};

vector<UInstanceGroup*>	 InstanceGroups;

// This class contains all variables that are in the client.cfg
CConfigFile			 ConfigFile;

CVector getPosOfEntity( uint32 entityId )
{
	_emap::iterator	ItEntityMap = entityMap.find( entityId );
	if( ItEntityMap != entityMap.end() )
		return (*ItEntityMap).second.position;
	else
	{
		CVector zero= CVector(0.0f, 0.0f, 0.0f);
		return zero;
	}
}

void	releasePACS()
{
	// create a move primite for each instance in the instance group
	uint	i;
	for (i=0; i<InstancesmovePrimitives.size(); ++i)
		MoveContainer->removePrimitive(InstancesmovePrimitives[i]);

	InstancesmovePrimitives.clear();

	// delete all allocated objects
	UGlobalRetriever::deleteGlobalRetriever(GlobalRetriever);
	URetrieverBank::deleteRetrieverBank(RetrieverBank);
	UMoveContainer::deleteMoveContainer(MoveContainer);

	// delete the visual collision manager
//	Scene->deleteVisualCollisionManager(VisualCollisionManager);

}
/*original header
// This include is mandatory to use NeL. It include NeL types.
#include <nel/misc/types_nl.h>

#include <nel/misc/vector.h>

#include <nel/misc/time_nl.h>

// We're using the NeL Service framework, and layer 4
#include <nel/net/service.h>
#include <nel/net/net_manager.h>

#include <map>
#include <list>

#include "physics.h"


using namespace NLMISC;
using namespace NLNET;
using namespace std;


#define PLAYER_RADIUS      1.0f
#define SNOWBALL_RADIUS    0.1f
#define START_SNOW_ID      2000000000
#define THROW_ANIM_OFFSET  1000


// Define information used for all connected players to the shard.
struct _player
{
	_player( uint32 Id, string Name, uint8 Race, CVector Position ) :
		id( Id ), name( Name ), race( Race ), position( Position ) { }
	uint32  id;
	string  name;
	uint8   race;
	CVector position;
};

// List of all the players connected to the shard.
typedef map<uint32, _player> _pmap;
_pmap playerList;

// Define informations used for the snowballs management
struct _snowball
{
	_snowball( uint32 Id, uint32 Owner, CTrajectory Traj, float ExplosionRadius ) :
		id( Id ), owner( Owner ), traj( Traj ), explosionRadius( ExplosionRadius ) { }
	uint32      id;
	uint32      owner;
	CTrajectory traj;
	float       explosionRadius;
};

// List of all the games snowballs
list<_snowball> snoList;
*/

/****************************************************************************
 * Function:   cbAddEntity
 *             Callback function called when the Mayan Service receive a
 *             "ADD_ENTITY" message
 * Arguments:
 *             - msgin:  the incomming message
 *             - from:   the "sockid" of the sender (usually useless for a
 *                       CCallbackClient)
 *             - server: the CCallbackNetBase object (which really is a
 *                       CCallbackServer object, for a server)
 ****************************************************************************/
/*void cbAddEntity ( CMessage& msgin, TSockId from, CCallbackNetBase& server )
{
	bool    all;
	uint32  id;
	string  name;
	uint8   race;
	CVector startPoint;
	std::string meshName;
//	CVector		position;
	CVector		heading;
	CVector		massCenter;
	CVector		dimensions;
	float		radius;
	float		weight;
	float		scale;
	// Extract the incomming message content from the Frontend and print it
	msgin.serial( id );
	msgin.serial( name );
	msgin.serial( race );
	msgin.serial( startPoint );
	nlinfo( "Received ADD_ENTITY line." );

	// Prepare to send back the message.
/*	all = true;
	CMessage msgout( CNetManager::getSIDA( "MYN" ), "ADD_ENTITY" );
	msgout.serial( all );
	msgout.serial( id );
	msgout.serial( id );
	msgout.serial( name );
	msgout.serial( race );
	msgout.serial( startPoint );
*/
	/*
	 * Send the message to all the connected Frontend. If we decide to send
	 * it back to the sender, that last argument should be 'from' inteed of '0'
	 */
/*	CNetManager::send( "MYN", msgout, 0 );

	nlinfo( "Send back ADD_ENTITY line." );

	// Send ADD_ENTITY message about all already connected client to the new one.
	all = false;
	_pmap::iterator ItPlayer;
	for (ItPlayer = playerList.begin(); ItPlayer != playerList.end(); ++ItPlayer)
	{
		CMessage msgout( CNetManager::getSIDA( "MYN" ), "ADD_ENTITY" );
		msgout.serial( all );
		msgout.serial( id );
		msgout.serial( ((*ItPlayer).second).id );
		msgout.serial( ((*ItPlayer).second).name );
		msgout.serial( ((*ItPlayer).second).race );
		msgout.serial( ((*ItPlayer).second).position );

		CNetManager::send( "MYN", msgout, from );
	}

	nlinfo( "Send ADD_ENTITY line about all already connected clients to the new one." );

	// ADD the current added entity in the player list.

	if (race!=1) 
	{
		nlinfo("race - 1");
		entityMap.insert( _emap::value_type( id,
										  _entity( id, race, name, meshName, startPoint, heading, massCenter, 
										  dimensions, radius, weight, scale ) ));
	}
	else
	{
		nlinfo("race != 1");
		playerMap.insert( _pmap::value_type( id,
										  _player( id, race, name, meshName, startPoint, heading, massCenter, 
										  dimensions, radius, weight, scale ) ));

	}

//	playerList.insert( _pmap::value_type( id,
//										  _player( id, name, race, startPoint ) ));
}
*/
/****************************************************************************
 * Function:   cbPosition
 *             Callback function called when the Mayan Service receive a
 *             "ENTITY_POS" message
 * Arguments:
 *             - msgin:  the incomming message
 *             - from:   the "sockid" of the sender (usually useless for a
 *                       CCallbackClient)
 *             - server: the CCallbackNetBase object (which really is a
 *                       CCallbackServer object, for a server)
 ****************************************************************************/
void cbPosition ( CMessage& msgin, TSockId from, CCallbackNetBase& server )
{
	uint32  id;
	CVector pos;
	float   angle;
	uint32  state;

	// Extract the incomming message content from the Frontend and print it
	msgin.serial( id );
	msgin.serial( pos );
	msgin.serial( angle );
	msgin.serial( state );
	//nlinfo( "Received ENTITY_POS line." );

	// Update position information in the player list
	_pmap::iterator ItPlayer;
	ItPlayer = playerMap.find( id );
	if ( ItPlayer == playerMap.end() )
	{
		nlinfo( "Player id %u not found !", id );
	}
	else
	{
		((*ItPlayer).second).position = pos;
		//nlinfo( "Player position updated" );
	}

	// Prepare to send back the message.
/*	CMessage msgout( CNetManager::getSIDA( "MYN" ), "ENTITY_POS" );
	msgout.serial( id );
	msgout.serial( pos );
	msgout.serial( angle );
	msgout.serial( state );
*/
	/*
	 * Send the message to all the connected Frontend. If we decide to send
	 * it back to the sender, that last argument should be 'from' inteed of '0'
	 */
//	CNetManager::send( "MYN", msgout, 0 );

	//nlinfo( "Send back ENTITY_POS line." );
}


/****************************************************************************
 * Function:   cbRemoveEntity
 *             Callback function called when the Mayan Service receive a
 *             "REMOVE_ENTITY" message
 * Arguments:
 *             - msgin:  the incomming message
 *             - from:   the "sockid" of the sender (usually useless for a
 *                       CCallbackClient)
 *             - server: the CCallbackNetBase object (which really is a
 *                       CCallbackServer object, for a server)
 ****************************************************************************/
void cbRemoveEntity ( CMessage& msgin, TSockId from, CCallbackNetBase& server )
{
	uint32 id;

	// Extract the incomming message content from the Frontend and print it
	msgin.serial( id );
	nlinfo( "Received REMOVE_ENTITY line." );

	// Prepare to send back the message.
//	CMessage msgout( CNetManager::getSIDA( "MYN" ), "REMOVE_ENTITY" );
//	msgout.serial( id );

	/*
	 * Send the message to all the connected Frontend. If we decide to send
	 * it back to the sender, that last argument should be 'from' inteed of '0'
	 */
//	CNetManager::send( "MYN", msgout, 0 );

	// Remove player form the player list.
	playerMap.erase( id );	

	nlinfo( "Send back REMOVE_ENTITY line. %d players left ...",
			playerMap.size() );
}



/////////////////////
//InOrbit functions//
/////////////////////
// we should be able to replace below 8 methods with two 
// using polymorphism???

CDynamicObject		loadDynamicObject(uint32 id)
{
	string idStr;
	char Buf[20];
	sprintf(Buf, "%d", id);
	idStr = Buf;

	CDynamicObject obj;
	obj.load( idStr );

	return obj;
}

CDynamicObject		getDynamicObject(uint32 id)
{
	CDynamicObjectMap::iterator ItDynamicObjectMap;

	ItDynamicObjectMap = DynamicObjectMap.find( id );
	
	if ( ItDynamicObjectMap != DynamicObjectMap.end() )
	{
		return (*ItDynamicObjectMap).second;
	}
	else
	{
		CDynamicObject cache = loadDynamicObject( id );
		DynamicObjectMap.insert( make_pair( id, cache ) );
		return cache;
	}
}

CFirearm		loadFirearm(uint32 typeId)
{
	string typeIdStr;
	char pBuf[20];
	sprintf(pBuf, "%d", typeId);
	typeIdStr = pBuf;

	CFirearm worldType;
	worldType.load( typeIdStr );

	return worldType;
}

CFirearm		getFirearm(uint32 typeId)
{
	CFirearmMap::iterator ItTypesMap;

	ItTypesMap = FirearmMap.find( typeId );
	
	if ( ItTypesMap != FirearmMap.end() )
	{
		return (*ItTypesMap).second;
	}
	else
	{
		CFirearm cacheType = loadFirearm( typeId );
		FirearmMap.insert( make_pair( typeId, cacheType ) );
		return cacheType;
	}
}

CBoxOfBullets		loadBoxOfBullets(uint32 typeId)
{
	string typeIdStr;
	char pBuf[20];
	sprintf(pBuf, "%d", typeId);
	typeIdStr = pBuf;

	CBoxOfBullets worldType;
	worldType.load( typeIdStr );

	return worldType;
}


CBoxOfBullets		getBoxOfBullets(uint32 typeId)
{
	CBoxOfBulletsMap::iterator ItTypesMap;

	ItTypesMap = BoxOfBulletsMap.find( typeId );
	
	if ( ItTypesMap != BoxOfBulletsMap.end() )
	{
		return (*ItTypesMap).second;
	}
	else
	{
		CBoxOfBullets cacheType = loadBoxOfBullets( typeId );
		BoxOfBulletsMap.insert( make_pair( typeId, cacheType ) );
		return cacheType;
	}
}

CNutrition		loadNutrition(uint32 typeId)
{
	string typeIdStr;
	char pBuf[20];
	sprintf(pBuf, "%d", typeId);
	typeIdStr = pBuf;

	CNutrition worldType;
	worldType.load( typeIdStr );

	return worldType;
}


CNutrition		getNutrition(uint32 typeId)
{
	CNutritionMap::iterator ItTypesMap;

	ItTypesMap = NutritionMap.find( typeId );
	
	if ( ItTypesMap != NutritionMap.end() )
	{
		return (*ItTypesMap).second;
	}
	else
	{
		CNutrition cacheType = loadNutrition( typeId );
		NutritionMap.insert( make_pair( typeId, cacheType ) );
		return cacheType;
	}
}

CFirearmType		loadFirearmType(uint32 typeId)
{
	string typeIdStr;
	char pBuf[20];
	sprintf(pBuf, "%d", typeId);
	typeIdStr = pBuf;

	CFirearmType worldType;
	worldType.load( typeIdStr );

	return worldType;
}


CFirearmType		getFirearmType(uint32 typeId)
{
	CFireTypesMap::iterator ItTypesMap;

	ItTypesMap = FireTypesMap.find( typeId );
	
	if ( ItTypesMap != FireTypesMap.end() )
	{
		return (*ItTypesMap).second;
	}
	else
	{
		CFirearmType cacheType = loadFirearmType( typeId );
		FireTypesMap.insert( make_pair( typeId, cacheType ) );
		return cacheType;
	}
}

CBulletType		loadBulletType(uint32 typeId)
{
	string typeIdStr;
	char pBuf[20];
	sprintf(pBuf, "%d", typeId);
	typeIdStr = pBuf;

	CBulletType worldType;
	worldType.load( typeIdStr );

	return worldType;
}


CBulletType		getBulletType(uint32 typeId)
{
	CBulletTypesMap::iterator ItTypesMap;

	ItTypesMap = BulletTypesMap.find( typeId );
	
	if ( ItTypesMap != BulletTypesMap.end() )
	{
		return (*ItTypesMap).second;
	}
	else
	{
		nlinfo("loading bulletType typeId: %d", typeId );
		CBulletType cacheType = loadBulletType( typeId );
		BulletTypesMap.insert( make_pair( typeId, cacheType ) );
		return cacheType;
	}
}

CBoxOfBulletsType		loadBoxOfBulletsType(uint32 typeId)
{
	string typeIdStr;
	char pBuf[20];
	sprintf(pBuf, "%d", typeId);
	typeIdStr = pBuf;

	CBoxOfBulletsType worldType;
	worldType.load( typeIdStr );

	return worldType;
}


CBoxOfBulletsType		getBoxOfBulletsType(uint32 typeId)
{
	CBoxOfBulletsTypeMap::iterator ItTypesMap;

	ItTypesMap = BoxOfBulletsTypeMap.find( typeId );
	
	if ( ItTypesMap != BoxOfBulletsTypeMap.end() )
	{
		return (*ItTypesMap).second;
	}
	else
	{
		CBoxOfBulletsType cacheType = loadBoxOfBulletsType( typeId );
		BoxOfBulletsTypeMap.insert( make_pair( typeId, cacheType ) );
		return cacheType;
	}
}

CNutritionType		loadNutritionType(uint32 typeId)
{
	string typeIdStr;
	char pBuf[20];
	sprintf(pBuf, "%d", typeId);
	typeIdStr = pBuf;

	CNutritionType worldType;
	worldType.load( typeIdStr );

	return worldType;
}


CNutritionType		getNutritionType(uint32 typeId)
{
	CNutritionTypeMap::iterator ItTypesMap;

	ItTypesMap = NutritionTypeMap.find( typeId );
	
	if ( ItTypesMap != NutritionTypeMap.end() )
	{
		return (*ItTypesMap).second;
	}
	else
	{
		CNutritionType cacheType = loadNutritionType( typeId );
		NutritionTypeMap.insert( make_pair( typeId, cacheType ) );
		return cacheType;
	}
}

/////////////////////
//InOrbit callbacks//
/////////////////////

void cbGiveDO ( CMessage& msgin, TSockId from, CCallbackNetBase& server )
{
	uint32  playerId,
			targetId,
			itemId;
	uint8	itemClassId;

	nlinfo("cbGiveDO");

	msgin.serial( playerId );
	msgin.serial( targetId );

	//find out what the player is holding
	_pmap::iterator ItPlayerMap;

	ItPlayerMap = playerMap.find( playerId );
	if(ItPlayerMap == playerMap.end())
	{
		nlinfo("player not found");
		return;
	}
	//check their inventory
	CInvMap::iterator ItInvMap;

	ItInvMap = InvMap.find( playerId );
	if(ItInvMap == InvMap.end())
	{
		nlinfo("player's inventory not found");
		return;
	}
	//get the selected item's id - 
	//should this be the left or right hand instead?
	itemId = (*ItInvMap).second.getSelected();
	itemClassId = (*ItInvMap).second.getClassId(itemId);

	//find the target
	_pmap::iterator ItPlayerMapTarget;

	ItPlayerMapTarget = playerMap.find( targetId );
	if(ItPlayerMapTarget != playerMap.end())
	{
		nlinfo("recipient is player %d",targetId);

		CInvMap::iterator ItInvMapTarget;

		ItInvMapTarget = InvMap.find( targetId );
		if(ItInvMapTarget != InvMap.end())
			(*ItInvMapTarget).second.addItem(itemId, itemClassId);

		nlinfo("added item to target player's inventory");
	}
	else
	{
		nlinfo("recipient is not a player");
		//check if target is in entities
		_emap::iterator ItEntityMapTarget;

		ItEntityMapTarget = entityMap.find( targetId );
		if(ItEntityMapTarget != entityMap.end())
		{
			nlinfo("recipient is an entity");
			//target is an entity
			//check class - send to appropriate service
			//for now just send it to the shaman service
			CMessage outmsg ( CNetManager::getSIDA( "MYN" ), "GIVE_TO_SHA" );
			outmsg.serial( targetId );
			outmsg.serial( itemId );

			CNetManager::send( "MYN", outmsg, from );
		}
		else
		{
			nlinfo("recipient not found");
			return;
		}
	}
	//remove from player's inventory
	(*ItInvMap).second.dropItem(itemId);

	bool add = 0;
	
	CMessage outmsg2 ( CNetManager::getSIDA( "MYN" ), "CH_INV" );
	outmsg2.serial( playerId );
	outmsg2.serial( itemId );
	outmsg2.serial( add );
	outmsg2.serial( itemClassId );
	CNetManager::send( "MYN", outmsg2, from );

	nlinfo("done with cbGiveDO");
}

void cbInvSelect ( CMessage& msgin, TSockId from, CCallbackNetBase& server )
{
	uint32  playerId,
			selected;

	nlinfo("cbInvSelect");

	msgin.serial( playerId );
	msgin.serial( selected );

	CInvMap::iterator ItInvMap;

	ItInvMap = InvMap.find( playerId );
	if(ItInvMap != InvMap.end())
		(*ItInvMap).second.select(selected);
	nlinfo("done with invselect");
}

void cbHoldDynamicObject ( CMessage& msgin, TSockId from, CCallbackNetBase& server )
{
	uint32  playerId,
			dynamicObjectId;
	bool leftHand;

	msgin.serial( playerId );
	msgin.serial( dynamicObjectId );
	msgin.serial( leftHand );

	CInvMap::iterator ItInvMap;

	ItInvMap = InvMap.find( playerId );
	
	if ((*ItInvMap).second.getSelected() == dynamicObjectId)
	{


		uint8 classId;

		if (dynamicObjectId>40000 && dynamicObjectId<60000) classId=2;
		if (dynamicObjectId>60000 && dynamicObjectId<90000) classId=1;
		if (dynamicObjectId>10000 && dynamicObjectId<20000) classId=3;
		if (dynamicObjectId>20000 && dynamicObjectId<40000) classId=4;

		uint32 oldHand = (*ItInvMap).second.getHand( leftHand );

		if (oldHand==dynamicObjectId)
		{
			classId=0;
			dynamicObjectId=0;
		}

		(*ItInvMap).second.hold( leftHand, dynamicObjectId );

		CDynamicObject theDynamicObject;
		CMessage outmsg;
		CFirearm theFirearm;
		CFirearmType theFirearmType;
		CBoxOfBullets boxOfBullets;
		CBoxOfBulletsType boxOfBulletsType;
		CNutrition nutrition;
		CNutritionType nutritionType;
		std::string meshName="";

		switch(classId)
		{
		case CDynamicObject::None :


			outmsg = CMessage( CNetManager::getSIDA( "MYN" ), "CH_ENTITY_HAND" );
			outmsg.serial( serviceId );
			outmsg.serial( playerId );
			outmsg.serial( leftHand );
			outmsg.serial( dynamicObjectId );
			outmsg.serial( meshName );
		
			CNetManager::send( "MYN", outmsg, from );
			break;

		case CDynamicObject::DynamicObject :

			//load object properties for client's use
			theDynamicObject = getDynamicObject( dynamicObjectId );

			outmsg = CMessage( CNetManager::getSIDA( "MYN" ), "CH_ENTITY_HAND" );
			outmsg.serial( serviceId );
			outmsg.serial( playerId );
			outmsg.serial( leftHand );
			outmsg.serial( dynamicObjectId );
			outmsg.serial( theDynamicObject.MeshName );
		
			CNetManager::send( "MYN", outmsg, from );
		break;

		case CDynamicObject::Firearm:

			theFirearm = getFirearm( dynamicObjectId );
			theFirearmType = getFirearmType( theFirearm.TypeId );

			//only what the client needs
		
			outmsg = CMessage( CNetManager::getSIDA( "MYN" ), "CH_ENTITY_HAND" ); 
			outmsg.serial( serviceId );
			outmsg.serial( playerId );
			outmsg.serial( leftHand );
			outmsg.serial( dynamicObjectId );
			outmsg.serial( theFirearm.MeshName );

			CNetManager::send( "MYN", outmsg, from );
		break;

		case CDynamicObject::BoxOfBullets:

			boxOfBullets = getBoxOfBullets( dynamicObjectId );
			boxOfBulletsType = getBoxOfBulletsType( boxOfBullets.TypeId );	

			outmsg = CMessage( CNetManager::getSIDA( "MYN" ), "CH_ENTITY_HAND" ); 
			outmsg.serial( serviceId );
			outmsg.serial( playerId );
			outmsg.serial( leftHand );
			outmsg.serial( dynamicObjectId );
			outmsg.serial( boxOfBullets.MeshName );
			CNetManager::send( "MYN", outmsg, from );
		break;

		case CDynamicObject::Nutrition:

			nutrition = getNutrition( dynamicObjectId );
			nutritionType = getNutritionType( nutrition.TypeId );	

			outmsg = CMessage( CNetManager::getSIDA( "MYN" ), "CH_ENTITY_HAND" );
			outmsg.serial( serviceId );
			outmsg.serial( playerId );
			outmsg.serial( leftHand );
			outmsg.serial( dynamicObjectId );
			outmsg.serial( nutrition.MeshName );
			CNetManager::send( "MYN", outmsg, from );
		break;
		}

	}
}


void cbPickDynamicObject ( CMessage& msgin, TSockId from, CCallbackNetBase& server )
{
	nlinfo("cbPickDynamicObject");

	uint32 thePlayerId;
	uint32 dynamicObjectId;
	CInventory	characterInventory;
	std::string	emptySound = "";

	msgin.serial(thePlayerId);
	msgin.serial(dynamicObjectId);
	
	nlinfo("player %d is picking up %d",thePlayerId,dynamicObjectId);

	CInvMap::iterator ItInvMap;

	ItInvMap = InvMap.find( thePlayerId );
	
	string strpId;
	char pBuf[20];
	sprintf(pBuf, "%d", thePlayerId);
	strpId = pBuf;
	
	characterInventory = (*ItInvMap).second;

	bool add = 1;
	uint8 classId = characterInventory.getClassId(characterInventory.getSelected());
	nlinfo("object %d is of class %d",dynamicObjectId,classId);
	if (dynamicObjectId>40000 && dynamicObjectId<60000) classId=2;
	if (dynamicObjectId>60000 && dynamicObjectId<90000) classId=1;
	if (dynamicObjectId>10000 && dynamicObjectId<20000) classId=3;
	if (dynamicObjectId>20000 && dynamicObjectId<40000) classId=4;
	nlinfo("after hack object %d is of class %d",dynamicObjectId,classId);
	CDynamicObject theDynamicObject;
	CMessage outmsg3;
	CFirearm theFirearm;
	CFirearmType theFirearmType;
	CBoxOfBullets boxOfBullets;
	CBoxOfBulletsType boxOfBulletsType;
	CNutrition nutrition;
	CNutritionType nutritionType;

	switch(classId)
	{
	case CDynamicObject::None :

		break;

	case CDynamicObject::DynamicObject :

		//load object properties for client's use
		theDynamicObject = getDynamicObject( dynamicObjectId );

		outmsg3 = CMessage( CNetManager::getSIDA( "MYN" ), "ADD_DO" );

		nlinfo("XXXname %s  XXXmeshname %s   typeid %d", theDynamicObject.Name, theDynamicObject.MeshName, theDynamicObject.TypeId);

		outmsg3.serial(thePlayerId);
		outmsg3.serial(dynamicObjectId);
		outmsg3.serial(theDynamicObject.Owner);
		outmsg3.serial(theDynamicObject.ClassId);
		outmsg3.serial(theDynamicObject.TypeId);
		outmsg3.serial(theDynamicObject.Name );
		outmsg3.serial(theDynamicObject.MeshName );
		outmsg3.serial(theDynamicObject.Position );
		outmsg3.serial(theDynamicObject.Heading);
		outmsg3.serial(theDynamicObject.TrajVector);
		outmsg3.serial(theDynamicObject.AngVelocity);
		outmsg3.serial(theDynamicObject.Radius);
		outmsg3.serial(theDynamicObject.Height);
		outmsg3.serial(theDynamicObject.Scale);
		outmsg3.serial(theDynamicObject.Weight);
		outmsg3.serial(emptySound);
		
		CNetManager::send( "MYN", outmsg3, from );
		break;

	case CDynamicObject::Firearm:

		theFirearm = getFirearm( dynamicObjectId );
		theFirearmType = getFirearmType( theFirearm.TypeId );

		//only what the client needs
		
		nlinfo("name %s  meshname %s   typeid %d   maxbullets %d", theFirearm.Name, theFirearm.MeshName, theFirearm.TypeId, theFirearmType.MaxBullets);


		outmsg3 = CMessage( CNetManager::getSIDA( "MYN" ), "ADD_FIREARM" ); 
		outmsg3.serial(thePlayerId);
		outmsg3.serial( dynamicObjectId, theFirearm.ClassId, theFirearm.Owner, theFirearm.TypeId );
		outmsg3.serial( theFirearm.Name );
		outmsg3.serial( theFirearm.MeshName );
		outmsg3.serial( theFirearm.Position, theFirearm.Heading );
		outmsg3.serial( theFirearm.TrajVector );
		outmsg3.serial( theFirearm.AngVelocity );
		outmsg3.serial( theFirearm.Radius, theFirearm.Height, theFirearm.Scale, theFirearm.Weight );
		outmsg3.serial( theFirearmType.SoundName );

		outmsg3.serial( theFirearm.Maintenance );
		outmsg3.serial( theFirearm.RoundPresent );
		outmsg3.serial( theFirearm.BulletsLeft );
		outmsg3.serial( theFirearm.Safety );
		outmsg3.serial( theFirearm.Jammed );
		outmsg3.serial( theFirearm.Chambered );
		outmsg3.serial( theFirearmType.BulletsTypeId, theFirearmType.MaxBullets  );
		outmsg3.serial( theFirearmType.FireRate1Round, theFirearmType.FireRateAuto, theFirearmType.Loudness, theFirearmType.RecoilMod );
		outmsg3.serial( theFirearmType.ParticleSystem );
		outmsg3.serial( theFirearmType.ParticleSystemScale );
		outmsg3.serial( theFirearmType.FireSound );

		CNetManager::send( "MYN", outmsg3, from );
		break;

	case CDynamicObject::BoxOfBullets:

		boxOfBullets = getBoxOfBullets( dynamicObjectId );
		boxOfBulletsType = getBoxOfBulletsType( boxOfBullets.TypeId );

		//only what the client needs
		
		nlinfo("name %s  meshname %s   typeid %d   maxbullets %d", boxOfBullets.Name, boxOfBullets.MeshName, boxOfBullets.TypeId, boxOfBulletsType.MaxBullets);

		outmsg3 = CMessage( CNetManager::getSIDA( "MYN" ), "ADD_BOXOFBULLETSG" ); 
		outmsg3.serial(thePlayerId);
		outmsg3.serial( dynamicObjectId, boxOfBullets.ClassId, boxOfBullets.Owner, boxOfBullets.TypeId );
		outmsg3.serial( boxOfBullets.Name );
		outmsg3.serial( boxOfBullets.MeshName );
		outmsg3.serial( boxOfBullets.Position, boxOfBullets.Heading );
		outmsg3.serial( boxOfBullets.TrajVector );
		outmsg3.serial( boxOfBullets.AngVelocity );
		outmsg3.serial( boxOfBullets.Radius, boxOfBullets.Height, boxOfBullets.Scale, boxOfBullets.Weight );
		outmsg3.serial( boxOfBulletsType.SoundName );

		outmsg3.serial( boxOfBulletsType.BulletsType );
		outmsg3.serial( boxOfBulletsType.MaxBullets, boxOfBullets.BulletsLeft );
	
		CNetManager::send( "MYN", outmsg3, from );
		break;

	case CDynamicObject::Nutrition:

		nutrition = getNutrition( dynamicObjectId );
		nutritionType = getNutritionType( nutrition.TypeId );

		//only what the client needs
		
		nlinfo("name %s  meshname %s   typeid %d ", nutrition.Name, nutrition.MeshName, nutrition.TypeId);

		outmsg3 = CMessage( CNetManager::getSIDA( "MYN" ), "ADD_NUTRITIONG" ); 
		outmsg3.serial(thePlayerId);
		outmsg3.serial( dynamicObjectId, nutrition.ClassId, nutrition.Owner, nutrition.TypeId );
		outmsg3.serial( nutrition.Name );
		outmsg3.serial( nutrition.MeshName );
		outmsg3.serial( nutrition.Position, nutrition.Heading );
		outmsg3.serial( nutrition.TrajVector );
		outmsg3.serial( nutrition.AngVelocity );
		outmsg3.serial( nutrition.Radius, nutrition.Height, nutrition.Scale, nutrition.Weight );
		outmsg3.serial( nutritionType.SoundName );

		outmsg3.serial( nutritionType.Carbos, nutritionType.Proteins, nutritionType.VitaminA, nutritionType.VitaminB, nutritionType.VitaminC );
		outmsg3.serial( nutritionType.Fats, nutritionType.Water );

		outmsg3.serial( nutrition.Quality, nutrition.Maintenance );

		CNetManager::send( "MYN", outmsg3, from );
		break;
	}

	nlinfo("TESTING 1..2..3.. out of loop?");

	(*ItInvMap).second.addItem(dynamicObjectId, classId);
	(*ItInvMap).second.save(strpId);

	CMessage outmsg ( CNetManager::getSIDA( "MYN" ), "CH_INV" );
	outmsg.serial( thePlayerId );
	outmsg.serial( dynamicObjectId );
	outmsg.serial( add );
	outmsg.serial( classId );
	CNetManager::send( "MYN", outmsg, from );

	CMessage remmsg ( CNetManager::getSIDA( "MYN" ), "REMOVE_ENTITY" );
	//remmsg.serial ( serviceId, dynamicObjectId);
	remmsg.serial ( dynamicObjectId);
	CNetManager::send( "MYN", remmsg );

	_emap::iterator ite;
	ite = entityMap.find( dynamicObjectId );
	if( ite != entityMap.end() )
		entityMap.erase( (*ite).first );
};

void cbDropDynamicObject ( CMessage& msgin, TSockId from, CCallbackNetBase& server )
{
	uint32 thePlayerId, noId = 0;
	uint32 dynamicObjectId;
	CInventory	characterInventory;

	bool	all = true;

	bool	reallyInInv = false;

	msgin.serial(thePlayerId);
	msgin.serial(dynamicObjectId);

	nlinfo("dropping dynobj # %d", dynamicObjectId);

	//find player's position
	_pmap::iterator	ItPlayerMap;
	ItPlayerMap = playerMap.find( thePlayerId );
	if( ItPlayerMap == playerMap.end() )
		return;
	CVector playerPosition = (*ItPlayerMap).second.position;
	playerPosition += CVector(1.0f,1.0f,1.0f);
	nlinfo("dropping at player position %f , %f",playerPosition.x, playerPosition.y);

	//todo Check to see if the player really has it in his inventory
	CInvMap::iterator ItInvMap;


	ItInvMap = InvMap.find( thePlayerId );
	if ( ItInvMap != InvMap.end() )
	{
		characterInventory = (*ItInvMap).second;
	
		if(characterInventory.getClassId(dynamicObjectId) > 0)
		{
			nlinfo("removing from inventory");
			reallyInInv = true;
			(*ItInvMap).second.dropItem(dynamicObjectId);
			
			string strpId;
			char pBuf[20];
			sprintf(pBuf, "%d", thePlayerId);
			strpId = pBuf;

			(*ItInvMap).second.save(strpId);
		}
	}
	if(reallyInInv)
	{
	uint8 classId = characterInventory.getClassId(characterInventory.getSelected());

	//if (dynamicObjectId>40000 && dynamicObjectId<60000) classId=2;
	//if (dynamicObjectId>60000 && dynamicObjectId<90000) classId=1;
	//if (dynamicObjectId>10000 && dynamicObjectId<20000) classId=3;
	//if (dynamicObjectId>20000 && dynamicObjectId<40000) classId=4;

	nlinfo("classId: %d", classId);

	CDynamicObject theDynamicObject;
	CFirearm theFirearm;
	CBoxOfBullets boxOfBullets;
	CNutrition nutrition;
	CMessage msgout;
	
	switch(classId)
	{
	case CDynamicObject::None :

		nlinfo("none");
		break;

	case CDynamicObject::DynamicObject :

		nlinfo("DO");
		//load object properties for client's use
		theDynamicObject = getDynamicObject( dynamicObjectId );
		theDynamicObject.Position = playerPosition;
		nlinfo("name %s  meshname %s   typeid %d", theDynamicObject.Name, theDynamicObject.MeshName, theDynamicObject.TypeId);

		msgout = CMessage( CNetManager::getSIDA( "MYN" ), "DROP_ENTITY");
	
		msgout.serial(all);
		msgout.serial(serviceId);
		msgout.serial(noId);
		msgout.serial(dynamicObjectId);
		msgout.serial(theDynamicObject.ClassId);
		msgout.serial(theDynamicObject.Name);
		msgout.serial(theDynamicObject.MeshName);
		msgout.serial(theDynamicObject.Position);
		msgout.serial(theDynamicObject.Heading);
		msgout.serial(theDynamicObject.MassCenter);
		msgout.serial(theDynamicObject.Dimensions);
		msgout.serial(theDynamicObject.Radius);
		msgout.serial(theDynamicObject.Weight);
		msgout.serial(theDynamicObject.Scale);
		CNetManager::send( "MYN", msgout, 0);
		
		break;

	case CDynamicObject::Firearm :

		nlinfo("FA");
		theFirearm = getFirearm( dynamicObjectId );
		theFirearm.Position = playerPosition;

		msgout = CMessage( CNetManager::getSIDA( "MYN" ), "DROP_ENTITY");
	
		msgout.serial(all);
		msgout.serial(serviceId);
		msgout.serial(noId);
		msgout.serial(dynamicObjectId);
		msgout.serial(theFirearm.ClassId);
		msgout.serial(theFirearm.Name);
		msgout.serial(theFirearm.MeshName);
		msgout.serial(theFirearm.Position);
		msgout.serial(theFirearm.Heading);
		msgout.serial(theFirearm.MassCenter);
		msgout.serial(theFirearm.Dimensions);
		msgout.serial(theFirearm.Radius);
		msgout.serial(theFirearm.Weight);
		msgout.serial(theFirearm.Scale);
		CNetManager::send( "MYN", msgout, 0);

		nlinfo("firearm classId: %d", classId);

		break;

	case CDynamicObject::BoxOfBullets:

		nlinfo("BOB");
		boxOfBullets = getBoxOfBullets( dynamicObjectId );
		boxOfBullets.Position = playerPosition;

		msgout = CMessage( CNetManager::getSIDA( "MYN" ), "DROP_ENTITY");
	
		msgout.serial(all);
		msgout.serial(serviceId);
		msgout.serial(noId);
		msgout.serial(dynamicObjectId);
		msgout.serial(boxOfBullets.ClassId);
		msgout.serial(boxOfBullets.Name);
		msgout.serial(boxOfBullets.MeshName);
		msgout.serial(boxOfBullets.Position);
		msgout.serial(boxOfBullets.Heading);
		msgout.serial(boxOfBullets.MassCenter);
		msgout.serial(boxOfBullets.Dimensions);
		msgout.serial(boxOfBullets.Radius);
		msgout.serial(boxOfBullets.Weight);
		msgout.serial(boxOfBullets.Scale);
		CNetManager::send( "MYN", msgout, 0);
		break;

	case CDynamicObject::Nutrition:

		nlinfo("NUT");
		nutrition = getNutrition( dynamicObjectId );
		nutrition.Position = playerPosition;

		msgout = CMessage( CNetManager::getSIDA( "MYN" ), "DROP_ENTITY");
	
		msgout.serial(all);
		msgout.serial(serviceId);
		msgout.serial(noId);
		msgout.serial(dynamicObjectId);
		msgout.serial(nutrition.ClassId);
		msgout.serial(nutrition.Name);
		msgout.serial(nutrition.MeshName);
		msgout.serial(nutrition.Position);
		msgout.serial(nutrition.Heading);
		msgout.serial(nutrition.MassCenter);
		msgout.serial(nutrition.Dimensions);
		msgout.serial(nutrition.Radius);
		msgout.serial(nutrition.Weight);
		msgout.serial(nutrition.Scale);
		CNetManager::send( "MYN", msgout, 0);
		break;
	}
	//}
	//if(reallyInInv)
	//{
		
		bool add = 0;
	
		CMessage outmsg ( CNetManager::getSIDA( "MYN" ), "CH_INV" );
		outmsg.serial( thePlayerId );
		outmsg.serial( dynamicObjectId );
		outmsg.serial( add );
		outmsg.serial( classId );
		CNetManager::send( "MYN", outmsg, from );
		nlinfo("ifreallyininv classId: %d", classId);
	}
}

void cbUseDynamicObject ( CMessage& msgin, TSockId from, CCallbackNetBase& server )
{
	uint32 thePlayerId;
	uint32 dynamicObjectId;

	msgin.serial(thePlayerId);
	msgin.serial(dynamicObjectId);
	
	CInventory	characterInventory;
	CInvMap::iterator ItInvMap;
	ItInvMap = InvMap.find( thePlayerId );
	
	characterInventory = (*ItInvMap).second;

	bool add = 1;
	uint8 classId = characterInventory.getClassId(characterInventory.getSelected());

	CMessage outmsg;
	CBoxOfBullets boxOfBullets;
	CBoxOfBulletsType boxOfBulletsType;

	map<uint32,uint8>::iterator	it;

	switch(classId)
	{
	case CDynamicObject::None :

		break;

	case CDynamicObject::DynamicObject :

		break;

	case CDynamicObject::Firearm:

		break;

	case CDynamicObject::BoxOfBullets:

		boxOfBullets = getBoxOfBullets( dynamicObjectId );
		boxOfBulletsType = getBoxOfBulletsType( boxOfBullets.TypeId );

		for(it = characterInventory._Inventory.begin(); it!=characterInventory._Inventory.end(); it++)
		{
			uint32 dynamicObjectId = (*it).first;

			uint8 aClassId = characterInventory.getClassId(dynamicObjectId);

			CFirearm theFirearm;
			CFirearmType theFirearmType;

			switch(aClassId)
			{
			case CDynamicObject::None :
				break;
				
			case CDynamicObject::Firearm:

				theFirearm = getFirearm( dynamicObjectId );
				theFirearmType = getFirearmType( theFirearm.TypeId );

				if (boxOfBullets.BulletsLeft>0 && theFirearmType.BulletsTypeId==boxOfBulletsType.BulletsType)
				{
					uint8 bulletsNeeded = theFirearmType.MaxBullets-theFirearm.BulletsLeft;

					// if I need more bullets than are in the box
					if (bulletsNeeded >= boxOfBullets.BulletsLeft)
					{
						theFirearm.BulletsLeft=theFirearm.BulletsLeft+boxOfBullets.BulletsLeft;
						boxOfBullets.BulletsLeft=0;
					}
					else
					{
						theFirearm.BulletsLeft=theFirearm.BulletsLeft+bulletsNeeded;
						boxOfBullets.BulletsLeft = boxOfBullets.BulletsLeft-bulletsNeeded;
					}

					theFirearm.save();
					boxOfBullets.save();
				}

				break;

			case CDynamicObject::BoxOfBullets:
	
				break;

			case CDynamicObject::Nutrition:

				break;
			}

		}
		break;

	case CDynamicObject::Nutrition:

		break;
	}
};

void loadPlayer(uint32 playerId)
{
	nlinfo("loadPlayer %d:",playerId);

	
	_pmap::iterator	dit;
	for(dit = playerMap.begin(); dit!=playerMap.end(); dit++)
	{
		nlinfo("playermap: %d",(*dit).first);
	}

	_pmap::iterator ItPlayerMap;

	ItPlayerMap = playerMap.find(playerId); 

	nlinfo("find player");
	if (ItPlayerMap == playerMap.end())
	{
		nlinfo("didnt find player- loading");
		CStatsMap::iterator ItStatsMap;
		ItStatsMap = StatsMap.find( playerId );
		nlinfo("find stats");
		if(ItStatsMap != StatsMap.end() ) 
		{
			nlinfo("found stats");
			uint8	classId = (uint8) (*ItStatsMap).second.geti16()[119];
			nlinfo(".");
			string	name = (*ItStatsMap).second.getstr()[0];
			nlinfo("2");
			string	meshName = (*ItStatsMap).second.getstr()[8];
			nlinfo("3");
			CVector	position = (*ItStatsMap).second.getvec()[0];
			nlinfo("4");
			CVector	heading = (*ItStatsMap).second.getvec()[1];
			nlinfo("5");
			CVector	massCenter = (*ItStatsMap).second.getvec()[2];
			nlinfo("6");
			CVector	dimensions = (*ItStatsMap).second.getvec()[3];
			nlinfo("7");
			float radius = (*ItStatsMap).second.getfloat()[0];
			nlinfo("8");
			float weight = (*ItStatsMap).second.getfloat()[1];
			nlinfo("9");
			float scale = (*ItStatsMap).second.getfloat()[2];

			if(meshName == "") meshName = "shaman.shape";

			_player	newPlayer = _player( playerId, classId, name, meshName, position, heading, massCenter, dimensions,
										radius, weight, scale);
			nlinfo("player constructed");
			newPlayer.possessed = true;
/*
			// create a move primitive associated to the entity
			newPlayer.movePrimitive = MoveContainer->addCollisionablePrimitive(0, 1);
			// it's a cylinder
			newPlayer.movePrimitive->setPrimitiveType(UMovePrimitive::_2DOrientedCylinder);
			// the entity should slide against obstacles
			newPlayer.movePrimitive->setReactionType(UMovePrimitive::Slide);
			// do not generate event if there is a collision
			newPlayer.movePrimitive->setTriggerType(UMovePrimitive::NotATrigger);
			// which entity should collide against me
			newPlayer.movePrimitive->setCollisionMask(OtherCollisionBit+SnowballCollisionBit+StaticCollisionBit);
			// the self collision bit
			newPlayer.movePrimitive->setOcclusionMask(SelfCollisionBit);
			// the self is an obstacle
			newPlayer.movePrimitive->setObstacle(true);
			// the size of the cylinder
			newPlayer.movePrimitive->setRadius(0.8f);
			newPlayer.movePrimitive->setHeight(4.8f);
			// only use one world image, so use insert in world image 0
			newPlayer.movePrimitive->insertInWorldImage(0);
			// retreive the start position of the entity
			newPlayer.movePrimitive->setGlobalPosition(CVectorD(position.x, position.y, position.z), 0);

			nlinfo("pacs stuff done");
*/
			newPlayer.speed = 10.0f;
			newPlayer.angle = 0.0f;
			newPlayer.state = 0;

			newPlayer.name = name;
			newPlayer.classId = classId;
			newPlayer.position = position;
			newPlayer.heading = heading;	

			playerMap.insert( _pmap::value_type( playerId, newPlayer ));
			nlinfo("player inserted into map");

			bool	all = true;
			uint32	noId = 0;
			classId = 1;

			/*
			CMessage msgout( CNetManager::getSIDA( "MYN" ), "ADD_ENTITY");
	
			msgout.serial(all);
			msgout.serial(serviceId);
			msgout.serial(noId);
			msgout.serial(playerId);
			msgout.serial(classId);
			msgout.serial(name);
			msgout.serial(meshName);
			msgout.serial(position);
			msgout.serial(heading);
			msgout.serial(massCenter);
			msgout.serial(dimensions);
			msgout.serial(radius);
			msgout.serial(weight);
			msgout.serial(scale);

			CNetManager::send( "MYN", msgout );
			nlinfo("returning new player");
			*/
		}
		else
		{
			nlinfo("No Stats for Player id=%d so it is not added to the scene.", playerId );
		}
	}
	else
	{
		nlinfo("possessing player");
		(*ItPlayerMap).second.possessed=true;
	}
}
void cbEntityMissing ( CMessage& msgin, TSockId from, CCallbackNetBase& server )
{
	uint32 missingId, requestorId;
	bool all = false;

	msgin.serial( missingId );
	msgin.serial( requestorId );

	nlinfo(" #%d is missing #%d",requestorId,missingId);

	_pmap::iterator ItPlayer;

	ItPlayer = playerMap.find(missingId); 

	nlinfo("finding missing player");
	if (ItPlayer != playerMap.end())
	{
		nlinfo("sending player %d",(*ItPlayer).second.id);
		nlinfo("to %d",requestorId);
		CMessage msgout( CNetManager::getSIDA( "MYN" ), "ADD_ENTITY" );
		
		msgout.serial(all);
		msgout.serial(serviceId);
		msgout.serial(requestorId);
		msgout.serial((*ItPlayer).second.id);
		msgout.serial((*ItPlayer).second.classId);
		msgout.serial((*ItPlayer).second.name);
		msgout.serial((*ItPlayer).second.meshName);
		msgout.serial((*ItPlayer).second.position);
		msgout.serial((*ItPlayer).second.heading);
		msgout.serial((*ItPlayer).second.massCenter);
		msgout.serial((*ItPlayer).second.dimensions);
		msgout.serial((*ItPlayer).second.radius);
		msgout.serial((*ItPlayer).second.weight);
		msgout.serial((*ItPlayer).second.scale);
		CNetManager::send( "MYN", msgout, from );
	}
	else
		nlinfo("missing player not in map");
}


void cbPossess ( CMessage& msgin, TSockId from, CCallbackNetBase& server )
{
	nlinfo("cbPossess");
	uint8	skullClass = 1;
	uint32 thePlayerId, skullId = 60001;
	msgin.serial(thePlayerId);
	std::string	emptySound;

	string strId;
	char Buf[20];
	sprintf(Buf, "%d", thePlayerId);
	strId = Buf;

/*	if (dynamicObjectId>40000 && dynamicObjectId<60000) classId=2;
	if (dynamicObjectId>60000 && dynamicObjectId<90000) classId=1;
	if (dynamicObjectId>10000 && dynamicObjectId<20000) classId=3;
	if (dynamicObjectId>20000 && dynamicObjectId<40000) classId=4;*/

	nlinfo("bout to load inv");
	CInventory characterInventory = CInventory();
	characterInventory.load( strId );
	characterInventory.addItem( skullId, skullClass );
	nlinfo("inv loaded");
	map<uint32,uint8>::iterator	it;
	for(it = characterInventory._Inventory.begin(); it!=characterInventory._Inventory.end(); it++)
	{
		uint32 dynamicObjectId = (*it).first;

		uint8 classId = characterInventory.getClassId(dynamicObjectId);

		CDynamicObject theDynamicObject;
		CMessage outmsg3;
		CFirearm theFirearm;
		CFirearmType theFirearmType;
		CBoxOfBullets boxOfBullets;
		CBoxOfBulletsType boxOfBulletsType;
		CNutrition nutrition;
		CNutritionType nutritionType;

		switch(classId)
		{
		case CDynamicObject::None :
			nlinfo("none");
			break;

		case CDynamicObject::DynamicObject :

			//load object properties for client's use
			theDynamicObject = getDynamicObject( dynamicObjectId );

			outmsg3 = CMessage( CNetManager::getSIDA( "MYN" ), "ADD_DO" );

			nlinfo("DO.name %s", theDynamicObject.Name);
			nlinfo("DO.meshname %s", theDynamicObject.MeshName);
			nlinfo("DO.typeid %d", theDynamicObject.TypeId);

			outmsg3.serial(thePlayerId);
			outmsg3.serial(dynamicObjectId);
			outmsg3.serial(theDynamicObject.ClassId);
			outmsg3.serial(theDynamicObject.Owner);
			outmsg3.serial(theDynamicObject.TypeId);
			outmsg3.serial(theDynamicObject.Name );
			outmsg3.serial(theDynamicObject.MeshName );
			outmsg3.serial(theDynamicObject.Position );
			outmsg3.serial(theDynamicObject.Heading);
			outmsg3.serial(theDynamicObject.TrajVector);
			outmsg3.serial(theDynamicObject.AngVelocity);
			outmsg3.serial(theDynamicObject.Radius);
			outmsg3.serial(theDynamicObject.Height);
			outmsg3.serial(theDynamicObject.Scale);
			outmsg3.serial(theDynamicObject.Weight);
			outmsg3.serial(emptySound);
			CNetManager::send( "MYN", outmsg3, from );
			break;

		case CDynamicObject::Firearm:

			theFirearm = getFirearm( dynamicObjectId );
			theFirearmType = getFirearmType( theFirearm.TypeId );

			//only what the client needs
		
			nlinfo("name %s  meshname %s   typeid %d   maxbullets %d", theFirearm.Name, theFirearm.MeshName, theFirearm.TypeId, theFirearmType.MaxBullets);

			outmsg3 = CMessage( CNetManager::getSIDA( "MYN" ), "ADD_FIREARM" ); 
			outmsg3.serial(thePlayerId);
			outmsg3.serial( dynamicObjectId, theFirearm.ClassId, theFirearm.Owner, theFirearm.TypeId );
			outmsg3.serial( theFirearm.Name );
			outmsg3.serial( theFirearm.MeshName );
			outmsg3.serial( theFirearm.Position, theFirearm.Heading );
			outmsg3.serial( theFirearm.TrajVector );
			outmsg3.serial( theFirearm.AngVelocity );
			outmsg3.serial( theFirearm.Radius, theFirearm.Height, theFirearm.Scale, theFirearm.Weight );
			outmsg3.serial( theFirearmType.SoundName );

			outmsg3.serial( theFirearm.Maintenance );
			outmsg3.serial( theFirearm.RoundPresent );
			outmsg3.serial( theFirearm.BulletsLeft );
			outmsg3.serial( theFirearm.Safety );
			outmsg3.serial( theFirearm.Jammed );
			outmsg3.serial( theFirearm.Chambered );
			outmsg3.serial( theFirearmType.BulletsTypeId, theFirearmType.MaxBullets  );
			outmsg3.serial( theFirearmType.FireRate1Round, theFirearmType.FireRateAuto, theFirearmType.Loudness, theFirearmType.RecoilMod );
			outmsg3.serial( theFirearmType.ParticleSystem );
			outmsg3.serial( theFirearmType.ParticleSystemScale );
			outmsg3.serial( theFirearmType.FireSound );

			CNetManager::send( "MYN", outmsg3, from );
			break;

		case CDynamicObject::BoxOfBullets:

			boxOfBullets = getBoxOfBullets( dynamicObjectId );
			boxOfBulletsType = getBoxOfBulletsType( boxOfBullets.TypeId );

			//only what the client needs
		
			nlinfo("name %s  meshname %s   typeid %d   maxbullets %d", boxOfBullets.Name, boxOfBullets.MeshName, boxOfBullets.TypeId, boxOfBulletsType.MaxBullets);

			outmsg3 = CMessage( CNetManager::getSIDA( "MYN" ), "ADD_BULLETBOX" ); 
			outmsg3.serial(thePlayerId);
			outmsg3.serial( dynamicObjectId, boxOfBullets.ClassId, boxOfBullets.Owner, boxOfBullets.TypeId );
			outmsg3.serial( boxOfBullets.Name );
			outmsg3.serial( boxOfBullets.MeshName );
			outmsg3.serial( boxOfBullets.Position, boxOfBullets.Heading );
			outmsg3.serial( boxOfBullets.TrajVector );
			outmsg3.serial( boxOfBullets.AngVelocity );
			outmsg3.serial( boxOfBullets.Radius, boxOfBullets.Height, boxOfBullets.Scale, boxOfBullets.Weight );
			outmsg3.serial( boxOfBulletsType.SoundName );

			outmsg3.serial( boxOfBulletsType.BulletsType );
			outmsg3.serial( boxOfBulletsType.MaxBullets, boxOfBullets.BulletsLeft );
	
			CNetManager::send( "MYN", outmsg3, from );
			break;

		case CDynamicObject::Nutrition:

			nutrition = getNutrition( dynamicObjectId );
			nutritionType = getNutritionType( nutrition.TypeId );

			//only what the client needs
		
			nlinfo("name %s  meshname %s   typeid %d ", nutrition.Name, nutrition.MeshName, nutrition.TypeId);

			outmsg3 = CMessage( CNetManager::getSIDA( "MYN" ), "ADD_NUTRITION" ); 
			outmsg3.serial(thePlayerId);
			outmsg3.serial( dynamicObjectId, nutrition.ClassId, nutrition.Owner, nutrition.TypeId );
			outmsg3.serial( nutrition.Name );
			outmsg3.serial( nutrition.MeshName );
			outmsg3.serial( nutrition.Position, nutrition.Heading );
			outmsg3.serial( nutrition.TrajVector );
			outmsg3.serial( nutrition.AngVelocity );
			outmsg3.serial( nutrition.Radius, nutrition.Height, nutrition.Scale, nutrition.Weight );
			outmsg3.serial( nutritionType.SoundName );

			outmsg3.serial( nutritionType.Carbos, nutritionType.Proteins, nutritionType.VitaminA, nutritionType.VitaminB, nutritionType.VitaminC );
			outmsg3.serial( nutritionType.Fats, nutritionType.Water );

			outmsg3.serial( nutrition.Quality, nutrition.Maintenance );

			CNetManager::send( "MYN", outmsg3, from );
			break;
		}
	}
	nlinfo("sent objects");

	// Inform the client of character statistics
	CMessage outmsg2 ( CNetManager::getSIDA( "MYN" ), "INIT_STATS" );
	CStats characterStats = CStats();
	nlinfo("about to load stats of %s",strId);
	characterStats.load( strId );
	nlinfo("stats loaded");
	outmsg2.serial( thePlayerId );
	outmsg2.serial( characterStats );
	nlinfo("message formed");
	CNetManager::send( "MYN", outmsg2, from );
	nlinfo("message sent");
	StatsMap.insert( make_pair( thePlayerId, characterStats ) );

	nlinfo("init_stats sent");

	// Inform the client of character skills
	CSkills characterSkills;
	characterSkills.load( strId );
	CMessage skillsoutmsg ( CNetManager::getSIDA( "MYN" ), "INIT_SKILLS" );
	skillsoutmsg.serial( thePlayerId );
	skillsoutmsg.serial( characterSkills );
	CNetManager::send( "MYN", skillsoutmsg, from );
	SkillsMap.insert( make_pair( thePlayerId, characterSkills ) );

	nlinfo("init_skills sent");

	// Inform the client of character inventory
	CMessage outmsg ( CNetManager::getSIDA( "MYN" ), "INIT_INV" );
	outmsg.serial( thePlayerId );
	outmsg.serial( characterInventory );
	CNetManager::send( "MYN", outmsg, from );
	InvMap.insert( make_pair( thePlayerId, characterInventory ) );

	nlinfo("init_inv sent");

	loadPlayer(thePlayerId);

	nlinfo("player loaded");

	bool all=false;

	// send all entites except players (dynamic objects and mammoths)
	_emap::iterator ItEntity;
	for (ItEntity = entityMap.begin(); ItEntity != entityMap.end(); ++ItEntity)
	{
		nlinfo("sending entity %d",(*ItEntity).second.id);
		nlinfo("to %d",thePlayerId);
		nlinfo("classId: %d",(*ItEntity).second.classId);
		CMessage msgout( CNetManager::getSIDA( "MYN" ), "ADD_ENTITY" );
		msgout.serial(all);
		msgout.serial(serviceId);
		msgout.serial(thePlayerId);
		msgout.serial((*ItEntity).second.id);
		msgout.serial((*ItEntity).second.classId);
		msgout.serial((*ItEntity).second.name);
		msgout.serial((*ItEntity).second.meshName);
		msgout.serial((*ItEntity).second.position);
		msgout.serial((*ItEntity).second.heading);
		msgout.serial((*ItEntity).second.massCenter);
		msgout.serial((*ItEntity).second.dimensions);
		msgout.serial((*ItEntity).second.radius);
		msgout.serial((*ItEntity).second.weight);
		msgout.serial((*ItEntity).second.scale);
		CNetManager::send( "MYN", msgout, from );
	}
	nlinfo("sent non-players");

	// send all active players
	_pmap::iterator ItPlayer;
	for (ItPlayer = playerMap.begin(); ItPlayer != playerMap.end(); ++ItPlayer)
	{
		if ((*ItPlayer).second.id!=thePlayerId)
		{
		nlinfo("sending player %d",(*ItPlayer).second.id);
		nlinfo("to %d",thePlayerId);	CMessage msgout( CNetManager::getSIDA( "MYN" ), "ADD_ENTITY" );
			msgout.serial(all);
			msgout.serial(serviceId);
			msgout.serial(thePlayerId);
			msgout.serial((*ItPlayer).second.id);
			nlinfo("classId: %d",(*ItPlayer).second.classId);
			msgout.serial((*ItPlayer).second.classId);
			msgout.serial((*ItPlayer).second.name);
			msgout.serial((*ItPlayer).second.meshName);
			msgout.serial((*ItPlayer).second.position);
			msgout.serial((*ItPlayer).second.heading);
			msgout.serial((*ItPlayer).second.massCenter);
			msgout.serial((*ItPlayer).second.dimensions);
			msgout.serial((*ItPlayer).second.radius);
			msgout.serial((*ItPlayer).second.weight);
			msgout.serial((*ItPlayer).second.scale);
			CNetManager::send( "MYN", msgout, from );
		}
	}
	nlinfo("sent other players to the new player");
	
	nlinfo("cbPossessPlayer done");
}

/****************************************************************************
 * Function:   cbAddEntity
 *             Callback function called when the Position Service receive a
 *             "ADD_ENTITY" message
 * Arguments:
 *             - msgin:  the incomming message
 *             - from:   the "sockid" of the sender (usually useless for a
 *                       CCallbackClient)
 *             - server: the CCallbackNetBase object (which really is a
 *                       CCallbackServer object, for a server)
 ****************************************************************************/
void cbAddEntity ( CMessage& msgin, TSockId from, CCallbackNetBase& server )
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

	nlinfo( "Received ADD_ENTITY line." );

	// ADD the current added entity in the player list.
	if (classId!=1) 
	{
		nlinfo("classId - 1");
		entityMap.insert( _emap::value_type( id,
										  _entity( id, classId, name, meshName, position, heading, massCenter, 
										  dimensions, radius, weight, scale ) ));
	}
	else
	{
		nlinfo("classId != 1");
		playerMap.insert( _pmap::value_type( id,
										  _player( id, classId, name, meshName, position, heading, massCenter, 
										  dimensions, radius, weight, scale ) ));

	}
}

/****************************************************************************
 * CallbackArray
 *
 * It define the functions to call when receiving a specific message
 ****************************************************************************/
TCallbackItem CallbackArray[] =
{
	{ "ADD_ENTITY",    cbAddEntity    },
	{ "POSSESS",	   cbPossess	  },
	{ "ENTITY_POS",    cbPosition     },
	{ "REMOVE_ENTITY", cbRemoveEntity },
	{ "ENTITY_MISSING", cbEntityMissing },
	{ "DROP_DO", cbDropDynamicObject },
	{ "PICK_DO", cbPickDynamicObject },
//	{ "FIRE1",   cbFire1 },
//	{ "HIT",	  cbProjectileHit },
	{ "INV_SELECT",	cbInvSelect },
	{ "HOLD_DO", cbHoldDynamicObject },
	{ "GIVE_DO",		cbGiveDO	 }
};


/****************************************************************************
 * Function:   SendHITMsg
 *             Send HIT message to all clients
 * 
 * Arguments:
 *             - snowball:  snowball id
 *             - victim:    player touched by the snowball
 *             - direct:    define if the hit is direct or by the explosion
 *                          area
 ****************************************************************************/
void SendHITMsg ( uint32 snowball, uint32 victim, bool direct )
{
	CMessage msgout( CNetManager::getSIDA( "MYN" ), "HIT" );

	msgout.serial( snowball );
	msgout.serial( victim );
	msgout.serial( direct );

	CNetManager::send( "MYN", msgout, 0 );
}


/****************************************************************************
 * CMayanService
 ****************************************************************************/
class CMayanService : public IService
{
public:

	// Initialisation
	void init()
	{
		DebugLog->addNegativeFilter ("NETL");
	}

	// Update fonction, called at every frames
	bool update()
	{

		return true;
	}

};


/****************************************************************************
 * SNOWBALLS Mayan SERVICE MAIN Function
 *
 * This call create a main function for the Mayan service:
 *
 *    - based on the base service class "IService", no need to inherit from it
 *    - having the short name "Mayan"
 *    - having the long name "Mayan_service"
 *    - listening on an automatically allocated port (0) by the naming service
 *    - and callback actions set to "CallbackArray"
 *
 ****************************************************************************/
NLNET_SERVICE_MAIN( CMayanService,
					"MYN",
					"mayan_service",
					0,
					CallbackArray )


/* end of file */

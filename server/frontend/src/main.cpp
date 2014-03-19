/*
 * This file contain the Snowballs Frontend Service.
 *
 * $Id: main.cpp,v 1.1 2002/03/20 18:10:21 robofly Exp $
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

#include <nel/misc/vector.h>

// We're using the NeL Service framework and layer 4.
#include <nel/net/service.h>
#include <nel/net/net_manager.h>
#include <nel/net/login_server.h>

#include "stats.h"
#include "inventory.h"
#include "skills.h"

#include <map>
#include <utility>

using namespace NLMISC;
using namespace NLNET;
using namespace std;


//TSockId clientfrom;

/*
 * Keep a list of the players connected to that Frontend. Only map Id
 * to a Connection
 */

struct _player
{
	_player(uint32 Id, TSockId Con) : id(Id), con(Con) { }
	uint32   id;
	TSockId  con;
};

typedef map<uint32, _player> _pmap;

_pmap localPlayers;

//for reverse lookups
struct _playerSocket
{
	_playerSocket( TSockId Con, uint32 Id) :  con(Con), id(Id){ }
	TSockId  con;
	uint32   id;
};

typedef map<TSockId, _playerSocket> _psmap;

_psmap localPlayersSocket;

/****************************************************************************
 * reverse lookup for trusted playerId
 ****************************************************************************/
uint32 lookUpPlayerIdFromSock (TSockId sockId)
{
//	uint32 id;
	
	_psmap::iterator ItPlayerSock;
	ItPlayerSock = localPlayersSocket.find(sockId);
	if ( ItPlayerSock == localPlayersSocket.end() )
	{
		nlinfo( "player socket id not found !");
		return 0;
	}
	else
	{
		nlinfo("player socket found as %d",(*ItPlayerSock).second.id);
		return (*ItPlayerSock).second.id;
	}
}

/////////////////////
//InOrbit callbacks//
/////////////////////
/****************************************************************************
 * cbInitStats
 *
 * Receive an INIT_STATS messages from the Player Service to send it to all
 * the clients.
 ****************************************************************************/
void cbInitStats ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32  to;
	CStats stats;
//	CInventory inventory;

	// Input: process the reply of the position service
	msgin.serial( to );

	msgin.serial( stats );

	//msgin.serial( inventory );

	_pmap::iterator ItPlayer;
	ItPlayer = localPlayers.find(to);
	if ( ItPlayer == localPlayers.end() )
	{
		nlinfo( "New player id %u not found !", to );
	}
	else
	{
		nlinfo("found player");
		TSockId conToClient = ((*ItPlayer).second).con;

		CMessage msgout( CNetManager::getSIDA( "FS" ), "INIT_STATS" );
		msgout.serial( stats );		
		CNetManager::send( "FS", msgout, conToClient );
		nlinfo("sent stats");



		nlinfo( "Sent INIT_STATS to the new client.");
	}
	
	// Output: prepare the reply to the clients

}
/****************************************************************************
 * cdInitSkills
 *
 * Receive an INIT_SKILLS messages from the Player Service to send it to all
 * the clients.
 ****************************************************************************/
void cbInitSkills ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32  to;
	CSkills skills;


	// Input: process the reply of the position service
	msgin.serial( to );
	msgin.serial( skills );

	_pmap::iterator ItPlayer;
	ItPlayer = localPlayers.find(to);
	if ( ItPlayer == localPlayers.end() )
	{
		nlinfo( "New player id %u not found !", to );
	}
	else
	{
		nlinfo("found player");
		
		TSockId conToClient = ((*ItPlayer).second).con;

		CMessage msgout( CNetManager::getSIDA( "FS" ), "INIT_SKILLS" );
		
		msgout.serial( skills );		
		
		CNetManager::send( "FS", msgout, conToClient );
		
		nlinfo( "Sent INIT_SKILLS to the new client.");
	}

}




/****************************************************************************
 * cbUpdateStats
 *
 * Receive an UPDATE_STATS messages from the Player Service to send it to all
 * the clients.
 ****************************************************************************/
void cbUpdateStats ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	
	uint32  to;
	uint8 statId;
	sint16 value;

	msgin.serial( to );
	msgin.serial( statId );
	msgin.serial( value );

	_pmap::iterator ItPlayer;
	ItPlayer = localPlayers.find(to);
	if ( ItPlayer == localPlayers.end() )
	{
		nlinfo( "player id %u not found !", to );
	}
	else
	{
		TSockId conToClient = ((*ItPlayer).second).con;

		CMessage msgout( CNetManager::getSIDA( "FS" ), "CH_STATS" );
		msgout.serial( statId );		
		msgout.serial( value );
		CNetManager::send( "FS", msgout, conToClient );



		nlinfo( "Sent CHSTAT about statistics to the client.");
	}

	
}	

/****************************************************************************
 * cbInitInv
 *
 * Receive an INIT_INV messages from the Player Service to send it to all
 * the clients.
 ****************************************************************************/
void cbInitInv ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32  to;
	CInventory inventory;
	
	msgin.serial( to );
	msgin.serial( inventory );

	_pmap::iterator ItPlayer;
	ItPlayer = localPlayers.find(to);
	if ( ItPlayer == localPlayers.end() )
	{
		nlinfo( "player id %u not found !", to );
	}
	else
	{
		TSockId conToClient = ((*ItPlayer).second).con;

		CMessage msgoutinv( CNetManager::getSIDA( "FS" ), "INIT_INV" );
		msgoutinv.serial( inventory );		
		CNetManager::send( "FS", msgoutinv, conToClient );



		nlinfo( "Sent INIT_INV to  client.");
	}

	
}	
/****************************************************************************
 * cbUpdateInv
 *
 * Receive a CH_INV messages from the Player Service to send it to all
 * the clients.
 ****************************************************************************/
void cbUpdateInv ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32  to;
	uint32  doid;
	bool add;
	//uint32  quantity;
	uint8 dotype;

	nlinfo("receiving CH_INV");
	msgin.serial( to );
	nlinfo("to %d",to);
	msgin.serial( doid );
		nlinfo("doid %d",doid);
	msgin.serial( add );
		nlinfo("add");
	msgin.serial( dotype );
		nlinfo("class %d",dotype);
	//msgin.serial( quantity );


	_pmap::iterator ItPlayer;
	ItPlayer = localPlayers.find(to);
	if ( ItPlayer == localPlayers.end() )
	{
		nlinfo( "player id %u not found !", to );
	}
	else
	{
		TSockId conToClient = ((*ItPlayer).second).con;

		CMessage msgoutinv( CNetManager::getSIDA( "FS" ), "CH_INV" );
		msgoutinv.serial( doid );
		msgoutinv.serial( add );
		msgoutinv.serial( dotype );
		//msgoutinv.serial( quantity );
		CNetManager::send( "FS", msgoutinv, conToClient );

		nlinfo( "Sent CH_INV to  client.");
	}

	
}	
 /****************************************************************************
 * cbFire1Client
 *
 * Receive fire message from a client and send it to the Game Mech Service.
 ****************************************************************************/
void cbFire1Client ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32 playerId;
	NLMISC::CVector target;
	float	angle, pitch;

	nlinfo("cbFire");

	playerId = lookUpPlayerIdFromSock( from );
	// Input from the client is stored.

	msgin.serial( target );
	msgin.serial( angle );
	msgin.serial( pitch );

	// Prepare the message to send to the MYN service
	CMessage msgout( CNetManager::getSIDA( "MYN" ), "FIRE1" );
	//msgout.serial( message );
	msgout.serial( playerId );
	msgout.serial( target );
	msgout.serial( angle );
	msgout.serial( pitch );
	/*
	 * The incomming message from the client is sent to the MYN service
	 * under the "MYN" identification.
	 */
	CNetManager::send( "MYN", msgout );

//	nlinfo( "Received FIRE1C message \"%s\" from client \"%s\"",
//			msgout.c_str(),
//			clientcb.hostAddress(from).asString().c_str() );
}

 /****************************************************************************
 * cbHoldClient
 *
 * Receive hold message from a client and send it to the Game Mech Service.
 ****************************************************************************/
void cbHoldClient ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32 playerId;
	uint32 did;
	bool leftHand;

	playerId = lookUpPlayerIdFromSock( from );
	// Input from the client is stored.

	msgin.serial( did );
	msgin.serial( leftHand );

	// Prepare the message to send to the MYN service
	CMessage msgout( CNetManager::getSIDA( "MYN" ), "HOLD_DO" );
	//msgout.serial( message );
	msgout.serial( playerId );
	msgout.serial( did );
	msgout.serial( leftHand );
	/*
	 * The incomming message from the client is sent to the MYN service
	 * under the "MYN" identification.
	 */
	CNetManager::send( "MYN", msgout );

}

 /****************************************************************************
 * cbGiveClient
 *
 * Receive give message from a client and send it to the Mayan and Shaman Services.
 ****************************************************************************/
void cbGiveClient ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32 playerId, targetPlayerId;
	//uint32 did;
	//bool leftHand;

	playerId = lookUpPlayerIdFromSock( from );
	// Input from the client is stored.

	msgin.serial( targetPlayerId );
	//msgin.serial( leftHand );

	// Prepare the message to send to the MYN service
	CMessage msgout( CNetManager::getSIDA( "MYN" ), "GIVE_DO" );
	//msgout.serial( message );
	msgout.serial( playerId );
	msgout.serial( targetPlayerId );
	//msgout.serial( did );
	//msgout.serial( leftHand );

	CNetManager::send( "MYN", msgout );

	//CMessage msgout2( CNetManager::getSIDA( "SHA" ), "GIVE_DO" );
	//msgout.serial( message );
	//msgout2.serial( playerId );
	//CNetManager::send( "SHA", msgout2 );

}

 /****************************************************************************
 * cbGiveShaman
 *
 * Receive give message from the shaman service and send it to the Mayan Service.
 ****************************************************************************/
void cbGiveShaman ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32 targetPlayerId;
	uint32 did;
	//bool leftHand;

	//playerId = lookUpPlayerIdFromSock( from );
	// Input from the client is stored.

	msgin.serial( targetPlayerId );
	msgin.serial( did );
	//msgin.serial( leftHand );

	// Prepare the message to send to the MYN service
	CMessage msgout( CNetManager::getSIDA( "MYN" ), "GIVE_DO" );
	//msgout.serial( message );
	msgout.serial( targetPlayerId );
	msgout.serial( did );
	//msgout.serial( did );
	//msgout.serial( leftHand );

	CNetManager::send( "MYN", msgout );

	//CMessage msgout2( CNetManager::getSIDA( "SHA" ), "GIVE_DO" );
	//msgout.serial( message );
	//msgout2.serial( playerId );
	//CNetManager::send( "SHA", msgout2 );

}
/****************************************************************************
 * cbGiveMayan
 *
 * Receive give message from the mayan service and send it to the shaman Service.
 ****************************************************************************/
void cbGiveMayan ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32 targetPlayerId;
	uint32 did;
	//bool leftHand;

	//playerId = lookUpPlayerIdFromSock( from );
	// Input from the client is stored.

	msgin.serial( targetPlayerId );
	msgin.serial( did );
	//msgin.serial( leftHand );

	// Prepare the message to send to the MYN service
	//CMessage msgout( CNetManager::getSIDA( "MYN" ), "GIVE_DO" );
	//msgout.serial( message );
	//msgout.serial( playerId );
	//msgout.serial( did );
	//msgout.serial( leftHand );

	//CNetManager::send( "MYN", msgout );

	CMessage msgout2( CNetManager::getSIDA( "SHA" ), "GIVE_DO" );
	//msgout.serial( message );
	msgout2.serial( targetPlayerId );
	msgout2.serial( did );
	CNetManager::send( "SHA", msgout2 );

}

 /****************************************************************************
 * cbAddProjectile
 *
 * Receive projectile message from the Player Service 
 * and send it to the clients 
 ****************************************************************************/
void cbAddProjectile ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32  projectileId,
			playerId,
			bulletTypeId,
			time;
	std::string		shapeName,
					psName;
	CVector start,
			target;
		//	psScale;
	float   speed, angle, pitch;
		//	explosionRadius;


	// Input: process the reply of the player service
	msgin.serial( projectileId );
	msgin.serial( playerId );
	msgin.serial( bulletTypeId );
	msgin.serial( shapeName );
	msgin.serial( psName );
	msgin.serial( start );
	msgin.serial( target );
	msgin.serial( angle );
	msgin.serial( pitch );
	msgin.serial( speed );
	msgin.serial( time );

	// Output: send the reply to the client
	CMessage msgout( CNetManager::getSIDA( "FS" ), "ADD_PROJECTILE" );
	msgout.serial( projectileId );
	msgout.serial( playerId );
	msgout.serial( shapeName );
	msgout.serial( psName );
	msgout.serial( start );
	msgout.serial( target );
	msgout.serial( speed );

	// Send the message to all connected clients
	CNetManager::send( "FS", msgout, 0 );


	nlinfo( "Sent ADD_PROJECTILE message to all the connected clients");
	nlinfo("speed: %f",speed);
	CMessage pmsgout( CNetManager::getSIDA( "POS" ), "ADD_PROJECTILE" );
	pmsgout.serial( projectileId );
	pmsgout.serial( playerId );
	pmsgout.serial( bulletTypeId );
	pmsgout.serial( shapeName );
	pmsgout.serial( psName );
	pmsgout.serial( start );
	pmsgout.serial( angle );
	pmsgout.serial( pitch );
	pmsgout.serial( speed );
	pmsgout.serial( time );

	CNetManager::send( "POS", pmsgout );


}

 /****************************************************************************
 * cbTracer
 *
 * Receive an TRACER messages from the Position Service to send it to all
 * the clients.
 ****************************************************************************/
void cbTracer ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{

	CVector pos;
	string	text;

	msgin.serial( pos );
	msgin.serial( text );

	CMessage msgout( CNetManager::getSIDA( "FS" ), "TRACER" );
	msgout.serial( pos );
	msgout.serial( text );

	// Send the message to all connected clients
	CNetManager::send( "FS", msgout, 0 );

}

 /****************************************************************************
 * cbHitService
 *
 * Receive an HIT messages from the Position Service to send it to all
 * the clients.
 ****************************************************************************/
/*void cbHitService ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32  projectileId,
			playerId,
			bulletTypeId;
//	CVector start,
//			target;
	float   speed;
//			explosionRadius;
	bool	direct;

	// Input: process the reply of the position service
	msgin.serial( projectileId );
	msgin.serial( playerId );
	msgin.serial( direct );
	msgin.serial( speed );
	msgin.serial( bulletTypeId );

	// Output: send the reply to the client
	CMessage msgout( CNetManager::getSIDA( "FS" ), "HIT" );
	msgout.serial( projectileId );
	msgout.serial( playerId );

	// Send the message to all connected clients
	CNetManager::send( "FS", msgout, 0 );

	if(playerId > 1999)
	{
	CMessage msgoutm( CNetManager::getSIDA( "MAM" ), "HIT" );
	//msgout.serial( message );
	msgoutm.serial( playerId );
	msgoutm.serial( projectileId );
	msgoutm.serial( direct );
	msgoutm.serial( speed );
	msgoutm.serial( bulletTypeId );

	CNetManager::send( "MAM", msgoutm );
	}
	else
	{
	nlinfo( "Sent HITP message to all the MYN & connected clients");
	CMessage msgoutg( CNetManager::getSIDA( "MYN" ), "HIT" );
	//msgout.serial( message );
	msgoutg.serial( playerId );
	msgoutg.serial( projectileId );
	msgoutg.serial( direct );
	msgoutg.serial( speed );
	msgoutg.serial( bulletTypeId );

	CNetManager::send( "MYN", msgoutg );
	}

}
*/
 
/****************************************************************************
 * cbChangeEntityHand
 *
 * ChangeEntityHand message from the Player Service
 * and send it to the affected client.
 ****************************************************************************/ 
void cbChangeEntityHand ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32 playerId, did;
	bool leftHand;
	std::string meshName;
	uint16	sId;

	// Input from the client is stored.
	msgin.serial( sId );
	msgin.serial( playerId );
	msgin.serial( leftHand );
	msgin.serial( did );
	msgin.serial( meshName );

	_pmap::iterator ItPlayer;
	ItPlayer = localPlayers.find(playerId);
	if ( ItPlayer == localPlayers.end() )
	{
		nlinfo( "New player id %u not found !", playerId );
	}
	else
	{
		nlinfo("found player");

		CMessage msgout( CNetManager::getSIDA( "FS" ), "CH_ENTITY_HAND" );
		msgout.serial( playerId );
		msgout.serial( leftHand );
		msgout.serial( did );
		msgout.serial( meshName );		

		CNetManager::send( "FS", msgout, 0 );

		nlinfo( "Sent CH_ENTITY_HAND to the clients.");

		if(sId == 2001)
		{
			CMessage msgout( CNetManager::getSIDA( "MAM" ), "CH_ENTITY_HAND" );
			msgout.serial( playerId );
			msgout.serial( leftHand );
			msgout.serial( did );
			msgout.serial( meshName );		
			CNetManager::send( "MAM", msgout);
		}
		if(sId == 2010)
		{
			CMessage msgout( CNetManager::getSIDA( "MAM" ), "CH_ENTITY_HAND" );
			msgout.serial( playerId );
			msgout.serial( leftHand );
			msgout.serial( did );
			msgout.serial( meshName );		
			CNetManager::send( "MAM", msgout);
		}
	}
}

/****************************************************************************
 * cbAddSpotSound
 *
 * AddSpotSound message from the Player Service
 * and send it to the affected client.
 ****************************************************************************/ 
void cbAddSpotSound ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32 id;
	std::string soundName;
	float gain, pitch;

	msgin.serial (id);
	msgin.serial (soundName);
	msgin.serial (gain);
	msgin.serial (pitch);

	_pmap::iterator ItPlayer;
	ItPlayer = localPlayers.find(id);
	if ( ItPlayer == localPlayers.end() )
	{
		nlinfo( "New player id %u not found !", id );
	}
	else
	{
		nlinfo("found player");

		CMessage msgout( CNetManager::getSIDA( "FS" ), "ADD_SPOTSOUND" );

		msgout.serial (id);
		msgout.serial (soundName);
		msgout.serial (gain);
		msgout.serial (pitch);
	
		CNetManager::send( "FS", msgout, 0 );

		nlinfo( "Sent ADD_SPOTSOUND to the clients.");
	}
}

 /**************************************************************************** 
 * cbDropClient
 *
 * Receive drop messages from a client and send it to the Game Mech Service.
 ****************************************************************************/
void cbDropClient ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  playerId;
	uint32	dynamicObjectId;

	// Input from the client is stored.
	msgin.serial( playerId );
	msgin.serial( dynamicObjectId );

	// Prepare the message to send to the Position service
	CMessage msgout( CNetManager::getSIDA( "MYN" ), "DROP_DO" );
	msgout.serial( playerId );
	msgout.serial( dynamicObjectId );

	CNetManager::send( "MYN", msgout );
}

/****************************************************************************
 * cbPickClient
 *
 * Receive drop messages from a client and send it to the Game Mech Service.
 ****************************************************************************/
void cbPickClient ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  playerId;
	uint32	dynamicObjectId;

	// Input from the client is stored.
	msgin.serial( playerId );
	msgin.serial( dynamicObjectId );

	// Prepare the message to send to the Position service
	CMessage msgout( CNetManager::getSIDA( "MYN" ), "PICK_DO" );
	msgout.serial( playerId );
	msgout.serial( dynamicObjectId );

	CNetManager::send( "MYN", msgout );
}
/****************************************************************************
 * cbMisfire
 *
 * Receive drop messages from a client and send it to the Game Mech Service.
 ****************************************************************************/
void cbMisfire ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  playerId;
	//uint32	misfireType;
	
	msgin.serial( playerId );
	//msgin.serial( misfireType );

	_pmap::iterator ItPlayer;
	ItPlayer = localPlayers.find(playerId);
	if ( ItPlayer == localPlayers.end() )
	{
		nlinfo( "player id %u not found !", playerId );
	}
	else
	{
		nlinfo("found player");
		TSockId conToClient = ((*ItPlayer).second).con;

		CMessage msgout( CNetManager::getSIDA( "FS" ), "MISFIRE" );
//		msgout.serial( misfireType );		
		CNetManager::send( "FS", msgout, conToClient );

		nlinfo( "Sent MISFIREG to the appropriate client(s).");

	}
}
/****************************************************************************
 * cbAddFirearm
 *
 * Receive drop messages from a client and send it to the Game Mech Service.
 ****************************************************************************/
void cbAddFirearm ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  playerId, dynamicObjectId, Owner, TypeId, BulletsTypeId;
	uint16	Maintenance;
	uint8	BulletsLeft, MaxBullets, classId;

	//uint32	misfireType;
	std::string  Name, MeshName, ParticleSystem, SoundName, FireSound;
	CVector Position, Heading, TrajVector, ParticleSystemScale;
	bool Visable=true, Chambered, Jammed, Safety, RoundPresent;
	float Radius, Height, Scale, Weight, FireRate1Round, FireRateAuto, Loudness, RecoilMod, AngVelocity;

	nlinfo("recieving add firearm");
	msgin.serial( playerId );
	msgin.serial( dynamicObjectId, classId, Owner, TypeId);
	msgin.serial( Name );
	msgin.serial( MeshName );
	msgin.serial( Position, Heading);
	msgin.serial( TrajVector);
	msgin.serial( AngVelocity);
	msgin.serial( Radius, Height, Scale, Weight);
	msgin.serial( SoundName );
	msgin.serial( Maintenance );
	msgin.serial( RoundPresent );
	msgin.serial( BulletsLeft );
	msgin.serial( Safety );
	msgin.serial( Jammed );
	msgin.serial( Chambered );
	msgin.serial( BulletsTypeId, MaxBullets  );
	msgin.serial( FireRate1Round, FireRateAuto, Loudness, RecoilMod );
	msgin.serial( ParticleSystem );
	msgin.serial( ParticleSystemScale );
	msgin.serial( FireSound );

/*		outmsg3.serial(thePlayerId);
		outmsg3.serial( dynamicObjectId, theFirearm.Owner, theFirearm.TypeId );
		outmsg3.serial( theFirearm.Name );
		outmsg3.serial( theFirearm.MeshName );
		outmsg3.serial( theFirearm.Position, theFirearm.Heading );
		outmsg3.serial( theFirearm.TrajVector );
		outmsg3.serial( theFirearm.AngVelocity );
		outmsg3.serial( theFirearm.Radius, theFirearm.Height, theFirearm.Scale );
		outmsg3.serial( theFirearm.Maintenance );
		outmsg3.serial( theFirearm.RoundPresent );
		outmsg3.serial( theFirearm.BulletsLeft );
		outmsg3.serial( theFirearm.Safety );
		outmsg3.serial( theFirearm.Jammed );
		outmsg3.serial( theFirearm.Chambered );
		outmsg3.serial( theFirearmType.BulletsTypeId, theFirearmType.MaxBullets  );
		outmsg3.serial( theFirearmType.FireRate1Round, theFirearmType.FireRateAuto, theFirearmType.Loudness, theFirearmType.RecoilMod );
		outmsg3.serial( theFirearmType.ParticleSystem );
		outmsg3.serial( theFirearmType.ParticleSystemScale ); */

	_pmap::iterator ItPlayer;
	ItPlayer = localPlayers.find(playerId);
	if ( ItPlayer == localPlayers.end() )
	{
		nlinfo( "player id %u not found !", playerId );
	}
	else
	{
		nlinfo("found player");
		TSockId conToClient = ((*ItPlayer).second).con;

		CMessage msgout( CNetManager::getSIDA( "FS" ), "ADD_FIREARM" );
		nlinfo(".");
		uint	ver= msgout.serialVersion(0);
				nlinfo(".");
		msgout.serial( dynamicObjectId, classId, Owner, TypeId);
				nlinfo(".");
		msgout.serial( Name );
				nlinfo(".");
		msgout.serial( MeshName );
		msgout.serial( Position, Heading);
				nlinfo(".");
		msgout.serial( TrajVector);
		msgout.serial( AngVelocity);
				nlinfo(".");
		msgout.serial( Visable );
		msgout.serial( Radius, Height, Scale, Weight);
		msgout.serial( SoundName );
				nlinfo(".");
		msgout.serial( Maintenance );
		msgout.serial( RoundPresent );
				nlinfo(".");
		msgout.serial( BulletsLeft );
		msgout.serial( Safety );
				nlinfo(".");
		msgout.serial( Jammed );
		msgout.serial( Chambered );
				nlinfo(".");
		msgout.serial( BulletsTypeId, MaxBullets  );
				nlinfo(".");
		msgout.serial( FireRate1Round, FireRateAuto, Loudness, RecoilMod );
				nlinfo(".");
		msgout.serial( ParticleSystem );
		msgout.serial( ParticleSystemScale );
		msgout.serial( FireSound );
		nlinfo(".sent");
		CNetManager::send( "FS", msgout, conToClient );

		nlinfo( "Sent ADD_FIREARMG to the appropriate client(s).");

	}
}

/****************************************************************************
 * cbAddBoxOfBullets
 *
 * Receive bob messages from the player service and send it to the clients.
 ****************************************************************************/
void cbAddBoxOfBullets ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  playerId, dynamicObjectId, Owner, TypeId, BulletsTypeId;
//	uint16	Maintenance;
	uint8	BulletsLeft, MaxBullets, classId;

	//uint32	misfireType;
	std::string  Name, MeshName, ParticleSystem, SoundName, FireSound;
	CVector Position, Heading, TrajVector, ParticleSystemScale;
	bool Visable=true;
	float Radius, Height, Scale, Weight, AngVelocity;

	nlinfo("recieving add firearm");
	msgin.serial( playerId );
	msgin.serial( dynamicObjectId, classId, Owner, TypeId);
	msgin.serial( Name );
	msgin.serial( MeshName );
	msgin.serial( Position, Heading);
	msgin.serial( TrajVector);
	msgin.serial( AngVelocity);
	msgin.serial( Radius, Height, Scale, Weight);
	msgin.serial( SoundName );
	msgin.serial( BulletsTypeId );
	msgin.serial( MaxBullets );
	msgin.serial( BulletsLeft );



	_pmap::iterator ItPlayer;
	ItPlayer = localPlayers.find(playerId);
	if ( ItPlayer == localPlayers.end() )
	{
		nlinfo( "player id %u not found !", playerId );
	}
	else
	{
		nlinfo("found player");
		TSockId conToClient = ((*ItPlayer).second).con;

		CMessage msgout( CNetManager::getSIDA( "FS" ), "ADD_BOXOFBULLETS" );
		nlinfo(".");
		uint	ver= msgout.serialVersion(0);
				nlinfo(".");
		msgout.serial( dynamicObjectId, classId, Owner, TypeId);
				nlinfo(".");
		msgout.serial( Name );
				nlinfo(".");
		msgout.serial( MeshName );
		msgout.serial( Position, Heading);
				nlinfo(".");
		msgout.serial( TrajVector);
		msgout.serial( AngVelocity);
				nlinfo(".");
		msgout.serial( Visable );
		msgout.serial( Radius, Height, Scale, Weight);
		msgout.serial( SoundName );
				nlinfo(".");
		msgout.serial( BulletsTypeId );
		msgout.serial( MaxBullets );
				nlinfo(".");
		msgout.serial( BulletsLeft );
	
		nlinfo(".sent");
		CNetManager::send( "FS", msgout, conToClient );

		nlinfo( "Sent ADD_BOXOFBULLETSG to the appropriate client(s).");

	}
}
/****************************************************************************
 * cbAddNutrition
 *
 * Receive bob messages from the player service and send it to the clients.
 ****************************************************************************/
void cbAddNutrition ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  playerId, dynamicObjectId, Owner, TypeId;
	uint16	Maintenance, Quality;
	uint8	Carbos, Proteins, VitaminA, VitaminB, VitaminC, Fats, Water, classId;

	//uint32	misfireType;
	std::string  Name, MeshName, ParticleSystem, SoundName, FireSound;
	CVector Position, Heading, TrajVector, ParticleSystemScale;
	bool Visable=true;
	float Radius, Height, Scale, Weight, AngVelocity;

	nlinfo("recieving add firearm");
	msgin.serial( playerId );
	msgin.serial( dynamicObjectId, classId, Owner, TypeId);
	msgin.serial( Name );
	msgin.serial( MeshName );
	msgin.serial( Position, Heading);
	msgin.serial( TrajVector);
	msgin.serial( AngVelocity);
	msgin.serial( Radius, Height, Scale, Weight);
	msgin.serial( SoundName );
	msgin.serial( Carbos, Proteins, VitaminA, VitaminB, VitaminC );
	msgin.serial( Fats, Water );
	msgin.serial( Quality, Maintenance );


	_pmap::iterator ItPlayer;
	ItPlayer = localPlayers.find(playerId);
	if ( ItPlayer == localPlayers.end() )
	{
		nlinfo( "player id %u not found !", playerId );
	}
	else
	{
		nlinfo("found player");
		TSockId conToClient = ((*ItPlayer).second).con;

		CMessage msgout( CNetManager::getSIDA( "FS" ), "ADD_BOXOFBULLETS" );
		nlinfo(".");
		uint	ver= msgout.serialVersion(0);
				nlinfo(".");
		msgout.serial( dynamicObjectId, classId, Owner, TypeId);
				nlinfo(".");
		msgout.serial( Name );
				nlinfo(".");
		msgout.serial( MeshName );
		msgout.serial( Position, Heading);
				nlinfo(".");
		msgout.serial( TrajVector);
		msgout.serial( AngVelocity);
				nlinfo(".");
		msgout.serial( Visable );
		msgout.serial( Radius, Height, Scale, Weight);
		msgout.serial( SoundName );
		msgout.serial( Carbos, Proteins, VitaminA, VitaminB, VitaminC );
		msgout.serial( Fats, Water );
		msgout.serial( Quality, Maintenance );
	
		nlinfo(".sent");
		CNetManager::send( "FS", msgout, conToClient );

		nlinfo( "Sent ADD_BOXOFBULLETSG to the appropriate client(s).");

	}
}
/****************************************************************************
 * cbAddDo
 *
 * Receive drop messages from a client and send it to the Game Mech Service.
 ****************************************************************************/
void cbAddDo ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  playerId, dynamicObjectId, Owner, TypeId;
	//uint32	misfireType;
	std::string  Name, MeshName, SoundName;
	CVector Position, Heading, TrajVector;
	bool Visable=true;
	float Radius, Height, Scale, Weight, AngVelocity;
	uint8	classId;

	msgin.serial( playerId );
	msgin.serial( dynamicObjectId );
	msgin.serial( classId );
	msgin.serial( Owner );
	msgin.serial( TypeId);
	msgin.serial( Name );
	msgin.serial( MeshName );
nlinfo(".%s", MeshName);
	msgin.serial( Position );
	msgin.serial( Heading);
	msgin.serial( TrajVector);
	msgin.serial( AngVelocity);
nlinfo(".d");
//	msgin.serial( Visable);
	nlinfo(".e");
	msgin.serial( Radius );
	nlinfo(".f");
	msgin.serial( Height );
	nlinfo(".h");
	msgin.serial( Scale );
	nlinfo(".scaledone");
	msgin.serial( Weight );
	nlinfo(".weightdone");
	msgin.serial( SoundName );
nlinfo(".soundname");



	_pmap::iterator ItPlayer;
	ItPlayer = localPlayers.find(playerId);
	if ( ItPlayer == localPlayers.end() )
	{
		nlinfo( "player id %u not found !", playerId );
	}
	else
	{
		nlinfo("found player");
		TSockId conToClient = ((*ItPlayer).second).con;

		CMessage msgout( CNetManager::getSIDA( "FS" ), "ADD_DO" );
		
		uint	ver= msgout.serialVersion(0);
		msgout.serial( dynamicObjectId, classId, Owner, TypeId);
		msgout.serial( Name );
		msgout.serial( MeshName );
		msgout.serial( Position, Heading);
		msgout.serial( TrajVector);
		msgout.serial( AngVelocity);
		msgout.serial( Visable);
		msgout.serial( Radius, Height, Scale, Weight);
		msgout.serial( SoundName );
		CNetManager::send( "FS", msgout, conToClient );

		nlinfo( "Sent ADD_DO to the appropriate client(s).");

	}
}
 /****************************************************************************
 * cdRemoveDO
 *
 * Receive an REMOVE_DO messages from the Position Service to send it to all
 * the clients.
 ****************************************************************************/
void cbRemoveDo ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32 dynamicObjectId;

	msgin.serial( dynamicObjectId );

	CMessage msgout( CNetManager::getSIDA( "FS" ), "REMOVE_DO" );

	msgout.serial( dynamicObjectId );
	CNetManager::send( "FS", msgout, 0 );

	nlinfo( "Sent REMOVE_DOP message to all the connected clients");
}

/****************************************************************************
 * cbInvSelectClient
 *
 * Receive drop messages from a client and send it to the Game Mech Service.
 ****************************************************************************/
void cbInvSelectClient ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  playerId;
	uint32	dynamicObjectId;

	playerId = lookUpPlayerIdFromSock( from );
	// Input from the client is stored.
	//msgin.serial( playerId );
	msgin.serial( dynamicObjectId );

	// Prepare the message to send to the Position service
	CMessage msgout( CNetManager::getSIDA( "MYN" ), "INV_SELECT" );
	msgout.serial( playerId );
	msgout.serial( dynamicObjectId );

	CNetManager::send( "MYN", msgout );
}

/****************************************************************************
 * cbDropService
 *
 * Receive drop messages from a Position service and send it to all clients
 ****************************************************************************/
void cbDropService ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	bool		all;
	uint32      id, to;
	uint32		playerId;
	uint32      owner;
	uint32		type;
	std::string name;
	std::string meshName;
	CVector		position, massCenter, dimensions;
	CVector		heading;
	float		radius;
	float		weight;
	float		scale;
	uint8		classId;
	uint16		sId;

	// Input: process the reply of the position service
	msgin.serial( all );
	msgin.serial( sId );
	msgin.serial( to );
	msgin.serial( id );
	msgin.serial( classId );
	msgin.serial( name );
	msgin.serial( meshName );
	msgin.serial( position );
	msgin.serial( heading );
	msgin.serial( massCenter);
	msgin.serial( dimensions);
	msgin.serial( radius);
	msgin.serial( weight);
	msgin.serial( scale);

	// Output: send the reply to the client
			nlinfo("beginning dropservice send");
	CMessage msgout( CNetManager::getSIDA( "FS" ), "ADD_ENTITY" );

	msgout.serial( id );
	msgout.serial( classId );
	msgout.serial( name );;
	msgout.serial( meshName );;
	msgout.serial( position );
	msgout.serial( heading );
	msgout.serial( massCenter);
	msgout.serial( dimensions);
	msgout.serial( radius);
	msgout.serial( weight);
	msgout.serial(scale);
		nlinfo("finished send");
	if(all)
	{
		// Send the message to all connected clients
		CNetManager::send( "FS", msgout, 0 );

/*
		if(sId != 2001)
		{
			//and to the players
			CMessage psmsgout( CNetManager::getSIDA( "PS" ), "ADD_ENTITY" );
	
			psmsgout.serial( id );
			psmsgout.serial( classId );
			psmsgout.serial( name );;
			psmsgout.serial( meshName );;
			psmsgout.serial( position );
			psmsgout.serial( heading );
			psmsgout.serial( massCenter);
			psmsgout.serial( dimensions);
			psmsgout.serial( radius);
			psmsgout.serial( weight);
			psmsgout.serial( scale);

			CNetManager::send( "PS", psmsgout);
		}
		if(sId != 2010)//if its not coming from the mammoth service 
		{
			//send to the mammoths
			CMessage mmsgout( CNetManager::getSIDA( "MAM" ), "ADD_ENTITY" );
	
			mmsgout.serial( id );
			mmsgout.serial( classId );
			mmsgout.serial( name );;
			mmsgout.serial( meshName );;
			mmsgout.serial( position );
			mmsgout.serial( heading );
			mmsgout.serial( massCenter);
			mmsgout.serial( dimensions);
			mmsgout.serial( radius);
			mmsgout.serial( weight);
			mmsgout.serial( scale); 

			CNetManager::send( "MAM", mmsgout);
		}
		if(sId != 2012)
		{
			//and to the shamans
			CMessage smsgout( CNetManager::getSIDA( "SHA" ), "ADD_ENTITY" );
	
			smsgout.serial( id );
			smsgout.serial( classId );
			smsgout.serial( name );;
			smsgout.serial( meshName );;
			smsgout.serial( position );
			smsgout.serial( heading );
			smsgout.serial( massCenter);
			smsgout.serial( dimensions);
			smsgout.serial( radius);
			smsgout.serial( weight);
			smsgout.serial( scale); 

			CNetManager::send( "SHA", smsgout);
		}*/
		if(sId != 2013)
		{
			//and to the mayans
			CMessage mynsmsgout( CNetManager::getSIDA( "MYN" ), "ADD_ENTITY" );
	
			mynsmsgout.serial( id );
			mynsmsgout.serial( classId );
			mynsmsgout.serial( name );;
			mynsmsgout.serial( meshName );;
			mynsmsgout.serial( position );
			mynsmsgout.serial( heading );
			mynsmsgout.serial( massCenter);
			mynsmsgout.serial( dimensions);
			mynsmsgout.serial( radius);
			mynsmsgout.serial( weight);
			mynsmsgout.serial( scale);

			CNetManager::send( "PS", mynsmsgout);
		}


/*		if(sId == 2001)
		{
			CMessage msgout2( CNetManager::getSIDA( "MAM" ), "ADD_ENTITY" );
	
			msgout2.serial( id );
			msgout2.serial( classId );
			msgout2.serial( name );;
			msgout2.serial( meshName );;
			msgout2.serial( position );
			msgout2.serial( heading );
			msgout2.serial( massCenter);
			msgout2.serial( dimensions);
			msgout2.serial( radius);
			msgout2.serial( weight);	
			msgout2.serial(scale);
			CNetManager::send( "MAM", msgout2 );
		}
		else if(sId == 2010)
		{
			CMessage msgout2( CNetManager::getSIDA( "MYN" ), "ADD_ENTITY" );
		
			msgout2.serial( id );
			msgout2.serial( classId );
			msgout2.serial( name );;
			msgout2.serial( meshName );;
			msgout2.serial( position );
			msgout2.serial( heading );
			msgout2.serial( massCenter);
			msgout2.serial( dimensions);
			msgout2.serial( radius);
			msgout2.serial( weight);	
			msgout2.serial(scale);
			CNetManager::send( "MYN", msgout2 );
		}
		CMessage msgout3( CNetManager::getSIDA( "POS" ), "ADD_ENTITY" );
	
		msgout3.serial( id );
		msgout3.serial( classId );
		msgout3.serial( name );;
		msgout3.serial( meshName );;
		msgout3.serial( position );
		msgout3.serial( heading );
		msgout3.serial( massCenter);
		msgout3.serial( dimensions);
		msgout3.serial( radius);
		msgout3.serial( weight);
		msgout3.serial(scale);
		CNetManager::send( "POS", msgout3 );
*/
	}
	else //not all
	{
		_pmap::iterator ItPlayer;
		ItPlayer = localPlayers.find(playerId);
		if ( ItPlayer == localPlayers.end() )
		{
			nlinfo( "player id %u not found !", playerId );
		}
		else
		{
			nlinfo("found player");
			TSockId conToClient = ((*ItPlayer).second).con;
	
			CNetManager::send( "FS", msgout, conToClient );
		}
	}
	nlinfo( "Sent ADD_ENTITY (DO) message to all the connected clients and services");
}

/****************************************************************************
 * cbPossess
 *
 * Receive a POSSESS message from a client and send it to the Mayan
 * Service.
 ****************************************************************************/
void cbPossess ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  id;

	nlinfo("cbPossess");
	id = lookUpPlayerIdFromSock( from );
	nlinfo("id: %d", id);
	CMessage msgout( CNetManager::getSIDA( "MYN" ), "POSSESS" );
	
	msgout.serial( id );

	
	/*
	 * The incomming message from the client is sent to the Mayan service
	 * under the "MYN" identification.
	 */

	CNetManager::send( "MYN", msgout ); 
	nlinfo( "Received POSSESS from the client");
}

/****************************************************************************
 * cdAddMayan
 *
 * Receive an ADD_MAYAN messages from the Mayan Service to send it to all
 * the clients.
 ****************************************************************************/
void cbAddMayan ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	nlinfo("cbAddMayan");
	uint32      id, to;
	uint32      owner;
	uint32		type;
	uint16		sId;
	uint8		classId;
	std::string meshName, name;
	CVector		position;
	CVector		heading, massCenter, dimensions;
	float		radius;
	float		weight;
	float		scale;
	bool		all;

	// Input
	msgin.serial( all );
	msgin.serial( sId );
	msgin.serial( to );
	msgin.serial( id );
	msgin.serial( classId );
	msgin.serial( name );
	msgin.serial( meshName );
	msgin.serial( position );
	msgin.serial( heading );
	msgin.serial( massCenter);
	msgin.serial( dimensions);
//	msgin.serial( radius);
	msgin.serial( weight);
	msgin.serial( scale);


	nlinfo("position: x %f y %f z %f",position.x,position.y,position.z);
//	nlinfo("ecid: %d", ecid);
	/*
	nlinfo("IN ADD ENT");
	nlinfo ("Receive add entity %u '%s' %s (%f,%f,%f)", id, name.c_str(), race==0?"penguin":"gnu", startPosition.x, startPosition.y, startPosition.z);
//	enum TType	{ Self, Other, Dynamic, Snowball, Projectile };

	msgin.serial( id ); nlinfo("id=%d", id);
	msgin.serial( owner );
	msgin.serial( type );
	msgin.serial( name );nlinfo("name=%s", name);
	msgin.serial( mesh );nlinfo("mesh=%s", mesh);
	msgin.serial( position );nlinfo("1");
	msgin.serial( heading ); 
	msgin.serial( radius );nlinfo("1");
	msgin.serial( height );
	msgin.serial( scale );
	msgin.serial( entityType );nlinfo("et=%d", entityType);
	*/

	nlinfo("recieved ADD_MAYAN from service");

	nlinfo("id: %d  -  class: %d",id,classId);
	if(all) nlinfo("to all");

	scale = 1.0f;//fix this!!! to be set by player service

	// Output: send the reply to the client
	CMessage msgout( CNetManager::getSIDA( "FS" ), "ADD_ENTITY" );

	msgout.serial( id );
	msgout.serial( classId );
	msgout.serial( name );;
	msgout.serial( meshName );;
	msgout.serial( position );
	msgout.serial( heading );
	msgout.serial( massCenter);
	msgout.serial( dimensions);
	msgout.serial( radius);
	msgout.serial( weight);
	msgout.serial( scale); 

	//nlinfo( "Sent ADD_ENTITY message to all the connected clients");
	if ( all == true )
	{
		// Send the message to all connected clients
		CNetManager::send( "FS", msgout, 0 );

		nlinfo( "Sent ADD_ENTITY message to all the connected clients");

		/*
		if(sId != 2001)
		{
			//and to the players
			CMessage psmsgout( CNetManager::getSIDA( "PS" ), "ADD_ENTITY" );
	
			psmsgout.serial( id );
			psmsgout.serial( classId );
			psmsgout.serial( name );;
			psmsgout.serial( meshName );;
			psmsgout.serial( position );
			psmsgout.serial( heading );
			psmsgout.serial( massCenter);
			psmsgout.serial( dimensions);
			psmsgout.serial( radius);
			psmsgout.serial( weight);
			psmsgout.serial( scale);

			CNetManager::send( "PS", psmsgout);
		}
		if(sId != 2010)//if its not coming from the mammoth service 
		{
			//send to the mammoths
			CMessage mmsgout( CNetManager::getSIDA( "MAM" ), "ADD_ENTITY" );
	
			mmsgout.serial( id );
			mmsgout.serial( classId );
			mmsgout.serial( name );;
			mmsgout.serial( meshName );;
			mmsgout.serial( position );
			mmsgout.serial( heading );
			mmsgout.serial( massCenter);
			mmsgout.serial( dimensions);
			mmsgout.serial( radius);
			mmsgout.serial( weight);
			mmsgout.serial( scale); 

			CNetManager::send( "MAM", mmsgout);
		}
		if(sId != 2012)
		{
			//and to the shamans
			CMessage smsgout( CNetManager::getSIDA( "SHA" ), "ADD_ENTITY" );
	
			smsgout.serial( id );
			smsgout.serial( classId );
			smsgout.serial( name );;
			smsgout.serial( meshName );;
			smsgout.serial( position );
			smsgout.serial( heading );
			smsgout.serial( massCenter);
			smsgout.serial( dimensions);
			smsgout.serial( radius);
			smsgout.serial( weight);
			smsgout.serial( scale); 

			CNetManager::send( "SHA", smsgout);
		}*/
		if(sId != 2013)
		{
			//and to the mayans
			CMessage mynsmsgout( CNetManager::getSIDA( "MYN" ), "ADD_ENTITY" );
	
			mynsmsgout.serial( id );
			mynsmsgout.serial( classId );
			mynsmsgout.serial( name );;
			mynsmsgout.serial( meshName );;
			mynsmsgout.serial( position );
			mynsmsgout.serial( heading );
			mynsmsgout.serial( massCenter);
			mynsmsgout.serial( dimensions);
			mynsmsgout.serial( radius);
			mynsmsgout.serial( weight);
			mynsmsgout.serial( scale);

			CNetManager::send( "PS", mynsmsgout);
		}
		/*
		//and to the position service
		CMessage posmsgout( CNetManager::getSIDA( "POS" ), "ADD_ENTITY" );

		posmsgout.serial(id );
		posmsgout.serial(classId );
		posmsgout.serial(name );;
		posmsgout.serial(meshName );;
		posmsgout.serial(position );
		posmsgout.serial(heading );
		posmsgout.serial(massCenter);
		posmsgout.serial(dimensions);
		posmsgout.serial(radius);
		posmsgout.serial(weight);
		posmsgout.serial(scale);
		CNetManager::send( "POS", posmsgout);
		*/
	}
	else //not all
	{
		// Send the message about a former connected client to the new client
		_pmap::iterator ItPlayer;
		ItPlayer = localPlayers.find(to);
		if ( ItPlayer == localPlayers.end() )
		{
			nlinfo( "New player id %u not found !", to );
		}
		else
		{
			TSockId conToClient = ((*ItPlayer).second).con;
			CNetManager::send( "FS", msgout, conToClient );

			nlinfo( "Sent ADD_ENTITY about each of the connected clients to the new client.");
		}
	}

}

/****************************************************************************
 * cdAddShaman
 *
 * Receive an ADD_SHAMAN messages from the Mayan Service to send it to all
 * the clients.
 ****************************************************************************/
/*void cbAddShaman ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	nlinfo("cbAddShaman");
	uint32      id, to;
	uint32      owner;
	uint32		type;
	uint16		sId;
	uint8		classId;
	std::string meshName, name;
	CVector		position;
	CVector		heading, massCenter, dimensions;
	float		radius;
	float		weight;
	float		scale;
	bool		all;

	// Input
	msgin.serial( all );
	msgin.serial( sId );
	msgin.serial( to );
	msgin.serial( id );
	msgin.serial( classId );
	msgin.serial( name );
	msgin.serial( meshName );
	msgin.serial( position );
	msgin.serial( heading );
	msgin.serial( massCenter);
	msgin.serial( dimensions);
//	msgin.serial( radius);
	msgin.serial( weight);
	msgin.serial( scale);


	nlinfo("position: x %f y %f z %f",position.x,position.y,position.z);
//	nlinfo("ecid: %d", ecid);
	/*
	nlinfo("IN ADD ENT");
	nlinfo ("Receive add entity %u '%s' %s (%f,%f,%f)", id, name.c_str(), race==0?"penguin":"gnu", startPosition.x, startPosition.y, startPosition.z);
//	enum TType	{ Self, Other, Dynamic, Snowball, Projectile };

	msgin.serial( id ); nlinfo("id=%d", id);
	msgin.serial( owner );
	msgin.serial( type );
	msgin.serial( name );nlinfo("name=%s", name);
	msgin.serial( mesh );nlinfo("mesh=%s", mesh);
	msgin.serial( position );nlinfo("1");
	msgin.serial( heading ); 
	msgin.serial( radius );nlinfo("1");
	msgin.serial( height );
	msgin.serial( scale );
	msgin.serial( entityType );nlinfo("et=%d", entityType);
	*/
/*
	nlinfo("recieved ADD_MAYAN from service");

	nlinfo("id: %d  -  class: %d",id,classId);
	if(all) nlinfo("to all");

	scale = 1.0f;//fix this!!! to be set by player service

	// Output: send the reply to the client
	CMessage msgout( CNetManager::getSIDA( "FS" ), "ADD_ENTITY" );

	msgout.serial( id );
	msgout.serial( classId );
	msgout.serial( name );;
	msgout.serial( meshName );;
	msgout.serial( position );
	msgout.serial( heading );
	msgout.serial( massCenter);
	msgout.serial( dimensions);
	msgout.serial( radius);
	msgout.serial( weight);
	msgout.serial( scale); 

	//nlinfo( "Sent ADD_ENTITY message to all the connected clients");
	if ( all == true )
	{
		// Send the message to all connected clients
		CNetManager::send( "FS", msgout, 0 );

		nlinfo( "Sent ADD_ENTITY message to all the connected clients");

		if(sId == 2001)
		{
			//and to the shamans
			CMessage smsgout( CNetManager::getSIDA( "SHA" ), "ADD_ENTITY" );
	
			smsgout.serial( id );
			smsgout.serial( classId );
			smsgout.serial( name );;
			smsgout.serial( meshName );;
			smsgout.serial( position );
			smsgout.serial( heading );
			smsgout.serial( massCenter);
			smsgout.serial( dimensions);
			smsgout.serial( radius);
			smsgout.serial( weight);
			smsgout.serial( scale); 

			CNetManager::send( "SHA", smsgout);
		}
		if(sId == 2010)
		{
			//and the shamans to the mayans
			CMessage msmsgout( CNetManager::getSIDA( "MYN" ), "ADD_ENTITY" );
	
			msmsgout.serial( id );
			msmsgout.serial( classId );
			msmsgout.serial( name );;
			msmsgout.serial( meshName );;
			msmsgout.serial( position );
			msmsgout.serial( heading );
			msmsgout.serial( massCenter);
			msmsgout.serial( dimensions);
			msmsgout.serial( radius);
			msmsgout.serial( weight);
			msmsgout.serial( scale);

			CNetManager::send( "PS", msmsgout);
		}
		//and to the position service
		CMessage posmsgout( CNetManager::getSIDA( "POS" ), "ADD_ENTITY" );

		posmsgout.serial(id );
		posmsgout.serial(classId );
		posmsgout.serial(name );;
		posmsgout.serial(meshName );;
		posmsgout.serial(position );
		posmsgout.serial(heading );
		posmsgout.serial(massCenter);
		posmsgout.serial(dimensions);
		posmsgout.serial(radius);
		posmsgout.serial(weight);
		posmsgout.serial(scale);
		CNetManager::send( "POS", posmsgout);

	}
	else
	{
		// Send the message about a former connected client to the new client
		_pmap::iterator ItPlayer;
		ItPlayer = localPlayers.find(to);
		if ( ItPlayer == localPlayers.end() )
		{
			nlinfo( "New player id %u not found !", to );
		}
		else
		{
			TSockId conToClient = ((*ItPlayer).second).con;
			CNetManager::send( "FS", msgout, conToClient );

			nlinfo( "Sent ADD_ENTITY about each of the connected clients to the new client.");
		}
	}

}*/

/////////////////////////
//end InOrbit callbacks//
/////////////////////////

/****************************************************************************
 * cbChatClient
 *
 * Receive chat messages from a client and send it to the Chat Service.
 ****************************************************************************/
void cbChatClient ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	string message;

	// Input from the client is stored.
	msgin.serial( message );

	// Prepare the message to send to the CHAT service
	CMessage msgout( CNetManager::getSIDA( "CHAT" ), "CHAT" );
	msgout.serial( message );

	/*
	 * The incomming message from the client is sent to the CHAT service
	 * under the "CHAT" identification.
	 */
	CNetManager::send( "CHAT", msgout );

	nlinfo( "Received CHAT message \"%s\" from client \"%s\"",
			message.c_str(),
			clientcb.hostAddress(from).asString().c_str() );
}


/****************************************************************************
 * cbChatService
 *
 * Receive chat messages from the Chat Service to send it to all the clients.
 ****************************************************************************/
void cbChatService ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	string  message;

	// Input: process the reply of the chat service
	msgin.serial( message );

	// Output: send the reply to the client
	CMessage msgout( CNetManager::getSIDA( "FS" ), "CHAT" );
	msgout.serial( message );

	// Send the message to all connected clients
	CNetManager::send( "FS", msgout, 0 );

	nlinfo( "Sent chat message \"%s\" to all clients", message.c_str());
}


/****************************************************************************
 * cbPosClient
 *
 * Receive position messages from a client and send it to the Position Service.
 ****************************************************************************/
void cbPosClient ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  id;
	CVector pos;
	float   angle;
	uint32  state;

	// Input from the client is stored.
	msgin.serial( id );
	msgin.serial( pos );
	msgin.serial( angle );
	msgin.serial( state );

	// Prepare the message to send to the Position service
	CMessage msgout( CNetManager::getSIDA( "POS" ), "ENTITY_POS" );
	msgout.serial( id );
	msgout.serial( pos );
	msgout.serial( angle );
	msgout.serial( state );

	/*
	 * The incomming message from the client is sent to the Position service
	 * under the "POS" identification.
	 */
	CNetManager::send( "POS", msgout );

	// Prepare the message to send to the Position service
	CMessage msgout2( CNetManager::getSIDA( "MYN" ), "ENTITY_POS" );
	msgout2.serial( id );
	msgout2.serial( pos );
	msgout2.serial( angle );
	msgout2.serial( state );

	CNetManager::send( "MYN", msgout2 );

	//nlinfo( "Received ENTITY_POS from the client");
}


/****************************************************************************
 * cbPosService
 *
 * Receive position messages from the Position Service to send it to all the
 * clients.
 ****************************************************************************/
void cbPosService ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32  id;
	CVector pos;
	float   angle;
	uint32  state;

	// Input: process the reply of the position service
	msgin.serial( id );
	msgin.serial( pos );
	msgin.serial( angle );
	msgin.serial( state );

	// Output: send the reply to the client
	CMessage msgout( CNetManager::getSIDA( "FS" ), "ENTITY_POS" );
	msgout.serial( id );
	msgout.serial( pos );
	msgout.serial( angle );
	msgout.serial( state );

	// Send the message to all connected clients
	CNetManager::send( "FS", msgout, 0 );

	//nlinfo( "Sent ENTITY_POS message to all the connected clients");
}



/****************************************************************************
 * cbAddClient
 *
 * Receive an ADD_ENTITY message from a client and send it to the Position
 * Service.
 ****************************************************************************/
void cbAddClient ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  id;
	string  name;
	uint8   race;
	CVector start;

	// Input from the client is stored.
	msgin.serial( id );
	msgin.serial( name );
	msgin.serial( race );
	msgin.serial( start );

	// Prepare the message to send to the Position service
	CMessage msgout( CNetManager::getSIDA( "POS" ), "ADD_ENTITY" );
	msgout.serial( id );
	msgout.serial( name );
	msgout.serial( race );
	msgout.serial( start );

	/*
	 * The incomming message from the client is sent to the Position service
	 * under the "POS" identification.
	 */
	CNetManager::send( "POS", msgout );

	/*
	// Prepare the message to send to the Position service
	CMessage msgout2( CNetManager::getSIDA( "MYN" ), "ADD_ENTITY" );
	msgout2.serial( id );
	msgout2.serial( name );
	msgout2.serial( race );
	msgout2.serial( start );

	CNetManager::send( "MYN", msgout2 );
	*/

	nlinfo( "Received ADD_ENTITY from the client");
}



/****************************************************************************
 * cdAddService
 *
 * Receive an ADD_ENTITY messages from the Position Service to send it to all
 * the clients.
 ****************************************************************************/
void cbAddService ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	bool    all;
	uint32  to;
	uint32  id;
	string  name;
	uint8   race;
	CVector start;

	// Input: process the reply of the position service
	msgin.serial( all );
	msgin.serial( to );
	msgin.serial( id );
	msgin.serial( name );
	msgin.serial( race );
	msgin.serial( start );

	/* disabled so as not to change the position service 
	// but relegate the add_entity responsibility to the mayan service
	// Output: prepare the reply to the clients
	CMessage msgout( CNetManager::getSIDA( "FS" ), "ADD_ENTITY" );
	msgout.serial( id );
	msgout.serial( 2007 ); // classId  
	msgout.serial( "gnu.shape" ); // Name
	msgout.serial( gnu.shape );; // Shape name
	msgout.serial( CVector(1.0f, 1.0f, 1.0f )); // Position
	msgout.serial( 180 ); // Heading
	msgout.serial( CVector(1.0f, 1.0f, 1.0f ) ); // massCenter
	msgout.serial( CVector(1.0f, 1.0f, 1.0f ) ); // dimensions)
	msgout.serial( 2 );
	msgout.serial( 60.5f );
	msgout.serial( 1.0f ) // scale
	nlinfo("finished send");


	if ( all == true )
	{
		// Send the message to all connected clients
		//CNetManager::send( "FS", msgout, 0 );

		nlinfo( "Sent ADD_ENTITY message to all the connected clients");
	}
	else
	{
		// Send the message about a former connected client to the new client
		_pmap::iterator ItPlayer;
		ItPlayer = localPlayers.find(to);
		if ( ItPlayer == localPlayers.end() )
		{
			nlinfo( "New player id %u not found !", to );
		}
		else
		{
			TSockId conToClient = ((*ItPlayer).second).con;
			CNetManager::send( "FS", msgout, conToClient );

			nlinfo( "Sent ADD_ENTITY about all the connected clients to the new client.");
		}
	}
	*/

}


/****************************************************************************
 * cbRemoveClient
 *
 * Receive an REMOVE_ENTITY message from a client and send it to the Position
 * Service.
 ****************************************************************************/
void cbRemoveClient ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  id;

	// Input from the client is stored.
	msgin.serial( id );

	// Prepare the message to send to the Position service
	CMessage msgout( CNetManager::getSIDA( "POS" ), "REMOVE_ENTITY" );
	msgout.serial( id );

	/*
	 * The incomming message from the client is sent to the Position service
	 * under the "POS" identification.
	 */
	CNetManager::send( "POS", msgout );

	nlinfo( "Received REMOVE_ENTITY from the client");
}


/****************************************************************************
 * cdRemoveService
 *
 * Receive an REMOVE_ENTITY messages from the Position Service to send it to all
 * the clients.
 ****************************************************************************/
void cbRemoveService ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32  id;

	// Input: process the reply of the position service
	msgin.serial( id );

	// Output: send the reply to the client
	CMessage msgout( CNetManager::getSIDA( "FS" ), "REMOVE_ENTITY" );
	msgout.serial( id );

	// Send the message to all connected clients
	CNetManager::send( "FS", msgout, 0 );

	nlinfo( "Sent REMOVE_ENTITY message to all the connected clients");
}


/****************************************************************************
 * cdSnowballService
 *
 * Receive an SNOWBALL messages from the Position Service to send it to all
 * the clients.
 ****************************************************************************/
void cbSnowballService ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32  id,
			playerId;
	CVector start,
			target;
	float   speed,
			explosionRadius;

	// Input: process the reply of the position service
	msgin.serial( id );
	msgin.serial( playerId );
	msgin.serial( start );
	msgin.serial( target );
	msgin.serial( speed );
	msgin.serial( explosionRadius );

	// Output: send the reply to the client
	CMessage msgout( CNetManager::getSIDA( "FS" ), "SNOWBALL" );
	msgout.serial( id );
	msgout.serial( playerId );
	msgout.serial( start );
	msgout.serial( target );
	msgout.serial( speed );
	msgout.serial( explosionRadius );

	// Send the message to all connected clients
	CNetManager::send( "FS", msgout, 0 );

	nlinfo( "Sent SNOWBALL message to all the connected clients");
}


/****************************************************************************
 * cbSnowballClient
 *
 * Receive an SNOWBALL message from a client and send it to the Position
 * Service.
 ****************************************************************************/
void cbSnowballClient ( CMessage& msgin, TSockId from, CCallbackNetBase& clientcb )
{
	uint32  playerId;
	CVector start,
			target;
	float   speed,
			explosionRadius;

	// Input from the client is stored.
	msgin.serial( playerId );
	msgin.serial( start );
	msgin.serial( target );
	msgin.serial( speed );
	msgin.serial( explosionRadius );

	// Prepare the message to send to the Position service
	CMessage msgout( CNetManager::getSIDA( "POS" ), "SNOWBALL" );
	msgout.serial( playerId );
	msgout.serial( start );
	msgout.serial( target );
	msgout.serial( speed );
	msgout.serial( explosionRadius );

	/*
	 * The incomming message from the client is sent to the Position service
	 * under the "POS" identification.
	 */
	CNetManager::send( "POS", msgout );

	nlinfo( "Received SNOWBALL from the client");
}


/****************************************************************************
 * cbHitService
 *
 * Receive an HIT messages from the Position Service to send it to all
 * the clients.
 ****************************************************************************/
void cbHitService ( CMessage& msgin, TSockId from, CCallbackNetBase& servercb )
{
	uint32  snowballId,
			victimId;
	bool    direct;

	// Input: process the reply of the position service
	msgin.serial( snowballId );
	msgin.serial( victimId );
	msgin.serial( direct );

	// Output: send the reply to the client
	CMessage msgout( CNetManager::getSIDA( "FS" ), "HIT" );
	msgout.serial( snowballId );
	msgout.serial( victimId );
	msgout.serial( direct );

	// Send the message to all connected clients
	CNetManager::send( "FS", msgout, 0 );

	nlinfo( "Sent HIT message to all the connected clients");
}

/****************************************************************************
 * cbAddSpellFX
 *
 * Receive an add spell messages from a client to send it to all
 * the clients.
 ****************************************************************************/
void cbAddSpellFX(CMessage &msgin, TSockId from, CCallbackNetBase clientcb)
{
	uint32	playerId;
	std::string psName, soundName;

	msgin.serial( playerId );
	msgin.serial( psName );
	msgin.serial( soundName );

	CMessage msgout( CNetManager::getSIDA( "FS" ), "ADD_SPELL_FX" );

	msgout.serial( playerId );
	msgout.serial( psName );
	msgout.serial( soundName );

	CNetManager::send( "FS", msgout, 0 );
}

/****************************************************************************
 * cbRemoveSpellFX
 *
 * Receive an remove spell messages from a client to send it to all
 * the clients.
 ****************************************************************************/
void cbRemoveSpellFX(CMessage &msgin, TSockId from, CCallbackNetBase clientcb)
{
	uint32	playerId;
	msgin.serial( playerId );

	CMessage msgout( CNetManager::getSIDA( "FS" ), "REMOVE_SPELL_FX" );

	msgout.serial( playerId );

	CNetManager::send( "FS", msgout, 0 );
}

/****************************************************************************
 * cbAddEnvFX
 *
 * Receive an add env fx messages from a client to send it to all
 * the clients.
 ****************************************************************************/
void cbAddEnvFX(CMessage &msgin, TSockId from, CCallbackNetBase clientcb)
{
	uint32	playerId;
	std::string name, psName, soundName;

	msgin.serial( playerId );
	msgin.serial( name );
	msgin.serial( psName );
	msgin.serial( soundName );

	CMessage msgout( CNetManager::getSIDA( "FS" ), "ADD_ENV_FX" );

	msgout.serial( playerId );
	msgout.serial( name );
	msgout.serial( psName );
	msgout.serial( soundName );

	CNetManager::send( "FS", msgout, 0 );
}

/****************************************************************************
 * cbRemoveEnvFX
 *
 * Receive an remove env fx messages from a client to send it to all
 * the clients.
 ****************************************************************************/
void cbRemoveEnvFX(CMessage &msgin, TSockId from, CCallbackNetBase clientcb)
{
	uint32	playerId;
	string name;

	msgin.serial( playerId );

	CMessage msgout( CNetManager::getSIDA( "FS" ), "REMOVE_ENV_FX" );

	msgout.serial( playerId );

	CNetManager::send( "FS", msgout, 0 );
}



/****************************************************************************
 * ClientCallbackArray
 *
 * It define the functions to call when receiving a specific message from the
 * client
 ****************************************************************************/
TCallbackItem ClientCallbackArray[] =
{
	{ "ADD_ENTITY",		cbAddClient      },
	{ "POSSESS",		cbPossess		 },
	{ "ENTITY_POS",		cbPosClient      },
	{ "CHAT",			cbChatClient     },
	{ "REMOVE_ENTITY",	cbRemoveClient   },
	{ "SNOWBALL",		cbSnowballClient },
	{ "DROP_DO",		cbDropClient		},
	{ "PICK_DO",		cbPickClient		},
	{ "INV_SELECT",		cbInvSelectClient },
	{ "HOLD_DO",		cbHoldClient },
	{ "GIVE_DO",		cbGiveClient },
	{ "ADD_SPELL_FX",	cbAddSpellFX },
	{ "REMOVE_SPELL_FX", cbRemoveSpellFX },
	{ "ADD_ENV_FX",		cbAddEnvFX },
	{ "REMOVE_ENV_FX",	cbRemoveEnvFX }
};


/****************************************************************************
 * ChatCallbackArray
 *
 * It define the functions to call when receiving a specific message from
 * the Chat service
 ****************************************************************************/
TCallbackItem ChatCallbackArray[] =
{
	{ "CHAT", cbChatService }
};


/****************************************************************************
 * PosCallbackArray
 *
 * It define the functions to call when receiving a specific message from
 * the Position service
 ****************************************************************************/
TCallbackItem PosCallbackArray[] =
{
	{ "ADD_ENTITY",    cbAddService      },
	{ "ENTITY_POS",    cbPosService      },
	{ "REMOVE_ENTITY", cbRemoveService   },
	{ "SNOWBALL",      cbSnowballService },
	{ "HIT",           cbHitService }
};

/****************************************************************************
 * MayanCallbackArray
 *
 * It define the functions to call when receiving a specific message from
 * the Mayan service
 ****************************************************************************/
TCallbackItem MayanCallbackArray[] =
{
	{ "ADD_MAYAN",		cbAddMayan },
	{ "DROP_ENTITY",	cbDropService },
	{ "REMOVE_ENTITY",	cbRemoveService },
	{ "INIT_INV",		cbInitInv },
	{ "INIT_STATS",		cbInitStats },
	{ "INIT_SKILLS",	cbInitSkills },
	{ "CH_STAT",		cbUpdateStats },
	{ "CH_SKILL",		cbUpdateStats },
	{ "CH_INV",			cbUpdateInv },
	{ "ADD_DO",			cbAddDo },
	{ "ADD_FIREARM",	cbAddFirearm },
	{ "ADD_BOXOFBULLETS", cbAddBoxOfBullets },
	{ "ADD_NUTRITION",  cbAddNutrition },
	{ "CH_ENTITY_HAND", cbChangeEntityHand },
	{ "ADD_PROJECTILE",	cbAddProjectile },
	{ "MISFIRE",		cbMisfire },
	{ "ADD_SPOTSOUND",	cbAddSpotSound },
	{ "GIVE_TO_SHA",	cbGiveMayan }
};

/****************************************************************************
 * ShamanCallbackArray
 *
 * It defines the functions to call when receiving a specific message from the
 * shaman service
 ****************************************************************************/
TCallbackItem ShamanCallbackArray[] =
{
	{ "ADD_SHAMAN",		cbAddMayan      },
//	{ "ENTITY_POS",		cbPosShaman      },
//	{ "CHAT",			cbChatShaman     },
//	{ "REMOVE_SHAMAN",	cbRemoveShaman   },
//	{ "DROP_DO",		cbDropClient	 },
//	{ "PICK_DO",		cbPickClient	 },
//	{ "INV_SELECT",		cbInvSelectClient},
//	{ "HOLD_DO",		cbHoldShaman	 },
	{ "GIVE_DO",		cbGiveShaman	 }
};

/****************************************************************************
 * Connection callback for the Chat service
 ****************************************************************************/
void onReconnectChat ( const std::string &serviceName, TSockId from, void *arg )
{
	nlinfo( "Chat Service reconnected" );
}


/****************************************************************************
 * Disonnection callback for the Chat service
 ****************************************************************************/
void onDisconnectChat ( const std::string &serviceName, TSockId from, void *arg )
{
	/* Note: messages already forwarded should get no reply, but it may occure
	 * (e.g. if the server reconnects before the forwarding of a message and
	 * the reconnection callbacks is called after that). Then onReconnectChat()
	 * may send messagess that have already been sent and the front-end may get
	 * the same message twice. This is partially handled in cbChatService.
	 */

	nlinfo( "Chat Service disconnecting: messages will be delayed until reconnection" );
}


/****************************************************************************
 * Connection callback for the Position service
 ****************************************************************************/
void onReconnectPosition ( const std::string &serviceName, TSockId from, void *arg )
{
	nlinfo( "Position Service reconnected" );
}


/****************************************************************************
 * Disonnection callback for the Position service
 ****************************************************************************/
void onDisconnectPosition ( const std::string &serviceName, TSockId from, void *arg )
{
	/* Note: messages already forwarded should get no reply, but it may occure
	 * (e.g. if the server reconnects before the forwarding of a message and
	 * the reconnection callbacks is called after that). Then onReconnectChat()
	 * may send messagess that have already been sent and the front-end may get
	 * the same message twice. This is partially handled in cbPositionService.
	 */

	nlinfo( "Position Service disconnecting: messages will be delayed until reconnection" );
}

/****************************************************************************
 * Connection callback for the Mayan service
 ****************************************************************************/
void onReconnectMayan ( const std::string &serviceName, TSockId from, void *arg )
{
	nlinfo( "Mayan Service reconnected" );
}


/****************************************************************************
 * Disonnection callback for the Mayan service
 ****************************************************************************/
void onDisconnectMayan ( const std::string &serviceName, TSockId from, void *arg )
{
	/* Note: messages already forwarded should get no reply, but it may occure
	 * (e.g. if the server reconnects before the forwarding of a message and
	 * the reconnection callbacks is called after that). Then onReconnectChat()
	 * may send messagess that have already been sent and the front-end may get
	 * the same message twice. This is partially handled in cbPositionService.
	 */

	nlinfo( "Mayan Service disconnecting: messages will be delayed until reconnection" );
}
/****************************************************************************
 * Connection callback for the Shaman service
 ****************************************************************************/
void onReconnectShaman ( const std::string &serviceName, TSockId from, void *arg )
{
	nlinfo( "Shaman Service reconnected" );
}


/****************************************************************************
 * Disonnection callback for the Shaman service
 ****************************************************************************/
void onDisconnectShaman ( const std::string &serviceName, TSockId from, void *arg )
{
	/* Note: messages already forwarded should get no reply, but it may occure
	 * (e.g. if the server reconnects before the forwarding of a message and
	 * the reconnection callbacks is called after that). Then onReconnectChat()
	 * may send messagess that have already been sent and the front-end may get
	 * the same message twice. This is partially handled in cbPositionService.
	 */

	nlinfo( "Shaman Service disconnecting: messages will be delayed until reconnection" );
}

/****************************************************************************
 * Connection callback for a client
 ****************************************************************************/
void onConnectionClient (TSockId from, const CLoginCookie &cookie)
{
	uint32 id;

	id = cookie.getUserId();

	nlinfo( "The client with uniq Id %u is connected", id );

	// Add new client to the list of player managed by this FrontEnd
	pair<_pmap::iterator, bool>
		player = localPlayers.insert( _pmap::value_type( id,
														 _player( id,
																  from )));
	pair<_psmap::iterator, bool>
		playerSock = localPlayersSocket.insert( _psmap::value_type( from,
														 _playerSocket( from,
																  id )));
	// store the player info in appId

	_pmap::iterator it = player.first;
	_player *p = &((*it).second);
	from->setAppId((uint64)(uint)p);

	// Output: send the IDENTIFICATION number to the new connected client
	CMessage msgout( CNetManager::getSIDA( "FS" ), "IDENTIFICATION" );
	msgout.serial( id );

	// Send the message to connected client "from"
	CNetManager::send( "FS", msgout, from );

	nlinfo( "Sent IDENTIFICATION message to the new client");
}


/****************************************************************************
 * Disonnection callback for a client
 ****************************************************************************/
void onDisconnectClient (const std::string &serviceName, TSockId from, void *arg)
{
	uint32 id;

	uint64 i = from->appId();
	_player *p = (_player *)(uint)i;
	id = p->id;

	nlinfo ("A client with uniq Id %u has disconnected", id );

	// tell the login system that this client is disconnected
	CLoginServer::clientDisconnected ( id );

	// remove the player from the local player list
	localPlayers.erase( id );

	// Output: send the REMOVE_ENTITY to the position manager.
	CMessage msgout( CNetManager::getSIDA( "POS" ), "REMOVE_ENTITY" );
	msgout.serial( id );

	// Send the message to the position manager
	CNetManager::send( "POS", msgout);

	nlinfo( "Sent REMOVE_ENTITY message to the position manager.");
}


/****************************************************************************
 * CFrontEndService
 ****************************************************************************/
class CFrontEndService : public IService
{
public:

	// Initialisation
	void init()
	{
		DebugLog->addNegativeFilter ("NETL");

		// Connect the frontend to the login system
		CLoginServer::init( *getServer(), onConnectionClient, &ConfigFile); 

		/********************************************************************
		 * Client connection management
		 */

		// Set the callbacks for the client disconnection of the Frontend
		CNetManager::setDisconnectionCallback( "FS", onDisconnectClient, NULL );

		/********************************************************************
		 * Chat Service connection management
		 */

		 // Connect (as a client) to the Chat Service (as a server)
		CNetManager::addClient( "CHAT" );

		// Set the callbacks for that connection (comming from the Chat service)
		CNetManager::addCallbackArray( "CHAT",
									   ChatCallbackArray,
									   sizeof(ChatCallbackArray)/sizeof(ChatCallbackArray[0]) );

		/*
		 * Set the callback function when the Chat service reconnect to the
		 * frontend
		 */
		CNetManager::setConnectionCallback( "CHAT", onReconnectChat, NULL );

		/*
		 * Set the callback function when the Chat service disconnect from
		 * frontend
		 */
		CNetManager::setDisconnectionCallback( "CHAT", onDisconnectChat, NULL );

		/********************************************************************
		 * Position Service connection management
		 */

		// Connect (as a client) to the Position Service (as a server)
		CNetManager::addClient( "POS" );

		/*
		 * Set the callbacks for that connection (comming from the Position
		 * service)
		 */
		CNetManager::addCallbackArray( "POS",
									   PosCallbackArray,
									   sizeof(PosCallbackArray)/sizeof(PosCallbackArray[0]) );

		/*
		 * Set the callback function when the Position service reconnect to the
		 * frontend
		 */
		CNetManager::setConnectionCallback( "POS", onReconnectPosition, NULL );

		/*
		 * Set the callback function when the Position service disconnect from
		 * frontend
		 */
		CNetManager::setDisconnectionCallback( "POS", onDisconnectPosition, NULL );
		
		/********************************************************************
		 * Mayan Service connection management
		 */

		// Connect (as a client) to the Mayan Service (as a server)
		CNetManager::addClient( "MYN" );

		/*
		 * Set the callbacks for that connection (comming from the Mayan
		 * service)
		 */
		CNetManager::addCallbackArray( "MYN",
									   MayanCallbackArray,
									   sizeof(MayanCallbackArray)/sizeof(MayanCallbackArray[0]) );

		/*
		 * Set the callback function when the Mayan service reconnect to the
		 * frontend
		 */
		CNetManager::setConnectionCallback( "MYN", onReconnectMayan, NULL );

		/*
		 * Set the callback function when the Mayan service disconnect from
		 * frontend
		 */
		CNetManager::setDisconnectionCallback( "MYN", onDisconnectMayan, NULL );
		
		/********************************************************************
		 * Shaman Service connection management
		 */

		// Connect (as a client) to the Shaman Service (as a server)
		CNetManager::addClient( "SHA" );

		/*
		 * Set the callbacks for that connection (comming from the Shaman
		 * service)
		 */
		CNetManager::addCallbackArray( "SHA",
									   ShamanCallbackArray,
									   sizeof(ShamanCallbackArray)/sizeof(ShamanCallbackArray[0]) );

		/*
		 * Set the callback function when the Shaman service reconnect to the
		 * frontend
		 */
		CNetManager::setConnectionCallback( "SHA", onReconnectShaman, NULL );

		/*
		 * Set the callback function when the Shaman service disconnect from
		 * frontend
		 */
		CNetManager::setDisconnectionCallback( "SHA", onDisconnectShaman, NULL );

	}
};


/****************************************************************************   
 * SNOWBALLS FRONTEND SERVICE MAIN Function
 *
 * This call create a main function for a service:
 *
 *    - based on the "CFrontEndService" class
 *    - having the short name "FS"
 *    - having the long name "frontend_service"
 *    - listening on the port "37000"
 *    - and callback actions set to "ClientCallbackArray"
 *
 ****************************************************************************/
NLNET_SERVICE_MAIN( CFrontEndService,
					"FS",
					"frontend_service",
					37000,
					ClientCallbackArray )


/* end of file */

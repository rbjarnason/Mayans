/** \file network.h
 * Network interface between the game and NeL
 *
 * $Id: network.h,v 1.4 2002/04/05 03:58:45 robofly Exp $
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

#ifndef NETWORK_H
#define NETWORK_H

//
// Includes
//  s

#include <string>

#include <nel/misc/time_nl.h>

#include "entities.h"
#include "client_world_objects.h"


//
// External definitions
//

namespace NLMISC
{
	class CVector;
}

namespace NLNET
{
	class CCallbackClient;
}

//
// External variables
//

// Pointer to the connection to the server
extern NLNET::CCallbackClient	*Connection;

//
// External functions
//

// Return true if the client is online
bool	isOnline ();

// Send the new entity (the player)
void	sendAddEntity (uint32 id, std::string &name, uint8 race, NLMISC::CVector &startPosition, std::string shapeName);

// Send a chat line to the server
void	sendChatLine (std::string Line);

// Send the user entity position to the server
void	sendEntityPos (CEntity &entity);

// Inform the server that it is sending position info
// to us for an entity that is not in the scene
void	sendEntityMissing (uint32 id);

// Send a new snowball to the server
void	sendSnowBall (uint32 eid, const NLMISC::CVector &position, const NLMISC::CVector &target, float speed, float deflagRadius);

// Send a new snowball to the server
void	sendFire1 (const NLMISC::CVector &target, float angle, float pitch);

// Send selected inventory item to the server
void	sendSelectedInInventory (uint32 did);

// Give dynamic object to selected player
void    sendGive (uint32 targetId);

// Send the Pick Dynamic Object to the server
void	sendPickDynamicObject (uint32 playerId, uint32 dynamicObjectId);

// Send the Drop Dynamic Object to the server
void	sendDropDynamicObject (uint32 playerId, uint32 dynamicObjectId);

// Send the Hold Dynamic Object to the server
void	sendHoldDynamicObject (uint32 dynamicObjectId, bool leftHand);

// Send a request for a new SpellFX
void	sendAddSpellFX(uint32 playerId, std::string psName, std::string soundName);

// Send a request for removing a SpellFX
void	sendRemoveSpellFX(uint32 playerId);

// Send a request for an Environmental FX
void 	sendAddEnviromentalFX(uint32 playerId, std::string name, std::string psName, std::string soundName);

// Send a request for removing an Environmental FX
void	sendRemoveEnviromentalFX(uint32 playerId);

// Send summon A mammoth...
void	sendSummonMammoth (uint32 mammothId);

// Send summon A shaman...
void	sendSummonShaman (uint32 shamanId);

// Send start and end pathTrace
void	sendStartPathTrace ();
void	sendEndPathTrace (string pathName);
void	sendWalkPath (uint32 walkerId, string pathName);

void	initNetwork ();
void	updateNetwork ();
void	releaseNetwork ();

#endif // NETWORK_H

/* End of network.h */

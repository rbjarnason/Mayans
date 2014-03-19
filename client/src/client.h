/** \file client.h
 * Snowballs 2 main file
 *
 * $Id: client.h,v 1.2 2002/06/06 05:51:01 robofly Exp $
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

#ifndef CLIENT_H
#define CLIENT_H

//
// Includes
//

#include <nel/misc/config_file.h>
#include <nel/misc/time_nl.h>

//
// External definitions
//

class C3dMouseListener;

namespace NL3D
{
	class UDriver;
	class UScene;
	class UTextContext;
}

//
// External variables
// 

extern NLMISC::CConfigFile		 ConfigFile;

extern NL3D::UDriver			*Driver;
extern NL3D::UScene				*Scene;
extern NL3D::UTextContext		*TextContext;
extern C3dMouseListener			*MouseListener;

extern NLMISC::TTime			 LastTime, NewTime;

// Game mode defintions
extern enum GameModes	{ Exploring, Communication, Combat };

// The current active game mode
extern GameModes					GameMode;
extern bool							Falling;
extern bool							LoginComplete;
extern bool							ShowCommands;
extern bool							ShowDebugInChat;

#endif // CLIENT_H

/* End of client.h */

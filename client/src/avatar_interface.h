/** \avatar_interface.h
 * <File description>
 *
 * $Id: avatar_interface.h,v 1.3 2002/03/30 05:06:33 robofly Exp $
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


#ifndef AVATAR_INTERFACE_H
#define AVATAR_INTERFACE_H

//
// External variables
//

extern uint AvatarInterfaceState;

extern NL3D::UInstance					*MaleInstance;
extern NL3D::USkeleton					*MaleSkeleton;

extern NL3D::UInstance					*FemaleInstance;
extern NL3D::USkeleton					*FemaleSkeleton;

//
// External functions
//

void initAvatarInterface ();
void updateAvatarInterface ();
void releaseAvatarInterface ();

void nextAvatar();
void selectCurrentAvatar();

#endif // AVATAR_INTERFACE_H

/* End of AVATAR_INTERFACE.h */

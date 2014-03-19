/** \ode_interface.h
 * <File description>
 *
 * $Id: ode_interface.h,v 1.1 2002/03/20 18:05:25 robofly Exp $
 */

/* Copyright, 2001 InOrbit Entertainment, Inc.
 *
 *
 * This file is part of WARP STORM (alpha).
 * WARP STORM (alpha) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the 
 * Free Software Foundation; either version 2, or (at your option) any later 
 * version.
 *
 * WARP STORM (alpha) is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.

 * You should have received a copy of the GNU General Public License along 
 * with WARP STORM (alpha); see the file COPYING. If not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */


#ifndef NL_ODE_INTERFACE_H
#define NL_ODE_INTERFACE_H

#define dDOUBLE

//
// External functions
//

void initODEInterface ();
void updateODEInterface ();
void releaseODEInterface ();

void setBuggySpeed(float speed);
void setBuggySteer(float steer);

#endif // NL_ODE_INTERFACE_H
 
/* End of ode_interface.h */


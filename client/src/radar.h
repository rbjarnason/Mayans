/** \file radar.h
 * Snowballs 2 specific code for managing the radar.
 * This code was taken from Snowballs 1.
 *
 * $Id: radar.h,v 1.3 2002/04/17 03:39:30 robofly Exp $
 */

/* Copyright, 2001 Nevrax Ltd.
 *
 * This file is part of NEVRAX SNOWBALLS.
 * NEVRAX SNOWBALLS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * NEVRAX SNOWBALLS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with NEVRAX SNOWBALLS; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef RADAR_H
#define RADAR_H

//
// External variables
//

extern uint RadarDistance;
extern uint RadarState;
extern string SelectedURL;

//
// External functions
//

void initRadar ();
void updateRadar ();
void releaseRadar ();
void selectClosestURL ();
void selectNextURL ();

#endif // RADAR_H

/* End of radar.h */

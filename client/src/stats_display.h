/** \statsdisplay.h
 * <File description>
 *
 * $Id: stats_display.h,v 1.1 2002/03/20 18:05:25 robofly Exp $
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


#ifndef STATS_DISPLAY_H
#define STATS_DISPLAY_H

#include <string>

#include <nel/misc/rgba.h>
#include "stats.h"


//
// External variables
//

extern CStats CharacterStats;
extern uint StatsDisplayState;


//
// External functions
//

void initStatsDisplay ();
void updateStatsDisplay ();
void releaseStatsDisplay ();

#endif // STATS_DISPLAY_H

/* End of stats_display.h */

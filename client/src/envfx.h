/** \file envfx.h
 * Environmental FX Manager
 *
 * $Id: envfx.h,v 1.1 2002/03/20 18:05:25 robofly Exp $
 */

/* Copyright, 2002 InOrbit Entertainment, Inc.
 *
 * This file is part of BANG.
 * BANG is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * BANG is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with BANG; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef ENVFX_H
#define ENVFX_H

//
// External definitions
//

namespace NLSOUND
{
	class USource;
}


//
// External variables
//

//
// External functions
//

void	initEnvFX();
void	updateEnvFX();
void	releaseEnvFX();

void	toggleRain();
bool	isRaining();
void	rainOn(std::string psName, std::string soundName);
void	rainOff();

#endif // ENVFX_H

/* End of envfx.h */

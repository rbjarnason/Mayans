/** \file physics.cpp
 * Snowballs trajectory computation
 *
 * $Id: physics.cpp,v 1.1 2002/03/20 18:05:25 robofly Exp $
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

#include "physics.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;

CVector	CTrajectory::eval(NLMISC::TTime t) const
{
	if (t < _StartTime) return _StartPosition;

	float	ft = (float)(t-_StartTime)/(float)(_StopTime-_StartTime);
	ft = min(ft, 1.0f);
	CVector	res = _EndPosition*ft + _StartPosition*(1.0f-ft);
	if (_Type==0) res.z += 0.3f*_Distance*_Distance/90.0f*(float)sin(Pi*ft);
	return res;
}

CVector	CTrajectory::evalSpeed(NLMISC::TTime t) const
{
	if (t < _StartTime) return _StartPosition;

	float	ft = (float)(t-_StartTime)/(float)(_StopTime-_StartTime);
	ft = min(ft, 1.0f);
	CVector res = (_EndPosition-_StartPosition).normed()*_Speed;
	if (_Type==0) res.z += 0.3f*_Distance*_Distance/90.0f*(float)Pi*(float)cos(Pi*ft);
	return res;
}

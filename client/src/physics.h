/** \file physics.h
 * Physics computation for particles (and thus snowballs)
 *
 * $Id: physics.h,v 1.1 2002/03/20 18:05:25 robofly Exp $
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

#ifndef PHYSICS_H
#define PHYSICS_H

//
// Includes
//

#include <nel/misc/vector.h>
#include <nel/misc/time_nl.h>

//
// External classes
//

class CTrajectory
{
	NLMISC::CVector		_StartPosition;
	NLMISC::CVector		_EndPosition;
	float				_Speed;
	NLMISC::TTime		_StartTime;
	NLMISC::TTime		_StopTime;
	float				_Distance;

	uint8				_Type;

		// The state enum of the entity

public:

	void				init(const NLMISC::CVector &position, const NLMISC::CVector &target, float speed, NLMISC::TTime startTime, uint8 type)
	{
		_StartPosition = position;
		_EndPosition = target;
		_Speed = speed;
		_StartTime = startTime;
		_Distance = (_EndPosition-_StartPosition).norm();
		_StopTime = (NLMISC::TTime)(_Distance/_Speed*1000.0+_StartTime);
		_Type = type;
//			nlinfo(" TRAJ created with type = %d and speed = ", type, speed);

	}

//	void				compute(const NLMISC::CVector &position, const NLMISC::CVector &target, float speed, NLMISC::TTime startTime);

	NLMISC::CVector		eval(NLMISC::TTime t) const;
	NLMISC::CVector		evalSpeed(NLMISC::TTime t) const;

	NLMISC::TTime		getStartTime() const { return _StartTime; }
	NLMISC::CVector		getStartPosition() const { return _StartPosition; }
	NLMISC::TTime		getStopTime() const { return _StopTime; }
};

#endif // PHYSICS_H

/* End of physics.h */

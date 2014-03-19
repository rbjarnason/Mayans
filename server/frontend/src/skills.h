/** \file skills.h
 * <File description>
 *
 * $Id: skills.h,v 1.1 2002/03/20 18:10:21 robofly Exp $
 */

/* Copyright, 2001 InOrbit Entertainment, Inc.
 *
 * This file is part of NEVRAX NEL.
 * NEVRAX NEL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * NEVRAX NEL is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with NEVRAX NEL; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef NL_SKILLS_H
#define NL_SKILLS_H


#include "nel/misc/types_nl.h"
#include "nel/misc/smart_ptr.h"
#include "nel/misc/matrix.h"
#include "nel/misc/stream.h"
#include "nel/misc/debug.h"
#include <stdio.h>
#include <vector>
#include <map>

using namespace std;


// ***************************************************************************
/**
 * Keeps track of character skills, is serialized into files
 * Called 
 * \author Róbert Bjarnason
 * \author InOrbit Entertainment, Inc.
 * \date 2000
 */
class CSkills
{


public:

	/// Constructor
	CSkills()
	{
		_I16Skills.resize(151);
		_I16Tallies.resize(151);
	};

	~CSkills() {}


	void				createFromConfigFile( std::string configFileName );

	void				load( std::string configFileName );

	void				serializeIfUpdated();

	void				serialize();

	void				createFromTemplate(std::string templateFileName, std::string newFile);

	void				serial(NLMISC::IStream &f);

	void				update(uint8 skillId, uint16 value);
	void				updateTally(uint8 skillId, uint16 value);

	vector<uint16>			geti16() const { return _I16Skills; }
	vector<uint16>			gett16() const { return _I16Tallies; }

private:
	
	vector<uint16>				_I16Skills;
	vector<uint16>				_I16Tallies;
	void						save( std::string configFileName );
	std::string					_MyName;
	bool						_Updated;
};


#endif // NL_SKILLS_H

/* End of skills.h */


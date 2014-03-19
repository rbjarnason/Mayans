/** \file interface.cpp
 * Snowballs 2 specific code for managing interface
 *
 * $Id: interface.cpp,v 1.1 2002/03/20 18:05:25 robofly Exp $
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

//
// Includes
//

#include <math.h>
#include <nel/misc/vectord.h>
#include <nel/misc/event_listener.h>

#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_3d_mouse_listener.h>
#include <nel/3d/u_material.h>
#include <nel/3d/u_landscape.h>

#include "client.h"
#include "interface.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NL3D;

//
// Variables
//

static string QueryString, AnswerString;
static float DisplayWidth;
static float DisplayHeight, nbLines;
static CRGBA DisplayColor;

static sint Prompt;

static uint FontSize = 20;

//
// Functions
//

// Class that handle the keyboard input when an interface is displayed (login request for example)
class CInterfaceListener : public IEventListener
{
	virtual void	operator() ( const CEvent& event )
	{
		// ignore keys if interface is not open
		if (!interfaceOpen ()) return;

		CEventChar &ec = (CEventChar&)event;

		// prompt = 2 then we wait a any user key
		if (Prompt == 2)
		{
			QueryString = "";
			AnswerString = "";
			_Line = "";
			return;
		}

		switch ( ec.Char )
		{
		case 13 : // RETURN : Send the chat message
			
			if (_Line.size() == 0)
				break;

			QueryString = "";
			AnswerString = _Line;

			_LastCommand = _Line;
			_Line = "";
			_MaxWidthReached = false;
			break;

		case 8 : // BACKSPACE
			if ( _Line.size() != 0 )
			{
				_Line.erase( _Line.end()-1 );
				// _MaxWidthReached = false; // no need
			}
			break;
		
		case 27 : // ESCAPE
			QueryString = "";
			AnswerString = "";
			_LastCommand = _Line;
			_Line = "";
			_MaxWidthReached = false;
			break;

		default: 
			
			if ( ! _MaxWidthReached )
			{
				_Line += (char)ec.Char;
			}
		}
	}

public:
	CInterfaceListener() : _MaxWidthReached( false )
	{}

	const string&	line() const
	{
		return _Line;
	}

 	void setLine(const string &str) { _Line = str; }

	void			setMaxWidthReached( bool b )
	{
		_MaxWidthReached = b;
	}

private:
	string			_Line;
	bool			_MaxWidthReached;
	string			_LastCommand;
};

// Instance of the listener
static CInterfaceListener InterfaceListener;


void	initInterface()
{
	// Add the keyboard listener in the event server
	Driver->EventServer.addListener (EventCharId, &InterfaceListener);
}

void	updateInterface()
{
	if (QueryString.empty()) return;

	// Draw background
	Driver->setMatrixMode2D11 ();
	Driver->drawQuad (0.5f-DisplayWidth/2.0f, 0.5f-DisplayHeight/2.0f, 0.5f+DisplayWidth/2.0f, 0.5f+DisplayHeight/2.0f, DisplayColor);

	// Set the text context
	TextContext->setHotSpot (UTextContext::MiddleMiddle);
	TextContext->setColor (CRGBA (255,255,255,255));
	TextContext->setFontSize (FontSize);

	// Display the text
	string str;
	float y = (nbLines * FontSize / 2.0f) / 600.0f;

	for (uint i = 0; i < QueryString.size (); i++)
	{
		if (QueryString[i] == '\n')
		{
			TextContext->printfAt (0.5f, 0.5f+y, str.c_str());
			str = "";
			y -= FontSize / 600.0f;
		}
		else
		{
			str += QueryString[i];
		}
	}
	TextContext->printfAt (0.5f, 0.5f+y, str.c_str());

	// Display the user input line
	y-= (2.0f * FontSize) / 600.0f;
	string str2;
	switch (Prompt)
	{
	case 0:
		str2 = InterfaceListener.line();
		str2 += "_";
		break;
	case 1:
		str2.resize (InterfaceListener.line().size (), '*');
		str2 += "_";
		break;
	case 2:
		str2 = "";
		break;
	}
	TextContext->printfAt (0.5f, 0.5f+y, str2.c_str());
}

void	releaseInterface()
{
	// Rmove the keyboard listener from the server
	Driver->EventServer.removeListener (EventCharId, &InterfaceListener);
}

void	askString (const string &queryString, const string &defaultString, sint prompt, const CRGBA &color)
{
	QueryString = queryString;
	
	if (prompt == 2)
		QueryString += "\n\n(Press any key to continue)";
	else
		QueryString += "\n\n(Press enter to validate and ESC to cancel)";

	nbLines = 1;
	for (uint i = 0; i < QueryString.size (); i++)
	{
		if (QueryString[i] == '\n') nbLines++;
	}

	DisplayWidth = float(queryString.size () * FontSize) / 600.0f;
	DisplayHeight = float((nbLines + 4) * FontSize) / 600.0f;
	DisplayColor = color;

	InterfaceListener.setLine (defaultString);
	Prompt = prompt;
}

bool	haveAnswer (string &answer)
{
	bool haveIt = !AnswerString.empty();
	if (haveIt)
	{
		answer = AnswerString;
		AnswerString = "";
	}
	return haveIt;
}

bool	interfaceOpen ()
{
	return !QueryString.empty();
}

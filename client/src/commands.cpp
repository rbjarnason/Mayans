/** \file commands.cpp
 * Snowballs 2 specific code for managing the command interface
 *
 * $Id: commands.cpp,v 1.1 2002/03/20 18:05:25 robofly Exp $
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

#include <list> 

#include <nel/misc/types_nl.h>
#include <nel/misc/event_listener.h>
#include <nel/misc/command.h>
#include <nel/misc/log.h>
#include <nel/misc/displayer.h>

#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_3d_mouse_listener.h>
#include <nel/3d/u_material.h>
#include <nel/3d/u_landscape.h>

#include "network.h"
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

CLog CommandsLog;

static list <string> StoredLines;
static uint32 NbStoredLines = 100;

// These variables are automatically set with the config file

static float CommandsBoxX, CommandsBoxY, CommandsBoxWidth;
static float CommandsBoxBorder;
static int CommandsNbLines;
static float CommandsLineHeight;
static int CommandsFontSize;
static CRGBA CommandsBackColor, CommandsFrontColor;

//
// Functions
//

// Display a string to the commands interface
void addLine (const string &line)
{
	// Add the line
	StoredLines.push_back (line);

	// Clear old lines if too much lines are stored
	while (StoredLines.size () > NbStoredLines)
	{
		StoredLines.pop_front ();
	}
}

// Display used to display on the commands interface
class CCommandsDisplayer : public IDisplayer
{
	virtual void doDisplay (const TDisplayInfo &args, const char *message)
	{
		bool needSpace = false;
		stringstream ss;

		if (args.LogType != CLog::LOG_NO)
		{
			ss << logTypeToString(args.LogType);
			needSpace = true;
		}

		if (needSpace) { ss << ": "; needSpace = false; }

		ss << message;

		addLine (ss.str());
	}
};

// Instance of the displayer
static CCommandsDisplayer CommandsDisplayer;

// Check if the user line is a command or not (a commands precede by a '/')
bool commandLine (const string &str)
{
	string command = "";

	if (str[0]=='/')
	{
		// If it's a command call it
		command = str.substr(1);
		// add the string in to the chat
		addLine (string ("command> ") + str);
		ICommand::execute (command, CommandsLog);
		return true;
	}
	else
	{
		return false;
	}
}

// Manage the user keyboard input
class CCommandsListener : public IEventListener
{
	virtual void	operator() ( const CEvent& event )
	{
		// If the interface is open, ignore keys for the command interface
		if (interfaceOpen ()) return;

		// If the communication interface is not open, ignore keys for the command interface
		if (GameMode!=Communication) return;

		// Get the key
		CEventChar &ec = (CEventChar&)event;

		switch ( ec.Char )
		{
		case 13 : // RETURN : Send the chat message
			
			// If the line is empty, do nothing
			if ( _Line.size() == 0 ) break;

			// If it's a command, execute it and don't send the command to the network
			if ( ! commandLine( _Line ) )
			{
				// If online, send the chat line, otherwise, locally displays it
				if (isOnline ())
					sendChatLine (_Line);
				else
					addLine (string ("you said> ") + _Line);
			}
			// Reset the command line
			_LastCommand = _Line;
			_Line = "";
			_MaxWidthReached = false;
			break;

		case 8 : // BACKSPACE : remove the last character

			if ( _Line.size() != 0 )
			{
				_Line.erase( _Line.end()-1 );
			}
			break;

		case 9 : // TAB : If it's a command, try to auto complete it
			
			if (_Line.empty())
			{
				_Line = _LastCommand;
			}
			else if (!_Line.empty() && _Line[0] == '/')
			{
				string command = _Line.substr(1);
				ICommand::expand(command);
				_Line = '/' + command;
			}
			break;

		case 27 : // ESCAPE : clear the command
		
			_Line = "";
			_MaxWidthReached = false;
			break;

		default: // OTHERWISE : add the character to the line

			if (! _MaxWidthReached)
			{
				_Line += (char)ec.Char;
			}
		}
	}

public:
	CCommandsListener() : _MaxWidthReached( false )
	{}

	const string&	line() const
	{
		return _Line;
	}

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
static CCommandsListener CommandsListener;

// This functions is automatically called when the config file changed (dynamically)
void cbUpdateCommands (CConfigFile::CVar &var)
{
	if (var.Name == "CommandsBoxX") CommandsBoxX = var.asFloat ();
	else if (var.Name == "CommandsBoxY") CommandsBoxY = var.asFloat ();
	else if (var.Name == "CommandsBoxWidth") CommandsBoxWidth = var.asFloat ();
	else if (var.Name == "CommandsBoxBorder") CommandsBoxBorder = var.asFloat ();
	else if (var.Name == "CommandsNbLines") CommandsNbLines = var.asInt ();
	else if (var.Name == "CommandsLineHeight") CommandsLineHeight = var.asFloat ();
	else if (var.Name == "CommandsBackColor") CommandsBackColor.set (var.asInt(0), var.asInt(1), var.asInt(2), var.asInt(3));
	else if (var.Name == "CommandsFrontColor") CommandsFrontColor.set (var.asInt(0), var.asInt(1), var.asInt(2), var.asInt(3));
	else if (var.Name == "CommandsFontSize") CommandsFontSize = var.asInt ();
	else nlwarning ("Unknown variable update %s", var.Name.c_str());
}

void	initCommands()
{
	// Add the keyboard listener in the event server
	Driver->EventServer.addListener (EventCharId, &CommandsListener);

	// Add the command displayer to the standard log (to display NeL info)
	CommandsLog.addDisplayer (&CommandsDisplayer);
	InfoLog->addDisplayer (&CommandsDisplayer);
	WarningLog->addDisplayer (&CommandsDisplayer);
	AssertLog->addDisplayer (&CommandsDisplayer);
	ErrorLog->addDisplayer (&CommandsDisplayer);

	// Add callback for the config file
	ConfigFile.setCallback ("CommandsBoxX", cbUpdateCommands);
	ConfigFile.setCallback ("CommandsBoxY", cbUpdateCommands);
	ConfigFile.setCallback ("CommandsBoxWidth", cbUpdateCommands);
	ConfigFile.setCallback ("CommandsBoxBorder", cbUpdateCommands);
	ConfigFile.setCallback ("CommandsNbLines", cbUpdateCommands);
	ConfigFile.setCallback ("CommandsLineHeight", cbUpdateCommands);
	ConfigFile.setCallback ("CommandsBackColor", cbUpdateCommands);
	ConfigFile.setCallback ("CommandsFrontColor", cbUpdateCommands);
	ConfigFile.setCallback ("CommandsFontSize", cbUpdateCommands);
  
	// Init the config file variable
	cbUpdateCommands (ConfigFile.getVar ("CommandsBoxX"));
	cbUpdateCommands (ConfigFile.getVar ("CommandsBoxY"));
	cbUpdateCommands (ConfigFile.getVar ("CommandsBoxWidth"));
	cbUpdateCommands (ConfigFile.getVar ("CommandsBoxBorder"));
	cbUpdateCommands (ConfigFile.getVar ("CommandsNbLines"));
	cbUpdateCommands (ConfigFile.getVar ("CommandsLineHeight"));
	cbUpdateCommands (ConfigFile.getVar ("CommandsBackColor"));
	cbUpdateCommands (ConfigFile.getVar ("CommandsFrontColor"));
	cbUpdateCommands (ConfigFile.getVar ("CommandsFontSize"));
}

void	updateCommands()
{
	// Display the background
	Driver->setMatrixMode2D11 ();
	Driver->drawQuad (CommandsBoxX-CommandsBoxBorder, CommandsBoxY-CommandsBoxBorder, CommandsBoxX+CommandsBoxWidth+CommandsBoxBorder, CommandsBoxY + (CommandsNbLines+1) * CommandsLineHeight + CommandsBoxBorder, CommandsBackColor);

	// Set the text context
	TextContext->setHotSpot (UTextContext::BottomLeft);
	TextContext->setColor (CommandsFrontColor);
	TextContext->setFontSize (CommandsFontSize);

	// Display the user input line
	string line = string("> ")+CommandsListener.line() + string ("_");
	TextContext->printfAt (CommandsBoxX, CommandsBoxY + CommandsBoxBorder, line.c_str());
	CommandsListener.setMaxWidthReached (TextContext->getLastXBound() > (CommandsBoxWidth-CommandsBoxBorder)*1.33f); // max is 1.33=4/3

	// Display stored lines
	float yPos = CommandsBoxY + CommandsBoxBorder;
	list<string>::reverse_iterator rit = StoredLines.rbegin();
	for (sint i = 0; i < CommandsNbLines; i++)
	{
		yPos += CommandsLineHeight;
		if (rit == StoredLines.rend()) break;
		TextContext->printfAt (CommandsBoxX, yPos, (*rit).c_str());
		rit++;
	}
}

void	clearCommands ()
{
	StoredLines.clear ();
}

void	releaseCommands()
{
	// Rmove the keyboard listener from the server
	Driver->EventServer.removeListener (EventCharId, &CommandsListener);
}


string getZoneNameByCoord(float x, float y)
{
 const float zoneDim = 160.0f;

 float xcount = x/zoneDim;
 float ycount = -y/zoneDim + 1;

 char ych[32];
 sprintf(ych,"%d_%c%c",(sint)ycount, 'A'+(sint)xcount/26,
'A'+(sint)xcount%26);
 return string(ych);
}

CVector getZoneCenterById(uint16 id)
{
 CAABBox  bbox;
 uint  x, y;
 const float zdim = 160.0f;

 x = id%256;
 y = id/256;

 return CVector(zdim*((float)x+0.5f), -zdim*((float)y+0.5f), 0.0f);
}

uint32 getIdByCoord(uint x, uint y)
{
 return y*256+x;
}

/*******************************************************************\
						getZoneCoordByName()
\*******************************************************************/
bool getZoneCoordByName(const char * name, uint16& x, uint16& y)
{
	uint i;
	
	std::string zoneName(name);

	// y
	uint ind1 = zoneName.find("_");
	if(ind1>=zoneName.length())
	{
		nlwarning("bad file name");
		return false;
	}
	std::string ystr = zoneName.substr(0,ind1);
	for(i=0; i<ystr.length(); i++)
	{
		if(!isdigit(ystr[i]))
		{
			nlwarning("y code size is not a 2 characters code");
			return false;
		}
	}
	y = atoi(ystr.c_str());

	// x
	x = 0;
	uint ind2 = zoneName.length();
	if((ind2-ind1-1)!=2)
	{
		nlwarning("x code size is not a 2 characters code");
		return false;
	}
	std::string xstr = zoneName.substr(ind1+1,ind2-ind1-1);
	for(i=0; i<xstr.length(); i++)
	{
		if (isalpha(xstr[i]))
		{
			x *= 26;
			x += (tolower(xstr[i])-'a');
		}
		else
		{
			nlwarning("invalid");
			return false;
		}
	}
	return true;
}

/*******************************************************************\
						getLettersFromNum()
\*******************************************************************/
void getLettersFromNum(uint16 num, std::string& code)
{
	if(num>26*26) 
	{
		nlwarning("zone index too high");
		return;
	}
	code.resize(0);
	uint16 remainder = num%26;
	code += 'A' + num/26;
	code += 'A' + remainder;
} 


/*******************************************************************\
						getZoneNameByCoord()
\*******************************************************************/
void getZoneNameByCoord(uint16 x, uint16 y, std::string& zoneName)
{
	// y str
	char stmp[10];
	sprintf(stmp,"%d",y);
	std::string ystrtmp = std::string(stmp);

	// x str
	std::string xstrtmp;
	getLettersFromNum(x, xstrtmp);

	// name
	zoneName = ystrtmp;
	zoneName +="_";
	zoneName +=xstrtmp;
}




/*******************************************************************\
						createZoneId()
\*******************************************************************/
uint16 createZoneId(std::string zoneName)
{
	uint16 x,y;
	getZoneCoordByName(zoneName.c_str(), x, y);
	return ((y-1)<<8) + x;
}


NLMISC_COMMAND(clear,"clear the chat history","")
{
	// check args, if there s not the right number of parameter, return bad
	if(args.size() != 0) return false;
	clearCommands ();
	return true;
}


NLMISC_COMMAND(zonecenter,"returns the zone center of a zone","")
{
	// check args, if there s not the right number of parameter, return bad
	if(args.size() != 1) return false;
	
	string zoneName = args[0];

	uint16 zoneId = createZoneId(zoneName);

	CVector zoneCenter = getZoneCenterById(zoneId);

	nlinfo("Zone Center for %s is %f %f %f", zoneName, zoneCenter.x, zoneCenter.y, zoneCenter.z);

	return true;
}

NLMISC_COMMAND(zonename,"returns the name of a zone","")
{
	// check args, if there s not the right number of parameter, return bad
	if(args.size() != 2) return false;
	
	float x = (float) atoi (args[0].c_str());
	float y = (float) atoi (args[1].c_str());

	string theName = getZoneNameByCoord(x,y);

	nlinfo("Zone Name is %s", theName);

	return true;
}

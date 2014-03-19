/** \file radar.cpp
 * Snowballs 2 specific code for managing the radar.
 * This code was taken from Snowballs 1.
 *
 * $Id: radar.cpp,v 1.8 2002/06/06 05:51:01 robofly Exp $
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

#include <vector>

#include <nel/misc/types_nl.h>
#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>
#include <nel/misc/command.h>

#include <nel/3d/u_material.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_texture.h>

#include "camera.h"
#include "client.h"
#include "commands.h"
#include "mouse_listener.h"
#include "entities.h"

//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;
using namespace std;

//
// Variables
//

// These variables are automatically set with the config file

static	float	RadarPosX, RadarPosY, RadarWidth, RadarHeight;
static	uint	RadarMinDistance, RadarMaxDistance;
static	CRGBA	RadarBackColor, RadarFrontColor, RadarSelfColor, RadarOtherColor, RadarPlaceColor, RadarDynamicColor;
static	float	RadarEntitySize;
static	uint	RadarFontSize;
static	float	RadarLittlePosX, RadarLittlePosY, RadarLittleRadius;

uint RadarState;
uint RadarDistance;

string SelectedURL="";
string SelectedURLname="";
NLMISC::TTime LastSelectedTime;
uint8 SelectionTimeOut; // in seconds

// float minimumDistanceToURL=500.0f;

// Structure for particular places
struct RadarParticularPlace
{
	RadarParticularPlace (float X, float Y, string Name, string Url) : x(X), y(Y), name(Name), url(Url) { }
	float x, y;
	string name;
	string url;
};

// Container for the particular places (automatically filled by the config file)
vector<RadarParticularPlace> RadarParticularPlaces;

// ***************************************************************************
struct		CPotentialURLTarget
{
	string		name;
	string		url;
	float		distanceFromPlayer;

	// For find().
	bool	operator==(const CPotentialURLTarget &o) const
	{
		return name==o.name;
	}
	// For sort().
	bool	operator<(const CPotentialURLTarget &o) const
	{
		return distanceFromPlayer>o.distanceFromPlayer;
	}
};

static vector<CPotentialURLTarget> potentialURLTargets;

/*********************************************************\
					initSelectableTargets ()
\*********************************************************/
void initSelectableTargets ()
{
	LastSelectedTime = CTime::getLocalTime();
	potentialURLTargets.clear();

	for(uint i = 0; i < RadarParticularPlaces.size(); i++)
	{

		CVector objectPos;

		objectPos.x = RadarParticularPlaces[i].x;
		objectPos.y = RadarParticularPlaces[i].y;


		// position of neighbour
		float posx = objectPos.x;
		float posy = objectPos.y;

		// relative position
		posx = (posx-Self->Position.x);
		posy = (posy-Self->Position.y);

		float dist = (float) sqrt((posx*posx)+(posy*posy));

//		nlinfo("Distance = %f", dist);

		if (dist<(float)(RadarDistance))
		{
			CPotentialURLTarget	pt;
			pt.name = RadarParticularPlaces[i].name;
			pt.url = RadarParticularPlaces[i].url;
			pt.distanceFromPlayer = dist;

			potentialURLTargets.push_back(pt);
		}
	}

	if (!potentialURLTargets.empty())
	{
		vector<CPotentialURLTarget>::iterator	it= potentialURLTargets.begin();

		while(it != potentialURLTargets.end())
		{
//				nlinfo("before sort target id id=%s", it->name);
				*it++;
		}

		std::sort( potentialURLTargets.begin(), potentialURLTargets.end() );

		it= potentialURLTargets.begin();

		while(it != potentialURLTargets.end())
		{
//				nlinfo("after sort target id id=%s", it->name);
				*it++;
		}
	}
}

void selectClosestURL()
{
	if (ShowDebugInChat) nlinfo("in sel closest tar: selected Object is now=%s", SelectedURL);
	initSelectableTargets();

	if (!potentialURLTargets.empty())
	{
		vector<CPotentialURLTarget>::iterator	it= potentialURLTargets.begin();
		if (it != potentialURLTargets.end())
		{
			SelectedURL= it->url;
			SelectedURLname= it->name;
//			nlinfo("Now I have changed DO to %s", SelectedURL);
//			setSelectedEntity(SelectedURL, "selected.ps");
		}
	} else
	{
		SelectedURL = "";
		SelectedURLname = "";
	}
}

void selectNextURL()
{
	initSelectableTargets();
//	CPotentialTarget	pt;
//	pt.name = SelectedURL;

//	vector<CPotentialTarget>::iterator	it= find(potentialURLTargets.begin(), potentialURLTargets.end(), pt);
	
	bool chFlag=false;
	if (!potentialURLTargets.empty())
	{
		vector<CPotentialURLTarget>::iterator	it= potentialURLTargets.begin();
		while(it != potentialURLTargets.end())
		{
			if(it->name== SelectedURLname)
			{
//				nlinfo("Last target id=%s", it->name);
				*it++;
//				nlinfo("New target id=%s", it->name);
				if(it!=potentialURLTargets.end())
				{
					SelectedURLname= it->name;
					SelectedURL= it->url;
//					nlinfo("Now I have changed DO to %s", SelectedURL);
					it=potentialURLTargets.end();
					chFlag=true;
//					setSelectedEntity(SelectedURL, "selected.ps");
				}
			}
			if (it!=potentialURLTargets.end()) *it++;
		}

		if (!chFlag)
		{
			it=potentialURLTargets.begin();
			if (it!=potentialURLTargets.end())
				SelectedURLname=it->name;
				SelectedURL=it->url;
//				setSelectedEntity(SelectedURL, "selected.ps");
		}
	} else
	{
		SelectedURL="";
		SelectedURLname="";
	}
}

//
// Functions
//


/*********************************************************\
					displayRadar()
\*********************************************************/
void displayRadar ()
{
	float xLeft = RadarPosX;
	float xRight = RadarPosX + RadarWidth;
	float yTop = RadarPosY + RadarHeight;
	float yBottom = RadarPosY;
	float xCenter = RadarPosX + RadarWidth/2.0f;
	float yCenter = RadarPosY + RadarHeight/2.0f;

	Driver->setMatrixMode2D11 ();

/*	if (SelectedURL!="")
	{
		// Print selected object
		TextContext->setHotSpot(UTextContext::TopLeft);
		TextContext->setColor(CRGBA(0,200,100,250));
		TextContext->setFontSize( 14 );
		TextContext->printfAt(xLeft+0.02f,0.91f,"Selected URL %s", SelectedURL );
	}
*/
	// Background
	Driver->drawQuad (xLeft,yBottom,xRight,yTop,RadarBackColor);

//	nlinfo("xLeft %f yBottom %f xRight %f yTop %f", xLeft,yBottom,xRight,yTop);
	// Print radar's range
	TextContext->setHotSpot(UTextContext::TopRight);
	TextContext->setColor(RadarFrontColor);
	TextContext->setFontSize(RadarFontSize);
	TextContext->printfAt(xRight-0.01f,yTop-0.01f,"%d m",RadarDistance);
		
	// Radar unit
	float stepV = 50.0f;
	float stepH = stepV*3.f/4.f;
	// Changing scale
	stepV = 0.8f*stepV/RadarDistance;
	stepH = 0.8f*stepH/RadarDistance;

	// Drawing radar's lines
	float gapV = stepV/2;
	float gapH = stepH/2;
	
	while(gapH<=RadarWidth/2.0)
	{
		// v lines
		Driver->drawLine (xCenter+gapH,yTop,xCenter+gapH,yBottom,RadarFrontColor);
		Driver->drawLine (xCenter-gapH,yTop,xCenter-gapH,yBottom,RadarFrontColor);

		gapH += stepH;
	}

	while(gapV<=RadarHeight/2.0)
	{
		// h lines
		Driver->drawLine (xLeft,yCenter+gapV,xRight,yCenter+gapV,RadarFrontColor);
		Driver->drawLine (xLeft,yCenter-gapV,xRight,yCenter-gapV,RadarFrontColor);

		gapV += stepV;
	}

	float scale = 1.0f;

	float xscreen = xCenter;
	float yscreen = yCenter;

	Driver->setFrustum (CFrustum(0.f, 1.0f, 0.f, 1.f, -1.f, 1.f, false));
	
	// distance between user and neighbour player
	float myPosx = Self->Position.x;
	float myPosy = Self->Position.y;

	// Quads size
	float radius = RadarEntitySize;

	// Arrow in center (user)
	Driver->drawTriangle(xscreen-2*radius,yscreen-2*radius, xscreen,yscreen-radius, xscreen,yscreen+2*radius, RadarSelfColor);
	Driver->drawTriangle(xscreen,yscreen-radius, xscreen+2*radius,yscreen-2*radius, xscreen,yscreen+2*radius, RadarSelfColor);

	TextContext->setColor(RadarOtherColor);

	for(EIT eit=Entities.begin(); eit!=Entities.end(); eit++)
	{
		if((*eit).second.Type == CEntity::Other || (*eit).second.Type == CEntity::Dynamic)
		{
			CRGBA myColor;
			if ((*eit).second.Type == CEntity::Other) myColor=RadarOtherColor;
			else myColor=RadarDynamicColor;
			
			CVector playerPos = (*eit).second.Position;

			// position of neighbour
			float posx = playerPos.x;
			float posy = playerPos.y;

			// relative position
			posx = (posx-myPosx)*0.4f/RadarDistance;
			posy = (posy-myPosy)*0.4f/RadarDistance;

			float dist = (float) sqrt((posx*posx)+(posy*posy));

			// Display a quad to show a player
			float an;
			float az;
			float x;
			float y;
			az = MouseListener->getOrientation ();
			if(posx==0)
			{
				if(posy==0)
				{
					x = xscreen;
					y = yscreen;
				}
				else
				{
					if(posy>0)
					{
						x = (float) (xscreen - dist*cos(Pi-az)*3.f/4.f);
						y = (float) (yscreen - dist*sin(Pi-az));
					}
					else
					{
						x = (float) (xscreen - dist*cos(-az)*3.f/4.f);
						y = (float) (yscreen - dist*sin(-az));
					}
				}
			}
			else
			{
				an = (float) atan(posy/posx);
				if(posx<0) an = an + (float)Pi;
				x = (float) (xscreen - dist*cos(-Pi/2 + an-az)*3.f/4.f); 
				y = (float) (yscreen - dist*sin(-Pi/2 + an-az));
			}

			// Players out of range are not displayed
			if(x<xLeft || x>xRight || y>yTop || y<yBottom) continue;

			TextContext->setColor(myColor);

			Driver->drawQuad (x-radius,y-radius, x+radius,y+radius,myColor);

			// Print his name
			TextContext->setFontSize(RadarFontSize);
			if(x>=xCenter)
			{
				if(y>=yCenter)
				{
					TextContext->setHotSpot(UTextContext::BottomLeft);
					TextContext->printfAt(x+2*radius, y+2*radius, (*eit).second.Name.c_str());
				}
				else
				{
					TextContext->setHotSpot(UTextContext::TopLeft);
					TextContext->printfAt(x+2*radius, y-2*radius, (*eit).second.Name.c_str());
				}
			}
			else
			{
				if(y>=yCenter)
				{
					TextContext->setHotSpot(UTextContext::BottomRight);
					TextContext->printfAt(x-2*radius, y+2*radius, (*eit).second.Name.c_str());
				}
				else
				{
					TextContext->setHotSpot(UTextContext::TopRight);
					TextContext->printfAt(x-2*radius, y-2*radius, (*eit).second.Name.c_str());
				}
			}
		}
	}

	// display particular places
	for(uint i = 0; i < RadarParticularPlaces.size(); i++)
	{
		// relative position
		float posx = (RadarParticularPlaces[i].x-myPosx)*0.4f/RadarDistance;
		float posy = (RadarParticularPlaces[i].y-myPosy)*0.4f/RadarDistance;
		
		float dist = (float) sqrt((posx*posx)+(posy*posy));

		// Display a quad to show a player
		float an;
		float az;
		float x;
		float y;
		az = MouseListener->getOrientation ();
		if(posx==0)
		{
			if(posy==0)
			{
				x = xscreen;
				y = yscreen;
			}
			else
			{
				if(posy>0)
				{
					x = (float) (xscreen - dist*cos(Pi-az)*3.f/4.f);
					y = (float) (yscreen - dist*sin(Pi-az));
				}
				else
				{
					x = (float) (xscreen - dist*cos(-az)*3.f/4.f);
					y = (float) (yscreen - dist*sin(-az));
				}
			}
		}
		else
		{
			an = (float) atan(posy/posx);
			if(posx<0) an = an + (float)Pi;
			x = (float) (xscreen - dist*cos(-Pi/2 + an-az)*3.f/4.f); 
			y = (float) (yscreen - dist*sin(-Pi/2 + an-az));
		}


		if(x<xLeft || x>xRight || y>yTop || y<yBottom) 
		{
			continue;
		}

		Driver->drawTriangle(x-radius,y-radius, x+radius,y-radius, x,y+radius, RadarPlaceColor);

		TextContext->setFontSize(RadarFontSize);
		if (SelectedURLname==RadarParticularPlaces[i].name) TextContext->setColor(CRGBA(255,255,255,255));
		else TextContext->setColor(RadarPlaceColor);

		if(x>=xCenter)
		{
			if(y>=yCenter)
			{
				TextContext->setHotSpot(UTextContext::BottomLeft);
				TextContext->printfAt(x+2*radius, y+2*radius, RadarParticularPlaces[i].name.c_str());
			}
			else
			{
				TextContext->setHotSpot(UTextContext::TopLeft);
				TextContext->printfAt(x+2*radius, y-2*radius, RadarParticularPlaces[i].name.c_str());
			}
		}
		else
		{
			if(y>=yCenter)
			{
				TextContext->setHotSpot(UTextContext::BottomRight);
				TextContext->printfAt(x-2*radius, y+2*radius, RadarParticularPlaces[i].name.c_str());
			}
			else
			{
				TextContext->setHotSpot(UTextContext::TopRight);
				TextContext->printfAt(x-2*radius, y-2*radius, RadarParticularPlaces[i].name.c_str());
			}
		}
	}
}



/*********************************************************\
					displayLittleRadar()
\*********************************************************/
void displayLittleRadar(bool combat)
{
	float radius = RadarLittleRadius;

	float xLeft = RadarLittlePosX - radius*3.f/4.f;
	float yBottom = RadarLittlePosY - radius;

	float xRight = RadarLittlePosX + radius*3.f/4.f;
	float yTop = RadarLittlePosY + radius;

	CRGBA combatColor = CRGBA(255,0,0,255);
	Driver->setMatrixMode2D11 ();
	
	// Background
	Driver->drawQuad (xLeft,yBottom,xRight,yTop,RadarBackColor);
//	nlinfo("xLeft %f yBottom %f xRight %f yTop %f", xLeft,yBottom,xRight,yTop);

	// Print radar's range
	TextContext->setHotSpot(UTextContext::MiddleBottom);
	TextContext->setColor(RadarFrontColor);
	TextContext->setFontSize(RadarFontSize);
	TextContext->printfAt(xLeft+radius*3.f/4.f,yTop+0.01f,"%d m",RadarDistance);
		
	// Radar unit
	float stepV = 50.0f;
	float stepH = stepV*3.f/4.f;
	// Changing scale
	stepV = radius*stepV/RadarDistance;
	stepH = radius*stepH/RadarDistance;

	// Drawing radar's lines
	// h lines
	Driver->drawLine (xLeft, yTop,         xRight, yTop,          RadarFrontColor);
	Driver->drawLine (xLeft, yBottom+radius,xRight, yBottom+radius, RadarFrontColor);
	Driver->drawLine (xLeft, yBottom,      xRight, yBottom,       RadarFrontColor);
	
	// v lines
	Driver->drawLine (xLeft,               yTop, xLeft,               yBottom, RadarFrontColor);
	Driver->drawLine (xLeft+radius*3.f/4.f, yTop, xLeft+radius*3.f/4.f, yBottom, RadarFrontColor);
	Driver->drawLine (xRight,              yTop, xRight,              yBottom, RadarFrontColor);

	float scale = 1.0f;

	float xscreen = xLeft + radius*3.f/4.f;
	float yscreen = yBottom + radius;

	Driver->setFrustum (CFrustum(0.f, 1.0f, 0.f, 1.f, -1.f, 1.f, false));
	
	// distance between user and neighbour player
	float myPosx = Self->Position.x;
	float myPosy = Self->Position.y;

	// Quads size
	float entitySize = RadarEntitySize/2.0f;

	if (combat) TextContext->setColor(RadarOtherColor);
	else TextContext->setColor(combatColor);

	for(EIT eit=Entities.begin(); eit!=Entities.end(); eit++)
	{
		if((*eit).second.Type == CEntity::Other || (*eit).second.Type == CEntity::Dynamic)
		{
			CRGBA myColor;
			if ((*eit).second.Type == CEntity::Other) myColor=RadarOtherColor;
			else myColor=RadarDynamicColor;

			CVector playerPos = (*eit).second.Position;

			// position of neighbour
			float posx = playerPos.x;
			float posy = playerPos.y;

			// relative position
			posx = (posx-myPosx)*radius/RadarDistance;
			posy = (posy-myPosy)*radius/RadarDistance;

			float dist = (float) sqrt((posx*posx)+(posy*posy));

			// Display a quad to show a player
			float an;
			float az;
			float x;
			float y;
			az = MouseListener->getOrientation ();
			if(posx==0)
			{
				if(posy==0)
				{
					x = xscreen;
					y = yscreen;
				}
				else
				{
					if(posy>0)
					{
						x = (float) (xscreen - dist*cos(Pi-az)*3.f/4.f);
						y = (float) (yscreen - dist*sin(Pi-az));
					}
					else
					{
						x = (float) (xscreen - dist*cos(-az)*3.f/4.f);
						y = (float) (yscreen - dist*sin(-az));
					}
				}
			}
			else
			{
				an = (float) atan(posy/posx);
				if(posx<0) an = an + (float)Pi;
				x = (float) (xscreen - dist*cos(-Pi/2 + an-az)*3.f/4.f); 
				y = (float) (yscreen - dist*sin(-Pi/2 + an-az));
			}

			// Players out of range are not displayed
			if(x<xLeft || x>xRight || y>yTop || y<yBottom) continue;

			TextContext->setColor(myColor);
			Driver->drawQuad (x-entitySize,y-entitySize,x+entitySize,y+entitySize,myColor);
		}
	}


	// display particular places
	for(uint i = 0; i < RadarParticularPlaces.size(); i++)
	{
		// relative position
		float posx = (RadarParticularPlaces[i].x-myPosx)*radius/RadarDistance;
		float posy = (RadarParticularPlaces[i].y-myPosy)*radius/RadarDistance;

		float dist = (float) sqrt((posx*posx)+(posy*posy));

		// Display a quad to show a player
		float an;
		float az;
		float x;
		float y;
		az = MouseListener->getOrientation ();
		if(posx==0)
		{
			if(posy==0)
			{
				x = xscreen;
				y = yscreen;
			}
			else
			{
				if(posy>0)
				{
					x = (float) (xscreen - dist*cos(Pi-az)*3.f/4.f);
					y = (float) (yscreen - dist*sin(Pi-az));
				}
				else
				{
					x = (float) (xscreen - dist*cos(-az)*3.f/4.f);
					y = (float) (yscreen - dist*sin(-az));
				}
			}
		}
		else
		{
			an = (float) atan(posy/posx);
			if(posx<0) an = an + (float)Pi;
			x = (float) (xscreen - dist*cos(-Pi/2 + an-az)*3.f/4.f); 
			y = (float) (yscreen - dist*sin(-Pi/2 + an-az));
		}


		if(x<xLeft || x>xRight || y>yTop || y<yBottom) 
		{
			continue;
		}
		if (SelectedURLname==RadarParticularPlaces[i].name) Driver->drawTriangle(x-entitySize,y-entitySize, x+entitySize,y-entitySize, x,y+entitySize, CRGBA(255,255,255,255));
		else Driver->drawTriangle(x-entitySize,y-entitySize, x+entitySize,y-entitySize, x,y+entitySize, RadarPlaceColor);
	}
}

void cbUpdateRadar (CConfigFile::CVar &var)
{
	if (var.Name == "RadarPosX") RadarPosX = var.asFloat ();
	else if (var.Name == "RadarPosY") RadarPosY = var.asFloat ();
	else if (var.Name == "RadarWidth") RadarWidth = var.asFloat ();
	else if (var.Name == "RadarHeight") RadarHeight = var.asFloat ();
	else if (var.Name == "RadarBackColor") RadarBackColor.set (var.asInt(0), var.asInt(1), var.asInt(2), var.asInt(3));
	else if (var.Name == "RadarFrontColor") RadarFrontColor.set (var.asInt(0), var.asInt(1), var.asInt(2), var.asInt(3));
	else if (var.Name == "RadarSelfColor") RadarSelfColor.set (var.asInt(0), var.asInt(1), var.asInt(2), var.asInt(3));
	else if (var.Name == "RadarOtherColor") RadarOtherColor.set (var.asInt(0), var.asInt(1), var.asInt(2), var.asInt(3));
	else if (var.Name == "RadarDynamicColor") RadarDynamicColor.set (var.asInt(0), var.asInt(1), var.asInt(2), var.asInt(3));
	else if (var.Name == "RadarPlaceColor") RadarPlaceColor.set (var.asInt(0), var.asInt(1), var.asInt(2), var.asInt(3));
	else if (var.Name == "RadarEntitySize") RadarEntitySize = var.asFloat ();
	else if (var.Name == "RadarState") RadarState = var.asInt ();
	else if (var.Name == "RadarDistance") RadarDistance = var.asInt ();
	else if (var.Name == "RadarMinDistance") RadarMinDistance = var.asInt ();
	else if (var.Name == "RadarMaxDistance") RadarMaxDistance = var.asInt ();
	else if (var.Name == "URLSelectionTimeout") SelectionTimeOut = var.asInt ();
	else if (var.Name == "RadarParticularPlaces")
	{
		RadarParticularPlaces.clear ();
		for (sint i = 0; i < var.size(); i += 4)
		{
			RadarParticularPlaces.push_back (RadarParticularPlace(var.asFloat(i), var.asFloat(i+1), var.asString(i+2), var.asString(i+3)));
		}
	}
	else if (var.Name == "RadarFontSize") RadarFontSize = var.asInt ();
	else if (var.Name == "RadarLittlePosX") RadarLittlePosX = var.asFloat ();
	else if (var.Name == "RadarLittlePosY") RadarLittlePosY = var.asFloat ();
	else if (var.Name == "RadarLittleRadius") RadarLittleRadius = var.asFloat ();
	else nlwarning ("Unknown variable update %s", var.Name.c_str());
}

void initRadar ()
{
	ConfigFile.setCallback ("RadarPosX", cbUpdateRadar);
	ConfigFile.setCallback ("RadarPosY", cbUpdateRadar);
	ConfigFile.setCallback ("RadarWidth", cbUpdateRadar);
	ConfigFile.setCallback ("RadarHeight", cbUpdateRadar);
	ConfigFile.setCallback ("RadarBackColor", cbUpdateRadar);
	ConfigFile.setCallback ("RadarFrontColor", cbUpdateRadar);
	ConfigFile.setCallback ("RadarSelfColor", cbUpdateRadar);
	ConfigFile.setCallback ("RadarOtherColor", cbUpdateRadar);
	ConfigFile.setCallback ("RadarDynamicColor", cbUpdateRadar);
	ConfigFile.setCallback ("RadarPlaceColor", cbUpdateRadar);
	ConfigFile.setCallback ("RadarEntitySize", cbUpdateRadar);
	ConfigFile.setCallback ("RadarState", cbUpdateRadar);
	ConfigFile.setCallback ("RadarDistance", cbUpdateRadar);
	ConfigFile.setCallback ("RadarMinDistance", cbUpdateRadar);
	ConfigFile.setCallback ("RadarMaxDistance", cbUpdateRadar);
	ConfigFile.setCallback ("RadarParticularPlaces", cbUpdateRadar);
	ConfigFile.setCallback ("RadarFontSize", cbUpdateRadar);
	ConfigFile.setCallback ("RadarLittlePosX", cbUpdateRadar);
	ConfigFile.setCallback ("RadarLittlePosY", cbUpdateRadar);
	ConfigFile.setCallback ("RadarLittleRadius", cbUpdateRadar);
	ConfigFile.setCallback ("URLSelectionTimeout", cbUpdateRadar);
	
	cbUpdateRadar (ConfigFile.getVar ("RadarPosX"));
	cbUpdateRadar (ConfigFile.getVar ("RadarPosY"));
	cbUpdateRadar (ConfigFile.getVar ("RadarWidth"));
	cbUpdateRadar (ConfigFile.getVar ("RadarHeight"));
	cbUpdateRadar (ConfigFile.getVar ("RadarFrontColor"));
	cbUpdateRadar (ConfigFile.getVar ("RadarBackColor"));
	cbUpdateRadar (ConfigFile.getVar ("RadarSelfColor"));
	cbUpdateRadar (ConfigFile.getVar ("RadarOtherColor"));
	cbUpdateRadar (ConfigFile.getVar ("RadarDynamicColor"));
	cbUpdateRadar (ConfigFile.getVar ("RadarPlaceColor"));
	cbUpdateRadar (ConfigFile.getVar ("RadarEntitySize"));
	cbUpdateRadar (ConfigFile.getVar ("RadarState"));
	cbUpdateRadar (ConfigFile.getVar ("RadarDistance"));
	cbUpdateRadar (ConfigFile.getVar ("RadarMinDistance"));
	cbUpdateRadar (ConfigFile.getVar ("RadarMaxDistance"));
	cbUpdateRadar (ConfigFile.getVar ("RadarParticularPlaces"));
	cbUpdateRadar (ConfigFile.getVar ("RadarFontSize"));
	cbUpdateRadar (ConfigFile.getVar ("RadarLittlePosX"));
	cbUpdateRadar (ConfigFile.getVar ("RadarLittlePosY"));
	cbUpdateRadar (ConfigFile.getVar ("RadarLittleRadius"));
	cbUpdateRadar (ConfigFile.getVar ("URLSelectionTimeout"));
}

void updateRadar ()
{
	if (RadarDistance > RadarMaxDistance) RadarDistance = RadarMaxDistance;
	else if (RadarDistance < RadarMinDistance) RadarDistance = RadarMinDistance;

	if (SelectedURL!="")
	{
		TTime	newTime = CTime::getLocalTime();
		float	delta = (float)(newTime-LastSelectedTime)/1000.0f;
		if (delta>=SelectionTimeOut)
		{
			SelectedURL = "";
			SelectedURLname = "";
		}
	}

	switch (RadarState)
	{
	case 0:
		break;
	case 1:
		displayRadar ();
		break;
	case 2:
		displayLittleRadar(false);
		break;
	case 3:
		displayLittleRadar(true);
		break;
	}
}

void releaseRadar ()
{
}

NLMISC_COMMAND(go,"change position of the player with a player name or location","<player_name>|<location_name>")
{
	// check args, if there s not the right number of parameter, return bad
	if (args.size() == 1)
	{
		bool gotoplayer = true;

		vector<RadarParticularPlace>::iterator itpp;
		for(itpp = RadarParticularPlaces.begin(); itpp != RadarParticularPlaces.end(); itpp++)
		{
			if((*itpp).name==args[0])
			{
				string cmd = "goto " + toString ((*itpp).x) + " " + toString ((*itpp).y);
				ICommand::execute (cmd, CommandsLog);

				gotoplayer = false;
				break;
			}
		}
		
		if(gotoplayer)
		{
			EIT itre;
			for(itre=Entities.begin(); itre!=Entities.end(); itre++)
			{
				if((*itre).second.Type != CEntity::Snowball)
				{
					if((*itre).second.Name == args[0])
					{
						string cmd = "goto " + toString ((*itre).second.Position.x) + " " + toString ((*itre).second.Position.y);
						ICommand::execute (cmd, CommandsLog);

						break;
					}
				}
			}
		}
	}
	else
		return false;


	return true;
}

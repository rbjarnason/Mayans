/** \file painter.h
 * <File description>
 *
 * $Id: painter.h,v 1.1 2002/03/20 18:05:25 robofly Exp $
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

#ifndef NL_PAINTER_H
#define NL_PAINTER_H


#include "nel/misc/types_nl.h"
#include "nel/misc/matrix.h"
#include "nel/misc/plane.h"
#include "nel/misc/triangle.h"
#include <../src/3d/landscape.h>
#include <set>

#define MAX_TILE_IN_PATCH 16
#define NUM_TILE_SEL (MAX_TILE_IN_PATCH*MAX_TILE_IN_PATCH)

extern NL3D::CTileBank bank;

#define RPO_DEFAULT_TESSEL 4

enum typeBind { BIND_25=0, BIND_75, BIND_50, BIND_SINGLE, BIND_COUNT, BIND_ALIGN=0xffffffff };

extern float bindWhere[BIND_COUNT];

class tileIndex
{
public:
	tileIndex ()
	{}
	tileIndex ( int tile, int rotate)
	{
		Tile=tile;
		Rotate=rotate; 
	}
	uint Tile:16;
	int Rotate:8;
};

class tileDesc
{
#define CASE_MASK 0x7
//	friend class RPatchMesh;
public:
	void setTile (int num, int ncase, tileIndex tile0, tileIndex tile1, tileIndex tile2)
	{
		_Num=num;
		_MatIDTab[0]=tile0;
		_MatIDTab[1]=tile1;
		_MatIDTab[2]=tile2;
		setCase (ncase);
	}
	tileIndex& getLayer (int num)
	{
		return _MatIDTab[num];
	}
	const tileIndex& getLayer (int num) const
	{
		return _MatIDTab[num];
	}
	int getNumLayer ()
	{
		return _Num;
	}
	void setEmpty ()
	{
		_Num=0;
		_Flags=0;
	}
	bool isEmpty ()
	{
		return _Num==0;
	}
	void rotate (int nRotate)
	{
		for (int i=0; i<3; i++)
		{
			_MatIDTab[i].Rotate+=nRotate;
			_MatIDTab[i].Rotate&=3;
		}
	}
	int getCase() const
	{
		return _Flags&CASE_MASK;
	}
	void setCase(int nCase)
	{
		nlassert ((nCase>=0)&&(nCase<5));
		_Flags&=~CASE_MASK;
		_Flags|=nCase;
	}
private:
	tileIndex _MatIDTab[3];
	uint8	_Num;
	uint8	_Flags;
};
 
class UI_PATCH
{
public:
	int					NbTilesU;		// Default = 3 (2^3 = 8)
	int					NbTilesV;		// Default = 3 (2^3 = 8)
	std::vector<tileDesc>	Tile;			// Tabl for tile number ( size: (2^NbTilesU) * (2^NbTilesV) ) default, all 0
	std::vector<uint>		Colors;			// Tabl for color on tile ( size: ((2^NbTilesU)+1) * ((2^NbTilesV)+1) ) default, all 0
										// Color in 32 bits ARGB.
	void Init (int nU=RPO_DEFAULT_TESSEL, int nV=RPO_DEFAULT_TESSEL, bool bKeep=false)
	{
		// New size
		int nOldU=1<<NbTilesU;
		int nOldV=1<<NbTilesV;
		int nNewU=1<<nU;
		int nNewV=1<<nV;
		NbTilesU=nU;
		NbTilesV=nV;
		int nTileCount=(1<<NbTilesU)*(1<<NbTilesV);
		int nVertexCount=((1<<NbTilesU)+1)*((1<<NbTilesV)+1);

		// Keep old infos
		if (bKeep)
		{
			// Copy old buffers
			std::vector<tileDesc> oldTile=Tile;
			std::vector<uint> oldColors=Colors;

			// Resize new one
			Tile.resize(nTileCount);
			Colors.resize(nVertexCount);

			// Copy old coord...
			int i,j;
			int nMinU=std::min (nOldU, nNewU);
			int nMinV=std::min (nOldV, nNewV);
			for (j=0; j<nMinV; j++)
			{
				for (i=0; i<nMinU; i++)
				{
					Tile[i+j*nNewU]=oldTile[i+j*nOldU];
				}
				for (; i<nNewU; i++)
				{
					Tile[i+j*nNewU].setEmpty ();
				}
			}
			for (; j<nNewV; j++)
			{
				for (i=0; i<nNewU; i++)
				{
					Tile[i+j*nNewU].setEmpty ();
				}
			}
			for (j=0; j<nMinV+1; j++)
			{
				for (i=0; i<nMinU+1; i++)
				{
					Colors[i+j*(nNewU+1)]=oldColors[i+j*(nOldU+1)];
				}
				for (; i<nNewU+1; i++)
				{
					Colors[i+j*nNewU]=0xffffff;
				}
			}
			for (; j<nNewV+1; j++)
			{
				for (i=0; i<nMinU+1; i++)
				{
					Colors[i+j*nNewU]=0xffffff;
				}
			}
		}
		else
		{
			// Resize new buffers
			Tile.resize(nTileCount);
			Colors.resize(nVertexCount);

			// Init new coord
			int j;
			for(j=0; j<nTileCount; j++)
			{
				Tile[j].setEmpty ();
			}
			for(j=0; j<nVertexCount; j++)
			{
				Colors[j]=0xffffff;
			}
		}
	}
};

typedef std::multiset< std::vector<UI_PATCH>* > testSet;

struct RPOTess
{
	int				TileTesselLevel;
	bool			ModeTile;
	bool			KeepMapping;
	int				TransitionType;
};

struct PaintTile
{
	PaintTile ()
	{
		Mesh=-1;
	}
	PaintTile* get2Voisin (int i)
	{
		if (voisins[i])
		{
			return voisins[i]->voisins[(i+rotate[i])&3];
		}
		return NULL;
	}
	int get2VoisinRotate (int i)
	{
		if (voisins[i])
		{
			return rotate[i]+voisins[i]->rotate[(i+rotate[i])&3];
		}
		return NULL;
	}
	PaintTile* getRight256 (int rot, int& _rotate)
	{
		_rotate=rotate[(2+rot)&3];
		return voisins[(2+rot)&3];
	}
	PaintTile* getBottom256 (int rot, int& _rotate)
	{
		_rotate=rotate[(1+rot)&3];
		return voisins[(1+rot)&3];
	}
	PaintTile* getRightBottom256 (int rot, int& _rotate)
	{
		int rightRot;
		PaintTile* right=getRight256 (rot, rightRot);
		if (right)
			return right->getBottom256 ((rot-rightRot)&3, _rotate);
		else
			return NULL;
	}
	PaintTile* getBottomRight256 (int rot, int& _rotate)
	{
		int bottomRot;
		PaintTile* bottom=getBottom256 (rot, bottomRot);
		if (bottom)
			return bottom->getRight256 ((rot-bottomRot)&3, _rotate);
		else
			return NULL;
	}
	bool validFor256 (int rot)
	{
		int _rotate;
		if (!getRight256 (rot, _rotate))
			return false;
		if (!getBottom256 (rot, _rotate))
			return false;
		if (!getRightBottom256 (rot, _rotate))
			return false;
		if (getRightBottom256 (rot, _rotate)!=getBottomRight256 (rot, _rotate))
			return false;
		return true;
	}
	void set256 (int rotate)
	{
	}
	sint16		Mesh;
	sint32		patch;
	sint32		tile;
	uint8		u;
	uint8		v;
	PaintTile*	voisins[4];
	uint8			rotate[4];
};

struct PaintPatch
{
	enum TBorder {left=0, bottom, right, top, count};
	PaintPatch ()
	{
		Mesh=-1;
	}
	PaintPatch (sint32 mesh, sint32 p)
	{
		Mesh=mesh;
		patch=p;
	}
	sint32		Mesh;
	sint32		patch;
};

/*-------------------------------------------------------------------*/

class tileSetIndex
{
public:
	int TileSet;
	int Rotate;
	bool operator< (const tileSetIndex& other) const
	{
		if (TileSet<other.TileSet)
			return true;
		if (TileSet>other.TileSet)
			return false;
		int delta=(other.Rotate-Rotate)&3;
		if (delta==1)
			return true;
		if (delta==3)
			return false;
		if (delta==0)
			return false;
		nlassert (0);	// no!
		return false;
	}
	bool operator!= (const tileSetIndex& other) const
	{
		return ((TileSet!=other.TileSet)||(Rotate!=other.Rotate));
	}
	bool operator== (const tileSetIndex& other) const
	{
		return ((TileSet==other.TileSet)&&(Rotate==other.Rotate));
	}
};


namespace NL3D 
{

using	NLMISC::CVector;
using	NLMISC::CPlane;

// ***************************************************************************
/**
 * This is a in-engine patch painter utility
 * \author Róbert Bjarnason
 * \author Doug Posner
 * \author Michael Warnock 
 * 
 * \author InOrbit Entertainment, Inc.
 * \date 2001
 *
 * \thanks to Nevrax from code snippets from around the source tree
 *
 */
class CPainter
{
public:

	/// Constructor
	CPainter(); 

	/// Init the paintersystem and set the landscape.
	void			init(CLandscape *land);

	void			updatePatch(sint zoneId, sint patchId, int tileSet, const CTileBank& bank);
	void			paintPatch(sint zoneId, sint patchId);
	void			setTile (int mesh, int tile, const tileDesc& desc, CLandscape* land);
	void			putATile ( sint zoneId, sint patchId, PaintTile* pTile, int tileSet, int curRotation, const CTileBank& bank, bool selectCycle, std::set<PaintTile*>& visited, std::set<int>& modified, CLandscape* land, bool _256, int u, int v);
	void			setPaintMode(bool painting) { paint = painting; };
	void			setFillMode(bool fill) { fillMode = fill; };
	void			setSelectedTile(int tile) { selectedTile = tile; };

	// Return the tile number
	int GetTileNumber(int nPatch, int nU, int nV) const
	{
		nlassert (nU>=0);
		nlassert (nU<MAX_TILE_IN_PATCH);
		nlassert (nV>=0);
		nlassert (nV<MAX_TILE_IN_PATCH);
		return nV*MAX_TILE_IN_PATCH+nU+nPatch*NUM_TILE_SEL;
	}

// *****************************
private:
	CRefPtr<CLandscape>	_Landscape;
	bool				paint, fillMode;
	int					selectedTile;
	int TileIndex;

};


} // NL3D


#endif // NL_PAINTER_H

/* End of painter.h */

/** \file painter.cpp
 * <File description>
 *
 * $Id: painter.cpp,v 1.1 2002/03/20 18:05:25 robofly Exp $
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

#include "painter.h"
#include <set>

using namespace NLMISC;
using namespace std;


namespace NL3D 
{


// ***************************************************************************
CPainter::CPainter()
{
	nlinfo("CPAINTER");
	paint=false;
	fillMode=false;
	selectedTile=6;
	TileIndex=0;
}

// ***************************************************************************
void			CPainter::init(CLandscape *land)
{
	_Landscape= land;
	nlinfo("PINIT");
}

// ***************************************************************************
void CPainter::setTile (int zoneId, int tile, const tileDesc& desc, CLandscape* land)
{

	// NeL update
	if (land)
	{
		int patchId=tile/NUM_TILE_SEL;
		int ttile=tile%NUM_TILE_SEL;
		int v=ttile/MAX_TILE_IN_PATCH;
		int u=ttile%MAX_TILE_IN_PATCH;

		// Get the zone
		CZone* zone=land->getZone (zoneId);
		nlassert (zone);

		// Get the patchId
		std::vector<CTileElement> copyZone = zone->getPatchTexture (patchId);
		const CPatch	*pPatch = zone->getPatchWritable((sint) patchId);
		int OrderS=pPatch->getOrderS();
		
		for (int l=0; l<3; l++)
		{
			if (l>=2)
			{
				copyZone[u+v*OrderS].Tile[l]=0xffff;
			}
			else
			{
				int toto=copyZone[u+v*OrderS].Tile[l];
				copyZone[u+v*OrderS].Tile[l]=desc.getLayer (l).Tile;
				copyZone[u+v*OrderS].setTileOrient (l, desc.getLayer (l).Rotate);

			}
		}
		if (copyZone[u+v*OrderS].Tile[0]==0xffff)
			copyZone[u+v*OrderS].setTile256Info (false, 0);
		else
		{
			if (desc.getCase()==0)
				copyZone[u+v*OrderS].setTile256Info (false, 0);
			else
				copyZone[u+v*OrderS].setTile256Info (true, desc.getCase()-1);
		}

		// Set the patchId
		zone->changePatchTextureAndColor (patchId, &copyZone, NULL);

	}
}

void	CPainter::updatePatch(sint zoneId, sint patchId, int tileSet, const CTileBank& bank)
{
			set<PaintTile*> alreadyRecursed;
			set<PaintTile*> visited;
			set<int> modified;

			// Get the zone
			CZone* zone=_Landscape->getZone (zoneId);
			nlassert (zone);

			// Get the patchId
			std::vector<CTileElement> copyZone = zone->getPatchTexture (patchId);
			CPatch	*pPatch = zone->getPatchWritable((sint) patchId);
			int OrderS= pPatch->getOrderS();
			int OrderT= pPatch->getOrderT();

//			int nU=1<<OrderS;
//			int nV=1<<OrderT;

			nlinfo("OrderS = %d", OrderS);
			nlinfo("OrderT = %d", OrderT);

//			nlinfo("nU = %d", nU);
//			nlinfo("nV = %d", nV);

			for (int u=0; u<OrderS; u++)
			{
			for (int v=0; v<OrderT; v++)
			{
				PaintTile* pTile = new PaintTile();
				PaintTile* v1 = new PaintTile();
					
				PaintTile* v2 = new PaintTile();
				PaintTile* v3 = new PaintTile();
				PaintTile* v4 = new PaintTile();

//				PaintTile* pTile = new PaintTile();
				pTile->Mesh=zoneId;
				pTile->patch=patchId;
				nlinfo("XXU = %d XXV=%d", u, v);

				pTile->tile=patchId*NUM_TILE_SEL+v*MAX_TILE_IN_PATCH+u;	

				int uu=0;
				int vv=0;

				v1->Mesh=zoneId;
				v1->patch=patchId;
				v1->tile=patchId*NUM_TILE_SEL+(v-1)*MAX_TILE_IN_PATCH+u;

				v2->Mesh=zoneId;
				v2->patch=patchId;
				v2->tile=patchId*NUM_TILE_SEL+(v+1)*MAX_TILE_IN_PATCH+u;

				v3->Mesh=zoneId;
				v3->patch=patchId;
				v3->tile=patchId*NUM_TILE_SEL+v*MAX_TILE_IN_PATCH+u-1;	

				v4->Mesh=zoneId;
				v4->patch=patchId;
				v4->tile=patchId*NUM_TILE_SEL+v*MAX_TILE_IN_PATCH+u+1;

				pTile->voisins[3]= v==0 ? NULL : v1;
				pTile->voisins[1]= v==(OrderS-1) ? NULL : v2;
				pTile->voisins[0]= u==0 ? NULL : v3;
				pTile->voisins[2]= u==(OrderT-1) ? NULL : v4;

				pTile->rotate[3]=0;
				pTile->rotate[1]=0;
				pTile->rotate[0]=0;
				pTile->rotate[2]=0;
				pTile->u=u;
				pTile->v=v;
				putATile( zoneId, patchId, pTile, tileSet, 0, bank, false, alreadyRecursed, modified, _Landscape, true, v, u);
//				delete pTile;
			}
			}
}


void	CPainter::putATile ( sint zoneId, sint patchId, PaintTile* pTile, int tileSet, int curRotation, 
							const CTileBank& bank, bool selectCycle, std::set<PaintTile*>& visited, 
							std::set<int>& modified, CLandscape* land, bool _256, int v, int u)
{

	nlinfo("putting a tile zoneId = %d patch=%d", zoneId, patchId); 
	// If tile 256, must have delta pos aligned on 2x2
	if (_256)
	{
		if (pTile->u&1)
			pTile=pTile->voisins[0];
		if (pTile->v&1)
			pTile=pTile->voisins[3];
	}	

	nlinfo("1...");
	// 256 valide ?
//	if ((_256)&&(!pTile->validFor256 (0)))
//		return false;

	// ** 1) Backup of the tile

	// Get the zone
	CZone* zone=land->getZone (zoneId);
	nlassert (zone);
	nlinfo("2...");

	CPatch	*pPatch = zone->getPatchWritable((sint) patchId);
	int OrderS=pPatch->getOrderS();
	nlinfo("3...");

	// Get the tileElements
	std::vector<CTileElement> tileElements = zone->getPatchTexture (patchId);
/*
	//number of layers
	tileDesc backup;
	backup.setTile (0, 0, tileIndex (tileElements[0].Tile[0],tileElements[0].getTileOrient(0) ), 
								   tileIndex (tileElements[0].Tile[1],tileElements[0].getTileOrient(1) ), 
								   tileIndex (tileElements[0].Tile[2],tileElements[0].getTileOrient(2) ) );

//	tileDesc backup.setTile (0, 0, tileIndex (0,0), tileIndex (0,0), tileIndex (0,0));;
	tileDesc backupRight, backupBottom, backupCorner;

	if (_256)
	{
		// get right
	//	int nRot;
	//	PaintTile* other=pTile->getRight256 (0, nRot);
	//	nlassert (other);

		backupRight.setTile (0, 0, tileIndex (tileElements[0].Tile[0],tileElements[0].getTileOrient(0) ), 
								   tileIndex (tileElements[0].Tile[1],tileElements[0].getTileOrient(1) ), 
								   tileIndex (tileElements[0].Tile[2],tileElements[0].getTileOrient(2) ) );

		// get bottom
//		other=pTile->getBottom256 (0, nRot);
//		nlassert (other);

 		backupBottom.setTile (0, 0, tileIndex (tileElements[0].Tile[0],tileElements[0].getTileOrient(0) ), 
								   tileIndex (tileElements[0].Tile[1],tileElements[0].getTileOrient(1) ), 
								   tileIndex (tileElements[0].Tile[2],tileElements[0].getTileOrient(2) ) );
		
//		backupBottom=vectMesh[other->Mesh].RMesh->getTileDesc (other->tile);

		// get corner
//		other=pTile->getRightBottom256 (0, nRot);
//		nlassert (other);
//		nlassert (other==pTile->getBottomRight256 (0, nRot));
 		backupCorner.setTile (0, 0, tileIndex (tileElements[0].Tile[0],tileElements[0].getTileOrient(0) ), 
								   tileIndex (tileElements[0].Tile[1],tileElements[0].getTileOrient(1) ), 
								   tileIndex (tileElements[0].Tile[2],tileElements[0].getTileOrient(2) ) );

*///		backupCorner=vectMesh[other->Mesh].RMesh->getTileDesc (other->tile);
//	}

	// ** 2) Add to visited tile set
	visited.insert (pTile);
//nlinfo("Debug543534");	

	// Clear...
	if (tileSet==-1)
	{
		if (_256)
		{
			tileDesc desc;
			desc.setTile (0, 0, tileIndex (0,0), tileIndex (0,0), tileIndex (0,0));

			// Main tile
			setTile (pTile->Mesh, pTile->tile, desc, land);
			modified.insert (pTile->Mesh);

			// get right
			int nRot;
			PaintTile* other=pTile->getRight256 (0, nRot);
			nlassert (other);
			setTile (other->Mesh, other->tile, desc, land);
			modified.insert (other->Mesh);

			// get bottom
			other=pTile->getBottom256 (0, nRot);
			nlassert (other);
			setTile (other->Mesh, other->tile, desc, land);
			modified.insert (other->Mesh);
//nlinfo("Debug76534");	

			// get corner
			other=pTile->getRightBottom256 (0, nRot);
			nlassert (other);
			nlassert (other==pTile->getBottomRight256 (0, nRot));
			setTile (other->Mesh, other->tile, desc, land);
			modified.insert (other->Mesh);
		}
		else
		{
			tileDesc desc;
			desc.setTile (0, 0, tileIndex (0,0), tileIndex (0,0), tileIndex (0,0));
			setTile (pTile->Mesh, pTile->tile, desc, land);
			modified.insert (pTile->Mesh);
		}
	}

	// TileSet
	const CTileSet *TileSet=bank.getTileSet (tileSet);
//nlinfo("Debug876867");	
	nlinfo("5...");

	// The number
	int nTile=3;
//	if (_256)
//	{
//		if (selectCycle)
//			nTile=TileSet->getTile256 (TileIndex++%(uint32)TileSet->getNumTile256 ());
//		else
//			nTile=TileSet->getTile256 (((uint32)rand())%(uint32)TileSet->getNumTile256 ());
//	}
//	else
//	{
//		if (selectCycle)
//			nTile=TileSet->getTile128 (TileIndex++%(uint32)TileSet->getNumTile128 ());
//		else
//			nTile=TileSet->getTile128 (((uint32)rand())%(uint32)TileSet->getNumTile128 ());
//	}
nlinfo("RRRRRRRRRRRRRR");
	// ** 3) Put the tile

	if (_256)
	{
	try
	{
	if (pTile!=NULL)
	{// Main tile
		tileDesc desc;
		desc.setTile (1, 1+((-curRotation)&3), tileIndex (nTile, curRotation), tileIndex (0,0), tileIndex (0,0));

		// Main tile
//		setTile (pTile->Mesh, pTile->tile, desc, land);

//		modified.insert (pTile->Mesh);

//		int u=pTile->u;
//		int v=pTile->v;

//		nlinfo("U = %d V=%d", u, v);
		int nRot = 0;
//		PaintTile* other=pTile->getRight256 (0, nRot);
//		nlassert (other);
		desc.setTile (1, 1+((-curRotation)&3), tileIndex (nTile, curRotation), tileIndex (0,0), tileIndex (0,0));
		setTile(pTile->Mesh, patchId*NUM_TILE_SEL+(v-1)*MAX_TILE_IN_PATCH+u, desc, land);
		desc.setTile (1, 1+((-curRotation-1)&3), tileIndex (nTile, (curRotation-nRot)&3), tileIndex (0,0), tileIndex (0,0));
		setTile(pTile->Mesh, patchId*NUM_TILE_SEL+(v+1)*MAX_TILE_IN_PATCH+u, desc, land);
		desc.setTile (1, 1+((-curRotation+1)&3), tileIndex (nTile, (curRotation-nRot)&3), tileIndex (0,0), tileIndex (0,0));
		setTile(pTile->Mesh, patchId*NUM_TILE_SEL+v*MAX_TILE_IN_PATCH+u-1, desc, land);
		desc.setTile (1, 1+((-curRotation+2)&3), tileIndex (nTile, (curRotation-nRot)&3), tileIndex (0,0), tileIndex (0,0));
		setTile(pTile->Mesh, patchId*NUM_TILE_SEL+v*MAX_TILE_IN_PATCH+u+1, desc, land);

		// get right
//		int nRot;
//		PaintTile* other=pTile->getRight256 (0, nRot);
nlinfo("GGGGGGGGGGGGGGGGG");
//		desc.setTile (1, 1+((-curRotation-1)&3), tileIndex (nTile, (curRotation-nRot)&3), tileIndex (0,0), tileIndex (0,0));
nlinfo("GGGGGGGGGGGGGGGGG");
	}
	} catch (...)
	{}
//nlinfo("RRRRRRRRRRRRRR");
/*		setTile (other->Mesh, other->tile, desc, land);
nlinfo("FDASFFFFFFFFF");
		modified.insert (other->Mesh);
		
		// Add to visited tile set
		visited.insert (other);

nlinfo("GGGGGGGGGGGGGGGGG");
		// get bottom
		other=pTile->getBottom256 (0, nRot);
		desc.setTile (1, 1+((-curRotation+1)&3), tileIndex (nTile, (curRotation-nRot)&3), tileIndex (0,0), tileIndex (0,0));
		nlassert (other);
		setTile (other->Mesh, other->tile, desc, land);
		modified.insert (other->Mesh);
nlinfo("FDASFFFFFFFFF");
		
		// Add to visited tile set
		visited.insert (other);

		// get corner
		other=pTile->getRightBottom256 (0, nRot);
		desc.setTile (1, 1+((-curRotation+2)&3), tileIndex (nTile, (curRotation-nRot)&3), tileIndex (0,0), tileIndex (0,0));
nlinfo("ACBCDEFG");
		nlassert (other);
		nlassert (other==pTile->getBottomRight256 (0, nRot));
nlinfo("ACBCDEFG");
//		setTile (other->Mesh, other->tile, desc, land);
//		modified.insert (other->Mesh);
nlinfo("ACBCDEFG");
		// Add to visited tile set
		visited.insert (other);
*/
  }
	else
	{
		tileDesc desc;
		desc.setTile (1, 0, tileIndex (nTile, curRotation), tileIndex (0,0), tileIndex (0,0));
		setTile (pTile->Mesh, pTile->tile, desc, land);
		modified.insert (pTile->Mesh);
	}

	// return value
	bool bContinue=true;
//nlinfo("GDBDF");

	if (_256)
	{
		int nRot;
		PaintTile* other;

		// Main
/*		if (pTile->voisins[3])
			if (!PropagateBorder (pTile->voisins[3], (pTile->rotate[3]+curRotation)&3, tileSet, visited, bank, modified,
					vectMesh, land))
			{
				bContinue=false;
				goto zap;
			}
		if (pTile->voisins[0])
			if (!PropagateBorder (pTile->voisins[0], (pTile->rotate[0]+curRotation)&3, tileSet, visited, bank, modified,
					vectMesh, land))
			{
				bContinue=false;
				goto zap;
			}

		// Bottom
		other=pTile->getBottom256 (0, nRot);
		if (other->voisins[(0-nRot)&3])
			if (!PropagateBorder (other->voisins[(0-nRot)&3], (other->rotate[(0-nRot)&3]+curRotation)&3, tileSet, visited, bank, modified,
					vectMesh, land))
			{
				bContinue=false;
				goto zap;
			}
		if (other->voisins[(1-nRot)&3])
			if (!PropagateBorder (other->voisins[(1-nRot)&3], (other->rotate[(1-nRot)&3]+curRotation)&3, tileSet, visited, bank, modified,
					vectMesh, land))
			{
				bContinue=false;
				goto zap;
			}


		// Corner
		other=pTile->getBottomRight256 (0, nRot);
		if (other->voisins[(1-nRot)&3])
			if (!PropagateBorder (other->voisins[(1-nRot)&3], (other->rotate[(1-nRot)&3]+curRotation)&3, tileSet, visited, bank, modified,
					vectMesh, land))
			{
				bContinue=false;
				goto zap;
			}
		if (other->voisins[(2-nRot)&3])
			if (!PropagateBorder (other->voisins[(2-nRot)&3], (other->rotate[(2-nRot)&3]+curRotation)&3, tileSet, visited, bank, modified,
					vectMesh, land))
			{
				bContinue=false;
				goto zap;
			}


		// Right
		other=pTile->getRight256 (0, nRot);
		if (other->voisins[(2-nRot)&3])
			if (!PropagateBorder (other->voisins[(2-nRot)&3], (other->rotate[(2-nRot)&3]+curRotation)&3, tileSet, visited, bank, modified,
					vectMesh, land))
			{
				bContinue=false;
				goto zap;
			}
		if (other->voisins[(3-nRot)&3])
			if (!PropagateBorder (other->voisins[(3-nRot)&3], (other->rotate[(3-nRot)&3]+curRotation)&3, tileSet, visited, bank, modified,
					vectMesh, land))
			{
				bContinue=false;
				goto zap;
			}
zap:;
	}
	else
	{
		// For all patches voisins
		for (int i=0; i<4; i++)
		{
			// 4) Propagate the borders to voisins
			if (pTile->voisins[i])
			{
				if (!PropagateBorder (pTile->voisins[i], (pTile->rotate[i]+curRotation)&3, tileSet, visited, bank, modified,
					vectMesh, land))
				{
					bContinue=false;
					break;
				}
			}
		}
	}

	// Backup !!
	if (!bContinue)
	{
		setTile (pTile->Mesh, pTile->tile, backup,  land);
			
		if (_256)
		{
			// get right
			int nRot;
			PaintTile* other=pTile->getRight256 (0, nRot);
			nlassert (other);
			setTile (other->Mesh, other->tile, backupRight, land);

			// get bottom
			other=pTile->getBottom256 (0, nRot);
			nlassert (other);
			setTile (other->Mesh, other->tile, backupBottom, land);

			// get corner
			other=pTile->getRightBottom256 (0, nRot);
			nlassert (other);
			nlassert (other==pTile->getBottomRight256 (0, nRot));
			setTile (other->Mesh, other->tile, backupCorner, land);
		}
	}
*/
  }
}

// ***************************************************************************
void			CPainter::paintPatch(sint zoneId, sint patchId)
{
	nlinfo("PpaintPatch =%d",patchId);
	if (paint==true)
	{
		CZone *theZone = _Landscape->getZone( zoneId );
	nlinfo("PpaintPatch2 %d", zoneId);

		if (!fillMode)
		{
			if (patchId>3)
			{
				for(sint myFill=(sint) patchId-3; myFill<=(sint) patchId+3;myFill++)
				{
					std::vector<CTileElement> copyZone = theZone->getPatchTexture ( myFill );
					copyZone[myFill].Tile[0]=selectedTile;
					theZone->changePatchTextureAndColor ( (sint) myFill, &copyZone, NULL);
				}
			}
		} 
		else
	{
			nlinfo("PpaintPatch3");

			std::vector<CTileElement> copyZone = theZone->getPatchTexture ( (sint) patchId );
			for(sint i=0;i<(sint)theZone->getNumPatchs()-1;i++)
			{
				nlinfo("PpaintPatch5");

				std::vector<CTileElement> copyZone = theZone->getPatchTexture ( i );
				copyZone[i].Tile[0]=selectedTile;
				copyZone[i].setTileOrient(1, 2);
				copyZone[i].setTile256Info( true, 0);
				theZone->changePatchTextureAndColor ( i, &copyZone, NULL);
//				nlinfo("True, %d of %d", i, (sint)theZone->getNumPatchs());
			}
		}
}



} // NL3D
}
/** \ode_interface.cpp
 * <File description>
 *
 * $Id: ode_interface.cpp,v 1.2 2002/06/06 05:51:01 robofly Exp $
 */

/* Copyright, 2001 InOrbit Entertainment, Inc.
 *
 *
 * This file is part of WARP STORM (alpha).
 * WARP STORM (alpha) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the 
 * Free Software Foundation; either version 2, or (at your option) any later 
 * version.
 *
 * WARP STORM (alpha) is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.

 * You should have received a copy of the GNU General Public License along 
 * with WARP STORM (alpha); see the file COPYING. If not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#include "ode_interface.h"
#include "nel/misc/types_nl.h"
#include "ode/ode.h"

// BUGGY test
// some constants

#define LENGTH 0.7	// chassis length
#define WIDTH 0.5	// chassis width
#define HEIGHT 0.2	// chassis height
#define RADIUS 0.18	// wheel radius
#define STARTZ 0.5	// starting height of chassis
#define CMASS 1		// chassis mass
#define WMASS 0.2	// wheel mass


// dynamics and collision objects (chassis, 3 wheels, environment)

static dWorldID world;
static dSpaceID space;
static dBodyID body[4];
static dJointID joint[3];	// joint[0] is the front wheel
static dJointGroupID contactgroup;
static dGeomID ground;
static dGeomID box[1];
static dGeomID sphere[3];
static dGeomID ground_box;

// things that the user controls

static dReal speed=0,steer=0;	// user commands

// this is called by dSpaceCollide when two objects in space are
// potentially colliding.

static void pacsTriggerCallback ()
{
  dContact contact;
  contact.surface.mode = dContactSlip1 | dContactSlip2 |
    dContactSoftERP | dContactSoftCFM;
  contact.surface.mu = dInfinity;
  contact.surface.slip1 = 0.1;
  contact.surface.slip2 = 0.1;
  contact.surface.soft_erp = 0.5;
  contact.surface.soft_cfm = 0.3;

  contact.geom.pos = POS;
  contact.geom.normal = NORMAL;
  contact.geom.depth = DEPTH;
  contact.geom.g1 = 0;
  contact.geom.g2 = 0;

  contact.fdir1 = NORMAL;

  dJointID c = dJointCreateContact (world,contactgroup,&contact);
  dJointAttach (c,dGeomGetBody(o1),dGeomGetBody(o2));
}


// initialize the world
static void initOde ()
{
	 if (ShowDebugInChat) nlinfo("initOde");
	 world = dWorldCreate();
	 space = dSpaceCreate();
	 contactgroup = dJointGroupCreate (1000000);
	 dWorldSetGravity (world,0,0,-0.5);
	 dCreatePlane (space, 0, 0, 4,0);
     static float xyz[3] = {2.1640f,-1.3079f,1.7600f};
     static float hpr[3] = {125.5000f,-17.0000f,0.0000f};

}



// simulation loop
static void simLoop (int pause)
{
		
//nlinfo("simLoop");
  dSpaceCollide (space,0, &nearCallback);
//nlinfo("wWorldStep");
  if (!pause) dWorldStep (world,0.05);
//nlinfo("wWorldStep done");
	
  // remove all contact joints
  dJointGroupEmpty (contactgroup);

  myLocalPointer->updateAllPositions();
//nlinfo("simLoop done");

}



COdeInterface::~COdeInterface()
{
  // destroy world
  dJointGroupDestroy (contactgroup);
  dSpaceDestroy (space);
  dWorldDestroy (world);
}


/*******************************************************************************/
//Jul: trying to convert the data for pacs:
// Some defines
#define ARENA_SIZE 50
#define NUM_CELL 10
#define SIZE_PRIMITIVE_MAX 2
#define BORDER_SIZE SIZE_PRIMITIVE_MAX
#define MAX_WORLD_IMAGE 31


//the function
void buildPacsData()
{
  int i = 0;

  // Array of dynamic objects
  std::vector<CObjectDyn*>	arrayObj;
  
  //for every entities:
  Vector <CEntities> tmp;
  tmp = CollisionManager.getEntities(&tmp);
  for(i; i < tmp.size(); i++)
	{
	  //what do we need here to add entities? positions, scene, rotations
	  //get the bodyID
	  dBodyID BoID = dGeomGetBody(tmp[i].geom);
	  //lets get some data:
	  //dMass mass;			// mass parameters about POR
	  //dMatrix3 invI;		// inverse of mass.I
	  //dReal invMass;		// 1 / mass.mass
	  //dVector3 pos;		// position of POR (point of reference)
	  //dQuaternion q;		// orientation quaternion
	  //dMatrix3 R;			// rotation matrix, always corresponds to q
	  //dVector3 lvel,avel;		// linear and angular velocity of POR
	  //NB: typedef dReal dVector3[4];
	  
	  //i guess we shall be using this kind of functions
	  const dReal * dBodyGetPosition   (BoID);
	  const dReal * dBodyGetRotation   (BoID);
	  const dReal * dBodyGetQuaternion (BoID);
	  const dReal * dBodyGetLinearVel  (BoID);
	  const dReal * dBodyGetAngularVel (BoID);

	  //to be changed and feeded with the right datas...
	  // Create one object
	  if(tmp[i].objtype) //this one is a box
		{
		arrayObj.push_back (CObjectDyn (double width, double depth, double height, double orientation, const NLMISC::CVectorD& pos, const NLMISC::CVectorD& speed, true, NLPACS::UMoveContainer &container, NL3D::UScene &scene, UMovePrimitive::Reflexion,  1, 30, worldImage));

		}
	  else //this one is a sphere
		{
		  /// Cylinder constructor
		  CObjectDyn (double radius, double height, const NLMISC::CVectorD& pos, const NLMISC::CVectorD& speed, bool obstacle, NLPACS::UMoveContainer &container, NL3D::UScene &scene, UMovePrimitive::Reflexion,  1, 30, worldImage);
		}
	}
  //now let s get the triggers:
  
  // Eval the system
  MoveContainer->evalCollision (deltaTime, 0);
  
  // Callback info vector
  uint triggerCount=MoveContainer->getNumTriggerInfo();
  static std::vector<UTriggerInfo> tableID;
  tableID.resize (triggerCount);
  
  // Fill it
  for (uint i=0; i<triggerCount; i++)
	{
	  // Copy trigger info
	  tableID[i]=MoveContainer->getTriggerInfo (i);
	}
  //let s hope that the trigger give me the information that would 
  //help build the contatct join: position ,surface normal vector, penetration depth.
  //also I shall remove all the contacts joins if the 2 bodies are already connected by 
  //a joint.
  //for every Trigger:
  for(i = 0; i < triggerCount; i++)
	{
	  //if they ARE not connected:
	  //i JUST HAVE TO KNOW WHO ARE THE 2 OBJECTS THE TRIGGERS REFERS TO. 
	  //i CAN KNOW THAT BY LOOKING WHO ARE THE OBJECTS AT THIS POSITION
	  //pb: complexity is O(M* n) (numbertrigger * numberObjects)
	  if(! dAreConnected (b1,b2) )
		{
		  dJointID c = dJointCreateContact ( getTriggerInfo(i));
		  dJointAttach (c,b1,b2);
		}
	}
}


vector<CTransformShape*> *_transVector;

// ***************************************************************************
void COdeInterface::init(CLandscape *land, vector<CTransformShape*> *transVector, vector<uint32> idVector, float radMin, float radDelta)
{
	_Landscape= land;
	_RadMin= radMin;
	_RadMax= radMin+radDelta;
	_transVector = transVector;

	 initOde();

	 fn.version = DS_VERSION;
     fn.start = 0;
	 fn.step = &simLoop;
	 fn.command = 0;
	 fn.stop = 0;

//	 fn.command('B'); 
//	dCreatePlane (space,0,0,1,0);
}


void COdeInterface::update(int pause)
{
	fn.step( pause );
}

void COdeInterface::updateAllPositions()
{
//	nlinfo("In updateAll");
	const double* odePos;
    for (int j=0; j<num; j++)	
	{
//		if (entityIds[j]!=0)
//		{
		if (body[j]!=0)
		{
			odePos = dBodyGetPosition(body[j]);
			if (entityIds[j]==72)
			{
//				nlinfo("Updating camera to: %f %f %f", odePos[0], odePos[1], odePos[2]);
				odePos = dBodyGetPosition(body[j]);
				LocalArea->User.setPos((float) odePos[0], (float) odePos[1], (float) odePos[2]);
			}
			else
			{
//				nlinfo("note me %d", j);
				vector<CTransformShape *>::iterator	itsh;
				for(itsh = _transVector->begin(); itsh!=_transVector->end();itsh++)
				{
//					nlinfo("Updating object to: %f %f %f", odePos[0], odePos[1], odePos[2]);
					CMatrix transform;
					transform.identity();
					transform.setPos( CVector((float) odePos[0], (float) odePos[1], (float) odePos[2]) );
					(*itsh)->setMatrix( transform );
				}
	
//				LocalArea->neighbors()[entityIds[j]]->setPos(odePos[0], odePos[1], odePos[2]);
//				oldPos[j][0]=odePos[0];
//				oldPos[j][1]=odePos[1];
//				oldPos[j][2]=odePos[2];
			}

//		}
		}
	}
}

void COdeInterface::setpos(uint32 theEntityId, const NLMISC::CVector&  pos)
{
    for (int j=0; j<num; j++)	
	{
		if (body[j]!=0)
		{
			if (entityIds[j]==(int)theEntityId)
			{
				dBodySetPosition (body[j],pos.x , pos.y, pos.z);
			}
		}
	}
}




void COdeInterface::addForce(int id, float fx, float fy, float fz )
{
	nlinfo("adding force");
	dBodyAddRelForce(body[id], fx, fy, fz);
}

void COdeInterface::setForceTo(int id, float fx, float fy, float fz )
{
	nlinfo("setting force");
	dBodySetLinearVel  (body[id], fx, fy, fz);
	dBodySetAngularVel (body[id], fx, fy, fz);
}


int COdeInterface::addBoxGeometry( const NLMISC::CVector& pos, float side0, float side1, float side2, uint32 theEntityId)
{ 
	nlinfo( "in addBoxGeometry the num=%d", num );
	int i;
    if (num < NUM) {
      i = num;
      num++;
    }
    else {
      i = nextobj;
      nextobj++;
      if (nextobj >= num) nextobj = 0;
      dBodyDestroy (body[i]);
      dGeomDestroy (geom[i]);
    }
	nlinfo( "in addBoxGeometry 2" );

    body[i] = dBodyCreate (world);

    sides[i][0] = side0;
    sides[i][1] = side1;
    sides[i][2] = side2;
  
	nlinfo( "in addBoxGeometry 3 %f", pos.z );
	dBodySetPosition (body[i],pos.x , pos.y, pos.z);
	nlinfo( "in addBoxGeometry 4 %f", pos.x );

//    dMatrix3 R;
  //  dRFromAxisAndAngle (R,dRandReal()*2.0-1.0,dRandReal()*2.0-1.0,
//			dRandReal()*2.0-1.0,dRandReal()*10.0-5.0);
 //   dBodySetRotation (body[i],R);

   dMass m;
// dMassSetBox (&m,1,sides[i][0],sides[i][1],sides[i][2]);
   dMassSetZero (&m);

   if (theEntityId==89) dMassSetSphere (&m,1.4, side0);
   else dMassSetSphere (&m, side1, side0);
 
   dMassAdjust (&m,MASS);
   dBodySetMass (body[i],&m);
   dBodySetData (body[i],(void*) i);
   if (theEntityId==89) geom[i] = dCreateSphere (space, 2.0f);
   else geom[i] = dCreateSphere (space, side0 );
   dGeomSetData (geom[i],(void*) i);
	
//	nlinfo( "in addBoxGeometr5" );

    objtype[i] = 0;
	entityIds[i] = theEntityId;
	nlinfo( "in addBoxGeometr6" );

    dGeomSetBody (geom[i],body[i]);
	nlinfo( "in addBoxGeometr9" );
	return i;
}




/** \file sound.cpp
 * Sound interface between the game and NeL
 *
 * $Id: sound.cpp,v 1.1 2002/03/20 18:05:25 robofly Exp $
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

#include <math.h>
#include <vector>

#include <nel/misc/vector.h>

#include <nel/sound/u_audio_mixer.h>
#include <nel/sound/u_env_sound.h>
#include <nel/sound/u_listener.h>
#include <nel/sound/u_source.h>

#include "client.h"
#include "entities.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLSOUND;

//
// Variables
//

UAudioMixer *AudioMixer = NULL;

TSoundId SoundId;

const vector<TSoundId> *SoundIdArray;

//
// Functions
//

void	initSound()
{
	AudioMixer = UAudioMixer::createAudioMixer ();
	AudioMixer->init ();

	AudioMixer->loadSoundBuffers ("sounds.nss");
/*	nlassert( SoundIdArray->size() == 2 );
	SoundId = (*SoundIdArray)[0];
	StSoundId = (*SoundIdArray)[1];
*/
}


void	playSound (CEntity &entity, TSoundId id)
{
	entity.Source = AudioMixer->createSource (id);
	entity.Source->setLooping (true);
	entity.Source->play ();
}


void	playSoundEnvFX (uint8 channel, CEntity &entity, string id, bool loop, float gain)
{
	if (channel=0)
	{
		entity.SourceEnvFX0 = AudioMixer->createSource (id.c_str());
		entity.SourceEnvFX0->setLooping (loop);
		entity.SourceEnvFX0->setGain(gain);
		entity.SourceEnvFX0->play ();
	}
	else if (channel=1)
	{
		entity.SourceEnvFX1 = AudioMixer->createSource (id.c_str());
		entity.SourceEnvFX1->setLooping (loop);
		entity.SourceEnvFX1->setGain(gain);
		entity.SourceEnvFX1->play ();
	}
}

void	deleteSound (CEntity &entity)
{
	if (entity.Source != NULL)
	{
		if (entity.Source->isPlaying ())
			entity.Source->stop ();

		AudioMixer->removeSource (entity.Source);
		entity.Source = NULL;
	}
}

void	deleteSoundEnvFX (uint8 channel, CEntity &entity)
{
	if (channel=0)
	{
		if (entity.SourceEnvFX0 != NULL)
		{
			if (entity.SourceEnvFX0->isPlaying ())
				entity.SourceEnvFX0->stop ();

			AudioMixer->removeSource (entity.SourceEnvFX0);
			entity.SourceEnvFX0 = NULL;
		}
	}
	else if (channel=1)
	{
		if (entity.SourceEnvFX1 != NULL)
		{
			if (entity.SourceEnvFX1->isPlaying ())
				entity.SourceEnvFX1->stop ();

			AudioMixer->removeSource (entity.SourceEnvFX1);
			entity.SourceEnvFX1 = NULL;
		}
	}
}

void	setEnvFXGain (uint8 channel, CEntity &entity, float gain)
{
	if (channel=0)
	{
		if (entity.SourceEnvFX0 != NULL)
		{
			if (entity.SourceEnvFX0->isPlaying ())
				entity.SourceEnvFX0->setGain(gain);
		}
	}
	else if (channel=1)
	{
		if (entity.SourceEnvFX1 != NULL)
		{
			if (entity.SourceEnvFX1->isPlaying ())
				entity.SourceEnvFX1->setGain(gain);
		}
	}
}

void	updateSound()
{
	// Move forward
	AudioMixer->getListener()->setPos( Self->Position );
	AudioMixer->getListener()->setVelocity( CVector(Self->Speed, Self->Speed, Self->Speed));

}

void	releaseSound()
{
	deleteSoundEnvFX(0, *Self);
	deleteSoundEnvFX(1, *Self);
	deleteSound(*Self);
	/// \todo 
}

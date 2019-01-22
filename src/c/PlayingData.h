/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#pragma once

#include "RegisteredSoundData.h"
#include "AbstractSoundData.h"
#include "ogg/ogg.h"
#include "vorbis/vorbisfile.h"

class PlayingData : public AbstractSoundData
{
public:
	PlayingData(RegisteredSoundData* registeredSoundData, std::string soundId);
	~PlayingData();

	void initialize();

	void write_raw_pcm(int* combinedBuffer, int numSamples);
	void pause();
	void resume();
	void stop();
	void mute();
	void unmute();

	void destroy();

	ogg_int64_t get_current_position();
	std::string _get_registered_sound_id();

	void _pause_by_group(std::string soundGroupId);
	void _resume_by_group(std::string soundGroupId);
	void _mute_by_group(std::string soundGroupId);
	void _unmute_by_group(std::string soundGroupId);

private:
	
	RegisteredSoundData* _registeredSoundData;
	ogg_int64_t _currentPositionOnBinary;

	OggVorbis_File _oggFile;

	char* _buffer;
	int* _intBuffer;
};


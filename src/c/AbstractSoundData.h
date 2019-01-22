/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#pragma once

#include "AbstractEffectCommand.h"
#include "SoundStatus.h"
#include <list>
#include <string>

class AbstractSoundData
{
public:
	AbstractSoundData(std::string id);
	virtual ~AbstractSoundData();

	virtual void write_raw_pcm(int* combinedBuffer, int size);
	virtual void pause();
	virtual void resume();
	virtual void stop();
	virtual void mute();
	virtual void unmute();
	
	
	float get_volume();
	void set_volume(float volume);

	std::string get_id();
	SoundStatus get_status();
	std::string get_parent();



	virtual void _pause_by_group(std::string soundGroupId);
	virtual void _resume_by_group(std::string soundGroupId);
	virtual void _mute_by_group(std::string soundGroupId);
	virtual void _unmute_by_group(std::string soundGroupId);
	
	void _setParent(std::string parentGroupId);
	

protected:
	std::string _id;
	std::string _parentGroupId;
	float _currentVolume = 1;
	SoundStatus _currentStatus = SoundStatus::PLAYING;
	bool _isMuted = false;

	std::list<std::string>* _mutedGroupList;
	std::list<std::string>* _pausedGroupList;

	std::list<AbstractEffectCommand*>* _effectCommandList;
};


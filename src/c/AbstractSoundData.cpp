/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#include "AbstractSoundData.h"
#include "SoundStatus.h"

#include <algorithm>
#include <string>

AbstractSoundData::AbstractSoundData(std::string id)
{
	_id = id;
	_isMuted = false;
	_currentStatus = SoundStatus::PLAYING;
	_currentVolume = 1.0f;


	_mutedGroupList = new std::list<std::string>();
	_pausedGroupList = new std::list<std::string>();

	_effectCommandList = new std::list<AbstractEffectCommand*>();
}


AbstractSoundData::~AbstractSoundData()
{
	delete _mutedGroupList;
	delete _pausedGroupList;

	size_t size = _effectCommandList->size();

	if (size > 0)
	{
		std::for_each(_effectCommandList->cbegin(), _effectCommandList->cend(), [](AbstractEffectCommand* command) {
			delete command;
		});
	}

	delete _effectCommandList;
}

void AbstractSoundData::write_raw_pcm(int* combinedBuffer, int size)
{
}

void AbstractSoundData::pause()
{
}

void AbstractSoundData::resume()
{
}

void AbstractSoundData::stop()
{
}

void AbstractSoundData::mute()
{
}

void AbstractSoundData::unmute()
{
}

std::string AbstractSoundData::get_id()
{
	return _id;
}

float AbstractSoundData::get_volume()
{
	return _currentVolume;
}

void AbstractSoundData::set_volume(float volume)
{
	if (volume > 1)
	{
		_currentVolume = 1;
	}
	else if (volume < 0)
	{
		_currentVolume = 0;
	}
	else
	{
		_currentVolume = volume;
	}
}

SoundStatus AbstractSoundData::get_status()
{
	return _currentStatus;
}

std::string AbstractSoundData::get_parent()
{
	return _parentGroupId;
}
void AbstractSoundData::_setParent(std::string parentGroupId)
{
	_parentGroupId = parentGroupId;
}

void AbstractSoundData::_pause_by_group(std::string soundGroupId)
{
}

void AbstractSoundData::_resume_by_group(std::string soundGroupId)
{
}

void AbstractSoundData::_mute_by_group(std::string soundGroupId)
{
}

void AbstractSoundData::_unmute_by_group(std::string soundGroupId)
{
}

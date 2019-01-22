/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#pragma once

#include "SoundEventData.h"
#include "SoundEventType.h"

SoundEventData::SoundEventData(std::string registeredSoundId, std::string soundId, SoundEventType type)
{
	_rid = registeredSoundId;
	_sid = soundId;
	_type = type;
}

SoundEventData::~SoundEventData()
{

}

std::string SoundEventData::get_rid()
{
	return _rid;
}

std::string SoundEventData::get_sid()
{
	return _sid;
}

SoundEventType SoundEventData::get_type()
{
	return _type;
}
/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#pragma once
#include "SoundEventType.h"
#include <string>

class SoundEventData
{
public:
	SoundEventData(std::string registeredSoundId, std::string soundId ,SoundEventType type);
	~SoundEventData();

	std::string get_rid();
	std::string get_sid();
	SoundEventType get_type();

private:
	std::string _rid;
	std::string _sid;
	SoundEventType _type;
};

/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#pragma once

#include "AbstractSoundData.h"
#include "PlayingData.h"

class SoundGroup : public AbstractSoundData
{
public:
	SoundGroup(std::string id, int maxPolyphony = -1);
	~SoundGroup();

	void add_sound(PlayingData* data);
	void remove_sound(PlayingData* data);
	void add_group(SoundGroup* group);
	void remove_group(SoundGroup* group);

	void set_ducker(SoundGroup* triggerGroup, float ratio, float attackTimeByMS = 0, float releaseTimeByMS = 0);
	
	int get_current_polyphony();
	int get_max_polyphony();

	void write_raw_pcm(int* combinedBuffer, int numSamples);
	void pause();
	void resume();
	void stop();
	void mute();
	void unmute();

	void _pause_by_group(std::string soundGroupId);
	void _resume_by_group(std::string soundGroupId);
	void _mute_by_group(std::string soundGroupId);
	void _unmute_by_group(std::string soundGroupId);

	void _registerStoppedSound(PlayingData* data);

private:
	int _maxPolyphony;
	int _currentPolyphony;
	std::list<AbstractSoundData*>* _soundDataList;
	std::list<PlayingData*>* _stoppedSoundDataList;

	void addData(AbstractSoundData* data);
	void removeData(AbstractSoundData* data);


};


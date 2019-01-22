/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#include "SoundGroup.h"
#include "DuckerCommand.h"
#include "Harmonia.h"
#include <list>
#include <algorithm>
#include <typeinfo>


SoundGroup::SoundGroup(std::string id, int maxPolyphony) : AbstractSoundData(id)
{
	_soundDataList = new std::list<AbstractSoundData*>();
	_stoppedSoundDataList = new std::list<PlayingData*>();

	_maxPolyphony = maxPolyphony;
	_currentPolyphony = 0;
}


SoundGroup::~SoundGroup()
{
	size_t size = _soundDataList->size();

	if (size > 0)
	{
		std::for_each(_soundDataList->cbegin(), _soundDataList->cend(), [](AbstractSoundData* data) {
			delete data;
		});
	}

	_stoppedSoundDataList->clear();
	delete _stoppedSoundDataList;

	_soundDataList->clear();
	delete _soundDataList;
}


void SoundGroup::add_sound(PlayingData* data)
{
	if (_maxPolyphony == _currentPolyphony)
	{
		return;
	}

	_currentPolyphony++;
	addData(data);
}

void SoundGroup::remove_sound(PlayingData* data)
{
	_currentPolyphony--;

	Harmonia::_remove_playing_data(data);
	removeData(data);
}

void SoundGroup::add_group(SoundGroup* group)
{
	addData(group);
}
void SoundGroup::remove_group(SoundGroup* group)
{
	removeData(group);
}


void SoundGroup::set_ducker(SoundGroup* triggerGroup, float ratio, float attackTimeByMS, float releaseTimeByMS)
{
	_effectCommandList->push_back(new DuckerCommand(triggerGroup, ratio, attackTimeByMS, releaseTimeByMS));
}

int SoundGroup::get_current_polyphony()
{
	return _currentPolyphony;
}

int SoundGroup::get_max_polyphony()
{
	return _maxPolyphony;
}

void SoundGroup::write_raw_pcm(int* combinedBuffer, int numSamples)
{
	// 再生中以外、あるいはグループによって一時停止されている場合は、何もせずに終了
	if (_currentStatus != SoundStatus::PLAYING || _pausedGroupList->size() > 0)
	{
		return;
	}

	int* buffer = new int[numSamples] {0};

	// 停止しているものは削除
	size_t size = _stoppedSoundDataList->size();
	if (size > 0)
	{
		std::for_each(_stoppedSoundDataList->cbegin(), _stoppedSoundDataList->cend(), [](PlayingData* data) {
			data->destroy();
		});
		_stoppedSoundDataList->clear();
	}

	// 書き込み
	size = _soundDataList->size();
	if (size > 0)
	{
		std::for_each(_soundDataList->cbegin(), _soundDataList->cend(), [buffer, numSamples](AbstractSoundData* data) {
			data->write_raw_pcm(buffer, numSamples);
		});
	}

	// ボリューム反映
	for (int i = 0; i < numSamples; i++)
	{
		buffer[i] = (int)(buffer[i] * _currentVolume);
	}
	
	// エフェクトコマンド実行
	size = _effectCommandList->size();
	if (size > 0)
	{
		std::for_each(_effectCommandList->cbegin(), _effectCommandList->cend(), [buffer, numSamples](AbstractEffectCommand* command) {
			command->execute(buffer, numSamples);
		});
	}

	// 合算
	for (int i = 0; i < numSamples; i++)
	{
		combinedBuffer[i] += buffer[i];
	}

	delete[] buffer;
}

void SoundGroup::pause()
{
	if (_currentStatus != SoundStatus::PLAYING)
	{
		return;
	}

	_currentStatus = SoundStatus::PAUSED;
	
	_pause_by_group(_id);
}

void SoundGroup::resume()
{
	if (_currentStatus != SoundStatus::PAUSED)
	{
		return;
	}

	_currentStatus = SoundStatus::PLAYING;

	_resume_by_group(_id);
}

void SoundGroup::stop()
{
	printf("group stop [%s]\n", _id.c_str());
	size_t size = _soundDataList->size();
	if (size > 0)
	{
		std::for_each(_soundDataList->cbegin(), _soundDataList->cend(), [](AbstractSoundData* data) {
			data->stop();
		});
	}

}

void SoundGroup::mute()
{
	if (_isMuted)
		return;

	_isMuted = true;

	_mute_by_group(_id);
}

void SoundGroup::unmute()
{
	if (!_isMuted)
		return;

	_isMuted = false;
	
	_unmute_by_group(_id);
}



void SoundGroup::_pause_by_group(std::string soundGroupId)
{
	_pausedGroupList->push_back(soundGroupId);

	size_t size = _soundDataList->size();
	if (size > 0)
	{
		std::for_each(_soundDataList->cbegin(), _soundDataList->cend(), [soundGroupId](AbstractSoundData* data) {
			data->_pause_by_group(soundGroupId);
		});
	}
}

void SoundGroup::_resume_by_group(std::string soundGroupId)
{
	_pausedGroupList->remove(soundGroupId);

	size_t size = _soundDataList->size();
	if (size > 0)
	{
		std::for_each(_soundDataList->cbegin(), _soundDataList->cend(), [soundGroupId](AbstractSoundData* data) {
			data->_resume_by_group(soundGroupId);
		});
	}
}

void SoundGroup::_mute_by_group(std::string soundGroupId)
{
	_mutedGroupList->push_back(soundGroupId);

	size_t size = _soundDataList->size();
	if (size > 0)
	{
		std::for_each(_soundDataList->cbegin(), _soundDataList->cend(), [soundGroupId](AbstractSoundData* data) {
			data->_mute_by_group(soundGroupId);
		});
	}
}

void SoundGroup::_unmute_by_group(std::string soundGroupId)
{
	_mutedGroupList->remove(soundGroupId);

	size_t size = _soundDataList->size();
	if (size > 0)
	{
		std::for_each(_soundDataList->cbegin(), _soundDataList->cend(), [soundGroupId](AbstractSoundData* data) {
			data->_unmute_by_group(soundGroupId);
		});
	}
}



void SoundGroup::_registerStoppedSound(PlayingData* data)
{
	_stoppedSoundDataList->push_back(data);
}

void SoundGroup::addData(AbstractSoundData* data)
{
	_soundDataList->push_back(data);
	data->_setParent(_id);

	// 子にミュート状態とポーズ状態を継承
	size_t size = _mutedGroupList->size();
	if (size > 0)
	{
		std::for_each(_mutedGroupList->cbegin(), _mutedGroupList->cend(), [data](std::string groupId) {
			data->_mute_by_group(groupId);
		});
	}

	size = _pausedGroupList->size();
	if (size > 0)
	{
		std::for_each(_pausedGroupList->cbegin(), _pausedGroupList->cend(), [data](std::string groupId) {
			data->_pause_by_group(groupId);
		});
	}

	if (_isMuted)
	{
		data->_mute_by_group(_id);
	}

	if (_currentStatus == SoundStatus::PAUSED)
	{
		data->_pause_by_group(_id);
	}

}
void SoundGroup::removeData(AbstractSoundData* data)
{
	_soundDataList->remove(data);
	delete data;
}





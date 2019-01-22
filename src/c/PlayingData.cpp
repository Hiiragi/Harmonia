/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#include "PlayingData.h"
#include "SoundGroup.h"
#include "Harmonia.h"
#include "SoundStatus.h"
#include "SoundEventType.h"

#include "ogg/ogg.h"
#include "vorbis/vorbisfile.h"

#include <algorithm>
#include <string>



ogg_int64_t __currentPositionOnBinary = 0;
ogg_int64_t __binarySize = 0;

size_t read_callback(void* ptr, size_t size, size_t nmemb, void *datasource)
{
	ogg_int64_t resSize;
	ogg_int64_t count;
	ogg_int64_t writeSize;
	ogg_int64_t currentPositionOnBinary = __currentPositionOnBinary;

	if (datasource == 0)
	{
		return 0;
	}

	// 取得可能カウント数を算出
	resSize = __binarySize - currentPositionOnBinary;
	count = resSize / size;
	if (count > (int)nmemb)
	{
		count = nmemb;
	}

	writeSize = size * count;

	memcpy(ptr, (char*)datasource + currentPositionOnBinary, writeSize);

	// ポインタ位置を移動
	__currentPositionOnBinary = writeSize + currentPositionOnBinary;


	return count;
}


int seek_callback(void* datasource, ogg_int64_t offset, int whence)
{

	switch (whence)
	{
		case SEEK_CUR:
			__currentPositionOnBinary += offset;
			break;

		case SEEK_END:
			__currentPositionOnBinary = __binarySize + offset;
			break;

		case SEEK_SET:
			__currentPositionOnBinary = offset;
			break;

		default:
			return -1;
	}

	if (__currentPositionOnBinary > __binarySize)
	{
		__currentPositionOnBinary = __binarySize;
		return -1;
	}
	else if (__currentPositionOnBinary < 0)
	{
		__currentPositionOnBinary = 0;
		return -1;
	}

	return 0;
}

int close_callback(void* datasource)
{
	return 0;
}

long tell_callback(void* datasource)
{
	return (long)__currentPositionOnBinary;
}


PlayingData::PlayingData(RegisteredSoundData* registeredSoundData, std::string soundId)
	: AbstractSoundData(soundId)
	, _buffer(nullptr)
{
	_registeredSoundData = registeredSoundData;
	_currentPositionOnBinary = 0;
	_currentStatus = SoundStatus::WAIT_INITIALIZE;
}



PlayingData::~PlayingData()
{
	delete[] _buffer;
    ov_clear(&_oggFile);
}

void PlayingData::initialize()
{
	if (_currentStatus != SoundStatus::WAIT_INITIALIZE)
	{
		return;
	}

	printf("PlayingData construct\n");

	// グローバル変数に割り当て
	__currentPositionOnBinary = _currentPositionOnBinary;
	__binarySize = _registeredSoundData->get_binary_size();

	ov_callbacks callbacks;
	callbacks.read_func = &read_callback;
	callbacks.seek_func = &seek_callback;
	callbacks.close_func = &close_callback;
	callbacks.tell_func = &tell_callback;


	printf("start ov_open_callbacks\n");
	unsigned char* binary = _registeredSoundData->get_binary_data();
	int result = ov_open_callbacks((void *)binary, &_oggFile, 0, 0, callbacks);
	if (result == OV_ENOTVORBIS)
	{
		printf("OV_ENOTVORBIS\n");
	}
	printf("ov_open_callbacks result : %d\n", result);

	vorbis_comment *comment = ov_comment(&_oggFile, -1);

	for (int i = 0; i < comment->comments; i++)
	{
		printf("	%d, %s\n", comment->comment_lengths[i], comment->user_comments[i]);
		if (strncmp(comment->user_comments[i], "LOOPSTART=", 10) == 0)
		{
			//_loopStartPosition = strtol((const char*)(comment->user_comments[i] + 10), NULL, 10);

		}
		else if (strncmp(comment->user_comments[i], "LOOPLENGTH=", 11) == 0)
		{
			//_loopLength = strtol((const char*)(comment->user_comments[i] + 11), NULL, 10);
		}
	}

	//printf("\nLOOPSTART : %lld\n", _loopStartPosition);
	//printf("LOOPLENGTH : %lld\n", _loopLength);

	// グローバル変数から取り出し
	_currentPositionOnBinary = __currentPositionOnBinary;

	_currentStatus = SoundStatus::PLAYING;
}

void PlayingData::write_raw_pcm(int* combinedBuffer, int numSamples)
{
	// 再生中以外、あるいはグループによって一時停止されている場合は、何もせずに終了
	if (_currentStatus != SoundStatus::PLAYING || _pausedGroupList->size() > 0)
	{
		return;
	}

	// グローバル変数に割り当て
	__currentPositionOnBinary = _currentPositionOnBinary;
	__binarySize = _registeredSoundData->get_binary_size();

	// 個数から short 型配列のバイナリサイズを求める
	int bufferSize = numSamples * 2;

	if (_buffer == NULL)
	{
		_buffer = new char[bufferSize];
		_intBuffer = new int[numSamples];
	}

	memset(_buffer, 0, bufferSize);

    const int maxRequestSize = std::min(bufferSize, 4096);
	ogg_int64_t requestSize = maxRequestSize;
	int bitstream = 0;
	ogg_int64_t readSize = 0;
	ogg_int64_t comSize = 0;
	
	ogg_int64_t loopStartPoint = _registeredSoundData->get_loop_start_point();
	ogg_int64_t loopLength = _registeredSoundData->get_loop_length();
	ogg_int64_t loopPoint = loopStartPoint + loopLength;
	ogg_int64_t currentPosition = ov_pcm_tell(&_oggFile);

	bool isMuted = _isMuted || _mutedGroupList->size() > 0;

	//printf("id : %s / current position : %lld / buffer size : %d / loop position : %lld\n", _id, currentPosition, bufferSize, loopPoint);

	if (currentPosition < 0)
	{
		// error
		return;
	}

	// ループありで、今回のバッファ書き込みでループ地点をまたぐ場合の処理
	// 4で割っているのは、2チャンネル * short のバイトサイズ
	ogg_int64_t destinationPoint = currentPosition + bufferSize / 4;
	if (loopLength > 0 && destinationPoint > loopPoint)
	{
		// ミュート時はシークだけして終了
		if (isMuted || _mutedGroupList->size() > 0)
		{
			// seek
			ov_pcm_seek(&_oggFile, destinationPoint - loopPoint + loopStartPoint);

			// グローバル変数から取り出し
			_currentPositionOnBinary = __currentPositionOnBinary;
			__currentPositionOnBinary = 0;
			return;
		}

		// ループ地点まで読み込み
		while (1)
		{
			requestSize = loopPoint - currentPosition;
			if (requestSize > maxRequestSize)
			{
				requestSize = maxRequestSize;
			}

			readSize = ov_read(&_oggFile, (char*)(_buffer + comSize), (int)requestSize, 0, 2, 1, &bitstream);

			if (readSize < 0)
			{
				break;
			}

			currentPosition = ov_pcm_tell(&_oggFile);
			comSize += readSize;

			printf("	adjuting -> request size : %lld, readed size : %lld, position : %lld\n", requestSize, readSize, currentPosition);

			if (currentPosition >= loopPoint)
			{
				break;
			}

		}

		// ループ開始地点へ移動
		int seekResult = ov_pcm_seek(&_oggFile, loopStartPoint);
        if (seekResult < 0)
        {
            // error
        }
        
		currentPosition = ov_pcm_tell(&_oggFile);
		printf("loop back! -> position : %lld\n", currentPosition);

		// 残りを読み込むための前処理
		requestSize = bufferSize - comSize;
		printf("	recent request size : %lld\n", requestSize);
		if (requestSize > maxRequestSize)
		{
			requestSize = maxRequestSize;
		}
		printf("	limited recent request size : %lld\n", requestSize);
	}


	while (1)
	{
		// ミュート時はシークだけして終了
		if (isMuted || _mutedGroupList->size() > 0)
		{
			// seek
			ov_pcm_seek(&_oggFile, destinationPoint);

			// グローバル変数から取り出し
			_currentPositionOnBinary = __currentPositionOnBinary;
			__currentPositionOnBinary = 0;
			return;
		}

		readSize = ov_read(&_oggFile, (char*)(_buffer + comSize), (int)requestSize, 0, 2, 1, &bitstream);
		
		// 読み込みが最後に達したので抜ける
		if (readSize == 0)
		{
			stop();
			if (_id == "")
			{
				Harmonia::_set_captured_event(_get_registered_sound_id(), "", SoundEventType::SOUND_PLAY_COMPLETE);
			}
			else
			{
				Harmonia::_set_captured_event(_get_registered_sound_id(), _id.c_str(), SoundEventType::SOUND_PLAY_COMPLETE);
			}
			
			break;
		}

		comSize += readSize;

		if (comSize >= bufferSize) {
			// バッファを埋め尽くしたので抜ける
			break;
		}

		if (bufferSize - comSize < maxRequestSize) {
			requestSize = bufferSize - comSize;
		}
	}

	// int 配列に変換 & ボリューム反映
	union shortConverter {
		char c[2];
		short value;
	} converter;

	for (int i = 0; i < comSize / 2; ++i)
	{
		converter.c[0] = _buffer[i * 2];
		converter.c[1] = _buffer[i * 2 + 1];

		short sample = (short)(converter.value * _currentVolume);
		_intBuffer[i] = (int)sample;
	}

	// エフェクトコマンド実行
	size_t size = _effectCommandList->size();
	int* intBuffer = _intBuffer;
	if (size > 0)
	{
		std::for_each(_effectCommandList->cbegin(), _effectCommandList->cend(), [intBuffer, bufferSize](AbstractEffectCommand* command) {
			command->execute(intBuffer, bufferSize * 2);
		});
	}

	// int として合算
	for (int i = 0; i < comSize / 2; ++i)
	{
		combinedBuffer[i] += _intBuffer[i];
	}

	// グローバル変数から取り出し
	_currentPositionOnBinary = __currentPositionOnBinary;
	__currentPositionOnBinary = 0;
}


void PlayingData::pause()
{
	printf("sound pause [%s]\n", _id.c_str());
	if (_currentStatus == SoundStatus::PLAYING)
	{
		_currentStatus = SoundStatus::PAUSED;
	}
}

void PlayingData::resume()
{
	printf("sound resume [%s]\n", _id.c_str());
	if (_currentStatus == SoundStatus::PAUSED)
	{
		_currentStatus = SoundStatus::PLAYING;
	}
}

void PlayingData::stop()
{
	printf("sound stop [%s]\n", _id.c_str());
	_currentStatus = SoundStatus::STOPPED;
	// 削除
	SoundGroup* soundGroup = Harmonia::get_group(_parentGroupId.c_str());
	soundGroup->_registerStoppedSound(this);
}

void PlayingData::mute()
{
	printf("sound mute [%s]\n", _id.c_str());

	if (_isMuted)
		return;

	_isMuted = true;
}

void PlayingData::unmute()
{
	printf("sound unmute [%s]\n", _id.c_str());

	if (!_isMuted)
		return;

	_isMuted = false;
}


void PlayingData::destroy()
{
	Harmonia::_remove_playing_data(this);

	SoundGroup* soundGroup = Harmonia::get_group(_parentGroupId.c_str());
	soundGroup->remove_sound(this);
}

ogg_int64_t PlayingData::get_current_position()
{
	ogg_int64_t currentPosition = ov_pcm_tell(&_oggFile);
	if (currentPosition < 0)
	{
		currentPosition = 0;
	}

	return currentPosition;
}

std::string PlayingData::_get_registered_sound_id()
{
	return _registeredSoundData->get_id();
}



void PlayingData::_pause_by_group(std::string soundGroupId)
{
	_pausedGroupList->push_back(soundGroupId);
}

void PlayingData::_resume_by_group(std::string soundGroupId)
{
	_pausedGroupList->remove(soundGroupId);
}

void PlayingData::_mute_by_group(std::string soundGroupId)
{
	_mutedGroupList->push_back(soundGroupId);
}

void PlayingData::_unmute_by_group(std::string soundGroupId)
{
	_mutedGroupList->remove(soundGroupId);
}
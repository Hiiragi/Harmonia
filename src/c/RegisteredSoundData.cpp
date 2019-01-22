/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#include "RegisteredSoundData.h"
#include <string.h>

RegisteredSoundData::RegisteredSoundData(const char* id, unsigned char* binaryData, unsigned int binarySize, unsigned int loopStartPoint, unsigned int loopLength)
{
	// id
	_id = std::string(id);

	// binary
	// メモリアラインメント対応
	short memoryAlignDiff = binarySize % 4;
	if (memoryAlignDiff > 0)
	{
		binarySize += 4 - memoryAlignDiff;
	}

	_binaryData = new unsigned char[binarySize];
	memset(_binaryData, 0, binarySize);
	memcpy(_binaryData, binaryData, binarySize);


	_binarySize = binarySize;
	_loopStartPoint = loopStartPoint;
	_loopLength = loopLength;
}



RegisteredSoundData::~RegisteredSoundData()
{
	delete[] _binaryData;
	_binaryData = nullptr;
}

std::string RegisteredSoundData::get_id()
{
	return _id;
}

unsigned char* RegisteredSoundData::get_binary_data()
{
	return _binaryData;
}

unsigned int RegisteredSoundData::get_binary_size()
{
	return _binarySize;
}

unsigned int RegisteredSoundData::get_loop_start_point()
{
	return _loopStartPoint;
}

unsigned int RegisteredSoundData::get_loop_length()
{
	return _loopLength;
}

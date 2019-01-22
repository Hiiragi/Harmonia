/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#pragma once

#include <string>

class RegisteredSoundData
{
public:
	RegisteredSoundData(const char* id, unsigned char* binaryData, unsigned int binarySize, unsigned int loopStartPoint, unsigned int loopLength);
	~RegisteredSoundData();

	std::string get_id();
	unsigned char* get_binary_data();
	unsigned int get_binary_size();
	unsigned int get_loop_start_point();
	unsigned int get_loop_length();

private:
	std::string _id;
	unsigned char* _binaryData;
	unsigned int _binarySize;
	unsigned int _loopStartPoint;
	unsigned int _loopLength;

};


/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#include "HarmoniaInterface.h"
#include "Harmonia.h"
#include "ogg/ogg.h"
#include <algorithm>
#include <string>
#include <sstream>

// General
#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_initialize(unsigned int bufferSize)
{
	Harmonia::initialize(bufferSize);
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_finalize()
{
	Harmonia::finalize();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_register_sound(const char* id, unsigned char* binaryData, int size, unsigned int loopStartPoint, unsigned int loopLength)
{
	Harmonia::register_sound(id, binaryData, size, loopStartPoint, loopLength);
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_register_sounds(const char** idList, unsigned char** binaryDataList, int* sizeList, unsigned int* loopStartPointList, unsigned int* loopLengthList, unsigned int numRegister)
{
	Harmonia::register_sounds(idList, binaryDataList, sizeList, loopStartPointList, loopLengthList, numRegister);
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_unregister_sound(const char* id)
{
	Harmonia::unregister_sound(id);
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_pause_all()
{
	Harmonia::pause();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_resume_all()
{
	Harmonia::resume();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_stop_all()
{
	Harmonia::stop();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
float harmonia_get_master_volume()
{
	SoundGroup* masterGroup = Harmonia::get_group(Harmonia::MASTER_GROUP_NAME.c_str());
	return masterGroup->get_volume();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_set_master_volume(float volume)
{
	SoundGroup* masterGroup = Harmonia::get_group(Harmonia::MASTER_GROUP_NAME.c_str());
	return masterGroup->set_volume(volume);
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_mute_all()
{
	Harmonia::mute();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_unmute_all()
{
	Harmonia::unmute();
}



#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_start_capture_errors()
{
	Harmonia::start_capture_errors();
}

int _harmonia_capture_data_size;

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
char* harmonia_get_capture_errors()
{
	std::list<HarmoniaErrorData*>* list = Harmonia::get_capture_errors();
	if (list != NULL && list->size() > 0)
	{
		std::string jsonStr = "{\"errors\":[";
		std::for_each(list->begin(), list->end(), [&jsonStr](HarmoniaErrorData* data) {
			int type = static_cast<int>(data->get_error_type());

			std::string typeStr;
#if ANDROID || _ANDROID_
            std::stringstream stream;
            stream << "" << type;
            typeStr = stream.str();
#else
            typeStr = std::to_string(type);
#endif

			jsonStr += "{\"type\":" + typeStr + "},";
		});
		jsonStr = jsonStr.substr(0, jsonStr.size() - 1);
		jsonStr += "]}";

		const char* str = jsonStr.c_str();
		size_t length = strlen(str) + 1;
		char* returnChar = (char*)malloc(length);
#if _WIN32
		strcpy_s(returnChar, length, str);
#else
		strcpy(returnChar, str);
#endif
		_harmonia_capture_data_size = (int)length;
		return returnChar;
	}

	return NULL;
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
char* harmonia_get_capture_errors_with_size(int* size)
{
	char* result = harmonia_get_capture_errors();

	if (result == NULL)
	{
		*size = 0;
	}
	else
	{
		*size = _harmonia_capture_data_size;
	}

	return result;
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_stop_capture_errors()
{
	Harmonia::stop_capture_errors();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_start_capture_events()
{
	Harmonia::start_capture_events();
}



#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
char* harmonia_get_capture_events()
{
	std::list<SoundEventData*>* list = Harmonia::get_captured_events();
	if (list != NULL && list->size() > 0)
	{
		std::string jsonStr = "{\"events\":[";
		std::for_each(list->begin(), list->end(), [&jsonStr](SoundEventData* data) {
			jsonStr += "{\"rid\":\"" + std::string(data->get_rid()) + "\",\"sid\":\"" + std::string(data->get_sid()) + "\",";
			int type = static_cast<int>(data->get_type());

            std::string typeStr;
#if ANDROID || _ANDROID_
            std::stringstream stream;
            stream << "" << type;
            typeStr = stream.str();
#else
            typeStr = std::to_string(type);
#endif

			jsonStr += "\"type\":" + typeStr;
			jsonStr += "},";
			delete data;
		});
		jsonStr = jsonStr.substr(0, jsonStr.size() - 1);
		jsonStr += "]}";

		delete list;

		const char* str = jsonStr.c_str();
		size_t length = strlen(str) + 1;
		char* returnChar = (char*)malloc(length);
#if _WIN32
		strcpy_s(returnChar, length, str);
#else
		strcpy(returnChar, str);
#endif

		_harmonia_capture_data_size = (int)length;
		return returnChar;
	}

	return NULL;
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
char* harmonia_get_capture_events_with_size(int* size)
{
	char* result = harmonia_get_capture_events();

	if (result == NULL)
	{
		*size = 0;
	}
	else
	{
		*size = _harmonia_capture_data_size;
	}

	return result;
}


#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_stop_capture_events()
{
	Harmonia::stop_capture_events();
}

// Sound
#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_sound_play(const char* registeredId, const char* soundId, const char* targetGroupId)
{
	Harmonia::play(registeredId, soundId, targetGroupId);
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_sound_pause(const char* playingDataId)
{
	PlayingData* data = Harmonia::get_playing_data(playingDataId);
	data->pause();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_sound_resume(const char* playingDataId)
{
	PlayingData* data = Harmonia::get_playing_data(playingDataId);
	data->resume();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_sound_stop(const char* playingDataId)
{
	PlayingData* data = Harmonia::get_playing_data(playingDataId);
	data->stop();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_sound_mute(const char* playingDataId)
{
	PlayingData* data = Harmonia::get_playing_data(playingDataId);
	data->mute();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_sound_unmute(const char* playingDataId)
{
	PlayingData* data = Harmonia::get_playing_data(playingDataId);
	data->unmute();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
float harmonia_get_sound_volume(const char* playingDataId)
{
	PlayingData* data = Harmonia::get_playing_data(playingDataId);
	return data->get_volume();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_set_sound_volume(const char* playingDataId, float volume)
{
	PlayingData* data = Harmonia::get_playing_data(playingDataId);
	data->set_volume(volume);
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
unsigned int harmonia_get_sound_status(const char* playingDataId)
{
	PlayingData* data = Harmonia::get_playing_data(playingDataId);
	return data->get_status();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
unsigned int harmonia_get_sound_current_position(const char* playingDataId)
{
	PlayingData* data = Harmonia::get_playing_data(playingDataId);
	return (unsigned int)data->get_current_position();
}




// Group
#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_create_group(const char* groupId, const char* parentGroupId, int maxPolyphony)
{
	Harmonia::create_group(groupId, parentGroupId, maxPolyphony);
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_delete_group(const char* groupId)
{
	Harmonia::delete_group(groupId);
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_group_pause(const char* groupId)
{
	SoundGroup* group = Harmonia::get_group(groupId);
	group->pause();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_group_resume(const char* groupId)
{
	SoundGroup* group = Harmonia::get_group(groupId);
	group->resume();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_group_stop(const char* groupId)
{
	SoundGroup* group = Harmonia::get_group(groupId);
	group->stop();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_group_mute(const char* groupId)
{
	SoundGroup* group = Harmonia::get_group(groupId);
	group->mute();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_group_unmute(const char* groupId)
{
	SoundGroup* group = Harmonia::get_group(groupId);
	group->unmute();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
float harmonia_get_group_volume(const char* groupId)
{
	SoundGroup* group = Harmonia::get_group(groupId);
	return group->get_volume();
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_set_group_volume(const char* groupId, float volume)
{
	SoundGroup* group = Harmonia::get_group(groupId);
	group->set_volume(volume);
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
void harmonia_set_ducker(const char* triggerGroupId, const char* targetGroupId, float ratio, float attackTimeByMS, float releaseTimeByMS)
{
	SoundGroup* triggerGroup = Harmonia::get_group(triggerGroupId);
	SoundGroup* targetGroup = Harmonia::get_group(targetGroupId);
	targetGroup->set_ducker(triggerGroup, ratio, attackTimeByMS, releaseTimeByMS);
}

#if _WIN32
extern "C" _declspec(dllexport)
#else
extern "C"
#endif
unsigned int harmonia_get_group_status(const char* groupId)
{
	SoundGroup* group = Harmonia::get_group(groupId);
	return group->get_status();
}


/*
#if _WIN32

 extern "C" _declspec(dllexport)
 void initialize()
 {
 Harmonia::initialize();
 }
 
 extern "C" _declspec(dllexport)
 unsigned int registerSound(unsigned char* binaryPointer, unsigned long size)
 {
 return Harmonia::registerSound(binaryPointer, size);
 }
 
 extern "C" _declspec(dllexport)
 int play(unsigned int registeredId)
 {
 return Harmonia::play(registeredId);
 }
 
 extern "C" _declspec(dllexport)
 ogg_int64_t getCurrentTimeInPlayer(unsigned int playerId)
 {
 return Harmonia::getCurrentTimeInPlayer(playerId);
 }
 
 extern "C" _declspec(dllexport)
 void finalize()
 {
 Harmonia::finalize();
 }
 
#elif (__ANDROID__ || ANDROID)

extern "C"
{
    void initialize()
    {
        Harmonia::initialize();
    }

    void initializeForAndroid(int sampleRate, int bufferSize)
    {
        Harmonia::initializeForAndroid(sampleRate, bufferSize);
    }

    unsigned int registerSound(unsigned char* binaryPointer, unsigned int size)
    {
        return Harmonia::registerSound(binaryPointer, size);
    }

    int play(unsigned int registeredId)
    {
        return Harmonia::play(registeredId);
    }

    ogg_int64_t getCurrentTimeInPlayer(unsigned int playerId)
    {
        return Harmonia::getCurrentTimeInPlayer(playerId);
    }

    void harmonia_pause()
    {
        Harmonia::pause();
    }

    void harmonia_resume()
    {
        Harmonia::resume();
    }

    void finalize()
    {
        Harmonia::finalize();
    }
}

#elif __APPLE__

#include "TargetConditionals.h"

#if TARGET_OS_IPHONE || TARGET_OS_MAC

#include <string.h>
#include <stdlib.h>

extern "C" {
    void initialize()
    {
        Harmonia::initialize();
    }
    
    unsigned int registerSound(unsigned char* binaryPointer, unsigned int size)
    {
        return Harmonia::registerSound(binaryPointer, size);
    }
    
    int play(unsigned int registeredId)
    {
        return Harmonia::play(registeredId);
    }
    
    ogg_int64_t getCurrentTimeInPlayer(unsigned int playerId)
    {
        return Harmonia::getCurrentTimeInPlayer(playerId);
    }
    
    void finalize()
    {
        Harmonia::finalize();
    }
    
    const char* aaaaa()
    {
        const char *str = "{\"e\":[{\"t\":1,\"v\":2},{\"t\":3,\"v\":4}]}";
        char* retStr = (char*)malloc(strlen(str) + 1);
        strcpy(retStr, str);
        return retStr;
    }
}



#endif


#endif
*/


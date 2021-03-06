// HarmoniaTestConsoleApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdlib.h>
#include <windows.h>

#include <fstream> 

// General
extern "C" __declspec(dllimport) void harmonia_initialize(unsigned int bufferSize);
extern "C" __declspec(dllimport) void harmonia_finalize();

extern "C" __declspec(dllimport) void harmonia_register_sound(const char* id, unsigned char* binaryData, int size, unsigned int loopStartPoint = 0, unsigned int loopLength = 0);
extern "C" __declspec(dllimport) void harmonia_unregister_sound(const char* id);
extern "C" __declspec(dllimport) void harmonia_pause_all();
extern "C" __declspec(dllimport) void harmonia_resume_all();
extern "C" __declspec(dllimport) void harmonia_stop_all();

extern "C" __declspec(dllimport) float harmonia_get_master_volume();
extern "C" __declspec(dllimport) void harmonia_set_master_volume(float volume);

extern "C" __declspec(dllimport) void harmonia_mute_all();
extern "C" __declspec(dllimport) void harmonia_unmute_all();

extern "C" __declspec(dllimport) void harmonia_start_capture_events();
extern "C" __declspec(dllimport) const char* harmonia_get_capture_events();
extern "C" __declspec(dllimport) void harmonia_stop_capture_events();

extern "C" __declspec(dllimport) void harmonia_start_capture_errors();
extern "C" __declspec(dllimport) const char* harmonia_get_capture_errors();
extern "C" __declspec(dllimport) void harmonia_stop_capture_errors();

// Sound
extern "C" __declspec(dllimport) void harmonia_sound_play(const char* registeredId, const char* soundId = "", const char* targetGroupId = "");
extern "C" __declspec(dllimport) void harmonia_sound_pause(const char* playingDataId);
extern "C" __declspec(dllimport) void harmonia_sound_resume(const char* playingDataId);
extern "C" __declspec(dllimport) void harmonia_sound_stop(const char* playingDataId);
extern "C" __declspec(dllimport) void harmonia_sound_mute(const char* playingDataId);
extern "C" __declspec(dllimport) void harmonia_sound_unmute(const char* playingDataId);
extern "C" __declspec(dllimport) float harmonia_get_sound_volume(const char* playingDataId);
extern "C" __declspec(dllimport) void harmonia_set_sound_volume(const char* playingDataId, float volume);
extern "C" __declspec(dllimport) unsigned int harmonia_get_sound_status(const char* playingDataId);

// Group
extern "C" __declspec(dllimport) void harmonia_create_group(const char* groupId, const char* parentGroupId = "", int maxPolyphony = -1);
extern "C" __declspec(dllimport) void harmonia_delete_group(const char* groupId);
extern "C" __declspec(dllimport) void harmonia_group_pause(const char* groupId);
extern "C" __declspec(dllimport) void harmonia_group_resume(const char* groupId);
extern "C" __declspec(dllimport) void harmonia_group_stop(const char* groupId);
extern "C" __declspec(dllimport) void harmonia_group_mute(const char* groupId);
extern "C" __declspec(dllimport) void harmonia_group_unmute(const char* groupId);
extern "C" __declspec(dllimport) float harmonia_get_group_volume(const char* groupId);
extern "C" __declspec(dllimport) void harmonia_set_group_volume(const char* groupId, float volume);
extern "C" __declspec(dllimport) void harmonia_set_ducker(const char* triggerGroupId, const char* targetGroupId, float ratio);
extern "C" __declspec(dllimport) unsigned int harmonia_get_group_status(const char* targetGroupId);



static const char* idList[3];
static unsigned char* binaryDataList[3];
static int sizeList[3];
static unsigned int loopStartPointList[3];
static unsigned int loopLengthList[3];
int registerCount = 0;

void registerSound(const char* fileName, const char* registerSoundId, unsigned int loopStartPoint = 0, unsigned int loopLength = 0)
{
	std::ifstream infile(fileName, std::ifstream::binary);

	// get size of file
	infile.seekg(0, infile.end);
	long size = infile.tellg();
	infile.seekg(0);

	// allocate memory for file content
	char* buffer = new char[size];

	// read content of infile
	infile.read(buffer, size);
	infile.close();

	// register
	harmonia_register_sound(registerSoundId, (unsigned char*)buffer, size, loopStartPoint, loopLength);

	delete[] buffer;
}

int main()
{
#ifdef _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(218);
#endif

	

	//int* hoge = new int[10];
	//hoge = nullptr;

	// initialize
	harmonia_initialize(2048);
	
	registerSound("se.ogg", "se1");
	registerSound("test.ogg", "bgm1", 269128, 2116816);
	registerSound("test1.ogg", "bgn2");
	
	//harmonia_register_sounds(idList, binaryDataList, sizeList, loopStartPointList, loopLengthList, registerCount);

	harmonia_set_master_volume(0.2);
	// create group
	harmonia_create_group("bgmGroup");
	harmonia_create_group("seGroup");
	//harmonia_create_group("se", "group1", 1);

	// play
	harmonia_sound_play("bgm1", "bgm", "bgmGroup");
	//harmonia_sound_play("hoge1", "moge1");
	//harmonia_sound_play("hoge1", "moge1", "group1");
	//harmonia_sound_play("hoge2", "moge2", "group2");

	harmonia_set_ducker("seGroup", "bgmGroup", 0.5f);

	harmonia_start_capture_events();

	

	while (1)
	{
		
		const char* eventText = harmonia_get_capture_events();
		if (eventText != NULL)
		{
			printf("%s\n", eventText);
			eventText = NULL;
		}
		

		if (GetAsyncKeyState(VK_ESCAPE))
		{
			harmonia_finalize();
			return 0;
		}
		if (GetAsyncKeyState('D'))
		{
			harmonia_delete_group("group1");
			break;
		}
		else if (GetAsyncKeyState(VK_UP))
		{
			//float volume = harmonia_get_master_volume();
			//float volume = harmonia_get_group_volume("group2");
			float volume = harmonia_get_sound_volume("moge1");
			volume += 0.01;
			if (volume > 1)
			{
				volume = 1;
			}
			//harmonia_set_master_volume(volume);
			//harmonia_set_group_volume("group2", volume);
			harmonia_set_sound_volume("moge1", volume);
			Sleep(50);
		}
		else if (GetAsyncKeyState(VK_DOWN))
		{
			//float volume = harmonia_get_master_volume();
			//float volume = harmonia_get_group_volume("group2");
			float volume = harmonia_get_sound_volume("moge1");
			volume -= 0.01;
			if (volume < 0)
			{
				volume = 0;
			}
			//harmonia_set_master_volume(volume);
			//harmonia_set_group_volume("group2", volume);
			harmonia_set_sound_volume("moge1", volume);
			Sleep(100);
		}
		else if (GetAsyncKeyState('A'))
		{
			harmonia_sound_play("se1", "", "seGroup");
			Sleep(100);
		}
		else if (GetAsyncKeyState('P'))
		{
			harmonia_group_pause("bgmGroup");
			Sleep(100);
		}
		else if (GetAsyncKeyState('R'))
		{
			harmonia_group_resume("bgmGroup");
			Sleep(100);
		}
	}

	

	while (1)
	{

	}

	//harmonia_finalize();

    return 0;
}


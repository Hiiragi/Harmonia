/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#pragma once

#include "RegisteredSoundData.h"
#include "SoundGroup.h"
#include "HarmoniaErrorData.h"
#include "SoundEventData.h"
#include <list>
#include <map>
#include <thread>

#if __ANDROID__ || ANDROID
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE || TARGET_OS_MAC
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>
#endif
#endif

class Harmonia
{
public:
	static std::string const MASTER_GROUP_NAME;

	// general
	static void initialize(unsigned int bufferSize);
	static void finalize();

	static void mute();
	static void unmute();

	static void pause();
	static void resume();
	static void stop();

	// sound
	static void register_sound(const char* id, unsigned char* binaryData, unsigned int size, unsigned int loopStartPoint = 0, unsigned int loopLength = 0);
	static void register_sounds(const char** idList, unsigned char** binaryDataList, int* sizeList, unsigned int* loopStartPointList, unsigned int* loopLengthList, unsigned int numRegister);
	static void unregister_sound(const char* id);

	static void play(const char* registeredId, const char* soundId = "", const char* targetGroupId = "");
	static PlayingData* get_playing_data(const char* soundId);

	// group
	static SoundGroup* create_group(const char* groupId, const char* parentGroupId = "", int maxPolyphony = -1);
	static SoundGroup* get_group(const char* groupId);
	static void delete_group(const char* groupId);

	static unsigned int get_buffer_size();

	// events
	static void start_capture_events();
	static std::list<SoundEventData*>* get_captured_events();
	static void stop_capture_events();

	// errors
	static void start_capture_errors();
	static std::list<HarmoniaErrorData*>*  get_capture_errors();
	static void stop_capture_errors();

	// internal
	static void _remove_playing_data(PlayingData* data);
	static void _set_captured_event(std::string registeredSoundId, std::string soundId, SoundEventType type);
	static void _set_captured_error(HarmoniaErrorType type);

    
private:
	static bool _needCaptureEvents;
	static bool _needCaptureErrors;
	static std::list<SoundEventData*>* _capturedEvents;
	static std::list<HarmoniaErrorData*>* _capturedErrors;

	static std::list<PlayingData*>* _queueForCreatePlayingDataList;

	static std::map<std::string, RegisteredSoundData*>* _registeredSoundDictionary;
	static std::map<std::string, SoundGroup*>* _soundGroupDictionary;
	static std::map<std::string, PlayingData*>* _playingDataDictionary;
	static SoundGroup* _masterGroup;
	static unsigned int _bufferSize;
	static float _currentRatio;
	static bool _isInitialized;
	static bool _isFinalizing;
	static bool _isRunningAudioThread;
	static bool _isPause;

	static void initialize_internal();
	static void initialize_playing_data();
	static void normalize_and_write_buffer(int* combinedBuffer, char* buffer, int numSamples);

	static bool exists_registered_binary(const char* &registeredId);
	static bool exists_group(const char* &groupId);
	static bool exists_playing_data(const char* &playingDataId);
	
	static int* combinedBuffer;

#if _WIN32
	static void worker();
    
#elif __ANDROID__ || ANDROID
// engine interfaces
    static SLObjectItf engineObject;
	static SLEngineItf engineEngine;

	// output mix interfaces
	static SLObjectItf outputMixObject;

	// buffer queue player interfaces
	static SLObjectItf bqPlayerObject;
	static SLPlayItf bqPlayerPlay;
	static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;

	static SLmilliHertz bqPlayerSampleRate;
	static jint bqPlayerBufSize;

	static int bufferSwitch;
	static short* buffer1;
	static short* buffer2;

	/*
	static int _androidSampleRate;
	static int _androidBufferSize;
	*/
    static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

#elif __APPLE__
#if TARGET_OS_IPHONE || TARGET_OS_MAC

    static char* _buffer;
    
    static OSStatus renderCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData);
#endif
#endif
};

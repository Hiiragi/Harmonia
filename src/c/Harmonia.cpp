/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#include "Harmonia.h"
#include "PlayingData.h"
#include "SoundGroup.h"

#include <string>
#include <algorithm>
#include <map>
#include <stdio.h>

std::string const Harmonia::MASTER_GROUP_NAME = "master_group";

std::list<PlayingData*>* Harmonia::_queueForCreatePlayingDataList;

SoundGroup* Harmonia::_masterGroup;

std::map<std::string, RegisteredSoundData*>* Harmonia::_registeredSoundDictionary;
std::map<std::string, SoundGroup*>* Harmonia::_soundGroupDictionary;
std::map<std::string, PlayingData*>* Harmonia::_playingDataDictionary;

float Harmonia::_currentRatio;
unsigned int Harmonia::_bufferSize;

bool Harmonia::_isInitialized;
bool Harmonia::_isFinalizing;
bool Harmonia::_isRunningAudioThread;

bool Harmonia::_isPause = false;

bool  Harmonia::_needCaptureEvents;
bool  Harmonia::_needCaptureErrors;
std::list<SoundEventData*>*  Harmonia::_capturedEvents;
std::list<HarmoniaErrorData*>*  Harmonia::_capturedErrors;

int* Harmonia::combinedBuffer = NULL;

void Harmonia::initialize_playing_data()
{
	if (_queueForCreatePlayingDataList->size() > 0)
	{
		std::for_each(_queueForCreatePlayingDataList->begin(), _queueForCreatePlayingDataList->end(), [](PlayingData* playingData)
		{
			playingData->initialize();
		});
		_queueForCreatePlayingDataList->clear();
	}
}

void Harmonia::normalize_and_write_buffer(int* combinedBuffer, char* buffer, int numSamples)
{
	
	// calcurate normalize ratio
	int max = 0;
	int min = 0;
	for (int i = 0; i < numSamples; ++i)
	{
		int sample = combinedBuffer[i];
		if (sample > max)
		{
			max = sample;
		}
		else if (sample < min)
		{
			min = sample;
		}
	}

	float ratioByMax = max / 32767.0f;
	float ratioByMin = -(min / -32767.0f);

	float ratio = std::max(ratioByMax, ratioByMin);

	if (ratio > _currentRatio)
	{
		_currentRatio = ratio;
	}
	else
	{
		if (_currentRatio < 1)
		{
			_currentRatio += 0.001f;
		}


	}

	if (_currentRatio > 1)
	{
		_currentRatio = 1;
	}


	union shortConverter {
		char c[2];
		short value;
	} converter;

	for (int i = 0; i < numSamples; ++i)
	{
		short sample = (short)(combinedBuffer[i] / _currentRatio);
		converter.value = sample;
		buffer[i * 2] = converter.c[0];
		buffer[i * 2 + 1] = converter.c[1];
	}
	
}

#if _WIN32

#include <windows.h>
#include <tchar.h>
#include <MMSystem.h>
#include <dsound.h>
#include <thread>
#include <iostream>

#pragma comment ( lib, "libogg_static.lib" )
#pragma comment ( lib, "libvorbis_static.lib" )
#pragma comment ( lib, "libvorbisfile_static.lib" )
#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "dsound.lib" )

void Harmonia::worker()
{
	_isRunningAudioThread = true;

	// DirectSound�̍쐬
	IDirectSound8 *pDS8;
	DirectSoundCreate8(NULL, &pDS8, NULL);
	pDS8->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY);

	// �Z�J���_���o�b�t�@�쐬���
	double playTime = 0.1; // 1�b��

	// WAVE���
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = 2;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nBlockAlign = 2 * 16 / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// DirectSoundBuffer���
	DSBUFFERDESC DSBufferDesc;
	DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	DSBufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS;
	DSBufferDesc.dwBufferBytes = (DWORD)(waveFormat.nAvgBytesPerSec * playTime);
	DSBufferDesc.dwReserved = 0;
	DSBufferDesc.lpwfxFormat = &waveFormat;
	DSBufferDesc.guid3DAlgorithm = GUID_NULL;

	// �Z�J���_���o�b�t�@�쐬
	IDirectSoundBuffer *ptmpBuf = 0;
	IDirectSoundBuffer8 *pDSBuffer = 0;
	if (SUCCEEDED(pDS8->CreateSoundBuffer(&DSBufferDesc, &ptmpBuf, NULL)))
	{
		ptmpBuf->QueryInterface(IID_IDirectSoundBuffer8, (void**)&pDSBuffer);
	}
	else
	{
		pDS8->Release();
		return;
	}
	ptmpBuf->Release();

	// �o�b�t�@�����b�N���ď����f�[�^��������

	void* AP1 = 0, *AP2 = 0;
	DWORD AB1 = 0, AB2 = 0;
	/*
	if (SUCCEEDED(pDSBuffer->Lock(0, 0, &AP1, &AB1, &AP2, &AB2, DSBLOCK_ENTIREBUFFER))) {
	pDSBuffer->Unlock(AP1, AB1, AP2, AB2);
	}
	else {
	pDSBuffer->Release();
	pDS8->Release();
	return;
	}
	*/
	// �Đ��J�n
	pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);

	// �X�g���[���Đ�
	unsigned int size = DSBufferDesc.dwBufferBytes / 2;
	unsigned int flag = 0;
	DWORD point = 0;

	bool isPausing = false;

	while (1)
	{
        if (_isFinalizing)
        {
            break;
        }
		else if (_isPause && !isPausing)
		{
			pDSBuffer->Stop();
			isPausing = true;
			continue;
		}
		else if (!_isPause && isPausing)
		{
			pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
			isPausing = false;
			continue;
		}

		//time_t now = time(NULL);
		//std::cout << now << " seconds since the Epoch\n";

		pDSBuffer->GetCurrentPosition(&point, 0);

		if (flag == 0 && point >= size) {
			// �O���ɏ�������
			if (SUCCEEDED(pDSBuffer->Lock(0, size, &AP1, &AB1, &AP2, &AB2, 0))) {
				
				if (combinedBuffer == NULL)
				{
					combinedBuffer = new int[AB1 / 2];
				}

				memset(combinedBuffer, 0, AB1 * 2);

				// 書き込み処理
				initialize_playing_data();
				_masterGroup->write_raw_pcm(combinedBuffer, AB1 / 2);
				normalize_and_write_buffer(combinedBuffer, (char*)AP1, AB1 / 2);
				pDSBuffer->Unlock(AP1, AB1, AP2, AB2);
				flag = 1;
			}
		}
		else if (flag == 1 && point < size) {
			// �㔼�ɏ�������
			if (SUCCEEDED(pDSBuffer->Lock(size, size * 2, &AP1, &AB1, &AP2, &AB2, 0))) {
				/*
				if (combinedBuffer == NULL)
				{
					combinedBuffer = new int[AB1 / 2];
				}
				*/
				memset(combinedBuffer, 0, AB1 * 2);

				initialize_playing_data();
				_masterGroup->write_raw_pcm(combinedBuffer, AB1 / 2);
				normalize_and_write_buffer(combinedBuffer, (char*)AP1, AB1 / 2);
				pDSBuffer->Unlock(AP1, AB1, AP2, AB2);
				flag = 0;
			}
		}
	}

	pDSBuffer->Release();
	pDS8->Release();

	delete combinedBuffer;

	_isRunningAudioThread = false;
}

void Harmonia::initialize_internal()
{
	// thread
	std::thread thread(worker);
	thread.detach();
}

#elif defined(__ANDROID__) || defined(ANDROID)

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <assert.h>
#include <android/log.h>
#include <sstream>

// engine interfaces
SLObjectItf Harmonia::engineObject;
SLEngineItf Harmonia::engineEngine;

// output mix interfaces
SLObjectItf Harmonia::outputMixObject = NULL;

// buffer queue player interfaces
SLObjectItf Harmonia::bqPlayerObject = NULL;
SLPlayItf Harmonia::bqPlayerPlay;
SLAndroidSimpleBufferQueueItf Harmonia::bqPlayerBufferQueue;

SLmilliHertz Harmonia::bqPlayerSampleRate = 0;
jint Harmonia::bqPlayerBufSize = 0;

int Harmonia::bufferSwitch;
short* Harmonia::buffer1;
short* Harmonia::buffer2;
/*
int _androidSampleRate;
int _androidBufferSize;
*/
void Harmonia::bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{

    assert(bq == bqPlayerBufferQueue);
    assert(NULL == context);
    short *bufferPointer;

    if (bufferSwitch == 0)
    {
        //__android_log_print(ANDROID_LOG_DEBUG, "harmonia", "switch to buffer1");
        bufferPointer = buffer1;
        bufferSwitch = 1;
    }
    else
    {
        //__android_log_print(ANDROID_LOG_DEBUG, "harmonia", "switch to buffer2");
        bufferPointer = buffer2;
        bufferSwitch = 0;
    }

    for (int i = 0; i < _bufferSize; i++)
    {
        bufferPointer[i] = 0;
        combinedBuffer[i] = 0;
    }

    initialize_playing_data();
    _masterGroup->write_raw_pcm(combinedBuffer, _bufferSize / 2);
    normalize_and_write_buffer(combinedBuffer, (char *)bufferPointer, _bufferSize / 2);

    SLresult result;
    result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, bufferPointer, _bufferSize);
    (void)result;
}

void Harmonia::initialize_internal()
{
    SLresult result;

    SLEngineOption engineOption[] = {
            SL_ENGINEOPTION_THREADSAFE, SL_BOOLEAN_TRUE,
            SL_ENGINEOPTION_LOSSOFCONTROL, SL_BOOLEAN_TRUE
    };

    // create engine
    result = slCreateEngine(&engineObject, 2, engineOption, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // create output mix
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;



	/*
    if (_androidSampleRate >= 0 && _androidBufferSize >= 0 ) {
        bqPlayerSampleRate = _androidSampleRate * 1000;
         //device native buffer size is another factor to minimize audio latency, not used in this
         //sample: we only play one giant buffer here
        bqPlayerBufSize = _androidBufferSize;
    }
	*/
	//bqPlayerSampleRate = 44100;

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 4};
    SLDataFormat_PCM format_pcm;
    format_pcm.formatType = SL_DATAFORMAT_PCM;
    format_pcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
    format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    format_pcm.containerSize = 16;
//    format_pcm.numChannels = 1;
//    format_pcm.channelMask = SL_SPEAKER_FRONT_CENTER;
    format_pcm.numChannels = 2;
    format_pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

    /*
    * Enable Fast Audio when possible:  once we set the same rate to be the native, fast audio path
    * will be triggered
    */
    if(bqPlayerSampleRate) {
        //format_pcm.samplesPerSec = bqPlayerSampleRate * 1000;       //sample rate in mili second
    }
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    /*
     * create audio player:
     *     fast audio does not support when SL_IID_EFFECTSEND is required, skip it
     *     for fast audio case
     */
    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk, 1, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                             &bqPlayerBufferQueue);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;


    bufferSwitch = 0;
    buffer1 = new short[_bufferSize];
    buffer2 = new short[_bufferSize];

    combinedBuffer = new int[_bufferSize];

    //short buffer[bqPlayerBufSize];
    //short *bufferPointer = buffer;

    for (int i = 0; i < _bufferSize; i++)
    {
        buffer2[i] = 0;
    }

    (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, buffer2, _bufferSize);
}

#elif __APPLE__

#include "TargetConditionals.h"

#if TARGET_OS_IPHONE || TARGET_OS_MAC

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>


char* Harmonia::_buffer;

OSStatus Harmonia::renderCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{
    short *outL = (short*)ioData->mBuffers[0].mData;
    short *outR = (short*)ioData->mBuffers[1].mData;
    
    if (!_isFinalizing)
    {
        int numSamples = inNumberFrames * 2;
        if (_buffer == NULL)
        {
            _bufferSize = numSamples * sizeof(short);
            _buffer = new char[numSamples];
        }
        
        if (combinedBuffer == NULL)
        {
            combinedBuffer = new int[numSamples];
        }
        
        char* buffer = _buffer;
        
        memset(buffer, 0, _bufferSize);
        memset(combinedBuffer, 0, numSamples * 4);
        
        initialize_playing_data();
        _masterGroup->write_raw_pcm(combinedBuffer, numSamples);
        normalize_and_write_buffer(combinedBuffer, (char *)buffer, _bufferSize);
        
        size_t byteSize = sizeof(short);
        UInt32 current = 0;
        
        for (int i = 0; i < inNumberFrames; i++)
        {
            memcpy(outL + current, buffer, byteSize);
            buffer += byteSize;
            
            memcpy(outR + current, buffer, byteSize);
            buffer += byteSize;
            current++;
        }
        
        //buffer -= _bufferSize;
        //delete[] buffer;
    }
    else
    {
        memset(outL, 0, _bufferSize / 2);
        memset(outR, 0, _bufferSize / 2);
        
        delete[] _buffer;
        
        _isRunningAudioThread = false;
    }
    
    return noErr;
}


AudioUnit audioUnit;

void Harmonia::initialize_internal()
{
    printf("initialize_internal for mac.\n");
    
    //AudioUnit audioUnit;
    
    AudioComponentDescription cd;
    cd.componentType = kAudioUnitType_Output;
#if TARGET_OS_IPHONE
    cd.componentSubType = kAudioUnitSubType_RemoteIO;
#elif TARGET_OS_MAC
    cd.componentSubType = kAudioUnitSubType_DefaultOutput;
#endif
    cd.componentManufacturer = kAudioUnitManufacturer_Apple;
    cd.componentFlags = 0;
    cd.componentFlagsMask = 0;
    
    AudioComponent component = NULL;
    component = AudioComponentFindNext(NULL, &cd);
    AudioComponentInstanceNew(component, &audioUnit);
    
    AudioUnitInitialize(audioUnit);
    
    
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc = renderCallback;
    //callbackStruct.inputProcRefCon = self;
    
    AudioUnitSetProperty(audioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callbackStruct, sizeof(AURenderCallbackStruct));
    
    float sampleRate = 44100.0;
    
    AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate = sampleRate;
    audioFormat.mFormatID = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger
    | kAudioFormatFlagsNativeEndian
    | kAudioFormatFlagIsPacked
    | kAudioFormatFlagIsNonInterleaved;
    
    audioFormat.mChannelsPerFrame = 2;
    audioFormat.mBytesPerPacket = sizeof(short);
    audioFormat.mBytesPerFrame = sizeof(short);
    audioFormat.mFramesPerPacket = 1;
    audioFormat.mBitsPerChannel = 8 * sizeof(short);
    audioFormat.mReserved = 0;
    
    AudioUnitSetProperty(audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &audioFormat, sizeof(audioFormat));
    
    
    AudioOutputUnitStart(audioUnit);
}

#endif

#endif


void Harmonia::initialize(unsigned int bufferSize)
{
	if (_isInitialized)
	{
		return;
	}

	_isInitialized = true;
	_isFinalizing = false;
	_bufferSize = bufferSize;

	_queueForCreatePlayingDataList = new std::list<PlayingData*>();
	_registeredSoundDictionary = new std::map<std::string, RegisteredSoundData*>();

	_masterGroup = new SoundGroup(Harmonia::MASTER_GROUP_NAME);
	_soundGroupDictionary = new std::map<std::string, SoundGroup*>();
	_playingDataDictionary = new std::map<std::string, PlayingData*>();

	_currentRatio = 1.0f;

	_isRunningAudioThread = false;

	_isPause = false;

	_needCaptureEvents = false;
	_needCaptureErrors = false;
	_capturedEvents = new std::list<SoundEventData*>();
	_capturedErrors = new std::list<HarmoniaErrorData*>();

	initialize_internal();
}

void Harmonia::pause()
{
	if (_isPause)
	{
		return;
	}

	_isPause = true;

#if defined(__ANDROID__) || defined(ANDROID)
	SLresult result;
	result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PAUSED);
	assert(SL_RESULT_SUCCESS == result);
#endif

}

void Harmonia::resume()
{
	if (!_isPause)
	{
		return;
	}

	_isPause = false;

#if defined(__ANDROID__) || defined(ANDROID)
		SLresult result;

		// set the player's state to playing
		result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
#endif

}

void Harmonia::mute()
{
	_masterGroup->mute();
}

void Harmonia::unmute()
{
	_masterGroup->unmute();
}

void Harmonia::stop()
{
	_masterGroup->stop();
}



void Harmonia::finalize()
{
	if (!_isInitialized)
	{
		return;
	}

	_isFinalizing = true;

#ifdef __EMSCRIPTEN__
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

#elif _WIN32
	// �X���b�h�I���҂�
	while (1)
	{
		if (!_isRunningAudioThread)
		{
			break;
		}
	}
#elif defined(__ANDROID__) || defined(ANDROID)

	(*engineObject)->Destroy(engineObject);
	engineObject = NULL;
	(*outputMixObject)->Destroy(outputMixObject);
	outputMixObject = NULL;
	(*bqPlayerObject)->Destroy(bqPlayerObject);
	bqPlayerObject = NULL;

	delete[] buffer1;
	delete[] buffer2;

#elif __APPLE__
#if TARGET_OS_IPHONE || TARGET_OS_MAC
	while (1)
	{
		if (!_isRunningAudioThread)
		{
			break;
		}
	}

	AudioOutputUnitStop(audioUnit);
	AudioUnitReset(audioUnit, kAudioUnitScope_Global, 0);
#endif
#endif


	// finalize variables
	_isFinalizing = false;
	_isInitialized = false;

	delete _masterGroup;

	for (std::map<std::string, RegisteredSoundData*>::iterator i = _registeredSoundDictionary->begin(); i != _registeredSoundDictionary->end(); i++) {
		delete i->second;
	}
	_registeredSoundDictionary->clear();
	delete _registeredSoundDictionary;

	/*
	for (std::map<std::string, SoundGroup*>::iterator i = _soundGroupDictionary->begin(); i != _soundGroupDictionary->end(); i++) {
		delete i->second;
	}
	*/
	_soundGroupDictionary->clear();
	delete _soundGroupDictionary;
	/*
	for (std::map<std::string, PlayingData*>::iterator i = _playingDataDictionary->begin(); i != _playingDataDictionary->end(); i++) {
		delete i->second;
	}
	*/
	_playingDataDictionary->clear();
	delete _playingDataDictionary;

	_queueForCreatePlayingDataList->clear();
	delete _queueForCreatePlayingDataList;

	_capturedEvents->clear();
	delete _capturedEvents;

	_capturedErrors->clear();
	delete _capturedErrors;

	_currentRatio = 1.0f;
	_bufferSize = 0;

	_needCaptureEvents = false;
	_needCaptureErrors = false;
}

char* hoge;


void Harmonia::register_sound(const char* id, unsigned char* binaryData, unsigned int size, unsigned int loopStartPoint, unsigned int loopLength)
{
	bool exists = exists_registered_binary(id);
	if (exists)
	{
		// 登録済によるエラー
		_set_captured_error(HarmoniaErrorType::ALREADY_REGISTERED);
		return;
	}

	RegisteredSoundData* registeredSoundData = new RegisteredSoundData(id, binaryData, size, loopStartPoint, loopLength);
	_registeredSoundDictionary->insert(std::make_pair(registeredSoundData->get_id(), registeredSoundData));

	Harmonia::_set_captured_event(id, "", SoundEventType::REGISTER_SOUND_COMPLETE);
}

void Harmonia::register_sounds(const char** idList, unsigned char** binaryDataList, int* sizeList, unsigned int* loopStartPointList, unsigned int* loopLengthList, unsigned int numRegister)
{
	for (int i = 0; i < numRegister; ++i)
	{
		Harmonia::register_sound(idList[i], binaryDataList[i], sizeList[i], loopStartPointList[i], loopLengthList[i]);
	}
}

void Harmonia::unregister_sound(const char* id)
{
	bool exists = exists_registered_binary(id);
	if (!exists)
	{
		// 未登録によるエラー
		_set_captured_error(HarmoniaErrorType::UNREGISTERED_SOUND);
		return;
	}
	
	// 鳴っている音を停止
	for (auto itr = _playingDataDictionary->begin(); itr != _playingDataDictionary->end(); ++itr) {
		PlayingData* data = itr->second;
		if (data->_get_registered_sound_id() == id)
		{
			data->stop();
		}
	}
	

	// 登録削除
	RegisteredSoundData* data = _registeredSoundDictionary->at(id);

	delete data;

	_registeredSoundDictionary->erase(id);

}

void Harmonia::play(const char* registeredSoundId, const char* soundId, const char* targetGroupId)
{
	bool isRegistered = exists_registered_binary(registeredSoundId);

	if (isRegistered)
	{
		bool groupIsExist = exists_group(targetGroupId);
		if (groupIsExist)
		{
			RegisteredSoundData* registeredSoundData = _registeredSoundDictionary->at(registeredSoundId);

			PlayingData* playingData = new PlayingData(registeredSoundData, soundId);

			if (strcmp(targetGroupId, "") == 0)
			{
				_masterGroup->add_sound(playingData);
			}
			else
			{
				SoundGroup* soundGroup = _soundGroupDictionary->at(targetGroupId);
				soundGroup->add_sound(playingData);
			}

			_queueForCreatePlayingDataList->push_back(playingData);

			if (strcmp(soundId, "") != 0)
			{
				_playingDataDictionary->insert(std::make_pair(soundId, playingData));
			}

		}
		else
		{
			// グループ未作成エラー
			_set_captured_error(HarmoniaErrorType::UNCREATED_GROUP);
		}
	}
	else
	{
		// バイナリ未登録エラー
		_set_captured_error(HarmoniaErrorType::UNREGISTERED_SOUND);
	}
}


PlayingData* Harmonia::get_playing_data(const char * soundId)
{
	bool exists = exists_playing_data(soundId);
	if (!exists)
	{
		// 再生中のデータがない
		_set_captured_error(HarmoniaErrorType::NOT_PLAYING);
		return NULL;
	}

	return _playingDataDictionary->at(soundId);
}



SoundGroup* Harmonia::create_group(const char* groupId, const char* parentGroupId, int maxPolyphony)
{
	bool exists = exists_group(groupId);
	if (exists)
	{
		// 既存によるエラー
		_set_captured_error(HarmoniaErrorType::ALREADY_CREATED_GROUP);
		return NULL;
	}

	SoundGroup* soundGroup = new SoundGroup(groupId, maxPolyphony);

	if (strcmp(parentGroupId, "") == 0)
	{
		_masterGroup->add_group(soundGroup);
	}
	else
	{
		SoundGroup* parentGroup = _soundGroupDictionary->at(parentGroupId);
		parentGroup->add_group(soundGroup);
	}

	_soundGroupDictionary->insert(std::make_pair(groupId, soundGroup));

	return soundGroup;
}

SoundGroup* Harmonia::get_group(const char* groupId)
{
	if (groupId == MASTER_GROUP_NAME)
	{
		return _masterGroup;
	}

	bool exists = exists_group(groupId);
	if (exists)
	{
		SoundGroup* soundGroup = _soundGroupDictionary->at(groupId);
		return soundGroup;
	}

	return NULL;
}

void Harmonia::delete_group(const char* groupId)
{
	if (groupId == MASTER_GROUP_NAME)
	{
		return;
	}

	bool exists = exists_group(groupId);
	if (exists)
	{
		SoundGroup* targetGroup = get_group(groupId);
		SoundGroup* parentGroup = get_group(targetGroup->get_parent().c_str());
		parentGroup->remove_group(targetGroup);
	}
	else
	{
		// 未登録によるエラー
		_set_captured_error(HarmoniaErrorType::UNCREATED_GROUP);
	}
}




unsigned int Harmonia::get_buffer_size()
{
	return _bufferSize;
}


void Harmonia::start_capture_events()
{
	if (!_needCaptureEvents)
	{
		_needCaptureEvents = true;
	}
}

std::list<SoundEventData*>* Harmonia::get_captured_events()
{
	if (_needCaptureEvents)
	{
		
		if (_capturedEvents->size() > 0)
		{
			std::list<SoundEventData*>* clonedList = new std::list<SoundEventData*>();

			std::for_each(_capturedEvents->cbegin(), _capturedEvents->cend(), [clonedList](SoundEventData* data) {
				SoundEventData* clonedData = new SoundEventData(data->get_rid(), data->get_sid(), data->get_type());
				clonedList->push_back(clonedData);
				delete data;
			});

			_capturedEvents->clear();

			return clonedList;
		}
	}

	return NULL;
}

void Harmonia::stop_capture_events()
{
	if (_needCaptureEvents)
	{
		_needCaptureEvents = false;
		_capturedEvents->clear();
	}
}


void Harmonia::start_capture_errors()
{
	if (!_needCaptureErrors)
	{
		_needCaptureErrors = true;
	}
}

std::list<HarmoniaErrorData*>*  Harmonia::get_capture_errors()
{
	std::list<HarmoniaErrorData*>* clonedList = new std::list<HarmoniaErrorData*>();

	if (_needCaptureErrors)
	{
		if (_capturedErrors->size() > 0)
		{
			std::for_each(_capturedErrors->cbegin(), _capturedErrors->cend(), [clonedList](HarmoniaErrorData* data) {
				HarmoniaErrorData* clonedData = new HarmoniaErrorData(data->get_error_type());
				clonedList->push_back(clonedData);
				delete data;
			});
		}

		_capturedErrors->clear();
	}

	return clonedList;
}

void Harmonia::stop_capture_errors()
{
	if (_needCaptureErrors)
	{
		_needCaptureErrors = false;
		_capturedErrors->clear();
	}
}



void Harmonia::_set_captured_event(std::string registeredSoundId, std::string soundId, SoundEventType type)
{
	if (_needCaptureEvents)
	{
		_capturedEvents->push_back(new SoundEventData(registeredSoundId, soundId, type));
	}
}

void Harmonia::_set_captured_error(HarmoniaErrorType type)
{
	if (_needCaptureErrors)
	{
		_capturedErrors->push_back(new HarmoniaErrorData(type));
	}
	else
	{
		std::string typeStr;
#if ANDROID || _ANDROID_
		std::stringstream stream;
		stream << "" << type;
		typeStr = stream.str();
#else
		typeStr = std::to_string(type);
#endif
        throw new std::runtime_error("Harmonia Error : " + typeStr);
	}
}

void Harmonia::_remove_playing_data(PlayingData* data)
{
	const char* id = data->get_id().c_str();
	bool exists = exists_playing_data(id);
	if (exists)
	{
		_playingDataDictionary->erase(id);
	}
	
}


bool Harmonia::exists_registered_binary(const char* &registeredId)
{
	if (_registeredSoundDictionary->count(registeredId) == 0)
	{
		return false;
	}

	return true;
}

bool Harmonia::exists_group(const char* &groupId)
{
	if (strcmp(groupId, "") == 0)
	{
		return true;
	}
	else if (_soundGroupDictionary->count(groupId) == 0)
	{
		return false;
	}

	return true;
}

bool Harmonia::exists_playing_data(const char* &playingDataId)
{
	if (_playingDataDictionary->count(playingDataId) == 0)
	{
		return false;
	}

	return true;
}



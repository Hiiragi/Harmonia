/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#pragma once

extern "C" {
	// General
	void harmonia_initialize(unsigned int bufferSize);
	void harmonia_finalize();

	void harmonia_register_sound(const char* id, unsigned char* binaryData, int size, unsigned int loopStartPoint = 0, unsigned int loopLength = 0);
	void harmonia_unregister_sound(const char* id);
	void harmonia_pause_all();
	void harmonia_resume_all();
	void harmonia_stop_all();

	float harmonia_get_master_volume();
	void harmonia_set_master_volume(float volume);

	void harmonia_mute_all();
	void harmonia_unmute_all();

	void harmonia_start_capture_events();
	char* harmonia_get_capture_events();
	char* harmonia_get_capture_events_with_size(int* size);
	void harmonia_stop_capture_events();

	void harmonia_start_capture_errors();
	char* harmonia_get_capture_errors();
	char* harmonia_get_capture_errors_with_size(int* size);
	void harmonia_stop_capture_errors();

	// Sound
	void harmonia_sound_play(const char* registeredId, const char* soundId = "", const char* targetGruopId = "");
	void harmonia_sound_pause(const char* playingDataId);
	void harmonia_sound_resume(const char* playingDataId);
	void harmonia_sound_stop(const char* playingDataId);
	void harmonia_sound_mute(const char* playingDataId);
	void harmonia_sound_unmute(const char* playingDataId);
	float harmonia_get_sound_volume(const char* playingDataId);
	void harmonia_set_sound_volume(const char* playingDataId, float volume);
	unsigned int harmonia_get_sound_status(const char* playingDataId);
	unsigned int harmonia_get_sound_current_position(const char* playingDataId);

	// Group
	void harmonia_create_group(const char* groupId, const char* parentGroupId = "", int maxPolyphony = -1);
	void harmonia_delete_group(const char* groupId);
	void harmonia_group_pause(const char* groupId);
	void harmonia_group_resume(const char* groupId);
	void harmonia_group_stop(const char* groupId);
	void harmonia_group_mute(const char* groupId);
	void harmonia_group_unmute(const char* groupId);
	float harmonia_get_group_volume(const char* groupId);
	void harmonia_set_group_volume(const char* groupId, float volume);
	void harmonia_set_ducker(const char* triggerGroupId, const char* targetGroupId, float ratio, float attackTimeByMS = 0, float releaseTimeByMS = 0);
	unsigned int harmonia_get_group_status(const char* groupId);
}

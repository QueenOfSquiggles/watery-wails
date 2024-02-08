#pragma once
#include <audio_file/AudioFile.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <filesystem>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include "engine/safety/safety.hpp"

std::string get_al_error_string();

class AudioData
{
	friend class AudioSource;

	bool data_loaded;
	unsigned int al_buffer = 0;

public:
	std::filesystem::path file_path;
	void load_audio_data();
	AudioData(std::filesystem::path file);
	~AudioData();
};

class AudioSource
{
	friend class AudioSystem;
	std::shared_ptr<AudioData> audio;
	unsigned int source_buffer;

	bool looping;
	bool is_playing;

public:
	AudioSource(std::shared_ptr<AudioData> audio);
	~AudioSource();
	void play();
	void pause();
	void stop();
	bool get_is_playing();

	void set_loop(bool is_looping);
};

class AudioSystem
{
	ALCdevice *device;
	std::vector<std::weak_ptr<AudioSource>> streams;

public:
	AudioSystem();
	~AudioSystem();
	void queue_audio(std::shared_ptr<AudioSource> &audio);
	void tick_streams();

	void pause_all();
	void resume_all();
	void set_gain(float value);
};
#pragma once
#include <stb_vorbis.h>
#include <AL/al.h>
#include <filesystem>
#include <memory>
#include <vector>
#include <iostream>

class AudioFile
{
	bool streamed;
	bool looping;
	bool is_playing;
	std::filesystem::path audio_file;
	unsigned int current_index = 0;
	unsigned int al_buffer = 0;

	void load_audio_data();

public:
	AudioFile(std::filesystem::path file);
	~AudioFile();
	void set_loop(bool is_looping);
	void set_streamed(bool is_streaming);

	void play();
	void pause();
	void stop();
	bool get_is_playing();
};

class AudioSystem
{
	std::vector<std::shared_ptr<AudioFile>> streams;

public:
	static std::shared_ptr<AudioSystem> instance;
	void queue_audio(std::shared_ptr<AudioFile> audio);
	void tick_streams();
};
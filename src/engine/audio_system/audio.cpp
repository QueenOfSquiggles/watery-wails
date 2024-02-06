#include "audio.hpp"

AudioFile::AudioFile(std::filesystem::path file) : audio_file(file)
{
	// do nothing in case we want to stream the file instead
}

AudioFile::~AudioFile()
{
	alDeleteBuffers(1, &al_buffer);
}
void AudioFile::load_audio_data()
{
	// stb_vorbis_alloc alloc;
	// int err;
	// auto stbfile = stb_vorbis_open_filename(audio_file.c_str(), &err, &alloc);
	// auto info = stb_vorbis_get_info(stbfile);
	short *data;
	int channels, sample_rate, len;
	len = stb_vorbis_decode_filename(audio_file.c_str(), &channels, &sample_rate, &data);
	if (len)
	{
		std::cout << "Loaded vorbis data!" << std::endl
				  << "\tChannels=" << channels << std::endl
				  << "\tSample Rate=" << sample_rate << "Hz" << std::endl
				  << "\tLength=" << len << std::endl;
	}
}

void AudioFile::set_loop(bool is_looping) { looping = is_looping; }
void AudioFile::set_streamed(bool is_streaming) { streamed = is_streaming; }

void AudioFile::play()
{
	if (!al_buffer)
	{
		load_audio_data();
	}
	is_playing = true;
}

void AudioFile::pause()
{
	is_playing = false;
}

void AudioFile::stop()
{
	pause();
	current_index = 0;
}
bool AudioFile::get_is_playing() { return is_playing; }

std::shared_ptr<AudioSystem> AudioSystem::instance = std::shared_ptr<AudioSystem>(new AudioSystem());

void AudioSystem::queue_audio(std::shared_ptr<AudioFile> audio)
{
	streams.push_back(audio);
}
void AudioSystem::tick_streams() {}

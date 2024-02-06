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
	alGenBuffers(1, &al_buffer);
	// alBufferData(al_buffer, AL_FORMA)
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

//
//
//

AudioSource::AudioSource(std::shared_ptr<AudioFile> audio) : audio(audio)
{
	alGenSources(1, &source_buffer);
	alSourcei(source_buffer, AL_BUFFER, audio->al_buffer);
}

AudioSource::~AudioSource()
{
}

//
//
//

std::shared_ptr<AudioSystem> AudioSystem::instance = std::shared_ptr<AudioSystem>(new AudioSystem());

AudioSystem::AudioSystem()
{
	this->device = std::shared_ptr<ALCdevice>(alcOpenDevice(NULL));
	if (device)
	{
		auto ctx = alcCreateContext(device.get(), NULL);
		alcMakeContextCurrent(ctx);
	}
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	float orientation[] = {0, 0, 0, 0, 1, 0};
	alListenerfv(AL_ORIENTATION, orientation);
	// alListenerf(AL_GAIN, 1);
}

AudioSystem::~AudioSystem()
{
	auto ctx = alcGetCurrentContext();
	auto device = alcGetContextsDevice(ctx);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(device);
}
void AudioSystem::queue_audio(std::shared_ptr<AudioFile> audio)
{
	streams.push_back(audio);
}
void AudioSystem::tick_streams() {}

void AudioSystem::set_gain(float value)
{
	alListenerf(AL_GAIN, value);
}

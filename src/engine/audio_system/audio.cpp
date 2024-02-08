#include "audio.hpp"
// #include <chrono>

std::string get_al_error_string()
{
	int err = alGetError();
	switch (err)
	{
	case AL_NO_ERROR:
		return "";
	case AL_INVALID_NAME:
		return "Invalid name!";
	case AL_INVALID_ENUM:
		return "Invalid enum!";
	case AL_INVALID_OPERATION:
		return "Invalid operation!";
	case AL_INVALID_VALUE:
		return "Invalid value!";
	case AL_OUT_OF_MEMORY:
		return "Out of memory!";
	default:
		return "Unexpected error type!";
	}
}

AudioData::AudioData(std::filesystem::path file) : data_loaded(false), file_path(file)
{
	// do nothing in case we want to stream the file instead
}

AudioData::~AudioData()
{
	alDeleteBuffers(1, &al_buffer);
}

constexpr unsigned int PCM_DATA_SIZE = 4096;
char pcm_data[PCM_DATA_SIZE]; // more efficient than using heap, so says libvorbis

void AudioData::load_audio_data()
{
	if (data_loaded)
	{
		return;
	}
	AudioFile<int> a; // this is a good start, but it seems there's a 0% chance of streaming audio data
	a.shouldLogErrorsToConsole(true);
	if (!a.load(file_path.string()))
	{

		std::cerr << "Failed to load audio file data : " << file_path << std::endl;
		return;
	}
	a.setBitDepth(16);

	int buffer[a.samples.size() * a.samples[0].size()];
	unsigned int index = 0;
	for (auto frame : a.samples)
	{
		for (auto channel : frame)
		{
			buffer[index++] = channel;
		}
	}
	auto format = a.isStereo() ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	alGenBuffers(1, &al_buffer);
	alBufferData(al_buffer, format, buffer, sizeof(buffer), a.getSampleRate());
	auto al_err = get_al_error_string();
	if (!al_err.empty())
	{
		std::cerr << "AL Error: " << al_err << std::endl;
	}
	data_loaded = true;
}

void AudioSource::set_loop(bool is_looping) { looping = is_looping; }

void AudioSource::play()
{
	alSourcePlay(source_buffer);
	// std::cout << "Playing audio file" << std::endl;
}

void AudioSource::pause()
{
	alSourcePause(source_buffer);
}

void AudioSource::stop()
{
	pause();
	alSourceStop(source_buffer);
}
bool AudioSource::get_is_playing() { return is_playing; }

//
//
//

AudioSource::AudioSource(std::shared_ptr<AudioData> audio) : audio(audio)
{
	is_playing = false;
	alGenSources(1, &source_buffer);
	audio->load_audio_data();
	alSourcei(source_buffer, AL_BUFFER, audio->al_buffer);
}

AudioSource::~AudioSource()
{
}

//
//
//

AudioSystem::AudioSystem()
{
	device = alcOpenDevice(NULL);
	if (device)
	{
		auto ctx = alcCreateContext(device, NULL);
		alcMakeContextCurrent(ctx);
	}
	else
	{
		std::cerr << "Failed to load audio device" << std::endl;
		auto err = get_al_error_string();
		if (!err.empty())
		{
			std::cerr << "Error: " << err << std::endl;
		}
	}
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	float orientation[] = {0, 0, 0, 0, 1, 0};
	alListenerfv(AL_ORIENTATION, orientation);

	auto err = get_al_error_string();
	if (!err.empty())
	{
		std::cerr << "Error: " << err << std::endl;
	}
}

AudioSystem::~AudioSystem()
{
	auto ctx = alcGetCurrentContext();
	auto device = alcGetContextsDevice(ctx);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(device);
}
void AudioSystem::queue_audio(std::shared_ptr<AudioSource> &audio)
{
	streams.push_back(std::weak_ptr<AudioSource>(audio));
}
void AudioSystem::tick_streams()
{

	for (auto s : streams)
	{
		auto stream = s.lock();
		if (!stream)
			continue;

		int state;
		alGetSourcei(stream->source_buffer, AL_SOURCE_STATE, &state);
		stream->is_playing = (state == AL_PLAYING);
	}
}

void AudioSystem::set_gain(float value)
{
	alListenerf(AL_GAIN, value);
}
void AudioSystem::pause_all()
{
	for (auto s : streams)
	{
		if (auto sfx = s.lock())
		{
			sfx->pause();
		}
	}
}

void AudioSystem::resume_all()
{
	for (auto s : streams)
	{
		if (auto sfx = s.lock())
		{
			sfx->play();
		}
	}
}

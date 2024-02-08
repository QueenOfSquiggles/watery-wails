#include "game/opening_scene.hpp"
#include "engine/audio_system/audio.hpp"
int main()
{
	// creates engine context
	Engine *engine = new Engine();

	// loads input settings from disk
	engine->input->register_from_toml("res/actions.toml");

	// manufactures game scene
	load_opening_scene(engine);

	// begins game loop (locks thread until completion)
	engine->start();

	return 0;
}
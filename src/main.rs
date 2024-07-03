use bevy::prelude::*;
use bevy_tweening::TweeningPlugin;
use game::GamePlugin;
use gen::GAME_VERSION;
use haalka::HaalkaPlugin;

mod game;
mod gen;

fn main() {
    info!("Watery Wails '{}' starting", GAME_VERSION);
    let mut app = App::new();
    #[cfg(target_family = "wasm")]
    {
        // this helps WASM builds to work better especially with ItchIO, according to an official bevy example.
        use bevy::asset::AssetMetaCheck;
        app = app.insert_resource(AssetMetaCheck::Never);
    }
    app.add_plugins(DefaultPlugins)
        .add_plugins(TweeningPlugin) // Tweens
        .add_plugins(HaalkaPlugin) // GUI layouting
        .add_plugins(GamePlugin) // game systems & data
        .run();
}

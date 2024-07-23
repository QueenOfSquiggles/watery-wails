#![allow(dead_code)]

use bevy::prelude::*;
use game::GamePlugin;
use gen::GAME_VERSION;

mod game;
mod gen;

fn main() {
    let mut app = App::new();
    app.add_plugins(DefaultPlugins)
        .add_plugins(GamePlugin) // game systems & data
        .add_systems(Startup, || {
            info!("Watery Wails '{}' starting", GAME_VERSION)
        });
    #[cfg(target_family = "wasm")]
    {
        // this helps WASM builds to work better especially with ItchIO, according to an official bevy example.
        use bevy::asset::AssetMetaCheck;
        app = app.insert_resource(AssetMetaCheck::Never);
    }
    // this is a new line insert to test dependency eval
    #[cfg(not(target_family = "wasm"))]
    {
        // include for all desktop builds
        //TODO: init bevy-panic-handler and steamworks
    }
    app.run();
}

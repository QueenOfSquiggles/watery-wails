#![allow(dead_code)]

use bevy::prelude::*;
use bevy_htnp::HtnAgent;
use game::GamePlugin;
use gen::GAME_VERSION;

mod game;
mod gen;

fn main() {
    let mut app = App::new();
    #[cfg(target_family = "wasm")]
    {
        // this helps WASM builds to work better especially with ItchIO, according to an official bevy example.
        use bevy::asset::AssetMetaCheck;
        app = app.insert_resource(AssetMetaCheck::Never);
    }
    #[cfg(not(target_family = "wasm"))]
    {
        // include for all desktop builds
        //TODO: init bevy-panic-handler and steamworks
    }
    app.add_plugins(DefaultPlugins)
        .add_plugins(GamePlugin) // game systems & data
        .add_systems(Startup, startup_prints)
        .run();
}

fn startup_prints(mut commands: Commands) {
    info!("Watery Wails '{}' starting", GAME_VERSION);
    info!("Test line!");

    commands.spawn(HtnAgent);
}

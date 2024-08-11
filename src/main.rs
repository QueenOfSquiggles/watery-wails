use bevy::prelude::*;
use bevy_htnp::prelude::HtnPlanningPlugin;
use game::GamePlugin;

mod game;
mod gen;

fn main() {
    let mut app = App::new();
    #[cfg(feature = "steam")]
    {
        use bevy_steamworks::SteamworksPlugin;
        // steam requires being initialized before render plugins. Looks gross but I guess I gotta do it this way.
        if let Ok(steam_plugin) = SteamworksPlugin::init_app(gen::STEAM_APP_ID) {
            app.add_plugins(steam_plugin);
        }
    }

    app.add_plugins(DefaultPlugins)
        .add_plugins(HtnPlanningPlugin::new())
        .add_plugins(GamePlugin); // game systems & data
    #[cfg(target_family = "wasm")]
    {
        // this helps WASM builds to work better especially with ItchIO, according to an official bevy example.
        use bevy::asset::AssetMetaCheck;
        app = app.insert_resource(AssetMetaCheck::Never);
    }
    app.run();
}

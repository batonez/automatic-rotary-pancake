#include <glade/glade-game.h>
#include <glade/util/FileManager.h>
#include <strug/ResourceManager.h>
#include <strug/states/Play.h>

Glade::ResourceManager *resource_manager = NULL;
Strug::ResourceManager *game_resource_manager = NULL;

void glade_game_init(InitialGameData *game_init_data, FileManager *file_manager)
{
  ::resource_manager = new Strug::ResourceManager(file_manager);
  ::game_resource_manager = (Strug::ResourceManager *) ::resource_manager;

  game_init_data->initialState = std::unique_ptr<State>(new Play());
}


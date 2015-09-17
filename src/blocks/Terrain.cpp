#include <glade/render/Texture.h>
#include <glade/render/Drawable.h>
#include <glade/render/ShaderProgram.h>
#include <glade/render/TextureTransform.h>
#include <glade/render/meshes/Rectangle.h>
#include <strug/blocks/Terrain.h>
#include <strug/ResourceManager.h>

extern Strug::ResourceManager *game_resource_manager;

Terrain::Terrain():
  Block(),
  baseView(NULL)
{
  setName("Standard terrain");
}

Terrain::~Terrain()
{
  if (baseView != NULL) {
    delete baseView;
    baseView = NULL;
  }
}

void Terrain::initialize(const std::string &texture_pack_name, float block_width, float block_height)
{
  if (!initialized) {
    Block::initialize(texture_pack_name, block_width, block_height);
    
    std::shared_ptr<ShaderProgram> program =
      game_resource_manager->getShaderProgram(
        "texcoord_frames.vertex.glsl",
        "textured.fragment.glsl"
      );
    
    baseView = new Drawable(Rectangle::INSTANCE, program);
    baseView->setTexture(game_resource_manager->getTexture(texture_pack_name, "burnable.png", ""));
    addDrawable(baseView);
    
    updateView();
  }
}


void Terrain::updateView()
{
  Block::updateView();
}

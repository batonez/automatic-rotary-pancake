#include <glade/render/Drawable.h>
#include <glade/exception/GladeFileNotFoundException.h>
#include <strug/blocks/Mud.h>

extern Strug::ResourceManager *game_resource_manager;

Mud::Mud(Block::Properties *properties):
  Terrain(properties),
  baseView(NULL),
  topOverlay(NULL)
{
  setName("Standard burnable terrain");
}

Mud::~Mud()
{
  if (baseView != NULL) {
    delete baseView;
    baseView = NULL;
  }
  
  if (topOverlay != NULL) {
    delete topOverlay;
    topOverlay = NULL;
  }
}

void Mud::initialize(const std::string &texture_pack_name, float block_width, float block_height)
{
  if (!initialized) {
    burnable = true;
    
    std::shared_ptr<ShaderProgram> program =
      game_resource_manager->getShaderProgram(
        "texcoord_frames.vertex.glsl",
        "textured.fragment.glsl"
      );
    
    baseView = new Drawable(Rectangle::INSTANCE, program);
    baseView->setTexture(game_resource_manager->getTexture(texture_pack_name, "burnable.png", ""));
    addDrawable(baseView);
    
    std::shared_ptr<Texture> overlay;
    
    try {
      overlay = game_resource_manager->getTexture(texture_pack_name, "burnable_overlay.png", "", false);
      topOverlay = new Drawable(Rectangle::INSTANCE, program);
      topOverlay->setTexture(overlay);
      addDrawable(topOverlay);
    } catch (GladeFileNotFoundException &e) {}
    
    Terrain::initialize(texture_pack_name, block_width, block_height);
  }
}

void Mud::updateView()
{
  Terrain::updateView();
  
  if (topOverlay != NULL && !burned) {
    if (topEdge) {
      toggleView(*topOverlay, true);
    } else {
      toggleView(*topOverlay, false);
    }
  }
}

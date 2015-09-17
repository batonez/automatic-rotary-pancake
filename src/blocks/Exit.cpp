#include <glade/render/ShaderProgram.h>
#include <glade/render/meshes/Rectangle.h>
#include <glade/render/Drawable.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Exit.h>

extern Strug::ResourceManager *game_resource_manager;

Exit::Exit():
  Block(NULL),
  activeView(NULL),
  inactiveView(NULL),
  active(false)
{
  setName("Exit from the level");
}
  
Exit::~Exit()
{
  if (activeView) {
    delete activeView;
    activeView = NULL;
  }
  
  if (inactiveView) {
    delete inactiveView;
    inactiveView = NULL;
  }
}

void Exit::initialize(const std::string &texture_pack_name, float block_width, float block_height)
{
  if (!initialized) {
    Block::initialize(texture_pack_name, block_width, block_height);
    
    std::shared_ptr<ShaderProgram> program =
      game_resource_manager->getShaderProgram(
        "texcoord_frames.vertex.glsl",
        "textured.fragment.glsl"
      );
    
    activeView = new Drawable(Rectangle::INSTANCE, program);
    inactiveView = new Drawable(Rectangle::INSTANCE, program);
    addDrawable(activeView);
    addDrawable(inactiveView);
    
    getTransform()->setScale(block_width, block_height, 0);
    
    // TODO why do we need two textures. We need them becouse our textransform & texinfo are shitty. We need arbitrary regions/frame strips
    std::shared_ptr<Texture> textureInactive = game_resource_manager->getTexture(texture_pack_name, "exit_inactive.png", "");
    std::shared_ptr<Texture> textureActive = game_resource_manager->getTexture(texture_pack_name, "exit_active.png", "");
    
    activeView->setTexture(textureActive);
    inactiveView->setTexture(textureInactive);
    
    activeView->getTextureTransform()->animate(ANIMATION_ACTIVE, ANIMATION_TIME_ACTIVE, true);
    
    updateView(); // FIXME Is it really needed?
  }
}

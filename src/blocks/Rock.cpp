#include <strug/blocks/Rock.h>

Rock::Rock(Block::Properties *properties):
  Terrain(properties),
  baseView(NULL),
  rightEdgeOverlay(NULL),
  bothEdgesOverlay(NULL),
  noEdgesOverlay(NULL)
{
  burnable = false;
  setName("Unburnable terrain");
  ::srand(time(0));
}

Rock::~Rock()
{
  if (baseView) {
    delete baseView;
    baseView = NULL;
  }
  
  if (rightEdgeOverlay) {
    delete rightEdgeOverlay;
    rightEdgeOverlay = NULL;
  }
  
  if (bothEdgesOverlay) {
    delete bothEdgesOverlay;
    bothEdgesOverlay = NULL;
  }
  
  if (noEdgesOverlay) {
    delete noEdgesOverlay;
    noEdgesOverlay = NULL;
  }
}

void Rock::initialize(const std::string &texture_pack_name, float block_width, float block_height)
{
  if (!initialized) {
    std::shared_ptr<ShaderProgram> program =
      game_resource_manager->getShaderProgram(
        "texcoord_frames.vertex.glsl",
        "textured.fragment.glsl"
      );
  
    std::shared_ptr<Texture> texture = game_resource_manager->getTexture(texture_pack_name, "burnable.png", "", false);
    
    baseView = new Drawable(Rectangle::INSTANCE, program);
    addDrawable(baseView);
    baseView->setTexture(texture);
    
    texture = game_resource_manager->getTexture(texture_pack_name, "unburnable_overlay.png", "", false);
    noEdgesOverlay = new Drawable(Rectangle::INSTANCE, program);
    addDrawable(noEdgesOverlay);
    noEdgesOverlay->setTexture(texture);
    
    texture = game_resource_manager->getTexture(texture_pack_name,  "unburnable_overlay_right.png", "", false);
    rightEdgeOverlay = new Drawable(Rectangle::INSTANCE, program);
    addDrawable(rightEdgeOverlay);
    rightEdgeOverlay->setTexture(texture);
    
    texture = game_resource_manager->getTexture(texture_pack_name, "unburnable_overlay_left_right.png", "", false);
    bothEdgesOverlay = new Drawable(Rectangle::INSTANCE, program);
    addDrawable(bothEdgesOverlay);
    bothEdgesOverlay->setTexture(texture);
    
    if (::rand() % 2) {
      bothEdgesOverlay->getTextureTransform()->flipTextureHorizontally(true);
    }
    
    Terrain::initialize(texture_pack_name, block_width, block_height);
  }
}

void Rock::updateView()
{
  Terrain::updateView();
  
  if (!burned) {
    toggleView(*baseView,         true);
    toggleView(*bothEdgesOverlay, rightEdge && leftEdge );
    toggleView(*noEdgesOverlay,  !rightEdge && !leftEdge);
    toggleView(*rightEdgeOverlay, rightEdge != leftEdge );
    
    rightEdgeOverlay->getTextureTransform()
      ->flipTextureHorizontally(leftEdge && !rightEdge);
  }
}
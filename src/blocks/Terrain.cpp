#include <glade/render/Texture.h>
#include <glade/render/TextureTransform.h>
#include <strug/blocks/Terrain.h>

extern Strug::ResourceManager *game_resource_manager;

class HideLavaAfterBurned: public TextureTransform::AnimationCallback
{
  private:
    Terrain &terrainBlock;
  public:
    HideLavaAfterBurned(Terrain &terrain_block):
      TextureTransform::AnimationCallback(NULL),
      terrainBlock(terrain_block)
    {
    }
    
    virtual void call(TextureTransform &texTransform)
    {
      terrainBlock.isLavaVisible = false;
      terrainBlock.updateView();
    }
    
    virtual int getTriggeringFrame()
    {
      return 7; // TODO Последний кадр не виден, т.к. лава скрывается сразу по достижению последнего кадра
    }

    virtual float getTriggeringTime()
    {
      return -1.0f; // no triggering time
    }    
};

const float      Terrain::TRAVELATOR_ANIMATION_TIME = 159.999995f;
TextureTransform Terrain::swampTexTransform;
TextureTransform Terrain::propulsionTexTransform;
TextureTransform Terrain::leftToRightTravelatorTexTransform;
TextureTransform Terrain::rightToLeftTravelatorTexTransform;
TextureTransform Terrain::travelatorLeftEdgeTexTransform;
TextureTransform Terrain::travelatorRightEdgeTexTransform;

void Terrain::initialize(const std::string &texture_pack_name, float block_width, float block_height)
{
  if (!initialized) {
    Block::initialize(texture_pack_name, block_width, block_height);
    
    std::shared_ptr<ShaderProgram> program =
      game_resource_manager->getShaderProgram(
        "texcoord_frames.vertex.glsl",
        "textured.fragment.glsl"
      );
    
    swampView           = new Drawable(Rectangle::INSTANCE, program);
    propulsionView      = new Drawable(Rectangle::INSTANCE, program);
    travelatorView      = new Drawable(Rectangle::INSTANCE, program);
    travelatorLeftEdge  = new Drawable(Rectangle::INSTANCE, program);
    travelatorRightEdge = new Drawable(Rectangle::INSTANCE, program);
    pedalView           = new Drawable(Rectangle::INSTANCE, program);
    lavaView            = new Drawable(Rectangle::INSTANCE, program);
    
    swampTexTransform.animate(0, 1000, true);
    swampView->setTexture(game_resource_manager->getTexture(texture_pack_name, "slowing.png", ""));
    swampView->setTextureTransform(swampTexTransform);
    addDrawable(swampView);
    
    propulsionTexTransform.animate(0, TRAVELATOR_ANIMATION_TIME, true);
    propulsionView->setTexture(game_resource_manager->getTexture(texture_pack_name, "propulsion.png", ""));
    propulsionView->setTextureTransform(propulsionTexTransform);
    addDrawable(propulsionView);
    
    leftToRightTravelatorTexTransform.animate(0, TRAVELATOR_ANIMATION_TIME, true);
    rightToLeftTravelatorTexTransform.animate(0, TRAVELATOR_ANIMATION_TIME, true);
    
    if (!rightToLeftTravelatorTexTransform.isFlippedHorizontally()) {
      rightToLeftTravelatorTexTransform.flipTextureHorizontally(true);
    }
    
    travelatorView->setTexture(game_resource_manager->getTexture(texture_pack_name, "travelator.png", ""));
    addDrawable(travelatorView);
    
    std::shared_ptr<Texture> texture = game_resource_manager->getTexture(texture_pack_name, "travelator_edge_left.png", "", false);
    
    travelatorLeftEdge->setTexture(texture);
    addDrawable(travelatorLeftEdge);
    travelatorLeftEdge->setTextureTransform(travelatorLeftEdgeTexTransform);
    
    travelatorRightEdge->setTexture(texture);
    addDrawable(travelatorRightEdge);
    
    if (!travelatorRightEdgeTexTransform.isFlippedHorizontally()) {
      travelatorRightEdgeTexTransform.flipTextureHorizontally(true);
    }
    
    travelatorRightEdge->setTextureTransform(travelatorRightEdgeTexTransform);

    addDrawable(pedalView);
    pedalView->setTexture(game_resource_manager->getTexture(texture_pack_name, "pedal_overlay.png", ""));
    
    addDrawable(lavaView);
    lavaView->setTexture(game_resource_manager->getTexture(texture_pack_name, "lava_overlay.png", ""));
    
    updateView();
  }
}

void Terrain::burn()
{
  lavaView->getTextureTransform()->setCurrentFrameNumber(0);
  lavaView->getTextureTransform()->animate(0, 400, false);
  lavaView->getTextureTransform()->addAnimationCallback(new HideLavaAfterBurned(*this));
  
  burningTimer.reset();
  burned = true;
  isLavaVisible = true;
  updateView();
}

void Terrain::updateView()
{
  Block::updateView();
  
  toggleView(!burned);
  toggleView(*lavaView, isLavaVisible);
  toggleCollisionShape(!burned);
  
  if (leftToRight) {
    travelatorView->setTextureTransform(leftToRightTravelatorTexTransform);
  } else {
    travelatorView->setTextureTransform(rightToLeftTravelatorTexTransform);
  }

  toggleView(*travelatorLeftEdge, travelator && leftEdge);
  toggleView(*travelatorRightEdge, travelator && rightEdge);
  toggleView(*travelatorView, travelator);
  toggleView(*propulsionView, propulsing);
  toggleView(*swampView, slowing);
  toggleView(*pedalView, pedal);
}

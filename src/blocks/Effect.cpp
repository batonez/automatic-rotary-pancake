#include <glade/render/Drawable.h>
#include <glade/render/meshes/Rectangle.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Effect.h>

extern Strug::ResourceManager *game_resource_manager;

class EffectAnimationCallback: public TextureTransform::AnimationCallback
{
  private:
    Effect &host;
    
  public:
    EffectAnimationCallback(Callable *callback, Effect &host_param):
      TextureTransform::AnimationCallback(callback),
      host(host_param)
    {}
    
    virtual void call() { host.toggleView(*host.view, false); }
    virtual int getTriggeringFrame() { return 7; }
};
    
Effect Effect::INSTANCE;

Effect::Effect():
  Block(),
  view(NULL)
{}
  
void Effect::play(float x, float y, Callable *callback)
{
  INSTANCE.getTransform()->getPosition()->x = x;
  INSTANCE.getTransform()->getPosition()->y = y;
  INSTANCE.toggleView(*INSTANCE.view, true);
  INSTANCE.view->getTextureTransform()->addAnimationCallback(new EffectAnimationCallback(callback, Effect::INSTANCE));
  INSTANCE.view->getTextureTransform()->animate(0, 200, false, true);
  //SoundManager.getRandomSound("common", SoundManager.EXPLOSION_GROUP_ID, 2).play();
}

Effect::~Effect()
{
  if (view) {
    delete view;
    view = NULL;
  }
}

void Effect::initialize(const std::string &texture_pack_name, float block_width, float block_height)
{
  if (!initialized) {
    Block::initialize(texture_pack_name, block_width, block_height);
    
    getTransform()->setScale(block_width, block_height, 0);
    
    std::shared_ptr<ShaderProgram> program =
      game_resource_manager->getShaderProgram(
        "texcoord_frames.vertex.glsl",
        "textured.fragment.glsl"
      );
    
    view = new Drawable(Rectangle::INSTANCE, program);
    addDrawable(view);
    view->setTexture(game_resource_manager->getTexture(texture_pack_name, "explosion.png", "frame_size_128.csv"));
    toggleView(*view, false);
    
    //addSound(SoundManager.getSound("common", SoundManager.EXPLOSION_GROUP_ID, 0, true));
    //addSound(SoundManager.getSound("common", SoundManager.EXPLOSION_GROUP_ID, 1, true));
  }
}

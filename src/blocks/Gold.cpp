#include <glade/render/Drawable.h>
#include <strug/Blocks/Gold.h>

Gold::Gold():
  Collectable()
{
  setName("Gold");
}

void Gold::initialize(const std::string &texture_pack_name, float block_width, float block_height)
{
  if (!initialized) {
    Collectable::initialize(texture_pack_name, block_width, block_height);
    
    view->setUniform("uMaterialAmbient",   Vector4f(0.1f, 0.8f, 0.3f, 1.0f));
    view->setUniform("uMaterialDiffuse",   Vector4f(0.1f, 0.8f, 0.3f, 1.0f));
    view->setUniform("uMaterialSpecular",  Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    view->setUniform("uMaterialShininess", 128.0f);
  }
}
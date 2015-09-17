#include <glade/render/ShaderProgram.h>
#include <glade/render/Drawable.h>
#include <glade/render/meshes/Cube.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Collectable.h>

extern Strug::ResourceManager *game_resource_manager;

// TODO initial angle was random and it was cool
Transform::SharedVector Collectable::cubeRotation(new Vector3f(45.0f, 0, 45.0f));

Collectable::Collectable():
  Block(NULL),
  view(NULL)
{
  setName("A collectable");
}

Collectable::~Collectable()
{
  if (view) {
    delete view;
    view = NULL;
  }
}

Collectable::Weapon Collectable::getWeaponPowerUp()
{
  return Weapon::NONE;
}
 
void Collectable::initialize(const std::string &texture_pack_name, float block_width, float block_height)
{
  Block::initialize(texture_pack_name, block_width, block_height);
    
  std::shared_ptr<ShaderProgram> program =
    game_resource_manager->getShaderProgram(
      "lit_shape.vertex.glsl",
      "lit_shape.fragment.glsl"
    );
  
  view = new Drawable(Cube::INSTANCE, program);
  
  view->getTransform()->setRotation(cubeRotation);
  view->getTransform()->setRotation(0.33f, 0.33f, 0.33f);
  
  addDrawable(view);
}

#pragma once

#include <strug/blocks/Block.h>

extern Strug::ResourceManager *game_resource_manager;

class Ladder: public Block
{
  private:
    Drawable *rectView;
    
  public:
    Ladder():
      Block(NULL),
      rectView(NULL)
    {
      setName("Vertical ladder");
    }
    
    virtual ~Ladder()
    {
      if (rectView) {
        delete rectView;
        rectView = NULL;
      }
    }
  
    virtual void initialize(const std::string &texture_pack_name, float block_width, float block_height)
    {
      if (!initialized) {
        Block::initialize(texture_pack_name, block_width, block_height);
        
        std::shared_ptr<ShaderProgram> program =
          game_resource_manager->getShaderProgram(
            "texcoord_frames.vertex.glsl",
            "textured.fragment.glsl"
          );
        
        rectView = new Drawable(Rectangle::INSTANCE, program);
        addDrawable(rectView);
        rectView->setTexture(game_resource_manager->getTexture(texture_pack_name, "ladder.png", ""));
      }
    }
    
    virtual Block::Type getType() const
    {
      return Block::LADDER;
    }

    virtual bool isOverlapChecked()
    {
      return false;
    }

    virtual bool isMoveable()
    {
      return false;
    }
    
    virtual bool isUsable()
    {
      return false;
    }
};

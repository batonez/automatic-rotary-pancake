#pragma once

#include <glade/GladeObject.h>

class Block : public GladeObject
{
  class DrawingOrderComparator : public GladeObject::Comparator
  {
    virtual int compare(GladeObject &first, GladeObject &second);
  };

  public:
    enum Type {
      MUD           = 1,
      LADDER        = 3,
      PIPE          = 4,
      NPC           = 5,
      PLAYER        = 6,
      GOLD          = 7,
      ROCK          = 8,
      SPIKES        = 10,
      EXIT          = 11,
      EFFECT        = 15,
      SLOWING_SHOT  = 16,
      SPEEDUP_SHOT  = 17
    };
    
    typedef std::vector<Type> DrawingOrder;
    typedef std::map<std::string, int> Properties;

  protected:
    bool initialized;
  
  private:
    static DrawingOrder drawingOrder;
    int label;
    
  public:    
    static int getDrawingOrder(Block *block);
        
    Block(Properties *properties = NULL);
    
    virtual Type  getType() const   = 0;
    virtual bool isOverlapChecked() = 0;
    virtual bool isMoveable()       = 0;
    virtual bool isUsable()         = 0;
    
    virtual void        initialize(const std::string &texture_pack_name, float block_width, float block_height);
    virtual std::string toString();
    virtual void        updateView() {}
    
    int getLabel() const { return label; }
    void setLabel(int label_param) { label = label_param; }
    bool looksLike(Block &block) { return getType() == block.getType(); }
};

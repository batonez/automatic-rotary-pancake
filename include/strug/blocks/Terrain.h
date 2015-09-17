#pragma once

#include <memory>
#include <map>
#include <string>

#include <glade/Timer.h>
#include <glade/render/Drawable.h>
#include <glade/render/TextureTransform.h>
#include <glade/render/meshes/Rectangle.h>

#include <strug/ResourceManager.h>
#include <strug/blocks/Block.h>

class Level;

class Terrain: public Block
{
  friend class HideLavaAfterBurned;
  
  public:
    static const float TRAVELATOR_ANIMATION_TIME;
    Timer burningTimer;
    
  protected:
    bool leftEdge;
    bool rightEdge;
    bool topEdge;
    bool isLavaVisible;
    bool burnable;
    bool burned;
    double burnedTimestamp;
  
  private:
    Drawable *swampView;
    Drawable *propulsionView;
    Drawable *travelatorView, *travelatorLeftEdge, *travelatorRightEdge;
    Drawable *pedalView;
    Drawable *lavaView;
    static TextureTransform swampTexTransform;
    static TextureTransform propulsionTexTransform;
    static TextureTransform leftToRightTravelatorTexTransform;
    static TextureTransform rightToLeftTravelatorTexTransform;
    static TextureTransform travelatorLeftEdgeTexTransform;
    static TextureTransform travelatorRightEdgeTexTransform;
  
    bool slowing;
    bool propulsing;
    bool travelator, leftToRight;
    
    bool pedal, pushed;
    int controlledObjectLabel;
  
  public:
    Terrain(Block::Properties *properties = NULL):
      Block(properties),
      leftEdge(false),
      rightEdge(false),
      topEdge(false),
      isLavaVisible(false),
      burnable(true),
      burned(false),
      burnedTimestamp(0),
      slowing(false),
      propulsing(false),
      travelator(false),
      leftToRight(false),
      pedal(false),
      pushed(false),
      controlledObjectLabel(-1),
      swampView(NULL),
      propulsionView(NULL),
      travelatorView(NULL),
      travelatorLeftEdge(NULL),
      travelatorRightEdge(NULL),
      pedalView(NULL),
      lavaView(NULL)
    {
      if (properties == NULL) {
        return;
      }
      
      slowing = (properties->find("slowing") == properties->end() ? false : true);
      propulsing = (properties->find("propulsing") == properties->end() ? false : true);
      travelator = (properties->find("travelator") == properties->end() ? false : true);
      leftToRight = (properties->find("leftToRight") == properties->end() ? false : true);
      pedal = (properties->find("pedal") == properties->end() ? false : true);
      controlledObjectLabel = properties->find("controlledObject") == properties->end() ? -1 :
        properties->find("controlledObject")->second;
    }
    
    ~Terrain()
    {
      if (swampView != NULL) {
        delete swampView;
        swampView = NULL;
      }
      
      if (propulsionView != NULL) {
        delete propulsionView;
        propulsionView = NULL;
      }
      
      if (travelatorView != NULL) {
        delete travelatorView;
        travelatorView = NULL;
      }
      
      if (travelatorLeftEdge != NULL) {
        delete travelatorLeftEdge;
        travelatorLeftEdge = NULL;
      }
      
      if (travelatorRightEdge != NULL) {
        delete travelatorRightEdge;
        travelatorRightEdge = NULL;
      }
      
      if (pedalView != NULL) {
        delete pedalView;
        pedalView = NULL;
      }
      
      if (lavaView != NULL) {
        delete lavaView;
        lavaView = NULL;
      }
    }
    
    virtual void initialize(const std::string &texture_pack_name, float block_width, float block_height);
    void burn();
    virtual void updateView();
    
    bool looksLike(Block *block)
    {
      return
        Block::looksLike(*block) &&
        !(travelator ^ ((Terrain*)block)->travelator) &&
        !(leftToRight ^ ((Terrain*)block)->leftToRight) &&
        !(slowing ^ ((Terrain*)block)->slowing) &&
        !(propulsing ^ ((Terrain*)block)->propulsing) &&
        !(pedal ^ ((Terrain*)block)->pedal) &&
        !(burned ^ ((Terrain*)block)->burned);
    }
    
    void setPushed(bool pushed) { this->pushed = pushed; }
    int getControlledObjectLabel() { return controlledObjectLabel; }
    bool isOverlapChecked() { return false; }
    bool isMoveable() { return false; }
    bool isUsable() { return false; }
    bool isBurnable() { return burnable; }
    bool isBurned() { return burned; }
    double getBurnedTimestamp() { return burnedTimestamp; }
    void setBurnable(bool burnable) { this->burnable = burnable; }
    bool isSlowing() { return slowing; }
    
    void setSlowing(bool slowing)
    {
      if (slowing) {
        setPropulsing(false);
        setTravelator(false);
        setBurnable(false);
        setPedal(false);
      }
      
      this->slowing = slowing;
      updateView();
    }
    
    bool isPropulsing() { return propulsing; }
    
    void setPropulsing(bool propulsing)
    {
      if (propulsing) {
        setSlowing(false);
        setTravelator(false);
        setBurnable(false);
        setPedal(false);
      }
      
      this->propulsing = propulsing;
      updateView();
    }
    
    bool isTravelator() { return travelator; }
    
    void setTravelator(bool travelator)
    {
      if (travelator) {
        setSlowing(false);
        setPropulsing(false);
        setBurnable(false);
        setPedal(false);
      } else {
        setLeftToRight(false);
      }
      
      this->travelator = travelator;
      updateView();
    }
    
    bool isPedal() { return pedal; }
    
    void setPedal(bool pedal)
    {
      if (pedal) {
        setSlowing(false);
        setPropulsing(false);
        setBurnable(false);
        setTravelator(false);
      }
      
      this->pedal = pedal;
      updateView();
    }
    
    bool isPushed() { return pushed; }
    bool isLeftToRight() { return leftToRight; }
    
    void setLeftToRight(bool leftToRight)
    {
      this->leftToRight = leftToRight;
      updateView();
    }
    
    void restore()
    {
      burned = false;
      updateView();
    }
    
    void setLeftEdge(bool leftEdge)
    {
      if (this->leftEdge ^ leftEdge) {
        this->leftEdge = leftEdge;
        updateView();
      }
    }
    
    void setRightEdge(bool rightEdge)
    {
      if (this->rightEdge ^ rightEdge) {
        this->rightEdge = rightEdge;
        updateView();
      }
    }
    
    void setTopEdge(bool topEdge)
    {
      if (this->topEdge ^ topEdge) {
        this->topEdge = topEdge;
        updateView();
      }
    }
    
    virtual std::string toString()
    {
      std::string result = Block::toString();
      
      result += slowing ? " slowing=1" : "";
      result += propulsing ? " propulsing=1" : "";
      result += travelator ? " travelator=1" : "";
      result += travelator && leftToRight ? " leftToRight=1" : "";
      result += pedal ? " pedal=1" : "";
      result += controlledObjectLabel > 0 ? " controlledObject=" + controlledObjectLabel : "";
      
      return result;
    }
};

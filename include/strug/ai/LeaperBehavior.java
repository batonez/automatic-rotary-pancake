package ru.hatchery.strug.ai;

import ru.hatchery.glade.GladeObject;
import ru.hatchery.glade.State;
import ru.hatchery.glade.ai.Behavior;
import ru.hatchery.strug.blocks.Character;
import ru.hatchery.strug.states.Play;

public class LeaperBehavior implements Behavior
{
  public static final int LEFT = 0;
  public static final int RIGHT = 1;
  public static final int UP = 2;
  public static final int DOWN = 3;
  
  private Integer direction;
  private Integer initialBlockX, initialBlockY;
  private float targetCoordX, targetCoordY;
  
  @Override
  public void act(State world, GladeObject selfObject)
  {
    try {
      act((Play) world, (Character) selfObject);
    } catch (ClassCastException e) {
      throw new RuntimeException("This behavior is not applicable for the given host object and world");
    }
  }
  
  public void setDirection(Play world, Character self, int direction)
  {
    this.direction = direction;
    initialBlockX = world.getBlockCoordX(self);
    initialBlockY = world.getBlockCoordY(self);
    
    if (direction == LEFT) {
      targetCoordX = world.blockToWorldCoordX(initialBlockX - 1);
      targetCoordY = world.blockToWorldCoordY(initialBlockY);
    } else if (direction == RIGHT) {
      targetCoordX = world.blockToWorldCoordX(initialBlockX + 1);
      targetCoordY = world.blockToWorldCoordY(initialBlockY);
    } else if (direction == UP) {
      targetCoordX = world.blockToWorldCoordX(initialBlockX);
      targetCoordY = world.blockToWorldCoordY(initialBlockY - 1);
    } else if (direction == DOWN) {
      targetCoordX = world.blockToWorldCoordX(initialBlockX);
      targetCoordY = world.blockToWorldCoordY(initialBlockY + 1);
    }
  }
  
  public void act(Play world, Character self)
  {
    self.resetIntentions();
    
    if (direction == null) {
      self.toggleBehavior(false);
      return;
    }
    
    switch (direction) {
      case LEFT:
        if (self.getTransform().getPosition().x <= targetCoordX) {
          world.alignToGridHorizontally(self);
          direction = null;
          self.locked = false;
        } else {
          self.wantsToGoLeft = true;
        }
        
        break;
      case RIGHT:
        if (self.getTransform().getPosition().x >= targetCoordX) {
          world.alignToGridHorizontally(self);
          direction = null;
          self.locked = false;
        } else {
          self.wantsToGoRight = true;
        }
        
        break;
      case UP:
        if (self.getTransform().getPosition().y <= targetCoordY) {
          world.alignToGridVertically(self);
          direction = null;
          self.locked = false;
        } else {
          self.wantsToGoUp = true;
        }
        
        break;
      case DOWN:
        if (self.getTransform().getPosition().y >= targetCoordY) {
          world.alignToGridVertically(self);
          direction = null;
          self.locked = false;
        } else {
          self.wantsToGoDown = true;
        }
        
        break;
    }
  }
}

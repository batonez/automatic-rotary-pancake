package ru.hatchery.strug.ai;

import java.util.Deque;
import java.util.NoSuchElementException;

import ru.hatchery.glade.GladeObject;
import ru.hatchery.glade.State;
import ru.hatchery.glade.ai.Behavior;
import ru.hatchery.glade.ai.DijkstraPathFinder;
import ru.hatchery.glade.model.GraphNode;
import ru.hatchery.strug.blocks.Block;
import ru.hatchery.strug.blocks.Character;
import ru.hatchery.strug.states.Play;

public class StalkerBehavior implements Behavior
{
  private Integer rememberedTargetCoordX, rememberedTargetCoordY, rememberedSelfCoordX, rememberedSelfCoordY;
  private Deque<GraphNode> route;
  private Block target;
  private GraphNode next;
  private DijkstraPathFinder pathFinder = new DijkstraPathFinder(); // TODO Не нужен отдельный инстанс PathFinder'a в каждом инстансе Behavior'a
  
  @Override
  public void act(State world, GladeObject selfObject)
  {
    if (!(world instanceof Play)) {
      throw new ClassCastException("This behavior is not applicable for the given world");
    }
    
    if (!(selfObject instanceof Character)) {
      throw new ClassCastException("This behavior is not applicable for the given self object");
    }
    
    act((Play) world, (Character) selfObject);
  }
  
  public void act(Play world, Character self)
  {
    self.resetIntentions();
    setTarget(null);
    
    for (Character character: world.getPlayerCharacters()) {
      if (!character.isDead()) {
        setTarget(character);
      }
    }
    
    if (target == null) {
      return;
    }
    
    if (targetMoved(world)) {
      recalculateRoute(world, self);
    } else if (selfMoved(world, self)) {
      if (selfMovedUnexpectedly(world, self)) {
        recalculateRoute(world, self);
      } else {
        rememberedSelfCoordX = world.getBlockCoordX(self);
        rememberedSelfCoordY = world.getBlockCoordY(self);
        
        pop();
      }
    }
    
    setIntentions(world, self);  
  }
  
  public void setTarget(Block target)
  {
    this.target = target;
  }
  
  private void setIntentions(Play world, Character self)
  {
    if (next == null) {
      return;
    }
    
    if (world.getBlockCoordsOfTheNode(next).x > world.getBlockCoordX(self)) {
      self.wantsToGoRight = true;
    } else if (world.getBlockCoordsOfTheNode(next).x < world.getBlockCoordX(self)) {
      self.wantsToGoLeft = true;
    } else if (world.getBlockCoordsOfTheNode(next).y > world.getBlockCoordY(self)) {
      self.wantsToGoDown = true;
    } else if (world.getBlockCoordsOfTheNode(next).y < world.getBlockCoordY(self)) {
      self.wantsToGoUp = true;
    }
  }
  
  private void recalculateRoute(Play world, Character self)
  {
    rememberedTargetCoordX = world.getBlockCoordX(target);
    rememberedTargetCoordY = world.getBlockCoordY(target);
    
    GraphNode targetNode = world.getNodeAtBlock(world.getBlockCoordX(target), world.getBlockCoordY(target));
    GraphNode sourceNode = world.getNodeAtBlock(world.getBlockCoordX(self), world.getBlockCoordY(self));
    
    if (targetNode != null && sourceNode != null) {
      route = pathFinder.findPath(
        world.getGraph(),
        sourceNode,
        targetNode
      );
      
      pop();
    }
  }
  
  private boolean targetMoved(Play world)
  {
    if (rememberedTargetCoordX == null || rememberedTargetCoordY == null) {
      rememberedTargetCoordX = world.getBlockCoordX(target);
      rememberedTargetCoordY = world.getBlockCoordY(target);
      
      return true;
    }
    
    return
      rememberedTargetCoordX != world.getBlockCoordX(target) ||
      rememberedTargetCoordY != world.getBlockCoordY(target);
  }
  
  private boolean selfMoved(Play world, Character self)
  {
    if (rememberedSelfCoordX == null || rememberedSelfCoordY == null) {
      return true;
    }
    
    return rememberedSelfCoordX != world.getBlockCoordX(self) || rememberedSelfCoordY != world.getBlockCoordY(self);
  }
  
  private boolean selfMovedUnexpectedly(Play world, Character self)
  {
    if (next == null) {
      return true;
    }
    
    return world.getBlockCoordsOfTheNode(next).x != world.getBlockCoordX(self) || world.getBlockCoordsOfTheNode(next).y != world.getBlockCoordY(self);
  }
  
  private void pop()
  {
    try {
      next = route.pop();
    } catch (NoSuchElementException e) { }
  }
}

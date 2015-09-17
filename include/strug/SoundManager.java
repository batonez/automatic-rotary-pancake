package ru.hatchery.strug;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

import ru.hatchery.glade.audio.Sound;
import ru.hatchery.glade.exception.GladeInputStreamIsNullException;

public class SoundManager
{
  public static final String SOUNDS_DIRECTORY = "/sounds";
  public static final String DEFAULT_SOUND_FILENAME = SOUNDS_DIRECTORY + "/nosound.wav";
  
  public static final int BACKGROUND_MUSIC_GROUP_ID = 0;
  public static final int IMPALE_GROUP_ID          = 1;
  public static final int EXPLOSION_GROUP_ID      = 2;
  public static final int LASER_GROUP_ID         = 3;
  public static final int LANDING_GROUP_ID       = 5;
  public static final int ROBOT_FALLING_GROUP_ID    = 6;
  
  private static final Map<String,Sound> soundPool = new HashMap<String,Sound>();  
  private static Sound defaultSound;
  private static final Random random = new Random();
  
  static
  {
    defaultSound = null;
    
    try {
      defaultSound = Sound.create(SoundManager.class.getResourceAsStream(DEFAULT_SOUND_FILENAME));
    } catch (IOException e) {
      e.printStackTrace();
    } catch (GladeInputStreamIsNullException e) {
      
    }
  }
  
  public static Sound getRandomSound(String soundPackName, int soundGroupId, int numberOfSounds)
  {
    return getSound(soundPackName, soundGroupId, random.nextInt(numberOfSounds), true);
  }
  
  public static Sound getSound(String soundPackName, int soundGroupId)
  {
    return getSound(soundPackName, soundGroupId, 0, true);
  }
  
  public static Sound getSound(String soundPackName, int soundGroupId, int soundId, boolean useDefault)
  {
    String key = getPoolKey(soundPackName, soundGroupId, soundId);
    Sound sound = soundPool.get(key);
    
    if (sound == null) {
      sound = createSound(soundPackName, soundGroupId, soundId);
      
      if (sound == null) {
        key = getPoolKey("common", soundGroupId, soundId);
        sound = soundPool.get(key);
        
        if (sound == null) {
          sound = createSound("common", soundGroupId, soundId);
        }
      }
    }
    
    if (sound == null) {
      if (useDefault) {
        sound = defaultSound;
      }
    } else {
      soundPool.put(key, sound);
    }
    
    return sound;
  }
  
  public void clear()
  {
    soundPool.clear();
  }
  
  private static Sound createSound(String soundPackName, int soundGroupId, int soundId)
  {
    Sound sound = null;
    
    try {
      sound = Sound.create(
        SoundManager.class.getResourceAsStream(
          SOUNDS_DIRECTORY + "/" + soundPackName + "/" + soundGroupId + "/" + soundId + ".wav"
        )
      );
    } catch (GladeInputStreamIsNullException e) {
      
    } catch (IOException e) {
      e.printStackTrace();
    }
    
    return sound;
  }
  
  private static String getPoolKey(String packName, Integer groupId, Integer resourceId)
  {
    return packName + groupId + resourceId;
  }
}

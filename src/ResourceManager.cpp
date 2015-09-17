#pragma once

#include <sstream>

#include <glade/util/CSVReader.h>
#include <glade/debug/log.h>
#include <glade/exception/GladeFileNotFoundException.h>
#include <strug/exception/StrugException.h>
#include <strug/LevelInfo.h>
#include <strug/Level.h>
#include <strug/blocks/Block.h>
#include <strug/blocks/Terrain.h>
#include <strug/blocks/Player.h>
#include <strug/ResourceManager.h>

namespace Strug
{
  ResourceManager::ResourceManager(FileManager *file_manager):
    Glade::ResourceManager(file_manager),
    LEVELS_SUBDIRECTORY("levels"),
    LEVELS_DESC_FILE_PATH("levels.desc"),
    TEXTURES_SUBDIRECTORY("textures")
  {}

  int ResourceManager::getNumberOfLevelPacks()
  {
    if (levelPacks.empty()) {
      loadLevelPacksInfo();
    }
    
    return levelPacks.size();
  }
  
  std::shared_ptr<LevelPackInfo> ResourceManager::getLevelPackInfo(int index)
  {
    if (levelPacks.empty()) {
      loadLevelPacksInfo();
    }
    
    return levelPacks[index];
  }
  
  void ResourceManager::loadLevelPacksInfo()
  {
    std::ifstream rawLevelsDesc;
    fileManager->getFileContents(LEVELS_SUBDIRECTORY + LEVELS_DESC_FILE_PATH, rawLevelsDesc);
    
    std::vector<std::vector<std::string> > levelsDesc;
    CSVReader::read(rawLevelsDesc, levelsDesc);
    
    std::vector<std::vector<std::string> >::iterator i = levelsDesc.begin();
    std::shared_ptr<LevelPackInfo> pack;
    LevelInfo levelInfo;
    
    while (i != levelsDesc.end()) {
      if ((*i)[0] == "pack") {
        if (pack != nullptr && !pack->levels.empty()) {
          levelPacks.push_back(pack);
        }
        
        pack.reset(new LevelPackInfo);
        pack->title = (*i)[1];
      } else if (pack != nullptr) {
        levelInfo.path = (*i)[0];
        levelInfo.title = (*i)[1];
        pack->levels.push_back(levelInfo);
      }
      
      ++i;
    }
    
    if (pack != nullptr && !pack->levels.empty()) {
      levelPacks.push_back(pack);
    }
  }
  
  std::shared_ptr<Level> ResourceManager::getLevel(Path &filename)
  {
    // FIXME! Use cache
    std::shared_ptr<Level> result;
    
    std::ifstream levelDataStream;
    fileManager->getFileContents(LEVELS_SUBDIRECTORY + filename, levelDataStream);
    
    std::vector<std::vector<std::string> > rawLevelData;
		CSVReader::read(levelDataStream, rawLevelData);
		
    try {
      if (rawLevelData.size() < 3) {
        throw StrugException("Level data is invalid: file is less than 3 lines");
      }
      
      std::vector<std::vector<std::string> >::iterator line = rawLevelData.begin();
		
      if (line->size() != 3) {
        throw StrugException("Level data is invalid: line 1 must contain 3 values");
      }
      
      result.reset(new Level(atoi(line->at(1).c_str()), atoi(line->at(2).c_str())));
			++line;
			
			if (line->size() != 2) {
				throw StrugException("Level data is invalid: line 2 must contain 2 values");
			}

			result->texturePackName = line->at(1);
      
			for (int y = 0; y < result->getHeightInBlocks(); y++) {
				for (int x = 0; x < result->getWidthInBlocks(); x++) {
          // for every cell in the level read a line from csv
					++line;
          
					if (line != rawLevelData.end()) {
						for (int i = 0; i < line->size(); i++) {
              // every csv value is a block
							std::istringstream singleBlockData(line->at(i));
              std::string  property;
              std::getline(singleBlockData, property, ' ');             
              Block::Type blockType = (Block::Type) atoi(property.c_str());
              Block::Properties blockProperties;
              
              // parsing block properties
              while (!singleBlockData.eof()) {
                std::string propertyName;
                std::string propertyValue;
                std::getline(singleBlockData, propertyName,  '=');
                std::getline(singleBlockData, propertyValue, ' ');
                blockProperties[propertyName] = atoi(propertyValue.c_str());
              }
                            
              Block *object = NULL;
              
              switch (blockType) {
                case 0:
                  // no block created
                  break;
                case Block::TERRAIN:
                  object = new Terrain();
                  break;
                case Block::PLAYER:
                  object = new Player();
                  break;
                default:
                  log("Block: warning: could not create block of unknown type");
                  break;
              }

              if (object != NULL) {
                //log("ADDING BLOCK OF TYPE: %d", blockType);
                result->add(object, x, y);
              }
						}
					} else {
						throw StrugException("Level data is invalid: not enough lines for the given level size");
					}
				}
			}
		} catch (std::out_of_range &e) {
			throw StrugException("Invalid level data");
		}
    
		return result;
  }
  
  std::shared_ptr<Texture> ResourceManager::getTexture(const std::string texture_pack_name, const Path &image_filename,
    const Path &texture_data_filename, bool use_nulltexture)
  {
    Vector2i frameSize;
    
    Path bar = TEXTURES_SUBDIRECTORY + texture_pack_name + texture_data_filename;
    
    if (texture_data_filename.toString().length()) {
      // FIXME! Since we are loading texture data file without Glade resource manager, we should cache it here
      std::ifstream textureInfoStream;
      std::vector<std::vector<std::string> > textureInfo;
      
      try {
        fileManager->getFileContents(TEXTURES_SUBDIRECTORY + texture_pack_name + texture_data_filename, textureInfoStream);
        CSVReader::read(textureInfoStream, textureInfo);
      } catch (GladeFileNotFoundException &e) {        
        fileManager->getFileContents(TEXTURES_SUBDIRECTORY + "common" + texture_data_filename, textureInfoStream);
        CSVReader::read(textureInfoStream, textureInfo);
      }
     
      try {
        frameSize.x = atoi(textureInfo.at(0).at(0).c_str());
        frameSize.y = atoi(textureInfo.at(0).at(1).c_str());
      } catch (std::out_of_range &e) {
        log("Strug ResourceManger: Warning: Texture data is incorrect or was not provided, using zero values");
        frameSize.x = frameSize.y = 0;
      }
    } else {
      frameSize.x = frameSize.y = 0;
    }
    
    std::shared_ptr<Texture> result;
    
    
    Path foo = TEXTURES_SUBDIRECTORY + texture_pack_name + image_filename;
    
    try {
      result = Glade::ResourceManager::getTexture(TEXTURES_SUBDIRECTORY + texture_pack_name + image_filename, frameSize.x, frameSize.y);
    } catch (GladeFileNotFoundException &e) {
      try {
        result = Glade::ResourceManager::getTexture(TEXTURES_SUBDIRECTORY + "common" + image_filename, frameSize.x, frameSize.y);
      } catch (GladeFileNotFoundException &e) {
        if (use_nulltexture) {
          result = Glade::ResourceManager::getTexture(TEXTURES_SUBDIRECTORY + "notexture.png", 0, 0);
        } else {
          throw e;
        }
      }
    }
    
    return result;
  }
}


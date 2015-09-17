#pragma once

#include <vector>
#include <memory>
#include <string>

#include <glade/util/ResourceManager.h>
#include <glade/util/Path.h>
#include <glade/math/Vector.h>

struct LevelPackInfo;
class  Level;

namespace Strug
{
  class ResourceManager: public Glade::ResourceManager
  {
    private:
      std::vector<std::shared_ptr<LevelPackInfo> > levelPacks;
      const Path LEVELS_SUBDIRECTORY;
      const Path TEXTURES_SUBDIRECTORY;
      const Path LEVELS_DESC_FILE_PATH;
      
    public:
      ResourceManager(FileManager *file_manager);
      int getNumberOfLevelPacks();
      std::shared_ptr<LevelPackInfo> getLevelPackInfo(int index);
      std::shared_ptr<Level> getLevel(Path &filename);
      std::shared_ptr<Texture> getTexture(const std::string texture_pack_name, const Path &image_filename,
        const Path &texture_data_filename, bool use_nulltexture = true);
      
    private:    
      void loadLevelPacksInfo();
  };
}

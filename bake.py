import os
import baker

######################## Dependencies
baker.compileDependencies = [
os.path.join(os.environ['HATCHERY_SOURCES'], 'Gladen'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'lodepng'),
]

######################## Compiler
baker.compilerOutputDir = os.path.join(os.environ['HATCHERY_BUILDS'], 'strug', 'obj')

baker.compilerIncludes.extend([
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug'  , 'include'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'Gladen' , 'include'),
os.path.join(os.environ['HATCHERY_TOOLS']  , 'OpenGL' , 'include'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'sha1'),
])

baker.compilerSources = [
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'ResourceManager.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'Level.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'generator', 'WorldGenerator.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'generator', 'SimpleGenerator.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'generator', 'StraightPassage.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'generator', 'PassageTurn.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'generator', 'TCross.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'generator', 'XCross.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'generator', 'common.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'generator', 'MazeGenerator.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'states', 'GeneratorTest.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'states', 'MazeTest.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'states', 'CollisionTest.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'states', 'Play.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'blocks', 'Terrain.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'blocks', 'Block.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'blocks', 'Character.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'blocks', 'Player.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'src', 'main.cpp'),
os.path.join(os.environ['HATCHERY_SOURCES'], 'sha1',  'sha1.cpp'),
]

######################## Linker
baker.linkerOutputFilename = os.path.join(os.environ['HATCHERY_BUILDS'], 'strug', 'game.exe')

baker.linkerLibPath.extend([
os.path.join(os.environ['HATCHERY_BUILDS'], 'freetype-2.5.5')
])

baker.linkerObjects = [
'kernel32.lib',
'user32.lib',
'gdi32.lib',
'Opengl32.lib',
'DbgHelp.lib',
'freetype.lib',
os.path.join(os.environ['HATCHERY_BUILDS'], 'Gladen', '*.obj'),
os.path.join(os.environ['HATCHERY_BUILDS'], 'strug', 'obj', '*.obj'),
os.path.join(os.environ['HATCHERY_BUILDS'], 'lodepng', '*.obj'),
]

######################## Run
baker.parseCommandLine()
baker.sanitizeBakeInput()
baker.compile()
baker.linkExe()

######################## Copy FIXME baker copy function should copy file-by-file
baker.copyDir(os.path.join(os.environ['HATCHERY_SOURCES'], 'strug', 'assets'), os.path.join(os.environ['HATCHERY_BUILDS'], 'strug', 'assets'))
baker.copyDir(os.path.join(os.environ['HATCHERY_SOURCES'], 'Gladen', 'res'), os.path.join(os.environ['HATCHERY_BUILDS'], 'strug', 'assets'))

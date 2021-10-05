strug: main.o glade-game.o Play.o Area.o BlockyArea.o MonolithArea.o ResourceManager.o Block.o Character.o Player.o Terrain.o CollisionTest.o MazeTest.o MazeGenerator.o PassageTurn.o StraightPassage.o XCross.o TCross.o WorldGenerator.o SimpleGenerator.o common.o sha1.o builddir
	clang++ -O0 -g -o build/strug/strug -L../glade/build/glade -L../../vendor/freetype-2.10.1/objs/.libs/ -lfreetype -lglade -framework OpenGL -framework AppKit build/strug/main.o build/strug/glade-game.o build/strug/Area.o  build/strug/BlockyArea.o  build/strug/MonolithArea.o  build/strug/ResourceManager.o  build/strug/Block.o  build/strug/Character.o  build/strug/Player.o  build/strug/Terrain.o  build/strug/Play.o  build/strug/CollisionTest.o  build/strug/MazeTest.o  build/strug/WorldGenerator.o build/strug/MazeGenerator.o  build/strug/PassageTurn.o  build/strug/StraightPassage.o  build/strug/XCross.o  build/strug/TCross.o  build/strug/SimpleGenerator.o  build/strug/common.o  build/sha1/sha1.o

######################

sha1.o: builddir
	clang -O0 -g -c ../../vendor/sha1/sha1.c -o build/sha1/sha1.o

main.o: builddir
	clang -O0 -g -x objective-c -I ../glade/include -DGLADE_MACOS -DGL_SILENCE_DEPRECATION -c src/main.mm -o build/strug/main.o

glade-game.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/glade-game.cpp -o build/strug/glade-game.o

Area.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/Area.cpp -o build/strug/Area.o

BlockyArea.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/BlockyArea.cpp -o build/strug/BlockyArea.o

MonolithArea.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/MonolithArea.cpp -o build/strug/MonolithArea.o

ResourceManager.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/ResourceManager.cpp -o build/strug/ResourceManager.o

Block.o: builddir
	clang++ -O0 -g -std=c++11 -I include -I ../glade/include -DGLADE_MACOS -c src/blocks/Block.cpp -o build/strug/Block.o

Character.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/blocks/Character.cpp -o build/strug/Character.o

Player.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/blocks/Player.cpp -o build/strug/Player.o

Terrain.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/blocks/Terrain.cpp -o build/strug/Terrain.o

CollisionTest.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/states/CollisionTest.cpp -o build/strug/CollisionTest.o

Play.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/states/Play.cpp -o build/strug/Play.o

MazeTest.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/states/MazeTest.cpp -o build/strug/MazeTest.o

WorldGenerator.o: builddir
	clang++ -O0 -g -I ../../vendor/sha1 -I include -I ../glade/include -DGLADE_MACOS -c src/generator/WorldGenerator.cpp -o build/strug/WorldGenerator.o

MazeGenerator.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/generator/MazeGenerator.cpp -o build/strug/MazeGenerator.o

PassageTurn.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/generator/PassageTurn.cpp -o build/strug/PassageTurn.o

StraightPassage.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/generator/StraightPassage.cpp -o build/strug/StraightPassage.o

TCross.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/generator/TCross.cpp -o build/strug/TCross.o

XCross.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/generator/XCross.cpp -o build/strug/XCross.o

SimpleGenerator.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/generator/SimpleGenerator.cpp -o build/strug/SimpleGenerator.o

common.o: builddir
	clang++ -O0 -g -I include -I ../glade/include -DGLADE_MACOS -c src/generator/common.cpp -o build/strug/common.o

######################

builddir: 
	mkdir -p build
	mkdir -p build/strug
	mkdir -p build/sha1

clean:
	rm -rf build

<p float="left">
    <img src="resources/WhiteKing.png" width=40 />
    <img src="resources/WhiteQueen.png" width=40 />
    <img src="resources/WhiteRook.png" width=40 />
    <img src="resources/WhiteBishop.png" width=40 />
    <img src="resources/WhiteKnight.png" width=40 />
    <img src="resources/WhitePawn.png" width=40 />
    <img src="resources/BlackKing.png" width=40 />
    <img src="resources/BlackQueen.png" width=40 />
    <img src="resources/BlackRook.png" width=40 />
    <img src="resources/BlackBishop.png" width=40 />
    <img src="resources/BlackKnight.png" width=40 />
    <img src="resources/BlackPawn.png" width=40 />
</p>

Sample UI for fatpup chess library based on SDL2. It's currently a user - engine game using MicroMax chess engine. You always play white, make a move and wait for the engine to reply (can take up to 10 seconds).

## Get the code
fatpup chess engine is submodule'd, so you have to pull it separately:

    git clone https://github.com/witaly-iwanow/fatpup-ui-sdl.git
    cd fatpup-ui-sdl
    git submodule update --init --recursive

## Build (macOS)
Install SDL2 and SDL2 Image with Homebrew:

    brew install sdl2 sdl2_image

Throw it under CMake, make it and run:

    mkdir _cmake && cd _cmake
    cmake ..
    make && ./fatpup-ui

or if you need an Xcode project:

    cmake -GXcode ..
    Open fatpup-ui.xcodeproj

## Build (Linux)
Install SDL2 and SDL2 Image with apt/yum:

    sudo apt install libsdl2-dev libsdl2-image-dev

or

    sudo yum install SDL2-devel SDL2_image-devel

Throw it under CMake, make it and run:

    mkdir _cmake && cd _cmake
    cmake ..
    make && ./fatpup-ui

## Build (Windows)
Download the latest SDL2 and SDL2-image **Development** libraries (as of the time of writing [SDL 2.0.10](https://www.libsdl.org/release/SDL2-devel-2.0.10-VC.zip) and [SDL2 Image 2.0.5](https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.5-VC.zip)) and unzip them somewhere nice and tidy, i.e. no spaces in the path, C:\sdl for example.  

Throw it under CMake with paths to SDL2 and SDL2 Image set:

    mkdir _cmake && cd _cmake
    cmake -DSDL2_PATH=C:\sdl\SDL2-2.0.10 -DSDL2_IMAGE_PATH=C:\sdl\SDL2_image-2.0.5 ..

Open fatpup-ui.sln with Visual Studio set fatpup-ui as startup project. You're all set, feel free to hit Ctrl-F5 to see it running.

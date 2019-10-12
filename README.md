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

Sample UI for fatpup chess library based on SDL2

## How to build

Clone:

    git clone https://github.com/witaly-iwanow/fatpup-ui-sdl.git

Fetch submodules:

    cd fatpup-ui-sdl
    git submodule update --init --recursive

Install SDL2 (Linux):

    sudo apt install libsdl2-dev libsdl2-image-dev

Install SDL2 (macOS):

    brew install sdl2 sdl2_image

Install SDL2 (Windows):

    coming up shortly...

Build & run:

    mkdir _cmake
    cd _cmake
    cmake ..
    make
    ./fatpup-ui

# Ms. Pacman

Dot eating maze game with ghosts. For copyright reasons, no artwork is included in the repo, but the same 648x1488 png image used was easily found on [https://www.spriters-resource.com/](https://www.spriters-resource.com/). If one were to find that image and name it `assets/game.png`, you'd be well on your way to seeing the game as it develops (circa November 2025). There's also a `font.png` file that I've found, though don't remember where.

I started writing this game as [Ms. Packer Fan](https://github.com/sspencer/mspackerfan) in Go in July 2025, but lost focus.  Haven't programmed in C in decades, so decided to at least repimplement a little in C.  

## Building

The game is built with CMake and is configured for MacOS.

    cmake -B build
    cd build
    make

## Background

[The Pac-Man Dossier](https://www.gamedeveloper.com/design/the-pac-man-dossier)
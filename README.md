# Termtris

A Terminal Tetris clone written in C and without dependencies.

![termtris screenshot](/screenshots/termtris.png)

## Compiling

This project is built using `make` and a C compiler.

## Running

The game can be started using `./termtris`, which starts on level 0, or
`./termtris LEVEL` which allows you to choose any level between 0 and 99 to
start on.

## Rendering

This project uses Unicode box-drawing characters and Unicode arrow characters to
draw the border and input display. The blocks themselves are drawn using
terminal background modes. This means the game is only playable if the terminal
supports background colors and the colors red, green, yellow, blue, and magenta
are clearly distinguishable from the default background color.

## Controls

The game is controlled via regular tty keyboard input, with the following button
mappings:

- move left: A or left arrow key
- move right: D or right arrow key
- soft drop: S or down arrow key
- rotate clockwise: space bar
- exit the game: escape

Key repeat when a button is held down is assumed to be provided by the terminal.

## Scoring and Difficulty

The scoring and gravity tables are based on NES Tetris, however some details
are not implemented the same way, notably that soft-dropping does not give extra
points, and that the number of cleared lines before the next level is always 10.
The timing and RNG are also not accurate, so the game plays slightly differently
than the original.

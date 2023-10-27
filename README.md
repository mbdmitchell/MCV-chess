# MCV Chess

MCV Chess is a console-based chess program implemented using the Model-Controller-View (MCV) design pattern. This design pattern separates the chess program into three interconnected components to promote modularity and maintainability.

## Features

- Console-based chess game with text-based interface.
- Support for playing a standard game of chess.
- Model-Controller-View architecture for organized code structure.
- Utilizes the `fmt` library for improved text formatting.
- Implements the `gsl` (Guidelines Support Library) for safer and more reliable code.

## Building and Running

To build and run MCV Chess, follow these steps:

1. Clone or download this repository to your local machine.
2. Make sure you have the required dependencies (such as `fmt` and `gsl`) installed. (NB: this repository already includes these libraries)
3. Open your terminal and navigate to the project directory.
4. Compile the source code using your preferred C++20 compiler, ensuring that the required dependencies are linked (if needed).
5. Run the program.
   ```bash
   ./MCV-chess
   ```

## Usage

- Use the command-line interface to play chess, following standard chess rules.
- Make your moves by specifying the source and destination locations of your pieces in algebraic chess notation.

## Contributing

MCV Chess is a personal project, and I do not actively accept contributions. However, you are welcome to fork the repository and use the code as a starting point for your own projects or experiments. Feel free to explore, modify, and extend the code to suit your needs.

## Going forward

As is the nature of any project, there are near-infinite ways to expand it's scope. At time of writing, there are a number of smaller TODOs in the code. 
Some things I wish to not covered by TODOs are:
- Compile it for browser and have it run in the browser (using emscripten and WA)

#include "Sudoku.h"

int main(int argc, const char* argv[]) {
  auto puzzleOpt = SudokuPuzzle::load(argv[1]);
  if (!puzzleOpt) {
    return -1;
  }

  const auto& puzzle = *puzzleOpt;

  if (checkPuzzle(puzzle)) {
    std::cout << "Puzzle is OK" << std::endl;
  } else {
    std::cout << "Puzzle is not OK" << std::endl;
    return 0;
  }

  std::cout << puzzle << std::endl;
  PuzzleSolver(puzzle).solve();

  return 0;
}

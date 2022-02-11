#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct SudokuGraph {
  struct Cell {
    int row, col;
  };

  std::vector<std::vector<Cell>> g;
  int rows, cols, N;

  SudokuGraph(int rows, int cols) : rows(rows), cols(cols), N(rows * cols) {
    for (int r = 0; r < N; ++r) {
      for (int c = 0; c < N; ++c) {
        std::vector<Cell> cells;
        for (int x = 0; x < N; ++x) {
          if (x != c) cells.push_back({r, x});
        }
        for (int x = 0; x < N; ++x) {
          if (x != r) cells.push_back({x, c});
        }
        int startRow = r - r % rows;
        int startCol = c - c % cols;
        for (int x = 0; x < rows; ++x) {
          for (int y = 0; y < cols; ++y) {
            if (startRow + x != r || startCol + y != c)
              cells.push_back({startRow + x, startCol + y});
          }
        }
        g.push_back(cells);
      }
    }
  }

  const std::vector<Cell>& neighbors(int r, int c) const {
    assert(r >= 0 && r < N);
    assert(c >= 0 && c < N);
    return g[N * r + c];
  }
};

struct SudokuPuzzle {
  std::vector<int> cells;
  int rows, cols, N;
  SudokuGraph graph;

  SudokuPuzzle(int rows, int cols)
      : rows(rows), cols(cols), N(rows * cols), graph(rows, cols) {}

  static std::optional<SudokuPuzzle> load(const std::string& filename) {
    std::ifstream in(filename);

    int rows = -1;
    if (!(in >> rows)) {
      std::cerr << "Error reading rows" << std::endl;
      return {};
    }

    int cols = -1;
    if (!(in >> cols)) {
      std::cerr << "Error reading cols" << std::endl;
      return {};
    }

    SudokuPuzzle puzzle(rows, cols);
    int N = rows * cols;

    for (int r = 0; r < N; ++r) {
      for (int c = 0; c < N; ++c) {
        int val;
        if (in >> val) {
          puzzle.cells.push_back(val);
        } else {
          std::cerr << "Error reading: " << r << " " << c << std::endl;
          return {};
        }
      }
    }

    return puzzle;
  }

  int& at(int r, int c) {
    assert(r >= 0 && r < N);
    assert(c >= 0 && c < N);
    return cells[N * r + c];
  }

  const int& at(int r, int c) const {
    assert(r >= 0 && r < N);
    assert(c >= 0 && c < N);
    return cells[N * r + c];
  }

  template <class F>
  void forAllPossibleValuesOf(int row, int col, F f) {
    std::vector<bool> possible(N, true);
    for (const auto& [r, c] : graph.neighbors(row, col)) {
      int val = at(r, c);
      if (val > 0) {
        possible[val - 1] = false;
      }
    }

    for (int val = 1; val <= N; ++val) {
      if (possible[val - 1]) {
        f(val);
      }
    }
  }

  int countPossibleValuesOf(int row, int col) {
    int count = N;
    std::vector<bool> possible(N, true);
    for (const auto& [r, c] : graph.neighbors(row, col)) {
      int val = at(r, c);
      if (val > 0 && possible[val - 1]) {
        possible[val - 1] = false;
        --count;
      }
    }
    return count;
  }
};

inline std::ostream& operator<<(std::ostream& out, const SudokuPuzzle& puzzle) {
  out << puzzle.rows << " " << puzzle.cols << std::endl;
  const int N = puzzle.N;
  for (int r = 0; r < N; ++r) {
    for (int c = 0; c < N; ++c) {
      int val = puzzle.at(r, c);
      if (val < 10) out << " ";
      out << val << " ";
    }
    out << std::endl;
  }
  return out;
}

struct PuzzleSolver {
  const SudokuPuzzle puzzle;
  SudokuPuzzle candidate;
  int rows;
  int cols;
  int N;
  int nodes;
  int s;

  PuzzleSolver(const SudokuPuzzle& puzzle)
      : puzzle(puzzle),
        candidate(puzzle),
        rows(puzzle.rows),
        cols(puzzle.cols),
        N(rows * cols),
        nodes(0),
        s(0) {}

  void solve() {
    int lowerCount = N + 1;
    int row = -1;
    int col = -1;
    for (int r = 0; r < N; ++r) {
      for (int c = 0; c < N; ++c) {
        if (candidate.at(r, c) == 0) {
          int count = candidate.countPossibleValuesOf(r, c);
          if (count == 0) {
            ++nodes;
            return;
          }
          if (lowerCount > count) {
            lowerCount = count;
            row = r;
            col = c;
          }
        }
      }
    }

    if (row == -1 && col == -1) {
      std::cout << "Solution-" << ++s << ", nodes=" << nodes << std::endl;
      std::cout << candidate << std::endl;
      nodes = 0;
      return;
    }

    candidate.forAllPossibleValuesOf(row, col, [this, row, col](int val) {
      candidate.at(row, col) = val;
      solve();
      candidate.at(row, col) = 0;
    });
  }
};

inline bool checkPuzzle(const SudokuPuzzle& puzzle) {
  int N = puzzle.N;
  for (int row = 0; row < N; ++row) {
    for (int col = 0; col < N; ++col) {
      int val = puzzle.at(row, col);
      if (val == 0) continue;
      for (const auto& [r, c] : puzzle.graph.neighbors(row, col)) {
        int v = puzzle.at(r, c);
        if (v == val) {
          std::cout << "Same value in (" << r << ", " << c << ") and (" << row
                    << ", " << col << ")" << std::endl;
          return false;
        }
      }
    }
  }

  return true;
}

int board[20];

int canplace(int row, int column) {
  int i;
  for (i=1; i<=row-1; i=i+1) {
    if (board[i] == column)  {
      return 0;
    } else {
      if (abs(board[i]-column) == abs(i-row)) {
        return 0;
      }
    }
  }

  return 1;
}

int queen(int row, int n) {
  int count;
  count = 0;

  int column;
  for (column=1; column<=n; column=column+1) {
    if (canplace(row, column)) {
      board[row] = column;
      if (row == n) {
        count = count+1;
      } else {
        count = count + queen(row+1, n);
      }
    }
  }

  return count;
}

int main() {
  printf("%d", queen(1, 8));
}


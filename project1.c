#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

int indices[] = {-1, 0, +1};
static int n = 5;

// For seeding
double r()
{
    return (double)rand() / (double)RAND_MAX ;
}

bool adj_exists(int lattice[n][n], int row, int column)
{
  if ((row >= 0) && (row < n) && (column >= 0) && (column < n)) {
    if (lattice[row][column] == 1) {
      return true;
    }
  }
  return false;
}

void DFS(int lattice[n][n], int row, int column, int visited[n][n], int *cluster, bool *percolation, int r[n], int c[n]){
    if (visited[row][column]) {
      return;
    }

    // Check for percolation
    int new_index = 0;
    for (int i = 0; i < n; i++) {
      if (row == r[i]) {
        new_index = 0;
      } else {
        new_index = 1;
      }
    }
    if (new_index == 1) { r[row] = row; }
    new_index = false;
    for (int i = 0; i < n; i++) {
      if (column == c[i]) {
        new_index = false;
      } else {
        new_index = true;
      }
    }
    if (new_index) { c[column] = column; }
    int r_unique = 1;
    int c_unique = 1;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (i != j) {
          if (r[i] == r[j]) {
            //printf("%d", r[j]);
            r_unique = 0;
          }
          if (c[i] == c[j]) {
            c_unique = 0;
          }
        }
      }
    }
    if (c_unique == 1 || r_unique == 1) {
      *percolation = true;
    }


    visited[row][column] = 1;
    *cluster += 1;

    // DFS
    int xindex, yindex;
    for (int x = 0; x < 3; x++) {
      xindex = indices[x];
      for (int y = 0; y < 3; y++) {
        //(4-connected compenents)
        if ((x == 0 && y == 0) || (x == 2 && y == 2) || (x == 0 && y == 2) || (x == 2 && y == 0)) {
          continue;
        }
        yindex = indices[y];

        if (xindex == 0 && yindex == 0) {
          continue;
        }

        // CHECK Wrap-around adjacent nodes (4-connected compenents)
        if (xindex == -1 && row == 0) {
          if (adj_exists(lattice, n-1, column + yindex)) {
            DFS(lattice, n-1, column + yindex, visited, cluster, percolation, r, c);
          }
        } else if (yindex == -1 && column == 0) {
          if (adj_exists(lattice, row + xindex, n-1)) {
            DFS(lattice, row + xindex, n-1, visited, cluster, percolation, r, c);
          }
        } else {
          if (adj_exists(lattice, row + xindex, column + yindex)) {
            DFS(lattice, row + xindex, column + yindex, visited, cluster, percolation, r, c);
          }
        }
      }
    }
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "ERROR - USAGE: perc.exe (-s || -b) p\n");
        return 1;
    }
    else if ((strcmp("-s", argv[1]) != 0) && (strcmp("-b", argv[1]) != 0)) {
        fprintf(stderr, "ERROR - Unknown option: \"%s\" (-s or -b are accepted)\n", argv[1]);
        return 2;
    }

    float p = strtof(argv[2], NULL);
    printf("%.2f\n",p);

    if (!p || p <= 0 || p >= 1) {
        fprintf(stderr, "ERROR - p must be a number between 0 and 1\n");
        return 3;
    }

    srand ( time(NULL) );
    //int n = 64;
    int lattice[n][n];
    int visited[n][n];
    memset(visited, 0, sizeof(visited));

    // Make/seed lattice
    if (argv[1] == "-s") {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                if(r() >= p){
                    lattice[i][j] = 1;
                }
                else {
                    lattice[i][j] = 0;
                }
            }
        }
    }
    if (argv[1] == "-b") {
        for(int i = 0; i < n; i++) {
          for (int j = 0; j < n; j++) {
            //DO BOND SEEDING
          }
        }
    }

    // Cluster search via DFS
    int max_cluster = 0;
    int no_clusters = 0;
    int cluster = 0;
    bool percolation = false;
    int r[n];
    memset(r, 0, sizeof(r));
    int c[n];
    memset(c, 0, sizeof(c));
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if ((lattice[i][j] == 1) && (!visited[i][j])) {
          no_clusters += 1;
          cluster = 0;
          memset(r, 0, sizeof(r));
          memset(c, 0, sizeof(c));
          DFS(lattice, i, j, visited, &cluster, &percolation, r, c);
          if (cluster > max_cluster) { max_cluster = cluster;}
        }
      }
    }

    //PRINTING LATTICE
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%i ", lattice[i][j]);
        }
        printf("\n");
    }
    printf("%d\n", no_clusters);
    printf("%d\n", max_cluster);
    printf("%s\n", percolation ? "true" : "false");
}

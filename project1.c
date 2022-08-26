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

int DFS(int lattice[n][n], int row, int column, int visited[n][n], int cluster){
    if (visited[row][column]) {
      return cluster;
    }

    visited[row][column] = 1;
    cluster += 1;

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
            cluster = DFS(lattice, n-1, column + yindex, visited, cluster);
          }
        } else if (yindex == -1 && column == 0) {
          if (adj_exists(lattice, row + xindex, n-1)) {
            cluster = DFS(lattice, row + xindex, n-1, visited, cluster);
          }
        } else {
          if (adj_exists(lattice, row + xindex, column + yindex)) {
            cluster = DFS(lattice, row + xindex, column + yindex, visited, cluster);
          }
        }
      }
    }
    return cluster;
}

int main(int argc, char *argv[]){
    srand ( time(NULL) );
    double p = 0.5;
    //int n = 64;
    int lattice[n][n];
    int visited[n][n];
    memset(visited, 0, sizeof(visited));

    // Make/seed lattice
    int i, j;
    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            if(r() >= p){
                lattice[i][j] = 1;
            }
            else {
                lattice[i][j] = 0;
            }
        }
    }

    // Cluster search via DFS
    int max_cluster = 0;
    int no_clusters = 0;
    int cluster = 0;
    bool percolation = false;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if ((lattice[i][j] == 1) && (!visited[i][j])) {
          no_clusters += 1;
          cluster = 0;
          cluster = DFS(lattice, i, j, visited, cluster);
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
    printf("%d", max_cluster);
}

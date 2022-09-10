#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <omp.h>

int indices[] = {-1, 0, +1};
static int n = 256;

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

void DFS(int lattice[n][n], int row, int column, int visited[n][n], int beg_row, int end_row, int beg_col, int end_col, int *cluster, bool *percolation, int r[n], int c[n]){
    if (row < beg_row || row >= end_row || column < beg_col || column >= end_col || visited[row][column]) {
      return;
    }

    // Check for percolation
    // int new_index = 0;
    // for (int i = 0; i < n; i++) {
    //   if (row == r[i]) {
    //     new_index = 0;
    //   } else {
    //     new_index = 1;
    //   }
    // }
    // if (new_index == 1) { r[row] = row; }
    // new_index = false;
    // for (int i = 0; i < n; i++) {
    //   if (column == c[i]) {
    //     new_index = false;
    //   } else {
    //     new_index = true;
    //   }
    // }
    // if (new_index) { c[column] = column; }
    // int r_unique = 1;
    // int c_unique = 1;
    // for (int i = 0; i < n; i++) {
    //   for (int j = 0; j < n; j++) {
    //     if (i != j) {
    //       if (r[i] == r[j]) {
    //         //printf("%d", r[j]);
    //         r_unique = 0;
    //       }
    //       if (c[i] == c[j]) {
    //         c_unique = 0;
    //       }
    //     }
    //   }
    // }
    // if (c_unique == 1 || r_unique == 1) {
    //   *percolation = true;
    // }


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
            DFS(lattice, n-1, column + yindex, visited, beg_row, end_row, beg_col, end_col, cluster, percolation, r, c);
          }
        } else if (yindex == -1 && column == 0) {
          if (adj_exists(lattice, row + xindex, n-1)) {
            DFS(lattice, row + xindex, n-1, visited, beg_row, end_row, beg_col, end_col,  cluster, percolation, r, c);
          }
        } else {
          if (adj_exists(lattice, row + xindex, column + yindex)) {
            DFS(lattice, row + xindex, column + yindex, visited, beg_row, end_row, beg_col, end_col, cluster, percolation, r, c);
          }
        }
      }
    }
}

int main(int argc, char *argv[]){

    
    clock_t tic = clock();  

  
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

    // Seed lattice (Site)
    if (strcmp("-s", argv[1]) == 0) {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                double rand = r();
                //printf("%i %i %.2f\n", i, j, rand);

                if(rand >= p){
                    lattice[i][j] = 1;
                }
                else {
                    lattice[i][j] = 0;
                }
            }
        }
    }
    // Seed lattice (Bond)
    if (strcmp("-b", argv[1]) == 0) {
        for(int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i == 0 && j == 0) {
                    if (r() >= p) { lattice[i+1][j] = 1;
                    } else { lattice[i+1][j] = 0;}
                    if (r() >= p) { lattice[n-1][j] = 1;
                    } else { lattice[n-1][j] = 0;}
                    if (r() >= p) { lattice[i][j+1] = 1;
                    } else { lattice[i][j+1] = 0;}
                    if (r() >= p) { lattice[i][n-1] = 1;
                    } else { lattice[i][n-1] = 0;}
                } else if (i == 0 && ((n-1) > j > 0)) {
                    if (r() >= p) { lattice[i+1][j] = 1;
                    } else { lattice[i+1][j] = 0;}
                    if (r() >= p) { lattice[n-1][j] = 1;
                    } else { lattice[n-1][j] = 0;}
                    if (r() >= p) { lattice[i][j+1] = 1;
                    } else { lattice[i][j+1] = 0;}
                    if (r() >= p) { lattice[i][j-1] = 1;
                    } else { lattice[i][j-1] = 0;}
                } else if (((n-1) > i > 0) && j == 0) {
                    if (r() >= p) { lattice[i+1][j] = 1;
                    } else { lattice[i+1][j] = 0;}
                    if (r() >= p) { lattice[i-1][j] = 1;
                    } else { lattice[i-1][j] = 0;}
                    if (r() >= p) { lattice[i][j+1] = 1;
                    } else { lattice[i][j+1] = 0;}
                    if (r() >= p) { lattice[i][n-1] = 1;
                    } else { lattice[i][n-1] = 0;}
                } else if (i == (n-1) || j == (n-1)) {
                    continue;
                } else {
                    if (r() >= p) { lattice[i+1][j] = 1;
                    } else { lattice[i+1][j] = 0;}
                    if (r() >= p) { lattice[i-1][j] = 1;
                    } else { lattice[i-1][j] = 0;}
                    if (r() >= p) { lattice[i][j+1] = 1;
                    } else { lattice[i][j+1] = 0;}
                    if (r() >= p) { lattice[i][j-1] = 1;
                    } else { lattice[i][j-1] = 0;}
                }
            }
        }
        if (r() >= p) { lattice[n-1][n-1] = 1; } else { lattice[n-1][n-1] = 0;}
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

    /* Create number of threads according to lattice size - might add more threads if it increases
    * performance */
    int num_threads;
    if (n >= 256) {
        num_threads = 64;
    } else {
        num_threads = 16;
    }

    /* The rows and columns are split by the square root of the number of threads, splitting the lattice
    * into sections equal to the total number of threads */
    int sqroot = sqrt(num_threads);

    /* Each thread assigns itself the rows and columns (section) of the lattice they are handling */
    #pragma omp parallel num_threads(num_threads) shared(visited)
    {
      int beg_row = omp_get_thread_num()/sqroot * (n/sqrt(num_threads));
      int last_row = (omp_get_thread_num()/sqroot + 1) * (n/sqrt(num_threads));

      int beg_col = omp_get_thread_num()%sqroot * (n/sqrt(num_threads));
      int last_col;
      if ((omp_get_thread_num() + 1)%sqroot * (n/sqrt(num_threads)) == 0){
          last_col = n;
      } else {
          last_col = (omp_get_thread_num() + 1)%sqroot * (n/sqrt(num_threads));
      }

      #pragma omp critical
      {
          printf("%d beg row: %i end row: %i", omp_get_thread_num(), beg_row, last_row);
          printf("\t beg col: %i end col: %i\n", beg_col, last_col);
      }

      // Each thread performs recursive dfs in their respective lattice sections
      for (int i = beg_row; i < last_row; i++){
        for (int j = beg_col; j < last_col; j++) {
        if ((lattice[i][j] == 1) && (!visited[i][j])) {
          // no_clusters += 1;
          // cluster = 0;
          // memset(r, 0, sizeof(r));
          // memset(c, 0, sizeof(c));
          DFS(lattice, i, j, visited, beg_row, last_row, beg_col, last_col, &cluster, &percolation, r, c);
          if (cluster > max_cluster) { max_cluster = cluster;}
        }
      }
      }
    }

    // for (int i = 0; i < n; i++) {
    //   for (int j = 0; j < n; j++) {
    //     if ((lattice[i][j] == 1) && (!visited[i][j])) {
    //       no_clusters += 1;
    //       cluster = 0;
    //       memset(r, 0, sizeof(r));
    //       memset(c, 0, sizeof(c));
    //       DFS(lattice, i, j, visited, &cluster, &percolation, r, c);
    //       if (cluster > max_cluster) { max_cluster = cluster;}
    //     }
    //   }
    // }

    //PRINTING LATTICE
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         printf("%i ", lattice[i][j]);
    //     }
    //     printf("\n");
    // }
    printf("%d\n", no_clusters);
    printf("%d\n", max_cluster);
    printf("%s\n", percolation ? "true" : "false");

    
    clock_t toc = clock();
    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
}

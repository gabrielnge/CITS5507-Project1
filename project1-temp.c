#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <omp.h>

int total = 0;
int indices[] = {-1, 0, +1};
#define n 100

// Structure for keeping details such as its constituent locations and length for each cluster
typedef struct cluster {
  int id;
  int locs[n*n][2];
  int length;
} cluster;

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

void DFS(int lattice[n][n], int row, int column, int visited[n][n], int beg_row, int end_row, int beg_col, int end_col, cluster cluster_list[n], int id, bool *percolation, int r[n], int c[n]){
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

    // Add site to visited array
    visited[row][column] = id;
    total += 1;

    // Add site to corresponding cluster in list of clusters
    int next = cluster_list[id].length;
    //printf("\n-- %i  %i", id, next);

    cluster_list[id].locs[next][0] = row;
    cluster_list[id].locs[next][1] = column;
    //printf("\n found at: %i %i by %d ", cluster_list[id].locs[next][0], cluster_list[id].locs[next][1], omp_get_thread_num());
    cluster_list[id].length += 1;

    // Continue DFS recursively
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
            DFS(lattice, n-1, column + yindex, visited, beg_row, end_row, beg_col, end_col, cluster_list, id, percolation, r, c);
          }
        } else if (yindex == -1 && column == 0) {
          if (adj_exists(lattice, row + xindex, n-1)) {
            DFS(lattice, row + xindex, n-1, visited, beg_row, end_row, beg_col, end_col, cluster_list, id, percolation, r, c);
          }
        } else {
          if (adj_exists(lattice, row + xindex, column + yindex)) {
            DFS(lattice, row + xindex, column + yindex, visited, beg_row, end_row, beg_col, end_col, cluster_list, id, percolation, r, c);
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

    int (*lattice)[n] = malloc(sizeof(int[n][n]));
    int visited[n][n];
    memset(visited, 0, sizeof(visited)); // Initialise visisted array to 0's
    cluster (*cluster_list) = calloc(n*n,sizeof(int[n*n]));

    //memset(visited, 0, sizeof(visited));

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
    //int cluster = 0;
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

    int id = 1;
    /* Each thread assigns itself the rows and columns (section) of the lattice they are handling */
    #pragma omp parallel num_threads(num_threads)
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

      // #pragma omp critical
      // {
      //     printf("\n%d beg row: %i end row: %i", omp_get_thread_num(), beg_row, last_row);
      //     printf("\tbeg col: %i end col: %i", beg_col, last_col);
      // }



      // Each thread performs recursive dfs in their respective lattice sections
      int current_id = 0;
      for (int i = beg_row; i < last_row; i++){
        // printf("\nrow: %i", i);
        for (int j = beg_col; j < last_col; j++) {
          // printf("\ncol: %i", j);
          // printf("\nsite = %i", lattice[i][j]);
          // printf("\nvisited = %d", visited[i][j]);
        if ((lattice[i][j] == 1) && (!visited[i][j])) {

          #pragma omp critical
          {
            current_id = id;

            cluster_list[current_id].length = 0;
            //printf("\nid: %i", current_id);
            id += 1;
          }

          //no_clusters += 1;
          // memset(r, 0, sizeof(r));
          // memset(c, 0, sizeof(c));
          DFS(lattice, i, j, visited, beg_row, last_row, beg_col, last_col, cluster_list, current_id, &percolation, r, c);
          
          //if (cluster > max_cluster) { max_cluster = cluster;}
        }
        }
      } 
    }

    // printf("\ntotal sites %i", total);
    
    // printf("\n");
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         printf("%i ", visited[i][j]);
    //     }
    //     printf("\n");
    // }

    /* Parallel stitching of the lattice segments */
    int id1, id2, new_row, new_col, current_row;
     #pragma omp parallel num_threads(sqroot) private(id1, id2, new_row, new_col, current_row)
     //#pragma omp master
     { 

      // -- Stitch along the columns of neighbouring sections first --
       
      int beg_row = omp_get_thread_num()%sqroot * (n/sqrt(num_threads));
      int last_row;
      if ((omp_get_thread_num() + 1)%sqroot * (n/sqrt(num_threads)) == 0){
          last_row = n;
      } else {
          last_row = (omp_get_thread_num() + 1)%sqroot * (n/sqrt(num_threads));
      }

      //printf("\n%d rows: %i %i", omp_get_thread_num(), beg_row, last_row);

      /* The neighbouring columns in each section in each row are looped through */

      for (int section = 1; section < sqroot + 1; section++) {
        int last_col = section%sqroot * (n/sqrt(num_threads));
        int beg_col;
        if (last_col == 0){
          beg_col = n - 1;
        } else {
          beg_col = last_col - 1;
        }
        
        //printf("\n%d rows: %i %i cols: %i %i", omp_get_thread_num(), beg_row, last_row, beg_col, last_col);

        //printf("\ncol: %i %i", beg_col, last_col);
        
        int id1, id2;
        for (int current_row = beg_row; current_row < last_row; current_row++) {

          //printf("\n thread id: %d \t  row:  %i  [%i][%i]", omp_get_thread_num(), current_row, visited[current_row][beg_col], visited[current_row][last_col]);

          if (visited[current_row][beg_col] != 0 && visited[current_row][last_col] != 0 && visited[current_row][beg_col] != visited[current_row][last_col]) {

            //printf(" merge! ");

            // The variable holding the number of unique cluster ids is decremented 
            id -= 1;

            // The clusters are merged
            id1 = visited[current_row][beg_col];
            id2 = visited[current_row][last_col];

            //printf("\nAdded length: %i ",cluster_list[id2].length);

            for (int site = 0; site < cluster_list[id2].length; site++){

              int new_row = cluster_list[id2].locs[site][0];
              int new_col = cluster_list[id2].locs[site][1];
              //printf("\nnew loc: %i %i ", new_row, new_col);

              // Add the locations the id2 cluster to the locations of the id1 cluster
              cluster_list[id1].locs[cluster_list[id1].length][0] = new_row;
              cluster_list[id1].locs[cluster_list[id1].length][1] = new_col;

              
              // The cluster being merged into (made bigger) has its length increased by the length of the other cluster
              cluster_list[id1].length += 1;

              //printf("\nNew length: %i ",cluster_list[id1].length);

              // Replace id2 occurences with id1 on the visisted array
              visited[new_row][new_col] = id1;
            }
            
          }
          // printf("\n");
          // for (int i = 0; i < n; i++) {
          //     for (int j = 0; j < n; j++) {
          //         printf("%i ", visited[i][j]);
          //     }
          //     printf("\n");
          // }
        }
      }
     }
     
    // printf("\n column stitching complete\n");
    
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         printf("%i ", visited[i][j]);
    //     }
    //     printf("\n");
    // }

    int current_col;
    #pragma omp parallel num_threads(sqroot) private(id1, id2, new_row, new_col, current_col) shared(max_cluster)
      //#pragma omp master
      { 

       // -- Stitch along the columns of neighbouring sections first --
       
       int beg_col = omp_get_thread_num()%sqroot * (n/sqrt(num_threads));
       int last_col;
       if ((omp_get_thread_num() + 1)%sqroot * (n/sqrt(num_threads)) == 0){
           last_col = n;
       } else {
           last_col = (omp_get_thread_num() + 1)%sqroot * (n/sqrt(num_threads));
       }

      //printf("\n%d rows: %i %i", omp_get_thread_num(), beg_row, last_row);

      /* The neighbouring columns in each section in each row are looped through */

       for (int section = 1; section < sqroot + 1; section++) {
        int last_row = section%sqroot * (n/sqrt(num_threads));
        int beg_row;
        if (last_row == 0){
          beg_row = n - 1;
        } else {
          beg_row = last_row - 1;
        }
        
        //printf("\n%d rows: %i %i cols: %i %i", omp_get_thread_num(), beg_row, last_row, beg_col, last_col);

         //printf("\ncol: %i %i", beg_col, last_col);
        
        int id1, id2;
        for (int current_col = beg_col; current_col < last_col; current_col++) {

          //printf("\n thread id: %d \t  row:  %i  [%i][%i]", omp_get_thread_num(), current_col, visited[beg_row][current_col], visited[last_row][current_col]);

          if (visited[beg_row][current_col] != 0 && visited[last_row][current_col] != 0 && visited[beg_row][current_col] != visited[last_row][current_col]) {

            //printf(" merge! ");

             // The variable holding the number of unique cluster ids is decremented 
            id -= 1;

             // The clusters are merged
            id1 = visited[beg_row][current_col];
            id2 = visited[last_row][current_col];

             //printf("\nAdded length: %i ",cluster_list[id2].length);

             for (int site = 0; site < cluster_list[id2].length; site++){

               int new_row = cluster_list[id2].locs[site][0];
               int new_col = cluster_list[id2].locs[site][1];
               //printf("\nnew loc: %i %i ", new_row, new_col);

               // Add the locations the id2 cluster to the locations of the id1 cluster
               cluster_list[id1].locs[cluster_list[id1].length][0] = new_row;
               cluster_list[id1].locs[cluster_list[id1].length][1] = new_col;

              
               // The cluster being merged into (made bigger) has its length increased by the length of the other cluster
               cluster_list[id1].length += 1;

               // Replace id2 occurences with id1 on the visisted array
               visited[new_row][new_col] = id1;
             }

            if (cluster_list[id1].length > max_cluster){
              max_cluster = cluster_list[id1].length;
            }

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
    // printf("\n");
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         printf("%i ", visited[i][j]);
    //     }
    //     printf("\n");
    // }

    // for (int i = 0; i < n; i++){
    //   printf("\ncluster id: %i length: %i ", cluster_list[i].id, cluster_list[i].length);
    // }

    // for (int i = 0; i < id; i++){
    //   for (int j = 0; j < cluster_list[i].length; j++){
    //     printf("\ncluster id: %i row: %i col: %i", i, cluster_list[i].locs[j][0], cluster_list[i].locs[j][1]);
    //   }
    // }

    // printf("\nlength: %i", cluster_list[0].length);

    printf("\nnumber of cluster: %i", id);
    printf("\nlargest cluster: %i", max_cluster);
    printf("\n%s", percolation ? "true" : "false");

    
    clock_t toc = clock();
    printf("\nElapsed: %f seconds", (double)(toc - tic) / CLOCKS_PER_SEC);
}




#include <stdio.h>
#include <stdbool.h>

int main(){
    int array1[4][2] = {{1,2},{1,1},{0,3},{2,1}};
    int array2[4] = {0};
    int count = 0;
    bool unique;

    for (int i = 0; i < sizeof(array1)/sizeof(array1[0]); i++){

        unique = true;

        printf("\n %i", array1[i][1]);

        for (int j = 0; j < sizeof(array2)/sizeof(array2[0]); j++){

            if(array2[j] == array1[i][1]){
                unique = false;
            }
        }

        if(unique){

            printf("\nNew");
            array2[count] = array1[i][1];
            count += 1;

        }
        
    }

    printf("\nlength: %i", count);
    for (int j = 0; j < sizeof(array2)/sizeof(array2[0]); j++){
            printf("\n%i",array2[j]);
        }
}
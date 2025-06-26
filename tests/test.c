#include <stdio.h>
#define SZ 5


int main(){
    int arr[SZ] = {1, 2, 4, 5};
    int new[SZ + 1];

    int key = 55;
    int i = 0;
    while (arr[i] < key && i < SZ - 1){
        new[i] = arr[i];
        i++;
    }
    new[i] = key;
    while (i < SZ - 1){
        new[i + 1] = arr[i];
        i++;
    }

    for (int i = 0; i < SZ; i++)
        printf("%d ", new[i]);
}

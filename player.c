#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
int guess(int id, int i){ 
    // initialize random seed
    srand ((id + i) * 323);
    // generate guess between 1 and 1000
    int num = rand() % 1001;
    return num;
}
int main(int argc, char *argv[]){
    int player_id = atoi(argv[2]); // player id
    for (int i = 1; i <= 10; i++){ // iterate 10 rounds
        int g = guess(player_id, i);
        printf("%d %d\n", player_id, g);
        fflush(stdout);
    }
    exit(0);
}
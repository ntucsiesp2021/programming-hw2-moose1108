#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
int main(int argc, char **argv){
    int host_id, depth, lucky_number;
    char host_id_str[3], depth_str[2], lucky_number_str[5];
    int c;
    while ((c = getopt(argc, argv, "m:d:l:")) != -1){
        switch(c){
            case 'm':
                host_id = atoi(optarg);
                strcpy(host_id_str, optarg);
                break;
            case 'd':
                depth = atoi(optarg);
                strcpy(depth_str, optarg);
                break;
            case 'l':
                lucky_number = atoi(optarg);
                strcpy(lucky_number_str, optarg);
                break;
        }
    }
    if (depth == 0){ // root
        char fifo_file[25];
        snprintf(fifo_file, sizeof(fifo_file), "fifo_%d.tmp", host_id);
        // FILE *write_fifo = fopen(fifo_file, 'a+');
        int fd_r_c1[2], fd_c1_r[2], fd_r_c2[2], fd_c2_r[2]; // pipe talk to child
        pipe(fd_r_c1);
        pipe(fd_c1_r);
        pipe(fd_r_c2);
        pipe(fd_c2_r);
        pid_t pid;
        pid = fork();
        if (pid == 0){ // first child
            close(fd_r_c1[0]);
            dup2(fd_r_c1[1], 1);
            close(fd_c1_r[1]);
            dup2(fd_c1_r[0], 0);
            execlp("./host", "./host", "-d", "1", "-l", lucky_number_str, "-m", host_id_str, NULL);
        }
        pid = fork();
        if (pid == 0){ // second child
            close(fd_r_c2[0]);
            dup2(fd_r_c2[1], 1);
            close(fd_c2_r[1]);
            dup2(fd_c2_r[0], 0);
            execlp("./host", "./host", "-d", "1", "-l", lucky_number_str, "-m", host_id_str, NULL);
        }
        FILE *read_c1 = fdopen(fd_r_c1[0], "r");
        FILE *read_c2 = fdopen(fd_r_c2[0], "r");
        FILE *read_fifo = fopen(fifo_file, "r");
        FILE *write_fifo = fopen("fifo_0.tmp", "a+");
        while(1){
            int ID[9];
            fscanf(read_fifo, "%d %d %d %d %d %d %d %d", &ID[1], &ID[2], &ID[3], &ID[4], &ID[5], &ID[6], &ID[7], &ID[8]);
            // scanf("%d %d %d %d %d %d %d %d", &ID[1], &ID[2], &ID[3], &ID[4], &ID[5], &ID[6], &ID[7], &ID[8]);
            dprintf(fd_c1_r[1], "%d %d %d %d\n", ID[1], ID[2], ID[3], ID[4]);
            dprintf(fd_c2_r[1], "%d %d %d %d\n", ID[5], ID[6], ID[7], ID[8]);
            if (ID[1] == -1 && ID[2] == -1 && ID[3] == -1 && ID[4] == -1 && ID[5] == -1 && ID[6] == -1 && ID[7] == -1 && ID[8] == -1) // ultimate code
                break;
            int result[13] = {0};
            for (int i = 1; i <= 10; i++){
                int player1, player2, guess1, guess2;
                fscanf(read_c1, "%d %d", &player1, &guess1);
                fscanf(read_c2, "%d %d", &player2, &guess2);
                if (abs(lucky_number - guess1) < abs(lucky_number - guess2))
                    result[player1] += 10;
                else if (abs(lucky_number - guess1) > abs(lucky_number - guess2))
                    result[player2] += 10;
                else{
                    if (player1 < player2)
                        result[player1] += 10;
                    else
                        result[player2] += 10;
                }
            }
            fprintf(write_fifo, "%d\n", host_id);
            for (int i = 1; i <= 8 ; i++){
                fprintf(write_fifo, "%d %d\n", ID[i], result[ID[i]]);
                // printf("%d %d\n", i, result[i]);
            }
            fflush(write_fifo);
                
        }
        wait(NULL);
        wait(NULL);
        exit(0);
    }
    else if (depth == 1){ // child
        int fd_c_g1[2], fd_g1_c[2], fd_c_g2[2], fd_g2_c[2]; // pipe talk to child
        pipe(fd_c_g1);
        pipe(fd_g1_c);
        pipe(fd_c_g2);
        pipe(fd_g2_c);
        pid_t pid;
        if (pid = fork() == 0){ // first child
            dup2(fd_g1_c[0], 0);
            dup2(fd_c_g1[1], 1);
            close(fd_c_g1[0]);
            close(fd_c_g2[0]);
            close(fd_c_g2[1]);
            close(fd_g1_c[1]);
            close(fd_g2_c[0]);
            close(fd_g2_c[1]);
            execlp("./host", "./host", "-d", "2", "-l", lucky_number_str, "-m", host_id_str, NULL);
        }
        if (pid = fork() == 0){ // second child
            dup2(fd_c_g2[1], 1);
            dup2(fd_g2_c[0], 0);
            close(fd_c_g2[0]);
            close(fd_g1_c[0]);
            close(fd_g1_c[1]);
            close(fd_g2_c[1]);
            close(fd_c_g1[0]);
            close(fd_c_g1[1]);
            execlp("./host", "./host", "-d", "2", "-l", lucky_number_str, "-m", host_id_str, NULL);
        }
        FILE *read_g1 = fdopen(fd_c_g1[0], "r");
        FILE *read_g2 = fdopen(fd_c_g2[0], "r");
        close(fd_g1_c[0]);
        close(fd_c_g1[1]);
        close(fd_c_g2[1]);
        close(fd_g2_c[0]);
        while(1){
            int ID[5];
            scanf("%d %d %d %d", &ID[1], &ID[2], &ID[3], &ID[4]);
            dprintf(fd_g1_c[1], "%d %d\n", ID[1], ID[2]);
            dprintf(fd_g2_c[1], "%d %d\n", ID[3], ID[4]);
            if (ID[1] == -1 && ID[2] == -1 && ID[3] == -1 && ID[4] == -1)
                break;
            int player1, player2, guess1, guess2, player, guess;
            for (int i = 1; i <= 10; i++){
                fscanf(read_g1, "%d %d", &player1, &guess1);
                fscanf(read_g2, "%d %d", &player2, &guess2);
                if (abs(lucky_number - guess1) < abs(lucky_number - guess2)){
                    player = player1;
                    guess = guess1;
                }
                else if (abs(lucky_number - guess1) > abs(lucky_number - guess2)){
                    player = player2;
                    guess = guess2;
                }
                else{
                    if (player1 < player2){
                        player = player1;
                        guess = guess1;
                    }
                    else{
                        player = player2;
                        guess = guess2;
                    }
                }
                printf("%d %d\n",player,guess);
                fflush(stdout);
            }
        }
        wait(NULL);
        wait(NULL);
        exit(0);
    }
    else if (depth == 2){ // grandchild
        int fd_g_p1[2], fd_p1_g[2], fd_g_p2[2], fd_p2_g[2]; // pipe talk to child
        char player1_buf[5] = {'\0'};
        char player2_buf[5] = {'\0'};
        int ID[3];
        pid_t pid;
        while(1){
            pipe(fd_g_p1);
            pipe(fd_p1_g);
            pipe(fd_g_p2);
            pipe(fd_p2_g);
            scanf("%d %d", &ID[1], &ID[2]);
            if (ID[1] == -1 && ID[2] == -1)
                break;
            if (pid = fork() == 0){ // first child
                close(fd_g_p1[0]);
                close(fd_p1_g[0]);
                close(fd_p1_g[1]);
                dup2(fd_g_p1[1], 1);
                sprintf(player1_buf, "%d", ID[1]);
                execlp("./player", "./player", "-n", player1_buf, NULL);
            }
            if (pid = fork() == 0){ // second child
                close(fd_g_p2[0]);
                close(fd_p2_g[0]);
                close(fd_p2_g[1]);
                dup2(fd_g_p2[1], 1);
                sprintf(player2_buf, "%d", ID[2]);
                execlp("./player", "./player", "-n", player2_buf, NULL);
            }
            FILE *leaf_p1 = fdopen(fd_g_p1[0], "r");
            FILE *leaf_p2 = fdopen(fd_g_p2[0], "r");
            close(fd_g_p1[1]);
            close(fd_g_p2[1]);
            int player1, player2, guess1, guess2, player, guess;
            for (int i = 1; i <= 10; i++){
                fscanf(leaf_p1, "%d %d", &player1, &guess1);
                fscanf(leaf_p2, "%d %d", &player2, &guess2);
                if (abs(lucky_number - guess1) < abs(lucky_number - guess2)){
                    player = player1;
                    guess = guess1;
                }
                else if (abs(lucky_number - guess1) > abs(lucky_number - guess2)){
                    player = player2;
                    guess = guess2;
                }
                else{
                    if (player1 < player2){
                        player = player1;
                        guess = guess1;
                    }
                    else{
                        player = player2;
                        guess = guess2;
                    }
                }
                printf("%d %d\n", player, guess);
                fflush(stdout);
            }
            wait(NULL);
            wait(NULL);
        }
        exit(0);
    }
}
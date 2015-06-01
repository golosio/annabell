#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TRAIN_VIRTUAL_H
#define TRAIN_VIRTUAL_H

extern int door[25][4];
extern char room_name[25][15];
extern int room_array[5][5];
extern char direction[4][10];

extern int trans_dir_arr[8];
extern int dest_dir_arr[8];
extern int trans_room_arr[8];
extern int trans_room_arr_alt[8];
extern int dest_room_arr[8];
extern int close_pos1[8];
extern int close_dir1[8];
extern int close_pos2[8];
extern int close_dir2[8];
extern int close_pos1_alt[8];
extern int close_dir1_alt[8];
extern int close_pos2_alt[8];
extern int close_dir2_alt[8];
extern int train_index[8];
extern int reach_list[13];
extern int NObj;
extern int object_pos[8];
extern char object_name_arr[8][15];
extern int person_pos;
extern char person_name[];
extern char you_see_arr[25][30];
extern int you_see_flag;
extern char house_room_name[11][15];
extern int house_room_pos[11];

int setup_doors();
int setup_names();
int display(int pos);
int fdisplay(FILE *fp, int pos);
int move(int pos, char *command);
int setup_train();
int setup_house();
int room_pos(char *room_nm);

#endif

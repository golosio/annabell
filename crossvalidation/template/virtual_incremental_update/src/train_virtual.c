#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "train_virtual.h"

int door[25][4];
char room_name[25][15];
int room_array[5][5];
char direction[4][10]={"east", "north", "west", "south"};

int trans_dir_arr[8] = {0, 1, 2, 3, 0, 1, 2, 3};
int dest_dir_arr[8] =  {1, 2, 3, 0, 0, 1, 2, 3};
int trans_room_arr[8] = {13, 7, 11, 17, 13, 7, 11, 17};
int trans_room_arr_alt[8] = {7, 11, 17, 13, -1, -1, -1, -1};
int dest_room_arr[8] =  {8, 6, 16, 18, 14, 2, 10, 22};
int close_pos1[8]={7,  6, 16, 13, -1, -1, -1, -1};
int close_dir1[8]={0,  3,  0,  3, -1, -1, -1, -1};
int close_pos2[8]={8, 11, 17, 18, -1, -1, -1, -1};
int close_dir2[8]={2,  1,  2,  1, -1, -1, -1, -1};
int close_pos1_alt[8]={ 8, 7, 11, 17, -1, -1, -1, -1};
int close_dir1_alt[8]={ 3, 2,  3,  0, -1, -1, -1, -1};
int close_pos2_alt[8]={13, 6, 16, 18, -1, -1, -1, -1};
int close_dir2_alt[8]={ 1, 0,  1,  2, -1, -1, -1, -1};
int train_index[8];
int reach_list[13] = {2, 6, 7, 8, 10, 11, 12, 13, 14, 16, 17, 18, 22};
int NObj = 8;
int object_pos[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
char object_name_arr[8][15] = {"object1", "object2", "object3", "object4",
			   "object5", "object6", "object7", "object8"};
int person_pos = -1;
char person_name[] = "John";
char you_see_arr[25][30];
int you_see_flag = 0;

char house_room_name[11][15] = {
  "kitchen", "library", "bathroom", "veranda", "living_room", "hall",
  "corridor", "bedroom", "garden", "studio", "street"
};
int house_room_pos[11] = {6, 7, 8, 10, 11, 12, 13, 14, 17, 18, 22};

int setup_doors()
{
  int i, ix, iy;
  
  for (iy=0; iy<5; iy++) {
    for (ix=0; ix<5; ix++) {
      i = 5*iy+ix;
      room_array[iy][ix] = i;
    }
  }

  for (iy=0; iy<5; iy++) {
    for (ix=0; ix<5; ix++) {
      i = 5*iy+ix;
      if (ix<4) door[i][0] = i+1; // east
      else door[i][0] = -1;

      if (iy>0) door[i][1] = i-5; // north
      else door[i][1] = -1;

      if (ix>0) door[i][2] = i-1; // west
      else door[i][2] = -1;

      if (iy<4) door[i][3] = i+5; // south
      else door[i][3] = -1;

    }
  }

  return 0;
}

int setup_names()
{
  int i;

  for (i=0; i<25; i++) {
    sprintf(room_name[i], "room_%d", i);
  }

  return 0;
}

int display(int pos)
{
  int k;
  int pos1, dpos[4] = {1, -5, -1, 5};


  printf("you are in the %s\n", room_name[pos]);

  for (k=0; k<4; k++) {
    if (door[pos][k]>=0) {
      pos1 = pos + dpos[k];
      printf("to the %s there is the %s\n", direction[k], room_name[pos1]);
    }
  }

  if (you_see_flag!=0) printf("you see %s\n", you_see_arr[pos]);
  if (pos==person_pos)   printf("%s is here\n", person_name);
  printf("? what do you want to do\n");

  return 0;
}

int fdisplay(FILE *fp, int pos)
{
  int k;
  int pos1, dpos[4] = {1, -5, -1, 5};


  fprintf(fp, "you are in the %s\n", room_name[pos]);

  for (k=0; k<4; k++) {
    if (door[pos][k]>=0) {
      pos1 = pos + dpos[k];
      fprintf(fp, "to the %s there is the %s\n", direction[k], room_name[pos1]);
    }
  }

  if (you_see_flag!=0) fprintf(fp, "you see %s\n", you_see_arr[pos]);
  if (pos==person_pos)   fprintf(fp, "%s is here\n", person_name);
  fprintf(fp, "? what do you want to do\n");

  return 0;
}

int move(int pos, char *command)
{
  int pos1;

  pos1 = pos;
  if ((strcmp(command, "east")==0 || strcmp(command, "e")==0) &&
      door[pos][0]>=0) pos1 = pos+1; // east

  else if ((strcmp(command, "north")==0 || strcmp(command, "n")==0) &&
	   door[pos][1]>=0) pos1 = pos-5; // north

  else if ((strcmp(command, "west")==0 || strcmp(command, "w")==0) &&
	   door[pos][2]>=0) pos1 = pos-1; // west

  else if ((strcmp(command, "south")==0 || strcmp(command, "s")==0) &&
	   door[pos][3]>=0) pos1 = pos+5; // south

  else printf("error!\n");

  return pos1;
}

int setup_train()
{
  int i, j, k, dir_tmp, iter;

  for (i=0; i<25; i++) {
    strcpy(you_see_arr[i],"nothing");
  }

  for (i=0; i<4; i++) {
    if (rand()%2==1) {
      dir_tmp = trans_dir_arr[i];
      trans_dir_arr[i] = dest_dir_arr[i];
      dest_dir_arr[i] = dir_tmp;

      trans_room_arr[i] = trans_room_arr_alt[i];
      close_pos1[i]=close_pos1_alt[i];
      close_dir1[i]=close_dir1_alt[i];
      close_pos2[i]=close_pos2_alt[i];
      close_dir2[i]=close_dir2_alt[i];
    }
    door[close_pos1[i]][close_dir1[i]]=-1;
    door[close_pos2[i]][close_dir2[i]]=-1;
  }

  for (i=0; i<8; i++) train_index[i] = i;

  for (iter=0; iter<10000; iter++) {
    //swap
    i = rand()%8;
    j = rand()%8;
    k = train_index[i];
    train_index[i] = train_index[j];
    train_index[j] = k;
  }
   
  return 0;
}

int setup_house()
{
  int i, pos;

  door[6][0] = -1;
  door[6][1] = -1;
  door[6][2] = -1;
  door[7][0] = -1;
  door[7][1] = -1;
  door[7][2] = -1;
  door[8][0] = -1;
  door[8][1] = -1;
  door[8][2] = -1;
  door[10][1] = -1;
  door[10][2] = -1;
  door[10][3] = -1;
  door[11][3] = -1;
  door[14][0] = -1;
  door[14][1] = -1;
  door[14][3] = -1;
  door[17][0] = -1;
  door[17][2] = -1;
  door[18][0] = -1;
  door[18][2] = -1;
  door[18][3] = -1;
  door[22][0] = -1;
  door[22][2] = -1;
  door[22][3] = -1;

  for (i=0; i<11; i++) {
    pos = house_room_pos[i];
    strcpy(room_name[pos], house_room_name[i]);
  }

  return 0;
}

int room_pos(char *room_nm)
{
  int i, pos;

  pos=-1;
  for (i=0; i<25; i++) {
    if (strcmp(room_nm, room_name[i])==0) {
      pos = i;
      break;
    }
  }
  if (pos<0) {
    printf("Error: room name not found!\n");
    exit(0);
  }
  
  return pos;    
}


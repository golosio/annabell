#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "train_virtual.h"

int main(int argc, char *argv[])
{
  int err, round_num;
  long seed[4] = {34256125, 90834875, 87234642, 91243690};
  int i, j, pos, start, dest, trans, in_flag, trans_dir, dest_dir;
  //char command[100];
  int NTrain;// = 8;

  err=0;
  if (argc!=3) err = 1;
  else {
    sscanf(argv[1], "%d", &round_num);
    sscanf(argv[2], "%d", &NTrain);
    //printf("Round-number: %d\n", round_num);
    //printf("Number of training examples: %d\n", NTrain);
    if (round_num<1 || round_num>4 || NTrain<1 || NTrain>8) err = 1;
  }
  if (err!=0) {
    printf("Usage: %s round-number(1-4) number-of-training-examples(1-8)\n",
	   argv[0]);
    exit(0);
  }
  srand(seed[round_num]);

  setup_doors();
  setup_names();
  setup_train();

  //for(i=0; i<13; i++) {
  //  printf("\n");
  //  display(reach_list[i]);
  //}
  for(i=0; i<25; i++) {
    printf("\n");
    display(i);
  }
  printf("\n");

  for(i=0; i<NTrain; i++) {
    j = train_index[i];

    start = 12;
    dest = dest_room_arr[j];
    trans = trans_room_arr[j];
    trans_dir = trans_dir_arr[j];
    dest_dir = dest_dir_arr[j];

    display(start);
    printf("go to the %s\n", room_name[dest]);
    printf(".wg %s\n", room_name[dest]);
    printf(".push_goal\n");
    printf(".sctx you are in the %s\n", room_name[start]);
    printf(".wg you are in the %s\n", room_name[start]);
    printf(".sctx ? what do you want to do\n");
    printf(".wg\n");
    in_flag = 1;
    pos = door[start][0];
    if (pos>=0) {
      printf(".sctx to the east there is the %s\n", room_name[pos]);
      printf(".wg %s\n", room_name[pos]);
      printf(".ph you are in the %s\n", room_name[pos]);
      if (pos!=trans) {
	printf(".sctx ? what do you want to do\n");
	printf(".rip\n");
	printf(".sctx to the east there is the %s\n", room_name[pos]);
	printf(".wg\n");
      }
      else in_flag = 0;
    }
    pos = door[start][1];
    if (in_flag==1 && pos>=0) {
      printf(".ph to the north there is the %s\n", room_name[pos]);
      printf(".wg %s\n", room_name[pos]);
      printf(".ph you are in the %s\n", room_name[pos]);
      if (pos!=trans) {
	printf(".sctx ? what do you want to do\n");
	printf(".rip\n");
	printf(".sctx to the north there is the %s\n", room_name[pos]);
	printf(".wg\n");
      }
      else in_flag = 0;
    }
    pos = door[start][2];
    if (in_flag==1 && pos>=0) {
      printf(".ph to the west there is the %s\n", room_name[pos]);
      printf(".wg %s\n", room_name[pos]);
      printf(".ph you are in the %s\n", room_name[pos]);
      if (pos!=trans) {
	printf(".sctx ? what do you want to do\n");
	printf(".rip\n");
	printf(".sctx to the west there is the %s\n", room_name[pos]);
	printf(".wg\n");
      }
      else in_flag = 0;
    }
    pos = door[start][3];
    if (in_flag==1 && pos>=0) {
      printf(".ph to the south there is the %s\n", room_name[pos]);
      printf(".wg %s\n", room_name[pos]);
      printf(".ph you are in the %s\n", room_name[pos]);
      if (pos!=trans) {
	printf("Error!\n");
	exit(0);
      }
    }
    printf(".sctx to the %s there is the %s\n", direction[dest_dir],
	   room_name[dest]);
    printf(".push_goal\n");
    printf(".rip\n");
    printf(".sctx to the %s there is the %s\n",  direction[trans_dir],
	   room_name[trans]);
    printf(".wg %s\n",  direction[trans_dir]);
    printf(".drop_goal\n");
    printf(".rw\n");
    printf("\n");
    display(trans);
    printf(".sctx you are in the %s\n", room_name[trans]);
    printf(".wg you are in the %s\n", room_name[trans]);
    printf(".sctx to the %s there is the %s\n", direction[dest_dir],
	   room_name[dest]);
    printf(".wg %s\n", direction[dest_dir]);
    printf(".drop_goal\n");
    printf(".rw\n");
    printf("\n");
    printf(".save links_goto_%d_%d.dat\n", round_num, i+1);
  }

  return 0;
}


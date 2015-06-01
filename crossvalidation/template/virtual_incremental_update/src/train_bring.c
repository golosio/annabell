#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "train_virtual.h"

int main(int argc, char *argv[])
{
  int err, round_num;
  long seed[4] = {34256134, 90834483, 87234692, 91243633};
  int i, j, pos, start, dest, trans, in_flag, trans_dir, dest_dir;
  //char command[100];
  int NTrain; // = 8;
  char object_name[15];
  int first_room_flag;

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

  person_pos = start = 12;
  you_see_flag = 1;
  for (i=0; i<NObj; i++) {
    object_pos[i] = dest_room_arr[i];
    sprintf(you_see_arr[object_pos[i]], "some %s -s", object_name_arr[i]);
  }

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

    dest = dest_room_arr[j];
    strcpy(object_name, object_name_arr[j]);
    trans = trans_room_arr[j];
    trans_dir = trans_dir_arr[j];
    dest_dir = dest_dir_arr[j];

    display(start);
    printf("bring an %s to %s\n", object_name, person_name);
    printf(".wg %s\n", object_name);
    printf(".push_goal\n");
    printf(".rip\n");
    printf(".sctx ? what do you want to do\n");
    printf(".ggp\n");
    printf(".wg %s\n", object_name);
    printf(".ph you see some %s -s\n", object_name);
    printf(".sctx you are in the %s\n", room_name[dest]);
    printf(".wg %s\n", room_name[dest]);
    printf(".push_goal\n");
    printf(".rip\n");
    printf(".sctx you are in the %s\n", room_name[start]);
    printf(".wg you are in the %s\n", room_name[start]);
    //printf(".sctx ? what do you want to do\n");
    printf(".sctx you see %s\n", you_see_arr[start]);
    printf(".wg\n");
    in_flag = 1;
    first_room_flag = 1;
    pos = door[start][0];
    if (pos>=0) {
      first_room_flag = 0;
      printf(".sctx to the east there is the %s\n", room_name[pos]);
      printf(".wg %s\n", room_name[pos]);
      printf(".ph you are in the %s\n", room_name[pos]);
      if (pos!=trans) {
	//printf(".sctx ? what do you want to do\n");
	printf(".sctx you see %s\n", you_see_arr[pos]);
	printf(".rip\n");
	printf(".sctx to the east there is the %s\n", room_name[pos]);
	printf(".wg\n");
      }
      else in_flag = 0;
    }
    pos = door[start][1];
    if (in_flag==1 && pos>=0) {
      if (first_room_flag!=0) printf(".sctx ");
      else printf(".ph ");
      first_room_flag = 0;
      printf("to the north there is the %s\n", room_name[pos]);
      printf(".wg %s\n", room_name[pos]);
      printf(".ph you are in the %s\n", room_name[pos]);
      if (pos!=trans) {
	//printf(".sctx ? what do you want to do\n");
	printf(".sctx you see %s\n", you_see_arr[pos]);
	printf(".rip\n");
	printf(".sctx to the north there is the %s\n", room_name[pos]);
	printf(".wg\n");
      }
      else in_flag = 0;
    }
    pos = door[start][2];
    if (in_flag==1 && pos>=0) {
      if (first_room_flag!=0) printf(".sctx ");
      else printf(".ph ");
      first_room_flag = 0;
      printf("to the west there is the %s\n", room_name[pos]);
      printf(".wg %s\n", room_name[pos]);
      printf(".ph you are in the %s\n", room_name[pos]);
      if (pos!=trans) {
	//printf(".sctx ? what do you want to do\n");
	printf(".sctx you see %s\n", you_see_arr[pos]);
	printf(".rip\n");
	printf(".sctx to the west there is the %s\n", room_name[pos]);
	printf(".wg\n");
      }
      else in_flag = 0;
    }
    pos = door[start][3];
    if (in_flag==1 && pos>=0) {
      if (first_room_flag!=0) printf(".sctx ");
      else printf(".ph ");
      first_room_flag = 0;
      printf("to the south there is the %s\n", room_name[pos]);
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
    //printf(".drop_goal\n");
    printf(".rw\n");
    printf("\n");

    display(dest);
    printf(".sctx you are in the %s\n", room_name[dest]);
    printf(".drop_goal\n");
    printf(".sctx you see %s\n", you_see_arr[dest]);
    printf(".ggp\n");
    printf(".wg bring\n");
    printf(".ph to bring an object to a person\n");
    printf(".sctx take the object\n");
    printf(".wg take the\n");
    printf(".prw\n");
    printf(".ggp\n");
    printf(".wg %s\n", object_name);
    printf(".rw\n");
    printf("you take an %s\n", object_name);
    printf(".ggp\n");
    printf(".wg %s\n", person_name);
    printf(".drop_goal\n");
    printf(".push_goal\n");
    printf(".rip\n");
    printf(".sctx ? what do you want to do\n");
    printf(".ggp\n");
    printf(".wg %s\n", person_name);
    printf(".ph %s is here\n", person_name);

    printf(".sctx you are in the %s\n", room_name[start]);
    printf(".wg %s\n", room_name[start]);
    printf(".push_goal\n");
    printf(".rip\n");
    printf(".sctx you are in the %s\n", room_name[dest]);
    printf(".wg you are in the %s\n", room_name[dest]);
    //printf(".sctx ? what do you want to do\n");
    printf(".sctx you see %s\n", you_see_arr[dest]);
    printf(".wg\n");
    in_flag = 1;
    first_room_flag = 1;
    pos = door[dest][0];
    if (pos>=0) {
      first_room_flag = 0;
      printf(".sctx to the east there is the %s\n", room_name[pos]);
      printf(".wg %s\n", room_name[pos]);
      printf(".ph you are in the %s\n", room_name[pos]);
      if (pos!=trans) {
	//printf(".sctx ? what do you want to do\n");
	printf(".sctx you see %s\n", you_see_arr[pos]);
	printf(".rip\n");
	printf(".sctx to the east there is the %s\n", room_name[pos]);
	printf(".wg\n");
      }
      else in_flag = 0;
    }
    pos = door[dest][1];
    if (in_flag==1 && pos>=0) {
      if (first_room_flag!=0) printf(".sctx ");
      else printf(".ph ");
      first_room_flag = 0;
      printf("to the north there is the %s\n", room_name[pos]);
      printf(".wg %s\n", room_name[pos]);
      printf(".ph you are in the %s\n", room_name[pos]);
      if (pos!=trans) {
	//printf(".sctx ? what do you want to do\n");
	printf(".sctx you see %s\n", you_see_arr[pos]);
	printf(".rip\n");
	printf(".sctx to the north there is the %s\n", room_name[pos]);
	printf(".wg\n");
      }
      else in_flag = 0;
    }
    pos = door[dest][2];
    if (in_flag==1 && pos>=0) {
      if (first_room_flag!=0) printf(".sctx ");
      else printf(".ph ");
      first_room_flag = 0;
      printf("to the west there is the %s\n", room_name[pos]);
      printf(".wg %s\n", room_name[pos]);
      printf(".ph you are in the %s\n", room_name[pos]);
      if (pos!=trans) {
	//printf(".sctx ? what do you want to do\n");
	printf(".sctx you see %s\n", you_see_arr[pos]);
	printf(".rip\n");
	printf(".sctx to the west there is the %s\n", room_name[pos]);
	printf(".wg\n");
      }
      else in_flag = 0;
    }
    pos = door[dest][3];
    if (in_flag==1 && pos>=0) {
      if (first_room_flag!=0) printf(".sctx ");
      else printf(".ph ");
      first_room_flag = 0;
      printf("to the south there is the %s\n", room_name[pos]);
      printf(".wg %s\n", room_name[pos]);
      printf(".ph you are in the %s\n", room_name[pos]);
      if (pos!=trans) {
	printf("Error!\n");
	exit(0);
      }
    }
    printf(".sctx to the %s there is the %s\n", direction[(2+trans_dir)%4],
	   room_name[start]);
    printf(".push_goal\n");
    printf(".rip\n");
    printf(".sctx to the %s there is the %s\n",  direction[(2+dest_dir)%4],
	   room_name[trans]);
    printf(".wg %s\n",  direction[(2+dest_dir)%4]);
    printf(".drop_goal\n");
    printf(".rw\n");
    printf("\n");
    display(trans);
    printf(".sctx you are in the %s\n", room_name[trans]);
    printf(".wg you are in the %s\n", room_name[trans]);
    printf(".sctx to the %s there is the %s\n", direction[(2+trans_dir)%4],
	   room_name[start]);
    printf(".wg %s\n", direction[(2+trans_dir)%4]);
    //printf(".drop_goal\n");
    printf(".rw\n");
    printf("\n");

    display(start);
    printf(".sctx you are in the %s\n", room_name[start]);
    printf(".drop_goal\n");
    printf(".sctx %s is here\n", person_name);
    printf(".ggp\n");
    printf(".wg bring\n");
    printf(".ph to bring an object to a person\n");
    printf(".sctx and give the object to the person\n");
    printf(".wg give the\n");
    printf(".prw\n");
    printf(".ggp\n");
    printf(".wg %s to %s\n", object_name, person_name);
    printf(".rw\n");
    printf("you give the %s to %s\n", object_name, person_name);
    printf(".drop_goal\n");
    printf(".rw\n");
    printf("\n");
    printf(".save links_bring_%d_%d.dat\n", round_num, i+1);
  }
  return 0;
}


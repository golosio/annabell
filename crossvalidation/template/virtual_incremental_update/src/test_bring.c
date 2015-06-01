#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "train_virtual.h"
#include "move_comb.h"

int test_bring(FILE *fp, int start, int dest, int trans, int log_flag);

int main()
{
  int i; //, j, pos;
  int start, dest, trans; //, trans_dir, dest_dir;
  //int NTrain = 8;
  FILE *fp;
  char file_name[100];

  setup_doors();
  setup_names();
  //setup_train();
  setup_house();

  you_see_flag = 1;
  strcpy(person_name, "Alfred");
  strcpy(object_name_arr[0], "book");

  for (i=0; i<NComb; i++) {
    start = room_pos(comb_name[i][0]);
    trans = room_pos(comb_name[i][1]);
    dest = room_pos(comb_name[i][2]);

    sprintf(file_name, "test_bring_files/test_bring_%d.txt", i);
    fp=fopen(file_name, "w");
    test_bring(fp, start, dest, trans, 0);
    fclose(fp);
    sprintf(file_name, "test_bring_files/log_bring_%d.txt.correct", i);
    fp=fopen(file_name, "w");
    test_bring(fp, start, dest, trans, 1);
    fclose(fp);
  }

  return 0;
}

int test_bring(FILE *fp, int start, int dest, int trans, int log_flag)
{
  int i, j, pos, trans_dir, dest_dir, trans_opp_dir, dest_opp_dir;
  char object_name[15];
  
  if ((trans-start)==1) trans_dir = 0;
  else if ((trans-start)==-1) trans_dir = 2;
  else if ((trans-start)==-5) trans_dir = 1;
  else if ((trans-start)==5) trans_dir = 3;

  if ((dest-trans)==1) dest_dir = 0;
  else if ((dest-trans)==-1) dest_dir = 2;
  else if ((dest-trans)==-5) dest_dir = 1;
  else if ((dest-trans)==5) dest_dir = 3;

  trans_opp_dir = (trans_dir+2)%4;
  dest_opp_dir = (dest_dir+2)%4;

  strcpy(object_name, object_name_arr[0]);
  for (i=0; i<25; i++) {
    strcpy(you_see_arr[i],"nothing");
  }
  person_pos = start;
  object_pos[0] = dest;
  sprintf(you_see_arr[object_pos[0]], "some %s -s", object_name_arr[0]);

  for (j=0; j<11; j++) {
    pos = house_room_pos[j];
    fprintf(fp, "\n");
    if (log_flag!=0)
      fprintf(fp, " >>> End context\n");
    fdisplay(fp, pos);
  }
  //trans_dir = 0;
  //dest_dir = 1;
  
  fprintf(fp, "\n");
  if (log_flag!=0)
    fprintf(fp, " >>> End context\n");
  fdisplay(fp, start);
  fprintf(fp, "bring a %s to %s\n", object_name, person_name);
  fprintf(fp, ".x\n");
  if (log_flag!=0) {
    fprintf(fp, " -> %s\n", direction[trans_dir]);
    fprintf(fp, ".\n");
    fprintf(fp, "%s\n", direction[trans_dir]);
  }

  fprintf(fp, "\n");
  if (log_flag!=0)
    fprintf(fp, " >>> End context\n");
  fdisplay(fp, trans);
  fprintf(fp, ".x\n");
  if (log_flag!=0) {
    fprintf(fp, " -> %s\n", direction[dest_dir]);
    fprintf(fp, ".\n");
    fprintf(fp, "%s\n", direction[dest_dir]);
  }

  fprintf(fp, "\n");
  if (log_flag!=0)
    fprintf(fp, " >>> End context\n");
  fdisplay(fp, dest);
  fprintf(fp, ".x\n");
  if (log_flag!=0) {
    fprintf(fp, " -> take the\n");
    fprintf(fp, " ...  -> %s\n", object_name);
    fprintf(fp, ".\n");
    fprintf(fp, "take the %s\n", object_name);
  }
  fprintf(fp, "you take a %s\n", object_name);
  fprintf(fp, ".x\n");
  if (log_flag!=0) {
    fprintf(fp, " -> %s\n", direction[dest_opp_dir]);
    fprintf(fp, ".\n");
    fprintf(fp, "%s\n", direction[dest_opp_dir]);
  }

  fprintf(fp, "\n");
  if (log_flag!=0)
    fprintf(fp, " >>> End context\n");
  fdisplay(fp, trans);
  fprintf(fp, ".x\n");
  if (log_flag!=0) {
    fprintf(fp, " -> %s\n", direction[trans_opp_dir]);
    fprintf(fp, ".\n");
    fprintf(fp, "%s\n", direction[trans_opp_dir]);
  }

  fprintf(fp, "\n");
  if (log_flag!=0)
    fprintf(fp, " >>> End context\n");
  fdisplay(fp, start);
  fprintf(fp, ".x\n");
  if (log_flag!=0) {
    fprintf(fp, " -> give the\n");
    fprintf(fp, " ...  -> %s to %s\n", object_name, person_name);
    fprintf(fp, ".\n");
    fprintf(fp, "give the %s to %s\n", object_name, person_name);
  }
  fprintf(fp, "you give the %s to %s\n", object_name, person_name);
  fprintf(fp, ".x\n");
  if (log_flag!=0) {
    fprintf(fp, " -> \n");
    fprintf(fp, ".\n");
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
  if (log_flag!=0)
    fprintf(fp, " >>> End context\n");
  fprintf(fp, ".q\n");

  return 0;
}


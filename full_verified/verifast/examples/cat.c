#include "stdio.h"
#include "malloc.h"
#include "stdlib.h"

/*
Prints the contents of given file to stdout.
*/
int main(int argc, char** argv) //@ : main
  //@ requires 0 <= argc &*& [_]argv(argv, argc, _);
  //@ ensures true;
{
  struct file* fp = 0; char* buffer = 0; char* res = 0;
  if(argc < 2) { fputs("Enter a file name.", stderr); return -1; }
  //@ open [_]argv(argv, argc, _);
  //@ open [_]argv(argv + 1, argc - 1, _);
  fp = fopen(* (argv + 1), "r");
  buffer = malloc(sizeof(char) * 100);
  res = 0;
  if(fp == 0 || buffer == 0) { abort(); }
  res = fgets(buffer, 100, fp);
  while(res != 0) 
    //@ invariant file(fp) &*& chars(buffer, 100, ?cs) &*& res != 0 ? mem('\0', cs) == true : true;
  {
    //@ chars_separate_string(buffer);
    puts(buffer);
    //@ chars_unseparate_string(buffer);
    res = fgets(buffer, 100, fp);
  }
  free((void *)buffer);
  fclose(fp);
  return 0;
}

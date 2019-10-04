/*******************************************************************
 * EECS3221: Operating Systems, Summer 2019
 * Assignment 3: Memory
 * Student Name: Umar Abdulselam
 * Student EECS account: uaabduls
 * Student ID: 215995616
 *******************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "array.h"
#include "functions.h"

/**********************************************************************/
/********************       O P E N  A R R A Y     ********************/
/** Opens array from file filename and returns its pointer and size. **/
/** Size is the size of the file divided by size of the array struct **/
/************ Pointer to the array is obtained with mmap **************/
/**********************************************************************/
void open_array(char *filename, array_t *arrayp, int *sizep)
{
  int fd, length; // fd and length of file to be opened
  struct stat sb;

  // open existing array
  if ((fd = open(filename, O_RDWR, 0777)) < 0)
    fatalerr("mmaparray", 0, "Failed to open file.\n");
  // fstat to get info about the file; specifically the length
  if (fstat(fd, &sb) < 0)
    fatalerr("mmaparray", 0, "Failed to get file stats.\n");

  length = sb.st_size;

  // dereference sizep and point it to the number of array elements
  // the file (array) can hold
  *sizep = (int)(length / sizeof(**(arrayp)));

  // mmap "length" bytes of memory and return pointer to base adress
  *arrayp = (array_t) mmap(NULL, length, PROT_READ | PROT_WRITE,
              MAP_SHARED, fd, 0);
  if (arrayp == MAP_FAILED)
    fatalerr("mmaparray", 0, "Failed to mmap.\n");
  close(fd);
}

/**********************************************************************/
/********************     C L O S E  A R R A Y     ********************/
/******************* Unmaps array and sets it to NULL *****************/
/**********************************************************************/
void close_array(array_t *arrayp, int size)
{
  if (munmap(*arrayp, size) < 0){
    fatalerr("mmaparray", 0, "Failed to unmap array.\n");
  } else {
    arrayp = NULL;
  }
}

/**********************************************************************/
/********************    C R E A T E  A R R A Y    ********************/
/*** Creates a file with an array with index elements, all with the ***/
/* valid member false. It is fatal error if file exists. Size is the **/
/* size of the file divided by the size of the array struct. Pointer **/
/******************** array is obtained with mmap *********************/
/**********************************************************************/
void create_array(char *filename, int index, array_t *arrayp)
{
  int i, fd, length, size_struct;
  struct stat sb;

  size_struct = (int)(sizeof(**arrayp));

  if ((fd = open(filename, O_RDWR | O_CREAT | O_EXCL, 0777)) < 0)
    fatalerr("mmaparray", 0, "Failed to create file.\n");
  // ftruncate the file size to index * the size of an entry in the array
  if (ftruncate(fd, index*size_struct) < 0)
    fatalerr("mmaparray", 0, "Failed to truncate file.\n");
  if (fstat(fd, &sb) < 0)
    fatalerr("mmaparray", 0, "Failed to get file stats.\n");

  length = sb.st_size;

  // mmap "length" bytes of memory and return pointer to base adress
  *arrayp = (array_t)mmap(NULL, length, PROT_READ | PROT_WRITE,
              MAP_SHARED, fd, 0);
  if (arrayp == MAP_FAILED)
    fatalerr("mmaparray", 0, "Failed to mmap.\n");

  // setting valid field of allocated entries to false
  i = 0;
  while (i < index){
    (*arrayp)[i].index = i;
    (*arrayp)[i].valid = 0;
    i++;
  }
  close(fd);
}


/**********************************************************************/
/********************       S E T  E N T R Y       ********************/
/** Sets the elements of the index'ed entry to name and age and the ***/
/*********************** valid member to true *************************/
/**********************************************************************/
void set_entry(array_t array, char *name, int index, float age)
{
  strncpy(array[index].name, name, (MAXNAME*sizeof(char)));
  array[index].valid = 1;
  array[index].index = index;
  array[index].age = age;
}

/**********************************************************************/
/********************       G E T  E N T R Y       ********************/
/* Returns the index'd entry's name and age if valid, fatal error o/w */
/**********************************************************************/
void get_entry(array_t array, char **name, int index, float *age)
{
  if (array[index].valid != 1){
    fatalerr("mmaparray", 0, "Index not set. Failed to get.\n");
  } else {
    *name = array[index].name;
    *age = array[index].age;
  }
}

/**********************************************************************/
/********************   D E L E T E   E N T R Y    ********************/
/******** Sets the valid element of the index'ed entry to false *******/
/**********************************************************************/
void delete_entry(array_t array, int index)
{
  if (array[index].valid == 1){
    array[index].valid = 0;
  } else {
    printf("Warning: Can't delete entry that doesn't exist.\n");
  }
}

/**********************************************************************/
/********************     P R I N T   A R R A Y    ********************/
/* Prints all entries with valid member true using the same format as */
/***************************** in the main ****************************/
/**********************************************************************/
void print_array(array_t array, int size)
{
  int i = 0;
  while (i <= size){
    if (array[i].valid == 1){ // check if member/index is valid
      printf("index: %d, name: %s, age: %f\n",
        array[i].index, array[i].name, array[i].age);
    }
    i++;
  }
}

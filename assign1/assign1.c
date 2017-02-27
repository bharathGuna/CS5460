#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>

/*********************************************************************
 *
 * These C functions use patterns and functionality often found in
 * operating system code. Your job is to implement them. Of course you
 * should write test cases. However, do not hand in your test cases
 * and (especially) do not hand in a main() function since it will
 * interfere with our tester.
 *
 * Additional requirements on all functions you write:
 *
 * - you may not refer to any global variables
 *
 * - you may not call any functions except those specifically
 *   permitted in the specification
 *
 * - your code must compile successfully on CADE lab Linux
 *   machines when using:
 *
 * /usr/bin/gcc -O2 -fmessage-length=0 -pedantic-errors -std=c99 -Werror -Wall
 *-Wextra -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith
 *-Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter
 *-Wshadow -Wuninitialized -Wold-style-definition -c assign1.c
 *
 * NOTE 1: Some of the specifications below are specific to 64-bit
 * machines, such as those found in the CADE lab.  If you choose to
 * develop on 32-bit machines, some quantities (the size of an
 * unsigned long and the size of a pointer) will change. Since we will
 * be grading on 64-bit machines, you must make sure your code works
 * there.
 *
 * NOTE 2: You should not need to include any additional header files,
 * but you may do so if you feel that it helps.
 *
 * HANDIN: submit your finished file, still called assign.c, in Canvas
 *
 *
 *********************************************************************/

/*********************************************************************
 *
 * byte_sort()
 *
 * specification: byte_sort() treats its argument as a sequence of
 * 8 bytes, and returns a new unsigned long integer containing the
 * same bytes, sorted numerically, with the smaller-valued bytes in
 * the lower-order byte positions of the return value
 *
 * EXAMPLE: byte_sort (0x0403deadbeef0201) returns 0xefdebead04030201
 *
 *********************************************************************/

unsigned long byte_sort(unsigned long arg) { 
  //printf("Working %lu\n", arg);
  unsigned long bytes[8];
  unsigned long mask = 0x00000000000000ff;
  int i;
  for(i = 0; i < 8;i++)
    {
      bytes[i] = arg & mask;
      //printf("%lu\n",bytes[i]);
      arg = arg >> 8;
    }
  int j;
  int small = 0;
  for(i = 0; i < 8; i++){
    small = i;
    for(j = i+1; j < 8; j++)
      {
	if(bytes[j]<= bytes[small])
	  {
	    small = j;
	  }
      }
    long temp = bytes[i];
    bytes[i] = bytes[small];
    bytes[small]= temp;
    
  }

  /*  printf("Sorted");
  for(i=0; i < 8; i++)
    {
      printf("%lu\n",bytes[i]);
    }
  */
  
    

  //need to build the number
  unsigned long result = 0;
  
  for(i = 7; i >=0; i--)
    {
      result = result |(bytes[i] << 8*i);
    }
  //printf("%lu",result);
  return result;
}

/*********************************************************************
 *
 * nibble_sort()
 *
 * specification: nibble_sort() treats its argument as a sequence of
 * 16 4-bit numbers, and returns an unsigned long integer containing
 * the same nibbles, sorted numerically, with smaller-valued nibbles
 * towards the "small end" of the unsigned long value that you return
 *
 * the fact that nibbles and hex digits correspond should make it easy to
 * verify that your code is working correctly
 *
 * EXAMPLE: nibble_sort (0x0403deadbeef0201) returns 0xfeeeddba43210000
 *
 *********************************************************************/

unsigned long nibble_sort(unsigned long arg) { 
  //printf("Starting number: %0x\n",arg);
  unsigned long bits[16];
  unsigned long mask = 0x000000000000000f;
  int i;
  for(i = 0; i < 16;i++)
    {
      bits[i] = arg & mask;
      //printf("%016x\n",bits[i]);
      arg = arg >> 4;
    }
  int j;
  int small = 0;
  for(i = 0; i < 16; i++){
    small = i;
    for(j = i+1; j < 16; j++)
      {
	if(bits[j]<= bits[small])
	  {
	    small = j;
	  }
      }
    unsigned long temp = bits[i];
    bits[i] = bits[small];
    bits[small]= temp;
    
  }
    

  //need to build the number
  unsigned long result = 0;
  
  for(i = 15; i >=0; i--)
    {
      result = result |(bits[i] << 4*i);
    }
  return result;
}

/*********************************************************************
 *
 * name_list()
 *
 * specification: allocate and return a pointer to a linked list of
 * struct elts
 *
 * - the first element in the list should contain in its "val" field the first
 *   letter of your first name; the second element the second letter, etc.;
 *
 * - the last element of the linked list should contain in its "val" field
 *   the last letter of your first name and its "link" field should be a null
 *   pointer
 *
 * - each element must be dynamically allocated using a malloc() call
 *
 * - if any call to malloc() fails, your function must return NULL and must also
 *   free any heap memory that has been allocated so far; that is, it must not
 *   leak memory when allocation fails
 *
 *********************************************************************/

struct elt {
  char val;
  struct elt *link;
};

struct elt * name_list(void) {
  char name[] = "Bharath";
  printf("%s",name);
  struct elt *head = NULL;
  struct elt *current = NULL;
  struct elt *temp = NULL;
  int size = sizeof(struct elt);
  int idx;
  head = malloc(size);
  current = head;
  if(head == NULL)
    return NULL;
  // printf("Working");
  head -> val = name[idx];
  
  for(idx = 1; idx < 7; idx++)
    {
      temp = malloc(size);
  
      if(temp == NULL)
	{
	  while(head != NULL)
	    {
	      struct elt *next = head->link;
	      free(head);
	      head = next;
	      
	    }
	  return NULL;
	}
      // printf("Working %d",idx);
      temp -> val = name[idx];

      if(current != NULL)
	current -> link = temp;
      current = current -> link;
    }
  
  return head;
  /*
  current = head;
  while(current != NULL)
    {
      printf("%s",current->val);
      current = current->link;
      }*/
}

/*********************************************************************
 *
 * convert()
 *
 * specification: depending on the value of "mode", print "value" as
 * octal, binary, or hexidecimal to stdout, followed by a newline
 * character
 *
 * extra requirement 1: output must be via putc()
 *
 * extra requirement 2: print nothing if "mode" is not one of OCT,
 * BIN, or HEX
 *
 * extra requirement 3: all leading/trailing zeros should be printed
 *
 * EXAMPLE: convert (HEX, 0xdeadbeef) should print
 * "00000000deadbeef\n" (including the newline character but not
 * including the quotes)
 *
 *********************************************************************/

enum format_t { OCT = 66, BIN, HEX };

void convert(enum format_t mode, unsigned long value) {
  int i;
  unsigned long digit;
  char c;
  if(mode == OCT)
    {
      for(i = 21; i >= 0; i--)
	{
	  digit = 0x0000000000000007 & (value >> i*3);
	  c = digit + '0';
	  putc(c,stdout);
	}
      putc('\n',stdout);
    }
  else if(mode == BIN)
    {
      for(i = 63; i >= 0; i--)
	{
	  digit = 0x0000000000000001 & (value >> i*1);
	  
	  if(digit)
	    putc('1',stdout);
	  else
	    putc('0',stdout);
	}
      putc('\n',stdout);
    }
  else if(mode == HEX)
    {
      
      for(i=15; i >=0; i--)
	{
	  digit = 0x000000000000000f & (value >> i*4);
	  
	    switch(digit)
	      {
	    case 15:
	      putc('f',stdout);
	      break;
	    case 14:
	      putc('e',stdout);
	      break;
	    case 13:
	      putc('d',stdout);
	      break;
	    case 12:
	      putc('c',stdout);
	      break;
	    case 11:
	      putc('b',stdout);
	      break;
	    case 10:
	      putc('a',stdout);
	      break;
	    default:
	      c = digit+'0';
	      putc(c,stdout);
	      }
	}
      putc('\n',stdout);
    }
  else
    putc('\n', stdout);
    
    
}

/*********************************************************************
 *
 * draw_me()
 *
 * this function creates a file called me.txt which contains an ASCII-art
 * picture of you (it does not need to be very big). the file must (pointlessly,
 * since it does not contain executable content) be marked as executable.
 *
 * extra requirement 1: you may only call the function syscall() (type "man
 * syscall" to see what this does)
 *
 * extra requirement 2: you must ensure that every system call succeeds; if any
 * fails, you must clean up the system state (closing any open files, deleting
 * any files created in the file system, etc.) such that no trash is left
 * sitting around
 *
 * you might be wondering how to learn what system calls to use and what
 * arguments they expect. one way is to look at a web page like this one:
 * http://blog.rchapman.org/post/36801038863/linux-system-call-table-for-x86-64
 * another thing you can do is write some C code that uses standard I/O
 * functions to draw the picture and mark it as executable, compile the program
 * statically (e.g. "gcc foo.c -O -static -o foo"), and then disassemble it
 * ("objdump -d foo") and look at how the system calls are invoked, then do
 * the same thing manually using syscall()
 *
 *********************************************************************/

void draw_me(void) {

  //creating file and setting it to write
  int file = syscall(SYS_open, "me.txt", O_WRONLY|O_CREAT,777);

  if(file == -1)
    return;

  char draw[] = "\\⌒(ʘ_ʘ)⌒/";

  int wres = syscall(SYS_write, file, draw, sizeof(draw)-1);

  if(wres == -1)
    {
      syscall(SYS_close, file);
      syscall(SYS_unlink, "me.txt");
      return;
    }

  int cres = syscall(SYS_close, file);
	
  if (cres < -1)
    {
      return;
    }
}

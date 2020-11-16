#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/std.h"
#include "../include/terminal.h"

unsigned char inportb (unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb (unsigned short _port, unsigned char _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void putch(char c) 
{

	if (c == '\n') {
		main_terminal.row++;
		main_terminal.column = 0;
	}
	else if (c == '\t') {
		main_terminal.column += 4;
	}
  else if (c == '\b' && main_terminal.column == 0) {
    main_terminal.row--;
    main_terminal.column = 79;
	}
  else if (c == '\b' && main_terminal.column > 0) {
    insert_at(' ', --main_terminal.column, main_terminal.row);
	}
  else if (c == 0){

  }
	else {
    insert_at(c, main_terminal.column, main_terminal.row);
    main_terminal.column++;
  } 
	if (main_terminal.column == WIDTH_T){
    main_terminal.column = 0;
    main_terminal.row++;
  } 
	if (main_terminal.row == HEIGHT_T){
    scroll();
    main_terminal.row = HEIGHT_T - 1;
    for (int i = 0; i < WIDTH_T; i++) {
      insert_at(' ', main_terminal.column + i, main_terminal.row);
    }
  }
  move_csr(); 
}

void move_csr()
{
    unsigned temp = main_terminal.row * 80 + main_terminal.column;
    outportb(0x3D4, 14);
    outportb(0x3D5, temp >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, temp);
}

unsigned int strlen(char *txt){
	unsigned int i = 0;
	while (*txt++){
		i++;
	}
	return i;
}
void *mem_set(void *dest, char c, size_t size)
{
    uint8_t *temp = (uint8_t *)dest;
    while(size--) 
      *temp++ = c;
    return dest;
}
void printstring(char *txt)
{
	for(unsigned int i = 0; i < strlen(txt); i++){
		putch(*(txt + i));
	}
}

long long power10(int base)
{
  int i;
  long long a = 1;
  for(i=0;i<base;i++)
  {
    a *= 10;
  }
  return a;
}
int abs(int a){
	if (a >= 0) return a;
	else return -a;  
}
long long llabs(long long a){
	if (a >= 0) return a;
	else return -a;  
}

unsigned int num_len(long long num){
	if (!num) return 1;
	for (unsigned int i = 0; i < 15; i++) {
		if (power10(i) > num) return i;
	}
}
int printint(int a)
{
  if (a == 0)
  {
    putch('0');
    return 1;
  }
  int size = num_len(abs(a)), i;
  char x;
  if (a > 0)
  {
    for(i=size-1;i>=0;i--)
    {
      x = a / (int)power10(i) % 10 + '0';
      putch(x);
    }
  }
  else
  {
    a = abs(a);
    for(i=size;i>=0;i--)
    {
      if (i == size) putch('-');
      else
      {
        x = a / (int)power10(i) % 10 + '0';
        putch(x);
      }
    }
    size++;
  }
  return size;
}
int printdouble(double a)
{
  if (a == 0)
  {
    printstring("0.00000");
    return 7;
  }
  int size = num_len(abs(a)) + 5, i;

  long long b = (long long)(a * 100000);
  char x;
  if (a > 0)
  {
    for(i=size-1;i>=0;i--)
    {
      x = (long)b / (long)power10(i) % (long)10 + '0';
      putch(x);
      if (i == 5) putch('.');
    }
    size++;
  }
  else
  {
    b = llabs(b);
    for(i=size;i>=0;i--)
    {
      if (i == size) putch('-');
      else
      {
        x = (long)b / (long)power10(i) % (long)10 + '0';
        putch(x);
        if (i == 5) putch('.');
      }
    }
    size += 2;
  }
  return size;
}
void print(char *output, ...)
{
  int counter = 0, i = 0, size = strlen(output);
  va_list tab;
  va_start(tab, output);
  char *s;
  int a = 0;
  double b = 0;
  for (i=0;i<size;)
  {
    if(*(output + i) == '%' && i < size - 1)
    {
      if(*(output + i + 1) == 's')
      {
        s = va_arg(tab, char*);
        printstring(s);
        i += 2;
      }
      else if(*(output + i + 1) == 'd')
      {
        a = va_arg(tab, int);
        printint(a);
        i += 2;
      }
      else if(*(output + i + 1) == 'f')
      {
        b = va_arg(tab, double);
        printdouble(b);
        i += 2;
      }
      else {
        putch(*(output + i));
        i++;
      }
    }
    else {
      putch(*(output + i));
      i++;
    }
  }
  va_end(tab);
}

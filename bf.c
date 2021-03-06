#include <stdio.h>
#include <stdlib.h>

#define SIZE 65536

char memory[SIZE];
int memptr = 0;
char *program;

void exec (int pc) {
  switch(program[pc]) {
  case '+': memory[memptr]++;                    break;
  case '-': memory[memptr]--;                    break;
  case '>': memptr = (memptr + 1) % SIZE;        break;
  case '<': memptr = (memptr + SIZE - 1) % SIZE; break;
  case '.': putchar(memory[memptr]);             break;
  case ',': memory[memptr] = getchar();          break;
  default : break;
  }
}

int loopend (int pc) {
  char c;
  int count = 1;
  while (c = program[++pc]) {
    if (c == '[') {
      count++;
    } else if (c == ']') {
      count--;
    }
    if (count == 0) {
      return pc;
    }
  }
  return -1;
}

void loop (const int start, const int end) {
  int i, j;
  while (memory[memptr] != 0) {
    i = start;
    while (i <= end) {
      if (program[i] == '[') {
	j = loopend(i);
	loop(i + 1, j - 1);
	i = j + 1;
      } else {
	exec(i);
	i++;
      }
    }
  }
  return;
}

void bfrun (void) {
  int j, i = 0;
  while (program[i] != '\0') {
    if (program[i] == '[') {
      j = loopend(i);
      loop(i + 1, j - 1);
      i = j + 1;
    } else {
      exec(i);
      i++;
    }
  }
}

char *readfile(char *filename) {
  FILE *in = fopen(filename, "r");
  int i = 0;
  int length = lseek(fileno(in), 0, SEEK_END) + 1;
  fseek(in, 0, SEEK_SET);
  char *s = (char *)calloc(length, sizeof(char));

  char c;
  while((c = getc(in)) != EOF) {
    s[i++] = c;
  }
  s[i-1] = '\0';
  fclose(in);

  return s;
}

int main (int argc, char** argv) {
  program = readfile (argv[1]);
  if (syntaxcheck(program) != 0) {
    return -1;
  }
  bfrun();
  free(program);
  return 0;
}

int syntaxcheck(char *s) {
  // Basically: check if loop openers and closers match up.
  int i, count = 0;
  int line = 0;
  int charnum = 0;
  char c;
  for (i = 0, c = s[i++]; c != '\0'; c = s[i++]) {
    if (c == '\n') {
      line++;
      charnum = 0;
      continue;
    }

    if (c == '[') {
      count++;
    } else if (c == ']') {
      count--;
    }
    charnum++;

    if (count < 0) {
      printf("Error: No match for closing square bracket at (%d, %d).\n",
	     line, charnum);
      return -1;
    }
  }

  if (count > 0) {
    printf("Error: %d unclosed loops.\n", count);
  }
  return count;
}

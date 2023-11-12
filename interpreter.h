#ifndef INTERPRETER
#define INTERPRETER

int interpreter(char* command_args[], int args_size);
int runFromMemInterval(int startInt, int PC, int numLines, int programLength, char *script);
int help();
#endif // INTERPRETER
//search for ??? for the todos
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "textcolor.h"

int create_stack(char* Stack);
void print_intro(char* Stack);
void print_result(char* Stack, char* StackResult, int InputLen);
int accept_input(char* Input, int InputLen);
void result_stack(char* StackResult, char* Input, int InputLen);
int check_ra(char* StackResult);
int check_shellcode(char* StackResult, int Result);
void select_joke();

//constant for default RA???  currently ffabc123 hardcoded
const int STACKSIZE = 512;
const int ROWLEN = 32;
//make sure that both the RA and BUFF have at least 4 bytes on the same from where for they started for coloring purposes
//logic in result_stack second
const int RA = 200;
const int BUF = 112;
char SHELLCODE[] = "\\x31\\xc0\\x31\\xdb\\x31\\xc9\\x99\\xb0\\xa4\\xcd\\x80\\x6a\\x0b\\x58\\x51\\x68\\x2f\\x2f\\x73\\x68\\x68\\x2f\\x62\\x69\\x6e\\x89\\xe3\\x51\\x89\\xe2\\x53\\x89\\xe1\\xcd\\x80";

//int check_canary(char *Canary);
//int random_ra_addr();
//int random_buffer_addr();
//ret2libc stuff


int main()
{	
	char Input[STACKSIZE - BUF];	//STACKSIZE - BUF
	char Stack[STACKSIZE];	//STACKSIZE
	char StackResult[STACKSIZE];  //STACKSIZE
	int InputLen, Result;
	
	//program
	//change print functions to accept stack string???
	InputLen = STACKSIZE - BUF;
	create_stack(Stack);
	print_intro(Stack);
	InputLen = accept_input(Input, InputLen);
	result_stack(StackResult, Input, InputLen);
	print_result(Stack, StackResult, InputLen);
	Result = check_ra(StackResult);
	printf("\n");
	if (Result == -1)
	{
		textcolor(UNDERLINE, WRONG, BLACK);
		printf("Wrong: The overflowed return address does not point to a valid address within the stack.\n");
		textcolor(UNDERLINE, DEFAULT, BLACK);
		printf("\n");
	}
	else
	{
		Result = check_shellcode(StackResult, Result);
		if(Result != 0)
		{
			textcolor(UNDERLINE, WRONG, BLACK);
			printf("Wrong: The overflowed return address does not point to a correct shellcode string.\n");
			textcolor(UNDERLINE, DEFAULT, BLACK);
			printf("\n");
		}
		else
		{
			textcolor(UNDERLINE, CORRECT, BLACK);
			printf("Correct!  Good job.\n");
			textcolor(UNDERLINE, DEFAULT, BLACK);
			printf("\n");
		}
	}
	//free(Input);
	return 0;
}

int create_stack(char* Stack)
{
	
	for(int i = 0; i < STACKSIZE; i++)
	{
		Stack[i] = '0';
	}
	//insert RA and buffer to stack
	char tmp[] = "ffabc123[buffer]";
	for(int i = 0; i < 8; i++)
	{
		Stack[RA + i] = tmp[i];
		Stack[BUF + i] = tmp[i+8];
	}

	return RA;
}

void print_intro(char* Stack)
{
	//printf("%s\n","");
	printf("%s\n","---------------------------------------------------------------------------------");
	textcolor(UNDERLINE, MENU, BLACK);
	printf("%s\n", "Welcome to the buffer overflow educational module!");
	printf("%s\n", "The goal is to run a shell via input that the program accepts.");
	printf("%s\n","");
	//shellcode
	textcolor(UNDERLINE, DEFAULT, BLACK);
	printf("%s%s\n", "Here is the shellcode in hex form: ", SHELLCODE);
	printf("%s\n","");
	printf("%s\n", "Here is the stack of the program:");
	printf("%s\n","");
	
	textcolor(UNDERLINE, MEMORY, BLACK);
	printf("%s\n","--Memory--    ------------ Pre-Stack ------------");
	char tmp[] = "0123456789abcdef";
	char cur;
	int count = 0;
	for(int i = 15; i >= 0; i--)
	{
		textcolor(UNDERLINE, MEMORY, BLACK);
		printf( "%s%c%c    ","0xffffff",tmp[i],'0');
		for(int j = 0; j < ROWLEN; j++)
		{
			cur = Stack[i*ROWLEN+j];
			if(cur == '0'){textcolor(UNDERLINE, DEFAULT, BLACK);}
			else 
			{
				if(count < 8)
				{
					textcolor(UNDERLINE, RETADDR, BLACK);
					count++;
				}
				else
				{
					textcolor(UNDERLINE, BUFFER, BLACK);
					count++;
				}
			}
			if(j == 8 || j == 16 || j == 24)
			{
				printf(" %c",cur);
				continue;
			}
			if(j == ROWLEN - 1)
			{
				if(i == RA / ROWLEN)
				{
					printf("%c",cur);
					textcolor(UNDERLINE, RETADDR, BLACK);
					printf("    <-- Here is the return address\n");
					textcolor(UNDERLINE, DEFAULT, BLACK);
					continue;
				}
				else if(i == BUF / ROWLEN)
				{
					printf("%c",cur);
					textcolor(UNDERLINE, BUFFER, BLACK);
					printf("    <-- Here is the buffer address\n");
					textcolor(UNDERLINE, DEFAULT, BLACK);
					continue;
				}
				else
				{
					printf("%c\n",cur);
					continue;
				}
			}
			printf("%c",cur);
		}
		textcolor(UNDERLINE, DEFAULT, BLACK);
	}
	char tmp1[] = "0 1 2 3  4 5 6 7  8 9 a b  c d e f";
	textcolor(UNDERLINE, MEMORY, BLACK);
	printf("              -----------------------------------\n");
	printf("LSB:          %s\n",tmp1);
	textcolor(UNDERLINE, DEFAULT, BLACK);
	printf("%s\n","");
}

void print_result(char* Stack, char* StackResult, int InputLen)
{
	printf("Results:\n");
	textcolor(UNDERLINE, MEMORY, BLACK);
	printf("%s\n","--Memory--    ------------ Pre-Stack ------------    ----------- Post-Stack ------------");
	char tmp[] = "0123456789abcdef";
	char cur;
	int count = 0;
	int failed;
	if(InputLen + BUF >= RA + 8){failed = 0;}
	else{failed = 1;}
	for(int i = 15; i >= 0; i--)
	{
		textcolor(UNDERLINE, MEMORY, BLACK);
		printf("%s%c%c    ","0xffffff",tmp[i],'0');
		for(int j = 0; j < ROWLEN; j++)
		{
			cur = Stack[i*ROWLEN+j];
			if(cur == '0'){textcolor(UNDERLINE, DEFAULT, BLACK);}
			else 
			{
				if(count < 8)
				{
					textcolor(UNDERLINE, RETADDR, BLACK);
					count++;
				}
				else
				{
					textcolor(UNDERLINE, BUFFER, BLACK);
					count++;
				}
			}
			if(j == 8 || j == 16 || j == 24)
			{
				printf(" %c",cur);
				continue;
			}
			if(j == ROWLEN - 1)
			{
				printf("%c    ",cur);
				continue;
			}
			printf("%c",cur);
		}
		for(int j = 0; j < ROWLEN; j++)
		{
			cur = StackResult[i*ROWLEN+j];
			if(cur == '0' && (i*ROWLEN+j < BUF || i*ROWLEN+j > BUF + InputLen))
			{
				textcolor(UNDERLINE, DEFAULT, BLACK);
			}
			else 
			{
				if(failed)
				{
					if((i*ROWLEN+j) >= RA && (i*ROWLEN+j) > BUF + InputLen ){textcolor(UNDERLINE, RETADDR, BLACK);}
					else{textcolor(UNDERLINE, BUFFER, BLACK);}
				}
				else {textcolor(UNDERLINE, BUFFER, BLACK);}
			}
			if(j == 8 || j == 16 || j == 24)
			{
				printf(" %c",cur);
				continue;
			}
			if(j == ROWLEN - 1)
			{
				printf("%c\n",cur);
				continue;
			}
			printf("%c",cur);
		}
	}
	textcolor(UNDERLINE, DEFAULT, BLACK);
}

int accept_input(char* Input, int InputLen)
{
	int Len, j;
	char *tmp;
	tmp = (char *) malloc(InputLen*4);
	
	printf("Please input your payload: ");
	//fgets(Input, InputLen, stdin);
	fgets(tmp, InputLen, stdin);
	
	Len = strlen(tmp) - 1;
	if (InputLen > Len){InputLen = Len;}
	
	j = 0;
	for(int i = 0; i < InputLen;i++)
	{
		if(tmp[i] != '\\')
		{
			sprintf(Input+2*j++,"%02X", tmp[i]);
			continue;
		}
		if(tmp[i+1] != 'x' && tmp[i+1] != 'X')
		{
			sprintf(Input+2*j++,"%02X", tmp[i]);
		}
		else
		{
			sprintf(Input+2*j,"%c", tmp[i+2]);
			sprintf(Input+1+2*j++,"%c", tmp[i+3]);
			i = i+3;
		}
	}
	Len = strlen(Input) - 1;
	InputLen = Len;
	printf("Here is your payload in hex: ");
	textcolor(UNDERLINE, BUFFER, BLACK);
	printf("%s\n", Input);
	textcolor(UNDERLINE, DEFAULT, BLACK);
	printf("\n");
	
	free(tmp);
	return InputLen;
}

void result_stack(char* StackResult, char* Input, int InputLen)
{
	create_stack(StackResult);
	//printf("This is the %d\n",InputLen);
	for(int i = 0; i <= InputLen; i++)
	{
		StackResult[i+BUF] = Input[i];
		//StackResult[i*2+BUF+1] = Input[i*2 +1];
		//printf("%c%c\n",Input[i*2],Input[i*2 +1]);
	}
}

int check_ra(char* StackResult)
{
	int result = 0;
	char tmp;
	//assumes first 6 bits are f in the address
	for(int i = 0; i < 6; i++)
	{
		tmp = StackResult[RA + i];
		if(!(tmp == 'f' || tmp == 'F'))
		{
			return -1;
		}
	}
	//only checks the last two bits
	int pos = 1;
	for(int i = 6; i < 8; i++)
	{
		tmp = StackResult[RA + i];
		if(tmp == '0'){result = result + pow(16,pos) * 0;}
		else if(tmp == '1'){result = result + pow(16,pos) * 1;}
		else if(tmp == '2'){result = result + pow(16,pos) * 2;}
		else if(tmp == '3'){result = result + pow(16,pos) * 3;}
		else if(tmp == '4'){result = result + pow(16,pos) * 4;}
		else if(tmp == '5'){result = result + pow(16,pos) * 5;}
		else if(tmp == '6'){result = result + pow(16,pos) * 6;}
		else if(tmp == '7'){result = result + pow(16,pos) * 7;}
		else if(tmp == '8'){result = result + pow(16,pos) * 8;}
		else if(tmp == '9'){result = result + pow(16,pos) * 9;}
		else if(tmp == 'a' || tmp == 'A'){result = result + pow(16,pos) * 10;}
		else if(tmp == 'b' || tmp == 'B'){result = result + pow(16,pos) * 11;}
		else if(tmp == 'c' || tmp == 'C'){result = result + pow(16,pos) * 12;}
		else if(tmp == 'd' || tmp == 'D'){result = result + pow(16,pos) * 13;}
		else if(tmp == 'e' || tmp == 'E'){result = result + pow(16,pos) * 14;}
		else if(tmp == 'f' || tmp == 'F'){result = result + pow(16,pos) * 15;}
		else {return -1;}
		pos--;
	}
	return result;
}

int check_shellcode(char* StackResult, int Result)
{
	char tmp[] = "31c031db31c999b0a4cd806a0b5851682f2f7368682f62696e89e35189e25389e1cd80";
	int Len = strlen(tmp);
	for(int i = 0; i < Len; i++)
	{
		if(tmp[i] != StackResult[Result*2 + i])
		{
			return -1;
		}
	}
	return 0;
}

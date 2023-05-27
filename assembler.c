#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OPMAX 59

char opt_name[OPMAX][50], opt_num[OPMAX][50];

typedef struct symtable *node;
struct symtable
{
	char name[10];
	char num[10];
	node next;
};
node head = NULL, tail = NULL;

void insert_sym(char temp_name[], char *temp_num)
{
	node temp;
	temp = (node)malloc(sizeof(struct symtable));
	strcpy(temp->name, temp_name);
	strcpy(temp->num, temp_num);
	temp->next = NULL;
	if (head == NULL)
		head = temp;
	else
		tail->next = temp;
	tail = temp;
}

int is_sym(char temp_name[])
{
	node ptr = head;
	while (ptr)
	{
		if (strcmp(ptr->name, temp_name) == 0)
			return 1;
		ptr = ptr->next;
	}
	return 0;
}

void update_sym(char temp_name[], char temp_num[])
{
	node ptr = head;
	while (ptr)
	{
		if (strcmp(ptr->name, temp_name) == 0)
			strcpy(ptr->num, temp_num);
		ptr = ptr->next;
	}	
}

char* search_sym(char temp_name[])
{
	node ptr = head;
	while (ptr)
	{
		if (strcmp(ptr->name, temp_name) == 0)
			return ptr->num;
		ptr = ptr->next;
	}
	return "";
}

void output_sym()
{
	node ptr = head;
	while (ptr)
	{
		printf("%s -> %s\n", ptr->name, ptr->num);
		ptr = ptr->next;
	}
}

int search_opt(char temp_name[])
{
	int i;
	for (i = 0; i < OPMAX; i++)
		if (strcmp(temp_name, opt_name[i]) == 0)
			return i;
	return -1;
}

char change_x(char error)
{
	int cx[4], i, temp = error - '0', add = 0, cnt = 1;
	for (i = 3; i >= 0; i--, temp/=2)
		cx[i] = temp%2;
	cx[0] = 1;
	for (i = 3; i >= 0; i--, cnt*=2)
		add += cx[i]*cnt;
	return (add+'0');
}


int main()
{
	// path 1
	
	int i, j, record_opt = 0, file_line = 0;
	char str[50];
	FILE *ot;
	FILE *fs;
	FILE *im;
	ot = fopen("op_table.txt", "r");
	ot = fopen("op_table.txt", "r");
	while (!feof(ot))
	{
		fgets(str, 50, ot);
		int opt_flag = 0, opt_cnt = 0;
		for (i = 0; i < strlen(str); i++)
		{
			if (opt_flag == 0)
			{
				if (str[i] == '\t')
				{
					opt_flag = 1;
					opt_name[record_opt][i] = '\0';
					opt_cnt = 0;
				}
				else
					opt_name[record_opt][opt_cnt++] = str[i];
			}	
			else if (opt_flag == 1)
			{
				if (str[i] == '\n')
				{
					opt_num[record_opt][opt_cnt] = '\0';
					break;
				}
				else
					opt_num[record_opt][opt_cnt++] = str[i];
			}
		}
		record_opt++;
	}
	memset(str, 0, 50);
	fclose(ot);
	fs = fopen("source_code.txt", "r");
	im = fopen("immediate_file.txt", "a");
	char field[3][50], locctr[10];
	int initial = 0, locnum = 0;
	while (!feof(fs))
	{
		int flag = 0, cin = 0;
		char output_line[50];
		fgets(str, 50, fs);
		for (i = 0; i < strlen(str), flag < 3; i++)
		{
			if (str[i] == '\t' || str[i] == '\n')
			{
				field[flag][cin] = '\0';
				cin = 0;
				flag++;
			}
			else
				field[flag][cin++] = str[i];
		}
		if (initial == 0)
		{
			if (strcmp(field[1], "START") == 0)
				strcpy(locctr, field[2]);
			else
				strcpy(locctr, "0\0");
			
			int trans_temp = atoi(locctr);
			locnum = 1;
			while (trans_temp >= 1)
			{
				locnum *= 16;
				trans_temp /= 16;
			}
			initial = 1;
		}
		if (strcmp(field[1], "END") == 0)
		{
			memset(output_line, 0, 50);
			strcat(output_line, "\t");
			strcat(output_line, str);
//			printf("%s", output_line);
//			fputs(output_line, im);
			break;
		}	
		sprintf(locctr, "%x", locnum);
		strcpy(output_line, locctr);
		for (i = 0; i < 3; i++)
		{
			strcat(output_line, "\t");
			strcat(output_line, field[i]);
		}
		strcat(output_line, "\n");
		update_sym(field[0], locctr);
//		printf("%s", output_line);
//		fputs(output_line, im);
		if (initial == 1)
		{
			if (str[0] != '.')
			{
				if (*field[0] != 0)		// There is a label
				{
					if (is_sym(field[0]) == 1)
						printf("Error Flag : Duplicate symbol !\n");
					else if (is_sym(field[0]) == 0)
						insert_sym(field[0], locctr);
				}
				if (search_opt(field[1]) != -1 || strcmp(field[1], "WORD") == 0)
					locnum += 3;
				else
				{
					if (strcmp(field[1], "RESW") == 0)
						locnum += 3*atoi(field[2]);
					else if (strcmp(field[1], "RESB") == 0)
						locnum += atoi(field[2]);
					else if (strcmp(field[1], "BYTE") == 0)
					{
						char operand_type = field[2][0];
						if (operand_type == 'C')
							locnum += (strlen(field[2])-3);
						else if (operand_type == 'X')
							locnum += (strlen(field[2])-3)/2;
					}
				}
			}
		}
	}
	fclose(fs);
	memset(str, 0, 50);
//	memset(output_line, 0, 50);
	
	//path 2
	file_line = 46;
	int p2_cnt = 0, total_;
	char loc[file_line][50], f2[4][50];
	im = fopen("immediate_file.txt", "r");
	while (!feof(im))
	{
		int flag2 = 0, f_cnt = 0;
		char opcode[5], final_object_code[10];
		fgets(str, 50, im);
//		printf("%s\n", str);
		for (i = 0; i < strlen(str); i++)
		{
			if (str[i] == '\t' || str[i] == ' ' || str[i] == '\n')
			{
				f2[flag2][f_cnt] = '\0';
				f_cnt = 0;
				flag2++;
			}
			else
				f2[flag2][f_cnt++] = str[i];
		}
		printf("%s\t%s\t%s\t%s\t", f2[0], f2[1], f2[2], f2[3]);
		int find_o = search_opt(f2[2]);
		strcpy(opcode, opt_num[find_o]);
//		printf("%s -> %s,\t", f2[2], opcode);
		char trans_hex[10];
		if (find_o != -1)
		{
			char find_operand[10];
//			printf("f3 = %s.\n", f2[3]);
			if (*f2[3] != 0)
			{
				strcpy(find_operand, search_sym(f2[3]));
				if (*find_operand == 0)
				{
					char type_x[10];
					int check_x = 0;
					strcpy(find_operand, "0000");
					for(i = 0; i < strlen(f2[3]); i++)
					{
						if (f2[3][i] == ',')
						{
							strcpy(find_operand, search_sym(type_x));
							find_operand[0] = change_x(find_operand[0]);
//							printf("new = %s\n", find_operand);
							check_x = 1;
							break;
						}
						type_x[i] = f2[3][i];
					}
					if (check_x == 0)
						printf("Error Flag : Undefined Symbol ");
				}
//				printf("%s -> %s.\n", f2[3], find_operand);
			}
			else
			{
				strcpy(find_operand, "0000");
//				printf("%s -> %s.\n", f2[3], find_operand);
			}
			strcpy(final_object_code, opcode);
			strcat(final_object_code, find_operand);
			printf("%s\n", final_object_code);
		}
		else if (strcmp(f2[2], "BYTE") == 0)
		{
//			printf("** now line **\t%s\n", f2[3]);
			if (f2[3][0] == 'C')
			{
				for (i = 2; f2[3][i] != '\''; i++)
				{
					
					sprintf(trans_hex, "%x", f2[3][i]);
//					printf("_%s ", trans_hex);
					strcat(final_object_code, trans_hex);
				}
			}
			else if (f2[3][0] == 'X')
			{
				int ttc = 0;
				for (i = 2; f2[3][i] != '\''; i++)
				{
//					printf("_%d ", f2[3][i]);
					final_object_code[ttc++] = f2[3][i];
				}	
			}
			printf("%s\n", final_object_code);
		}
		else if (strcmp(f2[2], "WORD") == 0)
		{
//			printf("** f2[3] = %s ** ", f2[3]);
			sprintf(trans_hex, "%x", atoi(f2[3]));
			strcpy(final_object_code, trans_hex);
			printf("%s\n", final_object_code);
		}
		else
			printf("\n");
		printf("\n");
		memset(final_object_code, 0, 50);
	}
	printf("====================\n");
	output_sym();
	printf("====================\n");
	fclose(im);
	return 0;
}

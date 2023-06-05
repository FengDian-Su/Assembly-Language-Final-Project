#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OPMAX 59
char opt_name[OPMAX][50], opt_num[OPMAX][50], loc_temp[50], hex_loc[5], restore[50][1000], r2[50][4];
typedef struct symtable *node;
struct symtable{
	char name[10];
	char num[10];
	node next;
};
node head = NULL, tail = NULL;
void insert_sym(char temp_name[], char *temp_num){
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
int is_sym(char temp_name[]){
	node ptr = head;
	while (ptr){
		if (strcmp(ptr->name, temp_name) == 0)
			return 1;
		ptr = ptr->next;
	}
	return 0;
}
void update_sym(char temp_name[], char temp_num[]){
	node ptr = head;
	while (ptr){
		if (strcmp(ptr->name, temp_name) == 0)
			strcpy(ptr->num, temp_num);
		ptr = ptr->next;
	}	
}
char* search_sym(char temp_name[]){
	node ptr = head;
	while (ptr){
		if (strcmp(ptr->name, temp_name) == 0)
			return ptr->num;
		ptr = ptr->next;
	}
	return "";
}
void output_sym(){
	node ptr = head;
	while (ptr){
		printf("%s -> %s\n", ptr->name, ptr->num);
		ptr = ptr->next;
	}
}
int search_opt(char temp_name[]){
	int i;
	for (i = 0; i < OPMAX; i++)
		if (strcmp(temp_name, opt_name[i]) == 0)
			return i;
	return -1;
}
char change_x(char error){
	int cx[4], i, temp = error - '0', add = 0, cnt = 1;
	for (i = 3; i >= 0; i--, temp/=2)
		cx[i] = temp%2;
	cx[0] = 1;
	for (i = 3; i >= 0; i--, cnt*=2)
		add += cx[i]*cnt;
	return (add+'0');
}
int hex_to_dec(char ary[]){
	int trans[strlen(ary)], i, add = 0, mul = 1;
    	for (i = 0; i < strlen(ary); i++){
   		switch(ary[i]){
		   	case 'a':trans[i] = 10;break;
		   	case 'b':trans[i] = 11;break;
		   	case 'c':trans[i] = 12;break;
		   	case 'd':trans[i] = 13;break;
		   	case 'e':trans[i] = 14;break;
		   	case 'f':trans[i] = 15;break;
		   	default:trans[i] = ary[i]-'0';
		}
    	}
   	for (i = strlen(ary)-1; i >= 0; i--, mul*=16)
   		add += trans[i]*mul;
   	return add;
}
int main(){
	int i, j, record_opt = 0, file_line = 0;
	char str[50];
	FILE *ot;
	FILE *fs;
	FILE *im;
	ot = fopen("op_table.txt", "r");
	ot = fopen("op_table.txt", "r");
	while (!feof(ot)){
		fgets(str, 50, ot);
		int opt_flag = 0, opt_cnt = 0;
		for (i = 0; i < strlen(str); i++){
			if (opt_flag == 0){
				if (str[i] == '\t'){
					opt_flag = 1;
					opt_name[record_opt][i] = '\0';
					opt_cnt = 0;
				}
				else
					opt_name[record_opt][opt_cnt++] = str[i];
			}	
			else if (opt_flag == 1){
				if (str[i] == '\n'){
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
	while (!feof(fs)){
		int flag = 0, cin = 0;
		char output_line[50];
		fgets(str, 50, fs);
		for (i = 0; i < strlen(str), flag < 3; i++){
			if (str[i] == '\t' || str[i] == '\n'){
				field[flag][cin] = '\0';
				cin = 0;
				flag++;
			}
			else
				field[flag][cin++] = str[i];
		}
		if (initial == 0){
			if (strcmp(field[1], "START") == 0)
				strcpy(locctr, field[2]);
			else
				strcpy(locctr, "0\0");
			
			int trans_temp = atoi(locctr);
			locnum = 1;
			while (trans_temp >= 1){
				locnum *= 16;
				trans_temp /= 16;
			}
			initial = 1;
		}
		if (strcmp(field[1], "END") == 0){
			memset(output_line, 0, 50);
			strcat(output_line, "\t");
			strcat(output_line, str);
//			fputs(output_line, im);
			break;
		}	
		sprintf(locctr, "%x", locnum);
		strcpy(output_line, locctr);
		for (i = 0; i < 3; i++){
			strcat(output_line, "\t");
			strcat(output_line, field[i]);
		}
		strcat(output_line, "\n");
		update_sym(field[0], locctr);
//		fputs(output_line, im);
		if (initial == 1){
			if (str[0] != '.'){
				if (*field[0] != 0){		// There is a label
					if (is_sym(field[0]) == 1)
						printf("Error Flag : Duplicate symbol !\n");
					else if (is_sym(field[0]) == 0)
						insert_sym(field[0], locctr);
				}
				if (search_opt(field[1]) != -1 || strcmp(field[1], "WORD") == 0)
					locnum += 3;
				else{
					if (strcmp(field[1], "RESW") == 0)
						locnum += 3*atoi(field[2]);
					else if (strcmp(field[1], "RESB") == 0)
						locnum += atoi(field[2]);
					else if (strcmp(field[1], "BYTE") == 0){
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
	file_line = 46;
	int p2_cnt = 0, total_, line_count = 0, loc_flag = 0, limit = 0, control = 0;
	char loc[file_line][50], f2[4][50], loc_start[50], loc_end[50], temp_file[1000];
	char trans_hex[10], object_file[1000], of_temp[1000], element[6];
	loc_start[0] = '\0';loc_end[0] = '\0';temp_file[0] = '\0';
	im = fopen("immediate_file.txt", "r");
	object_file[0] = 'H';object_file[1] = '\0';
	fscanf(im, "%s%s%s%s", f2[0], f2[1], f2[2], f2[3]);
	strcat(object_file, f2[1]);
	for (i = strlen(object_file); i < 7; i++)
		object_file[i] = ' ';
	int f_count = strlen(f2[3]); // 4
	for (i = 0; i < 6-f_count; i++)
		element[i] = '0';
	for (j = 0; j < f_count; j++)
		element[i++] = f2[3][j];
	strcat(object_file, element);
	strcat(object_file, "00107A");
	printf("%s\n", object_file);
	memset(object_file, 0, 1000);
	fgets(str, 50, im);
	while (!feof(im)){
		int flag2 = 0, f_cnt = 0;
		char opcode[5], final_object_code[10];
		fgets(str, 50, im);
		line_count++;
		for (i = 0; i < strlen(str); i++){
			if (str[i] == '\t' || str[i] == ' ' || str[i] == '\n'){
				f2[flag2][f_cnt] = '\0';
				f_cnt = 0;
				flag2++;
			}
			else
				f2[flag2][f_cnt++] = str[i];
		}
		if (line_count == 1){			// line1 : T001000、T00101E ... 
			object_file[0] = 'T';
			f_count = strlen(f2[0]);
			for (i = 0; i < 6-f_count; i++)
				element[i] = '0';
			for (j = 0; j < f_count; j++)
				element[i++] = f2[0][j];
			strcat(object_file, element);
		}
		int find_o = search_opt(f2[2]);
		strcpy(opcode, opt_num[find_o]);
		if (find_o != -1){
			char find_operand[10];
			if (*f2[3] != 0){
				strcpy(find_operand, search_sym(f2[3]));
				if (*find_operand == 0){
					char type_x[10];
					int check_x = 0;
					strcpy(find_operand, "0000");
					for(i = 0; i < strlen(f2[3]); i++){
						if (f2[3][i] == ','){
							strcpy(find_operand, search_sym(type_x));
							find_operand[0] = change_x(find_operand[0]);
							check_x = 1;
							break;
						}
						type_x[i] = f2[3][i];
					}
					if (check_x == 0)
						printf("Error Flag : Undefined Symbol ");
				}
			}
			else
				strcpy(find_operand, "0000");
			strcpy(final_object_code, opcode);
			strcat(final_object_code, find_operand);
		}
		else if (strcmp(f2[2], "BYTE") == 0){
			if (f2[3][0] == 'C'){
				for (i = 2; f2[3][i] != '\''; i++){
					sprintf(trans_hex, "%x", f2[3][i]);
					strcat(final_object_code, trans_hex);
				}
			}
			else if (f2[3][0] == 'X'){
				int ttc = 0;
				for (i = 2; f2[3][i] != '\''; i++)
					final_object_code[ttc++] = f2[3][i];
			}
		}
		else if (strcmp(f2[2], "WORD") == 0){
			sprintf(trans_hex, "%x", atoi(f2[3]));
			strcpy(final_object_code, trans_hex);
		}
		f_count = strlen(final_object_code);			//create element
		if (f_count > 0 && f_count < 6){
			if (f2[3][0] == 'X')
				strcpy(element, final_object_code);
			else{
				for (i = 0; i < 6-f_count; i++)
					element[i] = '0';
				for (j = 0; j < f_count; j++)
					element[i++] = final_object_code[j];
			}
			strcpy(final_object_code, element);
		}
		strcat(temp_file, final_object_code);
		memset(final_object_code, 0, 50);
		if (line_count == 1){
			if (loc_flag == 1){
				strcpy(loc_end, loc_start);		//start = 101e
				strcpy(loc_start, loc_temp);    //temp = 1033
				loc_flag = 2;
			}
			else{
				strcpy(loc_end, loc_start);
				strcpy(loc_start, f2[0]);
			}
			sprintf(hex_loc, "%x", hex_to_dec(loc_start)-hex_to_dec(loc_end));
		}
		else if (loc_flag == 0 && (strcmp(f2[2], "RESW") == 0 || strcmp(f2[2], "RESB") == 0)){
			strcpy(loc_temp, f2[0]);
			loc_flag = 1;
		}
		if (line_count == 10 || strcmp(f2[2],"END") == 0){
			if (control > 0){
				restore[control-1][7] = hex_loc[0];
				restore[control-1][8] = hex_loc[1];
			}
			strcat(object_file, "  ");
			strcat(object_file, temp_file);
			strcat(restore[control++], object_file);
			memset(object_file, 0, 1000);
			memset(temp_file, 0, 1000);
		}
		if (loc_flag == 2 && line_count == 1){
			strcpy(loc_start, f2[0]);
			loc_flag = 0;
		}
		if (line_count == 10)
			line_count = 0;
		if (strcmp(f2[2],"END") == 0){
			strcpy(loc_end, "207a");
			sprintf(hex_loc, "%x", hex_to_dec(loc_end)-hex_to_dec(loc_start));
			if (strlen(hex_loc) < 2){
				hex_loc[1] = hex_loc[0];
				hex_loc[0] = '0';
			}
			restore[control-1][7] = hex_loc[0];
			restore[control-1][8] = hex_loc[1];
		}
			
	}
//	output_sym();
	for (i = 0; i < control; i++){
		for (j = 0; j < strlen(restore[i]); j++)
			if (restore[i][j] >= 97 && restore[i][j] <= 122)
				restore[i][j] -= 32;
		printf("%s\n", restore[i]);
	}
	memset(object_file, 0, 50);
	object_file[0] = 'E';
	for (i = 1; i < 7; i++)
		object_file[i] = restore[0][i];
	printf("%s\n", object_file);
	fclose(im);
	return 0;
}

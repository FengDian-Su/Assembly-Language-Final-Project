#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OPMAX 59    // set the number of optable 

char opt_name[OPMAX][50], opt_num[OPMAX][50], loc_temp[50], hex_loc[5], restore[50][1000], r2[50][4], end_loc[10];
// symbol table using link list structure
typedef struct symtable *node;  
struct symtable{
	char name[10];
	char num[10];
	node next;
};
node head = NULL, tail = NULL;
// insert and create symbol table
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
// check if label is in symbol table
int is_sym(char temp_name[]){
	node ptr = head;
	while (ptr){
		if (strcmp(ptr->name, temp_name) == 0)
			return 1;
		ptr = ptr->next;
	}
	return 0;
}
// update symbol label address
void update_sym(char temp_name[], char temp_num[]){
	node ptr = head;
	while (ptr){
		if (strcmp(ptr->name, temp_name) == 0)
			strcpy(ptr->num, temp_num);
		ptr = ptr->next;
	}	
}
// return label's address
char* search_sym(char temp_name[]){
	node ptr = head;
	while (ptr){
		if (strcmp(ptr->name, temp_name) == 0)
			return ptr->num;
		ptr = ptr->next;
	}
	return "";
}
// output and free whole symbol table
void output_sym(){
	node ptr = head, temp;
	while (ptr){
		printf("%s\t%s\n", ptr->name, ptr->num);
		temp = ptr;
		ptr = ptr->next;
		free(temp);
	}
}
// return opcode
int search_opt(char temp_name[]){
	int i;
	for (i = 0; i < OPMAX; i++)
		if (strcmp(temp_name, opt_name[i]) == 0)
			return i;
	return -1;
}
// change x bit when indexed addressing mode
char change_x(char error){
	int cx[4], i, temp = error - '0', add = 0, cnt = 1;
	for (i = 3; i >= 0; i--, temp/=2)
		cx[i] = temp%2;
	cx[0] = 1;
	for (i = 3; i >= 0; i--, cnt*=2)
		add += cx[i]*cnt;
	return (add+'0');
}
// translate from hex to dec
int hex_to_dec(char ary[]){
	int trans[strlen(ary)], i, add = 0, mul = 1;
    for (i = 0; i < strlen(ary); i++)
   		switch(ary[i]){
		   	case 'a':trans[i] = 10;break;
		   	case 'b':trans[i] = 11;break;
		   	case 'c':trans[i] = 12;break;
		   	case 'd':trans[i] = 13;break;
		   	case 'e':trans[i] = 14;break;
		   	case 'f':trans[i] = 15;break;
		   	default:trans[i] = ary[i]-'0';
		}
   for (i = strlen(ary)-1; i >= 0; i--, mul*=16)
   		add += trans[i]*mul;
   	return add;
}
// translate to uppercase
void upper_case(char temp_name[]){
	int j;
	for (j = 0; j < strlen(temp_name); j++)
		if (temp_name[j] >= 97 && temp_name[j] <= 122)
			temp_name[j] -= 32;
}
// ot: optable, fs: source file, im: immediate file, op: object file
int main(){
	FILE *ot, *fs, *im, *op;
	int i, j, record_opt = 0, initial = 0, locnum = 0;
	// read and restore optable
	ot = fopen("op_table.txt", "r");
	while (!feof(ot)){
		fscanf(ot, "%s%s", opt_name[record_opt], opt_num[record_opt]);
		record_opt++;
	}
	fclose(ot);
	// path1 : read source file and create immediate file
	fs = fopen("source_code.txt", "r");
	im = fopen("immediate_file.txt", "a");
	char field[3][50], locctr[10], str[50];
	while (!feof(fs)){
		int flag = 0, cin = 0;
		char output_line[50];
		fgets(str, 50, fs);
		if (str[0] == '.') // if this is a comment line
			continue;
		for (i = 0; i < strlen(str), flag < 3; i++){  // cut the string to three fields
			if (str[i] == '\t' || str[i] == '\n'){
				field[flag][cin] = '\0';
				cin = 0;
				flag++;
			}
			else
				field[flag][cin++] = str[i];
		}
		if (initial == 0){    // only do for first line
			if (strcmp(field[1], "START") == 0)
				strcpy(locctr, field[2]);  // start locctr
			else
				strcpy(locctr, "0\0");  // if no assign, initial with 0
			int trans_temp = atoi(locctr);
			locnum = 1;
			while (trans_temp >= 1){   // translate from hex to dec 
				locnum *= 16;
				trans_temp /= 16;
			}
			initial = 1;
		}
		sprintf(locctr, "%x", locnum);  // generate locctr
		strcpy(output_line, locctr);
		for (i = 0; i < 3; i++)
			strcat(strcat(output_line, "\t"), field[i]);
		strcat(output_line, "\n");
		update_sym(field[0], locctr);
		if (strcmp(field[1], "END") == 0){
			strcpy(end_loc, locctr);   // set program final location
			memset(output_line, 0, 50);
			strcat(strcat(output_line, "\t"), str);
//			printf("%s", output_line);
			fputs(output_line, im);    // write into immediate file
			break;
		}	
//		printf("%s", output_line);
		fputs(output_line, im);    // write into immediate file
		if (initial == 1){
			if (*field[0] != 0){
				if (is_sym(field[0]) == 1)    // duplicate symbol detect
					printf("Error Flag : Duplicate symbol !\n");
				else if (is_sym(field[0]) == 0)
					insert_sym(field[0], locctr);  // add into symtab
			}
			if (search_opt(field[1]) != -1 || strcmp(field[1], "WORD") == 0)
				locnum += 3;    // common or WORD condition
			else{    // special case
				if (strcmp(field[1], "RESW") == 0)
					locnum += 3*atoi(field[2]);
				else if (strcmp(field[1], "RESB") == 0)
					locnum += atoi(field[2]);
				else if (strcmp(field[1], "BYTE") == 0){ // C and X mode
					char operand_type = field[2][0];
					if (operand_type == 'C')
						locnum += (strlen(field[2])-3);
					else if (operand_type == 'X')
						locnum += (strlen(field[2])-3)/2;
				}
			}
		}
	}
	fclose(fs);
	fclose(im);
	memset(str, 0, 50);
//	printf("\n");
	//path 2 : read immediate file and create object file
	int line_count = 0, loc_flag = 0, control = 0;
	char f2[4][50], object_file[100], loc_start[10], loc_end[10], temp_file[100], element[6], trans_hex[10];
	loc_start[0] = loc_end[0] = temp_file[0] = '\0';
	im = fopen("immediate_file.txt", "r");
	op = fopen("object_file.txt", "a");
	strcpy(object_file, "H\0");  // add H
	fscanf(im, "%s%s%s%s\n", f2[0], f2[1], f2[2], f2[3]);
	strcat(object_file, f2[1]);    // add file name
	for (i = strlen(object_file); i < 7; i++) // add space to specified size
		object_file[i] = ' ';
	memset(element, '0', 6);    // adjust element size to 6
	for (i = 6-strlen(f2[3]); i < 6; i++)
		element[i] = f2[3][i+strlen(f2[3])-6];  // add start locctr
	strcat(object_file, element);
	sprintf(hex_loc, "%x", hex_to_dec(end_loc)-hex_to_dec(f2[3]));
	upper_case(hex_loc);    // calculate program total length
	memset(element, '0', 6);    // adjust hex_loc size to 6
	for (i = 6-strlen(hex_loc); i < 6; i++)
		element[i] = hex_loc[i+strlen(hex_loc)-6];
	strcat(object_file, element);
//	printf("%s\n", object_file);
	fprintf(op, "%s\n", object_file);  // write into object file
	memset(object_file, 0, 100);
	while (!feof(im)){
		int flag2 = 0, f_cnt = 0;
		char opcode[5], final_object_code[10];
		fgets(str, 50, im);
		line_count++;    // count from 1 to 10
		for (i = 0; i < strlen(str); i++){  // cut the string to three fields
			if (str[i] == '\t' || str[i] == '\n'){
				f2[flag2][f_cnt] = '\0';
				f_cnt = 0;
				flag2++;
			}
			else
				f2[flag2][f_cnt++] = str[i];
		}
		if (line_count == 1){    // start new line in object file
			object_file[0] = 'T';
			memset(element, '0', 6);
			for (i = 6-strlen(f2[0]); i < 6; i++)
				element[i] = f2[0][i+strlen(f2[0])-6];  // add start locctr
			strcat(object_file, element);
		}
//		printf("%s\t%s\t%s\t%s\t", f2[0], f2[1], f2[2], f2[3]);
		int find_o = search_opt(f2[2]);    // find opcode_index
		strcpy(opcode, opt_num[find_o]);
		if (find_o != -1){    // if the operation is exist
			char find_operand[10];
			if (*f2[3] != 0){    // if operand exist
				strcpy(find_operand, search_sym(f2[3]));  // search operand
				if (*find_operand == 0){    // if not found in symtab
					char type_x[10];
					int check_x = 0;
					memset(find_operand, '0', 4);  // store 0
					for(i = 0; i < strlen(f2[3]); i++){
						if (f2[3][i] == ','){     // check indexed addressing
							strcpy(find_operand, search_sym(type_x));
							find_operand[0] = change_x(find_operand[0]);
							check_x = 1;  // correct
							break;
						}
						type_x[i] = f2[3][i];  // insert until ','
					}
					if (check_x == 0) // still wrong
						printf("Error Flag : Undefined Symbol ");
				}
			}
			else    // if operand not exist
				memset(find_operand, '0', 4);  // store 0
			strcat(strcpy(final_object_code, opcode), find_operand);  // finish object code
		}
		else if (strcmp(f2[2], "BYTE") == 0){
			if (f2[3][0] == 'C')    // C case: add f2[3] in ASCII type (Hex)
				for (i = 2; f2[3][i] != '\''; i++){
					sprintf(trans_hex, "%x", f2[3][i]);
					strcat(final_object_code, trans_hex);
				}
			else if (f2[3][0] == 'X'){    // X case: add f2[3]
				int ttc = 0;
				for (i = 2; f2[3][i] != '\''; i++)
					final_object_code[ttc++] = f2[3][i];    // finish object code
			}
		}
		else if (strcmp(f2[2], "WORD") == 0){  // WORD case: add f2[3] in ASCII type (Hex)
			sprintf(trans_hex, "%x", atoi(f2[3]));
			strcpy(final_object_code, trans_hex);
		}
		int f_count = strlen(final_object_code);    // adjust element size to 6
		if (f_count > 0 && f_count < 6 && f2[3][0] != 'X'){
			memset(element, '0', 6);
			for (i = 6-f_count; i < 6; i++)
				element[i] = final_object_code[i+f_count-6];
			strcpy(final_object_code, element);
		}
//		printf("%s\n", final_object_code);
		strcat(temp_file, final_object_code);  // restore final_object_code
		memset(final_object_code, 0, 10);      // reset final_object_code
		if (line_count == 1){
			strcpy(loc_end, loc_start);	       // swap end and last-start
			if (loc_flag == 1){                // flag1: RESW/RESB
				strcpy(loc_start, loc_temp);
				loc_flag = 2;
			}
			else
				strcpy(loc_start, f2[0]);      // else: normal
			sprintf(hex_loc, "%x", hex_to_dec(loc_start)-hex_to_dec(loc_end));
		}
		else if (loc_flag == 0 && (strcmp(f2[2], "RESW") == 0 || strcmp(f2[2], "RESB") == 0)){
			strcpy(loc_temp, f2[0]);
			loc_flag = 1;
		}
		if (line_count == 10 || strcmp(f2[2],"END") == 0){
			if (control > 0){    // in the first time, no need to calculate the hex_loc
				restore[control-1][7] = hex_loc[0];
				restore[control-1][8] = hex_loc[1];  // restore the offset end to start
			}
			strcat(object_file, "  ");  // initial with empty
			strcat(object_file, temp_file);
			strcat(restore[control++], object_file);
			memset(object_file, 0, 100);
			memset(temp_file, 0, 100);
		}
		if (loc_flag == 2 && line_count == 1){  // after RESW/RESB
			strcpy(loc_start, f2[0]);
			loc_flag = 0;
		}
		if (line_count == 10)
			line_count = 0;
		if (strcmp(f2[2],"END") == 0){    // end of the program
			sprintf(hex_loc, "%x", hex_to_dec(end_loc)-hex_to_dec(loc_start));
			if (strlen(hex_loc) < 2){    // adjust hex_loc size to 2
				hex_loc[1] = hex_loc[0];
				hex_loc[0] = '0';
			}
			restore[control-1][7] = hex_loc[0];
			restore[control-1][8] = hex_loc[1];  // restore the offset end to start
		}
	}
	for (i = 0; i < control; i++){  // control is number of line in object file
		upper_case(restore[i]);
//		printf("%s\n", restore[i]);
		fprintf(op, "%s\n", restore[i]);  // write into object file
	}
	memset(object_file, 0, 100);
	object_file[0] = 'E';    // start with E
	for (i = 1; i < 7; i++)  // starting position
		object_file[i] = restore[0][i];
//	printf("%s\n", object_file);
	fprintf(op, "%s", object_file);  // \n is no need
	output_sym();
	fclose(op);
	fclose(im);
	return 0;
}

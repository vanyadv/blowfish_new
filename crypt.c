#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "blowfish.h"
char chr[50]; 
unsigned long left, right;
char test_key[20];
blowfish_vars *vars;
int changer = 0;
void swap(unsigned long *a, unsigned long *b)
{
    unsigned long temp;

    if (a && b)
        temp = *a, *a = *b, *b = temp;
}
unsigned long Function_x(blowfish_vars *chr, unsigned long x)
{
    return ((chr->sbox[0][(x >> 24) & 0xFF] + chr->sbox[1][(x >> 16) & 0xFF]) ^ chr->sbox[2][(x >> 8) & 0xFF]) + chr->sbox[3][(x) & 0xFF];
}
void crypt(blowfish_vars *vars, unsigned long *left, unsigned long *right) 
{   
    int i;
    for (i = 0; i < 16; i++)   
    {
        *left ^= vars->P[i]; 
        *right ^= Function_x(vars, *left);  
        swap(right, left);    
    }
    swap(right, left);
    *right ^= vars->P[16]; 
    *left ^= vars->P[17];    
}
int inicialization(blowfish_vars *vars, unsigned char *left, size_t left_len)
{
    int i, j;
    unsigned long k, l;
    unsigned long long_left;
    if (vars && left && left_len > 0 && left_len <= 56)
    {
        memcpy(vars->P, FIXED_P, 18 * sizeof(FIXED_P));
        for (i = 0; i < 4; i++)
            memcpy(vars->sbox[i], FIXED_S[i], sizeof(FIXED_S[i]));
        for (i = 0, k = 0; i < 18; i++)
        {
            for (j = 0, long_left = 0; j < 4; j++, k++)
                long_left = (long_left << 8) | left[k % left_len];
            vars->P[i] ^= long_left;
        }
        for (i = 0, k = 0, l = 0; i < 18; i++)
        {
            crypt(vars, &k, &l);
            vars->P[i] = k;
            vars->P[++i] = l;
        }
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 256; j++)
            {
                crypt(vars, &k, &l);
                vars->sbox[i][j] = k;
                vars->sbox[i][++j] = l;
            }
        }
        return 0;
    }
    return -1;
}
WIN32_FIND_DATA Find_file;  
int open_file(char *file1, char *file2 )
{

    HANDLE hFile;
    hFile = FindFirstFile(file1, &Find_file);
    if (hFile == NULL)
    {
        printf("error 1\n");
        return 0;
    }
    int size = Find_file.nFileSizeLow;
    int all_text = 0;
    unsigned long buffer;
    byte flag = 1;
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "wb");
    if (f1 == NULL || f2 == NULL)
    {
        printf("invalid  file!!!\n");
        return 0;

    }
            while (fread(&buffer, 4, 1, f1))
            {
                if (flag == 1)
                {
                    left = buffer;
                    flag = 2;
                }
                else
                {
                    right = buffer;
                    flag = 1;
                    crypt(vars, &left, &right);
                    fwrite(&left, 4, 1, f2);
                    fwrite(&right, 4, 1, f2);
                }   
                
                all_text += 4;        
            }

            if(size > all_text)
            {
                if(flag == 1)
                {
                    fread(&buffer, (size - all_text), 1, f1);
                    right = buffer;
                    crypt(vars, &left, &right);
                    fwrite(&left, 4, 1, f2);
                    fwrite(&right, 4, 1, f2);
                }
                else
                {
                    fread(&buffer, (size - all_text), 1, f1);
                    left = buffer;
                    right = 0;
                    crypt(vars, &left, &right);
                    fwrite(&left, 4, 1, f2);
                    fwrite(&right, 4, 1, f2);
                    printf("key= %x\n", left); 
                 
                }
            }
    fclose(f1);
    fclose(f2);
    return 1;
}

int main(int argc, char **argv)
{
    unsigned long key;
    char inp[50];
    char out[50];
    char Key[50];
    char test[50];
    int cycle;
    int new;
    instruction();
    vars = (blowfish_vars *)malloc(sizeof(blowfish_vars));
    printf("File for crypt: \n");
    scanf("%s",inp);
    printf("Crypt file: \n");
    scanf("%s",out);
     if (!vars)
    {
        puts("Insufficient memory allocated");
        return -1;
    }

    if (inicialization(vars, (unsigned char *)"TESTKEY", 7) != 0)
    {
        free(vars);
        puts("Key initialization failed");
        return -1;
    }
 FILE *TESTKEY = fopen("Key.txt", "w");
    open_file(inp,out);
        fprintf(TESTKEY,"%x",left);  
    fclose(TESTKEY);
        free(vars);
    system("PAUSE");
    return 0;
}
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct
{
    double* begin;
    double* end;
    size_t size;
} S_tack;

typedef struct
{
    void* data;
    long size;
} Polis;

typedef struct
{
    double value;
    char name[7];
} Variable;

typedef struct
{
    char* example;
    size_t size;
} Expression;

typedef int (*Calculate_elem)(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1);

int f_new_process(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1);
int f_kill_process(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1);

int f_add_to_stack(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1); // важно

int f_fd_read(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1);
int f_fd_write(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1);

int f_check_and(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1);

int f_check_or(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1);

int f_l(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1);
int f_file(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1);
int f_r(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1);
int f_rr(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1);

int f_pipe(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1);


int p_chng_str(Polis* pol, Expression* s, int* skobka_l, int* skobka_r, int* operation, int* size, int fd_save[2], int* flag_file);
int p_log(Polis* pol, Expression* s, int* skobka_l, int* skobka_r, int* operation, int* size, int fd_save[2], int* flag_file);
int p_pipe(Polis* pol, Expression* s, int* skobka_l, int* skobka_r, int* operation, int* size, int fd_save[2], int* flag_file);
int p_file(Polis* pol, Expression* s, int* skobka_l, int* skobka_r, int* operation, int* size, int fd_save[2], int* flag_file);

int p_chng_str(Polis* pol, Expression* s, int* skobka_l, int* skobka_r, int* operation, int* size, int fd_save[2], int* flag_file)
{
    int flag_chislo = 0;
    char a = ' ';
    while(1)
    {
        a = *(s->example);
        if ((a == '\n'))
        {
            return 0;
        }
        if (a == '(')
        {
            *skobka_l += 1;
            (s->example)++;
            int op = 0;
            int f_file = 0;

            pol->data = realloc(pol->data, 3*sizeof(double) + pol->size);
            ((long*)pol->data)[pol->size] = 8;
            ((Calculate_elem*)pol->data)[pol->size + sizeof(double)] = f_new_process;
            ((double*)pol->data)[pol->size + 2*sizeof(double)] = 0;
            pol->size += 3*sizeof(double);
            //
            int f_s[2];
            if(p_log(pol, s, skobka_l, skobka_r, &op, size, f_s, &f_file))
            {
                return 1;
            }
            a = *(s->example);
        }
        
        if (a == ')')
        {
            *skobka_r += 1;
            (s->example)++;
            a = *(s->example); 
            flag_chislo = 1;

            pol->data = realloc(pol->data, 3*sizeof(double) + pol->size);
            ((long*)pol->data)[pol->size] = 8;
            ((Calculate_elem*)pol->data)[pol->size + sizeof(double)] = f_kill_process;
            ((double*)pol->data)[pol->size + 2*sizeof(double)] = 0;
            

            ((long*)pol->data)[pol->size + 3*sizeof(double)] = 8;
            ((Calculate_elem*)pol->data)[pol->size + 3*sizeof(double) + sizeof(double)] = f_fd_read;
            ((long*)pol->data)[pol->size + 3*sizeof(double) + 2*sizeof(double)] = (long)0;
            

            ((long*)pol->data)[pol->size + 3*sizeof(double) + 3*sizeof(double)] = 8;
            ((Calculate_elem*)pol->data)[pol->size + 3*sizeof(double) + 4*sizeof(double)] = f_fd_write;
            ((long*)pol->data)[pol->size + 3*sizeof(double) + 5*sizeof(double)] = (long)1;
            
            pol->size += 9*sizeof(double);
            
        }
        if ((a != '|') && (a != '&') && (a != '>') && (a != ')') && (a != '(') && (a != '<') && (a != '\n') && (flag_chislo == 0) )
        {
            char* str;
            int str_size = 0;
            str = malloc(sizeof(char));
            while (a == ' ')
            {
                (s->example)++;
                a = *(s->example);
            }
            while ((a != '|') && (a != '&') && (a != '>') && (a != ')') && (a != '\n') && (a != '(') && (a != '<'))
            {
                str[str_size] = a;
                str_size += 1;
                str = realloc(str, sizeof(char)*(str_size+1));
                (s->example)++;
                a = *(s->example);
            }
            str[str_size] = '\0';
            int f;
            if (*flag_file != 0)
            {
                if (*flag_file == 1) // >> 
                {
                    f = open(str, O_WRONLY | O_CREAT | O_APPEND, 0666);
                    *(long*)(&((char*)pol->data)[pol->size - sizeof(char*)]) = (long)f; //изменили дескриптор записи предыдущей программы
                }
                else if (*flag_file == 2) // >
                {
                    f = open(str, O_WRONLY | O_CREAT | O_TRUNC, 0666); 
                    *(long*)(&((char*)pol->data)[pol->size - sizeof(char*)]) = (long)f; //изменили дескриптор записи предыдущей программы
                }
                else if (*flag_file == 3) // <
                {
                    f = open(str, O_RDONLY); 
                    *(long*)(&((char*)pol->data)[pol->size - 4*sizeof(char*)]) = (long)f; //изменили дескриптор чтения предыдущей программы
                }
                printf("f = %d\n", f);
                if (f == -1)
                {
                    printf("OPEN()\n");
                    return 1;
                }
            }
            if ((str_size + 1) % 8 != 0)
            {
                str = realloc(str, sizeof(char)*(str_size + 1 + 8 - ((str_size + 1) % 8)));
                for (int i = str_size + 1; i < str_size + 1 + 8 - ((str_size + 1) % 8); i++)
                {
                    str[i] = '*';
                }
                str[str_size + 8 - ((str_size + 1) % 8)] = '\0';
                str_size += 1 + 8 - ((str_size + 1) % 8);
            }

            flag_chislo = 1;
            // тут нужно заложить утилиту с параметрами и файлами в полиз + заложить дескрипторы
            
            pol->data = realloc(pol->data, 2*sizeof(char*) + str_size + 3*sizeof(char*) + 3*sizeof(char*) + pol->size);
            *(long*)(&((char*)pol->data)[pol->size]) = (long)str_size;
            if (*flag_file != 0)
            {   
                *(Calculate_elem*)(&((char*)pol->data)[pol->size + sizeof(char*)]) = f_file;
            }
            else
            {
                *(Calculate_elem*)(&((char*)pol->data)[pol->size + sizeof(char*)]) = f_add_to_stack;
            }
   
            for (int i = 0; i < str_size; i++)
            {
                ((char*)pol->data)[pol->size + 2*sizeof(char*) + i] = str[i];
            }
            // дескриптор на чтение
            
            *(long*)(&((char*)pol->data)[pol->size + 2*sizeof(char*) + str_size]) = (long)8;
            *(Calculate_elem*)(&((char*)pol->data)[pol->size + 2*sizeof(double) + str_size + sizeof(double)]) = f_fd_read;
            *(long*)(&((char*)pol->data)[pol->size + 2*sizeof(double) + str_size + 2*sizeof(double)]) = (long)0;
            
            // дескриптор на запись
            *(long*)(&((char*)pol->data)[pol->size + 2*sizeof(double) + str_size + 3*sizeof(double)]) = (long)8;
            *(Calculate_elem*)(&((char*)pol->data)[pol->size + 2*sizeof(double) + str_size + 4*sizeof(double)]) = f_fd_write;
            *(long*)(&((char*)pol->data)[pol->size + 2*sizeof(double) + str_size + 5*sizeof(double)]) = (long)1;
            
            *flag_file = 0;
            pol->size += 2*sizeof(double) + str_size + 3*sizeof(double) + 3*sizeof(double);
  
            free(str);
            
        }
        while ((a == ' ') || (a == '\r') || (a == '\t')) // \n - признак конца строки 
        {
            (s->example)++;
            a = *(s->example);
        }
        if ((*(s->example) == '\n')) // дошли до конца строки
        {
            *operation = 0;
            break;
        }
        if (flag_chislo && ( (a == '|') || !strncmp("||", s->example, 2) || !strncmp("&&", s->example, 2) || (a == '<') || (a == '>') || !strncmp(">>", s->example, 2)))
        {
            flag_chislo = 0;
            if (!strncmp("||", s->example, 2))
            {
                *operation = 3;
                (s->example) += 2;
                // рассказываем полизу, что первая часть логической операции закончена и что, возможно, наша работа на эта должна быть закончена
                pol->data = realloc(pol->data, 3*sizeof(double) + pol->size);
                *(long*)(&((char*)pol->data)[pol->size]) = (long)8;
                *(Calculate_elem*)(&((char*)pol->data)[pol->size + sizeof(double)]) = f_check_or;
		        *(long*)(&((char*)pol->data)[pol->size + 2*sizeof(double)]) = (long)0;
		        pol->size += 3*sizeof(double);
                return 0;
            }
            if (!strncmp("&&", s->example, 2))
            {
                *operation = 2;
                (s->example) += 2;
                // рассказываем полизу, что первая часть логической операции закончена и что, возможно, наша работа на эта должна быть закончена
                pol->data = realloc(pol->data, 3*sizeof(double) + pol->size);
		        pol->data = realloc(pol->data, 3*sizeof(double) + pol->size);
                *(long*)(&((char*)pol->data)[pol->size]) = (long)8;
                *(Calculate_elem*)(&((char*)pol->data)[pol->size + sizeof(double)]) = f_check_and;
		        *(long*)(&((char*)pol->data)[pol->size + 2*sizeof(double)]) = (long)0;
		        pol->size += 3*sizeof(double);
                return 0;
            }
            if (!strncmp(">>", s->example, 2))
            {
                *operation = 6;
                *flag_file = 1;
                (s->example) += 2;
                return 0;
            }
            if (a == '|')
            {
                *operation = 1;
                (s->example)++;
                // открываем канал и сохраняем его дискрипторы
                int fd[2];
                if(pipe(fd)) {};
                fd_save[0] = fd[0];
                fd_save[1] = fd[1];
                printf("fd[0] = %d\n", fd[0]);
                printf("fd[1] = %d\n", fd[1]);
                // меняем дескриптор на запись у pr1 (это также могут быть скобки)
                *(long*)(&((char*)pol->data)[pol->size - sizeof(double)]) =(long)fd[1];
                
                return 0;
            }
            if (a == '>')
            {
                *operation = 5;
                *flag_file = 2;
                (s->example)++;
                return 0;
            }
            if (a == '<')
            {
                *operation = 4;
                *flag_file = 3;
                (s->example)++;
                return 0;
            }
        }
        *operation = 0;
        break;

    }
    return 0;
}

int p_file(Polis* pol, Expression* s, int* skobka_l, int* skobka_r, int* operation, int* size, int fd_save[2], int* flag_file)
{
    int op = 0; // тут храним предыдущую операцию
    //int fd_s[2]; // тут храним дескрипторы потоков
    while ( (*(s->example) != '\n'))
    {
        if (p_chng_str(pol, s, skobka_l, skobka_r, operation, size, fd_save, flag_file))
        {
            return 1;
        }

        if (op >= 4) // если прeдыдущая операция - перенаправление из программы в файл, то заложим все красивенько в полиз
        {
            pol->data = realloc(pol->data, 3*sizeof(double) + pol->size);
            *(long*)(&((char*)pol->data)[pol->size]) =(long)8;
            *(long*)(&((char*)pol->data)[pol->size + 2*sizeof(double)]) =(long)0;
            if (op == 4)
            {
                *(Calculate_elem*)(&((char*)pol->data)[pol->size + sizeof(double)]) = f_l;
            }
            else if (op == 5)
            {
                *(Calculate_elem*)(&((char*)pol->data)[pol->size + sizeof(double)]) = f_r;
            }
            else if (op == 6)
            {
                *(Calculate_elem*)(&((char*)pol->data)[pol->size + sizeof(double)]) = f_rr;
            }
            pol->size += 3*sizeof(double);
            // дескрипторы на запись в файл меняем уже стеком 
        }
        if ((*(s->example) == '\n'))
        {
            return 0;
        }
        
        if (*operation == 1) // нас выбивает наверх, в p_pipe
        {
            return 0;
        }
        else if (*operation >= 2) // нас выбивает наверх, в p_pipe, а потом в p_log
        {
            return 0;
        }
        op = *operation;
    }
    return 0; 
}





int p_pipe(Polis* pol, Expression* s, int* skobka_l, int* skobka_r, int* operation, int* size, int fd_save[2], int* flag_file)
{
    int op = 0;
    int fd_s[2];
    while ( (*(s->example) != '\n')) 
    {
        if (p_file(pol, s, skobka_l, skobka_r, operation, size, fd_save, flag_file))
        {
            return 1;
        }
        if (op == 1) // предыдущая операция - |
        {
            *(long*)(&((char*)pol->data)[pol->size - 4*sizeof(char*)]) = (long)fd_s[0]; //изменили дескриптор чтения для процесса 
            // дескриптор записи меняем, как только встретили | (или оставляем 0 по умолчанию, если не встретили)
        }
        if (*operation == 1)
        {
            fd_s[0] = fd_save[0];
            fd_s[1] = fd_save[1];
        }
        if ((*operation == 2) || (*operation == 3)) // логическая операция - выбиваемся наверх в pipe_log
        {
            return 0;
        }
        op = *operation;
    }
    return 0; 
}


int p_log(Polis* pol, Expression* s, int* skobka_l, int* skobka_r, int* operation, int* size, int fd_save[2], int* flag_file)
{
    //int fd_s[2];
    while ( (*(s->example) != '\n'))
    {
        if (p_pipe(pol, s, skobka_l, skobka_r, operation, size, fd_save, flag_file))
        {
            return 1;
        }
        // логические операции мы заложили в полиз еще в p_chng_str - проверять части выражений на истиннойсть будем в стеке
    }
    return 0;
}

void deleter(char* stck, Polis* pol, char* s_1, char* s_2)
{
    if (stck != NULL)
    {
        free(stck);
    }
    if (pol != NULL)
    {
        free(pol->data);
    }
}
int f_add_to_stack(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1)
{
    char** mas;
    int mas_size = 0;
    char* s;
    int s_s = 0;
    s = malloc(1);
    int i = 0;
    int indx = 0;
    mas = malloc(8);
    while (1)
    {
        if (((elem[i] == ' ') || (elem[i] == '\0')) && (s_s != 0))
        {
            mas = realloc(mas, (indx + 1)*8);
            mas[indx] = malloc(s_s+1);
            mas[indx][s_s] = '\0';
            s[s_s] = '\0';
            strcpy(mas[indx], s);

            indx++;
            free(s);
            mas_size += s_s + 1;
            s_s = 0;
            s = malloc(1);
        }
        else if (elem[i] != ' ')
        {
            s[s_s] = elem[i];
            s_s += 1;
            s = realloc(s, s_s + 1);
        }
        if (elem[i] == '\0')
        {
            break;
        }
        i++;
    }
    free(s);
    mas = realloc(mas, (indx + 1)*8);
    mas[indx] = NULL;
    
    char** safer;
    safer = mas;

    if (fork() == 0)
    {
        if ((int)(*fd_0) > 0)
        {
            dup2((int)(*fd_0), 0);
        }
        if ((int)(*fd_1) > 1)
        {
            dup2((int)(*fd_1), 1);
        }
        if (*fd_1 != 1)
        {
            close((int)(*fd_1));
        }
        else if (*fd_0 != 0)
        {
            close((int)(*fd_0));
        }
        execvp(mas[0], mas);
    }
    int status;
    wait(&status);
    
    if ((*(Calculate_elem*)(&((char*)stack->data)[*size]) == f_check_or))
    {
        printf("***\n");
        if ((WIFEXITED(status) == 1) && (WEXITSTATUS(status) == 0))
        {
            return 1;
        }
    }
    else if ((*(Calculate_elem*)(&((char*)stack->data)[*size]) == f_check_and)) // &&
    {
        if ((WIFEXITED(status) == 1) && (WEXITSTATUS(status) != 0))
        {
            return 1;
        }
    }
    
    for (int j = 0; j <= indx; j++)
    {
        free(safer[j]);
    }
    free(safer);
    
    
    
    return 0;
}

int f_file(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1)
{
   return 0;
}

int f_l(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1)
{ 
    return 0;
}

int f_r(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1)
{
    return 0;
}

int f_rr(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1)
{  
    return 0;
}
int f_check_and(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1)
{
    return 0;
}

int f_check_or(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1)
{
    return 0;
}
int f_fd_write(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1)
{
    return 0;
}
int f_fd_read(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1)
{
    return 0;
}
int f_kill_process(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1)
{
    return 0;
}
int f_new_process(char* elem, Polis* stack, int* size, long* fd_0, long* fd_1)
{
    return 0;
}

void 
initialize_polis(Polis* t, size_t s)
{
    t->data = malloc(1);
    t->size = 0;
}

int main(void)
{
    Polis p;
    initialize_polis(&p, 1); //////////////////////////////////////////////////////////
    Polis* pol;
    pol = &p;
    Expression s, w;
    s.example = malloc(1);
    s.size = 0;
    int y;
    while ((y = getchar()) != '\n')
    {
        s.example[s.size] = y;
        s.size += 1;
        s.example = realloc(s.example, s.size+1);
    }
    s.example[s.size] = '\n';//realloc(s.example, s.size);
    w.example = s.example;
    int skobka_l = 0, skobka_r = 0, op = 0, size = 0, fl_fg = 0;
    int fd[2];
    if(p_log(pol, &s, &skobka_l, &skobka_r, &op, &size, fd, &fl_fg))
    {
        deleter(NULL, pol, w.example, NULL);
        return 1;
    }
    for (int i = 8; i < pol->size; )
	{
	    if (*(Calculate_elem*)(&((char*)pol->data)[i]) == f_new_process)
	    {
		
	    }
	    else if (*(Calculate_elem*)(&((char*)pol->data)[i]) == f_kill_process)
	    {
		
	    }
	    else if ( (*(Calculate_elem*)(&((char*)pol->data)[i]) == f_add_to_stack) )
	    {
		    if  ( (*(Calculate_elem*)(&(((char*)pol->data)[i])))(((char*)pol->data) + i + sizeof(double), pol, &i,
		    (long*)((char*)pol->data + i + 3*sizeof(double) + *(long*)(&((char*)pol->data)[i - sizeof(double)]) ),
		    (long*)((char*)pol->data + i + 6*sizeof(double) + *(long*)(&((char*)pol->data)[i - sizeof(double)]) ) ) )
		    {
			    deleter(s.example, pol, NULL, NULL);
			    return 1;
		    }
	    }

	    // прибавили размер элемента, размер сслыки на функцию и размер ссылки на размер элемента    
	    i += 2*sizeof(double) + *(long*)(&((char*)pol->data)[i - sizeof(double)]);
	}
}



/*
File: ex7b.c
Interactions Between Prime Checker, Palindrome Checker and Client
=====================================================================
Written by: Tali Kalev, ID:208629691, Login: talikal
		and	Noga Levy, ID:315260927, Login: levyno

The main process creates three threads and ends. The first thread receives an
integer via a global array and checks whether it is prime or not. The second
thread receives a string via a global array and checks whether the string
is a palindrome or not. The third thread reads from user p or q and then an
integer or string respectively. The thread puts the integer/string into
an appropriate global array and waits for the answer from the appropriate
thread. When it receives the answer, it prints it to the standard output.

Compile: gcc ex7b.c -o ex7b -lpthread

Run:  ./ex7b

Input: p or q. If input is p and then integer.
       If input is q and then a series of chars ending in newline.
        Example: p
                 7

Output: Whether the integer/string is a prime/palindrome.
        Example:
		is prime ? yes

Race Modes: there are no race modes here.

*/

// --------include section------------------------

#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#define NUM_OF_CHILD 3
#define SIZE 2
#define STR_SIZE 100
#define RES 0
#define START_NUM 1

// --------const and enum section------------------------

enum threads {PRIME = 0, PALI, CLIENT};

pthread_t child_id[NUM_OF_CHILD];
int prime_arr[SIZE];
char *pali_arr[SIZE];

// --------prototype section------------------------

void create_threads_and_die();
void *read_data(void *arg);
void *handle_prime(void *arg);
void *handle_pali(void *arg);

int is_pali(char str[], int n);
int is_prime(int num);
void perror_and_exit(char *msg);
void p_request();
void q_request();

void catch_sigusr1(int sig_num);
void catch_sigusr2(int sig_num);
void catch_sigint(int sig_num);

// --------main section------------------------

int main()
{
    signal(SIGINT, catch_sigint);
    signal(SIGUSR1, catch_sigusr1);
    signal(SIGUSR2, catch_sigusr2);

    create_threads_and_die();

    return EXIT_SUCCESS;
}

//-------------------------------------------------

void create_threads_and_die()
{
    int status;

    status = pthread_create(&child_id[PRIME], NULL, handle_prime, NULL); //creating thread
    if(status != 0)
        perror_and_exit("p_thread_create\n");

    status = pthread_create(&child_id[PALI], NULL, handle_pali, NULL);
    if(status != 0)
        perror_and_exit("p_thread_create\n");

    status = pthread_create(&child_id[CLIENT], NULL, read_data, NULL);
    if(status != 0)
        perror_and_exit("p_thread_create\n");

    pthread_exit(NULL);

}

//-------------------------------------------------

void *read_data(void *arg)
{
    char c;

    while(true)
    {
        c = getchar();
        switch (c)
        {
            //read num to check is prime
            case 'p':
                p_request();
                break;
            //read series of num to check is palindrome
            case 'q':
                q_request();
                break;
            //end process
            default:
                break;
        }
    }
}

//------------------------------------------------
// get int and sends to prime server - print the result
void p_request()
{
    int num;

    scanf(" %d", &num);
    getchar();

    prime_arr[START_NUM] = num;

    pthread_kill(child_id[0], SIGUSR1);
    pause(); // wait to SIGUSR1

    printf("is prime ? %s\n", prime_arr[RES] ? "yes" : "no");
}

//------------------------------------------------
// get string and sends to palindrome server - print the result
void q_request()
{
    char str[STR_SIZE];
    scanf(" %s", str);
    pali_arr[1] = str;

    pthread_kill(child_id[PALI], SIGUSR2);

    pause(); // wait to SIGUSR2

    printf("is palindrome ? %s\n", strcmp(pali_arr[RES], "1") == 0 ? "yes" : "no");
}

//-------------------------------------------------
//prime server
void *handle_prime(void *arg)
{
    int res;
    while(true)
    {
        pause(); //wait to SIGUSR1

        res = is_prime(prime_arr[START_NUM]);
        prime_arr[RES] = res;
        sleep(0.1);

        pthread_kill(child_id[CLIENT], SIGUSR2);
    }
}

//-------------------------------------------------
//palindrome server
void *handle_pali(void *arg)
{
    //char res[1];
    while(true)
    {
        pause(); //wait to SIGUSR1

        pali_arr[RES] = is_pali(pali_arr[1], strlen(pali_arr[1])) ? "1" : "0";

        //prime_arr[RES] = res;
        sleep(0.1);

        pthread_kill(child_id[CLIENT], SIGUSR2);
    }
}

//-------------------------------------------------
//check is prime
int is_prime(int num)
{
    int i;
	for(i = 2; i*i <= num; i++)
		if(num % i == 0)
			return 0;
    return 1;
}

//-------------------------------------------------

int is_pali(char str[], int n)
{
    // Initialise flag to zero.
    int flag = 1;

    // Loop till array size n/2.
    for (int i = 0; i <= n / 2 && n != 0; i++)
    {
        // Check if first and last element are different
        // Then set flag to 1.
        if (str[i] != str[n - 1- i ]) {
            flag = 0;
            break;
        }
    }

    // If flag is set then print Not Palindrome
    // else print Palindrome.
    return flag;
}

//-------------------------------------------------

void catch_sigusr1(int sig_num) { }

//-------------------------------------------------

void catch_sigusr2(int sig_num) { }

//-------------------------------------------------

void catch_sigint(int sig_num)
{
    exit(EXIT_SUCCESS);
}

//-------------------------------------------------

void perror_and_exit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

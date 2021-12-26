/*
Parent Thread:
    - creates 3 subthreads
    - saves in global array their pids
    - finishes

First Child: "Prime server"
    - in a loop:
        - goes to sleep until it receives SIGUSR1
        - reads number from his personal global array (array size = 2 cells)
        - checks if number is prime
        - puts result into array
        - sends signal SIGUSR1 to "front end subthread"

Second Child: "Palindrome Server"
    - in a loop:
        - same as prime server except uses 3rd global array and SIGUSR2

Third Chid: "Front End Process"
    - in a loop:
        - reads from user p and then number thats needs to be checked if its
            prime or q and then string thats palindromiut needs to be checked
        - stores input read from user into appropriate array
        - sends signal to appropriate server
        - goes to sleep until receives SIGUSR1 or SIGUSR2
        - wakes, checks result, prints to output


בתכנית עלולה להיווצר תקלה אם השרת ישלח את הסיגנל לתהליך ה: end front לפני
שהאחרון עושה pause .בתור 'טלאי' אפשר שהשרת ימתין מעט )עם sleep או usleep לפני
שהוא שולח את התשובה(

PROGRAM ENDS WHEN RECEIVING SIGINT

- explain types of race modes exist in program


TO COMPILE:
    gcc myprog.c -o myprog -lpthread

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
    pthread_t thread_p, thread_q, thread_client;
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


    // pthread_join(thread1, NULL); //waiting for thread
    // pthread_join(thread2, NULL);
    // pthread_join(thread3, NULL);
    pthread_exit(NULL); //?

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
	puts("1");
    prime_arr[START_NUM] = num;
	printf("%d ", child_id[0]);
    pthread_kill(child_id[0], SIGUSR1);
puts("3");
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
    //getchar();

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
		puts("hi");
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
// mmmmmm ? ulay lo
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

void catch_sigusr1(int sig_num)
{
puts("sigusr1");
 }

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

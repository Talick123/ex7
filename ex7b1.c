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
#define SIZE_PRIME 2
#define SIZE_PALI 100
#define RES 0
#define START_NUM 1

// --------const and enum section------------------------

enum threads {PRIME, PALI, CLIENT}

int child_id[NUM_OF_CHILD];
int prime_arr[SIZE_PRIME];
int pali_arr[SIZE_PALI];

// --------prototype section------------------------

void reset_arr();
void create_threads_and_die();
void *fill_arr(void *arg);
bool prime(int num);
int count_appearances(int curr_ind);
void print_thread_data(int new_count, int max, int max_prime);
void read_and_print_data();
void perror_and_exit(char *msg);

// --------main section------------------------

int main()
{
    create_threads_and_die();
    read_and_print_data();

    return EXIT_SUCCESS;
}

//-------------------------------------------------

void reset_arr()
{
    int i;

    for(i = 0; i < ARR_SIZE; i++)
        arr[i] = 0;
}

//-------------------------------------------------

void create_threads_and_die()
{
    pthread_t thread_p, thread_q, thread_client;
    int status;

    status = pthread_create(&thread_p, NULL, handle_prime, NULL); //creating thread
    if(status != 0)
        perror_and_exit("p_thread_create\n");

    status = pthread_create(&thread_q, NULL, handle_pali, NULL);
    if(status != 0)
        perror_and_exit("p_thread_create\n");

    status = pthread_create(&thread_client, NULL, read_data, NULL);
    if(status != 0)
        perror_and_exit("p_thread_create\n");

    //save the thread id
    child_id[PRIME] = thread_p;
    child_id[PALI] = thread_q;
    child_id[CLIENT] = thread_client;

    // pthread_join(thread1, NULL); //waiting for thread
    // pthread_join(thread2, NULL);
    // pthread_join(thread3, NULL);

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
                p_request(my_socket_p);
                break;
            //read series of num to check is palindrome
            case 'q':
                q_request(my_socket_q);
                break;
            //end process
            case 'e':
                return;
            default:
                break;
        }
    }
}

//------------------------------------------------
// get int and sends to prime server - print the result
void p_request(int my_socket_p)
{
    int num;

    scanf(" %d", &num);
    getchar();

    prime_arr[START_NUM] = num;

    pthread_kill(child_id[PRIME], SIGUSR1);

    pause(); // wait to SIGUSR1

    printf("is prime ? %s\n", prime_arr[RES] ? "yes" : "no");
}

//------------------------------------------------
// get string and sends to palindrome server - print the result
void q_request(int my_socket_q)
{
    int num, i;

    for(i = Q_START_NUM; i < Q_RES - 1; i++)
    {
        scanf(" %d", &num);

        if(num == 0)
        {
            shm_ptr_q[i] = 0;
            break;
        }
        else
            shm_ptr_q[i] = num;
    }
    getchar();

    pthread_kill(child_id[PALI], SIGUSR2);

    pause(); // wait to SIGUSR2

    printf("is prime ? %s\n", pali_arr[RES] ? "yes" : "no");
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
    int res, size = 0;
    while(true)
    {
        size = 0;
        pause(); //wait to SIGUSR1
        size = get_size();
        res = is_pali(size);

        prime_arr[RES] = res;
        sleep(0.1);

        pthread_kill(child_id[CLIENT], SIGUSR2);
    }
}

//-------------------------------------------------

int get_size()
{
    for(i = 0; i < RES - START_NUM; i++)
    {
        if(pali_arr[START_NUM + i] == 0)
            break;
        size++;
    }
}

//-------------------------------------------------
//check is prime
int is_prime(num)
{
    int i;
	for(i = 2; i*i <= num; i++)
		if(num % i == 0)
			return 0;
    return 1;
}

//-------------------------------------------------
// mmmmmm ? ulay lo
int is_pal(int n)
{
    // Initialise flag to zero.
    int flag = 1;

    // Loop till array size n/2.
    for (int i = 0; i <= n / 2 && n != 0; i++)
    {
        // Check if first and last element are different
        // Then set flag to 1.
        if (pali_arr[START_NUM + i] != pali_arr[((START_NUM + n -1) - i]) {
            flag = 0;
            break;
        }
    }

    // If flag is set then print Not Palindrome
    // else print Palindrome.
    return flag;
}

//-------------------------------------------------

void perror_and_exit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

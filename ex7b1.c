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

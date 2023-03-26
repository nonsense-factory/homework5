/*
     CIS26B
     Complex Structures and Functions
        - define and initialize an array of FRIEND structures
        - use structures as reference parameters (pointers)
*/
#include <stdio.h>
#include <string.h>

typedef struct{
    char *name;
    char  phone[11];
    int   age;
} FRIEND;

// Function Declarations
void exchange( FRIEND *one, FRIEND *two );
void print_a_friend( const FRIEND *a_friend );

int main( void )
{
    int i;

    FRIEND a_friend = { "John", "4081112345", 25 };
    FRIEND friend_2;
    FRIEND friends[10] = 
    {
        { "John", "4081112345", 25 },
        { "Mary", "4089992222", 21 },
        { "Mira", "4086668888", 22 }
    };
    
    friend_2.name = "Mary";
    strcpy(friend_2.phone, "4089992222");
    friend_2.age = 21;

    printf( "%s %s %d\n", a_friend.name, a_friend.phone, a_friend.age );
    printf( "%s %s %d\n", friend_2.name, friend_2.phone, friend_2.age );
    printf( "\n" );

    for( i = 0; i < 3; i++ )
        print_a_friend( &friends[i] );
    printf( "\n" );

    exchange( &friends[0], &friends[2] );

    for( i = 0; i < 3; i++ )
        print_a_friend( &friends[i] );
    printf( "\n" );

	return 0;
}

/*
 This function swaps
 two FRIEND structures
 */
void exchange( FRIEND *one, FRIEND *two )
{
    FRIEND _temp; // needed for swapping

    _temp = *one;
    *one = *two;
    *two = _temp;
}

/*
 This function receives a pointer to a FRIEND structure
 and displays its fields on one line.
 */
void print_a_friend( const FRIEND *a_friend )
{
    printf( "%s %s %d\n", a_friend->name, a_friend->phone, a_friend->age );
}

/*
 ************ OUTPUT *****************

 John 4081112345 25
 Mary 4089992222 21
 
 John 4081112345 25
 Mary 4089992222 21
 Mira 4086668888 22
 
 Mira 4086668888 22
 Mary 4089992222 21
 John 4081112345 25

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {

        //The variables
        char c;
        char * str = "Hello world.";
        char * str1 = "How are you doing?";

        // The pointers
        char * buffer;

        // Assign values
        c = 'a';
        buffer = &c;

        // print!
        printf ("Variables:\n");
        printf ("c: %c\n", c);
        printf ("\n");
        printf ("Pointers:\n");
        printf ("str: %s\n", str);
        printf ("buffer(%p): %c\n", buffer, *buffer);
        printf ("\n");

        //repoint the pointers
        printf ("Repointing the pointers\n");
        buffer = str;
        printf ("buffer(%p): %s\n", buffer, buffer);
        printf ("\n");

        //Allocate memory
        printf ("Allocating memory\n");
        buffer = malloc (1024);
        if (buffer == NULL) {
                printf ("Could not allocate memory, aborting!!");
                abort ();
        }

        printf ("Copying stuff to memory\n");
        memset(buffer, 0, 1024);
        strncpy (buffer, str1, strlen(str1));
        printf ("buffer(%p): %s\n", buffer, buffer);
        printf ("\n");

//      free(buffer);

        //Memory leak! Uncomment the free call above to fix.
        printf ("resetting buffer:\n");
        buffer = "good bye world!";
        printf ("buffer(%p): %s\n", buffer, buffer);
        printf ("\n");

        exit (0);
}

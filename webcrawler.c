// credit to user Jay Taylor for the wget call used here to retreive hyperlinks from a webpage
// http://stackoverflow.com/a/2804721

#include <stdio.h>
#include <string.h>

#define BUFFLEN 2500

//http://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program

int main()
{

	// char web_address[BUFFLEN] = "wget -q http://www.github.com -O - |     tr \"\\t\\r\\n'\" '   \"' |     grep -i -o '<a[^>]\\+href[ ]*=[ \\t]*\"\\(ht\\|f\\)tps\\?:[^\"]\\+\"' |     sed -e 's/^.*\"\\([^\"]\\+\\)\".*$/\\1/g' > test.txt"; 
	// system(web_address);

	char test[] = "http://www.mcgill.ca";
	char * s = strchr(test,'/');
	char *t = strchr(s+2*sizeof(char),'.');


	//produces actual value location, e.g. test[6]
	int last_slash = s-test+1;
	int first_dot = t - test;

	printf("%d %d\n", last_slash, first_dot);





	// FILE * ifp = fopen("test.txt","r");

	// char output[50];

	// fscanf(ifp,"%s", output);

	// char * strA = "Blahblahblah", * strB = "123", strC[50];
	// int x = 4;
	// strncpy(strC,strA,x);
	// strC[x] = '\0';
	// strcat(strC,strB);
	// strcat(strC,strA+x);
	// printf("%s\n",strC);

	// printf("%s\n",output );

	return 0;

}
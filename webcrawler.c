// credit to user Jay Taylor for the wget call used here to retreive hyperlinks from a webpage
// http://stackoverflow.com/a/2804721

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define BUFFLEN 2500

//http://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program

char * flip_url(char * url);
char * fix_errors(char * url);
void wget_wrapper(char * website, char * outputfile);
void crawl_page(char * inputfile, char * newinputfile, char * outputfile);

int total_hyperlinks = 1;

int main()
{

	char * flipped; 
	int current_state = 0;
	FILE * toclear;

	char ** possible_filenames = (char **)malloc(2*sizeof(char *));
	char filename1[40] = "input1.txt";
	char filename2[40] = "input2.txt";
	possible_filenames[0] = filename1;
	possible_filenames[1] = filename2;

	char final_output[15] = "final.txt";

	toclear = fopen(possible_filenames[0],"w");
	fclose(toclear);
	toclear = fopen(possible_filenames[1],"w");
	fclose(toclear);
	toclear = fopen(final_output,"w");
	fclose(toclear);	
	
	char output[200];	// holds the most recently obtained hyperlink
	char starter[] = "http://www.google.com";	// the starting node

	//open the output file
	FILE * ofp = fopen(final_output,"w");
	//get the flipped version of the starter node
	flipped = fix_errors(flip_url(starter));
	// print it to the output file so that we know if we've seen it before
	fprintf(ofp, "%s %s %d\n",starter,flipped,total_hyperlinks++);
	//close to allow for appending to this file
	fclose(ofp);

	// get all the hyperlinks from the starter node and print them to the initiial input file
	wget_wrapper("https://accounts.google.com/ServiceLogin?hl=en&continue=http://www.google.com/imghp%%3Fhl%%3Den", possible_filenames[0]);
	


	// crawl_page(possible_filenames[current_state],possible_filenames[abs(1-current_state)],final_output);
	// current_state = abs(1-current_state);
	// crawl_page(possible_filenames[current_state],possible_filenames[abs(1-current_state)],final_output);
	// current_state = abs(1-current_state);
	// crawl_page(possible_filenames[current_state],possible_filenames[abs(1-current_state)],final_output);
	


	return 0;

}


void wget_wrapper(char * website, char * outputfile)
{
	char web_address[BUFFLEN] = "wget -q x -O - | tr \"\\t\\r\\n'\" '   \"' | grep -i -o '<a[^>]\\+href[ ]*=[ \\t]*\"\\(ht\\|f\\)tps\\?:[^\"]\\+\"' |  sed -e 's/^.*\"\\([^\"]\\+\\)\".*$/\\1/g' >> y"; 
	char * after_address = strchr(web_address,'x');
	int first_copy = after_address - web_address;
	after_address += sizeof(char);
	char * after_file = strchr(web_address,'y');
	int second_copy = after_file - after_address;
	after_file += sizeof(char); 

	char final_addr[200];

	int length = 0;
	strncpy(final_addr + length*sizeof(char), web_address, first_copy);
	length += first_copy;
	strncpy(final_addr + length*sizeof(char), website,strlen(website));
	length += strlen(website);
	strncpy(final_addr + length*sizeof(char), after_address, second_copy);
	length += second_copy;
	strncpy(final_addr + length*sizeof(char), outputfile, strlen(outputfile));
	length += strlen(outputfile);
	strcpy(final_addr + length*sizeof(char), after_file);



	printf("final address = %s\n",final_addr );

	system(final_addr);

}



void crawl_page(char * inputfile, char * newinputfile, char * outputfile)
{

	int i;
	char * flipped;
	int exists = 0;
	

	FILE * ifp = fopen(inputfile,"r"); //where we will retreive the next hyperlink to be scoured
	FILE * ofp = fopen(newinputfile,"w"); //where we will store the remainder of the hyperlinks in the file

	char output[200];	//this will hold the first hyperlink in the inputfile
	char dummy[200], test[200];

	//get the first hyperlink
	fscanf(ifp,"%s", output);
	printf("output = %s\n", output );

	//get the link as how it would appear in the final file
	flipped = fix_errors(flip_url(output));

	//store the remainder of the hyperlinks in the new input file;
	while(fscanf(ifp,"%s",test) == 1)
	{
		// printf("output = %s\n",output );
		fprintf(ofp, "%s\n", test);

	};

	fclose(ifp);
	fclose(ofp);

	// use the outputfile, which contains a list of already-crawled pages, as an input
	ifp = fopen(outputfile,"r");

	// search through the output file to determine if the link has already been used before
	while(fscanf(ifp,"%s %s %d", dummy,test, &i) == 3)
	{	
		if (strcmp(flipped,test) == 0)
		{
			exists = 1;
		}
	};



	// if it hasn't been used before, append it to the list
	if (exists == 0)
	{
		ofp = fopen(outputfile,"a");

		fprintf(ofp, "%s %s %d\n", output, flipped, total_hyperlinks++);

		fclose(ofp);

		// add all the links we see while going through this link
		wget_wrapper(output,newinputfile);

	}

}

char * fix_errors(char * url)
{
	char * test = url;
	char * result = (char *)malloc(sizeof(char) * (strlen(url) + 2));

	int i = 0;

	while(test[i] != '\0')
	{
		i++;
	};

	if (test[i-1] != '/')
	{
		strncpy(result,url,strlen(url));
		result[strlen(url)]='/';
	}
	else
	{
		result = url;
	}

	return result;
}




char * flip_url(char * url)
{
	int length = strlen(url);
	char * host, * top_level_domain, * end_of_string, * middle, * filepath, * result, * test = url;
	char * new_host = (char *)malloc(length*sizeof(char));
	int tld_size, last_letter_of_host, i, middle_size, hostname_size;
	char * hostname;




	//search for either the '\0' string terminator or the first / to start the path portion
	// of the url

	do 
	{
		test = strchr(test + sizeof(char),'/');
		// printf("test = %s\n", test);
		// sleep(1);
	}while ((test != NULL) && (((test[-1] == '/') || (test[1] == '/'))));

	result = test;

	test = url;

	if( strchr(&(strchr(url, '.')[1]),'.') == NULL )
	{
		//there was only one dot in the url, meaning that we have something like
		// http://google.com/

		host = (char *)malloc(3*sizeof(char));
		host[0] = host[1] = host[2] = 'w';
		hostname_size = 3;

		if(result == NULL)
		{
			end_of_string = strchr(url,'\0');

			top_level_domain = &((strchr(url,'.'))[1]);

			// printf("tld = %s\n", top_level_domain);

			tld_size = end_of_string - top_level_domain;

		}
		else
		{
			do
			{
				// printf("here\n");
				//find the last dot before the path. this immediately precedes the top level domain.
				test = strchr(&(test[1]),'.');
				// printf("test = %s\n", test);
			}
			while((strchr(&(test[1]),'.') != NULL) && (strchr(&(test[1]),'.') - result < 0));

			top_level_domain = &(test[1]);

			// printf("tld = %s\n", top_level_domain);

			// find the tld length
			tld_size = result - top_level_domain;

			test = url;
		}

		do 
		{
			test = strchr(test + sizeof(char),'/');
			// printf("test = %s\n", test);
			// sleep(1);
		}while (test[-1] != '/');

		test = &(test[1]); //starts with the hostname

		middle = (char *)malloc((length+1)*sizeof(char));
		middle[length] = '\0';
		middle[0] = '.';
		strcpy(middle + sizeof(char), test);
		middle_size = top_level_domain - test + 1;

		// printf("middle = %s\n", middle);

		// printf("hostname_size = %d, tld_size = %d, middle_size = %d\n", hostname_size,tld_size,middle_size);

		strncpy(new_host, top_level_domain, tld_size);
		// printf("new_host = %s\n", new_host );
		strncpy(new_host + sizeof(char)*tld_size,middle,middle_size);
		// printf("new_host = %s\n", new_host );
		strncpy(new_host + sizeof(char)*(tld_size+middle_size),host,hostname_size);
		// printf("new_host = %s\n", new_host );

		if(result !=NULL)
		{
			strcpy(new_host + sizeof(char)*(tld_size+middle_size+hostname_size), result);
			// printf("new_host = %s\n", new_host );
		}




		return new_host;
	}







	if ( result == NULL)
	{
		// printf("the test was null\n");
		// there was no filepath, just the main site
		end_of_string = strchr(url,'\0');
		test = url;
		do
		{
			//find the last dot. this immediately precedes the top level domain.
			test = strchr(&(test[1]),'.');
		}
		while(strchr(&(test[1]),'.') != NULL);

		top_level_domain = &(test[1]);
		// printf("tld = %s\n", top_level_domain);

		// find the tld length
		tld_size = end_of_string - top_level_domain;

		test = url;

		do
		{	
			//find the second last dot. this is what seperates the hostname
			//from the remainder of the host
			test = strchr(&(test[1]),'.');
		}while( strchr(&(strchr(&(test[1]),'.')[1]),'.') != NULL);

		middle = test;
		middle_size = top_level_domain - middle;
		// printf("middle = %s\n", middle);
	}
	else
	{
		// here we do have a filepath at the end and test points to the beginning of this
		// filepath, i.e. the first '/' of the path

		filepath = result;

		test = url;

		do
		{
			// printf("here\n");
			//find the last dot before the path. this immediately precedes the top level domain.
			test = strchr(&(test[1]),'.');
			// printf("test = %s\n", test);
		}
		while((strchr(&(test[1]),'.') - filepath < 0) && (strchr(&(test[1]),'.') != NULL));

		top_level_domain = &(test[1]);

		// printf("tld = %s\n", top_level_domain);

		// find the tld length
		tld_size = filepath - top_level_domain;

		test = url;

		do
		{	
			//find the second last dot. this is what seperates the hostname
			//from the remainder of the host
			test = strchr(&(test[1]),'.');
		}while( (strchr(&(strchr(&(test[1]),'.')[1]),'.') - filepath < 0) && (strchr(&(strchr(&(test[1]),'.')[1]),'.') != NULL));

		middle = test;

		// printf("middle = %s\n",middle );
		middle_size = top_level_domain - middle;
	}

	test = url;
	do 
	{
		test = strchr(test + sizeof(char),'/');
		// printf("test = %s\n", test);
		// sleep(1);
	}while (test[-1] != '/');

	host = test + sizeof(char);

	// printf("host = %s\n",host );

	hostname_size = middle - host;

	// printf("hostname_size = %d, tld_size = %d, middle_size = %d\n", hostname_size,tld_size,middle_size);

	strncpy(new_host, top_level_domain, tld_size);
	// printf("new_host = %s\n", new_host );
	strncpy(new_host + sizeof(char)*tld_size,middle,middle_size);
	// printf("new_host = %s\n", new_host );
	strncpy(new_host + sizeof(char)*(tld_size+middle_size),host,hostname_size);
	// printf("new_host = %s\n", new_host );

	if(result !=NULL)
	{
		strcpy(new_host + sizeof(char)*(tld_size+middle_size+hostname_size), filepath);
		// printf("new_host = %s\n", new_host );
	}

	// printf("%s\n",test);

	return new_host;

}
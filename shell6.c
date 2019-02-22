#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <dirent.h>  
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <readline/readline.h> 
#include <readline/history.h>
#include "tinyexpr.h"
#include "tinyexpr.c"
#include <regex.h>
#include <curl/curl.h>
#include <time.h>

  
#define MAXCOM 1000 // max number of letters to be supported 
#define MAXLIST 100 // max number of commands to be supported 

// Clearing the shell using escape sequences 
#define clear() printf("\033[H\033[J") 
int compareFile(FILE * fPtr1, FILE * fPtr2, int * line, int * col);
void listfiles();
void searchtext();
int my_compare (const void *a, const void *b) {
    return strcmp(a, b);
}

int isSubstring(char* s1, char* s2) 
{ 
    int M = strlen(s1); 
    int N = strlen(s2); 
  
    /* A loop to slide pat[] one by one */
    for (int i = 0; i <= N - M; i++) { 
        int j; 
  
        /* For current index i, check for pattern match */
        for (j = 0; j < M; j++) 
            if (s2[i + j] != s1[j]) 
                break; 
  
        if (j == M) 
            return i; 
    } 
  
    return -1; 
}

int dirindex=-1;
int dirindexd=-1;
char dirlist[80][40];
char dirlistd[80][80];

void dirsort(){
	int i,j;
	char temp[40];
	char tempd[80];
	for(i=1;i<=dirindex;i++)
		for(j=0;j<dirindex;j++)
		{
			if (my_compare(dirlist[i],dirlist[j])<0){
				strcpy(temp,dirlist[i]);
				strcpy(dirlist[i],dirlist[j]);
				strcpy(dirlist[j],temp);
				
				//printf("%s\n",dirlistd[i]);
				strcpy(tempd,dirlistd[i]);
				strcpy(dirlistd[i],dirlistd[j]);
				strcpy(dirlistd[j],tempd);
			}
		}
}

int dirshower(char x[]){
	int j;
	if(1){
		//strcpy(s[dirindex++],x);
		struct dirent *de;  
		DIR *dr = opendir(x); 
		if (dr == NULL)  // opendir returns NULL if couldn't open directory 
			{
				//printf("Could not open current directory" ); 
				return 0; 
			}
		while ((de = readdir(dr)) != NULL) 
			{	
				//j=1;
				//printf("%s\n", de->d_name);
				char *b=de->d_name;
				if(b[0]!='.'){
					char c[100]="";
					char *d="/";
					strcpy(c,x);
					strcat(c,d);
					strcat(c,b);
					//printf("%s\n",c);
					strcpy(dirlist[++dirindex],b);
					strcpy(dirlistd[++dirindexd],x);
					dirshower(c);
				}
			}
		closedir(dr);
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
void listfiles()
{
	int k,i;
	
	char typelist[5][5];
	strcpy(typelist[0],"txt");
	strcpy(typelist[1],"c");
	strcpy(typelist[2],"cpp");
		
	char path[100]="/home/aki/Desktop";
	
	k=dirshower(path);
	int y;
	printf("\nSort The Files? 0/1?");
	scanf("%d",&y);
	if(y)dirsort();
    
	int m;
	for(k=0;k<dirindex;k++){
		printf("\n%d. %s",k+1,dirlist[k]);
		m=strlen(dirlist[k]);
		if(k>8) m+=1;
		for(i=m;i<17;i++) printf(" ");
		printf(" %s",dirlistd[k]);
		//getfn(dirlist[k]);
	}
	printf("\n");
}

///////////////////////////////////////////////////////////////////////////////////////
void findfile(char a[])
{
	int k,i;
	int flag=0;
	char typelist[5][5];
	strcpy(typelist[0],"txt");
	strcpy(typelist[1],"c");
	strcpy(typelist[2],"cpp");
		
	char path[100]="/home/aki/Desktop";
	
	k=dirshower(path);
	int y;
	int m;
	for(k=0;k<dirindex;k++){
		if(strcmp(a,dirlist[k])==0){
			printf("\nFound %s at %s",a,dirlistd[k]);
			flag++;
		}
	}
	if(flag==0) printf("\nFile not found");
	printf("\n");
}
///////////////////////////////////////////////////////////////////////////////////////
void compare()
{
	int k,i;
	
	char typelist[5][5];
	strcpy(typelist[0],"txt");
	strcpy(typelist[1],"c");
	strcpy(typelist[2],"cpp");
		
	char path[100]="/home/aki/Desktop";
	
	k=dirshower(path);
	dirsort();
    
	//qsort(dirlist,dirindex,sizeof(dirlist[0]),my_compare);
	int m;
	for(k=0;k<dirindex;k++){
		printf("\n%d. %s",k+1,dirlist[k]);
		m=strlen(dirlist[k]);
		if(k>8) m+=1;
		for(i=m;i<17;i++) printf(" ");
		printf(" %s",dirlistd[k]);
		//getfn(dirlist[k]);
	}
	
	char path1[120],path2[120];
	char ins[2];
	ins[0]='/';
	ins[1]='\0';
	int a1,a2;
	printf("\nEnter File #1:");
	scanf("%d",&a1);
	printf("Enter File #2:");
	scanf("%d",&a2);
	strcpy(path1,dirlistd[a1-1]);
	strcat(path1,ins);
	strcat(path1,dirlist[a1-1]);
	
	strcpy(path2,dirlistd[a2-1]);
	strcat(path2,ins);
	strcat(path2,dirlist[a2-1]);
		
	printf("\n%s\n%s\n",path1,path2);
	//scanf("%s",path1);
	//scanf("%s",path2);
	FILE *fPtr1,*fPtr2;
	fPtr1 = fopen(path1, "r");
    fPtr2 = fopen(path2, "r");
	int diff;
	int line, col;
	diff = compareFile(fPtr1, fPtr2, &line, &col);
	printf("%d",diff);
	if(diff==0) printf("The Files Are Exactly Same");
	else printf("The Files are not same\nFound Difference at: line%d,col%d",line,col);
	fclose(fPtr1);
    fclose(fPtr2);
	printf("\n");
}

///////////////////////////////////////////////////////////////////////////////////////
struct string {
  char *ptr;
  size_t len;
};
  
void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

int getweather(){
  CURL *curl;
  CURLcode res;
  regex_t reg;
  int i;
  const char *regex=">..&nbsp";
  curl = curl_easy_init();
  
  time_t rawtime;
   struct tm *info;
   char buffer[80];

   time( &rawtime );

   info = localtime( &rawtime );

   strftime(buffer,80,"%x - %I:%M%p", info);
   printf("%s", buffer );
  
  
  if(curl) {
    struct string s;
    init_string(&s);

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.timeanddate.com/weather/india/vellore/ext");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    res = curl_easy_perform(curl);

    //printf("\n$%s", s.ptr);
	
	regmatch_t matches[16];

	regcomp(&reg, regex, REG_EXTENDED);
	//clear();
	if(regexec(&reg, s.ptr, 16, matches, 0) == 0)
	{
	//printf("regex /%s/ matched string '%s' at bytes %d-%d\n",regex, s.ptr, matches[0].rm_so, matches[0].rm_eo);
	printf("\nCurrent Weather :");
	for(i=matches[0].rm_so+1;i<matches[0].rm_eo-5;i++) printf("%c",s.ptr[i]);
	printf("°C\n");
	}else
	printf("Couldn't Retrieve Weather Right Now\n");
	free(s.ptr);
    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return 0;
}
  
// Greeting shell during startup 
void init_shell() 
{ 
    clear(); 
    printf("\n▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄"); 
    printf("\n\tAK ENHANCED SHELL"); 
    printf("\n▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄▀▄"); 
	
    char* username = getenv("USER"); 
    printf("\n\n\nUSER is: @%s", username); 
    printf("\n"); 
	printf("\n____________________________________"); 
    printf("\n\tWelcome"); 
    printf("\n____________________________________");
	sleep(1); 
    clear(); 
} 
  
int takeInput(char* str) 
{ 
    char* buf; 
  
    buf = readline("\n>>> "); 
    if (strlen(buf) != 0) { 
        add_history(buf); 
        strcpy(str, buf); 
        return 0; 
    } else { 
        return 1; 
    } 
} 

void dircw() 
{ 
    char cwd[1000]; 
    getcwd(cwd, sizeof(cwd)); 
    printf("▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"); 
    printf("\nDir: %s", cwd); 
} 
  
// Function where the system command is executed 
void execArgs(char** parsed) 
{ 
    // Forking a child 
    pid_t pid = fork();  
  
    if (pid == -1) { 
        printf("\nFork Error"); 
        return; 
    } else if (pid == 0) { 
        if (execvp(parsed[0], parsed) < 0) { 
            printf("\nCommand Error"); 
        } 
        exit(0); 
    } else { 
        wait(NULL);  
        return; 
    } 
} 
  
void execArgsPiped(char** parsed, char** parsedpipe) 
{ 
    // 0 is read end, 1 is write end 
    int pipefd[2];  
    pid_t p1, p2; 
  
    if (pipe(pipefd) < 0) { 
        printf("\nPipe could not be initialized"); 
        return; 
    } 
    p1 = fork(); 
    if (p1 < 0) { 
        printf("\nCould not fork"); 
        return; 
    } 
  
    if (p1 == 0) { 
        // Child 1 executing.. 
        // It only needs to write at the write end 
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]); 
  
        if (execvp(parsed[0], parsed) < 0) { 
            printf("\nCould not execute command 1.."); 
            exit(0); 
        } 
    } else { 
        // Parent executing 
        p2 = fork(); 
  
        if (p2 < 0) { 
            printf("\nCould not fork"); 
            return; 
        } 
  
        // Child 2 executing.. 
        // It only needs to read at the read end 
        if (p2 == 0) { 
            close(pipefd[1]); 
            dup2(pipefd[0], STDIN_FILENO); 
            close(pipefd[0]); 
            if (execvp(parsedpipe[0], parsedpipe) < 0) { 
                printf("\nCould not execute command 2.."); 
                exit(0); 
            } 
        } else { 
            // parent executing, waiting for two children 
            wait(NULL); 
            wait(NULL); 
        } 
    } 
} 
  
// Help command builtin 
void openHelp() 
{ 
    puts("[Help]"
        "\nList of New Commands:"
        "\n>listfiles"
        "\n>compf"
        "\n>hello"
        "\n>exit"
        "\n>All basic Shell commands are supported"
        "\n>Variable and calculation supported"); 
  
    return; 
} 
  
// Function to execute builtin commands 
int ownCmdHandler(char** parsed) 
{ 
    int NoOfOwnCmds = 7, i, switchOwnArg = 0; 
    char* ListOfOwnCmds[NoOfOwnCmds]; 
    char* username; 
  
    ListOfOwnCmds[0] = "exit"; 
    ListOfOwnCmds[1] = "cd"; 
    ListOfOwnCmds[2] = "help"; 
    ListOfOwnCmds[3] = "hello"; 
    ListOfOwnCmds[4] = "compf"; 
    ListOfOwnCmds[5] = "listfiles"; 
    ListOfOwnCmds[6] = "search"; 
  
    for (i = 0; i < NoOfOwnCmds; i++) { 
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) { 
            switchOwnArg = i + 1; 
            break; 
        } 
    } 
  
    switch (switchOwnArg) { 
    case 1: 
        printf("\nThank You\nRegards : Akshit"); 
        exit(0); 
    case 2: 
        chdir(parsed[1]); 
        return 1; 
    case 3: 
        openHelp(); 
        return 1; 
    case 4: 
        username = getenv("USER"); 
        printf("\nHello %s.\nMind that this is "
            "not a place to play around."
            "\nUse help to know more..\n", 
            username); 
        return 1;
	case 5: 
		compare();
		return 1;
	case 6: 
		listfiles();
		return 1;
	case 7: 
		searchtext();
		return 1;
    default: 
        break; 
    } 
  
    return 0; 
} 
  
int parsePipe(char* str, char** strpiped) 
{ 
    int i; 
    for (i = 0; i < 2; i++) { 
        strpiped[i] = strsep(&str, "|"); 
        if (strpiped[i] == NULL)break; } 
  
    if (strpiped[1] == NULL) 
        return 0;
    else { 
        return 1; } 
} 
  
// function for parsing command words 
void parseSpace(char* str, char** parsed) 
{ 
    int i; 
  
    for (i = 0; i < MAXLIST; i++) { 
        parsed[i] = strsep(&str, " "); 
  
        if (parsed[i] == NULL) 
            break; 
        if (strlen(parsed[i]) == 0) 
            i--; 
    } 
} 
  
int processString(char* str, char** parsed, char** parsedpipe) 
{ 
  
    char* strpiped[2]; 
    int piped = 0; 
  
    piped = parsePipe(str, strpiped); 
  
    if (piped) { 
        parseSpace(strpiped[0], parsed); 
        parseSpace(strpiped[1], parsedpipe); 
  
    } else { 
  
        parseSpace(str, parsed); 
    } 
  
    if (ownCmdHandler(parsed)) 
        return 0; 
    else
        return 1 + piped; 
} 

int checkvar(char *a){
	int k,l,i;
	int checkint=0;
	//printf("%s %d",a,strlen(a));
	for(i=0;i<strlen(a);i++){
		k=(int)a[i];
		if(k==36){
			l=(int)a[i+1];
			if(((64<l)&&(l<91))||((96<l)&&(l<123)))
				checkint++;
		}
		if(k==43) checkint++;
		else if(k==42) checkint++;
		else if(k==45) checkint++;
		else if(k==61) checkint++;
		else if(k==37) checkint++;
	}
	return checkint;
}
///////////////////////////////////////////////////////////////////////////////////////
void aluexp(char *a){
char numlist[20][5];int nl=-1;
char varlist[20][5],vl=-1;
//int varlistvals[20],vlv=-1;
int oplist[20],op=-1;
char finarr[30]; int finnum=-1;
int i=0,k;
char temp;
//scanf("%s",a);
strcat(a,".");
double x;
int j;
int flag;
int assign=0;
int assignvar=0;
char assigndvar[5];
char assvarlist[10][5];int assvarn=0;

for(i=0;i<strlen(a);i++){
	k=(int)a[i];
	if(k==36){
		assignvar++;
	}
	else if(k==61){
		assign++; break;
	}
}
if(assign>1) printf("\nAssignment Error: Too many equals");
if((assignvar>1)&&(assign))printf("\nAssignment Error: Too many variables");
i=0;
if((assign==1)&&(assignvar==1)){
	k=(int)a[0];
	if(k==36){
		k=(int)a[++i];
		j=-1;
		while(((64<k)&&(k<91))||((96<k)&&(k<123))){
			assigndvar[++j]=a[i];
			k=(int)a[++i];
		}
		assigndvar[++j]='\0';
	}
	if(k==61){
		i++;
		while(i<strlen(a)){
		k=(int)a[i];
		if(k==36)
		{ ++vl;
		k=(int)a[++i];
		j=-1; flag=1;
		while(((64<k)&&(k<91))||((96<k)&&(k<123))){
			varlist[vl][++j]=a[i];
			k=(int)a[++i];
		}
		varlist[vl][++j]='\0';
		finarr[++finnum]='x';
		}
		else if(k==46){
			break;
		}
		else{
			finarr[++finnum]=a[i++];
		}
		}
	}
	finarr[++finnum]='\0';
	printf("\nFinal Array: ");
	printf("%s",finarr);

}
i=0;
if(assign==0){	
	while(i<strlen(a)){
		k=(int)a[i];
		if(k==36)
		{ ++vl;
		k=(int)a[++i];
		j=-1; flag=1;
		while(((64<k)&&(k<91))||((96<k)&&(k<123))){
			varlist[vl][++j]=a[i];
			k=(int)a[++i];
		}
		varlist[vl][++j]='\0';
		finarr[++finnum]='x';
		}
		else if(k==61){
			k=(int)a[++i];
			++nl;
			j=-1;
			while((47<k)&&(k<58)){
				numlist[nl][++j]=a[i];
				k=(int)a[++i];
			}
			numlist[nl][++j]='\0';
		}
		else if(k==46){
			break;
		}
		else{
			finarr[++finnum]=a[i++];
		}
}
finarr[++finnum]='\0';
//printf("\nFinal Array: ");
//printf("%s",finarr);
}
/*
printf("\nVariable List");
for(i=0;i<=vl;i++){
	printf("\n%s",varlist[i]);
}

for(i=0;i<=nl;i++){
	printf("\n%s",numlist[i]);
}*/

char varlistvalsx[20][5],vlvx=-1;
int foundflag=0;
char fin[5];
char str[200]="Hello";
char strb[200]="Hello";
i=0;
FILE *fptr;
fptr = fopen("filename.txt","r");
char finarrx[30]="";
//printf("# S\n");
if(assign==0)
	{
	while(i<=vl)
	{
	strcpy(fin,varlist[i]);
	foundflag=0;
	while(1)
		{
		fscanf((FILE*)fptr,"%s",str);
		if(strcmp(str,".")==0) break;
		
		if(strcmp(str,fin)==0)
			{foundflag=1;
			fscanf((FILE*)fptr,"%s",strb);
			break;}
		fscanf((FILE*)fptr,"%s",strb);
		}
	if(foundflag==0) {printf("\nError Variable Not Found"); break;}
	//printf("\n#%s %d",str,d);
	strcpy(varlistvalsx[++vlvx],strb);
	i++;
	fseek(fptr, 0, SEEK_SET); 
	}
	if(foundflag)
	{
		//for(i=0;i<=vlvx;i++)printf("\n%s %s",varlist[i],varlistvalsx[i]);
		j=-1;
		for(i=0;i<strlen(finarr);i++){
			if(finarr[i]=='x'){
				strcat(finarrx,varlistvalsx[++j]);
				}
			else
				{
					char xtemp[2];
					xtemp[0]=finarr[i];
					xtemp[1]='\0';
					strcat(finarrx,xtemp);
				}
		}
		//printf("\n%s",finarr);
		//printf("\n%s",finarrx);
		x = te_interp(finarrx,0);
		printf("\n%f",x);
	}
}
if((vl==-1)&&(assign==0))
	{
	x = te_interp(finarr,0);
	printf("\n%f",x);
}
if((assign==1)&&(assignvar==1)){
	//printf("Assign:");
	if(vl>=0)
		{
		i=0;
		while(i<=vl)
		{
			strcpy(fin,varlist[i]);
			foundflag=0;
			while(1)
			{
				fscanf((FILE*)fptr,"%s",str);
				if(strcmp(str,".")==0) break;
				if(strcmp(str,fin)==0){
					foundflag=1;
					fscanf((FILE*)fptr,"%s",strb);
					break;
					}
			fscanf((FILE*)fptr,"%s",strb);
			}
			if(foundflag==0) {printf("\nError Variable Not Found"); break;}
		//printf("\n#%s %d",str,d);
			strcpy(varlistvalsx[++vlvx],strb);
			i++;
			fseek(fptr, 0, SEEK_SET); 
		}
		if(foundflag)
		{
		//for(i=0;i<=vlvx;i++)printf("\n%s %s",varlist[i],varlistvalsx[i]);
		j=-1;
		for(i=0;i<strlen(finarr);i++){
			if(finarr[i]=='x'){
				strcat(finarrx,varlistvalsx[++j]);
				}
			else
				{
					char xtemp[2];
					xtemp[0]=finarr[i];
					xtemp[1]='\0';
					strcat(finarrx,xtemp);
				}
		}
		//printf("\n%s",finarr);
		//printf("\n%s",finarrx);
		x = te_interp(finarrx,0);
		printf("\n%f",x);
		}
	}
	else{
		//printf("\n%s",finarr);
		x=te_interp(finarr,0);
		printf("\n%f",x);
	}
	printf("\n%s",assigndvar);
	
fseek(fptr, 0, SEEK_SET);
FILE *fpw;
fpw = fopen("filenamew.txt","w");
i=0;
foundflag=0;
while(1)
	{
	fscanf((FILE*)fptr,"%s",str);
	fscanf((FILE*)fptr,"%s",strb);
	if(strcmp(str,assigndvar)==0){
	foundflag=1;
	sprintf(strb,"%d",(int)floor(x));
	}
	if(strcmp(str,".")==0) break;
	printf("\n%s",strb);
	fprintf(fpw,"%s %s\n",str,strb); 
	}
	sprintf(strb,"%d",(int)floor(x));
if(foundflag==0) fprintf(fpw,"%s %s\n",assigndvar,strb); 
fprintf(fpw,"%s\n",". .");
printf("\n %s",strb);
remove("filename.txt");
rename("filenamew.txt","filename.txt");
fclose(fptr);
fclose(fpw);

}

printf("\n");
}

///////////////////////////////////////////////////////////////////////
int compareFile(FILE * fPtr1, FILE * fPtr2, int * line, int * col)
{
    char ch1, ch2;
    *line = 1;
    *col  = 0;

    do
    {
        ch1 = fgetc(fPtr1);
        ch2 = fgetc(fPtr2);

        if (ch1 == '\n')
        {
            *line += 1;
            *col = 0;
        }


        if (ch1 != ch2)return -1;
        *col  += 1;

    } while (ch1 != EOF && ch2 != EOF);

    if (ch1 == EOF && ch2 == EOF)
        return 0;
    else
        return -1;
}

int browse(char a[]){
	int i=0;
	int flag=0;
	int webflag=0;
	char b[50];
	int j=-1;
	char web[4];
	char webf[4]="web";
	web[0]=a[0];
	web[1]=a[1];
	web[2]=a[2];
	web[3]='\0';
	if(strcmp(webf,web)==0){webflag=1;}
	//printf("%s %s",web,webf);
	if(webflag)
	{
	while(i<strlen(a)){
		if(a[i]==' ')flag++;
		if(flag) b[++j]=a[i++]; 
		else i++;
	}
	b[++j]='\0';
	char c[50]="firefox ";
	strcat(c,b);
	strcpy(a,c);
	//printf("$%s",c);
	}
	return 1;
}

int find(char a[]){
	int i=0;
	int flag=0;
	int webflag=0;
	char b[50];
	int j=-1;
	char find[5];
	char findf[5]="find";
	find[0]=a[0];
	find[1]=a[1];
	find[2]=a[2];
	find[3]=a[3];
	find[4]='\0';
	if(strcmp(findf,find)==0){webflag=1;}
	//printf("%s %s",web,webf);
	if(webflag)
	{
	while(i<strlen(a)){
		if(a[i]==' '){flag++;i++;}
		if(flag) b[++j]=a[i++]; 
		else i++;
	}
	b[++j]='\0';
	//strcat(c,b);
	strcpy(a,b);
	printf("$%s",a);
	}
	return webflag;
}

void searchtext(){
	int m,i,k;
	char path[100]="/home/aki/Desktop";
	k=dirshower(path);
	dirsort();
	
	char ff[2]="/";
	char cx,readstr[30];
	FILE *fpr,*fpw;
	char pol[100];
	char txtfr[4]="txt";
	char txtfrt[4];
	txtfrt[3]='\0';
	char ax[20];
	printf("Input String: ");
	scanf("%s",ax);
	for(k=0;k<dirindex;k++){
		strcpy(pol,dirlistd[k]);
		strcat(pol,ff);
		strcat(pol,dirlist[k]);
		int lk=strlen(dirlist[k]);
		txtfrt[0]=dirlist[k][lk-3];
		txtfrt[1]=dirlist[k][lk-2];
		txtfrt[2]=dirlist[k][lk-1];
		
	char tstring[900],sub[20]="test";
	if(strcmp(txtfrt,txtfr)==0) 
	{
	strcpy(tstring,"");
		int i=-1;
		fpr = fopen(pol,"r");
		while ((cx = getc(fpr)) != EOF){tstring[++i]=cx;}
		tstring[++i]='\0';
		fclose(fpr);
		//printf("%s\n",tstring);
		if(isSubstring(ax,tstring)>0) printf("Found in file: %s\n",pol);
	}
	
	}	
}

int main() 
{ 
    char inputString[MAXCOM], *parsedArgs[MAXLIST]; 
    char* parsedArgsPiped[MAXLIST]; 
    int execFlag = 0; 
    init_shell(); 
	int checkalu;
	int checkweb;
	getweather();
  
    while (1) { 
        
		checkalu=0;
		
        dircw(); 
        // take input 
        if (takeInput(inputString)) 
            continue; 
		checkalu=checkvar(inputString);
		//printf("\nChecking Variable : %d",checkalu);
        // process
		dirindex=-1;
		dirindexd=-1;
		if(checkalu) aluexp(inputString);
		if(find(inputString)) {
			checkalu++;
			findfile(inputString);
		}
		checkweb=browse(inputString);
		if(checkalu==0){
			execFlag = processString(inputString,parsedArgs, parsedArgsPiped); 
        	if (execFlag == 1) execArgs(parsedArgs); 
			if (execFlag == 2) execArgsPiped(parsedArgs, parsedArgsPiped); 
		}
		
    } 
    return 0; 
} 
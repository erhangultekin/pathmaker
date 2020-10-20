#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<unistd.h>


void reader(FILE* fileR,char c,char list1[]) //decide which operation is to be done
{
    if(strcmp("go", list1)==0)
    {
        go(fileR,c);
    }
    else if(strcmp("make",list1)==0)
    {
        char curr[PATH_MAX];           //save the first current directory on this block
        getcwd(curr, sizeof(curr));     //then do the operation.
        make(fileR,c);                  //then,change the directory to the first current directory.
        chdir(curr);
    }
    else if(strcmp("if",list1)==0)
    {
        char curr[PATH_MAX];               //save the current directory,
        int isIf=1;                        //return the value 1,if it is an "if"
        iforifnot(fileR,c,isIf);            //do the operation.
        getcwd(curr, sizeof(curr));         //then,change the directory to the first current directory.
        chdir(curr);

    }
    else if(strcmp("ifnot",list1)==0)
    {
        int isIf=0;
        char curr[PATH_MAX];                 //same operations but with an isIf value of "0",since it is "ifnot"
        iforifnot(fileR,c,isIf);
        getcwd(curr, sizeof(curr));
        chdir(curr);
    }
}

int directoryController(char list[]) //do the string operations and sort out syntax errors.
{
    if(isalpha(list[0])==0 && list[0]!='*')
    {
        printf("Error:Path must begin with a letter \n");
        return 0;
    }else{
        for(int l=1; l<strlen(list); l++)
        {
            if(isalnum(list[l]) || list[l]=='_' || list[l]=='*' || list[l]=='/')
            {
                //do nothing if it has,* ,/ between.
            }
            else
            {
                printf("Invalid directory type.");
                return 0;
            }
            if(list[strlen(list)-1]=='/')
            {
                printf("Error:Path cannot end with a '/' \n");
                return 0;
            }
        }
    }

    if(list[0]!='*')
    {
        for(int k=0; k<strlen(list); k++)
        {
            if(list[k]=='*')
            {
                printf("Error:Cannot use '*' between to directories \n");
                return 0;
            }
        }
    }
}

void go(FILE* fileR,char c) //operate go command
{
    char directorylist[300]= {'0'}; //
    char s[100];
    int j=0;
    int z=0;

    while(isspace(c)!=0) //ignore the spaces
    {
        c=fgetc(fileR);
    }
    if(c=='<') //append the characters to directorylist,to control the directory between <,>
    {
        while((c=fgetc(fileR))!='>')
        {
            directorylist[j++]=c;
        }
        if(directoryController(directorylist)!=0 && fgetc(fileR)==';')  //if the command is complete,and has ; at the end,then do the operation
        {

            char curr[PATH_MAX];
            getcwd(curr, sizeof(curr)); //appending first current directory to a list

            char * token = strtok(directorylist, "/");//splitting the tokens between two "/"

            while( token != NULL )
            {
                if(strcmp(token,"*") == 0)
                {
                    char maxpathbefore[PATH_MAX]; //path before operating
                    char maxpathafter[PATH_MAX]; //path after operating

                    getcwd(maxpathbefore, sizeof(maxpathbefore));

                    chdir("..");

                    getcwd(maxpathafter, sizeof(maxpathafter));

                    if(strcmp(maxpathbefore,maxpathafter) ==0)
                    {
                        printf("You've reached the maximum directory... \n");
                    }else{
                        printf("%s found. \n Directory: %s \n",token, getcwd(s, 100));
                    }
                }else{
                    char before[PATH_MAX]; //path before operating
                    char after[PATH_MAX]; //path after operating
                    getcwd(before, sizeof(before));

                    chdir(token);

                    getcwd(after, sizeof(after));

                    if(strcmp(before, after) !=0)
                    {
                        printf("%s found. \n Directory: %s \n",token, getcwd(s, 100));
                    }else{
                        printf("%s cannot be found.\n",token);
                        z++; //a value for deciding whether directory exist or not
                    }
                }

                token = strtok(NULL, "/"); //split the token
            }

            if (z!=0)
            {
                printf("Directory cannot be found,returning to the old directory...  \n");
                chdir(curr);
                printf("The returned directory : %s \n",getcwd(curr,sizeof(curr)));
            }
        }
    }

}

void make(FILE* fileR,char c) //operate make command
{
    char directorylist[300]= {'0'};
    char s[100];
    int j=0;

    while(isspace(c)!=0) //ignore the spaces
    {
        c=fgetc(fileR);
    }
    if(c=='<')
    {
        while((c=fgetc(fileR))!='>')
        {
            if(c!=' ')
            {
                directorylist[j++]=c; //append the characters to directorylist,to control the directory between <,>
            }
        }
        if(directoryController(directorylist)!=0 && fgetc(fileR)==';')  //if the command is complete,and has ; at the end,then do the operation
        {
            char * token = strtok(directorylist, "/"); //splitting the tokens between two "/"
            while( token != NULL )
            {
                if(strcmp(token,"*") == 0)
                {
                    char maxpathbefore[PATH_MAX]; //path before operating
                    char maxpathafter[PATH_MAX]; //path after operating

                    getcwd(maxpathbefore, sizeof(maxpathbefore));

                    chdir("..");

                    getcwd(maxpathafter, sizeof(maxpathafter));

                    if(strcmp(maxpathbefore,maxpathafter) ==0)
                    {
                        printf("You've reached the maximum directory... \n");
                    }
                }else{
                    char before[PATH_MAX]; //path before operating
                    char after[PATH_MAX]; //path after operating

                    getcwd(before, sizeof(before));

                    chdir(token);

                    getcwd(after, sizeof(after));

                    if(strcmp(before, after) !=0)
                    {
                        printf("%s file already exists. \n Directory: %s \n",token, getcwd(s, 100));

                    }
                    else
                    {
                        printf("%s cannot be found.\n",token);
                        mkdir(token);
                        chdir(token);
                        printf("%s file created.\n",token);
                    }
                }
                token = strtok(NULL, "/"); //split the token
            }
        }
    }

}

void ifnotParanthesis(FILE* fileR,char c,int z) //check if "ifnot" command contains block,then operate
{
    fpos_t pos3; //position of "{"
    fpos_t pos4; //position of "}"
    int i=0;

    c=fgetc(fileR);

    while(isspace(c)!=0)
    {
        c=fgetc(fileR); //ignore the spaces
    }

    if(c=='{')
    {
        fgetpos(fileR,&pos3);

        while((c=fgetc(fileR))!=EOF)
        {
            if(c=='}')
            {
                fgetpos(fileR,&pos4);
                if(z!=0) //if z value isn't 0,it means ifnot is true,which we decide in iforifnot() method.
                {
                    fsetpos(fileR,&pos3); //if ifnot is true and has a block after command,take the position of '{' and continue reading from this position in main.

                }
                else
                {
                    fsetpos(fileR,&pos4); //if ifnot is false and has a block after command,take the position of '}' and continue reading from this position in main.
                }
                break;
            }
        }
    }
    else //if ifnot doesn't have any block
    {
        if(z!=0) //if ifnot is true,
        {
            fseek(fileR, -1, SEEK_CUR);
            int i=0;
            char keywordlist[200]= {'0'};

            while((c = fgetc(fileR)) != EOF)
            {
                while(isspace(c)!=0 || iscntrl(c)!=0 || c=='}')
                {
                    c=fgetc(fileR);
                }
                while(isalpha(c))
                {
                    keywordlist[i++]=c;
                    c=fgetc(fileR);
                }
                if(isKeyword(keywordlist)==1)
                {
                    reader(fileR,c,keywordlist);
                    memset(keywordlist, 0, sizeof(keywordlist));
                    i=0;
                    return;
                }
            }
        }
        else //if ifnot is false
        {
            while((c = fgetc(fileR)) != '>')
            {
                continue;
            }
        }
    }

}


void ifParanthesis(FILE* fileR,char c,int z)
{
    fpos_t pos; //position of "{"
    fpos_t pos2;//position of "}"
    int i=0;
    c=fgetc(fileR);

    while(isspace(c)!=0) //ignore the spaces
    {
        c=fgetc(fileR);
    }
    if(c=='{')
    {
        fgetpos(fileR,&pos);
        while((c=fgetc(fileR))!=EOF)
        {
            if(c=='}')
            {
                fgetpos(fileR,&pos2);

                if(z==0) //if z value is 0,it means if is true,which we decide in iforifnot() method.
                {
                    fsetpos(fileR,&pos);  //if if is true and has a block after command,take the position of '{' and continue reading from this position in main.

                }
                else
                {
                    fsetpos(fileR,&pos2); //if if is false and has a block after command,take the position of '}' and continue reading from this position in main.
                }
                break;
            }
        }

    }
    else //if if doesn't have any block
    {
        if(z==0) //if if is true,
        {
            fseek(fileR, -1, SEEK_CUR);
            int i=0;
            char keywordlist[200]= {'0'};
            while((c = fgetc(fileR)) != EOF)
            {
                while(isspace(c)!=0 || iscntrl(c)!=0 || c=='}')
                {
                    c=fgetc(fileR);
                }
                while(isalpha(c))
                {
                    keywordlist[i++]=c;
                    c=fgetc(fileR);
                }
                if(isKeyword(keywordlist)==1)
                {
                    printf("%s \n",keywordlist);
                    reader(fileR,c,keywordlist);
                    memset(keywordlist, 0, sizeof(keywordlist));
                    i=0;
                    return;
                }
            }
        }
        else //if is false,
        {
            while((c = fgetc(fileR)) != '>')
            {
                continue;
            }
        }

    }
    ungetc(c,fileR);
}
void iforifnot(FILE* fileR,char c,int isIf) //in this function,check if the command in if<> is true or not.And return z value.Execute same operations for ifnot ,too.
{
    char directorylist[300]= {'0'};
    char s[100];
    int j=0;
    int z=0; //control value for if

    while(isspace(c)!=0) //ignore the spaces
    {
        c=fgetc(fileR);
    }
    if(c=='<')
    {
        while((c=fgetc(fileR))!='>')
        {
            directorylist[j++]=c;
        }
        if(directoryController(directorylist)!=0)  ////if the command is complete
        {
            char curr[PATH_MAX];
            getcwd(curr, sizeof(curr));

            char * token = strtok(directorylist, "/");
            while( token != NULL )
            {
                printf( "\n token  %s\n", token );
                if(strcmp(token,"*") == 0)
                {
                    char maxpathbefore[PATH_MAX];
                    char maxpathafter[PATH_MAX];

                    getcwd(maxpathbefore, sizeof(maxpathbefore));

                    chdir("..");

                    getcwd(maxpathafter, sizeof(maxpathafter));

                    if(strcmp(maxpathbefore,maxpathafter) ==0)
                    {
                        printf("You've reached the maximum directory... \n");
                        z++;
                    }
                    else
                    {
                         printf("%s found. \n Directory: %s \n",token, getcwd(s, 100));
                    }
                }
                else
                {
                    char after[PATH_MAX];

                    chdir(token);

                    getcwd(after, sizeof(after));
                    if(strcmp(curr, after) !=0)
                    {
                        printf("%s found. \n Directory: %s \n",token, getcwd(s, 100)); //if command in if<> is true,return z=0,
                    }
                    else
                    {
                        printf("%s cannot be found.\n",token);  //if command in if<> is false,return z!=0,
                        z++;
                    }
                }
                token = strtok(NULL, "/");
            }
            chdir(curr);

            if(isIf==1) //if keyword is if,do the opreations
            {
                ifParanthesis(fileR,c,z);

            }
            else if(isIf==0) //if keyword is ifnot,do the operations
            {
                ifnotParanthesis(fileR,c,z);

            }
        }
    }
}

int isKeyword(char list1[]) //check whether a command is a keyword or not
{
    char keywords[4][10] = {"go","make","if","ifnot"};
    for(int j=0; j<strlen(list1); j++) //make case sensitive
    {
        list1[j]=tolower(list1[j]);
    }

    for(int i=0; i<4; ++i)
    {
        if(strcmp(keywords[i], list1) == 0) //check equality of two strings
        {
            return 1;
        }
    }
    return 0; //return 0 if they are not equal

}

int main()
{
    int i=0; //index for our list
    char keywordlist[200]= {'0'};
    char c;
    FILE *fileR;
    fileR=fopen("x.pmk.txt","r");
    if(fileR== NULL)
    {
        printf("File not found !!\n");
        exit(0);
    }
    while((c = fgetc(fileR)) != EOF)
    {
        while(isspace(c)!=0 || iscntrl(c)!=0 || c=='}') //Control for space,/n,/t characters
        {
            c=fgetc(fileR);
        }
        while(isalpha(c))
        {
            keywordlist[i++]=c;
            c=fgetc(fileR);
        }
        if(isKeyword(keywordlist)==1) //if word is a keyword,then returns 1
        {
            reader(fileR,c,keywordlist); //it is a method for determining keyword type
            memset(keywordlist, 0, sizeof(keywordlist));//assigning all array elements to '0'
            i=0;
        }
    }
    return 0;

}

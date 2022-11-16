#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STRING_LENGTH 256
//-----------------------------------------------------------------------------
typedef char String[STRING_LENGTH];

typedef enum {
        none,
        text,
        lineNums
} lineSpecType;


typedef union {
        String text;
        int lineNums[2];
} lineSpec;

typedef struct{
        lineSpecType specEnum;
        lineSpec spec;
        char opSpec;
        String data;
} EditCommands;

EditCommands makeStruct(String commandLine) {
        EditCommands build;

        if (commandLine[strlen(commandLine) - 1] == '\n')
                        commandLine[strlen(commandLine) - 1] = '\0';

        if (commandLine[0] == '/') {
                build.specEnum = text;

                //finds the <text>
                String temp = "";
                int count = 1;
                while (commandLine[count] != '/') {
                count++;
                }

                strncpy(temp, commandLine + 1, count - 1);
                strcpy(build.spec.text, temp);

                //find the commandValue
                char commandValue;
                int indexOfCommand = 0;

                indexOfCommand = strlen(temp) + 2;
                commandValue = commandLine[indexOfCommand];
                build.opSpec = commandValue;

                //gets the string after command value
                strcpy(build.data, commandLine + (indexOfCommand + 1));
        } else if (isdigit(commandLine[0])) {
                build.specEnum = lineNums;

                String start;
                String end;

                //gets the first int
                int count = 0;
                while (commandLine[count] != ',') {
                count++;
                }
                build.spec.lineNums[0] = atoi(strncpy(start, commandLine, count));

                //gets the second int
                count++;
                int count2 = count;
                while(isdigit(commandLine[count2])) {
                count2++;
                }
                build.spec.lineNums[1] = atoi(strncpy(end, commandLine + count, count2 - 1));

                //find the commmandValue
                char commandValue;
                int indexOfCommand = 0;

                indexOfCommand = count2 + 1;
                commandValue = commandLine[indexOfCommand];
                build.opSpec = commandValue;

                //gets the string after command value
                strcpy(build.data, commandLine + (indexOfCommand + 1));
        } else {
                build.specEnum = none;
                build.opSpec = commandLine[0];
                strcpy(build.data, commandLine + 1);
        }

        return build;
        }

int readIn(FILE * commandFile, EditCommands editCommandArray[]) {
        String line;
        int count = 0;

        //goes through the command file and stores each commands 
        while(fgets(line, sizeof line, commandFile) != NULL && count < 100) {
                editCommandArray[count] = makeStruct(line);
                count++;
        }
        return count;
}

int lineRangeSpecifications(EditCommands editCommand, int currLineNumber, String input) {
        int valid = 0;
        switch (editCommand.specEnum) {
                case none:
                        return 1;
                        break;
                case text:
                        //if input contains the text return true
                        if (strstr(input, editCommand.spec.text) != NULL) {
                                valid = 1;
                        }
                        return valid;
                case lineNums:
                        //if the line range is valid return true
                        if (currLineNumber >= editCommand.spec.lineNums[0] && currLineNumber <= editCommand.spec.lineNums[1]) {
                                valid = 1;
                        }
                        return valid;
                        break;
                default:
                        return 0;
                        break;
        }
}

int makeEdit(EditCommands editCommand, String input) {
        String temp;
        char * ret;
        String oldString;
        String newString;

        switch (editCommand.opSpec) {
                case 'I':
                        //copies the data to temp and appends input to temp and then copies the whole sentence to input
                        strcpy(temp, editCommand.data);
                        strcat(temp, input);
                        strcpy(input, temp);
                        break;
                case 'A':
                        //copies the data to temp and appends it to input
                        strcpy(temp, editCommand.data);
                        strcat(input, temp);
                        break;
                case 'd':
                        //if sentence is suppose to be deleted, change sentence to DONT PRINT so later in main, it will check for DONT PRINT
                        strcpy(input, "DONT PRINT");
                        break;
                case 'O':
                        printf("%s\n", editCommand.data);
                        break;
                case 's':
                        //puts newString into input where oldString use to be
                        strcpy(temp, editCommand.data + 1);
                        
                        //puts the new string in its own place
                        ret = strchr(temp, '/');
                        strcpy(newString, ret + 1);
                        newString[strlen(newString) - 1] = '\0';
                        
                        //puts the old string in its own place
                        strncpy(oldString, temp, strlen(temp) - strlen(newString));
                        oldString[strlen(oldString) - 2] = '\0';

                        char* result; 
                        int i;
                        int cnt = 0; 
                        int newWlen = strlen(newString); 
                        int oldWlen = strlen(oldString); 
                        
                        // Counting the number of times old word 
                        // occur in the string 
                        for (i = 0; input[i] != '\0'; i++) { 
                                if (strstr(&input[i], oldString) == &input[i]) { 
                                        cnt++; 
                        
                                        // Jumping to index after the old word. 
                                        i += oldWlen - 1; 
                                } 
                        } 
                        
                        // Making new string of enough length 
                        result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1); 
                        
                        i = 0; 
                        while (*input) { 
                                // compare the substring with the result 
                                if (strstr(input, oldString) == input) { 
                                strcpy(&result[i], newString); 
                                i += newWlen; 
                                input += oldWlen; 
                                } 
                                else
                                result[i++] = *input++; 
                        } 
                        
                        result[i] = '\0';
                        memcpy(input, "", 1);
                        memcpy(input, result, strlen(result) + 1);
                        break; 
        }
        return 0;
}

//int argc, char * argv[]
int main(void) {
        FILE * commandFile;

        String input;
        String dontPrint = "DONT PRINT";
        int numOfEditCommands,currLineNumber,index;


        //checks for two command line arugments
        /*
        if (argc != 2) {
                printf("Make sure you enter two arguments\n");
                return (EXIT_FAILURE);
        }
        */
        
        //argv[1]
        //opens file and makes commandFile point to it  
        if ((commandFile = fopen("Commands.txt", "r")) == NULL) {
                perror("Opening command file");
                exit(EXIT_FAILURE);
        }

        EditCommands editCommandArray[100];

        numOfEditCommands = readIn(commandFile, editCommandArray);
        
        if (numOfEditCommands > 0) {
                currLineNumber = 0;
                //asks for user input and stores the input into a string called input
                while(fgets(input, STRING_LENGTH, stdin) != NULL) {
                        if (input[strlen(input) -1] == '\n') 
                                input[strlen(input) - 1] = '\0';
                        
                        currLineNumber++;
                        //goes through every command stored in the editCommandArray 
                        for (index = 0; index < numOfEditCommands; index++) {
                                //checks if range spec of the command applies to the input
                                //if valid it will go to makeEdit and procceed with the right command
                                if (lineRangeSpecifications(editCommandArray[index], currLineNumber, input)) {
                                        int tempLength = strlen(input);
                                        makeEdit(editCommandArray[index], input);
                                        if(editCommandArray[index].opSpec == 's') {
                                                strcpy(input, input + tempLength);
                                        }
                                }
                        }         
                        //prints the output and check for DONT PRINT   
                        if (strstr(input, dontPrint) == NULL) {
                                printf("%s\n", input);
                        }
                }
        }
        return (EXIT_SUCCESS);
}
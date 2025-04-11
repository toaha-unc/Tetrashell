#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "tetris.h"

void shortenQSName(char a[FILENAME_MAX], char b[7]) {
  int j = 0;
  for (int i = 0; i < 6; ++i) {
    while (a[j] == '.' || a[j] == '/') {
      j++;
    }
    b[i] = a[j];
    j++;
  }
  b[6] = '\0';
}

int main(int argc, char *argv[]) {
  char quicksave[FILENAME_MAX];
  printf(
      "Welcome to...\n _____     _             __ _          _ _ \n/__   "
      "\\___| |_ _ __ __ _/ _\\ |__   ___| | |\n  / /\\/ _ \\ __| '__/ _` \\ "
      "\\| '_ \\ / _ \\ | |\n / / |  __/ |_| | | (_| |\\ \\ | | |  __/ | |\n "
      "\\/   \\___|\\__|_|  \\__,_\\__/_| |_|\\___|_|_|\n                      "
      "                     \nthe ultimate Tetris quicksave hacking "
      "tool!\nEnter the path to the quicksave you'd like to begin hacking: ");
  scanf("%s", quicksave);
  if (access(quicksave, F_OK) != 0) {
    fprintf(stderr, "The quicksave '%s' does not exist.\n", quicksave);
    return EXIT_FAILURE;
  }
  printf(
      "'%s' set as the current quicksave.\nEnter your command below to get "
      "started:\n",
      quicksave);

  TetrisGameState currentGame;
  FILE *fileForThePrompt = fopen(quicksave, "rb");
  if (fileForThePrompt == NULL) {
    fprintf(stderr, "Could not open %s\n", quicksave);
    return EXIT_FAILURE;
  }
  fread(&currentGame, sizeof(TetrisGameState), 1, fileForThePrompt);
  fclose(fileForThePrompt);
  char shortenedQSName[7];
  shortenQSName(quicksave, shortenedQSName);
  printf("\x1b[38;2;0;255;255m%s@Tshell[%s...][score: %d/lines: %d]>\x1b[m ",
         getlogin(), shortenedQSName, currentGame.score, currentGame.lines);

  TetrisGameState *theBuffer = NULL;
  int lengthOfTheBuffer = 0;
  char command[FILENAME_MAX];
  fgetc(stdin);  // Takes care of the new line left by scanf
  while (fgets(command, sizeof(command), stdin) != NULL) {
    bool errorFlag = false;
    int numberOfWords = 1;
    for (int n = 0; command[n] != '\0'; n++) {
      if (command[n] == ' ') {
        numberOfWords++;
      }
    }
    if (strstr(command, "help") != NULL) {
      if (numberOfWords != 1 && numberOfWords != 2) {
        errorFlag = true;
        goto errorMessage;
      }
      strtok(command, " ");  // Gets the first argument "help"
      char *secondArgument = strtok(NULL, " ");  // Gets the second argument
      if (secondArgument != NULL) {
        secondArgument[strlen(secondArgument) - 1] = '\0';
        if (strcmp(secondArgument, "exit") == 0) {
          printf("This command exits from the tetrashell.\n");
        } else if (strcmp(secondArgument, "recover") == 0) {
          printf(
              "This command calls the 'recover' program to recover quicksaves "
              "with the provided disk image path. It also provides a "
              "pleasant interface for the user to view the recover command "
              "output and the option to switch the current quicksave to one of "
              "the recovered ones.\n");
        } else if (strcmp(secondArgument, "check") == 0) {
          printf(
              "This command calls the 'check' program with the current "
              "quicksave to verify if it passes the legitimacy check.\n");
        } else if (strcmp(secondArgument, "modify") == 0) {
          printf(
              "This command calls the 'modify' program with the current "
              "quicksave to modify a field of the quicksave by changing its "
              "value.\n");
        } else if (strcmp(secondArgument, "rank") == 0) {
          printf(
              "This command calls the 'rank' program with the current "
              "quicksave to rank and display the number of top quicksaves "
              "based on the selected metrics.\n");
        } else if (strcmp(secondArgument, "switch") == 0) {
          printf(
              "This command permits the user to change the current "
              "quicksave.\n");
        } else if (strcmp(secondArgument, "info") == 0) {
          printf(
              "This command prints basic information about the current "
              "quicksave, such as its path, score, and lines.\n");
        } else if (strcmp(secondArgument, "undo") == 0) {
          printf(
              "This command reverts the last modify command executed on the "
              "current quicksave.\n");
        } else if (strcmp(secondArgument, "train") == 0) {
          printf(
              "This command runs a conversion trainer, which invites the user "
              "to convert hex numbers to binary or decimal and guides the user "
              "along the way.\n");
        } else if (strcmp(secondArgument, "help") == 0) {
          printf(
              "This command displays summarized information about what the "
              "alongside-provided command does.\n");
        } else {
          printf(
              "Incorrect arguments. Type 'help' to search for assistance with "
              "a particular command.\n");
        }
      } else {
        printf(
            "Add a command after typing help to view its help description. "
            "Available commands are as "
            "follows:"
            "\nmodify\nrank\ncheck\nrecover\nexit\nswitch\nhelp\ninfo\nundo\ntr"
            "ain\n");
      }
    } else if (strstr(command, "exit") != NULL) {
      if (numberOfWords != 1) {
        errorFlag = true;
        goto errorMessage;
      }
      break;
    } else if (strstr(command, "recover") != NULL &&
               strstr(command, "recovered") == NULL) {
      if (numberOfWords != 2) {
        errorFlag = true;
        goto errorMessage;
      }
      char *my_args[3] = {"./recover", NULL, NULL};
      strtok(command, " ");  // Gets the first argument "recover"
      char *secondArgument = strtok(NULL, " ");  // Gets the second argument
      secondArgument[strlen(secondArgument) - 1] = '\0';
      if (access(secondArgument, F_OK) != 0) {
        fprintf(stderr, "The disk '%s' does not exist.\n", secondArgument);
        return EXIT_FAILURE;
      }
      my_args[1] = (char *)malloc(strlen(secondArgument) + 1);
      sprintf(my_args[1], "%s", secondArgument);
      int fd[2];
      if (pipe(fd) == -1) {
        fprintf(stderr, "The pipe function failed to create a pipe.\n");
        return EXIT_FAILURE;
      }
      pid_t pid = fork();
      if (pid == -1) {
        fprintf(stderr,
                "The fork system call failed to create a new process.\n");
        return EXIT_FAILURE;
      } else if (pid == 0) {  // In the Child Process
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        dup2(fd[1], STDERR_FILENO);
        close(fd[1]);
        if (execvp(my_args[0], my_args) == -1) {
          fprintf(stderr, "Could not execute the argument %s\n", my_args[0]);
          return EXIT_FAILURE;
        }
      } else {  // In the Parent Process
        if (wait(0) == -1) {
          perror("Wait Error: ");
          return EXIT_FAILURE;
        }
        free(my_args[1]);
        my_args[1] = NULL;
        char **theRecoverBuffer;
        if ((theRecoverBuffer = (char **)malloc(sizeof(char *))) == NULL) {
          fprintf(stderr, "%s", "Out of Memory, malloc failed.\n");
          return EXIT_FAILURE;
        }
        int lengthOfTheRecoverBuffer = 0;
        pid_t secondPid = fork();
        if (secondPid == -1) {
          fprintf(stderr,
                  "The fork system call failed to create a new process.\n");
          return EXIT_FAILURE;
        } else if (secondPid == 0) {  // In the Child Process
          close(fd[1]);
          dup2(fd[0], STDIN_FILENO);
          close(fd[0]);
          char temporaryString[FILENAME_MAX];
          FILE *temporaryFile = fopen("recoverFile.txt", "w");
          if (temporaryFile == NULL) {
            fprintf(stderr, "Could not open recoverFile.txt\n");
            return EXIT_FAILURE;
          }
          while (fgets(temporaryString, FILENAME_MAX, stdin) != NULL) {
            char *secondTemporaryString = strtok(temporaryString, "\n");
            if (strstr(secondTemporaryString, ".bin") != NULL) {
              fputs(secondTemporaryString, temporaryFile);
              fputs("\n", temporaryFile);
            }
          }
          fclose(temporaryFile);
        } else {  // In the Parent Process
          close(fd[0]);
          close(fd[1]);
          if (wait(0) == -1) {
            perror("Wait Error: ");
            return EXIT_FAILURE;
          }
          FILE *temporaryFile = fopen("recoverFile.txt", "r");
          if (temporaryFile == NULL) {
            fprintf(stderr, "Could not open recoverFile.txt\n");
            return EXIT_FAILURE;
          }
          char temporaryString[FILENAME_MAX];
          while (fgets(temporaryString, FILENAME_MAX, temporaryFile) != NULL) {
            int lengthOfTemporaryString = strlen(temporaryString);
            temporaryString[lengthOfTemporaryString - 1] = '\0';
            if ((theRecoverBuffer[lengthOfTheRecoverBuffer] = (char *)malloc(
                     lengthOfTemporaryString * sizeof(char))) == NULL) {
              fprintf(stderr, "%s", "Out of Memory, realloc failed.\n");
              return EXIT_FAILURE;
            }
            strcpy(theRecoverBuffer[lengthOfTheRecoverBuffer], temporaryString);
            lengthOfTheRecoverBuffer++;
            if ((theRecoverBuffer = (char **)realloc(
                     theRecoverBuffer, (1 + lengthOfTheRecoverBuffer) *
                                           sizeof(char *))) == NULL) {
              fprintf(stderr, "%s", "Out of Memory, realloc failed.\n");
              return EXIT_FAILURE;
            }
          }
          fclose(temporaryFile);
          printf(
              "Recovered quicksaves:\n----  --------------------- ------- "
              "-------\n#     File path             Score   Lines\n----  "
              "--------------------- ------- -------\n");
          for (int j = 0; j < lengthOfTheRecoverBuffer; j++) {
            TetrisGameState theObject;
            char fileName[FILENAME_MAX];
            sprintf(fileName, "./%s", theRecoverBuffer[j]);
            FILE *theFilePointerThatReads = fopen(fileName, "rb");
            if (theFilePointerThatReads == NULL) {
              fprintf(stderr, "Could not open %s\n", quicksave);
              return EXIT_FAILURE;
            }
            fread(&theObject, sizeof(TetrisGameState), 1,
                  theFilePointerThatReads);
            fclose(theFilePointerThatReads);
            printf("#%-3i  %-21s %-5i   %-3i\n", j + 1, theRecoverBuffer[j],
                   theObject.score, theObject.lines);
          }
          char text[FILENAME_MAX];
          printf("Would you like to switch to one of these (y/n): ");
          fgets(text, sizeof(text), stdin);
          if (strstr(text, "y") != NULL) {
            printf("Which quicksave (enter a #): ");
            fgets(text, sizeof(text), stdin);
            int i = atol(text) - 1;
            strcpy(quicksave, theRecoverBuffer[i]);
            if (lengthOfTheBuffer != 0) {
              free(theBuffer);
              theBuffer = NULL;
              lengthOfTheBuffer = 0;
            }
            FILE *fileForThePrompt = fopen(quicksave, "rb");
            if (fileForThePrompt == NULL) {
              fprintf(stderr, "Could not open %s\n", quicksave);
              return EXIT_FAILURE;
            }
            fread(&currentGame, sizeof(TetrisGameState), 1, fileForThePrompt);
            fclose(fileForThePrompt);
            shortenQSName(quicksave, shortenedQSName);
            printf("Done! Current quicksave is now '%s'\n", quicksave);
          }
          for (int k = 0; k < lengthOfTheRecoverBuffer; k++) {
            free(theRecoverBuffer[k]);
            theRecoverBuffer[k] = NULL;
          }
          free(theRecoverBuffer);
          theRecoverBuffer = NULL;
          remove("recoverFile.txt");
        }
      }
    } else if (strstr(command, "check") != NULL) {
      if (numberOfWords != 1) {
        errorFlag = true;
        goto errorMessage;
      }
      char *my_args[3] = {"./check", NULL, NULL};
      my_args[1] = (char *)malloc(strlen(quicksave) + 1);
      sprintf(my_args[1], "%s", quicksave);
      pid_t pid = fork();
      if (pid == -1) {
        fprintf(stderr,
                "The fork system call failed to create a new process.\n");
        return EXIT_FAILURE;
      } else if (pid == 0) {  // In the Child Process
        if (execvp(my_args[0], my_args) == -1) {
          fprintf(stderr, "Could not execute the argument %s\n", my_args[0]);
          return EXIT_FAILURE;
        }
      } else {  // In the Parent Process
        if (wait(0) == -1) {
          perror("Wait Error: ");
          return EXIT_FAILURE;
        }
        free(my_args[1]);
        my_args[1] = NULL;
      }
    } else if (strstr(command, "modify") != NULL) {
      if (numberOfWords != 3) {
        errorFlag = true;
        goto errorMessage;
      }
      FILE *theFilePointerThatReads = fopen(quicksave, "rb");
      if (theFilePointerThatReads == NULL) {
        fprintf(stderr, "Could not open %s\n", quicksave);
        return EXIT_FAILURE;
      }
      char *my_args[5] = {"./modify", NULL, NULL, NULL, NULL};
      strtok(command, " ");  // Gets the first argument "modify"
      char *secondArgument = strtok(NULL, " ");  // Gets the second argument
      if (strcmp(secondArgument, "score") != 0 &&
          strcmp(secondArgument, "lines") != 0 &&
          strcmp(secondArgument, "next_piece") != 0) {
        errorFlag = true;
        goto errorMessage;
      }
      char *thirdArgument = strtok(NULL, " ");  // Gets the third argument
      thirdArgument[strlen(thirdArgument) - 1] = '\0';
      my_args[1] = (char *)malloc(strlen(secondArgument) + 1);
      my_args[2] = (char *)malloc(strlen(thirdArgument) + 1);
      my_args[3] = (char *)malloc(strlen(quicksave) + 1);
      sprintf(my_args[1], "%s", secondArgument);
      sprintf(my_args[2], "%s", thirdArgument);
      sprintf(my_args[3], "%s", quicksave);
      if ((theBuffer = (TetrisGameState *)realloc(
               theBuffer, (1 + lengthOfTheBuffer) * sizeof(TetrisGameState))) ==
          NULL) {
        fprintf(stderr, "%s", "Out of Memory, realloc failed.\n");
        return EXIT_FAILURE;
      }
      fread(&theBuffer[lengthOfTheBuffer], sizeof(TetrisGameState), 1,
            theFilePointerThatReads);
      fclose(theFilePointerThatReads);
      lengthOfTheBuffer++;
      pid_t pid = fork();
      if (pid == -1) {
        fprintf(stderr,
                "The fork system call failed to create a new process.\n");
        return EXIT_FAILURE;
      } else if (pid == 0) {  // In the Child Process
        if (execvp(my_args[0], my_args) == -1) {
          fprintf(stderr, "Could not execute the argument %s\n", my_args[0]);
          return EXIT_FAILURE;
        }
      } else {  // In the Parent Process
        if (wait(0) == -1) {
          perror("Wait Error: ");
          return EXIT_FAILURE;
        }
        free(my_args[1]);
        my_args[1] = NULL;
        free(my_args[2]);
        my_args[2] = NULL;
        free(my_args[3]);
        my_args[3] = NULL;
        FILE *fileForThePrompt = fopen(quicksave, "rb");
        if (fileForThePrompt == NULL) {
          fprintf(stderr, "Could not open %s\n", quicksave);
          return EXIT_FAILURE;
        }
        fread(&currentGame, sizeof(TetrisGameState), 1, fileForThePrompt);
        fclose(fileForThePrompt);
      }
    } else if (strstr(command, "rank") != NULL) {
      if (numberOfWords > 3) {
        errorFlag = true;
        goto errorMessage;
      }
      int fileDestination[2];
      if (pipe(fileDestination) == -1) {
        fprintf(stderr, "The pipe function failed to create a pipe.\n");
        return EXIT_FAILURE;
      }
      char *my_args[3] = {"echo", NULL, NULL};
      my_args[1] = (char *)malloc(strlen(quicksave) + 1);
      sprintf(my_args[1], "%s", quicksave);
      pid_t pid = fork();
      int length;
      if (pid == -1) {
        fprintf(stderr,
                "The fork system call failed to create a new process.\n");
        return EXIT_FAILURE;
      } else if (pid == 0) {  // In the Child Process
        close(fileDestination[0]);
        dup2(fileDestination[1], STDOUT_FILENO);
        close(fileDestination[1]);
        if (execvp(my_args[0], my_args) == -1) {
          fprintf(stderr, "Could not execute the argument %s\n", my_args[0]);
          return EXIT_FAILURE;
        }
      } else {  // In the Parent Process
        if (wait(0) == -1) {
          perror("Wait Error: ");
          return EXIT_FAILURE;
        }
        free(my_args[1]);
        my_args[1] = NULL;
        char *my_args[5] = {"./rank", NULL, NULL, "uplink", NULL};
        strtok(command, " ");  // Gets the first argument "rank"
        char *secondArgument = strtok(NULL, " ");  // Gets the second argument
        bool flag;
        if (secondArgument == NULL) {
          secondArgument = "score";
          my_args[1] = (char *)malloc(strlen(secondArgument) + 1);
          sprintf(my_args[1], "%s", secondArgument);
          char *thirdArgument = "10000";
          my_args[2] = (char *)malloc(strlen(thirdArgument) + 1);
          sprintf(my_args[2], "%s", thirdArgument);
          flag = true;
          length = 11;
        } else {
          char *thirdArgument = strtok(NULL, " ");  // Gets the third argument
          if (thirdArgument == NULL) {
            secondArgument[strlen(secondArgument) - 1] = '\0';
            my_args[1] = (char *)malloc(strlen(secondArgument) + 1);
            sprintf(my_args[1], "%s", secondArgument);
            char *thirdArgument = "10000";
            my_args[2] = (char *)malloc(strlen(thirdArgument) + 1);
            sprintf(my_args[2], "%s", thirdArgument);
            flag = true;
            length = 11;
          } else {
            my_args[1] = (char *)malloc(strlen(secondArgument) + 1);
            sprintf(my_args[1], "%s", secondArgument);
            thirdArgument[strlen(thirdArgument) - 1] = '\0';
            my_args[2] = (char *)malloc(strlen(thirdArgument) + 1);
            sprintf(my_args[2], "%s", thirdArgument);
            flag = false;
            length = atol(thirdArgument);
          }
        }
        if (strcmp(secondArgument, "score") != 0 &&
            strcmp(secondArgument, "lines") != 0 &&
            strcmp(secondArgument, "next_piece") != 0) {
          fprintf(stderr, "Incorrect arguments were given.\n");
          return EXIT_FAILURE;
        }
        pid_t secondPid = fork();
        if (secondPid == -1) {
          fprintf(stderr,
                  "The fork system call failed to create a new process.\n");
          return EXIT_FAILURE;
        } else if (secondPid == 0) {  // In the Child Process
          close(fileDestination[1]);
          dup2(fileDestination[0], STDIN_FILENO);
          close(fileDestination[0]);
          FILE *temporaryFile = fopen("rankFile.txt", "w");
          if (temporaryFile == NULL) {
            fprintf(stderr, "Could not open rankFile.txt\n");
            return EXIT_FAILURE;
          }
          dup2(fileno(temporaryFile), STDOUT_FILENO);
          dup2(fileno(temporaryFile), STDERR_FILENO);
          if (execvp(my_args[0], my_args) == -1) {
            fprintf(stderr, "Could not execute the argument %s\n", my_args[0]);
            return EXIT_FAILURE;
          }
          fclose(temporaryFile);
        } else {  // In the Parent Process
          close(fileDestination[0]);
          close(fileDestination[1]);
          if (wait(0) == -1) {
            perror("Wait Error: ");
            return EXIT_FAILURE;
          }
          free(my_args[1]);
          my_args[1] = NULL;
          free(my_args[2]);
          my_args[2] = NULL;
          FILE *temporaryFile = fopen("rankFile.txt", "r");
          if (temporaryFile == NULL) {
            fprintf(stderr, "Could not open rankFile.txt\n");
            return EXIT_FAILURE;
          }
          char **theRankBuffer;
          if ((theRankBuffer = (char **)malloc(sizeof(char *))) == NULL) {
            fprintf(stderr, "%s", "Out of Memory, malloc failed.\n");
            return EXIT_FAILURE;
          }
          int lengthOfTheRankBuffer = 0;
          int desiredNumber;
          char temporaryString[FILENAME_MAX];
          while (fgets(temporaryString, FILENAME_MAX, temporaryFile) != NULL) {
            int lengthOfTemporaryString = strlen(temporaryString);
            temporaryString[lengthOfTemporaryString - 1] = '\0';
            if (strstr(temporaryString, getlogin()) != NULL) {
              desiredNumber = lengthOfTheRankBuffer;
            }
            if ((theRankBuffer[lengthOfTheRankBuffer] = (char *)malloc(
                     lengthOfTemporaryString * sizeof(char))) == NULL) {
              fprintf(stderr, "%s", "Out of Memory, realloc failed.\n");
              return EXIT_FAILURE;
            }
            strcpy(theRankBuffer[lengthOfTheRankBuffer], temporaryString);
            lengthOfTheRankBuffer++;
            if ((theRankBuffer = (char **)realloc(
                     theRankBuffer,
                     (1 + lengthOfTheRankBuffer) * sizeof(char *))) == NULL) {
              fprintf(stderr, "%s", "Out of Memory, realloc failed.\n");
              return EXIT_FAILURE;
            }
          }
          fclose(temporaryFile);
          if (flag) {
            int lower = desiredNumber - (length / 2);
            int upper = desiredNumber + (length / 2);
            if (lower < 0) {
              upper = upper - lower;
              lower = 0;
            }
            if (upper >= lengthOfTheRankBuffer) {
              lower = lower - (upper - lengthOfTheRankBuffer) + 1;
              upper = lengthOfTheRankBuffer - 1;
            }
            for (int j = lower; j <= upper; j++) {
              if (j == desiredNumber) {
                printf("\033[1m>>> %-4i %s <<<\033[0m\n", j + 1,
                       theRankBuffer[j]);
              } else {
                printf("%-4i %s\n", j + 1, theRankBuffer[j]);
              }
            }
          } else {
            for (int j = 0; j < lengthOfTheRankBuffer; j++) {
              printf("%-4i %s\n", j + 1, theRankBuffer[j]);
            }
          }
          for (int k = 0; k < lengthOfTheRankBuffer; k++) {
            free(theRankBuffer[k]);
            theRankBuffer[k] = NULL;
          }
          free(theRankBuffer);
          theRankBuffer = NULL;
          remove("rankFile.txt");
        }
      }
    } else if (strstr(command, "switch") != NULL) {
      if (numberOfWords != 2) {
        errorFlag = true;
        goto errorMessage;
      }
      strtok(command, " ");  // Gets the first argument "switch"
      char *secondArgument = strtok(NULL, " ");  // Gets the second argument
      secondArgument[strlen(secondArgument) - 1] = '\0';
      if (access(secondArgument, F_OK) != 0) {
        fprintf(stderr, "The quicksave '%s' does not exist.\n", secondArgument);
        return EXIT_FAILURE;
      }
      printf("Switched the current quicksave from '%s' to '%s'.\n", quicksave,
             secondArgument);
      strcpy(quicksave, secondArgument);
      if (lengthOfTheBuffer != 0) {
        free(theBuffer);
        theBuffer = NULL;
        lengthOfTheBuffer = 0;
      }
      FILE *fileForThePrompt = fopen(quicksave, "rb");
      if (fileForThePrompt == NULL) {
        fprintf(stderr, "Could not open %s\n", quicksave);
        return EXIT_FAILURE;
      }
      fread(&currentGame, sizeof(TetrisGameState), 1, fileForThePrompt);
      fclose(fileForThePrompt);
      shortenQSName(quicksave, shortenedQSName);
    } else if (strstr(command, "info") != NULL) {
      if (numberOfWords != 1) {
        errorFlag = true;
        goto errorMessage;
      }
      TetrisGameState theObject;
      FILE *theFilePointerThatReads = fopen(quicksave, "rb");
      if (theFilePointerThatReads == NULL) {
        fprintf(stderr, "Could not open %s\n", quicksave);
        return EXIT_FAILURE;
      }
      fread(&theObject, sizeof(TetrisGameState), 1, theFilePointerThatReads);
      fclose(theFilePointerThatReads);
      printf("Current Savefile: %s\nScore: %d\nLines: %d\n", quicksave,
             theObject.score, theObject.lines);
    } else if (strstr(command, "undo") != NULL) {
      if (numberOfWords != 1) {
        errorFlag = true;
        goto errorMessage;
      }
      if (lengthOfTheBuffer == 0) {
        printf("Cannot revert the quicksave any further.\n");
      } else {
        FILE *theFilePointerThatReads = fopen(quicksave, "wb");
        if (theFilePointerThatReads == NULL) {
          fprintf(stderr, "Could not open %s\n", quicksave);
          return EXIT_FAILURE;
        }
        fwrite(&theBuffer[lengthOfTheBuffer - 1], sizeof(TetrisGameState), 1,
               theFilePointerThatReads);
        fclose(theFilePointerThatReads);
        lengthOfTheBuffer--;
        if (lengthOfTheBuffer != 0) {
          if ((theBuffer = (TetrisGameState *)realloc(
                   theBuffer, (lengthOfTheBuffer) * sizeof(TetrisGameState))) ==
              NULL) {
            fprintf(stderr, "%s", "Out of Memory, realloc failed.\n");
            return EXIT_FAILURE;
          }
        } else {
          free(theBuffer);
          theBuffer = NULL;
        }
        FILE *fileForThePrompt = fopen(quicksave, "rb");
        if (fileForThePrompt == NULL) {
          fprintf(stderr, "Could not open %s\n", quicksave);
          return EXIT_FAILURE;
        }
        fread(&currentGame, sizeof(TetrisGameState), 1, fileForThePrompt);
        fclose(fileForThePrompt);
      }
    } else if (strstr(command, "train") != NULL) {
      if (numberOfWords != 1) {
        errorFlag = true;
        goto errorMessage;
      }
      char *my_args[2] = {"./train", NULL};
      pid_t pid = fork();
      if (pid == -1) {
        fprintf(stderr,
                "The fork system call failed to create a new process.\n");
        return EXIT_FAILURE;
      } else if (pid == 0) {  // In the Child Process
        if (execvp(my_args[0], my_args) == -1) {
          fprintf(stderr, "Could not execute the argument %s\n", my_args[0]);
          return EXIT_FAILURE;
        }
      } else {  // In the Parent Process
        if (wait(0) == -1) {
          perror("Wait Error: ");
          return EXIT_FAILURE;
        }
        free(my_args[1]);
        my_args[1] = NULL;
      }
    } else {
      printf(
          "This command is not supported. Please type 'help' to view the "
          "supported commands.\n");
    }
  errorMessage:
    if (errorFlag) {
      printf(
          "You have passed incorrect arguments. Please refer to the README to "
          "view the supported commands and the arguments they take.\n");
    }
    printf("\x1b[38;2;0;255;255m%s@Tshell[%s...][score: %d/lines: %d]>\x1b[m ",
           getlogin(), shortenedQSName, currentGame.score, currentGame.lines);
  }
  if (theBuffer != NULL) {
    free(theBuffer);
    theBuffer = NULL;
  }
  return EXIT_SUCCESS;
}

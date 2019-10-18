

Skip to content
Using Rochester Institute of Technology Mail with screen readers

1 of 1,437
Fwd: project3

Drake Busdiecker
Attachments
12:19 AM (13 minutes ago)
to Jessica



---------- Forwarded message ---------
From: Drake Busdiecker <dxb9392@pinkfloyd.cs.rit.edu>
Date: Fri, Oct 18, 2019 at 12:15 AM
Subject: project3
To: <dxb9392@rit.edu>



Attachments area

/*threads.c
 * Main program for CSCI-243 Project number 3.
 * author: Drake Busdiecker*/

#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ncurses.h>
#include <ctype.h>

int maxH, widee, running = 1;
pthread_mutex_t raceLock;

/// Run function for player thread. Creates a "sheild" on the screen 5 spaces above
/// tallest building. Can be moved left or right by user input.
/// @param name Name of the defender, supplied by config file in main.
/// @return NULL
void *runPlayer(void *name){
	char *player = "#####", *clear = "     ";
	int wide = getmaxx(stdscr);;
	int locate = (wide/2)-2, go = 1, height = maxH - 5;
	mvprintw(height, locate, player);
	refresh();
	pthread_mutex_init(&raceLock, NULL);
	while (go == 1){
		int ch = getch();
		switch(ch){
			case KEY_LEFT:
				if (locate != 0){
					pthread_mutex_lock(&raceLock);
					mvprintw(height, locate, clear);
					locate--;
					wmove(stdscr, height, locate);
					printw(player);
					refresh();
					pthread_mutex_unlock(&raceLock);
				}
				break;
			case KEY_RIGHT:
				if (locate != wide - 5){
					pthread_mutex_lock(&raceLock);
					mvprintw(height, locate, clear);
                                	locate++;
                        	        wmove(stdscr, height, locate);
        	                        printw(player);
	                                refresh();
                                	pthread_mutex_unlock(&raceLock);
				}
                                break;
			case 'q':
				mvprintw(3, (wide/2)-20, "The %s", name);
				mvprintw(3, (wide/2)-20+(4+strlen(name)), "defense has ended!");
				refresh();
				running = 0;
				go = 0;
				break;
		}
	}
	return NULL;
}

/// Run function for missile threads. Moves Missiles from top of screen until it hits something.
/// @param name Name of the attacker, supplied by config file in main.
/// @return NULL
void *runMissile(void *name){
	const char *missile = "|", *space = " ", *smoke = "?", *rubble= "*";
	int col = rand()%(widee + 1), fall = 2;
	int go = 1, wide = getmaxx(stdscr);
	int fallSpeed = rand()%(750000 + 1 - 250000) + 250000;
	mvprintw(fall, col, missile);
	fall++;
	while (go == 1){
		usleep(fallSpeed);
		if (running != 1){
			mvprintw(5, (wide/2)-20, "The %s", name);
			mvprintw(5, (wide/2)-20+(4+strlen(name)), "attack has ended!");
			refresh();
			break;
		}
		pthread_mutex_lock(&raceLock);
		if (mvinch(fall, col) == *smoke){
			if ((fall + 1 == getmaxy(stdscr) - 2)){
				mvprintw(fall - 1, col, space);
				go = 0;
			}
			else if ((mvinch(fall + 2, col) != *space) || ((mvinch(fall + 2, col) == *space) &&
				mvinch(fall + 1, col) == *rubble)){
				mvprintw(fall - 1, col, space);
				mvprintw(fall, col, space);
                        	mvprintw(fall + 1, col, smoke);
				mvprintw(fall + 2, col, rubble);
				go = 0;
			}
			else{
				mvprintw(fall - 1, col, space);
				mvprintw(fall, col, missile);
			}
		}
		else if (mvinch(fall, col) == *space){
			mvprintw(fall - 1, col, space);
			mvprintw(fall, col, missile);
		}
		else{
			mvprintw(fall - 1, col, smoke);
			mvprintw(fall, col, rubble);
			go = 0;
		}
		refresh();
		pthread_mutex_unlock(&raceLock);
		fall++;
	}
	if (running != 1){
	}
	return NULL;
}

///Main function of the project. Creates the city and all threads needed after processing config file.
///@param argc Number of command line args
///@param argv Command line args. Only accepts one in the form of a configuration file
//@return an integer representing successful completion.
int main(int argc, char **argv){
	char defender[81], attacker[81], missilesBuff[81];
	FILE *config;
	int check = 0, miss = 0, missiles;
	int height, width;
	char *pound = "#";
	srand(time(NULL));
	if (argc !=2 && argc != 1){
		fprintf(stderr, "Usage: ./threads config-file\n");
		if (argc == 1)
			fprintf(stderr, "Missing config file\n");
		else
			fprintf(stderr, "Too many arguments\n");
		return EXIT_FAILURE;
	}
	if (argc == 2){
		config = fopen(argv[1], "r");
	}
	else
		config = stdin;
	if (config == NULL){
		fprintf(stderr, "ERROR: Invalid filename (%s)\n", argv[1]);
		return EXIT_FAILURE;
	}
	fgets(defender, 80, config);
	while(true){
		if (defender[0] == *pound){
			fgets(defender, 80, config);
			if (feof(config)){
				fprintf(stderr, "ERROR: No defender name provided\n");
        	                fclose(config);
	                        return EXIT_FAILURE;
				}
		}
		else if (feof(config)){
        	        fprintf(stderr, "ERROR: No defender name provided\n");
	                fclose(config);
                	return EXIT_FAILURE;
        }
		else
			break;
	}
	if (defender == 0 || defender == NULL){
		fprintf(stderr, "ERROR: No defender name provided\n");
		fclose(config);
		return EXIT_FAILURE;
	}
	fgets(attacker, 80, config);
	printf("%s", defender);
	while(true){
                if (attacker[0] == *pound){
                        fgets(attacker, 80, config);
			if (feof(config)){
                                fprintf(stderr, "ERROR: No defender name provided\n");
                                fclose(config);
                                return EXIT_FAILURE;
                                }
                }
		else if (feof(config)){
                	fprintf(stderr, "ERROR: No attacker name provided\n");
        	        fclose(config);
	                return EXIT_FAILURE;
        }
                else
                        break;
	}
	if (attacker == 0){
		fprintf(stderr, "ERROR: No attacker name provided\n");
                fclose(config);
                return EXIT_FAILURE;
	}
	printf("%s", attacker);
	fgets(missilesBuff, 80, config);
	char *testMiss;
	     while(true){
                if (missilesBuff[0] == *pound){
                        fgets(missilesBuff, 80, config);
			if (feof(config)){
				fprintf(stderr, "ERROR: Invalid missile number\n");
                                fclose(config);
                                return EXIT_FAILURE;
			}
		}
                else{
			strtok(missilesBuff, "\n");
			missiles = strtol(missilesBuff, &testMiss, 10);
			if ((missiles == 0 && missilesBuff[0] != 48) || feof(config)){
				fprintf(stderr, "ERROR: Invalid missile number\n");
				fclose(config);
				return EXIT_FAILURE;
			}
                        break;
		}
        }
	/*check = fscanf(config, "%d", &missiles);
	if (missiles == 0 && check == 0){
		fprintf(stderr, "ERROR: invalid input for missile number\n");
		fclose(config);
		return EXIT_FAILURE;
	}*/
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, height, width);
	widee = width;
	char bBuff[81];
	char *tok, *checkk, *tester;
	int buildings[width], bIndex = 0;
	long int tokInt;
	while (bIndex < width){
	//	fscanf(config, "%d", &current);
		checkk = fgets(bBuff, 80, config);
		if (bBuff[0] == *pound)
			checkk == fgets(bBuff, 80, config);
		while (1){
			if (bBuff[0] == *pound){
				checkk == fgets(bBuff, 80, config);
				printf("%s\n", bBuff);
				if (feof(config)){
					fclose(config);
                                 	endwin();
                         	        fprintf(stderr, "ERROR: Missing city configuration\n");
                                	 return EXIT_FAILURE;
				}
			}
			else if (bIndex == 0 && feof(config)){
				 fclose(config);
				 endwin();               
				 fprintf(stderr, "ERROR: Missing city configuration\n");
				 return EXIT_FAILURE;
		}
			else
				break;
		}
		//else{
			tok = strtok(bBuff, " ");
			tokInt = strtol(tok, &tester, 10);
			while (tok != NULL && !feof(config)){
				if (tok != NULL)
                                        tokInt = strtol(tok, &tester, 10);
                                if (tokInt == 0 && bIndex != 122 && !feof(config)){
                                        fclose(config);
                                        endwin();
                                        fprintf(stderr, "ERROR: Invalid city configuration\n");
                                        return EXIT_FAILURE;
                                }
				buildings[bIndex] = tokInt;
				bIndex++;
				tok = strtok(NULL, " ");
			}
		//}
		buildings[bIndex] = 2;
		bIndex++;
	}
	fclose(config);
	int curHeight, xIndex = 0, prev = 0, yIndex = 2;
	char *prevChar = " ";
	char *stick = "|";
	int *maxHeight;
	maxH = getmaxy(stdscr);
	maxHeight = &maxH;
	for (bIndex = 0; bIndex < width; bIndex++){
		curHeight = height - buildings[bIndex];
		if (curHeight < *maxHeight)
			*maxHeight = curHeight;
		if (curHeight > height - 2){
			//prevChar = " ";
			//prev = height;
			mvprintw(height - 2, xIndex, "_");
                        prev = height - 2;
                        prevChar = "_";

		}
		else if (curHeight == height - 2){
			mvprintw(height - 2, xIndex, "_");
			prev = height - 2;
			prevChar = "_";
		}
		else if (prev == curHeight){
			 if ((height - buildings[bIndex+1]) > curHeight && bIndex != 0){
                                yIndex = height - 2;
                                while (yIndex > curHeight){
                                        mvprintw(yIndex, xIndex, "|");
                                        yIndex--;
                                }
                                prev = curHeight;
                                prevChar = "|";
                        }
                        else{
				mvprintw(curHeight, xIndex, "_");
				prev = curHeight;
				prevChar = "_";
			}
		}
		else if (prev < curHeight && prevChar == stick){
			mvprintw(curHeight, xIndex, "_");
                        prev = curHeight;
                        prevChar = "_";
		}
		else if (prev > curHeight && prevChar == stick){
			yIndex = height - 2;
                        while (yIndex > prev){
                                mvprintw(yIndex, xIndex, "|");
                                yIndex--;
                        }
                        prev = curHeight;
                        prevChar = "|";
		}
		else{
			yIndex = height - 2;
			if ( prev < curHeight){
				while (yIndex > prev){
					mvprintw(yIndex, xIndex, "|");
					yIndex--;
				}
			}
			else{
				while (yIndex > curHeight){
					mvprintw(yIndex, xIndex, "|");
					yIndex--;
				}
			}
			prev = curHeight;
			prevChar = "|";
		}
		refresh();
		xIndex++;
	}
	char *topmsg = "Press 'q' to exit, or press ctrl+c.";
	mvprintw(0, (width/2)- 20, topmsg);
	refresh();
	pthread_t threads[missiles + 1];
	pthread_create(&threads[0], NULL, runPlayer, defender);
	int mIndex = 1;
	int numMiss = 1;
	if (missiles == 0){ 
		while (1){
			if (running != 1)
				break;
			usleep(1000000);
			pthread_create(&threads[mIndex], NULL, runMissile, attacker);
			numMiss++;
		}
	}
	while (mIndex <= missiles){
		if (running != 1)
			break;
		usleep(1000000);
		pthread_create(&threads[mIndex], NULL, runMissile, attacker);
		mIndex++;
		numMiss++;
	}
	if (mIndex == missiles + 1){
			mvprintw(5, (getmaxx(stdscr)/2)-20, "The %s", attacker);
			mvprintw(5, (width/2)-20+(4+strlen(attacker)), "attack has ended!");
			refresh();
	}
	for (int check = 0; check < numMiss; check++)
		pthread_join(threads[check], NULL);
	mvprintw(6, (getmaxx(stdscr)/2)-20, "Press any key to exit.");
	while(1){
		if (getch() != 0){
			endwin();
			break;
		}
	}
	endwin();
}
threads.c
Displaying threads.c.

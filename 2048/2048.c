#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ncurses.h>

// COPYRIGHT WARNING. THIS FILE AND ALL IT'S CONTENTS BELONG TO RAPHAEL GONTIJO LOPES.
// IF YOU TAKE CREDIT FOR IT IN ANY WAY, WITHOUT GIVING CREDIT TO ME, YOU'RE DEAD TO ME. :)

// CURRENT VERSION: 1.7
// CHANGE LOG: now using scanw (instead of getch) when asking the user for his name (for the leaderboard). This enables deleting a character and not making a mess

// FUTURE UPDATES: cryptograph save file so user can't easily alter it. fix clock() precision while in debian... (see note 1), see if I can implement endless play (with numbers WAY bigger than 2048), windows port, absolute location for files (e.g.: var/games/2048/save.db)

// NOTE 1: Um problema surgiu: no Debian, clock() tem uma precisao muito pequena. Isso provoca problemas com o rand() (o 2 sempre aparece em uma mesma casa). Isso nao acontece com o Ubuntu... O que sera que esta acontecendo?

void startGame(int load, int settings);
void printBoard(int *v, int *score, int diditsave);
void askForMove(char *move, int settings);
int changeBoard(int *v, char *move, int *score);
void assignRandom(int *v);
int isWinner(int *v);
int checkPossibleMoves(int *v);
void displayLeaderboard(void);

int main(void) {
	int i=0;
	char start = ' ';
	int selected = 0; //0 for start, 1 for quit
	int noload = 0, nosave = 0;
	int settings = 0; //0 for wasd and 1 for hjkl
	FILE *ifp;
	FILE *ofp;
	int selectedset = 0;
	char startset = ' ';
	
	initscr();
	refresh();
	curs_set(0);
	noecho();
	cbreak();
	
	fflush(stdin);
	
	//MENU SCREEN LOOP
	while(true){
		if(!(ifp = fopen("save.db", "r"))){
			system("print \"SAVE FILE ERROR. CREATING NEW SAVE FILE.\"");
			//return 0;
			if(!(ifp = fopen("save.db", "w"))) {
				endwin();
				printf("FATAL FILE ERROR. COULD NOT CREATE SAVE FILE\nERROR 5");	//ERROR 5
				exit(-1);
			}
			
			fprintf(ifp, "0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;");
			fclose(ifp);
			ifp = fopen("save.db", "r");
		}
		
		if(start == '\n'){
			if(selected == 0){
				startGame(0, settings);
			}else if(selected == 1){
				fscanf(ifp, "%d;", &noload);
				fclose(ifp);
				
				if(noload == 1){
					noload = 0;
					startGame(1, settings);
				}else {
					noload = 1;
				}
			}else if(selected == 2){
				displayLeaderboard();
			}else if(selected == 3){
				//display settings until player goes back
				selectedset = settings;
				while(true){
					clear();
					printw("\n\twelcome to...\n\n");
	
					printw("\t  _|_|      _|    _|  _|      _|_|   \n");
					printw("\t_|    _|  _|  _|  _|  _|    _|    _| \n");
					printw("\t    _|    _|  _|  _|_|_|_|    _|_|   \n");
					printw("\t  _|      _|  _|      _|    _|    _| \n");
					printw("\t_|_|_|_|    _|        _|      _|_|   \n");
		
					printw("\n\tSETTINGS\n");
					
					if(startset == '\n'){
						if(selectedset == 0){
							startset = ' ';
							settings = 0;
							break;
						}else if(selectedset == 1){
							startset = ' ';
							settings = 1;
							break;
						}else if(selectedset == 2){
							startset = ' ';
							break;
						}
					}else if((startset == 'w' || startset == 'k') && (selectedset != 0)){
						selectedset--;
					}else if((startset == 's' || startset == 'j') && (selectedset != 2)){
						selectedset++;
					}
					
					if(selectedset == 0){
						printw("\n\t> [w/a/s/d]");
						printw("\n\t  [h/j/k/l]");
						printw("\n\n\t  BACK");
					}else if(selectedset == 1){
						printw("\n\t  [w/a/s/d]");
						printw("\n\t> [h/j/k/l]");
						printw("\n\n\t  BACK");
					}else if(selectedset == 2){
						printw("\n\t  [w/a/s/d]");
						printw("\n\t  [h/j/k/l]");
						printw("\n\n\t> BACK");
					}
					
					refresh();
					
					startset = getch();
					
				}
			}else if(selected == 4){
				fscanf(ifp, "%d;", &nosave);
				fclose(ifp);
				if(nosave == 1){
					if(!(ofp = fopen("save.db", "w"))){
						endwin();
						system("print \"FILE ERROR\"");
						exit(-1);
					}
				
					fprintf(ofp, "0;");
					for(i = 0; i < 16; i++){
						fprintf(ofp, "0;");
					}
					
					fflush(ofp);
					
					fclose(ofp);
					nosave = 2; //2 means we will show an succesfully deleted message
				}else if(nosave == 0){
					nosave = 1; //1 means there are no files to delete
				}
			}else if(selected == 5){
				clear();
				endwin();
		
				printf("\n\t2048: see you later then!\n\n");
				break;
			}
		}else if(start == 'w'){
			if(selected != 0){
				selected--;
			}
		}else if(start == 's'){
			if(selected != 5){
				selected++;
			}
		}
		
		
		//printing the main menu
		clear();
		
		printw("\n\twelcome to...\n\n");
	
		printw("\t  _|_|      _|    _|  _|      _|_|   \n");
		printw("\t_|    _|  _|  _|  _|  _|    _|    _| \n");
		printw("\t    _|    _|  _|  _|_|_|_|    _|_|   \n");
		printw("\t  _|      _|  _|      _|    _|    _| \n");
		printw("\t_|_|_|_|    _|        _|      _|_|   \n");
	
		printw("\n\twould you like to start?\n");
		
		if(selected == 0) {
			printw("\n\t> START");
			printw("\n\t  LOAD");
			printw("\n\t  LEADERBOARD");
			printw("\n\t  SETTINGS");
			printw("\n\t  DEL SAVE");
			printw("\n\t  QUIT");
		}else if(selected == 1){
			printw("\n\t  START");
			printw("\n\t> LOAD");
			printw("\n\t  LEADERBOARD");
			printw("\n\t  SETTINGS");
			printw("\n\t  DEL SAVE");
			printw("\n\t  QUIT");
		}else if(selected == 2){
			printw("\n\t  START");
			printw("\n\t  LOAD");
			printw("\n\t> LEADERBOARD");
			printw("\n\t  SETTINGS");
			printw("\n\t  DEL SAVE");
			printw("\n\t  QUIT");
		}else if(selected == 3){
			printw("\n\t  START");
			printw("\n\t  LOAD");
			printw("\n\t  LEADERBOARD");
			printw("\n\t> SETTINGS");
			printw("\n\t  DEL SAVE");
			printw("\n\t  QUIT");
		}else if(selected == 4){
			printw("\n\t  START");
			printw("\n\t  LOAD");
			printw("\n\t  LEADERBOARD");
			printw("\n\t  SETTINGS");
			printw("\n\t> DEL SAVE");
			printw("\n\t  QUIT");
		}else if(selected == 5){
			printw("\n\t  START");
			printw("\n\t  LOAD");
			printw("\n\t  LEADERBOARD");
			printw("\n\t  SETTINGS");
			printw("\n\t  DEL SAVE");
			printw("\n\t> QUIT");
		}
		
		if(noload){
			printw("\n\n\tno saved game");
			noload = 0;
		}
		
		if(nosave == 2){
			printw("\n\n\tsave game deleted");
			nosave = 0;
		}else if(nosave == 1){
			printw("\n\n\tno saved game to delete");
			nosave = 0;
		}
		
		refresh();
		
		start = getch();		
	}
	
	clear();
	refresh();
	endwin();
	return 0;
}

void startGame(int load, int settings) {
	int v[16];	//holds the value of each piece in the board
	int i, j, m;
	char move = 'l';
	int score = 0;
	char p[64];
	int diditsave = 0;
	int haswonbefore = 0;
	int isleader = 0;
	char newleader[8];
	int nosave = 0;
	
	FILE *ifp;
	FILE *ofp;
	
	FILE *ifplead;
	FILE *ofplead;
	
	struct leader {
		char name[8];
		int score;	
	};
	struct leader pessoas[5];
	
	if(!(ifp = fopen("save.db", "r"))){
		//endwin();
		system("print \"SAVE FILE ERROR. CREATING NEW SAVE FILE.\"");
		if(!(ifp = fopen("save.db", "w"))) {
			endwin();
			printf("FATAL FILE ERROR. COULD NOT CREATE SAVE FILE\nERROR 4");	//ERROR 4
			exit(-1);		
		}
		fprintf(ifp, "0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;");
		fclose(ifp);
		ifp = fopen("save.db", "r");
		//return;
	}
	
	//sets the value of all pieces to 0
	for(i = 0; i < 16; i++){
		v[i] = 0;
	}
	
	if(load){
		fscanf(ifp, "%d;", &v[0]);
		if(v[0] == 1) {
			for(i = 0; i < 16; i++){
				fscanf(ifp, "%d;", &v[i]);
			}
			fscanf(ifp, "%d;", &score);
			fclose(ifp);
		}else {
			fclose(ifp);
			endwin();
			printf("FATAL FILE ERROR. TRIED TO LOAD FROM NON-EXISTENT SAVE FILE\nERROR 1");	//ERROR 1
			exit(-1);
		}
	}

	if(!load){
		assignRandom(v);
	}
	
	while(!(isWinner(v))){
		printBoard(v, &score, diditsave);
		
		askForMove(&move, settings);
		diditsave = 0;	
		
		if(move == 'e'){
			if(!(ofp = fopen("save.db", "w"))){
				endwin();
				printf("FATAL FILE ERROR. COULD NOT LOAD OR CREATE SAVE FILE\nERROR 2");	//ERROR 2
				exit(-1);
			}
			
			fprintf(ofp, "1;");
			for(i = 0; i < 16; i++){
				fprintf(ofp, "%d;", v[i]);
			}
			fprintf(ofp, "%d;", score);
			fflush(ofp);
			
			fclose(ofp);
			
			diditsave = 1;
		}
		
		if(move == 'q'){
			if(!(ifplead = fopen("leaderboard.db", "r"))){
				/*endwin();
				printf("FILE ERROR");
				return;*/
				//assuming no leaderboard file existed. creating one
				system("print \"LEADER FILE ERROR. CREATING LEADER FILE.\"");
				if(!(ifplead = fopen("leaderboard.db", "w"))) {
					endwin();
					printf("FATAL FILE ERROR. COULD NOT CREATE LEADER FILE\nERROR 9");	//ERROR 9
					exit(-1);
				}
				
				fprintf(ifplead, "raphael;22048;player1;2000;player2;1500;player3;1000;player4;500;");
				fclose(ifplead);
				ifplead = fopen("leaderboard.db", "r");
			}
			
			for(i = 0; i < 5; i++){
				for(j = 0; j < 8; j++){
					fscanf(ifplead, "%c", &pessoas[i].name[j]);
					if(pessoas[i].name[j] == ';'){
						pessoas[i].name[j] = '\0';
						break;
					}
					
				}
				
				pessoas[i].name[7] = '\0';
				
				fscanf(ifplead, "%d;", &pessoas[i].score);
			}
			
			fclose(ifplead);
			
			if(score >= pessoas[4].score){
				isleader = 1;
			}
		
			clear();
			printw("\n\t2048: final score: %d\n", score);
			
			//print if its leaderboard winner or not.
			if(isleader){
				printw("\n\tYou got in the leaderboard!\n");
			}
			
			printw("\n\tPress any key to continue...");
			refresh();
			//wait for the user to press a key, then go back to the menu or to leaderboard name giving
			while(!getch()){
			}
			
			if(isleader){
				clear();
				printw("\n\tPlease enter your name (max 7 chars)\n\n\t> ");
				echo();
				curs_set(1);
				refresh();
				
				scanw("%6s", newleader);
				/*
				for(i = 0; i < 7; i++){
					newleader[i] = getch();
					if(newleader[i] == '\n'){
						newleader[i] = '\0';
						break;
					}
				}
				newleader[7] = '\0';
				*/
				//check the new leader's position in the leaderboard.db
				for(i = 4; i >= 0; i--){
					if(score < pessoas[i].score){
						break;
					}
				}
				//pessoas[i+1] eh nosso novo leader
				//updating the pessoas variable to exclude pessoa[4] and include the new leader
				for(j = 4; j >= (i+2); j--){
					for(m = 0; m < 8; m++){
						pessoas[j].name[m] = pessoas[j-1].name[m];
					}
					pessoas[j].score = pessoas[j-1].score;
				}
				for(j = 0; j < 8; j++){
					pessoas[i+1].name[j] = newleader[j];
				}
				pessoas[i+1].score = score;
					
					
				//inserting the new pessoas into the database
				if(!(ofplead = fopen("leaderboard.db", "w"))){
					endwin();
					printf("FATAL FILE ERROR. COULD NOT CREATE LEAD FILE\nERROR 6");	//ERROR 6
					exit(-1);
				}
			
				for(i = 0; i < 5; i++){
					j = 0;
					
					while(pessoas[i].name[j]){
						fprintf(ofplead, "%c", pessoas[i].name[j]);
						j++;
					}
					
					fprintf(ofplead, ";");
					fprintf(ofplead, "%d;", pessoas[i].score);
				}
				fflush(ofplead);
				
				fclose(ofplead);
				noecho();
				curs_set(0);
				isleader = 0;
			}
			
			return;
		}
		
		if(changeBoard(v, &move, &score)){
			assignRandom(v);
		}
		
		if(!checkPossibleMoves(v)) {
			printBoard(v, &score, diditsave);
			break;
		}
	}
	
	if(isWinner(v)){
		//CHECKING FOR LEADERBOARD
		if(!(ifplead = fopen("leaderboard.db", "r"))){
			//endwin();
			//printf("FILE ERROR");
			//return;
			system("print \"LEADER FILE ERROR. CREATING LEADER FILE.\"");
			if(!(ifplead = fopen("leaderboard.db", "w"))) {
				endwin();
				printf("FATAL FILE ERROR. COULD NOT CREATE LEADER FILE\nERROR 3");	//ERROR 3
				exit(-1);
			}
				
			fprintf(ifplead, "raphael;22048;player1;2000;player2;1500;player3;1000;player4;500;");
			fclose(ifplead);
			ifplead = fopen("leaderboard.db", "r");
		}
		
		for(i = 0; i < 5; i++){
			for(j = 0; j < 8; j++){
				fscanf(ifplead, "%c", &pessoas[i].name[j]);
				if(pessoas[i].name[j] == ';'){
					pessoas[i].name[j] = '\0';
					break;
				}
				
			}
			
			pessoas[i].name[7] = '\0';
			
			fscanf(ifplead, "%d;", &pessoas[i].score);
		}
		
		fclose(ifplead);
		
		if(score >= pessoas[4].score){
			isleader = 1;
		}
		//CHECKING FOR LEADERBOARD END
		
		clear();
		printw("\n\t2048: Congratulations! You Win!\n\tfinal score: %d", score);
		
		//print if its leaderboard winner or not.
		if(isleader){
			printw("\n\n\tYou got in the leaderboard!\n");
		}
		
		printw("\n\tPress any key to continue...");
		refresh();
		//wait for the user to press a key, then go back to the menu or to leaderboard name giving
		while(!getch()){
		}
		
		//INSERT LEADER
		if(isleader){
			clear();
			printw("\n\tPlease enter your name (max 7 chars)\n\n\t> ");
			echo();
			curs_set(1);
			refresh();
			
			scanw("%6s", newleader);
			/*
			for(i = 0; i < 7; i++){
				newleader[i] = getch();
				if(newleader[i] == '\n'){
					newleader[i] = '\0';
					break;
				}
			}
			newleader[7] = '\0';
			*/
			
			//check the new leader's position in the leaderboard.db
			for(i = 4; i >= 0; i--){
				if(score < pessoas[i].score){
					break;
				}
			}
			//pessoas[i+1] eh nosso novo leader
			//updating the pessoas variable to exclude pessoa[4] and include the new leader
			for(j = 4; j >= (i+2); j--){
				for(m = 0; m < 8; m++){
					pessoas[j].name[m] = pessoas[j-1].name[m];
				}
				pessoas[j].score = pessoas[j-1].score;
			}
			for(j = 0; j < 8; j++){
				pessoas[i+1].name[j] = newleader[j];
			}
			pessoas[i+1].score = score;
				
				
			//inserting the new pessoas into the database
			if(!(ofplead = fopen("leaderboard.db", "w"))){
				endwin();
				printf("FATAL FILE ERROR. COULD NOT LOAD OR CREATE LEADER FILE.\n ERROR 7");	//ERROR 7
				exit(-1);
			}
		
			for(i = 0; i < 5; i++){
				j = 0;
				
				while(pessoas[i].name[j]){
					fprintf(ofplead, "%c", pessoas[i].name[j]);
					j++;
				}
				
				fprintf(ofplead, ";");
				fprintf(ofplead, "%d;", pessoas[i].score);
			}
			fflush(ofplead);
			
			fclose(ofplead);
			//DELETING SAVE FILE
			if(!(ifp = fopen("save.db", "r"))){
				system("print \"SAVE FILE ERROR. CREATING NEW SAVE FILE.\"");
				//return 0;
				if(!(ifp = fopen("save.db", "w"))) {
					endwin();
					printf("FATAL FILE ERROR. COULD NOT CREATE SAVE FILE\nERROR 11");	//ERROR 11
					exit(-1);
				}
			
				fprintf(ifp, "0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;");
				fclose(ifp);
				ifp = fopen("save.db", "r");
			}
				
			fscanf(ifp, "%d;", &nosave);
			fclose(ifp);
			if(nosave == 1){
				if(!(ofp = fopen("save.db", "w"))){
					endwin();
					system("print \"FILE ERROR\"");
					exit(-1);
				}
				
				fprintf(ofp, "0;");
				for(i = 0; i < 16; i++){
					fprintf(ofp, "0;");
				}
				
				fflush(ofp);
				
				fclose(ofp);
				nosave = 0;
			}
			//DELETING SAVE FILE END
			noecho();
			curs_set(0);
			isleader = 0;
		}
		//INSERT LEADER END
		
		return;
	}else {
		
		clear();
		//this only printfs the last tabuleiro so the player can see how there really are no more moves left.
		for(i = 0; i < 64; i++){
			p[i] = ' ';
		}
		
		for(i = 0; i < 16; i++) {		
			if(v[i] != 0){
				if((v[i] / 10) == 0){
					p[(i*4 + 2)] = 48 + v[i];
				}else if((v[i] / 100) == 0){
					p[(i*4 + 1)] = 48 + (v[i] / 10);
					p[(i*4 + 2)] = 48 + (v[i] % 10);
				}else if((v[i] / 1000) == 0){
					p[(i*4 + 1)] = 48 + (v[i] / 100);
					p[(i*4 + 2)] = 48 + ((v[i] / 10) % 10);
					p[(i*4 + 3)] = 48 + (v[i] % 10);
				}else {
					p[(i*4)] = 48 + (v[i] / 1000);
					p[(i*4 + 1)] = 48 + ((v[i] / 100) % 10);
					p[(i*4 + 2)] = 48 + ((v[i] / 10) % 10);
					p[(i*4 + 3)] = 48 + (v[i] % 10);
				}
			}
		}
		
		printw("\n");
		printw("\t  _|_|      _|    _|  _|      _|_|   \n");
		printw("\t_|    _|  _|  _|  _|  _|    _|    _| \n");
		printw("\t    _|    _|  _|  _|_|_|_|    _|_|   \n");
		printw("\t  _|      _|  _|      _|    _|    _| \n");
		printw("\t_|_|_|_|    _|        _|      _|_|   \n\n");
		
		printw("\tscore: %d\n\n", score);
		printw("\t        |        |        |        |\n");
		printw("\t  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |\n", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
		printw("\t _______| _______| _______| _______|\n\n");
		printw("\t        |        |        |        |\n");
		printw("\t  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |\n", p[16], p[17], p[18], p[19], p[20], p[21], p[22], p[23], p[24], p[25], p[26], p[27], p[28], p[29], p[30], p[31]);
		printw("\t _______| _______| _______| _______|\n\n");
		printw("\t        |        |        |        |\n");
		printw("\t  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |\n", p[32], p[33], p[34], p[35], p[36], p[37], p[38], p[39], p[40], p[41], p[42], p[43], p[44], p[45], p[46], p[47]);
		printw("\t _______| _______| _______| _______|\n\n");
		printw("\t        |        |        |        |\n");
		printw("\t  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |\n", p[48], p[49], p[50], p[51], p[52], p[53], p[54], p[55], p[56], p[57], p[58], p[59], p[60], p[61], p[62], p[63]);
		printw("\t _______| _______| _______| _______|\n\n");
		//ended last printfing of the last board.
			
		printw("\n\t2048: No more moves. You Lose!\n\tfinal score: %d\n", score);
		
		//CHECKING FOR LEADERBOARD
		if(!(ifplead = fopen("leaderboard.db", "r"))){
			/*endwin();
			printf("FILE ERROR");
			return;*/
			system("print \"LEADER FILE ERROR. CREATING LEADER FILE.\"");
			if(!(ifplead = fopen("leaderboard.db", "w"))) {
				endwin();
				printf("FATAL FILE ERROR. COULD NOT CREATE LEADER FILE\nERROR 8");	//ERROR 8
				exit(-1);
			}
				
			fprintf(ifplead, "raphael;22048;player1;2000;player2;1500;player3;1000;player4;500;");
			fclose(ifplead);
			ifplead = fopen("leaderboard.db", "r");
		}
		
		for(i = 0; i < 5; i++){
			for(j = 0; j < 8; j++){
				fscanf(ifplead, "%c", &pessoas[i].name[j]);
				if(pessoas[i].name[j] == ';'){
					pessoas[i].name[j] = '\0';
					break;
				}
				
			}
			
			pessoas[i].name[7] = '\0';
			
			fscanf(ifplead, "%d;", &pessoas[i].score);
		}
		
		fclose(ifplead);
		
		if(score >= pessoas[4].score){
			isleader = 1;
		}
		//CHECKING FOR LEADERBOARD END
		
		//print if its leaderboard winner or not.
		if(isleader){
			printw("\n\n\tYou got in the leaderboard!\n");
		}
		
		printw("\n\tPress any key to continue...");		
		
		refresh();
		
		//wait for the user to press a key, then go back to the menu or to leaderboard insertion
		while(!getch()){
		}
		
		//INSERT LEADER
		if(isleader){
			clear();
			printw("\n\tPlease enter your name (max 7 chars)\n\n\t> ");
			echo();
			curs_set(1);
			refresh();
			
			scanw("%6s", newleader);
			/*
			for(i = 0; i < 7; i++){
				newleader[i] = getch();
				if(newleader[i] == '\n'){
					newleader[i] = '\0';
					break;
				}
			}
			newleader[7] = '\0';
			*/
			
			//check the new leader's position in the leaderboard.db
			for(i = 4; i >= 0; i--){
				if(score < pessoas[i].score){
					break;
				}
			}
			//pessoas[i+1] eh nosso novo leader
			//updating the pessoas variable to exclude pessoa[4] and include the new leader
			for(j = 4; j >= (i+2); j--){
				for(m = 0; m < 8; m++){
					pessoas[j].name[m] = pessoas[j-1].name[m];
				}
				pessoas[j].score = pessoas[j-1].score;
			}
			for(j = 0; j < 8; j++){
				pessoas[i+1].name[j] = newleader[j];
			}
			pessoas[i+1].score = score;
				
				
			//inserting the new pessoas into the database
			if(!(ofplead = fopen("leaderboard.db", "w"))){
				endwin();
				printf("FATAL FILE ERROR. COULD NOT LOAD OR CREATE LEADER FILE\nERROR 9");	//ERROR 9
				exit(-1);
			}
		
			for(i = 0; i < 5; i++){
				j = 0;
				
				while(pessoas[i].name[j]){
					fprintf(ofplead, "%c", pessoas[i].name[j]);
					j++;
				}
				
				fprintf(ofplead, ";");
				fprintf(ofplead, "%d;", pessoas[i].score);
			}
			fflush(ofplead);
			
			fclose(ofplead);
			//DELETING SAVE FILE
			if(!(ifp = fopen("save.db", "r"))){
				system("print \"SAVE FILE ERROR. CREATING NEW SAVE FILE.\"");
				//return 0;
				if(!(ifp = fopen("save.db", "w"))) {
					endwin();
					printf("FATAL FILE ERROR. COULD NOT CREATE SAVE FILE\nERROR 11");	//ERROR 11
					exit(-1);
				}
			
				fprintf(ifp, "0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;");
				fclose(ifp);
				ifp = fopen("save.db", "r");
			}
				
			fscanf(ifp, "%d;", &nosave);
			fclose(ifp);
			if(nosave == 1){
				if(!(ofp = fopen("save.db", "w"))){
					endwin();
					system("print \"FILE ERROR\"");
					exit(-1);
				}
				
				fprintf(ofp, "0;");
				for(i = 0; i < 16; i++){
					fprintf(ofp, "0;");
				}
				
				fflush(ofp);
				
				fclose(ofp);
				nosave = 0;
			}
			//DELETING SAVE FILE END
			noecho();
			curs_set(0);
			isleader = 0;
		}
		//INSERT LEADER END
		
		return;
	}
}
	
void printBoard(int *v, int *score, int diditsave) {
	char p[64];
	int i;
	
	clear();
	
	for(i = 0; i < 64; i++){
		p[i] = ' ';
	}
	
	for(i = 0; i < 16; i++) {		
		if(v[i] != 0){
			if((v[i] / 10) == 0){
				p[(i*4 + 2)] = 48 + v[i];
			}else if((v[i] / 100) == 0){
				p[(i*4 + 1)] = 48 + (v[i] / 10);
				p[(i*4 + 2)] = 48 + (v[i] % 10);
			}else if((v[i] / 1000) == 0){
				p[(i*4 + 1)] = 48 + (v[i] / 100);
				p[(i*4 + 2)] = 48 + ((v[i] / 10) % 10);
				p[(i*4 + 3)] = 48 + (v[i] % 10);
			}else {
				p[(i*4)] = 48 + (v[i] / 1000);
				p[(i*4 + 1)] = 48 + ((v[i] / 100) % 10);
				p[(i*4 + 2)] = 48 + ((v[i] / 10) % 10);
				p[(i*4 + 3)] = 48 + (v[i] % 10);
			}
		}
	}
	
	printw("\n");
	printw("\t  _|_|      _|    _|  _|      _|_|   \n");
	printw("\t_|    _|  _|  _|  _|  _|    _|    _| \n");
	printw("\t    _|    _|  _|  _|_|_|_|    _|_|   \n");
	printw("\t  _|      _|  _|      _|    _|    _| \n");
	printw("\t_|_|_|_|    _|        _|      _|_|   \n\n");
	
	printw("\tscore: %d\n\n", *score);
	printw("\t        |        |        |        |\n");
	printw("\t  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |\n", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
	printw("\t _______| _______| _______| _______|\n\n");
	printw("\t        |        |        |        |\n");
	printw("\t  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |\n", p[16], p[17], p[18], p[19], p[20], p[21], p[22], p[23], p[24], p[25], p[26], p[27], p[28], p[29], p[30], p[31]);
	printw("\t _______| _______| _______| _______|\n\n");
	printw("\t        |        |        |        |\n");
	printw("\t  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |\n", p[32], p[33], p[34], p[35], p[36], p[37], p[38], p[39], p[40], p[41], p[42], p[43], p[44], p[45], p[46], p[47]);
	printw("\t _______| _______| _______| _______|\n\n");
	printw("\t        |        |        |        |\n");
	printw("\t  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |  %c%c%c%c  |\n", p[48], p[49], p[50], p[51], p[52], p[53], p[54], p[55], p[56], p[57], p[58], p[59], p[60], p[61], p[62], p[63]);
	printw("\t _______| _______| _______| _______|\n\n");
	
	if(diditsave){
		printw("\tgame saved!\n\n");
		diditsave = 0;
	}
	
	refresh();
}

void askForMove(char *move, int settings) {
	if(settings == 0){
		printw("\tplease move [w/a/s/d][q:quit/e:save]");
		refresh();
	
		*move = getch();
	
		while(!(*move == 'w' || *move == 'a' || *move == 's' || *move == 'd' || *move == 'q' || *move == 'e')) {
			*move = getch();
		}
	}else if(settings == 1){
		printw("\tplease move [h/j/k/l][q:quit/e:save]");
		refresh();
	
		*move = getch();
	
		while(!(*move == 'h' || *move == 'j' || *move == 'k' || *move == 'l' || *move == 'q' || *move == 'e')) {
			*move = getch();
		}
	}
}

int changeBoard(int *v, char *move, int *score) {
	//this is where the magic happens, we analise what pieces we need to change to the board, based on the move (up, down, left, right), and we change the board (moving pieces or adding them and placing the sum in the desired piece)
	int diditmove = 0;
	int i, j;
	
	if(*move == 'w' || *move == 'k') {
		for(i = 0; i < 12; i++) {
			if(v[i] == 0){
				//see if any of the below can be placed here
				if(v[i+4] != 0){
					v[i] = v[i+4];
					v[i+4] = 0;
					diditmove++;
				}else if(v[i+8] != 0 && i < 8){
					v[i] = v[i+8];
					v[i+8] = 0;
					diditmove++;
				}else if(v[i+12] != 0 && i < 4){
					v[i] = v[i+12];
					v[i+12] = 0;
					diditmove++;
				}
			}
			
			if(v[i] != 0){
				//see if the below can be added here
				//(but only if there are none between "here" and the one below)
				if(v[i+4] != 0){
					if(v[i] == v[i+4]){
						v[i] += v[i+4];
						v[i+4] = 0;
						diditmove++;
						*score += v[i];
					}
				}else if(v[i+8] != 0 && i < 8){
					if(v[i] == v[i+8]){
						v[i] += v[i+8];
						v[i+8] = 0;
						diditmove++;
						*score += v[i];
					}
				}else if(v[i+12] != 0 && i < 4){
					if(v[i] == v[i+12]){
						v[i] += v[i+12];
						v[i+12] = 0;
						diditmove++;
						*score += v[i];
					}
				}
			}
		}
	}
	
	if(*move == 's' || *move == 'j'){		
		for(i = 15; i > 3; i--) {
			if(v[i] == 0){
				//see if any of the below can be placed here
				if(v[i-4] != 0){
					v[i] = v[i-4];
					v[i-4] = 0;
					diditmove++;
				}else if(v[i-8] != 0 && i > 7){
					v[i] = v[i-8];
					v[i-8] = 0;
					diditmove++;
				}else if(v[i-12] != 0 && i > 11){
					v[i] = v[i-12];
					v[i-12] = 0;
					diditmove++;
				}
			}
			
			if(v[i] != 0){
				//see if the above can be added here
				//(but only if there are none between "here" and the one above)
				if(v[i-4] != 0){
					if(v[i] == v[i-4]){
						v[i] += v[i-4];
						v[i-4] = 0;
						diditmove++;
						*score += v[i];
					}
				}else if(v[i-8] != 0 && i > 7){
					if(v[i] == v[i-8]){
						v[i] += v[i-8];
						v[i-8] = 0;
						diditmove++;
						*score += v[i];
					}
				}else if(v[i-12] != 0 && i > 11){
					if(v[i] == v[i-12]){
						v[i] += v[i-12];
						v[i-12] = 0;
						diditmove++;
						*score += v[i];
					}
				}
			}
		}
	}

	if(*move == 'd' || *move == 'l') {
		for(j = 3; j < 16; j += 4){
			for(i = j; i > j - 3; i--) {
				if(v[i] == 0){
					//see if any of the right can be placed here
					if(v[i-1] != 0){
						v[i] = v[i-1];
						v[i-1] = 0;
						diditmove++;
					}else if(v[i-2] != 0 && i > (j-2)){
						v[i] = v[i-2];
						v[i-2] = 0;
						diditmove++;
					}else if(v[i-3] != 0 && i > (j-1)){
						v[i] = v[i-3];
						v[i-3] = 0;
						diditmove++;
					}
				}
				
				if(v[i] != 0){
					//see if the right can be added here
					//(but only if there are none between "here" and the one on right)
					if(v[i-1] != 0){
						if(v[i] == v[i-1]){
							v[i] += v[i-1];
							v[i-1] = 0;
							diditmove++;
							*score += v[i];
						}
					}else if(v[i-2] != 0 && i > (j-2)){
						if(v[i] == v[i-2]){
							v[i] += v[i-2];
							v[i-2] = 0;
							diditmove++;
							*score += v[i];
						}
					}else if(v[i-3] != 0 && i > (j-1)){
						if(v[i] == v[i-3]){
							v[i] += v[i-3];
							v[i-3] = 0;
							diditmove++;
							*score += v[i];
						}
					}
				}
			}
		}	
	}
	
	if(*move == 'a' || *move == 'h') {
		for(j = 0; j < 13; j += 4){
			for(i = j; i < j + 3; i++) {
				if(v[i] == 0){
					//see if any of the right can be placed here
					if(v[i+1] != 0){
						v[i] = v[i+1];
						v[i+1] = 0;
						diditmove++;
					}else if(v[i+2] != 0 && i < (j+2)){
						v[i] = v[i+2];
						v[i+2] = 0;
						diditmove++;
					}else if(v[i+3] != 0 && i < (j+1)){
						v[i] = v[i+3];
						v[i+3] = 0;
						diditmove++;
					}
				}
				
				if(v[i] != 0){
					//see if the right can be added here
					//(but only if there are none between "here" and the one on right)
					if(v[i+1] != 0){
						if(v[i] == v[i+1]){
							v[i] += v[i+1];
							v[i+1] = 0;
							diditmove++;
							*score += v[i];
						}
					}else if(v[i+2] != 0 && i < (j+2)){
						if(v[i] == v[i+2]){
							v[i] += v[i+2];
							v[i+2] = 0;
							diditmove++;
							*score += v[i];
						}
					}else if(v[i+3] != 0 && i < (j+1)){
						if(v[i] == v[i+3]){
							v[i] += v[i+3];
							v[i+3] = 0;
							diditmove++;
							*score += v[i];
						}
					}
				}
			}
		}	
	}
	
	return diditmove;
}

void assignRandom(int *v) {
	int i;
	int assign = 0;
	
	//gets a random piece to assign the first number 2, but only if there are free pieces
	
	for(i = 0; i < 16; i++) {
		if(v[i] == 0){
			assign = 1;
			break;
		}
	}
	
	if(assign){
		srand(clock());
		i = (rand() % 16);

		while(v[i] != 0){
			i = (rand() % 16);
		}
	
		v[i] = 2;
	}
}

int isWinner(int *v) {
	//1 for winner, 0 for not winner
	int i;
	
	for(i = 0; i < 16; i++){
		if(v[i] == 2048){
			return 1;
		}
	}
	
	return 0;
}

int checkPossibleMoves(int *v){
	//for possible moves, return 1, else 0
	int value = 0;
	int i, j;
	
	for(i = 0; i < 16; i++){
		if(v[i] == 0) {
			value = 1;
		}
	}
	
	//check if any adjacent pieces are equal. if so, value = 1 else value = 0
	for(j = 0; j < 3; j++) {
		for(i = j; i < (13 + j); i += 4) {
			if(v[i] == v[(i+1)]) {
				value = 1;
			}
		}
	}
		
	for(i = 0; i < 12; i++) {
		if(v[i] == v[(i+4)]) {
			value = 1;
		}
	}
	
	return value;
}

void displayLeaderboard(void){
	FILE *ifplead;
	int i, j;
	
	struct leader {
		char name[8];
		int score;	
	};
	
	struct leader pessoas[5];
	
	if(!(ifplead = fopen("leaderboard.db", "r"))){
		system("print \"LEADER FILE ERROR. CREATING LEADER FILE.\"");
		if(!(ifplead = fopen("leaderboard.db", "w"))) {
			endwin();
			printf("FATAL FILE ERROR. COULD NOT CREATE LEADER FILE\nERROR 3");	//ERROR 3
			exit(-1);
		}
				
		fprintf(ifplead, "raphael;22048;player1;2000;player2;1500;player3;1000;player4;500;");
		fclose(ifplead);
		ifplead = fopen("leaderboard.db", "r");
	}
	
	for(i = 0; i < 5; i++){
		for(j = 0; j < 8; j++){
			fscanf(ifplead, "%c", &pessoas[i].name[j]);
			if(pessoas[i].name[j] == ';'){
				pessoas[i].name[j] = '\0';
				break;
			}
			
		}
		
		pessoas[i].name[7] = '\0';
		
		fscanf(ifplead, "%d;", &pessoas[i].score);
	}
	
	/*for(i = 0; i < 5; i++){
		fscanf(ifplead, "%7s;%d;", &pessoas[i].name[0], &pessoas[i].score);
	}*/
	
	fclose(ifplead);
	
	clear();
	printw("\n\twelcome to...\n\n");
	
	printw("\t  _|_|      _|    _|  _|      _|_|   \n");
	printw("\t_|    _|  _|  _|  _|  _|    _|    _| \n");
	printw("\t    _|    _|  _|  _|_|_|_|    _|_|   \n");
	printw("\t  _|      _|  _|      _|    _|    _| \n");
	printw("\t_|_|_|_|    _|        _|      _|_|   \n");

	printw("\n\tLEADERBOARD\n");
	
	for(i = 0; i < 5; i++){
		printw("\n\t  %s\t\t%d", pessoas[i].name, pessoas[i].score);
	}
	
	printw("\n\n\tPress any key to continue...\n");
	
	refresh();
	
	while(!getch()){
		//waits for the user to press something before going back to the menu
	}
	
	return;
}

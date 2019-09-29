#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/config.h"
#include "../headers/definitions.h"
#include "../headers/globals.h"
#include "../headers/logic.h"
#include "../headers/screen.h"
#include "../headers/system_logic.h"

int advanceSection() {
	if(currentSectionNumber!=favoritesSectionNumber&&currentSectionNumber<favoritesSectionNumber-1) {
		currentSectionNumber++;
		return 1;
	} else if (currentSectionNumber!=favoritesSectionNumber) {
		currentSectionNumber=0;
		return 1;
	}
	return 0;
}

int rewindSection() {
	if(currentSectionNumber!=favoritesSectionNumber&&currentSectionNumber>0) {
		currentSectionNumber--;
		return 1;
	} else if (currentSectionNumber!=favoritesSectionNumber) {
		currentSectionNumber=menuSectionCounter-2;
		return 1;
	}
	return 0;
}

void showPicture() {
	displayGamePicture();
}

void launchGame() {
	char fileToBeExecutedwithFullPath[200];
	if (favoritesSectionSelected && favoritesSize > 0) {
		struct Favorite favorite = findFavorite(CURRENT_GAME_NAME);
		strcpy(fileToBeExecutedwithFullPath,favorite.filesDirectory);
		strcat(fileToBeExecutedwithFullPath,favorite.name);
		executeCommand(favorite.emulatorFolder,favorite.executable,fileToBeExecutedwithFullPath);
	} else if (CURRENT_GAME_NAME!=NULL) {
		strcpy(fileToBeExecutedwithFullPath,CURRENT_SECTION.filesDirectory);
		strcat(fileToBeExecutedwithFullPath,CURRENT_GAME_NAME);
		executeCommand(CURRENT_SECTION.emulatorFolder, CURRENT_SECTION.executable,fileToBeExecutedwithFullPath);
	}
}

void scrollUp() {
	if(CURRENT_SECTION.currentGame == 0) {
		if (CURRENT_SECTION.currentPage >0) {
			CURRENT_SECTION.currentPage--;
		} else {
			CURRENT_SECTION.currentPage=CURRENT_SECTION.totalPages;
		}
		gamesInPage=countGamesInPage();
		if (gamesInPage>0) {
			CURRENT_SECTION.currentGame=gamesInPage-1;
		}
		return;
	}
	if (CURRENT_SECTION.currentGame > 0) {
		CURRENT_SECTION.currentGame--;
	}
}

void scrollDown() {
	if (CURRENT_SECTION.currentGame < gamesInPage-1) {
		if (NEXT_GAME_NAME!=NULL) {
			CURRENT_SECTION.currentGame++;
		} else {
			CURRENT_SECTION.currentPage=0;
			CURRENT_SECTION.currentGame=0;
		}
	} else {
		if (CURRENT_SECTION.currentPage < CURRENT_SECTION.totalPages) {
			CURRENT_SECTION.currentPage++;
		} else {
			CURRENT_SECTION.currentPage=0;
		}
		CURRENT_SECTION.currentGame=0;
	}
}

void advancePage() {
	if(CURRENT_SECTION.currentPage<=CURRENT_SECTION.totalPages) {
		if (CURRENT_SECTION.alphabeticalPaging) {
			char currentLetter=tolower(CURRENT_GAME_NAME[0]);
			while((tolower(CURRENT_GAME_NAME[0])==currentLetter||isdigit(CURRENT_GAME_NAME[0]))) {
				if (CURRENT_SECTION.currentPage==CURRENT_SECTION.totalPages&&CURRENT_SECTION.currentGame==countGamesInPage()-1) {
					scrollDown();
					break;
				}
				scrollDown();
			}
		} else {
			if(CURRENT_SECTION.currentPage!=CURRENT_SECTION.totalPages) {
				CURRENT_SECTION.currentPage++;
			} else {
				CURRENT_SECTION.currentPage=0;
			}
			CURRENT_SECTION.currentGame=0;
		}
	}
	gamesInPage=countGamesInPage();
}

void rewindPage() {
	if (CURRENT_SECTION.alphabeticalPaging) {
		int hitStart = 0;
		while(!(CURRENT_SECTION.currentPage==0&&CURRENT_SECTION.currentGame==0)&&(tolower(CURRENT_GAME_NAME[0])==tolower(PREVIOUS_GAME_NAME[0]))) {
			if (CURRENT_SECTION.currentPage==0&&CURRENT_SECTION.currentGame==0) {
				hitStart = 1;
				break;
			} else {
				scrollUp();
			}
		}
		if (!hitStart) {
			scrollUp();
		}
		hitStart=0;
		while(!(CURRENT_SECTION.currentPage==0&&CURRENT_SECTION.currentGame==0)&&(tolower(CURRENT_GAME_NAME[0])==tolower(PREVIOUS_GAME_NAME[0])||
				(isdigit(CURRENT_GAME_NAME[0])&&isdigit(PREVIOUS_GAME_NAME[0]))
		)) {
			if (CURRENT_SECTION.currentPage==0&&CURRENT_SECTION.currentGame==0) {
				hitStart = 1;
				break;
			} else {
				scrollUp();
			}
		}
	} else if (CURRENT_SECTION.currentPage > 0) {
		CURRENT_SECTION.currentPage--;
		CURRENT_SECTION.currentGame=0;
	} else {
		CURRENT_SECTION.currentPage=CURRENT_SECTION.totalPages;
		CURRENT_SECTION.currentGame=0;

	}
	gamesInPage=countGamesInPage();
}

void showOrHideFavorites() {
	if (favoritesSectionSelected) {
		favoritesSectionSelected=0;
		currentSectionNumber=returnTo;
		determineStartingScreen(menuSectionCounter);
//		loadGameList();
		return;
	}
	favoritesSectionSelected=1;
	returnTo=currentSectionNumber;
	currentSectionNumber=favoritesSectionNumber;
	loadFavoritesSectionGameList();
}

void removeFavorite() {
	favoritesChanged=1;
	if (favoritesSize>0) {
		for (int i=CURRENT_GAME_NUMBER;i<favoritesSize;i++) {
			strcpy(favorites[i].emulatorFolder,favorites[i+1].emulatorFolder);
			strcpy(favorites[i].executable,favorites[i+1].executable);
			strcpy(favorites[i].filesDirectory,favorites[i+1].filesDirectory);
			strcpy(favorites[i].name,favorites[i+1].name);
		}
		strcpy(favorites[favoritesSize-1].emulatorFolder,"\0");
		strcpy(favorites[favoritesSize-1].executable,"\0");
		strcpy(favorites[favoritesSize-1].filesDirectory,"\0");
		strcpy(favorites[favoritesSize-1].name,"\0");
		favoritesSize--;
		loadFavoritesSectionGameList();
	}
	if(CURRENT_GAME_NAME==NULL) {
		scrollUp();
	}
}

void markAsFavorite() {
	favoritesChanged=1;
	if (favoritesSize<FAVORITES_SIZE) {
		if (!doesFavoriteExist(CURRENT_GAME_NAME)) {
			strcpy(favorites[favoritesSize].name, CURRENT_GAME_NAME);
			strcpy(favorites[favoritesSize].emulatorFolder,CURRENT_SECTION.emulatorFolder);
			strcpy(favorites[favoritesSize].executable,CURRENT_SECTION.executable);
			strcpy(favorites[favoritesSize].filesDirectory,CURRENT_SECTION.filesDirectory);
			favoritesSize++;
		}
	}
}

int isSelectPressed() {
	return keys[BTN_SELECT];
}

int performAction() {
	if(keys[BTN_A]) {
		if (keys[BTN_DOWN]) {
			hotKeyPressed=1;
			CURRENT_SECTION.alphabeticalPaging=1;
			advancePage();
			CURRENT_SECTION.alphabeticalPaging=0;
			return 0;
		}
		if (keys[BTN_UP]) {
			hotKeyPressed=1;
			CURRENT_SECTION.alphabeticalPaging=1;
			rewindPage();
			CURRENT_SECTION.alphabeticalPaging=0;
			return 0;
		}
		if(keys[BTN_RIGHT]) {
			hotKeyPressed=0;
			int advanced = advanceSection();
			if(advanced) {
				loadGameList();
				while(CURRENT_SECTION.hidden) {
					advanceSection();
					loadGameList();
				}
			}
			return 0;
		}
		if(keys[BTN_LEFT]) {
			hotKeyPressed=0;
			int rewinded = rewindSection();
			if(rewinded) {
				loadGameList();
				while(CURRENT_SECTION.hidden) {
					rewindSection();
					loadGameList();
				}
			}
			return 0;
		}
	}

	if(keys[BTN_LB]) {
		hotKeyPressed=0;
		int rewinded = rewindSection();
		if(rewinded) {
			loadGameList();
			while(CURRENT_SECTION.hidden) {
				rewindSection();
				loadGameList();
			}
		}
		return 0;
	}

	if(keys[BTN_RB]) {
		hotKeyPressed=0;
		int advanced = advanceSection();
		if(advanced) {
			loadGameList();
			while(CURRENT_SECTION.hidden) {
				advanceSection();
				loadGameList();
			}
		}
		return 0;
	}

	if (!hotKeyPressed) {
		if (keys[BTN_SELECT] && keys[BTN_START]) {
			running=0;
			return 0;
		}
		if (keys[BTN_R] && keys[BTN_START]) {
			freeResources();
			saveLastState();
			saveFavorites();
			exit(0);
		}
		if (keys[BTN_B]) {
			if (!favoritesSectionSelected) {
				markAsFavorite();
			} else {
				removeFavorite();
			}
			return 0;
		}
		if (keys[BTN_START]) {
			cycleFrequencies();
			drawHeader();
			return 0;
		}
		if (keys[BTN_R]) {
			showOrHideFavorites();
			return 0;
		}
		if (keys[BTN_TA]) {
			if (countGamesInPage()>0) {
				saveFavorites();
				freeResources();
				launchGame();
			}
			return 0;
		}
		if (keys[BTN_TB]) {
			if (pictureMode) {
				pictureMode=0;
			} else {
				pictureMode=1;
			}
		}
		if (keys[BTN_DOWN]) {
			scrollDown();
			return 1;
		}
		if(keys[BTN_UP]) {
			scrollUp();
			return 1;
		}
		if(keys[BTN_RIGHT]) {
			advancePage();
			return 1;
		}
		if(keys[BTN_LEFT]) {
			rewindPage();
			return 1;
		}
	}
	return 0;
}

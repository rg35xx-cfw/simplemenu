#include <stdlib.h>
#include "../headers/config.h"
#include "../headers/control.h"
#include "../headers/definitions.h"
#include "../headers/globals.h"
#include "../headers/graphics.h"
#include "../headers/logic.h"
#include "../headers/screen.h"
#include "../headers/system_logic.h"

int performAction(struct Rom *rom) {
	if (keys[BTN_SELECT] && keys[BTN_START]) {
		running=0;
		return 0;
	}
	if (CURRENT_SECTION.executables[1]!=NULL&&keys[BTN_START]&&!favoritesSectionSelected) {
		currentlyChoosingEmulator=1;
		return 0;
	}

	if (keys[BTN_START]&&isUSBMode) {
		hotKeyPressed=0;
		isUSBMode=0;
		int ret = system("./usb_mode_off.sh");
		if (ret==-1) {
			generateError("FATAL ERROR", 1);
		}
		return 0;
	}
	if(itsStoppedBecauseOfAnError&&!keys[BTN_A]) {
		return(0);
	}
	if(keys[BTN_B]) {
		if (keys[BTN_A]&&!currentlySectionSwitching) {
			launchEmulator();
		}
		if (keys[BTN_X]&&!currentlySectionSwitching) {
			callDeleteGame();
			return 1;
		}
		if (keys[BTN_START]&&!currentlySectionSwitching) {
			hotKeyPressed=0;
			int returnedValue = system("./usb_mode_on.sh");
			if (returnedValue==0) {
				isUSBMode = 1;
			} else {
				generateError("USB MODE  NOT AVAILABLE",0);
			}
		}
		if (keys[BTN_SELECT]&&!currentlySectionSwitching) {
			for(int i=0;i<100;i++) {
				selectRandomGame();
				updateScreen();
			}
			saveFavorites();
			launchGame();
		}
		if (keys[BTN_DOWN]&&!currentlySectionSwitching) {
			hotKeyPressed=1;
			CURRENT_SECTION.alphabeticalPaging=1;
			advancePage();
			CURRENT_SECTION.alphabeticalPaging=0;
			if(pictureMode) {
				resetPicModeHideMenuTimer();
			}
			return 0;
		}
		if (keys[BTN_UP]&&!currentlySectionSwitching) {
			hotKeyPressed=1;
			CURRENT_SECTION.alphabeticalPaging=1;
			rewindPage();
			CURRENT_SECTION.alphabeticalPaging=0;
			if(pictureMode) {
				resetPicModeHideMenuTimer();
			}
			return 0;
		}
		if(keys[BTN_RIGHT]) {
			hotKeyPressed=0;
			int advanced = advanceSection();
			if(advanced) {
				currentlySectionSwitching=1;
				if(theCurrentSectionHasGames()) {
					displayBackgroundPicture();
					showConsole();
					refreshScreen();
				}
				loadGameList(0);
				while(CURRENT_SECTION.hidden) {
					advanceSection();
					if(theCurrentSectionHasGames()) {
						displayBackgroundPicture();
						showConsole();
						refreshScreen();
					}
					loadGameList(0);
				}
			}
			return 0;
		}
		if(keys[BTN_LEFT]) {
			hotKeyPressed=0;
			int rewinded = rewindSection();
			if(rewinded) {
				currentlySectionSwitching=1;
				if(theCurrentSectionHasGames()) {
					displayBackgroundPicture();
					showConsole();
					refreshScreen();
				}
				loadGameList(0);
				while(CURRENT_SECTION.hidden) {
					rewindSection();
					if(theCurrentSectionHasGames()) {
						displayBackgroundPicture();
						showConsole();
						refreshScreen();
					}
					loadGameList(0);
				}
			}
			return 0;
		}
	}

	if(keys[BTN_L1]) {
		hidePicModeMenu();
		hotKeyPressed=0;
		if (pictureMode&&!favoritesSectionSelected) {
			resetPicModeHideLogoTimer();
			currentlySectionSwitching=1;
		}
		int rewinded = rewindSection();
		if(rewinded) {
			while(CURRENT_SECTION.hidden) {
				rewindSection();
			}
		}
		if (!pictureMode) {
			currentlySectionSwitching=0;
		}
		return 0;
	}
	if(keys[BTN_R1]) {
		hidePicModeMenu();
		hotKeyPressed=0;
		if (pictureMode&&!favoritesSectionSelected) {
			resetPicModeHideLogoTimer();
			currentlySectionSwitching=1;
		}
		int advanced = advanceSection();
		if(advanced) {
			while(CURRENT_SECTION.hidden) {
				advanceSection();
			}
		}
		if (!pictureMode) {
			currentlySectionSwitching=0;
		}
		return 0;
	}

	if (!hotKeyPressed&&!currentlySectionSwitching&&!isUSBMode) {
		if (keys[BTN_X]) {
			if(!isPicModeMenuHidden) {
				resetPicModeHideMenuTimer();
			}
			if (!favoritesSectionSelected) {
				markAsFavorite();
			} else {
				removeFavorite();
				if(favoritesSize==0) {
					showOrHideFavorites();
					hidePicModeMenu();
				}
			}
			return 0;
		}
		if (keys[BTN_START]) {
			cycleFrequencies();
			return 0;
		}
		if (keys[BTN_R]) {
			showOrHideFavorites();
			return 0;
		}
		if (keys[BTN_A]) {
			if(itsStoppedBecauseOfAnError) {
				if(thereIsACriticalError) {
					#ifndef TARGET_PC
					running=0;
					#else
					freeResources();
					saveLastState();
					saveFavorites();
					exit(0);
					#endif
				}
				itsStoppedBecauseOfAnError=0;
				return 0;
			}
			if (countGamesInPage()>0) {
				saveFavorites();
				launchGame();
			}
			return 0;
		}
		if (keys[BTN_Y]) {
			if (pictureMode) {
				pictureMode=0;
			} else {
				pictureMode=1;
			}
		}
		if (keys[BTN_DOWN]) {
			if(pictureMode) {
				resetPicModeHideMenuTimer();
			}
			scrollDown();
			return 1;
		}
		if(keys[BTN_UP]) {
			if(pictureMode) {
				resetPicModeHideMenuTimer();
			}
			scrollUp();
			return 1;
		}
		if(keys[BTN_RIGHT]) {
			if(pictureMode) {
				resetPicModeHideMenuTimer();
			}
			advancePage();
			return 1;
		}
		if(keys[BTN_LEFT]) {
			if(pictureMode) {
				resetPicModeHideMenuTimer();
			}
			rewindPage();
			return 1;
		}
	}
	return 0;
}

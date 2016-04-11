uint32_t ADC_Convert(uint32_t input){		//from ADC value to position in FP	
	uint32_t pos; 
	pos = (1638*input + 692224);		//pos = (num*input); (pixel location?) 
	pos = pos >> 16; 
	keep = 0;
	//PF3 = 0;         
 // end of Convert Profile	
  return pos;
}

//converts ASCII xlocation to pixel location
uint32_t Ascii_ConvertX(uint32_t location){
	location = (location*5)+3;
	//location += 3; //maybe? 
	return location; 
}

Notes clearNotes(Notes noteArray[]){
	int n = 0; 
	for(n = 0; n<80; n++){
		noteArray[n].status = New; 
	}
	return noteArray[80];
}


//changes score string
void changeScoreString(void){
	//DisableInterrupts(); 
	uint32_t div = score; 
	div = score/10000; 
	stringScore[0] = div+0x30; 
	div = score%10000; 
	div = div/1000; 
	stringScore[1] = div+0x30; 
	div = (score%10000)%1000; 
	div = div/100; 
	stringScore[2] = div+0x30; 
	div = ((score%10000)%1000)%100; 
	div = div/10;
	stringScore[3] = div+0x30; 
	div = (((score%10000)%1000)%100)%10;
	stringScore[4] = div+0x30; 
	//previous = 1; 
	//EnableInterrupts();
}

//prints current score
void printScore(void){
	DisableInterrupts(); 
		ST7735_SetCursor(15, 1);
		changeScoreString(); 
		//char num = toString(score); 
		ST7735_OutString(stringScore); 
	EnableInterrupts(); 
}

//prints number of lives 
void printLives(void){
	DisableInterrupts(); 
	if (lives == 0){
		gameScreen = GameOver; 
	}
	if(lives >= 0){
	ST7735_SetCursor(17, 2);
	if(lives != 3){
	ST7735_OutChar(0x00); 
	ST7735_OutChar(0x00);
	ST7735_OutChar(0x00);
	ST7735_SetCursor(17, 2);
	}
	int n = 0; 
	for(n = 0; n< lives; n++)
		ST7735_OutChar(0x03); 
}
	EnableInterrupts(); 
}


//plays the next note in the song
//goes to systick? 

void PlaySong(void){
	
	//last note of song won't play? 
	i = (i+1)%songLength;
	
	if(gameScreen == Game1){
		Sound_Play(HotBuns[i].period);	 	//change note	
	}	 	
	else if(gameScreen == Game2){
		Sound_Play(Twinkle[i].period);	 	//change note	
	}	 
	else if(gameScreen == Game3){
		Sound_Play(Hold[i].period);	 	//change note	
	}	 
	else if(gameScreen == Game4){
		Sound_Play(Wish[i].period);	 	//change note	
	}	
	else if(gameScreen == Game5){
		Sound_Play(Dreaming[i].period);	 	//change note	
	}	 	
	else if(gameScreen == Game6){
		Sound_Play(Wind[i].period);	 	//change note	
	}	
	else if(gameScreen == Game7){
		Sound_Play(World[i].period);	 	//change note	
	}		
	else if(gameScreen == Game8){
		Sound_Play(Hakuna[i].period);	 	//change note	
	}		
	else if(gameScreen == Game9){
		Sound_Play(Man[i].period);	 	//change note	
	}	
	else if(gameScreen == Game10){
		Sound_Play(Favorite[i].period);	 	//change note	
	}	
		if(i == (songLength-1))
	{
		//i = 29;  
		loopNumber++; 
		if(loopNumber <=3){
			tempo = (tempo-loopNumber*1000000)%30000000; 
			changeTempo(tempo);
		}
		else{
			tempo = (tempo-loopNumber*80000)%30000000;
			changeTempo(tempo);
		}
		
	}
	//i = (i+1)%songLength;

}

//checks to see if note is caught
//returns a one if true
int checkCatch(Notes note){
	int location = note.xLocation; 
	location = Ascii_ConvertX(location); 
	int charloc = player.xLocation; 
  if((location>=(charloc-16)) && (location<= (charloc+20))){
		return 1; 
	}
	else
		return 0; 	
		
	}

//moves a note down
Notes moveNote (Notes note){
	//Random_Init(NVIC_ST_CURENT_R*ADC_In()); 		
	if(note.yLocation == (14)){
		int catch = checkCatch(note);
		if(catch == 1){
			score+=15*(loopNumber+1); 
			PlaySong(); 
			printScore(); 
		}
		else{
			lives--; 
      printLives();
			//PlaySong();
			if(i == (songLength-1))
				{
					//i = 29;  
					loopNumber++; 
					tempo = (tempo-loopNumber*1000000)%30000000; 
					changeTempo(tempo);
				}
					i = (i+1)%songLength;
		}
		note.status = New; 		
	}			
	ST7735_SetCursor(note.xLocation, note.yLocation); 
	ST7735_OutChar(0x00);																			//erase location
	note.yLocation = (note.yLocation+1)%15; 
	if(note.yLocation == 0){
		note.yLocation = 3; 
	}
	if(note.status == Falling){
	ST7735_SetCursor(note.xLocation, note.yLocation); 
	ST7735_OutChar(0x0E);
	}																				//draw new note	
	return note; 
	
}

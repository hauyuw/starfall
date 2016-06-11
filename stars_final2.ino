//STARFALL
//By Hau-yu Wong
//Gadgets Spring 2016: Project 1
//Catch as many of the 50 falling stars generated as possible

// Pins
const int cols[8] = {2, 3, 4, 5, 6, 7, 8, 10}; //array of pins for the columns in the 8x8 LED matrix
const int rows[8] = {11, 12, 13, 14, 15, 16, 17, 18}; //array of pins for the rows in the 8x8 LED matrix
const int potPin = A5; //pin for potentiometer input

int xVal; //track position of the bowl element
int lastStar = 9; //column location of the last star generated, set to out of bounds initially so that the first star can generate anywhere
int totalStars = 50; //total number of stars to be randomly generated
int currentStars = 1; //tracks number of stars
int frameRateCounter = 0; //tracks number of time that the screen has been refreshed
int targetFrameRate = 6; //number of time screen is freshed before the star locations are updated/generated; increase to slow stars down, decrease to speed them up
int score = 0; //initalize score
boolean finished = false; //track game state; false = in play, true = complete

//play screen matrix
int screen[8][8] = { 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 2, 0, 0, 0, 2, 0, 0}, 
  {0, 0, 3, 3, 3, 0, 0, 0}  
};

//star screen matrix
int starDisplay[8][8] = {
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 1, 1, 1, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 1, 1, 1, 0, 0},
  {0, 0, 1, 1, 1, 0, 0, 0},
  {0, 1, 1, 0, 1, 1, 0, 0},
  {1, 0, 0, 0, 0, 0, 1, 0}
};

//score display matrix
int scoreDisplay[8][14] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

//turn all LEDs off
void clearColumns() {
  for (int i = 0; i < 8; i++){
    digitalWrite(cols[i], LOW);
  }
}

//randomly generates stars to insert into the game screen; prevent stars from being made in directly adjacent spaces; 30% chance of no new star being generated at a given refresh
void makeStars() {
  if (currentStars > totalStars) {
    return;
  }
  
  int newStar = random(0, 10);

  while (newStar == lastStar) {
    newStar = random(0, 10);
  }

  if (newStar > 6 || newStar == 0) {
    return;
  }

  //only make up to x number of stars specified in totalStars variable
  if (currentStars < totalStars) {
    screen[0][newStar] = 1;
    lastStar = newStar;
    currentStars++;
  } else if (currentStars == totalStars) {
    screen[0][newStar] = 8;
    currentStars++;
  }
}

//controls the screen by lighting the appropriate columns and rows for LED output during gameplay
void makeScreen (int arrayInput[8][8], int offset) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(rows[i], LOW);
    for (int j = offset; j < offset+8; j++) {
      if (arrayInput[i][j] != 0) {
        digitalWrite(cols[j], HIGH);
      }
    }
    delay(0);
    digitalWrite(rows[i], HIGH);
    clearColumns();
  }
}

//controls the screen by lighting the appropriate columns and rows for LED output for scrolling game title
void printTitle (int arrayInput[8][42], int offset) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(rows[i], LOW);
    for (int j = offset; j < offset+8; j++) {
      if (arrayInput[i][j] == 1) {
        digitalWrite(cols[j-offset], HIGH);
      }
    }
    delay(0);
    digitalWrite(rows[i], HIGH);
    clearColumns();
  }
}

//controls the screen by lighting the appropriate columns and rows for LED output for scrolling score report
void printScore (int arrayInput[8][14], int offset) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(rows[i], LOW);
    for (int j = offset; j < offset+8; j++) {
      if (arrayInput[i][j] == 1) {
        digitalWrite(cols[j-offset], HIGH);
      }
    }
    delay(0);
    digitalWrite(rows[i], HIGH);
    clearColumns();
  }
}

//setup to scroll game title
void scrollingText (int arrayInput[8][42], int arrayLength) {
  for (int offset = 0; offset < arrayLength-8; offset++) {
    unsigned long start = micros();
    while (micros() < start+200000) {
      printTitle(arrayInput, offset);
    }
  }
}

//copies a digit into score array for display
void copyArray (int inputArray[8][5], int offset) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 5; j++) {
      scoreDisplay[i][j+offset] = inputArray[i][j];
    }
  }
}

//fills out an array for the ten's digit and one's digit separately and copies them into the score display array
void scrollingScore (int arrayInput[8][14], int arrayLength) {
  //copies number displays into the score display array
  int digit1[8][5];
  int digit2[8][5];

  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 5; j++) {
      digit1[i][j] = 0;
      digit2[i][j] = 0;
    }
  }


  switch(score/10) {
    case 0: 
      digit1[1][2] = 1;
      digit1[1][3] = 1;
      digit1[2][1] = 1;
      digit1[2][4] = 1;
      digit1[3][1] = 1;
      digit1[3][4] = 1;
      digit1[4][1] = 1;
      digit1[4][4] = 1;
      digit1[5][1] = 1;
      digit1[5][4] = 1;
      digit1[6][2] = 1;
      digit1[6][3] = 1;
      copyArray(digit1, 0);
      break;

    case 1: 
      digit1[1][2] = 1;
      digit1[1][3] = 1;
      digit1[2][1] = 1;
      digit1[2][3] = 1;
      digit1[3][3] = 1;
      digit1[4][3] = 1;
      digit1[5][3] = 1;
      digit1[6][1] = 1;
      digit1[6][2] = 1;
      digit1[6][3] = 1;
      digit1[6][4] = 1;
      copyArray(digit1, 0);
      break;

    case 2: 
      digit1[1][1] = 1;
      digit1[1][2] = 1;
      digit1[1][3] = 1;
      digit1[2][1] = 1;
      digit1[2][4] = 1;
      digit1[3][4] = 1;
      digit1[4][3] = 1;
      digit1[5][2] = 1;
      digit1[6][1] = 1;
      digit1[6][2] = 1;
      digit1[6][3] = 1;
      digit1[6][4] = 1;
      copyArray(digit1, 0);
      break;

    case 3: 
      digit1[1][1] = 1;
      digit1[1][2] = 1;
      digit1[1][3] = 1;
      digit1[2][4] = 1;
      digit1[3][2] = 1;
      digit1[3][3] = 1;
      digit1[4][4] = 1;
      digit1[5][4] = 1;
      digit1[6][1] = 1;
      digit1[6][2] = 1;
      digit1[6][3] = 1;
      copyArray(digit1, 0);
      break;

    case 4: 
      digit1[1][1] = 1;
      digit1[1][4] = 1;
      digit1[2][1] = 1;
      digit1[2][4] = 1;
      digit1[3][1] = 1;
      digit1[3][2] = 1;
      digit1[3][3] = 1;
      digit1[3][4] = 1;
      digit1[4][4] = 1;
      digit1[5][4] = 1;
      digit1[6][4] = 1;      
      copyArray(digit1, 0);
      break;

    case 5: 
      digit1[1][1] = 1;
      digit1[1][2] = 1;
      digit1[1][3] = 1;
      digit1[1][4] = 1;
      digit1[2][1] = 1;
      digit1[3][1] = 1;
      digit1[3][2] = 1;
      digit1[3][3] = 1;
      digit1[4][4] = 1;
      digit1[5][4] = 1;
      digit1[6][1] = 1;
      digit1[6][2] = 1;
      digit1[6][3] = 1;
      copyArray(digit1, 0);
      break;

    case 6: 
      digit1[1][2] = 1;
      digit1[1][3] = 1;
      digit1[1][4] = 1;
      digit1[2][1] = 1;
      digit1[3][1] = 1;
      digit1[4][1] = 1;
      digit1[4][2] = 1;
      digit1[4][3] = 1;
      digit1[4][4] = 1;
      digit1[5][1] = 1;
      digit1[5][4] = 1;
      digit1[6][2] = 1;
      digit1[6][3] = 1;
      copyArray(digit1, 0);
      break;

    case 7: 
      digit1[1][1] = 1;
      digit1[1][2] = 1;
      digit1[1][3] = 1;
      digit1[1][4] = 1;
      digit1[2][4] = 1;
      digit1[3][3] = 1;
      digit1[4][2] = 1;
      digit1[5][1] = 1;
      digit1[6][1] = 1;
      copyArray(digit1, 0);
      break;

    case 8: 
      digit1[1][2] = 1;
      digit1[1][3] = 1;
      digit1[2][1] = 1;
      digit1[2][4] = 1;
      digit1[3][2] = 1;
      digit1[3][3] = 1;
      digit1[4][1] = 1;
      digit1[4][4] = 1;
      digit1[5][1] = 1;
      digit1[5][4] = 1;
      digit1[6][2] = 1;
      digit1[6][3] = 1;
      copyArray(digit1, 0);
      break;

    case 9:
      digit1[1][2] = 1;
      digit1[1][3] = 1;
      digit1[1][4] = 1;
      digit1[2][1] = 1;
      digit1[2][4] = 1;
      digit1[3][1] = 1;
      digit1[3][4] = 1;
      digit1[4][2] = 1;
      digit1[4][3] = 1;
      digit1[4][4] = 1;
      digit1[5][4] = 1;
      digit1[6][4] = 1;
      copyArray(digit1, 0);
      break;
  }

  switch(score%10) {
    case 0: 
      digit2[1][2] = 1;
      digit2[1][3] = 1;
      digit2[2][1] = 1;
      digit2[2][4] = 1;
      digit2[3][1] = 1;
      digit2[3][4] = 1;
      digit2[4][1] = 1;
      digit2[4][4] = 1;
      digit2[5][1] = 1;
      digit2[5][4] = 1;
      digit2[6][2] = 1;
      digit2[6][3] = 1;
      copyArray(digit2, 5);
      break;

    case 1: 
      digit2[1][2] = 1;
      digit2[1][3] = 1;
      digit2[2][1] = 1;
      digit2[2][3] = 1;
      digit2[3][3] = 1;
      digit2[4][3] = 1;
      digit2[5][3] = 1;
      digit2[6][1] = 1;
      digit2[6][2] = 1;
      digit2[6][3] = 1;
      digit2[6][4] = 1;
      copyArray(digit2, 5);
      break;

    case 2: 
      digit2[1][1] = 1;
      digit2[1][2] = 1;
      digit2[1][3] = 1;
      digit2[2][1] = 1;
      digit2[2][4] = 1;
      digit2[3][4] = 1;
      digit2[4][3] = 1;
      digit2[5][2] = 1;
      digit2[6][1] = 1;
      digit2[6][2] = 1;
      digit2[6][3] = 1;
      digit2[6][4] = 1;
      copyArray(digit2, 5);
      break;

    case 3: 
      digit2[1][1] = 1;
      digit2[1][2] = 1;
      digit2[1][3] = 1;
      digit2[2][4] = 1;
      digit2[3][2] = 1;
      digit2[3][3] = 1;
      digit2[4][4] = 1;
      digit2[5][4] = 1;
      digit2[6][1] = 1;
      digit2[6][2] = 1;
      digit2[6][3] = 1;
      copyArray(digit2, 5);
      break;

    case 4: 
      digit2[1][1] = 1;
      digit2[1][4] = 1;
      digit2[2][1] = 1;
      digit2[2][4] = 1;
      digit2[3][1] = 1;
      digit2[3][2] = 1;
      digit2[3][3] = 1;
      digit2[3][4] = 1;
      digit2[4][4] = 1;
      digit2[5][4] = 1;
      digit2[6][4] = 1;      
      copyArray(digit2, 5);
      break;

    case 5: 
      digit2[1][1] = 1;
      digit2[1][2] = 1;
      digit2[1][3] = 1;
      digit2[1][4] = 1;
      digit2[2][1] = 1;
      digit2[3][1] = 1;
      digit2[3][2] = 1;
      digit2[3][3] = 1;
      digit2[4][4] = 1;
      digit2[5][4] = 1;
      digit2[6][1] = 1;
      digit2[6][2] = 1;
      digit2[6][3] = 1;
      copyArray(digit2, 5);
      break;

    case 6: 
      digit2[1][2] = 1;
      digit2[1][3] = 1;
      digit2[1][4] = 1;
      digit2[2][1] = 1;
      digit2[3][1] = 1;
      digit2[4][1] = 1;
      digit2[4][2] = 1;
      digit2[4][3] = 1;
      digit2[4][4] = 1;
      digit2[5][1] = 1;
      digit2[5][4] = 1;
      digit2[6][2] = 1;
      digit2[6][3] = 1;
      copyArray(digit2, 5);
      break;

    case 7: 
      digit2[1][1] = 1;
      digit2[1][2] = 1;
      digit2[1][3] = 1;
      digit2[1][4] = 1;
      digit2[2][4] = 1;
      digit2[3][3] = 1;
      digit2[4][2] = 1;
      digit2[5][1] = 1;
      digit2[6][1] = 1;
      copyArray(digit2, 5);
      break;

    case 8: 
      digit2[1][2] = 1;
      digit2[1][3] = 1;
      digit2[2][1] = 1;
      digit2[2][4] = 1;
      digit2[3][2] = 1;
      digit2[3][3] = 1;
      digit2[4][1] = 1;
      digit2[4][4] = 1;
      digit2[5][1] = 1;
      digit2[5][4] = 1;
      digit2[6][2] = 1;
      digit2[6][3] = 1;
      copyArray(digit2, 5);
      break;

    case 9:
      digit2[1][2] = 1;
      digit2[1][3] = 1;
      digit2[1][4] = 1;
      digit2[2][1] = 1;
      digit2[2][4] = 1;
      digit2[3][1] = 1;
      digit2[3][4] = 1;
      digit2[4][2] = 1;
      digit2[4][3] = 1;
      digit2[4][4] = 1;
      digit2[5][4] = 1;
      digit2[6][4] = 1;
      copyArray(digit2, 5);
      break;
  }
  
  for (int offset = 0; offset < arrayLength-8; offset++) {
    unsigned long start = micros();
    while (micros() < start+500000) {
      printScore(arrayInput, offset);
    }
  }
}

//deletes bowl game element from internal representation
void deleteBowl() {
  for (int i = 0; i < 8; i++) {
    if (screen[6][i] == 2) {
      screen[6][i] = 0;
    }

    if (screen[7][i] == 3) {
      screen[7][i] = 0;
    } 
  }
}

//helps map the bowl to the screen based on the current reading from the potentiometer
void checkBowl() {
  switch(xVal) {
    case 0:
      screen[6][0] = 2;
      screen[6][4] = 2;
      screen[7][1] = 3;
      screen[7][2] = 3;
      screen[7][3] = 3;
      break;

    case 1:
      screen[6][1] = 2;
      screen[6][5] = 2;
      screen[7][2] = 3;
      screen[7][3] = 3;
      screen[7][4] = 3;
      break;

    case 2:
      screen[6][2] = 2;
      screen[6][6] = 2;
      screen[7][3] = 3;
      screen[7][4] = 3;
      screen[7][5] = 3;
      break;

    case 3:
      screen[6][3] = 2;
      screen[6][7] = 2;
      screen[7][4] = 3;
      screen[7][5] = 3;
      screen[7][6] = 3;
      break;
  }
}

//MAGIC! making stars, scoring points, and crushing dreams
//array values translation:
//1 = normal stars
//8 = final star of the game
//2 = rim of the bowl
//3 = bottom of the bowl
void starfall() {
  for (int i = 7; i > -1; i--) {
     for (int j = 0; j < 8; j++) {
       if (i == 7) {
         //stars disappear when they hit the ground
         if (screen[i][j] == 1) {
          screen[i][j] = 0;
         } else if (screen[i][j] == 8) {
          screen[i][j] = 0;
          finished = true;
         }
       } else if (screen[i][j] == 8) {
         if (screen[i+1][j] == 3) {
           score++;
           screen[i][j] = 0;
           finished = true;
         } else if (screen[i+1][j] == 2) {
           //stars and bowl collision detection
           screen[i][j] = 0;
           finished = true;
         } else {
           //starfall
           screen[i+1][j] = 8;
           screen[i][j] = 0;
         }
       } else if (screen[i][j] == 1) {
         if (screen[i+1][j] == 3) {
           score++;
           screen[i][j] = 0;
         } else if (screen[i+1][j] == 2) {
           //stars and bowl collision detection
           screen[i][j] = 0;
         } else {
           //starfall
           screen[i+1][j] = 1;
           screen[i][j] = 0;
         }
       }
     }
   }
}

//keeps track of the internal game state that needs to be updated on the LED matrix
//stars fall once every refresh of the screen = targetFrameRate
void updateState() {
  if (frameRateCounter >= targetFrameRate) {
    starfall();
    makeStars();
    frameRateCounter = 0;    
  }
   
   deleteBowl();
   checkBowl();
   frameRateCounter++;
}

void setup() {
  //opening title screen: STARFALL
int opening [8][42] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
  
  for (int thisPin = 0; thisPin < 8; thisPin++) {
    // initialize the output pins:
    pinMode(cols[thisPin], OUTPUT);
    pinMode(rows[thisPin], OUTPUT);
    // take the row pins (i.e. the cathodes) high to ensure that the LEDS are off:
    digitalWrite(rows[thisPin], HIGH);
  }

  randomSeed(analogRead(7)); //use noise from unused analog pin to seed the random() function
  scrollingText(opening, 42); //display opening title
  delay(500);
  while (micros() < 9000000) {
    makeScreen(starDisplay, 0);
  }
  delay(500);
}

void loop() {
  xVal = analogRead(A5);            // reads the value of the potentiometer
  xVal = map(xVal, 885, 1023, 0, 3);     // scale it to use in game (value between 0 and 3); potentiometer seems wonky so initial values were calibrated for proper functioning

  unsigned long start = micros(); //track the time from program start
  
  if (finished) {
    //ends game and displays score
    while (micros() < start+3000000) {
      makeScreen(starDisplay, 0);
    }
    delay(500);
    scrollingScore(scoreDisplay, 14);
    delay(500);
    while (micros() < start+8000000) {
      makeScreen(starDisplay, 0);
    }
    delay(500);
    finished = false;
    score = 0;
    currentStars = 1;
  } else {
    //displaying the game state
    while (micros() < start+100000) {
      makeScreen(screen, 0);
    }
  
    updateState();
  }
}

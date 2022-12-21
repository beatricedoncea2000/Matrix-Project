#include <LiquidCrystal.h>
#include <string.h>
#include <EEPROM.h>
#include "LedControl.h"


// LCD  
const byte RS = 9;
const byte EPin = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;

LiquidCrystal lcd(RS, EPin, d4, d5, d6, d7);
LedControl lc = LedControl(12, 11, 10, 1);


// JOYSTICK 
const int pinSW = 13;
const int pinX = A0;
const int pinY = A1;
int xValue = 0;
int yValue = 0;
bool swState;
bool lastSwState;
int minThreshold = 400;
int maxThreshold = 600;
bool joyMovedX = false;
bool joyMovedY = false;
bool selectedOption = false;

// BUZZER
const int buzzerPin = A2;
int soundOn = 0;


// MENU
const String MenuOptions[5] = {
  "Start game",
  "Highscores",
  "Settings",
  "About",
  "How to play"  
};

int lastSelectedOption = 0;
bool changed = false;
int option = 0;
bool startLCD = true;
int viewMenu = 2500;
int menu = 0;


// PLAYER
String player1 = "", player2 = "", player3 = "", player4 = "", player5 = "";
int highscore1 = 0, highscore2 = 0, highscore3 = 0, highscore4 = 0, highscore5 = 0;
String currentPlayerName = "DefPlay01";
unsigned int charPosition = 0;
unsigned int character = 65;
unsigned int highscore = 0;
unsigned int score = 0;

// LEVEL DIFFICULTY
unsigned int selectedDifficultyValue = 1;

// SETTINGS
unsigned int LCDContrast = 90;
const int LCDContrastPin = 9;
unsigned int LCDBrightness = 128;
const int LCDBrightnessPin = 3;
unsigned int matrixBrightness = 60;

// MATRIX
const int matrixSize = 8;
int snake[matrixSize * matrixSize][2];
int snakeLength = matrixSize / 3;
int food[2];
int velocity[2];
bool gameOver = false;
bool gamePause = false;
int blinkCount;
const long advanceDelay = 20;
bool isGameOver = false;
bool exitScreenState1 = false;
bool exitScreenState2 = false;


// LIVES & GAME
byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};
int lives = 1;
long currentTime;
long prevMove;



void setup() {
  analogWrite(3, 200);
  // lcd.begin(16, 2);

  // displayHello();

  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  lcd.begin(16, 2);
  pinMode(LCDBrightnessPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lc.shutdown(0, false);
  LCDBrightness = readStringFromEEPROM(100).toInt();
  matrixBrightness = readStringFromEEPROM(200).toInt();
  analogWrite(LCDBrightnessPin, LCDBrightness);
  lc.setIntensity(0, matrixBrightness);
  // displayHello();

  attachInterrupt(digitalPinToInterrupt(pinSW), RestartOrPause , RISING);
  initGame();
  DisplaySnake();
}

void loop() {
  if (startLCD == true) {
    displayHello();
  }

  if (millis() > viewMenu) {
    startLCD = false;
    lcd.clear();
  }

  if (startLCD == false) {
    lcd.setCursor(0, 0);
    xValue = analogRead(pinX);
    menu = updateJoystick(menu);
    swState = digitalRead(pinSW);

    if (swState != lastSwState && swState == LOW) {
      selectedOption = !selectedOption;
      option = 0;
      delay(10);
    }

    displaySubMenu(menu);
    lastSwState = swState;
  }
  delay(100);
  MatrixStartGame();
}


void displayHello(){
  byte dino[8] =       
  { B00000,
    B00111,
    B00101,
    B10111,
    B11100,
    B11111,
    B01101,
    B01100,
  };

  lcd.createChar(0, dino);
  lcd.clear();

  lcd.print("Hi! Loading..."); 
  lcd.setCursor(7, 1);
  lcd.write(byte(0));
}


void GameOverMessage(){
  byte dino[8] =       
  { B00000,
    B00111,
    B00101,
    B10111,
    B11100,
    B11111,
    B01101,
    B01100,
  };

  lcd.createChar(0, dino);
  lcd.clear();

  lcd.print("Game Over"); 
  lcd.setCursor(7, 1);
  lcd.write(byte(0));

  if (soundOn == 1)
    tone(buzzerPin,150);
}


int updateJoystick(int selectOnMenu) {
  // scrolling down the menu
  if (xValue < minThreshold && joyMovedX == false && !selectedOption) {
    if (selectOnMenu < 4) {
      selectOnMenu++;
    }

    else {
      selectOnMenu = 0;
    }

    joyMovedX = true;
  }

  // scrolling up the menu
  if (xValue > maxThreshold && joyMovedX == false && !selectedOption) {
    if (selectOnMenu > 0) {
      selectOnMenu--;
    }

    else {
      selectOnMenu = 4;
    }

    joyMovedX = true;
  }

  // nothing
  if (xValue >= minThreshold && xValue <= maxThreshold && !selectedOption) {
    joyMovedX = false;
  }

  return selectOnMenu;
}


// displaying the menu options on lcd
void displayMenu(int menu) {
  switch(menu) {
    case 0:
      lcd.setCursor(0, 1);
      lcd.print(MenuOptions[0]);
      break;

    case 1:
      lcd.setCursor(0, 1);
      lcd.print(MenuOptions[1]);
      break;

    case 2:
      lcd.setCursor(0, 1);
      lcd.print(MenuOptions[2]);
      break;

    case 3:
      lcd.setCursor(0, 1);
      lcd.print(MenuOptions[3]);
      break;
    
    case 4:
      lcd.setCursor(0, 1);
      lcd.print(MenuOptions[4]);
      break;
  }
}


// select a submenu
void displaySubMenu(int menu) {
  if (!selectedOption) {
    displayMenu(menu);
  }

  if (selectedOption) {
    switch(menu) {
      case 0:
        SnakeGame();
        break;

      case 1:
        option = 0;
        DisplayHighscores();
        break;

      case 2:
        option = 0;
        DisplaySettings(option);
        break;

      case 3:
        option = 0;
        DisplayAbout();
        break;

      case 4:
        option = 0;
        DisplayHowToPlay();
        break;
    }
  }

  if (lastSelectedOption != menu) {
    changed = !changed;
    clearLCD(changed);
    lastSelectedOption = menu;
  }
}


void clearLCD(bool changed){
  if(changed){
    lcd.clear();
    changed = !changed;
  }
}


void DisplayHighscores() {
  String name = "";
  // read the name of the player1
  name = readStringFromEEPROM(10);
  // name of player1
  name = name.substring(0, 4);
  player1.remove(0, 4);
  player1 += name;
  //highscore of player1
  highscore1 = readStringFromEEPROM(20).toInt();

  // same operations for the other 4 players
  name = readStringFromEEPROM(30);
  name = name.substring(0, 4);
  player2.remove(0, 4);
  player2 += name;
  highscore2 = readStringFromEEPROM(40).toInt();

  name = readStringFromEEPROM(50);
  name = name.substring(0, 4);
  player3.remove(0, 4);
  player3 += name;
  highscore3 = readStringFromEEPROM(60).toInt();

  name = readStringFromEEPROM(70);
  name = name.substring(0, 4);
  player4.remove(0, 4);
  player4 += name;
  highscore4 = readStringFromEEPROM(80).toInt();

  name = readStringFromEEPROM(90);
  name = name.substring(0, 4);
  player5.remove(0, 4);
  player5 += name;
  highscore5 = readStringFromEEPROM(100).toInt();

  // display the scores in order
  while (true) {
    delay(100);
    xValue = analogRead(pinX);
    yValue = analogRead(pinY);
    swState = digitalRead(pinSW);

    // scroll down with the joystick
    if (yValue > maxThreshold && joyMovedY == false) {
      if (option < 4) {
        option++;
      }

      else {
        option = 0;
      }

      joyMovedY = true;
    }   

    // scroll up with the joystick
    if (yValue < minThreshold && joyMovedY == false) {
      if (option > 0) {
        option--;
      }

      else {
        option = 4;
      }

      joyMovedY = true;
    }

    if (yValue >= minThreshold && yValue <= maxThreshold) {
      joyMovedY = false;
    }

    // clear the LCD
    if (lastSelectedOption != option) {
      lcd.clear();
    }

    // display the top 5 scores
    switch(option) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Highscore ");
        lcd.setCursor(0, 1);
        lcd.print("1.");
        lcd.print(player1);
        lcd.print(": ");
        lcd.print(highscore1);
        break;
    
      case 1:
        lcd.setCursor(0, 0);
        lcd.print("Highscore ");
        lcd.setCursor(0, 1);
        lcd.print("2.");
        lcd.print(player2);
        lcd.print(": ");
        lcd.print(highscore2);
        break;

      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Highscore ");
        lcd.setCursor(0, 1);
        lcd.print("3.");
        lcd.print(player3);
        lcd.print(": ");
        lcd.print(highscore3);
        break;

      case 3:
        lcd.setCursor(0, 0);
        lcd.print("Highscore ");
        lcd.setCursor(0, 1);
        lcd.print("4.");
        lcd.print(player4);
        lcd.print(": ");
        lcd.print(highscore4);
        break;

      case 4:
        lcd.setCursor(0, 0);
        lcd.print("Highscore ");
        lcd.setCursor(0, 1);
        lcd.print("5.");
        lcd.print(player5);
        lcd.print(": ");
        lcd.print(highscore5);
        break;
    }

    // back to the menu when push the joystick
    if (swState != lastSwState && swState == LOW) {
      selectedOption = !selectedOption;
      break;
    }

    lastSelectedOption = option;
    lastSwState = swState;
  }
}




void DisplaySettings(int option) {
  while (true) {
    delay(100);
    yValue = analogRead(pinY);
    swState = digitalRead(pinSW);

    if (yValue > maxThreshold && joyMovedY == false) {
      if (option < 5) {
        option++;
      }

      else {
        option = 0;
      }

      joyMovedY = true;
    } 

    if (yValue < minThreshold && joyMovedY == false) {
      if (option > 0) {
        option--;
      }

      else {
        option = 5;
      }

      joyMovedY = true;
    }

    if (yValue >= minThreshold && yValue <= maxThreshold) {
      joyMovedY = false;
    }

    switch(option) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Settings");
        lcd.setCursor(0, 1);
        lcd.print("Enter name");
        break;

      case 1:
        lcd.setCursor(0, 0);
        lcd.print("Settings");
        lcd.setCursor(0, 1);
        lcd.print("Set difficulty");
        break;

      // case 2:
      //   lcd.setCursor(0, 0);
      //   lcd.print("Settings");
      //   lcd.setCursor(0, 1);
      //   lcd.print("Set LCD contrast");
      //   break;

      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Settings");
        lcd.setCursor(0, 1);
        lcd.print("Set LCD brightness");
        break;

      case 3:
        lcd.setCursor(0, 0);
        lcd.print("Settings");
        lcd.setCursor(0, 1);
        lcd.print("Set matrix brightness");
        break;

      case 4:
        lcd.setCursor(0, 0);
        lcd.print("Settings");
        lcd.setCursor(0, 1);
        lcd.print("Sound on/off");
        break;
    }

    // select an option by pushing the joystick
    if (swState != lastSwState && swState == LOW) {
      break;
    }
    lastSwState = swState;
    
    if (lastSelectedOption != option) {
      lcd.clear();
    } 

    delay(100);
    lastSelectedOption = option;
  }

  String name = "";
  name = readStringFromEEPROM(0);
  name = name.substring(0, 4);
  currentPlayerName.remove(0, 4);
  currentPlayerName += name;
  lcd.clear();

  switch(option) {
    case 0:
      while (true) {
        delay(100);
        xValue = analogRead(pinX);
        yValue = analogRead(pinY);
        swState = digitalRead(pinSW);
        lcd.setCursor(0, 0);
        lcd.print("Enter your name: ");
        lcd.setCursor(0, 1);
        lcd.print(currentPlayerName);

        if (yValue > maxThreshold && charPosition < 3 && joyMovedY == false) {
          charPosition++;
          joyMovedY = true;
        }

        if (yValue < minThreshold && charPosition > 0 && joyMovedY == false) {
          charPosition--;
          joyMovedY = true;
        }

        if (yValue >= minThreshold && yValue <= maxThreshold) {
          joyMovedY = false;
        }

        // move on the Ox axis to change the current character
        character = (int)(currentPlayerName[charPosition]);

        if (xValue > maxThreshold && character > 65 && joyMovedX == false) {
          lcd.clear();
          character--;
          joyMovedX = true;
        }

        if (xValue < minThreshold && character < 90 && joyMovedX == false) {
          lcd.clear();
          character++;
          joyMovedX = true;
        }

        if (xValue >= minThreshold && xValue <= maxThreshold) {
          joyMovedX = false;
        }

        // display the new name
        currentPlayerName[charPosition] = character;
        lcd.setCursor(charPosition, 1);
        lcd.print((char)(character));

        if (swState != lastSwState && swState == LOW) {
          writeStringInEEPROM(0, currentPlayerName);
          selectedOption = !selectedOption;
          option = 0;
          break;
        }
        delay(100);
        lastSwState = swState;
      }
      break;

    case 1:
      while (true) {
        delay(100);
        yValue = analogRead(pinY);
        swState = digitalRead(pinSW);

        // select the difficulty of the level at the game starting
        if (yValue > maxThreshold && joyMovedY == false && selectedDifficultyValue < 3) {
          selectedDifficultyValue++;
          joyMovedY = true;
        }

        if (yValue < minThreshold && joyMovedY == false && selectedDifficultyValue > 1) {
          selectedDifficultyValue--;
          joyMovedY = true;
        }

        if (yValue >= minThreshold && yValue <= maxThreshold) {
          joyMovedY = false;
        }

        setDifficulty(selectedDifficultyValue);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Level ");
        lcd.print(selectedDifficultyValue);

        if (swState != lastSwState && swState == LOW) {
          selectedOption = !selectedOption;
          option = 0;
          delay(10);
          lcd.clear();
          break;
        }
        delay(100);
        lastSwState = swState;
      }
      break;

    // case 2:
    //   while (true) {
    //     delay(100);
    //     yValue = analogRead(pinY);
    //     swState = digitalRead(pinSW);

    //     LCDContrast = readStringFromEEPROM(100).toInt();

    //     if (yValue > maxThreshold && LCDContrast < 255) {
    //       LCDContrast++;
    //     }

    //     if (yValue < minThreshold && LCDContrast > 0) {
    //       LCDContrast--;
    //     }

    //     // save the new contrast value in EEPROM
    //     writeStringInEEPROM(100, String(LCDContrast));
    //     analogWrite(LCDContrastPin, LCDContrast);

    //     lcd.clear();
    //     lcd.setCursor(0, 0);
    //     lcd.print("Contrast ");
    //     lcd.print(LCDContrast);

    //     // exit to the main menu
    //     if (swState != lastSwState && swState == LOW) {
    //       selectedOption = !selectedOption;
    //       option = 0;
    //       delay(10);
    //       lcd.clear();
    //       break;
    //     }
    //     lastSwState = swState;
    //   }
    //   break;

    case 2:
      while (true) {
        delay(100);
        yValue = analogRead(pinY);
        swState = digitalRead(pinSW);

        LCDBrightness = readStringFromEEPROM(200).toInt();

        if (yValue > maxThreshold && LCDBrightness < 255) {
          LCDBrightness++;
        }

        if (yValue < minThreshold && LCDBrightness > 0) {
          LCDBrightness--;
        }

        writeStringInEEPROM(200, String(LCDBrightness));
        analogWrite(LCDBrightnessPin, LCDBrightness);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Brightness ");
        lcd.print(LCDBrightness);

        // exit to the main menu
        if (swState != lastSwState && swState == LOW) {
          selectedOption = !selectedOption;
          option = 0;
          delay(10);
          lcd.clear();
          break;
        }
        lastSwState = swState;
      }
      break;

    case 3:
    while(true){
      delay(100);
      yValue = analogRead(pinY);
      swState = digitalRead(pinSW);

      matrixBrightness = readStringFromEEPROM(300).toInt();
      
      if(yValue > maxThreshold && matrixBrightness < 16){
        matrixBrightness++;
      }

      if(yValue < minThreshold && matrixBrightness > 1){
        matrixBrightness--;
      }

      writeStringInEEPROM(300, String(matrixBrightness));
      lc.setIntensity(0, matrixBrightness);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Matrix ");
      lcd.print(matrixBrightness);
      
      for(int i = 0; i < matrixSize; i++){
        for(int j = 0; j < matrixSize; j++){
          lc.setLed(0, i, j, true);
        }
      }
      
      // exit to the main menu
      if(swState != lastSwState && swState == LOW){
        selectedOption = !selectedOption;
        option = 0;
        delay(10);
        lcd.clear();
        
        for(int i = 0; i < matrixSize;i++){
          for(int j = 0; j < matrixSize; j++){
            lc.setLed(0,i,j,false);
          }
        }
        break;
      }
      lastSwState = swState;
    }
    break;

    case 4:
      while (true) {
        switch(soundOn) {
          case 0:
            lcd.setCursor(0, 0);
            lcd.print("Sound");
            lcd.setCursor(0, 1);
            lcd.print("On");
            break;

          case 1:
            lcd.setCursor(0, 0);
            lcd.print("Sound");
            lcd.setCursor(0, 1);
            lcd.print("Off");
            break;
        }

        while (true) {
          delay(100);
          yValue = analogRead(pinY);
          swState = digitalRead(pinSW);

          if (yValue > maxThreshold && joyMovedY == false && soundOn < 1) {
            soundOn++;
            joyMovedY = true;
          }

          if (yValue < minThreshold && joyMovedY == false && soundOn > 0) {
            soundOn--;
            joyMovedY = true;
          }

          if (yValue >= minThreshold && yValue <= maxThreshold) {
            joyMovedY = false;
          }

          // exit to the main menu
          if(swState != lastSwState && swState == LOW){
            selectedOption = !selectedOption;
            option = 0;
            delay(10);
            lcd.clear();
          }
        }

        switch(soundOn) {
          case 0:
            break;

          case 1:
            noTone(buzzerPin);
        }
      }
  }
}




void DisplayAbout() {
  while(true){
    delay(100);
    
    yValue = analogRead(pinY);
    swState = digitalRead(pinSW);
    
    if (yValue > maxThreshold && joyMovedY == false) {
      if (option > 0){
       option--;
      }
      
      else {
       option = 2;
      }

      joyMovedY = true;
    }

    if (yValue < minThreshold && joyMovedY == false) {
      if (option < 2){
       option++;
      }
      
      else {
        option = 0;
      }

      joyMovedY = true;
    }

    if (yValue >= minThreshold && yValue <= maxThreshold ) {
      joyMovedY = false;
    }
    
    switch(option){
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Name");
        lcd.setCursor(0,1);
        lcd.print("SNAKE");
        break;

      case 1:  
        lcd.setCursor(0, 0);
        lcd.print("Owner");
        lcd.setCursor(0,1);
        lcd.print("Beatrice D");
        break;
      
      case 2:  
        lcd.setCursor(0, 0);
        lcd.print("Github");
        lcd.setCursor(0,1);
        lcd.print("beatricedoncea2000");
        break;
    }
    
    if(swState != lastSwState && swState == LOW){
      selectedOption = !selectedOption;
      option = 0;
      delay(10);
      break;
    }
    
    if(lastSelectedOption != option){
      lcd.clear();
    }

    lastSelectedOption = option;
    lastSwState = swState;
  }
}


void DisplayHowToPlay() {
  int selected = 0;

  while(true) {
    delay(100);

    yValue = analogRead(pinY);
    swState = digitalRead(pinSW);

    if (yValue < minThreshold && joyMovedY == false) {
      selected = 1;
      joyMovedY = true;
    }

    else if (yValue > maxThreshold && joyMovedY == false){
      selected = 0;
      joyMovedY = true;
    }

    if (yValue >= minThreshold && yValue <= maxThreshold ) {
      joyMovedY = false;
    }

    switch(selected) {
      case 1:
      lcd.setCursor(0, 0);
      lcd.print("Use joystick");
      lcd.setCursor(0,1);
      lcd.print("move snake");
      break;
    }

    if(swState != lastSwState && swState == LOW){
      selectedOption = !selectedOption;
      option = 0;
      delay(10);
      break;
    }
    
    if(lastSelectedOption != option){
      lcd.clear();
    }

    lastSelectedOption = option;
    lastSwState = swState;
  }
}


void writeStringInEEPROM(int addressOffset, const String &stringToWrite) {
  int length = stringToWrite.length();

  // clear the stored values in EEPROM
  for (int i = addressOffset; i < addressOffset + length; ++i) {
    EEPROM.update(i, 0);
  }

  // write in EEPROM
  EEPROM.update(addressOffset, length);
  for (int i = 0; i < length; ++i) {
    EEPROM.update(addressOffset + i + 1, stringToWrite[i]);
  }
}


String readStringFromEEPROM(int addressOffset) {
  // read the length of the message from EEPROM
  int newStringLength = EEPROM.read(addressOffset);
  char data[newStringLength];

  // read the string from EEPROM
  for (int i = 0; i < newStringLength; ++i) {
    data[i] = EEPROM.read(addressOffset + i + 1);
  }

  return String(data);
}



void setDifficulty (int selectedDifficultyValue){
  snakeLength = matrixSize + selectedDifficultyValue + 1;
}


void StartGameMenu() {
  int newScore = 0;

  String gamePlayer = readStringFromEEPROM(0);
  String player1 = readStringFromEEPROM(10);
  int highScore1 = readStringFromEEPROM(20).toInt();
  String player2 = readStringFromEEPROM(30);
  int highScore2 = readStringFromEEPROM(40).toInt();
  String player3 = readStringFromEEPROM(50);
  int highScore3 = readStringFromEEPROM(60).toInt();
  String player4 = readStringFromEEPROM(70);
  int highScore4 = readStringFromEEPROM(80).toInt();
  String player5 = readStringFromEEPROM(90);
  int highScore5 = readStringFromEEPROM(100).toInt();

  lcd.clear();
  
  // display lives, score and difficulty
  lcd.createChar(1, heart);
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.print(":1 Score: 0");
  lcd.setCursor(0, 1);
  lcd.print("Difficulty: ");
  lcd.print(selectedDifficultyValue);
  lives = 1;

  newScore = SnakeGame();
  swState = digitalRead(pinSW);

  // update the highscore top
  if (newScore > highscore1) {
    writeStringInEEPROM(10, gamePlayer);
    writeStringInEEPROM(20, String(newScore));

    writeStringInEEPROM(30, player1);
    writeStringInEEPROM(40, String(highscore1));
    writeStringInEEPROM(50, player2);
    writeStringInEEPROM(60, String(highscore2));
    writeStringInEEPROM(70, player3);
    writeStringInEEPROM(80, String(highscore3));
    writeStringInEEPROM(90, player4);
    writeStringInEEPROM(100, String(highscore4));

    highscore5 = highscore4;
    player5 = player4;
    highscore4 = highscore3;
    player4 = player3;
    highscore3 = highscore2;
    player3 = player2;
    highscore2 = highscore1;
    player2 = player1;
    highscore1 = newScore;
    player1 = gamePlayer;
    highscore = newScore;
  }

    else if (newScore > highscore2) {
      writeStringInEEPROM(30, gamePlayer);
      writeStringInEEPROM(40, String(newScore));
      writeStringInEEPROM(50, player2);
      writeStringInEEPROM(60, String(highscore2));
      writeStringInEEPROM(70, player3);
      writeStringInEEPROM(80, String(highscore3));
      writeStringInEEPROM(90, player4);
      writeStringInEEPROM(100, String(highscore4));
      highscore5 = highscore4;
      player5 = player4;
      highscore4 = highscore3;
      player4 = player3;
      highScore3 = highScore2;
      player3 = player2;
      highScore2 = newScore;
      player2 = gamePlayer;
    }

    else if (newScore > highscore3) {
      writeStringInEEPROM(50, gamePlayer);
      writeStringInEEPROM(60, String(newScore));
      writeStringInEEPROM(70, player3);
      writeStringInEEPROM(80, String(highscore3));
      writeStringInEEPROM(90, player4);
      writeStringInEEPROM(100, String(highscore4));
      highscore5 = highscore4;
      player5 = player4;
      highscore4 = highscore3;
      player4 = player3;
      highScore3 = newScore;
      player3 = gamePlayer;
    }

    else if (newScore > highscore4) {
      writeStringInEEPROM(70, gamePlayer);
      writeStringInEEPROM(80, String(newScore));
      writeStringInEEPROM(90, player4);
      writeStringInEEPROM(100, String(highscore4));
      highscore5 = highscore4;
      player5 = player4;
      highScore4 = newScore;
      player4 = gamePlayer;
    }

    else if (newScore > highscore5) {
      writeStringInEEPROM(90, gamePlayer);
      writeStringInEEPROM(100, String(newScore));
      highScore5 = newScore; 
      player5 = gamePlayer;
    }

    selectedOption = !selectedOption;
} 


void winPoints(int score, int lives){
  // winning sound
  tone(buzzerPin, 261); 
 
  lcd.clear();
  lcd.createChar(1, heart);
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.print(":");
  lcd.print(lives);
  
  // new score
  lcd.print(" Score:");
  lcd.print(score);
  lcd.setCursor(0, 1);
  
  // difficulty
  lcd.print("Difficulty:");
  lcd.print(selectedDifficultyValue);
}




// intialize game
void initGame() {
  prevMove = currentTime = 0;
  blinkCount = 3;

  int half = matrixSize / 2;

  for (int i = 0; i < snakeLength; ++i) {
    snake[i][0] = half - 1;
    snake[i][1] = half + 1;
  }

  food[0] = half + 1;
  food[1] = half - 1;

  velocity[0] = 0;
  velocity[1] = -1;
}


void DisplaySnake() {
  for (int i = 0; i < snakeLength; ++i) {
    lc.setLed(0, snake[i][0], snake[i][1], 1);
  }
  lc.setLed(0, food[0], food[1], 1);
}


void clearMatrix() {
  for (int i = 0; i < matrixSize; ++i) {
    for (int j = 0; j < matrixSize; ++j) {
      lc.setLed(0, i, j, 0);
    }
  }
}

int SnakeGame() {
  int score = 0;
  int head[2] = {snake[0][0] + velocity[0], snake[0][1] + velocity[1]};

  // if the snake goes out of bounds for i position
  if (head[0] < 0 || head[0] >= matrixSize) {
    delay(1000);
    GameOverMessage();
    isGameOver = true;
    return score;
  }

  // if the snake goes out of bounds for j position
  if (head[1] < 0 || head[1] >= matrixSize) {
    delay(1000);
    GameOverMessage();
    isGameOver = true;
    return score;
  }

  // snake eats itself
  for (int i = 0; i < snakeLength; ++i) {
    if (snake[i][0] == head[0] && snake[i][1] == head[1]) {
      delay(1000);
      exitScreenState1 = EndGameShowingPoints(score);
      exitScreenState2 = EndGameScoreInTop(score);
      lives = 0;
      isGameOver = true;
      return score;
    }
  }

  // snake eats food 
  bool snakeGrow = (head[0] == food[0] && head[1] == food[1]);
  if (snakeGrow) {
    snakeLength++;
    randomSeed(currentTime);
    food[0] = random(matrixSize);
    food[1] = random(matrixSize);
    score++;
  }

  // move snake
  for (int i = snakeLength - 1; i >= 0; --i) {
    snake[i + 1][0] = snake[i][0];
    snake[i + 1][1] = snake[i][1];
  }

  // move snake head according to velocity
  snake[0][0] += velocity[0];
  snake[0][1] += velocity[1];

  return score;
}



void RestartOrPause() {
  // restart
  if (gamePause == false && gameOver == true) {
    initGame();
    gameOver = false;
  }

  // pause
  else if (gamePause == false && gameOver == false) {
    gamePause = true;
  }

  // resume
  else if (gamePause == true && gameOver == false) {
    prevMove = currentTime = 0;
    blinkCount = 3;
    gamePause = false;
  }
}



void JoystickMovement() {
  int dx = map(analogRead(pinX), 0, 906, 2, -2);
  int dy = map(analogRead(pinY), 0, 906, -2, 2);

  // xValue = analogRead(pinX);
  // yValue = analogRead(pinY);

  if (dx != 0) {
    dx = dx / abs(dx);
  }
  
  if (dy != 0) {
    dy = dy / abs(dy);
  }

  // move on the y axis
  if (dy != 0 && velocity[0] != 0) {
      velocity[0] = 0;
      velocity[1] = dy;    
  }
  
  // move on the x axis
  if (dx != 0 && velocity[1] != 0) {
      velocity[0] = dx;
      velocity[1] = 0;
  }
}


void MatrixStartGame() {
  if (!gameOver && !gamePause) {
    clearMatrix();
    DisplaySnake();

    if (currentTime - prevMove > advanceDelay) {
        gameOver = isGameOver;
        prevMove = currentTime;    
    }

    else {
        // blink to indicate Game Start
        while (blinkCount > 0) {
            clearMatrix();
            delay(300);
            DisplaySnake();
            delay(300);
            blinkCount--;
        }
    }

    JoystickMovement();
    currentTime++;
  }
}


bool EndGameShowingPoints(int score){
  // displays a text with the no. of points 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Yey! You won");
  lcd.setCursor(0, 1);
  lcd.print(score);
  lcd.print(" points");
  
  while (true){
    swState = digitalRead(pinSW);
    
    // exit when button is pressed
    if (swState != lastSwState && swState == LOW) {
      for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
          lc.setLed(0, i, j, false);
        }
      }
      break;
    }
    lastSwState = swState;
  }
  return true;
}


bool EndGameScoreInTop(int score){
  // displaying the score
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.print(score);
  highscore1 = readStringFromEEPROM(20).toInt();
  
  if (score > highscore1){
    lcd.setCursor(0, 1);
    lcd.print("New highscore");
  }

  while (true){
    delay(100);
    swState = digitalRead(pinSW);
    
    // exit when button is pressed
    if (swState != lastSwState && swState == LOW){
      for (int i = 0; i < matrixSize; i++){
        for (int j = 0; j < matrixSize; j++){
          lc.setLed(0, i, j, false);
        }
      }
      break;
    }
    lastSwState = swState;
  }
  return true;
}
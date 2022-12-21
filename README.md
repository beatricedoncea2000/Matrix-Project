# Snake Game

#### Short story about the snake game
In 1997 Snake quickly became a phenomenon after launching it on phones. We have to say "Thank you" for our childhood game to Taneli Armanto.
I am pretty sure that each of us was at grandparents house some time and took the grandparent's phone and find this game. You are a child, 
obviously curious and started playing. One game, game over. Second game and so on. That little snake makes you want to achieve a new highscore.
I had my first phone at 10 and I found this game a new era because I could play this everywhere not only in front of a PC (yes, I belong to that generation sadly, but not as sadly as these new children generation).

#### Game description
The player controls a long, thin creature, resembling a snake, which roams around on a bordered plane, picking up food (or some other item), trying to avoid hitting its own tail or the edges of the playing area. Each time the snake eats a piece of food, its tail grows longer, making the game increasingly difficult.

#### How to play
Firstly, this game has only 1 life. You as a player are now the snake. If you eat yourself, the game is over because even the snake can be poisoned by its own venom :).
If you eat the food, let's say an apple, you will grow larger. 
Obviously, if you eat that food it disappears and another one appears on a random position. 
The goal here is to make the snake as large as possible before the game is over (the snake bites itself).

<details>
<summary> Requirements </summary>

##### MENU
Create a menu for your game, emphasis on the game. You should scroll on the LCD with the joystick. The menu should include:
1. When powering up a game, a greeting message should be shown fora few moments.

2. Categories like:
  - **Start Game** that start the initial level of the game
  - **Highscore**: 
    - Initially the score is 0
    - Highscore is updating when the game is done
    - Save the top 5+ values in EEPROM with the name and score
  - **Settings**:
    - Enter name
    - Starting level or difficulty of the game should be selected
    - LCD contrast control using either potentiometer of saving the value in EEPROM
    - LCD brightness control and save the value to EEPROM
    - Matrix brightness control and save it to EEPROM
    - Sound on or off and save it to EEPROM
    - Extra stuff that should be saved to EEPROM
  - **About**, here are the details about the creator of the game like game name, author name, github link or user
  - **How to play**, a short description about the game
  
3. **While playing the game** display information like:
    - Lives
    - Level/Difficulty
    - Score
    - Etc.
    
4. **Upon game ending**:
  - Screen 1: a message such as "Congratulations on reaching level/score X". "You did better than y people.". etc. Switches to screen 2 upon interaction (button press) or after a few moments.
  - Screen 2: display relevant game info: score, time, lives left etc. Must inform player if he/she beat the highscore. This menu should only be closed by the player, pressing a button.
  
  ##### GAME
- You must add basic sounds to the game
- It must be intuitive andfun to play
- It must make sense in the current setup. Study the idea of a panning camera - aka the 8x8 led doesn’t need to be the entire map. It can only be the current field of view of the player.
- The levels must progress dynamically. Aka the level difficulty, score and other properties should be a function of the level number or time. However, the challenge here is for it to grow in a playable way - not too easy for a long time so it becomes boring, not too hard too fast so it’s not attractive. Also, it’s nice to have an ending, a final level, a boss etc. It shouldn’t necessarily go on forever (but that’s fine, though). 
</details>


<details>
<summary> Components </summary>
- LCD
- 8x8 matrix
- resistors
- electrolytic capacitor
- ceramic capacitor
- MAX7219 Driver
- joystick
- buzzer
- potentiometer
- wires
</details>


<details>
<summary> Setup </summary>

</details>


<details>
<summary> Demo </summary>

</details>

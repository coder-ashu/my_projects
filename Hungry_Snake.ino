#include <LedControl.h>

//Define The Snake
typedef struct Snake{
  int head[2];     // the (row, column) of the snake head
  int body[40][2]; //An array that contains the (row, column) coordinates
  int len;         //The length of the snake 
  int dir[2];      //A direction to move the snake along
}Snake;

//Define The Apple
typedef struct Apple{
  int rPos; //The row index of the apple
  int cPos; //The column index of the apple
}Apple;


//MAX72XX led Matrix
const int CLK =10;
const int CS =11;
const int DIN =12;
LedControl lc = LedControl(DIN, CLK, CS,1);

int buzzer = 9;
const int varXPin = A3;//X Value  from Joystick
const int varYPin = A4;//Y Value from Joystick

byte pic[8] = {0,0,0,0,0,0,0,0};//The 8 rows of the LED Matrix

Snake snake = {{1,5},{{0,5}, {1,5}}, 2, {1,0}};//Initialize a snake object
Apple apple = {(int)random(0,8),(int)random(0,8)};//Initialize an apple object

//Variables To Handle The Game Time
float oldTime = 0;
int start = 0;
float timer = 0;
float updateRate = 3;

byte sad[8]= {0x3C,0x42,0xA5,0x81,0x99,0xA5,0x42,0x3C};

int i,j;//Counters

void printByte(byte character[])
{
  int i = 0;
  for(i=0;i<8;i++)
  {
    lc.setRow(0,i,character[i]);
  }
}

void setup() 
{
  Serial.begin(9600);
  delay(1000);

  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,5);
  /* and clear the display */
  lc.clearDisplay(0);

  //Set Joystick Pins as INPUTs
  pinMode(varXPin, INPUT);
  pinMode(varYPin, INPUT);
}

int score = 0;
void loop()
{
  float deltaTime = deltaTime();
  timer += deltaTime;

  //Check For Inputs
  int xVal = analogRead(varXPin);
  int yVal = analogRead(varYPin);
  
  if(xVal<500 && snake.dir[1]==0)
  {
    snake.dir[0] = 0;
    snake.dir[1] = -1;
  }
  else if(xVal >890 && snake.dir[1]==0)
  {
    snake.dir[0] = 0;
    snake.dir[1] = 1;
  }
  else if(yVal<500 && snake.dir[0]==0)
  {
    snake.dir[0] = -1;
    snake.dir[1] = 0;
  }
  else if(yVal >890 && snake.dir[0]==0)
  {
    snake.dir[0] = 1;
    snake.dir[1] = 0;
  }
  
  //Update
  if(timer > 1000/updateRate){
    timer = 0;
    reset();//Reset (Clear) the 8x8 LED matrix
  
  int newHead[2] = {snake.head[0]+snake.dir[0], snake.head[1]+snake.dir[1]};

  //Handle Borders
  if(newHead[0]==8)
  {
    newHead[0]=0;
  }
  else if(newHead[0]==-1)
  {
    newHead[0] = 7;
  }
  else if(newHead[1]==8)
  {
    newHead[1]=0;
  }
  else if(newHead[1]==-1)
  {
    newHead[1]=7;
  }
  
  //Check If The Snake hits itself
   for(j=0;j<snake.len;j++)
   {
    if(snake.body[j][0] == newHead[0] && snake.body[j][1] == newHead[1])
    {
      //Pause the game for 1 sec then Reset it
      printByte(sad);
      tone(buzzer, 1000);
      delay(2000);
      noTone(buzzer);

      snake = {{1,5},{{0,5}, {1,5}}, 2, {1,0}};//Reinitialize the snake object
      apple = {(int)random(0,8),(int)random(0,8)};//Reinitialize an apple object
      return;
    }
  }

  //Check if The snake ate the apple
  if(newHead[0] == apple.rPos && newHead[1] ==apple.cPos)
  {
    snake.len++;
    apple.rPos = (int)random(0,8);
    apple.cPos = (int)random(0,8);
  }
  else
  {
    move();//Shifting the array to the left
  }
  
  snake.body[snake.len-1][0]= newHead[0];
  snake.body[snake.len-1][1]= newHead[1];
  
  snake.head[0] = newHead[0];
  snake.head[1] = newHead[1];
  
  //Update the Display(snake and apple)
  for(j=0;j<snake.len;j++)
  {
    pic[snake.body[j][0]] |= 128 >> snake.body[j][1];
  }
  pic[apple.rPos] |= 128 >> apple.cPos;

  }
  
  Render();
}

float deltaTime()
{
  float currentTime = millis();
  float dt = currentTime - oldTime;
  oldTime = currentTime;
  return dt;
}

void reset()
{
  for(int j=0;j<8;j++)
  {
    pic[j] = 0;
  }
}

void Render()
{
   for(i=0;i<8;i++)
   {
    lc.setRow(0,i,pic[i]);
   }
}

void move()
{
  for(j=1;j<snake.len;j++)
  {
    snake.body[j-1][0] = snake.body[j][0];
    snake.body[j-1][1] = snake.body[j][1];
  }
}
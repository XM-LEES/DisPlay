#include <TFT_eSPI.h>
#include "img.h"



//各个管脚接口宏定义：
#define KEY_F 14
#define KEY_G 25
#define KEY_A 35
#define KEY_B 34


/***KEY declare***/
void KEY_Init();  //按键管脚初始化
void KEY_F_Handler();
//void KEY_G_Handler();
//void KEY_A_Handler();
//void KEY_B_Handler();


/***global***/
int RECORD = 0;
bool start = false;
bool det_G = false;
//bool det_A = false;
//bool det_B = false;





/***const int define***/
const int GROUND = 167;
const int GRAVITY = 4;

/***Dino pos define***/
const int X_D = 10;
int Y_D = GROUND;
int V_D = 0;
int X_O = 250;

/***img***/
void img_init();
void draw_dino(int Y_OLD, int Y_NEW); //for dino jump
/***logic***/
void game();
void gameover(int score);
void walk(int &);
void jump(int &, bool &);

void updateobs1(int score);
bool detect();


TFT_eSPI tft;
TFT_eSprite sprite_dino = TFT_eSprite(&tft);
TFT_eSprite sprite1 = TFT_eSprite(&tft);


void setup() {
  Serial.begin(115200); //串口打印，波特率115200
  KEY_Init(); //按键初始化
  // put your setup code here, to run once:
  img_init();
  
}

void loop()
{
//  Serial.println(det_A);
//  delay(500);
  // put your main code here, to run repeatedly:
  if(start == false && digitalRead(KEY_A) == LOW)
  {
    delay(50);
    if(digitalRead(KEY_A) == LOW)
    {
      start = false;
      tft.fillScreen(TFT_WHITE);
      game(); 
    }
  }
}


/***KEY***/
void KEY_Init()
{
  pinMode(KEY_F, INPUT_PULLUP);
  pinMode(KEY_G, INPUT_PULLUP);
  pinMode(KEY_A, INPUT_PULLUP);
  pinMode(KEY_B, INPUT_PULLUP);
}


/***IMG***/
void img_init()
{
  tft.begin();
  tft.setRotation(0);                 //屏幕旋转
  tft.fillScreen(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK);
  sprite_dino.setColorDepth(16);
  sprite_dino.createSprite(40, 43);
  sprite1.setColorDepth(16);
  sprite1.createSprite(15, 33);
  sprite1.pushImage(0, 0, 15, 33, tree1);
  
  tft.pushImage(0, 0, 240, 240, menu);//fill start menu
  tft.drawString("HI: " + String(RECORD), 120, 5);
}

void draw_dino(int Y_OLD, int Y_NEW)
{
  sprite_dino.pushImage(0, 0, 40, 43, dino0);
  sprite_dino.pushSprite(10, Y_NEW);
  if(V_D < 0)
    tft.fillRect(10, Y_OLD + 43 - abs(Y_NEW - Y_OLD), 40, abs(Y_NEW - Y_OLD), TFT_WHITE);
  if(V_D >= 0)
    tft.fillRect(10, Y_OLD, 40, abs(Y_NEW - Y_OLD), TFT_WHITE);
}


/***LOGIC***/
void game()
{
  //add ground here
  Serial.println("in game\n");
  bool alive = true;
  bool isjump = false;
  int score = 0;
  //reset
  Y_D = GROUND;
  V_D = 0;
  X_O = 250;
  delay(500);
  //create obstacle
  
  while(alive)
  {
    //choose move type
    if(digitalRead(KEY_F) == LOW)
    {
      delay(20);
      if(digitalRead(KEY_F) == LOW){
      isjump = true;
      Serial.println("F");
      }
    }
    switch (isjump)
    {
      case false:
      walk(score);
      break;
      case true:
      jump(score, isjump);
      break;
    }

    //
    updateobs1(score);
    alive = detect();
  }
  gameover(score);
}


void gameover(int score)
{ 
  RECORD = RECORD > score ? RECORD : score;
  tft.pushImage(0, 0, 240, 240, endmenu);//fill over menu
  tft.drawString("HI: " + String(RECORD), 120, 5);
  tft.drawString("YOU: " + String(score), 120, 20);
  while(!digitalRead(KEY_B) == LOW);
  tft.pushImage(0, 0, 240, 240, menu);//fill start menu
  tft.drawString("HI: " + String(RECORD), 120, 5);
}


void walk(int & score)
{
  sprite_dino.pushImage(0, 0, 40, 43, dino1);
  sprite_dino.pushSprite(X_D, GROUND);
  updateobs1(score);
  delay(130);
  updateobs1(score);
  tft.fillRect(190, 5, 50, 15, TFT_WHITE);
  tft.drawString("SCORE: " + String(score), 110, 5);
  score++;
  sprite_dino.pushImage(0, 0, 40, 43, dino2);
  sprite_dino.pushSprite(X_D, GROUND);
  updateobs1(score);
  delay(130);
  updateobs1(score);
  tft.fillRect(190, 5, 50, 15, TFT_WHITE);
  tft.drawString("SCORE: " + String(score), 110, 5);
  score++;
}

void jump(int & score, bool & isjump)
{
  Serial.println("jump start");
  Y_D = GROUND - 1;
  V_D = -26;
  int Y_OLD = Y_D;
  double dt = 0.4;
  int counter = 0;
  while(1)
  {
    updateobs1(score);
    if(Y_D < GROUND)
    {
      Y_OLD = Y_D;
      Y_D += V_D * dt + GRAVITY * dt * dt / 2;
      V_D += GRAVITY * dt;
      draw_dino(Y_OLD, Y_D);
      delay(60);
      if(counter++ % 2 == 0)
      {
        tft.fillRect(190, 5, 50, 15, TFT_WHITE);
        tft.drawString("SCORE: " + String(score), 110, 5);
        score++;
      }
      updateobs1(score);
    }
    else
      break;
  }
  Y_D = GROUND;
  draw_dino(Y_OLD, Y_D);
  V_D = 0;
  isjump = false;
  Serial.println("jump end");
}



/******obstacle********/
//IF   YOU WANT TO ADD NEW OBSTACLE, CHECK HERE


void updateobs1(int score)
{
  int X_OLD = X_O;
  int index = 1 + score / 50;
  X_O -= index;
  sprite1.pushSprite(X_O, 177);
  tft.fillRect(X_O + 15, 177, index, 33, TFT_WHITE);
  if(X_O < -15)
    X_O = 240 + random(240);
}


bool detect()
{
  bool alive = true;
  if(X_O < 50 && X_O > -5)
      alive = Y_D > 220 ? true : false;

 if(digitalRead(KEY_G) == LOW)
 {
   delay(20);
   if(digitalRead(KEY_G) == LOW){
   alive = true;
   Serial.println("YOU HIT THE POINT");
  }
 }
  return alive;
}

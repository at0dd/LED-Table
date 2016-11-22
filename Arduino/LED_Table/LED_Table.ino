/* LED Table
   Alex Todd & Blake Winchester
   Using Adafruit Neopixel, Bluetooth Low Engery, and RedBearLab Libraries

   Conditions:
   0 - Solid Color
   1 - Solid Color Chase
   2 - Rainbow
   3 - Rainbow Chase
   4 - Connect Four
   5 - Wildcat
   6 - Tree
*/

/* ------------ Initial Setup ------------ */
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <boards.h>
#include <RBL_nRF8001.h>
#include <RBL_services.h>
#define PIN 6
#define ROWS 12
#define COLUMN_WIDTH 12
#define N_LEDS 288
#define BLUETOOTH_TX 2
#define BLUETOOTH_RX 1

SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);

int Condition = EEPROM.read(0);     //Initial Condition
byte RedValue = EEPROM.read(1);     //Initial Red Value
byte GreenValue = EEPROM.read(2);   //Initial Green Value
byte BlueValue = EEPROM.read(3);    //Initial Blue Value

/* ------------ LED Setup & Functions ------------ */
class NeoPatterns : public Adafruit_NeoPixel
{
  public:
    unsigned long Interval;             //Milliseconds between Updates
    unsigned long lastUpdate;           //Last update of position
    uint32_t Color1, Color2;            //Current Colors
    uint16_t TotalSteps;                //Total steps in a pattern
    uint16_t Index;                     //Current step in the pattern
    void (*OnComplete)();               //Callback on completion of pattern
    int ConnectFour[6][7];
    int PlayerTurn;

    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
      : Adafruit_NeoPixel(pixels, pin, type)
    {
      OnComplete = callback;
    }

    /* ------------ Update ------------ */
    //Updates the LEDs.
    void Update()
    {        
      Condition = ble_read();
      if(EEPROM.read(0) == 4 && Condition > 9) //We're mid-connect 4 game
      {
        int column = Condition;
        DropPieceInColumn(column);
      }
      else
      {
        EEPROM.update(0, Condition);
        switch (Condition)
        {
          case 0:
            UpdateEEPROM();
            SolidColor();
            break;
          case 1:
            UpdateEEPROM();
            for (int i = 0; i < N_LEDS; i++)
              setPixelColor(i, 0);
            show();
            break;
          case 2:
            RainbowCycleSetup();
            break;
          case 3:
            for (int i = 0; i < N_LEDS; i++)
              setPixelColor(i, 0);
            rainbowChase();
            break;
          case 4:
            PrepareConnectFour();
            break;
          case 5:
            DisplayWildcat();
            break;
          case 6:
            DisplayTree();
            break;
          default:
            break;
        }
      }

    }

    /* ------------ Update EEPROM ------------ */
    //Updates the EEPROM with new values.
    void UpdateEEPROM() {
      RedValue = ble_read();
      GreenValue = ble_read();
      BlueValue = ble_read();
      EEPROM.update(1, RedValue);
      EEPROM.update(2, GreenValue);
      EEPROM.update(3, BlueValue);
    }

    /* ------------ Solid Color ------------ */
    void SolidColor() 
    {
      for (int i = 0; i < numPixels(); i++)
      {
        setPixelColor(i, Color(RedValue, GreenValue, BlueValue));
      }
      show();
    }

    /* ------------ Solid Color Chase ------------ */
    void chase() 
    {
      for (uint16_t i = 0; ((i < N_LEDS + 4) && ble_available() == 0); i++)
      {
        setPixelColor(i  , Color(RedValue, GreenValue, BlueValue)); // Draw new pixel
        setPixelColor(i - 4, 0); // Erase pixel a few steps back
        show();
        if (i % 5 == 0)
          ble_do_events();
        delay(25);
      }
      //ble_do_events();
    }
    /* ------------ Rainbow ------------ */
    void RainbowCycleSetup() 
    {
      Interval = 5;
      TotalSteps = 1280;
      Index = 0;
      for (int j = 0; j < TotalSteps && ble_available() == 0; j++)
        RainbowCycle(j);
    }

    void RainbowCycle(int j)
    {
      ble_do_events();
      for (int i = 0; i < N_LEDS; i++)
        setPixelColor(i, Wheel(((i * 256 / numPixels()) + j) & 255));
      show();
    }

    /* ------------ Wheel ------------ */
    // Input a value 0 to 255 to get a color value.
    // The colors are a transition r - g - b - back to r.
    uint32_t Wheel(byte WheelPos)
    {
      WheelPos = 255 - WheelPos;
      if (WheelPos < 85)
      {
        return Color(255 - WheelPos * 3, 0, WheelPos * 3);
      }
      else if (WheelPos < 170)
      {
        WheelPos -= 85;
        return Color(0, WheelPos * 3, 255 - WheelPos * 3);
      }
      else
      {
        WheelPos -= 170;
        return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
      }
    }

    /* ------------ Rainbow Chase ------------ */
    void rainbowChase()
    {
      int counter = 0;
      uint32_t Rainbow[7] = { Color(209, 0, 0), Color(254, 78, 0), Color(255, 212, 0), Color(51, 221, 0), Color(17, 51, 204), Color(34, 0, 102), Color(51, 0, 68)};

      for (uint16_t i = 0; (i < N_LEDS + 7 && ble_available() == 0); i++)
      {
        setPixelColor(i, Rainbow[counter]);
        setPixelColor(i - 7, 0);
        show();
        if (counter == 6)
        {
          counter = 0;
        }
        else
        {
          counter++;
        }
        ble_do_events();
        delay(20);
      }
    }

    void clearBoard()
    {
      for (int i = 0; i < N_LEDS; i++)
      {
        setPixelColor(i, 0);
      }
      show();
    }

    void PrepareConnectFour()
    {      
      clearBoard();

      /* Outline Border */
      for(int i = 80; i <= 287; i++)
      {
        setPixelColor(i, Color(0, 0, 60));        
        if(i % 2 == 0 && i < 271)
        {
          i += 14;
        }
        else if(i < 271)
        {
          setPixelColor((i + 1), Color(0, 0, 60));
          i += 15;
        }
      }
      show();

      for(int i = 0; i <= 5; i++)
      {
        for(int j = 0; j <= 6; j++)
        {
          ConnectFour[i][j] = 0;
        }
      }      
               
      PlayerTurn = 1;
    }

    void DropPieceInColumn(int col)
    {
      col = col - 10;
      int oppositeTurn = (PlayerTurn == 1) ? 2 : 1;
      int row;
      for(row = 0; row <= 5; row++)
      {
        if(row == 5 && (ConnectFour[row][col] == 0 && ConnectFour[row][col] != oppositeTurn)) //If we're at the very bottom, place piece regardless
        {
          ConnectFour[row][col] = PlayerTurn;
        }
        else if(ConnectFour[row+1][col] != 0 && ConnectFour[row][col] != oppositeTurn)
        {
          ConnectFour[row][col] = PlayerTurn;
          break;
        }
      }

      if(row == 6)
        row--;
      
      PlacePlayerPiece();

      bool PlayerWin = DidPlayerWin(row, col);
      if(PlayerWin)
      {
        //for(int i = 0; i <= 15; i++)
        //{
        //  setPixelColor(i, Color(0, 0, 255));
        //}
        DeclareWinner(PlayerTurn);
        show();
        delay(6000);
        PrepareConnectFour();
      }
      PlayerTurn = (PlayerTurn == 1) ? 2 : 1;
    }

    void PlacePlayerPiece()
    {
      int pos = 81;
      for(int row = 0; row <= 5; row++)
      {
        if(row % 2 == 0)
        {
          for(int col = 6; col >= 0; col--)
          {
            if(ConnectFour[row][col] != 0)
            {
              uint32_t color = (ConnectFour[row][col] == 1) ? Color(222, 0, 0) : Color(160, 156, 0);
              setPixelColor(pos, color);
              setPixelColor((pos+1), color);
              setPixelColor(((pos+1)+((col*3)+3)+(col)), color);
              setPixelColor((((pos+1)+((col*3)+3))+(col+1)), color);
            }
            pos += 2;
          }
        }
        else
        {
          for(int col = 0; col <= 6; col++)
          {            
            pos -= 2;
            if(ConnectFour[row][col] != 0)
            {
              uint32_t color = (ConnectFour[row][col] == 1) ? Color(222, 0, 0) : Color(160, 156, 0);
              setPixelColor(pos, color);
              setPixelColor((pos+1), color);
              setPixelColor(((pos+1)+((col*3)+3)+(col)), color);
              setPixelColor((((pos+1)+((col*3)+3))+(col+1)), color);
            }
          }
        }        
        pos += 32;
      }
      show();
    }

    bool DidPlayerWin(int row, int col)
    {
      int counter = 0;
      /*Left-Right Checking*/
         for(int column = col; column >= 0; column--)//Check Left
         {
          if(ConnectFour[row][column] == PlayerTurn)
          {
            counter++;   
          }
          else
          {
            break;
          }
         }
         for(int column = (col+1); column <= 6; column++)//Check Right
         {
          if(ConnectFour[row][column] == PlayerTurn)
          {
            counter++;   
          }
          else
          {
            break;
          }
         }

      if(counter >= 4)
      {
        return true;
      }

      counter = 0;

      /*Vertical-Down Checking*/
      for(int r = row; r <= 5; r++)
      {
        if(ConnectFour[r][col] == PlayerTurn)
          {
            counter++;   
          }
          else
          {
            break;
          }
      }
      for(int r = (row-1); r >= 0; r--)
      {
          if(ConnectFour[r][col] == PlayerTurn)
          {
            counter++;   
          }
          else
          {
            break;
          }
      }
      if(counter >= 4)
      {
        return true;
      }

      counter = 0;

      /*This will check SW to NE*/
      int c = col;
      for(int r = row; r >= 0 && c <= 6; r--)
      {
        if(ConnectFour[r][c] == PlayerTurn)
        {
          counter++;  
        }
        else
        {
          break;
        }
        c++;
      }
      c = (col-1);
      for(int r = (row+1); r <= 5 && c >= 0; r++)
      {
          Serial.println("R: " + String(r) + " C: " + String(c));
          if(ConnectFour[r][c] == PlayerTurn)
          {
            counter++;   
          }
          else
          {
            break;
          }
        c--;
      }

      if(counter >= 4)
      {
        return true;
      }

      counter = 0;

      /*This will check SE to NW*/
      c = col;
      for(int r = row; r >= 0 && c >= 0; r--)
      {
        if(ConnectFour[r][c] == PlayerTurn)
        {
          counter++;  
        }
        else
        {
          break;
        }
        c--;
      }
      c = (col+1);
      for(int r = (row+1); r <= 5 && c <= 6; r++)
      {
          Serial.println("R: " + String(r) + " C: " + String(c));
          if(ConnectFour[r][c] == PlayerTurn)
          {
            counter++;   
          }
          else
          {
            break;
          }
        c++;
      }

      if(counter >= 4)
      {
        return true;
      }

      return false;
    }

    void DeclareWinner(int player)
    {
      uint32_t color = (player == 1) ? Color(222, 0, 0) : Color(160, 156, 0);        
      setPixelColor(5, color);
      setPixelColor(6, color);
      setPixelColor(7, color);
      setPixelColor(9, color);        
      setPixelColor(24, color);
      setPixelColor(26, color);        
      setPixelColor(37, color);
      setPixelColor(38, color);
      setPixelColor(39, color);        
      setPixelColor(58, color);
      setPixelColor(69, color);
      if(player == 1)
      {        
        setPixelColor(22, color);
        setPixelColor(41, color);
        setPixelColor(54, color);
        setPixelColor(73, color);
      }
      else
      {
        setPixelColor(10, color);
        setPixelColor(11, color);
        setPixelColor(20, color);
        setPixelColor(41, color);
        setPixelColor(42, color);
        setPixelColor(43, color);
        setPixelColor(54, color);
        setPixelColor(73, color);
        setPixelColor(74, color);
        setPixelColor(75, color);
      }
    }

    void DisplayWildcat()
    {
        clearBoard();
        
        uint32_t light = Color(74, 35, 128);
        uint32_t dark = Color(31, 9, 60);
        setPixelColor(32, light);
        setPixelColor(33, light);
        setPixelColor(34, light);
        setPixelColor(38, light);
        setPixelColor(42, light);
        setPixelColor(43, light);
        setPixelColor(50, light);
        setPixelColor(51, light);
        setPixelColor(58, light);
        setPixelColor(59, light);
        setPixelColor(60, light);
        setPixelColor(61, light);
        setPixelColor(68, light);
        setPixelColor(78, light);
        setPixelColor(79, light);
        setPixelColor(80, light);
        setPixelColor(102, light);
        setPixelColor(103, light);
        setPixelColor(107, light);
        setPixelColor(108, light);
        setPixelColor(111, light);
        setPixelColor(113, light);
        setPixelColor(114, light);
        for(int i = 116; i <= 124; i++)
        {
          setPixelColor(i, light);
        }
        setPixelColor(129, light);
        setPixelColor(130, light);
        setPixelColor(131, light);
        setPixelColor(135, light);
        setPixelColor(136, light);
        setPixelColor(137, light);
        setPixelColor(138, light);
        setPixelColor(149, light);
        setPixelColor(150, light);
        setPixelColor(151, light);
        setPixelColor(152, light);
        setPixelColor(156, light);
        setPixelColor(157, light);
        setPixelColor(158, light);
        setPixelColor(160, light);
        setPixelColor(166, light);
        setPixelColor(167, light);
        setPixelColor(170, light);
        setPixelColor(172, light);
        setPixelColor(186, light);
        setPixelColor(190, light);
        setPixelColor(193, light);
        setPixelColor(198, light);
        setPixelColor(204, light);
        setPixelColor(218, light);
        setPixelColor(222, light);
        setPixelColor(226, light);
        setPixelColor(227, light);
        setPixelColor(229, light);
        
        setPixelColor(35, dark);
        setPixelColor(36, dark);
        setPixelColor(37, dark);
        setPixelColor(39, dark);
        setPixelColor(40, dark);
        setPixelColor(41, dark);
        setPixelColor(52, dark);
        setPixelColor(53, dark);
        setPixelColor(54, dark);
        setPixelColor(55, dark);
        setPixelColor(56, dark);
        setPixelColor(57, dark);
        for(int i = 62; i <= 101; i++)
        {
          setPixelColor(i, dark);
          if(i == 67)
          {
            i++;
          }
          else if(i == 77)
          {
            i = 80;
          }
        }
        setPixelColor(104, dark);
        setPixelColor(105, dark);
        setPixelColor(106, dark);
        setPixelColor(109, dark);
        setPixelColor(110, dark);
        setPixelColor(125, dark);
        setPixelColor(126, dark);
        setPixelColor(127, dark);
        setPixelColor(128, dark);
        setPixelColor(132, dark);
        setPixelColor(133, dark);
        setPixelColor(134, dark);
        setPixelColor(153, dark);
        setPixelColor(154, dark);
        setPixelColor(155, dark);
        setPixelColor(159, dark);
        setPixelColor(162, dark);
        setPixelColor(163, dark);
        setPixelColor(164, dark);
        setPixelColor(165, dark);
        setPixelColor(168, dark);
        setPixelColor(169, dark);
        setPixelColor(179, dark);
        setPixelColor(180, dark);
        setPixelColor(181, dark);
        setPixelColor(182, dark);
        setPixelColor(183, dark);
        setPixelColor(184, dark);
        setPixelColor(187, dark);
        setPixelColor(188, dark);
        setPixelColor(189, dark);
        setPixelColor(194, dark);
        setPixelColor(195, dark);
        setPixelColor(196, dark);
        setPixelColor(199, dark);
        setPixelColor(200, dark);
        setPixelColor(201, dark);
        setPixelColor(202, dark);
        setPixelColor(203, dark);
        setPixelColor(219, dark);
        setPixelColor(220, dark);
        setPixelColor(221, dark);
        setPixelColor(228, dark);
        show();        
    }

    void DisplayTree()
    {
      uint32_t trunk = Color(37, 17, 0);
      uint32_t leaves = Color(0, 139, 0);
      uint32_t mid = Color(0, 83, 0);

      clearBoard();
      
      setPixelColor(37, leaves);
      setPixelColor(38, leaves);
      setPixelColor(39, leaves);
      setPixelColor(40, leaves);
      setPixelColor(41, leaves);
      setPixelColor(51, leaves);
      setPixelColor(52, leaves);
      setPixelColor(53, leaves);
      setPixelColor(55, leaves);
      setPixelColor(56, leaves);
      setPixelColor(58, leaves);
      setPixelColor(59, leaves);
      setPixelColor(60, leaves);
      setPixelColor(61, leaves);
      setPixelColor(65, leaves);
      setPixelColor(67, leaves);
      setPixelColor(68, leaves);
      setPixelColor(70, leaves);
      setPixelColor(71, leaves);
      setPixelColor(72, leaves);
      setPixelColor(73, leaves);
      setPixelColor(74, leaves);
      setPixelColor(75, leaves);
      setPixelColor(77, leaves);
      setPixelColor(82, leaves);
      setPixelColor(84, leaves);
      setPixelColor(85, leaves);
      setPixelColor(86, leaves);
      setPixelColor(87, leaves);
      setPixelColor(88, leaves);
      setPixelColor(89, leaves);
      setPixelColor(91, leaves);
      setPixelColor(92, leaves);
      setPixelColor(93, leaves);
      setPixelColor(94, leaves);
      for(int i = 97; i <= 117; i++)
      {
        setPixelColor(i, leaves);
        if(i == 110)
          i = 112;
      }
      setPixelColor(122, leaves);
      setPixelColor(123, leaves);
      setPixelColor(124, leaves);
      setPixelColor(132, leaves);
      setPixelColor(133, leaves);
      setPixelColor(134, leaves);
      setPixelColor(135, leaves);
      setPixelColor(136, leaves);
      setPixelColor(137, leaves);
      setPixelColor(138, leaves);
      setPixelColor(139, leaves);
      setPixelColor(140, leaves);
      setPixelColor(142, leaves);
      setPixelColor(147, leaves);
      setPixelColor(150, leaves);
      setPixelColor(151, leaves);
      setPixelColor(153, leaves);
      setPixelColor(154, leaves);
      setPixelColor(156, leaves);
      setPixelColor(157, leaves);
      setPixelColor(163, leaves);
      setPixelColor(164, leaves);
      setPixelColor(168, leaves);
      setPixelColor(171, leaves);
      setPixelColor(172, leaves);
      setPixelColor(173, leaves);

      setPixelColor(54, mid);
      setPixelColor(57, mid);
      setPixelColor(66, mid);
      setPixelColor(69, mid);
      setPixelColor(76, mid);
      setPixelColor(83, mid);
      setPixelColor(90, mid);
      setPixelColor(118, mid);
      setPixelColor(119, mid);
      setPixelColor(120, mid);
      setPixelColor(121, mid);
      setPixelColor(125, mid);
      setPixelColor(126, mid);
      setPixelColor(129, mid);
      setPixelColor(130, mid);
      setPixelColor(131, mid);
      setPixelColor(141, mid);
      setPixelColor(146, mid);


      setPixelColor(148, trunk);
      setPixelColor(149, trunk);
      setPixelColor(152, trunk);
      setPixelColor(155, trunk);
      setPixelColor(165, trunk);
      setPixelColor(166, trunk);
      setPixelColor(167, trunk);
      setPixelColor(169, trunk);
      setPixelColor(170, trunk);
      setPixelColor(182, trunk);
      setPixelColor(183, trunk);
      setPixelColor(184, trunk);
      setPixelColor(185, trunk);
      setPixelColor(198, trunk);
      setPixelColor(199, trunk);
      setPixelColor(200, trunk);
      setPixelColor(201, trunk);
      setPixelColor(214, trunk);
      setPixelColor(215, trunk);
      setPixelColor(216, trunk);
      setPixelColor(231, trunk);
      setPixelColor(232, trunk);
      setPixelColor(233, trunk);
      setPixelColor(246, trunk);
      setPixelColor(247, trunk);
      setPixelColor(248, trunk);
      setPixelColor(262, trunk);
      setPixelColor(263, trunk);
      setPixelColor(264, trunk);
      setPixelColor(265, trunk);
      setPixelColor(266, trunk);
      setPixelColor(276, trunk);
      setPixelColor(277, trunk);
      setPixelColor(278, trunk);
      setPixelColor(279, trunk);
      setPixelColor(280, trunk);
      setPixelColor(281, trunk);
      setPixelColor(282, trunk);

      show();
    }
    
}; //End NeoPatterns class

void StripComplete() {}

/* ------------ Create Strip Instance ------------ */
NeoPatterns strip(N_LEDS, PIN, NEO_GRB + NEO_KHZ800, &StripComplete);

int board[ROWS][COLUMN_WIDTH];
int SnakeLength = 1; //default is 1
int MoveDirection = 11; //8 is UP
//9 is DOWN
//10 is LEFT
//11 is RIGHT


void Refresh() {
  strip.Update();
}

/* ------------ Setup EEPROM ------------ */
//Code that runs only when the Arduino powers on.
// Writes the EEPROM to set initial values.
void FirstLifetime()
{
  if (EEPROM.read(0) == 255)
  {
    EEPROM.write(0, 0);
    EEPROM.write(1, 81);
    EEPROM.write(2, 40);
    EEPROM.write(3, 136);
  }
}

/* ------------ Setup ------------ */
void setup()
{
  Serial.begin(57600);    //DEBUG
  FirstLifetime();
  strip.begin();          //Initialize the LEDs
  ble_set_name("BLE");    //Set BLE Name
  ble_begin();            //Initialize the BLE
  if (Condition == 0)
    strip.SolidColor();
}

/* ------------ Main Loop ------------ */
void loop()
{
  ble_do_events();
  if (ble_available() > 0)
    strip.Update();
  if (Condition == 1)
    strip.chase();
  else if (Condition == 2)
    strip.RainbowCycleSetup();
  else if (Condition == 3)
    strip.rainbowChase();
}

#include <Watchy.h> //include the Watchy library
#include "NunitoSans_Bold17pt7b.h" //include fonts
#include "NunitoSans_Light17pt7b.h"
#include "NunitoSans_ExtraLight10pt7b.h" //for step counter, date and temperature
#include "settings.h" //same file as the one from 7_SEG example
#include "icons.h" //stock icons

#define DARKMODE false //toggle between true or false to have light or dark mode

class NonSlidingText : public Watchy{ //inherit and extend Watchy class
    public:
        NonSlidingText(const watchySettings& s) : Watchy(s) {}

        void drawSteps();
        void drawBattery();
        void drawDate();
        void drawWeather();

        void drawWatchFace(){ //override this method to customize how the watch face looks
          uint16_t lines = 0;
          const char *lows [12] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven"};
          const char *teensone [11] = {"","ten", "eleven", "twelve", "thir-", "four-", "fif-", "six-", "seven-", "eigh-", "nine-"};
          const char *teenstwo [11] = {"", "","", "teen", "teen", "teen", "teen", "teen", "teen", "teen", "teen"};
          const char *tens [10] = {"zero", "ten", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"};

          //draw Background
          display.fillScreen(GxEPD_WHITE);
          display.setTextColor(GxEPD_BLACK);
          display.setTextWrap(false);

          //draw Steps
          uint32_t stepCount = sensor.getCounter();
          if (currentTime.Hour == 0 && currentTime.Minute == 0){
            sensor.resetStepCounter();
          }// reset step counter at midnight

          display.setFont(&NunitoSans_ExtraLight10pt7b);
          display.drawBitmap(155, 148, steps, 19, 23, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
          display.setCursor(160, 190); 
          display.println(stepCount);

          //draw Date
          String dayOfWeek = dayShortStr(currentTime.Wday);
          display.setFont(&NunitoSans_ExtraLight10pt7b);
          char* dateStr;
          display.setCursor(15, 25);
          display.println(dayOfWeek);

          String month = monthShortStr(currentTime.Month);
          display.setFont(&NunitoSans_ExtraLight10pt7b);
          display.setCursor(60, 25); //printing date
          if(currentTime.Day < 10){
            display.print("0");
          }
          display.println(currentTime.Day);

          display.setCursor(90, 25);//printing month
          display.println(month);

          //draw Battery
          const uint8_t BATTERY_SEGMENT_WIDTH = 7;
          const uint8_t BATTERY_SEGMENT_HEIGHT = 11;
          const uint8_t BATTERY_SEGMENT_SPACING = 9;

          display.drawBitmap(154, 10, battery, 37, 21, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
          display.fillRect(159, 15, 27, BATTERY_SEGMENT_HEIGHT, DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);//clear battery segments
          int8_t batteryLevel = 0;
          float VBAT = getBatteryVoltage();
          if(VBAT > 4.1){
            batteryLevel = 3;
          }
          else if(VBAT > 3.95 && VBAT <= 4.1){
            batteryLevel = 2;
          }
          else if(VBAT > 3.80 && VBAT <= 3.95){
            batteryLevel = 1;
          }
          else if(VBAT <= 3.80){
            batteryLevel = 0;
          }
          for(int8_t batterySegments = 0; batterySegments < batteryLevel; batterySegments++){
            display.fillRect(159 + (batterySegments * BATTERY_SEGMENT_SPACING), 15, BATTERY_SEGMENT_WIDTH, BATTERY_SEGMENT_HEIGHT, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
          }

          //draw Weather
          const uint8_t WEATHER_ICON_WIDTH = 48;
          const uint8_t WEATHER_ICON_HEIGHT = 32;

          weatherData currentWeather = getWeatherData();  
          int8_t temperature = currentWeather.temperature;
          int16_t weatherConditionCode = currentWeather.weatherConditionCode;

          display.setFont(&NunitoSans_ExtraLight10pt7b);
          int16_t  x1, y1; 
          uint16_t w, h;
          display.getTextBounds(String(temperature), 160, 100, &x1, &y1, &w, &h);
          display.setCursor(160 - w, 100);
          display.println(temperature);
          display.setFont(&NunitoSans_ExtraLight10pt7b);
          display.drawBitmap(165, 85, currentWeather.isMetric ? celsius : fahrenheit, 26, 20, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
          const unsigned char* weatherIcon;

          //https://openweathermap.org/weather-conditions 
          if(weatherConditionCode > 801){//Cloudy
            weatherIcon = cloudy;
          } else if(weatherConditionCode == 801){//Few Clouds
            weatherIcon = cloudsun; 
          } else if(weatherConditionCode == 800){//Clear
            weatherIcon = sunny;
          } else if(weatherConditionCode >=700){//Atmosphere
            weatherIcon = atmosphere;
          } else if(weatherConditionCode >=600){//Snow
            weatherIcon = snow;
          } else if(weatherConditionCode >=500){//Rain
            weatherIcon = rain;
          } else if(weatherConditionCode >=300){//Drizle
            weatherIcon = drizzle;
          } else if(weatherConditionCode >=200){//Thunderstorm
            weatherIcon = thunderstorm;
          } else
          return;
          display.drawBitmap(148, 50, weatherIcon, WEATHER_ICON_WIDTH, WEATHER_ICON_HEIGHT, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);


          //draw Time
          lines += 1.5;
          display.setCursor(8, lines*65-5);
          display.setFont(&NunitoSans_Bold17pt7b);
          //setting hours in 12h format
          if(currentTime.Hour == 0){//midnight
            display.print(teensone[3]);
          } else if(currentTime.Hour == 12){//noon
            display.print(teensone[3]);
          }
            else if(currentTime.Hour < 12) {
            display.print(lows[currentTime.Hour]);
          } else if(currentTime.Hour < 24) {
            display.print(lows[currentTime.Hour-12]);
          }

          lines += 1;
          display.setCursor(8, lines*65-5);
          display.setFont(&NunitoSans_Light17pt7b);
          if(currentTime.Minute == 0) {
            display.print("o'clock");
          } else if (currentTime.Minute < 10) {
            display.setCursor(8, lines*65-5);
            display.print(lows[currentTime.Minute]);//modded aesthetics to have 1-9 minutes printed below hour
          } else if(currentTime.Minute < 20) {
            display.print(teensone[currentTime.Minute-9]);
            if (currentTime.Minute > 12) {
              lines += 1;
              display.setCursor(8, lines*65-5);
              display.print(teenstwo[currentTime.Minute%10]);
            };
          }
          else {
            display.println(tens[currentTime.Minute/10]);
            if(currentTime.Minute%10 > 0) {
              lines += 1;
              display.setCursor(8, lines*65-5);
              display.print(lows[currentTime.Minute%10]);
            }
          }

            
        }
};

NonSlidingText m(settings); //instantiate your watchface

void setup() {
  m.init(); //call init in setup
}

void loop() {
  // this should never run, Watchy deep sleeps after init();
}
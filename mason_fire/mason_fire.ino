#include <NeoPixelBrightnessBus.h>

#define LED_PIN 2
#define NUM_LEDS 140

NeoPixelBrightnessBus<NeoRgbFeature, Neo800KbpsMethod> strip(NUM_LEDS, LED_PIN);

#define R_IN 33
#define G_IN 34
#define B_IN 35

static byte heat1[NUM_LEDS];

void setup() {
  //Serial.begin(115200);
  pinMode(17, OUTPUT);
  digitalWrite(17, HIGH);
  
  strip.Begin();
  strip.Show(); // Initialize all pixels to 'off'
  strip.SetBrightness(100);
  strip.Show();
  strip.Show();

  delay(500);
}

int16_t adc_color[3] = {0,0,0};
uint8_t hue_color[3] = {0,0,0};

uint16_t wakeup = false;
uint16_t poweron = true;

HsbColor hue_hsb;

void loop() {

  adc_color[0] = (analogRead(R_IN));
  adc_color[1] = (analogRead(G_IN));
  adc_color[2] = (analogRead(B_IN));

  if (adc_color[0] < 10 && adc_color[1] < 10 && adc_color[2] < 10) {
    hue_color[0] = 0;
    hue_color[1] = 0;
    hue_color[2] = 0;
    wakeup = 50;
    if (poweron) {
      for (int i = 0; i < 155; i+=5) {
        strip.SetPixelColor(31, RgbColor(i,0,0));
        strip.Show();
        delay(2);
      }
      for (int i = 155; i >= 0; i-=5) {
        strip.SetPixelColor(31, RgbColor(i,0,0));
        strip.Show();
        delay(2);
      }
    }
    delay(100);
  } else {
    poweron = false;
    hue_color[0] = map(adc_color[0], 0, 4095, wakeup, 0);
    hue_color[1] = map(adc_color[1], 0, 4095, wakeup, 0);
    hue_color[2] = map(adc_color[2], 0, 4095, wakeup, 0);
    wakeup++;
    if (wakeup > 255) {
      wakeup = 255;
    }
  }

  RgbColor hue_rgb(hue_color[0], hue_color[1], hue_color[2]);
  hue_hsb = hue_rgb;
  
  
  Fire1Up(30,17,0, 0, 7);
  Fire1Up(25,25,0, 7, 14);
  Fire1Up(20,30,0, 14, 21);
  Fire1Up(15,40,0, 21, 28);
  Fire1Up(15,40,0, 28, 35);
  Fire1Up(25,40,0, 35, 42);
  Fire1Up(30,30,0, 42, 49);
  Fire1Up(25,25,0, 49, 56);
  Fire1Up(30,17,0, 56, 63);
}

// copied from https://www.tweaking4all.com/forums/topic/led-strip-effects-two-fire-effects/
void Fire1Up(int Cooling, int Sparking, int SpeedDelay, int from, int to) {

  int cooldown;

  int dir = 1;

  // Step 1.  Cool down every cell a little
  //for( int i = 0; i < NUM_LEDS; i++) {
  for( int i = from; i < to; i = i + dir) {
    //cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
    cooldown = random(0, ((Cooling * 10) / 20) + 2);
    if(cooldown>heat1[i]) {
      heat1[i]=0;
    } else {
      heat1[i]=heat1[i]-cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  //for( int k= NUM_LEDS - 1; k >= 2; k--) {
  for( int k = to - 1; k >= from + 2; k = k - dir) {
    heat1[k] = (heat1[k - dir] + heat1[k - dir - dir] + heat1[k - dir - dir]) / 3;
    //heat1[k] = (heat1[k - 1] + heat1[k - 2] + heat1[k - 2]) / 3;
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  /*for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat1[k] = (heat1[k - 1] + heat1[k - 2] + heat1[k - 2]) / 3;
  }*/

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    //int y = random(7);
    int y = random(4);
    y = (from + y);
    //heat1[y] = heat1[y] + random(160,255);
    //heat1[y] = heat1[y] + random(80,110);
    heat1[y] = random(120,160);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = from; j < to; j = j + dir) {
    setPixelHeatColor(j, heat1[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

// copied from https://www.tweaking4all.com/forums/topic/led-strip-effects-two-fire-effects/
void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  //red
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
  //blue
  /*
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, heatramp, 255, 255);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 0, heatramp, 64);
  } else {                               // coolest
    setPixel(Pixel, 0, heatramp/8, heatramp/4);
  }*/
}

void showStrip() {
   strip.Show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 
   Pixel = (6 - (Pixel - ((Pixel / 7) * 7))) * 9 + Pixel / 7;
   RgbColor rgb(red, green, blue);
   HsbColor hsb = rgb;
   hsb.B = hsb.B * hue_hsb.B;
   hsb.H = hsb.H + hue_hsb.H;
   if (hsb.H > 1.0f)  {
    hsb.H = hsb.H - 1.0f;
   }
   Serial.println(hue_hsb.B);
   
   strip.SetPixelColor(Pixel, hsb);
}

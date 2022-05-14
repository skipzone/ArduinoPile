#define FASTSPI_USE_DMX_SIMPLE
#define DMX_SIZE 81    // 3 DMX-27 boards

#include <DmxSimple.h>
#include "FastLED.h"
#include "dataStructures.h"

#define NUM_LEDS 20
#define DATA_PIN 2 //8

#define ENCODER_ACTIVE_PIN 7

// patterns
#define PANEL_RAINBOW
#define RED_FADE

CRGB    leds[NUM_LEDS];
CHSV    hsv_color[NUM_LEDS];
CRGB    rgb_color[NUM_LEDS];

unsigned long next_update; // use to schedule the next pattern call
uint8_t step_num;
uint8_t start_step;


// gamma correction for human-eye perception of WS2812 RGB LED brightness
// (from http://rgb-123.com/ws2812-color-output/ on 3 April 2014)
uint8_t g_gamma[] = {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,
      2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,
      6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 11, 11,
     11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
     19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28,
     29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
     40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54,
     55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
     71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
     90, 91, 93, 94, 95, 96, 98, 99,100,102,103,104,106,107,109,110,
    111,113,114,116,117,119,120,121,123,124,126,128,129,131,132,134,
    135,137,138,140,142,143,145,146,148,150,151,153,155,157,158,160,
    162,163,165,167,169,170,172,174,176,178,179,181,183,185,187,189,
    191,193,194,196,198,200,202,204,206,208,210,212,214,216,218,220,
    222,224,227,229,231,233,235,237,239,241,244,246,248,250,252,255
};

CRGB    backgroundRgb(g_gamma[128], 0, 0);
CRGB    foregroundRgb(g_gamma[192], 0, 0);
CHSV    tmpHsv(255,255,255);

// parameter structures for patterns
fadeParams_t fade_params;
singlePanelRainbowParams_t spr_params;
sparkleParams_t sparkle_params;
spiralParams_t spiral_params;

/*!
 * Set slow fade pattern parameters
 *
 * See dataStructures.h for parameter meanings
 */
void setFadeParams(fadeParams_t *params,
        uint8_t         num_panels,
        uint16_t        update_delay,
        CRGB            rgb_value,
        uint8_t         step_num,
        bool            increment)
{
    params->num_panels = num_panels;
    params->update_delay = update_delay;
    params->rgb_value = rgb_value;
//    params->rgb_value.r = 0;
//    params->rgb_value.g = 0;
//    params->rgb_value.b = 0;
    params->step_num = step_num;
    params->increment = increment;
}

/*!
 * Fade leds in and out, from off to full on and back
 * 
 * Only works for red.
 * 
 * \param leds struct CRGB * to list of pixel values
 * \param params fadeParams_t * to the parameters struct
 *
 * \return time (ms) until the next pattern update
 */
uint16_t fade(struct CRGB *leds, fadeParams_t *params)
{
    fill_solid(leds, params->num_panels, params->rgb_value);
    if (params->increment) {
        params->step_num++;
        if (params->step_num == 128) {
            params->increment = false;
        }
    } else {
        params->step_num--;
        if (params->step_num == 0) {
            params->increment = true;
        }
    }

    params->rgb_value.r = g_gamma[params->step_num];
    return params->update_delay;
}

/*!
 * Set single panel rainbow params
 *
 * See dataStructure.h for parameter meanings
 */
void setSinglePanelRainbowParams(singlePanelRainbowParams_t *params,
        uint8_t     num_panels,
        uint8_t     active_panel,
        CRGB        active_rgb_value,
        CHSV        active_hsv_value,
        bool        rand,
        uint16_t    update_delay,
        bool        increment
)
{
    params->num_panels = num_panels;
    params->active_panel = active_panel;
    params->active_rgb_value = active_rgb_value;
//    params->active_rgb_value.r = 0;
//    params->active_rgb_value.g = 0;
//    params->active_rgb_value.b = 0;
    params->active_hsv_value = active_hsv_value;
//    params->active_hsv_value.h = 255;
//    params->active_hsv_value.s = 255;
//    params->active_hsv_value.v = 255;
    params->rand = rand;
    params->update_delay = update_delay;
    params->increment = increment;
}

/*!
 * Run a rainbow through each individual panel, from red back to red
 *
 * \param leds struct CRGB * to list of pixel values
 * \param params singlePanelRainbowParams_t pointer to pattern parameters
 *
 * \return time (ms) until the next pattern update
 */
uint16_t singlePanelRainbow(struct CRGB *leds, singlePanelRainbowParams_t *params)
{
    long rand_num;
    hsv2rgb_rainbow(params->active_hsv_value, params->active_rgb_value);
    params->active_hsv_value.h--;

    leds[params->active_panel] = params->active_rgb_value;

    if (params->active_rgb_value.r >= backgroundRgb.r
            && params->active_rgb_value.g == 0
            && params->active_rgb_value.b == 0) {
        if (params->rand) {
            params->active_hsv_value.h = 255;
            // seed with analog noise to (hopefully) get non-repeating numbers
            randomSeed(analogRead(0) + analogRead(1) + analogRead(2));
            rand_num = random(0,params->num_panels);
            params->active_panel = rand_num;
        } else {
            if (params->increment) {
                params->active_hsv_value.h = 255;
                params->active_panel++;
            } else {
                params->active_hsv_value.h = 255;
                params->active_panel--;
            }
        }
    }
    return params->update_delay;
}

/*!
 * Set the parameters for the sparkle pattern
 *
 * See dataStructures.h for parameter meanings
 */
void setSparkleParams(sparkleParams_t * params,
        uint8_t     num_panels,
        uint8_t     active_panel,
        uint16_t    update_delay
)
{
    params->num_panels = num_panels;
    params->active_panel = active_panel;
    params->update_delay = update_delay;
}

/*!
 * Do a random sparkle of white a pixel at a time
 *
 * \param leds struct CRGB * to a list of pixel values
 * \param params sparkleParams_t * to the pattern parameters
 *
 * return time (ms) until the next pattern update
 */
uint16_t sparkle(struct CRGB *leds, sparkleParams_t *params)
{
    long rand_num;
    fill_solid(leds, NUM_LEDS, backgroundRgb);
    leds[params->active_panel] = CRGB::White;
    // repeat random call if it repeats
    do {
        randomSeed(analogRead(0) + analogRead(1) + analogRead(2));
        rand_num = random(0,params->num_panels);
    } while (rand_num == params->active_panel);
    params->prev_panel = params->active_panel;
    params->active_panel = rand_num;
    return params->update_delay;
}

/*!
 * Set the spiral pattern parameters.
 *
 * See dataStructures.h for pattern meanings
 */
void setSpiralParams(spiralParams_t *params,
        uint8_t num_panels,
        uint8_t active_panel,
        uint16_t update_delay,
        bool increment
)
{
    params->num_panels = num_panels;
    params->active_panel = active_panel;
    params->update_delay = update_delay;
    params->increment = increment;
}

/*!
 * Do a spiral in or out.  Out turns off panels, in turns them on
 *
 * \param leds struct CRGB * to the list of pixel values
 * \param params spiralParams_t * to the pattern parameters
 *
 * \return time (ms) until the next pattern update
 */
uint16_t spiral(struct CRGB *leds, spiralParams_t *params)
{
    if (params->increment) {
        leds[params->active_panel] = backgroundRgb;
        if (params->active_panel == params->num_panels) {
            params->increment = false;
        } else {
            params->active_panel++;
        }
    } else {
        leds[params->active_panel] = CRGB::Black;
        if (params->active_panel == 0) {
            params->increment = true;
        } else {
            params->active_panel--;
        }
    }

    return params->update_delay;
}


void initializePatterns(void)
{
    // initialize each pattern's parameters
    setFadeParams(&fade_params, 10, 30, backgroundRgb, 192, false);
    setSinglePanelRainbowParams(&spr_params, 10, 0, backgroundRgb, tmpHsv, true, 5, true);
    setSparkleParams(&sparkle_params, 10, random(0,10), 100);
    setSpiralParams(&spiral_params, 10, 0, 500, false);

    // initalize LEDs to red
    fill_solid(leds, NUM_LEDS, backgroundRgb);

    // set the next pattern update to now
    next_update = millis();
}


void setup() {

    // sanity check delay - allows reprogramming if accidently blowing power w/leds
    delay(2000);

    Serial.begin(9600);

    pinMode(ENCODER_ACTIVE_PIN, INPUT); 

//  FastLED.addLeds<DMXSIMPLE, DATA_PIN, RGB>(leds, NUM_LEDS);
//    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // white pixel strips
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // white pixel strips, GRB

    initializePatterns();
}


uint8_t h = 0;
uint8_t s = 0;
uint8_t v = 0;

// set the pattern
// 1 - fade
// 2 - single panel rainbow
// 3 - sparkle
// 4 - spiral
uint8_t pattern_num = 2; //4;


void loop() {

    static bool lastEncoderActive = false;
  
    bool encoderActive = digitalRead(ENCODER_ACTIVE_PIN);
  
    if (!encoderActive)
    {
        if (lastEncoderActive) {
            lastEncoderActive = false;
            initializePatterns();
        }  
        // update if enough time has elapsed
        if (millis() >= next_update) {
            switch (pattern_num) {
                case 1:
                    next_update += fade(leds, &fade_params);
                    break;
                case 2:
                    next_update += singlePanelRainbow(leds, &spr_params);
                    break;
                case 3:
                    next_update += sparkle(leds, &sparkle_params);
                    break;
                case 4:
                    next_update += spiral(leds, &spiral_params);
                    break;
                default:
                    break;
            }
    
            FastLED.show();
        }
        return;
    }

    if (!lastEncoderActive) {
        lastEncoderActive = true;
        fill_solid(leds, NUM_LEDS, CRGB::Green);
        FastLED.show();
    }

    if (Serial.available() > 0) {
        leds[4] = CRGB::Blue;
        FastLED.show();
        int sof = Serial.parseInt();
        if (sof == 999) {
            leds[5] = CRGB::Blue;
            FastLED.show();
            uint8_t encoderNum = Serial.parseInt();
            leds[6] = CRGB::Blue;
            FastLED.show();
            int rpm = Serial.parseInt();
            leds[7] = CRGB::Blue;
            FastLED.show();
    //        if (Serial.read() == '\n' && encoderNum < 3) {
            if (encoderNum < 3) {
                leds[8] = CRGB::Blue;
                uint8_t i = rpm;
                CRGB testRgb(g_gamma[i], 0, 0);
                leds[encoderNum] = testRgb;
                FastLED.show();
            }
        }
        Serial.flush();
        leds[4] = leds[5] = leds[6] = leds[7] = leds[8] = CRGB::Black;
        FastLED.show();
    }

}

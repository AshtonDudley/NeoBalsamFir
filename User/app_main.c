#include "main.h"
#include "app_main.h"
#include "math.h"
#include "ARGB.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_tim1_ch1;

#define FRAME_DELAY_MS 50 

#define STAR_PIXEL 14

typedef enum {
    SHOW_OFF            =   0,
    SHOW_BREATHING      =   1,
    SHOW_INTENSE        =   2,
    SHOW_RAINBOW        =   3,
    SHOW_CANDLE         =   4,
    SHOW_TWINKLE_STARS  =   5,
    NUM_OF_SHOWS    
} t_ShowType;

t_ShowType currentShow = 0;

bool buttonPressed = 0;


void app_init(){
    return;
}

// Utility function to make a simple "oscillating" brightness.
// t is a time/frame counter, period controls how fast it cycles,
// minVal and maxVal control the brightness range.
static uint8_t oscillateBrightness(float t, float period, uint8_t minVal, uint8_t maxVal) {
    // Create a normalized sine wave between minVal and maxVal
    float angle = (2.0f * M_PI * t) / period;
    float sineVal = (sin(angle) + 1.0f) / 2.0f; // map sine(-1..1) to (0..1)
    float range = (float)(maxVal - minVal);
    return (uint8_t)(minVal + sineVal * range);
}

void show_off (uint32_t frame){
    (void)frame;
    ARGB_FillRGB(0,0,0);
    ARGB_FillHSV(0,0,0);
    ARGB_FillWhite(0);
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET); // Disable Indicator LED
}

void show_breathing (uint32_t frame){
    float t = (float)frame;

    // Star at LED 14
    uint8_t starVal = oscillateBrightness(t, 100.0f, 50, 255); 
    ARGB_SetHSV(STAR_PIXEL, 43, 255, starVal);

    // Ornaments (0-13)
    for (uint16_t i = 0; i < NUM_PIXELS - 1; i++) {
        uint8_t hue = (i % 2 == 0) ? 0 : 85; // Red/Green
        uint8_t sat = 255;
        float offset = i * 7.0f;
        uint8_t val = oscillateBrightness(t + offset, 200.0f, 20, 220);
        ARGB_SetHSV(i, hue, sat, val);
    }
}

void show_intense (uint32_t frame){
    float t = (float)frame;

    // Star at LED 14: faster twinkle
    uint8_t starVal = oscillateBrightness(t, 60.0f, 0, 255);
    ARGB_SetHSV(STAR_PIXEL, 43, 255, starVal);

    // Ornaments (0-13)
    for (uint16_t i = 0; i < NUM_PIXELS - 1; i++) {
        uint8_t hue = (i % 2 == 0) ? 0 : 85; // Red/Green
        uint8_t sat = 255;
        float offset = i * 5.0f; 
        uint8_t val = oscillateBrightness(t + offset, 120.0f, 0, 255);
        ARGB_SetHSV(i, hue, sat, val);
    }
}

void show_rainbow_fade(uint32_t frame) {
    // The HSV hue is typically 0-255 for a full cycle of colors.
    // Let's pick a base hue that shifts over time, causing the rainbow to move.
    uint8_t baseHue = (uint8_t)(frame % 256); 

    // Iterate over each LED and assign a hue offset based on its index.
    for (uint16_t i = 0; i < NUM_PIXELS; i++) {
        // Spread the rainbow evenly across all LEDs.
        // For a continuous rainbow, let's map the LED index so that 
        // the entire strip covers 256 hue steps. 
        // If NUM_LEDS is small, this may repeat colors, but it still creates a gradient.
        uint8_t hueOffset = (uint8_t)((256UL * i) / NUM_PIXELS);
        uint8_t hue = (uint8_t)(baseHue + hueOffset);
        uint8_t saturation = 255;
        uint8_t value = 255;

        ARGB_SetHSV(i, hue, saturation, value);
    }
}

// Flickering Candle Effect
// Adjust ranges as needed for desired look.
void show_candle(uint32_t frame) {
    // Base hue for a warm flame: around orange (~43°)
    uint8_t hue = 43;
    for (uint16_t i = 0; i < NUM_PIXELS; i++) {
        // Generate random saturation and value
        // rand() % (max-min+1) + min gives a random number in [min, max].
        uint8_t sat_min = 180;
        uint8_t sat_max = 255;
        uint8_t val_min = 40;
        uint8_t val_max = 120;

        uint8_t sat = (uint8_t)(rand() % (sat_max - sat_min + 1) + sat_min);
        uint8_t val = (uint8_t)(rand() % (val_max - val_min + 1) + val_min);

        ARGB_SetHSV(i, hue, sat, val);
    }
}

void show_twinkle_stars(uint32_t frame) {
    static uint8_t starBrightness[NUM_PIXELS];

    uint8_t bgHue = 160;   // Blue-ish hue
    uint8_t bgSat = 255;
    uint8_t bgVal = 20;    // Dim background
    
    // Probability of twinkle each frame: about 1 in 30
    int twinkleChance = 30;
    // Randomly pick a star to twinkle sometimes
    if ((rand() % twinkleChance) == 0) {
        uint16_t i = rand() % NUM_PIXELS;
        // Only twinkle if it's currently dim to avoid overlapping twinkles
        if (starBrightness[i] < 10) {
            starBrightness[i] = 255; // Bright white twinkle
        }
    }
    
    // Update star brightness for each LED
    // Fade out any twinkling stars
    for (uint16_t i = 0; i < NUM_PIXELS; i++) {
        if (starBrightness[i] > 0) {
            // Fade out star: reduce brightness by ~10 each frame
            // Adjust fade speed as desired
            int newVal = (int)starBrightness[i] - 10;
            starBrightness[i] = (uint8_t)((newVal < 0) ? 0 : newVal);
        }
    }
    
    // Set each LED color
    // Combine background and star brightness:
    // The star is white, so we can blend by using starBrightness as the final V 
    // in HSV with hue/sat for white (sat=0 for pure white).
    // Alternatively, just set it to white if starBrightness is higher than background.
    for (uint16_t i = 0; i < NUM_PIXELS; i++) {
        if (starBrightness[i] > bgVal) {
            // Star dominates, show white
            // For white, saturation=0
            ARGB_SetHSV(i, 0, 0, starBrightness[i]);
        } else {
            // Just show background color
            ARGB_SetHSV(i, bgHue, bgSat, bgVal);
        }
    }
}

void app_main(){
    
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

    ARGB_Init();  // Initialization
    ARGB_SetBrightness(128); // Set a moderate global brightness (0-255)
    ARGB_Clear();
    ARGB_Show();
    uint32_t frame = 0;
    for(;;){
       // Wait for strip to be ready
        while (ARGB_Ready() != ARGB_READY) { }

        switch (currentShow) {
            case SHOW_OFF:
                show_off(frame);
                break;
            case SHOW_BREATHING:
                show_breathing(frame);
                break;
            case SHOW_INTENSE:
                show_intense(frame);
                break;
            case SHOW_RAINBOW:
                show_rainbow_fade(frame);
                break;
            case SHOW_CANDLE:
                show_candle(frame);
                break;
            case SHOW_TWINKLE_STARS:
                show_twinkle_stars(frame);
                break;
            default:
                break;
        }

        if (buttonPressed) {
            buttonPressed = false;
            currentShow = (currentShow + 1) % NUM_OF_SHOWS;
        }

        ARGB_Show();
        HAL_Delay(FRAME_DELAY_MS);
        frame++;
        
    }
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == USER_BUTTON_Pin){
        buttonPressed = true;
    }
}

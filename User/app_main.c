#include "main.h"
#include "app_main.h"
#include "math.h"
#include "ARGB.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_tim1_ch1;

#define FRAME_DELAY_MS 50 

uint8_t get_sine_brightness(float freq){
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
    static uint32_t lastTick = 0 ;
    uint32_t currentTick = HAL_GetTick();
    

    int period = 1 / freq * 1000.0f;
    float elapsedTime = (currentTick - lastTick) / 1000.0f;
    if (elapsedTime >=  period / 1000.0f) {
        lastTick = currentTick;
        elapsedTime = 0.0f;
    }

    float sineValue = sinf(2 * M_PI * freq * elapsedTime);
    // Map sine wave from [-1, 1] to [0, 255]
    uint8_t ledBrightness = (uint8_t)((sineValue + 1.00f) * 127.5f);    // 127.5f
    
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

    return ledBrightness;
}

void global_brightness_sweep(){
    uint8_t ledBrightness = get_sine_brightness(0.2f);
    ARGB_SetBrightness(ledBrightness);
    ARGB_FillRGB(20, 0, 0); // Fill all the strip with Red
}

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

void app_main(){

    ARGB_Init();  // Initialization
    ARGB_SetBrightness(128); // Set a moderate global brightness (0-255)
    ARGB_Clear();
    ARGB_Show();
    uint32_t frame = 0;
    for(;;){
       // Wait for strip to be ready
        while (ARGB_Ready() != ARGB_READY) { }

        float t = (float)frame;

        // The star is at LED 14 (the 15th pixel)
        // Let's give it a twinkling warm glow
        uint8_t starVal = oscillateBrightness(t, 60.0f, 50, 255); 
        // Use a warm hue (e.g., Hue = 43°, full saturation, variable brightness)
        ARGB_SetHSV(14, 43, 255, starVal);

        // The ornaments: LEDs 0 to 13
        // Alternate red/green and give them a breathing effect
        for (uint16_t i = 0; i < NUM_PIXELS - 1; i++) {
            uint8_t hue = (i % 2 == 0) ? 0 : 85;  // Red or Green
            uint8_t sat = 255;

            // Adjust offset for variation
            float offset = i * 5.0f; 
            uint8_t val = oscillateBrightness(t + offset, 120.0f, 0, 255);

            ARGB_SetHSV(i, hue, sat, val);
        }

        ARGB_Show();

        HAL_Delay(FRAME_DELAY_MS);
        frame++;
    }
}


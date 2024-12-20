#include "main.h"
#include "app_main.h"
#include "math.h"
#include "ARGB.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_tim1_ch1;


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

void app_init(){
    return;
}

void app_main(){

    ARGB_Init();  // Initialization

    // ARGB_Clear(); // Clear stirp
    // while (ARGB_Show() != ARGB_OK); // Update - Option 1

    // ARGB_SetBrightness(100);  // Set global brightness to 40%

    // ARGB_SetRGB(2, 0, 255, 0); // Set LED №3 with 255 Green
    // while (!ARGB_Show());  // Update - Option 2

    // ARGB_SetHSV(0, 0, 255, 255); // Set LED №1 with Red
    // while (!ARGB_Ready()); // Update - Option 3
    // ARGB_Show();

    // ARGB_FillWhite(230); // Fill all white component with 230
    // while (ARGB_Ready() == ARGB_BUSY); // Update - Option 4
    // ARGB_Show();

    // while (!ARGB_Show());
    ARGB_SetBrightness(50);
    for(;;){
        uint8_t ledBrightness = get_sine_brightness(0.5f);
        ARGB_SetBrightness(ledBrightness);
        ARGB_FillRGB(20, 0, 0); // Fill all the strip with Red
        while (ARGB_Show() != ARGB_OK);
    }
}


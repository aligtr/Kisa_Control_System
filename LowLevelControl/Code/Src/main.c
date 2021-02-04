#include "main.h"

static void MX_GPIO_Init(void);

void vLedTask(void *pvParameters)
{
  while (1)
  {
    GPIOA->BSRR = GPIO_BSRR_BS_0;
    vTaskDelay(500);
    GPIOA->BSRR = GPIO_BSRR_BR_0;
    vTaskDelay(500);
  }
  vTaskDelete(NULL);
}

int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  GPIOA->MODER |= GPIO_MODER_MODER0_0;
  xTaskCreate(vLedTask, (signed char *)"LedTask", configMINIMAL_STACK_SIZE,NULL, tskIDLE_PRIORITY + 1, NULL);
  vTaskStartScheduler();
  while (1)
  {


  }
}

static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

#include "main.h"


static void MX_GPIO_Init(void);

int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  
  while (1)
  {

  }

}


static void MX_GPIO_Init(void)
{

  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

void Error_Handler(void)
{

}

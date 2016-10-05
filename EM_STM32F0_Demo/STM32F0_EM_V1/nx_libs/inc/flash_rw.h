#ifndef FLASH_RW_H
#define FLASH_RW_H

#include "main.h"

/* USER CODE BEGIN Includes */
/** @addtogroup STM32F0xx_HAL_Examples
  * @{
  */

/** @addtogroup FLASH_WriteProtection
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* Private define ------------------------------------------------------------*/

/* Uncomment this line to Enable Write Protection */
//#define WRITE_PROTECTION_ENABLE

/* Uncomment this line to Disable Write Protection */
//#define WRITE_PROTECTION_DISABLE

#define FLASH_USER_START_ADDR       ADDR_FLASH_PAGE_24   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR         ADDR_FLASH_PAGE_28   /* End @ of user Flash area */
#define FLASH_PAGE_TO_BE_PROTECTED (OB_WRP_PAGES20TO23 | OB_WRP_PAGES24TO27)  



/* Check the status of the switches */
/* Enable by default the disable protection */
#if !defined(WRITE_PROTECTION_ENABLE)&&!defined(WRITE_PROTECTION_DISABLE)
#define WRITE_PROTECTION_DISABLE
#endif /* !WRITE_PROTECTION_ENABLE && !WRITE_PROTECTION_DISABLE */

/* Both switches cannot be enabled in the same time */
#if defined(WRITE_PROTECTION_ENABLE)&&defined(WRITE_PROTECTION_DISABLE)
#error "Switches WRITE_PROTECTION_ENABLE & WRITE_PROTECTION_DISABLE cannot be enabled in the time!"
#endif /* WRITE_PROTECTION_ENABLE && WRITE_PROTECTION_DISABLE */



int8_t flash_write(uint32_t address, uint32_t data);
void flash_transfer_start(uint32_t val);

#endif 
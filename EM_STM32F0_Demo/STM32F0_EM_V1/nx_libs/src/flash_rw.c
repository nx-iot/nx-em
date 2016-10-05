#include "flash_rw.h"

#define DATA_32                     ((uint32_t)0x12345678)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t Address = 0;
uint32_t PageError = 0;
__IO TestStatus MemoryProgramStatus = PASSED;
/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;
/*Variable used to handle the Options Bytes*/
static FLASH_OBProgramInitTypeDef OptionsBytesStruct;
/* USER CODE END Includes */

int8_t flash_write(uint32_t address, uint32_t data){
	int8_t ret = 0;

	flash_transfer_start(data);
	return ret;
}

void flash_transfer_start(uint32_t val){
	/* Initialize test status */
  MemoryProgramStatus = PASSED;
  
  /* Unlock the Flash to enable the flash control register access *************/ 
  HAL_FLASH_Unlock();

  /* Unlock the Options Bytes *************************************************/
  HAL_FLASH_OB_Unlock();

  /* Get pages write protection status ****************************************/
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

#ifdef WRITE_PROTECTION_DISABLE
  /* Check if desired pages are already write protected ***********************/
  if((OptionsBytesStruct.WRPPage & FLASH_PAGE_TO_BE_PROTECTED) != FLASH_PAGE_TO_BE_PROTECTED)
  {
    /* Restore write protected pages */
    OptionsBytesStruct.OptionType   = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPState     = OB_WRPSTATE_DISABLE;
    OptionsBytesStruct.WRPPage = FLASH_PAGE_TO_BE_PROTECTED;
    if(HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
    {
      /* Error occurred while options bytes programming. **********************/
			print_debug(0,"Error occurred while options bytes programming.\n");
      while (1)
      {
        //BSP_LED_On(LED5);
      }
    }

    /* Generate System Reset to load the new option byte values ***************/
    HAL_FLASH_OB_Launch();
  }
#elif defined WRITE_PROTECTION_ENABLE
  /* Check if desired pages are not yet write protected ***********************/
  if(((~OptionsBytesStruct.WRPPage) & FLASH_PAGE_TO_BE_PROTECTED )!= FLASH_PAGE_TO_BE_PROTECTED)
  {
    /* Enable the pages write protection **************************************/
    OptionsBytesStruct.OptionType = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPState   = OB_WRPSTATE_ENABLE;
    OptionsBytesStruct.WRPPage    = FLASH_PAGE_TO_BE_PROTECTED;
    if(HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
    {
      /* Error occurred while options bytes programming. **********************/
			print_debug(0,"Error occurred while options bytes programming.\n");
      while (1)
      {
        //BSP_LED_On(LED5);
      }
    }

    /* Generate System Reset to load the new option byte values ***************/
    HAL_FLASH_OB_Launch();
  }
#endif /* WRITE_PROTECTION_DISABLE */

  /* Lock the Options Bytes *************************************************/
  HAL_FLASH_OB_Lock();

  /* The selected pages are not write protected *******************************/
  if ((OptionsBytesStruct.WRPPage & FLASH_PAGE_TO_BE_PROTECTED) != 0x00)
  {
    /* Fill EraseInit structure************************************************/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
    EraseInitStruct.NbPages     = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR)/FLASH_PAGE_SIZE;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
      /* 
        Error occurred while page erase. 
        User can add here some code to deal with this error. 
        PageError will contain the faulty page and then to know the code error on this page,
        user can call function 'HAL_FLASH_GetError()'
      */
			print_debug(0,"Error occurred while page erase.\n");
      while (1)
      {
        //BSP_LED_On(LED5);
      }
    }

    /* FLASH Word program of DATA_32 at addresses defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */
    Address = FLASH_USER_START_ADDR;
    while (Address < FLASH_USER_END_ADDR)
    {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, val) == HAL_OK)
      {
        Address = Address + 4;
      }
      else
      {
        /* Error occurred while writing data in Flash memory. 
           User can add here some code to deal with this error */
				print_debug(0,"Error occurred while writing data in Flash memory.\n");
        while (1)
        {
          //BSP_LED_On(LED5);
        }
      }
    }

    /* Check the correctness of written data */
    Address = FLASH_USER_START_ADDR;

    while (Address < FLASH_USER_END_ADDR)
    {
      if((*(__IO uint32_t*) Address) != val)
      {
        MemoryProgramStatus = FAILED;
      }
      Address += 4;
    }
  }
  else
  { 
    /* The desired pages are write protected */ 
    /* Check that it is not allowed to write in this page */
    Address = FLASH_USER_START_ADDR;
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, val) != HAL_OK)
    {
      /* Error returned during programmation. */
      /* Check that WRPERR flag is well set */
      if (HAL_FLASH_GetError() == HAL_FLASH_ERROR_WRP) 
      {
        MemoryProgramStatus = FAILED;
      }
      else
      {
        /* Another error occurred.
           User can add here some code to deal with this error */
				print_debug(0,"Another error occurred.\n");
        while (1)
        {
					
          //BSP_LED_On(LED5);
        }
      }
    }
    else
    {
      /* Write operation is successful. Should not occur
         User can add here some code to deal with this error */
			print_debug(0,"Write operation is successful.\n");
      while (1)
      {
				
        //BSP_LED_On(LED5);
      }
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /*Check if there is an issue to program data*/
  if (MemoryProgramStatus == PASSED)
  {
    /* No error detected. Switch on LED3*/
		print_debug(0,"No error detected\n");
    //BSP_LED_On(LED3);
  }
  else
  {
    /* Error detected. Switch on LED4*/
		print_debug(0,"Error detected\n");
    //BSP_LED_On(LED4);
  }
	
	
}
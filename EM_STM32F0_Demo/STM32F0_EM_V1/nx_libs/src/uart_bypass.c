#include "uart_bypass.h"
#include "uart1_manag.h"
#include "uart2_manag.h"

extern recv_uart_t recv_uart_1,recv_uart_2;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
data_t data_uart_bypass;


int8_t uart_bypass(){
	int8_t ret = 0;
	
	ret = uart1_read(data_uart_bypass.value,&data_uart_bypass.length,255);
	if(ret){
		if(data_uart_bypass.length > 0){
			print_debug(0,"[INFO]{uart_bypass}uart1 interrupt.\n");
			print_payload(0,data_uart_bypass.value,data_uart_bypass.length);
		}
	}
	
	ret = uart2_read(data_uart_bypass.value,&data_uart_bypass.length,255);
	if(ret){
		if(data_uart_bypass.length > 0){
			print_debug(0,"[INFO]{uart_bypass}uart2 interrupt.\n");
			print_payload(0,data_uart_bypass.value,data_uart_bypass.length);
			uart2_write(data_uart_bypass.value,data_uart_bypass.length);
		}
	}
	return ret;
}
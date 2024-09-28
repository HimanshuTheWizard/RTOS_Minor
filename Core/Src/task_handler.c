/*
 * task_handler.c
 *
 *  Created on: Sep 28, 2024
 *      Author: himanshu
 */

#include "main.h"

void Menu_Task_Handler(void *param)
{
	BaseType_t status;
	uint32_t cmd_addr;
	command_t *cmd;
	uint8_t val;
	const char *print_msg = "\n=======menu======\r\n"
							"LED Effect         :0\r\n"
							"Date and Time      :1\r\n"
							"Exit               :2\r\n"
							"Enter Your Choice  :";
	const char *invalid_msg = "Invalid Input\n";
	while(1)
	{
		xQueueSend(print_queue, &print_msg, 0);
		status = xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
		cmd = (command_t *)cmd_addr;
		if(status == pdTRUE)
		{
			if(cmd->len == 1)
			{
				val = cmd->value[0] - 48;
				switch(val)
				{
					case 0:
						e_state = led_effect;
						xTaskNotify(LED_Task_Handle, 0, eNoAction);
						break;
					case 1:
						e_state = rtc_report;
						xTaskNotify(RTC_Task_Handle, 0, eNoAction);
						break;
					case 2:
						break;
					default:
						xQueueSend(print_queue,(void *)&invalid_msg, 0);
						continue;
				}
			}
			else
			{
				xQueueSend(print_queue,(void *)&invalid_msg, 0);
			}
		}
	}
}
void LED_Task_Handler(void *param)
{
	while(1)
	{

	}
}
void RTC_Task_Handler(void *param)
{
	while(1)
	{

	}
}
void Print_Task_Handler(void *param)
{
	uint32_t *msg;
	while(1)
	{
		xQueueReceive(print_queue, &msg, portMAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen((char *)msg), HAL_MAX_DELAY);
	}
}
void Command_Task_Handler(void *param)
{
	command_t cmd;
	BaseType_t status;
	while(1)
	{
		status = xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
		if(status == pdTRUE)
		{
			Process_User_Cmd(&cmd);
		}
	}
}
int Extract_Cmd(command_t *cmd_ptr)
{
	uint8_t item;
	uint8_t i = 0;
	BaseType_t status;
	//check if queue is empty
	//if empty return 1 else return 0

	while(item != '\r')
	{
		status = xQueueReceive(input_data_queue ,(void *)&item, 0);
		if(status == pdTRUE)
		{
			cmd_ptr->value[i++] = item;
		}
	}
	cmd_ptr->value[i-1] = '\0';
	cmd_ptr->len = i-1;
	return 0;
}

void Process_User_Cmd(command_t *cmd_ptr)
{
	Extract_Cmd(cmd_ptr);
	switch(e_state)
	{
		case main_menu:
			xTaskNotify(Menu_Task_Handle, (uint32_t)cmd_ptr, eSetValueWithOverwrite);
			break;
		case led_effect:
			xTaskNotify(LED_Task_Handle, (uint32_t)cmd_ptr, eSetValueWithOverwrite);
			break;
		case rtc_menu:
			break;
		case rtc_time_config:
			break;
		case rtc_date_config:
			break;
		case rtc_report:
			xTaskNotify(RTC_Task_Handle, (uint32_t)cmd_ptr, eSetValueWithOverwrite);
			break;
	}
}


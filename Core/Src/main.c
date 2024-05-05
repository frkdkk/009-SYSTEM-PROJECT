/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fatfs_sd.h"
#include "stdio.h"
#include "string.h"
#include "time.h"
#include "math.h"
#include "stdlib.h"
#define CT 50
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
SPI_HandleTypeDef hspi1;
FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
uint32_t totalSpace, freeSpace;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

DAC_HandleTypeDef hdac;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DAC_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_UART4_Init(void);
static void MX_TIM8_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

	 /*
	 * SD karta kaydederken veriler aşağı doğru yazdırılıyor onları yan yana yazdıracaksın.
	 *USART2 NEXTION HMI DISPLAY
	 *USART3 SD CARD LOGS
	 *USART4 TIME SPEND
	 *12-bit ADC, the digital output value will be between 0-4095.
	 *�?u an 480 cycle dönüyor ve 4000000 örnek alıyor. Bunun örnek sayısını ve cycle sayısını düşür.
	 *TIME IT diye bir fonksiyon var iki kod arasıdna geçen süreyi hesaplıyır.
	 *Time sequence of adc, adc'nin bir çevrimde geçtiği süre
	 *
	 *Uint32_t ile int arası değiştirme yap.
	 */


int cont=118, sec1=0, sec8=0;;
uint32_t adc_Buffer[12];
uint32_t adc1_Value[12];
uint32_t adc1_Square[12];
uint32_t adc1_Square_Root[12];
uint32_t adc1_Add[12];
uint32_t adc1_Mean[12];
uint32_t k=0,count=0, sd_Flag=0;
uint32_t I1L,I2L,I3L,V1L,V2L,V3L,V12L,V23L,V13L,I12L,I23L,I13L,Ifb,Vfb;
uint32_t bara_Arti,bara_Eksi,Vout,t_sample,akim,Iout,Vin,Iin,temp,bara_Toplam;
int iMean=0, iV1L=0, iV2L=0, iV3L=0, iI1L=0, iI2L=0, iI3L=0, ibara_Arti=0, ibara_Eksi=0, iVout=0, itemp=0, iIout=0, iVin=0, iIin=0;
uint8_t RX_Data[4];
uint8_t end_Command[3] = {0xFF, 0xFF, 0xFF};
uint32_t dac_V=0, dac_I=0,send_V=0,send_I=0;
char adc1_Last0[20], adc1_Last1[20], adc1_Last2[20], adc1_Last3[20], adc1_Last4[20], adc1_Last5[20];
char adc1_Last6[20], adc1_Last7[20], adc1_Last8[20], adc1_Last9[20], adc1_Last1[20], adc1_Last10[20], adc1_Last11[20];
char nextion_Buffer[50];
char buffer[100];

//SD karta yazmada kullanılan transmit_uart'ın fonskiyonu.
void transmit_uart(char *string)
{
     uint8_t len = strlen(string);
     HAL_UART_Transmit(&huart3, (uint8_t*) string, len, 200);
}



//Nextion'a değer göndermede kullanılan fonksiyon.
void nextion_Send(char *obj, uint32_t num ) //obj parametresine num değeri gönderilir.
{


	uint8_t *nextion_Buffer = malloc(30*sizeof(char)); //buffer için bellek tahsis edilir.
	int len = sprintf((char *)nextion_Buffer, "%s.val=%ld", obj, num); //nextion ekranına gönderilecek komut
	HAL_UART_Transmit(&huart2, nextion_Buffer, len, 1000);
	HAL_UART_Transmit(&huart2, end_Command, 3, 100);
	free(nextion_Buffer); //bellek serbest bırakılır.


}

//Bu fonksiyon belirli bir değer aralığını başka bir değer aralığına dönüştürür.
uint32_t map(uint32_t min1, uint32_t max1, uint32_t min2, uint32_t max2, uint32_t width)
{
	return (min1*width)/min2;
}

//DAC ile alınan verinin gönderimi
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	 if (huart == &huart2)
	 {

		 if(RX_Data[1] == 0x31)
		 {
		 nextion_Send("n2", Vin);
		 nextion_Send("n3", Iin);
		 nextion_Send("n4", 666);
		 nextion_Send("n5", 777);
		 }

		 else if(RX_Data[1] == 0x32)
		 {
		 nextion_Send("n6", Vout);
		 nextion_Send("n7", Iout);
		 nextion_Send("n8", 666);
		 nextion_Send("n9", 777);
		 }

		 else if(RX_Data[1] == 0x33)
		 {
		 nextion_Send("n10", V1L);
		 nextion_Send("n11", I1L);
		 nextion_Send("n12", V2L);
		 nextion_Send("n13", I2L);
		 nextion_Send("n14", V3L);
		 nextion_Send("n15", I3L);
		 }

		 else if(RX_Data[1] == 0x34)
		 {
		 nextion_Send("n16", V12L);
		 nextion_Send("n17", I12L);
		 nextion_Send("n18", V23L);
		 nextion_Send("n19", I23L);
		 nextion_Send("n20", V13L);
		 nextion_Send("n21", I13L);
		 }

		 else if(RX_Data[1] == 0x35)
		 {
		 nextion_Send("n22", temp);
		 }

		 else
		 {
		 nextion_Send("n0", send_V);
		 nextion_Send("n1", send_I);
		 }



		if(RX_Data[2] == 0x03)
		{
			dac_V -= 0.5;
			if(dac_V == 20) dac_V = 20;
			send_V = map(dac_V,0,20,0,1275);
		}

		if(RX_Data[2] == 0x04)
		{
			dac_V += 0.5;
			if(dac_V == 0) dac_V = 0;
			send_V = map(dac_V,0,20,0,1275);
		}

		if(RX_Data[2] == 0x05)
		{
			dac_I -= 50;
			if(dac_I == 3000) dac_I = 3000;
			send_I = map(dac_I,0,3000,0,1275);
		}
		if(RX_Data[2] == 0x06)
		{
			dac_I += 50;
			if(dac_I == 0) dac_I = 0;
			send_I = map(dac_I,0,3000,0,1275);
		}


		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R,send_V);
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R,send_I);

	   HAL_UART_Receive_IT(&huart2, RX_Data, 4);
	 }



}







//Timer1 50us'de bir kesmeye girdi ve ADC1 bu kısımda örnek almaya başladı.

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{


while(count<1600)
	{

	switch(k)
		{

			case 0: //REF
				adc1_Value[k] += adc_Buffer[k];
				k++;
				break;


			case 1: //VL1
				adc1_Value[k] += adc_Buffer[k];
				k++;
				break;

			case 2: //VL2
				adc1_Value[k] += adc_Buffer[k];
				k++;
				break;

			case 3: //VL3
				adc1_Value[k] += adc_Buffer[k];
				k++;
				break;

			case 4: //IL1
				adc1_Value[k] += adc_Buffer[k];
				k++;
				break;

			case 5: //IL2
				adc1_Value[k] += adc_Buffer[k];
				k++;
				break;

			case 6: //IL3
				adc1_Value[k] += adc_Buffer[k];
				k++;
				break;

			case 7: //BARA+
				adc1_Value[k] += adc_Buffer[k];
				k++;
				break;

			case 8: //BARA-
				adc1_Value[k] += adc_Buffer[k];
				k++;
				break;

			case 9: //FB VOLTAGE
				adc1_Value[k] += adc_Buffer[k];
				k++;
				break;

			case 10: //T_SAMPLE
				adc1_Value[k] += adc_Buffer[k];
				k++;
				break;

			case 11: //AKIM
				adc1_Value[k] += adc_Buffer[k];
				count++;
				k=0;
				break;

		}




		if(count==1600)
		{

				adc1_Mean[0] = adc1_Value[0] / 1600;

			for(uint8_t t=1;t<7;t++)
				{
				//RMS Değer Hesabı
				adc1_Mean[t] = adc1_Value[t] / 1600; //Alınan değerler toplamı toplam sayıya bölündü ve ort değer elde edildi.
				adc1_Square[t] = (adc1_Mean[t]-adc1_Mean[0]) * (adc1_Mean[t]-adc1_Mean[0]); //Ortalama değerin karesi alındı.
				adc1_Square_Root[t] = 331 * sqrt(adc1_Square[t]  * 0.707 / 868.8); //Karesi alınan değerlerin karekökü alındı. 331:Gerilim Bölücü  0.707:RMS Hesabı(sqrt(2)/2)  868.8:Kalibrasyon Çarpanı
				}


				V1L=adc1_Square_Root[1];  //V1L
				V2L=adc1_Square_Root[2];  //V2L
				V3L=adc1_Square_Root[3];  //V3L

				I1L = CT * (330 * 3.3 / 4095 / 55 * sqrt(adc1_Square[4])); // IL1  330,55:Direnç ve kalibrasyon çarpanı
				I2L = CT * (330 * 3.3 / 4095 / 55 * sqrt(adc1_Square[5])); // IL2
				I3L = CT * (330 * 3.3 / 4095 / 55 * sqrt(adc1_Square[6])); //IL3

				Vin = (V1L+V2L+V3L) / 3;
				Iin = (I1L+I2L+I3L) / 3;

				for(uint8_t j=7;j<12;j++) adc1_Mean[j] = adc1_Value[j] / 1600;

				bara_Arti=adc1_Mean[7] * 3.3 / 4096; //bara+
				bara_Eksi=adc1_Mean[8] * 3.3 / 4096; //bara-
				Vout=adc1_Mean[9] * 3.3 / 4096 * 0.9615; // Vout
				t_sample=adc1_Mean[10] * 3.3 / 4096; //s_sample sıcaklık
				akim=adc1_Mean[11] * 3.3 / 4096; //akım
				Iout=map(adc1_Mean[11],180,995,0,3680)-697; // Iout
				if(Iout<0) Iout=0;
				bara_Toplam=bara_Arti-bara_Eksi;

				//Sıcaklık hesabı
				temp = 16.628 * t_sample * t_sample + 535.2484 * t_sample -16.129 ;


				//Faz-Faz arası gerilim ve akımlar
				V12L=sqrt(3)*(adc1_Square_Root[1] + adc1_Square_Root[2] / 2);
				V23L=sqrt(3)*(adc1_Square_Root[2] + adc1_Square_Root[3] / 2);
				V13L=sqrt(3)*(adc1_Square_Root[1] + adc1_Square_Root[3] / 2);
				I12L=sqrt(3)*(adc1_Square_Root[4] + adc1_Square_Root[5] / 2);
				I23L=sqrt(3)*(adc1_Square_Root[5] + adc1_Square_Root[6] / 2);
				I13L=sqrt(3)*(adc1_Square_Root[4] + adc1_Square_Root[6] / 2);


				//Değerler int'e çevriliyor.
				adc1_Mean[0]=(int)adc1_Mean[0];
				V1L = (int)V1L;
				V2L = (int)V2L;
				V3L = (int)V3L;
				I1L = (int)I1L;
				I2L = (int)I2L;
				I3L = (int)I3L;
				bara_Arti = (int)bara_Arti;
				bara_Eksi = (int)bara_Eksi;
				Vout = (int)Vout;
				temp = (int)temp;
				Iout = (int)Iout;
				Vin = (int)Vin;
				Iin = (int)Iin;
				V12L = (int)V12L;
				V23L = (int)V23L;
				V13L = (int)V13L;
				I12L = (int)I12L;
				I23L = (int)I23L;
				I13L = (int)I13L;
				bara_Toplam = (int)bara_Toplam;




				//int değerler char'a çevriliyor
				sprintf(adc1_Last0, "%d", 0); //geçen süre yazması gerekiyor.
				sprintf(adc1_Last1, "%d", adc1_Mean[0]);
				sprintf(adc1_Last2, "%d", V1L);
				sprintf(adc1_Last3, "%d", V2L);
				sprintf(adc1_Last4, "%d", V3L);
				sprintf(adc1_Last5, "%d", I1L);
				sprintf(adc1_Last6, "%d", I2L);
				sprintf(adc1_Last7, "%d", I3L);
				sprintf(adc1_Last8, "%d", bara_Toplam);
				sprintf(adc1_Last9, "%d", Vout);
				sprintf(adc1_Last10, "%d", Iout);
				sprintf(adc1_Last11, "%d", temp);





		}



	}

	sd_Flag=1;


}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  // Zaman ölçümü için DWT(Data Watch Point and Trace)
  /*
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL = DWT_CTRL_CYCCNTENA_Msk;
  */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_DAC_Init();
  MX_SPI1_Init();
  MX_USART3_UART_Init();
  MX_FATFS_Init();
  MX_UART4_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);  // Örnekleme için timer kesmesi başlangıcı USER CODE 4'e git.
  HAL_TIM_Base_Start_IT(&htim8);  // HMI Ekrana her 1s'de yazması için timer kesmesi başlangıcı

  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_2);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if(sd_Flag==1)
	  {
		    fres = f_mount(&fs, "", 0);
			if (fres == FR_OK) {
				transmit_uart("Micro SD card is mounted successfully!\n");
			} else if (fres != FR_OK) {
				transmit_uart("Micro SD card's mount error!\n");
			}

			// FA_OPEN_APPEND opens file if it exists and if not then creates it,
			// the pointer is set at the end of the file for appending
			fres = f_open(&fil, "log-file.txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);
			if (fres == FR_OK) {
				transmit_uart("File opened for reading and checking the free space.\n");
			} else if (fres != FR_OK) {
				transmit_uart("File was not opened for reading and checking the free space!\n");
			}


			 //ADC değerlerinin char'a dönüştürülüp sd karta yazdırıldığı bölüm

				 f_puts(adc1_Last0, &fil);   // ADC değerleri metin dosyasına yazdırıldı.
				 f_puts(" ", &fil);
				 f_puts(adc1_Last1, &fil);
				 f_puts(" ", &fil);
				 f_puts(adc1_Last2, &fil);
				 f_puts(" ", &fil);
				 f_puts(adc1_Last3, &fil);
				 f_puts(" ", &fil);
				 f_puts(adc1_Last4, &fil);
				 f_puts(" ", &fil);
				 f_puts(adc1_Last5, &fil);
				 f_puts(" ", &fil);
				 f_puts(adc1_Last6, &fil);
				 f_puts(" ", &fil);
				 f_puts(adc1_Last7, &fil);
				 f_puts(" ", &fil);
				 f_puts(adc1_Last8, &fil);
				 f_puts(" ", &fil);
				 f_puts(adc1_Last9, &fil);
				 f_puts(" ", &fil);
				 f_puts(adc1_Last10, &fil);
				 f_puts(" ", &fil);
				 f_puts(adc1_Last11, &fil);
				 f_puts("\n", &fil);




			fres = f_close(&fil);
			if (fres == FR_OK) {
				transmit_uart("The file is closed.\n");
			} else if (fres != FR_OK) {
				transmit_uart("The file was not closed.\n");}

			sd_Flag=0;
			count=0;
			k=0;

		for(uint8_t y=0;y<12;y++)
			{
				adc_Buffer[y]=0;
				adc1_Value[y]=0;
				adc1_Mean[y]=0;
				adc1_Square[y]=0;
				adc1_Square_Root[y]=0;
			}
	  }



  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 12;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = 6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = 8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 9;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = 10;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = 11;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = 12;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT2 config
  */
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 2-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 8400-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 20000-1;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 8400-1;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

  /*Configure GPIO pin : REF_Pin */
  GPIO_InitStruct.Pin = REF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(REF_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)  /* TIMER CAGRILDI  */
{
	 if(htim->Instance == TIM1)  // 50US'DE BIR KESMEYE GIRIP DEGER TOPLAYACAK.
	 {
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_Buffer, sizeof(adc_Buffer)); /* ADC1 BASLADI VE DEGERLER ADC1'DEN CEKILDI */
		 sec1++;
	 }

	 if(htim->Instance == TIM8) // 1s'de bir tft ekrana verileri gönderecek.
	 {


		 HAL_UART_Receive_IT(&huart2, RX_Data, 4);  //UART CallBack'e git ve basılan butona göre değer gönder ekrana.
		 sec8++;

	 }


}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

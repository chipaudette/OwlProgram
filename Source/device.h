#include <inttypes.h>

#define DEBUG_MEM
/* #define DEBUG_DWT */
/* #define STARTUP_CODE */

#define AUDIO_BIGEND
/* #define AUDIO_SATURATE_SAMPLES // SATURATE adds almost 500 cycles to 24-bit mode */
#define AUDIO_CHANNELS               2
#define AUDIO_BITDEPTH               24    /* bits per sample */
#define AUDIO_MAX_BLOCK_SIZE         1024

#define MAX_BUFFERS_PER_PATCH        8
#define MAX_NUMBER_OF_PATCHES        32

#define NOF_ADC_VALUES               5

#define LED_PORT                     GPIOE
#define LED_GREEN                    GPIO_Pin_5
#define LED_RED                      GPIO_Pin_3
#define LED_CLOCK                    RCC_AHB1Periph_GPIOE

#ifdef  USE_FULL_ASSERT
#ifdef __cplusplus
 extern "C" {
#endif
   void assert_failed(uint8_t* file, uint32_t line);
#ifdef __cplusplus
}
#endif
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
#endif

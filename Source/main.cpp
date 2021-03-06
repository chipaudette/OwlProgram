#include <string.h>
#include <inttypes.h>
#include "ProgramVector.h"
#include "ServiceCall.h"
#include "device.h"
#include "main.h"

#ifdef DEBUG_MEM
#include <malloc.h>
#endif /* DEBUG_MEM */

#ifdef STARTUP_CODE
extern char _sbss[];
extern char _ebss[];
extern char _sidata[];
extern char _sdata[];
extern char _edata[];
extern "C" void __libc_init_array();
#endif /* STARTUP_CODE */

ProgramVector programVector __attribute__ ((section (".pv")));
// ProgramVector* getProgramVector() { return &programVector; }

int main(void){
#ifdef STARTUP_CODE
  memcpy(_sidata, _sdata, _sdata-_edata); // Copy the data segment initializers
  memset(_sbss, 0, _ebss-_sbss); // zero fill the BSS segment
  __libc_init_array(); // Call static constructors
#endif /* STARTUP_CODE */

#ifdef DEBUG_DWT
  volatile unsigned int *DWT_CYCCNT = (volatile unsigned int *)0xE0001004; //address of the register
  volatile unsigned int *DWT_CONTROL = (volatile unsigned int *)0xE0001000; //address of the register
  volatile unsigned int *SCB_DEMCR = (volatile unsigned int *)0xE000EDFC; //address of the register
  *SCB_DEMCR = *SCB_DEMCR | 0x01000000;
  *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter
#endif /* DEBUG_DWT */
  ProgramVector* pv = getProgramVector();
  if(pv->checksum != sizeof(ProgramVector)){    
    pv->error = CHECKSUM_ERROR_STATUS;
    pv->message = (char*)"ProgramVector checksum error";
    pv->programStatus(AUDIO_ERROR_STATUS);
    return -1;
  }

  if(pv->audio_blocksize <= 0 || 
     pv->audio_blocksize > AUDIO_MAX_BLOCK_SIZE){
    pv->error = CONFIGURATION_ERROR_STATUS;
    pv->message = (char*)"Invalid blocksize";
    pv->programStatus(AUDIO_ERROR_STATUS);
    return -1;
  }

  setup();

#ifdef DEBUG_MEM
  struct mallinfo minfo = mallinfo();
  // pv->heap_bytes_used = minfo.uordblks;
  pv->heap_bytes_used = minfo.arena;
#endif /* DEBUG_MEM */

  for(;;){
    pv->programReady();
#ifdef DEBUG_DWT
      *DWT_CYCCNT = 0; // reset the counter
#endif /* DEBUG_DWT */
      processBlock();
#ifdef DEBUG_DWT
      pv->cycles_per_block = *DWT_CYCCNT;
#endif /* DEBUG_DWT */
  }
}

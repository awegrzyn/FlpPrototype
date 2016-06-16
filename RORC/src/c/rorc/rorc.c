#include "rorc.h"

void rorcReset (volatile void *buff, int option){
  DEBUG_PRINTF(PDADEBUG_ENTER, "");
  int prorc_cmd;
  long long int longret, timeout;

  timeout = DDL_RESPONSE_TIME * pci_loop_per_usec;
  prorc_cmd = 0;
  if (option & RORC_RESET_DIU)
    prorc_cmd |= DRORC_CMD_RESET_DIU;
  if (option & RORC_RESET_FF)
    prorc_cmd |= DRORC_CMD_CLEAR_RXFF | DRORC_CMD_CLEAR_TXFF;
  if (option & RORC_RESET_FIFOS)
    prorc_cmd |= DRORC_CMD_CLEAR_FIFOS;
  if (option & RORC_RESET_ERROR)
    prorc_cmd |= DRORC_CMD_CLEAR_ERROR;
  if (option & RORC_RESET_COUNTERS)
    prorc_cmd |= DRORC_CMD_CLEAR_COUNTERS;
  if (prorc_cmd)   // any reset
    {
      rorcWriteReg(buff, C_CSR, (__u32) prorc_cmd); 
    }
    if (option & RORC_RESET_SIU)
    {
      rorcPutCommandRegister(buff, PRORC_CMD_RESET_SIU);
      longret = ddlWaitStatus(buff, timeout);
      if (longret < timeout)
        ddlReadStatus(buff);
    }
    if (!option || (option & RORC_RESET_RORC))
    {
      rorcWriteReg(buff, RCSR, DRORC_CMD_RESET_CHAN);  //channel reset
    }
  }

int rorcEmptyDataFifos(volatile void *buff, int empty_time)

/* try to empty D-RORC's data FIFOs                            
               empty_time:  time-out value in usecs
 * Returns:    RORC_STATUS_OK or RORC_TIMEOUT 
 */

{
  struct timeval start_tv, now_tv;
  int dsec, dusec;
  double dtime;
  DEBUG_PRINTF(PDADEBUG_ENTER, "");
  gettimeofday(&start_tv, NULL);
  dtime = 0;
  while (dtime < empty_time){
    if (!rorcCheckRxData(buff))
      return (RORC_STATUS_OK);

    rorcWriteReg(buff, C_CSR, (__u32) DRORC_CMD_CLEAR_FIFOS);
    gettimeofday(&now_tv, NULL);
    elapsed(&now_tv, &start_tv, &dsec, &dusec);
    dtime = (double)dsec * MEGA + (double)dusec;
  }

  if (rorcCheckRxData(buff))        // last check
    return (RORC_TIMEOUT);
  else
    return (RORC_STATUS_OK);
}

int rorcArmDDL(volatile void *buff, int option){
  DEBUG_PRINTF(PDADEBUG_ENTER, "");
  int ret;
  unsigned long retlong;
  int print = 0; // -1;
  int stop = 1;
  long long int TimeOut;

  TimeOut = DDL_RESPONSE_TIME * pci_loop_per_usec;

  if (diu_version){
    if (option & RORC_RESET_FEE){
      /* not implemented */
      return RORC_CMD_NOT_ALLOWED;
    }
    if (option & RORC_RESET_SIU){
      ret = ddlResetSiu(buff, 0, 3, TimeOut);
      if (ret == -1){
        printf(" Unsuccessful SIU reset\n");
        return RORC_NOT_ACCEPTED;
      }
    }
    if (option & RORC_LINK_UP){
      if (diu_version <= NEW){
        retlong = ddlLinkUp(buff, 1, print, stop, TimeOut);
        if (retlong == -1){
          printf(" Can not read DIU status");
          return RORC_LINK_NOT_ON;
        }
      }
      else{
        rorcReset(buff, RORC_RESET_RORC);
        rorcReset(buff, RORC_RESET_DIU);
        rorcReset(buff, RORC_RESET_SIU);
        usleep(100000); 

        if (rorcCheckLink(buff))
          return (RORC_LINK_NOT_ON);
        if (rorcEmptyDataFifos(buff, 100000))
          return (RORC_TIMEOUT);

        rorcReset(buff, RORC_RESET_SIU);
        rorcReset(buff, RORC_RESET_DIU);
        rorcReset(buff, RORC_RESET_RORC); 
        usleep(100000);

        if (rorcCheckLink(buff))
          return (RORC_LINK_NOT_ON);
      }
    }
    if (option & RORC_RESET_DIU)
       rorcReset(buff, RORC_RESET_DIU);
  }
  else{
    printf(" No DIU plugged into the RORC\n");
    return (RORC_LINK_NOT_ON);
  }
  if (option & RORC_RESET_FF)
    rorcReset(buff, RORC_RESET_FF);
  if (option & RORC_RESET_RORC)
    rorcReset(buff, RORC_RESET_RORC);
  
  return RORC_STATUS_OK;
}
 
int rorcCheckRxFreeFifo(volatile void *buff)  //RX Address FIFO
{
   __u32 st;
DEBUG_PRINTF(PDADEBUG_ENTER, "");
  st = rorcReadReg(buff, C_CSR);
  if (st & DRORC_STAT_RXAFF_FULL)  
    return (RORC_FF_FULL);
  else if (st & DRORC_STAT_RXAFF_EMPTY)
    return (RORC_FF_EMPTY);
  else
    return (RORC_STATUS_OK);  //Not Empty
  
}

__u32 rorcReadFw(volatile void *buff){
  __u32 fw;

  fw = rorcReadReg (buff, RFID);

  return (fw);
}

void rorcInterpretVersion(__u32 x){
  int major, minor, month, day, year;

  char* monthName[] = {"unknown month", "January", "February", "March",
                       "April", "May", "June", "July", "August",
                       "September", "October", "November", "December",
                       "unknown month", "unknown month", "unknown month"};

  major   = rorcFWVersMajor(x);
  minor   = rorcFWVersMinor(x);
  year    = (x >>  9) & 0xf;
  month   = (x >>  5) & 0xf;
  day     =  x        & 0x1f;

  printf(" Version: %d.%d\n Release date : %s %d 20%02d\n",
           major, minor, monthName[month], day, year);

}

int rorcStartDataReceiver(volatile void *buff,
                          unsigned long   readyFifoBaseAddress){
  int fw_major, fw_minor;
  unsigned long fw;
  DEBUG_PRINTF(PDADEBUG_ENTER, "");
  rorcWriteReg(buff, C_RRBAR, (readyFifoBaseAddress & 0xffffffff));
  if (rorc_revision >= DRORC2){
    fw = rorcReadFw(buff);
    fw_major = rorcFWVersMajor(fw);
    fw_minor = rorcFWVersMinor(fw);
    if ((rorc_revision >= PCIEXP) ||
        (fw_major > 2) || ((fw_major == 2) && (fw_minor >= 16))){
#if __WORDSIZE > 32
      rorcWriteReg(buff, C_RRBX, (readyFifoBaseAddress >> 32));
#else
      rorcWriteReg(buff, C_RRBX, 0x0);
#endif
    }  
  }
  if (!(rorcReadReg(buff, C_CSR) & DRORC_CMD_DATA_RX_ON_OFF))
    rorcWriteReg(buff, C_CSR, DRORC_CMD_DATA_RX_ON_OFF);

  return (RORC_STATUS_OK);
}

int rorcStopDataReceiver(volatile void *buff){
  DEBUG_PRINTF(PDADEBUG_ENTER, "");
  if (rorcReadReg(buff, C_CSR) & DRORC_CMD_DATA_RX_ON_OFF)
    rorcWriteReg(buff, C_CSR, DRORC_CMD_DATA_RX_ON_OFF);

  return (RORC_STATUS_OK);
}

int rorcStartTrigger(volatile void *buff, long long int timeout, stword_t *stw){
  int ret;
  long long int longret;
  DEBUG_PRINTF(PDADEBUG_ENTER, "");
  ret = ddlSendCommand(buff, FEE, RDYRX, 0, 0, timeout);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(buff, timeout);
  if (longret >= timeout)
    return RORC_NOT_ACCEPTED;
  else
    *stw = ddlReadStatus(buff);

  return RORC_STATUS_OK;
}

int rorcStopTrigger(volatile void *buff, long long int timeout, stword_t *stw)
{
  int ret;
  long long int longret;
  int once = 0;

 repeat:
  ret = ddlSendCommand(buff, FEE, EOBTR, 0, 0, timeout);
  if (ret == RORC_LINK_NOT_ON)
    return (ret);
  if (ret == RORC_TIMEOUT)
    return RORC_STATUS_ERROR;

  longret = ddlWaitStatus(buff, timeout);
  if (longret >= timeout)
  {
    if (once == 0)
    {
      once = 1;
      goto repeat;
    }
    return RORC_NOT_ACCEPTED;
  }
  else
    *stw = ddlReadStatus(buff);

  return RORC_STATUS_OK;
}

int rorcArmDataGenerator(volatile void *buff,
                         __u32           initEventNumber,
                         __u32           initDataWord,
                         int             dataPattern,
                         int             eventLen,
                         int             seed,
                         int             *rounded_len){
  unsigned long blockLen;

  if ((eventLen < 1) || (eventLen >= 0x00080000))
    return (RORC_INVALID_PARAM);

  *rounded_len = eventLen;

  if (seed){
    /* round to the nearest lower power of two */
    *rounded_len = roundPowerOf2(eventLen);
    blockLen = ((*rounded_len - 1) << 4) | dataPattern;
    blockLen |= 0x80000000;
    rorcWriteReg(buff, C_DG2, seed);
  }
  else{
    blockLen = ((eventLen - 1) << 4) | dataPattern;
    rorcWriteReg(buff, C_DG2, initDataWord);
  }

  rorcWriteReg(buff, C_DG1, blockLen);
  rorcWriteReg(buff, C_DG3, initEventNumber);

  return (RORC_STATUS_OK);
}

int rorcParamOn(volatile void *buff, int param){
  if (param != PRORC_PARAM_LOOPB)
    return (RORC_INVALID_PARAM);
  if (!rorcCheckLoopBack(buff))
    rorcChangeLoopBack(buff);

  return (RORC_STATUS_OK);
}

int rorcParamOff(volatile void *buff)
// switches off both loopback and stop_on_error
{
  if (rorcCheckLoopBack(buff))
    rorcChangeLoopBack(buff);
  
  return (RORC_STATUS_OK);
}

int rorcStartDataGenerator(volatile void *buff, __u32 maxLoop){
  __u32 cycle;

  if (maxLoop)
    cycle = (maxLoop - 1) & 0x7fffffff;
  else
    cycle = 0x80000000;

  rorcWriteReg(buff, C_DG4, cycle);
  rorcWriteReg(buff, C_CSR, DRORC_CMD_START_DG);

  return (RORC_STATUS_OK);
}

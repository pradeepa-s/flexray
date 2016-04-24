/*******************************************************************
 *
 *    DESCRIPTION: FlexRay Drivers
 *
 *    AUTHOR: AARORA
 *
 *    HISTORY: v1.1 12/05/2011
 *
 *******************************************************************/ 

#include"Fr.h"

/***********************************************************************
	Fr_PrepareLPdu
	The function Fr_PrepareLPdu shall perform the following tasks on FlexRay
	CC Fr_CtrIdx:
	1. Figure out the physical resource (e.g., a buffer) mapped to the processing of
	the FlexRay frame identified by Fr_LPduIdx.
	2. Configure the physical resource (a buffer) appropriate for Fr_LPduPtr
	operation (SlotId, Cycle filter, payload length, header CRC, etc.) if the MCG
	uses the reconfiguration feature.
***********************************************************************/
void Fr_PrepareLPdu(FRAY_ST *Fray_PST, wrhs *Fr_LPduPtr)
{
	int wrhs1;
	int wrhs2;
	wrhs1  = ((Fr_LPduPtr->mbi) & 0x1)  <<29;
	wrhs1 |= (Fr_LPduPtr->txm & 0x1)  << 28;
	wrhs1 |= (Fr_LPduPtr->ppit & 0x1) << 27;
	wrhs1 |= (Fr_LPduPtr->cfg & 0x1)  << 26;
	wrhs1 |= (Fr_LPduPtr->chb & 0x1)  << 25;
	wrhs1 |= (Fr_LPduPtr->cha & 0x1)  << 24;
	wrhs1 |= (Fr_LPduPtr->cyc & 0x7F) << 16;
	wrhs1 |= (Fr_LPduPtr->fid & 0x7FF);
	Fray_PST->WRHS1_UN.WRHS1_UL = wrhs1;

	wrhs2  = ((Fr_LPduPtr->pl & 0x7F) << 16) | (Fr_LPduPtr->crc & 0x7FF);
	Fray_PST->WRHS2_UN.WRHS2_UL = wrhs2;

	Fray_PST->WRHS3_UN.WRHS3_UL = (Fr_LPduPtr->dp & 0x7FF);
}

/***********************************************************************
	Fr_Init
	The function Fr_Init shall internally store the configuration address to enable
	subsequent API calls to access the configuration.

***********************************************************************/

void Fr_Init(FRAY_ST *Fray_PST, cfg *Fr_ConfigPtr)
{
	Fray_PST->MRC_UN.MRC_UL       = Fr_ConfigPtr->mrc;
	Fray_PST->PRTC1_UN.PRTC1_UL   = Fr_ConfigPtr->prtc1;
	Fray_PST->PRTC2_UN.PRTC2_UL   = Fr_ConfigPtr->prtc2;
	Fray_PST->MHDC_UN.MHDC_UL     = Fr_ConfigPtr->mhdc;
	Fray_PST->GTUC1_UN.GTUC1_UL   = Fr_ConfigPtr->gtu1;  
	Fray_PST->GTUC2_UN.GTUC2_UL   = Fr_ConfigPtr->gtu2;  
	Fray_PST->GTUC3_UN.GTUC3_UL   = Fr_ConfigPtr->gtu3;  
	Fray_PST->GTUC4_UN.GTUC4_UL   = Fr_ConfigPtr->gtu4;  
	Fray_PST->GTUC5_UN.GTUC5_UL   = Fr_ConfigPtr->gtu5;  
	Fray_PST->GTUC6_UN.GTUC6_UL   = Fr_ConfigPtr->gtu6;  
	Fray_PST->GTUC7_UN.GTUC7_UL   = Fr_ConfigPtr->gtu7;  
	Fray_PST->GTUC8_UN.GTUC8_UL   = Fr_ConfigPtr->gtu8;  
	Fray_PST->GTUC9_UN.GTUC9_UL   = Fr_ConfigPtr->gtu9;  
	Fray_PST->GTUC10_UN.GTUC10_UL = Fr_ConfigPtr->gtu10;  
	Fray_PST->GTUC11_UN.GTUC11_UL = Fr_ConfigPtr->gtu11;  
	Fray_PST->SUCC2_UN.SUCC2_UL   = Fr_ConfigPtr->succ2; 
	Fray_PST->SUCC3_UN.SUCC3_UL   = Fr_ConfigPtr->succ3; 
}

/***********************************************************************
	Fr_TransmitTxLPdu
	

***********************************************************************/

void Fr_TransmitTxLPdu(FRAY_ST *Fray_PST, bc *Fr_LSduPtr)
{
	// ensure nothing is pending
	while ((Fray_PST->IBCR_UN.IBCR_UL & 0x0008000) != 0);
	Fray_PST->IBCM_UN.IBCM_UL=((Fr_LSduPtr->stxrh & 0x1) << 2) | ((Fr_LSduPtr->ldsh & 0x1) << 1) | (Fr_LSduPtr->lhsh & 0x1);
	Fray_PST->IBCR_UN.IBCR_UL=(Fr_LSduPtr->ibrh & 0x3F);
	// optimization possible for future by not gating like below
	// wait for completion on host registers
	while ((Fr_LSduPtr->ibsyh != 0) && ((Fray_PST->IBCR_UN.IBCR_UL & 0x00008000) != 0));
	// wait for completion on shadow registers
	while ((Fr_LSduPtr->ibsys != 0) && ((Fray_PST->IBCR_UN.IBCR_UL & 0x80000000) != 0));
}


/***********************************************************************
	Fr_ReceiveRxLPdu
	

***********************************************************************/

void Fr_ReceiveRxLPdu(FRAY_ST *Fray_PST, bc *Fr_LSduPtr)
{
	// ensure no transfer in progress on shadow registers
	while (((Fray_PST->OBCR_UN.OBCR_UL) & 0x00008000) != 0);
	Fray_PST->OBCM_UN.OBCM_UL=(((Fr_LSduPtr->rdss & 0x1) << 1) | (Fr_LSduPtr->rhss & 0x1));
	Fray_PST->OBCR_UN.OBCR_UL=((1 << 9) | (Fr_LSduPtr->obrs & 0x3F)); //req=1, view=0
	// wait for completion on shadow registers
	while (((Fray_PST->OBCR_UN.OBCR_UL) & 0x00008000) != 0);

	Fray_PST->OBCM_UN.OBCM_UL=(((Fr_LSduPtr->rdss & 0x1) << 1) | (Fr_LSduPtr->rhss & 0x1));
	Fray_PST->OBCR_UN.OBCR_UL=((1 << 8) | (Fr_LSduPtr->obrs & 0x3F)); //req=0, view=1
}


/***********************************************************************
	Fr_ControllerInit
	

***********************************************************************/

int Fr_ControllerInit(FRAY_ST *Fray_PST)
{
	unsigned int error=0;
	// write SUCC1 configuration
	Fray_PST->SUCC1_UN.SUCC1_UL = 0x0F1FFB00 | CMD_CONFIG;
	// Check if POC has accepted last command 
	if ((Fray_PST->SUCC1_UN.SUCC1_UL & 0xF) == 0x0) return 1;
	// Wait for PBSY bit to clear - POC not busy
	while ((Fray_PST->SUCC1_UN.SUCC1_UL & 0x00000080) != 0x0);

	// unlock CONFIG and enter READY state
	Fray_PST->LCK_UN.LCK_ST.clk_B8=0xCE;
	Fray_PST->LCK_UN.LCK_ST.clk_B8=0x31;
	// write SUCC1 configuration
	Fray_PST->SUCC1_UN.SUCC1_ST.cmd_B4=(0xFB00 | CMD_READY);
	// Check if POC has accepted last command 
	if ((Fray_PST->SUCC1_UN.SUCC1_UL & 0xF) == 0x0) error = 1;
	// Wait for PBSY bit to clear - POC not busy
	while ((Fray_PST->SUCC1_UN.SUCC1_UL & 0x00000080) != 0x0);	 
	return error;
}


/***********************************************************************
	Fr_AllowColdStart
	

***********************************************************************/

int Fr_AllowColdStart(FRAY_ST *Fray_PST)
{
	unsigned int error=0;
	// write SUCC1 configuration
	(Fray_PST->SUCC1_UN.SUCC1_UL = CMD_ALLOW_COLDSTART);
	// Check if POC has accepted last command 
	if ((Fray_PST->SUCC1_UN.SUCC1_UL & 0xF) == 0x0) error = 1;
	// Wait for PBSY bit to clear - POC not busy
	while ((Fray_PST->SUCC1_UN.SUCC1_UL & 0x00000080) != 0x0);
	return error;
}


/***********************************************************************
	Fr_StartCommunication
	

***********************************************************************/

int Fr_StartCommunication(FRAY_ST *Fray_PST)
{
	unsigned int error=0;
	// write SUCC1 configuration
	(Fray_PST->SUCC1_UN.SUCC1_UL = CMD_RUN);
	// Check if POC has accepted last command 
	if ((Fray_PST->SUCC1_UN.SUCC1_UL & 0xF) == 0x0) error = 1;
	return error;
}

/***********************************************************************
	header_crc_calc
	This function calculates the header CRC.
***********************************************************************/

int header_crc_calc(wrhs *Fr_LPduPtr)
{
  unsigned int header;

  int CrcInit = 0x1A;
  int length  = 20;
  int CrcNext;
  unsigned long CrcPoly  = 0x385;
  unsigned long CrcReg_X = CrcInit;
  unsigned long header_temp, reg_temp;

  header  = ((Fr_LPduPtr->sync & 0x1)  << 19) | ((Fr_LPduPtr->sfi & 0x1) << 18);
  header |= ((Fr_LPduPtr->fid & 0x7FF) <<  7) |  (Fr_LPduPtr->pl & 0x7F);
  
  header   <<= 11;
  CrcReg_X <<= 21;
  CrcPoly  <<= 21;

  while(length--) {
    header    <<= 1;
    header_temp = header & 0x80000000;
    reg_temp    = CrcReg_X & 0x80000000;

    if(header_temp ^ reg_temp){  // Step 1
      CrcNext = 1;
    } else {	
      CrcNext = 0;
    }
		
    CrcReg_X <<= 1;              // Step 2
	
    if(CrcNext) {
      CrcReg_X ^= CrcPoly;       // Step 3
    }	
  }
	
  CrcReg_X >>= 21;

  return CrcReg_X;
}


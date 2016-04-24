/*******************************************************************
 *
 *    DESCRIPTION: FlexRay configuration and Initialization of buffers
 *
 *    AUTHOR: AARORA
 *
 *    HISTORY: v1.1 12/05/2011
 *
 *******************************************************************/


#include "Fr.h"
	wrhs Fr_LPdu;
	cfg Fr_Config;
	bc Fr_LSdu1;
	bc Fr_LSdu2;

void configure_initialize_node_a(FRAY_ST *Fray_PST)
{
	wrhs *Fr_LPduPtr=&Fr_LPdu;
	cfg *Fr_ConfigPtr=&Fr_Config;
	bc *Fr_LSduPtr=&Fr_LSdu1;

	// GTUs, PRTC configuration
	Fr_ConfigPtr->gtu1  = 0x00036B00; // pMicroPerCycle = 224000d = 36B00h
	Fr_ConfigPtr->gtu2  = 0x000F15E0; // gSyncodeMax = Fh, gMacroPerCycle = 5600d = 15E0h
	Fr_ConfigPtr->gtu3  = 0x00061818; // gMacroInitialOffset = 6h, pMicroInitialOffset = 24d = 18h
	Fr_ConfigPtr->gtu4  = 0x0AE40AE3; // gOffsetCorrectionStart - 1 = 2788d = AE4h, gMacroPerCycle - gdNIT - 1 = 2787d = AE3h
	Fr_ConfigPtr->gtu5  = 0x33010303; // pDecodingCorrection = 51d = 33h, pClusterDriftDamping = 1h, pDelayCompensation = 3h
	Fr_ConfigPtr->gtu6  = 0x01510081; // pdMaxDrift = 337d = 151h, pdAcceptedStartupRange = 129d = 81h
	Fr_ConfigPtr->gtu7  = 0x00080056; // gNumberOfStaticSlots = 8h, gdStaticSlot = 86d = 56h
	Fr_ConfigPtr->gtu8  = 0x015A0004; // gNumberOfMinislots = 346d = 15Ah, gdMinislot = 4h
	Fr_ConfigPtr->gtu9  = 0x00010204; // gdDynamicSlotIdlePhase = 1, gdMinislotActionPointOffset = 2, gdActionPointOffset = 4h
	Fr_ConfigPtr->gtu10 = 0x015100CD; // pRateCorrectionOut = 337d = 151h, pOffsetCorrectionOut = 205d = CDh
	Fr_ConfigPtr->gtu11 = 0x00000000; // pExternRateCorrection = 0, pExternOffsetCorrection = 0, no ext. clk. corr.
	Fr_ConfigPtr->succ2 = 0x0F036DA2; // gListenNoise = Fh, pdListenTimeout = 224674d = 36DA2h
	Fr_ConfigPtr->succ3 = 0x000000FF; // gMaxWithoutClockCorrectionFatal = Fh , passive = Fh
	Fr_ConfigPtr->prtc1 = 0x084C000A; // pWakeupPattern = 2h, gdWakeupSymbolRxWindow = 76d, BRP = 0, gdTSSTransmitter = Ah
	Fr_ConfigPtr->prtc2 = 0x3CB41212; // gdWakeupSymbolTxLow = 60d, gdWakeupSymbolTxIdle = 180d, gdWakeupSymbolRxLow = 18d, gdWakeupSymbolRxIdle = 18d
	Fr_ConfigPtr->mhdc  = 0x010D0009; // pLatestTransmit = 269d = 010Dh, gPayloadLengthStatic = 9h
	Fr_ConfigPtr->mrc   = 0x00174004; // LCB=23d, FFB=64d, FDB=4d (0..3 static, 4..23 dyn., 0 fifo)


	// Wait for PBSY bit to clear - POC not busy
	while(((Fray_PST->SUCC1_UN.SUCC1_UL) & 0x00000080) != 0);
	
	// Initialize
	Fr_Init(Fray_PST, Fr_ConfigPtr);

	// Buffer config initialization 
	Fr_LPduPtr->mbi  = 1;   // message buffer interrupt
	Fr_LPduPtr->txm  = 0;   // transmission mode - continuous mode
	Fr_LPduPtr->ppit = 0;   // network management Enable
	Fr_LPduPtr->cfg  = 0;   // message buffer configuration bit (0=RX, 1 = TX)
	Fr_LPduPtr->chb  = 1;   // Ch B
	Fr_LPduPtr->cha  = 1;   // Ch A
	Fr_LPduPtr->cyc  = 0;   // Cycle Filtering Code (no cycle filtering)
	Fr_LPduPtr->fid  = 0;   // Frame ID
	Fr_LPduPtr->pl   = 0;   // Payload Length
	Fr_LPduPtr->dp   = 0;   // Pointer to start of data in message RAM
	Fr_LPduPtr->sfi  = 0;   // startup frame indicator
	Fr_LPduPtr->sync = 0;   // sync frame indicator

	Fr_LSduPtr->ibrh = 0;  // input buffer number
	Fr_LSduPtr->stxrh= 0;  // set transmission request
	Fr_LSduPtr->ldsh = 0;  // load data section
	Fr_LSduPtr->lhsh = 0;  // load header section
	Fr_LSduPtr->ibsyh = 1; // check for input buffer busy shadow
	Fr_LSduPtr->ibsys = 1; // check for input buffer busy host
	Fr_LSduPtr->obrs = 0;  // output buffer number
	Fr_LSduPtr->rdss = 0;  // read data section
	Fr_LSduPtr->rhss = 0;  // read header section


	// Message buffers

	// Buffer #1
	Fr_LPduPtr->fid  = 1;    // frame ID
	Fr_LPduPtr->dp   = 0x80; // Pointer to start of data in message RAM
	Fr_LPduPtr->cfg  = 1;    // TX frame
	Fr_LPduPtr->sync = 1;    // sync frame indicator
	Fr_LPduPtr->sfi  = 1;    // startup frame indicator
	Fr_LPduPtr->pl   = 9;	 // 18 byte payload
	Fr_LPduPtr->crc  = header_crc_calc(Fr_LPduPtr);

	Fr_LSduPtr->lhsh = 1;  // load header section
	Fr_LSduPtr->ibrh = 0;  // input buffer number

	Fr_PrepareLPdu(Fray_PST, Fr_LPduPtr);
	Fr_TransmitTxLPdu(Fray_PST, Fr_LSduPtr);

	// Buffer #2
	Fr_LPduPtr->fid  = 2;    // frame ID
	Fr_LPduPtr->dp   = 0x85; // Pointer to start of data in message RAM
	Fr_LPduPtr->cfg  = 0;    // RX frame
	Fr_LPduPtr->sync = 0;    // sync frame indicator
	Fr_LPduPtr->sfi  = 0;    // startup frame indicator
	Fr_LPduPtr->crc  = 0;

	Fr_LSduPtr->ibrh = 2;  // input buffer number

	Fr_PrepareLPdu(Fray_PST, Fr_LPduPtr);
	Fr_TransmitTxLPdu(Fray_PST, Fr_LSduPtr);

    // buffer #9
	Fr_LPduPtr->fid  = 9;   // frame ID
	Fr_LPduPtr->dp   = 0x200; // Pointer to start of data in message RAM
	Fr_LPduPtr->cfg  = 1;    // TX frame
	Fr_LPduPtr->chb  = 0;    // No transmission on Ch B
	Fr_LPduPtr->pl   = 127;	 // 254 byte payload
	Fr_LPduPtr->crc  = header_crc_calc(Fr_LPduPtr);

	Fr_LSduPtr->ibrh = 9;  // input buffer number

	Fr_PrepareLPdu(Fray_PST, Fr_LPduPtr);
	Fr_TransmitTxLPdu(Fray_PST, Fr_LSduPtr);

	// buffer #10
	Fr_LPduPtr->fid  = 10;     // frame ID
	Fr_LPduPtr->dp   = 0x240; // Pointer to start of data in message RAM
	Fr_LPduPtr->cfg  = 0;    // RX frame
	Fr_LPduPtr->crc  = 0;

	Fr_LSduPtr->ibrh = 10;  // input buffer number

	Fr_PrepareLPdu(Fray_PST, Fr_LPduPtr);
	Fr_TransmitTxLPdu(Fray_PST, Fr_LSduPtr);


	Fr_ControllerInit(Fray_PST);
	// Initialize Interrupts
	Fray_PST->EIR_UN.EIR_UL       = 0xFFFFFFFF; // Clear Error Int.
	Fray_PST->SIR_UN.SIR_UL       = 0xFFFFFFFF; // Clear Status Int.
	Fray_PST->SILS_UN.SILS_UL     = 0x00000000; // all Status Int. to eray_int0
	Fray_PST->SIER_UN.SIER_UL     = 0xFFFFFFFF; // Disable all Status Int.
	Fray_PST->SIES_UN.SIES_UL     = 0x00000004; // Enable CYCSE Int.
	Fray_PST->ILE_UN.ILE_UL       = 0x00000002; // enable eray_int1

	Fr_AllowColdStart(Fray_PST);
}
	

int transmit_check_node_a(FRAY_ST *Fray_PST)
{
	bc *write_buffer=&Fr_LSdu1;
	bc *read_buffer=&Fr_LSdu2;
	unsigned int  ndat1;
	int error=0;
	write_buffer->ibrh = 0;  // input buffer number
	write_buffer->stxrh= 1;  // set transmission request
	write_buffer->ldsh = 1;  // load data section
	write_buffer->lhsh = 0;  // load header section
	write_buffer->ibsys = 0; // check for input buffer busy shadow
	write_buffer->ibsyh = 1; // check for input buffer busy host

    // wait for cycle start interrupt flag
    Fray_PST->SIR_UN.SIR_UL = 0xFFFFFFFF;            // clear all status int. flags
    while ((Fray_PST->SIR_UN.SIR_UL & 0x4) == 0x0);    // wait for CYCS interrupt flag
    Fray_PST->SIR_UN.SIR_UL = 0xFFFFFFFF;            // clear all status int. flags

	// write payload for buffers
	// buffer #1
	(Fray_PST->WRDS[0] = 0x00000001);    // Data 1
    (Fray_PST->WRDS[1] = 0x000000FF);    // Data 2
	Fr_TransmitTxLPdu(Fray_PST, write_buffer);

	// buffer #9
	write_buffer->ibrh = 9;
	(Fray_PST->WRDS[0] = 0xFF);    		 // Data 1
    (Fray_PST->WRDS[1] = 0xFFFF);   	 // Data 2
	(Fray_PST->WRDS[2] = 0xFFFFFF);      // Data 3
    (Fray_PST->WRDS[3] = 0xFFFFFFFF);    // Data 4
	(Fray_PST->WRDS[4] = 0xFFFFFF00);    // Data 5
    (Fray_PST->WRDS[5] = 0xFFFF0000);    // Data 6
	Fr_TransmitTxLPdu(Fray_PST, write_buffer);

	 // check received frames
    ndat1 = Fray_PST->NDAT1_UN.NDAT1_UL;

    // check if a message is received in buffer 2 from node B
    if ((ndat1 & 0x4) != 0)
    {
      read_buffer->obrs=2;  // output buffer number
	  read_buffer->rdss=1;  // read data section
	  read_buffer->rhss=0;  // read header section
      // Transfer message buffer 1 data to output buffer registers
      Fr_ReceiveRxLPdu(Fray_PST, read_buffer);
      if (Fray_PST->RDDS[1] != 0x87654321) error++; 
	}
	return error;
}

void configure_initialize_node_b(FRAY_ST *Fray_PST)
{
	wrhs *Fr_LPduPtr=&Fr_LPdu;
	cfg *Fr_ConfigPtr=&Fr_Config;
	bc *Fr_LSduPtr=&Fr_LSdu1;

	// GTUs, PRTC configuration
	Fr_ConfigPtr->gtu1  = 0x00036B00; // pMicroPerCycle = 224000d = 36B00h
	Fr_ConfigPtr->gtu2  = 0x000F15E0; // gSyncodeMax = Fh, gMacroPerCycle = 5600d = 15E0h
	Fr_ConfigPtr->gtu3  = 0x00061818; // gMacroInitialOffset = 6h, pMicroInitialOffset = 24d = 18h
	Fr_ConfigPtr->gtu4  = 0x0AE40AE3; // gOffsetCorrectionStart - 1 = 2788d = AE4h, gMacroPerCycle - gdNIT - 1 = 2787d = AE3h
	Fr_ConfigPtr->gtu5  = 0x33010303; // pDecodingCorrection = 51d = 33h, pClusterDriftDamping = 1h, pDelayCompensation = 3h
	Fr_ConfigPtr->gtu6  = 0x01510081; // pdMaxDrift = 337d = 151h, pdAcceptedStartupRange = 129d = 81h
	Fr_ConfigPtr->gtu7  = 0x00080056; // gNumberOfStaticSlots = 8h, gdStaticSlot = 86d = 56h
	Fr_ConfigPtr->gtu8  = 0x015A0004; // gNumberOfMinislots = 346d = 15Ah, gdMinislot = 4h
	Fr_ConfigPtr->gtu9  = 0x00010204; // gdDynamicSlotIdlePhase = 1, gdMinislotActionPointOffset = 2, gdActionPointOffset = 4h
	Fr_ConfigPtr->gtu10 = 0x015100CD; // pRateCorrectionOut = 337d = 151h, pOffsetCorrectionOut = 205d = CDh
	Fr_ConfigPtr->gtu11 = 0x00000000; // pExternRateCorrection = 0, pExternOffsetCorrection = 0, no ext. clk. corr.
	Fr_ConfigPtr->succ2 = 0x0F036DA2; // gListenNoise = Fh, pdListenTimeout = 224674d = 36DA2h
	Fr_ConfigPtr->succ3 = 0x000000FF; // gMaxWithoutClockCorrectionFatal = Fh , passive = Fh
	Fr_ConfigPtr->prtc1 = 0x084C000A; // pWakeupPattern = 2h, gdWakeupSymbolRxWindow = 76d, BRP = 0, gdTSSTransmitter = Ah
	Fr_ConfigPtr->prtc2 = 0x3CB41212; // gdWakeupSymbolTxLow = 60d, gdWakeupSymbolTxIdle = 180d, gdWakeupSymbolRxLow = 18d, gdWakeupSymbolRxIdle = 18d
	Fr_ConfigPtr->mhdc  = 0x010D0009; // pLatestTransmit = 269d = 010Dh, gPayloadLengthStatic = 9h
	Fr_ConfigPtr->mrc   = 0x00174004; // LCB=23d, FFB=64d, FDB=4d (0..3 static, 4..23 dyn., 0 fifo)


	// Wait for PBSY bit to clear - POC not busy
	while(((Fray_PST->SUCC1_UN.SUCC1_UL) & 0x00000080) != 0);
	
	// Initialize
	Fr_Init(Fray_PST, Fr_ConfigPtr);

	// Buffer config initialization 
	Fr_LPduPtr->mbi  = 1;   // message buffer interrupt
	Fr_LPduPtr->txm  = 0;   // transmission mode - continuous mode
	Fr_LPduPtr->ppit = 0;   // network management Enable
	Fr_LPduPtr->cfg  = 0;   // message buffer configuration bit (0=RX, 1 = TX)
	Fr_LPduPtr->chb  = 1;   // Ch B
	Fr_LPduPtr->cha  = 1;   // Ch A
	Fr_LPduPtr->cyc  = 0;   // Cycle Filtering Code (no cycle filtering)
	Fr_LPduPtr->fid  = 0;   // Frame ID
	Fr_LPduPtr->pl   = 0;   // Payload Length
	Fr_LPduPtr->dp   = 0;   // Pointer to start of data in message RAM
	Fr_LPduPtr->sfi  = 0;   // startup frame indicator
	Fr_LPduPtr->sync = 0;   // sync frame indicator

	Fr_LSduPtr->ibrh = 0;  // input buffer number
	Fr_LSduPtr->stxrh= 0;  // set transmission request
	Fr_LSduPtr->ldsh = 0;  // load data section
	Fr_LSduPtr->lhsh = 0;  // load header section
	Fr_LSduPtr->ibsyh = 1; // check for input buffer busy shadow
	Fr_LSduPtr->ibsys = 1; // check for input buffer busy host
	Fr_LSduPtr->obrs = 0;  // output buffer number
	Fr_LSduPtr->rdss = 0;  // read data section
	Fr_LSduPtr->rhss = 0;  // read header section


	// Message buffers

	// Buffer #2
	Fr_LPduPtr->fid  = 2;    // frame ID
	Fr_LPduPtr->dp   = 0x85; // Pointer to start of data in message RAM
	Fr_LPduPtr->cfg  = 1;    // TX frame
	Fr_LPduPtr->sync = 1;    // sync frame indicator
	Fr_LPduPtr->sfi  = 1;    // startup frame indicator
	Fr_LPduPtr->pl   = 9;	 // 18 byte payload
	Fr_LPduPtr->crc  = header_crc_calc(Fr_LPduPtr);

	Fr_LSduPtr->lhsh = 1;  // load header section
	Fr_LSduPtr->ibrh = 0;  // input buffer number

	Fr_PrepareLPdu(Fray_PST, Fr_LPduPtr);
	Fr_TransmitTxLPdu(Fray_PST, Fr_LSduPtr);

	// Buffer #1
	Fr_LPduPtr->fid  = 1;    // frame ID
	Fr_LPduPtr->dp   = 0x80; // Pointer to start of data in message RAM
	Fr_LPduPtr->cfg  = 0;    // RX frame
	Fr_LPduPtr->sync = 0;    // sync frame indicator
	Fr_LPduPtr->sfi  = 0;    // startup frame indicator
	Fr_LPduPtr->crc  = 0;

	Fr_LSduPtr->ibrh = 1;  // input buffer number

	Fr_PrepareLPdu(Fray_PST, Fr_LPduPtr);
	Fr_TransmitTxLPdu(Fray_PST, Fr_LSduPtr);

    // buffer #10
	Fr_LPduPtr->fid  = 10;   // frame ID
	Fr_LPduPtr->dp   = 0x240; // Pointer to start of data in message RAM
	Fr_LPduPtr->cfg  = 1;    // TX frame
	Fr_LPduPtr->chb  = 0;    // No transmission on Ch B
	Fr_LPduPtr->pl   = 127;	 // 254 byte payload
	Fr_LPduPtr->crc  = header_crc_calc(Fr_LPduPtr);

	Fr_LSduPtr->ibrh = 10;  // input buffer number

	Fr_PrepareLPdu(Fray_PST, Fr_LPduPtr);
	Fr_TransmitTxLPdu(Fray_PST, Fr_LSduPtr);

	// buffer #9
	Fr_LPduPtr->fid  = 9;     // frame ID
	Fr_LPduPtr->dp   = 0x200; // Pointer to start of data in message RAM
	Fr_LPduPtr->cfg  = 0;    // RX frame
	Fr_LPduPtr->crc  = 0;

	Fr_LSduPtr->ibrh = 9;  // input buffer number

	Fr_PrepareLPdu(Fray_PST, Fr_LPduPtr);
	Fr_TransmitTxLPdu(Fray_PST, Fr_LSduPtr);


	Fr_ControllerInit(Fray_PST);
	// Initialize Interrupts
	Fray_PST->EIR_UN.EIR_UL       = 0xFFFFFFFF; // Clear Error Int.
	Fray_PST->SIR_UN.SIR_UL       = 0xFFFFFFFF; // Clear Status Int.
	Fray_PST->SILS_UN.SILS_UL     = 0x00000000; // all Status Int. to eray_int0
	Fray_PST->SIER_UN.SIER_UL     = 0xFFFFFFFF; // Disable all Status Int.
	Fray_PST->SIES_UN.SIES_UL     = 0x00000004; // Enable CYCSE Int.
	Fray_PST->ILE_UN.ILE_UL       = 0x00000002; // enable eray_int1

	Fr_AllowColdStart(Fray_PST);
}
	

int transmit_check_node_b(FRAY_ST *Fray_PST)
{
	bc *write_buffer=&Fr_LSdu1;
	bc *read_buffer=&Fr_LSdu2;
	unsigned int  ndat1;
	int error=0;
	write_buffer->ibrh = 0;  // input buffer number
	write_buffer->stxrh= 1;  // set transmission request
	write_buffer->ldsh = 1;  // load data section
	write_buffer->lhsh = 0;  // load header section
	write_buffer->ibsys = 0; // check for input buffer busy shadow
	write_buffer->ibsyh = 1; // check for input buffer busy host

    // wait for cycle start interrupt flag
    Fray_PST->SIR_UN.SIR_UL = 0xFFFFFFFF;            // clear all status int. flags
    while ((Fray_PST->SIR_UN.SIR_UL & 0x4) == 0x0);    // wait for CYCS interrupt flag
    Fray_PST->SIR_UN.SIR_UL = 0xFFFFFFFF;            // clear all status int. flags

	// write payload for buffers
	// buffer #2
	(Fray_PST->WRDS[0] = 0x12345678);    // Data 1
    (Fray_PST->WRDS[1] = 0x87654321);    // Data 2
	Fr_TransmitTxLPdu(Fray_PST, write_buffer);

	// buffer #10
	write_buffer->ibrh = 10;
	(Fray_PST->WRDS[0] = 0xFF);    		 // Data 1
    (Fray_PST->WRDS[1] = 0xFFFF);   	 // Data 2
	(Fray_PST->WRDS[2] = 0xFFFFFF);      // Data 3
    (Fray_PST->WRDS[3] = 0xFFFFFFFF);    // Data 4
	(Fray_PST->WRDS[4] = 0xFFFFFF00);    // Data 5
    (Fray_PST->WRDS[5] = 0xFFFF0000);    // Data 6
	Fr_TransmitTxLPdu(Fray_PST, write_buffer);

	 // check received frames
    ndat1 = Fray_PST->NDAT1_UN.NDAT1_UL;

    // check if a message is received in buffer 2 from node A
    if ((ndat1 & 0x2) != 0)
    {
      read_buffer->obrs=1;  // output buffer number
	  read_buffer->rdss=1;  // read data section
	  read_buffer->rhss=0;  // read header section
      // Transfer message buffer 1 data to output buffer registers
      Fr_ReceiveRxLPdu(Fray_PST, read_buffer);
      if (Fray_PST->RDDS[1] != 0x000000FF) error++; 
	}
	  return error;		
}

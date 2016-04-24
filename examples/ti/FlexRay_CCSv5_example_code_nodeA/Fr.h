/*******************************************************************
 *
 *    DESCRIPTION: FlexRay header
 *
 *    AUTHOR: AARORA
 *
 *    HISTORY: v1.1 12/05/2011
 *
 *******************************************************************/


// CMD constants (SUCC1)
//
#define CMD_command_not_accepted             0x0
#define CMD_CONFIG                           0x1
#define CMD_READY                            0x2
#define CMD_WAKEUP                           0x3
#define CMD_RUN                              0x4
#define CMD_ALL_SLOTS                        0x5
#define CMD_HALT                             0x6
#define CMD_FREEZE                           0x7
#define CMD_SEND_MTS                         0x8
#define CMD_ALLOW_COLDSTART                  0x9
#define CMD_RESET_STATUS_INDICATORS          0xA
#define CMD_MONITOR_MODE                     0xB
#define CMD_CLEAR_RAMS                       0xC
#define CMD_ASYNCHRONOUS_TRANSFER_MODE       0xE


typedef volatile struct fray_registers
{
/* ------------------------------------------------------------------------- */
/* FRAY_ST                                                                   */
/*  Definition of the FLEXRAY register map                                   */

unsigned      : 32;           /* Customer CPU Interface                      */
unsigned      : 32;           /* Customer CPU Interface                      */
unsigned      : 32;           /* Customer CPU Interface                      */
unsigned      : 32;           /* Customer CPU Interface                      */


/*  Special Registers                                                        */
/* Test Register 1                                                           */
/* 0x10 */
union test1
{
  unsigned long TEST1_UL;
  struct
    {
      unsigned           : 5;
      unsigned mt_B1     : 1; /* Control of Bus Guardian Macrotick Pin       */
      unsigned bgt_B1    : 1; /* Control of Bus Guardian Tick Pin            */
      unsigned arm_B1    : 1; /* Control of Bus Guardian ARM Pin             */
      unsigned bgeb_B1   : 1; /* Monitor Channel B Bus Guardian Enable Pin   */
      unsigned bgea_B1   : 1; /* Monitor Channel A Bus Guardian Enable Pin   */
      unsigned txenb_B1  : 1; /* Control of Channel B Transmit Enable Pin    */
      unsigned txena_B1  : 1; /* Control of Channel A Transmit Enable Pin    */
      unsigned txb_B1    : 1; /* Control of Channel B Transmit Pin           */
      unsigned txa_B1    : 1; /* Control of Channel A Transmit Pin           */
      unsigned rxb_B1    : 1; /* Monitor Channel B Receive Pin               */
      unsigned rxa_B1    : 1; /* Monitor Channel A Receive Pin               */
      unsigned           : 10;
      unsigned tmc_B2    : 2; /* Test Mode Control                           */
      unsigned           : 3;
      unsigned wrten_B1  : 1; /* Write Test Register Enable                  */
    } TEST1_ST;
} TEST1_UN;


/* Test Register 2                                                           */
/* 0x14 */
union test2
{
  unsigned long TEST2_UL;
  struct
    {
      unsigned           : 16;
      unsigned rdpb_B1   : 1;
      unsigned wrpb_B1   : 1;
      unsigned           : 7;
      unsigned ssel_B3   : 3;
      unsigned           : 1;
      unsigned rs_B3     : 3;
    } TEST2_ST;
} TEST2_UN;


unsigned      : 32;


/* Lock Register                                                             */
/* 0x1C */
union lck
{
  unsigned long LCK_UL;
  struct
    {
      unsigned           : 16;
      unsigned tmk_B8    : 8;
      unsigned clk_B8    : 8;
    } LCK_ST;
} LCK_UN;


/*  Interrupt Registers                                                      */
/* Error Input Register                                                      */
/* 0x20 */
union eir
{
  unsigned long EIR_UL;
  struct
    {
      unsigned           : 5;
      unsigned smeb_B1   : 1;
      unsigned ltvb_B1   : 1;
      unsigned edb_B1    : 1;
      unsigned           : 5;
      unsigned smea_B1   : 1;
      unsigned ltva_B1   : 1;
      unsigned eda_B1    : 1;
      unsigned           : 8;
      unsigned rfo_B1    : 1;
      unsigned perr_B1   : 1;
      unsigned           : 1;
      unsigned ccf_B1    : 1;
      unsigned sfo_B1    : 1;
      unsigned sfbm_B1   : 1;
      unsigned cna_B1    : 1;
      unsigned pemc_B1   : 1;
     } EIR_ST;
} EIR_UN;


/* Status Interrupt Register                                                 */
/* 0x24 */
union sir
{
  unsigned long SIR_UL;
  struct
    {
      unsigned           : 6;
      unsigned mtsb_B1   : 1;
      unsigned wupb_B1   : 1;
      unsigned           : 6;
      unsigned mtsa_B1   : 1;
      unsigned wupa_B1   : 1;
      unsigned           : 2;
      unsigned sucs_B1   : 1;
      unsigned swe_B1    : 1;
      unsigned tobc_B1   : 1;
      unsigned tibc_B1   : 1;
      unsigned ti1_B1    : 1;
      unsigned ti0_B1    : 1;
      unsigned nmvc_B1   : 1;
      unsigned rff_B1    : 1;
      unsigned rfne_B1   : 1;
      unsigned rxi_B1    : 1;
      unsigned txi_B1    : 1;
      unsigned cycs_B1   : 1;
      unsigned cas_B1    : 1;
      unsigned wst_B1    : 1;
     } SIR_ST;
} SIR_UN;


/* Error Interrupt Line Select                                               */
/* 0x28 */
union eils
{
  unsigned long EILS_UL;
  struct
    {
      unsigned           : 5;
      unsigned smebl_B1  : 1;
      unsigned ltvbl_B1  : 1;
      unsigned edbl_B1   : 1;
      unsigned           : 5;
      unsigned smeal_B1  : 1;
      unsigned ltval_B1  : 1;
      unsigned edal_B1   : 1;
      unsigned           : 8;
      unsigned rfol_B1   : 1;
      unsigned perrl_B1  : 1;
      unsigned           : 1;
      unsigned ccfl_B1   : 1;
      unsigned sfol_B1   : 1;
      unsigned sfbml_B1  : 1;
      unsigned cnal_B1   : 1;
      unsigned pemcl_B1  : 1;
    } EILS_ST;
} EILS_UN;


/* Status Interrupt Line Select                                              */
/* 0x2C */
union sils
{
  unsigned long SILS_UL;
  struct
    {
      unsigned           : 6;
      unsigned mtsbl_B1  : 1;
      unsigned wupbl_B1  : 1;
      unsigned           : 6;
      unsigned mtsal_B1  : 1;
      unsigned wupal_B1  : 1;
      unsigned           : 2;
      unsigned sucsl_B1  : 1;
      unsigned swel_B1   : 1;
      unsigned tobcl_B1  : 1;
      unsigned tibcl_B1  : 1;
      unsigned ti1l_B1   : 1;
      unsigned ti0l_B1   : 1;
      unsigned nmvcl_B1  : 1;
      unsigned rffl_B1   : 1;
      unsigned rfnel_B1  : 1;
      unsigned rxil_B1   : 1;
      unsigned txil_B1   : 1;
      unsigned cycsl_B1  : 1;
      unsigned casl_B1   : 1;
      unsigned wstl_B1   : 1;
    } SILS_ST;
} SILS_UN;


/* Error Interrupt Enable Set                                                */
/* 0x30 */
union eies
{
  unsigned long EIES_UL;
  struct
    {
      unsigned           : 5;
      unsigned smebe_B1  : 1;
      unsigned ltvbe_B1  : 1;
      unsigned edbe_B1   : 1;
      unsigned           : 5;
      unsigned smeae_B1  : 1;
      unsigned ltvae_B1  : 1;
      unsigned edae_B1   : 1;
      unsigned           : 8;
      unsigned rfoe_B1   : 1;
      unsigned perre_B1  : 1;
      unsigned           : 1;
      unsigned ccfe_B1   : 1;
      unsigned sfoe_B1   : 1;
      unsigned sfbme_B1  : 1;
      unsigned cnae_B1   : 1;
      unsigned pemce_B1  : 1;
    } EIES_ST;
} EIES_UN;


/* Error Interrupt Enable Reset                                              */
/* 0x34 */
union eier
{
  unsigned long EIER_UL;
  struct
    {
      unsigned           : 5;
      unsigned smebe_B1  : 1;
      unsigned ltvbe_B1  : 1;
      unsigned edbe_B1   : 1;
      unsigned           : 5;
      unsigned smeae_B1  : 1;
      unsigned ltvae_B1  : 1;
      unsigned edae_B1   : 1;
      unsigned           : 8;
      unsigned rfoe_B1   : 1;
      unsigned perre_B1  : 1;
      unsigned           : 1;
      unsigned ccfe_B1   : 1;
      unsigned sfoe_B1   : 1;
      unsigned sfbme_B1  : 1;
      unsigned cnae_B1   : 1;
      unsigned pemce_B1  : 1;
    } EIER_ST;
} EIER_UN;


/* Status Interrupt Enable Set                                               */
/* 0x38 */
union sies
{
  unsigned long SIES_UL;
  struct
    {
      unsigned           : 6;
      unsigned mtsbe_B1  : 1;
      unsigned wupbe_B1  : 1;
      unsigned           : 6;
      unsigned mtsae_B1  : 1;
      unsigned wupae_B1  : 1;
      unsigned           : 2;
      unsigned sucse_B1  : 1;
      unsigned swee_B1   : 1;
      unsigned tobce_B1  : 1;
      unsigned tibce_B1  : 1;
      unsigned ti1e_B1   : 1;
      unsigned ti0e_B1   : 1;
      unsigned nmvce_B1  : 1;
      unsigned rffe_B1   : 1;
      unsigned rfnee_B1  : 1;
      unsigned rxie_B1   : 1;
      unsigned txie_B1   : 1;
      unsigned cycse_B1  : 1;
      unsigned case_B1   : 1;
      unsigned wste_B1   : 1;
    } SIES_ST;
} SIES_UN;


/* Status Interrupt Enable Reset                                             */
/* 0x3C */
union sier
{
  unsigned long SIER_UL;
  struct
    {
      unsigned           : 6;
      unsigned mtsbe_B1  : 1;
      unsigned wupbe_B1  : 1;
      unsigned           : 6;
      unsigned mtsae_B1  : 1;
      unsigned wupae_B1  : 1;
      unsigned           : 2;
      unsigned sucse_B1  : 1;
      unsigned swee_B1   : 1;
      unsigned tobce_B1  : 1;
      unsigned tibce_B1  : 1;
      unsigned ti1e_B1   : 1;
      unsigned ti0e_B1   : 1;
      unsigned nmvce_B1  : 1;
      unsigned rffe_B1   : 1;
      unsigned rfnee_B1  : 1;
      unsigned rxie_B1   : 1;
      unsigned txie_B1   : 1;
      unsigned cycse_B1  : 1;
      unsigned case_B1   : 1;
      unsigned wste_B1   : 1;
    } SIER_ST;
} SIER_UN;


/* Interrupt Line Enable                                                     */
/* 0x40 */
union ile
{
  unsigned long ILE_UL;
  struct
    {
      unsigned           : 30;
      unsigned eint1_B1  : 1;
      unsigned eint0_B1  : 1;
    } ILE_ST;
} ILE_UN;


/* Timer 0 Configuration                                                     */
/* 0x44 */
union t0c
{
  unsigned long T0C_UL;
  struct
    {
      unsigned           : 2;
      unsigned t0mo_B14  : 14;
      unsigned           : 1;
      unsigned t0cc_B7   : 7;
      unsigned           : 6;
      unsigned t0ms_B1   : 1;
      unsigned t0rc_B1   : 1;
    } T0C_ST;
} T0C_UN;


/* Timer 1 Configuration                                                     */
/* 0x48 */
union t1c
{
  unsigned long T1C_UL;
  struct
    {
      unsigned           : 2;
      unsigned t1mc_B14  : 14;
      unsigned           : 14;
      unsigned t1ms_B1   : 1;
      unsigned t1rc_B1   : 1;
    } T1C_ST;
} T1C_UN;


/* Stop Watch Register                                                       */
/* 0x4C */
union stpw
{
  unsigned long STPW_UL;
  struct
    {
      unsigned           : 2;
      unsigned smtv_B14  : 14;
      unsigned           : 2;
      unsigned sccv_B6   : 6;
      unsigned           : 4;
      unsigned sswt_B1   : 1;
      unsigned edge_B1   : 1;
      unsigned swms_B1   : 1;
      unsigned eswt_B1   : 1;
    } STPW_ST;
} STPW_UN;


unsigned long RES1[12];       /* Reserved                                    */


/*  CC Control Registers                                                     */
/* SUC Configuration Register 1                                              */
/* 0x80 */
union succ1
{
  unsigned long SUCC1_UL;
  struct
    {
      unsigned           : 4;
      unsigned cchb_B1   : 1;
      unsigned ccha_B1   : 1;
      unsigned mtsb_B1   : 1;
      unsigned mtsa_B1   : 1;
      unsigned hcse_B1   : 1;
      unsigned tsm_B1    : 1;
      unsigned wucs_B1   : 1;
      unsigned pta_B5    : 5;
      unsigned csa_B5    : 5;
      unsigned           : 1;
      unsigned txsy_B1   : 1;
      unsigned txst_B1   : 1;
      unsigned pbsy_B1   : 1;
      unsigned           : 3;
      unsigned cmd_B4    : 4;
    } SUCC1_ST;
} SUCC1_UN;


/* SUC Configuration Register 2                                              */
/* 0x84 */
union succ2
{
  unsigned long SUCC2_UL;
  struct
    {
      unsigned           : 4;
      unsigned ltn_B4    : 4;
      unsigned           : 3;
      unsigned lt_B21    : 21;
    } SUCC2_ST;
} SUCC2_UN;


/* SUC Configuration Register 3                                              */
/* 0x88 */
union succ3
{
  unsigned long SUCC3_UL;
  struct
    {
      unsigned           : 24;
      unsigned wcf_B4    : 4;
      unsigned wcp_B4    : 4;
    } SUCC3_ST;
} SUCC3_UN;


/* NEM Configuration Register                                                */
/* 0x8C */
union nemc
{
  unsigned long NEMC_UL;
  struct
    {
      unsigned           : 28;
      unsigned nml_B4    : 4;
    } NEMC_ST;
} NEMC_UN;


/* PRT Configuration Register 1                                              */
/* 0x90 */
union prtc1
{
  unsigned long PRTC1_UL;
  struct
    {
      unsigned rwp_B6    : 6;
      unsigned           : 1;
      unsigned rxw_B9    : 9;
      unsigned brp_B2    : 2;
      unsigned           : 10;
      unsigned tsst_B4   : 4;
    } PRTC1_ST;
} PRTC1_UN;


/* PRT Configuration Register 2                                              */
/* 0x94 */
union prtc2
{
  unsigned long PRTC2_UL;
  struct
    {
      unsigned           : 2;
      unsigned txl_B6    : 6;
      unsigned txi_B8    : 8;
      unsigned           : 2;
      unsigned rxl_B6    : 6;
      unsigned           : 2;
      unsigned rxi_B6    : 6;
    } PRTC2_ST;
} PRTC2_UN;


/* MHD Configuration Register                                                */
/* 0x98 */
union mhdc
{
  unsigned long MHDC_UL;
  struct
    {
      unsigned           : 3;
      unsigned slt_B13   : 13;
      unsigned           : 9;
      unsigned sfdl_B7   : 7;
    } MHDC_ST;
} MHDC_UN;


unsigned      : 32;


/* GTU Configuration Register 1                                              */
/* 0xA0 */
union gtuc1
{
  unsigned long GTUC1_UL;
  struct
    {
      unsigned           : 12;
      unsigned ut_B20    : 20;
    } GTUC1_ST;
} GTUC1_UN;


/* GTU Configuration Register 2                                              */
/* 0xA4 */
union gtuc2
{
  unsigned long GTUC2_UL;
  struct
    {
      unsigned           : 12;
      unsigned snm_B4    : 4;
      unsigned           : 2;
      unsigned mpc_B14   : 14;
    } GTUC2_ST;
} GTUC2_UN;


/* GTU Configuration Register 3                                              */
/* 0xA8 */
union gtuc3
{
  unsigned long GTUC3_UL;
  struct
    {
      unsigned           : 10;
      unsigned mtio_B6   : 6;
      unsigned uiob_B8   : 8;
      unsigned uioa_B8   : 8;
    } GTUC3_ST;
} GTUC3_UN;


/* GTU Configuration Register 4                                              */
/* 0xAC */
union gtuc4
{
  unsigned long GTUC4_UL;
  struct
    {
      unsigned           : 2;
      unsigned ocs_B14   : 14;
      unsigned           : 2;
      unsigned nit_B14   : 14;
    } GTUC4_ST;
} GTUC4_UN;


/* GTU Configuration Register 5                                              */
/* 0xB0 */
union gtuc5
{
  unsigned long GTUC5_UL;
  struct
    {
      unsigned           : 1;
      unsigned dcc_B7    : 7;
      unsigned           : 3;
      unsigned cdd_B5    : 5;
      unsigned dcb_B8    : 8;
      unsigned dca_B8    : 8;
    } GTUC5_ST;
} GTUC5_UN;


/* GTU Configuration Register 6                                              */
/* 0xB4 */
union gtuc6
{
  unsigned long GTUC6_UL;
  struct
    {
      unsigned           : 5;
      unsigned mod_B11   : 11;
      unsigned           : 5;
      unsigned asr_B11   : 11;
    } GTUC6_ST;
} GTUC6_UN;


/* GTU Configuration Register 7                                              */
/* 0xB8 */
union gtuc7
{
  unsigned long GTUC7_UL;
  struct
    {
      unsigned           : 6;
      unsigned nss_B10   : 10;
      unsigned           : 5;
      unsigned ssl_B11   : 11;
    } GTUC7_ST;
} GTUC7_UN;


/* GTU Configuration Register 8                                              */
/* 0xBC */
union gtuc8
{
  unsigned long GTUC8_UL;
  struct
    {
      unsigned           : 3;
      unsigned nms_B13   : 13;
      unsigned           : 10;
      unsigned msl_B6    : 6;
    } GTUC8_ST;
} GTUC8_UN;


/* GTU Configuration Register 9                                              */
/* 0xC0 */
union gtuc9
{
  unsigned long GTUC9_UL;
  struct
    {
      unsigned           : 14;
      unsigned dsi_B2    : 2;
      unsigned           : 3;
      unsigned mapo_B5   : 5;
      unsigned           : 3;
      unsigned apo_B5    : 5;
    } GTUC9_ST;
} GTUC9_UN;


/* GTU Configuration Register 10                                             */
/* 0xC4 */
union gtuc10
{
  unsigned long GTUC10_UL;
  struct
    {
      unsigned           : 5;
      unsigned mrc_B11   : 11;
      unsigned           : 3;
      unsigned moc_B13   : 13;
    } GTUC10_ST;
} GTUC10_UN;


/* GTU Configuration Register 11                                             */
/* 0xC8 */
union gtuc11
{
  unsigned long GTUC11_UL;
  struct
    {
      unsigned           : 5;
      unsigned erc_B3    : 3;
      unsigned           : 5;
      unsigned eoc_B3    : 3;
      unsigned           : 14;
      unsigned ecc_B2    : 2;
    } GTUC11_ST;
} GTUC11_UN;


/* BGS Configuration Register                                                */
/* 0xCC */
union bgs
{
  unsigned long BGS_UL;
  struct
    {
      unsigned           : 22;
      unsigned dse_B1    : 1;
      unsigned bgd_B1    : 1;
      unsigned           : 2;
      unsigned bgt_B6    : 6;
    } BGS_ST;
} BGS_UN;


unsigned long RES2[12];       /* Reserved                                    */


/* CC Status Registers                                                       */
/* CC Status Vector                                                          */
/* 0x100 */
union ccsv
{
  unsigned long CCSV_UL;
  struct
    {
      unsigned           : 8;
      unsigned rca_B5    : 5;
      unsigned wsv_B3    : 3;
      unsigned dcreq_B1  : 1;
      unsigned csi_B1    : 1;
      unsigned csai_B1   : 1;
      unsigned csni_B1   : 1;
      unsigned           : 2;
      unsigned slm_B2    : 2;
      unsigned hrq_B1    : 1;
      unsigned fsi_B1    : 1;
      unsigned pocs_B6   : 6;
    } CCSV_ST;
} CCSV_UN;


/* CC Error Vector                                                           */
/* 0x104 */
union ccev
{
  unsigned long CCEV_UL;
  struct
    {
      unsigned           : 19;
      unsigned ptac_B5   : 5;
      unsigned errm_B2   : 2;
      unsigned           : 2;
      unsigned ccfc_B4   : 4;
    } CCEV_ST;
} CCEV_UN;


unsigned      : 32;
unsigned      : 32;


/* Slot Counter Value                                                        */
/* 0x110 */
union scv
{
  unsigned long SCV_UL;
  struct
    {
      unsigned           : 5;
      unsigned sccb_B11  : 11;
      unsigned           : 5;
      unsigned scca_B11  : 11;
    } SCV_ST;
} SCV_UN;


/* Macrotick and Cycle Counter Value                                         */
/* 0x114 */
union mtccv
{
  unsigned long MTCCV_UL;
  struct
    {
      unsigned           : 10;
      unsigned ccv_B6    : 6;
      unsigned           : 2;
      unsigned mtv_B14   : 14;
    } MTCCV_ST;
} MTCCV_UN;


/* Rate Correction Value                                                    */
/* 0x118 */
union rcv
{
  unsigned long RCV_UL;
  struct
    {
      unsigned           : 20;
      unsigned rcv_B12   : 12;
    } RCV_ST;
} RCV_UN;


/* Offset Correction Value                                                   */
/* 0x11C */
union ocv
{
  unsigned long OCV_UL;
  struct
    {
      unsigned           : 12;
      unsigned ocv_B14   : 20;
    } OCV_ST;
} OCV_UN;


/* Sync Frame Status                                                         */
/* 0x120 */
union sfs
{
  unsigned long SFS_UL;
  struct
    {
      unsigned           : 12;
      unsigned rclr_B1   : 1;
      unsigned mrcs_B1   : 1;
      unsigned olcr_B1   : 1;
      unsigned mocs_B1   : 1;
      unsigned vsbo_B4   : 4;
      unsigned vsbe_B4   : 4;
      unsigned vsao_B4   : 4;
      unsigned vsae_B4   : 4;
    } SFS_ST;
} SFS_UN;


/* Symbol Windows and NIT Status                                             */
/* 0x124 */
union swnit
{
  unsigned long SWNIT_UL;
  struct
    {
      unsigned           : 20;
      unsigned sbnb_B1   : 1;
      unsigned senb_B1   : 1;
      unsigned sbna_B1   : 1;
      unsigned sena_B1   : 1;
      unsigned           : 2;
      unsigned tcsb_B1   : 1;
      unsigned sbsb_B1   : 1;
      unsigned sesb_B1   : 1;
      unsigned tcsa_B1   : 1;
      unsigned sbsa_B1   : 1;
      unsigned sesa_B1   : 1;
    } SWNIT_ST;
} SWNIT_UN;


/* Aggregated Channel Status                                                 */
/* 0x128 */
union acs
{
  unsigned long ACS_UL;
  struct
    {
      unsigned           : 19;
      unsigned sbvb_B1   : 1;
      unsigned cib_B1    : 1;
      unsigned cedb_B1   : 1;
      unsigned sedb_B1   : 1;
      unsigned vfrb_B1   : 1;
      unsigned           : 3;
      unsigned sbva_B1   : 1;
      unsigned cia_B1    : 1;
      unsigned ceda_B1   : 1;
      unsigned seda_B1   : 1;
      unsigned vfra_B1   : 1;
    } ACS_ST;
} ACS_UN;


unsigned      : 32;


/* Even Sync ID [1..15]                                                      */
/* 0x130 .. 0x168 */
unsigned long ESID_UL[15];


unsigned      : 32;


/* Odd Sync ID [1..15]                                                       */
/* 0x170 .. 0x1A8 */
unsigned long OSID_UL[15];


unsigned      : 32;


/* Network Management Vector 1                                               */
/* 0x230 */
union nmv1
{
  unsigned long NMV1_UL;
} NMV1_UN;


/* Network Management Vector 2                                               */
/* 0x234 */
union nmv2
{
unsigned long NMV2_UL;
} NMV2_UN;


/* Network Management Vector 3                                               */
/* 0x238 */
unsigned long NMV3_UL;


unsigned long RES3[81];       /* Reserved                                    */

   
/* Message Buffer Control Registers                                          */
/* Message RAM Configuration                                                 */
/* 0x300 */
union mrc
{
  unsigned long MRC_UL;
  struct
    {
      unsigned           : 9;
      unsigned lcb_B7    : 7;
      unsigned           : 1;
      unsigned ffb_B7    : 7;
      unsigned           : 1;
      unsigned fdb_B7    : 7;
    } MRC_ST;
} MRC_UN;


/* FIFO Rejection Filter                                                     */
/* 0x304 */
union frf
{
  unsigned long FRF_UL;
  struct
    {
      unsigned           : 7;
      unsigned rnf       : 1;
      unsigned rss       : 1;
      unsigned cyf_B7    : 7;
      unsigned           : 3;
      unsigned fid_B11   : 11;
      unsigned ch_B2     : 2;
    } FRF_ST;
} FRF_UN;

/* FIFO Rejection Filter Mask                                                */
/* 0x308 */
union frfm
{
  unsigned long FRFM_UL;
  struct
    {
      unsigned           : 19;
      unsigned mfid_B11  : 11;
      unsigned           : 2;
    } FRFM_ST;
} FRFM_UN;


unsigned      : 32;

/* Message Buffer Status Registers                                           */

/* Message Handler Status                                                    */
/* 0x310 */
union mhds
{
  unsigned long MHDS_UL;
  struct
    {
      unsigned           : 2;
      unsigned mbu_B6    : 6;
      unsigned           : 2;
      unsigned mbt_B6    : 6;
      unsigned           : 2;
      unsigned fmb_B6    : 6;
      unsigned cram_B1   : 1;
      unsigned mfmb_B1   : 1;
      unsigned fmbd_B1   : 1;
      unsigned ptbf2_B1  : 1;
      unsigned ptbf1_B1  : 1;
      unsigned pmr_B1    : 1;
      unsigned pobf_B1   : 1;
      unsigned pibf_B1   : 1;
    } MHDS_ST;
} MHDS_UN;


unsigned      : 32;
unsigned      : 32;
unsigned      : 32;


/* Transmission Request Register 1                                           */
/* 0x320 */
union txrq1
{
  unsigned long TXRQ1_UL;
} TXRQ1_UN;


/* Transmission Request Register 2                                           */
/* 0x324 */
union txrq2
{
  unsigned long TXRQ2_UL;
} TXRQ2_UN;


unsigned      : 32;
unsigned      : 32;


/* New Data Register 1                                                       */
/* 0x330 */
union ndat1
{
  unsigned long NDAT1_UL;
} NDAT1_UN;


/* New Data Register 2                                                       */
/* 0x334 */
union ndat2
{
  unsigned long NDAT2_UL;
} NDAT2_UN;


unsigned      : 32;
unsigned      : 32;


/* Message Buffer Status Changed 1                                           */
/* 0x340 */
union mbsc1
{
  unsigned long MBSC1_UL;
} MBSC1_UN;


/* Message Buffer Status Changed 2                                           */
/* 0x344 */
union mbsc2
{
  unsigned long MBSC2_UL;
} MBSC2_UN;


unsigned long RES4[46];           /* Reserved                                */


/* Input Buffer                                                              */
/* Write Data Section [1..64]                                                */
/* 0x400 .. 0x4FC */
unsigned long WRDS[64];


/* Write Header Section 1                                                    */
/* 0x500 */
union wrhs1
{
  unsigned long WRHS1_UL;
  struct
    {
      unsigned           : 2;
      unsigned mbi_B1    : 1;
      unsigned txm_B1    : 1;
      unsigned ppit_B1    : 1;
      unsigned cfg_B1    : 1;
      unsigned chb_B1    : 1;
      unsigned cha_B1    : 1;
      unsigned           : 1;
      unsigned cyc_B7    : 7;
      unsigned           : 5;
      unsigned fid_B11   : 11;
    } WRHS1_ST;
} WRHS1_UN;


/* Write Header Section 2                                                    */
/* 0x504 */
union wrhs2
{
  unsigned long WRHS2_UL;
  struct
    {
      unsigned           : 9;
      unsigned pl_B7     : 7;
      unsigned           : 5;
      unsigned crc_B11   : 11;
    } WRHS2_ST;
} WRHS2_UN;


/* Write Header Section 3                                                    */
/* 0x508 */
union wrhs3
{
  unsigned long WRHS3_UL;
  struct
    {
      unsigned           : 21;
      unsigned dp_B11    : 11;
    } WRHS3_ST;
} WRHS3_UN;


unsigned      : 32;


/* Input Buffer Command Mask                                                 */
/* 0x510 */
union ibcm
{
  unsigned long IBCM_UL;
  struct
    {
      unsigned           : 13;
      unsigned stxrs_B1  : 1;
      unsigned ldss_B1   : 1;
      unsigned lhss_B1   : 1;
      unsigned           : 13;
      unsigned stxrh_B1  : 1;
      unsigned ldsh_B1   : 1;
      unsigned lhsh_B1   : 1;
    } IBCM_ST;
} IBCM_UN;


/* Input Buffer Command Request                                              */
/* 0x514 */
union ibcr
{
  unsigned long IBCR_UL;
  struct
    {
      unsigned ibsys_B1  : 1;
      unsigned           : 9;
      unsigned ibrs_B6   : 6;
      unsigned ibsyh_B1  : 1;
      unsigned           : 9;
      unsigned ibrh_B6   : 6;
    } IBCR_ST;
} IBCR_UN;


unsigned long RES5[58];           /* Reserved                                */


/* Output Buffer                                                             */
/* Read Data Section [1..64]                                                 */
/* 0x600 .. 0x6FC */
unsigned long RDDS[64];


/* Read Header Section 1                                                     */
/* 0x700 */
union rdhs1
{
  unsigned long RDHS1_UL;
  struct
    {
      unsigned           : 2;
      unsigned mbi_B1    : 1;
      unsigned txm_B1    : 1;
      unsigned ppit_B1    : 1;
      unsigned cfg_B1    : 1;
      unsigned chb_B1    : 1;
      unsigned cha_B1    : 1;
      unsigned           : 1;
      unsigned cyc_B7    : 7;
      unsigned           : 5;
      unsigned fid_B11   : 11;
    } RDHS1_ST;
} RDHS1_UN;


/* Read Header Section 2                                                     */
/* 0x704 */
union rdhs2
{
  unsigned long RDHS2_UL;
  struct
    {
      unsigned           : 1;
      unsigned plr_B7    : 7;
      unsigned           : 1;
      unsigned plc_B7    : 7;
      unsigned           : 5;
      unsigned crc_B11   : 11;
    } RDHS2_ST;
} RDHS2_UN;


/* Read Header Section 3                                                     */
/* 0x708 */
union rdhs3
{
  unsigned long RDHS3_UL;
  struct
    {
      unsigned           : 2;
      unsigned res_B1    : 1;
      unsigned ppi_B1    : 1;
      unsigned nfi_B1    : 1;
      unsigned syn_B1    : 1;
      unsigned sfi_B1    : 1;
      unsigned rci_B1    : 1;
      unsigned           : 2;
      unsigned rcc_B6    : 6;
      unsigned           : 5;
      unsigned dp_B11    : 11;
    } RDHS3_ST;
} RDHS3_UN;


/* Message Buffer Status                                                     */
/* 0x70C */
union mbs
{
  unsigned long MBS_UL;
  struct
    {
      unsigned           : 19;
      unsigned mlst_B1   : 1;
      unsigned esb_B1    : 1;
      unsigned esa_B1    : 1;
      unsigned tcib_B1   : 1;
      unsigned tcia_B1   : 1;
      unsigned svob_B1   : 1;
      unsigned svoa_B1   : 1;
      unsigned ceob_B1   : 1;
      unsigned ceoa_B1   : 1;
      unsigned seob_B1   : 1;
      unsigned seoa_B1   : 1;
      unsigned vfrb_B1   : 1;
      unsigned vfra_B1   : 1;
    } MBS_ST;
} MBS_UN;


/* Output Buffer Command Mask                                                */
/* 0x710 */
union obcm
{
  unsigned long OBCM_UL;
  struct
    {
      unsigned           : 14;
      unsigned rdsh_B1   : 1;
      unsigned rhsh_B1   : 1;
      unsigned           : 14;
      unsigned rdss_B1   : 1;
      unsigned rhss_B1   : 1;
    } OBCM_ST;
} OBCM_UN;


/* Output Buffer Command Request                                             */
/* 0x714 */
union obcr
{
  unsigned long OBCR_UL;
  struct
    {
      unsigned           : 10;
      unsigned obrh_B6   : 6;
      unsigned obsys_B1  : 1;
      unsigned           : 5;
      unsigned req_B1    : 1;
      unsigned view_B1   : 1;
      unsigned           : 2;
      unsigned obrs_B6   : 6;
    } OBCR_ST;
} OBCR_UN;

}FRAY_ST;

#define FRAY1 ((FRAY_ST *)0xFFF7C800U)

//**********************************************************
// Structure for initializing CC - Fr_Init - Fr_ConfigPtr
typedef volatile struct cfg
	{
	int mrc;
	int prtc1;
	int prtc2;
	int mhdc;
	int gtu1;
	int gtu2;
	int gtu3;
	int gtu4;
	int gtu5;
	int gtu6;
	int gtu7;
	int gtu8;
	int gtu9;
	int gtu10;
	int gtu11;
	int succ2;
	int succ3;
	} cfg;

// Structure for configuring buffers - Fr_PrepareLPdu - Fr_LPduPtr
typedef volatile struct wrhs
	{
	int mbi;
	int txm;
	int ppit;
	int cfg;
	int chb;
	int cha;
	int cyc;
	int fid;
	int pl;
	int crc;
	int dp;
	int sync;
	int sfi;
	} wrhs;

// Structure for initializing buffer - Fr_TransmitTxLPdu, Fr_ReceiveRxLPdu - Fr_LSduPtr
typedef volatile struct bc
	{
		int ibrh;
		int stxrh;
		int ldsh;
		int lhsh;
		int ibsyh;
		int ibsys;
		int obrs;
		int rdss;
		int rhss;
	} bc;

//**********************************************************
// Functions
int header_crc_calc(wrhs *Fr_LPduPtr);
void Fr_Init(FRAY_ST *Fray_PST, cfg *Fr_ConfigPtr);
int Fr_ControllerInit(FRAY_ST *Fray_PST);
void Fr_PrepareLPdu(FRAY_ST *Fray_PST, wrhs *Fr_LPduPtr);
int Fr_AllowColdStart(FRAY_ST *Fray_PST);
int Fr_StartCommunication(FRAY_ST *Fray_PST);
void Fr_TransmitTxLPdu(FRAY_ST *Fray_PST, bc *Fr_LSduPtr);
void Fr_ReceiveRxLPdu(FRAY_ST *Fray_PST, bc *Fr_LSduPtr);
void configure_initialize_node_a(FRAY_ST *Fray_PST);
void configure_initialize_node_b(FRAY_ST *Fray_PST);
int transmit_check_node_a(FRAY_ST *Fray_PST);
int transmit_check_node_b(FRAY_ST *Fray_PST);


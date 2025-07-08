/********************************************************************
  *** Copyright (C) 2024 Tejas Inc . All Rights Reserved
  ***
  *** $Id: srrm.h,v 1.2
  ***
  *** Description: Files contails HA implementation of sr
  *** NOTE: This file should included in release packaging.
  *** ***********************************************************************/

#ifndef _SRRM_H
#define _SRRM_H

#define SR_RM_MSG_HDR_SIZE           3
#define SR_RM_ILM_GBL_MOD            1
#define SR_RM_FTN_GBL_MOD            2
#define SR_RM_BULK_UPD_REQ_MSG_LEN   3
#define SR_RM_MOD_COMPLETED          3
#define SR_RM_BULK_TAIL_MSG_LEN      3
#define SR_RM_BULK_LEN_OFFSET        1
#define SR_RM_BULK_TYPE_OFFSET       1
#define SR_RM_MAX_BULK_SIZE          1500
#define SR_RM_BLKUPDT_NOT_STARTED    4

VOID SrProcessRmEvent(tSrRmEvtInfo *pRmSrMsg);
INT4 SrRmSendEventToRm(tRmProtoEvt * pEvt);
INT4 SrRmRegisterWithRM (VOID);
UINT4 SrRmGetRmNodeState (VOID);
INT4 SrRmDeRegisterWithRM(VOID);
INT4 SrRmRcvPktFromRm (UINT1 u1Event, tRmMsg * pData, UINT2 u2DataLen);
INT4 SrRmRelRmMsgMem (UINT1 *pu1Block);
VOID SrProgramMPLSentriesAfterGR (VOID);
VOID SrDeleteStaleEntries(VOID);

enum {
    SR_RED_BULK_UPDT_REQ_MSG  = RM_BULK_UPDT_REQ_MSG,
    SR_RED_BULK_UPDATE_MSG ,
    SR_RED_BULK_UPDT_TAIL_MSG ,
    SR_RED_ILM_HW_LIST_SYNC_MSG,
    SR_RED_FTN_HW_LIST_SYNC_MSG
};

enum {
    SR_RM_INIT = 1,
    SR_RM_ACTIVE_STANDBY_UP,
    SR_RM_ACTIVE_STANDBY_DOWN,
    SR_RM_STANDBY
};

#endif

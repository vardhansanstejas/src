/********************************************************************
 *** Copyright (C) 2024 Tejas . All Rights Reserved
 ***
 *** $Id: srrmport.c,v 1.1 2024/10/02 17:43:00  $
 ***
 *** Description: Files contails HA implementation of SR
 *** NOTE: This file should included in release packaging.
 *** ***********************************************************************/

#include "srincs.h"

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmRcvPktFromRm                                           */
/* Description  : This API constructs a message containing the               */
/*                given RM event and RM message. And posts it to the         */
/*                LDP RM queue                                               */
/*                                                                           */
/* Input        : u1Event   - Event type given by RM module                  */
/*              : pData     - RM Message to enqueue                          */
/*              : u2DataLen - Length of the message                          */
/*                                                                           */
/* Output       : None                                                       */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/
INT4
SrRmRcvPktFromRm (UINT1 u1Event, tRmMsg * pData, UINT2 u2DataLen)
{
    tSrQMsg *pRmSrMsg = NULL;

    if ((u1Event != RM_MESSAGE) && (u1Event != GO_ACTIVE) &&
        (u1Event != GO_STANDBY) && (u1Event != RM_PEER_UP) &&
        (u1Event != RM_PEER_DOWN) &&
        (u1Event != L2_INITIATE_BULK_UPDATES) &&
        (u1Event != RM_CONFIG_RESTORE_COMPLETE)
#ifdef TEJAS_WANTED
        && (u1Event != PLANNED_RESTART) &&
        (u1Event != STATIC_CONFIG_RESTORE_COMPLETE) &&
        (u1Event != REBOOT_MODE_WARM_REBOOT) &&
        (u1Event != GR_START) &&
        (u1Event != SYSTEM_READY)
#endif
      )
    {
        SR_TRC (SR_FAIL_TRC, "Invalid event received from RM\n");
        return OSIX_FAILURE ;
    }

    if ((u1Event == RM_MESSAGE) ||
        (u1Event == RM_PEER_UP) ||
        (u1Event == RM_PEER_DOWN))
    {
        if(pData == NULL)
        {
            SR_TRC (SR_FAIL_TRC, "NULL pointer message is received\n");
            return OSIX_FAILURE;
        }

        if (u1Event == RM_MESSAGE)
        {
            if (u2DataLen < SR_RM_MSG_HDR_SIZE)
            {
                SR_TRC (SR_FAIL_TRC, "Received invalid RM Message\n");
                RM_FREE (pData);
                return OSIX_FAILURE;
            }
        }
        else
        {
            if (u2DataLen != RM_NODE_COUNT_MSG_SIZE)
            {
                SrRmRelRmMsgMem ((UINT1 *) pData);
                SR_TRC (SR_FAIL_TRC, "Received invalid standby node information\n");
                return OSIX_FAILURE;
            }

        }
    }

    pRmSrMsg =  MemAllocMemBlk (SR_QDEPTH_QID);

    if (pRmSrMsg == NULL)
    {
        if (u1Event == RM_MESSAGE)
        {
            RM_FREE (pData);
        }
        else if ((u1Event == RM_PEER_UP) || (u1Event == RM_PEER_DOWN))
        {
            SrRmRelRmMsgMem ((UINT1 *) pData);
        }

        SR_TRC (SR_FAIL_TRC, "Memory allocation failed for message to post Sr Queue\n");
        return OSIX_FAILURE;
    }

    MEMSET (pRmSrMsg, 0, sizeof(tSrQMsg));
    pRmSrMsg->u4MsgType = SR_RM_MSG;
    pRmSrMsg->u.SrRmEvt.pBuff = pData;
    pRmSrMsg->u.SrRmEvt.u1Event = u1Event;
    pRmSrMsg->u.SrRmEvt.u2Length = u2DataLen;

    if(SrEnqueueMsgToSrQ (SR_MSG_EVT, pRmSrMsg) == SR_FAILURE)
    {
        if(MemReleaseMemBlock (SR_QDEPTH_QID, (UINT1 *) pRmSrMsg) == SR_MEM_FAILURE)
        {
            SR_TRC (SR_FAIL_TRC,"ERROR! Failed to release Memory for SrMsg\n");
            return OSIX_FAILURE;
        }

        if (u1Event == RM_MESSAGE)
        {
            RM_FREE (pData);
        }

        else if ((u1Event == RM_PEER_UP) || (u1Event == RM_PEER_DOWN))
        {
            SrRmRelRmMsgMem ((UINT1 *) pData);
        }

        SR_TRC (SR_FAIL_TRC, "Sending RM message to SR queue failed\n");
        return OSIX_FAILURE;
    }

    if(MemReleaseMemBlock (SR_QDEPTH_QID, (UINT1 *)pRmSrMsg) == SR_MEM_FAILURE)
    {
        SR_TRC (SR_FAIL_TRC, "ERROR! Failed to release the Memory for SrMsg\n");
        return OSIX_FAILURE;
    }

    SR_TRC (SR_CRITICAL_TRC, "SR receive packet from RM SUCCESS\n");
    return OSIX_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmRelRmMsgMem                                            */
/*                                                                           */
/* Description  : This function calls the RM API to release the memory       */
/*                allocated for RM message.                                  */
/*                                                                           */
/* Input        : pu1Block - Memory block to be released                     */
/*                                                                           */
/* Output       : None.                                                      */
/*                                                                           */
/* Returns      : SR_SUCCESS on Successfull release         .                */
/*                else returns SR_FAILURE.                                   */
/*                                                                           */
/*****************************************************************************/
INT4
SrRmRelRmMsgMem (UINT1 *pu1Block)
{
    UINT4   u4RetVal = RM_FAILURE;

    u4RetVal = RmReleaseMemoryForMsg (pu1Block);

    if (u4RetVal != RM_SUCCESS)
    {
        SR_TRC (SR_FAIL_TRC, "Releasing the RM message memory failed\n");
        return SR_FAILURE;
    }

    SR_TRC (SR_CRITICAL_TRC, "SR release RM message memory SUCCESS\n");
    return SR_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmSendEventToRm                                          */
/*                                                                           */
/* Description  : This function calls the RM API to post the event           */
/* Input        : u1Event - Event to be sent to RM. And the event can be     */
/*                any of the following                                       */
/*                RM_PROTOCOL_BULK_UPDT_COMPLETION                           */
/*                RM_BULK_UPDT_ABORT                                         */
/*                RM_STANDBY_TO_ACTIVE_EVT_PROCESSED                         */
/*                RM_IDLE_TO_ACTIVE_EVT_PROCESSED                            */
/*                RM_STANDBY_EVT_PROCESSED                                   */
/*                                                                           */
/*                u1Error - In case the event is RM_BULK_UPDATE_ABORT,       */
/*                the reason for the failure is send in the error. And       */
/*                the error can be any of the following,                     */
/*                RM_MEMALLOC_FAIL                                           */
/*                RM_SENTO_FAIL                                              */
/*                RM_PROCESS_FAIL                                            */
/*                                                                           */
/* Output       : None.                                                      */
/*                                                                           */
/* Returns      : SR_SUCCESS on Success                                      */
/*                else returns SR_FAILURE.                                   */
/*                                                                           */
/*****************************************************************************/
INT4
SrRmSendEventToRm (tRmProtoEvt * pEvt)
{
    INT4  i4RetVal = RM_FAILURE;

    i4RetVal = RmApiHandleProtocolEvent (pEvt);

    if (i4RetVal != RM_SUCCESS)
    {
        SR_TRC (SR_FAIL_TRC, "SR send event to RM FAILED\n");
        return SR_FAILURE;
    }

    SR_TRC (SR_CRITICAL_TRC, "SR send event to RM SUCCESS\n");
    return SR_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmRegisterWithRM                                         */
/*                                                                           */
/* Description  : This function calls the Redundancy Module API to register  */
/*                with the redundancy module.                                */
/*                                                                           */
/* Input        : None.                                                      */
/*                                                                           */
/* Output       : None.                                                      */
/*                                                                           */
/* Returns      : SR_SUCCESS on Successfull registration.                    */
/*                else returns SR_FAILURE.                                   */
/*                                                                           */
/*****************************************************************************/
INT4
SrRmRegisterWithRM (VOID)
{
    UINT4   u4RetVal = RM_FAILURE;

    tRmRegParams SrRmRegParams;

    SrRmRegParams.u4EntId = RM_SR_APP_ID;
    SrRmRegParams.pFnRcvPkt = SrRmRcvPktFromRm;

#ifdef TEJAS_WANTED
    tRtmAppGrInfo       RtmAppGrInfo;
#endif

    u4RetVal = RmRegisterProtocols (&SrRmRegParams);

    if (u4RetVal != RM_SUCCESS)
    {
        SR_TRC (SR_FAIL_TRC, "SR registration with RM failed\n");
        return SR_FAILURE;
    }

    gSrGlobalInfo.srRmInfo.u1AdminState = SR_ENABLED;

#ifdef TEJAS_WANTED
    MEMSET (&RtmAppGrInfo, 0, sizeof (tRtmAppGrInfo));
    RtmAppGrInfo.u1SrcEntId = RTM_GR_SR_APP_ID;
    RtmAppGrInfo.pFnRcvPkt = SrRmRcvPktFromRm;

    if (RtmAppGrRegisterProtcols (&RtmAppGrInfo) !=
                        RTM_SUCCESS)
    {
        SR_TRC (SR_FAIL_TRC, "SR registration with RTM failed\n");
        return SR_FAILURE;
    }
    else
    {
        SR_TRC (SR_CRITICAL_TRC, "SR registration with RTM Success\n");
    }
#endif
    SR_TRC (SR_CRITICAL_TRC, "SR registration with RM Success\n");
    return SR_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmDeRegisterWithRM                                       */
/*                                                                           */
/* Description  : This function calls the Redundancy Module API              */
/*                for de-registration                                        */
/*                                                                           */
/* Input        : None.                                                      */
/*                                                                           */
/* Output       : None.                                                      */
/*                                                                           */
/* Returns      : SR_SUCCESS on Successfull de-registration.                 */
/*                else returns SR_FAILURE.                                   */
/*                                                                           */
/*****************************************************************************/
INT4
SrRmDeRegisterWithRM (VOID)
{
    UINT4 u4RetVal = RM_FAILURE;

    u4RetVal = RmDeRegisterProtocols (RM_SR_APP_ID);

    if (u4RetVal != RM_SUCCESS)
    {
        SR_TRC (SR_FAIL_TRC, "SR de-registration with RM failed\n");
        return SR_FAILURE;
    }

    SR_TRC (SR_CRITICAL_TRC, "SR de-registration with RM success\n");
    return SR_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmGetRmNodeState                                         */
/*                                                                           */
/* Description  : This function calls the RM API to get the node state.      */
/*                                                                           */
/* Input        : None                                                       */
/* Output       : None.                                                      */
/* Returns      : Node state.                                                */
/*                                                                           */
/*****************************************************************************/
UINT4
SrRmGetRmNodeState ()
{
    UINT4  u4RmState = RM_ACTIVE;

    u4RmState = RmGetNodeState();

    return u4RmState;
}

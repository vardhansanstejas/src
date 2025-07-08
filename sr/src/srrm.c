
/********************************************************************
 *** Copyright (C) 2024 Tejas Inc . All Rights Reserved
 ***
 *** $Id: srrm.c
 ***
 *** Description: Files contails HA implementation of SR
 *** NOTE: This file should included in release packaging.
 *** ***********************************************************************/

#include "srincs.h"
#include "srrm.h"

VOID SrRmHandlePlannedRestartEvent(VOID);
VOID SrRmHandleGoActiveEvent (VOID);
VOID SrRmHandleConfRestoreCompEvt (VOID);
VOID SrRmHandleWarmRebootModeEvent (VOID);
VOID SrRmInitBulkUpdateFlags (VOID);
VOID SrRmHandleSystemReadyEvent (VOID);
VOID SrRmHandleStaticConfRestoreCompEvt (VOID);
VOID SrRmHandleGoStandbyEvent (VOID);
VOID SrRmHandleGrStartEvent (VOID);
VOID SrRmInit (VOID);

/*****************************************************************************/
/* Function     : SrProcessRmEvent                                           */
/*                                                                           */
/* Description  : This function receives the RM events and RM message.       */
/*                Based on the events it will call the corresponding         */
/*                functions to process the events.                           */
/* Input        : pRmSrMsg - RM Message                                      */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/
VOID
SrProcessRmEvent (tSrRmEvtInfo *pRmSrMsg)
{
    SR_TRC (SR_CRITICAL_TRC, "ENTRY:SrProcessRmEvent\n");

    switch(pRmSrMsg->u1Event)
    {
        case GO_ACTIVE:
            SR_TRC (SR_CRITICAL_TRC, "Received GO_ACTIVE event.\n");
            SrRmHandleGoActiveEvent();
            break;

        case GO_STANDBY:
            SR_TRC (SR_CRITICAL_TRC, "Received GO_STANDBY event.\n");
            SrRmHandleGoStandbyEvent();
            break;

#ifdef TEJAS_WANTED
        case RM_CONFIG_RESTORE_COMPLETE:
            SR_TRC (SR_CRITICAL_TRC, "Received RM_CONFIG_RESTORE_COMPLETE event.\n");
            SrRmHandleConfRestoreCompEvt();
            break;

       /*Warm reboot command handling*/
        case PLANNED_RESTART:
            SR_TRC (SR_CRITICAL_TRC, "Received PLANNED_RESTART event from RM\n");
            SrRmHandlePlannedRestartEvent();
            break;

        case STATIC_CONFIG_RESTORE_COMPLETE:
            SR_TRC (SR_CRITICAL_TRC, "Received STATIC_CONFIG_RESTORE_COMPLETE event from RM\n");
            SrRmHandleStaticConfRestoreCompEvt();
            break;

        case REBOOT_MODE_WARM_REBOOT:
            SR_TRC (SR_CRITICAL_TRC, "Received REBOOT_MODE_WARM_REBOOT event from RM\n");
            gSrGlobalInfo.u1RebootMode = SR_WARM_REBOOT;
            SrRmHandleWarmRebootModeEvent();
            break;

        case GR_START:
            SR_TRC (SR_CRITICAL_TRC, "Received GR_START event from RM\n");
            gSrGlobalInfo.u1IsGrStart = SR_TRUE;
            SrRmHandleGrStartEvent();
            break;

        case SYSTEM_READY:
            SR_TRC (SR_CRITICAL_TRC, "Received SYSTEM_READY event from RM\n");
            SrRmHandleSystemReadyEvent();
            break;
#endif /*End Of TEJAS_WANTED*/

        default:
            SR_TRC (SR_CRITICAL_TRC, "Received Invalid event.\n");
            break;

    }
    SR_TRC (SR_CRITICAL_TRC, "EXIT:SrProcessRmEvent\n");
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmHandleGrStartEvent                                     */
/*                                                                           */
/* Description  : This function handles the GR_START event from RM.          */
/*                                                                           */
/* Input        : None                                                       */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/
VOID
SrRmHandleGrStartEvent()
{
    SR_TRC (SR_CRITICAL_TRC, "ENTRY:SrRmHandleGrStartEvent\n");
#ifdef TEJAS_WANTED
    tRtmProtoEvt ProtoEvt;
    MEMSET (&ProtoEvt, 0, sizeof (tRtmProtoEvt));
    ProtoEvt.u1AppId = RTM_GR_SR_APP_ID;
#endif

	/*Unblock FTN, ILM creation calls*/
    SR_TRC (SR_CRITICAL_TRC, "Unblocking FTN/ILM creation for SR\n");
    gSrGlobalInfo.u1IsBlocked = SR_FALSE;

    /* Program all FTN, ILM entries which is learnt from the stored LSA */
    SR_TRC (SR_CRITICAL_TRC, "Programming MPLS entries after GR\n");
    SrProgramMPLSentriesAfterGR();

    /* Cleanup the stale entries */
    SR_TRC (SR_CRITICAL_TRC, "Deleteing Stale Entries\n");
    SrDeleteStaleEntries();

#ifdef TEJAS_WANTED
    /*Send Ack to RM*/
    ProtoEvt.u1Event = RM_GR_COMPLETED;
    ProtoEvt.u1Error = RM_NONE;
    SR_TRC (SR_CRITICAL_TRC, "Handled GR_START event from RM and sending ACK\n");

    /* send event to RM */
    if (RtmRedApiHandleProtocolEvent (&ProtoEvt) == RTM_FAILURE)
    {
        SR_TRC (SR_FAIL_TRC, "RtmRedApiHandleProtocolEvent failed\r\n");
    }
#endif
    SR_TRC (SR_CRITICAL_TRC, "EXIT:SrRmHandleGrStartEvent\n");
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmHandleGoActiveEvent                                    */
/*                                                                           */
/* Description  : This function handles the GO_ACTIVE event from RM.         */
/*                                                                           */
/* Input        : None                                                       */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/
VOID
SrRmHandleGoActiveEvent()
{
    tRmProtoEvt         ProtoEvt;
    UINT4               u4NodeState = gSrGlobalInfo.srRmInfo.u4SrRmState;

    MEMSET (&ProtoEvt, 0, sizeof (tRmProtoEvt));

    SR_TRC1 (SR_CRITICAL_TRC,"ENTRY: SrRmHandleGoActiveEvent: u4NodeState %d \n",u4NodeState);

    /*check the node state to process the event */
    if(u4NodeState == SR_RM_INIT)
    {
        /* consolidate the HW list and populate to  RBTree*/
        ProtoEvt.u4Event = RM_IDLE_TO_ACTIVE_EVT_PROCESSED;
        gSrGlobalInfo.srRmInfo.u4SrRmGoActiveReason = RM_IDLE_TO_ACTIVE_EVT_PROCESSED;
#ifdef TEJAS_WANTED
        /* During WR, SrGlobal DS would have been flushed out.
           Restore the MplsTnlIFIndex from the persistent DB */
        if(gSrGlobalInfo.u1RebootMode == SR_WARM_REBOOT)
        {
            /*Block FTN and ILM creation calls*/
            SR_TRC (SR_CRITICAL_TRC,"Blocking FTN/ILM create calls \n");
            gSrGlobalInfo.u1IsBlocked = SR_TRUE;

            /*Restore DataStructures*/
            SR_TRC (SR_CRITICAL_TRC,"Restoring SR DB Tables \n");
            if (SrDbRestoreAllTables () != SR_SUCCESS)
            {
                SR_TRC (SR_FAIL_TRC,"Restoration of TnlInfoTable failed \n");
                return;
            }
        }
#endif
    }
    else if (u4NodeState == SR_RM_STANDBY)
    {
        /* This part of code is only required when
           redudency architecture is of ISS */
        ProtoEvt.u4Event = RM_STANDBY_TO_ACTIVE_EVT_PROCESSED;
        gSrGlobalInfo.srRmInfo.u4SrRmGoActiveReason = RM_STANDBY_TO_ACTIVE_EVT_PROCESSED;
    }
    else
    {
        SR_TRC (SR_CRITICAL_TRC, "Received GO_ACTIVE event while the node is neither in INIT nor STANDBY state\n");
        return;
    }

    ProtoEvt.u4AppId = RM_SR_APP_ID;
    ProtoEvt.u4Error = RM_NONE;
    SR_TRC (SR_CRITICAL_TRC, "Handled GO_ACTIVE event from RM and sending ACK\n");

    /* send event to RM */
    (VOID)SrRmSendEventToRm (&ProtoEvt);

    SR_TRC1 (SR_CRITICAL_TRC, "Send event %u to RM Success\n", ProtoEvt.u4Event);
    SR_TRC (SR_CRITICAL_TRC, "EXIT: SrRmHandleGoActiveEvent\n");
    return;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmHandleGoStandbyEvent                                   */
/*                                                                           */
/* Description  : This function handles the GO_STANDBY event from RM         */
/*                                                                           */
/* Input        : None                                                       */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/
VOID
SrRmHandleGoStandbyEvent()
{
    tRmProtoEvt         ProtoEvt;
    UINT4               u4NodeState = gSrGlobalInfo.srRmInfo.u4SrRmState;

    MEMSET (&ProtoEvt, 0, sizeof (tRmProtoEvt));

    SR_TRC1 (SR_CRITICAL_TRC,"RpteRmHandleGoStandbyEvent: u4NodeState %d\n", u4NodeState);

    if(u4NodeState == SR_RM_STANDBY)
    {
        SR_TRC1 (SR_CRITICAL_TRC, "Received GO_STANDBY event when the node is :%s state\n", u4NodeState);
        return;
    }
    else
    {
#ifdef TEJAS_WANTED
        if (SrDbRestoreAllTables () != SR_SUCCESS)
        {
            SR_TRC (SR_CRITICAL_TRC, "ERROR! Restoration of TnlInfoTable failed \n");
            return;
        }
#endif
        gSrGlobalInfo.srRmInfo.u4SrRmState = SR_RM_STANDBY;

        /* send ACK to RM */
        ProtoEvt.u4AppId = RM_SR_APP_ID;
        ProtoEvt.u4Event = RM_STANDBY_EVT_PROCESSED;
        ProtoEvt.u4Error = RM_NONE;

        /* send event to RM */
        SR_TRC (SR_CRITICAL_TRC,"Handled GO_STANDBY event from RM and sending ACK\n");
        (VOID)SrRmSendEventToRm (&ProtoEvt);
    }

    return;
}

#ifdef TEJAS_WANTED
/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmHandlePlannedRestartEvent                              */
/*                                                                           */
/* Description  : This function handles PLANNED_RESTART event from RM by     */
/*                sending ack to RM                                          */
/*                                                                           */
/* Input        : None                                                       */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/
VOID
SrRmHandlePlannedRestartEvent (VOID)
{
    tRmProtoEvt ProtoEvt;

    MEMSET (&ProtoEvt, 0, sizeof (tRmProtoEvt));

    /*Send Ack to RM*/
    ProtoEvt.u4Event = RM_ACK_PLANNED_RESTART;
    ProtoEvt.u4AppId = RM_SR_APP_ID;
    ProtoEvt.u4Error = RM_NONE;

    SR_TRC (SR_CRITICAL_TRC, "Handled PLANNED_RESTART event from RM and sending ACK\n");

    /* send event to RM */
    (VOID)SrRmSendEventToRm (&ProtoEvt);

    return;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmHandleStaticConfRestoreCompEvt                         */
/*                                                                           */
/* Description  : This function handle the STATIC_CONFIG_RESTORE_COMPLETE    */
/*                event received from RM and sending ACK to RM               */
/*                                                                           */
/* Input        : None                                                       */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/
VOID
SrRmHandleStaticConfRestoreCompEvt (VOID)
{
    tRmProtoEvt         ProtoEvt;

    MEMSET (&ProtoEvt, 0, sizeof (tRmProtoEvt));

    /*Send Ack to RM*/
    ProtoEvt.u4Event = RM_RESTORE_COMPLETED;
    ProtoEvt.u4AppId = RM_SR_APP_ID;
    ProtoEvt.u4Error = RM_NONE;

    /* send event to RM */
    SR_TRC (SR_CRITICAL_TRC, "Handled STATIC_CONFIG_RESTORE_COMPLETE event from RM and sending ACK\n");
    (VOID)SrRmSendEventToRm (&ProtoEvt);

    return;
}
/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmHandleWarmRebootModeEvent                              */
/*                                                                           */
/* Description  : This function handles PLANNED_RESTART event from RM by     */
/*                sending ack to RM                                          */
/*                                                                           */
/* Input        : None                                                       */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/
VOID
SrRmHandleWarmRebootModeEvent (VOID)
{
    SR_TRC (SR_CRITICAL_TRC, "ENTRY: SrRmHandleWarmRebootModeEvent\n");
    tRmProtoEvt         ProtoEvt;

    MEMSET (&ProtoEvt, 0, sizeof (tRmProtoEvt));

    /*Send Ack to RM*/
    ProtoEvt.u4Event = RM_ACK_REBOOT_MODE_WARM_REBOOT;
    ProtoEvt.u4AppId = RM_SR_APP_ID;
    ProtoEvt.u4Error = RM_NONE;
    SR_TRC (SR_CRITICAL_TRC, "Handled REBOOT_MODE_WARM_REBOOT event from RM and sending ACK\n");

    /* send event to RM */
    (VOID)SrRmSendEventToRm (&ProtoEvt);

    SR_TRC (SR_CRITICAL_TRC, "EXIT: SrRmHandleWarmRebootModeEvent\n");
    return;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmInitBulkUpdateFlags                                    */
/*                                                                           */
/* Description  : This function initialize the flags used to bulk update.    */
/*                                                                           */
/* Input        : None                                                       */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/

VOID
SrRmInitBulkUpdateFlags(VOID)
{
    gSrGlobalInfo.srRmInfo.u1BulkUpdModuleStatus = SR_RM_ILM_GBL_MOD ;
    gSrGlobalInfo.srRmInfo.u4DynBulkUpdatStatus  = SR_RM_BLKUPDT_NOT_STARTED;

    return;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmHandleConfRestoreCompEvt                               */
/*                                                                           */
/* Description  : This function handle the event  RM_CONFIG_RESTORE_COMPLETE */
/*                received from the RM.Move the node to STANDBY state from   */
/*                INIT.                                                      */
/*                                                                           */
/* Input        : Nine                                                       */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/

VOID
SrRmHandleConfRestoreCompEvt()
{
    if ((gSrGlobalInfo.srRmInfo.u4SrRmState == SR_RM_INIT) &&
        (SrRmGetRmNodeState() == RM_STANDBY))
    {
        gSrGlobalInfo.srRmInfo.u4SrRmState =  SR_RM_STANDBY;
    }

    return;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmHandleSystemReadyEvent                                 */
/*                                                                           */
/* Description  : This function handle the SYSTEM_READY                      */
/*                event received from RM and sending ACK to RM               */
/*                                                                           */
/* Input        : None                                                       */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/
VOID
SrRmHandleSystemReadyEvent (VOID)
{
    gSrGlobalInfo.u1RebootMode    = SR_COLD_REBOOT;
    gSrGlobalInfo.u1IsGrStart     = SR_FALSE;
    gSrGlobalInfo.u1IsSystemReady = SR_TRUE;

    SR_TRC (SR_CRITICAL_TRC, "Handled SYSTEM_READY event from RM\r\n");

    return;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrRmInit                                                   */
/*                                                                           */
/* Description  : This function initialize the Sr RM related objects.        */
/*                                                                           */
/* Input        : None                                                       */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/
VOID
SrRmInit ()
{
    SR_TRC (SR_CRITICAL_TRC, "SrRmInit: Entry\n");
    MEMSET (&gSrGlobalInfo.srRmInfo, 0, sizeof(tSrRMInfo));

    /* Initialize the RM variables */
    gSrGlobalInfo.srRmInfo.u4SrRmState = SR_RM_INIT;
    gSrGlobalInfo.srRmInfo.u4PeerCount  = 0;
    gSrGlobalInfo.srRmInfo.u1AdminState = SR_DISABLED;
    SR_TRC (SR_CRITICAL_TRC, "SrRmInit: Exit\n");
}

#endif /*End Of TEJAS_WANTED*/

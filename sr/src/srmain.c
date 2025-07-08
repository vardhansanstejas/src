/********************************************************************
* Copyright (C) 2006 Altran Inc . All Rights Reserved
*
* $Id$
*
* Description:This file contains the sr task main loop.
*             and the initialisation routines.
*
*******************************************************************/

#define SR_MAIN_C

#include "srincs.h"
#include "srmain.h"
#include "srclip.h"
#include "fssrwr.h"
#include "rtm6.h"
#include "mplsdbinc.h"
#include "srdb.h"
#include "srrm.h"

UINT2               gu2SrLblGroupId;
tSrGlobalInfo       gSrGlobalInfo;
tTMO_SLL            gSrV6OspfNbrList;
tTMO_SLL            gSrV4OspfNbrList;
UINT4               gu4AddrV4Family = SR_ZERO;
UINT4               gu4AddrV6Family = SR_ZERO;
UINT4               gu4SegmentMpls = SR_ZERO;
extern tSrAltModuleStatus  gu4AltModuleStatus;

/******************************************************************************
 * Function Name      : SrMainCreateTaskMain
 *
 * Description        : This is the entry routine for the SR module. 
 *
 * Input(s)           : None
 *
 * Output(s)          : None
 *
 * Return Value(s)    : VOID
 *****************************************************************************/
PUBLIC VOID
SrMainCreateTaskMain (INT1 *pi1Arg)
{

    UNUSED_PARAM (pi1Arg);
    SR_TRC (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "ENTRY: SrMainCreateTaskMain \n");
    RegisterFSSR ();

    INT4  i4RetVal = SR_FAILURE;

    /* Initialize the SR Task */
    if (SrMainTaskInit () != SR_SUCCESS)
    {
        SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC, "SrMainTaskInit failed \n");
        SrMainTaskDeInit ();
        lrInitComplete (OSIX_FAILURE);
        return;
    }
    if (OsixTskIdSelf ((tOsixTaskId *) & SR_TASK_ID) == OSIX_FAILURE)
    {
        SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC, "Error in getting task Id for SR\n");
        SrMainTaskDeInit ();
        lrInitComplete (OSIX_FAILURE);
        return;
    }

    /**SR-TODO: need to HA flag? **/

#ifdef TEJAS_WANTED
    SrRmInit();

    i4RetVal = SrRmRegisterWithRM();

    if (i4RetVal != SR_SUCCESS)
    {
        lrInitComplete(OSIX_FAILURE);
        return;
    }
#endif

    lrInitComplete (OSIX_SUCCESS);
    SrMainTask ();

}

/******************************************************************************
 * Function Name      : SrMainMemInit
 *
 * Description        : This function is called to allocate memory pools for
 *                      the SR Task 
 * Input(s)           : None
 * Output(s)          : None
 * Return Value(s)    : None
 *****************************************************************************/
PUBLIC UINT4
SrMainMemInit (VOID)
{
    if (SrSizingMemCreateMemPools () != OSIX_SUCCESS)
    {
        SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                "SrMainMemInit: Memory Initialization Failed\n");
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/******************************************************************************
 * Function Name      : SrMainMemClear
 *
 * Description        : This function is called to de-allocate memory pools
 *                      allocated to the SR Task 
 * Input(s)           : None
 * Output(s)          : None
 * Return Value(s)    : None
 *****************************************************************************/
PUBLIC VOID
SrMainMemClear (VOID)
{
    SrSizingMemDeleteMemPools ();
}

/******************************************************************************
 * Function Name      : SrMainTaskInit
 *
 * Description        : This routine is used to initialize the data structure,
 *                      allocate memory pools,create Semaphore and message Queue.       
 *
 * Input(s)           : None
 *
 * Output(s)          : None
 *
 * Return Value(s)    : SR_SUCCESS /SR_FAILURE
 *****************************************************************************/
PUBLIC INT4
SrMainTaskInit (VOID)
{
    UINT4               u4OspfCxtId = SR_ZERO;

    /* Reset Global Info to all zeros */
    MEMSET (&gSrGlobalInfo, SR_ZERO, sizeof (tSrGlobalInfo));

    gSrGlobalInfo.u4SrTrcFlag = SR_ZERO;
    SR_TRC_FLAG |= SR_CRITICAL_TRC;

    /* Creating SR Queue */
    if (OsixCreateQ ((const UINT1 *) "SRQ", MAX_SR_QUEUE_SIZE,
                     (UINT4) 0, (tOsixQId *) & gSrGlobalInfo.gSrQId)
        != OSIX_SUCCESS)
    {
        SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC, "SrMainTaskInit: Queue Creation Failed\n");
        return SR_FAILURE;
    }

    /* Creating SR Semaphore */
    if (OsixCreateSem
        (SR_SEM_NAME, 1, 0,
         (tOsixSemId *) & gSrGlobalInfo.gSrSemId) != OSIX_SUCCESS)
    {
        SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC, "SrMainTaskInit: Semaphore Creation Failed\n");
        return SR_FAILURE;
    }

    if (SrMainMemInit () == SR_FAILURE)
    {
        SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC,
                "SrMainTaskInit: Memory Initialization Failed\n");
        SrMainTaskDeInit ();
        return SR_FAILURE;
    }

    gSrGlobalInfo.pV6SrSelfNodeInfo = NULL;

#ifdef TEJAS_WANTED
    /* Initialize the reboot mode as cold reboot and GR start message
     * received state as false */
    gSrGlobalInfo.u1RebootMode = SR_COLD_REBOOT;
    gSrGlobalInfo.u1IsGrStart = SR_FALSE;
#endif

    /*Dynamic-SR */
    TMO_SLL_Init (&(gSrV6OspfNbrList));
    TMO_SLL_Init (&(gSrV4OspfNbrList));
    TMO_SLL_Init (&(gSrGlobalInfo.routerList));
    /*Initialize Link info List to carry Adj SID in ext Link LSAs */
    TMO_SLL_Init (&(gSrGlobalInfo.linkInfoList));

    /* Initialize route delay list to delay route update to mpls
       db for LFA switch over */
    TMO_SLL_Init (&(gSrGlobalInfo.rtDelayList));
    gSrGlobalInfo.bIsDelayTimerStarted = SR_FALSE;

    gSrGlobalInfo.pSrSidRbTree =
        RBTreeCreate (SR_MAX_NUM_SID, SrUtilSidRbTreeCmpFunc);

    if (gSrGlobalInfo.pSrSidRbTree == NULL)
    {
        SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC,
                "SrMainTaskInit: gSrGlobalInfo.pSrSidRbTree == NULL\n");
        SrMainTaskDeInit ();
        return SR_FAILURE;
    }

    /* RbTree to store RLFA routes */
    gSrGlobalInfo.pSrRlfaPathRbTree =
        RBTreeCreate (SR_MAX_RLFA_PATH_ENTRIES, SrUtilRlfaPathRbTreeCmpFunc);

    if (gSrGlobalInfo.pSrRlfaPathRbTree == NULL)
    {
        SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC,
                "SrMainTaskInit: gSrGlobalInfo.pSrRlfaPathRbTree == NULL\n");
        SrMainTaskDeInit ();
        return SR_FAILURE;
    }

    /* RbTree to store SR TE routes */
    gSrGlobalInfo.pSrTeRouteRbTree =
        RBTreeCreateEmbedded (SR_OFFSET (tSrTeRtEntryInfo, RbNode),
                              SrUtilTeRouteRbTreeCmpFunc);
    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL)
    {
        SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC,
                "SrMainTaskInit: gSrGlobalInfo.pSrTeRouteRbTree == NULL\n");
        SrMainTaskDeInit ();
        return SR_FAILURE;
    }

    /* RbTree to store SR static binding Info */
    gSrGlobalInfo.pSrStaticRbTree =
        RBTreeCreateEmbedded (SR_OFFSET (tSrStaticEntryInfo, RbNode),
                              SrStaticRbTreeCmpFunc);
    if (gSrGlobalInfo.pSrStaticRbTree == NULL)
    {
        SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC,
                "SrMainTaskInit: gSrGlobalInfo.pSrStaticRbTree == NULL\n");
        SrMainTaskDeInit ();
        return SR_FAILURE;
    }

    gSrGlobalInfo.pSrOsRiLsRbTree =
        RBTreeCreateEmbedded (SR_OFFSET (tSrOsRiLsInfo, RbNode), 
                              SrUtilOsRiLsCmpFunc);

    if (gSrGlobalInfo.pSrOsRiLsRbTree == NULL)
    {
        SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC,
                "SrMainTaskInit: gSrGlobalInfo.pSrOsRiLsRbTree == NULL\n");
	}
	
    /* RbTree to store destinaitons has Ti-LFA back up path */
    gSrGlobalInfo.pSrTilfaPathRbTree =
        RBTreeCreateEmbedded (SR_OFFSET (tSrTilfaPathInfo, RbNode),
                              SrTiLfaPathRbTreeCmpFunc);
    if (gSrGlobalInfo.pSrTilfaPathRbTree == NULL)
    {
        SrMainTaskDeInit ();
        return SR_FAILURE;
    }

    /* RbTree to store LFA nexthop/P/Q node and its destinaitons */
    gSrGlobalInfo.pSrLfaNodeRbTree =
        RBTreeCreateEmbedded (SR_OFFSET (tSrLfaNodeInfo, RbNode),
                              SrLfaNodeTreeCmpFunc);
    if (gSrGlobalInfo.pSrLfaNodeRbTree == NULL)
    {
        SrMainTaskDeInit ();
        return SR_FAILURE;
    }

    /* RbTree to store LFA nexthop/P/Q node and its destinaitons */
    gSrGlobalInfo.pSrLfaAdjRbTree =
        RBTreeCreateEmbedded (SR_OFFSET (tSrLfaAdjInfo, RbNode),
                              SrLfaAdjTreeCmpFunc);
    if (gSrGlobalInfo.pSrLfaAdjRbTree == NULL)
    {
        SrMainTaskDeInit ();
        return SR_FAILURE;
    }

    /* RbTree to store FTN entry during WR */
    gSrGlobalInfo.SrFTNTnlDbTree =
        RBTreeCreateEmbedded (SR_OFFSET (tSrTnlIfTable, RbNode),
                              SrFTNRbTreeCmpFunc);
    if (gSrGlobalInfo.SrFTNTnlDbTree == NULL)
    {
        SR_TRC (SR_FAIL_TRC, "SrMainTaskInit: gSrGlobalInfo.SrFTNTnlDbTree is NULL\n");
        SrMainTaskDeInit ();
        return SR_FAILURE;
    }

    /* RbTree to store ILM entry during WR */
    gSrGlobalInfo.SrILMTnlDbTree =
        RBTreeCreateEmbedded (SR_OFFSET (tSrTnlIfTable, RbNode),
                              SrILMRbTreeCmpFunc);
    if (gSrGlobalInfo.SrILMTnlDbTree == NULL)
    {
        SR_TRC (SR_FAIL_TRC, "SrMainTaskInit: gSrGlobalInfo.SrILMTnlDbTree is NULL\n");
        SrMainTaskDeInit ();
        return SR_FAILURE;
    }
    if (TmrCreateTimerList ("SRT", SR_TIMER_EXP_EVENT,
                            NULL, &(SR_TMR_LIST_ID)) == TMR_FAILURE)
    {
        SR_TRC1 (SR_CRITICAL_TRC | SR_FAIL_TRC, "Timer list creation FAILED ID = %d!!!\n",
                SR_TMR_LIST_ID);
        SrMainTaskDeInit ();
        return SR_FAILURE;
    }

#ifdef TEJAS_WANTED
    /* Register call back function to DB for dynamic DB sync
     * to standby node and for table restoration purpose */

    if (SrDbRegAllRestoreTableNotifyCallback() != SR_SUCCESS)
    {
        SR_TRC (SR_FAIL_TRC,"Error - SrDbRegAllRestoreTableNotifyCallback fail.\n");
        SrMainTaskDeInit();
        return SR_FAILURE;
    }
#endif

#ifdef MPLS_WANTED
    gSrGlobalInfo.u4GenLblSpaceMinLbl =
        gSystemSize.MplsSystemSize.u4MinSrLblRange;
    gSrGlobalInfo.u4GenLblSpaceMaxLbl =
        gSystemSize.MplsSystemSize.u4MaxSrLblRange;
#endif
    gSrGlobalInfo.isLabelSpaceRsvd = SR_FALSE;

    gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex = SR_ZERO;
    gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex = SR_ZERO;
    gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex = SR_ZERO;
    gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex = SR_ZERO;
    IS_V4_SRGB_CONFIGURED = SR_FALSE;
    IS_V6_SRGB_CONFIGURED = SR_FALSE;
    gSrGlobalInfo.u4CliCmdMode = SR_FALSE;
    gSrGlobalInfo.u1StartupRILSASent = SR_FALSE;
    gSrGlobalInfo.u1StartupExtPrfLSASent = SR_FALSE;
    gSrGlobalInfo.u1StartupExtLinkLSASent = SR_FALSE;
    gSrGlobalInfo.u1AdjSidConfigured = SR_FALSE;
    gSrGlobalInfo.u1SrOspfDisable = SR_TRUE;
    gSrGlobalInfo.u1OspfDeRegInProgress = SR_FALSE;
    gSrGlobalInfo.u1Ospfv3DeRegInProgress = SR_FALSE;

    gSrGlobalInfo.u4SrV4ModuleStatus = SR_DISABLED;
    gSrGlobalInfo.u4SrV6ModuleStatus = SR_DISABLED;
    gu4AltModuleStatus.u4SrIPV4AltModuleStatus = ALTERNATE_ENABLED;
    gu4AltModuleStatus.u4SrIPV6AltModuleStatus = ALTERNATE_ENABLED;

    gSrGlobalInfo.SrContext.u2SrLblGroupId = SR_ZERO;
    gSrGlobalInfo.bIsMicroLoopAvd = SR_FALSE;
    SR_MODE = SR_MODE_DEFAULT;
    SR_IPV6_NODE_SID = SR_ZERO;
    for (u4OspfCxtId = 0; u4OspfCxtId < SYS_DEF_MAX_NUM_CONTEXTS; u4OspfCxtId++)
    {
        gSrGlobalInfo.u1SrOspfRegState[u4OspfCxtId] = SR_FALSE;
    }
    return SR_SUCCESS;
}

/******************************************************************************
 * Function Name      : SrMainTaskDeInit
 *
 * Description        : This routine is used to de-initialize the data structure,
 *                      clear memory pools, delete Semaphore and message Queue.       
 *
 * Input(s)           : None
 *
 * Output(s)          : None
 *
 * Return Value(s)    : SR_SUCCESS /SR_FAILURE
 *****************************************************************************/
VOID
SrMainTaskDeInit (VOID)
{
    if (SR_INITIALISED == TRUE)
    {
        nmhSetFsSrMode (SR_MODE_DEFAULT);

        if (SrSndMsgToOspf (SR_OSPF_DEREG, SR_DEFAULT, NULL) != SR_SUCCESS)
        {
            SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC, "De-Registeration failed\n");
            return;
        }

        RBTreeDelete (gSrGlobalInfo.pSrSidRbTree);
        RBTreeDelete (gSrGlobalInfo.pSrRlfaPathRbTree);
        RBTreeDelete (gSrGlobalInfo.pSrTeRouteRbTree);
        RBTreeDelete (gSrGlobalInfo.pSrOsRiLsRbTree);

        /* Delete the SR Semaphore */
        if (gSrGlobalInfo.gSrSemId != SR_ZERO)
        {
            OsixSemDel (gSrGlobalInfo.gSrSemId);
            gSrGlobalInfo.gSrSemId = SR_ZERO;
        }

        /* Delete the Queue */
        if (gSrGlobalInfo.gSrQId != SR_ZERO)
        {
            OsixQueDel (gSrGlobalInfo.gSrQId);
            gSrGlobalInfo.gSrQId = SR_ZERO;
        }
        UnRegisterFSSR ();
        SrMainMemClear ();
        SrRmDeRegisterWithRM();
    }

}

/******************************************************************************
 * Function Name      : SrQueueHandler
 *
 * Description        : This process messages on the Queue
 *
 * Input(s)           : pQMsg - pointer to the message received
 * Output(s)          : None
 * Return Value(s)    : None
 *****************************************************************************/
PUBLIC VOID
SrQueueHandler (tSrQMsg * pQMsg)
{
    tSrRouteEntryInfo  *pRouteInfo = NULL;
    UINT1               u1LSAType = TYPE10_OPQ_LSA;
    INT4                i4SrV4Status;
    INT4                i4SrV6Status;
    INT4                i4RetVal;

    SR_TRC1 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s ENTRY \n", __func__);
    i4SrV4Status = (INT4) gSrGlobalInfo.u4SrV4ModuleStatus;
    i4SrV6Status = (INT4) gSrGlobalInfo.u4SrV6ModuleStatus;

    pRouteInfo = (tSrRouteEntryInfo *) (VOID *) pQMsg->u.SrIpEvtInfo.pRouteInfo;

    switch (pQMsg->u4MsgType)
    {
        case SR_IP_EVENT:
            if ((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
            {
                SrProcessIpEvents (&pQMsg->u.SrIpEvtInfo);
            }
            /* Free RouteInfo Memory that has been allocated at IP Rt Event */
            SR_RTINFO_MEM_FREE (pRouteInfo);
            break;
        case SR_IP_LFA_EVENT:
            if ((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
            {
                SrProcessNetIpLfaEvents (&pQMsg->u.SrNetIpLfaRtInfo);
            }
            break;

        case SR_IP6_EVENT:
            if ((SR_MODE & SR_CONF_OSPFV3) == SR_CONF_OSPFV3)
            {
                SrProcessIp6Events (&pQMsg->u.SrIpEvtInfo);
            }
            /* Free RouteInfo Memory that has been allocated at IP Rt Event */
            SR_RTINFO_MEM_FREE (pRouteInfo);
            break;
        case SR_NETIP6_EVENT:
            if ((SR_MODE & SR_CONF_OSPFV3) == SR_CONF_OSPFV3)
            {
                SrProcessNetIp6LfaEvents (&pQMsg->u.SrNetIpv6LfaRtInfo);
            }
            break;
        case SR_OSPF_UP_EVENT:
            SR_TRC2 (SR_CTRL_TRC, "%s:%d SR_OSPF_UP_EVENT Received \n", __func__,
                     __LINE__);
            if (i4SrV4Status == SR_ENABLED)
            {

                if ((SR_RI_LSA_SENT & SR_EXT_PRF_LSA_SENT) &&
                    (SR_FAILURE == SrUtilSendLSA (NEW_LSA, SR_ZERO, u1LSAType)))
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrUtilSendLSA NEW_LSA Failed \n", __func__,
                             __LINE__);
                }
            }
            break;

        case SR_OSPFV3_UP_EVENT:
            SR_TRC2 (SR_MAIN_TRC, "%s:%d SR_OSPF_UP_EVENT Received \n", __func__,
                     __LINE__);
            if (i4SrV6Status == SR_ENABLED)
            {
                if (SrV3UtilSendLSA (NEW_LSA, SR_ZERO) == SR_FAILURE)
                {
                    SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC,
                             "%s:%d SrV3UtilSendLSA NEW_LSA Failed \n",
                             __func__, __LINE__);
                }
            }
            break;

        case SR_CFA_EVENT:
            SR_TRC2 (SR_MAIN_TRC, "%s:%d SR_CFA EVENT Received \n", __func__,
                     __LINE__);
            SrUtilHandleCfaIfStatusChange (pQMsg->u.SrCfaInfo.i4IfIndex);
            break;

        case SR_OSPF_STATE_EVENT:
            SR_TRC2 (SR_MAIN_TRC, "%s:%d SR_OSPF_STATE_EVENT Received \n",
                     __func__, __LINE__);
            i4RetVal =
                (INT4) SrSetOspfSrStatus (pQMsg->u4OspfCxtId, pQMsg->u4Status);
            if (i4RetVal == SR_FAILURE)
            {
                SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC, "%s:%d Unable to set SR status \n",
                         __func__, __LINE__);
            }
            break;

        case SR_OSPFV3_STATE_EVENT:
            SR_TRC2 (SR_MAIN_TRC, "%s:%d SR_OSPFV3_STATE_EVENT Received \n",
                     __func__, __LINE__);

            i4RetVal =
                (INT4) SrV3SetOspfSrStatus (pQMsg->u4OspfCxtId,
                                            pQMsg->u4Status);
            if (i4RetVal == SR_FAILURE)
            {
                SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC, "%s:%d Unable to set SR status \n",
                         __func__, __LINE__);
            }
            break;
        case SR_OSPFV3_ALT_STATE_EVENT:
            SR_TRC2 (SR_MAIN_TRC, "%s:%d SR_OSPFV3_ALT_STATE_EVENT Received \n",
                     __func__, __LINE__);
            i4RetVal = SrCliConfigV6Alternate (pQMsg->u4Status);
            if (i4RetVal == SR_FAILURE)
            {
                SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC, "%s:%d Unable to set Alt SR status \n",
                         __func__, __LINE__);
            }
            break;
        case SR_OSPF_TE_ADD_EVENT:

            if ((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
            {
                if (SrCliTestAndSetTeConfig
                    (&pQMsg->u.SrTeRtInfo.destAddr,
                     pQMsg->u.SrTeRtInfo.u4DestMask,
                     &pQMsg->u.SrTeRtInfo.mandatoryRtrId,
                     (pQMsg->u.SrTeRtInfo.au4OptRouterIds),
                     pQMsg->u.SrTeRtInfo.u1NoOfOptRouters) == SR_FAILURE)
                {
                    SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC, "%s:%d SR TE SET failure \n", __func__,
                             __LINE__);
                }

            }
            break;
        case SR_OSPF_TE_DEL_EVENT:

            if ((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
            {
                if (SrCliTestAndDeleteTeConfig
                    (&pQMsg->u.SrTeRtInfo.destAddr, SR_ZERO) == SR_FAILURE)
                {
                    SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC, "%s:%d SR TE DELETE failure \n",
                             __func__, __LINE__);
                }

            }
            break;

        case SR_OSPF_MSG_EVENT:

            SR_TRC2 (SR_MAIN_TRC, "%s:%d SR_OSPF_MSG_EVENT Received \n", __func__,
                     __LINE__);
            SrOspfMsgHandler (&pQMsg->u.OstoSrMsgParams);
            break;

        case SR_RM_MSG:

            SR_TRC2 (SR_MAIN_TRC, "%s:%d SR_RM_MSG Received \n", __func__, __LINE__);
            SrProcessRmEvent(&pQMsg->u.SrRmEvt);
            break;

        case SR_OSPF_RTR_DISABLE_EVENT:
            SR_TRC2 (SR_MAIN_TRC, "%s:%d SR_OSPF_RTR_DISABLE_EVENT Received\n",
                     __func__, __LINE__);
            /* While OSPF rtr disable - all self originated LSAs will get flushed.
               Later while processing router lsa in opaque module, type-10 LSAs 
               will get regenerate. 
               These flags should be resetted for regenerating LSAs. */
            if ((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
            {
                gSrGlobalInfo.u1StartupRILSASent = SR_FALSE;
                gSrGlobalInfo.u1StartupExtPrfLSASent = SR_FALSE;
                gSrGlobalInfo.u1StartupExtLinkLSASent = SR_FALSE;
            }
            break;

        default:
            break;
    }
}

PUBLIC VOID
SRProcessDelayedRouteInfo(tSrRtrInfo  *pRtrInfo)
{
    tTMO_SLL_NODE      *pSrRtrNextHopNode    = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo    = NULL;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrRouteEntryInfo  *pRouteInfo = NULL;
    tSrDelayRtInfo     *pRtInfo     = NULL;
    tSrDelayRtInfo     *pTempRtInfo = NULL;
    tSrIpEvtInfo        SrIpEvtInfo;
    UINT4               u4DestAddr;
    UINT4               u4PrefixId;
    UINT1               bNhFound    = SR_FALSE;
    UINT1               bDestFound  = SR_FALSE;

    MEMCPY (&u4PrefixId, &pRtrInfo->prefixId, sizeof (UINT4));

    TMO_DYN_SLL_Scan (&(gSrGlobalInfo.rtDelayList), pRtInfo, pTempRtInfo,
                      tSrDelayRtInfo *)
    {
        bDestFound = SR_FALSE;
        bNhFound   = SR_FALSE;
        pRouteInfo = (tSrRouteEntryInfo *) (VOID *) &(pRtInfo->srRouteInfo);
        u4DestAddr = pRouteInfo->destAddr.Addr.u4Addr;

        if (u4DestAddr == u4PrefixId)
        {   bDestFound = SR_TRUE;
            TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                    pSrRtrNextHopNode, tTMO_SLL_NODE *)
            {
                pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
                if ((pRouteInfo->u4OldNextHop == pSrRtrNextHopInfo->nextHop.Addr.u4Addr) &&
                    (pSrRtrNextHopInfo->bIsLfaNextHop != TRUE))
                {
                    bNhFound  = SR_TRUE;
                    break;
                }
            }
            break;
        }
    }

    if ((SR_FALSE == bDestFound) || (SR_FALSE == bNhFound))
    {
        SR_TRC6 (SR_UTIL_TRC | SR_CRITICAL_TRC,
            "%s:%d Deleted Prefix %x from Delay List as Rtr node/Old NH %x not found (%x %x)\n" ,
            __func__, __LINE__, u4DestAddr, pRouteInfo->u4OldNextHop, bDestFound, bNhFound);
        if (bDestFound = SR_TRUE)
        {
            TMO_SLL_Delete (&(gSrGlobalInfo.rtDelayList),
                            (tTMO_SLL_NODE *) pRtInfo);
            SR_RTR_DELAY_RT_INFO_MEM_FREE (pRtInfo);
        }

        pRtrInfo->u1OprFlags &= ~SR_OPR_F_ADD_DELAY_LIST;
        return;
    }

    SR_TRC4 (SR_UTIL_TRC | SR_CTRL_TRC,
        "%s:%d Processing Prefix %x Rt Id %x from Delay List \n" ,
        __func__, __LINE__, u4DestAddr, pRtrInfo->advRtrId.Addr.u4Addr);

    SrIpEvtInfo.pRouteInfo            = &(pRtInfo->srRouteInfo);
    SrIpEvtInfo.u4IfIndex             = pRtInfo->u4IfIndex;
    SrIpEvtInfo.u4BitMap              = pRtInfo->u4BitMap;
    SrIpEvtInfo.u4IpAddr.Addr.u4Addr  = pRtInfo->u4IpAddr.Addr.u4Addr;
    SrIpEvtInfo.u4IpAddr.u2AddrType   = pRtInfo->u4IpAddr.u2AddrType;
    SrProcessFRRModifyRoute(&SrIpEvtInfo, pRtrInfo, pSrRtrNextHopInfo);
    TMO_SLL_Scan (&(pRtrInfo->NextHopList), pNewSrRtrNextHopInfo, tTMO_SLL_NODE *)
    {
        pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
        if (pSrRtrNextHopInfo->bIsLfaNextHop != TRUE)
        {
            SrLfaCreateLSP (pRtrInfo, pNewSrRtrNextHopInfo);
            break;
        }
    }

    TMO_SLL_Delete (&(gSrGlobalInfo.rtDelayList), (tTMO_SLL_NODE *) pRtInfo);
    /* Reset the delay list flag */
    pRtrInfo->u1OprFlags &= ~SR_OPR_F_ADD_DELAY_LIST;
    SR_TRC4 (SR_UTIL_TRC | SR_CTRL_TRC,
        "%s:%d Deleted Prefix %x Rt Id %x from Delay List \n" ,
        __func__, __LINE__, u4DestAddr, pRtrInfo->advRtrId.Addr.u4Addr);
    SR_RTR_DELAY_RT_INFO_MEM_FREE (pRtInfo);
    return;
}

PUBLIC VOID
SrProcessTimerEvent ()
{
    tTMO_SLL_NODE      *pRtrNode             = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode    = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo    = NULL;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRouteEntryInfo  *pRouteInfo = NULL;
    tSrDelayRtInfo     *pRtInfo     = NULL;
    tSrDelayRtInfo     *pTempRtInfo = NULL;
    tSrIpEvtInfo        SrIpEvtInfo;
    UINT4               u4DestAddr;
    UINT4               u4PrefixId;
    UINT1               bRtrFound = SR_FALSE;
    UINT1               bNhFound  = SR_FALSE;
    gSrGlobalInfo.bIsDelayTimerStarted = SR_FALSE;
    TMO_DYN_SLL_Scan (&(gSrGlobalInfo.rtDelayList), pRtInfo, pTempRtInfo,
                      tSrDelayRtInfo *)
    {
        pRouteInfo = (tSrRouteEntryInfo *) (VOID *) &(pRtInfo->srRouteInfo);
        u4DestAddr = pRouteInfo->destAddr.Addr.u4Addr;
        SR_TRC3 (SR_UTIL_TRC | SR_CTRL_TRC, "%s:%d Prefix %x from Delay List \n",
                __func__, __LINE__, u4DestAddr);
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            bRtrFound = SR_FALSE;
            bNhFound   = SR_FALSE;
            pRtrInfo = (tSrRtrInfo *) pRtrNode;
            MEMCPY (&u4PrefixId, &pRtrInfo->prefixId, sizeof (UINT4));
            if (u4DestAddr == u4PrefixId)
            {
                TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                        pSrRtrNextHopNode, tTMO_SLL_NODE *)
                {
                    pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
                    if ((pRouteInfo->u4OldNextHop == pSrRtrNextHopInfo->nextHop.Addr.u4Addr) &&
                        (pSrRtrNextHopInfo->bIsLfaNextHop != TRUE))
                    {
                        bNhFound  = SR_TRUE;
                        break;
                    }
                }
                bRtrFound  = SR_TRUE;
                break;
            }
        }
        if ((SR_FALSE == bRtrFound) || (SR_FALSE == bNhFound))
        {
            SR_TRC6 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                "%s:%d Deleted Prefix %x from Delay List as Rtr node/Old NH %x not found (%x %x)\n" ,
                __func__, __LINE__, u4DestAddr, pRouteInfo->u4OldNextHop, bRtrFound, bNhFound);
            TMO_SLL_Delete (&(gSrGlobalInfo.rtDelayList), (tTMO_SLL_NODE *) pRtInfo);
            if ((SR_TRUE == bRtrFound) && (NULL != pRtrInfo))
            {
                pRtrInfo->u1OprFlags &= ~SR_OPR_F_ADD_DELAY_LIST;
            }
            SR_RTR_DELAY_RT_INFO_MEM_FREE (pRtInfo);
            continue;
        }
        SR_TRC4 (SR_UTIL_TRC | SR_CTRL_TRC,
            "%s:%d Processing Prefix %x Rt Id %x from Delay List \n" ,
            __func__, __LINE__, u4DestAddr, pRtrInfo->advRtrId.Addr.u4Addr);
        SrIpEvtInfo.pRouteInfo            = &(pRtInfo->srRouteInfo);
        SrIpEvtInfo.u4IfIndex             = pRtInfo->u4IfIndex;
        SrIpEvtInfo.u4BitMap              = pRtInfo->u4BitMap;
        SrIpEvtInfo.u4IpAddr.Addr.u4Addr  = pRtInfo->u4IpAddr.Addr.u4Addr;
        SrIpEvtInfo.u4IpAddr.u2AddrType   = pRtInfo->u4IpAddr.u2AddrType;
        SrProcessFRRModifyRoute(&SrIpEvtInfo, pRtrInfo, pSrRtrNextHopInfo);
        TMO_SLL_Scan (&(pRtrInfo->NextHopList), pNewSrRtrNextHopInfo, tTMO_SLL_NODE *)
        {
            pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
            if (pSrRtrNextHopInfo->bIsLfaNextHop != TRUE)
            {
                SrLfaCreateLSP (pRtrInfo, pNewSrRtrNextHopInfo);
                break;
            }
        }
        TMO_SLL_Delete (&(gSrGlobalInfo.rtDelayList), (tTMO_SLL_NODE *) pRtInfo);
        /* Reset the delay list flag */
        pRtrInfo->u1OprFlags &= ~SR_OPR_F_ADD_DELAY_LIST;
        SR_TRC4 (SR_UTIL_TRC | SR_CTRL_TRC,
            "%s:%d Deleted Prefix %x Rt Id %x from Delay List \n" ,
            __func__, __LINE__, u4DestAddr, pRtrInfo->advRtrId.Addr.u4Addr);
        SR_RTR_DELAY_RT_INFO_MEM_FREE (pRtInfo);
    }
    return;
}
/******************************************************************************
 * Function Name      : SrProcessQMsg
 *
 * Description        : This routine deque the Queue Messages and calls
 *                      QueueHandler process the messages.
 *
 * Input(s)           : u4Event - Event to indicate messages present in the
 *                      message queues.
 * Output(s)          : None
 * Return Value(s)    : None
 *****************************************************************************/
PUBLIC INT4
SrProcessQMsg (UINT4 u4Event)
{
    tSrQMsg            *pQMsg = NULL;

    if (u4Event & SR_EV_ALL)
    {
        while (OsixQueRecv (gSrGlobalInfo.gSrQId, (UINT1 *) &pQMsg,
                            OSIX_DEF_MSG_LEN, SR_ZERO) == OSIX_SUCCESS)
        {
            SR_LOCK ();
            SrQueueHandler (pQMsg);
            SR_UNLOCK ();
            SR_Q_MEM_FREE (pQMsg);

#if 0
            MemReleaseMemBlock (SrMemPoolIds[MAX_SR_QUEUE_SIZE_SIZING_ID],
                                (UINT1 *) pQMsg);
#endif
        }
    }
    return SR_SUCCESS;
}

/******************************************************************************
 * Function Name      : SrMainTask
 *
 * Description        : This routine is responsible for handling of events 
 *             received by SR task.
 *
 * Input(s)           : None.
 * Output(s)          : None
 * Return Value(s)    : None
 *****************************************************************************/
PUBLIC VOID
SrMainTask (VOID)
{
    UINT4               u4RcvEvents = SR_ZERO;

    SR_INITIALISED = TRUE;
    while (SR_TRUE)
    {
        if (OsixEvtRecv (SR_TASK_ID, SR_EV_ALL,
                         OSIX_WAIT, &u4RcvEvents) == OSIX_SUCCESS)
        {
            if (u4RcvEvents == SR_TIMER_EXP_EVENT)
            {
                SR_LOCK ();
                SrProcessTimerEvent();
                SR_UNLOCK ();
            }
            else
            {
            SrProcessQMsg (u4RcvEvents);
            }
        }
    }
}

/*****************************************************************************/
/* Function Name : SrStartSrTask                                           */
/* Description   : This function Starts the SR Task                         */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : OSIX_FAILURE / OSIX_SUCCESS                               */
/*****************************************************************************/
PUBLIC INT4
SrStartTask (VOID)
{
    INT4                i4RetVal;

    SR_TRC (SR_MAIN_TRC, "SrStartTask: " "SR Component Started.\n");

    i4RetVal = (INT4) OsixTskCrt ((UINT1 *) "SRT", 100 | OSIX_SCHED_RR,
                                  (UINT4) (3 * OSIX_DEFAULT_STACK_SIZE),
                                  (OsixTskEntry) SrMainCreateTaskMain, 0,
                                  &SR_TASK_ID);

    return i4RetVal;
}

/*****************************************************************************/
/* Function Name : SrRegisterWithRtmForAlt                                   */
/* Description   : Function for registering with Rtm.                        */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrRegisterWithRtmForAlt ()
{
    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d: Entry\n", __func__, __LINE__);
    tLfaRouteRegInfo    LfaRouteRegInfo;

    MEMSET (&LfaRouteRegInfo, 0, sizeof (tLfaRouteRegInfo));

    /* Ask for Interface Status Change and Route Change Events */
    LfaRouteRegInfo.u1ProtoId = MPLS_IP_PROTOCOL_ID;
    LfaRouteRegInfo.pLfaChng = MplsIpLfaRtChgEventHandler;

    if (RtmRegisterHigherLayerProtocolForLfa (&LfaRouteRegInfo) == RTM_FAILURE)
    {
        SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC, "%s:%d: Registration with RTM failed\n", __func__,
                 __LINE__);
        return;
    }
    SR_TRC2 (SR_MAIN_TRC, "%s:%d: Registration with RTM complete\n", __func__,
             __LINE__);
    return;
}

/*****************************************************************************/
/* Function Name : SrDeRegisterWithRtmForAlt                                 */
/* Description   : Function for Deregistering with Rtm.                      */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrDeRegisterWithRtmForAlt ()
{
    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d: Entry\n", __func__, __LINE__);
    UINT4               u4ContextId = SR_ZERO;
    UINT1               u1ProtoId = MPLS_IP_PROTOCOL_ID;

    if (RtmDeRegisterHigherLayerProtocolForLfa (u4ContextId, u1ProtoId) ==
        RTM_FAILURE)
    {
        SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC, "%s:%d: Deregistration with RTM failed\n",
                 __func__, __LINE__);
        return;
    }

    SR_TRC2 (SR_MAIN_TRC, "%s:%d:Deregistration done with RTM \n", __func__,
             __LINE__);
    return;
}

/*****************************************************************************/
/* Function Name : SrV3RegisterWithRtmForAlt                                 */
/* Description   : Function for registering with Rtm.                        */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrV3RegisterWithRtmForAlt ()
{
    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d: Entry\n", __func__, __LINE__);
    tNetIpv6LfaRegTbl   LfaRouteRegInfo;
    MEMSET (&LfaRouteRegInfo, 0, sizeof (tLfaRouteRegInfo));

    /* Ask for Interface Status Change and Route Change Events */
    LfaRouteRegInfo.u1ProtoId = MPLS_IP_PROTOCOL_ID;
    LfaRouteRegInfo.pLfaRouteChange = LfaIpv6RtChangeEventHandler;

    if (Rtm6RegisterHigherLayerProtocolForLfa (&LfaRouteRegInfo) == RTM_FAILURE)
    {
        SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC, "%s:%d: Registration with RTM failed\n", __func__,
                 __LINE__);
        return;
    }
    SR_TRC2 (SR_MAIN_TRC, "%s:%d: Registration with RTM complete\n", __func__,
             __LINE__);
    return;
}

/*****************************************************************************/
/* Function Name : SrV3DeRegisterWithRtmForAlt                               */
/* Description   : Function for Deregistering with Rtm.                      */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrV3DeRegisterWithRtmForAlt ()
{
    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d: Entry\n", __func__, __LINE__);
    UINT4               u4ContextId = SR_ZERO;
    UINT1               u1ProtoId = MPLS_IP_PROTOCOL_ID;

    if (Rtm6DeRegisterHigherLayerProtocolForLfa (u4ContextId, u1ProtoId) ==
        RTM_FAILURE)
    {
        SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC, "%s:%d: Deregistration with RTM failed\n",
                 __func__, __LINE__);
        return;
    }
    SR_TRC2 (SR_MAIN_TRC, "%s:%d:Deregistration done with RTM \n", __func__,
             __LINE__);
    return;
}

/******************************************************************************
 * Function Name      : SrOspfMsgHandler
 *
 * Description        : This routine process the OSPF Msg events on the SR Queue 
 *
 * Input(s)           : pOsToSrMsg - pointer to the message received
 * Output(s)          : None
 * Return Value(s)    : None
 *****************************************************************************/
PUBLIC INT4
SrOspfMsgHandler (tOsToOpqApp * pOsToSrMsg)
{
    tSrV4OspfNbrInfo   *pSrOspfNbrInfo = NULL;
    tTMO_SLL_NODE      *pOspfNbr = NULL;
    UINT1               isNbrExist = SR_FALSE;
    UINT1               u1LSAType = TYPE10_OPQ_LSA;

    if (pOsToSrMsg->u4MsgSubType == OSPF_TE_LSA_INFO)
    {
        if (pOsToSrMsg->lsaInfo.u2LsaLen > MAX_SR_LSAS)
        {
            SR_OSPF_LSA_MEM_FREE (pOsToSrMsg->lsaInfo.pu1Lsa);
            SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC, "%s:%d: pOsToSrMsg->lsaInfo.u2LsaLen is gretaer than MAX_SR_LSAS \n",
                    __func__, __LINE__);
            return SR_FAILURE;
        }

        if (SrProcessOspfMsg (pOsToSrMsg) != SR_SUCCESS)
        {
            SR_OSPF_LSA_MEM_FREE (pOsToSrMsg->lsaInfo.pu1Lsa);
            SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC, "%s:%d: SrProcessOspfMsg Failed \n",
                    __func__, __LINE__);
            return SR_FAILURE;
        }

        SR_OSPF_LSA_MEM_FREE (pOsToSrMsg->lsaInfo.pu1Lsa);
    }
    else if (pOsToSrMsg->u4MsgSubType == OSPF_REGISTER_WITH_OPQ_APP)
    {
        if (pOsToSrMsg->u4ContextId == OSPF_DEFAULT_CXT_ID)
        {
            gSrGlobalInfo.u1SrOspfDisable = SR_TRUE;
            if (SrSndMsgToOspf (SR_OSPF_DEREG,
                        pOsToSrMsg->u4ContextId, NULL) != SR_SUCCESS)
            {
                SR_TRC (SR_MAIN_TRC | SR_FAIL_TRC, "De-Registration failed\n");
                return CLI_FAILURE;
            }
            SR_TRC2 (SR_MAIN_TRC, "%s:%d De-reg successfully \n", __func__,
                    __LINE__);
        }
    }
    else if (pOsToSrMsg->u4MsgSubType == OSPF_NBR_DOWN_IND_TO_OPQ_APP)
    {
        TMO_SLL_Scan (&(gSrV4OspfNbrList), pOspfNbr, tTMO_SLL_NODE *)
        {
            pSrOspfNbrInfo = (tSrV4OspfNbrInfo *) pOspfNbr;
            if (pSrOspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr ==
                    pOsToSrMsg->nbrInfo.u4NbrIpAddr)
            {
                isNbrExist = SR_TRUE;
                break;
            }
        }

        if (isNbrExist == SR_TRUE)
        {
            SrProcessOspfNbrADjSidILMUpdate(pSrOspfNbrInfo, SR_TRUE);

            TMO_SLL_Delete (&(gSrV4OspfNbrList),
                    (tTMO_SLL_NODE *) pSrOspfNbrInfo);
            SR_TRC3 (SR_MAIN_TRC,
                    "%s:%d Deleted OSPF Nbr 0x%x from  gSrV4OspfNbrList \n",
                    __func__, __LINE__, pSrOspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr);
            SR_OSPFV4_NBR_INFO_MEM_FREE (pSrOspfNbrInfo);
        }
    }
    else if (pOsToSrMsg->u4MsgSubType == OSPF_NBR_UP_IND_TO_OPQ_APP)
    {
        TMO_SLL_Scan (&(gSrV4OspfNbrList), pOspfNbr, tTMO_SLL_NODE *)
        {
            pSrOspfNbrInfo = (tSrV4OspfNbrInfo *) pOspfNbr;
            if (pSrOspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr ==
                    pOsToSrMsg->nbrInfo.u4NbrIpAddr)
            {
                isNbrExist = SR_TRUE;
                break;
            }
        }
        if (isNbrExist == SR_FALSE)
        {
            if (SR_OSPFV4_NBR_INFO_MEM_ALLOC (pSrOspfNbrInfo) == NULL)
            {
                SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                        "%s:%d Memory Allocation Failure for OSPFV4 Neighbor Info \n",
                        __func__, __LINE__);
                return SR_FAILURE;
            }

            pSrOspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr =
                pOsToSrMsg->nbrInfo.u4NbrIpAddr;
            pSrOspfNbrInfo->ospfV4NbrInfo.u4NbrRtrId =
                pOsToSrMsg->nbrInfo.u4NbrRtrId;
            pSrOspfNbrInfo->u4IfIpAddr   = pOsToSrMsg->u4IfIpAddr;
            pSrOspfNbrInfo->u4AddrlessIf = pOsToSrMsg->u4AddrlessIf;
            pSrOspfNbrInfo->u4AreaId     = pOsToSrMsg->u4AreaId;

            TMO_SLL_Insert (&(gSrV4OspfNbrList), NULL,
                    &(pSrOspfNbrInfo->nextNbrNode));
            SR_TRC4 (SR_MAIN_TRC,
                    "%s:%d Added OSPF Nbr 0x%x on Intf 0x%x to gSrV4OspfNbrList \n",
                    __func__, __LINE__, pSrOspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr,
                    pSrOspfNbrInfo->u4IfIpAddr);
            SrProcessOspfNbrADjSidILMUpdate(pSrOspfNbrInfo, SR_FALSE);
        }

        if (SR_FAILURE == SrUtilSendLSA (NEW_LSA, SR_ZERO, u1LSAType))
        {
            SR_TRC2 (SR_MAIN_TRC | SR_FAIL_TRC, "%s:%d Send LSA Failed \n", __func__, __LINE__);
            return SR_FAILURE;
        }
    }
    else if (pOsToSrMsg->u4MsgSubType == OSPF_TE_NBR_IP_ADDR_INFO)
    {
        /* When SR is enabled later after OSPF settlement, then During Opqaue
           registration, NBR info will be sent with this message type */
        TMO_SLL_Scan (&(gSrV4OspfNbrList), pOspfNbr, tTMO_SLL_NODE *)
        {
            pSrOspfNbrInfo = (tSrV4OspfNbrInfo *) pOspfNbr;
            if (pSrOspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr ==
                    pOsToSrMsg->nbrInfo.u4NbrIpAddr)
            {
                isNbrExist = SR_TRUE;
                break;
            }
        }
        if (isNbrExist == SR_FALSE)
        {
            if (SR_OSPFV4_NBR_INFO_MEM_ALLOC (pSrOspfNbrInfo) == NULL)
            {
                SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                        "%s:%d Memory Allocation Failure for OSPFV4 Neighbor Info \n",
                        __func__, __LINE__);
                return SR_FAILURE;
            }

            pSrOspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr =
                pOsToSrMsg->nbrInfo.u4NbrIpAddr;
            pSrOspfNbrInfo->ospfV4NbrInfo.u4NbrRtrId =
                pOsToSrMsg->nbrInfo.u4NbrRtrId;
            pSrOspfNbrInfo->u4IfIpAddr   = pOsToSrMsg->u4IfIpAddr;
            pSrOspfNbrInfo->u4AddrlessIf = pOsToSrMsg->u4AddrlessIf;
            pSrOspfNbrInfo->u4AreaId     = pOsToSrMsg->u4AreaId;
            TMO_SLL_Insert (&(gSrV4OspfNbrList), NULL,
                    &(pSrOspfNbrInfo->nextNbrNode));
            SR_TRC4 (SR_MAIN_TRC,
                    "%s:%d Added OSPF Nbr 0x%x on Intf 0x%x to gSrV4OspfNbrList \n",
                    __func__, __LINE__, pSrOspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr,
                    pSrOspfNbrInfo->u4IfIpAddr);
            SrProcessOspfNbrADjSidILMUpdate(pSrOspfNbrInfo, SR_FALSE);
        }
    }

    return SR_SUCCESS;
}

/********************************************************************
 * Copyright (C) 2007 Aricent Inc . All Rights Reserved
 *
 * $Id$
 *
 * Description: This file contains utility routines for SR Traffic
 *                    Engineering and its FRR
 *********************************************************************/
#include "srincs.h"
#include "rtm6.h"
#include "../../netip/rtm/inc/rtmtdfs.h"
#include "../../mpls/mplsdb/mplslsr.h"

extern tSrAltModuleStatus gu4AltModuleStatus;

/*******************************************************************************
 * Function Name   : SrTeCreateFtnRtChangeEvt
 * Description     : Function to Create FTN for Te route Entry in route event flow.
 * Input           : destAddr
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesfully creation)/SR_FAILURE
 *******************************************************************************/
INT4
SrTeCreateFtnRtChangeEvt (tGenU4Addr * destAddr, UINT4 *pu4ErrCode)
{
    tSrTeRtEntryInfo   *pSrTeDestRtInfo = NULL;
    tSrTeLblStack       SrTeLblStack;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tGenU4Addr          nextHopAddr;
    tGenU4Addr          dummyAddr;
    UINT1               u1RouteSetFlag = SR_ZERO;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));

    SR_TRC3 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d Entry with prefix %x \n", __func__, __LINE__,
             destAddr->Addr.u4Addr);
    /* Case 1. If any SR-TE configured with DestAddr (ACTIVE) for which RT_NEW is recieved */
    pSrTeDestRtInfo = SrGetTeRouteEntryFromDestAddr (destAddr);
    if (pSrTeDestRtInfo == NULL)
    {
        SR_TRC3 (SR_CTRL_TRC,
                 "%s:%d No TePathEntry Found with destination prefix : %x \n",
                 __func__, __LINE__, destAddr->Addr.u4Addr);
    }
    else
    {

        /* TE Route entry exists with this destination prefix */
        *pu4ErrCode = SR_TE_VIA_DEST;
        /* SR TE Processing via destination address flow */
        if (SrTeFtnIlmCreateDelViaDestRt (pSrTeDestRtInfo, SR_TRUE) ==
            SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d SR TE LSP creation failed via destination prefix flow for %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
        }
    }

    /* SR TE Processing via Mandatory node */
    /* SR TE Lsps will be created for this prefixes whichever */
    /* is having this destAddr as its mandatory node */
    if (SrTeFtnIlmCreateDelViaMandRt (destAddr, SR_TRUE, u1RouteSetFlag) ==
        SR_FAILURE)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d SR TE LSP creation failed via Mandatory node flow for prefix : %x \n",
                 __func__, __LINE__, destAddr->Addr.u4Addr);
        return SR_FAILURE;
    }

    SR_TRC3 (SR_CTRL_TRC,
             "%s:%d SR TE LSP creation is successfull for prefix %x in RTEVENT flow \n",
             __func__, __LINE__, destAddr->Addr.u4Addr);
    return SR_SUCCESS;
}

/*************************************************************************
 * Function Name   : SrTeCreateFtnType10
 * Description     : Function to Create FTN and ILMfor TePath Entry via Type10 flow
 * Input           : destAddr
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesfully creation)/SR_FAILURE
 *************************************************************************/
INT4
SrTeCreateFtnType10 (tGenU4Addr * destAddr, UINT4 *pu4ErrCode)
{
    tSrTeRtEntryInfo   *pSrTeDestRtInfo = NULL;
    tSrTeLblStack       SrTeLblStack;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tGenU4Addr          nextHopAddr;
    tGenU4Addr          dummyAddr;
    INT4                i4LfaStatus = SR_ZERO;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));

    SR_TRC3 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d Entry with prefix %x \n", __func__, __LINE__,
             destAddr->Addr.u4Addr);
    /* To get SR_LFA status to process SR TE LFA LSP */
    nmhGetFsSrV4AlternateStatus (&i4LfaStatus);

    /* Case 1. If any SR-TE configured with DestAddr (ACTIVE) for which RT_NEW is recieved */
    pSrTeDestRtInfo = SrGetTeRouteEntryFromDestAddr (destAddr);
    if (pSrTeDestRtInfo == NULL)
    {
        SR_TRC3 (SR_CTRL_TRC,
                 "%s:%d No TePathEntry Found with destination prefix : %x \n",
                 __func__, __LINE__, destAddr->Addr.u4Addr);
    }
    else
    {
        /* TE Route entry exists with this destination prefix */
        *pu4ErrCode = SR_TE_VIA_DEST;

        /* SR TE Processing via destination address flow */
        if (SrTeFtnIlmCreateDelViaDestRt (pSrTeDestRtInfo, SR_TRUE) ==
            SR_FAILURE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d SR TE LSP creation failed via Dest prefix flow for %x \n",
                     __func__, __LINE__, destAddr->Addr.u4Addr);
        }
        if (i4LfaStatus == ALTERNATE_ENABLED)
        {
            /* SR TE LFA LSP processing */
            if (SrTeLfaRouteUpdate
                (&pSrTeDestRtInfo->destAddr,
                 &pSrTeDestRtInfo->mandRtrId) == SR_SUCCESS)
            {
                /* Creation of SR-TE LFA FTN and SR-FRR ILM */
                if (SrTeLfaCreateFtnIlm (pSrTeDestRtInfo) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d SR TE LFA FTN and SR FRR ILM creation failed for prefix %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                }
            }
            else
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         " %s : %d  SR TE LFA route updated failed for prefix %x \n",
                         __func__, __LINE__, destAddr->Addr.u4Addr);
            }
        }
    }

    /* SR TE Processing via Mandatory node */
    /* SR TE Lsps will be created for this prefixes whichever */
    /* is having this destAddr as its mandatory node */

    if (SrTeFtnIlmCreateDelWithLfaViaMandRt (destAddr, SR_TRUE) == SR_FAILURE)
    {
        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                 "%s:%d SR TE LSP creation failed via Mandatory node flow for prefix %x \n",
                 __func__, __LINE__, destAddr->Addr.u4Addr);
        return SR_FAILURE;
    }

    SR_TRC3 (SR_CTRL_TRC,
             "%s:%d SR TE LSP creation is successfull for prefix %x in Type10 flow \n",
             __func__, __LINE__, destAddr->Addr.u4Addr);
    return SR_SUCCESS;
}

/************************************************************************
 * Function Name   : SrTeDeleteFtnRtChangeEvt 
 * Description     : Function to Delete FTN  and ILM for TePath Entry in route event
 *                   flow
 * Input           : pDestAddr
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesfully deletion)
 *                   SR_FAILURE (No TEPATH Entry Found or any other Failure
 *                   scenario)
 *************************************************************************/

INT4
SrTeDeleteFtnRtChangeEvt (tGenU4Addr * pDestAddr, UINT4 *pu4ErrCode,
                          tGenU4Addr * pNextHop)
{
    tSrTeRtEntryInfo   *pSrTeDestRtInfo = NULL;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrTeLblStack       SrTeLblStack;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrInSegInfo        SrInSegInfo;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tGenU4Addr          tmpNextHop;
    tGenU4Addr          dummyAddr;
    UINT1               u1RouteSetFlag = SR_ZERO;

    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&tmpNextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    SR_TRC3 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s Entry with prefix %x NextHop %x \n", __func__,
             pDestAddr->Addr.u4Addr, pNextHop->Addr.u4Addr);

    /* Case 1. If any "SR-TE Path Entry" configured with DestAddr (ACTIVE) for which RT_DELETE is recieved */
    pSrTeDestRtInfo = SrGetTeRouteEntryFromDestAddr (pDestAddr);

    if (pSrTeDestRtInfo == NULL)
    {
        SR_TRC3 (SR_CTRL_TRC, "%s:%d No TePathEntry Found for prefix %x \n",
                 __func__, __LINE__, pDestAddr->Addr.u4Addr);
    }
    else
    {
        *pu4ErrCode = SR_TE_VIA_DEST;
        /* if SrTeDest is configured with DestAddr */

        /* Case 1.1:  SR TE LSP deletion via destination prefix */
        if ((pDestSrRtrInfo =
             SrGetSrRtrInfoFromRtrId (&pSrTeDestRtInfo->destAddr)) == NULL)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrRtInfo not exits for TE prefix %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);

            return SR_FAILURE;
        }
        /* Checking whether SR Primary Path has same Nexthop as that of Route Event Received */
        if ((pNextHop->u2AddrType == SR_IPV4_ADDR_TYPE)
            && (pDestSrRtrInfo->teSwapNextHop.Addr.u4Addr ==
                pNextHop->Addr.u4Addr))
        {
            if (SrTeFtnIlmCreateDelViaDestRt (pSrTeDestRtInfo, SR_FALSE) ==
                SR_FAILURE)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d SR TE LSP deletion failed via destination prefix flow for prefix %x \n",
                         __func__, __LINE__, pDestAddr->Addr.u4Addr);

            }
        }

        else if ((pNextHop->u2AddrType == SR_IPV6_ADDR_TYPE) &&
                 (MEMCMP
                  (pDestSrRtrInfo->teSwapNextHop.Addr.Ip6Addr.u1_addr,
                   pNextHop->Addr.Ip6Addr.u1_addr,
                   SR_IPV6_ADDR_LENGTH) == SR_ZERO))
        {
            if (SrTeFtnIlmCreateDelViaDestRt (pSrTeDestRtInfo, SR_FALSE) ==
                SR_FAILURE)
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d SR TE LSP deletion failed via destination prefix flow for prefix %x \n",
                         __func__, __LINE__, pDestAddr->Addr.u4Addr);

            }
        }

    }
    /* SR TE Processing via Mandatory node */
    /* SR TE Lsps will be deleted for this prefixes whichever */
    /* is having this destAddr as its mandatory node */

    if (SrTeFtnIlmCreateDelViaMandRt (pDestAddr, SR_FALSE, u1RouteSetFlag) ==
        SR_FAILURE)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d SR TE LSP deletion failed via Mandatory node flow for prefix %x \n",
                 __func__, __LINE__, pDestAddr->Addr.u4Addr);
        return SR_FAILURE;
    }

    SR_TRC3 (SR_CTRL_TRC,
             "%s:%d SR TE LSP deletion is successfull for prefix %x in RTEVENT flow \n",
             __func__, __LINE__, pDestAddr->Addr.u4Addr);
    return SR_SUCCESS;

}

/**************************************************************************
 * Function Name   : SrTeDeleteFtnType10 
 * Description     : Function to Delete FTN and ILMfor TePath Entry via Type10
 *                    flow
 * Input           : pDestAddr
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesfully deletion)
 *                   SR_FAILURE (No TEPATH Entry Found or any other Failure
 *                   scenario)
 *************************************************************************/

INT4
SrTeDeleteFtnType10 (tGenU4Addr * pDestAddr, UINT4 *pu4ErrCode)
{
    tSrTeRtEntryInfo   *pSrTeDestRtInfo = NULL;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrTeLblStack       SrTeLblStack;
    tGenU4Addr          tmpNextHop;
    tGenU4Addr          dummyAddr;
    tSrInSegInfo        SrInSegInfo;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;

    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&tmpNextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    SR_TRC3 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d Entry with prefix %x \n", __func__, __LINE__,
             pDestAddr->Addr.u4Addr);
    /* Case 1. If any "SR-TE Path Entry" configured with DestAddr (ACTIVE) for which RT_DELETE is recieved */
    pSrTeDestRtInfo = SrGetTeRouteEntryFromDestAddr (pDestAddr);

    if (pSrTeDestRtInfo == NULL)
    {
        SR_TRC3 (SR_CTRL_TRC, "%s:%d No TePathEntry Found for prefix %x \n",
                 __func__, __LINE__, pDestAddr->Addr.u4Addr);
    }
    else
    {
        *pu4ErrCode = SR_TE_VIA_DEST;
        /* if SrTeDest is configured with DestAddr */
        if (pSrTeDestRtInfo->LfaNexthop.Addr.u4Addr != SR_ZERO)
        {
            /* Deletion of SR-TE LFA FTN and SR-FRR ILM */
            if (SrTeLfaDeleteFtnIlm (pSrTeDestRtInfo) == SR_FAILURE)
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d SR TE LFA FTN and SR_FRR ILM deletion failed for prefix %x \n",
                         __func__, __LINE__, pDestAddr->Addr.u4Addr);
            }

        }
        /* Case 1.1:  SR TE LSP deletion via destination prefix */
        if (SrTeFtnIlmCreateDelViaDestRt (pSrTeDestRtInfo, SR_FALSE) ==
            SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d SR TE LSP deletion failed via destination prefix flow for prefix %x \n",
                     __func__, __LINE__, pDestAddr->Addr.u4Addr);
        }
    }
    /* Case 2. If Rt Delete Event came for "Mandatory RtrId" being used in any SrTeRtInfo,
     * then, Delete FTNs for all SrTeRtInfo that are using that Mandatory RtrId*/

    if (SrTeFtnIlmCreateDelWithLfaViaMandRt (pDestAddr, SR_FALSE) == SR_FAILURE)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d SR TE LSP deletion failed via Mandatory node flow for prefix %x \n",
                 __func__, __LINE__, pDestAddr->Addr.u4Addr);
        return SR_FAILURE;
    }
    SR_TRC3 (SR_CTRL_TRC,
             "%s:%d SrTeDeleteFtnType10 processing is successful for prefix %x\n",
             __func__, __LINE__, pDestAddr->Addr.u4Addr);

    return SR_SUCCESS;
}

/**************************************************************************
 * Function Name   : SrTeFtnIlmCreateDelViaDestRt
 * Description     : Function is to create or delete SR TE FTN and ILM
                     with destination prefix as reference.
 * Input           : pSrTeDestRtInfo, u1cmdType
                     if u1cmdType is SR_TRUE, then SR TE creation 
                     else, SR TE deletion flow
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesfully deletion)
 *                   SR_FAILURE (No TEPATH Entry Found or any other Failure
 *                   scenario)
 *************************************************************************/
INT4
SrTeFtnIlmCreateDelViaDestRt (tSrTeRtEntryInfo * pSrTeDestRtInfo,
                              UINT1 u1cmdType)
{
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrNextHopInfo   SrRtrNextHopInfo;
    tSrTeLblStack       SrTeLblStack;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;
    tSrInSegInfo        SrInSegInfo;
    tGenU4Addr          nextHopAddr;
    tGenU4Addr          dummyAddr;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4TopLabel = SR_ZERO;
    UINT4               u4DestLabel = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    UINT4               u4SrZero = SR_ZERO;
    UINT4               u4SrGbMinIndex = SR_ZERO;
    UINT4               u4Counter = SR_ZERO;
    UINT4               u4OptLabelCount = SR_ZERO;
    UINT4               u4TmpOptLabelCounter = SR_ZERO;
    UINT4               au4SrOptRtrLabel[SR_MAX_TE_OPTIONAL_RTR] = { SR_ZERO };

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrRtrNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    if (pSrTeDestRtInfo == NULL)
    {
        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                 "%s : %d Failure as TERtInfo Null with CmdType %d \n",
                 __func__, __LINE__, u1cmdType);
        return SR_FAILURE;
    }

    SR_TRC3 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d Entry with prefix %x \n", __func__, __LINE__,
             pSrTeDestRtInfo->destAddr.Addr.u4Addr);
    if (u1cmdType == SR_TRUE)    /* SR TE FTN and ILM creation */
    {
        /*Get SrRtrInfo for DestAddr for which TE path is to be created */
        if ((pDestSrRtrInfo =
             SrGetSrRtrInfoFromRtrId (&pSrTeDestRtInfo->destAddr)) == NULL)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrRtInfo not exits for TE prefix %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);

            return SR_FAILURE;
        }
        else
        {
            pDestSrRtrInfo->bIsTeconfigured = SR_TRUE;
        }

        if (pSrTeDestRtInfo->u1RowStatus != MPLS_STATUS_ACTIVE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d TePathEntry not ACTIVE for prefix %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }

        if ((pSrTeDestRtInfo->u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
        {
            SR_TRC3 (SR_CTRL_TRC,
                     "%s:%d FTN already created for TE-Path with Dest : %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_SUCCESS;
        }

        /* 2. If SR-TE Found, then check if Labels are available for reaching
         * [MandatoryRtrId (TopLabel)] & [DestAddr (NextLabel)] */

        if (SrTeGetLabelForRtrId (&pSrTeDestRtInfo->mandRtrId,
                                  &dummyAddr, &u4TopLabel) == SR_FAILURE)
        {

            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Unable to get TopLabel for prefix %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }

        if (SrTeGetLabelForRtrId (&pSrTeDestRtInfo->destAddr,
                                  &pSrTeDestRtInfo->mandRtrId,
                                  &u4DestLabel) == SR_FAILURE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d Unable to get DestLabel for prefix %x \n", __func__,
                     __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }

        pSrOptRtrInfo = NULL;
        pSrPeerRtrInfo = NULL;
        u4Counter = SR_ZERO;
        u4OptLabelCount = SR_ZERO;

        if (pSrTeDestRtInfo != NULL)
        {
            TMO_SLL_Scan (&(pSrTeDestRtInfo->srTeRtrListIndex.RtrList),
                          pSrOptRtrInfo, tTMO_SLL_NODE *)
            {
                pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                if (SrTeGetLabelForRtrId (&pSrPeerRtrInfo->routerId,
                                          &dummyAddr,
                                          &au4SrOptRtrLabel[u4Counter]) ==
                    SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Unable to get label for optional Router %x \n",
                             __func__, __LINE__,
                             pSrTeDestRtInfo->destAddr.Addr.u4Addr);
                    return SR_FAILURE;
                }
                u4Counter++;
                u4OptLabelCount++;
            }
        }

        /*Get Best Next-Hop & u4OutIfIndex for MandRtrId */
        if (SrGetNHForPrefix
            (&pSrTeDestRtInfo->mandRtrId, &nextHopAddr,
             &u4OutIfIndex) == SR_FAILURE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrGetNHForPrefix FAILURE for %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }
#ifdef CFA_WANTED
        /*u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
        if (CfaIfmCreateStackMplsTunnelInterface (u4OutIfIndex,
                                                  &u4MplsTnlIfIndex)
            == CFA_SUCCESS)
        {
            SR_TRC4 (SR_CTRL_TRC,
                     "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                     __func__, __LINE__, u4OutIfIndex, u4MplsTnlIfIndex);

            u4OutIfIndex = u4MplsTnlIfIndex;
        }
        else
        {
            SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                     __func__, __LINE__, u4OutIfIndex, u4MplsTnlIfIndex);
            return SR_FAILURE;
        }
#endif
        /* Set the TopLabel and the following Label Stack
         * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
        SrTeLblStack.u4TopLabel = u4TopLabel;
        SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;

        u4TmpOptLabelCounter = u4OptLabelCount - 1;
        for (u4Counter = SR_ONE; u4Counter <= u4OptLabelCount;
             u4Counter++, u4TmpOptLabelCounter--)
        {
            SrTeLblStack.u4LabelStack[u4Counter] =
                au4SrOptRtrLabel[u4TmpOptLabelCounter];
        }
        SrTeLblStack.u1StackSize = (UINT1) (u4OptLabelCount + SR_ONE);

        MEMCPY (&(pSrTeDestRtInfo->SrPriTeLblStack), &SrTeLblStack,
                sizeof (tSrTeLblStack));

        if (pSrTeDestRtInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            pDestSrRtrInfo->teSwapNextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        }
        else
        {
            pDestSrRtrInfo->teSwapNextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
        }

        /*Create new Next-Hop Node */
        if ((pSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrCreateNewNextHopNode returns NULL \n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
        /* Update OutIf & NextHop in SrRtrInfo for DestAddr */
        pSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
        pSrRtrNextHopInfo->nextHop.Addr.u4Addr = nextHopAddr.Addr.u4Addr;
        pSrTeDestRtInfo->PrimaryNexthop.Addr.u4Addr = nextHopAddr.Addr.u4Addr;
        pSrTeDestRtInfo->PrimaryNexthop.u2AddrType = SR_IPV4_ADDR_TYPE;
        pSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;

        /* Add NextHop Node in NextHopList in SrRtrNode */
        TMO_SLL_Insert (&(pDestSrRtrInfo->NextHopList), NULL,
                        &(pSrRtrNextHopInfo->nextNextHop));

        /* Create FTN */
        if (SrMplsCreateOrDeleteStackLsp
            (MPLS_MLIB_FTN_CREATE, pDestSrRtrInfo, NULL, &SrTeLblStack,
             pSrRtrNextHopInfo, SR_FALSE) == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d SrMplsCreateOrDeleteStackLsp  FAILURE for destination %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);

            /* Delete all the resources reserved on FAILURE & reset OutIfIndex */
#ifdef CFA_WANTED
            if (CfaUtilGetIfIndexFromMplsTnlIf
                (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
            {
                if (CfaIfmDeleteStackMplsTunnelInterface
                    (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                {
                    SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                             __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                    TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                                    (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                    SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                    return SR_FAILURE;
                }
            }
#endif
            TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                            (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
            SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
            return SR_FAILURE;
        }

        pSrTeDestRtInfo->u1MPLSStatus |= SR_FTN_CREATED;
        pSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;

        SR_TRC3 (SR_CTRL_TRC,
                 "%s:%d SR TE FTN creation is successful via Dest for prefix %x \n",
                 __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);

        if (pDestSrRtrInfo->u4TeSwapOutIfIndex != SR_FALSE)
        {
            SR_TRC3 (SR_CTRL_TRC,
                     "%s SR Primary ILM already created for Dest prefix %x  Primary ILM TunIndex %d \n",
                     __func__, pSrTeDestRtInfo->destAddr.Addr.u4Addr,
                     pDestSrRtrInfo->u4TeSwapOutIfIndex);

        }
        else
        {
/* ILM creation wrt primary nexthop */
            MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
            u4OutIfIndex = SR_ZERO;
            u4MplsTnlIfIndex = SR_ZERO;
            if (SrGetNHForPrefix
                (&pSrTeDestRtInfo->destAddr, &nextHopAddr,
                 &u4OutIfIndex) == SR_SUCCESS)
            {

#ifdef CFA_WANTED
                /*u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
                if (CfaIfmCreateStackMplsTunnelInterface (u4OutIfIndex,
                                                          &u4MplsTnlIfIndex)
                    == CFA_SUCCESS)
                {
                    SR_TRC4 (SR_CTRL_TRC,
                             "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                             __func__, __LINE__, u4OutIfIndex,
                             u4MplsTnlIfIndex);

                    u4OutIfIndex = u4MplsTnlIfIndex;
#endif

                    /* Create ILM Swap */
                    if (((pDestSrRtrInfo->teSwapNextHop.u2AddrType ==
                          SR_IPV4_ADDR_TYPE)
                         && (SrUtilCheckNodeIdConfigured () == SR_SUCCESS))
                        ||
                        ((pDestSrRtrInfo->teSwapNextHop.u2AddrType ==
                          SR_IPV6_ADDR_TYPE)
                         && (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)))
                    {

                        /*Creating Swap Entry with Best Path for TE LSP */
                        if (pDestSrRtrInfo->teSwapNextHop.u2AddrType ==
                            SR_IPV4_ADDR_TYPE)
                        {
                            pDestSrRtrInfo->teSwapNextHop.Addr.u4Addr =
                                u4SrZero;
                        }
                        else if (pDestSrRtrInfo->teSwapNextHop.u2AddrType ==
                                 SR_IPV6_ADDR_TYPE)
                        {
                            MEMCPY (&pDestSrRtrInfo->teSwapNextHop.Addr.Ip6Addr.
                                    u1_addr, &dummyAddr.Addr.Ip6Addr.u1_addr,
                                    MAX_IPV6_ADDR_LEN);
                        }
                        pDestSrRtrInfo->u4TeSwapOutIfIndex = u4OutIfIndex;
                        pDestSrRtrInfo->u1TeSwapStatus = TRUE;
                        SR_TRC2 (SR_CTRL_TRC, "%s:%d SET TeSwapStatus = TRUE \n",
                                 __func__, __LINE__);
                        if (nextHopAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
                        {
                            pDestSrRtrInfo->teSwapNextHop.Addr.u4Addr =
                                nextHopAddr.Addr.u4Addr;
                            pDestSrRtrInfo->teSwapNextHop.u2AddrType =
                                SR_IPV4_ADDR_TYPE;
                        }
                        else if (pDestSrRtrInfo->teSwapNextHop.u2AddrType ==
                                 SR_IPV6_ADDR_TYPE)
                        {
                            MEMCPY (pDestSrRtrInfo->teSwapNextHop.Addr.Ip6Addr.
                                    u1_addr, nextHopAddr.Addr.Ip6Addr.u1_addr,
                                    MAX_IPV6_ADDR_LEN);
                            pDestSrRtrInfo->teSwapNextHop.u2AddrType =
                                SR_IPV6_ADDR_TYPE;
                        }

                        if (SrMplsCreateILM (pDestSrRtrInfo, NULL) ==
                            SR_SUCCESS)
                        {

                            SR_TRC4 (SR_CTRL_TRC,
                                     "%s:%d SR ILM creation is successful via Dest for prefix %x with nexthop %x \n",
                                     __func__, __LINE__,
                                     pSrTeDestRtInfo->destAddr.Addr.u4Addr,
                                     nextHopAddr.Addr.u4Addr);
                            return SR_SUCCESS;
                        }
                    }
                }
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d SR-TE: SrMplsCreateILM  FAILURE for %x and Setting TeSwapStatus to FALSE \n",
                         __func__, __LINE__,
                         pSrTeDestRtInfo->destAddr.Addr.u4Addr);
                pDestSrRtrInfo->u1TeSwapStatus = FALSE;
                return SR_FAILURE;
            }
        }

    }
    else                        /* SR TE FTN and ILM deletion flow */
    {

        /* Case 1.1:  SR TE LSP deletion via destination prefix */
        if ((pSrTeDestRtInfo->u1MPLSStatus & SR_FTN_CREATED) ==
            SR_FTN_NOT_CREATED)
        {
            SR_TRC3 (SR_CTRL_TRC,
                     "%s:%d Sr TE LSPs were not created for %x \n", __func__,
                     __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_SUCCESS;
        }
        /* Get the SrRtrId */
        if ((pDestSrRtrInfo =
             SrGetSrRtrInfoFromRtrId (&pSrTeDestRtInfo->destAddr)) == NULL)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }

        /* There will be only one SrRtrNhNode for SR-TE */
        pSrRtrNextHopInfo =
            (tSrRtrNextHopInfo *)
            TMO_SLL_First (&(pDestSrRtrInfo->NextHopList));
        if (pSrRtrNextHopInfo == NULL)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s: %d Cannot find the next hop for %x \r\n", __func__,
                     __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }

        if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
        {
            u4SrGbMinIndex = gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
        }
        /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
        SrInSegInfo.u4InLabel = (u4SrGbMinIndex + pDestSrRtrInfo->u4SidValue);

        if (SrMplsDeleteILM (&SrInSegInfo, pDestSrRtrInfo, NULL) == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to delete ILM SWAP for %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
        }
        else
        {
#ifdef CFA_WANTED
            u4MplsTnlIfIndex = pDestSrRtrInfo->u4TeSwapOutIfIndex;

            if (CfaUtilGetIfIndexFromMplsTnlIf
                (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
            {
                if (CfaIfmDeleteStackMplsTunnelInterface
                    (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                {
                    SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                             __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                    return SR_FAILURE;
                }
            }

#endif
            SR_TRC4 (SR_CTRL_TRC,
                     "%s:%d SR ILM deletion is successful via dest for prefix %x with nexthop %x\n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr,
                     pDestSrRtrInfo->teSwapNextHop.Addr.u4Addr);
            pDestSrRtrInfo->u1TeSwapStatus = SR_FALSE;
            pDestSrRtrInfo->u4TeSwapOutIfIndex = SR_ZERO;
            MEMSET (&pDestSrRtrInfo->teSwapNextHop, SR_ZERO,
                    sizeof (tGenU4Addr));

        }

        /* Delete FTN */
        if ((pSrTeDestRtInfo->u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
        {
            if (SrMplsCreateOrDeleteStackLsp
                (MPLS_MLIB_FTN_DELETE, pDestSrRtrInfo, NULL, &SrTeLblStack,
                 pSrRtrNextHopInfo, SR_FALSE) == SR_FAILURE)
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d SrMplsCreateOrDeleteStackLsp  FAILURE for %x \n",
                         __func__, __LINE__,
                         pSrTeDestRtInfo->destAddr.Addr.u4Addr);
                return SR_FAILURE;
            }
        }

#ifdef CFA_WANTED
        u4MplsTnlIfIndex = pSrRtrNextHopInfo->u4OutIfIndex;

        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {
            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                         __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }
        }

        pSrRtrNextHopInfo->u4OutIfIndex = SR_ZERO;

#endif

        TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                        (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
        SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
        /* Setting FTN Status to SR_FTN_NOT_CREATED by
         *          * unsetting the FTN_CREATED Bit*/
        pSrTeDestRtInfo->u1MPLSStatus &= (UINT1) (~SR_FTN_CREATED);
        SR_TRC3 (SR_CTRL_TRC,
                 "%s:%d SR TE FTN deletion is successful via dest for prefix %x \n",
                 __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);

    }
    SR_TRC3 (SR_CTRL_TRC,
             "%s:%d SR TE LSP Process(creation/deletion) is successfull  via dest prefix %x \n",
             __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
    return SR_SUCCESS;
}

/**************************************************************************
 * Function Name   : SrTeFtnIlmCreateDelViaMandRt
 * Description     : Function is to create or delete SR TE FTN and ILM
                     with mandatory node  as reference.
 * Input           : destAddr, u1cmdType
                     if u1cmdType is SR_TRUE, then SR TE creation
                     else, SR TE deletion flow
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesfully deletion)
 *                   SR_FAILURE (No TEPATH Entry Found or any other Failure
 *                   scenario)
 *************************************************************************/

INT4
SrTeFtnIlmCreateDelViaMandRt (tGenU4Addr * destAddr, UINT1 u1cmdType,
                              UINT1 u1RouteSetFlag)
{
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrInSegInfo        SrInSegInfo;
    tSrTeLblStack       SrTeLblStack;
    tGenU4Addr          nextHopAddr;
    tGenU4Addr          dummyAddr;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4TopLabel = SR_ZERO;
    UINT4               u4DestLabel = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    UINT4               u4SrZero = SR_ZERO;
    UINT4               u4Counter = SR_ZERO;
    UINT4               u4OptLabelCount = SR_ZERO;
    UINT4               u4TmpOptLabelCounter = SR_ZERO;
    UINT1               u4isRtrEntryPresent = SR_FALSE;
    UINT4               au4SrOptRtrLabel[SR_MAX_TE_OPTIONAL_RTR] = { SR_ZERO };
    INT4                i4LfaStatus = SR_ZERO;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    SR_TRC3 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d Entry with prefix %x \n", __func__, __LINE__,
             destAddr->Addr.u4Addr);
    if (u1cmdType == SR_TRUE)    /* SR TE FTN and ILM Creation flow via Mandatory */
    {
        /* Case 2. If Rt_Add Event came for "Mandatory RtrId" being used in any SrTeRtInfo,
         * then, Add FTNs for all SrTeRtInfo that are using that Mandatory RtrId
         * Only if LSA Info for DestAddr is present */

        if (destAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            /* SR TE processing via Mandatory node */
            SrTeRtInfo.mandRtrId.Addr.u4Addr = destAddr->Addr.u4Addr;
            SrTeRtInfo.mandRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;

            while ((pSrTeRtInfo =
                    SrGetNextTeRouteTableEntry (&SrTeRtInfo)) != NULL)
            {
                u4isRtrEntryPresent = SR_FALSE;
                if (pSrTeRtInfo->mandRtrId.Addr.u4Addr != destAddr->Addr.u4Addr)
                {
                    TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                                  pSrOptRtrInfo, tTMO_SLL_NODE *)
                    {
                        pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                        if (pSrPeerRtrInfo->routerId.Addr.u4Addr ==
                            destAddr->Addr.u4Addr)
                        {
                            u4isRtrEntryPresent = SR_TRUE;
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    if (u4isRtrEntryPresent == SR_FALSE)
                    {
                        /*Variable updation for next iteration - Start */
                        MEMSET (&SrTeRtInfo, SR_ZERO,
                                sizeof (tSrTeRtEntryInfo));
                        SrTeRtInfo.destAddr.Addr.u4Addr =
                            pSrTeRtInfo->destAddr.Addr.u4Addr;
                        SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                        SrTeRtInfo.mandRtrId.Addr.u4Addr =
                            pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                        SrTeRtInfo.mandRtrId.u2AddrType =
                            pSrTeRtInfo->mandRtrId.u2AddrType;
                        SrTeRtInfo.destAddr.u2AddrType =
                            pSrTeRtInfo->destAddr.u2AddrType;
                        pSrTeRtInfo = NULL;
                        /*Variable updation for next iteration- End */
                        continue;
                    }
                }

                if ((pSrTeRtInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                    SR_FTN_CREATED)
                {
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d FTN already created for TE-Path with Dest : %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);

                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));

                    SrTeRtInfo.destAddr.Addr.u4Addr =
                        pSrTeRtInfo->destAddr.Addr.u4Addr;
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.mandRtrId.Addr.u4Addr =
                        pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                /* Check if Label is available for Sr-TE MandRtrId */
                if (SrTeGetLabelForRtrId
                    (&pSrTeRtInfo->mandRtrId, &dummyAddr,
                     &u4TopLabel) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Unable to get TopLabel for %x\n", __func__,
                             __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    SrTeRtInfo.destAddr.Addr.u4Addr =
                        pSrTeRtInfo->destAddr.Addr.u4Addr;
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.mandRtrId.Addr.u4Addr =
                        pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                /* Check if Label is available for Sr-TE DestAddr */
                if (SrTeGetLabelForRtrId (&pSrTeRtInfo->destAddr,
                                          &pSrTeRtInfo->mandRtrId,
                                          &u4DestLabel) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Unable to get Destination Label for %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    SrTeRtInfo.destAddr.Addr.u4Addr =
                        pSrTeRtInfo->destAddr.Addr.u4Addr;
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.mandRtrId.Addr.u4Addr =
                        pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                pSrOptRtrInfo = NULL;
                pSrPeerRtrInfo = NULL;
                u4Counter = SR_ZERO;
                u4OptLabelCount = SR_ZERO;
                TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                              pSrOptRtrInfo, tTMO_SLL_NODE *)
                {
                    pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                    if (SrTeGetLabelForRtrId (&pSrPeerRtrInfo->routerId,
                                              &dummyAddr,
                                              &au4SrOptRtrLabel[u4Counter]) ==
                        SR_FAILURE)
                    {
                        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 " %s:%d Unable to get label for optional Router %x \n",
                                 __func__, __LINE__, destAddr->Addr.u4Addr);
                        return SR_FAILURE;
                    }
                    u4Counter++;
                    u4OptLabelCount++;
                }
                /*Get Best Next-Hop & u4OutIfIndex for MandRtrId */
                if (SrGetNHForPrefix
                    (&pSrTeRtInfo->mandRtrId, &nextHopAddr,
                     &u4OutIfIndex) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetNHForPrefix FAILURE for %x \n", __func__,
                             __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    SrTeRtInfo.destAddr.Addr.u4Addr =
                        pSrTeRtInfo->destAddr.Addr.u4Addr;
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.mandRtrId.Addr.u4Addr =
                        pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variabu4SrOptRtrLabel[u4Counter]le updation for next iteration- End */
                    continue;
                }

#ifdef CFA_WANTED
                u4MplsTnlIfIndex = SR_ZERO;
                /*u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
                if (CfaIfmCreateStackMplsTunnelInterface (u4OutIfIndex,
                                                          &u4MplsTnlIfIndex)
                    == CFA_SUCCESS)
                {
                    SR_TRC4 (SR_CTRL_TRC,
                             "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                             __func__, __LINE__, u4OutIfIndex,
                             u4MplsTnlIfIndex);

                    u4OutIfIndex = u4MplsTnlIfIndex;
                }
                else
                {
                    SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                             __func__, __LINE__, u4OutIfIndex,
                             u4MplsTnlIfIndex);
                    return SR_FAILURE;
                }
#endif

                /* Set the TopLabel and the following Label Stack
                 * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
                SrTeLblStack.u4TopLabel = u4TopLabel;
                SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;

                u4TmpOptLabelCounter = u4OptLabelCount - 1;
                /*MEMCPY(&(SrTeLblStack.u4LabelStack[1]), au4SrOptRtrLabel, u4OptLabelCount * sizeof(UINT4)) */
                for (u4Counter = SR_ONE; u4Counter <= u4OptLabelCount;
                     u4Counter++, u4TmpOptLabelCounter--)
                {
                    SrTeLblStack.u4LabelStack[u4Counter] =
                        au4SrOptRtrLabel[u4TmpOptLabelCounter];
                }
                SrTeLblStack.u1StackSize = (UINT1) (u4OptLabelCount + SR_ONE);

                MEMCPY (&(pSrTeRtInfo->SrPriTeLblStack), &SrTeLblStack,
                        sizeof (tSrTeLblStack));

                /*Get SrRtrInfo for DestAddr for which TE path is to be created */
                if ((pDestSrRtrInfo =
                     SrGetSrRtrInfoFromRtrId (&pSrTeRtInfo->destAddr)) == NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

                if (destAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
                {
                    pDestSrRtrInfo->teSwapNextHop.u2AddrType =
                        SR_IPV4_ADDR_TYPE;
                }
                else
                {
                    pDestSrRtrInfo->teSwapNextHop.u2AddrType =
                        SR_IPV6_ADDR_TYPE;
                }

                /*Create new Next-Hop Node */
                if ((pSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrCreateNewNextHopNode returns NULL \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }

                /*Copy data to new next-hop Node & add to SrRtr's NextHop List and SR TE database */
                pSrRtrNextHopInfo->nextHop.Addr.u4Addr =
                    nextHopAddr.Addr.u4Addr;
                pSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;

                pSrTeRtInfo->PrimaryNexthop.Addr.u4Addr =
                    nextHopAddr.Addr.u4Addr;
                pSrTeRtInfo->PrimaryNexthop.u2AddrType = SR_IPV4_ADDR_TYPE;

                /* Update OutIf & NextHop in SrRtrInfo for DestAddr */
                pSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;

                /* Add NextHop Node in NextHopList in SrRtrNode */
                TMO_SLL_Insert (&(pDestSrRtrInfo->NextHopList), NULL,
                                &(pSrRtrNextHopInfo->nextNextHop));

                /* Create FTN */
                if (SrMplsCreateOrDeleteStackLsp
                    (MPLS_MLIB_FTN_CREATE, pDestSrRtrInfo, NULL, &SrTeLblStack,
                     pSrRtrNextHopInfo, SR_FALSE) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d SrMplsCreateOrDeleteStackLsp  FAILURE for destination %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);

                    /* Delete all the resources reserved on FAILURE & reset OutIfIndex */
#ifdef CFA_WANTED
                    if (CfaUtilGetIfIndexFromMplsTnlIf
                        (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
                    {
                        if (CfaIfmDeleteStackMplsTunnelInterface
                            (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                        {
                            SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                                     "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                                     __func__, __LINE__, u4MplsTnlIfIndex,
                                     u4L3VlanIf);
                            TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                                            (tTMO_SLL_NODE *)
                                            pSrRtrNextHopInfo);
                            SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                            return SR_FAILURE;
                        }
                    }

#endif
                    TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                                    (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                    SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                    return SR_FAILURE;
                }

                pSrTeRtInfo->u1MPLSStatus |= SR_FTN_CREATED;
                pSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;

                SR_TRC3 (SR_CTRL_TRC,
                         "%s:%d SR TE FTN creation is successful via mandatory with dest address %x \n",
                         __func__, __LINE__, destAddr->Addr.u4Addr);

                if (pDestSrRtrInfo->u4TeSwapOutIfIndex != SR_FALSE)
                {
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s SR Primary ILM already created for Dest prefix %x  Primary ILM TunIndex %d \n",
                             __func__, pSrTeRtInfo->destAddr.Addr.u4Addr,
                             pDestSrRtrInfo->u4TeSwapOutIfIndex);
                }
                else
                {
                    /* ILM creation wrt primary nexthop */
                    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
                    u4OutIfIndex = SR_ZERO;
                    u4MplsTnlIfIndex = SR_ZERO;

                    if (SrGetNHForPrefix
                        (&pSrTeRtInfo->destAddr, &nextHopAddr,
                         &u4OutIfIndex) == SR_SUCCESS)
                    {

#ifdef CFA_WANTED
                        /*u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
                        if (CfaIfmCreateStackMplsTunnelInterface (u4OutIfIndex,
                                                                  &u4MplsTnlIfIndex)
                            == CFA_SUCCESS)
                        {
                            SR_TRC4 (SR_CTRL_TRC,
                                     "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                                     __func__, __LINE__, u4OutIfIndex,
                                     u4MplsTnlIfIndex);

                            u4OutIfIndex = u4MplsTnlIfIndex;
#endif

                            /* Create ILM Swap */
                            if (((pDestSrRtrInfo->teSwapNextHop.u2AddrType ==
                                  SR_IPV4_ADDR_TYPE)
                                 && (SrUtilCheckNodeIdConfigured () == SR_SUCCESS))
                                ||
                                ((pDestSrRtrInfo->teSwapNextHop.u2AddrType ==
                                  SR_IPV6_ADDR_TYPE)
                                 && (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)))
                            {
                                /*Creating Swap Entry with Best Path for TE LSP */
                                if (pDestSrRtrInfo->teSwapNextHop.u2AddrType ==
                                    SR_IPV4_ADDR_TYPE)
                                {
                                    pDestSrRtrInfo->teSwapNextHop.Addr.u4Addr =
                                        u4SrZero;
                                }
                                else if (pDestSrRtrInfo->teSwapNextHop.
                                         u2AddrType == SR_IPV6_ADDR_TYPE)
                                {
                                    MEMCPY (&pDestSrRtrInfo->teSwapNextHop.Addr.
                                            Ip6Addr.u1_addr,
                                            &dummyAddr.Addr.Ip6Addr.u1_addr,
                                            MAX_IPV6_ADDR_LEN);
                                }
                                pDestSrRtrInfo->u4TeSwapOutIfIndex =
                                    u4OutIfIndex;
                                pDestSrRtrInfo->u1TeSwapStatus = TRUE;
                                SR_TRC2 (SR_CTRL_TRC,
                                         "%s:%d SET TeSwapStatus = TRUE \n",
                                         __func__, __LINE__);
                                if (nextHopAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
                                {
                                    pDestSrRtrInfo->teSwapNextHop.Addr.u4Addr =
                                        nextHopAddr.Addr.u4Addr;
                                    pDestSrRtrInfo->teSwapNextHop.u2AddrType =
                                        SR_IPV4_ADDR_TYPE;
                                }
                                else if (pDestSrRtrInfo->teSwapNextHop.
                                         u2AddrType == SR_IPV6_ADDR_TYPE)
                                {
                                    MEMCPY (pDestSrRtrInfo->teSwapNextHop.Addr.
                                            Ip6Addr.u1_addr,
                                            nextHopAddr.Addr.Ip6Addr.u1_addr,
                                            MAX_IPV6_ADDR_LEN);
                                    pDestSrRtrInfo->teSwapNextHop.u2AddrType =
                                        SR_IPV6_ADDR_TYPE;
                                }

                                if (SrMplsCreateILM (pDestSrRtrInfo, NULL) ==
                                    SR_SUCCESS)
                                {
                                    SR_TRC4 (SR_CTRL_TRC,
                                             "%s:%d SR-TE: ILM creation is successful via mand for prefix %x with nexthop %x \n",
                                             __func__, __LINE__,
                                             pDestSrRtrInfo->prefixId.Addr.u4Addr,
                                             nextHopAddr.Addr.u4Addr);
                                }
                                else
                                {
                                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                             "%s:%d SR-TE: SrMplsCreateILM  FAILURE for %x and Setting TeSwapStatus to FALSE \n",
                                             __func__, __LINE__,
                                             pSrTeRtInfo->destAddr.Addr.u4Addr);
                                    pDestSrRtrInfo->u1TeSwapStatus = FALSE;
                                    pDestSrRtrInfo->u4TeSwapOutIfIndex =
                                        SR_ZERO;
                                }
                            }
                        }
                    }
                }

                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                SrTeRtInfo.destAddr.Addr.u4Addr =
                    pSrTeRtInfo->destAddr.Addr.u4Addr;
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.mandRtrId.Addr.u4Addr =
                    pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */
            }
        }
        else if (destAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            while ((pSrTeRtInfo =
                    SrGetNextTeRouteTableEntry (&SrTeRtInfo)) != NULL)
            {
                u4isRtrEntryPresent = SR_FALSE;
                if (MEMCMP (pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            destAddr->Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH) != SR_ZERO)
                {
                    TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                                  pSrOptRtrInfo, tTMO_SLL_NODE *)
                    {
                        pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                        if (MEMCMP
                            (pSrPeerRtrInfo->routerId.Addr.Ip6Addr.u1_addr,
                             destAddr->Addr.Ip6Addr.u1_addr,
                             SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                        {
                            u4isRtrEntryPresent = SR_TRUE;
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    if (u4isRtrEntryPresent == SR_FALSE)
                    {
                        /*Variable updation for next iteration - Start */
                        MEMSET (&SrTeRtInfo, SR_ZERO,
                                sizeof (tSrTeRtEntryInfo));
                        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                                pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);
                        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                                pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);
                        SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                        SrTeRtInfo.destAddr.u2AddrType =
                            pSrTeRtInfo->destAddr.u2AddrType;
                        SrTeRtInfo.mandRtrId.u2AddrType =
                            pSrTeRtInfo->mandRtrId.u2AddrType;
                        pSrTeRtInfo = NULL;
                        /*Variable updation for next iteration- End */
                        continue;
                    }
                }

                if ((pSrTeRtInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                    SR_FTN_CREATED)
                {
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d FTN already created for TE-Path with Dest : %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);

                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));

                    MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                /* Check if Label is available for Sr-TE MandRtrId */
                if (SrTeGetLabelForRtrId (&pSrTeRtInfo->mandRtrId,
                                          &dummyAddr,
                                          &u4TopLabel) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Unable to get TopLabel for %x\n", __func__,
                             __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                /* Check if Label is available for Sr-TE DestAddr */
                if (SrTeGetLabelForRtrId (&pSrTeRtInfo->destAddr,
                                          &pSrTeRtInfo->mandRtrId,
                                          &u4DestLabel) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Unable to get u4DestLabel for prefix %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                pSrOptRtrInfo = NULL;
                pSrPeerRtrInfo = NULL;
                u4Counter = SR_ZERO;
                u4OptLabelCount = SR_ZERO;
                TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                              pSrOptRtrInfo, tTMO_SLL_NODE *)
                {
                    pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                    if (SrTeGetLabelForRtrId (&pSrPeerRtrInfo->routerId,
                                              &dummyAddr,
                                              &au4SrOptRtrLabel[u4Counter]) ==
                        SR_FAILURE)
                    {
                        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d Unable to get label for optional Router %x \n",
                                 __func__, __LINE__, destAddr->Addr.u4Addr);
                        return SR_FAILURE;
                    }
                    u4Counter++;
                    u4OptLabelCount++;
                }

                /*Get Best Next-Hop & u4OutIfIndex for MandRtrId */
                if (SrGetNHForPrefix
                    (&pSrTeRtInfo->mandRtrId, &nextHopAddr,
                     &u4OutIfIndex) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetNHForPrefix FAILURE for %x \n", __func__,
                             __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variabu4SrOptRtrLabel[u4Counter]le updation for next iteration- End */
                    continue;
                }

#ifdef CFA_WANTED
                u4MplsTnlIfIndex = SR_ZERO;
                /*u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
                if (CfaIfmCreateStackMplsTunnelInterface (u4OutIfIndex,
                                                          &u4MplsTnlIfIndex)
                    == CFA_SUCCESS)
                {
                    SR_TRC4 (SR_CTRL_TRC,
                             "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                             __func__, __LINE__, u4OutIfIndex,
                             u4MplsTnlIfIndex);

                    u4OutIfIndex = u4MplsTnlIfIndex;
                }
                else
                {
                    SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                             __func__, __LINE__, u4OutIfIndex,
                             u4MplsTnlIfIndex);
                    return SR_FAILURE;
                }
#endif

                /* Set the TopLabel and the following Label Stack
                 * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
                SrTeLblStack.u4TopLabel = u4TopLabel;
                SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;

                u4TmpOptLabelCounter = u4OptLabelCount - 1;
                /*MEMCPY(&(SrTeLblStack.u4LabelStack[1]), au4SrOptRtrLabel, u4OptLabelCount * sizeof(UINT4)) */
                for (u4Counter = SR_ONE; u4Counter <= u4OptLabelCount;
                     u4Counter++, u4TmpOptLabelCounter--)
                {
                    SrTeLblStack.u4LabelStack[u4Counter] =
                        au4SrOptRtrLabel[u4TmpOptLabelCounter];
                }
                SrTeLblStack.u1StackSize = (UINT1) (u4OptLabelCount + SR_ONE);

                MEMCPY (&(pSrTeRtInfo->SrPriTeLblStack), &SrTeLblStack,
                        sizeof (tSrTeLblStack));

                /*Get SrRtrInfo for DestAddr for which TE path is to be created */
                if ((pDestSrRtrInfo =
                     SrGetSrRtrInfoFromRtrId (&pSrTeRtInfo->destAddr)) == NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

                /*Create new Next-Hop Node */
                if ((pSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrCreateNewNextHopNode returns NULL \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }

                /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
                MEMCPY (pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                        nextHopAddr.Addr.Ip6Addr.u1_addr, MAX_IPV6_ADDR_LEN);
                /* Update OutIf & NextHop in SrRtrInfo for DestAddr */
                pSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;

                /* Add NextHop Node in NextHopList in SrRtrNode */
                TMO_SLL_Insert (&(pDestSrRtrInfo->NextHopList), NULL,
                                &(pSrRtrNextHopInfo->nextNextHop));

                /* Create FTN */
                if (SrMplsCreateOrDeleteStackLsp
                    (MPLS_MLIB_FTN_CREATE, pDestSrRtrInfo, NULL, &SrTeLblStack,
                     pSrRtrNextHopInfo, SR_FALSE) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d SrMplsCreateOrDeleteStackLsp  FAILURE for destination %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    /* Delete all the resources reserved on FAILURE & reset OutIfIndex */
#ifdef CFA_WANTED
                    if (CfaUtilGetIfIndexFromMplsTnlIf
                        (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
                    {
                        if (CfaIfmDeleteStackMplsTunnelInterface
                            (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                        {
                            SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                                     "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                                     __func__, __LINE__, u4MplsTnlIfIndex,
                                     u4L3VlanIf);
                            return SR_FAILURE;
                        }
                    }

#endif
                    return SR_FAILURE;
                }

                /* Create ILM Swap */
                if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
                {
                    /*Creating Swap Entry with Best Path for TE LSP */
                    MEMCPY (&pDestSrRtrInfo->teSwapNextHop.Addr.Ip6Addr.u1_addr,
                            &dummyAddr.Addr.Ip6Addr.u1_addr, MAX_IPV6_ADDR_LEN);
                    pDestSrRtrInfo->u4TeSwapOutIfIndex = SR_ZERO;
                    /* Marked status true before creating ILM as SrGetNHForPrefix
                     * will fill ILM Nexthop accordingly*/
                    pDestSrRtrInfo->u1TeSwapStatus = TRUE;
                    SR_TRC2 (SR_CTRL_TRC, "%s:%d SET TeSwapStatus = TRUE \n",
                             __func__, __LINE__);
                    u4MplsTnlIfIndex = 0;

                    /* Find and Populate Next-Hop Address and OutInterface for Prefix IP */
                    if (SrGetNHForPrefix (&pSrTeRtInfo->destAddr,
                                          &nextHopAddr,
                                          &(pDestSrRtrInfo->
                                            u4TeSwapOutIfIndex)) == SR_FAILURE)
                    {
                        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d SrGetNHForPrefix FAILURE for %x so setting TeSwapStatus to FALSE \n",
                                 __func__, __LINE__, destAddr->Addr.u4Addr);
                        pDestSrRtrInfo->u1TeSwapStatus = FALSE;
                        /* Donot return from here as FTN for MandRtrId is yet to be created
                         * Intentional Fall*/
                    }
                    else
                    {
                        MEMCPY (pDestSrRtrInfo->teSwapNextHop.Addr.Ip6Addr.
                                u1_addr, nextHopAddr.Addr.Ip6Addr.u1_addr,
                                MAX_IPV6_ADDR_LEN);
                        pDestSrRtrInfo->teSwapNextHop.u2AddrType =
                            SR_IPV6_ADDR_TYPE;
#ifdef CFA_WANTED
                        /*pSrRtrInfo->u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
                        if (CfaIfmCreateStackMplsTunnelInterface
                            (pDestSrRtrInfo->u4TeSwapOutIfIndex,
                             &u4MplsTnlIfIndex) == CFA_SUCCESS)
                        {
                            SR_TRC4 (SR_CTRL_TRC,
                                     "%s:%d pDestSrRtrInfo->u4TeSwapOutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                                     __func__, __LINE__,
                                     pDestSrRtrInfo->u4TeSwapOutIfIndex,
                                     u4MplsTnlIfIndex);

                            pDestSrRtrInfo->u4TeSwapOutIfIndex =
                                u4MplsTnlIfIndex;
                        }
                        else
                        {
                            SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                                     "%s:%d pDestSrRtrInfo->u4TeSwapOutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                                     __func__, __LINE__,
                                     pDestSrRtrInfo->u4TeSwapOutIfIndex,
                                     u4MplsTnlIfIndex);
                            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                                     "%s:%d SET TeSwapStatus = FALSE \n", __func__,
                                     __LINE__);
                            pDestSrRtrInfo->u1TeSwapStatus = FALSE;
                            return SR_FAILURE;
                        }
#endif

                        SR_TRC2 (SR_CTRL_TRC,
                                 "%s:%d SR-TE: Creating ILM SWAP for TE-IP \n",
                                 __func__, __LINE__);

                        /* 2nd Param is NULL as TE ILM next-hop is stored in RtrInfo only */
                        if (SrMplsCreateILM (pDestSrRtrInfo, NULL) ==
                            SR_FAILURE)
                        {
                            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SR-TE: SrMplsCreateILM  FAILURE for %x and Setting TeSwapStatus to FALSE \n",
                                     __func__, __LINE__, destAddr->Addr.u4Addr);
                            pDestSrRtrInfo->u1TeSwapStatus = FALSE;
                        }
                    }
                }
                pSrTeRtInfo->u1MPLSStatus |= SR_FTN_CREATED;
                pSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;

                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                        pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                        pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */
            }
        }

    }
    else                        /* SR TE FTN and ILM Deletion flow via Mandatory */
    {
        /* Case 2. If Rt Delete Event came for "Mandatory RtrId" being used in any SrTeRtInfo,
         * then, Delete FTNs for all SrTeRtInfo that are using that Mandatory RtrId*/

        nmhGetFsSrV4AlternateStatus (&i4LfaStatus);
        if (destAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            SrTeRtInfo.mandRtrId.Addr.u4Addr = destAddr->Addr.u4Addr;
            SrTeRtInfo.mandRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;

            while ((pSrTeRtInfo =
                    SrGetNextTeRouteTableEntry (&SrTeRtInfo)) != NULL)
            {
                u4isRtrEntryPresent = SR_FALSE;
                if (pSrTeRtInfo->mandRtrId.Addr.u4Addr != destAddr->Addr.u4Addr)
                {
                    TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                                  pSrOptRtrInfo, tTMO_SLL_NODE *)
                    {
                        pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                        if (pSrPeerRtrInfo->routerId.Addr.u4Addr ==
                            destAddr->Addr.u4Addr)
                        {
                            u4isRtrEntryPresent = SR_TRUE;
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    if (u4isRtrEntryPresent == SR_FALSE)
                    {
                        /*Variable updation for next iteration- Start */
                        MEMSET (&SrTeRtInfo, SR_ZERO,
                                sizeof (tSrTeRtEntryInfo));
                        SrTeRtInfo.destAddr.Addr.u4Addr =
                            pSrTeRtInfo->destAddr.Addr.u4Addr;
                        SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                        SrTeRtInfo.mandRtrId.Addr.u4Addr =
                            pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                        SrTeRtInfo.mandRtrId.u2AddrType =
                            pSrTeRtInfo->mandRtrId.u2AddrType;
                        SrTeRtInfo.destAddr.u2AddrType =
                            pSrTeRtInfo->destAddr.u2AddrType;
                        pSrTeRtInfo = NULL;
                        /*Variable updation for next iteration- End */
                        continue;
                    }
                }

                if ((pSrTeRtInfo->u1MPLSStatus & SR_FTN_CREATED) !=
                    SR_FTN_CREATED)
                {
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d FTN not created for TE-Path with Dest %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);

                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    SrTeRtInfo.destAddr.Addr.u4Addr =
                        pSrTeRtInfo->destAddr.Addr.u4Addr;
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.mandRtrId.Addr.u4Addr =
                        pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                /* Get the SrRtrInfo belongs to this SrTeRt */
                if ((pDestSrRtrInfo =
                     SrGetSrRtrInfoFromRtrId (&pSrTeRtInfo->destAddr)) == NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

                /* There will be only one SrRtrNhNode for SR-TE */
                pSrRtrNextHopInfo =
                    (tSrRtrNextHopInfo *)
                    TMO_SLL_First (&(pDestSrRtrInfo->NextHopList));
                if (pSrRtrNextHopInfo == NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             " %s: %d Cannot find the next hop for %x \r\n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }
                /* Delete FTN For TE-Path DestAddr */
                if (SrMplsCreateOrDeleteStackLsp
                    (MPLS_MLIB_FTN_DELETE, pDestSrRtrInfo, NULL, &SrTeLblStack,
                     pSrRtrNextHopInfo, SR_FALSE) != SR_SUCCESS)
                {
                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d SrMplsCreateOrDeleteStackLsp  FAILURE for destination %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

                SR_TRC3 (SR_CTRL_TRC,
                         "%s:%d SR TE FTN deletion via Mand is successful for prefix %x \n",
                         __func__, __LINE__, destAddr->Addr.u4Addr);

#ifdef CFA_WANTED
                u4MplsTnlIfIndex = pSrRtrNextHopInfo->u4OutIfIndex;

                if (CfaUtilGetIfIndexFromMplsTnlIf
                    (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
                {

                    if (CfaIfmDeleteStackMplsTunnelInterface
                        (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                    {
                        SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                                 __func__, __LINE__, u4MplsTnlIfIndex,
                                 u4L3VlanIf);
                        return SR_FAILURE;
                    }
                }

                pSrRtrNextHopInfo->u4OutIfIndex = SR_ZERO;
#endif
                TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                                (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);

                /* Setting MPLS Status to SR_FTN_NOT_CREATED by
                 * unsetting the FTN_CREATED Bit*/
                pSrTeRtInfo->u1MPLSStatus &= (UINT1) (~SR_FTN_CREATED);
                SR_TRC4 (SR_CTRL_TRC,
                         "%s:%d SR-TE FTN deletion is successful via mand for prefix %x with nexthop %x \n",
                         __func__, __LINE__, pDestSrRtrInfo->prefixId.Addr.u4Addr,
                         nextHopAddr.Addr.u4Addr);

                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                SrTeRtInfo.destAddr.Addr.u4Addr =
                    pSrTeRtInfo->destAddr.Addr.u4Addr;
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.mandRtrId.Addr.u4Addr =
                    pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */
            }
            return SR_SUCCESS;
        }
        else if (destAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            while ((pSrTeRtInfo =
                    SrGetNextTeRouteTableEntry (&SrTeRtInfo)) != NULL)
            {
                u4isRtrEntryPresent = SR_FALSE;
                if (MEMCMP (pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            destAddr->Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH) != SR_ZERO)
                {
                    TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                                  pSrOptRtrInfo, tTMO_SLL_NODE *)
                    {
                        pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                        if (MEMCMP
                            (pSrPeerRtrInfo->routerId.Addr.Ip6Addr.u1_addr,
                             destAddr->Addr.Ip6Addr.u1_addr,
                             SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                        {
                            u4isRtrEntryPresent = SR_TRUE;
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    if (u4isRtrEntryPresent == SR_FALSE)
                    {
                        /*Variable updation for next iteration- Start */
                        MEMSET (&SrTeRtInfo, SR_ZERO,
                                sizeof (tSrTeRtEntryInfo));
                        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                                pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);
                        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                                pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);
                        SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                        SrTeRtInfo.destAddr.u2AddrType =
                            pSrTeRtInfo->destAddr.u2AddrType;
                        SrTeRtInfo.mandRtrId.u2AddrType =
                            pSrTeRtInfo->mandRtrId.u2AddrType;
                        pSrTeRtInfo = NULL;
                        /*Variable updation for next iteration- End */
                        continue;
                    }
                }

                if ((pSrTeRtInfo->u1MPLSStatus & SR_FTN_CREATED) !=
                    SR_FTN_CREATED)
                {
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d FTN not created for TE-Path with Dest %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                /* Get the SrRtrInfo belongs to this SrTeRt */
                if ((pDestSrRtrInfo =
                     SrGetSrRtrInfoFromRtrId (&pSrTeRtInfo->destAddr)) == NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

                /* There will be only one SrRtrNhNode for SR-TE */
                pSrRtrNextHopInfo =
                    (tSrRtrNextHopInfo *)
                    TMO_SLL_First (&(pDestSrRtrInfo->NextHopList));
                if (pSrRtrNextHopInfo == NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             " %s:%d Cannot find the next hop for %x\r\n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

                /* Delete FTN For TE-Path DestAddr */
                if (SrMplsCreateOrDeleteStackLsp
                    (MPLS_MLIB_FTN_DELETE, pDestSrRtrInfo, NULL, &SrTeLblStack,
                     pSrRtrNextHopInfo, SR_FALSE) != SR_SUCCESS)
                {
                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d SrMplsCreateOrDeleteStackLsp FAILURE for destination %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

#ifdef CFA_WANTED
                u4MplsTnlIfIndex = pSrRtrNextHopInfo->u4OutIfIndex;

                if (CfaUtilGetIfIndexFromMplsTnlIf
                    (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
                {

                    if (CfaIfmDeleteStackMplsTunnelInterface
                        (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                    {
                        SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                                 __func__, __LINE__, u4MplsTnlIfIndex,
                                 u4L3VlanIf);
                        return SR_FAILURE;
                    }
                }

                pSrRtrNextHopInfo->u4OutIfIndex = SR_ZERO;
#endif
                TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                                (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);

                /* Setting MPLS Status to SR_FTN_NOT_CREATED by
                 * unsetting the FTN_CREATED Bit*/
                pSrTeRtInfo->u1MPLSStatus &= (UINT1) (~SR_FTN_CREATED);
                pDestSrRtrInfo->u1TeSwapStatus = SR_FALSE;
                pDestSrRtrInfo->u4TeSwapOutIfIndex = SR_ZERO;
                MEMSET (&pDestSrRtrInfo->teSwapNextHop, SR_ZERO,
                        sizeof (tGenU4Addr));

                /* Specific Case: If any Route for u4DestAddr is present
                 * and the next-hop is SR-Enabled 'then create SWAP'
                 * else if Route present and next-hop is not SR-Enabled
                 * 'then create POP & FWD'*/

                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                        pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                        pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */
            }
        }
    }
    SR_TRC3 (SR_CTRL_TRC,
             "%s:%d SR TE LSP Process(creation/deletion) is successfull  via mandatory %x \n",
             __func__, __LINE__, destAddr->Addr.u4Addr);
    UNUSED_PARAM (u1RouteSetFlag);
    return SR_SUCCESS;
}

/**************************************************************************
 * Function Name   : SrTeFtnIlmCreateDelViaMandRt
 * Description     : Function is to create or delete SR TE FTN and SR ILM
                     with mandatory node  as reference. It will also handle
                     respective FRR. This API will be used only in 
                     packet flow.
 * Input           : destAddr, u1cmdType

                     if u1cmdType is SR_TRUE, then SR TE creation
                     else, SR TE deletion flow
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesfully deletion)
 *                   SR_FAILURE (No TEPATH Entry Found or any other Failure
 *                   scenario)
 *************************************************************************/
INT4
SrTeFtnIlmCreateDelWithLfaViaMandRt (tGenU4Addr * destAddr, UINT1 u1cmdType)
{
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrInSegInfo        SrInSegInfo;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrTeLblStack       SrTeLblStack;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;
    tGenU4Addr          nextHopAddr;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4TopLabel = SR_ZERO;
    UINT4               u4DestLabel = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    UINT4               u4SrZero = SR_ZERO;
    UINT4               u4Counter = SR_ZERO;
    UINT4               u4OptLabelCount = SR_ZERO;
    UINT4               u4TmpOptLabelCounter = SR_ZERO;
    UINT1               u4isRtrEntryPresent = SR_FALSE;
    UINT4               au4SrOptRtrLabel[SR_MAX_TE_OPTIONAL_RTR] = { SR_ZERO };
    tGenU4Addr          dummyAddr;
    INT4                i4LfaStatus = ALTERNATE_DISABLED;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    nmhGetFsSrV4AlternateStatus (&i4LfaStatus);

    SR_TRC3 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d Entry with prefix %x \n", __func__, __LINE__,
             destAddr->Addr.u4Addr);
    if (u1cmdType == SR_TRUE)    /* SR TE and TE_LFA creation via mandatory node */
    {
        /* Case 2. If Rt_Add Event came for "Mandatory RtrId" being used in any SrTeRtInfo,
         * then, Add FTNs for all SrTeRtInfo that are using that Mandatory RtrId
         * Only if LSA Info for DestAddr is present */

        if (destAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            /* SR TE processing via Mandatory node */
            SrTeRtInfo.mandRtrId.Addr.u4Addr = destAddr->Addr.u4Addr;
            SrTeRtInfo.mandRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;

            while ((pSrTeRtInfo =
                    SrGetNextTeRouteTableEntry (&SrTeRtInfo)) != NULL)
            {
                u4isRtrEntryPresent = SR_FALSE;
                if (pSrTeRtInfo->mandRtrId.Addr.u4Addr != destAddr->Addr.u4Addr)
                {
                    TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                                  pSrOptRtrInfo, tTMO_SLL_NODE *)
                    {
                        pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                        if (pSrPeerRtrInfo->routerId.Addr.u4Addr ==
                            destAddr->Addr.u4Addr)
                        {
                            u4isRtrEntryPresent = SR_TRUE;
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    if (u4isRtrEntryPresent == SR_FALSE)
                    {
                        /*Variable updation for next iteration - Start */
                        MEMSET (&SrTeRtInfo, SR_ZERO,
                                sizeof (tSrTeRtEntryInfo));
                        SrTeRtInfo.destAddr.Addr.u4Addr =
                            pSrTeRtInfo->destAddr.Addr.u4Addr;
                        SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                        SrTeRtInfo.mandRtrId.Addr.u4Addr =
                            pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                        SrTeRtInfo.mandRtrId.u2AddrType =
                            pSrTeRtInfo->mandRtrId.u2AddrType;
                        SrTeRtInfo.destAddr.u2AddrType =
                            pSrTeRtInfo->destAddr.u2AddrType;
                        pSrTeRtInfo = NULL;
                        /*Variable updation for next iteration- End */
                        continue;
                    }
                }

                if ((pSrTeRtInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                    SR_FTN_CREATED)
                {
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d FTN already created for TE-Path with Dest : %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));

                    SrTeRtInfo.destAddr.Addr.u4Addr =
                        pSrTeRtInfo->destAddr.Addr.u4Addr;
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.mandRtrId.Addr.u4Addr =
                        pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                /* Check if Label is available for Sr-TE MandRtrId */
                if (SrTeGetLabelForRtrId
                    (&pSrTeRtInfo->mandRtrId, &dummyAddr,
                     &u4TopLabel) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Unable to get TopLabel for %x\n", __func__,
                             __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    SrTeRtInfo.destAddr.Addr.u4Addr =
                        pSrTeRtInfo->destAddr.Addr.u4Addr;
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.mandRtrId.Addr.u4Addr =
                        pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                /* Check if Label is available for Sr-TE DestAddr */
                if (SrTeGetLabelForRtrId (&pSrTeRtInfo->destAddr,
                                          &pSrTeRtInfo->mandRtrId,
                                          &u4DestLabel) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Unable to get u4DestLabel for prefix %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    SrTeRtInfo.destAddr.Addr.u4Addr =
                        pSrTeRtInfo->destAddr.Addr.u4Addr;
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.mandRtrId.Addr.u4Addr =
                        pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                pSrOptRtrInfo = NULL;
                pSrPeerRtrInfo = NULL;
                u4Counter = SR_ZERO;
                u4OptLabelCount = SR_ZERO;
                TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                              pSrOptRtrInfo, tTMO_SLL_NODE *)
                {
                    pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                    if (SrTeGetLabelForRtrId (&pSrPeerRtrInfo->routerId,
                                              &dummyAddr,
                                              &au4SrOptRtrLabel[u4Counter]) ==
                        SR_FAILURE)
                    {
                        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d Unable to get label for optional Router %x \n",
                                 __func__, __LINE__, destAddr->Addr.u4Addr);
                        return SR_FAILURE;
                    }
                    u4Counter++;
                    u4OptLabelCount++;
                }
                /*Get Best Next-Hop & u4OutIfIndex for MandRtrId */
                if (SrGetNHForPrefix
                    (&pSrTeRtInfo->mandRtrId, &nextHopAddr,
                     &u4OutIfIndex) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetNHForPrefix FAILURE for %x \n", __func__,
                             __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    SrTeRtInfo.destAddr.Addr.u4Addr =
                        pSrTeRtInfo->destAddr.Addr.u4Addr;
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.mandRtrId.Addr.u4Addr =
                        pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variabu4SrOptRtrLabel[u4Counter]le updation for next iteration- End */
                    continue;
                }

#ifdef CFA_WANTED
                u4MplsTnlIfIndex = SR_ZERO;
                /*u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
                if (CfaIfmCreateStackMplsTunnelInterface (u4OutIfIndex,
                                                          &u4MplsTnlIfIndex)
                    == CFA_SUCCESS)
                {
                    SR_TRC4 (SR_CTRL_TRC,
                             "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                             __func__, __LINE__, u4OutIfIndex,
                             u4MplsTnlIfIndex);

                    u4OutIfIndex = u4MplsTnlIfIndex;
                }
                else
                {
                    SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                             __func__, __LINE__, u4OutIfIndex,
                             u4MplsTnlIfIndex);
                    return SR_FAILURE;
                }
#endif

                /* Set the TopLabel and the following Label Stack
                 * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
                SrTeLblStack.u4TopLabel = u4TopLabel;
                SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;

                u4TmpOptLabelCounter = u4OptLabelCount - 1;
                /*MEMCPY(&(SrTeLblStack.u4LabelStack[1]), au4SrOptRtrLabel, u4OptLabelCount * sizeof(UINT4)) */
                for (u4Counter = SR_ONE; u4Counter <= u4OptLabelCount;
                     u4Counter++, u4TmpOptLabelCounter--)
                {
                    SrTeLblStack.u4LabelStack[u4Counter] =
                        au4SrOptRtrLabel[u4TmpOptLabelCounter];
                }
                SrTeLblStack.u1StackSize = (UINT1) (u4OptLabelCount + SR_ONE);
                MEMCPY (&(pSrTeRtInfo->SrPriTeLblStack), &SrTeLblStack,
                        sizeof (tSrTeLblStack));

                /*Get SrRtrInfo for DestAddr for which TE path is to be created */
                if ((pDestSrRtrInfo =
                     SrGetSrRtrInfoFromRtrId (&pSrTeRtInfo->destAddr)) == NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetSrRtrInfoFromRtrId FAILURE \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

                if (destAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
                {
                    pDestSrRtrInfo->teSwapNextHop.u2AddrType =
                        SR_IPV4_ADDR_TYPE;
                }
                else
                {
                    pDestSrRtrInfo->teSwapNextHop.u2AddrType =
                        SR_IPV6_ADDR_TYPE;
                }

                /*Create new Next-Hop Node */
                if ((pSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrCreateNewNextHopNode returns NULL \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }

                /*Copy data to new next-hop Node & add to SrRtr's NextHop List and SR TE database */
                pSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
                pSrRtrNextHopInfo->nextHop.Addr.u4Addr =
                    nextHopAddr.Addr.u4Addr;
                pSrTeRtInfo->PrimaryNexthop.u2AddrType = SR_IPV4_ADDR_TYPE;
                pSrTeRtInfo->PrimaryNexthop.Addr.u4Addr =
                    nextHopAddr.Addr.u4Addr;
                /* Update OutIf & NextHop in SrRtrInfo for DestAddr */
                pSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;

                /* Add NextHop Node in NextHopList in SrRtrNode */
                TMO_SLL_Insert (&(pDestSrRtrInfo->NextHopList), NULL,
                                &(pSrRtrNextHopInfo->nextNextHop));

                /* Create FTN */
                if (SrMplsCreateOrDeleteStackLsp
                    (MPLS_MLIB_FTN_CREATE, pDestSrRtrInfo, NULL, &SrTeLblStack,
                     pSrRtrNextHopInfo, SR_FALSE) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d SrMplsCreateOrDeleteStackLsp FAILURE for destination %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    /* Delete all the resources reserved on FAILURE & reset OutIfIndex */
#ifdef CFA_WANTED
                    if (CfaUtilGetIfIndexFromMplsTnlIf
                        (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
                    {
                        if (CfaIfmDeleteStackMplsTunnelInterface
                            (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                        {
                            SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                                     "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                                     __func__, __LINE__, u4MplsTnlIfIndex,
                                     u4L3VlanIf);
                            TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                                            (tTMO_SLL_NODE *)
                                            pSrRtrNextHopInfo);
                            SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                            return SR_FAILURE;
                        }
                    }

#endif
                    TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                                    (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                    SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                    return SR_FAILURE;
                }
                pSrTeRtInfo->u1MPLSStatus |= SR_FTN_CREATED;
                pSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;
                SR_TRC3 (SR_CTRL_TRC,
                         "%s:%d SR TE FTN creation is successful via mandatory with dest address %x \n",
                         __func__, __LINE__, destAddr->Addr.u4Addr);
                if (pDestSrRtrInfo->u4TeSwapOutIfIndex != SR_FALSE)
                {
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s SR Primary ILM already created for Dest prefix %x  Primary ILM TunIndex %d \n",
                             __func__, pSrTeRtInfo->destAddr.Addr.u4Addr,
                             pDestSrRtrInfo->u4TeSwapOutIfIndex);

                }
                else
                {
                    /* ILM creation wrt primary nexthop */
                    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
                    u4OutIfIndex = SR_ZERO;
                    u4MplsTnlIfIndex = SR_ZERO;

                    if (SrGetNHForPrefix
                        (&pSrTeRtInfo->destAddr, &nextHopAddr,
                         &u4OutIfIndex) == SR_SUCCESS)
                    {

#ifdef CFA_WANTED
                        /*u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
                        if (CfaIfmCreateStackMplsTunnelInterface (u4OutIfIndex,
                                                                  &u4MplsTnlIfIndex)
                            == CFA_SUCCESS)
                        {

                            u4OutIfIndex = u4MplsTnlIfIndex;
#endif

                            /* Create ILM Swap */
                            if (((pDestSrRtrInfo->teSwapNextHop.u2AddrType ==
                                  SR_IPV4_ADDR_TYPE)
                                 && (SrUtilCheckNodeIdConfigured () == SR_SUCCESS))
                                ||
                                ((pDestSrRtrInfo->teSwapNextHop.u2AddrType ==
                                  SR_IPV6_ADDR_TYPE)
                                 && (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)))
                            {
                                /*Creating Swap Entry with Best Path for TE LSP */
                                if (pDestSrRtrInfo->teSwapNextHop.u2AddrType ==
                                    SR_IPV4_ADDR_TYPE)
                                {
                                    pDestSrRtrInfo->teSwapNextHop.Addr.u4Addr =
                                        u4SrZero;
                                }
                                else if (pDestSrRtrInfo->teSwapNextHop.
                                         u2AddrType == SR_IPV6_ADDR_TYPE)
                                {
                                    MEMCPY (&pDestSrRtrInfo->teSwapNextHop.Addr.
                                            Ip6Addr.u1_addr,
                                            &dummyAddr.Addr.Ip6Addr.u1_addr,
                                            MAX_IPV6_ADDR_LEN);
                                }
                                pDestSrRtrInfo->u4TeSwapOutIfIndex =
                                    u4OutIfIndex;
                                pDestSrRtrInfo->u1TeSwapStatus = TRUE;
                                SR_TRC2 (SR_CTRL_TRC,
                                         "%s:%d SET TeSwapStatus = TRUE \n",
                                         __func__, __LINE__);
                                if (nextHopAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
                                {
                                    pDestSrRtrInfo->teSwapNextHop.Addr.u4Addr =
                                        nextHopAddr.Addr.u4Addr;
                                    pDestSrRtrInfo->teSwapNextHop.u2AddrType =
                                        SR_IPV4_ADDR_TYPE;
                                }
                                else if (pDestSrRtrInfo->teSwapNextHop.
                                         u2AddrType == SR_IPV6_ADDR_TYPE)
                                {
                                    MEMCPY (pDestSrRtrInfo->teSwapNextHop.Addr.
                                            Ip6Addr.u1_addr,
                                            nextHopAddr.Addr.Ip6Addr.u1_addr,
                                            MAX_IPV6_ADDR_LEN);
                                    pDestSrRtrInfo->teSwapNextHop.u2AddrType =
                                        SR_IPV6_ADDR_TYPE;
                                }

                                if (SrMplsCreateILM (pDestSrRtrInfo, NULL) ==
                                    SR_SUCCESS)
                                {
                                    SR_TRC4 (SR_CTRL_TRC,
                                             "%s:%d SR-TE: ILM creation is successful via mand for prefix %x with nexthop %x \n",
                                             __func__, __LINE__,
                                             pDestSrRtrInfo->prefixId.Addr.u4Addr,
                                             nextHopAddr.Addr.u4Addr);
                                }

                                else
                                {
                                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                             "%s:%d SR-TE: SrMplsCreateILM  FAILURE for %x and Setting TeSwapStatus to FALSE \n",
                                             __func__, __LINE__,
                                             pSrTeRtInfo->destAddr.Addr.u4Addr);
                                    pDestSrRtrInfo->u1TeSwapStatus = FALSE;
                                    return SR_FAILURE;
                                }
                            }
                        }
                    }
                }

                if (i4LfaStatus == ALTERNATE_ENABLED)
                {
                    /* SR TE LFA LSP processing */
                    if (SrTeLfaRouteUpdate
                        (&pSrTeRtInfo->destAddr,
                         &pSrTeRtInfo->mandRtrId) == SR_SUCCESS)
                    {
                        /* To Create SR TE LFA FTN and SR FRR ILM */
                        if (SrTeLfaCreateFtnIlm (pSrTeRtInfo) == SR_FAILURE)
                        {
                            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SR TE LFA lsp creation failed for prefix %x \n",
                                     __func__, __LINE__, destAddr->Addr.u4Addr);
                        }
                        SR_TRC3 (SR_CTRL_TRC,
                                 "%s:%d SR TE LFA FTN and FRR ILM creation is successful via mandatory with dest address %x \n",
                                 __func__, __LINE__, destAddr->Addr.u4Addr);
                    }
                    else
                    {
                        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 " Line %d func %s SR TE LFA route updated failed for %x \r\n",
                                 __LINE__, __func__, destAddr->Addr.u4Addr);
                    }
                }

                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                SrTeRtInfo.destAddr.Addr.u4Addr =
                    pSrTeRtInfo->destAddr.Addr.u4Addr;
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.mandRtrId.Addr.u4Addr =
                    pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */
            }
        }
        else if (destAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            while ((pSrTeRtInfo =
                    SrGetNextTeRouteTableEntry (&SrTeRtInfo)) != NULL)
            {
                u4isRtrEntryPresent = SR_FALSE;
                if (MEMCMP (pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            destAddr->Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH) != SR_ZERO)
                {
                    TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                                  pSrOptRtrInfo, tTMO_SLL_NODE *)
                    {
                        pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                        if (MEMCMP
                            (pSrPeerRtrInfo->routerId.Addr.Ip6Addr.u1_addr,
                             destAddr->Addr.Ip6Addr.u1_addr,
                             SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                        {
                            u4isRtrEntryPresent = SR_TRUE;
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    if (u4isRtrEntryPresent == SR_FALSE)
                    {
                        /*Variable updation for next iteration - Start */
                        MEMSET (&SrTeRtInfo, SR_ZERO,
                                sizeof (tSrTeRtEntryInfo));
                        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                                pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);
                        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                                pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);
                        SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                        SrTeRtInfo.destAddr.u2AddrType =
                            pSrTeRtInfo->destAddr.u2AddrType;
                        SrTeRtInfo.mandRtrId.u2AddrType =
                            pSrTeRtInfo->mandRtrId.u2AddrType;
                        pSrTeRtInfo = NULL;
                        /*Variable updation for next iteration- End */
                        continue;
                    }
                }

                if ((pSrTeRtInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                    SR_FTN_CREATED)
                {
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d FTN already created for TE-Path with Dest : %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);

                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));

                    MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                /* Check if Label is available for Sr-TE MandRtrId */
                if (SrTeGetLabelForRtrId (&pSrTeRtInfo->mandRtrId,
                                          &dummyAddr,
                                          &u4TopLabel) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Unable to get TopLabel for %x\n", __func__,
                             __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                /* Check if Label is available for Sr-TE DestAddr */
                if (SrTeGetLabelForRtrId (&pSrTeRtInfo->destAddr,
                                          &pSrTeRtInfo->mandRtrId,
                                          &u4DestLabel) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Unable to get u4DestLabel for prefix %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                pSrOptRtrInfo = NULL;
                pSrPeerRtrInfo = NULL;
                u4Counter = SR_ZERO;
                u4OptLabelCount = SR_ZERO;
                TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                              pSrOptRtrInfo, tTMO_SLL_NODE *)
                {
                    pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                    if (SrTeGetLabelForRtrId (&pSrPeerRtrInfo->routerId,
                                              &dummyAddr,
                                              &au4SrOptRtrLabel[u4Counter]) ==
                        SR_FAILURE)
                    {
                        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d Unable to get label for optional Router %x \n",
                                 __func__, __LINE__, destAddr->Addr.u4Addr);
                        return SR_FAILURE;
                    }
                    u4Counter++;
                    u4OptLabelCount++;
                }

                /*Get Best Next-Hop & u4OutIfIndex for MandRtrId */
                if (SrGetNHForPrefix
                    (&pSrTeRtInfo->mandRtrId, &nextHopAddr,
                     &u4OutIfIndex) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetNHForPrefix FAILURE for %x \n", __func__,
                             __LINE__, destAddr->Addr.u4Addr);
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variabu4SrOptRtrLabel[u4Counter]le updation for next iteration- End */
                    continue;
                }

#ifdef CFA_WANTED
                u4MplsTnlIfIndex = SR_ZERO;
                /*u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
                if (CfaIfmCreateStackMplsTunnelInterface (u4OutIfIndex,
                                                          &u4MplsTnlIfIndex)
                    == CFA_SUCCESS)
                {
                    SR_TRC4 (SR_CTRL_TRC,
                             "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                             __func__, __LINE__, u4OutIfIndex,
                             u4MplsTnlIfIndex);

                    u4OutIfIndex = u4MplsTnlIfIndex;
                }
                else
                {
                    SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                             __func__, __LINE__, u4OutIfIndex,
                             u4MplsTnlIfIndex);
                    return SR_FAILURE;
                }
#endif

                /* Set the TopLabel and the following Label Stack
                 * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
                SrTeLblStack.u4TopLabel = u4TopLabel;
                SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;

                u4TmpOptLabelCounter = u4OptLabelCount - 1;
                /*MEMCPY(&(SrTeLblStack.u4LabelStack[1]), au4SrOptRtrLabel, u4OptLabelCount * sizeof(UINT4)) */
                for (u4Counter = SR_ONE; u4Counter <= u4OptLabelCount;
                     u4Counter++, u4TmpOptLabelCounter--)
                {
                    SrTeLblStack.u4LabelStack[u4Counter] =
                        au4SrOptRtrLabel[u4TmpOptLabelCounter];
                }
                SrTeLblStack.u1StackSize = (UINT1) (u4OptLabelCount + SR_ONE);
                MEMCPY (&(pSrTeRtInfo->SrPriTeLblStack), &SrTeLblStack,
                        sizeof (tSrTeLblStack));

                /*Get SrRtrInfo for DestAddr for which TE path is to be created */
                if ((pDestSrRtrInfo =
                     SrGetSrRtrInfoFromRtrId (&pSrTeRtInfo->destAddr)) == NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

                /*Create new Next-Hop Node */
                if ((pSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrCreateNewNextHopNode returns NULL \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }

                /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
                MEMCPY (pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                        nextHopAddr.Addr.Ip6Addr.u1_addr, MAX_IPV6_ADDR_LEN);
                /* Update OutIf & NextHop in SrRtrInfo for DestAddr */
                pSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;

                /* Add NextHop Node in NextHopList in SrRtrNode */
                TMO_SLL_Insert (&(pDestSrRtrInfo->NextHopList), NULL,
                                &(pSrRtrNextHopInfo->nextNextHop));

                /* Create FTN */
                if (SrMplsCreateOrDeleteStackLsp
                    (MPLS_MLIB_FTN_CREATE, pDestSrRtrInfo, NULL, &SrTeLblStack,
                     pSrRtrNextHopInfo, SR_FALSE) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d SrMplsCreateOrDeleteStackLsp FAILURE for destination %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);

                    /* Delete all the resources reserved on FAILURE & reset OutIfIndex */
#ifdef CFA_WANTED
                    if (CfaUtilGetIfIndexFromMplsTnlIf
                        (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
                    {
                        if (CfaIfmDeleteStackMplsTunnelInterface
                            (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                        {
                            SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                                     "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                                     __func__, __LINE__, u4MplsTnlIfIndex,
                                     u4L3VlanIf);
                            return SR_FAILURE;
                        }
                    }

#endif
                    return SR_FAILURE;
                }

                /* Create ILM Swap */
                if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
                {
                    /*Creating Swap Entry with Best Path for TE LSP */
                    MEMCPY (&pDestSrRtrInfo->teSwapNextHop.Addr.Ip6Addr.u1_addr,
                            &dummyAddr.Addr.Ip6Addr.u1_addr, MAX_IPV6_ADDR_LEN);
                    pDestSrRtrInfo->u4TeSwapOutIfIndex = SR_ZERO;
                    /* Marked status true before creating ILM as SrGetNHForPrefix
                     * will fill ILM Nexthop accordingly*/
                    pDestSrRtrInfo->u1TeSwapStatus = TRUE;
                    SR_TRC2 (SR_CTRL_TRC, "%s:%d SET TeSwapStatus = TRUE \n",
                             __func__, __LINE__);
                    u4MplsTnlIfIndex = 0;

                    /* Find and Populate Next-Hop Address and OutInterface for Prefix IP */
                    if (SrGetNHForPrefix (&pSrTeRtInfo->destAddr,
                                          &nextHopAddr,
                                          &(pDestSrRtrInfo->
                                            u4TeSwapOutIfIndex)) == SR_FAILURE)
                    {
                        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d SrGetNHForPrefix FAILURE for %x so Setting TeSwapStatus to FALSE \n",
                                 __func__, __LINE__, destAddr->Addr.u4Addr);
                        pDestSrRtrInfo->u1TeSwapStatus = FALSE;
                        /* Donot return from here as FTN for MandRtrId is yet to be created
                         * Intentional Fall*/
                    }
                    else
                    {
                        MEMCPY (pDestSrRtrInfo->teSwapNextHop.Addr.Ip6Addr.
                                u1_addr, nextHopAddr.Addr.Ip6Addr.u1_addr,
                                MAX_IPV6_ADDR_LEN);
                        pDestSrRtrInfo->teSwapNextHop.u2AddrType =
                            SR_IPV6_ADDR_TYPE;
#ifdef CFA_WANTED
                        /*pSrRtrInfo->u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
                        if (CfaIfmCreateStackMplsTunnelInterface
                            (pDestSrRtrInfo->u4TeSwapOutIfIndex,
                             &u4MplsTnlIfIndex) == CFA_SUCCESS)
                        {
                            SR_TRC4 (SR_CTRL_TRC,
                                     "%s:%d pDestSrRtrInfo->u4TeSwapOutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                                     __func__, __LINE__,
                                     pDestSrRtrInfo->u4TeSwapOutIfIndex,
                                     u4MplsTnlIfIndex);

                            pDestSrRtrInfo->u4TeSwapOutIfIndex =
                                u4MplsTnlIfIndex;
                        }
                        else
                        {
                            SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                                     "%s:%d pDestSrRtrInfo->u4TeSwapOutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                                     __func__, __LINE__,
                                     pDestSrRtrInfo->u4TeSwapOutIfIndex,
                                     u4MplsTnlIfIndex);
                            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                                     "%s:%d SET TeSwapStatus = FALSE \n", __func__,
                                     __LINE__);
                            pDestSrRtrInfo->u1TeSwapStatus = FALSE;
                            return SR_FAILURE;
                        }
#endif

                        SR_TRC2 (SR_CTRL_TRC,
                                 "%s:%d SR-TE: Creating ILM SWAP for TE-IP \n",
                                 __func__, __LINE__);

                        /* 2nd Param is NULL as TE ILM next-hop is stored in RtrInfo only */
                        if (SrMplsCreateILM (pDestSrRtrInfo, NULL) ==
                            SR_FAILURE)
                        {
                            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SR-TE: SrMplsCreateILM  FAILURE for %x and Setting TeSwapStatus to FALSE \n",
                                     __func__, __LINE__, destAddr->Addr.u4Addr);
                            pDestSrRtrInfo->u1TeSwapStatus = FALSE;
                        }
                        pSrTeRtInfo->u1MPLSStatus |= SR_FTN_CREATED;
                        pSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;

                    }
                }

                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                        pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                        pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */
            }
        }

    }
    else                        /* SR TE and TE_LFA deletion via mandatory node */
    {
        /* Case 2. If Rt Delete Event came for "Mandatory RtrId" being used in any SrTeRtInfo,
         * then, Delete FTNs for all SrTeRtInfo that are using that Mandatory RtrId*/

        if (destAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            SrTeRtInfo.mandRtrId.Addr.u4Addr = destAddr->Addr.u4Addr;
            SrTeRtInfo.mandRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;

            while ((pSrTeRtInfo =
                    SrGetNextTeRouteTableEntry (&SrTeRtInfo)) != NULL)
            {
                u4isRtrEntryPresent = SR_FALSE;
                if (pSrTeRtInfo->mandRtrId.Addr.u4Addr != destAddr->Addr.u4Addr)
                {
                    TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                                  pSrOptRtrInfo, tTMO_SLL_NODE *)
                    {
                        pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                        if (pSrPeerRtrInfo->routerId.Addr.u4Addr ==
                            destAddr->Addr.u4Addr)
                        {
                            u4isRtrEntryPresent = SR_TRUE;
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    if (u4isRtrEntryPresent == SR_FALSE)
                    {
                        /*Variable updation for next iteration- Start */
                        MEMSET (&SrTeRtInfo, SR_ZERO,
                                sizeof (tSrTeRtEntryInfo));
                        SrTeRtInfo.destAddr.Addr.u4Addr =
                            pSrTeRtInfo->destAddr.Addr.u4Addr;
                        SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                        SrTeRtInfo.mandRtrId.Addr.u4Addr =
                            pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                        SrTeRtInfo.mandRtrId.u2AddrType =
                            pSrTeRtInfo->mandRtrId.u2AddrType;
                        SrTeRtInfo.destAddr.u2AddrType =
                            pSrTeRtInfo->destAddr.u2AddrType;
                        pSrTeRtInfo = NULL;
                        /*Variable updation for next iteration- End */
                        continue;
                    }
                }

                if (pSrTeRtInfo->LfaNexthop.Addr.u4Addr != SR_ZERO)
                {
                    /* To Delete SR TE LFA FTN */
                    if (SrTeFRRFtnCreateOrDel (pSrTeRtInfo, SR_FALSE) ==
                        SR_FAILURE)
                    {
                        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                 "%s:%d SR TE LFA lsp deletion failed for prefix %x \n",
                                 __func__, __LINE__,
                                 pSrTeRtInfo->destAddr.Addr.u4Addr);
                    }
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d SR TE LFA FTN deletion is successful for prefix %x \n",
                             __func__, __LINE__,
                             pSrTeRtInfo->destAddr.Addr.u4Addr);

                }

                if ((pSrTeRtInfo->u1MPLSStatus & SR_FTN_CREATED) !=
                    SR_FTN_CREATED)
                {
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d FTN not created for TE-Path with Dest : %x\n",
                             __func__, __LINE__,
                             pSrTeRtInfo->destAddr.Addr.u4Addr);

                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    SrTeRtInfo.destAddr.Addr.u4Addr =
                        pSrTeRtInfo->destAddr.Addr.u4Addr;
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.mandRtrId.Addr.u4Addr =
                        pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                /* Get the SrRtrInfo belongs to this SrTeRt */
                if ((pDestSrRtrInfo =
                     SrGetSrRtrInfoFromRtrId (&pSrTeRtInfo->destAddr)) == NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

                /* There will be only one SrRtrNhNode for SR-TE */
                pSrRtrNextHopInfo =
                    (tSrRtrNextHopInfo *)
                    TMO_SLL_First (&(pDestSrRtrInfo->NextHopList));
                if (pSrRtrNextHopInfo == NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s: %dCannot find the next hop for %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }
                if (SrGetNHForPrefix (destAddr, &nextHopAddr, &u4OutIfIndex) ==
                    SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetNHForPrefix FAILURE for %x \n", __func__,
                             __LINE__, destAddr->Addr.u4Addr);

                }
                if ((pSrRtrNextHopInfo->nextHop.Addr.u4Addr ==
                     pSrTeRtInfo->FrrIlmNexthop.Addr.u4Addr)
                    && (i4LfaStatus == ALTERNATE_ENABLED))
                {
                    if (SrTeFRRIlmAddorDelete (destAddr, pSrTeRtInfo, SR_FALSE)
                        == SR_FAILURE)
                    {

                        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                 "%s:%d SrTeFRRIlmAddorDelete FAILURE for destination %x with nh %x \n",
                                 __func__, __LINE__, destAddr->Addr.u4Addr,
                                 nextHopAddr.Addr.u4Addr);
                    }
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d FRR ILM deletion is successful via mandatory with dest address %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);

                }
                /* Delete FTN For TE-Path DestAddr */
                if (SrMplsCreateOrDeleteStackLsp
                    (MPLS_MLIB_FTN_DELETE, pDestSrRtrInfo, NULL, &SrTeLblStack,
                     pSrRtrNextHopInfo, SR_FALSE) != SR_SUCCESS)
                {
                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d SrMplsCreateOrDeleteStackLsp FAILURE for destination %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                }
                SR_TRC4 (SR_CTRL_TRC,
                         "%s:%d SR-TE FTN deletion is successful via mand for prefix %x with nexthop %x \n",
                         __func__, __LINE__, pDestSrRtrInfo->prefixId.Addr.u4Addr,
                         nextHopAddr.Addr.u4Addr);

#ifdef CFA_WANTED
                u4MplsTnlIfIndex = pSrRtrNextHopInfo->u4OutIfIndex;

                if (CfaUtilGetIfIndexFromMplsTnlIf
                    (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
                {

                    if (CfaIfmDeleteStackMplsTunnelInterface
                        (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                    {
                        SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                                 __func__, __LINE__, u4MplsTnlIfIndex,
                                 u4L3VlanIf);
                        return SR_FAILURE;
                    }
                }

                pSrRtrNextHopInfo->u4OutIfIndex = SR_ZERO;
#endif
                TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                                (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                MEMSET (&(pSrTeRtInfo->SrPriTeLblStack), 0,
                        sizeof (tSrTeLblStack));

                /* Setting MPLS Status to SR_FTN_NOT_CREATED by
                 * unsetting the FTN_CREATED Bit*/
                pSrTeRtInfo->u1MPLSStatus &= (UINT1) (~SR_FTN_CREATED);
                /* Specific Case: If any Route for u4DestAddr is present
                 * and the next-hop is SR-Enabled 'then create SWAP'
                 * else if Route present and next-hop is not SR-Enabled
                 * 'then create POP & FWD'*/

                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                SrTeRtInfo.destAddr.Addr.u4Addr =
                    pSrTeRtInfo->destAddr.Addr.u4Addr;
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.mandRtrId.Addr.u4Addr =
                    pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */
            }
            return SR_SUCCESS;
        }
        else if (destAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            while ((pSrTeRtInfo =
                    SrGetNextTeRouteTableEntry (&SrTeRtInfo)) != NULL)
            {
                u4isRtrEntryPresent = SR_FALSE;
                if (MEMCMP (pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            destAddr->Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH) != SR_ZERO)
                {
                    TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                                  pSrOptRtrInfo, tTMO_SLL_NODE *)
                    {
                        pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                        if (MEMCMP
                            (pSrPeerRtrInfo->routerId.Addr.Ip6Addr.u1_addr,
                             destAddr->Addr.Ip6Addr.u1_addr,
                             SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                        {
                            u4isRtrEntryPresent = SR_TRUE;
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    if (u4isRtrEntryPresent == SR_FALSE)
                    {
                        /*Variable updation for next iteration- Start */
                        MEMSET (&SrTeRtInfo, SR_ZERO,
                                sizeof (tSrTeRtEntryInfo));
                        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                                pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);
                        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                                pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);
                        SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                        SrTeRtInfo.destAddr.u2AddrType =
                            pSrTeRtInfo->destAddr.u2AddrType;
                        SrTeRtInfo.mandRtrId.u2AddrType =
                            pSrTeRtInfo->mandRtrId.u2AddrType;
                        pSrTeRtInfo = NULL;
                        /*Variable updation for next iteration- End */
                        continue;
                    }
                }

                if ((pSrTeRtInfo->u1MPLSStatus & SR_FTN_CREATED) !=
                    SR_FTN_CREATED)
                {
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d FTN not created for TE-Path wit hDEst : %x\n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);

                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }

                /* Get the SrRtrInfo belongs to this SrTeRt */
                if ((pDestSrRtrInfo =
                     SrGetSrRtrInfoFromRtrId (&pSrTeRtInfo->destAddr)) == NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

                /* There will be only one SrRtrNhNode for SR-TE */
                pSrRtrNextHopInfo =
                    (tSrRtrNextHopInfo *)
                    TMO_SLL_First (&(pDestSrRtrInfo->NextHopList));
                if (pSrRtrNextHopInfo == NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s: %d Cannot find the next hop for %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

                /* Delete FTN For TE-Path DestAddr */
                if (SrMplsCreateOrDeleteStackLsp
                    (MPLS_MLIB_FTN_CREATE, pDestSrRtrInfo, NULL, &SrTeLblStack,
                     pSrRtrNextHopInfo, SR_FALSE) != SR_SUCCESS)
                {
                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d SrMplsCreateOrDeleteStackLsp FAILURE for destination %x \n",
                             __func__, __LINE__, destAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }

#ifdef CFA_WANTED
                u4MplsTnlIfIndex = pSrRtrNextHopInfo->u4OutIfIndex;

                if (CfaUtilGetIfIndexFromMplsTnlIf
                    (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
                {

                    if (CfaIfmDeleteStackMplsTunnelInterface
                        (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                    {
                        SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                                 __func__, __LINE__, u4MplsTnlIfIndex,
                                 u4L3VlanIf);
                        return SR_FAILURE;
                    }
                }

                pSrRtrNextHopInfo->u4OutIfIndex = SR_ZERO;
#endif
                TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                                (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);

                /* Setting MPLS Status to SR_FTN_NOT_CREATED by
                 * unsetting the FTN_CREATED Bit*/
                pSrTeRtInfo->u1MPLSStatus &= (UINT1) (~SR_FTN_CREATED);
                pDestSrRtrInfo->u1TeSwapStatus = SR_FALSE;
                pDestSrRtrInfo->u4TeSwapOutIfIndex = SR_ZERO;
                MEMSET (&pDestSrRtrInfo->teSwapNextHop, SR_ZERO,
                        sizeof (tGenU4Addr));

                /* Specific Case: If any Route for u4DestAddr is present
                 * and the next-hop is SR-Enabled 'then create SWAP'
                 * else if Route present and next-hop is not SR-Enabled
                 * 'then create POP & FWD'*/

                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                        pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                        pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */
            }
        }

    }
    SR_TRC3 (SR_CTRL_TRC,
             "%s:%d SR TE LSP Process(creation/deletion) is successfull  via mandatory%x \n",
             __func__, __LINE__, destAddr->Addr.u4Addr);
    return SR_SUCCESS;
}

/************************************************************************
 * Function Name   : SrTeProcess
 * Description     : Function to create SR TE LSPs and SR TE LFA Lsps
 *                   when lfa is enabled in SR
 * Input           : NONE
 * Output          : None
 * Returns         : SR_FAILURE/SR_SUCCESS
 *************************************************************************/
INT4
SrTeProcess (tGenU4Addr * pDestAddr, UINT4 u4TeEnable)
{
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrTeRtEntryInfo   *pSrTeDestRtInfo = NULL;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tGenU4Addr          tmpAddr;
    tGenU4Addr          tmpNextHopAddr;
    INT4                i4LfaStatus = SR_ZERO;

    MEMSET (&tmpAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&tmpNextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;

    nmhGetFsSrV4AlternateStatus (&i4LfaStatus);
    /*  We will fetch the SR router info for the prefix and if the prefix exists in the SR_TE Rbtree,
       will process SR TE LSP creation for for the prefix by removing LFA LSPs and
       Primary LSP exists for the same prefix */

    /*Get SrRtrInfo for DestAddr for which TE path is to be created */
    if ((pRtrInfo = SrGetSrRtrInfoFromRtrId (pDestAddr)) == NULL)
    {
        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for %x \n",
                 __func__, __LINE__, pDestAddr->Addr.u4Addr);
        return SR_FAILURE;
    }
    else
    {
        pSrTeDestRtInfo = SrGetTeRouteEntryFromDestAddr (&pRtrInfo->prefixId);
        if (pSrTeDestRtInfo != NULL)
        {
            if (u4TeEnable == SR_FALSE)    /* no form of SR_TE configuration */
            {
                /* To delete TE LSP in CLI flow */
                /* ST TE LFA lsp will also be deleted, if it exists for the prefix */

                if (pSrTeDestRtInfo->LfaNexthop.Addr.u4Addr != SR_ZERO)
                {
                    if (SrTeFRRFtnCreateOrDel (pSrTeDestRtInfo, SR_FALSE) ==
                        SR_FAILURE)
                    {
                        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d SR TE LFA lsp deletion failed for prefix %x \n",
                                 __func__, __LINE__, pDestAddr->Addr.u4Addr);
                    }
                }

                /* To delete TE LSP in CLI flow */
                /* Case 1.1:  SR TE LSP deletion via destination prefix */
                if (SrTeFtnCreateOrDel (pSrTeDestRtInfo, SR_FALSE) ==
                    SR_FAILURE)
                {
                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d SR TE LSP deletion failed via destination flow for prefix %x \n",
                             __func__, __LINE__, pDestAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }
                SR_TRC3 (SR_CTRL_TRC,
                         "%s:%d SR TE LSP deletion is successful via destination flow for prefix %x \n",
                         __func__, __LINE__, pDestAddr->Addr.u4Addr);

                pRtrInfo->bIsTeconfigured = SR_FALSE;
                pRtrInfo->u1TeSwapStatus = SR_FALSE;
                return SR_SUCCESS;

            }
            else                /* SR TE processing via cli */
            {
                if (SrDelFTNWithFRRForTE (pDestAddr, pRtrInfo) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             " line %d func %s SR Primary LSP Deletion failed for DESTIP %x. So, TE LSP wont be created\r\n",
                             __LINE__, __func__, tmpAddr.Addr.u4Addr);
                    if (SrDeleteTeRouteTableEntry (pSrTeDestRtInfo) ==
                        SR_FAILURE)
                    {
                        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d SR-TE path Deletion Failure for Prefix : %x \n",
                                 __func__, __LINE__,
                                 pSrTeDestRtInfo->destAddr.Addr.u4Addr);
                    }

                    return SR_FAILURE;
                }
                /* SR TE Processing via destination address flow */
                if (SrTeFtnIlmCreateDelViaDestRt (pSrTeDestRtInfo, SR_TRUE) ==
                    SR_FAILURE)
                {
                    SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d SR TE LSP creation failed via destination flow for %x \n",
                             __func__, __LINE__, pDestAddr->Addr.u4Addr);
                    return SR_FAILURE;
                }
                /* SR TE LFA LSP processing */
                if (i4LfaStatus == ALTERNATE_ENABLED)
                {
                    if (SrTeLfaRouteUpdate
                        (&pSrTeDestRtInfo->destAddr,
                         &pSrTeDestRtInfo->mandRtrId) == SR_SUCCESS)
                    {
                        if (SrTeLfaCreateFtnIlm (pSrTeDestRtInfo) == SR_FAILURE)
                        {
                            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SR TE LFA lsp creation failed for prefix %x \n",
                                     __func__, __LINE__,
                                     pDestAddr->Addr.u4Addr);
                        }
                        SR_TRC3 (SR_CTRL_TRC,
                                 "%s:%d SR TE LFA lsp creation is successful for prefix %x \n",
                                 __func__, __LINE__, pDestAddr->Addr.u4Addr);
                    }
                    else
                    {
                        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 " Line %d func %s SR TE LFA route updated failed for %x \r\n",
                                 __LINE__, __func__, pDestAddr->Addr.u4Addr);
                        return SR_FAILURE;
                    }
                }

            }

        }
    }
    return SR_SUCCESS;

}

/************************************************************************
**  Function Name   : SrUtilTeRouteRbTreeCmpFunc
**  Description     : RBTree Compare function for TE-Paths
**  Input           : Two RBTree Nodes to be compared
**  Output          : None
**  Returns         : SR_ZERO(FOUND)/SR_ONE(NOT_FOUND)
*************************************************************************/
PUBLIC INT4
SrUtilTeRouteRbTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2)
{
    tSrTeRtEntryInfo   *srTeRtEntry1 = (tSrTeRtEntryInfo *) pRBElem1;
    tSrTeRtEntryInfo   *srTeRtEntry2 = (tSrTeRtEntryInfo *) pRBElem2;
#ifdef MPLS_IPV6_WANTED
    if (srTeRtEntry1->destAddr.u2AddrType < srTeRtEntry2->destAddr.u2AddrType)
    {
        return -1;
    }
    else if (srTeRtEntry1->destAddr.u2AddrType >
             srTeRtEntry2->destAddr.u2AddrType)
    {
        return 1;
    }

    if (SR_IPV6_ADDR_TYPE == srTeRtEntry1->destAddr.u2AddrType)
    {
        return (MEMCMP (srTeRtEntry1->destAddr.Addr.Ip6Addr.u1_addr,
                        srTeRtEntry2->destAddr.Addr.Ip6Addr.u1_addr,
                        IPV6_ADDR_LENGTH));

    }
    else
#endif
    {
        if (srTeRtEntry1->destAddr.Addr.u4Addr >
            srTeRtEntry2->destAddr.Addr.u4Addr)
        {
            return SR_RB_LESS;;

        }
        else if (srTeRtEntry1->destAddr.Addr.u4Addr <
                 srTeRtEntry2->destAddr.Addr.u4Addr)
        {
            return SR_RB_GREATER;
        }
        if (srTeRtEntry1->mandRtrId.Addr.u4Addr >
            srTeRtEntry2->mandRtrId.Addr.u4Addr)
        {
            return SR_RB_GREATER;
        }
        else if (srTeRtEntry1->mandRtrId.Addr.u4Addr <
                 srTeRtEntry2->mandRtrId.Addr.u4Addr)
        {
            return SR_RB_LESS;
        }

        return SR_RB_EQUAL;
    }

}

/************************************************************************
**  Function Name   : SrGetTeRouteEntryFromDestAddr
**  Description     : Fetches SR TE routes from SR TE RBTREE
**  Input           : Destination IP
**  Output          : (tSrTeRtEntryInfo) SR TE RT infor
**  Returns         : NONE
*************************************************************************/

tSrTeRtEntryInfo   *
SrGetTeRouteEntryFromDestAddr (tGenU4Addr * pDestAddr)
{
    tSrTeRtEntryInfo   *pSrTeRouteInfo = NULL;
    tSrTeRtEntryInfo    SrTeRouteInfo;

    MEMSET (&SrTeRouteInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    SR_TRC3 (SR_CTRL_TRC, "%s:%d Entry with %x \n", __func__, __LINE__,
             pDestAddr->Addr.u4Addr);
    while ((pSrTeRouteInfo =
            SrGetNextTeRouteTableEntry (&SrTeRouteInfo)) != NULL)
    {
        if (pSrTeRouteInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            if (pSrTeRouteInfo->destAddr.Addr.u4Addr == pDestAddr->Addr.u4Addr)
            {
                SR_TRC2 (SR_CTRL_TRC, "%s:%d Exit: SUCCESS \n", __func__,
                         __LINE__);
                return pSrTeRouteInfo;
            }
            else
            {
                SrTeRouteInfo.destAddr.Addr.u4Addr =
                    pSrTeRouteInfo->destAddr.Addr.u4Addr;
                SrTeRouteInfo.u4DestMask = pSrTeRouteInfo->u4DestMask;
                SrTeRouteInfo.mandRtrId.Addr.u4Addr =
                    pSrTeRouteInfo->mandRtrId.Addr.u4Addr;
            }
        }
        else if (pSrTeRouteInfo->destAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            if (MEMCMP (pSrTeRouteInfo->destAddr.Addr.Ip6Addr.u1_addr,
                        pDestAddr->Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                SR_TRC2 (SR_CTRL_TRC, "%s:%d Exit: SUCCESS \n", __func__,
                         __LINE__);
                return pSrTeRouteInfo;
            }
            else
            {
                MEMCPY (SrTeRouteInfo.destAddr.Addr.Ip6Addr.u1_addr,
                        pSrTeRouteInfo->destAddr.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                MEMCPY (SrTeRouteInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                        pSrTeRouteInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                SrTeRouteInfo.u4DestMask = pSrTeRouteInfo->u4DestMask;
            }
        }
        SrTeRouteInfo.destAddr.u2AddrType = pSrTeRouteInfo->destAddr.u2AddrType;
        SrTeRouteInfo.mandRtrId.u2AddrType =
            pSrTeRouteInfo->mandRtrId.u2AddrType;
    }

    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Exit: FAILURE for %x \n", __func__, __LINE__,
             pDestAddr->Addr.u4Addr);
    return NULL;
}

/************************************************************************
 * Function Name   :SrGetTeRouteTableEntry
 * Description     : Function to get TePath Entry with DestAddr as its
                     prime key
 * Input           : pSrTeRtInfo
 * Output          : None
 * Returns         : Pointer to TePath Entry or NULL
 *************************************************************************/
tSrTeRtEntryInfo   *
SrGetTeRouteTableEntry (tSrTeRtEntryInfo * pSrTeRtInfo)
{
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrTeRtEntryInfo    dummySrTeRtInfo;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&dummySrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL || pSrTeRtInfo == NULL)
    {
        return NULL;
    }

    SrTeRtInfo.destAddr.Addr.u4Addr = pSrTeRtInfo->destAddr.Addr.u4Addr;
    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
    SrTeRtInfo.mandRtrId.Addr.u4Addr = pSrTeRtInfo->mandRtrId.Addr.u4Addr;
    SrTeRtInfo.mandRtrId.u2AddrType = pSrTeRtInfo->mandRtrId.u2AddrType;
    SrTeRtInfo.destAddr.u2AddrType = pSrTeRtInfo->destAddr.u2AddrType;
    if (pSrTeRtInfo->destAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
    {

        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
    }
    if (SrTeRtInfo.mandRtrId.Addr.u4Addr != SR_ZERO)
    {
        return (RBTreeGet (gSrGlobalInfo.pSrTeRouteRbTree, &SrTeRtInfo));
    }
    return (RBTreeGetNext (gSrGlobalInfo.pSrTeRouteRbTree, &SrTeRtInfo, NULL));
}

/************************************************************************
**  Function Name   : SrGetLabelForLFATEMandNode
**  Description     : To compute Top label for the Lfa Mandatory node
**  Input           : pLfaMandNode,pMandNode
**  Output          : pOutLabel
**  Returns         : SR_ZERO(FOUND)/SR_ONE(NOT_FOUND)
*************************************************************************/
INT4
SrGetLabelForLFATEMandNode (tGenU4Addr * pLfaMandNode, tGenU4Addr * pMandNode,
                            UINT4 *pOutLabel)
{
    tSrRtrInfo         *pSrLfaMandNodeRtrInfo = NULL;
    tSrRtrInfo         *pSrMandNodeRtrInfo = NULL;
    UINT4               u4MandSidValue = SR_ZERO;
    UINT4               u4LfaMandSrGb = SR_ZERO;

    /* To fetch SR RTR info for Lfa mandatory node */
    pSrLfaMandNodeRtrInfo = SrGetSrRtrInfoFromRtrId (pLfaMandNode);
    /* To fetch SR RTR info for mandatory node */
    pSrMandNodeRtrInfo = SrGetSrRtrInfoFromRtrId (pMandNode);
    if ((pSrLfaMandNodeRtrInfo == NULL) || (pSrMandNodeRtrInfo == NULL))
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                 "%s:%d Failed to Fetch SR Rtr info for Lfa Mandatory node or Mandatory node\n ",
                 __func__, __LINE__);
        return SR_FAILURE;
    }
    else
    {
        if (pSrLfaMandNodeRtrInfo->srgbRange.u4SrGbMinIndex <= SR_ZERO)
        {

            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d SRGB is zero, can't compute LFA mandatory node label\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        u4LfaMandSrGb = pSrLfaMandNodeRtrInfo->srgbRange.u4SrGbMinIndex;

        u4MandSidValue = pSrMandNodeRtrInfo->u4SidValue;
    }
    /* Top Label of Sr TE LFA = Minimum SRGB of Lfa-Mandatory node + Mandatory node's sid value */
    *pOutLabel = u4LfaMandSrGb + u4MandSidValue;
    return SR_SUCCESS;
}

/************************************************************************
 * Function Name   : SrTeLfaCreateFtnIlm
 * Description     : Function to Create TE LFA Ftn and Ilm for the configured 
                     TE config
 * Input           : u4DestAddr
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesfully creation)/SR_FAILURE
 *************************************************************************/
INT4
SrTeLfaCreateFtnIlm (tSrTeRtEntryInfo * pSrTeRtEntryInfo)
{
    tSrRtrInfo         *pTempSrRtrInfo = NULL;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrTeRtEntryInfo    getSrTeRtEntryInfo;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrRtrInfo          LfaTeDestSrRtrInfo;
    tSrRtrNextHopInfo   NewSrRtrNextHopInfo;
    tSrTeLblStack       SrTeLblStack;
    tGenU4Addr          nextHopAddr;
    tGenU4Addr          dummyAddr;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4TopLabel = SR_ZERO;
    UINT4               u4DestLabel = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4IpAddress = SR_ZERO;
    UINT4               u4OptLabelCount = SR_ZERO;
    UINT2               u2MlibOperation = MPLS_MLIB_FTN_CREATE;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&getSrTeRtEntryInfo, 0, sizeof (tSrTeRtEntryInfo));
    MEMSET (&NewSrRtrNextHopInfo, 0, sizeof (tSrRtrNextHopInfo));
    MEMSET (&LfaTeDestSrRtrInfo, 0, sizeof (tSrRtrInfo));

    if (pSrTeRtEntryInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        if ((pSrTeRtEntryInfo->u1LfaMPLSStatus & SR_FTN_CREATED) ==
            SR_FTN_CREATED)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d  Lfa FTN already exists for this prefix%x \n",
                     __func__, __LINE__,
                     pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }
        /*To fetch router-info from the LFA nh */
        pTempSrRtrInfo =
            SrGetRtrInfoFromNextHop (&pSrTeRtEntryInfo->LfaNexthop);
        if (pTempSrRtrInfo == NULL)
        {
            SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d SR router info fetch failed for D %x  Lfa Nexthop %x \n",
                     __func__, __LINE__, pSrTeRtEntryInfo->destAddr.Addr.u4Addr,
                     pSrTeRtEntryInfo->LfaNexthop);

            return SR_FAILURE;
        }
        pSrTeRtEntryInfo->LfaMandRtrId.Addr.u4Addr =
            pTempSrRtrInfo->prefixId.Addr.u4Addr;
        pSrTeRtEntryInfo->LfaMandRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;
        if (SrGetLabelForLFATEMandNode
            (&pSrTeRtEntryInfo->LfaMandRtrId, &pSrTeRtEntryInfo->mandRtrId,
             &u4TopLabel) == SR_FAILURE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d Unable to get TE LFA mandatory node %x label\n",
                     __func__, __LINE__,
                     pSrTeRtEntryInfo->LfaMandRtrId.Addr.u4Addr);
            return SR_FAILURE;
        }
        if (SrTeGetLabelForRtrId (&pSrTeRtEntryInfo->destAddr,
                                  &pSrTeRtEntryInfo->mandRtrId,
                                  &u4DestLabel) == SR_FAILURE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d Unable to get u4DestLabel for prefix %x \n", __func__,
                     __LINE__, pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }
        /*Get Best Next-Hop & u4OutIfIndex for MandRtrId */
        u4IpAddress = (pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr & 0xFFFFFFFF);
        if (CfaIpIfGetIfIndexForNetInCxt (SR_ZERO, u4IpAddress,
                                          &u4OutIfIndex) == CFA_FAILURE)
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Exiting: Not able to get Out IfIndex \n",
                     __func__, __LINE__);
        }
#ifdef CFA_WANTED
        /*u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
        if (CfaIfmCreateStackMplsTunnelInterface (u4OutIfIndex,
                                                  &u4MplsTnlIfIndex)
            == CFA_SUCCESS)
        {
            SR_TRC4 (SR_CTRL_TRC,
                     "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                     __func__, __LINE__, u4OutIfIndex, u4MplsTnlIfIndex);

            u4OutIfIndex = u4MplsTnlIfIndex;
            pSrTeRtEntryInfo->u4OutLfaIfIndex = u4OutIfIndex;
        }
        else
        {
            SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                     __func__, __LINE__, u4OutIfIndex, u4MplsTnlIfIndex);
            return SR_FAILURE;
        }
#endif
        /* Nexthop info update */
        NewSrRtrNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        NewSrRtrNextHopInfo.nextHop.Addr.u4Addr =
            pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr;
        NewSrRtrNextHopInfo.u4OutIfIndex = u4OutIfIndex;
        pSrTeRtEntryInfo->u4OutLfaIfIndex = u4OutIfIndex;

        /* Set the TopLabel and the following Label Stack
         * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
        SrTeLblStack.u4TopLabel = u4TopLabel;
        SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;

        SrTeLblStack.u1StackSize = (UINT1) (u4OptLabelCount + SR_ONE);

        /*Get SrRtrInfo for DestAddr for which TE path is to be created */
        if ((pDestSrRtrInfo =
             SrGetSrRtrInfoFromRtrId (&pSrTeRtEntryInfo->destAddr)) == NULL)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for prefix %x \n",
                     __func__, __LINE__,
                     pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }
        /* SR Route infor is copied to a Local variable to process SR TE LFA Lsp */
        MEMCPY ((&LfaTeDestSrRtrInfo), pDestSrRtrInfo, sizeof (tSrRtrInfo));

        /* Create FTN */
        if (SrMplsCreateOrDeleteStackLsp
            (u2MlibOperation, &LfaTeDestSrRtrInfo, NULL, &SrTeLblStack,
             &NewSrRtrNextHopInfo, SR_TRUE) == SR_FAILURE)
        {
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "line %d func %s -----SR TE LFA FTN creation failed  for DEST %x nexthop %x  \r\n",
                     __LINE__, __func__, pSrTeRtEntryInfo->destAddr.Addr.u4Addr,
                     NewSrRtrNextHopInfo.nextHop.Addr.u4Addr);
            return SR_FAILURE;

        }
        else
        {
            SR_TRC4 (SR_CTRL_TRC,
                     "line %d func %s -----SR TE LFA FTN sucessfully created for DEST %x nexthop %x  \r\n",
                     __LINE__, __func__, pSrTeRtEntryInfo->destAddr.Addr.u4Addr,
                     NewSrRtrNextHopInfo.nextHop.Addr.u4Addr);

        }

        if (pDestSrRtrInfo->u4LfaSwapOutIfIndex == SR_ZERO)
        {
            SR_TRC3 (SR_CTRL_TRC,
                     "%s:%d FRR ILM doesnot Exist so calling SrTeFRRIlm Create for %x \n",
                     __func__, __LINE__, LfaTeDestSrRtrInfo.prefixId.Addr.u4Addr);
            /* FRR ILM creation based on SR-LFA nexthop */
            if (SrTeFRRIlmAddorDelete
                (&LfaTeDestSrRtrInfo.prefixId, pSrTeRtEntryInfo,
                 SR_TRUE) == SR_FAILURE)
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d SrTeFRRIlmAddorDelete FAILURE for destination %x \n",
                         __func__, __LINE__,
                         LfaTeDestSrRtrInfo.prefixId.Addr.u4Addr);
            }
        }
        pSrTeRtEntryInfo->u1LfaMPLSStatus = SR_FTN_CREATED;

    }

    return SR_SUCCESS;
}

/************************************************************************
 * Function Name   : SrTeLfaRouteUpdate
 * Description     : Function  is to fetch SR TE LFA details and update 
                     the RBtree
 * Input           : pDestAddr,pMandatoryRtrId
 * Output          : NONE
 * Returns         : SR_SUCCESS (on succesfully creation)/SR_FAILURE
 *************************************************************************/

UINT4
SrTeLfaRouteUpdate (tGenU4Addr * pDestAddr, tGenU4Addr * pMandatoryRtrId)
{
    tSrTeRtEntryInfo   *pSrTeRtEntryInfo = NULL;
    tSrTeRtEntryInfo    getSrTeRtEntryInfo;
    tNetIpv4LfaRtInfo   NetIpLfaRtInfo;
    INT4                i4LfaStatus = SR_ZERO;

    MEMSET (&getSrTeRtEntryInfo, 0, sizeof (tSrTeRtEntryInfo));
    MEMSET (&NetIpLfaRtInfo, 0, sizeof (tNetIpv4LfaRtInfo));

    /* To get SR_LFA status */
    nmhGetFsSrV4AlternateStatus (&i4LfaStatus);
    if (i4LfaStatus != ALTERNATE_ENABLED)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "line %d func %s SR FRR is not enabled\r\n",
                 __LINE__, __func__);
        return SR_FAILURE;
    }
    /* SR TE LFA DATAstructure update */
    getSrTeRtEntryInfo.destAddr.Addr.u4Addr = pDestAddr->Addr.u4Addr;
    getSrTeRtEntryInfo.mandRtrId.Addr.u4Addr = pMandatoryRtrId->Addr.u4Addr;
    getSrTeRtEntryInfo.destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    getSrTeRtEntryInfo.mandRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;

    pSrTeRtEntryInfo =
        RBTreeGet (gSrGlobalInfo.pSrTeRouteRbTree, &getSrTeRtEntryInfo);
    if (pSrTeRtEntryInfo == NULL)
    {
        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                 "line %d func %s SR TE route doesn't exist for this prefix %x , So SR TE LFA computation is not supported\r\n",
                 __LINE__, __func__, pDestAddr->Addr.u4Addr);

        return SR_FAILURE;
    }

    /* If SR LFA is enabled, need to update LFA nexthop of a destination for Sr TE LFA route */

    if (SrTeLfaRtQuery (pMandatoryRtrId, &NetIpLfaRtInfo) == SR_SUCCESS)
    {
        if (NetIpLfaRtInfo.u1LfaType == SR_LFA)
        {
            pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr =
                NetIpLfaRtInfo.u4LfaNextHop;
            pSrTeRtEntryInfo->LfaNexthop.u2AddrType = SR_IPV4_ADDR_TYPE;

        }
    }
    /* LFA Route query for destination */
    MEMSET (&NetIpLfaRtInfo, 0, sizeof (tNetIpv4LfaRtInfo));
    if (SrTeLfaRtQuery (pDestAddr, &NetIpLfaRtInfo) == SR_SUCCESS)
    {
        if (NetIpLfaRtInfo.u1LfaType == SR_LFA)
        {
            pSrTeRtEntryInfo->FrrIlmNexthop.Addr.u4Addr =
                NetIpLfaRtInfo.u4LfaNextHop;
            pSrTeRtEntryInfo->FrrIlmNexthop.u2AddrType = SR_IPV4_ADDR_TYPE;
        }
    }

    return SR_SUCCESS;
}

/************************************************************************
 * Function Name   : SrTeLfaDeleteFtnIlm
 * Description     : Function to delete TE LFA ftn and Ilm for the 
                      configured TE prefix
 * Input           : pSrTeRtEntryInfo
 * Output          : NONE
 * Returns         : SR_SUCCESS (on succesfully creation)/SR_FAILURE
 *************************************************************************/
INT4
SrTeLfaDeleteFtnIlm (tSrTeRtEntryInfo * pSrTeRtEntryInfo)
{
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrTeRtEntryInfo    getSrTeRtEntryInfo;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrRtrInfo          LfaTeDestSrRtrInfo;
    tSrRtrNextHopInfo   NewSrRtrNextHopInfo;
    tSrTeLblStack       SrTeLblStack;
    tGenU4Addr          nextHopAddr;
    tGenU4Addr          dummyAddr;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4TopLabel = SR_ZERO;
    UINT4               u4DestLabel = SR_ZERO;
    UINT4               u4IpAddress = SR_ZERO;
    UINT4               u4OptLabelCount = SR_ZERO;

    tSrInSegInfo        SrInSegInfo;

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&getSrTeRtEntryInfo, 0, sizeof (tSrTeRtEntryInfo));
    MEMSET (&NewSrRtrNextHopInfo, 0, sizeof (tSrRtrNextHopInfo));
    MEMSET (&LfaTeDestSrRtrInfo, 0, sizeof (tSrRtrInfo));

    if (pSrTeRtEntryInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        if ((pSrTeRtEntryInfo->u1LfaMPLSStatus & SR_FTN_CREATED) ==
            SR_FTN_CREATED)
        {

            if (SrGetLabelForLFATEMandNode
                (&pSrTeRtEntryInfo->LfaMandRtrId, &pSrTeRtEntryInfo->mandRtrId,
                 &u4TopLabel) == SR_FAILURE)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d Unable to get TE LFA mandatory node label\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            if (SrTeGetLabelForRtrId (&pSrTeRtEntryInfo->destAddr,
                                      &pSrTeRtEntryInfo->mandRtrId,
                                      &u4DestLabel) == SR_FAILURE)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d Unable to get u4DestLabel for prefix %x \n",
                         __func__, __LINE__,
                         pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                return SR_FAILURE;
            }
            /*Get Best Next-Hop & u4OutIfIndex for MandRtrId */
            u4IpAddress =
                (pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr & 0xFFFFFFFF);
            if (CfaIpIfGetIfIndexForNetInCxt
                (SR_ZERO, u4IpAddress, &u4OutIfIndex) == CFA_FAILURE)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d Exiting: Not able to get Out IfIndex \n",
                         __func__, __LINE__);
            }

            /* Nexthop info update */
            NewSrRtrNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
            NewSrRtrNextHopInfo.nextHop.Addr.u4Addr =
                pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr;
            NewSrRtrNextHopInfo.u4OutIfIndex =
                pSrTeRtEntryInfo->u4OutLfaIfIndex;
            NewSrRtrNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
            NewSrRtrNextHopInfo.bIsFRRHwLsp = SR_FALSE;
            /* Set the TopLabel and the following Label Stack
             * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
            SrTeLblStack.u4TopLabel = u4TopLabel;
            SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;

            SrTeLblStack.u1StackSize = (UINT1) (u4OptLabelCount + SR_ONE);

            /*Get SrRtrInfo for DestAddr for which TE path is to be created */
            if ((pDestSrRtrInfo =
                 SrGetSrRtrInfoFromRtrId (&pSrTeRtEntryInfo->destAddr)) == NULL)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for prefix %x \n",
                         __func__, __LINE__,
                         pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                return SR_FAILURE;
            }
            /* SR Route infor is copied to a Local variable to process SR TE LFA Lsp */
            MEMCPY ((&LfaTeDestSrRtrInfo), pDestSrRtrInfo, sizeof (tSrRtrInfo));

            /* Delete FRR ILM for the TE DEST */
            if (SrTeFRRIlmAddorDelete
                (&LfaTeDestSrRtrInfo.prefixId, pSrTeRtEntryInfo,
                 SR_FALSE) == SR_FAILURE)
            {

                SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d SrTeFRRIlmAddorDelete FAILURE for destination %x with nh %x \n",
                         __func__, __LINE__,
                         LfaTeDestSrRtrInfo.prefixId.Addr.u4Addr,
                         nextHopAddr.Addr.u4Addr);
            }
            /* Delete FTN */
            if (SrMplsCreateOrDeleteStackLsp
                (MPLS_MLIB_FTN_DELETE, &LfaTeDestSrRtrInfo, NULL, &SrTeLblStack,
                 &NewSrRtrNextHopInfo, SR_TRUE) == SR_FAILURE)
            {
                SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "line %d func %s -----SR TE LFA FTN deletion failed  for DEST %x nexthop %x  \r\n",
                         __LINE__, __func__,
                         pSrTeRtEntryInfo->destAddr.Addr.u4Addr,
                         NewSrRtrNextHopInfo.nextHop.Addr.u4Addr);
                return SR_FAILURE;

            }
            else
            {
                SR_TRC4 (SR_CTRL_TRC,
                         "line %d func %s -----SR TE LFA FTN sucessfully deleted for DEST %x nexthop %x  \r\n",
                         __LINE__, __func__,
                         pSrTeRtEntryInfo->destAddr.Addr.u4Addr,
                         NewSrRtrNextHopInfo.nextHop.Addr.u4Addr);

            }
            pSrTeRtEntryInfo->u1LfaMPLSStatus &= (UINT1) (~SR_FTN_CREATED);
            pSrTeRtEntryInfo->u4OutLfaIfIndex = SR_ZERO;
            MEMSET (&pSrTeRtEntryInfo->LfaMandRtrId, SR_ZERO,
                    sizeof (tGenU4Addr));
            MEMSET (&pSrTeRtEntryInfo->LfaNexthop, SR_ZERO,
                    sizeof (tGenU4Addr));

        }

    }

    return SR_SUCCESS;
}

/************************************************************************
 * Function Name   :SrGetNextTeRouteTableEntry
 * Description     : Function to get TePath Entry from DestAddr, Mask
 *                   & MandRtrId.
 * Input           : pSrTeRtInfo
 * Output          : None
 * Returns         : Pointer to TePath Entry or NULL
 *************************************************************************/
tSrTeRtEntryInfo   *
SrGetNextTeRouteTableEntry (tSrTeRtEntryInfo * pSrTeRtInfo)
{
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrTeRtEntryInfo    dummySrTeRtInfo;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&dummySrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));

    if (gSrGlobalInfo.pSrTeRouteRbTree == NULL || pSrTeRtInfo == NULL)
    {
        return NULL;
    }

    if (MEMCMP (pSrTeRtInfo, &dummySrTeRtInfo, sizeof (tSrTeRtEntryInfo)) ==
        SR_ZERO)
    {
        return (RBTreeGetFirst (gSrGlobalInfo.pSrTeRouteRbTree));
    }
    SrTeRtInfo.destAddr.Addr.u4Addr = pSrTeRtInfo->destAddr.Addr.u4Addr;
    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
    SrTeRtInfo.mandRtrId.Addr.u4Addr = pSrTeRtInfo->mandRtrId.Addr.u4Addr;
    SrTeRtInfo.mandRtrId.u2AddrType = pSrTeRtInfo->mandRtrId.u2AddrType;
    SrTeRtInfo.destAddr.u2AddrType = pSrTeRtInfo->destAddr.u2AddrType;
    if (pSrTeRtInfo->destAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
    {

        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
    }
    return (RBTreeGetNext (gSrGlobalInfo.pSrTeRouteRbTree, &SrTeRtInfo, NULL));
}

tSrTeRtEntryInfo   *
SrCreateTeRouteTableEntry (tSrTeRtEntryInfo * pSrTeRtInfo)
{
    tSrTeRtEntryInfo   *pNewSrTeRtInfo = NULL;
    UINT4               u4RtrListIndex = SR_ZERO;
    UINT4               u4ErrCode = SR_ZERO;
    INT4                i4RetVal = SR_ZERO;
    UINT1               au1DestAddr[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1MandatoryRtrId[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1DestAddrV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };
    UINT1               au1MandatoryRtrIdV6[SR_IPV6_ADDR_LENGTH] = { SR_ZERO };

    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE snmpMandatoryRtrId;
    tSNMP_OCTET_STRING_TYPE *pSnmpDestAddr = NULL;
    tSNMP_OCTET_STRING_TYPE *pSnmpMandatoryRtrId = NULL;

    /*Allocate memory for tSrTeRtEntryInfo */
    if (SR_TE_RT_ENTRY_MEM_ALLOC (pNewSrTeRtInfo) == NULL)
    {
        return NULL;
    }

    MEMSET (pNewSrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&snmpDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&snmpMandatoryRtrId, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));

    if (pSrTeRtInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        snmpDestAddr.pu1_OctetList = au1DestAddr;
        snmpMandatoryRtrId.pu1_OctetList = au1MandatoryRtrId;

        pNewSrTeRtInfo->destAddr.Addr.u4Addr =
            pSrTeRtInfo->destAddr.Addr.u4Addr;
        pNewSrTeRtInfo->mandRtrId.Addr.u4Addr =
            pSrTeRtInfo->mandRtrId.Addr.u4Addr;
    }
    else if (pSrTeRtInfo->destAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        snmpDestAddr.pu1_OctetList = au1DestAddrV6;
        snmpMandatoryRtrId.pu1_OctetList = au1MandatoryRtrIdV6;

        MEMCPY (pNewSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        MEMCPY (pNewSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
    }

    pNewSrTeRtInfo->mandRtrId.u2AddrType = pSrTeRtInfo->mandRtrId.u2AddrType;
    pNewSrTeRtInfo->destAddr.u2AddrType = pSrTeRtInfo->destAddr.u2AddrType;
    pNewSrTeRtInfo->u4DestMask = pSrTeRtInfo->u4DestMask;
    pSnmpDestAddr = &snmpDestAddr;
    pSnmpMandatoryRtrId = &snmpMandatoryRtrId;

    TMO_SLL_Init (&pNewSrTeRtInfo->srTeRtrListIndex.RtrList);

    i4RetVal = nmhGetFsRouterListIndexNext (&(u4RtrListIndex));
    if (u4RtrListIndex == SR_ZERO)
    {
        SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC, "TE List Index not available : %d\n",
                 u4RtrListIndex);
        SR_TE_RT_ENTRY_MEM_FREE (pNewSrTeRtInfo);
        return NULL;
    }
    u4RtrListIndex = SrOptRtrListSetIndex (u4RtrListIndex);

    if (RBTreeAdd (gSrGlobalInfo.pSrTeRouteRbTree, (tRBElem *) pNewSrTeRtInfo)
        == RB_FAILURE)
    {
        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                "Create Sr-Te-Path Table : RBTree Node Add Failed \n");
        SrOptRtrListRelIndex (u4RtrListIndex);
        SR_TE_RT_ENTRY_MEM_FREE (pNewSrTeRtInfo);
        return NULL;
    }
    nmhTestv2FsSrTeRouterListIndex (&u4ErrCode, NULL, SR_ZERO, NULL,
                                    (INT4) pSrTeRtInfo->destAddr.u2AddrType,
                                    u4RtrListIndex);
    if (SNMP_ERR_NO_ERROR == u4ErrCode)
    {

        if (pSrTeRtInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            SR_INTEGER_TO_OCTETSTRING (pNewSrTeRtInfo->destAddr.Addr.u4Addr,
                                       pSnmpDestAddr);
            SR_INTEGER_TO_OCTETSTRING (pNewSrTeRtInfo->mandRtrId.Addr.u4Addr,
                                       pSnmpMandatoryRtrId);
        }
        else if (pSrTeRtInfo->destAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (pSnmpDestAddr->pu1_OctetList,
                    pNewSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);
            MEMCPY (pSnmpMandatoryRtrId->pu1_OctetList,
                    pNewSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);

            pSnmpDestAddr->i4_Length = SR_IPV6_ADDR_LENGTH;
            pSnmpMandatoryRtrId->i4_Length = SR_IPV6_ADDR_LENGTH;
        }

        nmhSetFsSrTeRouterListIndex (pSnmpDestAddr, pNewSrTeRtInfo->u4DestMask,
                                     pSnmpMandatoryRtrId,
                                     (INT4) pSrTeRtInfo->destAddr.u2AddrType,
                                     u4RtrListIndex);

    }
    else
    {
        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Invalid Te-Path List Index \n");
        SrOptRtrListRelIndex (u4RtrListIndex);
        SR_TE_RT_ENTRY_MEM_FREE (pNewSrTeRtInfo);
        return NULL;
    }
    UNUSED_PARAM (i4RetVal);
    return pNewSrTeRtInfo;
}

/*****************************************************************************
 * Function Name : SrTeProgramAlternateLsp
 * Description   : This routine programme the SR TE LFA LSP entry in hardware
 *
 * Input(s)      : pSrTeRtEntryInfo
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrTeProgramAlternateLsp (tSrTeRtEntryInfo * pSrTeRtEntryInfo)
{
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tSrTeLblStack       SrTeLblStack;
    tSrRtrInfo          LfaTeDestSrRtrInfo;
    UINT4               u4TopLabel = SR_ZERO;
    UINT4               u4DestLabel = SR_ZERO;
    UINT2               u2MlibOperation = MPLS_MLIB_FTN_MODIFY;

    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&LfaTeDestSrRtrInfo, SR_ZERO, sizeof (tSrRtrInfo));

    if (SrGetLabelForLFATEMandNode (&pSrTeRtEntryInfo->LfaMandRtrId,
                                    &pSrTeRtEntryInfo->mandRtrId,
                                    &u4TopLabel) == SR_FAILURE)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Unable to get TE LFA"
                 " mandatory node label\n", __func__, __LINE__);
        return SR_FAILURE;
    }
    if (SrTeGetLabelForRtrId (&pSrTeRtEntryInfo->destAddr,
                              &pSrTeRtEntryInfo->mandRtrId,
                              &u4DestLabel) == SR_FAILURE)
    {
        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Unable to get u4DestLabel for prefix %x \n",
                 __func__, __LINE__, pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
        return SR_FAILURE;
    }
    /* Nexthop info update */
    SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
    SrRtrAltNextHopInfo.nextHop.Addr.u4Addr =
        pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr;
    SrRtrAltNextHopInfo.u4OutIfIndex = pSrTeRtEntryInfo->u4OutLfaIfIndex;

    /* Set FRRModifyLsp flag which is allowed to programm the entry in Hw */
    SrRtrAltNextHopInfo.bIsFRRModifyLsp = SR_TRUE;

    /* Set the TopLabel and the following Label Stack
     * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
    SrTeLblStack.u4TopLabel = u4TopLabel;
    SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;
    SrTeLblStack.u1StackSize = SR_ONE;

    /*Get SrRtrInfo for DestAddr for which TE path is to be programmed in HW */
    if ((pDestSrRtrInfo = SrGetSrRtrInfoFromRtrId
         (&pSrTeRtEntryInfo->destAddr)) == NULL)
    {
        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrGetSrRtrInfoFromRtrId "
                 "FAILURE for prefix %x \n", __func__, __LINE__,
                 pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    if (SrMplsCreateOrDeleteStackLsp (u2MlibOperation, pDestSrRtrInfo, NULL,
                                   &SrTeLblStack, &SrRtrAltNextHopInfo,
                                   SR_FALSE) == SR_FAILURE)
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "line %d func %s -----SR TE LFA"
                 " FTN creation failed  for DEST %x nexthop %x  \r\n", __LINE__,
                 __func__, pSrTeRtEntryInfo->destAddr.Addr.u4Addr,
                 SrRtrAltNextHopInfo.nextHop.Addr.u4Addr);
        return SR_FAILURE;

    }
    else
    {
        SR_TRC4 (SR_CTRL_TRC, "line %d func %s -----SR TE LFA"
                 " FTN sucessfully created for DEST %x nexthop %x  \r\n",
                 __LINE__, __func__, pSrTeRtEntryInfo->destAddr.Addr.u4Addr,
                 SrRtrAltNextHopInfo.nextHop.Addr.u4Addr);

    }
    /* After successful programming of TE LFA, reset the TE LFA values
     * and update it as Priamry */

    /* Setting TE FRR Active Flag */
    pSrTeRtEntryInfo->bIsSrTeFRRActive = SR_TE_LFA_ACTIVE;

    /*Create new Next-Hop Node */
    if ((pNewSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrCreateNewNextHopNode returns NULL \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }
    /* Update new next hop info */
    pNewSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
    pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr =
        pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr;
    pNewSrRtrNextHopInfo->u4OutIfIndex = pSrTeRtEntryInfo->u4OutLfaIfIndex;

    pSrTeRtEntryInfo->PrimaryNexthop.u2AddrType = SR_IPV4_ADDR_TYPE;
    pSrTeRtEntryInfo->PrimaryNexthop.Addr.u4Addr =
        pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr;

    pDestSrRtrInfo->teSwapNextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
    pDestSrRtrInfo->teSwapNextHop.Addr.u4Addr =
        pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr;

    pDestSrRtrInfo->u4TeSwapOutIfIndex = pSrTeRtEntryInfo->u4OutLfaIfIndex;

    /* Add NextHop Node in NextHopList in SrRtrNode */
    TMO_SLL_Insert (&(pDestSrRtrInfo->NextHopList), NULL,
                    &(pNewSrRtrNextHopInfo->nextNextHop));

    /* Resetting TE LFA */
    pSrTeRtEntryInfo->u1LfaMPLSStatus &= (UINT1) (~SR_FTN_CREATED);
    pSrTeRtEntryInfo->u4OutLfaIfIndex = SR_ZERO;
    MEMSET (&pSrTeRtEntryInfo->LfaMandRtrId, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&pSrTeRtEntryInfo->LfaNexthop, SR_ZERO, sizeof (tGenU4Addr));

    /* Updating FTN status */
    pSrTeRtEntryInfo->u1MPLSStatus |= SR_FTN_CREATED;
    pNewSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;
    pDestSrRtrInfo->u1TeSwapStatus = TRUE;
    SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrTeConfigEvent
 * Description   : This routine will post an event to SR with TE paramaters
                    to configure SR TE
 *
 * Input(s)      : destAddr,u4DestMask,mandatoryRtrId,au4OptRouterIds,
                   u1NoOfOptRouters
 *
 * Output(s)     : NONE
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/

INT4
SrTeConfigEvent (tGenU4Addr destAddr, UINT4 u4DestMask,
                 tGenU4Addr mandatoryRtrId, tGenU4Addr au4OptRouterIds[],
                 UINT1 u1NoOfOptRouters)
{
    tSrQMsg             SrQMsg;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));

    SrQMsg.u4MsgType = SR_OSPF_TE_ADD_EVENT;

    MEMCPY (&SrQMsg.u.SrTeRtInfo.destAddr, &destAddr, sizeof (tGenU4Addr));
    MEMCPY (&SrQMsg.u.SrTeRtInfo.mandatoryRtrId, &mandatoryRtrId,
            sizeof (tGenU4Addr));
    MEMCPY (&SrQMsg.u.SrTeRtInfo.au4OptRouterIds, au4OptRouterIds,
            sizeof (tGenU4Addr) * u1NoOfOptRouters);

    SrQMsg.u.SrTeRtInfo.u1NoOfOptRouters = u1NoOfOptRouters;
    SrQMsg.u.SrTeRtInfo.u4DestMask = u4DestMask;

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                 "%s:%d Failed to EnQ SR_OSPF_TE_ADD_EVENT Event to SR Task \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrTeDeleteEvent
 * Description   : This routine will post an event to SR with TE
                   paramaters  to delete TE config
 *
 * Input(s)      : destAddr
 *
 * Output(s)     : NONE
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
INT4
SrTeDeleteEvent (tGenU4Addr destAddr)
{
    tSrQMsg             SrQMsg;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));

    SrQMsg.u4MsgType = SR_OSPF_TE_DEL_EVENT;

    MEMCPY (&SrQMsg.u.SrTeRtInfo.destAddr, &destAddr, sizeof (tGenU4Addr));

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                 "%s:%d Failed to EnQ SR_OSPF_TE_DEL_EVENT Event to SR Task \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrTeFRRIlmAddorDelete
 * Description   : This routine will Add/delete SRR FRR ILM for TE destination
 *
 * Input(s)      : pDestSrRtrInfo,pSrTeRtEntryInfo
 *
 * Output(s)     : NONE
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
INT4
SrTeFRRIlmAddorDelete (tGenU4Addr * destAddr,
                       tSrTeRtEntryInfo * pSrTeRtEntryInfo, UINT1 u1cmdType)
{
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrInSegInfo        SrInSegInfo;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tGenU4Addr          nextHopAddr;
    tSrRtrInfo          LfaTeDestSrRtrInfo;
    UINT4               u4SrGbMinIndex = SR_ZERO;
    UINT4               u4IpAddress = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4IntIf = SR_ZERO;

    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&LfaTeDestSrRtrInfo, 0, sizeof (tSrRtrInfo));
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));

    if ((destAddr == NULL) && (pSrTeRtEntryInfo == NULL))
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d DestAddr and pSrTeRtEntryInfo are NULL\n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }
    pRtrInfo = SrGetSrRtrInfoFromRtrId (&pSrTeRtEntryInfo->destAddr);
    if (pRtrInfo == NULL)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL\n", __func__, __LINE__);
        return SR_FAILURE;
    }

    /* Populate SrInSegInfo from Global SelfNodeInfo */
    if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
    {
        u4SrGbMinIndex = gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
    }

    SR_TRC3 (SR_CTRL_TRC, "%s:%d Entry with prefix %x \n", __func__, __LINE__,
             pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
    if (u1cmdType == SR_TRUE)    /* FRR ILM create */
    {
        SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
        SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;
        SrRtrAltNextHopInfo.PrimarynextHop.Addr.u4Addr =
            pRtrInfo->teSwapNextHop.Addr.u4Addr;
        u4IpAddress =
            (pSrTeRtEntryInfo->FrrIlmNexthop.Addr.u4Addr & 0xFFFFFFFF);
        if (CfaIpIfGetIfIndexForNetInCxt
            (SR_ZERO, u4IpAddress,
             &SrRtrAltNextHopInfo.u4SwapOutIfIndex) == CFA_FAILURE)
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Exiting: Not able to get Out IfIndex \n",
                     __func__, __LINE__);
        }
        MEMCPY (&SrRtrAltNextHopInfo.nextHop, &pSrTeRtEntryInfo->FrrIlmNexthop,
                sizeof (tGenU4Addr));
        pRtrInfo->u4LfaNextHop = pSrTeRtEntryInfo->FrrIlmNexthop.Addr.u4Addr;
        pRtrInfo->u1LfaType = SR_LFA_ROUTE;
        /*pSrRtrInfo->u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
#ifdef CFA_WANTED
        if (CfaIfmCreateStackMplsTunnelInterface
            (SrRtrAltNextHopInfo.u4SwapOutIfIndex,
             &u4MplsTnlIfIndex) == CFA_SUCCESS)
        {
            SR_TRC3 (SR_CTRL_TRC,
                     "%s:%d MPLS tunnel interface creation successful for interface %d: : CFA_SUCCESS\n",
                     __func__, __LINE__, SrRtrAltNextHopInfo.u4SwapOutIfIndex);

        }
        else
        {
            pRtrInfo->u4LfaNextHop = SR_FALSE;
            pRtrInfo->u1LfaType = SR_ZERO;
            pRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
            pSrTeRtEntryInfo->u4FRRIlmIfIndex = SR_ZERO;
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d MPLS tunnel interface creation failed for interface %d: : CFA_FAILURE\n",
                     __func__, __LINE__, SrRtrAltNextHopInfo.u4OutIfIndex);
            return SR_FAILURE;
        }
#endif
        SrRtrAltNextHopInfo.u4SwapOutIfIndex = u4MplsTnlIfIndex;
        pRtrInfo->u4LfaSwapOutIfIndex = u4MplsTnlIfIndex;
        pSrTeRtEntryInfo->u4FRRIlmIfIndex = u4MplsTnlIfIndex;
        pRtrInfo->u1TeSwapStatus = SR_TRUE;
        if (SrMplsCreateILM (pRtrInfo, &SrRtrAltNextHopInfo) == SR_SUCCESS)
        {
            SR_TRC4 (SR_CTRL_TRC,
                     "%s:%d Creation of LFA ILM successful for prefix %x with nexthop %x \n ",
                     __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                     SrRtrAltNextHopInfo.nextHop.Addr.u4Addr);
        }
        else
        {
            pRtrInfo->u4LfaNextHop = SR_FALSE;
            pRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
            pSrTeRtEntryInfo->u4FRRIlmIfIndex = SR_ZERO;
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d Creation of LFA ILM failed for prefix %x with nexthop %x \n ",
                     __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                     SrRtrAltNextHopInfo.nextHop.Addr.u4Addr);

            return SR_FAILURE;
        }

    }
    else                        /* FRR ILM Delete */
    {

        SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;
        SrRtrAltNextHopInfo.u4SwapOutIfIndex =
            pSrTeRtEntryInfo->u4FRRIlmIfIndex;
        SrRtrAltNextHopInfo.nextHop.Addr.u4Addr =
            pSrTeRtEntryInfo->FrrIlmNexthop.Addr.u4Addr;
        SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
        SrRtrAltNextHopInfo.PrimarynextHop.Addr.u4Addr =
            pRtrInfo->teSwapNextHop.Addr.u4Addr;
        SrInSegInfo.u4InLabel = (u4SrGbMinIndex + pRtrInfo->u4SidValue);
        pRtrInfo->u1TeSwapStatus = SR_TRUE;
        SR_TRC4 (SR_CTRL_TRC,
                 "%s:%d Deleting FRR ILM with NextHop : %x TEswapStatus : %d \n",
                 __func__, __LINE__, SrRtrAltNextHopInfo.nextHop.Addr.u4Addr,
                 pRtrInfo->u1TeSwapStatus);
        if (SrMplsDeleteILM (&SrInSegInfo, pRtrInfo, &SrRtrAltNextHopInfo) ==
            SR_FAILURE)
        {
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d Unable to delete ILM SWAP for prefix %x with nexthop %x \n",
                     __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                     SrRtrAltNextHopInfo.nextHop.Addr.u4Addr);
            return SR_FAILURE;
        }
        SR_TRC4 (SR_CTRL_TRC,
                 "%s:%d FRR ILM deletion is successful for prefix %x with nexthop %x \n",
                 __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                 SrRtrAltNextHopInfo.nextHop.Addr.u4Addr);

#ifdef CFA_WANTED
        if (CfaUtilGetIfIndexFromMplsTnlIf
            (SrRtrAltNextHopInfo.u4SwapOutIfIndex, &u4IntIf,
             TRUE) != CFA_FAILURE)
        {
            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4IntIf, SrRtrAltNextHopInfo.u4SwapOutIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed\n",
                         __func__, __LINE__,
                         SrRtrAltNextHopInfo.u4SwapOutIfIndex, u4IntIf);
                return SR_FAILURE;
            }
        }
#endif
        pSrTeRtEntryInfo->u4FRRIlmIfIndex = SR_ZERO;
        pSrTeRtEntryInfo->FrrIlmNexthop.Addr.u4Addr = SR_ZERO;
        pRtrInfo->u4LfaNextHop = SR_ZERO;
        pRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
        pRtrInfo->u4LfaOutIfIndex = SR_ZERO;
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrTeLfaRtQuery
 * Description   : This routine will query lfa nexthop for TE mandatory node
                    and for TE destination from RTM
 *
 * Input(s)      : pDestAddr,pNetIpLfaRtInfo
 *
 * Output(s)     : NONE
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
INT4
SrTeLfaRtQuery (tGenU4Addr * pDestAddr, tNetIpv4LfaRtInfo * pNetIpLfaRtInfo)
{
    tLfaRtInfoQueryMsg  LfaRtQuery;
    tGenU4Addr          nextHopAddr;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4Port = SR_ZERO;
    /* If SR LFA is enabled, need to update LFA nexthop of a destination for Sr TE LFA route */
    if (SrGetNHForPrefix (pDestAddr, &nextHopAddr, &u4OutIfIndex) == SR_FAILURE)
    {
        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                 "%s:%d Nexthop not exists for this mandatory node %x \n",
                 __func__, __LINE__, pDestAddr->Addr.u4Addr);
        return SR_FAILURE;

    }
    /* LFA route fetch for mandatory node */
    LfaRtQuery.u4DestinationIpAddress = pDestAddr->Addr.u4Addr;
    LfaRtQuery.u4DestinationSubnetMask = 0xFFFFFFFF;
    LfaRtQuery.u4PrimaryNextHop = nextHopAddr.Addr.u4Addr;
    LfaRtQuery.u2RtProto = OSPF_ID;
    LfaRtQuery.u4ContextId = 0;

    if (NetIpv4GetPortFromIfIndex (u4OutIfIndex, &u4Port) == IP_FAILURE)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                 "%s:%d Exiting: NetIpv4GetPortFromIfIndex failed\n", __func__,
                 __LINE__);
        return SR_FAILURE;
    }

    LfaRtQuery.u4NextHopIfIndx = u4Port;
    if (NetIpv4LfaGetRoute (&LfaRtQuery, pNetIpLfaRtInfo) != NETIPV4_SUCCESS)
    {
        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                 "%s:%d Exiting: Not able to fetch the lfa route for the destination %x\n",
                 __func__, __LINE__, LfaRtQuery.u4DestinationIpAddress);
        return SR_FAILURE;
    }
    return SR_SUCCESS;

}

/************************************************************************
**  Function Name   : SrDelFTNWithFRRForTE 
**  Description     : This function deletes the FTN and ILM created for
**                    SR Primary LSP, LFA/RLFA LSP, before adding TE LSP
**  Input           : Destination IP
**  Output          : None
**  Returns         : SR_SUCCESS/SR_FAILURE
*************************************************************************/
INT4
SrDelFTNWithFRRForTE (tGenU4Addr * pDestAddr, tSrRtrInfo * pRtrInfo)
{
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrTeRtEntryInfo   *pSrTeDestRtInfo = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tTMO_SLL_NODE      *pTempSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tGenU4Addr          PrimaryNextHop;
    tGenU4Addr          NextHop;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;

    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&NextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&PrimaryNextHop, SR_ZERO, sizeof (tGenU4Addr));

    SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;

    pRtrInfo->bIsTeconfigured = SR_TRUE;
    pSrTeDestRtInfo = SrGetTeRouteEntryFromDestAddr (pDestAddr);
    if (pSrTeDestRtInfo == NULL)
    {
        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d This prefix %x not exists in TE Rbtree \n",
                 __func__, __LINE__, pDestAddr->Addr.u4Addr);
        return SR_FAILURE;
    }
    if (pRtrInfo->u4LfaNextHop != SR_ZERO)
    {
        TMO_DYN_SLL_Scan (&(pRtrInfo->NextHopList),
                          pSrRtrNextHopNode, pTempSrRtrNextHopNode,
                          tTMO_SLL_NODE *)
        {
            pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
            /* if(pSrRtrNextHopInfo == NULL)
               {
               break;
               } */
            MEMCPY (&(PrimaryNextHop), &(pSrRtrNextHopInfo->nextHop),
                    sizeof (tGenU4Addr));
        }
        SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;

        SrRtrAltNextHopInfo.nextHop.Addr.u4Addr = pRtrInfo->u4LfaNextHop;
        SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
        SrRtrAltNextHopInfo.u4OutIfIndex = pRtrInfo->u4LfaOutIfIndex;

        MEMCPY (&(SrRtrAltNextHopInfo.PrimarynextHop), &(PrimaryNextHop),
                sizeof (tGenU4Addr));
        if (SrMplsCreateOrDeleteLsp
            (MPLS_MLIB_FTN_DELETE, pRtrInfo, NULL,
             &SrRtrAltNextHopInfo) != SR_SUCCESS)
        {
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     " line %d func %s LFA FTN deletion failed for DESTIP %x nh %x \r\n",
                     __LINE__, __func__, pRtrInfo->prefixId.Addr.u4Addr,
                     SrRtrAltNextHopInfo.nextHop.Addr.u4Addr);

            return SR_FAILURE;
        }
#ifdef CFA_WANTED
        u4MplsTnlIfIndex = SrRtrAltNextHopInfo.u4OutIfIndex;

        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {
            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                         __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }
        }

        SrRtrAltNextHopInfo.u4OutIfIndex = SR_ZERO;
#endif
    }                            /* LFA FTN and ILM  delete ends */
    else
    {
        /* If a RLFA path exists, it has to be deleted before deleting the primary LSP */
        pSrRlfaPathInfo = SrGetRlfaPathEntryFromDestAddr (pDestAddr);
        if (pSrRlfaPathInfo != NULL)
        {
            pRtrInfo->u4LfaSwapOutIfIndex = pSrRlfaPathInfo->u4RlfaIlmTunIndex;;
            NextHop.Addr.u4Addr = pSrRlfaPathInfo->rlfaNextHop.Addr.u4Addr;
            NextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
            if (SrRlfaDeleteFtnEntry (pDestAddr, &NextHop) != SR_SUCCESS)
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d Deletion of SR RLFA FTN/ILM Failure for %x\n",
                         __func__, __LINE__, pDestAddr->Addr.u4Addr);
            }

        }
    }                            /* RLFA Path deletion ends */

    TMO_DYN_SLL_Scan (&(pRtrInfo->NextHopList),
                      pSrRtrNextHopNode, pTempSrRtrNextHopNode, tTMO_SLL_NODE *)
    {
        pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
#if 0
        if (pSrRtrNextHopInfo == NULL)
        {
            break;
        }
#endif
        if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
            SR_FTN_CREATED)
        {

            if (SrMplsDeleteFTN (pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
            {
                SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         " line %d func %s Primary FTN deletion failed for DESTIP %x nh %x \r\n",
                         __LINE__, __func__, pRtrInfo->prefixId.Addr.u4Addr,
                         pSrRtrNextHopInfo->nextHop.Addr.u4Addr);

                return SR_FAILURE;
            }
            pRtrInfo->u4TeSwapOutIfIndex = pSrRtrNextHopInfo->u4SwapOutIfIndex;
            MEMCPY (&(pRtrInfo->teSwapNextHop), &(pSrRtrNextHopInfo->nextHop),
                    sizeof (tGenU4Addr));
            pRtrInfo->u1TeSwapStatus = TRUE;

            /* Remove SrRtrNhNode from NextHopList */
            TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                            (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
            SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);

        }
    }
    return SR_SUCCESS;
}

/************************************************************************
 * Function Name   : SrTeFRRFtnCreateOrDel
 * Description     : Function to Create TE LFA Ftn for the configured 
                     TE config
 * Input           : u4DestAddr
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesfully creation)/SR_FAILURE
 *************************************************************************/
INT4
SrTeFRRFtnCreateOrDel (tSrTeRtEntryInfo * pSrTeRtEntryInfo, UINT1 u1cmdType)
{
    tSrRtrInfo         *pTempSrRtrInfo = NULL;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrTeRtEntryInfo    getSrTeRtEntryInfo;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrRtrInfo          LfaTeDestSrRtrInfo;
    tSrRtrNextHopInfo   NewSrRtrNextHopInfo;
    tSrTeLblStack       SrTeLblStack;
    tGenU4Addr          nextHopAddr;
    tGenU4Addr          dummyAddr;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4TopLabel = SR_ZERO;
    UINT4               u4DestLabel = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4IpAddress = SR_ZERO;
    UINT4               u4OptLabelCount = SR_ZERO;
    UINT2               u2MlibOperation = MPLS_MLIB_FTN_CREATE;

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&getSrTeRtEntryInfo, 0, sizeof (tSrTeRtEntryInfo));
    MEMSET (&NewSrRtrNextHopInfo, 0, sizeof (tSrRtrNextHopInfo));
    MEMSET (&LfaTeDestSrRtrInfo, 0, sizeof (tSrRtrInfo));

    if (u1cmdType == SR_TRUE)    /* SR TE LFA FTN creation flow */
    {
        if (pSrTeRtEntryInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            if ((pSrTeRtEntryInfo->u1LfaMPLSStatus & SR_FTN_CREATED) ==
                SR_FTN_CREATED)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d  Lfa FTN already exists for this prefix%x \n",
                         __func__, __LINE__,
                         pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                return SR_FAILURE;
            }
            /*To fetch router-info from the LFA nh */
            pTempSrRtrInfo =
                SrGetRtrInfoFromNextHop (&pSrTeRtEntryInfo->LfaNexthop);
            if (pTempSrRtrInfo == NULL)
            {
                SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d SR router info fetch failed for D %x  Lfa Nexthop %x \n",
                         __func__, __LINE__,
                         pSrTeRtEntryInfo->destAddr.Addr.u4Addr,
                         pSrTeRtEntryInfo->LfaNexthop);

                return SR_FAILURE;
            }
            pSrTeRtEntryInfo->LfaMandRtrId.Addr.u4Addr =
                pTempSrRtrInfo->prefixId.Addr.u4Addr;
            pSrTeRtEntryInfo->LfaMandRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;
            if (SrGetLabelForLFATEMandNode
                (&pSrTeRtEntryInfo->LfaMandRtrId, &pSrTeRtEntryInfo->mandRtrId,
                 &u4TopLabel) == SR_FAILURE)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d Unable to get TE LFA mandatory node %x label\n",
                         __func__, __LINE__,
                         pSrTeRtEntryInfo->LfaMandRtrId.Addr.u4Addr);
                return SR_FAILURE;
            }
            if (SrTeGetLabelForRtrId (&pSrTeRtEntryInfo->destAddr,
                                      &pSrTeRtEntryInfo->mandRtrId,
                                      &u4DestLabel) == SR_FAILURE)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d Unable to get u4DestLabel for prefix %x \n",
                         __func__, __LINE__,
                         pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                return SR_FAILURE;
            }
            /*Get Best Next-Hop & u4OutIfIndex for MandRtrId */
            u4IpAddress =
                (pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr & 0xFFFFFFFF);
            if (CfaIpIfGetIfIndexForNetInCxt
                (SR_ZERO, u4IpAddress, &u4OutIfIndex) == CFA_FAILURE)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d Exiting: Not able to get Out IfIndex \n",
                         __func__, __LINE__);
            }

#ifdef CFA_WANTED
            /*u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
            if (CfaIfmCreateStackMplsTunnelInterface (u4OutIfIndex,
                                                      &u4MplsTnlIfIndex)
                == CFA_SUCCESS)
            {
                SR_TRC4 (SR_CTRL_TRC,
                         "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                         __func__, __LINE__, u4OutIfIndex, u4MplsTnlIfIndex);

                u4OutIfIndex = u4MplsTnlIfIndex;
                pSrTeRtEntryInfo->u4OutLfaIfIndex = u4OutIfIndex;
            }
            else
            {
                SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                         __func__, __LINE__, u4OutIfIndex, u4MplsTnlIfIndex);
                return SR_FAILURE;
            }
#endif
            /* Nexthop info update */
            NewSrRtrNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
            NewSrRtrNextHopInfo.nextHop.Addr.u4Addr =
                pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr;
            NewSrRtrNextHopInfo.u4OutIfIndex = u4OutIfIndex;

            /* Set the TopLabel and the following Label Stack
             * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
            SrTeLblStack.u4TopLabel = u4TopLabel;
            SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;

            SrTeLblStack.u1StackSize = (UINT1) (u4OptLabelCount + SR_ONE);

            /*Get SrRtrInfo for DestAddr for which TE path is to be created */
            if ((pDestSrRtrInfo =
                 SrGetSrRtrInfoFromRtrId (&pSrTeRtEntryInfo->destAddr)) == NULL)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for prefix %x \n",
                         __func__, __LINE__,
                         pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                return SR_FAILURE;
            }
            /* SR Route infor is copied to a Local variable to process SR TE LFA Lsp */
            MEMCPY ((&LfaTeDestSrRtrInfo), pDestSrRtrInfo, sizeof (tSrRtrInfo));

            /* Create FTN */
            if (SrMplsCreateOrDeleteStackLsp
                (u2MlibOperation, &LfaTeDestSrRtrInfo, NULL, &SrTeLblStack,
                 &NewSrRtrNextHopInfo, SR_TRUE) == SR_FAILURE)
            {
                SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "line %d func %s -----SR TE LFA FTN creation failed  for DEST %x nexthop %x  \r\n",
                         __LINE__, __func__,
                         pSrTeRtEntryInfo->destAddr.Addr.u4Addr,
                         NewSrRtrNextHopInfo.nextHop.Addr.u4Addr);
                return SR_FAILURE;

            }
            else
            {
                SR_TRC4 (SR_CTRL_TRC,
                         "line %d func %s -----SR TE LFA FTN sucessfully created for DEST %x nexthop %x  \r\n",
                         __LINE__, __func__,
                         pSrTeRtEntryInfo->destAddr.Addr.u4Addr,
                         NewSrRtrNextHopInfo.nextHop.Addr.u4Addr);

            }

            pSrTeRtEntryInfo->u1LfaMPLSStatus = SR_FTN_CREATED;

        }
    }
    else                        /* SR TE LFA FTN deletion flow */
    {

        if (pSrTeRtEntryInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            if ((pSrTeRtEntryInfo->u1LfaMPLSStatus & SR_FTN_CREATED) ==
                SR_FTN_CREATED)
            {
                if (SrGetLabelForLFATEMandNode
                    (&pSrTeRtEntryInfo->LfaMandRtrId,
                     &pSrTeRtEntryInfo->mandRtrId, &u4TopLabel) == SR_FAILURE)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Unable to get TE LFA mandatory node label\n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
                if (SrTeGetLabelForRtrId (&pSrTeRtEntryInfo->destAddr,
                                          &pSrTeRtEntryInfo->mandRtrId,
                                          &u4DestLabel) == SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Unable to get u4DestLabel for prefix %x \n",
                             __func__, __LINE__,
                             pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                    return SR_FAILURE;
                }
                /*Get Best Next-Hop & u4OutIfIndex for MandRtrId */
                u4IpAddress =
                    (pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr & 0xFFFFFFFF);
                if (CfaIpIfGetIfIndexForNetInCxt
                    (SR_ZERO, u4IpAddress, &u4OutIfIndex) == CFA_FAILURE)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Exiting: Not able to get Out IfIndex \n",
                             __func__, __LINE__);
                }

                /* Nexthop info update */
                NewSrRtrNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
                NewSrRtrNextHopInfo.nextHop.Addr.u4Addr =
                    pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr;
                NewSrRtrNextHopInfo.u4OutIfIndex =
                    pSrTeRtEntryInfo->u4OutLfaIfIndex;

                /* Set the TopLabel and the following Label Stack
                 * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
                SrTeLblStack.u4TopLabel = u4TopLabel;
                SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;

                SrTeLblStack.u1StackSize = (UINT1) (u4OptLabelCount + SR_ONE);

                /*Get SrRtrInfo for DestAddr for which TE path is to be created */
                if ((pDestSrRtrInfo =
                     SrGetSrRtrInfoFromRtrId (&pSrTeRtEntryInfo->destAddr)) ==
                    NULL)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for prefix %x \n",
                             __func__, __LINE__,
                             pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                    return SR_FAILURE;
                }
                /* SR Route infor is copied to a Local variable to process SR TE LFA Lsp */
                MEMCPY ((&LfaTeDestSrRtrInfo), pDestSrRtrInfo,
                        sizeof (tSrRtrInfo));

                /* Delete FTN */
                if (SrMplsCreateOrDeleteStackLsp
                    (MPLS_MLIB_FTN_DELETE, &LfaTeDestSrRtrInfo, NULL, &SrTeLblStack,
                     &NewSrRtrNextHopInfo, SR_TRUE) == SR_FAILURE)
                {
                    SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "line %d func %s -----SR TE LFA FTN deletion failed  for DEST %x nexthop %x  \r\n",
                             __LINE__, __func__,
                             pSrTeRtEntryInfo->destAddr.Addr.u4Addr,
                             NewSrRtrNextHopInfo.nextHop.Addr.u4Addr);
                    return SR_FAILURE;

                }
                else
                {
                    SR_TRC4 (SR_CTRL_TRC,
                             "line %d func %s -----SR TE LFA FTN sucessfully deleted for DEST %x nexthop %x  \r\n",
                             __LINE__, __func__,
                             pSrTeRtEntryInfo->destAddr.Addr.u4Addr,
                             NewSrRtrNextHopInfo.nextHop.Addr.u4Addr);

                }

            }
            pSrTeRtEntryInfo->u1LfaMPLSStatus &= (UINT1) (~SR_FTN_CREATED);
            pSrTeRtEntryInfo->u4OutLfaIfIndex = SR_ZERO;
            MEMSET (&pSrTeRtEntryInfo->LfaMandRtrId, SR_ZERO,
                    sizeof (tGenU4Addr));
            MEMSET (&pSrTeRtEntryInfo->LfaNexthop, SR_ZERO,
                    sizeof (tGenU4Addr));

        }
    }
    return SR_SUCCESS;
}

/**************************************************************************
 * Function Name   : SrTeFtnCreateOrDel
 * Description     : Function is to create or delete SR TE FTN 
                     with destination prefix as reference.
 * Input           : pSrTeDestRtInfo, u1cmdType
                     if u1cmdType is SR_TRUE, then SR TE creation 
                     else, SR TE deletion flow
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesful deletion/creation)
 *                   SR_FAILURE (No TEPATH Entry Found or any other Failure
 *                   scenario)
 *************************************************************************/
INT4
SrTeFtnCreateOrDel (tSrTeRtEntryInfo * pSrTeDestRtInfo, UINT1 u1cmdType)
{
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrTeLblStack       SrTeLblStack;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;
    tSrInSegInfo        SrInSegInfo;
    tGenU4Addr          nextHopAddr;
    tGenU4Addr          dummyAddr;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4TopLabel = SR_ZERO;
    UINT4               u4DestLabel = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    UINT4               u4Counter = SR_ZERO;
    UINT4               u4OptLabelCount = SR_ZERO;
    UINT4               u4TmpOptLabelCounter = SR_ZERO;
    UINT4               au4SrOptRtrLabel[SR_MAX_TE_OPTIONAL_RTR] = { SR_ZERO };

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    if (pSrTeDestRtInfo == NULL)
    {

        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                 "%s : %d Failure as TERtInfo Null with CmdType %d \n",
                 __func__, __LINE__, u1cmdType);
        return SR_FAILURE;
    }

    SR_TRC3 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d Entry with prefix %x \n", __func__, __LINE__,
             pSrTeDestRtInfo->destAddr.Addr.u4Addr);
    if (u1cmdType == SR_TRUE)    /* SR TE FTN and ILM creation */
    {
        /*Get SrRtrInfo for DestAddr for which TE path is to be created */
        if ((pDestSrRtrInfo =
             SrGetSrRtrInfoFromRtrId (&pSrTeDestRtInfo->destAddr)) == NULL)
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrRtInfo not exits for TE \n", __func__,
                     __LINE__);
            return SR_FAILURE;
        }
        else
        {
            pDestSrRtrInfo->bIsTeconfigured = SR_TRUE;
        }

        if (pSrTeDestRtInfo->u1RowStatus != MPLS_STATUS_ACTIVE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d TePathEntry not ACTIVE for prefix %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }

        if ((pSrTeDestRtInfo->u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
        {
            SR_TRC3 (SR_CTRL_TRC,
                     "%s:%d FTN already created for TE-Path with Dest : %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_SUCCESS;
        }

        /* 2. If SR-TE Found, then check if Labels are available for reaching
         * [MandatoryRtrId (TopLabel)] & [DestAddr (NextLabel)] */

        if (SrTeGetLabelForRtrId (&pSrTeDestRtInfo->mandRtrId,
                                  &dummyAddr, &u4TopLabel) == SR_FAILURE)
        {

            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Unable to get TopLabel for prefix %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }

        if (SrTeGetLabelForRtrId (&pSrTeDestRtInfo->destAddr,
                                  &pSrTeDestRtInfo->mandRtrId,
                                  &u4DestLabel) == SR_FAILURE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d Unable to get DestLabel for prefix %x \n", __func__,
                     __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }

        pSrOptRtrInfo = NULL;
        pSrPeerRtrInfo = NULL;
        u4Counter = SR_ZERO;
        u4OptLabelCount = SR_ZERO;

        if (pSrTeDestRtInfo != NULL)
        {
            TMO_SLL_Scan (&(pSrTeDestRtInfo->srTeRtrListIndex.RtrList),
                          pSrOptRtrInfo, tTMO_SLL_NODE *)
            {
                pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                if (SrTeGetLabelForRtrId (&pSrPeerRtrInfo->routerId,
                                          &dummyAddr,
                                          &au4SrOptRtrLabel[u4Counter]) ==
                    SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Unable to get label for optional Router %x \n",
                             __func__, __LINE__,
                             pSrTeDestRtInfo->destAddr.Addr.u4Addr);
                    return SR_FAILURE;
                }
                u4Counter++;
                u4OptLabelCount++;
            }
        }

        /*Get Best Next-Hop & u4OutIfIndex for MandRtrId */
        if (SrGetNHForPrefix
            (&pSrTeDestRtInfo->mandRtrId, &nextHopAddr,
             &u4OutIfIndex) == SR_FAILURE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrGetNHForPrefix FAILURE for %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }
#ifdef CFA_WANTED
        /*u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
        if (CfaIfmCreateStackMplsTunnelInterface (u4OutIfIndex,
                                                  &u4MplsTnlIfIndex)
            == CFA_SUCCESS)
        {
            SR_TRC4 (SR_CTRL_TRC,
                     "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                     __func__, __LINE__, u4OutIfIndex, u4MplsTnlIfIndex);

            u4OutIfIndex = u4MplsTnlIfIndex;
        }
        else
        {
            SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                     __func__, __LINE__, u4OutIfIndex, u4MplsTnlIfIndex);
            return SR_FAILURE;
        }
#endif
        /* Set the TopLabel and the following Label Stack
         * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
        SrTeLblStack.u4TopLabel = u4TopLabel;
        SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;

        u4TmpOptLabelCounter = u4OptLabelCount - 1;
        for (u4Counter = SR_ONE; u4Counter <= u4OptLabelCount;
             u4Counter++, u4TmpOptLabelCounter--)
        {
            SrTeLblStack.u4LabelStack[u4Counter] =
                au4SrOptRtrLabel[u4TmpOptLabelCounter];
        }
        SrTeLblStack.u1StackSize = (UINT1) (u4OptLabelCount + SR_ONE);

        MEMCPY (&(pSrTeDestRtInfo->SrPriTeLblStack), &SrTeLblStack,
                sizeof (tSrTeLblStack));
        if (pSrTeDestRtInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            pDestSrRtrInfo->teSwapNextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        }
        else
        {
            pDestSrRtrInfo->teSwapNextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
        }

        /*Create new Next-Hop Node */
        if ((pSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrCreateNewNextHopNode returns NULL \n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
        /* Update OutIf & NextHop in SrRtrInfo for DestAddr */
        pSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
        pSrRtrNextHopInfo->nextHop.Addr.u4Addr = nextHopAddr.Addr.u4Addr;
        pSrTeDestRtInfo->PrimaryNexthop.Addr.u4Addr = nextHopAddr.Addr.u4Addr;
        pSrTeDestRtInfo->PrimaryNexthop.u2AddrType = SR_IPV4_ADDR_TYPE;
        pSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;

        /* Add NextHop Node in NextHopList in SrRtrNode */
        TMO_SLL_Insert (&(pDestSrRtrInfo->NextHopList), NULL,
                        &(pSrRtrNextHopInfo->nextNextHop));

        /* Create FTN */
        if (SrMplsCreateOrDeleteStackLsp
            (MPLS_MLIB_FTN_CREATE, pDestSrRtrInfo, NULL, &SrTeLblStack,
             pSrRtrNextHopInfo, SR_FALSE) == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d SrMplsCreateOrDeleteStackLsp FAILURE for destination %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);

            /* Delete all the resources reserved on FAILURE & reset OutIfIndex */
#ifdef CFA_WANTED
            if (CfaUtilGetIfIndexFromMplsTnlIf
                (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
            {
                if (CfaIfmDeleteStackMplsTunnelInterface
                    (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                {
                    SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                             __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                    TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                                    (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                    SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);

                    return SR_FAILURE;
                }
            }
#endif
            TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                            (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
            SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
            return SR_FAILURE;
        }
        SR_TRC3 (SR_CTRL_TRC,
                 "%s:%d SR TE FTN creation is successful for destination %x \n",
                 __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);

        pSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;

    }
    else                        /* SR TE FTN deletion flow */
    {
        /* Case 1.1:  SR TE LSP deletion via destination prefix */
        if ((pSrTeDestRtInfo->u1MPLSStatus & SR_FTN_CREATED) ==
            SR_FTN_NOT_CREATED)
        {
            SR_TRC3 (SR_CTRL_TRC,
                     "%s:%d Sr TE LSPs were not created for %x \n", __func__,
                     __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_SUCCESS;
        }
        /* Get the SrRtrId */
        if ((pDestSrRtrInfo =
             SrGetSrRtrInfoFromRtrId (&pSrTeDestRtInfo->destAddr)) == NULL)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }

        /* There will be only one SrRtrNhNode for SR-TE */
        pSrRtrNextHopInfo =
            (tSrRtrNextHopInfo *)
            TMO_SLL_First (&(pDestSrRtrInfo->NextHopList));
        if (pSrRtrNextHopInfo == NULL)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s: %d Cannot find the next hop for %x \r\n", __func__,
                     __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }

        /* Delete FTN */
        if ((pSrTeDestRtInfo->u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
        {
            if (SrMplsCreateOrDeleteStackLsp
                (MPLS_MLIB_FTN_DELETE, pDestSrRtrInfo, NULL, &SrTeLblStack,
                 pSrRtrNextHopInfo, SR_FALSE) == SR_FAILURE)
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d SrMplsCreateOrDeleteStackLsp FAILURE for %x \n",
                         __func__, __LINE__,
                         pSrTeDestRtInfo->destAddr.Addr.u4Addr);
                return SR_FAILURE;
            }
        }

#ifdef CFA_WANTED
        u4MplsTnlIfIndex = pSrRtrNextHopInfo->u4OutIfIndex;

        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {
            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                         __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }
        }

        pSrRtrNextHopInfo->u4OutIfIndex = SR_ZERO;

#endif

        TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                        (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
        SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
        /* Setting FTN Status to SR_FTN_NOT_CREATED by
         *          * unsetting the FTN_CREATED Bit*/
        pSrTeDestRtInfo->u1MPLSStatus &= (UINT1) (~SR_FTN_CREATED);

        SR_TRC3 (SR_CTRL_TRC,
                 "%s:%d SR TE FTN deletion is successfull for prefix %x \n",
                 __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);

    }
    return SR_SUCCESS;
}

/**************************************************************************
 * Function Name   : SrFtnCreateorDelete
 * Description     : Function is to create or delete SR  FTN
                     with destination prefix as reference.
 * Input           : pRtrInfo, u1cmdType
                     if u1cmdType is SR_TRUE, then SR creation
                     else, SR deletion flow
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesfully deletion)
 *                   SR_FAILURE (No TEPATH Entry Found or any other Failure
 *                   scenario)
 *************************************************************************/

UINT4
SrFtnCreateorDelete (tSrRtrInfo * pRtrInfo,
                     tSrRtrNextHopInfo * pSrRtrNextHopInfo, UINT1 u1cmdType)
{
    tRtInfo             InRtInfo;
    tRtInfo            *pRt = NULL;
    tRtInfo            *pTmpRt = NULL;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pTempSrRtrInfo = NULL;
    UINT4               u2IncarnId = SR_ZERO;
    UINT4               u4NH = SR_ZERO;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4DestAddr = SR_ZERO;
    UINT4               u4DestMask = SR_IPV4_DEST_MASK;
    tGenU4Addr          destAddr;
    tGenU4Addr          tmpAddr;

    tNetIpv6RtInfo      NetIpv6RtInfo;

    MEMSET (&NetIpv6RtInfo, SR_ZERO, sizeof (tNetIpv6RtInfo));
    MEMSET (&InRtInfo, SR_ZERO, sizeof (tRtInfo));
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&tmpAddr, SR_ZERO, sizeof (tGenU4Addr));

    if (u1cmdType == SR_TRUE)
    {
        if (pSrRtrNextHopInfo == NULL)
        {
            if ((pRtrInfo != NULL)
                && (pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE))
            {
                SR_TRC3 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC | SR_CTRL_TRC, "%s:%d Entry : %x \n", __func__, __LINE__,
                         u4DestAddr);

                MEMCPY (&(destAddr.Addr.u4Addr), &pRtrInfo->prefixId.Addr.u4Addr,
                        MAX_IPV4_ADDR_LEN);

                InRtInfo.u4DestNet = destAddr.Addr.u4Addr;
                InRtInfo.u4DestMask = u4DestMask;

                RtmApiGetBestRouteEntryInCxt (u2IncarnId, InRtInfo, &pRt);

                pTmpRt = pRt;

                if (pTmpRt != NULL)
                {
                    SR_TRC4 (SR_CTRL_TRC,
                             "%s:%d ROUTE Found: NH %x u4OutIfIndex %d \n",
                             __func__, __LINE__, pTmpRt->u4NextHop,
                             pTmpRt->u4RtIfIndx);
                }

                for (;
                     ((pTmpRt != NULL)
                      && (pTmpRt->i4Metric1 == pRt->i4Metric1));
                     pTmpRt = pTmpRt->pNextAlternatepath)
                {
                    u4NH = pTmpRt->u4NextHop;
                    /*Create new Next-Hop Node */
                    pNewSrRtrNextHopInfo = NULL;
                    if ((pNewSrRtrNextHopInfo =
                         SrCreateNewNextHopNode ()) == NULL)
                    {
                        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d pNewSrRtrNextHopInfo == NULL \n", __func__,
                                 __LINE__);
                        return SR_FAILURE;
                    }

                    if (u4NH == SR_ZERO)
                    {
                        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d NO ROUTE Found (NH=0) FEC %x \n", __func__,
                                 __LINE__, u4DestAddr);
                        SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                        return SR_FAILURE;
                    }

                    if (NetIpv4GetCfaIfIndexFromPort (pTmpRt->u4RtIfIndx,
                                                      &u4OutIfIndex) ==
                        NETIPV4_FAILURE)
                    {
                        SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d No Port is mapped with u4RtIfIndx %d \n",
                                 __func__, __LINE__, pTmpRt->u4RtIfIndx);
                        SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                        return SR_FAILURE;
                    }

                    pNewSrRtrNextHopInfo->nextHop.u2AddrType =
                        SR_IPV4_ADDR_TYPE;
                    MEMCPY (&pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr, &(u4NH),
                            MAX_IPV4_ADDR_LEN);
                    pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;

                    SR_TRC4 (SR_CTRL_TRC,
                             "%s:%d ROUTE Found with NH %x u4OutIfIndex %d \n",
                             __func__, __LINE__, u4NH, u4OutIfIndex);

                    /* Add NextHop Node in NextHopList in SrRtrNode */
                    TMO_SLL_Insert (&(pRtrInfo->NextHopList), NULL,
                                    &(pNewSrRtrNextHopInfo->nextNextHop));

                    if (SrMplsCreateFTN (pRtrInfo, pNewSrRtrNextHopInfo) ==
                        SR_FAILURE)
                    {
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateFTN  FAILURE \n",
                                 __func__, __LINE__);

                        /* FTN & POP&FWD cannot co-exist (Preventive check as sometime MLIBUPDATE API failing "to do") */
                        if ((pNewSrRtrNextHopInfo->
                             u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
                        {
                            SR_TRC4 (SR_CTRL_TRC,
                                     "%s:%d FTN already created for %x \n",
                                     __func__, __LINE__, u4DestAddr, u4NH);
                            continue;
                        }

                        /* If Next-Hop is not SR Enabled Create ILM-Pop&Fwd and return */
                        tmpAddr.Addr.u4Addr = u4NH;
                        tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                        pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&tmpAddr);

                        if (pTempSrRtrInfo == NULL)
                        {
                            pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                            SR_TRC2 (SR_CTRL_TRC,
                                     "%s:%d pTempSrRtrInfo == NULL: ~ Next-Hop is not SR Enabled (Create: Pop&Fwd) ~ \n",
                                     __func__, __LINE__);
                            if (SrMplsCreateILMPopAndFwd
                                (pRtrInfo, pNewSrRtrNextHopInfo) == SR_SUCCESS)
                            {
                                SR_TRC2 (SR_CTRL_TRC,
                                         "%s:%d SrMplsCreateILMPop == SUCCESS \n",
                                         __func__, __LINE__);
                            }
                            else
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d POP&FWD Create Failure \n",
                                         __func__, __LINE__);
                                TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                                (tTMO_SLL_NODE *)
                                                pNewSrRtrNextHopInfo);
                                SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                            }
                        }
                        else
                        {
                            /* Delete the node from the list in case of FTN Creation failure */
                            TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                            (tTMO_SLL_NODE *)
                                            pNewSrRtrNextHopInfo);
                            SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                        }
                        continue;
                    }
                    u4OutIfIndex = SR_ZERO;
                }
            }
        }
    }
    else
    {
        if (SrMplsDeleteFTN (pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to delete FTN \n", __func__,
                     __LINE__);
        }
        if (&pRtrInfo->NextHopList != NULL)
        {
            TMO_SLL_FreeNodes ((tTMO_SLL *) & pRtrInfo->NextHopList,
                               SrMemPoolIds[MAX_SR_RTR_NH_SIZING_ID]);
        }

    }
    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC | SR_CTRL_TRC, "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/**************************************************************************
 * Function Name   : SrFrrFtnCreateOrDelete
 * Description     : Function is to create or delete SR FRR  FTN
                     with destination prefix as reference.
 * Input           : pRtrInfo, u1cmdType
                     if u1cmdType is SR_TRUE, then SR creation
                     else, SR deletion flow
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesfully deletion)
 *                   SR_FAILURE (No TEPATH Entry Found or any other Failure
 *                   scenario)
 *************************************************************************/

PUBLIC UINT4
SrFrrFtnCreateOrDelete (tSrRtrInfo * pSrRtrInfo,
                        tSrRtrNextHopInfo * pSrRtrNextHopInfo, UINT1 u1cmdType)
{
    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d ENTRY\n", __func__, __LINE__);

    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4IpAddress = SR_ZERO;
    UINT4               u4Port = SR_ZERO;
    UINT4               u4L3Ifindex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    INT4                i4LfaStatus = ALTERNATE_DISABLED;
    tNetIpv4LfaRtInfo   NetIpLfaRtInfo;
    tLfaRtInfoQueryMsg  LfaRtQuery;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tSrTeRtEntryInfo    getSrTeRtEntryInfo;

    MEMSET (&getSrTeRtEntryInfo, 0, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&NetIpLfaRtInfo, SR_ZERO, sizeof (tNetIpv4LfaRtInfo));
    SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;

    /* Ignore the Notification if LfaStatus is not ENABLED Administratively */
    nmhGetFsSrV4AlternateStatus (&i4LfaStatus);

    if (u1cmdType == SR_TRUE)
    {
        if ((pSrRtrInfo != NULL) && (pSrRtrNextHopInfo != NULL))
        {
            MEMCPY (&(LfaRtQuery.u4DestinationIpAddress),
                    &pSrRtrInfo->prefixId.Addr.u4Addr, MAX_IPV4_ADDR_LEN);
            LfaRtQuery.u4DestinationSubnetMask = 0xFFFFFFFF;
            MEMCPY (&(LfaRtQuery.u4PrimaryNextHop),
                    &pSrRtrNextHopInfo->nextHop.Addr.u4Addr, MAX_IPV4_ADDR_LEN);
            LfaRtQuery.u2RtProto = OSPF_ID;
            LfaRtQuery.u4ContextId = 0;
            if (CfaUtilGetIfIndexFromMplsTnlIf
                (pSrRtrNextHopInfo->u4OutIfIndex, &u4L3Ifindex,
                 TRUE) == CFA_FAILURE)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d Exiting: Not able to get IfIndex from MplsTnlIf\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            if (NetIpv4GetPortFromIfIndex (u4L3Ifindex, &u4Port) == IP_FAILURE)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d Exiting: NetIpv4GetPortFromIfIndex failed\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            LfaRtQuery.u4NextHopIfIndx = u4Port;
            if (NetIpv4LfaGetRoute (&LfaRtQuery, &NetIpLfaRtInfo) !=
                NETIPV4_SUCCESS)
            {
                SR_TRC3 (SR_CTRL_TRC,
                         "%s:%d Exiting: Not able to fetch the lfa route for the destination %x\n",
                         __func__, __LINE__, LfaRtQuery.u4DestinationIpAddress);
                return SR_SUCCESS;
            }
        }
        else
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d Exiting: pSrRtrInfo or pSrRtrNextHopInfo is NULL\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        if (NetIpLfaRtInfo.u1LfaType == SR_LFA)
        {
            {

                SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
                SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_TRUE;
                u4IpAddress =
                    (NetIpLfaRtInfo.u4LfaNextHop & NetIpLfaRtInfo.u4DestMask);
                if (CfaIpIfGetIfIndexForNetInCxt
                    (SR_ZERO, u4IpAddress,
                     &SrRtrAltNextHopInfo.u4OutIfIndex) == CFA_FAILURE)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Exiting: Not able to get Out IfIndex \n",
                             __func__, __LINE__);
                }
                MEMCPY (&SrRtrAltNextHopInfo.nextHop,
                        &NetIpLfaRtInfo.u4LfaNextHop, sizeof (UINT4));

                MEMCPY (&SrRtrAltNextHopInfo.PrimarynextHop,
                        &pSrRtrNextHopInfo->nextHop, sizeof (tGenU4Addr));
                /*pSrRtrInfo->u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */

                if (i4LfaStatus == ALTERNATE_ENABLED)    /*If LFA is enabled, create mpls tunnel and LSP */
                {
                    if (CfaIfmCreateStackMplsTunnelInterface
                        (SrRtrAltNextHopInfo.u4OutIfIndex,
                         &u4MplsTnlIfIndex) == CFA_SUCCESS)
                    {
                        SR_TRC4 (SR_CTRL_TRC,
                                 "%s:%d SrRtrAltNextHopInfo.u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS\n",
                                 __func__, __LINE__,
                                 SrRtrAltNextHopInfo.u4OutIfIndex,
                                 u4MplsTnlIfIndex);

                        SrRtrAltNextHopInfo.u4OutIfIndex = u4MplsTnlIfIndex;
                        pSrRtrInfo->u4LfaOutIfIndex = u4MplsTnlIfIndex;

                    }
                    else
                    {
                        pSrRtrInfo->u4LfaNextHop = SR_FALSE;
                        pSrRtrInfo->u1LfaType = SR_ZERO;
                        pSrRtrInfo->u4LfaOutIfIndex = SR_ZERO;
                        SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d MPLS tunnel interface creation failed for interface: %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                                 __func__, __LINE__,
                                 SrRtrAltNextHopInfo.u4OutIfIndex,
                                 u4MplsTnlIfIndex);
                        return SR_FAILURE;
                    }
                    /*LFA nexthop updated in SrRtrInfo */
                    pSrRtrInfo->u4LfaNextHop = NetIpLfaRtInfo.u4LfaNextHop;
                    pSrRtrInfo->u1LfaType = SR_LFA_ROUTE;
                    MEMCPY (&(pSrRtrInfo->u4DestMask),
                            &(NetIpLfaRtInfo.u4DestMask), sizeof (UINT4));

                    if (SrMplsCreateOrDeleteLsp
                        (MPLS_MLIB_FTN_CREATE, pSrRtrInfo, NULL,
                         &SrRtrAltNextHopInfo) == SR_SUCCESS)
                    {
                        SR_TRC4 (SR_CTRL_TRC,
                                 "%s:%d LFA LSP created successfully for FEC: %x via Next Hop: %x\n",
                                 __func__, __LINE__,
                                 pSrRtrInfo->prefixId.Addr.u4Addr,
                                 pSrRtrInfo->u4LfaNextHop);
                    }
                    else
                    {
                        pSrRtrInfo->u4LfaNextHop = SR_FALSE;
                        pSrRtrInfo->u1LfaType = SR_ZERO;
                        pSrRtrInfo->u4LfaOutIfIndex = SR_ZERO;
                        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                 "%s:%d LFA LSP creation failed for FEC: %x via Next Hop: %x\n",
                                 __func__, __LINE__,
                                 pSrRtrInfo->prefixId.Addr.u4Addr,
                                 pSrRtrInfo->u4LfaNextHop);
                        return SR_FAILURE;
                    }

                }
            }
        }
    }
    else                        /* LFA disable */
    {
        if (pSrRtrInfo->u4LfaNextHop == SR_FALSE)
        {
            SR_TRC2 (SR_CTRL_TRC, "%s:%d Exit: SR LFA doesn't exist\n",
                     __func__, __LINE__);
            return SR_SUCCESS;
        }

        SrRtrAltNextHopInfo.u4OutIfIndex = pSrRtrInfo->u4LfaOutIfIndex;
        u4MplsTnlIfIndex = SrRtrAltNextHopInfo.u4OutIfIndex;
        if (SrMplsCreateOrDeleteLsp
            (MPLS_MLIB_FTN_DELETE, pSrRtrInfo, NULL,
             &SrRtrAltNextHopInfo) == SR_SUCCESS)
        {
            SR_TRC4 (SR_CTRL_TRC,
                     "%s:%d LFA LSP created successfully for FEC: %x via Next Hop: %x\n",
                     __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
                     pSrRtrInfo->u4LfaNextHop);
        }
        else
        {
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d LFA LSP creation failed for FEC: %x via Next Hop: %x\n",
                     __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
                     pSrRtrInfo->u4LfaNextHop);
            return SR_FAILURE;
        }
        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {
            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed\n",
                         __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }
        }
        SrRtrAltNextHopInfo.u4OutIfIndex = SR_ZERO;
        pSrRtrInfo->u4LfaNextHop = SR_FALSE;
        pSrRtrInfo->u1LfaType = SR_ZERO;
        pSrRtrInfo->u4LfaOutIfIndex = SR_ZERO;

    }

    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d EXIT\n", __func__, __LINE__);
    return SR_SUCCESS;

}

/***********************************************************************************
 * Function Name : SrTeLfaFtnActivate
 * Description   : This routine will Activate TE LFA FTN for a mandatry Rtr
 * Input(s)      : pSrTeDestRtInfo - TeRouteEntry
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrTeLfaFtnActivate (tSrRtrInfo * pSrTeMandRtrInfo)
{
    UINT4               u4TopLabel = SR_ZERO;
    UINT4               u4DestLabel = SR_ZERO;
    tGenU4Addr          destAddr;
    tLspInfo            LspInfo;
    tNHLFE              Nhlfe;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    UINT4               u4isRtrEntryPresent = SR_ZERO;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    UINT4               u4InLabel = SR_ZERO;
    tSrRtrEntry        *pSrPeerRtrInfo = NULL;
    tTMO_SLL_NODE      *pSrOptRtrInfo = NULL;

    MEMSET (&LspInfo, SR_ZERO, sizeof (tLspInfo));
    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&Nhlfe, SR_ZERO, sizeof (tNHLFE));
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMCPY (&destAddr, &(pSrTeMandRtrInfo->prefixId), sizeof (tGenU4Addr));

    SR_TRC3 (SR_CTRL_TRC,
             "%s:%d Calling TE FTN Route_Set Processing Via 0x%x \n",
             __func__, __LINE__, destAddr.Addr.u4Addr);
    if (destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        SrTeRtInfo.mandRtrId.Addr.u4Addr = destAddr.Addr.u4Addr;
        SrTeRtInfo.mandRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;

        while ((pSrTeRtInfo = SrGetNextTeRouteTableEntry (&SrTeRtInfo)) != NULL)
        {
            u4isRtrEntryPresent = SR_FALSE;
            if (pSrTeRtInfo->mandRtrId.Addr.u4Addr != destAddr.Addr.u4Addr)
            {
                TMO_SLL_Scan (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                              pSrOptRtrInfo, tTMO_SLL_NODE *)
                {
                    pSrPeerRtrInfo = (tSrRtrEntry *) pSrOptRtrInfo;

                    if (pSrPeerRtrInfo->routerId.Addr.u4Addr ==
                        destAddr.Addr.u4Addr)
                    {
                        u4isRtrEntryPresent = SR_TRUE;
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }

                if (u4isRtrEntryPresent == SR_FALSE)
                {
                    /*Variable updation for next iteration- Start */
                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    SrTeRtInfo.destAddr.Addr.u4Addr =
                        pSrTeRtInfo->destAddr.Addr.u4Addr;
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    SrTeRtInfo.mandRtrId.Addr.u4Addr =
                        pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    pSrTeRtInfo = NULL;
                    /*Variable updation for next iteration- End */
                    continue;
                }
            }

            if ((pSrTeRtInfo->u1MPLSStatus & SR_FTN_CREATED) != SR_FTN_CREATED)
            {
                SR_TRC3 (SR_CTRL_TRC,
                         "%s:%d FTN not created for TE-Path with Dest %x \n",
                         __func__, __LINE__, destAddr.Addr.u4Addr);

                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                SrTeRtInfo.destAddr.Addr.u4Addr =
                    pSrTeRtInfo->destAddr.Addr.u4Addr;
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.mandRtrId.Addr.u4Addr =
                    pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */
                continue;
            }

            /* Get the SrRtrInfo belongs to this SrTeRt */
            if ((pDestSrRtrInfo =
                 SrGetSrRtrInfoFromRtrId (&pSrTeRtInfo->destAddr)) == NULL)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d SrGetSrRtrInfoFromRtrId FAILURE for %x \n",
                         __func__, __LINE__, pSrTeRtInfo->destAddr.Addr.u4Addr);
                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                SrTeRtInfo.destAddr.Addr.u4Addr =
                    pSrTeRtInfo->destAddr.Addr.u4Addr;
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.mandRtrId.Addr.u4Addr =
                    pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */

                continue;
            }

            /* There will be only one SrRtrNhNode for SR-TE */
            pSrRtrNextHopInfo =
                (tSrRtrNextHopInfo *)
                TMO_SLL_First (&(pDestSrRtrInfo->NextHopList));
            if (pSrRtrNextHopInfo == NULL)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         " %s: %d Cannot find the next hop for %x \r\n",
                         __func__, __LINE__, pSrTeRtInfo->destAddr.Addr.u4Addr);
                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                SrTeRtInfo.destAddr.Addr.u4Addr =
                    pSrTeRtInfo->destAddr.Addr.u4Addr;
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.mandRtrId.Addr.u4Addr =
                    pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */

                continue;
            }

            /* Populate In-Label */
            SR_GET_SELF_IN_LABEL (&u4InLabel, pDestSrRtrInfo->u4SidValue);
            if (SrGetLabelForLFATEMandNode (&pSrTeRtInfo->LfaMandRtrId,
                                            &pSrTeRtInfo->mandRtrId,
                                            &u4TopLabel) == SR_FAILURE)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Unable to get TE LFA"
                         " mandatory node label for Te dest %x \n", __func__,
                         __LINE__, pSrTeRtInfo->destAddr.Addr.u4Addr);
                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                SrTeRtInfo.destAddr.Addr.u4Addr =
                    pSrTeRtInfo->destAddr.Addr.u4Addr;
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.mandRtrId.Addr.u4Addr =
                    pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */

                continue;
            }
            if (SrTeGetLabelForRtrId (&pSrTeRtInfo->destAddr,
                                      &pSrTeRtInfo->mandRtrId,
                                      &u4DestLabel) == SR_FAILURE)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d Unable to get u4DestLabel for prefix %x \n",
                         __func__, __LINE__, pSrTeRtInfo->destAddr.Addr.u4Addr);
                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                SrTeRtInfo.destAddr.Addr.u4Addr =
                    pSrTeRtInfo->destAddr.Addr.u4Addr;
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.mandRtrId.Addr.u4Addr =
                    pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */

                continue;
            }

            Nhlfe.NextHopAddr.u4Addr = pSrTeRtInfo->LfaNexthop.Addr.u4Addr;
            Nhlfe.u4OutIfIndex = pSrTeRtInfo->u4OutLfaIfIndex;    /* TE FTN Tunnel index */
            LspInfo.u4LfaIlmTunIndex = pSrTeRtInfo->u4FRRIlmIfIndex;    /* TE ILM Tunnel Index */
            LspInfo.u4RemoteNodeLabel = u4TopLabel;    /* TE Mandatory Node Label */
            Nhlfe.u4OutLabel = u4DestLabel;    /* TE Dest Out-Label */

            LspInfo.u4IfIndex = pSrRtrNextHopInfo->u4OutIfIndex;    /* Primary TE FTN Tunnel if-index */
            LspInfo.u4InTopLabel = u4InLabel;
            LspInfo.FecParams.u4TnlId = pDestSrRtrInfo->u4TeSwapOutIfIndex;    /*primary ILM Tunnel if-index */
            LspInfo.FecParams.u1FecType = SR_FEC_PREFIX_TYPE;
            LspInfo.FecParams.u2AddrType = SR_IPV4_ADDR_TYPE;
            LspInfo.FecParams.DestAddrPrefix.u4Addr =
                pDestSrRtrInfo->prefixId.Addr.u4Addr;
            LspInfo.FecParams.DestMask.u4Addr = SR_IPV4_DEST_MASK;
            MEMCPY (&LspInfo.PrimarynextHop, &pSrTeRtInfo->PrimaryNexthop,
                    sizeof (tGenU4Addr));
            LspInfo.Direction = MPLS_DIRECTION_FORWARD;
            LspInfo.u1Owner = MPLS_OWNER_SR_TE;
            LspInfo.bIsTeFtn = SR_TRUE;    /* To do only FTN processing alone in NP for TE */
            LspInfo.bIsSrTeLsp = SR_TRUE;
            LspInfo.u1Protocol = MPLS_PROTOCOL_SR_OSPF;

            /* Fill LFA path information */
            Nhlfe.u1NHAddrType = SR_IPV4_ADDR_TYPE;
            Nhlfe.u1OperStatus = MPLS_STATUS_UP;

            LspInfo.pNhlfe = (tNHLFE *) (&Nhlfe);

            if (MplsMlibUpdate (MPLS_LFA_LSP_ACTIVATE, &LspInfo)
                == MPLS_FAILURE)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d MplsMlibUpdate Failed for"
                         "case:MPLS_LFA_LSP_ACTIVATE \n", __func__, __LINE__);
                /*Variable updation for next iteration- Start */
                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                SrTeRtInfo.destAddr.Addr.u4Addr =
                    pSrTeRtInfo->destAddr.Addr.u4Addr;
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                SrTeRtInfo.mandRtrId.Addr.u4Addr =
                    pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                pSrTeRtInfo = NULL;
                /*Variable updation for next iteration- End */

                continue;
            }
            pSrTeRtInfo->bIsSrTeFRRActive = SR_TRUE;

            /*Variable updation for next iteration- Start */
            MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
            SrTeRtInfo.destAddr.Addr.u4Addr = pSrTeRtInfo->destAddr.Addr.u4Addr;
            SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
            SrTeRtInfo.mandRtrId.Addr.u4Addr =
                pSrTeRtInfo->mandRtrId.Addr.u4Addr;
            SrTeRtInfo.destAddr.u2AddrType = pSrTeRtInfo->destAddr.u2AddrType;
            SrTeRtInfo.mandRtrId.u2AddrType = pSrTeRtInfo->mandRtrId.u2AddrType;
            pSrTeRtInfo = NULL;
            /*Variable updation for next iteration- End */
        }
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrTeMplsModifyLSP
 * Description   : This routine will Update the TE FTN and ILM entries in Control
 *                 plane.
 * Input(s)      : pSrRtrInfo - Router Info
 *                 pSrTeDestRtInfo - TeRouteEntry
 *                 uu1IsTeMandRtr - Process either TE FTN or TE ILM
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrTeMplsModifyLSP (tSrRtrInfo * pSrRtrInfo, tSrTeRtEntryInfo * pSrTeDestRtInfo,
                   UINT1 u1IsTeMandRtr)
{
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tSrRtrNextHopInfo   NewSrRtrNextHopInfo;
    tSrInSegInfo        SrInSegInfo;
    tSrRtrInfo          LfaTeDestSrRtrInfo;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    UINT4               u4TopLabel = SR_ZERO;
    UINT4               u4DestLabel = SR_ZERO;
    tSrTeLblStack       SrTeLblStack;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tGenU4Addr          dummyAddr;
    UINT4               u4Counter = SR_ZERO;
    UINT4               u4OptLabelCount = SR_ZERO;
    UINT4               u4TmpOptLabelCounter = SR_ZERO;
    UINT4               au4SrOptRtrLabel[SR_MAX_TE_OPTIONAL_RTR] = { SR_ZERO };
    tSrRtrInfo          TmpSrRtrInfo;

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&NewSrRtrNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&TmpSrRtrInfo, SR_ZERO, sizeof (tSrRtrInfo));
    MEMSET (&LfaTeDestSrRtrInfo, SR_ZERO, sizeof (tSrRtrInfo));

    if (u1IsTeMandRtr == SR_TRUE)
    {                            /* TE FTN modify */
        /* There will be only one SrRtrNhNode for SR-TE */
        SR_TRC3 (SR_CTRL_TRC, "%s:%d FTN Modify case for %x \n", __func__,
                 __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
        pSrRtrNextHopInfo =
            (tSrRtrNextHopInfo *) TMO_SLL_First (&(pSrRtrInfo->NextHopList));
        if (pSrRtrNextHopInfo == NULL)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s: %d Cannot find the next hop for %x \r\n", __func__,
                     __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }

        MEMCPY (&SrRtrAltNextHopInfo, pSrRtrNextHopInfo,
                sizeof (tSrRtrNextHopInfo));
        pSrRtrNextHopInfo->bIsOnlyCPUpdate = SR_TRUE;

        MEMCPY (&SrTeLblStack, &(pSrTeDestRtInfo->SrPriTeLblStack),
                sizeof (tSrTeLblStack));

        u4OptLabelCount = (UINT4) (SrTeLblStack.u1StackSize - SR_ONE);
        u4TmpOptLabelCounter = u4OptLabelCount - 1;
        for (u4Counter = SR_ONE; u4Counter <= u4OptLabelCount;
             u4Counter++, u4TmpOptLabelCounter--)
        {
            SrTeLblStack.u4LabelStack[u4Counter] =
                au4SrOptRtrLabel[u4TmpOptLabelCounter];
        }

        /* PRIMARY TE FTN DELETE  */
        if ((pSrTeDestRtInfo->u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
        {
            if (SrMplsCreateOrDeleteStackLsp
                (MPLS_MLIB_FTN_DELETE, pSrRtrInfo, NULL, &SrTeLblStack,
                 pSrRtrNextHopInfo, SR_FALSE) == SR_FAILURE)
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d SrMplsCreateOrDeleteStackLsp FAILURE for %x \n",
                         __func__, __LINE__,
                         pSrTeDestRtInfo->destAddr.Addr.u4Addr);
                return SR_FAILURE;
            }
        }

#ifdef CFA_WANTED
        u4MplsTnlIfIndex = pSrRtrNextHopInfo->u4OutIfIndex;

        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {
            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                         __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }
        }

        pSrRtrNextHopInfo->u4OutIfIndex = SR_ZERO;
#endif
        pSrTeDestRtInfo->u1MPLSStatus &= (UINT1) (~SR_FTN_CREATED);

        /* LFA FTN DELETION */
        if (SrGetLabelForLFATEMandNode
            (&pSrTeDestRtInfo->LfaMandRtrId, &pSrTeDestRtInfo->mandRtrId,
             &u4TopLabel) == SR_FAILURE)
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d Unable to get TE LFA mandatory node label\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        if (SrTeGetLabelForRtrId (&pSrTeDestRtInfo->destAddr,
                                  &pSrTeDestRtInfo->mandRtrId,
                                  &u4DestLabel) == SR_FAILURE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d Unable to get u4DestLabel for prefix %x \n", __func__,
                     __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }
        /* Nexthop info update */
        NewSrRtrNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        NewSrRtrNextHopInfo.nextHop.Addr.u4Addr =
            pSrTeDestRtInfo->LfaNexthop.Addr.u4Addr;
        NewSrRtrNextHopInfo.u4OutIfIndex = pSrTeDestRtInfo->u4OutLfaIfIndex;
        NewSrRtrNextHopInfo.bIsFRRHwLsp = SR_FALSE;
        NewSrRtrNextHopInfo.bIsOnlyCPUpdate = SR_TRUE;
        NewSrRtrNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
        MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
        u4OptLabelCount = 0;
        SrTeLblStack.u4TopLabel = u4TopLabel;
        SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;
        SrTeLblStack.u1StackSize = (UINT1) (u4OptLabelCount + SR_ONE);

        if (SrMplsCreateOrDeleteStackLsp
            (MPLS_MLIB_FTN_DELETE, pSrRtrInfo, NULL, &SrTeLblStack,
             &NewSrRtrNextHopInfo, SR_TRUE) == SR_FAILURE)
        {
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "line %d func %s -----SR TE LFA FTN deletion failed  for DEST %x nexthop %x  \r\n",
                     __LINE__, __func__, pSrTeDestRtInfo->destAddr.Addr.u4Addr,
                     NewSrRtrNextHopInfo.nextHop.Addr.u4Addr);
            return SR_FAILURE;

        }
        else
        {
            SR_TRC4 (SR_CTRL_TRC,
                     "line %d func %s -----SR TE LFA FTN sucessfully deleted for DEST %x nexthop %x  \r\n",
                     __LINE__, __func__, pSrTeDestRtInfo->destAddr.Addr.u4Addr,
                     NewSrRtrNextHopInfo.nextHop.Addr.u4Addr);

        }
        pSrTeDestRtInfo->u1LfaMPLSStatus &= (UINT1) (~SR_FTN_CREATED);
        /* NEW PRIMARY TE ADD with Lfa Values */
        pSrTeDestRtInfo->PrimaryNexthop.Addr.u4Addr =
            pSrTeDestRtInfo->LfaNexthop.Addr.u4Addr;
        pSrTeDestRtInfo->PrimaryNexthop.u2AddrType = SR_IPV4_ADDR_TYPE;
        pSrRtrNextHopInfo->u4OutIfIndex = pSrTeDestRtInfo->u4OutLfaIfIndex;
        pSrRtrNextHopInfo->nextHop.Addr.u4Addr =
            pSrTeDestRtInfo->LfaNexthop.Addr.u4Addr;

        pSrTeDestRtInfo->LfaMandRtrId.Addr.u4Addr = SR_ZERO;
        pSrTeDestRtInfo->LfaNexthop.Addr.u4Addr = SR_ZERO;
        pSrTeDestRtInfo->u4OutLfaIfIndex = SR_ZERO;
        u4TopLabel = SR_ZERO;
        if (SrTeGetLabelForRtrId (&pSrTeDestRtInfo->mandRtrId,
                                  &dummyAddr, &u4TopLabel) == SR_FAILURE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Unable to get TopLabel for prefix %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }
        u4DestLabel = SR_ZERO;
        if (SrTeGetLabelForRtrId (&pSrTeDestRtInfo->destAddr,
                                  &pSrTeDestRtInfo->mandRtrId,
                                  &u4DestLabel) == SR_FAILURE)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d Unable to get DestLabel for prefix %x \n", __func__,
                     __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }
        MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
        SrTeLblStack.u4TopLabel = u4TopLabel;
        SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;
        u4OptLabelCount = 0;
        SrTeLblStack.u1StackSize = (UINT1) (u4OptLabelCount + SR_ONE);
        MEMCPY (&(pSrTeDestRtInfo->SrPriTeLblStack), &SrTeLblStack,
                sizeof (tSrTeLblStack));

        if (SrMplsCreateOrDeleteStackLsp
            (MPLS_MLIB_FTN_CREATE, pSrRtrInfo, NULL, &SrTeLblStack, pSrRtrNextHopInfo,
             SR_FALSE) == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d SrMplsCreateOrDeleteStackLsp FAILURE for destination %x \n",
                     __func__, __LINE__, pSrTeDestRtInfo->destAddr.Addr.u4Addr);
            return SR_FAILURE;
        }
        pSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;
        pSrTeDestRtInfo->bIsSrTeFRRActive = SR_FALSE;
        pSrRtrNextHopInfo->bIsOnlyCPUpdate = SR_FALSE;
        /* Reset the cp flag */
    }
    /* TE ILM modify */
    else
    {
        SR_TRC4 (SR_CTRL_TRC,
                 "%s ILM Modify case for %x with SwapOutIfIndex : %d LfaSwapOutIfIndex : %d \n",
                 __func__, pSrRtrInfo->prefixId.Addr.u4Addr,
                 pSrRtrInfo->u4TeSwapOutIfIndex,
                 pSrRtrInfo->u4LfaSwapOutIfIndex);
        if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
        {
            /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
            SR_GET_SELF_IN_LABEL (&SrInSegInfo.u4InLabel,
                                  pSrRtrInfo->u4SidValue);

            SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;
            SrRtrAltNextHopInfo.bIsOnlyCPUpdate = SR_TRUE;
            SrRtrAltNextHopInfo.u4SwapOutIfIndex =
                pSrRtrInfo->u4TeSwapOutIfIndex;
            SrRtrAltNextHopInfo.nextHop.Addr.u4Addr =
                pSrRtrInfo->teSwapNextHop.Addr.u4Addr;
            SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
            SrRtrAltNextHopInfo.PrimarynextHop.Addr.u4Addr =
                pSrRtrInfo->teSwapNextHop.Addr.u4Addr;
            SrRtrAltNextHopInfo.u1FRRNextHop = SR_PRIMARY_NEXTHOP;

            if (SrMplsCreateOrDeleteLsp
                (MPLS_MLIB_ILM_DELETE, pSrRtrInfo, &SrInSegInfo,
                 &SrRtrAltNextHopInfo) != SR_SUCCESS)
            {
                SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d ILM Delete Failed for"
                         " prefix = %x Next hop = %x \n", __func__, __LINE__,
                         pSrRtrInfo->prefixId.Addr.u4Addr,
                         SrRtrAltNextHopInfo.nextHop.Addr.u4Addr);
                return SR_FAILURE;
            }
        }

#ifdef CFA_WANTED
        u4MplsTnlIfIndex = SrRtrAltNextHopInfo.u4SwapOutIfIndex;
        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {
            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete ILM Tunnel for"
                         " prefix = %x Next hop = %x \n", __func__, __LINE__,
                         pSrRtrInfo->prefixId.Addr.u4Addr,
                         SrRtrAltNextHopInfo.nextHop.Addr.u4Addr);
                return SR_FAILURE;
            }
        }
        else
        {
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d Failed to Fetch If Index from ILM Tunnel for"
                     " prefix = %x Next hop = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr,
                     SrRtrAltNextHopInfo.nextHop.Addr.u4Addr);
            return SR_FAILURE;
        }
        SrRtrAltNextHopInfo.u4SwapOutIfIndex = SR_ZERO;
#endif
        /* LFA LSP Delete */
        MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
        /* Delete the LFA Entry only in Control Plane */
        SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;
        SrRtrAltNextHopInfo.bIsOnlyCPUpdate = SR_TRUE;
        SrRtrAltNextHopInfo.u4SwapOutIfIndex = pSrRtrInfo->u4LfaSwapOutIfIndex;
        SrRtrAltNextHopInfo.nextHop.Addr.u4Addr = pSrRtrInfo->u4LfaNextHop;
        SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        SrRtrAltNextHopInfo.PrimarynextHop.Addr.u4Addr =
            pSrRtrInfo->teSwapNextHop.Addr.u4Addr;
        SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;

        MEMCPY (&(TmpSrRtrInfo), pSrRtrInfo, sizeof (tSrRtrInfo));
        if (SrMplsDeleteILM (&SrInSegInfo, pSrRtrInfo, &SrRtrAltNextHopInfo) !=
            SR_SUCCESS)
        {
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d ILM Delete Failed for"
                     " prefix = %x Next hop = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr,
                     SrRtrAltNextHopInfo.nextHop.Addr.u4Addr);
            return SR_FAILURE;
        }
        /* LFA FTN index will be used a Primary Index */
        pSrRtrInfo->u4TeSwapOutIfIndex = TmpSrRtrInfo.u4LfaSwapOutIfIndex;
        pSrRtrInfo->teSwapNextHop.Addr.u4Addr = TmpSrRtrInfo.u4LfaNextHop;
        pSrRtrInfo->u4LfaNextHop = SR_ZERO;
        pSrRtrInfo->u4LfaOutIfIndex = SR_ZERO;
        pSrRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
        pSrRtrInfo->u1LfaType = SR_ZERO;
        pSrRtrInfo->bIsLfaActive = SR_ZERO;
        MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
        SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;
        SrRtrAltNextHopInfo.bIsOnlyCPUpdate = SR_TRUE;
        SrRtrAltNextHopInfo.u4SwapOutIfIndex = pSrRtrInfo->u4TeSwapOutIfIndex;
        SrRtrAltNextHopInfo.nextHop.Addr.u4Addr =
            pSrRtrInfo->teSwapNextHop.Addr.u4Addr;
        SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        SrRtrAltNextHopInfo.PrimarynextHop.Addr.u4Addr =
            pSrRtrInfo->teSwapNextHop.Addr.u4Addr;
        SrRtrAltNextHopInfo.u1FRRNextHop = SR_PRIMARY_NEXTHOP;
        pSrRtrInfo->u1TeSwapStatus = 1;

        SR_TRC4 (SR_CTRL_TRC,
                 "In %s New ILM add for %x with SwapOutIfIndex : %d LfaSwapOutIfIndex : %d \n",
                 __func__, pSrRtrInfo->prefixId.Addr.u4Addr,
                 pSrRtrInfo->u4TeSwapOutIfIndex,
                 pSrRtrInfo->u4LfaSwapOutIfIndex);
        /* Create New Primary */
        if (SrMplsCreateILM (pSrRtrInfo, &SrRtrAltNextHopInfo) == SR_SUCCESS)
        {
            SR_TRC4 (SR_CTRL_TRC, "%s:%d ILM Create Successful for"
                     " prefix = %x Next hop = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr,
                     pSrRtrInfo->teSwapNextHop.Addr.u4Addr);
        }
        else
        {
            if (SrReleaseMplsInLabel (pSrRtrInfo->u2AddrType,
                                      pSrRtrInfo->u4SidValue) == SR_FAILURE)
            {
                SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Failed to Release in label for"
                         " prefix = %x Next hop = %x \n", __func__, __LINE__,
                         pSrRtrInfo->prefixId.Addr.u4Addr,
                         pSrRtrInfo->teSwapNextHop.Addr.u4Addr);
                return SR_FAILURE;
            }
#ifdef CFA_WANTED
            u4MplsTnlIfIndex = pSrRtrInfo->u4TeSwapOutIfIndex;
            if (CfaUtilGetIfIndexFromMplsTnlIf
                (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
            {
                if (CfaIfmDeleteStackMplsTunnelInterface
                    (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                {
                    SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d Failed to Delete ILM Tunnel for"
                             " prefix = %x Next hop = %x \n", __func__, __LINE__,
                             pSrRtrInfo->prefixId.Addr.u4Addr,
                             pSrRtrInfo->teSwapNextHop.Addr.u4Addr);
                    return SR_FAILURE;
                }
                pSrRtrInfo->u4TeSwapOutIfIndex = u4L3VlanIf;
            }
            else
            {
                SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d Failed to Fetch If Index from ILM Tunnel for"
                         " prefix = %x Next hop = %x \n", __func__, __LINE__,
                         pSrRtrInfo->prefixId.Addr.u4Addr,
                         pSrRtrInfo->teSwapNextHop.Addr.u4Addr);
                return SR_FAILURE;
            }
#endif
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d ILM Create Failed for"
                     " prefix = %x Next hop = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr,
                     pSrRtrInfo->teSwapNextHop.Addr.u4Addr);
            return SR_FAILURE;
        }
    }

    SR_TRC2 (SR_MAIN_TRC | SR_FN_ENTRY_EXIT_TRC | SR_CTRL_TRC, "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

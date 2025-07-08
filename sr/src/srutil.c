/********************************************************************
 * Copyright (C) 2007 Aricent Inc . All Rights Reserved
 *
 * $Id$
 *
 * Description: This file contains utility routines for SR task
 *
 *********************************************************************/
#ifndef _SR_UTIL_C
#define _SR_UTIL_C

#include "srincs.h"
#include "rtm6.h"

#include "../../netip/rtm/inc/rtmtdfs.h"
#include "../../mpls/mplsdb/mplslsr.h"
extern tSrAltModuleStatus gu4AltModuleStatus;

enum
{
    MPLS_INVALID = 0,
    MPLS_LABEL_POP,
    MPLS_LABEL_PUSH,
    MPLS_LABEL_SWAP
};

/******************************************************************************
 * Function Name      : SrMainTaskUnLock
 *
 * Description        : This function is used to unlock the semaphore in SR task
 *
 * Input(s)           : None
 * Output(s)          : None
 * Return Value(s)    : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
PUBLIC INT4
SrMainTaskUnLock (VOID)
{
    if (OsixSemGive (SR_SEM_ID) == OSIX_FAILURE)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC, "SemGive failure for Mutual Exclusion Semaphore\n");
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/******************************************************************************
 * Function Name      : SrMainTaskLock
 *
 * Description        : This function is used to lock the semaphore in SR task
 *
 * Input(s)           : None
 * Output(s)          : None
 * Return Value(s)    : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
PUBLIC INT4
SrMainTaskLock (VOID)
{
    if (OsixSemTake (SR_SEM_ID) == OSIX_FAILURE)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC, "TakeSem failure for %s \n");
        return SR_FAILURE;
    }

    return SR_SUCCESS;
}

/******************************************************************************
 * Function Name      : SrUtilGetLblId
 * Description        : This function returns the Group ID of SR registered
 *                         with label manager
 * Input(s)           : None
 * Output(s)          : Group Id of SR registered with label manager
 * Return Value(s)    : None
 *****************************************************************************/
PUBLIC VOID
SrUtilGetLblId (UINT2 *pu2SrLblId)
{
    if (gSrGlobalInfo.isLabelSpaceRsvd == SR_FALSE)
    {
        if (SrResvLabelSpace () == SR_SUCCESS)
        {
            gSrGlobalInfo.isLabelSpaceRsvd = SR_TRUE;
        }
        else
        {
            gSrGlobalInfo.isLabelSpaceRsvd = SR_FALSE;
            *pu2SrLblId = SR_ZERO;
            return;
        }
    }

    *pu2SrLblId = (UINT2) gSrGlobalInfo.SrContext.u2SrLblGroupId;
}

/************************************************************************
**  Function Name   : SrUtilSidRbTreeCmpFunc
**  Description     : RBTree Compare function for SID interface list
**  Input           : Two RBTree Nodes to be compared
**  Output          : None
**  Returns         : 1/(-1)/0
*************************************************************************/
PUBLIC INT4
SrUtilSidRbTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2)
{
    tSrSidInterfaceInfo *sidEntry1 = (tSrSidInterfaceInfo *) pRBElem1;
    tSrSidInterfaceInfo *sidEntry2 = (tSrSidInterfaceInfo *) pRBElem2;

#ifdef MPLS_IPV6_WANTED
    if (sidEntry1->ipAddrType < sidEntry2->ipAddrType)
    {
        return -1;
    }
    else if (sidEntry1->ipAddrType > sidEntry2->ipAddrType)
    {
        return 1;
    }

    if (SR_IPV6_ADDR_TYPE == sidEntry1->ipAddrType)
    {
        return (MEMCMP ((sidEntry1->ifIpAddr.Addr.Ip6Addr.u1_addr),
                        (sidEntry2->ifIpAddr.Addr.Ip6Addr.u1_addr),
                        SR_IPV6_ADDR_LENGTH));
    }
    else
#endif
    {
        return (MEMCMP (&(sidEntry1->ifIpAddr.Addr.u4Addr),
                        &(sidEntry2->ifIpAddr.Addr.u4Addr),
                        SR_IPV4_ADDR_LENGTH));
    }
}

/************************************************************************
**  Function Name   : SrUtilRlfaPathRbTreeCmpFunc
**  Description     : RBTree Compare function for TE-Paths
**  Input           : Two RBTree Nodes to be compared
**  Output          : None
**  Returns         : SR_ZERO(FOUND)/SR_ONE(NOT_FOUND)
*************************************************************************/
PUBLIC INT4
SrUtilRlfaPathRbTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2)
{
    tSrRlfaPathInfo    *srTeEntry1 = (tSrRlfaPathInfo *) pRBElem1;
    tSrRlfaPathInfo    *srTeEntry2 = (tSrRlfaPathInfo *) pRBElem2;
#ifdef MPLS_IPV6_WANTED
    if (srTeEntry1->destAddr.u2AddrType < srTeEntry2->destAddr.u2AddrType)
    {
        return -1;
    }
    else if (srTeEntry1->destAddr.u2AddrType > srTeEntry2->destAddr.u2AddrType)
    {
        return 1;
    }

    if (SR_IPV6_ADDR_TYPE == srTeEntry1->destAddr.u2AddrType)
    {
        return (MEMCMP (srTeEntry1->destAddr.Addr.Ip6Addr.u1_addr,
                        srTeEntry2->destAddr.Addr.Ip6Addr.u1_addr,
                        IPV6_ADDR_LENGTH));

    }
    else
#endif
    {
        return (MEMCMP (&(srTeEntry1->destAddr.Addr.u4Addr),
                        &(srTeEntry2->destAddr.Addr.u4Addr),
                        SR_IPV4_ADDR_LENGTH));
    }

}

/************************************************************************
**  Function Name   : SrUtilOsRiLsCmpFunc
**  Description     : RBTree Compare function for OSPF RI LSAs
**  Input           : Two RBTree Nodes to be compared
**  Output          : None
**  Returns         : SR_ZERO(FOUND)/SR_ONE(NOT_FOUND)
*************************************************************************/
PUBLIC INT4
SrUtilOsRiLsCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2)
{
    tSrOsRiLsInfo  *srOsRiEntry1 = (tSrOsRiLsInfo *) pRBElem1;
    tSrOsRiLsInfo  *srOsRiEntry2 = (tSrOsRiLsInfo *) pRBElem2;
#ifdef MPLS_IPV6_WANTED
    if (srOsRiEntry1->advRtrId.u2AddrType < srOsRiEntry2->advRtrId.u2AddrType)
    {
        return -1;
    }
    else if (srOsRiEntry1->advRtrId.u2AddrType > srOsRiEntry2->advRtrId.u2AddrType)
    {
        return 1;
    }

    if (SR_IPV6_ADDR_TYPE == srOsRiEntry1->advRtrId.u2AddrType)
    {
        return (MEMCMP (srOsRiEntry1->advRtrId.Addr.Ip6Addr.u1_addr,
                        srOsRiEntry2->advRtrId.Addr.Ip6Addr.u1_addr,
                        IPV6_ADDR_LENGTH));
    }
    else
#endif
    {
        return (MEMCMP (&(srOsRiEntry1->advRtrId.Addr.u4Addr),
                        &(srOsRiEntry2->advRtrId.Addr.u4Addr),
                        SR_IPV4_ADDR_LENGTH));
    }

}

/*****************************************************************************
 * Function Name : SrSidInterfaceAddToSortIfLst
 * Description   : This routine adds the SR interface to the list of SR
 *                    interfaces created sorted based on IP address.
 * Input(s)      : pSrSidInterface - Pointer to the SR interface
 * Output(s)     : SR interface list updated with given interface
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrSidEntryAddToRBtree (tSrSidInterfaceInfo * pSrSidEntry)
{
    if (RBTreeAdd (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry) == RB_FAILURE)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC,
                "Create SrSid Entry  Table : RBTree Node Add Failed \n");
        return SR_FAILURE;
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrInitSidInterface
 * Description   : This routine initialized the SR interface and its state.
 * Input(s)      : pSrSidInterface - Pointer to the SR interface
 *                    ifIpAddr   - IP address to be associated with interface.
 * Output(s)     : pSrSidInterface with intialized values
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrInitSidInterface (tSrSidInterfaceInfo * pSrSidInterface,
                    tGenU4Addr * pIfIpAddr)
{
    /** Inialize the Interface with default values **/
    if (pIfIpAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        pSrSidInterface->ifIpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        MEMCPY (&pSrSidInterface->ifIpAddr.Addr.u4Addr,
                &(pIfIpAddr->Addr.u4Addr), SR_IPV4_ADDR_LENGTH);
    }
    else if (pIfIpAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        pSrSidInterface->ifIpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (pSrSidInterface->ifIpAddr.Addr.Ip6Addr.u1_addr,
                pIfIpAddr->Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
    }

    pSrSidInterface->u1RowStatus = CREATE_AND_WAIT;
    pSrSidInterface->u1AdminStatus = SR_ADMIN_DOWN;
#if 0
    pSrSidInterface->u1OperStatus = SR_OPER_DOWN;
#endif
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCreateSidInterface
 * Description   : This routine created the SR interface with associated SID.
 * Input(s)      : ifIpAddr - IP address of the interface
 * Output(s)     : None
 * Return(s)     : pSrSidInterface - Pointer to the SR interface created
 *****************************************************************************/
PUBLIC tSrSidInterfaceInfo *
SrCreateSidInterface (tGenU4Addr * pIfIpAddr)
{
    tSrSidInterfaceInfo *pSrSidInterface = NULL;

    /** Allocate the Memory from Pool**/
    if (SR_INTF_ALLOC (pSrSidInterface) == NULL)
    {
        SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                "%s:%d Memory Allocation for SidInterface failed \n",
                __func__, __LINE__);
        return NULL;
    }

    MEMSET (pSrSidInterface, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    /** Inialize the Interface with default values **/
    if (SrInitSidInterface (pSrSidInterface, pIfIpAddr) != SR_SUCCESS)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                "%s:%d Failed to initialize the interface with default values \n",
                __func__, __LINE__);
        return NULL;
    }
    return pSrSidInterface;
}

/*****************************************************************************
 * Function Name : SrResvLabelSpace
 * Description   : This routine reserves the label space with Label manager
 *                    for SR module
 * Input(s)      : u4IfIpAddr - IP address of the interface
 * Output(s)     : None
 * Return(s)     : pSrSidInterface - Pointer to the SR interface created
 *****************************************************************************/
PUBLIC UINT4
SrResvLabelSpace (VOID)
{
    tKeyInfoStruct      LblRangeInfo;

    if (gSrGlobalInfo.SrContext.u2SrLblGroupId == SR_ZERO)
    {
        MEMSET (&LblRangeInfo, SR_ZERO, sizeof (tKeyInfoStruct));

        LblRangeInfo.u4Key2Min = gSrGlobalInfo.u4GenLblSpaceMinLbl;
        LblRangeInfo.u4Key2Max = gSrGlobalInfo.u4GenLblSpaceMaxLbl;

        if (LblMgrCreateLabelSpaceGroup
            (LBL_ALLOC_BOTH_NUM, &LblRangeInfo, 1,
             SR_LBL_MODULE_ID, PER_PLATFORM_INTERFACE_INDEX,
             &gSrGlobalInfo.SrContext.u2SrLblGroupId) == LBL_FAILURE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                    "%s:%d Failed to allocate both odd and even number from"
                    "the range of Key2 values \n", __func__, __LINE__);
            return SR_FAILURE;
        }
    }
    gSrGlobalInfo.isLabelSpaceRsvd = SR_TRUE;
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrReserveMplsInLabel
 * Description   : This routine is us to Create MPLS Label from Range & SID
 *
 * Input(s)      : u2AddrType - Address type
 *                 sidIndexType - SID Index Type - Absolute/Relative
 *                 u4SidValue - SID Value
 * Output(s)     : None
 * Return(s)     : MPLS Label / SR_ZERO
 *****************************************************************************/
PUBLIC UINT4
SrReserveMplsInLabel (UINT2 u2AddrType, UINT1 sidIndexType, UINT4 u4SidValue)
{

    UINT4               u4Label = SR_ZERO;
    UINT4               u4LblUsedCount = SR_ZERO;

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY: SRGB->MIN %d Peer-SID %d \n", __func__,
             __LINE__, gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex,
             u4SidValue);

    if (sidIndexType == SR_SID_REL_INDEX)
    {
        if (u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            u4Label =
                gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex + u4SidValue;
            if ((u4Label < gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex)
                || (u4Label > gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex))
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d The label value to be reserved for ILM"
                         "is out of this node's SRGB range.\n", __func__,
                         __LINE__);
                return SR_FAILURE;
            }
        }
        else if (u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            u4Label =
                gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex +
                u4SidValue;

            if ((u4Label < gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex)
                || (u4Label >
                    gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MaxIndex))
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d The label value to be reserved for ILM"
                         "is out of this node's SRGB range.\n", __func__,
                         __LINE__);
                return SR_FAILURE;
            }
        }
    }
    else
    {
        u4Label = u4SidValue;
    }
    MPLS_CMN_LOCK ();
    if ((MplsCountLblUsedByInSeg (u4Label, &u4LblUsedCount)) == SNMP_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                "%s:%d Failed to Count InSegEntries using u4InLabel \n",
                __func__,__LINE__);
        MPLS_CMN_UNLOCK ();
        return SR_ZERO;
    }
    if (u4LblUsedCount == SR_ZERO)
    {
        /* Reserve In-Label from Label_manager only when u4LblUsedCount == SR_ZERO
         *  This will reserve In-Label only once*/
        if (MplsAssignLblToLblGroup (u4Label) == MPLS_FAILURE)
        {
            MPLS_CMN_UNLOCK ();
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Label (%d) Reservation Failure \n",
                     __func__, __LINE__, u4Label);
            return SR_ZERO;
        }
    }
    MPLS_CMN_UNLOCK ();

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return (u4Label);

}

/*****************************************************************************
 * Function Name : SrReleaseMplsInLabel
 * Description   : This routine is us to Create MPLS Label from Range & SID
 *
 * Input(s)      : u2AddrType - Address Type
 *                 u4SidValue - SID Value
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrReleaseMplsInLabel (UINT2 u2AddrType, UINT4 u4SidValue)
{

    UINT4               u4Label = SR_ZERO;
    UINT4               u4LblUsedCount = SR_ZERO;

    SR_TRC3 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY: Peer-SID %d \n", __func__, __LINE__,
             u4SidValue);

    if (u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        u4Label = gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex + u4SidValue;
    }
    else if (u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        u4Label =
            gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex + u4SidValue;
    }
    MPLS_CMN_LOCK ();
    if ((MplsCountLblUsedByInSeg (u4Label, &u4LblUsedCount)) == SNMP_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                "%s:%d Failed to Count InSegEntries using u4InLabel \n",
                __func__,__LINE__);
        MPLS_CMN_UNLOCK ();
        return SR_ZERO;
    }

    if (u4LblUsedCount == SR_ZERO)
    {
        /* Release In-Label to Label_manager only when Label is used by One/Last InsegEntry
         * In Sr, we're first deleteing the ILM then releasing the label, hence check for count == 0*/
        if (MplsReleaseLblToLblGroup (u4Label) == MPLS_FAILURE)
        {
            MPLS_CMN_UNLOCK ();
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Label (%d) Release Failure \n", __func__,
                     __LINE__, u4Label);
            return SR_FAILURE;
        }
        SR_TRC3 (SR_UTIL_TRC, "%s:%d Label (%d) Released \n", __func__, __LINE__,
                 u4Label);
    }
    MPLS_CMN_UNLOCK ();
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;

}

/*****************************************************************************
 * Function Name : SrGetNHForPrefix
 * Description   : This routine is used find Next-Hop for a Prefix in RTM
 *
 * Input(s)      : pSrRtrInfo - Peer LSA info containing FEC
 * Output(s)     : None
 * Return(s)     : SUCCESS/FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrGetNHForPrefix (tGenU4Addr * pDestAddr, tGenU4Addr * pNextHop,
                  UINT4 *pu4OutIfIndex)
{

    tRtInfo             InRtInfo;
    tRtInfo            *pRt = NULL;
    tRtInfo            *pTmpRt = NULL;

    UINT4               u2IncarnId = SR_ZERO;
    UINT4               u4DestMask = SR_IPV4_DEST_MASK;
#ifdef IP6_WANTED
    UINT4               u4ContextId = VCM_DEFAULT_CONTEXT;
    INT4                i4Ipv6RoutePfxLength = 128;
#endif
    tNetIpv6RtInfo      NetIpv6RtInfo;
    tIp6Addr            tmpIp6Addr;

    MEMSET (&tmpIp6Addr.u1_addr, SR_ZERO, sizeof (tIp6Addr));
    MEMSET (&NetIpv6RtInfo, SR_ZERO, sizeof (tNetIpv6RtInfo));
    MEMSET (&InRtInfo, SR_ZERO, sizeof (tRtInfo));

    if (pDestAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_TRC3 (SR_UTIL_TRC, "%s:%d ENTRY with DestAddr %x \n", __func__,
                 __LINE__, pDestAddr->Addr.u4Addr);

        InRtInfo.u4DestNet = pDestAddr->Addr.u4Addr;
        InRtInfo.u4DestMask = u4DestMask;

        SR_TRC3 (SR_UTIL_TRC, "%s:%d Finding Nh for FEC %x \n", __func__, __LINE__,
                 pDestAddr->Addr.u4Addr);
        RtmApiGetBestRouteEntryInCxt (u2IncarnId, InRtInfo, &pRt);

        pTmpRt = pRt;

        if (pTmpRt != NULL)
        {
            SR_TRC4 (SR_UTIL_TRC, "%s:%d ROUTE: NH %x u4OutIfIndex %d \n",
                     __func__, __LINE__, pTmpRt->u4NextHop, pTmpRt->u4RtIfIndx);
        }

#ifdef MPLS_SR_ECMP_WANTED
        for (; ((pTmpRt != NULL) && (pTmpRt->i4Metric1 == pRt->i4Metric1));
             pTmpRt = pTmpRt->pNextAlternatepath)
#else
        if (pTmpRt != NULL)
#endif
        {
            MEMCPY (&pNextHop->Addr.u4Addr, &pTmpRt->u4NextHop, sizeof (UINT4));
            pNextHop->u2AddrType = SR_IPV4_ADDR_TYPE;
            if (pNextHop->Addr.u4Addr == SR_ZERO)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d NO ROUTE Found (NH=0) FEC %x \n",
                         __func__, __LINE__, pDestAddr->Addr.u4Addr);
                return SR_FAILURE;
            }

            if (NetIpv4GetCfaIfIndexFromPort (pTmpRt->u4RtIfIndx,
                                              pu4OutIfIndex) == NETIPV4_FAILURE)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d No Port is mapped with u4RtIfIndx %d \n", __func__,
                         __LINE__, pTmpRt->u4RtIfIndx);
                return SR_FAILURE;
            }

            SR_TRC4 (SR_UTIL_TRC,
                     "%s:%d ROUTE Found with NH %x u4OutIfIndex %d \n", __func__,
                     __LINE__, pNextHop->Addr.u4Addr, *pu4OutIfIndex);
#ifdef MPLS_SR_ECMP_WANTED
            break;
#endif
        }

        if (pTmpRt == NULL)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d NO ROUTE Found FEC %x \n",
                     __func__, __LINE__, pDestAddr->Addr.u4Addr);
            return SR_FAILURE;
        }
    }
#ifdef IP6_WANTED
    else if (pDestAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        if (Rtm6ApiGetBestRouteEntryInCxt
            (u4ContextId, &(pDestAddr->Addr.Ip6Addr),
             (UINT1) i4Ipv6RoutePfxLength, SR_ZERO,
             &NetIpv6RtInfo) != RTM6_FAILURE)
        {
            if (MEMCMP
                (NetIpv6RtInfo.NextHop.u1_addr, tmpIp6Addr.u1_addr,
                 SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d NH=0 \n", __func__,
                        __LINE__);
                return SR_FAILURE;
            }

            MEMCPY (pNextHop->Addr.Ip6Addr.u1_addr,
                    NetIpv6RtInfo.NextHop.u1_addr, SR_IPV6_ADDR_LENGTH);
            pNextHop->u2AddrType = SR_IPV6_ADDR_TYPE;
            if (NetIpv6RtInfo.u4Index != SR_ZERO)
            {
                if (NetIpv6GetCfaIfIndexFromPort
                    (NetIpv6RtInfo.u4Index, pu4OutIfIndex) == NETIPV4_FAILURE)
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d No Port to IfIndex Mapping \n", __func__,
                            __LINE__);
                    return SR_FAILURE;
                }
            }
            else
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Zero Interface Index \n", __func__,
                        __LINE__);
                return SR_FAILURE;
            }
        }
        else
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d No route available for destination \n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

    }
#endif
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrMplsCreateFTN
 * Description   : This routine is used to create MPLS FTN entry for a Prefix
 *
 * Input(s)      : pSrRtrInfo - Peer LSA info for which FTN is to be created
 * Output(s)     : None
 * Return(s)     : SUCCESS/FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsCreateFTN (tSrRtrInfo * pSrRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo)
{
    tSrRtrInfo * pNbrRtrInfo;
    UINT4        u4MplsTnlIfIndex = SR_ZERO;
    UINT4        u4L3VlanIf       = SR_ZERO;
    UINT2        u2MlibOperation  = MPLS_MLIB_FTN_CREATE;

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY for Rtr %x, AdvRtr %x\n",
            __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
            pSrRtrInfo->advRtrId.Addr.u4Addr);

    /*Check if creation is blocked because of GR*/
    if (gSrGlobalInfo.u1IsBlocked)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_CRITICAL_TRC, "%s:%d Returning since blocked for GR\n",
                 __func__, __LINE__);
        return SR_SUCCESS;
    }

    if (pSrRtrInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d FAILURE: pSrRtrInfo == NULL \n", __func__,
                 __LINE__);
        return SR_FAILURE;
    }
    if(pSrRtrNextHopInfo == NULL)
    {
       SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d FAILURE: pSrRtrNextHopInfo == NULL \n", __func__,
                 __LINE__);
        return SR_FAILURE;
    }
    /* Check introduced as in some scenarios, SID Value is never checked & FTN can get created
     * with SRGB_Min of Next-Hop*/
    if (pSrRtrInfo->u4SidValue == SR_ZERO)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d FAILURE: pSrRtrInfo->u4SidValue == 0 \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }

    if (pSrRtrInfo->u1SIDConflictWin == SR_CONFLICT_NOT_WINNER)
    {
        SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC, "%s:%d Returning since NOT_WINNER for %x \n",
                 __func__, __LINE__, pSrRtrInfo->advRtrId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    if (pSrRtrInfo->u1PrefixConflictMPLSDel == SR_TRUE)
    {
        SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC, "%s:%d Returning since Prefix_Conflict for %x \n",
                 __func__, __LINE__, pSrRtrInfo->advRtrId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    if (pSrRtrInfo->u1OutOfRangeAlarm == SR_ALARM_RAISE)
    {
        SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC, "%s:%d Returning since OOR Alarm for %x \n",
                 __func__, __LINE__, pSrRtrInfo->advRtrId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    if((pSrRtrNextHopInfo->u1MPLSStatus & SR_ILM_POP_CREATED) ==
            SR_ILM_POP_CREATED)
    {

        pNbrRtrInfo = SrGetRtrInfoFromNextHop (&(pSrRtrNextHopInfo->nextHop));
        if (NULL == pNbrRtrInfo)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC, "%s:%d Returning as Nbr Sr Info NULL and POP entry already created for %x\n",
                             __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
            return SR_SUCCESS;
        }


        if (SrMplsDeleteILMPopAndFwd(pSrRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsDeleteILMPopAndFwd Failed\n",
                 __func__, __LINE__);
            return SR_FAILURE;
        }
        else
        {
            SR_TRC2 (SR_UTIL_TRC | SR_CRITICAL_TRC, "%s:%d SrMplsDeleteILMPopAndFwd Success\n",
                 __func__, __LINE__);
        }
    }

#ifdef CFA_WANTED
    /*pSrRtrInfo->u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */

        if (SrFtnCfaIfmCreateStackMplsTunnelInterface (pSrRtrInfo->prefixId.Addr.u4Addr,
            pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
            pSrRtrNextHopInfo->u4OutIfIndex,
            &u4MplsTnlIfIndex) == SR_SUCCESS)
        {
            SR_TRC6 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d prefix = %x nexthop = %x OutIfIndex = %d TnlIfIndex %d : SR_SUCCESS \n",
                     __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
            pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
            pSrRtrNextHopInfo->u4OutIfIndex,
            u4MplsTnlIfIndex);
            pSrRtrNextHopInfo->u4OutIfIndex = u4MplsTnlIfIndex;
        }
        else
        {
            SR_TRC6 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d FAILURE: prefix = %x nexthop = %x OutIfIndex = %d: TnlIfIndex %d : SR_FAILURE \n",
                     __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
                     pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                     pSrRtrNextHopInfo->u4OutIfIndex,
                     u4MplsTnlIfIndex);
            return SR_FAILURE;
        }

#endif

    /*Populate LspInfo with respect to MlibOperation & Create MPLS FTN */
    if (SrMplsCreateOrDeleteLsp
        (u2MlibOperation, pSrRtrInfo, NULL, pSrRtrNextHopInfo) == SR_SUCCESS)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                 "%s:%d u2MlibOperation = MPLS_MLIB_FTN_CREATE; SrMplsCreateOrDeleteLsp: SR_SUCCESS \n",
                 __func__, __LINE__);
        /*LFA route */
    }

    else
    {
        /* Delete all the resources reserved  on FAILURE & reset OutIfIndex */
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d SrMplsCreateOrDeleteLsp SR_FAILURE \n",
                 __func__, __LINE__);
#ifdef CFA_WANTED
        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {

            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d FAILURE: MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                         __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }
            pSrRtrNextHopInfo->u4OutIfIndex = u4L3VlanIf;
        }

#endif
        return SR_FAILURE;
    }

    SR_TRC2 (SR_UTIL_TRC,
             "%s:%d pSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED \n",
             __func__, __LINE__);
    pSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;

    pSrRtrInfo->u1SIDStatus = SR_SID_STATUS_UP;
    SrStaticSetDynamicNextHop(pSrRtrInfo, SR_SID_STATUS_UP);

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrMplsCreateILM
 * Description   : This routine is used to create MPLS ILM (POP or SWAP) entry
 *
 * Input(s)      : pSrInSegInfo - In-mapping Info to be created
 *                 pSrRtrInfo - Peer LSA info for which FTN is to be created
 * Output(s)     : None
 * Return(s)     : SUCCESS/FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsCreateILM (tSrRtrInfo * pSrRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo)
{
    tSrInSegInfo        SrInSegInfo;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    UINT2               u2MlibOperation = MPLS_MLIB_ILM_CREATE;

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY for Rtr %x, AdvRtr %x\n",
            __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
            pSrRtrInfo->advRtrId.Addr.u4Addr);

    /*Check if creation is blocked because of GR*/
    if (gSrGlobalInfo.u1IsBlocked)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_CRITICAL_TRC, "%s:%d Returning since blocked for GR\n",
                 __func__, __LINE__);
        return SR_SUCCESS;
    }

    if (pSrRtrInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pSrRtrInfo == NULL \n", __func__, __LINE__);
        return SR_FAILURE;
    }

    if (pSrRtrInfo->u4SidValue == SR_ZERO)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d: FAILURE: pSrRtrInfo->u4SidValue == 0 \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }

    if (pSrRtrInfo->u1SIDConflictWin == SR_CONFLICT_NOT_WINNER)
    {
        SR_TRC3 (SR_UTIL_TRC, "%s:%d: Returning since NOT_WINNER for %x \n",
                 __func__, __LINE__, pSrRtrInfo->advRtrId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    if (pSrRtrInfo->u1PrefixConflictMPLSDel == SR_TRUE)
    {
        SR_TRC3 (SR_UTIL_TRC, "%s:%d Returning since PrefixConflictMPLSDel for %x \n",
                 __func__, __LINE__, pSrRtrInfo->advRtrId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    if (pSrRtrInfo->u1OutOfRangeAlarm == SR_ALARM_RAISE)
    {
        SR_TRC3 (SR_UTIL_TRC, "%s:%d Returning since OOR Alarm for %x \n",
                 __func__, __LINE__, pSrRtrInfo->advRtrId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    if((pSrRtrNextHopInfo->u1MPLSStatus & SR_ILM_POP_CREATED) ==
            SR_ILM_POP_CREATED)
    {
        SR_TRC3 (SR_CRITICAL_TRC, "%s:%d Returing since ILM POP Entry created for %x \n",
                         __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    SrInSegInfo.u4InLabel =
        SrReserveMplsInLabel (pSrRtrInfo->u2AddrType,
                              pSrRtrInfo->u1SidIndexType,
                              pSrRtrInfo->u4SidValue);

    SR_TRC3 (SR_UTIL_TRC, "%s:%d SrInSegInfo.u4InLabel == %d \n", __func__,
             __LINE__, SrInSegInfo.u4InLabel);
    if (SrInSegInfo.u4InLabel == SR_ZERO)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrInSegInfo.u4InLabel == SR_ZERO \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }

#ifdef CFA_WANTED
    /* In case of SR - TE, skip the MPLS create interface in hardware */
    if ((pSrRtrNextHopInfo != NULL) && (pSrRtrInfo->u1TeSwapStatus != TRUE))
    {
        /*pSrRtrInfo->u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
        if (SrIlmCfaIfmCreateStackMplsTunnelInterface
            (pSrRtrInfo->prefixId.Addr.u4Addr,
             pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
             pSrRtrNextHopInfo->u4SwapOutIfIndex,
             &u4MplsTnlIfIndex) == SR_SUCCESS)
        {
            SR_TRC6 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d prefix = %x nexthop = %x SwapOutIfIndex = %d u4MplsTnlIfIndex %d : SR_SUCCESS \n",
                     __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
                     pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                     pSrRtrNextHopInfo->u4SwapOutIfIndex,
                     u4MplsTnlIfIndex);

            pSrRtrNextHopInfo->u4SwapOutIfIndex = u4MplsTnlIfIndex;
        }
        else
        {
            SR_TRC6 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d prefix = %x nexthop = %x SwapOutIfIndex = %d u4MplsTnlIfIndex %d : SR_FAILURE \n",
                     __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
                     pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                     pSrRtrNextHopInfo->u4SwapOutIfIndex,
                     u4MplsTnlIfIndex);
            return SR_FAILURE;
        }
    }
#endif

    if (SrMplsCreateOrDeleteLsp
        (u2MlibOperation, pSrRtrInfo, &SrInSegInfo,
         pSrRtrNextHopInfo) == SR_SUCCESS)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                 "%s:%d u2MlibOperation = MPLS_MLIB_ILM_CREATE; SrMplsCreateOrDeleteLsp: SR_SUCCESS \n",
                 __func__, __LINE__);
        if ((pSrRtrInfo->u1TeSwapStatus != TRUE) && (pSrRtrNextHopInfo != NULL))
        {
            if (pSrRtrNextHopInfo->bIsLfaNextHop == SR_TRUE)
            {
                pSrRtrInfo->u4LfaInLabel = SrInSegInfo.u4InLabel;
                pSrRtrInfo->u4LfaSwapOutIfIndex =
                    pSrRtrNextHopInfo->u4SwapOutIfIndex;
                SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                "%s:%d pSrRtrInfo->u4LfaInLabel= %d pSrRtrNextHopInfo->u4SwapOutIfIndex= %d \n",
                 __func__, __LINE__,pSrRtrInfo->u4LfaInLabel,pSrRtrNextHopInfo->u4SwapOutIfIndex);

            }
        }
    }
    else
    {
        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d u2MlibOperation = MPLS_MLIB_ILM_CREATE; SrMplsCreateOrDeleteLsp: SR_FAILURE \n",
                 __func__, __LINE__);
        if (SrReleaseMplsInLabel
            (pSrRtrInfo->u2AddrType, pSrRtrInfo->u4SidValue) == SR_FAILURE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d Unable to Release In-Label \n", __func__,
                     __LINE__);
            return SR_FAILURE;
        }

#ifdef CFA_WANTED
        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {

            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                         __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }
            pSrRtrNextHopInfo->u4SwapOutIfIndex = u4L3VlanIf;
        }
#endif

        return SR_FAILURE;
    }

    pSrRtrInfo->u4LfaInLabel = SrInSegInfo.u4InLabel;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrMplsDeleteFTN
 * Description   : This routine is used to delete MPLS FTN entry for a Prefix
 *
 * Input(s)      : pSrRtrInfo - Peer LSA info for which FTN is to be deleted
 * Output(s)     : None
 * Return(s)     : SUCCESS/FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsDeleteFTN (tSrRtrInfo * pSrRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo)
{
    UINT2               u2MlibOperation = MPLS_MLIB_FTN_DELETE;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY for Rtr %x, AdvRtr %x\n",
            __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
            pSrRtrInfo->advRtrId.Addr.u4Addr);

    /*Check if creation is blocked because of GR*/
    if (gSrGlobalInfo.u1IsBlocked)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_CRITICAL_TRC, "%s:%d Returning since blocked for GR\n",
                 __func__, __LINE__);
        return SR_SUCCESS;
    }

    if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) != SR_FTN_CREATED)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d FTN Not Created: EXIT \n", __func__,
                 __LINE__);
        return SR_SUCCESS;
    }

    if (pSrRtrInfo->u1SIDConflictWin == SR_CONFLICT_NOT_WINNER)
    {
        SR_TRC3 (SR_UTIL_TRC, "%s:%d: Returning since NOT_WINNER for %x \n",
                 __func__, __LINE__, pSrRtrInfo->advRtrId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    pSrRtrInfo->u1SIDStatus = SR_SID_STATUS_DOWN;
    SrStaticSetDynamicNextHop(pSrRtrInfo, SR_SID_STATUS_DOWN);

    /*Populate LspInfo with respect to MLibOperation */
    if (SrMplsCreateOrDeleteLsp
        (u2MlibOperation, pSrRtrInfo, NULL, pSrRtrNextHopInfo) == SR_SUCCESS)
    {
        SR_TRC2 (SR_UTIL_TRC,
                 "%s:%d u2MlibOperation = MPLS_MLIB_FTN_DELETE; SrMplsCreateOrDeleteLsp: SR_SUCCESS \n",
                 __func__, __LINE__);
    }

#ifdef CFA_WANTED

    u4MplsTnlIfIndex = pSrRtrNextHopInfo->u4OutIfIndex;

    if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
        != CFA_FAILURE)
    {
        if (SrFtnCfaIfmDeleteStackMplsTunnelInterface
            (pSrRtrInfo->prefixId.Addr.u4Addr, pSrRtrNextHopInfo->nextHop.Addr.u4Addr, u4L3VlanIf, u4MplsTnlIfIndex) == SR_FAILURE)
        {
            SR_TRC6 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d Prefix: %x NextHop: %x MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                     __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr, pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                     u4MplsTnlIfIndex, u4L3VlanIf);
            return SR_FAILURE;
        }
    }

    pSrRtrNextHopInfo->u4OutIfIndex = SR_ZERO;
    if ((pSrRtrInfo->u1SIDConflictMPLSDel == SR_TRUE)
         || (pSrRtrInfo->u1PrefixConflictMPLSDel == SR_TRUE))
    {
        pSrRtrNextHopInfo->u4OutIfIndex = u4L3VlanIf;
        SR_TRC4 (SR_UTIL_TRC, "%s:%d: Rtr %x, Reassign u4OutIfIndex=%d \n",
                 __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr, u4L3VlanIf);
    }
#endif
    /* Setting MPLS Status to SR_FTN_NOT_CREATED by
     * unsetting the FTN_CREATED Bit*/
    SR_TRC2 (SR_UTIL_TRC,
             "%s:%d pSrRtrNextHopInfo->u1MPLSStatus &= ~SR_FTN_CREATED \n",
             __func__, __LINE__);
    pSrRtrNextHopInfo->u1MPLSStatus &= (UINT1) (~SR_FTN_CREATED);

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrMplsDeleteILM
 * Description   : This routine is used to delete MPLS ILM (POP or SWAP) entry
 *
 * Input(s)      : pSrInSegInfo - In-mapping Info to be deleted
 *
 *                 pSrRtrInfo - Peer LSA info for which FTN is to be deleted
 *
 * Output(s)     : None
 * Return(s)     : SUCCESS/FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsDeleteILM (tSrInSegInfo * pSrInSegInfo, tSrRtrInfo * pSrRtrInfo,
                 tSrRtrNextHopInfo * pSrRtrNextHopInfo)
{
    UINT2               u2MlibOperation = MPLS_MLIB_ILM_DELETE;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY for Rtr %x, AdvRtr %x\n",
            __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
            pSrRtrInfo->advRtrId.Addr.u4Addr);

    /*Check if creation is blocked because of GR*/
    if (gSrGlobalInfo.u1IsBlocked)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_CRITICAL_TRC, "%s:%d Returning since blocked for GR\n",
                 __func__, __LINE__);
        return SR_SUCCESS;
    }

    if (pSrRtrInfo->u1SIDConflictWin == SR_CONFLICT_NOT_WINNER)
    {
        SR_TRC3 (SR_UTIL_TRC, "%s:%d: Returning since NOT_WINNER for %x \n",
                 __func__, __LINE__, pSrRtrInfo->advRtrId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    if ((pSrRtrNextHopInfo != NULL) &&
        ((pSrRtrNextHopInfo->u1MPLSStatus & SR_ILM_POP_CREATED) ==
         SR_ILM_POP_CREATED))
    {
        if (SrMplsDeleteILMPopAndFwd
            (pSrRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d SrMplsDeleteILMPopAndFwd Failed\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        else
        {
            SR_TRC2 (SR_UTIL_TRC, "%s:%d SrMplsDeleteILMPopAndFwd Success\n",
                     __func__, __LINE__);
            return SR_SUCCESS;
        }
    }

    /*This structure will contain SR-Info about this Node/Router */
    if (pSrInSegInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pSrInSegInfo == NULL \n", __func__, __LINE__);
        return SR_FAILURE;
    }

    if (SrMplsCreateOrDeleteLsp
        (u2MlibOperation, pSrRtrInfo, pSrInSegInfo,
         pSrRtrNextHopInfo) == SR_SUCCESS)
    {
        SR_TRC2 (SR_UTIL_TRC,
                 "%s:%d u2MlibOperation = MPLS_MLIB_ILM_DELETE; SrMplsCreateOrDeleteLsp: SR_SUCCESS \n",
                 __func__, __LINE__);
    }

#ifdef CFA_WANTED
    if ((pSrRtrNextHopInfo != NULL) && (pSrRtrInfo->u1TeSwapStatus != TRUE))
    {
        u4MplsTnlIfIndex = pSrRtrNextHopInfo->u4SwapOutIfIndex;
        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {
            if (SrIlmCfaIfmDeleteStackMplsTunnelInterface
                (pSrRtrInfo->prefixId.Addr.u4Addr, pSrRtrNextHopInfo->nextHop.Addr.u4Addr, u4L3VlanIf, u4MplsTnlIfIndex) == SR_FAILURE)
            {
                SR_TRC6 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d Prefix: %x NextHop: %x MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                         __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr, pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                         u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }
            if ((pSrRtrNextHopInfo->u1FRRNextHop == SR_LFA_NEXTHOP) ||
                (SR_RLFA_NEXTHOP == pSrRtrNextHopInfo->u1FRRNextHop))
            {
                pSrRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
            }
        }
        pSrRtrNextHopInfo->u4SwapOutIfIndex = SR_ZERO;
        if ((pSrRtrInfo->u1SIDConflictMPLSDel == SR_TRUE)
            || (pSrRtrInfo->u1PrefixConflictMPLSDel == SR_TRUE))
        {
            pSrRtrNextHopInfo->u4SwapOutIfIndex = u4L3VlanIf;
            SR_TRC4 (SR_UTIL_TRC, "%s:%d: Rtr %x, Reassign u4OutIfIndex=%d \n",
                     __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
                     u4L3VlanIf);
        }
    }
#endif

    if (SrReleaseMplsInLabel (pSrRtrInfo->u2AddrType,
                              pSrRtrInfo->u4SidValue) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Unable to Release In-Label \n", __func__,
                 __LINE__);
        return SR_FAILURE;
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/******************************************************************************
 *
 *    Function Name      : SrFtnCfaIfmCreateStackMplsTunnelInterface
 *
 *    Description        : This function creates the MPLS Tunnel Interface for
 *                         FTN in the ifTable and creates a stack entry for
 *                         mpls and l3ipvlan interface in the stack table and
 *                         layers mpls interface on the vlan interface
 *
 *    Input(s)           : UINT4 prefix - L3IPVLAN or MPLS-Tunnel If
 *                         UINT4 nexthop
 *                         UINT4 u4IfIndex
 *
 *    Output(s)          : pu4MplsTnlIfIndex   - IfIndex of created MPLS-TUNNEL
 *
 *    Global Variables Referred : SrGlobalTable (gSrGlobalInfo) structure.
 *
 *    Global Variables Modified : SrGlobalTable (gSrGlobalInfo) structure.
 *
 *    Exceptions or Operating
 *    System Error Handling    : None.
 *
 *    Use of Recursion        : None.
 *
 *    Returns            : SR_SUCCESS if interface creation and stacking
 *                         succeeds
 *                         CFA_FAILURE otherwise.
 *
 *****************************************************************************/
INT4
SrFtnCfaIfmCreateStackMplsTunnelInterface (UINT4 prefix, UINT4 nexthop,
                                           UINT4 u4IfIndex, UINT4 *pu4MplsTnlIfIndex)
{
    SR_TRC2 (SR_CRITICAL_TRC, "ENTRY: %s:%d \n", __func__, __LINE__);
    SR_TRC2 (SR_CRITICAL_TRC, "Prefix: %x, NextHop: %x\n", prefix, nexthop);

    tSrTnlIfTable *pTnlDbEntry=NULL;

    if(prefix == 0)
    {
        SR_TRC1(SR_FAIL_TRC,"ERROR: Invalid Prefix Ip address  %x,\n",prefix);
        return SR_FAILURE;
    }

    /** Get the tnl id from local DB and give it here - pu4MplsTnlIfIndex **/
    if (SrUtlCheckTnlInTnlDbTable (SR_FTN_TNL_INFO_TABLE, prefix,
                                   nexthop, &pTnlDbEntry) == SR_SUCCESS)
    {
        *pu4MplsTnlIfIndex = pTnlDbEntry->u4SrTnlIfIndex;
        pTnlDbEntry->u1stale = SR_FALSE;

        SR_TRC4(SR_CRITICAL_TRC,"Found Tunnel Index in SR_FTN_TNL_INFO_TABLE, prefix %x, nexthop %x, u4IfIndex %d, pu4MplsTnlIfIndex %d\n",
                                 prefix, nexthop, u4IfIndex, *pu4MplsTnlIfIndex);
        return SR_SUCCESS;
    }

    /** if the entry is already present in DB then just create the mpls tunnel and return **/
    if (CfaIfmCreateStackMplsTunnelInterface(u4IfIndex,
        pu4MplsTnlIfIndex) == CFA_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                 "%s:%d uIfIndex = %d: pu4MplsTnlIfIndex %d : CFA_SUCCESS\n",
                 __func__, __LINE__, u4IfIndex,
                 *pu4MplsTnlIfIndex);
    }
    else
    {
        SR_TRC4(SR_FAIL_TRC,
                "%s:%d Failure: uIfIndex = %d: pu4MplsTnlIfIndex %d : CFA_FAILURE\n",
                __func__, __LINE__, u4IfIndex,
                *pu4MplsTnlIfIndex);
        return SR_FAILURE;
    }

    /* As of now, let the tunnel entry to store in the persistent DB even if it is already existing.
       Later it is not required if data is persistent across all reboot
       Store TunnelIfIndex in Stub-DB whenever MPLSTunnelIfIndex is created
     */

    if(SR_ALLOCATE_FTN_TNL_INFO_MEM_BLOCK(pTnlDbEntry) == NULL)
    {
        SR_TRC(SR_FAIL_TRC, "FTN Memory Allocation Failure\n");
        return SR_FAILURE;
    }

    pTnlDbEntry->u4Prefix = prefix;
    pTnlDbEntry->u4NextHop = nexthop;
    pTnlDbEntry->u4SrTnlIfIndex = *pu4MplsTnlIfIndex;
    pTnlDbEntry->u1stale = SR_FALSE;

    SrFTNDbStoreTnlInfo (pTnlDbEntry);

    if (SR_MEM_FAILURE == SR_FTN_TNL_INFO_MEM_FREE(pTnlDbEntry))
    {
        SR_TRC (SR_FAIL_TRC,"ERROR! Failed to release FTN entry \n");
        return SR_FAILURE;
    }

    SR_TRC2 (SR_CRITICAL_TRC, "EXIT: %s:%d SUCCESS \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/******************************************************************************
 *
 *    Function Name      : SrIlmCfaIfmCreateStackMplsTunnelInterface
 *
 *    Description        : This function creates the MPLS Tunnel Interface for
 *                         ILM in the ifTable and creates a stack entry for
 *                         mpls and l3ipvlan interface in the stack table and
 *                         layers mpls interface on the vlan interface
 *
 *    Input(s)           : UINT4 prefix - L3IPVLAN or MPLS-Tunnel If
 *                         UINT4 nexthop
 *                         UINT4 u4IfIndex
 *
 *    Output(s)          : pu4MplsTnlIfIndex   - IfIndex of created MPLS-TUNNEL
 *
 *    Global Variables Referred : SrGlobalTable (gSrGlobalInfo) structure.
 *
 *    Global Variables Modified : SrGlobalTable (gSrGlobalInfo) structure.
 *
 *    Exceptions or Operating
 *    System Error Handling    : None.
 *
 *    Use of Recursion        : None.
 *
 *    Returns            : SR_SUCCESS if interface creation and stacking
 *                         succeeds
 *                         CFA_FAILURE otherwise.
 *
 *****************************************************************************/
INT4
SrIlmCfaIfmCreateStackMplsTunnelInterface (UINT4 prefix, UINT4 nexthop,
                                           UINT4 u4IfIndex, UINT4 *pu4MplsTnlIfIndex)
{
    SR_TRC2 (SR_CRITICAL_TRC, "ENTRY: %s:%d \n", __func__, __LINE__);
    SR_TRC2 (SR_CRITICAL_TRC, "Prefix: %x, NextHop: %x\n", prefix, nexthop);

    tSrTnlIfTable *pTnlDbEntry = NULL;

    if(prefix == 0)
    {
        SR_TRC1(SR_FAIL_TRC,"ERROR: Invalid Prefix Ip address %x\n",prefix);
        return SR_FAILURE;
    }

    /** Get the tnl id from local DB and give it here - pu4MplsTnlIfIndex **/
    if (SrUtlCheckTnlInTnlDbTable (SR_ILM_TNL_INFO_TABLE, prefix,
                                   nexthop, &pTnlDbEntry) == SR_SUCCESS)
    {
        *pu4MplsTnlIfIndex = pTnlDbEntry->u4SrTnlIfIndex;
        pTnlDbEntry->u1stale = SR_FALSE;

        SR_TRC4(SR_CRITICAL_TRC,"Found Tunnel Index in SR_ILM_TNL_INFO_TABLE, prefix %x, nexthop %x, u4IfIndex %d, pu4MplsTnlIfIndex %d\n",
                                 prefix, nexthop, u4IfIndex, *pu4MplsTnlIfIndex);
        return SR_SUCCESS;
    }

    /** if the entry is already present in DB then just create the mpls tunnel and return **/
    if (CfaIfmCreateStackMplsTunnelInterface(u4IfIndex,
        pu4MplsTnlIfIndex) == CFA_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                 "%s:%d uIfIndex = %d: pu4MplsTnlIfIndex %d : CFA_SUCCESS",
                 __func__, __LINE__, u4IfIndex,
                 *pu4MplsTnlIfIndex);
    }
    else
    {
        SR_TRC4(SR_FAIL_TRC,
                "%s:%d Failure: uIfIndex = %d: pu4MplsTnlIfIndex %d : CFA_FAILURE",
                __func__, __LINE__, u4IfIndex,
                *pu4MplsTnlIfIndex);
        return SR_FAILURE;
    }

    /* As of now, let the tunnel entry to store in the persistent DB even if it is already existing.
       Later it is not required if data is persistent across all reboot
       Store TunnelIfIndex in Stub-DB whenever MPLSTunnelIfIndex is created
     */

    if(SR_ALLOCATE_ILM_TNL_INFO_MEM_BLOCK(pTnlDbEntry) == NULL)
    {
        SR_TRC(SR_FAIL_TRC, "ILM Memory Allocation Failure\n");
        return SR_FAILURE;
    }

    pTnlDbEntry->u4Prefix = prefix;
    pTnlDbEntry->u4NextHop = nexthop;
    pTnlDbEntry->u4SrTnlIfIndex = *pu4MplsTnlIfIndex;
    pTnlDbEntry->u1stale = SR_FALSE;

    SrILMDbStoreTnlInfo (pTnlDbEntry);

    if (SR_MEM_FAILURE == SR_ILM_TNL_INFO_MEM_FREE(pTnlDbEntry))
    {
        SR_TRC (SR_FAIL_TRC,"ERROR! Failed to release ILM entry \n");
        return SR_FAILURE;
    }

    SR_TRC2 (SR_CRITICAL_TRC, "EXIT: %s:%d SUCCESS\n", __func__, __LINE__);
    return SR_SUCCESS;
}

/******************************************************************************
 *
 *    Function Name      : SrFtnCfaIfmDeleteStackMplsTunnelInterface
 *
 *    Description        : This function deletes the MPLS Tunnel Interface for
 *                         FTN in the ifTable and deletes stack entry for
 *                         mpls and l3ipvlan interface in the stack table and
 *                         layers mpls interface on the vlan interface
 *
 *    Input(s)           : UINT4 prefix - L3IPVLAN or MPLS-Tunnel If
 *                         UINT4 nexthop
 *                         UINT4 u4IfIndex
 *
 *    Output(s)          : pu4MplsTnlIfIndex   - IfIndex of created MPLS-TUNNEL
 *
 *    Global Variables Referred : SrGlobalTable (gSrGlobalInfo) structure.
 *
 *    Global Variables Modified : SrGlobalTable (gSrGlobalInfo) structure.
 *
 *    Exceptions or Operating
 *    System Error Handling    : None.
 *
 *    Use of Recursion        : None.
 *
 *    Returns            : SR_SUCCESS if interface creation and stacking
 *                         succeeds
 *                         CFA_FAILURE otherwise.
 *
 *****************************************************************************/
INT4
SrFtnCfaIfmDeleteStackMplsTunnelInterface (UINT4 prefix, UINT4 nexthop,
                                           UINT4 u4IfIndex, UINT4 u4MplsTnlIfIndex)
{
    SR_TRC2 (SR_CRITICAL_TRC, "ENTRY: %s:%d \n", __func__, __LINE__);
    SR_TRC2 (SR_CRITICAL_TRC, "Prefix: %x, NextHop: %x\n", prefix, nexthop);

    tSrTnlIfTable *pTnlDbEntry = NULL;
    tSrTnlIfTable srTnlTable;

    MEMSET (&srTnlTable, SR_ZERO, sizeof (tSrTnlIfTable));

    if(prefix == 0)
    {
        SR_TRC1(SR_FAIL_TRC,"ERROR: Unable to fetch the Prefix Ip address from CfaIndex %d\n", u4IfIndex);
        return SR_FAILURE;
    }

    /* 1. Notify CFA to release the Tnl-If-Index */
    if (CfaIfmDeleteStackMplsTunnelInterface
            (u4IfIndex, u4MplsTnlIfIndex) == CFA_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                "%s:%d uIfIndex = %d: u4MplsTnlIfIndex %d : Tunnel-If-Index is deleted by CFA\n",
                __func__, __LINE__, u4IfIndex, u4MplsTnlIfIndex);
    }
    else
    {
        SR_TRC4 (SR_FAIL_TRC,
                "%s:%d CFA not releasing Tnl-If-Indx. uIfIndex = %d: u4MplsTnlIfIndex %d\n",
                __func__, __LINE__, u4IfIndex, u4MplsTnlIfIndex);
        return SR_FAILURE;
    }

    /* As of now, let the tunnel entry to store in the persistent DB even if it is already existing.
       Later it is not required if data is persistent across all reboot
       Store TunnelIfIndex in Stub-DB whenever MPLSTunnelIfIndex is deleted
     */

    srTnlTable.u4Prefix       = prefix;
    srTnlTable.u4NextHop      = nexthop;
    srTnlTable.u4SrTnlIfIndex = u4MplsTnlIfIndex;

    /* 2. Delete the entry from persistent DB */
    SrDbDelFTNTnlInfoFromDB (prefix, nexthop);

    /* 3. Delete the Tnl-If-Index entry from the RB tree */
    if(gSrGlobalInfo.u1RebootMode == SR_WARM_REBOOT)
    {
        pTnlDbEntry = RBTreeGet (gSrGlobalInfo.SrFTNTnlDbTree,(tRBElem *) &srTnlTable);

        if(pTnlDbEntry == NULL)
        {
            SR_TRC5 (SR_FAIL_TRC, "%s:%d Entry not found in RB tree u4Prefix: %x u4NextHop: %x u4SrTnlIfindex: %d\n",
            __func__, __LINE__,srTnlTable.u4Prefix, srTnlTable.u4NextHop, srTnlTable.u4SrTnlIfIndex);
        }
        else
        {
            if (RBTreeRem (gSrGlobalInfo.SrFTNTnlDbTree, pTnlDbEntry) == NULL)
            {
                SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to remove FTN entry from RB Tree\n", __func__, __LINE__);
                return SR_FAILURE;
            }

            if (SR_MEM_FAILURE == SR_FTN_TNL_INFO_MEM_FREE(pTnlDbEntry))
            {
                SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to release FTN entry memory\n", __func__, __LINE__);
                return SR_FAILURE;
            }
        }
    }

    SR_TRC2 (SR_CRITICAL_TRC, "EXIT:%s:%d SUCCESS\n", __func__, __LINE__);
    return SR_SUCCESS;
}

/******************************************************************************
 *
 *    Function Name      : SrIlmCfaIfmDeleteStackMplsTunnelInterface
 *
 *    Description        : This function deletes the MPLS Tunnel Interface for
 *                         ILM in the ifTable and deletes stack entry for
 *                         mpls and l3ipvlan interface in the stack table and
 *                         layers mpls interface on the vlan interface
 *
 *    Input(s)           : UINT4 prefix - L3IPVLAN or MPLS-Tunnel If
                           UINT4 nexthop
 *                         UINT4 u4IfIndex
 *
 *    Output(s)          : pu4MplsTnlIfIndex   - IfIndex of created MPLS-TUNNEL
 *
 *    Global Variables Referred : SrGlobalTable (gSrGlobalInfo) structure.
 *
 *    Global Variables Modified : SrGlobalTable (gSrGlobalInfo) structure.
 *
 *    Exceptions or Operating
 *    System Error Handling    : None.
 *
 *    Use of Recursion        : None.
 *
 *    Returns            : SR_SUCCESS if interface creation and stacking
 *                         succeeds
 *                         CFA_FAILURE otherwise.
 *
 *****************************************************************************/
INT4
SrIlmCfaIfmDeleteStackMplsTunnelInterface (UINT4 prefix, UINT4 nexthop,
                                           UINT4 u4IfIndex, UINT4 u4MplsTnlIfIndex)
{
    SR_TRC2 (SR_CRITICAL_TRC, "ENTRY:%s:%d \n", __func__, __LINE__);
    SR_TRC2 (SR_CRITICAL_TRC, "Prefix: %x, NextHop: %x\n", prefix, nexthop);

    tSrTnlIfTable *pTnlDbEntry = NULL;
    tSrTnlIfTable srTnlTable;

    MEMSET (&srTnlTable, SR_ZERO, sizeof (tSrTnlIfTable));

    if(prefix == 0)
    {
        SR_TRC1(SR_FAIL_TRC,"ERROR: Unable to fetch the Prefix Ip address from CfaIndex %d\n", u4IfIndex);
        return SR_FAILURE;
    }

    /* 1. Notify CFA to release the Tnl-If-Index */
    if (CfaIfmDeleteStackMplsTunnelInterface
            (u4IfIndex, u4MplsTnlIfIndex) == CFA_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                "%s:%d uIfIndex = %d: u4MplsTnlIfIndex %d : Tunnel-If-Index is deleted by CFA\n",
                __func__, __LINE__, u4IfIndex, u4MplsTnlIfIndex);
    }
    else
    {
        SR_TRC4 (SR_FAIL_TRC,
                "%s:%d CFA not releasing Tnl-If-Indx. uIfIndex = %d: u4MplsTnlIfIndex %d\n",
                __func__, __LINE__, u4IfIndex, u4MplsTnlIfIndex);
        return SR_FAILURE;
    }

    /* As of now, let the tunnel entry to store in the persistent DB even if it is already existing.
       Later it is not required if data is persistent across all reboot
       Store TunnelIfIndex in Stub-DB whenever MPLSTunnelIfIndex is deleted
     */

    srTnlTable.u4Prefix       = prefix;
    srTnlTable.u4NextHop      = nexthop;
    srTnlTable.u4SrTnlIfIndex = u4MplsTnlIfIndex;

    /* 2. Delete the entry from persistent DB */
    SrDbDelILMTnlInfoFromDB (prefix, nexthop);

    /* 3. Delete the Tnl-If-Index entry from the RB tree */
    if(gSrGlobalInfo.u1RebootMode == SR_WARM_REBOOT)
    {
        pTnlDbEntry = RBTreeGet (gSrGlobalInfo.SrILMTnlDbTree,(tRBElem *) &srTnlTable);

        if(pTnlDbEntry == NULL)
        {
            SR_TRC5 (SR_FAIL_TRC, "%s:%d Entry not found in RB tree u4Prefix: %x u4NextHop: %x u4SrTnlIfindex: %d\n",
            __func__, __LINE__,srTnlTable.u4Prefix, srTnlTable.u4NextHop, srTnlTable.u4SrTnlIfIndex);
        }
        else
        {
            if (RBTreeRem (gSrGlobalInfo.SrILMTnlDbTree, pTnlDbEntry) == NULL)
            {
                SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to remove ILM entry from RB Tree\n", __func__, __LINE__);
                return SR_FAILURE;
            }

            if (SR_MEM_FAILURE == SR_ILM_TNL_INFO_MEM_FREE(pTnlDbEntry))
            {
                SR_TRC2 (SR_FAIL_TRC, "%s:%d ERROR! Failed to release ILM entry memory\n", __func__, __LINE__);
                return SR_FAILURE;
            }
        }
    }

    SR_TRC2 (SR_CRITICAL_TRC, "EXIT:%s:%d SUCCESS\n", __func__, __LINE__);
    return SR_SUCCESS;
}

UINT1
SrMplsSetAction (UINT2 *pMlibOperation, UINT1 u1NextHopType)
{
    UINT1   u1ProtActionType = 0;
    UINT2   u2MlibOperation  = *pMlibOperation;
    switch (u2MlibOperation)
    {
        case MPLS_MLIB_FTN_CREATE:
            if (u1NextHopType == MPLS_SR_PRIMARY)
            {
                u1ProtActionType = MPLS_FTN_ADD;
            }
            else
            {
                u1ProtActionType = MPLS_FTN_SECONDARY_ADD;
            }
            break;
        case MPLS_MLIB_FTN_DELETE:
            if (u1NextHopType == MPLS_SR_PRIMARY)
            {
                u1ProtActionType = MPLS_FTN_DEL;
            }
            else
            {
                u1ProtActionType = MPLS_FTN_SECONDARY_DELETE;
            }
            break;
        case MPLS_MLIB_ILM_CREATE:
            if (u1NextHopType == MPLS_SR_PRIMARY)
            {
                u1ProtActionType = MPLS_ILM_ADD;
            }
            else
            {
                u1ProtActionType = MPLS_ILM_SECONDARY_ADD;
            }
            break;
        case MPLS_MLIB_ILM_DELETE:
            if (u1NextHopType == MPLS_SR_PRIMARY)
            {
                u1ProtActionType = MPLS_ILM_DEL;
            }
            else
            {
                u1ProtActionType = MPLS_ILM_SECONDARY_DELETE;
            }
            break;
        case MPLS_MLIB_FTN_MODIFY:
            if (u1NextHopType == MPLS_SR_PRIMARY)
            {
                u1ProtActionType = MPLS_FTN_REPLACE_AND_DEL_PRIMARY;
                *pMlibOperation = MPLS_MLIB_FTN_CREATE;
            }
            else
            {
                SR_TRC2 (SR_CRITICAL_TRC, "%s:%d Received NH Modify for LFA nexthop\n",
                        __func__, __LINE__);
            }
            break;
        case MPLS_MLIB_FRR_FTN_MODIFY:
            if (u1NextHopType == MPLS_SR_PRIMARY)
            {
                u1ProtActionType = MPLS_FTN_REPLACE_AND_DEL_PRIMARY;
                *pMlibOperation  = MPLS_MLIB_FTN_DELETE;
            }
            else
            {
                SR_TRC2 (SR_CRITICAL_TRC, "%s:%d Received NH Modify for LFA nexthop\n",
                        __func__, __LINE__);
            }
            break;
        case MPLS_MLIB_ILM_MODIFY:
            if (u1NextHopType == MPLS_SR_PRIMARY)
            {
                u1ProtActionType = MPLS_ILM_REPLACE_AND_DEL_PRIMARY;
                *pMlibOperation = MPLS_MLIB_ILM_CREATE;
            }
            else
            {
                SR_TRC2 (SR_CRITICAL_TRC, "%s:%d Received NH Modify for LFA nexthop\n",
                        __func__, __LINE__);
            }
            break;
        case MPLS_MLIB_FRR_ILM_MODIFY:
            if (u1NextHopType == MPLS_SR_PRIMARY)
            {
                u1ProtActionType = MPLS_ILM_REPLACE_AND_DEL_PRIMARY;
                *pMlibOperation = MPLS_MLIB_ILM_DELETE;
            }
            else
            {
                SR_TRC2 (SR_CRITICAL_TRC, "%s:%d Received NH Modify for LFA nexthop\n",
                        __func__, __LINE__);
            }
            break;
        default:
            break;
    }
    SR_TRC5 (SR_UTIL_TRC, "%s:%d u2MlibOperation:%d u1NextHopType:%d u1ProtActionType%d \r\n",
    __func__, __LINE__, u2MlibOperation, u1NextHopType, u1ProtActionType);
    return u1ProtActionType;
}
/*****************************************************************************
 * Function Name : SrMplsCreateOrDeleteLsp
 * Description   : This routine will populate LspInfo and NHLFE & Create/Delete
 *                 FTN/ILM
 *
 * Input(s)      : pSrRtrInfo - Peer LSA info for which FTN is to be
 *                 Created/Deleted
 *
 *                 pSrInSegInfo - In-mapping Info to be created/deleted
 *
 * Output(s)     : None
 * Return(s)     : SUCCESS/FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsCreateOrDeleteLsp (UINT2 u2MlibOperation, tSrRtrInfo * pSrRtrInfo,
                         tSrInSegInfo * pSrInSegInfo,
                         tSrRtrNextHopInfo * pSrRtrNextHopInfo)
{

    tNHLFE              Nhlfe;
    tRtInfo             InRtInfo;
    tLspInfo            LspInfo;
    tSrRtrInfo         *pTempSrRtrInfo = NULL;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT2               u2MplsOperation = SR_ZERO;
    UINT4               u4L3Ifindex = SR_ZERO;
    UINT4               u4LabelValue = SR_ZERO;

    tGenU4Addr          dummyAddr;
    tGenU4Addr          dummyAddr2;
    tGenU4Addr          tempAddr;
    tSrTeRtEntryInfo   *pSrTePathInfo = NULL;
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tSrRtrInfo         *pRemoteRtrInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrPriNextHopInfo = NULL;
    UINT4               u4MinSrgb = SR_ZERO;
    UINT4               u4MaxSrgb = SR_ZERO;
    UINT4               u4V3MinSrgb = SR_ZERO;
    UINT4               u4V3MaxSrgb = SR_ZERO;
    UINT1               u1IsPhp     = SR_FALSE;
    UINT1               u1IsENull   = SR_FALSE;
    UINT1               u1IsPrefixtoNbr = SR_FALSE;

    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr2, 0xff, sizeof (tGenU4Addr));
    MEMSET (&tempAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&Nhlfe, SR_ZERO, sizeof (tNHLFE));
    MEMSET (&LspInfo, SR_ZERO, sizeof (tLspInfo));
    MEMSET (&InRtInfo, SR_ZERO, sizeof (tRtInfo));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    if ((pSrRtrInfo == NULL) && (pSrInSegInfo == NULL))
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Node & Peer == NULL, Invalid Operation \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }

    if (pSrRtrInfo == NULL)
    {
        if ((((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
             || ((SR_MODE & SR_CONF_OSPFV3) == SR_CONF_OSPFV3))
            && gSrGlobalInfo.u4CliCmdMode == SR_FALSE)
        {
            u2MplsOperation = MPLS_OPR_POP_AND_FWD;
            SR_TRC2 (SR_UTIL_TRC,
                     "%s:%d Peer Info == NULL implies Operation: LABEL_POP_&_FWD \n",
                     __func__, __LINE__);
        }
        else
        {
            if (pSrInSegInfo->u4OutIfIndex == SR_ZERO)
            {
                u2MplsOperation = MPLS_OPR_POP;
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d FTN to be created | Operation: LABEL_POP \n",
                         __func__, __LINE__);
            }
            else
            {
                u2MplsOperation = MPLS_OPR_POP_AND_FWD;
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d Peer Info == NULL implies Operation: LABEL_POP_&_FWD \n",
                         __func__, __LINE__);
            }
        }
    }
    else
    {
        SR_TRC5 (SR_UTIL_TRC,  "%s:%d Prefix SID %x Flags %x for prefix = %x\n", __func__,
            __LINE__, pSrRtrInfo->u4SidValue, pSrRtrInfo->u1Flags, pSrRtrInfo->prefixId.Addr.u4Addr);

        if ((pSrRtrInfo->u1Flags&PREFIX_SID_NP_FLAG) == SR_ZERO)
        {
            u1IsPhp = SR_TRUE;
        }
        else if (((pSrRtrInfo->u1Flags&PREFIX_SID_NP_FLAG) == PREFIX_SID_NP_FLAG)
                && ((pSrRtrInfo->u1Flags&PREFIX_SID_E_FLAG) == PREFIX_SID_E_FLAG))
        {
            u1IsENull = SR_TRUE;
        }
    }

    if (pSrInSegInfo == NULL)
    {
        u2MplsOperation = MPLS_OPR_PUSH;
        SR_TRC2 (SR_UTIL_TRC,
                 "%s:%d Node Info == NULL implies Operation: LABEL PUSH \n",
                 __func__, __LINE__);
    }

    if ((pSrRtrInfo != NULL) && (pSrInSegInfo != NULL))
    {
        if (pSrRtrInfo->u1TeSwapStatus != TRUE)
        {
            if (pSrRtrNextHopInfo == NULL)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d pSrRtrNextHopInfo == NULL\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
        }
        u2MplsOperation = MPLS_OPR_POP_PUSH;

        pSrInSegInfo->Fec.u2AddrFmly = pSrRtrInfo->u2AddrType;
        if (pSrInSegInfo->Fec.u2AddrFmly == SR_IPV4_ADDR_TYPE)
        {
            MEMCPY (&(pSrInSegInfo->Fec.Prefix.u4Addr),
                    &(pSrRtrInfo->prefixId.Addr.u4Addr), MAX_IPV4_ADDR_LEN);
        }
        else if (pSrInSegInfo->Fec.u2AddrFmly == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (pSrInSegInfo->Fec.Prefix.Ip6Addr.u1_addr,
                    pSrRtrInfo->prefixId.Addr.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);
        }

        SR_TRC2 (SR_UTIL_TRC,
                 "%s:%d Node & Peer Info != NULL implies Operation: LABEL_SWAP \n",
                 __func__, __LINE__);
    }

    /* 1. Fill LspInfo for InSegment information */
    if ((u2MplsOperation == MPLS_OPR_POP_AND_FWD) ||
        (u2MplsOperation == MPLS_OPR_POP_PUSH) ||
        (u2MplsOperation == MPLS_OPR_POP))
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Filling In-Seg Info \n", __func__, __LINE__);
        /* Incoming IF is not required as Label will be used to
         * identify ILM Entry.
         * #"To DO"- It may be required to make multiple ILM SWAPS (ECMP)*/

        LspInfo.u4IfIndex = SR_ZERO;
        if (u2MplsOperation == MPLS_OPR_POP)
        {
            LspInfo.u4IfIndex = pSrInSegInfo->u4InIfIndex;
        }
        LspInfo.u4InTopLabel = pSrInSegInfo->u4InLabel;

        LspInfo.FecParams.u1FecType = SR_FEC_PREFIX_TYPE;

        LspInfo.FecParams.u2AddrType = (UINT1) pSrInSegInfo->Fec.u2AddrFmly;

        if (LspInfo.FecParams.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            MEMCPY (&(LspInfo.FecParams.DestAddrPrefix.u4Addr),
                    &(pSrInSegInfo->Fec.Prefix.u4Addr), MAX_IPV4_ADDR_LEN);
            LspInfo.FecParams.u2AddrType = SR_IPV4_ADDR_TYPE;
            LspInfo.FecParams.DestMask.u4Addr = SR_IPV4_DEST_MASK;

            SR_TRC4 (SR_UTIL_TRC,
                     "%s:%d Filled In-Seg Info: InLabel %d, FEC %x \n", __func__,
                     __LINE__, LspInfo.u4InTopLabel,
                     LspInfo.FecParams.DestAddrPrefix.u4Addr);
        }
        else if (LspInfo.FecParams.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (LspInfo.FecParams.DestAddrPrefix.Ip6Addr.u1_addr,
                    pSrInSegInfo->Fec.Prefix.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);
            LspInfo.FecParams.u2AddrType = SR_IPV6_ADDR_TYPE;
            MEMCPY (LspInfo.FecParams.DestMask.Ip6Addr.u1_addr,
                    dummyAddr2.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        }

        /*Setting NHLFE to default values */
        LspInfo.pNhlfe = (tNHLFE *) (&Nhlfe);

    }

    /* 2. Fill LspInfo for OutSegment information */
    if ((u2MplsOperation == MPLS_OPR_PUSH) && (pSrRtrInfo != NULL))
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Filling FTN Info \n", __func__, __LINE__);

        LspInfo.FecParams.u1FecType = SR_FEC_PREFIX_TYPE;

        LspInfo.FecParams.u2AddrType = pSrRtrInfo->u2AddrType;

        if (LspInfo.FecParams.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            MEMCPY (&(LspInfo.FecParams.DestAddrPrefix.u4Addr),
                    &(pSrRtrInfo->prefixId.Addr.u4Addr), MAX_IPV4_ADDR_LEN);

            LspInfo.FecParams.DestMask.u4Addr = SR_IPV4_DEST_MASK;

            SR_TRC3 (SR_UTIL_TRC, "%s:%d Filled FTN Info: FEC %x \n",
                     __func__, __LINE__,
                     LspInfo.FecParams.DestAddrPrefix.u4Addr);
        }
        else if (LspInfo.FecParams.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (LspInfo.FecParams.DestAddrPrefix.Ip6Addr.u1_addr,
                    pSrRtrInfo->prefixId.Addr.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);
            LspInfo.FecParams.u2AddrType = SR_IPV6_ADDR_TYPE;
            MEMCPY (LspInfo.FecParams.DestMask.Ip6Addr.u1_addr,
                    dummyAddr2.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        }
    }

    LspInfo.Direction = MPLS_DIRECTION_FORWARD;
    LspInfo.u1Owner = MPLS_OWNER_SR;

    /* Fill NHLFE Info for OutSegment Entry For PUSH/SWAP/POP&FWD */
    if (u2MplsOperation == MPLS_OPR_PUSH || u2MplsOperation == MPLS_OPR_POP_PUSH
        || u2MplsOperation == MPLS_OPR_POP_AND_FWD)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Filling NHLFE Info \n", __func__, __LINE__);

        Nhlfe.u1Operation = (UINT1) u2MplsOperation;

        if (u2MplsOperation == MPLS_OPR_POP_AND_FWD)
        {
            if (u2MlibOperation != MPLS_MLIB_ILM_DELETE)
            {
                /* In case of Dynamic & gSrGlobalInfo.u4CliCmdMode == SR_TRUE, then PoP&Fwd is being created for AdjSid */
                if ((((SR_MODE & SR_CONF_OSPF) == SR_CONF_OSPF)
                     || ((SR_MODE & SR_CONF_OSPFV3) == SR_CONF_OSPFV3))
                     && (gSrGlobalInfo.u4CliCmdMode == SR_FALSE)
                     && (pSrRtrNextHopInfo != NULL))
                {
                    /* Get Nexthop and outInterface to be filled for OutSegment in POP_AND_FWD */
                    if (pSrRtrNextHopInfo->nextHop.u2AddrType ==
                        SR_IPV4_ADDR_TYPE)
                    {
                        if (pSrRtrNextHopInfo->nextHop.Addr.u4Addr == SR_ZERO)
                        {
                            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d NH=0 \n", __func__,
                                     __LINE__);
                            return SR_FAILURE;
                        }
                        else
                        {
                            MEMCPY (&(Nhlfe.NextHopAddr.u4Addr),
                                    &pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                                    SR_IPV4_ADDR_LENGTH);
                        }
                    }
                    else if (pSrRtrNextHopInfo->nextHop.u2AddrType ==
                             SR_IPV6_ADDR_TYPE)
                    {
                        if (MEMCMP
                            (pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                             dummyAddr.Addr.Ip6Addr.u1_addr,
                             SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                        {
                            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d NH=0 \n", __func__,
                                     __LINE__);
                            return SR_FAILURE;
                        }
                        else
                        {
                            MEMCPY (Nhlfe.NextHopAddr.Ip6Addr.u1_addr,
                                    pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.
                                    u1_addr, SR_IPV6_ADDR_LENGTH);
                        }
                    }
                    u4OutIfIndex = pSrRtrNextHopInfo->u4OutIfIndex;
                }
                else
                {
                    u4OutIfIndex = pSrInSegInfo->u4OutIfIndex;
                    Nhlfe.NextHopAddr.u4Addr = pSrInSegInfo->Fec.Prefix.u4Addr;
                }
#ifdef CFA_WANTED
                if (CfaIfmCreateStackMplsTunnelInterface
                    (u4OutIfIndex, &u4MplsTnlIfIndex) == CFA_SUCCESS)
                {
                    SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                             __func__, __LINE__, u4OutIfIndex,
                             u4MplsTnlIfIndex);

                    u4OutIfIndex = u4MplsTnlIfIndex;

                    if (pSrRtrNextHopInfo != NULL)
                    {
                        pSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                    }
                }
                else
                {
                    SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                             __func__, __LINE__, u4OutIfIndex,
                             u4MplsTnlIfIndex);
                    return SR_FAILURE;
                }
#endif

                Nhlfe.u4OutIfIndex = (UINT4) u4OutIfIndex;
            }
            else
            {
                Nhlfe.u4OutIfIndex = (UINT4) u4OutIfIndex;
#ifdef CFA_WANTED
                if (pSrRtrNextHopInfo != NULL)
                {
                    if (CfaUtilGetIfIndexFromMplsTnlIf
                        (pSrRtrNextHopInfo->u4OutIfIndex, &u4L3Ifindex,
                         TRUE) != CFA_FAILURE)
                    {
                        if (CfaIfmDeleteStackMplsTunnelInterface
                            (u4L3Ifindex,
                             pSrRtrNextHopInfo->u4OutIfIndex) == CFA_SUCCESS)
                        {
                            SR_TRC1 (SR_UTIL_TRC,
                                     "MPLS tunnel interface deleted: %d \n",
                                     pSrRtrNextHopInfo->u4OutIfIndex);
                                 pSrRtrNextHopInfo->u4OutIfIndex=u4L3Ifindex;
                        }
                        else
                        {
                            SR_TRC1 (SR_UTIL_TRC | SR_FAIL_TRC,
                                     "Error in deleting MPLS tunnel interface deleted: %d \n",
                                     pSrRtrNextHopInfo->u4OutIfIndex);
                        }
                    }
                }
#endif
            }

            /* OutLabel will be INVALID for POP_AND_FWD */
            Nhlfe.u4OutLabel = MPLS_INVALID_LABEL;
            Nhlfe.u1OperStatus = MPLS_STATUS_UP;
            Nhlfe.u1NHAddrType = (UINT1) pSrInSegInfo->Fec.u2AddrFmly;

            SR_TRC5 (SR_UTIL_TRC,
                     "%s:%d Filled NHLFE Info for POP_&_FWD: OutLabel %d OutIfIndex %d NH %x \n",
                     __func__, __LINE__, Nhlfe.u4OutLabel, Nhlfe.u4OutIfIndex,
                     Nhlfe.NextHopAddr.u4Addr);
        }
        else if (pSrRtrInfo != NULL)
        {
            SR_TRC3 (SR_UTIL_TRC, "%s:%d ILM_SWAP: u1TeSwapStatus = %d \n",
                     __func__, __LINE__, pSrRtrInfo->u1TeSwapStatus);

            if (((pSrRtrInfo->u1TeSwapStatus == TRUE)
                 && (pSrRtrNextHopInfo == NULL))
                || ((pSrRtrInfo->u1TeSwapStatus == TRUE)
                    && (pSrRtrNextHopInfo != NULL)
                    && (pSrRtrNextHopInfo->u1FRRNextHop == SR_PRIMARY_NEXTHOP)))
            {
                if (pSrRtrInfo->teSwapNextHop.u2AddrType == SR_IPV4_ADDR_TYPE)
                {
                    MEMCPY (&(Nhlfe.NextHopAddr.u4Addr),
                            &(pSrRtrInfo->teSwapNextHop.Addr.u4Addr),
                            MAX_IPV4_ADDR_LEN);
                }
                else if (pSrRtrInfo->teSwapNextHop.u2AddrType ==
                         SR_IPV6_ADDR_TYPE)
                {
                    MEMCPY (Nhlfe.NextHopAddr.Ip6Addr.u1_addr,
                            pSrRtrInfo->teSwapNextHop.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                }
            }
            else
            {
                if (pSrRtrNextHopInfo != NULL)
                {
                    if (pSrRtrNextHopInfo->nextHop.u2AddrType ==
                        SR_IPV4_ADDR_TYPE)
                    {
                        MEMCPY (&(Nhlfe.NextHopAddr.u4Addr),
                                &(pSrRtrNextHopInfo->nextHop.Addr.u4Addr),
                                MAX_IPV4_ADDR_LEN);
                    }
                    else if (pSrRtrNextHopInfo->nextHop.u2AddrType ==
                             SR_IPV6_ADDR_TYPE)
                    {
                        MEMCPY (Nhlfe.NextHopAddr.Ip6Addr.u1_addr,
                                pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);
                    }
                }
            }
            SR_TRC3 (SR_UTIL_TRC,
                     "%s:%d ILM_SWAP: Nhlfe.NextHopAddr.u4Addr = %x \n", __func__,
                     __LINE__, Nhlfe.NextHopAddr.u4Addr);

            if (SR_MODE != SR_MODE_STATIC)
            {
                /* Create Label with respect to Peer SRGB */
                if ((u2MlibOperation != MPLS_MLIB_FTN_DELETE)
                    && (u2MlibOperation != MPLS_MLIB_ILM_DELETE))
                {
                    MEMCPY (&tempAddr, &Nhlfe.NextHopAddr, sizeof (uGenU4Addr));
                    if (pSrRtrInfo->u1TeSwapStatus == TRUE)
                    {
                        tempAddr.u2AddrType =
                            pSrRtrInfo->teSwapNextHop.u2AddrType;
                    }
                    else if (pSrRtrNextHopInfo != NULL)
                    {
                        tempAddr.u2AddrType =
                            pSrRtrNextHopInfo->nextHop.u2AddrType;
                    }

                    if (tempAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
                    {
                        pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&tempAddr);
                    }
                    else if (tempAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
                    {
                        pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&tempAddr);
                    }

                    if (pTempSrRtrInfo == NULL)
                    {
                        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                                 "%s:%d SrGetRtrInfoFromNextHop Failure: pTempSrRtrInfo == NULL \n",
                                 __func__, __LINE__);
                        return SR_FAILURE;
                    }
                    if (tempAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
                    {
                        if (pTempSrRtrInfo->srgbRange.u4SrGbMinIndex == SR_ZERO)
                        {
                            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                                     "%s:%d SrGetRtrInfoFromNextHop Failure:  pTempSrRtrInfo->srgbRange.u4SrGbMinIndex is 0 \n",
                                     __func__, __LINE__);
                            return SR_FAILURE;
                        }
                        u4MinSrgb = pTempSrRtrInfo->srgbRange.u4SrGbMinIndex;
                        u4MaxSrgb = pTempSrRtrInfo->srgbRange.u4SrGbMaxIndex;
                    }
                    else if (tempAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
                    {
                        if (pTempSrRtrInfo->srgbRange.u4SrGbV3MinIndex ==
                            SR_ZERO)
                        {
                            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                                     "%s:%d SrGetRtrInfoFromNextHop Failure:  pTempSrRtrInfo->srgbRange.u4SrGbV3MinIndex is 0\n",
                                     __func__, __LINE__);
                            return SR_FAILURE;
                        }
                        u4V3MinSrgb =
                            pTempSrRtrInfo->srgbRange.u4SrGbV3MinIndex;
                        u4V3MaxSrgb =
                            pTempSrRtrInfo->srgbRange.u4SrGbV3MaxIndex;
                    }

                    if ((pSrRtrInfo != NULL) && (pSrRtrNextHopInfo != NULL))
                    {
                        if (tempAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
                        {
                            u4LabelValue = pSrRtrInfo->u4SidValue + u4MinSrgb;
                            if ((u4LabelValue < u4MinSrgb) ||
                                (u4LabelValue > u4MaxSrgb))
                            {
                                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                                         "%s:%d Outlabel value is out of range for next hop SRGB.\n",
                                         __func__, __LINE__);
                                return SR_FAILURE;
                            }
                        }
                        else if (tempAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
                        {
                            u4LabelValue = pSrRtrInfo->u4SidValue + u4V3MinSrgb;
                            if ((u4LabelValue < u4V3MinSrgb) ||
                                (u4LabelValue > u4V3MaxSrgb))
                            {
                                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                                         "%s:%d Outlabel value is out of range for next hop SRGB.\n",
                                         __func__, __LINE__);
                                return SR_FAILURE;
                            }
                        }

                        if (pSrRtrNextHopInfo->nbrRtrId == pSrRtrInfo->advRtrId.Addr.u4Addr)
                        {
                            u1IsPrefixtoNbr = SR_TRUE;
                            SR_TRC4 (SR_UTIL_TRC,  "%s:%d Adv Rtr for preifx = %x is Nexthop Rtr %x\n", __func__,
                                    __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr, pSrRtrNextHopInfo->nbrRtrId);
                        }

                    }
                    if (((pSrRtrInfo->u1TeSwapStatus == TRUE)
                         && (pSrRtrNextHopInfo == NULL))
                        || ((pSrRtrInfo->u1TeSwapStatus == TRUE)
                            && (pSrRtrNextHopInfo != NULL)
                            && (pSrRtrNextHopInfo->u1FRRNextHop ==
                                SR_PRIMARY_NEXTHOP)))

                    {
                        if (tempAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
                        {
                            pSrRtrInfo->u4TeSwapOutLabel =
                                pSrRtrInfo->u4SidValue + u4MinSrgb;
                        }
                        else if (tempAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
                        {
                            pSrRtrInfo->u4TeSwapOutLabel =
                                pSrRtrInfo->u4SidValue + u4V3MinSrgb;
                        }
                        Nhlfe.u4OutLabel = pSrRtrInfo->u4TeSwapOutLabel;
                    }
                    else
                    {
                        if (tempAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
                        {
                            pSrRtrInfo->u4OutLabel =
                                pSrRtrInfo->u4SidValue + u4MinSrgb;
                        }
                        else if (tempAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
                        {
                            pSrRtrInfo->u4OutLabel =
                                pSrRtrInfo->u4SidValue + u4V3MinSrgb;
                        }

                        if (u1IsPrefixtoNbr == SR_TRUE)
                        {
                            if (u1IsENull == SR_TRUE)
                            {
                                pSrRtrInfo->u4OutLabel = MPLS_IPV4_EXPLICIT_NULL_LABEL; /* explicit NULL case */
                                SR_TRC4 (SR_UTIL_TRC,  "%s:%d Explicit NULL set for prefix = %x with Nexthop %x\n", __func__,
                                        __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr, Nhlfe.NextHopAddr.u4Addr);
                            }
                            else if (u1IsPhp == SR_TRUE)
                            {
                                pSrRtrInfo->u4OutLabel = MPLS_IMPLICIT_NULL_LABEL; /* implicit NULL/PHP case */
                                SR_TRC4 (SR_UTIL_TRC,  "%s:%d Implicit NULL set for prefix = %x with Nexthop %x\n", __func__,
                                        __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr, Nhlfe.NextHopAddr.u4Addr);
                            }
                        }

                        if ((pSrRtrNextHopInfo != NULL)
                            && (pSrRtrNextHopInfo->u1FRRNextHop !=
                                SR_PRIMARY_NEXTHOP)
                            && (pSrRtrInfo->u1LfaType == SR_RLFA_ROUTE))
                        {
                            TMO_SLL_Scan (&(gSrGlobalInfo.routerList),
                                          pRtrNode, tTMO_SLL_NODE *)
                            {
                                pRemoteRtrInfo = (tSrRtrInfo *) pRtrNode;
                                if (pSrRtrNextHopInfo->nextHop.u2AddrType ==
                                    SR_IPV4_ADDR_TYPE)
                                {
                                    if (pSrRtrNextHopInfo->RemoteNodeAddr.Addr.
                                        u4Addr ==
                                        pRemoteRtrInfo->prefixId.Addr.u4Addr)
                                    {
                                        pSrRtrInfo->u4OutLabel =
                                            pSrRtrInfo->u4SidValue +
                                            pRemoteRtrInfo->srgbRange.
                                            u4SrGbMinIndex;
                                        SR_TRC4 (SR_UTIL_TRC,
                                                 "%s:%d Remote Node found 0x%x label: %u \n",
                                                 __func__, __LINE__,
                                                 pSrRtrNextHopInfo->
                                                 RemoteNodeAddr.Addr.u4Addr,
                                                 pSrRtrInfo->u4OutLabel);
                                        break;
                                    }
                                }
                                else
                                {
                                    if (MEMCMP
                                        (&
                                         (pSrRtrNextHopInfo->RemoteNodeAddr.
                                          Addr.Ip6Addr),
                                         &(pRemoteRtrInfo->prefixId.Addr.Ip6Addr),
                                         SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                                    {
                                        pSrRtrInfo->u4OutLabel =
                                            pSrRtrInfo->u4SidValue +
                                            pRemoteRtrInfo->srgbRange.
                                            u4SrGbV3MinIndex;
                                        SR_TRC4 (SR_UTIL_TRC,
                                                 "%s:%d Remote Node found 0x%s label: %u \n",
                                                 __func__, __LINE__,
                                                 Ip6PrintAddr
                                                 (&pSrRtrNextHopInfo->
                                                  RemoteNodeAddr.Addr.Ip6Addr),
                                                 pSrRtrInfo->u4OutLabel);
                                        break;
                                    }
                                }
                            }
                        }

                        Nhlfe.u4OutLabel = pSrRtrInfo->u4OutLabel;
                    }
                }
                else
                {
#if 0
                    MEMCPY (&u4DestIpAddr, pSrRtrInfo->rtrId,
                            MAX_IPV4_ADDR_LEN);
#endif
                    pSrTePathInfo =
                        SrGetTeRouteEntryFromDestAddr (&(pSrRtrInfo->prefixId));
                    if (pSrTePathInfo != NULL
                        && u2MlibOperation == MPLS_MLIB_ILM_DELETE)
                    {
                        Nhlfe.u4OutLabel = pSrRtrInfo->u4TeSwapOutLabel;
                    }
                    else
                    {
                        Nhlfe.u4OutLabel = pSrRtrInfo->u4OutLabel;
                    }
                }
            }
            else
            {
                Nhlfe.u4OutLabel = pSrRtrInfo->u4SidValue;
            }

            Nhlfe.u1NHAddrType = (UINT1) pSrRtrInfo->u2AddrType;

            if (pSrRtrInfo->u1TeSwapStatus == TRUE)
            {
                Nhlfe.u4OutIfIndex = (UINT4) pSrRtrInfo->u4TeSwapOutIfIndex;
                if ((pSrRtrNextHopInfo != NULL)
                    && (pSrRtrNextHopInfo->u1FRRNextHop == SR_LFA_NEXTHOP))
                {
                    Nhlfe.u4OutIfIndex =
                        (UINT4) pSrRtrNextHopInfo->u4SwapOutIfIndex;
                }

                pSrTePathInfo =
                    SrGetTeRouteEntryFromDestAddr (&(pSrRtrInfo->prefixId));
                if (pSrTePathInfo != NULL)
                {
                    if ((pSrRtrNextHopInfo == NULL)
                        || ((pSrRtrNextHopInfo != NULL)
                            && (pSrRtrNextHopInfo->u1FRRNextHop ==
                                SR_PRIMARY_NEXTHOP)))
                    {
                        LspInfo.u1FRRNextHop = SR_TE_NEXTHOP;
                        /* If bIsFRRHwLsp is SR_FALSE, then we will program the route entry in H/W(SR_TE) */
                        LspInfo.bIsFRRHwLsp = SR_FALSE;
                    }
                }
            }
            else
            {
                if (MPLS_OPR_POP_PUSH == u2MplsOperation)
                {
                    Nhlfe.u4OutIfIndex =
                        (UINT4) pSrRtrNextHopInfo->u4SwapOutIfIndex;
                }
                else
                {
                    Nhlfe.u4OutIfIndex =
                        (UINT4) pSrRtrNextHopInfo->u4OutIfIndex;
                }
            }
            Nhlfe.u1OperStatus = MPLS_STATUS_UP;

            SR_TRC5 (SR_UTIL_TRC,
                     "%s:%d Filled NHLFE Info: OutLabel %d OutIfIndex %d NH %x \n",
                     __func__, __LINE__, Nhlfe.u4OutLabel, Nhlfe.u4OutIfIndex,
                     Nhlfe.NextHopAddr.u4Addr);
        }

        LspInfo.pNhlfe = (tNHLFE *) (&Nhlfe);

    }
    if (((u2MplsOperation == MPLS_OPR_PUSH)
         || (u2MplsOperation == MPLS_OPR_POP_PUSH))
        && (pSrRtrNextHopInfo != NULL))
    {
        /*if LSP creation is for LFA/RLFA route */
        LspInfo.u1FRRNextHop = pSrRtrNextHopInfo->u1FRRNextHop;
        LspInfo.bIsFRRHwLsp = pSrRtrNextHopInfo->bIsFRRHwLsp;
        LspInfo.bIsOnlyCPUpdate = pSrRtrNextHopInfo->bIsOnlyCPUpdate;
        LspInfo.bIsFRRHwModifyLsp = pSrRtrNextHopInfo->bIsFRRModifyLsp;
        MEMCPY (&LspInfo.PrimarynextHop, &pSrRtrNextHopInfo->PrimarynextHop,
                sizeof (tGenU4Addr));

        if ((pSrRtrNextHopInfo->u1FRRNextHop == SR_LFA_NEXTHOP) &&
            (pSrRtrNextHopInfo->bIsOnlyCPUpdate != MPLS_TRUE))
        {
            if ((u2MplsOperation == MPLS_OPR_POP_PUSH)
                && (pSrRtrInfo->u1TeSwapStatus == TRUE))
            {
                LspInfo.u4PrimaryTunIndex = pSrRtrInfo->u4TeSwapOutIfIndex;
            }
            else
            {
                pSrRtrPriNextHopInfo = SrV4GetSrNextHopInfoFromRtr (pSrRtrInfo,
                                                                    pSrRtrNextHopInfo->
                                                                    PrimarynextHop.
                                                                    Addr.
                                                                    u4Addr);
                if (pSrRtrPriNextHopInfo == NULL)
                {
                    SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                             "%s:%d Fetching Nexthop Info failed for prefix %x"
                             " with Nexthop %x has failed \n", __func__, __LINE__,
                             pSrRtrInfo->prefixId.Addr.u4Addr,
                             pSrRtrNextHopInfo->PrimarynextHop.Addr.u4Addr);
                    return SR_FAILURE;
                }
                if (u2MplsOperation == MPLS_OPR_POP_PUSH)
                {
                    LspInfo.u4PrimaryTunIndex = pSrRtrPriNextHopInfo->u4SwapOutIfIndex;
                }
                else
                {
                    LspInfo.u4PrimaryTunIndex = pSrRtrPriNextHopInfo->u4OutIfIndex;
                }

            }
        }

        else if (MPLS_MLIB_FRR_FTN_MODIFY == u2MlibOperation)
        {
            Nhlfe.u4OldOutIfIndex = pSrRtrNextHopInfo->u4OldOutIfIndex;
            LspInfo.u4PrimaryTunIndex = pSrRtrNextHopInfo->u4OutIfIndex;
            Nhlfe.NextHopAddr.u4Addr = pSrRtrNextHopInfo->nextHop.Addr.u4Addr;
            Nhlfe.OldPrimaryNextHopAddr.u4Addr = pSrRtrNextHopInfo->OldnextHop.Addr.u4Addr;
            Nhlfe.FRRNextHopAddr.u4Addr = pSrRtrInfo->u4LfaNextHop;
            LspInfo.u4LfaTunIndex = pSrRtrInfo->u4LfaOutIfIndex;

            if ((SR_RLFA_ROUTE == pSrRtrInfo->u1LfaType)
                || (SR_TILFA_ROUTE == pSrRtrInfo->u1LfaType))
            {
                SR_GET_SELF_IN_LABEL (&LspInfo.u4InTopLabel,
                                      pSrRtrInfo->u4SidValue);
                tempAddr.Addr.u4Addr = Nhlfe.NextHopAddr.u4Addr;
                tempAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&tempAddr);
                if (pTempSrRtrInfo == NULL)
                {
                    SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                             "%s:%d Fetching Nexthop Info failed for prefix %x"
                             " with Nexthop %x has failed \n", __func__, __LINE__,
                             pSrRtrInfo->prefixId.Addr.u4Addr,
                             tempAddr.Addr.u4Addr);
                    return SR_FAILURE;
                }
                Nhlfe.u4OutLabel = pTempSrRtrInfo->srgbRange.u4SrGbMinIndex +
                    pSrRtrInfo->u4SidValue;
            }
        }

        if ((pSrRtrNextHopInfo->u1FRRNextHop != SR_PRIMARY_NEXTHOP)
            && (pRemoteRtrInfo != NULL) && (pTempSrRtrInfo != NULL)
            && (pSrRtrInfo->u1LfaType == SR_RLFA_ROUTE))
        {
            if (pSrRtrNextHopInfo->nextHop.u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                if (pSrRtrNextHopInfo->RemoteNodeAddr.Addr.u4Addr != SR_ZERO)
                {
                    LspInfo.u4RemoteNodeLabel =
                        pRemoteRtrInfo->u4SidValue + u4MinSrgb;
                    SR_TRC3 (SR_UTIL_TRC,
                             "%s:%d Remote Node label to Push: %u\n", __func__,
                             __LINE__, LspInfo.u4RemoteNodeLabel);
                }
            }
            else
            {
                if (MEMCMP
                    (&(dummyAddr.Addr.Ip6Addr),
                     &(pRemoteRtrInfo->prefixId.Addr.Ip6Addr),
                     SR_IPV6_ADDR_LENGTH) != SR_ZERO)
                {
                    LspInfo.u4RemoteNodeLabel =
                        pRemoteRtrInfo->u4SidValue + u4V3MinSrgb;
                    SR_TRC3 (SR_UTIL_TRC,
                             "%s:%d Remote Node label to Push: %u\n", __func__,
                             __LINE__, LspInfo.u4RemoteNodeLabel);

                }
            }

        }
    }

    if (LspInfo.u1Owner == MPLS_OWNER_SR)
	{
		LspInfo.u1Preference = MPLS_FTN_SR_PREFERENCE;
        LspInfo.u1ProtActionType = SrMplsSetAction(&u2MlibOperation, LspInfo.u1FRRNextHop);
	}
    else if (LspInfo.u1Owner == MPLS_OWNER_SR_TE)
	{
		LspInfo.u1Preference = MPLS_FTN_SR_TE_PREFERENCE;
	}
    LspInfo.u1Protocol = MPLS_PROTOCOL_SR_OSPF;
    /*Create FTN/ILM with respect to LspInfo */
    if (MplsMlibUpdate (u2MlibOperation, &LspInfo) == MPLS_SUCCESS)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d MPLS_SUCCESS \n", __func__, __LINE__);
    }
    else
    {
        if (LspInfo.u1ArpResolveStatus == MPLS_ARP_RESOLVE_WAITING)
        {
            SR_TRC2 (SR_UTIL_TRC, "%s:%d MPLS_SUCCESS \n", __func__, __LINE__);
            return SR_SUCCESS;
        }
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d MPLS_FAILURE \n", __func__, __LINE__);

        return SR_FAILURE;
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    UNUSED_PARAM (Nhlfe.u4OutLabel);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrMplsDeleteAllILM
 * Description   : This routine is used to delete All the Dynamic MPLS ILM
 *                 (SWAP) entries
 *
 * Input(s)      : None
 *
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsDeleteAllILM (UINT2 u2AddrType)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tTMO_SLL_NODE      *pOspfNbr = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrV4OspfNbrInfo   *pSrOspfNbrInfo    = NULL;

    tSrInSegInfo        SrInSegInfo;
    UINT4               u4SrGbMinIndex = SR_ZERO;
    tGenU4Addr          teSwapNHop;
    tGenU4Addr          destAddr;

    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&teSwapNHop, SR_ZERO, sizeof (tGenU4Addr));

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;

        if (pRtrInfo->prefixId.u2AddrType != u2AddrType)
        {
            continue;
        }

        if (pRtrInfo->u1TeSwapStatus == TRUE)
        {
            if (pRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                destAddr.Addr.u4Addr = pRtrInfo->prefixId.Addr.u4Addr;
                teSwapNHop.Addr.u4Addr = pRtrInfo->teSwapNextHop.Addr.u4Addr;
                destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                teSwapNHop.u2AddrType = SR_IPV4_ADDR_TYPE;
            }
            else if (pRtrInfo->prefixId.u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                MEMCPY (destAddr.Addr.Ip6Addr.u1_addr,
                        pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr,
                        SR_IPV4_ADDR_LENGTH);
                destAddr.u2AddrType = SR_IPV6_ADDR_TYPE;

                MEMCPY (teSwapNHop.Addr.Ip6Addr.u1_addr,
                        pRtrInfo->teSwapNextHop.Addr.Ip6Addr.u1_addr,
                        SR_IPV4_ADDR_LENGTH);
                teSwapNHop.u2AddrType = SR_IPV6_ADDR_TYPE;
            }

            if ((SrDeleteILMFromSrId (&destAddr, &teSwapNHop)) == SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d  SrDeleteILMFromSrId FAILURE \n",
                         __func__, __LINE__);
            }
        }
        else
        {
            if (pRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                destAddr.Addr.u4Addr = pRtrInfo->prefixId.Addr.u4Addr;
                u4SrGbMinIndex =
                   gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
            }
            else if (pRtrInfo->prefixId.u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
                {
                    SrInSegInfo.Fec.u2AddrFmly =
                        gSrGlobalInfo.pV6SrSelfNodeInfo->ipAddrType;
                    MEMCPY (&(SrInSegInfo.Fec.Prefix.u4Addr),
                            &gSrGlobalInfo.pV6SrSelfNodeInfo->ifIpAddr,
                            SR_IPV4_ADDR_LENGTH);
                }
                MEMCPY (destAddr.Addr.Ip6Addr.u1_addr,
                        pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr,
                        SR_IPV4_ADDR_LENGTH);
                u4SrGbMinIndex =
                    gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;
            }
            /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
            SrInSegInfo.u4InLabel = (u4SrGbMinIndex + pRtrInfo->u4SidValue);

            TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                          pSrRtrNextHopNode, tTMO_SLL_NODE *)
            {
                pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

                if (SrMplsDeleteILM (&SrInSegInfo, pRtrInfo, pSrRtrNextHopInfo)
                          == SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to delete ILM SWAP \n",
                             __func__, __LINE__);
                    continue;
                }
            }
        }
    }

    /* Delete all ADj SID ILM installed for each neighbor */
    TMO_SLL_Scan (&(gSrV4OspfNbrList), pOspfNbr, tTMO_SLL_NODE *)
    {
        pSrOspfNbrInfo = (tSrV4OspfNbrInfo *) pOspfNbr;
        SrProcessOspfNbrADjSidILMUpdate(pSrOspfNbrInfo, SR_TRUE);
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrMplsDeleteAllFTN
 * Description   : This routine is used to delete All the Dynamic MPLS FTN
 *                 entries
 *
 * Input(s)      : None
 *
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsDeleteAllFTN (UINT2 u2AddrType)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    UINT4               u4ErrCode = SR_ZERO;
    UINT4               u4IpAddress = SR_ZERO;
    UINT4               u4OutIfIndex = SR_ZERO;
    INT4                i4LfaStatus = ALTERNATE_DISABLED;
    tGenU4Addr          tmpNextHopAddr;
    tGenU4Addr          destAddr;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&tmpNextHopAddr, SR_ZERO, sizeof (tGenU4Addr));

    nmhGetFsSrV4AlternateStatus (&i4LfaStatus);
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;
        u4ErrCode = SR_ZERO;

        if (pRtrInfo->prefixId.u2AddrType != u2AddrType)
        {
            continue;
        }

        destAddr.u2AddrType = pRtrInfo->prefixId.u2AddrType;
        if (pRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            destAddr.Addr.u4Addr = pRtrInfo->prefixId.Addr.u4Addr;
        }
        else if (pRtrInfo->prefixId.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (destAddr.Addr.Ip6Addr.u1_addr,
                    pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        }

        if (TMO_SLL_Count (&pRtrInfo->NextHopList) != SR_ZERO)
        {
            TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                          pSrRtrNextHopNode, tTMO_SLL_NODE *)
            {
                pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

                if (pSrRtrNextHopInfo->u1FRRNextHop == SR_RLFA_NEXTHOP)
                {
                    if (pRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE)
                    {
                        tmpNextHopAddr.Addr.u4Addr =
                            pSrRtrNextHopInfo->nextHop.Addr.u4Addr;
                    }
                    else if (pRtrInfo->prefixId.u2AddrType == SR_IPV6_ADDR_TYPE)
                    {
                        MEMCPY (tmpNextHopAddr.Addr.Ip6Addr.u1_addr,
                                pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);
                    }
                    if (SrRlfaDeleteFtnEntry (&destAddr, &tmpNextHopAddr) ==
                        SR_SUCCESS)
                    {
                        SR_TRC (SR_UTIL_TRC, "SrTeDeleteFtn SUCCESS \n");
                    }
                }
                else
                {
                    /* To delete TE LSP in Type10 flow */
                    if (SrTeDeleteFtnType10 (&destAddr, &u4ErrCode) ==
                        SR_SUCCESS)
                    {
                        if (u4ErrCode == SR_TE_VIA_DEST)
                        {
                            SR_TRC3 (SR_UTIL_TRC,
                                     "%s:%d SR TE LSP deleted via dest in Type10 flow for prefix %x \n",
                                     __func__, __LINE__, destAddr.Addr.u4Addr);
                        }
                    }
                    if (u4ErrCode != SR_TE_VIA_DEST)
                    {
                        /* Primary FTN Deletion */
                        if (SrMplsDeleteFTN (pRtrInfo, pSrRtrNextHopInfo) ==
                            SR_FAILURE)
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to delete FTN \n",
                                     __func__, __LINE__);
                        }

                        if ((i4LfaStatus == ALTERNATE_ENABLED) &&
                            (pRtrInfo->u4LfaNextHop != SR_ZERO))
                        {
                            /* LFA FTN Deletion */
                            u4IpAddress =
                                (pRtrInfo->u4LfaNextHop & pRtrInfo->u4DestMask);
                            if (CfaIpIfGetIfIndexForNetInCxt
                                (SR_ZERO, u4IpAddress,
                                 &u4OutIfIndex) == CFA_FAILURE)
                            {
                                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                                         "%s:%d Exiting: Not able to get Out IfIndex \n",
                                         __func__, __LINE__);
                            }
                            pSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                            pSrRtrNextHopInfo->u1MPLSStatus = SR_ONE;
                            MEMCPY (&pSrRtrNextHopInfo->nextHop,
                                    &pRtrInfo->u4LfaNextHop, sizeof (tIPADDR));
                            if (SrMplsDeleteFTN (pRtrInfo, pSrRtrNextHopInfo) ==
                                SR_FAILURE)
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                                         "%s:%d Unable to delete LFA FTN \n",
                                         __func__, __LINE__);
                            }
                        }
                    }
                }
            }

            if (&pRtrInfo->NextHopList != NULL)
            {
                TMO_SLL_FreeNodes ((tTMO_SLL *) & pRtrInfo->NextHopList,
                                   SrMemPoolIds[MAX_SR_RTR_NH_SIZING_ID]);
            }
        }
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrMplsDynamicCleanUp
 * Description   : This routine is used to delete All the Dynamic SR-MPLS FTN
 *                 and ILM Swap Entries.
 *
 * Input(s)      : u2AddrType - Address type (IPv4 or IPv6)
 *
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsDynamicCleanUp (UINT2 u2AddrType)
{
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    if (SrMplsDeleteAllILM (u2AddrType) == SR_FAILURE)
    {
        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsDeleteAllILM FAILURE \n", __func__,
                 __LINE__);
        return SR_FAILURE;
    }

    if (SrMplsDeleteAllFTN (u2AddrType) == SR_FAILURE)
    {
        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsDeleteAllFTN FAILURE \n", __func__,
                 __LINE__);
        return SR_FAILURE;
    }

    if (SrPeerInfoCleanUp (u2AddrType) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                 "%s:%d  SrPeerInfoCleanUp FAILURE : Cannot delete Peer Info \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }

    SrNbrInfoCleanup (SR_IPV4_ADDR_TYPE);

    TMO_SLL_FreeNodes ((tTMO_SLL *)&(gSrGlobalInfo.rtDelayList),
                       SrMemPoolIds[MAX_SR_RTR_DELAY_RT_INFO_SIZING_ID]);
    if ( gSrGlobalInfo.bIsDelayTimerStarted == SR_TRUE)
    {
        if (TmrStopTimer (SR_TMR_LIST_ID, &(gSrGlobalInfo.delayTimerNode)) !=
                           TMR_SUCCESS)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                    "%s:%d Failed to stop SR delay Timer", __func__, __LINE__);
        }

        gSrGlobalInfo.bIsDelayTimerStarted = SR_FALSE;
        SR_TRC2 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                "%s:%d SR delay Timer stopped in clean up flow",
                __func__, __LINE__);
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrPeerInfoCleanUp
 * Description   : This routine is used to delete All the Dynamic SR-MPLS FTN
 *                 and ILM Swap Entries.
 *
 * Input(s)      : None
 *
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrPeerInfoCleanUp (UINT2 u2AddrType)
{
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRtrInfo         *pTempRtrInfo = NULL;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    TMO_DYN_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrInfo, pTempRtrInfo,
                      tSrRtrInfo *)
    {
        if (pRtrInfo->prefixId.u2AddrType != u2AddrType)
        {
            continue;
        }
        if (pRtrInfo->u1SIDConflictAlarm == SR_ALARM_RAISE)
        {
            SrGenerateAlarm (pRtrInfo, SR_ALARM_CLEAR, SID_CONFLICT_ALARM);
        }

        if (pRtrInfo->u1PrefixConflictAlarm == SR_ALARM_RAISE)
        {
            SrGenerateAlarm (pRtrInfo, SR_ALARM_CLEAR, PREFIX_CONFLICT_ALARM);
        }

        if (pRtrInfo->u1OutOfRangeAlarm == SR_ALARM_RAISE)
        {
            SrGenerateAlarm (pRtrInfo, SR_ALARM_CLEAR, OOR_SID_ALARM);
        }

        TMO_SLL_FreeNodes ((tTMO_SLL *) & pRtrInfo->adjSidList,
                           SrMemPoolIds[MAX_SR_PEER_ADJ_SIZING_ID]);
        TMO_SLL_Delete (&(gSrGlobalInfo.routerList),
                        (tTMO_SLL_NODE *) pRtrInfo);
        SR_RTR_LIST_FREE (pRtrInfo);
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrIpRtChgEventHandler
 * Description   : This routine is used to Handle Route change Events from IP
 *                 Module
 *
 * Input(s)      : pNetIpv4RtInfo   - Route Info provided by IP Module
 *                 u1CmdType        - Type of Route Change (ADD/DELETE)
 *
 * Output(s)     : None
 * Return(s)     : None
 *****************************************************************************/
VOID
SrIpRtChgEventHandler (tNetIpv4RtInfo * pNetIpv4RtInfo, tNetIpv4RtInfo * pNetIpv4OldRtInfo, UINT1 u1CmdType)
{
    INT4                i4SrStatus = SR_ZERO;
    tSrRouteEntryInfo  *pRouteInfo = NULL;

    tSrQMsg             SrQMsg;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));

    nmhGetFsSrV4Status (&i4SrStatus);

    /* Ignore the Notification if i4SrStatus is not ENABLED Administratively */
    if (i4SrStatus == SR_DISABLED || SR_MODE == SR_MODE_STATIC)
    {
        return;
    }

    /* Notify only loopback ip addresses to segment routing module */
    if ((pNetIpv4RtInfo != NULL) &&
        (pNetIpv4RtInfo->u4DestMask != ISS_IP_DEF_NET_MASK))
    {
        return;
    }

    if (pNetIpv4RtInfo != NULL)
    {
        /* Alocate Memory to RouteInfo */
        if (NULL == SR_RTINFO_MEM_ALLOC (pRouteInfo))
        {
            SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                     "%s:%d Memory Allocation for Ip Route Info failed \n",
                     __func__, __LINE__);
            return;
        }

        /* Copying information to SR's structure */
        pRouteInfo->destAddr.Addr.u4Addr = pNetIpv4RtInfo->u4DestNet;
        pRouteInfo->u4DestMask = pNetIpv4RtInfo->u4DestMask;
        pRouteInfo->nextHop.Addr.u4Addr = pNetIpv4RtInfo->u4NextHop;
        pRouteInfo->u4RtIfIndx = pNetIpv4RtInfo->u4RtIfIndx;
        pRouteInfo->u4RtNxtHopAS = pNetIpv4RtInfo->u4RtNxtHopAs;
        pRouteInfo->i4Metric1 = pNetIpv4RtInfo->i4Metric1;
        pRouteInfo->u4RowStatus = pNetIpv4RtInfo->u4RowStatus;
        pRouteInfo->u2AddrType = SR_IPV4_ADDR_TYPE;
        pRouteInfo->u4Flag = pNetIpv4RtInfo->u4Flag;
    }
    else
    {
        SR_TRC2 (SR_CTRL_TRC, "%s:%d No Route Info passed by IP \n", __func__,
                 __LINE__);
        return;
    }

    SrQMsg.u4MsgType = SR_IP_EVENT;

    if (NetIpv4GetCfaIfIndexFromPort (pRouteInfo->u4RtIfIndx,
                                      &SrQMsg.u.SrIpEvtInfo.u4IfIndex) ==
        NETIPV4_FAILURE)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d No Port to IfIndex Mapping \n", __func__,
                 __LINE__);
        SR_RTINFO_MEM_FREE (pRouteInfo);
        return;
    }

    SR_TRC4 (SR_CTRL_TRC,
             "%s:%d pRouteInfo->u4RtIfIndx %d SrQMsg.u.SrIpEvtInfo.u4IfIndex %d \n",
             __func__, __LINE__, pRouteInfo->u4RtIfIndx,
             SrQMsg.u.SrIpEvtInfo.u4IfIndex);

    if (u1CmdType == NETIPV4_ADD_ROUTE)
    {
        SrQMsg.u.SrIpEvtInfo.u4BitMap = SR_RT_NEW;
    }

    else if (u1CmdType == NETIPV4_DELETE_ROUTE)
    {
        SrQMsg.u.SrIpEvtInfo.u4BitMap = SR_RT_DELETED;
    }
    else if (u1CmdType == NETIPV4_MODIFY_ROUTE)
    {
        /* From NetIP only Route Change will come so,
         * Just Mapped to Next Hop Change. We do not know
         * existing route info */
        SR_TRC1 (SR_CTRL_TRC, "SR-FRR Modify Route Event for prefix %x \r\n",
                pNetIpv4RtInfo->u4DestNet);
        pRouteInfo->u4OldNextHop = pNetIpv4OldRtInfo->u4NextHop;
        pRouteInfo->u4OldRtIfIndx = pNetIpv4OldRtInfo->u4RtIfIndx;
        SrQMsg.u.SrIpEvtInfo.u4BitMap = SR_RT_NH_CHG;
    }
    else if (u1CmdType == NETIPV4_FRR_PRIMARY_MODIFY_ROUTE)
    {
        SR_TRC1 (SR_CTRL_TRC, "SR-FRR Modify Route Event for prefix %x \r\n",
                pNetIpv4RtInfo->u4DestNet);
        pRouteInfo->u4OldNextHop = pNetIpv4OldRtInfo->u4NextHop;
        pRouteInfo-> u4OldRtIfIndx = pNetIpv4OldRtInfo->u4RtIfIndx;
        SrQMsg.u.SrIpEvtInfo.u4BitMap = SR_RT_FRR_NH_CHG;
    }

    SrQMsg.u.SrIpEvtInfo.pRouteInfo = (UINT1 *) pRouteInfo;

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d Failed to EnQ IP Event to SR Task \n",
                 __func__, __LINE__);
        SR_RTINFO_MEM_FREE (pRouteInfo);
    }
    return;
}

VOID
SrIpv6RtChgEventHandler (tNetIpv6HliParams * pNetIpv6HlParams)
{
    INT4                i4SrStatus = SR_ZERO;
    tSrRouteEntryInfo  *pRouteInfo = NULL;

    tSrQMsg             SrQMsg;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));

    nmhGetFsSrV6Status (&i4SrStatus);

    /* Ignore the Notification if i4SrStatus is not ENABLED Administratively */
    if (i4SrStatus == SR_DISABLED || SR_MODE == SR_MODE_STATIC)
    {
        return;
    }

    if (pNetIpv6HlParams == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d No Route Info passed by IP6 \n", __func__,
                 __LINE__);
        return;
    }

    /* Alocate Memory to RouteInfo */
    if (NULL == SR_RTINFO_MEM_ALLOC (pRouteInfo))
    {
        SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                 "%s:%d Memory Allocation for Ip Route Info failed \n", __func__,
                 __LINE__);
        return;
    }

    switch (pNetIpv6HlParams->u4Command)
    {
        case NETIPV6_ROUTE_CHANGE:
            MEMCPY (pRouteInfo->destAddr.Addr.Ip6Addr.u1_addr,
                    pNetIpv6HlParams->unIpv6HlCmdType.RouteChange.Ip6Dst.
                    u1_addr, SR_IPV6_ADDR_LENGTH);

            pRouteInfo->u4DestMask =
                pNetIpv6HlParams->unIpv6HlCmdType.RouteChange.u1Prefixlen;

            MEMCPY (pRouteInfo->nextHop.Addr.Ip6Addr.u1_addr,
                    pNetIpv6HlParams->unIpv6HlCmdType.RouteChange.NextHop.
                    u1_addr, SR_IPV6_ADDR_LENGTH);

            pRouteInfo->u4RtIfIndx =
                pNetIpv6HlParams->unIpv6HlCmdType.RouteChange.u4Index;
            pRouteInfo->i4Metric1 =
                (INT4) pNetIpv6HlParams->unIpv6HlCmdType.RouteChange.u4Metric;
            pRouteInfo->u4RowStatus =
                pNetIpv6HlParams->unIpv6HlCmdType.RouteChange.u4RowStatus;
            pRouteInfo->u2AddrType = SR_IPV6_ADDR_TYPE;

            switch (pNetIpv6HlParams->unIpv6HlCmdType.RouteChange.u4RowStatus)
            {
                case IP6FWD_DESTROY:
                    SrQMsg.u.SrIpEvtInfo.u4BitMap = SR_RT_DELETED;
                    SR_TRC2 (SR_UTIL_TRC, "%s:%d Route delete event reveived \n",
                             __func__, __LINE__);
                    break;
                case IP6FWD_ACTIVE:
                    SrQMsg.u.SrIpEvtInfo.u4BitMap = SR_RT_NEW;
                    SR_TRC2 (SR_UTIL_TRC, "%s:%d Route Add event reveived \n",
                             __func__, __LINE__);
                    break;
                default:
                    SR_RTINFO_MEM_FREE (pRouteInfo);
                    return;
            }
            break;
        default:
            SR_RTINFO_MEM_FREE (pRouteInfo);
            return;
    }

    SrQMsg.u4MsgType = SR_IP6_EVENT;

    if (pRouteInfo->u4RtIfIndx != SR_ZERO)
    {

#ifdef IP6_WANTED
        if (NetIpv6GetCfaIfIndexFromPort (pRouteInfo->u4RtIfIndx,
                                          &SrQMsg.u.SrIpEvtInfo.u4IfIndex)
            == NETIPV6_FAILURE)
#endif
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d No Port to IfIndex Mapping \n", __func__,
                     __LINE__);
            SR_RTINFO_MEM_FREE (pRouteInfo);
            return;
        }
    }
    else
    {
        SR_RTINFO_MEM_FREE (pRouteInfo);
        return;
    }

    SR_TRC4 (SR_UTIL_TRC,
             "%s:%d pRouteInfo->u4RtIfIndx %d SrQMsg.u.SrIpEvtInfo.u4IfIndex %d \n",
             __func__, __LINE__, pRouteInfo->u4RtIfIndx,
             SrQMsg.u.SrIpEvtInfo.u4IfIndex);

    SrQMsg.u.SrIpEvtInfo.pRouteInfo = (UINT1 *) pRouteInfo;

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to EnQ IP Event to SR Task \n",
                 __func__, __LINE__);
        SR_RTINFO_MEM_FREE (pRouteInfo);
    }
    return;
}

PUBLIC UINT4
SrEnqueueMsgToSrQ (UINT1 u1EvtType, tSrQMsg * pSrQMsg)
{
    tSrQMsg            *pSrQMsgNew = NULL;

    /* Alocate Memory to QMsg */
    if (NULL == SR_Q_MEM_ALLOC (pSrQMsgNew))
    {
        SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC, "%s:%d Memory Allocation failed for Q-Msg \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }

    MEMCPY (pSrQMsgNew, (UINT1 *) pSrQMsg, sizeof (tSrQMsg));

    if (OsixQueSend
        (gSrGlobalInfo.gSrQId, (UINT1 *) (&pSrQMsgNew),
         OSIX_DEF_MSG_LEN) != OSIX_SUCCESS)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to EnQ the Msg to SR Q \n", __func__,
                 __LINE__);
        SR_Q_MEM_FREE (pSrQMsgNew);
        return SR_FAILURE;
    }

    if (OsixEvtSend (SR_TASK_ID, u1EvtType) == OSIX_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to Send the Event to SR Q \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

PUBLIC UINT4
SrProcessIp6Events (tSrIpEvtInfo * pSrIpEvtInfo)
{
    tGenU4Addr          destAddr;
    tGenU4Addr          NextHop;

    tTMO_SLL_NODE      *pRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tTMO_SLL_NODE      *pSrRtrTmpNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrTmpNextHopInfo = NULL;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrRtrInfo         *pNewRtrNode = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRtrInfo         *pTempSrRtrInfo = NULL;

    tSrRouteEntryInfo  *pRouteInfo = NULL;

    tSrInSegInfo        SrInSegInfo;
    INT4                i4SrMode = SR_MODE_DEFAULT;
    UINT4               u4DestAddr = SR_ZERO;
    UINT4               u4RtrId = SR_ZERO;
    UNUSED_PARAM (u4RtrId);
    UINT4               u4SrGbMinIndex = SR_ZERO;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4ErrCode = SR_ZERO;
    UINT1               u1NHFound = SR_FALSE;
    UINT1               u1TmpFtnFound = SR_FALSE;
    UINT1               u1TmpNHFound = SR_FALSE;
    tGenU4Addr          tmpAddr;
    MEMSET (&tmpAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&NextHop, SR_ZERO, sizeof (tGenU4Addr));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);
    if (pSrIpEvtInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d  pSrIpEvtInfo is NULL \n",
                __func__,__LINE__);
        return SR_FAILURE;
    }

    if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
    {
        u4SrGbMinIndex = gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;

        SrInSegInfo.Fec.u2AddrFmly =
            gSrGlobalInfo.pV6SrSelfNodeInfo->ipAddrType;
        MEMCPY (SrInSegInfo.Fec.Prefix.Ip6Addr.u1_addr,
                gSrGlobalInfo.pV6SrSelfNodeInfo->ifIpAddr.Addr.Ip6Addr.u1_addr,
                MAX_IPV6_ADDR_LEN);
    }

    pRouteInfo = (tSrRouteEntryInfo *) (VOID *) pSrIpEvtInfo->pRouteInfo;
    MEMCPY (destAddr.Addr.Ip6Addr.u1_addr,
            pRouteInfo->destAddr.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
    destAddr.u2AddrType = SR_IPV6_ADDR_TYPE;

    u4OutIfIndex = pSrIpEvtInfo->u4IfIndex;

    nmhGetFsSrMode (&i4SrMode);

    if (pSrIpEvtInfo->u4BitMap == SR_RT_NEW)
    {
        SR_TRC2 (SR_UTIL_TRC, "Route ADD rt=%s and next hop = %s \r\n",
                 Ip6PrintAddr (&pRouteInfo->destAddr.Addr.Ip6Addr),
                 Ip6PrintAddr (&pRouteInfo->nextHop.Addr.Ip6Addr));

        /* Sr-TE: Start */

        MEMCPY (tmpAddr.Addr.Ip6Addr.u1_addr,
                pRouteInfo->destAddr.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        tmpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        /* To create TE LSP in the route event flow */
        if (SrTeCreateFtnRtChangeEvt (&tmpAddr, &u4ErrCode) == SR_SUCCESS)
        {
            if (u4ErrCode == SR_TE_VIA_DEST)
            {
                SR_TRC3 (SR_UTIL_TRC,
                         "%s:%d SR TE LSP processed via dest for prefix %s \n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&tmpAddr.Addr.Ip6Addr));
                return SR_SUCCESS;
            }
        }
        else
        {
            if (u4ErrCode == SR_TE_VIA_DEST)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d SR TE LSP processing via dest for prefix %s is failed \n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&tmpAddr.Addr.Ip6Addr));
                return SR_FAILURE;
            }
        }
        /* Sr-TE: End */

        /* SR LSP processing for non-TE starts */
        pSrRlfaPathInfo = SrGetRlfaPathEntryFromDestAddr (&destAddr);
        if (pSrRlfaPathInfo != NULL)
        {
            /* If RLFA is active for the same route then Delete it */
            if (pSrRlfaPathInfo->bIsRlfaActive == SR_ONE)
            {
                if (MEMCMP (pSrRlfaPathInfo->rlfaNextHop.Addr.Ip6Addr.u1_addr,
                            pRouteInfo->nextHop.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                {
                    MEMCPY (NextHop.Addr.Ip6Addr.u1_addr,
                            pRouteInfo->nextHop.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    NextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
                    if (SrRlfaDeleteFtnEntry (&destAddr, &NextHop) ==
                        SR_SUCCESS)
                    {
                        SR_TRC2 (SR_UTIL_TRC,
                                 "%s:%d Deletion of RLFA FTN success \n", __func__,
                                 __LINE__);
                    }
                    if (SrRlfaDeleteILM (&destAddr, &pSrRlfaPathInfo->mandRtrId,
                                         &pSrRlfaPathInfo->rlfaNextHop,
                                         &SrInSegInfo) == SR_SUCCESS)
                    {
                        SR_TRC2 (SR_UTIL_TRC,
                                 "%s:%d Deletion of RLFA ILM success \n", __func__,
                                 __LINE__);
                    }
                }
            }
        }

        /* Sr-Non-TE: Start */
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            pRtrInfo = (tSrRtrInfo *) pRtrNode;

            if (MEMCMP
                (destAddr.Addr.Ip6Addr.u1_addr,
                 pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr,
                 SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                pNewRtrNode = pRtrInfo;

                TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                              pSrRtrNextHopNode, tTMO_SLL_NODE *)
                {
                    pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

                    if (MEMCMP
                        (pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                         pRouteInfo->nextHop.Addr.Ip6Addr.u1_addr,
                         SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                    {
                        u1NHFound = SR_TRUE;
                        break;
                    }
                }

                SR_TRC5 (SR_UTIL_TRC,
                         "%s:%d SID %d SrGbMinIndex %d u1NHFound %d \n", __func__,
                         __LINE__, pNewRtrNode->u4SidValue,
                         pNewRtrNode->srgbRange.u4SrGbV3MinIndex, u1NHFound);
                if ((u1NHFound == SR_FALSE)
                    && (pNewRtrNode->u4SidValue != SR_ZERO))
                {
                    /*Create new Next-Hop Node */
                    if ((pNewSrRtrNextHopInfo =
                         SrCreateNewNextHopNode ()) == NULL)
                    {
                        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                                 "%s:%d pNewSrRtrNextHopInfo == NULL \n", __func__,
                                 __LINE__);
                        return SR_FAILURE;
                    }
                    /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
                    MEMCPY (pNewSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                            pRouteInfo->nextHop.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);

                    /* Bug: As pRouteInfo->u4RtIfIndex is coming Incorrect,
                     * a temporary has been intrudcued to find outIfindex from nexthop
                     * from NeighborTable*/
                    /* Temp Fix Start */
                    /*
                       SrGetOutIfFromNextHop (pRouteInfo->u4NextHop, &u4OutIfIndex);
                     */
                    /* Temp Fix End */
                    pNewSrRtrNextHopInfo->nextHop.u2AddrType =
                        SR_IPV6_ADDR_TYPE;
                    pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                    pNewSrRtrNextHopInfo->u4SwapOutIfIndex = u4OutIfIndex;

                    /* Add NextHop Node in NextHopList in SrRtrNode */
                    TMO_SLL_Insert (&(pNewRtrNode->NextHopList), NULL,
                                    &(pNewSrRtrNextHopInfo->nextNextHop));
                    /* Create FTN */
                    if (SrMplsCreateFTN (pNewRtrNode, pNewSrRtrNextHopInfo) ==
                        SR_FAILURE)
                    {
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateFTN  FAILURE \n",
                                 __func__, __LINE__);

                        /* FTN & POP&FWD cannot co-exist (Preventive check as sometime MLIBUPDATE API failing "to do") */
                        if ((pNewSrRtrNextHopInfo->
                             u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
                        {
                            continue;
                        }

                        /* If Next-Hop is not SR Enabled Create ILM-Pop&Fwd and return */
                        pTempSrRtrInfo =
                            SrGetRtrInfoFromNextHop (&pRouteInfo->nextHop);

                        if (pTempSrRtrInfo == NULL)
                        {
                            /*ECMP Check: Before creating Pop n Fwd, check if ILM Swap is already created for this DestAddr
                             * Scenario:  ECMP DestAddr, ILM Swap Already created from one side, SR shut from other side,
                             *            Now Rt Add come from Sr_Shut side*/

                            if (TMO_SLL_Count (&pNewRtrNode->NextHopList) >
                                SR_ONE)
                            {
                                u1TmpFtnFound = SR_FALSE;

                                TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                                              pSrRtrTmpNextHopNode,
                                              tTMO_SLL_NODE *)
                                {
                                    pSrRtrTmpNextHopInfo =
                                        (tSrRtrNextHopInfo *)
                                        pSrRtrTmpNextHopNode;
                                    if ((pSrRtrTmpNextHopInfo->
                                         u1MPLSStatus & SR_FTN_CREATED) ==
                                        SR_FTN_CREATED)
                                    {
                                        SR_TRC2 (SR_UTIL_TRC,
                                                 "%s:%d FTN already present \n",
                                                 __func__, __LINE__);
                                        u1TmpFtnFound = SR_TRUE;
                                        break;
                                    }
                                }

                                if (u1TmpFtnFound == SR_TRUE)
                                {
                                    /* This implies, Already atleast one FTN is created, and ILM swap will also be there
                                     * so no need to create ILM Pop n Fwd */
                                    continue;
                                    /*continue to SrRtrInfo Loop */
                                }
                            }

                            pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                            SR_TRC2 (SR_UTIL_TRC,
                                     "%s:%d pTempSrRtrInfo == NULL: ~ Next-Hop is not SR Enabled (Create: Pop&Fwd) ~ \n",
                                     __func__, __LINE__);
                            if (SrMplsCreateILMPopAndFwd
                                (pNewRtrNode,
                                 pNewSrRtrNextHopInfo) == SR_SUCCESS)
                            {
                                SR_TRC2 (SR_UTIL_TRC,
                                         "%s:%d SrMplsCreateILMPopAndFwd == SUCCESS \n",
                                         __func__, __LINE__);
                                return SR_SUCCESS;
                            }
                            else
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d POP&FWD Create Failure \n",
                                         __func__, __LINE__);
                            }
                        }
                        /* Delete the Node from the list in case of FTN creation failure */
                        TMO_SLL_Delete (&(pNewRtrNode->NextHopList),
                                        (tTMO_SLL_NODE *) pNewSrRtrNextHopInfo);
                        SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                        return SR_FAILURE;
                    }

                    if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
                    {
                        /*ECMP Case for POP n FWD Deletion if SWAP is to be created */
                        if (TMO_SLL_Count (&pNewRtrNode->NextHopList) !=
                            SR_ZERO)
                        {
                            TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                                          pSrRtrTmpNextHopNode, tTMO_SLL_NODE *)
                            {
                                pSrRtrTmpNextHopInfo =
                                    (tSrRtrNextHopInfo *) pSrRtrTmpNextHopNode;
                                if ((pSrRtrTmpNextHopInfo->
                                     u1MPLSStatus & SR_ILM_POP_CREATED) ==
                                    SR_ILM_POP_CREATED)
                                {
                                    SR_TRC2 (SR_UTIL_TRC,
                                             "%s:%d ILM POP n FWD already present \n",
                                             __func__, __LINE__);
                                    u1TmpNHFound = SR_TRUE;
                                    break;
                                }
                            }
                        }

                        if (u1TmpNHFound == SR_TRUE)
                        {
                            if (SrMplsDeleteILMPopAndFwd
                                (pNewRtrNode,
                                 pSrRtrTmpNextHopInfo) == SR_SUCCESS)
                            {
                                SR_TRC2 (SR_UTIL_TRC,
                                         "%s:%d POP&FWD Deleted; Removing from SrRtrNhList \n",
                                         __func__, __LINE__);

                                TMO_SLL_Delete (&(pNewRtrNode->NextHopList),
                                                (tTMO_SLL_NODE *)
                                                pSrRtrTmpNextHopInfo);
                                SR_RTR_NH_LIST_FREE (pSrRtrTmpNextHopInfo);
                            }
                            else
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d POP&FWD Delete Failure \n",
                                         __func__, __LINE__);
                                continue;
                            }
                        }

                        SR_TRC2 (SR_UTIL_TRC, "%s:%d Creating ILM SWAP \n",
                                 __func__, __LINE__);
                        if (SrMplsCreateILM (pNewRtrNode, pNewSrRtrNextHopInfo)
                            == SR_FAILURE)
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SrMplsCreateILM  FAILURE \n", __func__,
                                     __LINE__);
                        }
                    }
                    break;
                }
            }
            else
            {
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d PeerId do not matches with Route Add Event \n",
                         __func__, __LINE__);
            }
        }
        /* Sr-Non-TE: Ends */
    }
    else if (pSrIpEvtInfo->u4BitMap == SR_RT_DELETED)
    {
        SR_TRC2 (SR_UTIL_TRC, "Route DEL rt=%s and next hop = %s \r\n",
                 Ip6PrintAddr (&pRouteInfo->destAddr.Addr.Ip6Addr),
                 Ip6PrintAddr (&pRouteInfo->nextHop.Addr.Ip6Addr));
        /* If Pop&Fwd Created with same NH, then delete the Pop & Fwd & return
         * "return SR_Success": is done because POP&FWD and FTN cannot be present simultaneously*/
        MEMCPY (tmpAddr.Addr.Ip6Addr.u1_addr,
                pRouteInfo->destAddr.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        tmpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;

        MEMCPY (&(NextHop.Addr.u4Addr), &(pRouteInfo->nextHop.Addr.u4Addr),
                SR_IPV4_ADDR_LENGTH);
        NextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        if (SrDeletePopNFwdUsingRtInfo (&tmpAddr, &NextHop) == SR_SUCCESS)
        {
            return SR_SUCCESS;
        }
        /* Sr-TE: Start */

        pRouteInfo->nextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
        /* To delete TE LSP in the route event flow */
        if (SrTeDeleteFtnRtChangeEvt
            (&tmpAddr, &u4ErrCode, &pRouteInfo->nextHop) == SR_SUCCESS)
        {
            if (u4ErrCode == SR_TE_VIA_DEST)
            {
                SR_TRC3 (SR_UTIL_TRC,
                         "%s:%d SR TE LSP deleted via dest in Type10 flow for prefix %s \n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&tmpAddr.Addr.Ip6Addr));

                return SR_SUCCESS;
            }
        }
        else
        {
            if (u4ErrCode == SR_TE_VIA_DEST)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d SR TE LSP deletion via dest for prefix %s is failed \n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&tmpAddr.Addr.Ip6Addr));

                return SR_FAILURE;
            }
        }
        /* Sr-TE: End */

        /* Sr-Non-TE: Starts */
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            pRtrInfo = (tSrRtrInfo *) pRtrNode;

            if (MEMCMP
                (destAddr.Addr.Ip6Addr.u1_addr,
                 pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr,
                 SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                pNewRtrNode = pRtrInfo;

                TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                              pSrRtrNextHopNode, tTMO_SLL_NODE *)
                {
                    pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
                    if ((MEMCMP
                         (pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                          pRouteInfo->nextHop.Addr.Ip6Addr.u1_addr,
                          SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                        && (pSrRtrNextHopInfo->bIsLfaNextHop != TRUE))
                    {
                        u1NHFound = SR_TRUE;
                        break;
                    }
                }

                if (u1NHFound == SR_FALSE)
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                             "%s:%d u1NHFound == SR_FALSE: SR_FAILURE \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }

                SR_TRC3 (SR_UTIL_TRC, "%s:%d u1NHFound == TRUE; MplsStatus %x \n",
                         __func__, __LINE__, pSrRtrNextHopInfo->u1MPLSStatus);

                if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                    SR_FTN_CREATED)
                {
                    if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
                    {
                        /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
                        SrInSegInfo.u4InLabel =
                            (u4SrGbMinIndex + pNewRtrNode->u4SidValue);

                        SR_TRC2 (SR_UTIL_TRC, "%s:%d Deleting ILM SWAP \n",
                                 __func__, __LINE__);

                        if (SrMplsDeleteILM
                            (&SrInSegInfo, pNewRtrNode,
                             pSrRtrNextHopInfo) == SR_FAILURE)
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SrMplsDeleteILM  FAILURE \n", __func__,
                                     __LINE__);
                        }
                    }

                    SR_TRC3 (SR_UTIL_TRC,
                             "%s:%d ILM SWAP DELETED, Deleting FTN for %x \n",
                             __func__, __LINE__, u4DestAddr);

                    if (SrMplsDeleteFTN (pNewRtrNode, pSrRtrNextHopInfo) ==
                        SR_FAILURE)
                    {
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsDeleteFTN  FAILURE \n",
                                 __func__, __LINE__);
                        return SR_FAILURE;
                    }
                    /* Remove SrRtrNhNode from NextHopList */
                    TMO_SLL_Delete (&(pNewRtrNode->NextHopList),
                                    (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                    SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);

                    /*ECMP Case for ILM Creation */
                    if (TMO_SLL_Count (&pNewRtrNode->NextHopList) != SR_ZERO)
                    {
                        TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                                      pSrRtrTmpNextHopNode, tTMO_SLL_NODE *)
                        {
                            pSrRtrTmpNextHopInfo =
                                (tSrRtrNextHopInfo *) pSrRtrTmpNextHopNode;
                            if ((pSrRtrTmpNextHopInfo->
                                 u1MPLSStatus & SR_FTN_CREATED) ==
                                SR_FTN_CREATED)
                            {
                                u1TmpNHFound = SR_TRUE;
                                break;
                            }
                        }

                        if (u1TmpNHFound == SR_TRUE)
                        {
                            if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
                            {
                                SR_TRC2 (SR_UTIL_TRC,
                                         "%s:%d Creating ILM SWAP for next ECMP FTN \n",
                                         __func__, __LINE__);
                                if (SrMplsCreateILM
                                    (pNewRtrNode,
                                     pSrRtrTmpNextHopInfo) == SR_FAILURE)
                                {
                                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                             "%s:%d SrMplsCreateILM  FAILURE \n",
                                             __func__, __LINE__);
                                }
                                else
                                {
                                    SR_TRC2 (SR_UTIL_TRC,
                                             "%s:%d SrMplsCreateILM  SUCCESS: for next ECMP FTN \n",
                                             __func__, __LINE__);
                                }
                            }
                        }
                    }
                    break;
                }
                else
                {
                    SR_TRC2 (SR_UTIL_TRC,
                             "%s:%d No FTN Created , Hence No Action \n", __func__,
                             __LINE__);
                }
            }
            else
            {
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d PeerId do not matches with Route Delete Event \n",
                         __func__, __LINE__);
            }

        }
        /* Sr-Non-TE: Ends */
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

PUBLIC UINT4
SrProcessIpEvents (tSrIpEvtInfo * pSrIpEvtInfo)
{

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);
    if (pSrIpEvtInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pSrIpEvtInfo is NULL \n", __func__,
                __LINE__);
        return SR_FAILURE;
    }

    switch (pSrIpEvtInfo->u4BitMap)
    {
        case SR_RT_NEW:
        case SR_RT_DELETED:
            if (SrProcessRouteChangeEvent (pSrIpEvtInfo) == SR_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                        "%s:%d SrProcessRouteChangeEvent Failed for pSrIpEvtInfo \n",
                        __func__,__LINE__);
                return SR_FAILURE;
            }
            break;
        case SR_RT_NH_CHG:
            if (SrProcessRouteNextHopChangeEvent (pSrIpEvtInfo) == SR_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                        "%s:%d SrProcessRouteNextHopChangeEvent Failed for pSrIpEvtInfo \n",
                        __func__,__LINE__);
                return SR_FAILURE;
            }
            break;
        case SR_RT_FRR_NH_CHG:
            if (SrProcessFRRModifyRouteEvent (pSrIpEvtInfo) == SR_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                        "%s:%d SrProcessFRRModifyRouteEvent Failed for pSrIpEvtInfo \n",
                        __func__,__LINE__);
                return SR_FAILURE;
            }
            break;
        default:
            break;

    };

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrProcessNetIp6LfaEvents
 * Description   : This routine process the SR Main queue LFA event
 *
 * Input(s)      : pSrNetIp6LfaRtInfo - IPv6 Lfa Route Info
 *
 * Output(s)     :
 * Return(s)     : None
 *****************************************************************************/
VOID
SrProcessNetIp6LfaEvents (tSrNetIpv6LfaRtInfo * pSrNetIp6LfaRtInfo)
{

    tNetIpv6LfaRtInfo   NetIpv6LfaRtInfo;

    MEMSET (&NetIpv6LfaRtInfo, 0, sizeof (tNetIpv6LfaRtInfo));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);
    if (pSrNetIp6LfaRtInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d  pSrNetIp6LfaRtInfo is NULL \n", __func__,
                 __LINE__);
        return;
    }

    MEMCPY (&(NetIpv6LfaRtInfo.Ip6Dst),
            &pSrNetIp6LfaRtInfo->Ip6Dst, sizeof (tIp6Addr));
    MEMCPY (&(NetIpv6LfaRtInfo.NextHop),
            &pSrNetIp6LfaRtInfo->NextHop, sizeof (tIp6Addr));
    MEMCPY (&(NetIpv6LfaRtInfo.LfaNextHop),
            &pSrNetIp6LfaRtInfo->LfaNextHop, sizeof (tIp6Addr));
    MEMCPY (&(NetIpv6LfaRtInfo.AltRlfaNxtHop),
            &pSrNetIp6LfaRtInfo->AltRlfaNxtHop, sizeof (tIp6Addr));

    NetIpv6LfaRtInfo.u1Prefixlen = pSrNetIp6LfaRtInfo->u1Prefixlen;
    NetIpv6LfaRtInfo.u4Index = pSrNetIp6LfaRtInfo->u4Index;
    NetIpv6LfaRtInfo.uRemoteNodeId.u4RemNodeRouterId =
            pSrNetIp6LfaRtInfo->uRemoteNodeId.u4RemNodeRouterId;
    NetIpv6LfaRtInfo.u4Metric = pSrNetIp6LfaRtInfo->u4Metric;
    NetIpv6LfaRtInfo.u4AltCost = pSrNetIp6LfaRtInfo->u4AltCost;
    NetIpv6LfaRtInfo.u1CmdType = pSrNetIp6LfaRtInfo->u1CmdType;
    NetIpv6LfaRtInfo.i1Proto = pSrNetIp6LfaRtInfo->i1Proto;
    NetIpv6LfaRtInfo.u4AltIfIndex = pSrNetIp6LfaRtInfo->u4AltIfIndex;
    NetIpv6LfaRtInfo.u1LfaType = pSrNetIp6LfaRtInfo->u1LfaType;

    NetIpv6LfaRtInfo.u4ContextId = pSrNetIp6LfaRtInfo->u4ContextId;

    /*Procces LFA/RLFA IPv6 Event */
    SrProcessLfaIpv6RtChangeEvent (&NetIpv6LfaRtInfo);
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return;
}

UINT4
SrProcessRouteChangeEvent (tSrIpEvtInfo * pSrIpEvtInfo)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tTMO_SLL_NODE      *pSrRtrTmpNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrTmpNextHopInfo = NULL;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pTeTempSrRtInfo = NULL;
    tSrRtrInfo         *pNewRtrNode = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrRtrInfo         *pTempSrRtrInfo = NULL;
    tSrTeRtEntryInfo   *pSrTeRtEntryInfo = NULL;
    tSrRouteEntryInfo  *pRouteInfo = NULL;
    tGenU4Addr          destAddr;
    tGenU4Addr          NextHop;
    tGenU4Addr          tmpAddr;
    tSrInSegInfo        SrInSegInfo;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tSrTeRtEntryInfo    getSrTeRtEntryInfo;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tSrSidInterfaceInfo   *pSrSidIntf = NULL;
    UINT4               i4SrMode = SR_MODE_DEFAULT;
    UINT4               u4DestAddr = SR_ZERO;
    UINT4               u4RtrId = SR_ZERO;
    UINT4               u4SrGbMinIndex = SR_ZERO;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4ErrCode = SR_ZERO;
    UINT1               u1NHFound = SR_FALSE;
    UINT1               u1TmpFtnFound = SR_FALSE;
    UINT1               u1TmpNHFound = SR_FALSE;
    UINT4               u4TempNbrRtrId = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    UINT1               u1IsTeLspProcessed = SR_ZERO;

    MEMSET (&tmpAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&NextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&getSrTeRtEntryInfo, 0, sizeof (tSrTeRtEntryInfo));

    SR_TRC2 (SR_CTRL_TRC | SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY\n", __func__, __LINE__);
    if (pSrIpEvtInfo == NULL)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d pSrIpEvtInfo is NULL \n", __func__,
                __LINE__);
        return SR_FAILURE;
    }

    pRouteInfo = (tSrRouteEntryInfo *) (VOID *) pSrIpEvtInfo->pRouteInfo;

    MEMCPY (&(destAddr.Addr.u4Addr), &(pRouteInfo->destAddr.Addr.u4Addr),
            SR_IPV4_ADDR_LENGTH);
    destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    u4DestAddr = pRouteInfo->destAddr.Addr.u4Addr;
    u4OutIfIndex = pSrIpEvtInfo->u4IfIndex;

    nmhGetFsSrMode ((INT4 *) &i4SrMode);

    if (pSrIpEvtInfo->u4BitMap == SR_RT_NEW)
    {
        MEMCPY (&tmpAddr.Addr.u4Addr, &u4DestAddr, sizeof (UINT4));
        tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;

        /* Sr-TE: Start */
        /* Sr-TE ILM Starts */
        /* For TE Destination check if SR-FRR ILM is ACtive , then check Sr-FRR Nexthop is same as new Primary NextHop */
        if (((pSrTeRtInfo = SrGetTeRouteEntryFromDestAddr (&tmpAddr)) != NULL)
            && ((pDestSrRtrInfo = SrGetSrRtrInfoFromRtrId (&tmpAddr)) != NULL))
        {
            if ((pDestSrRtrInfo->bIsLfaActive == SR_TRUE)
                && (pDestSrRtrInfo->u4LfaNextHop ==
                    pRouteInfo->nextHop.Addr.u4Addr))
            {
                /*  Modify TE Ilm alone */
                if (SrTeMplsModifyLSP (pDestSrRtrInfo, pSrTeRtInfo, SR_FALSE) ==
                    SR_FAILURE)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SrTeMplsModifyLSP Failure for %x \n",
                             __func__, __LINE__, tmpAddr.Addr.u4Addr);
                }
            }
        }
        /* Sr-TE ILM Ends */

        /* SR TE FTN Processing via Mandatory node */
        while ((pSrTeRtEntryInfo =
                SrGetNextTeRouteTableEntry (&getSrTeRtEntryInfo)) != NULL)
        {
            if (pSrTeRtEntryInfo->mandRtrId.Addr.u4Addr == tmpAddr.Addr.u4Addr)
            {
                /* Rt_Add Evt came for Mandatory RtrId, So Process SR-TE routes */

                if (pSrTeRtEntryInfo->bIsSrTeFRRActive == SR_TRUE)
                {
                    SR_TRC4 (SR_CTRL_TRC,
                             "%s:%d Rt_Add Event came for Mandatory RtrId %x "
                             " -- Processing FRR Active TE Route %x \r\n",
                             __func__, __LINE__, tmpAddr.Addr.u4Addr,
                             pSrTeRtEntryInfo->destAddr.Addr.u4Addr);

                    if (pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr ==
                        pRouteInfo->nextHop.Addr.u4Addr)
                    {
                        SR_TRC4 (SR_CTRL_TRC,
                                 "%s:%d Lfa is active for Mandatory RtrId %x and Lfa NextHop is matched with New Nexthop %x \r\n",
                                 __func__, __LINE__, tmpAddr.Addr.u4Addr,
                                 pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr);

                        if ((pTeTempSrRtInfo =
                             SrGetSrRtrInfoFromRtrId (&pSrTeRtEntryInfo->
                                                      destAddr)) != NULL)
                        {
                            /*  Modify TE FTN alone */
                            if (SrTeMplsModifyLSP
                                (pTeTempSrRtInfo, pSrTeRtEntryInfo,
                                 SR_TRUE) == SR_FAILURE)
                            {
                                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                                         "%s:%d SrTeMplsModifyLSP Failure for %x \n",
                                         __func__, __LINE__,
                                         pSrTeRtEntryInfo->destAddr.Addr.
                                         u4Addr);
                            }
                            /* TE LSPs are Modified so no need of processing in SrTeCreateFtnRtChangeEvt */
                            u1IsTeLspProcessed = 1;
                        }
                    }
                    else
                    {
                        SR_TRC4 (SR_CTRL_TRC,
                                 "%s:%d Rt_Add Event came for Mandatory RtrId %x with New NextHop not same as Lfa Nexthop "
                                 "for FRR Active TE Route %x \r\n", __func__,
                                 __LINE__, tmpAddr.Addr.u4Addr,
                                 pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                        /* Delete TE LFA FTN */
                        if (SrTeLfaDeleteFtnIlm (pSrTeRtEntryInfo) ==
                            SR_FAILURE)
                        {
                            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SR TE LFA lsp deletion"
                                     " failed for prefix %x \n", __func__,
                                     __LINE__,
                                     pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                        }
                        /* Delete TE PRIMARY FTN */
                        if (SrTeFtnIlmCreateDelViaDestRt
                            (pSrTeRtEntryInfo, SR_FALSE) != SR_SUCCESS)
                        {
                            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SR TE LSP deletion FAILED for prefix %x \n",
                                     __func__, __LINE__, tmpAddr.Addr.u4Addr);
                        }
                    }
                }
                else
                {

                    SR_TRC4 (SR_CTRL_TRC,
                             "%s:%d Rt_Add Event came for Mandatory RtrId %x "
                             " -- Processing Non-FRR Active TE Route %x \r\n",
                             __func__, __LINE__, tmpAddr.Addr.u4Addr,
                             pSrTeRtEntryInfo->destAddr.Addr.u4Addr);

                    if (pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr ==
                        pRouteInfo->nextHop.Addr.u4Addr)
                    {
                        if (SrTeLfaDeleteFtnIlm (pSrTeRtEntryInfo) ==
                            SR_FAILURE)
                        {
                            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SR TE LFA lsp deletion"
                                     " failed for prefix %x \n", __func__,
                                     __LINE__,
                                     pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                        }

                    }
                }
            }
            MEMSET (&getSrTeRtEntryInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
            getSrTeRtEntryInfo.destAddr.Addr.u4Addr =
                pSrTeRtEntryInfo->destAddr.Addr.u4Addr;
            getSrTeRtEntryInfo.mandRtrId.Addr.u4Addr =
                pSrTeRtEntryInfo->mandRtrId.Addr.u4Addr;
            getSrTeRtEntryInfo.destAddr.u2AddrType =
                pSrTeRtEntryInfo->destAddr.u2AddrType;
            getSrTeRtEntryInfo.mandRtrId.u2AddrType =
                pSrTeRtEntryInfo->mandRtrId.u2AddrType;
        }
        /* Process Rt_Add for TE if LSPs are not already Modified  */
        if (u1IsTeLspProcessed == 0)
        {
            if (SrTeCreateFtnRtChangeEvt (&tmpAddr, &u4ErrCode) == SR_SUCCESS)
            {
                if (u4ErrCode == SR_TE_VIA_DEST)
                {
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d SR TE LSP processed via dest for prefix %x \n",
                             __func__, __LINE__, tmpAddr.Addr.u4Addr);
                    return SR_SUCCESS;
                }

            }
            else
            {
                if (u4ErrCode == SR_TE_VIA_DEST)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d SR TE LSP processing via dest for prefix %x is failed \n",
                             __func__, __LINE__, tmpAddr.Addr.u4Addr);
                    return SR_FAILURE;
                }
            }
        }
        /* Sr-TE: End */

        /* Sr-Non-TE: Start */
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            pRtrInfo = (tSrRtrInfo *) pRtrNode;
            u1NHFound = SR_FALSE;
            pTempSrRtrInfo = NULL;

            MEMCPY (&u4RtrId, &pRtrInfo->prefixId, sizeof (UINT4));

            if (u4DestAddr == u4RtrId)
            {
                pNewRtrNode = pRtrInfo;

                if (SR_TRUE == gSrGlobalInfo.bIsMicroLoopAvd)
                {
                    if ((pRtrInfo->u1OprFlags & SR_OPR_F_ADD_DELAY_LIST)
                                                == SR_OPR_F_ADD_DELAY_LIST)
                    {
                        SRProcessDelayedRouteInfo(pRtrInfo);
                    }
                }
                TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                              pSrRtrNextHopNode, tTMO_SLL_NODE *)
                {
                    pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

                    if ((MEMCMP
                         (&(pSrRtrNextHopInfo->nextHop.Addr.u4Addr),
                          &(pRouteInfo->nextHop.Addr.u4Addr),
                          SR_IPV4_ADDR_LENGTH) == SR_ZERO)
                        && (pSrRtrNextHopInfo->bIsLfaNextHop != TRUE))
                    {
                        u1NHFound = SR_TRUE;
                        break;
                    }
                }

                /* If New primary next hop is same as the LFA next hop,
                 * then the LFA FTN, ILM needs to be deleted in mpls,
                 * so, that the primary FTN, ILM can be created in mpls and in HW */
                if ((pRtrInfo->u4LfaNextHop == pRouteInfo->nextHop.Addr.u4Addr))
                {
                    SR_TRC2 (SR_CTRL_TRC, "%s:%d Deleting the LFA FTN and ILM \n",
                             __func__, __LINE__);
                    SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;
                    SrRtrAltNextHopInfo.u4OutIfIndex =
                        pRtrInfo->u4LfaOutIfIndex;
                    SrRtrAltNextHopInfo.u4SwapOutIfIndex =
                        pRtrInfo->u4LfaSwapOutIfIndex;
                    SrRtrAltNextHopInfo.nextHop.Addr.u4Addr =
                        pRtrInfo->u4LfaNextHop;
                    SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
                    SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
                    SrInSegInfo.u4InLabel =
                        (u4SrGbMinIndex + pRtrInfo->u4SidValue);

                    if (SrMplsDeleteILM (&SrInSegInfo, pRtrInfo,
                                         &SrRtrAltNextHopInfo) == SR_FAILURE)
                    {
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                 "%s:%d Unable to delete LFA ILM SWAP \n",
                                 __func__, __LINE__);
                        return SR_FAILURE;
                    }
                    if (SrMplsCreateOrDeleteLsp (MPLS_MLIB_FTN_DELETE,
                                                 pRtrInfo, NULL,
                                                 &SrRtrAltNextHopInfo) ==
                        SR_SUCCESS)
                    {
                        SR_TRC2 (SR_CTRL_TRC,
                                 "%s:%d Deletion of LFA FTN successfull \n",
                                 __func__, __LINE__);
                    }
                    else
                    {
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                 "%s:%d Deletion of LFA FTN failed \n",
                                 __func__, __LINE__);
                        return SR_FAILURE;
                    }

                    if (CfaUtilGetIfIndexFromMplsTnlIf
                        (SrRtrAltNextHopInfo.u4OutIfIndex, &u4L3VlanIf,
                         TRUE) != CFA_FAILURE)
                    {
                        if (CfaIfmDeleteStackMplsTunnelInterface
                            (u4L3VlanIf,
                             SrRtrAltNextHopInfo.u4OutIfIndex) == CFA_FAILURE)
                        {
                            SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                                     "%s:%d Unable to delete LFA FTN Tunnel"
                                     "for prefix=%x Lfa-NH=%x\r\n", __func__,
                                     __LINE__, u4DestAddr,
                                     pRtrInfo->u4LfaNextHop);
                            return SR_FAILURE;
                        }
                    }

                    /* Resetting the LFA Next hop */
                    pRtrInfo->bIsLfaActive = SR_FALSE;
                    pRtrInfo->u4LfaNextHop = SR_FALSE;
                    pRtrInfo->u1LfaType = SR_ZERO;
                    pRtrInfo->u4LfaOutIfIndex = SR_ZERO;
                    pRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
                }

                SR_TRC5 (SR_CTRL_TRC,
                         "%s:%d SID %d SrGbMinIndex %d u1NHFound %d \n", __func__,
                         __LINE__, pNewRtrNode->u4SidValue,
                         pNewRtrNode->srgbRange.u4SrGbMinIndex, u1NHFound);

                if ((u1NHFound == SR_FALSE) &&
                    (pNewRtrNode->u4SidValue != SR_ZERO))
                {
                    /*Create new Next-Hop Node */
                    if ((pNewSrRtrNextHopInfo =
                         SrCreateNewNextHopNode ()) == NULL)
                    {
                        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                                 "%s:%d pNewSrRtrNextHopInfo == NULL \n", __func__,
                                 __LINE__);
                        return SR_FAILURE;
                    }
                    /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
                    MEMCPY (&pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                            &(pRouteInfo->nextHop.Addr.u4Addr),
                            MAX_IPV4_ADDR_LEN);
                    pTempSrRtrInfo =
                        SrGetRtrInfoFromNextHop (&pRouteInfo->nextHop);
                    if (pTempSrRtrInfo != NULL)
                    {
                        MEMCPY (&u4TempNbrRtrId,
                                &(pTempSrRtrInfo->prefixId.Addr.u4Addr),
                                MAX_IPV4_ADDR_LEN);
                    }
                    else
                    {
                        u4TempNbrRtrId =
                            SrNbrRtrFromNextHop (pRouteInfo->nextHop.Addr.
                                                 u4Addr);
                    }

                    /* Bug: As pRouteInfo->u4RtIfIndex is coming Incorrect,
                     * a temporary has been intrudcued to find outIfindex from nexthop
                     * from NeighborTable*/
                    /* Temp Fix Start */
                    /*
                       SrGetOutIfFromNextHop (pRouteInfo->u4NextHop, &u4OutIfIndex);
                     */
                    /* Temp Fix End */
                    pNewSrRtrNextHopInfo->nextHop.u2AddrType =
                        SR_IPV4_ADDR_TYPE;
                    pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                    pNewSrRtrNextHopInfo->u4SwapOutIfIndex = u4OutIfIndex;
                    pNewSrRtrNextHopInfo->nbrRtrId = u4TempNbrRtrId;

                    /* Add NextHop Node in NextHopList in SrRtrNode */
                    TMO_SLL_Insert (&(pNewRtrNode->NextHopList), NULL,
                                    &(pNewSrRtrNextHopInfo->nextNextHop));
                    /* Create FTN */
                    if (SrMplsCreateFTN (pNewRtrNode, pNewSrRtrNextHopInfo) ==
                        SR_FAILURE)
                    {
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateFTN  FAILURE \n",
                                 __func__, __LINE__);

                        /* FTN & POP&FWD cannot co-exist (Preventive check as sometime MLIBUPDATE API failing "to do") */
                        if ((pNewSrRtrNextHopInfo->
                             u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
                        {
                            continue;
                        }

                        /* If Next-Hop is not SR Enabled Create ILM-Pop&Fwd and return */
                        pTempSrRtrInfo =
                            SrGetRtrInfoFromNextHop (&pRouteInfo->nextHop);

                        if (pTempSrRtrInfo == NULL)
                        {
                            /*ECMP Check: Before creating Pop n Fwd, check if ILM Swap is already created for this DestAddr
                             * Scenario:  ECMP DestAddr, ILM Swap Already created from one side, SR shut from other side,
                             *            Now Rt Add come from Sr_Shut side*/

                            if (TMO_SLL_Count (&pNewRtrNode->NextHopList) >
                                SR_ONE)
                            {
                                u1TmpFtnFound = SR_FALSE;

                                TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                                              pSrRtrTmpNextHopNode,
                                              tTMO_SLL_NODE *)
                                {
                                    pSrRtrTmpNextHopInfo =
                                        (tSrRtrNextHopInfo *)
                                        pSrRtrTmpNextHopNode;
                                    if ((pSrRtrTmpNextHopInfo->
                                         u1MPLSStatus & SR_FTN_CREATED) ==
                                        SR_FTN_CREATED)
                                    {
                                        SR_TRC2 (SR_CTRL_TRC,
                                                 "%s:%d FTN already present \n",
                                                 __func__, __LINE__);
                                        u1TmpFtnFound = SR_TRUE;
                                        break;
                                    }
                                }

                                if (u1TmpFtnFound == SR_TRUE)
                                {
                                    /* This implies, Already atleast one FTN is created, and ILM swap will also be there
                                     * so no need to create ILM Pop n Fwd */
                                    continue;
                                    /*continue to SrRtrInfo Loop */
                                }
                            }

                            pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                            SR_TRC2 (SR_CTRL_TRC,
                                     "%s:%d pTempSrRtrInfo == NULL: ~ Next-Hop is not SR Enabled (Create: Pop&Fwd) ~ \n",
                                     __func__, __LINE__);
                            if (SrMplsCreateILMPopAndFwd
                                (pNewRtrNode,
                                 pNewSrRtrNextHopInfo) == SR_SUCCESS)
                            {
                                SR_TRC2 (SR_CTRL_TRC,
                                         "%s:%d SrMplsCreateILMPopAndFwd == SUCCESS \n",
                                         __func__, __LINE__);
                                return SR_SUCCESS;
                            }
                            else
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d POP&FWD Create Failure \n",
                                         __func__, __LINE__);
                            }
                        }
                        /* Delete the node from the list in case of FTN Creation failure */
                        TMO_SLL_Delete (&(pNewRtrNode->NextHopList),
                                        (tTMO_SLL_NODE *) pNewSrRtrNextHopInfo);
                        SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                        return SR_FAILURE;
                    }

                    if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
                    {
                        /*ECMP Case for POP n FWD Deletion if SWAP is to be created */
                        if (TMO_SLL_Count (&pNewRtrNode->NextHopList) !=
                            SR_ZERO)
                        {
                            TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                                          pSrRtrTmpNextHopNode, tTMO_SLL_NODE *)
                            {
                                pSrRtrTmpNextHopInfo =
                                    (tSrRtrNextHopInfo *) pSrRtrTmpNextHopNode;
                                if ((pSrRtrTmpNextHopInfo->
                                     u1MPLSStatus & SR_ILM_POP_CREATED) ==
                                    SR_ILM_POP_CREATED)
                                {
                                    SR_TRC2 (SR_CTRL_TRC,
                                             "%s:%d ILM POP n FWD already present \n",
                                             __func__, __LINE__);
                                    u1TmpNHFound = SR_TRUE;
                                    break;
                                }
                            }
                        }

                        if (u1TmpNHFound == SR_TRUE)
                        {
                            if (SrMplsDeleteILMPopAndFwd
                                (pNewRtrNode,
                                 pSrRtrTmpNextHopInfo) == SR_SUCCESS)
                            {
                                SR_TRC2 (SR_CTRL_TRC,
                                         "%s:%d POP&FWD Deleted; Removing from SrRtrNhList \n",
                                         __func__, __LINE__);

                                TMO_SLL_Delete (&(pNewRtrNode->NextHopList),
                                                (tTMO_SLL_NODE *)
                                                pSrRtrTmpNextHopInfo);
                                SR_RTR_NH_LIST_FREE (pSrRtrTmpNextHopInfo);
                            }
                        }

                        SR_TRC2 (SR_CTRL_TRC, "%s:%d Creating ILM SWAP \n",
                                 __func__, __LINE__);

                        if (SrMplsCreateILM (pNewRtrNode, pNewSrRtrNextHopInfo)
                            == SR_FAILURE)
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SrMplsCreateILM  FAILURE \n", __func__,
                                     __LINE__);
                        }
                    }
                    break;
                }
            }
            else
            {
                SR_TRC3 (SR_CTRL_TRC,
                         "%s:%d PeerId do not matches with Route Add Event for %x \n",
                         __func__, __LINE__, u4DestAddr);
            }
        }
        /* Sr-Non-TE: Ends */
    }
    if (pSrIpEvtInfo->u4BitMap == SR_RT_DELETED)
    {
        SR_TRC2 (SR_CTRL_TRC,
                 "Priamry ROUTE_DEL event received for  Dest %x  NextHop %x \n",
                 u4DestAddr, pRouteInfo->nextHop.Addr.u4Addr);
        /* If Pop&Fwd Created with same NH, then delete the Pop & Fwd & return
         * "return SR_Success": is done because POP&FWD and FTN cannot be present simultaneously*/
        MEMCPY (&tmpAddr.Addr.u4Addr, &u4DestAddr, sizeof (UINT4));
        tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        MEMCPY (&(NextHop.Addr.u4Addr), &(pRouteInfo->nextHop.Addr.u4Addr),
                SR_IPV4_ADDR_LENGTH);
        NextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        if (SrDeletePopNFwdUsingRtInfo (&tmpAddr, &NextHop) == SR_SUCCESS)
        {
            return SR_SUCCESS;
        }
        /* Sr-TE: Start */
        if (((pSrTeRtInfo = SrGetTeRouteEntryFromDestAddr (&tmpAddr)) != NULL)
            && ((pDestSrRtrInfo = SrGetSrRtrInfoFromRtrId (&tmpAddr)) != NULL)
            && (pDestSrRtrInfo->bIsLfaActive == SR_TRUE))
        {
            SR_TRC3 (SR_CTRL_TRC,
                     "%s:%d SR-FRR ILM is active for %x and Route Delete Skipped for TE Dest \n",
                     __func__, __LINE__, tmpAddr.Addr.u4Addr);
            return SR_SUCCESS;
        }

        /* Te MAndatry Rt Delete Processing */
        while ((pSrTeRtEntryInfo =
                SrGetNextTeRouteTableEntry (&getSrTeRtEntryInfo)) != NULL)
        {
            if (pSrTeRtEntryInfo->mandRtrId.Addr.u4Addr == tmpAddr.Addr.u4Addr)
            {
                /* Rt_Del Evt came for Mandatory RtrId, So Process SR-TE routes */

                if (pSrTeRtEntryInfo->bIsSrTeFRRActive == SR_TRUE)
                {
                    SR_TRC4 (SR_CTRL_TRC,
                             "%s:%d Skipping Rt_DEL Event came for Mandatory RtrId %x "
                             " with FRR Active TE Route %x \r\n",
                             __func__, __LINE__, tmpAddr.Addr.u4Addr,
                             pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                    return SR_SUCCESS;
                }
                else
                {
                    SR_TRC4 (SR_CTRL_TRC,
                             "%s:%d Rt_DEL Event came for Mandatory RtrId %x "
                             " -- Processing Non-FRR Active TE Route %x \r\n",
                             __func__, __LINE__, tmpAddr.Addr.u4Addr,
                             pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                    break;        /* break since Rt_Del for TE is handled in SrTeDeleteFtnRtChangeEvt */
                }
            }
            MEMSET (&getSrTeRtEntryInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
            getSrTeRtEntryInfo.destAddr.Addr.u4Addr =
                pSrTeRtEntryInfo->destAddr.Addr.u4Addr;
            getSrTeRtEntryInfo.mandRtrId.Addr.u4Addr =
                pSrTeRtEntryInfo->mandRtrId.Addr.u4Addr;
            getSrTeRtEntryInfo.destAddr.u2AddrType =
                pSrTeRtEntryInfo->destAddr.u2AddrType;
            getSrTeRtEntryInfo.mandRtrId.u2AddrType =
                pSrTeRtEntryInfo->mandRtrId.u2AddrType;
        }

        if (SrTeDeleteFtnRtChangeEvt (&tmpAddr, &u4ErrCode, &NextHop) ==
            SR_SUCCESS)
        {
            if (u4ErrCode == SR_TE_VIA_DEST)
            {
                SR_TRC3 (SR_CTRL_TRC,
                         "%s:%d SR TE LSP deleted via dest for prefix %x \n",
                         __func__, __LINE__, tmpAddr.Addr.u4Addr);
                return SR_SUCCESS;
            }
        }
        else
        {
            if (u4ErrCode == SR_TE_VIA_DEST)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d SR TE LSP deletion via dest for prefix %x Failed \n",
                         __func__, __LINE__, tmpAddr.Addr.u4Addr);
                return SR_FAILURE;
            }
        }
        /* Sr-TE: End */

        /* Sr-Non-TE: Starts */
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            pRtrInfo = (tSrRtrInfo *) pRtrNode;

            /*u4RtrId = OSPF_CRU_BMC_DWFROMPDU (pRtrInfo->rtrId); */
            MEMCPY (&u4RtrId, &pRtrInfo->prefixId.Addr.u4Addr, sizeof (UINT4));

            if (u4DestAddr == u4RtrId)
            {
                pNewRtrNode = pRtrInfo;

                if (SR_TRUE == gSrGlobalInfo.bIsMicroLoopAvd)
                {
                    if ((pRtrInfo->u1OprFlags & SR_OPR_F_ADD_DELAY_LIST)
                                                == SR_OPR_F_ADD_DELAY_LIST)
                    {
                        SRProcessDelayedRouteInfo(pRtrInfo);
                    }
                }
                TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                              pSrRtrNextHopNode, tTMO_SLL_NODE *)
                {
                    pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

                    if ((MEMCMP
                         ((&pSrRtrNextHopInfo->nextHop.Addr.u4Addr),
                          &(pRouteInfo->nextHop.Addr.u4Addr),
                          SR_IPV4_ADDR_LENGTH) == SR_ZERO)
                        && (pSrRtrNextHopInfo->bIsLfaNextHop != TRUE))
                    {
                        u1NHFound = SR_TRUE;
                        break;
                    }
                }

                if (u1NHFound == SR_FALSE)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d u1NHFound == SR_FALSE: SR_FAILURE \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }

                SR_TRC3 (SR_CTRL_TRC, "%s:%d u1NHFound == TRUE; MplsStatus %x \n",
                         __func__, __LINE__, pSrRtrNextHopInfo->u1MPLSStatus);

                if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                    SR_FTN_CREATED)
                {
                    pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

                    while (pSrSidIntf != NULL)
                    {
                        if (pSrSidIntf->u4SidType != SR_SID_NODE)
                        {
                            SR_TRC3 (SR_CTRL_TRC, "%s:%d This Sid Interface is not node SID %d \n",
                         __func__, __LINE__,pSrSidIntf->u4SidType);
                            pSrSidIntf = RBTreeGetNext(gSrGlobalInfo.pSrSidRbTree,pSrSidIntf,NULL);
                            continue;
                        }
                           SR_TRC3 (SR_CTRL_TRC, "%s:%d This Sid Interface is node SID %d \n",
                         __func__, __LINE__,pSrSidIntf->u4SidType);
                        /* Populate SrInSegInfo from Global SelfNodeInfo */
                        if (pSrSidIntf != NULL)
                        {
                            u4SrGbMinIndex = gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;

                            SrInSegInfo.Fec.u2AddrFmly = pSrSidIntf->ipAddrType;
                            MEMCPY (&(SrInSegInfo.Fec.Prefix.u4Addr),
                                    &pSrSidIntf->ifIpAddr, MAX_IPV4_ADDR_LEN);
                        }

                        /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
                        SrInSegInfo.u4InLabel =
                            (u4SrGbMinIndex +
                             pNewRtrNode->u4SidValue);

                        if (pSrSidIntf != NULL)
                        {
                            /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
                            SrInSegInfo.u4InLabel =
                                (u4SrGbMinIndex + pNewRtrNode->u4SidValue);

                            SR_TRC2 (SR_CTRL_TRC, "%s:%d Deleting ILM SWAP",
                                     __func__, __LINE__);

                            if (SrMplsDeleteILM
                                (&SrInSegInfo, pNewRtrNode,
                                pSrRtrNextHopInfo) == SR_FAILURE)
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d SrMplsDeleteILM  FAILURE", __func__,
                                         __LINE__);
                            }
                        }
                        pSrSidIntf = RBTreeGetNext(gSrGlobalInfo.pSrSidRbTree,pSrSidIntf,NULL);
                    }

                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d ILM SWAP DELETED, Deleting FTN for %x \n",
                             __func__, __LINE__, u4DestAddr);
                    if (SrMplsDeleteFTN (pNewRtrNode, pSrRtrNextHopInfo) ==
                        SR_FAILURE)
                    {
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsDeleteFTN  FAILURE \n",
                                 __func__, __LINE__);
                        return SR_FAILURE;
                    }
                    /* Remove SrRtrNhNode from NextHopList */
                    TMO_SLL_Delete (&(pNewRtrNode->NextHopList),
                                    (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                    SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);

                    /*ECMP Case for ILM Creation */
                    if (TMO_SLL_Count (&pNewRtrNode->NextHopList) != SR_ZERO)
                    {
                        TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                                      pSrRtrTmpNextHopNode, tTMO_SLL_NODE *)
                        {
                            pSrRtrTmpNextHopInfo =
                                (tSrRtrNextHopInfo *) pSrRtrTmpNextHopNode;
                            if ((pSrRtrTmpNextHopInfo->
                                 u1MPLSStatus & SR_FTN_CREATED) ==
                                SR_FTN_CREATED)
                            {
                                u1TmpNHFound = SR_TRUE;
                                break;
                            }
                        }

                        if (u1TmpNHFound == SR_TRUE)
                        {
                            if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
                            {
                                SR_TRC2 (SR_CTRL_TRC,
                                         "%s:%d Creating ILM SWAP for next ECMP FTN \n",
                                         __func__, __LINE__);

                                if (SrMplsCreateILM
                                    (pNewRtrNode,
                                     pSrRtrTmpNextHopInfo) == SR_FAILURE)
                                {
                                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                             "%s:%d SrMplsCreateILM  FAILURE \n",
                                             __func__, __LINE__);
                                }
                                else
                                {
                                    SR_TRC2 (SR_CTRL_TRC,
                                             "%s:%d SrMplsCreateILM  SUCCESS: for next ECMP FTN \n",
                                             __func__, __LINE__);
                                }

                            }
                        }
                    }
                    break;
                }
                else
                {
                    SR_TRC3 (SR_CTRL_TRC,
                             "%s:%d No FTN Created for %x, Hence No Action \n",
                             __func__, __LINE__, u4DestAddr);
                }
            }
            else
            {
                SR_TRC3 (SR_CTRL_TRC,
                         "%s:%d PeerId do not matches with Route Delete Event for %x \n",
                         __func__, __LINE__, u4DestAddr);
            }

        }
        /* Sr-Non-TE: Ends */
    }

    SR_TRC2 (SR_CTRL_TRC | SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*Dynamic-SR*/
PUBLIC tSrRtrInfo  *
SrCreateNewRouterNode (VOID)
{
    tSrRtrInfo         *tempRtrNode = NULL;

    if (SR_RTR_LIST_ALLOC (tempRtrNode) == NULL)
    {
        SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                "%s:%d Memory Allocation failed for new router node \n",
                __func__, __LINE__);
        return NULL;
    }

    MEMSET (tempRtrNode, SR_ZERO, sizeof (tSrRtrInfo));
    TMO_SLL_Init (&(tempRtrNode->adjSidList));

    /*Initialize NextHop List */
    TMO_SLL_Init (&(tempRtrNode->NextHopList));

    return tempRtrNode;
}

PUBLIC tSrRtrInfo  *
SrGetRtrInfoFromNextHop (tGenU4Addr * pNextHop)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRtrInfo         *tempRtrNode = NULL;
    tIp6Addr            nbrV6LinkIpAddr;
    tIp6Addr            dummyV6LinkIpAddr;
    UINT4               u4NbrRtrId = SR_ZERO;
    UINT4               u4NbrRtrIpAddr = SR_ZERO;
    tTMO_SLL_NODE      *pSrV6OspfNbrNode = NULL;
    tTMO_SLL_NODE      *pSrV4OspfNbrNode = NULL;
    tSrV6OspfNbrInfo   *pSrV6OspfNbrInfo = NULL;
    tSrV4OspfNbrInfo   *pSrV4OspfNbrInfo = NULL;

    MEMSET (&nbrV6LinkIpAddr, SR_ZERO, sizeof (tIp6Addr));
    MEMSET (&dummyV6LinkIpAddr, SR_ZERO, sizeof (tIp6Addr));
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    if (pNextHop->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        /* Find Neighbor having the Neighbourship link same as NextHop IP */
        TMO_SLL_Scan (&(gSrV4OspfNbrList), pSrV4OspfNbrNode, tTMO_SLL_NODE *)
        {
            pSrV4OspfNbrInfo = (tSrV4OspfNbrInfo *) pSrV4OspfNbrNode;
            SR_TRC3 (SR_UTIL_TRC, "%s:%d u4NbrLinkIpAddr = %x \n", __func__,
                     __LINE__, pSrV4OspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr);
            if (pNextHop->Addr.u4Addr ==
                pSrV4OspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr)
            {
                u4NbrRtrId = pSrV4OspfNbrInfo->ospfV4NbrInfo.u4NbrRtrId;
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Nbr found with RtrId = %x \n",
                         __func__, __LINE__, u4NbrRtrId);
                break;
            }
        }

        /* No Neighbor found with the Neighbourship link same as NextHop IP */
        if (u4NbrRtrId == SR_ZERO)
        {
            SR_TRC3 (SR_UTIL_TRC, "%s:%d No Nbr found with connected Link = %x \n",
                     __func__, __LINE__, pNextHop->Addr.u4Addr);
            return NULL;
        }
        /* Find SrRtrInfo (LSA-10) with RtrId found in Neighbor list */
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            pRtrInfo = (tSrRtrInfo *) pRtrNode;
            SR_TRC3 (SR_UTIL_TRC, "%s:%d u4NbrRtrId = %x \n", __func__, __LINE__,
                     u4NbrRtrId);
            SR_TRC4 (SR_UTIL_TRC, "%s:%d AdvRtrId %x Area type %x \n", __func__, __LINE__,
                     pRtrInfo->advRtrId.Addr.u4Addr, pRtrInfo->u1RtrType);
            if ((u4NbrRtrId == pRtrInfo->advRtrId.Addr.u4Addr) &&
                     (pRtrInfo->u1RtrType == OSPF_INTRA_AREA))
            {
                SR_TRC3 (SR_UTIL_TRC,
                         "%s:%d SrRtrInfo found with same Nbr-RtrId = %x \n",
                         __func__, __LINE__, u4NbrRtrId);
                tempRtrNode = pRtrInfo;
                return tempRtrNode;
            }
        }
    }
    else if (pNextHop->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        /* Find Neighbor having the Neighbourship link same as NextHop IP */
        TMO_SLL_Scan (&(gSrV6OspfNbrList), pSrV6OspfNbrNode, tTMO_SLL_NODE *)
        {
            pSrV6OspfNbrInfo = (tSrV6OspfNbrInfo *) pSrV6OspfNbrNode;

            if (MEMCMP
                (pNextHop->Addr.Ip6Addr.u1_addr,
                 pSrV6OspfNbrInfo->ospfV6NbrInfo.nbrIpv6Addr.u1_addr,
                 SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                MEMCPY (nbrV6LinkIpAddr.u1_addr, pNextHop->Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Nbr found with RtrId = %x \n",
                         __func__, __LINE__, u4NbrRtrId);
                break;
            }
        }

        /* No Neighbor found with the Neighbourship link same as NextHop IP */
        if (MEMCMP
            (nbrV6LinkIpAddr.u1_addr, dummyV6LinkIpAddr.u1_addr,
             SR_IPV6_ADDR_LENGTH) == SR_ZERO)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d no neighbour found with neighbourship NH=0 \n",
                    __func__, __LINE__);
            return NULL;
        }

        /* Find SrRtrInfo (LSA-10) with RtrId found in Neighbor list */
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            pRtrInfo = (tSrRtrInfo *) pRtrNode;

            SR_TRC3 (SR_UTIL_TRC, "%s:%d u4NbrRtrId = %x \n", __func__, __LINE__,
                     u4NbrRtrId);
            u4NbrRtrIpAddr =
                OSIX_HTONL (pSrV6OspfNbrInfo->ospfV6NbrInfo.nbrRtrId);
            if ((u4NbrRtrIpAddr == pRtrInfo->advRtrId.Addr.u4Addr)
                && (pRtrInfo->srgbRange.u4SrGbV3MinIndex != SR_ZERO))
            {
                SR_TRC3 (SR_UTIL_TRC,
                         "%s:%d SrRtrInfo found with same Nbr-RtrId = %x \n",
                         __func__, __LINE__, u4NbrRtrId);
                tempRtrNode = pRtrInfo;
                return tempRtrNode;
            }
        }
    }
    return NULL;
}

PUBLIC VOID
SrGetNextHopFromNbrRtrId (tGenU4Addr * pNbrRtrId, tGenU4Addr * pNbrLinkIpAddr)
{
    tTMO_SLL_NODE      *pSrV4OspfNbrNode = NULL;
    tSrV4OspfNbrInfo   *pSrV4OspfNbrInfo = NULL;
    UINT4               u4TempNbrRtrId = SR_ZERO;
    UINT4               u4NbrLinkIpAddr = SR_ZERO;
    UINT4               u4NbrRtrId = SR_ZERO;
    BOOL1               bNbrFound = SR_FALSE;
    tIp6Addr            nbrV6IpAddr;
    tIp6Addr            dummyV6LinkIpAddr;

    tTMO_SLL_NODE      *pSrV6OspfNbrNode = NULL;
    tSrV6OspfNbrInfo   *pSrV6OspfNbrInfo = NULL;

    MEMSET (&nbrV6IpAddr, SR_ZERO, sizeof (tIp6Addr));
    MEMSET (&dummyV6LinkIpAddr, SR_ZERO, sizeof (tIp6Addr));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    if (pNbrLinkIpAddr == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d pNbrLinkIpAddr == NULL \n", __func__,
                 __LINE__);
        return;
    }
    if (pNbrRtrId == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d pNbrRtrId == NULL \n", __func__, __LINE__);
        return;
    }
    if (pNbrLinkIpAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_TRC3 (SR_UTIL_TRC, "%s:%d NbrRtrId = %x \n", __func__, __LINE__,
                 pNbrRtrId->Addr.u4Addr);
        /* Find Neighbor having the RtrId same as u4NbrRtrId */
        TMO_SLL_Scan (&(gSrV4OspfNbrList), pSrV4OspfNbrNode, tTMO_SLL_NODE *)
        {
            pSrV4OspfNbrInfo = (tSrV4OspfNbrInfo *) pSrV4OspfNbrNode;
            if (pNbrRtrId->Addr.u4Addr ==
                pSrV4OspfNbrInfo->ospfV4NbrInfo.u4NbrRtrId)
            {
                u4NbrLinkIpAddr = pSrV4OspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr;
                bNbrFound = SR_TRUE;
                SR_TRC3 (SR_UTIL_TRC,
                         "%s:%d Nbr found with u4NbrLinkIpAddr = %x \n", __func__,
                         __LINE__, u4NbrLinkIpAddr);
                break;
            }
        }

        if (bNbrFound == SR_TRUE)
        {
            pNbrLinkIpAddr->Addr.u4Addr = u4NbrLinkIpAddr;
        }
        else
        {
            pNbrLinkIpAddr->Addr.u4Addr = SR_ZERO;
        }
    }
    else if (pNbrLinkIpAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        /* Find Neighbor having the Neighbourship link same as NextHop IP */
        TMO_SLL_Scan (&(gSrV6OspfNbrList), pSrV6OspfNbrNode, tTMO_SLL_NODE *)
        {
            pSrV6OspfNbrInfo = (tSrV6OspfNbrInfo *) pSrV6OspfNbrNode;

            MEMCPY (&u4TempNbrRtrId, &pSrV6OspfNbrInfo->ospfV6NbrInfo.nbrRtrId,
                    MAX_IPV4_ADDR_LEN);
            u4NbrRtrId = OSIX_HTONL (u4TempNbrRtrId);
            if (pNbrRtrId->Addr.u4Addr == u4NbrRtrId)
            {
                bNbrFound = SR_TRUE;
                MEMCPY (&pNbrLinkIpAddr->Addr.Ip6Addr.u1_addr,
                        &pSrV6OspfNbrInfo->ospfV6NbrInfo.nbrIpv6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                break;
            }
        }

        if (bNbrFound != SR_TRUE)
        {
            MEMCPY (&pNbrLinkIpAddr->Addr.Ip6Addr.u1_addr, &nbrV6IpAddr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);
        }
    }
}

PUBLIC VOID
SrGetNbrRtrIdFromNbrAddr(tGenU4Addr * pNbrLinkIpAddr, UINT4 * pNbrRtrId)
{
    tTMO_SLL_NODE      *pSrV4OspfNbrNode = NULL;
    tSrV4OspfNbrInfo   *pSrV4OspfNbrInfo = NULL;
    UINT4               u4NbrLinkIpAddr = SR_ZERO;
    tTMO_SLL_NODE      *pSrV6OspfNbrNode = NULL;
    tSrV6OspfNbrInfo   *pSrV6OspfNbrInfo = NULL;

    SR_TRC2 (SR_MAIN_TRC, "%s:%d ENTRY \n", __func__, __LINE__);

    if (pNbrLinkIpAddr == NULL)
    {
        SR_TRC2 (SR_MAIN_TRC, "%s:%d pNbrLinkIpAddr == NULL \n", __func__,
                 __LINE__);
        return;
    }
    if (pNbrRtrId == NULL)
    {
        SR_TRC2 (SR_MAIN_TRC, "%s:%d pNbrRtrId == NULL \n", __func__, __LINE__);
        return;
    }

    if (pNbrLinkIpAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        SR_TRC3 (SR_MAIN_TRC, "%s:%d Nbraddr = %x \n", __func__, __LINE__,
                 pNbrLinkIpAddr->Addr.u4Addr);
        /* Find Neighbor having the RtrId same as u4NbrRtrId */
        TMO_SLL_Scan (&(gSrV4OspfNbrList), pSrV4OspfNbrNode, tTMO_SLL_NODE *)
        {
            pSrV4OspfNbrInfo = (tSrV4OspfNbrInfo *) pSrV4OspfNbrNode;
            if (pNbrLinkIpAddr->Addr.u4Addr ==
                pSrV4OspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr)
            {
                *pNbrRtrId = pSrV4OspfNbrInfo->ospfV4NbrInfo.u4NbrRtrId;
                SR_TRC4 (SR_MAIN_TRC,
                         "%s:%d Nbr(%x) found with u4NbrLinkIpAddr = %x\n", __func__,
                         __LINE__, *pNbrRtrId, pNbrLinkIpAddr->Addr.u4Addr);
                return;
            }
        }

    }
    else if (pNbrLinkIpAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        /* Find Neighbor having the Neighbourship link same as NextHop IP
           Todo: may be more check required as the link local address is link scope*/
        TMO_SLL_Scan (&(gSrV6OspfNbrList), pSrV6OspfNbrNode, tTMO_SLL_NODE *)
        {
            pSrV6OspfNbrInfo = (tSrV6OspfNbrInfo *) pSrV6OspfNbrNode;

            if (MEMCMP
                (pNbrLinkIpAddr->Addr.Ip6Addr.u1_addr,
                 pSrV6OspfNbrInfo->ospfV6NbrInfo.nbrIpv6Addr.u1_addr,
                 SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                *pNbrRtrId = pSrV6OspfNbrInfo->ospfV6NbrInfo.nbrRtrId;
                SR_TRC3 (SR_MAIN_TRC, "%s:%d Nbr found with RtrId = %x\n",
                         __func__, __LINE__, *pNbrRtrId);
                return;
            }
        }
    }

    return;
}

PUBLIC VOID
SrIPv4UpdateMplsBindingOnNbrSrUp(tSrRtrInfo         *pRtrInfo,
                                tSrRtrNextHopInfo  *pSrRtrNextHopInfo)


{
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tRtInfo             InRtInfo;
    tRtInfo            *pRt = NULL;
    tRtInfo            *pTmpRt = NULL;
    UINT4               u2IncarnId = SR_ZERO;
    INT4                i4AltStatus = SR_ZERO;
    tGenU4Addr          nextHop;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4nbrRtrId;

    MEMSET (&nextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&InRtInfo, SR_ZERO, sizeof (tRtInfo));

    u4nbrRtrId = pSrRtrNextHopInfo->nbrRtrId;
    nextHop.Addr.u4Addr = pSrRtrNextHopInfo->nextHop.Addr.u4Addr;

    if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_ILM_POP_CREATED) ==  SR_ILM_POP_CREATED)
    {
        if (SrMplsDeleteILMPopAndFwd(pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to Delete POP&Fwd entry for dest %x \n",
                 __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
        }
    }

    TMO_SLL_Delete (&(pRtrInfo->NextHopList), (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
    SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);

    /* Create FTN & ILM for SrRtrInfo with u4NbrRtrId as Next-Hop */
    InRtInfo.u4DestNet  = pRtrInfo->prefixId.Addr.u4Addr;
    InRtInfo.u4DestMask = SR_IPV4_DEST_MASK;

    RtmApiGetBestRouteEntryInCxt (u2IncarnId, InRtInfo, &pRt);

    pTmpRt = pRt;

    if (pTmpRt != NULL)
    {
        SR_TRC4 (SR_UTIL_TRC, "%s:%d ROUTE: NH %x u4OutIfIndex %d \n",
                 __func__, __LINE__, pTmpRt->u4NextHop, pTmpRt->u4RtIfIndx);
    }

#ifdef MPLS_SR_ECMP_WANTED
    for (; ((pTmpRt != NULL) && (pTmpRt->i4Metric1 == pRt->i4Metric1));
        pTmpRt = pTmpRt->pNextAlternatepath)
#else
    if (pTmpRt != NULL)
#endif
    {

        if (pTmpRt->u4NextHop != nextHop.Addr.u4Addr)
        {
#ifdef MPLS_SR_ECMP_WANTED
            continue;
#else
            SR_TRC5 (SR_CRITICAL_TRC| SR_FAIL_TRC, "%s:%d ROUTE %x: NH %x is different from SR Node's NH %x \n",
                 __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                 pTmpRt->u4NextHop, pTmpRt->u4RtIfIndx);

            return;
#endif
        }

        if (NetIpv4GetCfaIfIndexFromPort(pTmpRt->u4RtIfIndx,
             &u4OutIfIndex) == NETIPV4_FAILURE)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d No Port is mapped with u4RtIfIndx %d \n",
                     __func__, __LINE__, pTmpRt->u4RtIfIndx);
            return;
        }

        SR_TRC5 (SR_UTIL_TRC,
                 "%s:%d ROUTE %x Found with NH %x u4OutIfIndex %d \n",
                 __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                 pTmpRt->u4NextHop, u4OutIfIndex);

        /*Create new Next-Hop Node */
        if ((pNewSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d Failed to create New next hop node \n",
                     __func__, __LINE__);
            return;
        }

        /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
        pNewSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        MEMCPY (&pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                &(pTmpRt->u4NextHop), MAX_IPV4_ADDR_LEN);
        pNewSrRtrNextHopInfo->u4OutIfIndex =  u4OutIfIndex;
        pNewSrRtrNextHopInfo->u4SwapOutIfIndex = u4OutIfIndex;
        pNewSrRtrNextHopInfo->nbrRtrId = u4nbrRtrId;

        /* Add NextHop Node in NextHopList in SrRtrNode */
        TMO_SLL_Insert (&(pRtrInfo->NextHopList), NULL,
                        &(pNewSrRtrNextHopInfo->nextNextHop));

        /* Create FTN & ILM for u4DestAddr */
        if (SrMplsCreateFTN(pRtrInfo, pNewSrRtrNextHopInfo) == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d SrMplsCreateFTN failed for dest %x \n",
                     __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
            /* Delete the node from the list in case of FTN Creation failure */
            TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                            (tTMO_SLL_NODE *) pNewSrRtrNextHopInfo);
            SR_RTR_NH_LIST_FREE(pNewSrRtrNextHopInfo);
            return;
        }

        if (SrMplsCreateILM(pRtrInfo, pNewSrRtrNextHopInfo) == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d SrMplsCreateILM  failed for dest %x \n",
                     __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
            return;
        }
        /* Creating LFA LSPs for Prefix , where Primary Lsps are created above  */
        nmhGetFsSrV4AlternateStatus(&i4AltStatus);
        if (i4AltStatus == ALTERNATE_ENABLED)
        {
            if (LfaCreateAndDeleteLSP (MPLS_MLIB_FTN_CREATE, pRtrInfo, pNewSrRtrNextHopInfo) ==
                SR_FAILURE)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d Failed to Create LFA for Prefix = %x\n",
                         __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
            }
        }
    }

    return;

}

#ifdef IP6_WANTED
PUBLIC VOID
SrIPv6UpdateMplsBindingOnNbrSrUp(tSrRtrInfo         *pRtrInfo,
                                tSrRtrNextHopInfo  *pSrRtrNextHopInfo)


{

    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tRtInfo             InRtInfo;
    INT4                i4Ipv6RoutePfxLength = 128;
    UINT4               u4ContextId = VCM_DEFAULT_CONTEXT;
    tNetIpv6RtInfo      NetIpv6RtInfo;
    tGenU4Addr          destAddr;
    tGenU4Addr          rtrNextHop;
    UINT4               u4nbrRtrId;
    UINT4               u4OutIfIndex = SR_ZERO;

    MEMSET (&InRtInfo, SR_ZERO, sizeof (tRtInfo));
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&NetIpv6RtInfo, SR_ZERO, sizeof (tNetIpv6RtInfo));

    u4nbrRtrId = pSrRtrNextHopInfo->nbrRtrId;
    MEMCPY (&destAddr.Addr.Ip6Addr.u1_addr,
            pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr, MAX_IPV6_ADDR_LEN);
    MEMCPY (rtrNextHop.Addr.Ip6Addr.u1_addr,
            pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr, MAX_IPV6_ADDR_LEN);

    if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_ILM_POP_CREATED) ==  SR_ILM_POP_CREATED)
    {
        if (SrMplsDeleteILMPopAndFwd(pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to Delete POP&Fwd entry for dest %s \n",
                 __func__, __LINE__, Ip6PrintAddr(&pRtrInfo->prefixId.Addr.Ip6Addr));
        }
    }

    TMO_SLL_Delete (&(pRtrInfo->NextHopList), (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
    SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);

    if (Rtm6ApiGetBestRouteEntryInCxt(u4ContextId, &(destAddr.Addr.Ip6Addr),
                                         (UINT1) i4Ipv6RoutePfxLength, SR_ZERO,
                                         &NetIpv6RtInfo) != RTM6_FAILURE)
    {
        if (MEMCMP(NetIpv6RtInfo.NextHop.u1_addr, rtrNextHop.Addr.Ip6Addr.u1_addr, MAX_IPV6_ADDR_LEN) == SR_ZERO)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d RTM NH is not match for SR NH for dest %s \n",
                __func__, __LINE__,  Ip6PrintAddr(&pRtrInfo->prefixId.Addr.Ip6Addr));
            return;
        }

        if (NetIpv6RtInfo.u4Index != SR_ZERO)
        {
            if (NetIpv6GetCfaIfIndexFromPort(NetIpv6RtInfo.u4Index, &u4OutIfIndex) == NETIPV4_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d No Port to IfIndex Mapping \n", __func__, __LINE__);
                return;
            }
        }
        else
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Zero Interface index \n", __func__, __LINE__);
            return;
        }
    }
    else
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d No route available for destination \n", __func__, __LINE__);
        return;
    }

    /*Create new Next-Hop Node */
    if ((pNewSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Memory allocation failed\n", __func__, __LINE__);
        return;
    }

    /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
    pNewSrRtrNextHopInfo->nextHop.u2AddrType =  SR_IPV6_ADDR_TYPE;
    MEMCPY (pNewSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
            NetIpv6RtInfo.NextHop.u1_addr, MAX_IPV6_ADDR_LEN);
    pNewSrRtrNextHopInfo->u4OutIfIndex =   u4OutIfIndex;
    pNewSrRtrNextHopInfo->u4SwapOutIfIndex = u4OutIfIndex;
    pNewSrRtrNextHopInfo->nbrRtrId = u4nbrRtrId;

    /* Add NextHop Node in NextHopList in SrRtrNode */
    TMO_SLL_Insert (&(pRtrInfo->NextHopList), NULL,
                    &(pNewSrRtrNextHopInfo->nextNextHop));

    /* Create FTN & ILM for u4DestAddr */
    if (SrMplsCreateFTN(pRtrInfo, pNewSrRtrNextHopInfo) == SR_FAILURE)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d FTN creation failed for dest %s \n",
                 __func__, __LINE__, Ip6PrintAddr(&pRtrInfo->prefixId.Addr.Ip6Addr));
        /* Delete the node from the list in case of FTN Creation failure */
        TMO_SLL_Delete (&(pRtrInfo->NextHopList), (tTMO_SLL_NODE *) pNewSrRtrNextHopInfo);
        SR_RTR_NH_LIST_FREE(pNewSrRtrNextHopInfo);
        return;
    }

    if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
    {
        SR_TRC2 (SR_UTIL_TRC,  "%s:%d Creating ILM SWAP \n", __func__, __LINE__);
        if (SrMplsCreateILM(pRtrInfo, pNewSrRtrNextHopInfo) == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d ILM creation failed for dest %s \n",
                     __func__, __LINE__, Ip6PrintAddr(&pRtrInfo->prefixId.Addr.Ip6Addr));
            return;
        }
    }
}


#endif

PUBLIC UINT4
SrCreateMplsBindingOnNbrSrUp (tGenU4Addr * nbrPrefix, UINT4 u4AdvRtrId)
{

    tTMO_SLL_NODE      *pRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrTeRtEntryInfo    SrTeRtInfo;
    tRtInfo             InRtInfo;
    tRtInfo            *pRt = NULL;
    tRtInfo            *pTmpRt = NULL;
    UINT4               u2IncarnId = SR_ZERO;
    tGenU4Addr          nextHop;
    tGenU4Addr          destAddr;
    tGenU4Addr          rtrNextHop;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4SrTePathCount = SR_ZERO;
    tGenU4Addr          tmpNextHop;
    tSrTeRtEntryInfo   *pSrTeDestPathInfo = NULL;
    UINT4               u4TempAddr = SR_ZERO;
    tSrRtrInfo         *pTempSrRtrInfo = NULL;
    UINT4               u4TempNbrRtrId = SR_ZERO;
    UINT1               u1NHFound = SR_FALSE;
#ifdef IP6_WANTED
    INT4                i4Ipv6RoutePfxLength = 128;
    UINT4               u4ContextId = VCM_DEFAULT_CONTEXT;
#endif
    tNetIpv6RtInfo      NetIpv6RtInfo;
    tIp6Addr            tmpIp6Addr;

    UINT4               tePathCount = SR_ZERO;
    UINT4               tePathCounter = SR_ZERO;
    MEMSET (&tmpIp6Addr.u1_addr, SR_ZERO, sizeof (tIp6Addr));
    MEMSET (&NetIpv6RtInfo, SR_ZERO, sizeof (tNetIpv6RtInfo));
    MEMSET (&rtrNextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&nextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&tmpNextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&InRtInfo, SR_ZERO, sizeof (tRtInfo));

    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY for NbrPrefix=%x, u4AdvRtrId=%x\n",
              __func__,__LINE__,nbrPrefix->Addr.u4Addr, u4AdvRtrId);
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;
        if (nbrPrefix->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            MEMCPY (&(u4TempAddr), &pRtrInfo->prefixId.Addr.u4Addr,
                    MAX_IPV4_ADDR_LEN);
            if ((u4TempAddr == nbrPrefix->Addr.u4Addr)
                 && (pRtrInfo->advRtrId.Addr.u4Addr == u4AdvRtrId))
            {
                nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
                if (pRtrInfo->srgbRange.u4SrGbMinIndex == SR_ZERO)
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pRtrInfo->srgbRange.u4SrGbMinIndex == SR_ZERO \n",
                            __func__, __LINE__);
                    return SR_FAILURE;
                }

                pTempSrRtrInfo = pRtrInfo;
                SrGetNextHopFromNbrRtrId (&(pRtrInfo->advRtrId), &nextHop);
                break;
            }
        }
        else if (nbrPrefix->u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            /*API to get OSPF router ID needs to be called here */
            if (MEMCMP (nbrPrefix->Addr.Ip6Addr.u1_addr,
                        pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                nextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
                if (pRtrInfo->srgbRange.u4SrGbV3MinIndex == SR_ZERO)
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pRtrInfo->srgbRange.u4SrGbV3MinIndex == SR_ZERO \n",
                            __func__, __LINE__);
                    return SR_FAILURE;
                }

                pTempSrRtrInfo = pRtrInfo;
                SrGetNextHopFromNbrRtrId (&(pRtrInfo->advRtrId), &nextHop);

                break;
            }
        }
    }


    if (nbrPrefix->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        if (nextHop.Addr.u4Addr == SR_ZERO)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d ipv4 nexthop address is zero \n",
                    __func__, __LINE__);
            return SR_FAILURE;
        }
    }
    else
    {
        if (MEMCMP
            (nextHop.Addr.Ip6Addr.u1_addr, tmpIp6Addr.u1_addr,
             sizeof (tIp6Addr)) == SR_ZERO)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d NH=0 \n", __func__,
                    __LINE__);
            return SR_FAILURE;
        }
    }

    /* Sr-TE: Start */
    RBTreeCount (gSrGlobalInfo.pSrTeRouteRbTree, &u4SrTePathCount);
    {
        SR_TRC1 (SR_UTIL_TRC, "Sr-TE Configured with PathCount %d\r\n",
                 u4SrTePathCount);

        /* Find & Create TePath Entries that are using this u4NextHop that exist on SrRtrInfo */
        pSrTeRtInfo = RBTreeGetFirst (gSrGlobalInfo.pSrTeRouteRbTree);
        RBTreeCount (gSrGlobalInfo.pSrTeRouteRbTree, &tePathCount);
        tePathCounter = SR_ZERO;

        while (1)
        {
            if (tePathCounter++ == tePathCount)
            {
                break;
            }
            if (pSrTeRtInfo != NULL)
            {
                if ((pSrTeRtInfo->u1MPLSStatus & SR_FTN_CREATED) !=
                    SR_FTN_CREATED)
                {
                    /* Get the SrRtrInfo belongs to this MandRtrId of SrTeRt as
                     * NextHop is required for MandRtrId*/

                    if (SrGetNHForPrefix
                        (&pSrTeRtInfo->mandRtrId, &tmpNextHop,
                         &u4OutIfIndex) != SR_FAILURE)
                    {
                        if (nbrPrefix->u2AddrType == SR_IPV4_ADDR_TYPE)
                        {
                            if (nextHop.Addr.u4Addr == tmpNextHop.Addr.u4Addr)
                            {
                                if (SrTeFtnIlmCreateDelViaDestRt
                                    (pSrTeRtInfo, SR_TRUE) == SR_SUCCESS)
                                    /* Find & delete TePath Entries that are using this u4NextHop */
                                {
                                    SR_TRC4 (SR_UTIL_TRC,
                                             "%s:%d SR TE LSP creation is successfull with prefix %x NH %x \n",
                                             __func__, __LINE__,
                                             pSrTeRtInfo->destAddr.Addr.u4Addr,
                                             nextHop.Addr.u4Addr);
                                }
                            }
                        }
                        else if (nbrPrefix->u2AddrType == SR_IPV6_ADDR_TYPE)
                        {
                            if (MEMCMP (nextHop.Addr.Ip6Addr.u1_addr,
                                        tmpNextHop.Addr.Ip6Addr.u1_addr,
                                        SR_IPV6_ADDR_LENGTH))
                            {
                                if (SrTeFtnIlmCreateDelViaDestRt
                                    (pSrTeRtInfo, SR_TRUE) == SR_SUCCESS)
                                    /* Find & delete TePath Entries that are using this u4NextHop */
                                {
                                    SR_TRC4 (SR_UTIL_TRC,
                                             "%s:%d SR TE LSP creation is successfull with prefix %s NH %s \n",
                                             __func__, __LINE__,
                                             Ip6PrintAddr (&pSrTeRtInfo->
                                                           destAddr.Addr.
                                                           Ip6Addr),
                                             Ip6PrintAddr (&nextHop.Addr.
                                                           Ip6Addr));

                                }
                            }
                        }
                    }

                    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                    SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                    if (nbrPrefix->u2AddrType == SR_IPV4_ADDR_TYPE)
                    {
                        SrTeRtInfo.destAddr.Addr.u4Addr =
                            pSrTeRtInfo->destAddr.Addr.u4Addr;
                        SrTeRtInfo.mandRtrId.Addr.u4Addr =
                            pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                    }
                    else if (nbrPrefix->u2AddrType == SR_IPV6_ADDR_TYPE)
                    {
                        MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                                pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);
                        MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                                pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                                SR_IPV6_ADDR_LENGTH);
                    }

                    SrTeRtInfo.mandRtrId.u2AddrType =
                        pSrTeRtInfo->mandRtrId.u2AddrType;
                    SrTeRtInfo.destAddr.u2AddrType =
                        pSrTeRtInfo->destAddr.u2AddrType;
                    pSrTeRtInfo =
                        RBTreeGetNext (gSrGlobalInfo.pSrTeRouteRbTree,
                                       &SrTeRtInfo, NULL);
                }
            }
            else
            {
                break;
            }
        }
    }
    /* Sr-TE: End */

    pRtrNode = NULL;

    /* Find SrRtrInfo (LSA-10) with having same NH in Neighbor list */
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;
        u1NHFound = SR_FALSE;
        if (nbrPrefix->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            MEMCPY (&destAddr.Addr.u4Addr, &pRtrInfo->prefixId.Addr.u4Addr,
                    MAX_IPV4_ADDR_LEN);
            if (destAddr.Addr.u4Addr == nbrPrefix->Addr.u4Addr)
            {
                /* FTN/ILM Already created for u4NbrRtrId, so skip */
                continue;
            }
        }
        else if (nbrPrefix->u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (&destAddr.Addr.Ip6Addr.u1_addr,
                    pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr, MAX_IPV6_ADDR_LEN);
            if (MEMCMP
                (destAddr.Addr.Ip6Addr.u1_addr, nbrPrefix->Addr.Ip6Addr.u1_addr,
                 SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                /* FTN/ILM Already created for u4NbrRtrId, so skip */
                continue;
            }
        }

        u1NHFound = SR_FALSE;
        if (TMO_SLL_Count (&pRtrInfo->NextHopList) != SR_ZERO)
        {
            TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                          pSrRtrNextHopNode, tTMO_SLL_NODE *)
            {
                pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

                if (nbrPrefix->u2AddrType == SR_IPV4_ADDR_TYPE)
                {
                    MEMCPY (&(rtrNextHop.Addr.u4Addr),
                            &(pSrRtrNextHopInfo->nextHop.Addr.u4Addr),
                            MAX_IPV4_ADDR_LEN);

                    SR_TRC5 (SR_UTIL_TRC,
                             "%s:%d RtrId %x rtrNextHop = %x u1MPLSStatus = %x \n",
                             __func__, __LINE__, destAddr.Addr.u4Addr,
                             rtrNextHop.Addr.u4Addr,
                             pSrRtrNextHopInfo->u1MPLSStatus);
                    if ((nextHop.Addr.u4Addr == rtrNextHop.Addr.u4Addr))
                    {
                        u1NHFound = SR_TRUE;
                        break;
                    }
                }

                else if (nbrPrefix->u2AddrType == SR_IPV6_ADDR_TYPE)
                {
                    MEMCPY (rtrNextHop.Addr.Ip6Addr.u1_addr,
                            pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                            MAX_IPV6_ADDR_LEN);

                    if ((MEMCMP
                         (nextHop.Addr.Ip6Addr.u1_addr,
                          rtrNextHop.Addr.Ip6Addr.u1_addr,
                          MAX_IPV6_ADDR_LEN) == SR_ZERO))
                    {
                        u1NHFound = SR_TRUE;
                        break;
                    }
                }
            }
        }

        if (u1NHFound == SR_TRUE)
        {
            if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                                        SR_FTN_NOT_CREATED)
            {
                if (nbrPrefix->u2AddrType == SR_IPV4_ADDR_TYPE)
                {
                    SrIPv4UpdateMplsBindingOnNbrSrUp(pRtrInfo, pSrRtrNextHopInfo);
                }
#ifdef IP6_WANTED
                else if (nbrPrefix->u2AddrType == SR_IPV6_ADDR_TYPE)
                {
                    SrIPv6UpdateMplsBindingOnNbrSrUp(pRtrInfo, pSrRtrNextHopInfo);
                }
#endif
            }
        }
        else  /* Checking TeConf for Rtr as for TE case ECMP not applicable */
        {

            pSrTeDestPathInfo = SrGetTeRouteEntryFromDestAddr (&destAddr);
            if (pSrTeDestPathInfo != NULL)
            {
                continue;
            }

            /* Start of ECMP case */
            /* ECMP: Check if any Route for this NH is present,
             * if prsent then create FTN with this NH if not created */

            if (nbrPrefix->u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                u4TempNbrRtrId = pTempSrRtrInfo->prefixId.Addr.u4Addr;
                if ((u4TempNbrRtrId != nbrPrefix->Addr.u4Addr) ||
                    ((u4TempNbrRtrId == u4AdvRtrId)
                     && (pRtrInfo->srgbRange.u4SrGbMinIndex != SR_ZERO)))
                {
                    continue;
                }

                InRtInfo.u4DestNet = destAddr.Addr.u4Addr;
                InRtInfo.u4DestMask = SR_IPV4_DEST_MASK;

                RtmApiGetBestRouteEntryInCxt (u2IncarnId, InRtInfo, &pRt);

                pTmpRt = pRt;

                if (pTmpRt != NULL)
                {
                    SR_TRC4 (SR_UTIL_TRC, "%s:%d ROUTE: NH %x u4OutIfIndex %d \n",
                             __func__, __LINE__, pTmpRt->u4NextHop,
                             pTmpRt->u4RtIfIndx);
                }

#ifdef MPLS_SR_ECMP_WANTED
                for (;
                     ((pTmpRt != NULL)
                      && (pTmpRt->i4Metric1 == pRt->i4Metric1));
                     pTmpRt = pTmpRt->pNextAlternatepath)
#else
                if (pTmpRt != NULL)
#endif
                {
                    /* Skip the Existing Next hop */
                    TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                                  pSrRtrNextHopNode, tTMO_SLL_NODE *)
                    {
                        pSrRtrNextHopInfo =
                            (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
                        if (pTmpRt->u4NextHop ==
                            pSrRtrNextHopInfo->nextHop.Addr.u4Addr)
                        {
                            u1NHFound = SR_TRUE;
                            break;
                        }

                    }

                    if ((pTmpRt->u4NextHop == nextHop.Addr.u4Addr) ||
                        (u1NHFound == SR_TRUE))
                    {
                        u1NHFound = SR_FALSE;
                        continue;
                    }

                    if (NetIpv4GetCfaIfIndexFromPort (pTmpRt->u4RtIfIndx,
                                                      &u4OutIfIndex) ==
                        NETIPV4_FAILURE)
                    {
                        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                                 "%s:%d No Port is mapped with u4RtIfIndx %d \n",
                                 __func__, __LINE__, pTmpRt->u4RtIfIndx);
                        return SR_FAILURE;
                    }

                    SR_TRC5 (SR_UTIL_TRC,
                             "%s:%d ROUTE Found for FEC %x NH %x u4OutIfIndex %d \n",
                             __func__, __LINE__, destAddr.Addr.u4Addr,
                             pTmpRt->u4NextHop, u4OutIfIndex);

                    /*Create new Next-Hop Node */
                    if ((pNewSrRtrNextHopInfo =
                         SrCreateNewNextHopNode ()) == NULL)
                    {
                        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                                 "%s:%d pNewSrRtrNextHopInfo == NULL \n", __func__,
                                 __LINE__);
                        return SR_FAILURE;
                    }

                    /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
                    pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr =
                        pTmpRt->u4NextHop;
                    pNewSrRtrNextHopInfo->nextHop.u2AddrType =
                        nbrPrefix->u2AddrType;
                    pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                    pNewSrRtrNextHopInfo->u4SwapOutIfIndex = u4OutIfIndex;
                    pNewSrRtrNextHopInfo->nbrRtrId = nbrPrefix->Addr.u4Addr;
                    /* Add NextHop Node in NextHopList in SrRtrNode */
                    TMO_SLL_Insert (&(pRtrInfo->NextHopList), NULL,
                                    &(pNewSrRtrNextHopInfo->nextNextHop));
                    /* Create FTN & ILM for u4DestAddr */
                    if (SrMplsCreateFTN (pRtrInfo, pNewSrRtrNextHopInfo) ==
                        SR_FAILURE)
                    {
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateFTN  FAILURE \n",
                                 __func__, __LINE__);
                        TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                        (tTMO_SLL_NODE *) pNewSrRtrNextHopInfo);
                        SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                        continue;
                    }
                    if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
                    {
                        SR_TRC2 (SR_UTIL_TRC, "%s:%d Creating ILM SWAP \n",
                                 __func__, __LINE__);

                        if (SrMplsCreateILM (pRtrInfo, pNewSrRtrNextHopInfo) ==
                            SR_FAILURE)
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SrMplsCreateILM  FAILURE \n", __func__,
                                     __LINE__);
                            continue;
                        }
                    }

                    else
                    {
                        SR_TRC3 (SR_UTIL_TRC,
                                 "%s:%d SrMplsCreateFTN  SUCCESS for %x \n",
                                 __func__, __LINE__, destAddr.Addr.u4Addr);
                    }
                }
            }
#ifdef IP6_WANTED
            else if (nbrPrefix->u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                if (Rtm6ApiGetBestRouteEntryInCxt
                    (u4ContextId, &(destAddr.Addr.Ip6Addr),
                     (UINT1) i4Ipv6RoutePfxLength, SR_ZERO,
                     &NetIpv6RtInfo) != RTM6_FAILURE)
                {
                    if (MEMCMP
                        (NetIpv6RtInfo.NextHop.u1_addr, tmpIp6Addr.u1_addr,
                         MAX_IPV6_ADDR_LEN) == SR_ZERO)
                    {
                        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d NH=0 \n", __func__,
                                __LINE__);
                        return SR_FAILURE;
                    }

                    /* If RTM best next is not equal to SR Neighbor, then continue */
                    if (MEMCMP
                        (NetIpv6RtInfo.NextHop.u1_addr,
                         nextHop.Addr.Ip6Addr.u1_addr,
                         MAX_IPV6_ADDR_LEN) != SR_ZERO)
                    {
                        continue;
                    }

                    if (NetIpv6RtInfo.u4Index != SR_ZERO)
                    {
                        if (NetIpv6GetCfaIfIndexFromPort
                            (NetIpv6RtInfo.u4Index,
                             &u4OutIfIndex) == NETIPV4_FAILURE)
                        {
                            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d No Port to IfIndex Mapping \n", __func__,
                                    __LINE__);
                            return SR_FAILURE;
                        }
                    }
                    else
                    {
                        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Zero Interface Index \n", __func__,
                                __LINE__);
                        return SR_FAILURE;
                    }
                }
                else
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                             "%s:%d No route available for destination \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }

                /*Create new Next-Hop Node */
                if ((pNewSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pNewSrRtrNextHopInfo == NULL \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }

                /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
                MEMCPY (&pNewSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                        NetIpv6RtInfo.NextHop.u1_addr, SR_IPV6_ADDR_LENGTH);
                pNewSrRtrNextHopInfo->nextHop.u2AddrType = nbrPrefix->u2AddrType;
                pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;

                /* Add NextHop Node in NextHopList in SrRtrNode */
                TMO_SLL_Insert (&(pRtrInfo->NextHopList), NULL,
                                &(pNewSrRtrNextHopInfo->nextNextHop));

                /* Create FTN & ILM for u4DestAddr */
                if (SrMplsCreateFTN (pRtrInfo, pNewSrRtrNextHopInfo) ==
                    SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateFTN  FAILURE \n",
                             __func__, __LINE__);
                    TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                    (tTMO_SLL_NODE *) pNewSrRtrNextHopInfo);
                    SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                    continue;
                }
                else
                {
                    SR_TRC2 (SR_UTIL_TRC, "%s:%d SrMplsCreateFTN  SUCCESS \n",
                             __func__, __LINE__);
                    if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
                    {
                        SR_TRC2 (SR_UTIL_TRC, "%s:%d Creating ILM SWAP \n",
                                 __func__, __LINE__);

                        if (SrMplsCreateILM (pRtrInfo, pNewSrRtrNextHopInfo) ==
                            SR_FAILURE)
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SrMplsCreateILM  FAILURE \n", __func__,
                                     __LINE__);
                            continue;
                        }
                    }
                }
            }
#endif
            /* End of ECMP case */
            else
            {
                continue;
            }
        }
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/* This function will be called when Neighbor SR is made down and Flush LSA is recieved
 * u4NbrRtrId is passed with RTRIPAddr from which FLUSH LSA is received
 * SR_FAILURE - if u4NbrRtrId is not a neighbor router
 * SR_SUCCESS - if u4NbrRtrId is a neighbor router, & Delete MPLS binding of all SR Peers*/

PUBLIC UINT4
SrDeleteMplsBindingOnNbrSrDown (tGenU4Addr * nbrRtrId, UINT4 u4AdvRtrId)
{

    tTMO_SLL_NODE      *pRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tTMO_SLL_NODE      *pSrRtrTmpNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrTmpNextHopInfo = NULL;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tRtInfo            *pRt = NULL;
    tRtInfo            *pTmpRt = NULL;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tRtInfo             InRtInfo;
    tSrTeRtEntryInfo    SrTeRtInfo;

    tSrInSegInfo        SrInSegInfo;
    tSrInSegInfo        SrRlfaInSegInfo;
    UINT4               u2IncarnId = SR_ZERO;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4SrTePathCount = SR_ZERO;
    UINT1               u1FTNDelFlag = SR_FALSE;
    UINT4               u4TempAddr = SR_ZERO;
    UINT4               u4SrGbMinIndex = SR_ZERO;
    UINT1               u1NextHopFlag = SR_FALSE;
    UINT1               u1LfaNxtHopFlag = SR_FALSE;
    UINT1               u1RlfaNxtHopFlag = SR_FALSE;
    UINT1               u1TilfaNxtHopFlag = SR_FALSE;
    tGenU4Addr          tmpNextHop;
    tGenU4Addr          nextHop;
    tGenU4Addr          rtrNextHop;
    tGenU4Addr          teSwapNh;
    tGenU4Addr          destAddr;
#ifdef IP6_WANTED
    INT4                i4Ipv6RoutePfxLength = 128;
    UINT4               u4ContextId = VCM_DEFAULT_CONTEXT;
#endif
    tNetIpv6RtInfo      NetIpv6RtInfo;
    tIp6Addr            tmpIp6Addr;
    UINT1               u1TmpNHFound = SR_FALSE;

    UINT4               tePathCount = SR_ZERO;
    UINT4               tePathCounter = SR_ZERO;
    tSrRtrNextHopInfo  *pRlfaSrRtrNextHopInfo = NULL;

    tSrTilfaPathInfo    SrTilfaPathInfo;
    tSrTilfaPathInfo   *pSrTilfaPathInfo = NULL;

    MEMSET (&teSwapNh, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&tmpNextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&tmpIp6Addr.u1_addr, SR_ZERO, sizeof (tIp6Addr));
    MEMSET (&NetIpv6RtInfo, SR_ZERO, sizeof (tNetIpv6RtInfo));
    MEMSET (&rtrNextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&nextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&tmpNextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&InRtInfo, SR_ZERO, sizeof (tRtInfo));
    MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&SrRlfaInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrTilfaPathInfo, SR_ZERO, sizeof (tSrTilfaPathInfo));

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d ENTRY for NbrRtrId=%x, u4AdvRtrId=%x\n",
              __func__,__LINE__,nbrRtrId->Addr.u4Addr, u4AdvRtrId);
    /* If u4NbrRtrId is an adjacent neighbor then u4NextHop = a valid NextHop
     * If u4NbrRtrId is not adjacent neighbor then u4NextHop = SR_ZERO */
    if (nbrRtrId == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d nbrRtrId == NULL \n", __func__,
                __LINE__);
        return SR_FAILURE;
    }
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;
        if (nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            MEMCPY (&(u4TempAddr), &(pRtrInfo->prefixId.Addr.u4Addr),
                    MAX_IPV4_ADDR_LEN);
            if ((u4TempAddr == nbrRtrId->Addr.u4Addr)
                && (pRtrInfo->advRtrId.Addr.u4Addr == u4AdvRtrId))
            {
                nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
                if (pRtrInfo->srgbRange.u4SrGbMinIndex == SR_ZERO)
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Sr gb Minimum Range Index is zero \n", __func__,
                            __LINE__);
                    return SR_FAILURE;
                }
                if ((pRtrInfo->srgbRange.u4SrGbMinIndex == SR_ZERO) ||
                    (pRtrInfo->advRtrId.Addr.u4Addr != u4TempAddr))
                {
                    SrGetNextHopFromNbrRtrId (&(pRtrInfo->advRtrId), &nextHop);
                }
                else
                {
                    SrGetNextHopFromNbrRtrId (nbrRtrId, &nextHop);
                }
            }
        }
        else if (nbrRtrId->u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            /*API to get OSPF router ID needs to be called here */
            if (MEMCMP (nbrRtrId->Addr.Ip6Addr.u1_addr,
                        pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                nextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
                if (pRtrInfo->srgbRange.u4SrGbV3MinIndex != SR_ZERO)
                {
                    SrGetNextHopFromNbrRtrId (&(pRtrInfo->advRtrId), &nextHop);
                }
            }
        }
    }

    pRtrNode = NULL;

    if (nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        if (nextHop.Addr.u4Addr == SR_ZERO)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d NH = 0 \n", __func__,
                    __LINE__);
            return SR_FAILURE;
        }
    }
    else
    {
        if (MEMCMP
            (nextHop.Addr.Ip6Addr.u1_addr, tmpIp6Addr.u1_addr,
             sizeof (tIp6Addr)) == SR_ZERO)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d nbr router id nexthop is zero \n", __func__,
                    __LINE__);
            return SR_FAILURE;
        }
    }
    /* Sr-TE: Start */
    RBTreeCount (gSrGlobalInfo.pSrTeRouteRbTree, &u4SrTePathCount);
    if (u4SrTePathCount != SR_ZERO)
    {
        SR_TRC1 (SR_UTIL_TRC, "Sr-TE Configured with PathCount %d\r\n",
                 u4SrTePathCount);

        /* Find & Delete TePath Entries that are using this u4NextHop */
        pSrTeRtInfo = RBTreeGetFirst (gSrGlobalInfo.pSrTeRouteRbTree);
        RBTreeCount (gSrGlobalInfo.pSrTeRouteRbTree, &tePathCount);
        tePathCounter = SR_ZERO;

        while (1)
        {
            if (tePathCounter++ == tePathCount)
            {
                break;
            }

            if (pSrTeRtInfo != NULL)
            {
                if ((pSrTeRtInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                    SR_FTN_CREATED)
                {
                    /* Get the SrRtrInfo belongs to this SrTeRt */
                    if ((pRtrInfo =
                         SrGetSrRtrInfoFromRtrId (&pSrTeRtInfo->destAddr)) !=
                        NULL)
                    {
                        /* There will be only one SrRtrNhNode for SR-TE */
                        pSrRtrNextHopInfo =
                            (tSrRtrNextHopInfo *)
                            TMO_SLL_First (&(pRtrInfo->NextHopList));
                        if (pSrRtrNextHopInfo == NULL)
                        {
                            SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC, "No TE Next-Hop\r\n");
                            return SR_FAILURE;
                        }

                        if (nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
                        {
                            MEMCPY (&tmpNextHop.Addr.u4Addr,
                                    &pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                                    sizeof (UINT4));
                            SR_TRC3 (SR_UTIL_TRC, "%s:%d Sr-TE NH %x \n",
                                     __func__, __LINE__,
                                     tmpNextHop.Addr.u4Addr);
                            if (nextHop.Addr.u4Addr == tmpNextHop.Addr.u4Addr)
                            {
                                if (SrTeFtnIlmCreateDelViaDestRt
                                    (pSrTeRtInfo, SR_FALSE) == SR_SUCCESS)
                                    /* Find & delete TePath Entries that are using this u4NextHop */
                                {
                                    SR_TRC4 (SR_UTIL_TRC,
                                             "%s:%d SR TE LSP deletion is successfull with prefix %x NH %x \n",
                                             __func__, __LINE__,
                                             pSrTeRtInfo->destAddr.Addr.u4Addr,
                                             nextHop.Addr.u4Addr);
                                }
                            }
                        }
                        else if (nbrRtrId->u2AddrType == SR_IPV6_ADDR_TYPE)
                        {
                            MEMCPY (tmpNextHop.Addr.Ip6Addr.u1_addr,
                                    pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.
                                    u1_addr, SR_IPV6_ADDR_LENGTH);
                            {

                                if (SrTeFtnIlmCreateDelViaDestRt
                                    (pSrTeRtInfo, SR_FALSE) == SR_SUCCESS)
                                    /* Find & delete TePath Entries that are using this u4NextHop */
                                {
                                    SR_TRC4 (SR_UTIL_TRC,
                                             "%s:%d SR TE LSP deletion is successfull with prefix %s NH %s \n",
                                             __func__, __LINE__,
                                             Ip6PrintAddr (&pSrTeRtInfo->
                                                           destAddr.Addr.
                                                           Ip6Addr),
                                             Ip6PrintAddr (&nextHop.Addr.
                                                           Ip6Addr));

                                }

                            }
                        }
                    }
                }
                else
                {
                    /* Get the SrRtrInfo belongs to this SRTePath */
                    if ((pRtrInfo =
                         SrGetSrRtrInfoFromRtrId (&pSrTeRtInfo->destAddr)) !=
                        NULL)
                    {
                        if (nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
                        {
                            MEMCPY (&teSwapNh.Addr.u4Addr,
                                    &pRtrInfo->teSwapNextHop.Addr.u4Addr,
                                    sizeof (UINT4));

                            SR_TRC4 (SR_UTIL_TRC,
                                     "%s:%d  TE-SWAP status %d, TE-SWAP NH %x \n",
                                     __func__, __LINE__,
                                     pRtrInfo->u1TeSwapStatus,
                                     teSwapNh.Addr.u4Addr);

                            if ((pRtrInfo->u1TeSwapStatus == TRUE)
                                && (nextHop.Addr.u4Addr ==
                                    teSwapNh.Addr.u4Addr))
                            {
                                SR_TRC2 (SR_UTIL_TRC,
                                         "%s:%d  Deleting ILM-SWAP \n", __func__,
                                         __LINE__);
                                if ((SrDeleteILMFromSrId
                                     (&pSrTeRtInfo->destAddr,
                                      &nextHop)) == SR_FAILURE)
                                {
                                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                             "%s:%d  SrDeleteTeILM FAILURE \n",
                                             __func__, __LINE__);
                                    /* Intentional fall thru */
                                }
                            }
                        }
                        if (nbrRtrId->u2AddrType == SR_IPV6_ADDR_TYPE)
                        {
                            MEMCPY (teSwapNh.Addr.Ip6Addr.u1_addr,
                                    pRtrInfo->teSwapNextHop.Addr.Ip6Addr.
                                    u1_addr, SR_IPV6_ADDR_LENGTH);

                            SR_TRC3 (SR_UTIL_TRC, "%s:%d  TE-SWAP status %d \n",
                                     __func__, __LINE__,
                                     pRtrInfo->u1TeSwapStatus);

                            if ((pRtrInfo->u1TeSwapStatus == TRUE) &&
                                (MEMCMP
                                 (nextHop.Addr.Ip6Addr.u1_addr,
                                  teSwapNh.Addr.Ip6Addr.u1_addr,
                                  SR_IPV6_ADDR_LENGTH) == SR_ZERO))
                            {
                                SR_TRC2 (SR_UTIL_TRC,
                                         "%s:%d  Deleting ILM-SWAP \n", __func__,
                                         __LINE__);
                                if ((SrDeleteILMFromSrId
                                     (&pSrTeRtInfo->destAddr,
                                      &nextHop)) == SR_FAILURE)
                                {
                                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                             "%s:%d  SrDeleteTeILM FAILURE \n",
                                             __func__, __LINE__);
                                    /* Intentional fall thru */
                                }
                            }
                        }
                    }
                }

                MEMSET (&SrTeRtInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
                SrTeRtInfo.u4DestMask = pSrTeRtInfo->u4DestMask;
                if (nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
                {
                    SrTeRtInfo.destAddr.Addr.u4Addr =
                        pSrTeRtInfo->destAddr.Addr.u4Addr;
                    SrTeRtInfo.mandRtrId.Addr.u4Addr =
                        pSrTeRtInfo->mandRtrId.Addr.u4Addr;
                }
                else if (nbrRtrId->u2AddrType == SR_IPV6_ADDR_TYPE)
                {
                    MEMCPY (SrTeRtInfo.destAddr.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->destAddr.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                    MEMCPY (SrTeRtInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                            pSrTeRtInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                            SR_IPV6_ADDR_LENGTH);
                }

                SrTeRtInfo.mandRtrId.u2AddrType =
                    pSrTeRtInfo->mandRtrId.u2AddrType;
                SrTeRtInfo.destAddr.u2AddrType =
                    pSrTeRtInfo->destAddr.u2AddrType;
                pSrTeRtInfo =
                    RBTreeGetNext (gSrGlobalInfo.pSrTeRouteRbTree, &SrTeRtInfo,
                                   NULL);
            }
            else
            {
                break;
            }
        }
    }
    /* Sr-TE: End */

    /* Find SrRtrInfo (LSA-10) with RtrId found in Neighbor list */
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        u1FTNDelFlag = SR_FALSE;
        pRtrInfo = (tSrRtrInfo *) pRtrNode;
        if (nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            destAddr.Addr.u4Addr = pRtrInfo->prefixId.Addr.u4Addr;
            if ((nbrRtrId->Addr.u4Addr == destAddr.Addr.u4Addr) ||
                ((pRtrInfo->advRtrId.Addr.u4Addr == u4AdvRtrId) &&
                 (pRtrInfo->srgbRange.u4SrGbMinIndex != SR_ZERO)))
            {
                continue;
            }
        }
        else if (nbrRtrId->u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (&destAddr.Addr.Ip6Addr.u1_addr,
                    pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr, MAX_IPV6_ADDR_LEN);
            if ((MEMCMP
                 (nbrRtrId->Addr.Ip6Addr.u1_addr,
                  pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr,
                  SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                || ((pRtrInfo->advRtrId.Addr.u4Addr == u4AdvRtrId)
                    && (pRtrInfo->srgbRange.u4SrGbV3MinIndex != SR_ZERO)))
            {
                continue;
            }
        }
        if (TMO_SLL_Count (&pRtrInfo->NextHopList) != SR_ZERO)
        {
            TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                          pSrRtrNextHopNode, tTMO_SLL_NODE *)
            {
                pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
#if 0
                SR_TRC4 (SR_MAIN_TRC,
                         "%s:%d Check for u4DestAddr %x NbrLinkIp = %x ",
                         __func__, __LINE__, u4DestAddr, u4NextHop);
#endif
                if (nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
                {
                    if ((MEMCMP
                         (&pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                          &nextHop.Addr.u4Addr, MAX_IPV4_ADDR_LEN) == SR_ZERO)
                        &&
                        ((pSrRtrNextHopInfo->nbrRtrId == nbrRtrId->Addr.u4Addr)
                         || (pSrRtrNextHopInfo->nbrRtrId == u4AdvRtrId)))
                    {
                        u1NextHopFlag = SR_TRUE;
                    }
                    /* checking if neighbour (where SR is disabled) is Lfa Nexthop for given prefix */
                    else if ((nextHop.Addr.u4Addr == pRtrInfo->u4LfaNextHop))
                    {
                        u1LfaNxtHopFlag = SR_TRUE;
                    }
                    else if (pSrRtrNextHopInfo->bIsLfaNextHop == SR_TRUE)
                    {
                        if (pSrRtrNextHopInfo->u1FRRNextHop == SR_RLFA_NEXTHOP)
                        {
                            u1RlfaNxtHopFlag = SR_TRUE;
                        }
                        else if (pSrRtrNextHopInfo->u1FRRNextHop == SR_TILFA_NEXTHOP)
                        {
                            u1TilfaNxtHopFlag = SR_TRUE;
                        }
                    }
                }
                else if (nbrRtrId->u2AddrType == SR_IPV6_ADDR_TYPE)
                {
                    if (MEMCMP
                        (pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                         nextHop.Addr.Ip6Addr.u1_addr,
                         MAX_IPV6_ADDR_LEN) == SR_ZERO)
                    {
                        u1NextHopFlag = SR_TRUE;
                    }
                }
                if (u1LfaNxtHopFlag == SR_TRUE)
                {
                    SR_TRC2 (SR_UTIL_TRC,
                             "%s Deleting LFA LSPs for Prefix %x \n", __func__,
                             pRtrInfo->prefixId.Addr.u4Addr);
                    u1LfaNxtHopFlag = SR_FALSE;
                    if (((nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
                         && (SrUtilCheckNodeIdConfigured () == SR_SUCCESS))
                         || ((nbrRtrId->u2AddrType == SR_IPV6_ADDR_TYPE)
                             && (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)))
                    {
                        if (nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
                        {
                            /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
                            SrInSegInfo.u4InLabel =
                                (gSrGlobalInfo.SrContext.SrGbRange.
                                 u4SrGbMinIndex + pRtrInfo->u4SidValue);
                        }
                        else if (nbrRtrId->u2AddrType == SR_IPV6_ADDR_TYPE)
                        {
                            /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
                            SrInSegInfo.u4InLabel =
                                (gSrGlobalInfo.SrContextV3.SrGbRange.
                                 u4SrGbV3MinIndex + pRtrInfo->u4SidValue);
                        }
                    }

                    if (pRtrInfo->u4LfaNextHop != SR_ZERO)
                    {
                        if ((pRlfaSrRtrNextHopInfo =
                             SrCreateNewNextHopNode ()) == NULL)
                        {
                            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                                     "%s:%d pRlfaSrRtrNextHopInfo == NULL \n",
                                     __func__, __LINE__);
                            return SR_FAILURE;
                        }

                        /*Deleting LFA LSP */
                        pRlfaSrRtrNextHopInfo->u4OutIfIndex =
                            pRtrInfo->u4LfaOutIfIndex;
                        pRlfaSrRtrNextHopInfo->u4SwapOutIfIndex =
                            pRtrInfo->u4LfaSwapOutIfIndex;
                        pRlfaSrRtrNextHopInfo->u1MPLSStatus = SR_ONE;
                        pRlfaSrRtrNextHopInfo->u1FRRNextHop = SR_LFA_NEXTHOP;
                        pRlfaSrRtrNextHopInfo->nextHop.Addr.u4Addr =
                            pRtrInfo->u4LfaNextHop;
                        pRlfaSrRtrNextHopInfo->nextHop.u2AddrType =
                            SR_IPV4_ADDR_TYPE;
                        MEMCPY (&pRlfaSrRtrNextHopInfo->PrimarynextHop,
                                &pSrRtrNextHopInfo->nextHop,
                                sizeof (tGenU4Addr));
                        if (SrMplsDeleteFTN (pRtrInfo, pRlfaSrRtrNextHopInfo) ==
                            SR_FAILURE)
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d Unable tSR_CRITICAL_TRCSR_CRITICAL_TRCo delete LFA FTN \n", __func__,
                                     __LINE__);
                        }
                        if (SrMplsDeleteILM
                            (&SrInSegInfo, pRtrInfo,
                             pRlfaSrRtrNextHopInfo) == SR_FAILURE)
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d Unable to delete ILM SWAP \n",
                                     __func__, __LINE__);
                        }
                        pRtrInfo->bIsLfaActive = SR_FALSE;
                        pRtrInfo->u4LfaNextHop = SR_FALSE;
                        pRtrInfo->u1LfaType = SR_ZERO;
                        pRtrInfo->u4LfaOutIfIndex = SR_ZERO;
                        pRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
                        SR_RTR_NH_LIST_FREE (pRlfaSrRtrNextHopInfo);
                    }
                }

                if (u1RlfaNxtHopFlag == SR_TRUE)
                {
                    u1RlfaNxtHopFlag = SR_FALSE;

                    if (pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE)
                    {
                        u4SrGbMinIndex =
                            gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
                    }
                    else if (pRtrInfo->u2AddrType == SR_IPV6_ADDR_TYPE)
                    {
                        u4SrGbMinIndex =
                            gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;
                    }

                    if (pRtrInfo->u4LfaNextHop != SR_ZERO)
                    {
                        /* Deleting RLFA LSP */
                        pSrRlfaPathInfo =
                            SrGetRlfaPathEntryFromDestAddr (&(pRtrInfo->prefixId));
                        if (pSrRlfaPathInfo != NULL)
                        {
                            /* Deleting FTN and ILM for respective RLFA entry */
                            if (SrRlfaDeleteFTNAndILM
                                (&(pRtrInfo->prefixId),
                                 &(pSrRlfaPathInfo->mandRtrId),
                                 &(pSrRlfaPathInfo->rlfaNextHop),
                                 &SrRlfaInSegInfo,
                                 u4SrGbMinIndex) != SR_SUCCESS)
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d Deletion of SR RLFA FTN/ILM Failure for mantry Rtr Down\n",
                                         __func__, __LINE__);
                            }
                        }
                    }
                }

                if (u1TilfaNxtHopFlag == SR_TRUE)
                {
                    u1TilfaNxtHopFlag = SR_FALSE;

                    if (pRtrInfo->u4LfaNextHop != SR_ZERO)
                    {
                        SrTilfaPathInfo.destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                        MEMCPY (&SrTilfaPathInfo.destAddr.Addr.u4Addr,
                                &pRtrInfo->prefixId.Addr.u4Addr,
                                sizeof (UINT4));
                        /* Deleting TILFA LSP */
                        pSrTilfaPathInfo = RBTreeGet (gSrGlobalInfo.pSrTilfaPathRbTree,
                                                    &SrTilfaPathInfo);
                        if (pSrTilfaPathInfo != NULL)
                        {
                            SR_TRC3 (SR_CRITICAL_TRC | SR_UTIL_TRC,
                                    "%s:%d Deletion TILFA LSP for dest %x\n",
                                    __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
                            TiLfaHandleIpv4RouteDel(pSrTilfaPathInfo);
                        }
                        else
                        {
                            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                "%s:%d TILFA Path info is NULL for dest %x\n",
                                __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
                        }
                    }
                    else
                    {
                        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                            "%s:%d LFA Nexthop (TILFA) set to Zero for dest %x\n",
                            __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
                    }
                }

                if (u1NextHopFlag == SR_TRUE)
                {
#if 0
                    SR_TRC5 (SR_MAIN_TRC,
                             "%s:%d Matched for u4DestAddr %x NextHop %x MPLS_Status %x",
                             __func__, __LINE__, u4DestAddr, u4NextHop,
                             pSrRtrNextHopInfo->u1MPLSStatus);
#endif
                    /* 1. Delete FTN & ILM Swap
                     * 2. Create ILM Pop&Fwd for FECs
                     * that got disconnected at MPLS-Level due to Next-Hop SR-Shutdown
                     * This will Pop the Label at Penultimate hop(where SR is enabled)
                     * and Send the IP packet to the SR disabled Next-Hop*/

                    /* Reset the u1NextHopFlag to FALSE */
                    u1NextHopFlag = SR_FALSE;
                    /* 1. Delete ILM + FTN Starts */
                    if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                        SR_FTN_CREATED)
                    {
                        if (((nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
                             && (SrUtilCheckNodeIdConfigured () == SR_SUCCESS))
                            || ((nbrRtrId->u2AddrType == SR_IPV6_ADDR_TYPE)
                                && (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)))
                        {
                            if (nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
                            {
                                /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
                                SrInSegInfo.u4InLabel =
                                    (gSrGlobalInfo.SrContext.SrGbRange.
                                     u4SrGbMinIndex + pRtrInfo->u4SidValue);
                            }
                            else if (nbrRtrId->u2AddrType == SR_IPV6_ADDR_TYPE)
                            {
                                /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
                                SrInSegInfo.u4InLabel =
                                    (gSrGlobalInfo.SrContextV3.SrGbRange.
                                     u4SrGbV3MinIndex + pRtrInfo->u4SidValue);
                            }
                        }
                        SR_TRC3 (SR_UTIL_TRC, "%s:%d Deleting SR-LSPs for %x \n",
                                 __func__, __LINE__,
                                 pRtrInfo->prefixId.Addr.u4Addr);
                        if (pRtrInfo->u4LfaNextHop != SR_ZERO)
                        {
                            /*Create new Next-Hop Node */
                            if ((pRlfaSrRtrNextHopInfo =
                                 SrCreateNewNextHopNode ()) == NULL)
                            {
                                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                                         "%s:%d pRlfaSrRtrNextHopInfo == NULL \n",
                                         __func__, __LINE__);
                                return SR_FAILURE;
                            }
                            /* Deleting LFA LSP since Primary Lsps are Deleted Below , and there should be no LFA LSPs for Pop & Fwd */
                            pRlfaSrRtrNextHopInfo->u4OutIfIndex =
                                pRtrInfo->u4LfaOutIfIndex;
                            pRlfaSrRtrNextHopInfo->u4SwapOutIfIndex =
                                pRtrInfo->u4LfaSwapOutIfIndex;
                            pRlfaSrRtrNextHopInfo->u1MPLSStatus = SR_ONE;
                            pRlfaSrRtrNextHopInfo->u1FRRNextHop =
                                SR_LFA_NEXTHOP;
                            pRlfaSrRtrNextHopInfo->nextHop.Addr.u4Addr =
                                pRtrInfo->u4LfaNextHop;
                            pRlfaSrRtrNextHopInfo->nextHop.u2AddrType =
                                SR_IPV4_ADDR_TYPE;
                            MEMCPY (&pRlfaSrRtrNextHopInfo->PrimarynextHop,
                                    &pSrRtrNextHopInfo->nextHop,
                                    sizeof (tGenU4Addr));
                            if (SrMplsDeleteFTN
                                (pRtrInfo, pRlfaSrRtrNextHopInfo) == SR_FAILURE)
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d Unable to delete LFA FTN \n",
                                         __func__, __LINE__);
                            }
                            if (SrMplsDeleteILM
                                (&SrInSegInfo, pRtrInfo,
                                 pRlfaSrRtrNextHopInfo) == SR_FAILURE)
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d Unable to delete ILM SWAP \n",
                                         __func__, __LINE__);
                            }
                            pRtrInfo->bIsLfaActive = SR_FALSE;
                            pRtrInfo->u4LfaNextHop = SR_FALSE;
                            pRtrInfo->u1LfaType = SR_ZERO;
                            pRtrInfo->u4LfaOutIfIndex = SR_ZERO;
                            pRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
                            SR_RTR_NH_LIST_FREE (pRlfaSrRtrNextHopInfo);
                        }
                        else if (pRtrInfo->u4LfaNextHop != SR_ZERO)
                        {
                            if (pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE)
                            {
                                u4SrGbMinIndex =
                                    gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
                            }
                            else if (pRtrInfo->u2AddrType == SR_IPV6_ADDR_TYPE)
                            {
                                u4SrGbMinIndex =
                                    gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;
                            }


                            if (pRtrInfo->u4LfaNextHop != SR_ZERO)
                            {
                                /* Deleting RLFA LSP */
                                pSrRlfaPathInfo =
                                    SrGetRlfaPathEntryFromDestAddr (&
                                                                    (pRtrInfo->
                                                                     prefixId));
                                if (pSrRlfaPathInfo != NULL)
                                {
                                    /* Deleting FTN and ILM for respective RLFA entry */
                                    if (SrRlfaDeleteFTNAndILM
                                        (&(pRtrInfo->prefixId),
                                         &(pSrRlfaPathInfo->mandRtrId),
                                         &(pSrRlfaPathInfo->rlfaNextHop),
                                         &SrRlfaInSegInfo,
                                         u4SrGbMinIndex) != SR_SUCCESS)
                                    {
                                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                                 "%s:%d Deletion of SR RLFA FTN/ILM Failure for mantry Rtr Down\n",
                                                 __func__, __LINE__);
                                    }
                                }
                            }
                        }

                        /*Deleting Primary LSP */
                        if (SrMplsDeleteILM
                            (&SrInSegInfo, pRtrInfo,
                             pSrRtrNextHopInfo) == SR_FAILURE)
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SrMplsDeleteILM  FAILURE \n", __func__,
                                     __LINE__);
                        }

                        if (SrMplsDeleteFTN (pRtrInfo, pSrRtrNextHopInfo) ==
                            SR_FAILURE)
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SrMplsDeleteFTN  FAILURE \n", __func__,
                                     __LINE__);
                            break;
                        }
                        TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                        (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                        SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                        /* Delete ILM + FTN Ends */
                        u1FTNDelFlag = SR_TRUE;
                        break;
                    }
                }
            }

            /* If ECMP Scenario, Create ILM Swap with next FTN for this SrRtrInfo */
            /*ECMP Case for ILM Creation */
            if (u1FTNDelFlag == SR_TRUE)
            {
                if (TMO_SLL_Count (&pRtrInfo->NextHopList) != SR_ZERO)
                {
                    u1TmpNHFound = SR_FALSE;
                    TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                                  pSrRtrTmpNextHopNode, tTMO_SLL_NODE *)
                    {
                        pSrRtrTmpNextHopInfo =
                            (tSrRtrNextHopInfo *) pSrRtrTmpNextHopNode;
                        if ((pSrRtrTmpNextHopInfo->
                             u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
                        {
                            u1TmpNHFound = SR_TRUE;
                            break;
                        }
                    }

                    if (u1TmpNHFound == SR_TRUE)
                    {
                        if (((nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
                             && (SrUtilCheckNodeIdConfigured () == SR_SUCCESS))
                             || ((nbrRtrId->u2AddrType == SR_IPV6_ADDR_TYPE)
                                 && (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)))
                        {
                            SR_TRC2 (SR_UTIL_TRC,
                                     "%s:%d Creating ILM SWAP for next ECMP FTN \n",
                                     __func__, __LINE__);

                            if (SrMplsCreateILM (pRtrInfo, pSrRtrTmpNextHopInfo)
                                == SR_FAILURE)
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d SrMplsCreateILM  FAILURE \n",
                                         __func__, __LINE__);
                            }
                            else
                            {
                                SR_TRC2 (SR_UTIL_TRC,
                                         "%s:%d SrMplsCreateILM  SUCCESS: for next ECMP FTN \n",
                                         __func__, __LINE__);
                            }
                        }
                    }
                }
                else
                {
                    /* 2. Create Pop & Fwd Starts */
                    /* Create Pop & Fwd for SrRtrInfo who were using this u4NbrRtrId as Next-Hop
                     * Note: Their could be multiple Pop&Fwd in case of ECMP paths*/
                    if (nbrRtrId->u2AddrType == SR_IPV4_ADDR_TYPE)
                    {
                        InRtInfo.u4DestNet = destAddr.Addr.u4Addr;
                        InRtInfo.u4DestMask = SR_IPV4_DEST_MASK;

                        /* Donot Create ILM_POP for NbrRtr FEC */
                        if (MEMCMP
                            (&(nbrRtrId->Addr.u4Addr),
                             &(pRtrInfo->prefixId.Addr.u4Addr),
                             SR_IPV4_ADDR_LENGTH) != SR_ZERO)
                        {
                            RtmApiGetBestRouteEntryInCxt (u2IncarnId, InRtInfo,
                                                          &pRt);

                            pTmpRt = pRt;

                            if (pTmpRt != NULL)
                            {
                                SR_TRC4 (SR_UTIL_TRC,
                                         "%s:%d ROUTE: NH %x u4OutIfIndex %d \n",
                                         __func__, __LINE__, pTmpRt->u4NextHop,
                                         pTmpRt->u4RtIfIndx);
                            }

#ifdef MPLS_SR_ECMP_WANTED
                            for (;
                                 ((pTmpRt != NULL)
                                  && (pTmpRt->i4Metric1 == pRt->i4Metric1));
                                 pTmpRt = pTmpRt->pNextAlternatepath)
#else
                            if (pTmpRt != NULL)
#endif
                            {
                                if (pTmpRt->u4NextHop != nextHop.Addr.u4Addr)
                                {
                                    continue;
                                }

                                if (pTmpRt->u4NextHop == SR_ZERO)
                                {
                                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                                            "%s:%d ROUTE: NH = 0\n",__func__, __LINE__);
                                    return SR_FAILURE;
                                }

                                if (NetIpv4GetCfaIfIndexFromPort
                                    (pTmpRt->u4RtIfIndx,
                                     &u4OutIfIndex) == NETIPV4_FAILURE)
                                {
                                    SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                                             "%s:%d No Port is mapped with u4RtIfIndx %d \n",
                                             __func__, __LINE__,
                                             pTmpRt->u4RtIfIndx);
                                    return SR_FAILURE;
                                }

                                SR_TRC5 (SR_UTIL_TRC,
                                         "%s:%d ROUTE Found for FEC %x NH %x u4OutIfIndex %d \n",
                                         __func__, __LINE__,
                                         destAddr.Addr.u4Addr,
                                         pTmpRt->u4NextHop, u4OutIfIndex);

                                /*Create new Next-Hop Node */
                                if ((pNewSrRtrNextHopInfo =
                                     SrCreateNewNextHopNode ()) == NULL)
                                {
                                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                                             "%s:%d pNewSrRtrNextHopInfo == NULL \n",
                                             __func__, __LINE__);
                                    return SR_FAILURE;
                                }

                                /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
                                MEMCPY (&pNewSrRtrNextHopInfo->nextHop.Addr.
                                        u4Addr, &(pTmpRt->u4NextHop),
                                        MAX_IPV4_ADDR_LEN);
                                pNewSrRtrNextHopInfo->nextHop.u2AddrType =
                                    SR_IPV4_ADDR_TYPE;
                                pNewSrRtrNextHopInfo->u4OutIfIndex =
                                    u4OutIfIndex;
                                pNewSrRtrNextHopInfo->u4SwapOutIfIndex =
                                    u4OutIfIndex;

                                /* Add NextHop Node in NextHopList in SrRtrNode */
                                TMO_SLL_Insert (&(pRtrInfo->NextHopList), NULL,
                                                &(pNewSrRtrNextHopInfo->
                                                  nextNextHop));

                                if (SrMplsCreateILMPopAndFwd
                                    (pRtrInfo,
                                     pNewSrRtrNextHopInfo) == SR_SUCCESS)
                                {
                                    SR_TRC2 (SR_UTIL_TRC,
                                             "%s:%d SrMplsCreateILMPopAndFwd == SUCCESS \n",
                                             __func__, __LINE__);
                                }
                                else
                                {
                                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                             "%s:%d SrMplsCreateILMPopAndFwd == FAILURE \n",
                                             __func__, __LINE__);
                                    TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                                    (tTMO_SLL_NODE *)
                                                    pNewSrRtrNextHopInfo);
                                    SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                                }
                            }
                        }
                    }
#ifdef IP6_WANTED
                    else if ((nbrRtrId != NULL)
                             && (nbrRtrId->u2AddrType == SR_IPV6_ADDR_TYPE))
                    {
                        /* Donot Create ILM_POP for NbrRtr FEC */
                        if (MEMCMP
                            (nbrRtrId->Addr.Ip6Addr.u1_addr,
                             &(pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr),
                             SR_IPV6_ADDR_LENGTH) != SR_ZERO)
                        {
                            if (Rtm6ApiGetBestRouteEntryInCxt
                                (u4ContextId, &(destAddr.Addr.Ip6Addr),
                                 (UINT1) i4Ipv6RoutePfxLength, SR_ZERO,
                                 &NetIpv6RtInfo) != RTM6_FAILURE)
                            {
                                if (MEMCMP
                                    (NetIpv6RtInfo.NextHop.u1_addr,
                                     tmpIp6Addr.u1_addr,
                                     SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                                {
                                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                                            "%s:%d NH = 0\n", __func__, __LINE__);
                                    return SR_FAILURE;
                                }

                                if (NetIpv6RtInfo.u4Index != SR_ZERO)
                                {
                                    if (NetIpv6GetCfaIfIndexFromPort
                                        (NetIpv6RtInfo.u4Index,
                                         &u4OutIfIndex) == NETIPV6_SUCCESS)
                                    {
					SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d No Port to IfIndex Mapping \n", __func__,
						__LINE__);
                                        return SR_FAILURE;
                                    }
                                }
                                else
                                {
                                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                                            "%s:%d Zero Interface Index\n", __func__, __LINE__);
                                    return SR_FAILURE;
                                }
                            }
                            else
                            {
                                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                                         "%s:%d No route available for destination \n",
                                         __func__, __LINE__);
                                return SR_FAILURE;
                            }
                            /*Create new Next-Hop Node */
                            if ((pNewSrRtrNextHopInfo =
                                 SrCreateNewNextHopNode ()) == NULL)
                            {
                                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                                         "%s:%d pNewSrRtrNextHopInfo == NULL \n",
                                         __func__, __LINE__);
                                return SR_FAILURE;
                            }

                            /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
                            MEMCPY (&pNewSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.
                                    u1_addr, NetIpv6RtInfo.NextHop.u1_addr,
                                    SR_IPV6_ADDR_LENGTH);
                            pNewSrRtrNextHopInfo->nextHop.u2AddrType =
                                SR_IPV6_ADDR_TYPE;
                            pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;

                            /* Add NextHop Node in NextHopList in SrRtrNode */
                            TMO_SLL_Insert (&(pRtrInfo->NextHopList), NULL,
                                            &(pNewSrRtrNextHopInfo->
                                              nextNextHop));

                            if (SrMplsCreateILMPopAndFwd
                                (pRtrInfo, pNewSrRtrNextHopInfo) == SR_SUCCESS)
                            {
                                SR_TRC2 (SR_UTIL_TRC,
                                         "%s:%d SrMplsCreateILMPopAndFwd == SUCCESS \n",
                                         __func__, __LINE__);
                            }
                            else
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d SrMplsCreateILMPopAndFwd == FAILURE \n",
                                         __func__, __LINE__);
                                TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                                (tTMO_SLL_NODE *)
                                                pNewSrRtrNextHopInfo);
                                SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                            }
                        }
                    }
#endif
                    /* 2. Create Pop & Fwd Ends */
                }
            }
        }
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrHandleEventforAllExtLinkLsa
 * Description   : This routine to process all Ext Link LSAs during system events
 * Input(s)      : u1LsaStatus -- NEW/NEW_INSTANCE/FLUSH
 * Output(s)     : None
 * Return(s)     : None
 *****************************************************************************/
VOID
SrHandleEventforAllExtLinkLsa (UINT1 u1LsaStatus)
{
    tSrSidInterfaceInfo *pSrSidEntry = NULL;
    tTMO_SLL_NODE       *pOspfNbrNode = NULL;
    tSrV4OspfNbrInfo    *pOspfNbr     = NULL;
    tGenU4Addr          *pIfIpAddr    = NULL;
    UINT1                u1NbrFound   = SR_FALSE;

    pSrSidEntry = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);
    while (pSrSidEntry != NULL)
    {
        u1NbrFound = SR_FALSE;
        if (pSrSidEntry->u4SidType == SR_SID_ADJACENCY)
        {
            pIfIpAddr = &(pSrSidEntry->ifIpAddr);
            /* Search for OSPF nbr list whether Nbr node present */
            TMO_SLL_Scan (&(gSrV4OspfNbrList), pOspfNbrNode, tTMO_SLL_NODE *)
            {
                pOspfNbr = (tSrV4OspfNbrInfo *) pOspfNbrNode;
                if (pOspfNbr->u4IfIpAddr == pIfIpAddr->Addr.u4Addr)
                {
                    u1NbrFound = SR_TRUE;
                    break;
                }
            }

            if (SR_FALSE == u1NbrFound)
            {
                pSrSidEntry =  RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree,
                                            pSrSidEntry, NULL);
                continue;
            }

            if ((NEW_LSA == u1LsaStatus) || (NEW_LSA_INSTANCE == u1LsaStatus))
            {
                pSrSidEntry->u1AdjSidOprFlags = SR_ADJ_SID_OP_ADD_LSA;

            }
            else if (FLUSHED_LSA == u1LsaStatus)
            {
                pSrSidEntry->u1AdjSidOprFlags = SR_ADJ_SID_OP_DEL_LSA;
            }

            SR_TRC5 (SR_CTRL_TRC, "%s:%d Adj Sid %d for the interface %x cfg type %d\n",
            __func__, __LINE__, pSrSidEntry->u4PrefixSidLabelIndex,
            pSrSidEntry->ifIpAddr.Addr.u4Addr, pSrSidEntry->u1AdjSidOprFlags);

            SrProcessAdjSidChange(pSrSidEntry, pOspfNbr);
       }
       pSrSidEntry =
           RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry,
                          NULL);
    }

    return;
}

PUBLIC UINT4
SrUtilSendLSA (UINT1 u1LsaStatus, UINT4 lsaTLVReceived, UINT1 u1lsaType)
{
    tOpqLSAInfo         opqLSAInfo;
    tOpqLSAInfo         opqLSAInfo1;
    tOpqLSAInfo         opqLSAInfo2;
    tSrSidInterfaceInfo *pSrSidIntf = NULL;
    UINT1               au1RtrAddrTlv[SR_RI_LSA_TLV_LEN];
    UINT1               au1RtrAddrTlv1[SR_RI_LSA_TLV_LEN];

    MEMSET (&opqLSAInfo, 0, sizeof (tOpqLSAInfo));
    MEMSET (&opqLSAInfo1, 0, sizeof (tOpqLSAInfo));
    MEMSET (&opqLSAInfo2, 0, sizeof (tOpqLSAInfo));
    MEMSET (au1RtrAddrTlv, 0, sizeof (SR_RI_LSA_TLV_LEN));
    opqLSAInfo.pu1LSA = au1RtrAddrTlv;
    opqLSAInfo1.pu1LSA = au1RtrAddrTlv1;
    opqLSAInfo2.pu1LSA = au1RtrAddrTlv1;
    UINT4 u4AreaId = gSrGlobalInfo.SrContext.u4areaId;

    if (u1LsaStatus == FLUSHED_LSA)
    {
        if ((lsaTLVReceived == RI_LSA_OPAQUE_TYPE)
                || (lsaTLVReceived == SR_ZERO))
        {
            if (SrConstructRtrInfoLsa (&opqLSAInfo, u1LsaStatus, u1lsaType, u4AreaId) != SR_SUCCESS)
            {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CTRL_TRC, "%s:%d : SrConstructRtrInfoLsa Failed\r\n",
                             __func__, __LINE__);
                    return SR_FAILURE;
            }

            if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA,
                                    OSPF_DEFAULT_CXT_ID, &opqLSAInfo) != SR_SUCCESS)
            {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CTRL_TRC, "%s:%d : RtrInfoLsa Sent Failed\r\n",
                             __func__, __LINE__);
                    return SR_FAILURE;
            }
        }

        pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);
        while (pSrSidIntf != NULL)
        {
            if (pSrSidIntf->u4SidType != SR_SID_NODE)
            {
                pSrSidIntf = RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree,
                                            pSrSidIntf, NULL);
                continue;
            }

            /*Send own NODE SID */
            if ((lsaTLVReceived == OSPF_EXT_PRREFIX_TLV_TYPE)
                || (lsaTLVReceived == SR_ZERO))
            {
                if (SrConstructExtPrefixOpqLsa
                    (&opqLSAInfo1, u1LsaStatus, u1lsaType, pSrSidIntf) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC | SR_UTIL_TRC,
                             "%s:%d : SrConstructExtPrefixOpqLsa Failed\r\n",
                              __func__, __LINE__);
                }
                if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA,
                                    OSPF_DEFAULT_CXT_ID,
                                    &opqLSAInfo1) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC | SR_UTIL_TRC, "%s:%d : ExtPrefixOpqLsa Sent Failed ",
                             __func__, __LINE__);
                }
            }
            pSrSidIntf = RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree,pSrSidIntf,NULL);
        }

        if (lsaTLVReceived == SR_ZERO || lsaTLVReceived == OSPF_EXT_LINK_LSA_OPAQUE_TYPE)
        {
            SrHandleEventforAllExtLinkLsa (u1LsaStatus);
        }
        return SR_SUCCESS;
    }

    if (lsaTLVReceived == RI_LSA_OPAQUE_TYPE || lsaTLVReceived == SR_ZERO)
    {
        if (SrConstructRtrInfoLsa (&opqLSAInfo, u1LsaStatus, u1lsaType, u4AreaId) != SR_SUCCESS)
        {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC | SR_UTIL_TRC, "%s:%d : SrConstructRtrInfoLsa Failed \n",
                         __func__, __LINE__);
                return SR_FAILURE;
        }

        if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA,
                                OSPF_DEFAULT_CXT_ID, &opqLSAInfo) != SR_SUCCESS)
        {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC | SR_UTIL_TRC, "%s:%d : RtrInfoLsa Sent Failed \n",
                         __func__, __LINE__);
                return SR_FAILURE;
        }
    }
    pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    while (pSrSidIntf != NULL)
    {
        if (pSrSidIntf->u4SidType != SR_SID_NODE)
        {
            pSrSidIntf = RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree,
                                        pSrSidIntf, NULL);
            continue;
        }

        /*Send own NODE SID */
        if (lsaTLVReceived == OSPF_EXT_PRREFIX_TLV_TYPE ||
            lsaTLVReceived == SR_ZERO)
        {
            if (SrConstructExtPrefixOpqLsa
                (&opqLSAInfo1, u1LsaStatus, u1lsaType, pSrSidIntf) != SR_SUCCESS)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC | SR_UTIL_TRC,
                         "%s:%d : SrConstructExtPrefixOpqLsa Failed \n", __func__,
                         __LINE__);
            }
            if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA,
                                OSPF_DEFAULT_CXT_ID,
                                &opqLSAInfo1) != SR_SUCCESS)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC | SR_UTIL_TRC, "%s:%d : ExtPrefixOpqLsa Sent Failed \n",
                         __func__, __LINE__);
            }
        }
        pSrSidIntf = RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree,pSrSidIntf,NULL);
    }

#if 1
    if (lsaTLVReceived == SR_ZERO || lsaTLVReceived == OSPF_EXT_LINK_LSA_OPAQUE_TYPE)
    {
        SrHandleEventforAllExtLinkLsa (u1LsaStatus);
    }
#else
            pSrSidEntry = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);
            while (pSrSidEntry != NULL)
            {
                if (pSrSidEntry->u4SidType == SR_SID_ADJACENCY)
                {
                    u1AdjSidConfigured = SR_TRUE;
                    break;
                }
                pSrSidEntry =
                    RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry,
                                   NULL);
            }

            if (u1AdjSidConfigured == SR_TRUE)
            {
                if (SrConstructExtLinkOpqLsa
                    (&opqLSAInfo2, u1LsaStatus, u1lsaType) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d : SrConstructExtLinkOpqLsa Failed \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }

                if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA,
                                    OSPF_DEFAULT_CXT_ID,
                                    &opqLSAInfo2) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : ExtLinkOpqLsa Sent Failed \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
            }
#endif
    return SR_SUCCESS;
}

PUBLIC UINT4
SrV3UtilSendLSA (UINT1 u1LsaStatus, UINT4 lsaTLVReceived)
{
    tV3OpqLSAInfo       opqLSAInfo;
    INT4                i4AbrStatus = SR_FALSE;
    UINT2               u2lsaType = OSPFV3_EXT_INTRA_AREA_PREFIX_LSA;
    UINT1               au1RtrAddrTlv[V3_SR_RI_LSA_TLV_LEN];

    MEMSET (&opqLSAInfo, 0, sizeof (tV3OpqLSAInfo));
    MEMSET (au1RtrAddrTlv, 0, sizeof (V3_SR_RI_LSA_TLV_LEN));
    opqLSAInfo.pu1LSA = au1RtrAddrTlv;

    if (gSrGlobalInfo.u1Ospf3AbrStatus == SR_TRUE)
    {
        i4AbrStatus = SR_TRUE;
    }

    if ((u1LsaStatus == FLUSHED_LSA)
        && (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL))
    {
        if (i4AbrStatus == SR_TRUE)
        {
            u2lsaType = OSPFV3_EXT_INTER_AREA_PREFIX_LSA;
        }
        else
        {
            u2lsaType = OSPFV3_EXT_INTRA_AREA_PREFIX_LSA;
        }

        if (SrOspfV3ConstructExtendedLSA (&opqLSAInfo, u1LsaStatus, u2lsaType)
            != SR_SUCCESS)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d :  Failed \n", __func__, __LINE__);
            return SR_FAILURE;
        }
        if (SrSndMsgToOspfV3 (SR_OSPFV3_SEND_OPQ_LSA,
                              OSPF_DEFAULT_CXT_ID, &opqLSAInfo) != SR_SUCCESS)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d : V3 Extended LSA Sent Failed \n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        return SR_SUCCESS;
    }

    if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
    {
        /*Send own NODE SID */
        if (lsaTLVReceived == V3_RI_LSA_OPAQUE_TYPE ||
            lsaTLVReceived == SR_ZERO)
        {
            if (i4AbrStatus == SR_TRUE)
            {
                u2lsaType = OSPFV3_EXT_INTER_AREA_PREFIX_LSA;
            }
            else
            {
                u2lsaType = OSPFV3_EXT_INTRA_AREA_PREFIX_LSA;
            }

            if (SrOspfV3ConstructExtendedLSA
                (&opqLSAInfo, u1LsaStatus, u2lsaType) != SR_SUCCESS)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d :  Failed \n", __func__, __LINE__);
                return SR_FAILURE;
            }
            if (SrSndMsgToOspfV3 (SR_OSPFV3_SEND_OPQ_LSA,
                                  OSPF_DEFAULT_CXT_ID,
                                  &opqLSAInfo) != SR_SUCCESS)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d : V3 Extended LSA Sent Failed \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }

            u2lsaType = OSPFV3_AREA_SCOPE_OPQ_LSA;

            MEMSET (&opqLSAInfo, 0, sizeof (tV3OpqLSAInfo));
            MEMSET (au1RtrAddrTlv, 0, sizeof (V3_SR_RI_LSA_TLV_LEN));
            opqLSAInfo.pu1LSA = au1RtrAddrTlv;

            if (SrConstructRtrInfoLsaV3 (&opqLSAInfo, u1LsaStatus, u2lsaType) !=
                SR_SUCCESS)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d : SrConstructRtrInfoLsaV3 Failed \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            if (SrSndMsgToOspfV3 (SR_OSPFV3_SEND_OPQ_LSA,
                                  OSPF_DEFAULT_CXT_ID,
                                  &opqLSAInfo) != SR_SUCCESS)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d : RtrInfoLsaV3 Sent Failed \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
        }
    }
    return SR_SUCCESS;
}

PUBLIC UINT4
SrCreateMplsEntry (tSrRtrInfo * pRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo)
{
    tRtInfo             InRtInfo;
    tRtInfo            *pRt = NULL;
    tRtInfo            *pTmpRt = NULL;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pTempSrRtrInfo = NULL;
    UINT4               u4TempNbrRtrId = SR_ZERO;
    UINT4               u2IncarnId = SR_ZERO;
    UINT4               u4NH = SR_ZERO;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4DestAddr = SR_ZERO;
    UINT4               u4DestMask = SR_IPV4_DEST_MASK;
    UINT4               u4NextHop = SR_ZERO;
    tGenU4Addr          destAddr;
    tGenU4Addr          tmpAddr;
    tGenU4Addr          tmpNHAddr;
#ifdef IP6_WANTED
    INT4                i4Ipv6RoutePfxLength = 128;
    UINT4               u4ContextId = VCM_DEFAULT_CONTEXT;
#endif
    tNetIpv6RtInfo      NetIpv6RtInfo;

    MEMSET (&NetIpv6RtInfo, SR_ZERO, sizeof (tNetIpv6RtInfo));
    MEMSET (&InRtInfo, SR_ZERO, sizeof (tRtInfo));
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&tmpAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&tmpNHAddr, SR_ZERO, sizeof (tGenU4Addr));

    if (pSrRtrNextHopInfo == NULL)
    {
        if ((pRtrInfo != NULL) && (pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE))
        {
            SR_TRC3 (SR_UTIL_TRC, "%s:%d Entry : %x \n", __func__, __LINE__,
                     pRtrInfo->prefixId.Addr.u4Addr);

            MEMCPY (&(destAddr.Addr.u4Addr), &pRtrInfo->prefixId.Addr.u4Addr,
                    MAX_IPV4_ADDR_LEN);

            /* if route is added to delay list, remove the node from delay list */
            if ((pRtrInfo->u1OprFlags & SR_OPR_F_ADD_DELAY_LIST)
                == SR_OPR_F_ADD_DELAY_LIST)
            {
                SRProcessDelayedRouteInfo(pRtrInfo);
            }

            InRtInfo.u4DestNet = destAddr.Addr.u4Addr;
            InRtInfo.u4DestMask = u4DestMask;

            RtmApiGetBestRouteEntryInCxt (u2IncarnId, InRtInfo, &pRt);

            pTmpRt = pRt;

            if (pTmpRt != NULL)
            {
                SR_TRC4 (SR_UTIL_TRC,
                         "%s:%d ROUTE Found: NH %x u4OutIfIndex %d \n", __func__,
                         __LINE__, pTmpRt->u4NextHop, pTmpRt->u4RtIfIndx);
            }

#ifdef MPLS_SR_ECMP_WANTED
            for (; ((pTmpRt != NULL) && (pTmpRt->i4Metric1 == pRt->i4Metric1));
                 pTmpRt = pTmpRt->pNextAlternatepath)
#else
            if (pTmpRt != NULL)
#endif
            {
                u4NH = pTmpRt->u4NextHop;
                pNewSrRtrNextHopInfo = NULL;
                /*Create new Next-Hop Node */
                if ((pNewSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pNewSrRtrNextHopInfo == NULL \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
                if (u4NH == SR_ZERO)
                {
                    SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d NO ROUTE Found (NH=0) FEC %x \n",
                             __func__, __LINE__, u4DestAddr);
                    SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                    return SR_FAILURE;
                }

                if (NetIpv4GetCfaIfIndexFromPort (pTmpRt->u4RtIfIndx,
                                                  &u4OutIfIndex) ==
                    NETIPV4_FAILURE)
                {
                    SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                             "%s:%d No Port is mapped with u4RtIfIndx %d \n",
                             __func__, __LINE__, pTmpRt->u4RtIfIndx);
                    SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                    return SR_FAILURE;
                }
                /* Get Nbr Rtr Id from NextHop */
                tmpNHAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                MEMCPY (&tmpNHAddr.Addr.u4Addr, &(u4NH), MAX_IPV4_ADDR_LEN);
                pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&tmpNHAddr);
                if (pTempSrRtrInfo != NULL)
                {
                    MEMCPY (&u4TempNbrRtrId,
                            &(pTempSrRtrInfo->prefixId.Addr.u4Addr),
                            MAX_IPV4_ADDR_LEN);
                }
                else
                {
                    u4TempNbrRtrId = SrNbrRtrFromNextHop (u4NH);
                }
                pNewSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
                MEMCPY (&pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr, &(u4NH),
                        MAX_IPV4_ADDR_LEN);
                pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                pNewSrRtrNextHopInfo->u4SwapOutIfIndex = u4OutIfIndex;
                pNewSrRtrNextHopInfo->nbrRtrId = u4TempNbrRtrId;
                SR_TRC4 (SR_UTIL_TRC,
                         "%s:%d ROUTE Found with NH %x u4OutIfIndex %d \n",
                         __func__, __LINE__, u4NH, u4OutIfIndex);

                /* Add NextHop Node in NextHopList in SrRtrNode */
                TMO_SLL_Insert (&(pRtrInfo->NextHopList), NULL,
                                &(pNewSrRtrNextHopInfo->nextNextHop));

                if (SrMplsCreateFTN (pRtrInfo, pNewSrRtrNextHopInfo) ==
                    SR_FAILURE)
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateFTN  FAILURE \n",
                             __func__, __LINE__);

                    /* FTN & POP&FWD cannot co-exist (Preventive check as sometime MLIBUPDATE API failing "to do") */
                    if ((pNewSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                        SR_FTN_CREATED)
                    {
                        SR_TRC4 (SR_UTIL_TRC,
                                 "%s:%d FTN already created for %x \n", __func__,
                                 __LINE__, u4DestAddr, u4NH);
#ifdef MPLS_SR_ECMP_WANTED
                        continue;
#else
                       return SR_SUCCESS;
#endif
                    }

                    /* If Next-Hop is not SR Enabled Create ILM-Pop&Fwd and return */
                    tmpAddr.Addr.u4Addr = u4NH;
                    tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                    pTempSrRtrInfo = NULL;
                    pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&tmpAddr);

                    if (pTempSrRtrInfo == NULL)
                    {
                        pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                        SR_TRC2 (SR_UTIL_TRC,
                                 "%s:%d pTempSrRtrInfo == NULL: ~ Next-Hop is not SR Enabled (Create: Pop&Fwd) ~ \n",
                                 __func__, __LINE__);
                        if (SrMplsCreateILMPopAndFwd
                            (pRtrInfo, pNewSrRtrNextHopInfo) == SR_SUCCESS)
                        {
                            SR_TRC2 (SR_UTIL_TRC,
                                     "%s:%d SrMplsCreateILMPop == SUCCESS \n",
                                     __func__, __LINE__);
                        }
                        else
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d POP&FWD Create Failure \n", __func__,
                                     __LINE__);
                            /* Delete the node from the list in case of FTN Creation failure */
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
                                        (tTMO_SLL_NODE *) pNewSrRtrNextHopInfo);
                        SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                    }
#ifdef MPLS_SR_ECMP_WANTED
                    continue;
#else
                    return SR_FAILURE;
#endif
                }
                SR_TRC2 (SR_UTIL_TRC, "%s:%d Creating ILM SWAP \n", __func__,
                         __LINE__);

                if (SrMplsCreateILM (pRtrInfo, pNewSrRtrNextHopInfo) ==
                        SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateILM  FAILURE \n",
                            __func__, __LINE__);
#ifdef MPLS_SR_ECMP_WANTED
                    continue;
#else
                    return SR_FAILURE;
#endif
                }

                /* Create LFA LSP for the
                 *     a. Node-A which is coming up
                 *     b. Nodes which is having LFA next hop as the Node-A */
                SrLfaCreateLSP (pRtrInfo, pNewSrRtrNextHopInfo);

                u4OutIfIndex = SR_ZERO;
            }
            else
            {
                SR_TRC3 (SR_UTIL_TRC, "%s:%d: NH not exists for Rtr %x \n",
                         __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
            }
        }
#ifdef IP6_WANTED
        else if ((pRtrInfo != NULL)
                 && (pRtrInfo->u2AddrType == SR_IPV6_ADDR_TYPE))
        {
            MEMCPY (destAddr.Addr.Ip6Addr.u1_addr,
                    pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr, MAX_IPV6_ADDR_LEN);
            if (Rtm6ApiGetBestRouteEntryInCxt
                (u4ContextId, &(destAddr.Addr.Ip6Addr),
                 (UINT1) i4Ipv6RoutePfxLength, SR_ZERO,
                 &NetIpv6RtInfo) != RTM6_FAILURE)
            {
                if (NetIpv6RtInfo.u4Index != SR_ZERO)
                {
                    if (NetIpv6GetCfaIfIndexFromPort
                        (NetIpv6RtInfo.u4Index,
                         &u4OutIfIndex) == NETIPV6_SUCCESS)
                    {
                        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d No Port to IfIndex Mapping \n", __func__,
                                __LINE__);
                        return SR_FAILURE;
                    }
                }
                else
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d zero Interface index \n", __func__,
                            __LINE__);
                    return SR_FAILURE;
                }
            }
            else
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d No route available for destination \n", __func__,
                         __LINE__);
                return SR_FAILURE;
            }
            /*Create new Next-Hop Node */
            if ((pNewSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pNewSrRtrNextHopInfo == NULL \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }

            /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
            pNewSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
            MEMCPY (pNewSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                    NetIpv6RtInfo.NextHop.u1_addr, MAX_IPV6_ADDR_LEN);
            pNewSrRtrNextHopInfo->nextHop.u2AddrType = pRtrInfo->u2AddrType;
            pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
            pNewSrRtrNextHopInfo->u4SwapOutIfIndex = u4OutIfIndex;

            /* Add NextHop Node in NextHopList in SrRtrNode */
            TMO_SLL_Insert (&(pRtrInfo->NextHopList), NULL,
                            &(pNewSrRtrNextHopInfo->nextNextHop));

            if (SrMplsCreateFTN (pRtrInfo, pNewSrRtrNextHopInfo) == SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateFTN  FAILURE \n",
                         __func__, __LINE__);

                /* FTN & POP&FWD cannot co-exist (Preventive check as sometime MLIBUPDATE API failing "to do") */
                if ((pNewSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                    SR_FTN_CREATED)
                {
                    SR_TRC2 (SR_UTIL_TRC, "%s:%d FTN already created \n", __func__,
                             __LINE__);
                    return SR_SUCCESS;
                }
                else
                {
                    /* If Next-Hop is not SR Enabled Create ILM-Pop&Fwd and return */
                    MEMCPY (tmpAddr.Addr.Ip6Addr.u1_addr,
                            NetIpv6RtInfo.NextHop.u1_addr, sizeof (tIp6Addr));
                    tmpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
                    pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&tmpAddr);

                    if (pTempSrRtrInfo == NULL)
                    {
                        pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                        SR_TRC2 (SR_UTIL_TRC,
                                 "%s:%d pTempSrRtrInfo == NULL: ~ Next-Hop is not SR Enabled (Create: Pop&Fwd) ~ \n",
                                 __func__, __LINE__);
                        if (SrMplsCreateILMPopAndFwd
                            (pRtrInfo, pNewSrRtrNextHopInfo) == SR_SUCCESS)
                        {
                            SR_TRC2 (SR_UTIL_TRC,
                                     "%s:%d SrMplsCreateILMPop == SUCCESS \n",
                                     __func__, __LINE__);
                        }
                        else
                        {
                            /* Delete the node from the list in case of FTN Creation failure */
                            TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                            (tTMO_SLL_NODE *)
                                            pNewSrRtrNextHopInfo);
                            SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d POP&FWD Create Failure \n", __func__,
                                     __LINE__);
                            return SR_FAILURE;
                        }

                    }
                    else
                    {
                        TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                        (tTMO_SLL_NODE *) pNewSrRtrNextHopInfo);
                        SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d POP&FWD Create Failure \n",
                                 __func__, __LINE__);
                        return SR_FAILURE;
                    }
                }
            }
            else if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
            {
                SR_TRC2 (SR_UTIL_TRC, "%s:%d Creating ILM SWAP \n", __func__,
                         __LINE__);

                if (SrMplsCreateILM (pRtrInfo, pNewSrRtrNextHopInfo) ==
                    SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateILM  FAILURE \n",
                             __func__, __LINE__);
                }
            }
        }
#endif
    }
    else
    {
       MEMCPY (&u4NextHop, &pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                sizeof (UINT4));
       if ((pRtrInfo != NULL) && (pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE))
       {
            /* if route is added to delay list, remove the node from delay list */
            if ((pRtrInfo->u1OprFlags & SR_OPR_F_ADD_DELAY_LIST)
                       == SR_OPR_F_ADD_DELAY_LIST)
            {
                SRProcessDelayedRouteInfo(pRtrInfo);
            }

            if (SrMplsCreateFTN (pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateFTN  FAILURE \n", __func__,
                     __LINE__);
                /* FTN & POP&FWD cannot co-exist (Preventive check as sometime MLIBUPDATE API failing "to do") */
                if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                    SR_FTN_CREATED)
                {
                    SR_TRC2 (SR_UTIL_TRC, "%s:%d FTN already created \n", __func__,
                             __LINE__);
                    return SR_SUCCESS;
                }
                else
                {
                    /* If Next-Hop is not SR Enabled Create ILM-Pop&Fwd and return */
                    tmpAddr.Addr.u4Addr = u4NextHop;
                    tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                    pTempSrRtrInfo = NULL;
                    pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&tmpAddr);

                    if (pTempSrRtrInfo == NULL)
                    {
                        SR_TRC2 (SR_UTIL_TRC,
                                 "%s:%d pTempSrRtrInfo == NULL: ~ Next-Hop is not SR Enabled (Create: Pop&Fwd) ~ \n",
                                 __func__, __LINE__);
                        if (SrMplsCreateILMPopAndFwd
                            (pRtrInfo, pSrRtrNextHopInfo) == SR_SUCCESS)
                        {
                            SR_TRC2 (SR_UTIL_TRC,
                                     "%s:%d SrMplsCreateILMPop == SUCCESS \n",
                                     __func__, __LINE__);
                        }
                        else
                        {
                            /* Delete the node from the list in case of FTN Creation failure */
                            TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                            (tTMO_SLL_NODE *)
                                            pSrRtrNextHopInfo);
                            SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d POP&FWD Create Failure \n", __func__,
                                     __LINE__);
                            return SR_FAILURE;
                        }

                    }
                    else
                    {
                        TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                        (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                        SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d POP&FWD Create Failure \n",
                                 __func__, __LINE__);
                        return SR_FAILURE;
                    }
                }
            }
            SR_TRC2 (SR_UTIL_TRC, "%s:%d Creating ILM SWAP \n", __func__,
                     __LINE__);

            if (SrMplsCreateILM (pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateILM  FAILURE \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }

        /* Create LFA LSP for the
         *     a. Node-A which is coming up
         *     b. Nodes which is having LFA next hop as the Node-A */
        SrLfaCreateLSP (pRtrInfo, pSrRtrNextHopInfo);
      }
#ifdef IP6_WANTED
       else if ((pRtrInfo != NULL)
               && (pRtrInfo->u2AddrType == SR_IPV6_ADDR_TYPE))
       {

           if (SrMplsCreateFTN (pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
           {
               SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateFTN  FAILURE \n",
                       __func__, __LINE__);

               /* FTN & POP&FWD cannot co-exist (Preventive check as sometime MLIBUPDATE API failing "to do") */
               if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                       SR_FTN_CREATED)
               {
                   SR_TRC2 (SR_UTIL_TRC, "%s:%d FTN already created \n", __func__,
                           __LINE__);
                   return SR_SUCCESS;
               }
               else
               {
                   /* If Next-Hop is not SR Enabled Create ILM-Pop&Fwd and return */
                   MEMCPY (tmpAddr.Addr.Ip6Addr.u1_addr,
                           NetIpv6RtInfo.NextHop.u1_addr, sizeof (tIp6Addr));
                   tmpAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
                   pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&tmpAddr);

                   if (pTempSrRtrInfo == NULL)
                   {
                       pSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                       SR_TRC2 (SR_UTIL_TRC,
                               "%s:%d pTempSrRtrInfo == NULL: ~ Next-Hop is not SR Enabled (Create: Pop&Fwd) ~ \n",
                               __func__, __LINE__);
                       if (SrMplsCreateILMPopAndFwd
                               (pRtrInfo, pSrRtrNextHopInfo) == SR_SUCCESS)
                       {
                           SR_TRC2 (SR_UTIL_TRC,
                                   "%s:%d SrMplsCreateILMPop == SUCCESS \n",
                                   __func__, __LINE__);
                       }
                       else
                       {
                           /* Delete the node from the list in case of FTN Creation failure */
                           TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                   (tTMO_SLL_NODE *)
                                   pSrRtrNextHopInfo);
                           SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                           SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                   "%s:%d POP&FWD Create Failure \n", __func__,
                                   __LINE__);
                           return SR_FAILURE;
                       }

                   }
                   else
                   {
                       TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                               (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                       SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);
                       SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d POP&FWD Create Failure \n",
                               __func__, __LINE__);
                       return SR_FAILURE;
                   }
               }
           }
           else if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
           {
               SR_TRC2 (SR_UTIL_TRC, "%s:%d Creating ILM SWAP \n", __func__,
                       __LINE__);

               if (SrMplsCreateILM (pRtrInfo, pSrRtrNextHopInfo) ==
                       SR_FAILURE)
               {
                   SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateILM  FAILURE \n",
                           __func__, __LINE__);
               }
           }
       }
#endif
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

PUBLIC UINT4
SrDeleteMplsEntry (tSrRtrInfo * pRtrInfo)
{
    tSrInSegInfo        SrInSegInfo;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrNextHopInfo   SrRtrNextHopInfo;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrRlfaPathInfo     SrRlfaPathInfo;
    tSrInSegInfo        SrRlfaInSegInfo;
    UINT4               u4SrGbMinIndex = SR_ZERO;
    UINT4               u4IpAddress = SR_ZERO;
    UINT4               u4OutIfIndex = SR_ZERO;
    INT4                i4LfaStatus = ALTERNATE_DISABLED;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&SrRtrNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&SrRlfaInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&SrRlfaPathInfo, SR_ZERO, sizeof (tSrRlfaPathInfo));

    nmhGetFsSrV4AlternateStatus (&i4LfaStatus);

    if (pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        u4SrGbMinIndex = gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
    }
    else if (pRtrInfo->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        u4SrGbMinIndex = gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;
    }

    /* if route is added to delay list, remove the node from delay list */
    if ((pRtrInfo->u1OprFlags & SR_OPR_F_ADD_DELAY_LIST)
            == SR_OPR_F_ADD_DELAY_LIST)
    {
        SRProcessDelayedRouteInfo(pRtrInfo);
    }

    SrInSegInfo.u4InLabel = (u4SrGbMinIndex + pRtrInfo->u4SidValue);
    /* Handling RLFA LSP Deletion  */
    while ((pSrRlfaPathInfo = SrGetNextRlfaRtEntry (&SrRlfaPathInfo)) != NULL)
    {
        if (pSrRlfaPathInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            /* Checking RLFA LSP is present for given Rtr ID */
            if (pSrRlfaPathInfo->destAddr.Addr.u4Addr ==
                (pRtrInfo->prefixId.Addr.u4Addr))
            {
                SR_TRC4 (SR_UTIL_TRC,
                         "%s : %d Deleting RLFA LSP with Dest : %x Tunnel Rtr : %x \r\n",
                         __func__, __LINE__,
                         pSrRlfaPathInfo->destAddr.Addr.u4Addr,
                         pSrRlfaPathInfo->mandRtrId.Addr.u4Addr);
                if (SrRlfaDeleteFTNAndILM
                    (&(pRtrInfo->prefixId), &pSrRlfaPathInfo->mandRtrId,
                     &pSrRlfaPathInfo->rlfaNextHop, &SrInSegInfo,
                     u4SrGbMinIndex) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d Deletion of SR RLFA FTN/ILM Failure \n",
                             __func__, __LINE__);
                }
            }
            /* Checking given Rtr Id is Tunnel Router for any SR-RLFA LSP */
            else if ((pSrRlfaPathInfo->mandRtrId.Addr.u4Addr) ==
                     (pRtrInfo->prefixId.Addr.u4Addr))
            {
                SR_TRC4 (SR_UTIL_TRC,
                         "%s : %d Deleting RLFA LSP with Dest : %x Tunnel Rtr : %x \r\n",
                         __func__, __LINE__,
                         pSrRlfaPathInfo->destAddr.Addr.u4Addr,
                         pSrRlfaPathInfo->mandRtrId.Addr.u4Addr);
                if (SrRlfaDeleteFTNAndILM
                    (&(pSrRlfaPathInfo->destAddr),
                     &(pSrRlfaPathInfo->mandRtrId),
                     &(pSrRlfaPathInfo->rlfaNextHop), &SrInSegInfo,
                     u4SrGbMinIndex) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d Deletion of SR RLFA FTN/ILM Failure for mantry Rtr Down \n",
                             __func__, __LINE__);
                }

            }
            SrRlfaPathInfo.destAddr.Addr.u4Addr =
                pSrRlfaPathInfo->destAddr.Addr.u4Addr;
            SrRlfaPathInfo.u4DestMask = pSrRlfaPathInfo->u4DestMask;
            SrRlfaPathInfo.mandRtrId.Addr.u4Addr =
                pSrRlfaPathInfo->mandRtrId.Addr.u4Addr;
        }
        else if (pSrRlfaPathInfo->destAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            /* Checking RLFA LSP is present for given Rtr ID */
            if (MEMCMP (pSrRlfaPathInfo->destAddr.Addr.Ip6Addr.u1_addr,
                        pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                SR_TRC4 (SR_UTIL_TRC,
                         "%s : %d Deleting RLFA LSP with Dest : %x Tunnel Rtr : %x \r\n",
                         __func__, __LINE__,
                         pSrRlfaPathInfo->destAddr.Addr.Ip6Addr.u1_addr,
                         pSrRlfaPathInfo->mandRtrId.Addr.Ip6Addr.u1_addr);
                if (SrRlfaDeleteFTNAndILM
                    (&(pRtrInfo->prefixId), &pSrRlfaPathInfo->mandRtrId,
                     &pSrRlfaPathInfo->rlfaNextHop, &SrInSegInfo,
                     u4SrGbMinIndex) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d Deletion of SR RLFA FTN/ILM Failure for Dest \n",
                             __func__, __LINE__);
                }

            }
            /* Checking given Rtr Id is Tunnel Router for any SR-RLFA LSP */
            else if (MEMCMP (pSrRlfaPathInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                             pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr,
                             SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                SR_TRC4 (SR_UTIL_TRC,
                         "%s : %d Deleting RLFA LSP with Dest : %x Tunnel Rtr : %x \r\n",
                         __func__, __LINE__,
                         pSrRlfaPathInfo->destAddr.Addr.Ip6Addr.u1_addr,
                         pSrRlfaPathInfo->mandRtrId.Addr.Ip6Addr.u1_addr);
                if (SrRlfaDeleteFTNAndILM
                    (&(pSrRlfaPathInfo->destAddr),
                     &(pSrRlfaPathInfo->mandRtrId),
                     &(pSrRlfaPathInfo->rlfaNextHop), &SrInSegInfo,
                     u4SrGbMinIndex) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d Deletion of SR RLFA FTN/ILM Failure for mantry Rtr Down \n",
                             __func__, __LINE__);
                }
            }
            MEMCPY (SrRlfaPathInfo.destAddr.Addr.Ip6Addr.u1_addr,
                    pSrRlfaPathInfo->destAddr.Addr.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);
            MEMCPY (SrRlfaPathInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                    pSrRlfaPathInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);
            SrRlfaPathInfo.u4DestMask = pSrRlfaPathInfo->u4DestMask;
        }
        SrRlfaPathInfo.destAddr.u2AddrType =
            pSrRlfaPathInfo->destAddr.u2AddrType;
        SrRlfaPathInfo.mandRtrId.u2AddrType =
            pSrRlfaPathInfo->mandRtrId.u2AddrType;
    }

    /* Check and process this router is part of any Ti-LFA path  */
    SrTiLfaProcessForSrRouterNodeDelete(pRtrInfo);

    /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
    SrInSegInfo.u4InLabel = (u4SrGbMinIndex + pRtrInfo->u4SidValue);

    /* Delete All FTNs & ILMs for SrRtrIp and All Next-Hops(if more than 1 NH -in case of ECMP) */
    if (TMO_SLL_Count (&pRtrInfo->NextHopList) != SR_ZERO)
    {
        TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                      pSrRtrNextHopNode, tTMO_SLL_NODE *)
        {
            pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
            if ((i4LfaStatus == ALTERNATE_ENABLED) &&
                (pRtrInfo->u4LfaNextHop != SR_ZERO))
            {
                /*Deleting LFA LSP */
                u4IpAddress = (pRtrInfo->u4LfaNextHop & pRtrInfo->u4DestMask);
                if (CfaIpIfGetIfIndexForNetInCxt
                    (SR_ZERO, u4IpAddress, &u4OutIfIndex) == CFA_FAILURE)
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d Exiting: Not able to get Out IfIndex for Lfa NxtHop \n",
                             __func__, __LINE__);
                }
                SrRtrNextHopInfo.u4OutIfIndex = pRtrInfo->u4LfaOutIfIndex;
                SrRtrNextHopInfo.u4SwapOutIfIndex =
                    pRtrInfo->u4LfaSwapOutIfIndex;
                SrRtrNextHopInfo.u1MPLSStatus = SR_ONE;
                SrRtrNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
                SrRtrNextHopInfo.nextHop.Addr.u4Addr = pRtrInfo->u4LfaNextHop;
                SrRtrNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
                MEMCPY (&SrRtrNextHopInfo.PrimarynextHop,
                        &pSrRtrNextHopInfo->nextHop, sizeof (tGenU4Addr));
                if (SrMplsDeleteFTN (pRtrInfo, &SrRtrNextHopInfo) == SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to delete LFA FTN \n",
                             __func__, __LINE__);
                }
                if (SrMplsDeleteILM (&SrInSegInfo, pRtrInfo, &SrRtrNextHopInfo)
                    == SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to delete LFA ILM SWAP \n",
                             __func__, __LINE__);
                }
                pRtrInfo->bIsLfaActive = SR_FALSE;
                pRtrInfo->u4LfaNextHop = SR_FALSE;
                pRtrInfo->u1LfaType = SR_ZERO;
                pRtrInfo->u4LfaOutIfIndex = SR_ZERO;
                pRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
            }
            /* Delete Primary LSP */
            if (SrMplsDeleteILM (&SrInSegInfo, pRtrInfo, pSrRtrNextHopInfo) ==
                SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to delete ILM SWAP \n",
                         __func__, __LINE__);
            }
            if (SrMplsDeleteFTN (pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to delete FTN \n", __func__,
                         __LINE__);
            }
        }
        if (&pRtrInfo->NextHopList != NULL)
        {
            TMO_SLL_FreeNodes ((tTMO_SLL *) & pRtrInfo->NextHopList,
                               SrMemPoolIds[MAX_SR_RTR_NH_SIZING_ID]);
        }
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

PUBLIC UINT4
SrMplsCreateILMPopAndFwd (tSrRtrInfo * pRtrInfo,
                          tSrRtrNextHopInfo * pSrRtrNextHopInfo)
{
    tSrInSegInfo        SrInSegInfo;
    UINT2               u2MlibOperation = MPLS_MLIB_ILM_CREATE;

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    if (pRtrInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pRtrInfo == NULL \n", __func__, __LINE__);
        return SR_FAILURE;
    }

    if (pRtrInfo->u4SidValue == SR_ZERO)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d: FAILURE: pRtrInfo->u4SidValue == 0 \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }

    SR_TRC2 (SR_UTIL_TRC, "%s:%d Creating ILM POP \n", __func__, __LINE__);

    SrInSegInfo.u4InLabel =
        SrReserveMplsInLabel (pRtrInfo->u2AddrType, pRtrInfo->u1SidIndexType,
                              pRtrInfo->u4SidValue);

    if (pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        MEMCPY (&(SrInSegInfo.Fec.Prefix.u4Addr),
                &pRtrInfo->prefixId.Addr.u4Addr, MAX_IPV4_ADDR_LEN);
        SrInSegInfo.Fec.u2AddrFmly = pRtrInfo->u2AddrType;
    }
    else if (pRtrInfo->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY ((SrInSegInfo.Fec.Prefix.Ip6Addr.u1_addr),
                pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr, MAX_IPV6_ADDR_LEN);
        SrInSegInfo.Fec.u2AddrFmly =
            gSrGlobalInfo.pV6SrSelfNodeInfo->ipAddrType;
    }

    SR_TRC3 (SR_UTIL_TRC, "%s:%d SrInSegInfo.u4InLabel == %d \n", __func__,
             __LINE__, SrInSegInfo.u4InLabel);

    if (SrInSegInfo.u4InLabel == SR_ZERO)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrInSegInfo.u4InLabel == SR_ZERO \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }

    if (SrMplsCreateOrDeleteLsp
        (u2MlibOperation, NULL, &SrInSegInfo, pSrRtrNextHopInfo) == SR_SUCCESS)
    {
        pSrRtrNextHopInfo->u1MPLSStatus |= SR_ILM_POP_CREATED;
        SR_TRC2 (SR_UTIL_TRC,
                 "%s:%d u2MlibOperation = MPLS_MLIB_ILM_CREATE; SrMplsCreateOrDeleteLsp: SR_SUCCESS \n",
                 __func__, __LINE__);
    }
    else
    {
        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d u2MlibOperation = MPLS_MLIB_ILM_CREATE; SrMplsCreateOrDeleteLsp: SR_FAILURE \n",
                 __func__, __LINE__);
        if (SrReleaseMplsInLabel (pRtrInfo->u2AddrType, pRtrInfo->u4SidValue) ==
            SR_FAILURE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Unable to Release In-Label \n", __func__,
                     __LINE__);
            return SR_FAILURE;
        }
        return SR_FAILURE;
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

PUBLIC UINT4
SrMplsDeleteILMPopAndFwd (tSrRtrInfo * pRtrInfo,
                          tSrRtrNextHopInfo * pSrRtrNextHopInfo)
{
    tSrInSegInfo        SrInSegInfo;
    UINT2               u2MlibOperation = MPLS_MLIB_ILM_DELETE;

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    if (pRtrInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pRtrInfo == NULL \n", __func__, __LINE__);
        return SR_FAILURE;
    }

    if (((pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE)
         && (SrUtilCheckNodeIdConfigured () == SR_SUCCESS))
         || ((pRtrInfo->u2AddrType == SR_IPV6_ADDR_TYPE)
            && (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)))
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Deleting ILM POP \n", __func__, __LINE__);

        if (pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            MEMCPY (&(SrInSegInfo.Fec.Prefix.u4Addr),
                    &pRtrInfo->prefixId.Addr.u4Addr, MAX_IPV4_ADDR_LEN);
            /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
            SrInSegInfo.u4InLabel =
                (gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex +
                 pRtrInfo->u4SidValue);
            SrInSegInfo.Fec.u2AddrFmly = pRtrInfo->u2AddrType;
        }
        else if (pRtrInfo->u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY ((SrInSegInfo.Fec.Prefix.Ip6Addr.u1_addr),
                    pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr, MAX_IPV6_ADDR_LEN);
            /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
            SrInSegInfo.u4InLabel =
                (gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex +
                 pRtrInfo->u4SidValue);
            SrInSegInfo.Fec.u2AddrFmly =
                gSrGlobalInfo.pV6SrSelfNodeInfo->ipAddrType;
        }

        if (SrMplsCreateOrDeleteLsp
            (u2MlibOperation, NULL, &SrInSegInfo,
             pSrRtrNextHopInfo) == SR_SUCCESS)
        {
            pSrRtrNextHopInfo->u1MPLSStatus &= (UINT1) (~SR_ILM_POP_CREATED);

            SR_TRC2 (SR_UTIL_TRC,
                     "%s:%d u2MlibOperation = MPLS_MLIB_ILM_DELETE; SrMplsCreateOrDeleteLsp: SR_SUCCESS \n",
                     __func__, __LINE__);

            if (SrReleaseMplsInLabel (pRtrInfo->u2AddrType,
                                      pRtrInfo->u4SidValue) == SR_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Unable to Release In-Label \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
        }
        else
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateOrDeleteLsp: SR_FAILURE \n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrUtilCreateorDeleteILM
 * Description   : This routine will populate SrInSegInfo based on the paramters
 *                    received and call SrMplsCreateOrDeleteLsp for creation of
 *                    MPLS entries.
 *
 * Input(s)      : pSrSidInfo - Pointer to SID structure
 *                    u4L3IfIndex - Incoming Interface
 *                    u4SidValue  - SID Label Value
 *                    destPrefix  - Destination address
 *                    nextHop     - Next Hop address
 *
 * Output(s)     : None
 * Return(s)     : SUCCESS/FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrUtilCreateorDeleteILM (tSrSidInterfaceInfo * pSrSidInfo, UINT4 u4SidValue,
                         tGenU4Addr * pDestPrefix, tGenU4Addr * pNextHop,
                         UINT2 u2MlibOperation, UINT4 u4OutIfIndex)
{
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    tGenU4Addr          dummyNextHop;
    tSrInSegInfo        SrInSegInfo;
    UINT4               u4L3IfIndex = SR_ZERO;
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    UNUSED_PARAM (u4MplsTnlIfIndex);
    UNUSED_PARAM (pDestPrefix);
    MEMSET (&dummyNextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    if (pSrSidInfo == NULL)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
        "%s:%d Pointer to SID structure is NULL \n",__func__, __LINE__);

        return SR_FAILURE;
    }

    u4L3IfIndex = pSrSidInfo->u4IfIndex;
    UNUSED_PARAM (u4L3IfIndex);

    SrInSegInfo.u4InIfIndex = SR_ZERO;
    SrInSegInfo.Fec.u2AddrFmly = pSrSidInfo->ifIpAddr.u2AddrType;
    SrInSegInfo.u4InLabel = u4SidValue;

    SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrInSegInfo.u4InLabel == %d \n", __func__,
             __LINE__, SrInSegInfo.u4InLabel);

    if (SrInSegInfo.u4InLabel == SR_ZERO)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrInSegInfo.u4InLabel == SR_ZERO \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }

    if ((pSrSidInfo->u4SidType == SR_SID_NODE) ||
        (pSrSidInfo->u4SidTypeV3 == SR_SID_NODE))
    {
        if (pNextHop->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            MEMCPY (&(SrInSegInfo.Fec.Prefix.u4Addr), &(pSrSidInfo->ifIpAddr.Addr.u4Addr),
                    SR_IPV4_ADDR_LENGTH);
        }
        else if (pNextHop->u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (SrInSegInfo.Fec.Prefix.Ip6Addr.u1_addr,
                    pSrSidInfo->ifIpAddr.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        }
    }
    else
    {
        if (pNextHop->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            MEMCPY (&(SrInSegInfo.Fec.Prefix.u4Addr), &pNextHop->Addr.u4Addr,
                    SR_IPV4_ADDR_LENGTH);
        }
        else if (pNextHop->u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (SrInSegInfo.Fec.Prefix.Ip6Addr.u1_addr,
                    pNextHop->Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
        }
    }

    MEMCPY (&(SrInSegInfo.Fec.u4MaskOrPrefixLen), &pSrSidInfo->ifIpAddrMask,
            MAX_IPV4_ADDR_LEN);

    if (u2MlibOperation == MPLS_MLIB_ILM_CREATE)
    {
        SrInSegInfo.u4InLabel =
            SrReserveMplsInLabel (pNextHop->u2AddrType, SR_SID_ABS_INDEX,
                                  u4SidValue);
        if ((pNextHop->u2AddrType == SR_IPV4_ADDR_TYPE)
            && (pNextHop->Addr.u4Addr != SR_ZERO))
        {
            SrInSegInfo.u4OutIfIndex = u4OutIfIndex;
        }
        else if ((pNextHop->u2AddrType == SR_IPV6_ADDR_TYPE) &&
                 (MEMCMP
                  (pNextHop->Addr.Ip6Addr.u1_addr,
                   dummyNextHop.Addr.Ip6Addr.u1_addr,
                   SR_IPV6_ADDR_LENGTH) != SR_ZERO))
        {
            SrInSegInfo.u4OutIfIndex = u4OutIfIndex;
        }
        if (SrMplsCreateOrDeleteLsp (u2MlibOperation, NULL, &SrInSegInfo, NULL)
            == SR_SUCCESS)
        {
            SR_TRC3 (SR_UTIL_TRC,
                     "%s:%d u2MlibOperation = %d SrMplsCreateOrDeleteLsp: SR_SUCCESS \n",
                     __func__, __LINE__, u2MlibOperation);
        }
    }
    else if (u2MlibOperation == MPLS_MLIB_ILM_DELETE)
    {
        /* SrInSegInfo.u4InIfIndex   = pSrSidInfo->u4MplsInterface; */
        SrInSegInfo.u4OutIfIndex = u4OutIfIndex;
        if (SrMplsCreateOrDeleteLsp (u2MlibOperation, NULL, &SrInSegInfo, NULL)
            == SR_SUCCESS)
        {
            SR_TRC3 (SR_UTIL_TRC,
                     "%s:%d u2MlibOperation = %d SrMplsCreateOrDeleteLsp: SR_SUCCESS \n",
                     __func__, __LINE__, u2MlibOperation);
            /* Release In-Label to Label_manager */
            MPLS_CMN_LOCK ();
            if (MplsReleaseLblToLblGroup (u4SidValue) == MPLS_FAILURE)
            {
                MPLS_CMN_UNLOCK ();
                SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Label (%d) Release Failure \n",
                         __func__, __LINE__, u4SidValue);
                return SR_FAILURE;
            }

            MPLS_CMN_UNLOCK ();
        }
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/************************************************************************
 * Function Name   : SrGetNextRlfaRtEntry
 * Description     : Function to get TePath Entry from DestAddr, Mask
 *                   & MandRtrId.
 * Input           : pSrRlfaPathInfo
 * Output          : None
 * Returns         : Pointer to TePath Entry or NULL
 *************************************************************************/
tSrRlfaPathInfo    *
SrGetNextRlfaRtEntry (tSrRlfaPathInfo * pSrRlfaPathInfo)
{
    tSrRlfaPathInfo     SrRlfaPathInfo;
    tSrRlfaPathInfo     dummySrRlfaPathInfo;

    MEMSET (&SrRlfaPathInfo, SR_ZERO, sizeof (tSrRlfaPathInfo));
    MEMSET (&dummySrRlfaPathInfo, SR_ZERO, sizeof (tSrRlfaPathInfo));

    if (gSrGlobalInfo.pSrRlfaPathRbTree == NULL || pSrRlfaPathInfo == NULL)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC,
                "Sr Get Next Entry: pSrRlfaPathInfo == NULL \n");
        return NULL;
    }

    if (MEMCMP (pSrRlfaPathInfo, &dummySrRlfaPathInfo, sizeof (tSrRlfaPathInfo))
        == SR_ZERO)
    {
        return (RBTreeGetFirst (gSrGlobalInfo.pSrRlfaPathRbTree));
    }
    SrRlfaPathInfo.destAddr.Addr.u4Addr = pSrRlfaPathInfo->destAddr.Addr.u4Addr;
    SrRlfaPathInfo.u4DestMask = pSrRlfaPathInfo->u4DestMask;
    SrRlfaPathInfo.mandRtrId.Addr.u4Addr =
        pSrRlfaPathInfo->mandRtrId.Addr.u4Addr;
    SrRlfaPathInfo.mandRtrId.u2AddrType = pSrRlfaPathInfo->mandRtrId.u2AddrType;
    SrRlfaPathInfo.destAddr.u2AddrType = pSrRlfaPathInfo->destAddr.u2AddrType;
    if (pSrRlfaPathInfo->destAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
    {

        MEMCPY (SrRlfaPathInfo.destAddr.Addr.Ip6Addr.u1_addr,
                pSrRlfaPathInfo->destAddr.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        MEMCPY (SrRlfaPathInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                pSrRlfaPathInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        SrRlfaPathInfo.u4DestMask = pSrRlfaPathInfo->u4DestMask;
    }
    return (RBTreeGetNext
            (gSrGlobalInfo.pSrRlfaPathRbTree, &SrRlfaPathInfo, NULL));
}

INT4
SrDeleteRlfaPathTableEntry (tSrRlfaPathInfo * pSrRlfaPathInfo)
{
    if (pSrRlfaPathInfo == NULL)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC,
                "Delete Sr-Te-Path Table : pSrRlfaPathInfo == NULL \n");
        return SR_FAILURE;
    }

    if (RBTreeRem (gSrGlobalInfo.pSrRlfaPathRbTree, pSrRlfaPathInfo) == NULL)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC,
                "Delete Sr-Te-Path Table : RBTree Node Delete Failed \n");
        return SR_FAILURE;
    }

    SR_RLFA_PATH_MEM_FREE (pSrRlfaPathInfo);
    SR_TRC (SR_UTIL_TRC, "Deleted from RbTree \n");
    return SR_SUCCESS;
}

INT4
SrDeleteTeRouteTableEntry (tSrTeRtEntryInfo * pSrTeRtInfo)
{
    if (pSrTeRtInfo == NULL)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC,
                "Delete Sr-Te-Path Table : pSrTeRtInfo == NULL \n");
        return SR_FAILURE;
    }

    if (RBTreeRem (gSrGlobalInfo.pSrTeRouteRbTree, pSrTeRtInfo) == NULL)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC,
                "Delete Sr-Te-Path Table : RBTree Node Delete Failed \n");
        return SR_FAILURE;
    }

    TMO_SLL_FreeNodes ((tTMO_SLL *) & pSrTeRtInfo->srTeRtrListIndex.RtrList,
                       SrMemPoolIds[MAX_SR_TE_RTR_SIZING_ID]);

    /*Release memory for tSrTeRtEntryInfo */
    SrOptRtrListRelIndex (pSrTeRtInfo->srTeRtrListIndex.u4Index);
    SR_TE_RT_ENTRY_MEM_FREE (pSrTeRtInfo);

    SR_TRC (SR_UTIL_TRC, "Deleted from RbTree \n");
    return SR_SUCCESS;
}

INT4
SrDeleteILMFromSrId (tGenU4Addr * pDestAddr, tGenU4Addr * pNextHop)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pNewRtrNode = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrInSegInfo        SrInSegInfo;
    tGenU4Addr          rtrId;
    tGenU4Addr          dummyAddr;
    UINT4               u4SrGbMinIndex = SR_ZERO;
    UINT4               u4L3IfIndex = SR_ZERO;
    UINT1               u1NHFound = SR_FALSE;
    UINT1               u1Flag = SR_FALSE;

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&rtrId, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));

    /* Populate SrInSegInfo from Global SelfNodeInfo */
    if (((pNextHop->u2AddrType == SR_IPV4_ADDR_TYPE)
         && (SrUtilCheckNodeIdConfigured () == SR_SUCCESS))
         || ((pNextHop->u2AddrType == SR_IPV6_ADDR_TYPE)
             && (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)))
    {
        if (pNextHop->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            u4SrGbMinIndex =
                gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
        }
        else
        {
            u4SrGbMinIndex =
                gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;
        }
    }

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;

        /*u4RtrId = OSPF_CRU_BMC_DWFROMPDU (pRtrInfo->rtrId); */
        MEMCPY (&rtrId, &pRtrInfo->prefixId, sizeof (tGenU4Addr));

        if (MEMCMP (pDestAddr, &rtrId, sizeof (tGenU4Addr)) == SR_ZERO)
        {
            pNewRtrNode = pRtrInfo;

            SR_TRC3 (SR_UTIL_TRC, "%s:%d TE_SWAP_STATUS %d \n", __func__, __LINE__,
                     pNewRtrNode->u1TeSwapStatus);
            /* Delete TE ILM */
            if (pNewRtrNode->u1TeSwapStatus == TRUE)
            {
                if (((pNextHop->u2AddrType == SR_IPV4_ADDR_TYPE)
                     && (SrUtilCheckNodeIdConfigured () == SR_SUCCESS))
                     || ((pNextHop->u2AddrType == SR_IPV6_ADDR_TYPE)
                         && (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)))
                {
                    /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
                    SrInSegInfo.u4InLabel =
                        (u4SrGbMinIndex + pNewRtrNode->u4SidValue);

                    SR_TRC2 (SR_UTIL_TRC, "%s:%d Deleting ILM SWAP \n", __func__,
                             __LINE__);

                    if (SrMplsDeleteILM
                        (&SrInSegInfo, pNewRtrNode,
                         pSrRtrNextHopInfo) == SR_FAILURE)
                    {
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsDeleteILM  FAILURE \n",
                                 __func__, __LINE__);
                    }

#ifdef CFA_WANTED
                    if (CfaUtilGetIfIndexFromMplsTnlIf
                        (pNewRtrNode->u4TeSwapOutIfIndex, &u4L3IfIndex,
                         TRUE) != CFA_FAILURE)
                    {
                        if (CfaIfmDeleteStackMplsTunnelInterface
                            (u4L3IfIndex,
                             pNewRtrNode->u4TeSwapOutIfIndex) == CFA_FAILURE)
                        {
                            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                                     "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                                     __func__, __LINE__,
                                     pNewRtrNode->u4TeSwapOutIfIndex,
                                     u4L3IfIndex);
                            return SR_FAILURE;
                        }
                    }
#endif
                    pNewRtrNode->u4TeSwapOutIfIndex = SR_ZERO;
                }
                return SR_SUCCESS;
            }

            /* Delete Non-TE ILMs */
            TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                          pSrRtrNextHopNode, tTMO_SLL_NODE *)
            {
                pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

                if (pNextHop->u2AddrType == SR_IPV4_ADDR_TYPE)
                {
                    if (pNextHop->Addr.u4Addr == SR_ZERO)
                    {
                        u1Flag = SR_TRUE;
                    }
                }
                else if (pNextHop->u2AddrType == SR_IPV6_ADDR_TYPE)
                {
                    if (MEMCMP
                        (pNextHop->Addr.Ip6Addr.u1_addr,
                         dummyAddr.Addr.Ip6Addr.u1_addr,
                         SR_IPV6_ADDR_LENGTH) == SR_ZERO)
                    {
                        u1Flag = SR_TRUE;
                    }
                }
                if (u1Flag == SR_ZERO)
                {
                    if (pSrRtrNextHopInfo->u4OutIfIndex != SR_ZERO)
                    {
                        u1NHFound = SR_TRUE;
                        break;
                    }
                }
                else if (MEMCMP (&pSrRtrNextHopInfo->nextHop, &pNextHop,
                                 sizeof (tGenU4Addr)) == SR_ZERO)
                {
                    u1NHFound = SR_TRUE;
                    break;
                }
            }

            if (u1NHFound == SR_FALSE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d  u1NHFound == SR_FALSE \n", __func__,
                         __LINE__);
                return SR_FAILURE;
            }

            if (((pNextHop->u2AddrType == SR_IPV4_ADDR_TYPE)
                 && (SrUtilCheckNodeIdConfigured () == SR_SUCCESS))
                 || ((pNextHop->u2AddrType == SR_IPV6_ADDR_TYPE)
                     && (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)))
            {
                /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
                SrInSegInfo.u4InLabel =
                    (u4SrGbMinIndex + pNewRtrNode->u4SidValue);

                SR_TRC2 (SR_UTIL_TRC, "%s:%d Deleting ILM SWAP \n", __func__,
                         __LINE__);

                if (SrMplsDeleteILM
                    (&SrInSegInfo, pNewRtrNode,
                     pSrRtrNextHopInfo) == SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsDeleteILM  FAILURE \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
            }
            return SR_SUCCESS;
        }
    }
    return SR_FAILURE;
}

INT4
SrDeletePopNFwdUsingRtInfo (tGenU4Addr * pDestAddr, tGenU4Addr * pNextHop)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pNewRtrNode = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    UINT1               u1NhFound = SR_FALSE;

    tGenU4Addr          rtrId;
    MEMSET (&rtrId, SR_ZERO, sizeof (tGenU4Addr));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Entry \n", __func__, __LINE__);

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;

        MEMCPY (&rtrId, &pRtrInfo->prefixId, sizeof (tGenU4Addr));

        if (MEMCMP (pDestAddr, &rtrId, sizeof (tGenU4Addr)) == SR_ZERO)
        {
            pNewRtrNode = pRtrInfo;

            TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                          pSrRtrNextHopNode, tTMO_SLL_NODE *)
            {
                pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

                if ((MEMCMP
                     (&pSrRtrNextHopInfo->nextHop, pNextHop,
                      sizeof (tGenU4Addr)) == SR_ZERO))
                {
                    if (pSrRtrNextHopInfo->u1FRRNextHop == SR_PRIMARY_NEXTHOP)
                    {
                        u1NhFound = SR_TRUE;
                    }
                    /*break; */
                }
                else
                {
                    return SR_FAILURE;
                }
            }

            /* If Pop&Fwd Created with same NH, then delete the Pop & Fwd & return
             *  * "return SR_Success": is done because POP&FWD and FTN cannot be present simultaneously*/
            if ((u1NhFound == SR_TRUE) &&
                ((pSrRtrNextHopInfo->u1MPLSStatus & SR_ILM_POP_CREATED) ==
                 SR_ILM_POP_CREATED))
            {
                if (SrMplsDeleteILMPopAndFwd (pNewRtrNode, pSrRtrNextHopInfo) ==
                    SR_SUCCESS)
                {
                    SR_TRC2 (SR_UTIL_TRC,
                             "%s:%d POP&FWD Deleted; Removing from SrRtrNhList \n",
                             __func__, __LINE__);

                    TMO_SLL_Delete (&(pNewRtrNode->NextHopList),
                                    (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                    SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);

                    return SR_SUCCESS;
                }
                else
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d POP&FWD Delete Failure \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
            }
        }
    }

    return SR_FAILURE;
}

tSrRlfaPathInfo    *
SrGetRlfaPathEntryFromDestAddr (tGenU4Addr * pDestAddr)
{
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrRlfaPathInfo     SrRlfaPathInfo;

    MEMSET (&SrRlfaPathInfo, SR_ZERO, sizeof (tSrRlfaPathInfo));
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Entry with \n", __func__, __LINE__);
    while ((pSrRlfaPathInfo = SrGetNextRlfaRtEntry (&SrRlfaPathInfo)) != NULL)
    {
        if (pSrRlfaPathInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            if (pSrRlfaPathInfo->destAddr.Addr.u4Addr == pDestAddr->Addr.u4Addr)
            {
                SR_TRC2 (SR_UTIL_TRC, "%s:%d Exit: SUCCESS \n", __func__,
                         __LINE__);
                return pSrRlfaPathInfo;
            }
            else
            {
                SrRlfaPathInfo.destAddr.Addr.u4Addr =
                    pSrRlfaPathInfo->destAddr.Addr.u4Addr;
                SrRlfaPathInfo.u4DestMask = pSrRlfaPathInfo->u4DestMask;
                SrRlfaPathInfo.mandRtrId.Addr.u4Addr =
                    pSrRlfaPathInfo->mandRtrId.Addr.u4Addr;
            }
        }
        else if (pSrRlfaPathInfo->destAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            if (MEMCMP (pSrRlfaPathInfo->destAddr.Addr.Ip6Addr.u1_addr,
                        pDestAddr->Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                SR_TRC2 (SR_UTIL_TRC, "%s:%d Exit: SUCCESS \n", __func__,
                         __LINE__);
                return pSrRlfaPathInfo;
            }
            else
            {
                MEMCPY (SrRlfaPathInfo.destAddr.Addr.Ip6Addr.u1_addr,
                        pSrRlfaPathInfo->destAddr.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                MEMCPY (SrRlfaPathInfo.mandRtrId.Addr.Ip6Addr.u1_addr,
                        pSrRlfaPathInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                SrRlfaPathInfo.u4DestMask = pSrRlfaPathInfo->u4DestMask;
            }
        }
        SrRlfaPathInfo.destAddr.u2AddrType =
            pSrRlfaPathInfo->destAddr.u2AddrType;
        SrRlfaPathInfo.mandRtrId.u2AddrType =
            pSrRlfaPathInfo->mandRtrId.u2AddrType;
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d Exit: FAILURE \n", __func__, __LINE__);
    return NULL;
}

UINT4
SrTeGetLabelForRtrId (tGenU4Addr * pRouterId, tGenU4Addr * pPrevRouterId,
                      UINT4 *u4RouterLabel)
{
    tSrRtrInfo         *pSrRtrInfo = NULL;
    tSrRtrInfo         *pNewSrRtrInfo = NULL;
    tSrRtrInfo         *pTempSrRtrInfo = NULL;
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tGenU4Addr          rtrId;
    tGenU4Addr          nextHop;
    UINT4               u4OutIfIndex = SR_ZERO;
    tGenU4Addr          adjLinkAddr;
    tTMO_SLL_NODE      *pAdjSid = NULL;
    tAdjSidNode        *pAdjSidInfo = NULL;
    tGenU4Addr          dummyAddr;

    MEMSET (&nextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&rtrId, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&adjLinkAddr, SR_ZERO, sizeof (tGenU4Addr));
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Entry \n", __func__, __LINE__);

    /* Label = SID + SRGB_MIN */
    /* If u4PrevRouterId == ZERO, then find only Top-Label to be used */

    /* 1. Get SrRtrInfo for RouterId to find SID */
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pSrRtrInfo = (tSrRtrInfo *) pRtrNode;

        MEMCPY (&rtrId, &pSrRtrInfo->prefixId, sizeof (tGenU4Addr));

        if (MEMCMP (pRouterId, &rtrId, sizeof (tGenU4Addr)) == SR_ZERO)
        {
            pNewSrRtrInfo = pSrRtrInfo;
            break;
        }
        else
        {
            TMO_SLL_Scan (&(pSrRtrInfo->adjSidList), pAdjSid, tTMO_SLL_NODE *)
            {
                pAdjSidInfo = (tAdjSidNode *) pAdjSid;
                MEMCPY (&adjLinkAddr, &pAdjSidInfo->linkIpAddr,
                        sizeof (tGenU4Addr));
                if (MEMCMP (&pRouterId, &adjLinkAddr, sizeof (tGenU4Addr)) ==
                    SR_ZERO)
                {
                    *u4RouterLabel = pAdjSidInfo->u4Label;
                    SR_TRC3 (SR_UTIL_TRC, "%s:%d Exit with Label %d \n",
                             __func__, __LINE__, *u4RouterLabel);
                    return SR_SUCCESS;
                }
            }
        }
    }

    if ((pNewSrRtrInfo == NULL) || (pNewSrRtrInfo->u4SidValue == SR_ZERO))
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pNewSrRtrInfo == NULL or"
                "pNewSrRtrInfo->u4SidValue == SR_ZERO \n",
                __func__,__LINE__);
        return SR_FAILURE;
    }

    if (MEMCMP (pPrevRouterId, &dummyAddr, sizeof (tGenU4Addr)) == SR_ZERO)
    {
        /* 2. Get Next-Hop to be used for reaching this RouterId */
        if (SrGetNHForPrefix (&rtrId, &nextHop, &u4OutIfIndex) == SR_FAILURE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrGetNHForPrefix FAILURE \n", __func__,
                     __LINE__);
            return SR_FAILURE;
        }

        /* 3. Get LabelRange to be used with respect to Next-Hop */
        pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&nextHop);
        if (pTempSrRtrInfo == NULL)
        {

            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pTempSrRtrInfo == NULL \n", __func__,
                     __LINE__);
            return SR_FAILURE;
        }

        if (((nextHop.u2AddrType == SR_IPV4_ADDR_TYPE) &&
             (pTempSrRtrInfo->srgbRange.u4SrGbMinIndex <= SR_ZERO)) ||
            ((nextHop.u2AddrType == SR_IPV6_ADDR_TYPE) &&
             (pTempSrRtrInfo->srgbRange.u4SrGbV3MinIndex <= SR_ZERO)))
        {
            /* Label should not be computed if SRGB is zero */
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d SRGB is zero. TE label cannot be computed.\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

        if (nextHop.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            *u4RouterLabel =
                pNewSrRtrInfo->u4SidValue +
                pTempSrRtrInfo->srgbRange.u4SrGbMinIndex;
        }
        else if (nextHop.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            *u4RouterLabel =
                pNewSrRtrInfo->u4SidValue +
                pTempSrRtrInfo->srgbRange.u4SrGbV3MinIndex;
        }
    }
    else
    {
        /* Get SrRtrInfo for PrevRouterId    to find SRGB_MIN for NEXT_RTR_ID */
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            pTempSrRtrInfo = (tSrRtrInfo *) pRtrNode;

            MEMCPY (&rtrId, &pTempSrRtrInfo->prefixId, sizeof (tGenU4Addr));

            if (MEMCMP (pPrevRouterId, &rtrId, sizeof (tGenU4Addr)) == SR_ZERO)
            {
                break;
            }
        }

        if (pTempSrRtrInfo == NULL)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pTempSrRtrInfo == NULL \n", __func__,
                     __LINE__);
            return SR_FAILURE;
        }

        if (((pTempSrRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE) &&
             (pTempSrRtrInfo->srgbRange.u4SrGbMinIndex <= SR_ZERO)) ||
            ((pTempSrRtrInfo->prefixId.u2AddrType == SR_IPV6_ADDR_TYPE) &&
             (pTempSrRtrInfo->srgbRange.u4SrGbV3MinIndex <= SR_ZERO)))
        {
            /* Label should not be computed if SRGB is zero */
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SRGB is zero \n", __func__, __LINE__);
            return SR_FAILURE;
        }

        if (rtrId.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            *u4RouterLabel =
                pNewSrRtrInfo->u4SidValue +
                pTempSrRtrInfo->srgbRange.u4SrGbMinIndex;
        }
        else if (rtrId.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            *u4RouterLabel =
                pNewSrRtrInfo->u4SidValue +
                pTempSrRtrInfo->srgbRange.u4SrGbV3MinIndex;
        }
    }

    SR_TRC3 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Exit with Label %d \n", __func__, __LINE__,
             *u4RouterLabel);
    return SR_SUCCESS;
}

tSrRtrInfo         *
SrGetSrRtrInfoFromRtrId (tGenU4Addr * pRouterId)
{
    tSrRtrInfo         *pSrRtrInfo = NULL;
    tTMO_SLL_NODE      *pRtrNode = NULL;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Entry \n", __func__, __LINE__);

    /* Get SrRtrInfo from RouterId */
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pSrRtrInfo = (tSrRtrInfo *) pRtrNode;

        if (pRouterId->Addr.u4Addr == pSrRtrInfo->advRtrId.Addr.u4Addr)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Router Node found with Rotuer Id %x \n", __func__,
                     __LINE__, pRouterId->Addr.u4Addr);
            return pSrRtrInfo;
        }
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Exit Failure \n", __func__, __LINE__);
    return NULL;
}

tSrRtrInfo         *
SrGetSrRtrInfoFromNodeId (tGenU4Addr * pDestAddr)
{
    tSrRtrInfo         *pSrRtrInfo = NULL;
    tTMO_SLL_NODE      *pRtrNode = NULL;

    SR_TRC3 (SR_MAIN_TRC, "\n%s:%d Entry %x \n", __func__, __LINE__,
            pDestAddr->Addr.u4Addr);

    /* Get SrRtrInfo from RouterId */
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pSrRtrInfo = (tSrRtrInfo *) pRtrNode;

        SR_TRC3 (SR_MAIN_TRC, "\n%s:%d SR node dest %x \n", __func__,
                     __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);


        if (pDestAddr->Addr.u4Addr == pSrRtrInfo->prefixId.Addr.u4Addr)
        {
            SR_TRC3 (SR_MAIN_TRC, "\n%s:%d SR node found for dest %x \n", __func__,
                     __LINE__, pDestAddr->Addr.u4Addr);
            return pSrRtrInfo;
        }
    }

    SR_TRC2 (SR_MAIN_TRC, "\n%s:%d Exit Failure\n", __func__, __LINE__);
    return NULL;
}


tSrRtrInfo *
SrGetSrRtrInfoFromRtrAndAdvRtrId (tGenU4Addr * pDestAddr, UINT4 u4AdvRtrId)
{
    tSrRtrInfo         *pSrRtrInfo = NULL;
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tGenU4Addr          rtrId;

    MEMSET (&rtrId, SR_ZERO, sizeof (tGenU4Addr));

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Entry for Dest %x, AdvRtrId %x\n",
		    __func__, __LINE__, pDestAddr->Addr.u4Addr, u4AdvRtrId);

    /* Get SrRtrInfo from RouterId */
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pSrRtrInfo = (tSrRtrInfo *) pRtrNode;

        MEMCPY (&rtrId, &pSrRtrInfo->prefixId, sizeof (tGenU4Addr));

        if (MEMCMP (pDestAddr, &rtrId, sizeof (tGenU4Addr)) == SR_ZERO)
        {
            if (pSrRtrInfo->advRtrId.Addr.u4Addr == u4AdvRtrId)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Exit SUCCESS \n", __func__,
                          __LINE__);
                return pSrRtrInfo;
            }
        }
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Exit Failure \n", __func__, __LINE__);
    return NULL;
}
/**************************************************************************
 * Function Name   : SrTeMplsCreateOrDeleteLsp
 * Description     : Function is to create or delete SR TE and RLFA FTN
                     for a destination prefix
 * Input           :  pSrRtrInfo,pSrRtrNextHopInfo
                     if u1cmdType is SR_TRUE, then SR TE creation
                     else, SR TE deletion flow
 * Output          : pu4ErrCode (for future use)
 * Returns         : SR_SUCCESS (on succesfully deletion)
 *                   SR_FAILURE (No TEPATH Entry Found or any other Failure
 *                   scenario)
 *************************************************************************/
INT4
SrMplsCreateOrDeleteStackLsp (UINT2 u2MlibOperation, tSrRtrInfo * pSrRtrInfo,
                            tSrInSegInfo * pSrInSegInfo,
                            tSrTeLblStack * pSrTeLblStack,
                            tSrRtrNextHopInfo * pSrRtrNextHopInfo,
                            BOOL1 bFRRHwStatus)
{
    tSrRtrNextHopInfo  *pSrRtrPriNextHopInfo = NULL;
    tNHLFE              Nhlfe;
    tLspInfo            LspInfo;
    UINT1               u1StackLimit = SR_ZERO;
    UINT1               u1StackCount = SR_ZERO;
    UINT4               u4NextHop    = SR_ZERO;
    tGenU4Addr          dummyAddr;
    tGenU4Addr          dummyZeroAddr;
    tSrTeRtEntryInfo   *pSrTeDestRtInfo = NULL;
    tSrRtrNextHopInfo  *pPrimaryNextHopInfo = NULL;

    MEMSET (&dummyAddr, 0xff, sizeof (tGenU4Addr));
    MEMSET (&dummyZeroAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&Nhlfe, SR_ZERO, sizeof (tNHLFE));
    MEMSET (&LspInfo, SR_ZERO, sizeof (tLspInfo));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    switch(u2MlibOperation)
    {
        case MPLS_MLIB_FTN_CREATE:
            if ((pSrRtrInfo == NULL) || (pSrTeLblStack == NULL))
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                        "%s:%d pSrRtrInfo || pSrTeLblStack is NULL \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            break;
        case MPLS_MLIB_FTN_DELETE:
            /* For FTN Delete, only FEC & NH params are required to identify
            FTN to be deleted */
            if (pSrRtrInfo == NULL)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pSrRtrInfo is NULL \n", __func__,
                         __LINE__);
                return SR_FAILURE;
            }
            break;
        case MPLS_MLIB_ILM_CREATE:
            if ((pSrRtrInfo == NULL) || (pSrTeLblStack == NULL)
                || (pSrInSegInfo == NULL))
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                        "%s:%d pSrRtrInfo || pSrTeLblStack is NULL \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            break;
        case MPLS_MLIB_ILM_DELETE:
            if ((pSrRtrInfo == NULL) || (pSrTeLblStack == NULL)
                || (pSrInSegInfo == NULL))
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                        "%s:%d pSrRtrInfo || pSrTeLblStack is NULL \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            break;

    }

    /* 1. Fill FTN/ILM Info */
    SR_TRC2 (SR_UTIL_TRC, "%s:%d Filling FTN/ILM Info \n", __func__, __LINE__);

    LspInfo.FecParams.u1FecType = SR_FEC_PREFIX_TYPE;

    LspInfo.FecParams.u2AddrType = pSrRtrInfo->u2AddrType;
    if (LspInfo.FecParams.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        MEMCPY (&(LspInfo.FecParams.DestAddrPrefix),
                &pSrRtrInfo->prefixId.Addr, sizeof (uGenU4Addr));
        LspInfo.FecParams.DestMask.u4Addr = SR_IPV4_DEST_MASK;
    }
    else if (LspInfo.FecParams.u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY (LspInfo.FecParams.DestAddrPrefix.Ip6Addr.u1_addr,
                pSrRtrInfo->prefixId.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);

        MEMCPY (LspInfo.FecParams.DestMask.Ip6Addr.u1_addr,
                dummyAddr.Addr.Ip6Addr.u1_addr, SR_IPV6_ADDR_LENGTH);
    }
    LspInfo.Direction = MPLS_DIRECTION_FORWARD;

    SR_TRC3 (SR_UTIL_TRC, "%s:%d Filled FTN Info: FEC %x \n",
             __func__, __LINE__, LspInfo.FecParams.DestAddrPrefix.u4Addr);

    /* 2. Fill NHLFE Info along with Top-Label */
    SR_TRC2 (SR_UTIL_TRC, "%s:%d Filling NHLFE Info \n", __func__, __LINE__);

    Nhlfe.u1Operation = (UINT1) u2MlibOperation;
    Nhlfe.u1NHAddrType = (UINT1) pSrRtrInfo->u2AddrType;

    if (pSrRtrNextHopInfo == NULL)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC, "pSrRtrNextHopInfo is null \r\n");
        return SR_FAILURE;
    }
    if (MEMCMP
        (&pSrRtrNextHopInfo->nextHop.Addr, &dummyZeroAddr.Addr,
         sizeof (uGenU4Addr)) == SR_ZERO)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC, "Next hop is empty\r\n");
        return SR_FAILURE;
    }
    MEMCPY (&(Nhlfe.NextHopAddr), &pSrRtrNextHopInfo->nextHop.Addr,
            sizeof (uGenU4Addr));

    if (u2MlibOperation == MPLS_MLIB_FTN_CREATE)
    {
        Nhlfe.u4OutLabel = pSrTeLblStack->u4TopLabel;
        Nhlfe.u4OutIfIndex = (UINT4) pSrRtrNextHopInfo->u4OutIfIndex;
    }

    /* TODO: Currenlty we are using two different ways to fill ILM Label Stack
       for RLFA and TILFA */
    if (u2MlibOperation == MPLS_MLIB_ILM_CREATE)
    {
        if (SR_RLFA_NEXTHOP == pSrRtrNextHopInfo->u1FRRNextHop)
        {
            Nhlfe.u4OutLabel             = pSrTeLblStack->u4LabelStack[SR_ZERO]; /* Dest Label */
            LspInfo.u4RemoteNodeLabel    = pSrTeLblStack->u4TopLabel; /* RLFA PQ label */
            LspInfo.u4InTopLabel         = pSrInSegInfo->u4InLabel;
            Nhlfe.u4OutIfIndex           = (UINT4) pSrRtrNextHopInfo->u4SwapOutIfIndex;
        }
        else
        {
            LspInfo.u4InTopLabel         = pSrInSegInfo->u4InLabel;
            Nhlfe.u4OutLabel             = pSrTeLblStack->u4TopLabel;
            Nhlfe.u4OutIfIndex           = (UINT4) pSrRtrNextHopInfo->u4SwapOutIfIndex;
        }
    }

    if ((u2MlibOperation == MPLS_MLIB_FTN_CREATE)
        || ((u2MlibOperation == MPLS_MLIB_ILM_CREATE)
            && (SR_TILFA_NEXTHOP == pSrRtrNextHopInfo->u1FRRNextHop)))
    {
        /* 3. Fill Stack Labels other than Top-Label */
        u1StackLimit = pSrTeLblStack->u1StackSize;
        for (u1StackCount = SR_ZERO; u1StackCount < u1StackLimit;
             u1StackCount++)
        {
            LspInfo.SrTeLblStack.u4LabelStack[u1StackCount] =
                pSrTeLblStack->u4LabelStack[u1StackCount];
        }
        LspInfo.SrTeLblStack.u1StackSize = u1StackLimit;
    }

    if (u2MlibOperation == MPLS_MLIB_FTN_DELETE)
    {
        Nhlfe.u4OutIfIndex = (UINT4) pSrRtrNextHopInfo->u4OutIfIndex;
    }

    if (u2MlibOperation == MPLS_MLIB_ILM_DELETE)
    {
        LspInfo.u4InTopLabel = pSrInSegInfo->u4InLabel;
        Nhlfe.u4OutIfIndex   = (UINT4) pSrRtrNextHopInfo->u4SwapOutIfIndex;
    }

    LspInfo.pNhlfe = (tNHLFE *) (&Nhlfe);
    Nhlfe.u1OperStatus = MPLS_STATUS_UP;

    /* To prevent SR TE LFA programming in H/W */
    LspInfo.bIsFRRHwLsp = bFRRHwStatus;

    MEMCPY (&LspInfo.PrimarynextHop, &pSrRtrNextHopInfo->PrimarynextHop,
            sizeof (tGenU4Addr));
    /*Maintaing the RLFA/TILFA status */
    if (pSrRtrNextHopInfo->bIsLfaNextHop == TRUE)
    {
        u4NextHop = pSrRtrNextHopInfo->PrimarynextHop.Addr.u4Addr;
        pSrRtrPriNextHopInfo = SrV4GetSrNextHopInfoFromRtr (pSrRtrInfo,
                                                            u4NextHop);
        if (pSrRtrPriNextHopInfo == NULL)
        {
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                     "%s:%d Fetching Nexthop Info failed for prefix %x"
                     " with Nexthop %x has failed \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr,
                     u4NextHop);
            return SR_FAILURE;
        }

        if ((u2MlibOperation == MPLS_MLIB_FTN_CREATE)
            || (u2MlibOperation == MPLS_MLIB_FTN_DELETE))
        {
            LspInfo.u4PrimaryTunIndex = pSrRtrPriNextHopInfo->u4OutIfIndex;
        }

        if ((u2MlibOperation == MPLS_MLIB_ILM_CREATE)
            || (u2MlibOperation == MPLS_MLIB_ILM_DELETE))
        {
            LspInfo.u4PrimaryTunIndex = pSrRtrPriNextHopInfo->u4SwapOutIfIndex;
        }

        if (SR_RLFA_NEXTHOP == pSrRtrNextHopInfo->u1FRRNextHop)
        {
            LspInfo.u1FRRNextHop = SR_RLFA_NEXTHOP;
        }
        else if (SR_TILFA_NEXTHOP == pSrRtrNextHopInfo->u1FRRNextHop)
        {
            LspInfo.u1FRRNextHop = SR_TILFA_NEXTHOP;
        }
        else
        {
            SR_TRC5 (SR_CRITICAL_TRC,
                    "%s:%d Invalid LFA type %d added for LFA nexthop %x "
                    " to destination %x", __func__, __LINE__,
                    pSrRtrPriNextHopInfo->u1FRRNextHop, u4NextHop,
                    pSrRtrInfo->prefixId.Addr.u4Addr);
            return SR_FAILURE;
        }

        LspInfo.u1Owner      = MPLS_OWNER_SR;
        LspInfo.u1Preference = MPLS_FTN_SR_PREFERENCE;
        /* In case of RLFA delete after convergence, passing the
           ILM tunnel index so that the corresponding Tunnel initiator
           created during RLFA set event will be deleted */
        if ((MPLS_MLIB_FTN_DELETE == u2MlibOperation) &&
            (pSrRtrInfo->bIsLfaActive == SR_TRUE))
        {
            LspInfo.u4LfaIlmTunIndex = pSrRtrNextHopInfo->u4SwapOutIfIndex;    /* RLFA ILM Tunnel Index */
            LspInfo.bIsOnlyCPUpdate = pSrRtrNextHopInfo->bIsOnlyCPUpdate;
        }
        LspInfo.u1ProtActionType = SrMplsSetAction(&u2MlibOperation, LspInfo.u1FRRNextHop);
    }
    else if (pSrRtrInfo->bIsTeconfigured == SR_TRUE)
    {
        pSrTeDestRtInfo = SrGetTeRouteEntryFromDestAddr (&(pSrRtrInfo->prefixId));
        if (pSrTeDestRtInfo == NULL)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d No TePathEntry Found \n", __func__,
                     __LINE__);
            return SR_FAILURE;
        }

        LspInfo.bIsSrTeLsp = SR_TE_LSP;
        LspInfo.u1Owner = MPLS_OWNER_SR_TE;
        LspInfo.bIsTeFtn = SR_TRUE;    /* To do only FTN processing alone in NP for TE */

        MEMCPY (&LspInfo.PrimarynextHop, &pSrTeDestRtInfo->PrimaryNexthop,
                sizeof (tGenU4Addr));
        if (bFRRHwStatus == SR_FALSE)
        {
            LspInfo.u1FRRNextHop = SR_TE_NEXTHOP;
            MEMCPY (&(LspInfo.LfanextHop), &(pSrTeDestRtInfo->LfaNexthop),
                    sizeof (tGenU4Addr));
            LspInfo.u4LfaTunIndex = pSrTeDestRtInfo->u4OutLfaIfIndex;
        }
        else
        {
            LspInfo.u1FRRNextHop = SR_TE_LFA_NEXTHOP;
            LspInfo.bIsFRRHwLsp = SR_TRUE;
            pPrimaryNextHopInfo =
                SrV4GetSrNextHopInfoFromRtr (pSrRtrInfo,
                                             pSrTeDestRtInfo->PrimaryNexthop.
                                             Addr.u4Addr);
            if (pPrimaryNextHopInfo == NULL)
            {
                SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d Fetching Nexthop Info failed for prefix %x"
                         " with Nexthop %x has failed \n", __func__, __LINE__,
                         pSrRtrInfo->prefixId.Addr.u4Addr,
                         pSrTeDestRtInfo->PrimaryNexthop.Addr.u4Addr);
                return SR_FAILURE;
            }
            LspInfo.u4PrimaryTunIndex = pPrimaryNextHopInfo->u4OutIfIndex;
        }
    }
    LspInfo.bIsOnlyCPUpdate = pSrRtrNextHopInfo->bIsOnlyCPUpdate;
    LspInfo.bIsFRRHwModifyLsp = pSrRtrNextHopInfo->bIsFRRModifyLsp;

    SR_TRC5 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Lsp Info: Dest %x Lfa NH Type %d MibOp %d \n",
            __func__, __LINE__, LspInfo.FecParams.DestAddrPrefix.u4Addr,
            LspInfo.u1FRRNextHop, u2MlibOperation);
    SR_TRC5 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Lsp Info: Nhlfe.u4OutLabel %d Lsp.RemoteLabel %d Lsp.InTopLabel %d \n",
                __func__, __LINE__, Nhlfe.u4OutLabel,
            LspInfo.u4RemoteNodeLabel, LspInfo.u4InTopLabel);
    LspInfo.u1Protocol = MPLS_PROTOCOL_SR_OSPF;
    /* 4. Create/Delete FTN with respect to LspInfo */
    if (MplsMlibUpdate (u2MlibOperation, &LspInfo) == MPLS_SUCCESS)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d MPLS_SUCCESS \n", __func__, __LINE__);
    }
    else
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d MPLS_FAILURE \n", __func__, __LINE__);
        return SR_FAILURE;
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

PUBLIC tSrDelayRtInfo *
SrCreateDelayRouteNode (VOID)
{
    tSrDelayRtInfo  *pRouteInfo = NULL;
    if (SR_RTR_DELAY_RT_INFO_ALLOC (pRouteInfo) == NULL)
    {
        SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                "%s:%d Mem Alloc Failure for RouteInfo : \n", __func__,
                __LINE__);
        return NULL;
    }
    MEMSET (pRouteInfo, SR_ZERO, sizeof (tSrDelayRtInfo));
    return pRouteInfo;
}
PUBLIC tSrRtrNextHopInfo *
SrCreateNewNextHopNode (VOID)
{
    tSrRtrNextHopInfo  *tempNHNode = NULL;

    if (SR_RTR_NH_LIST_ALLOC (tempNHNode) == NULL)
    {
        SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                "%s:%d Mem Alloc Failure for Router Nexthop Info : \n", __func__,
                __LINE__);
        return NULL;
    }

    MEMSET (tempNHNode, SR_ZERO, sizeof (tSrRtrNextHopInfo));

    return tempNHNode;
}

tSrRtrEntry        *
SrCreateOptRtrEntry (UINT4 u4RtrListIndex, UINT4 u4OptRtrIndex)
{
    tSrTeRtEntryInfo   *pSrTmpTePathInfo = NULL;
    tSrTeRtEntryInfo   *pSrTeRtInfo = NULL;
    tSrRtrEntry        *pSrOptRtrInfo = NULL;

    pSrTmpTePathInfo = RBTreeGetFirst (gSrGlobalInfo.pSrTeRouteRbTree);

    while (1)
    {
        if (pSrTmpTePathInfo == NULL)
        {
            SR_TRC2 (SR_UTIL_TRC, "%s:%d TE-Path Table empty : \n", __func__,
                     __LINE__);
            return NULL;
        }

        if (u4RtrListIndex == pSrTmpTePathInfo->srTeRtrListIndex.u4Index)
        {
            pSrTeRtInfo = pSrTmpTePathInfo;
            break;
        }
        pSrTmpTePathInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrTeRouteRbTree, pSrTmpTePathInfo,
                           NULL);
    }
    if (pSrTeRtInfo == NULL)
    {
        SR_TRC3 (SR_UTIL_TRC,
                 "%s:%d No entry found with index %d in TE-Path Table \n",
                 __func__, __LINE__, u4RtrListIndex);
        return NULL;
    }

    if (SR_TE_RTR_MEM_ALLOC (pSrOptRtrInfo) == NULL)
    {
        SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                 "%s:%d Mem Alloc Failure for Optional Router : \n", __func__,
                 __LINE__);
        return NULL;
    }
    MEMSET (pSrOptRtrInfo, SR_ZERO, sizeof (tSrRtrEntry));
    pSrOptRtrInfo->u4RtrIndex = u4OptRtrIndex;
    TMO_SLL_Add (&(pSrTeRtInfo->srTeRtrListIndex.RtrList),
                 (tTMO_SLL_NODE *) pSrOptRtrInfo);

    return pSrOptRtrInfo;
}
VOID SrDeleteRtrFromDelayList(tSrRtrInfo  *pRtrInfo)
{
    tSrRouteEntryInfo  *pRouteInfo = NULL;
    tSrDelayRtInfo     *pRtInfo     = NULL;
    tSrDelayRtInfo     *pTempRtInfo = NULL;
    UINT4               u4DestAddr;
    UINT4               u4PrefixId;
    UINT1               bNodeFound = SR_FALSE;
    TMO_DYN_SLL_Scan (&(gSrGlobalInfo.rtDelayList), pRtInfo, pTempRtInfo,
                      tSrDelayRtInfo *)
    {
        pRouteInfo = (tSrRouteEntryInfo *) (VOID *) &(pRtInfo->srRouteInfo);
        u4DestAddr = pRouteInfo->destAddr.Addr.u4Addr;
        MEMCPY (&u4PrefixId, &pRtrInfo->prefixId, sizeof (UINT4));
        if (u4DestAddr == u4PrefixId)
        {
            bNodeFound  = SR_TRUE;
            break;
        }
        if (SR_FALSE == bNodeFound)
        {
            return;
        }
        TMO_SLL_Delete (&(gSrGlobalInfo.rtDelayList), (tTMO_SLL_NODE *) pRtInfo);
        /* Reset the delay list flag */
        pRtrInfo->u1OprFlags &= ~SR_OPR_F_ADD_DELAY_LIST;
        SR_TRC4 (SR_UTIL_TRC | SR_CTRL_TRC,
            "%s:%d Deleted Prefix %x Rt Id %x from Delay List" ,
            __func__, __LINE__, u4DestAddr, pRtrInfo->advRtrId.Addr.u4Addr);
        SR_RTR_DELAY_RT_INFO_MEM_FREE (pRtInfo);
    }

    if (SR_ZERO == TMO_SLL_Count (&(gSrGlobalInfo.rtDelayList)))
    {
        if (gSrGlobalInfo.bIsDelayTimerStarted == SR_TRUE)
        {
            if (TmrStopTimer (SR_TMR_LIST_ID, &(gSrGlobalInfo.delayTimerNode)) !=
                               TMR_SUCCESS)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                        "%s:%d Failed to stop SR delay Timer", __func__, __LINE__);
            }

            gSrGlobalInfo.bIsDelayTimerStarted = SR_FALSE;
            SR_TRC2 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                    "%s:%d SR delay Timer stopped due to 0 nodes",
                    __func__, __LINE__);
        }
    }

    return;
}

VOID
SrOspfUpEventHandler (void)
{
    tSrQMsg             SrQMsg;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));

    SrQMsg.u4MsgType = SR_OSPF_UP_EVENT;

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                 "%s:%d Failed to EnQ SR_OSPF_UP_EVENT Event to SR Task \n",
                 __func__, __LINE__);
    }
    return;
}

/*****************************************************************************/
/* Function Name : SrProcessRtrDisableInCxt                                  */
/* Description   : This routine is called by OSPF module, for informing that */
/*                 ospf rtr context is disabled.                             */
/*                                                                           */
/* Input(s)      : u4OspfCxt - Ospf Context                                  */
/* Output(s)     : None                                                      */
/* Return(s)     : None                                                      */
/*****************************************************************************/
VOID
SrProcessRtrDisableInCxt (UINT4 u4OspfCxt)
{
    tSrQMsg             SrQMsg;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));

    SrQMsg.u4MsgType = SR_OSPF_RTR_DISABLE_EVENT;

    SrQMsg.u4OspfCxtId = u4OspfCxt;

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                 "%s:%d Failed to EnQ SR_OSPF_STATE_EVENT Event to SR Task \n",
                 __func__, __LINE__);
    }
    return;
}

/*****************************************************************************/
/* Function Name : SrProcessStateChgFromOspf                                */
/* Description   : This routine is called by SR module, for informing that   */
/*                 SR is enabled on OSPF                                     */
/* Input(s)      : u4OspfCxt - Ospf Context                                  */
/*                 u4SrStatus - SR Status                                    */
/* Output(s)     : None                                                      */
/* Return(s)     : None                                                      */
/*****************************************************************************/
VOID
SrProcessStateChgFromOspf (UINT4 u4OspfCxt, UINT4 u4SrStatus)
{
    tSrQMsg             SrQMsg;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));

    SrQMsg.u4MsgType = SR_OSPF_STATE_EVENT;

    SrQMsg.u4OspfCxtId = u4OspfCxt;
    SrQMsg.u4Status = u4SrStatus;

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                 "%s:%d Failed to EnQ SR_OSPF_STATE_EVENT Event to SR Task \n",
                 __func__, __LINE__);
    }
    return;
}

/*****************************************************************************/
/* Function Name : SrProcessStateChgFromOspfV3                               */
/* Description   : This routine is called by SR module, for informing that   */
/*                 SR is enabled on OSPFV3                                   */
/* Input(s)      : u4SrStatus - SR Status                                    */
/* Output(s)     : None                                                      */
/* Return(s)     : None                                                      */
/*****************************************************************************/
VOID
SrProcessStateChgFromOspfV3 (UINT4 u4OspfV3Cxt, UINT4 u4SrStatus)
{
    tSrQMsg             SrQMsg;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));

    SrQMsg.u4MsgType = SR_OSPFV3_STATE_EVENT;

    SrQMsg.u4OspfCxtId = u4OspfV3Cxt;
    SrQMsg.u4Status = u4SrStatus;

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                 "%s:%d Failed to EnQ SR_OSPF_STATE_EVENT Event to SR Task \n",
                 __func__, __LINE__);
    }
    return;
}

/*****************************************************************************/
/* Function Name : SrProcessAltStateChgFromOspfV3                            */
/* Description   : This routine is called by SR module, for informing that   */
/*                 SR alternate (LFA/RLFA) is enabled on OSPFv3              */
/* Input(s)      : u4SrStatus - SR Status                                    */
/* Output(s)     : None                                                      */
/* Return(s)     : None                                                      */
/*****************************************************************************/
VOID
SrProcessAltStateChgFromOspfV3 (UINT4 u4AltStatus)
{
    tSrQMsg             SrQMsg;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));

    SrQMsg.u4MsgType = SR_OSPFV3_ALT_STATE_EVENT;

    SrQMsg.u4Status = u4AltStatus;

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                 "%s:%d Failed to EnQ SR_OSPF_ALT_STATE_EVENT Event to SR Task \n",
                 __func__, __LINE__);
    }
    return;
}

/*****************************************************************************/
/* Function Name : SrProcessLfaIpRtChangeEvent                               */
/* Description   : This routine is called by SR module, for informing the
 *                 LFA Route changes.                                        */
/* Input(s)      : tNetIpv4RtInfo - Route info  as given by RTM              */
/*              u1CmdType - Command to ADD | Delete | Modify the Route.      */
/* Output(s)     : None                                                      */
/* Return(s)     : None                                                      */
/*****************************************************************************/

VOID
SrProcessLfaIpRtChangeEvent (tNetIpv4LfaRtInfo * pLfaRtInfo)
{
    UINT2               u2MlibOperation = SR_ZERO;
    SR_TRC2 (SR_CTRL_TRC | SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY\n", __func__, __LINE__);

    if (pLfaRtInfo == NULL)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d FAILING pLfaRtInfo is NULL \n", __func__,
                 __LINE__);
        return;
    }
    if (pLfaRtInfo->u1CmdType == LFA_ROUTE_SET)
    {
        RTM_PROT_UNLOCK ();
        SR_TRC3 (SR_CTRL_TRC,
                 "%s:%d ROUTE_SET event received !!! Primary NH %x Down \r\n",
                 __func__, __LINE__, pLfaRtInfo->u4NextHop);

        if (SrHandleRouteSetEvent (pLfaRtInfo) != SR_FAILURE)
        {
            SR_TRC2 (SR_CTRL_TRC,
                     "%s:%d Deleted the primary LSPs successfully\r\n",
                     __func__, __LINE__);
            SR_TRC2 (SR_CTRL_TRC,
                     "%s:%d Alternate path of LFA/RLFA activated\r\n", __func__,
                     __LINE__);
        }
        RTM_PROT_LOCK ();
        SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
        return;
    }
    else if (pLfaRtInfo->u1LfaType == SR_LFA)
    {
        RTM_PROT_UNLOCK ();
        switch (pLfaRtInfo->u1CmdType)
        {
            case LFA_ROUTE_ADD:

                SR_TRC2 (SR_CTRL_TRC, "%s:%d LFA_ROUTE_ADD event received\n",
                         __func__, __LINE__);
                SR_TRC3 (SR_CTRL_TRC, "%s:%d Destination Address is %x\n",
                         __func__, __LINE__, pLfaRtInfo->u4DestNet);
                SR_TRC3 (SR_CTRL_TRC, "%s:%d Primary Next Hop is %x\n",
                         __func__, __LINE__, pLfaRtInfo->u4NextHop);
                SR_TRC3 (SR_CTRL_TRC, "%s:%d LFA Next Hop is %x\n", __func__,
                         __LINE__, pLfaRtInfo->u4LfaNextHop);

                u2MlibOperation = MPLS_MLIB_FTN_CREATE;
                if (LfaRtAddOrDeleteEventHandler (pLfaRtInfo, u2MlibOperation)
                    != SR_FAILURE)
                {
                    SR_TRC2 (SR_CTRL_TRC,
                             "%s:%d Successfully processed LFA_ROUTE_ADD event \n",
                             __func__, __LINE__);
                }
                break;
            case LFA_ROUTE_DELETE:
                SR_TRC2 (SR_CTRL_TRC,
                         "%s:%d LFA_ROUTE_DELETE event received\n", __func__,
                         __LINE__);
                SR_TRC3 (SR_CTRL_TRC, "%s:%d Destination Address is %x\n",
                         __func__, __LINE__, pLfaRtInfo->u4DestNet);
                SR_TRC3 (SR_CTRL_TRC, "%s:%d Primary Next Hop is %x\n",
                         __func__, __LINE__, pLfaRtInfo->u4NextHop);
                SR_TRC3 (SR_CTRL_TRC, "%s:%d LFA Next Hop is %x\n", __func__,
                         __LINE__, pLfaRtInfo->u4LfaNextHop);

                u2MlibOperation = MPLS_MLIB_FTN_DELETE;
                if (LfaRtAddOrDeleteEventHandler (pLfaRtInfo, u2MlibOperation)
                    != SR_FAILURE)
                {
                    SR_TRC2 (SR_CTRL_TRC,
                             "%s:%d Successfully processed LFA_ROUTE_DELETE event\n",
                             __func__, __LINE__);
                }

                break;
            case LFA_ROUTE_RESET:
                SR_TRC2 (SR_CTRL_TRC, "%s:%d LFA_ROUTE_RESET event received\n",
                         __func__, __LINE__);
                break;
            case LFA_ROUTE_MODIFY:
                SR_TRC2 (SR_CTRL_TRC,
                         "%s:%d LFA_ROUTE_MODIFY event received\n", __func__,
                         __LINE__);
                break;
            default:
                break;
        }
        RTM_PROT_LOCK ();
    }
    else if (pLfaRtInfo->u1LfaType == SR_RLFA)
    {
        SrRLfaRtChangeEventHandler (pLfaRtInfo);
    }
    else if (pLfaRtInfo->u1LfaType == SR_TILFA)
    {
        SrTiLfaRtChangeEventHandler (pLfaRtInfo);
    }
    SR_TRC2 (SR_MAIN_TRC, "%s:%d EXIT\n", __func__, __LINE__);
    SR_TRC2 (SR_CTRL_TRC | SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return;

}

/*****************************************************************************/
/* Function Name : SrRLfaRtChangeEventHandler                                   */
/* Description   : This routine is called by SR module, for informing the
 *                 LFA Route changes.                                        */
/* Input(s)      : tNetIpv4RtInfo - Route info  as given by RTM              */
/*              u1CmdType - Command to ADD | Delete | Modify the Route.      */
/* Output(s)     : None                                                      */
/* Return(s)     : None                                                      */
/*****************************************************************************/

VOID
SrRLfaRtChangeEventHandler (tNetIpv4LfaRtInfo * pLfaRtInfo)
{
    UINT2               u2MlibOperation = SR_ZERO;
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY\n", __func__, __LINE__);
    if (pLfaRtInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d FAILING pLfaRtInfo is NULL \n", __func__,
                 __LINE__);
        return;
    }
    RTM_PROT_UNLOCK ();
    if (pLfaRtInfo->u1LfaType == SR_RLFA)
    {
        switch (pLfaRtInfo->u1CmdType)
        {
            case RLFA_ROUTE_ADD:

                SR_TRC2 (SR_UTIL_TRC, "%s:%d RLFA_ROUTE_ADD event received\n",
                         __func__, __LINE__);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Destination Address is %x\n",
                         __func__, __LINE__, pLfaRtInfo->u4DestNet);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Primary Next Hop is %x\n",
                         __func__, __LINE__, pLfaRtInfo->u4NextHop);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d RLFA Next Hop is %x\n", __func__,
                         __LINE__, pLfaRtInfo->u4LfaNextHop);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d RLFA Tunnel Id is %x\n", __func__,
                         __LINE__, pLfaRtInfo->uRemoteNodeId.u4RemNodeRouterId);
                u2MlibOperation = MPLS_MLIB_FTN_CREATE;
                if (RLfaRtAddOrDeleteEventHandler (pLfaRtInfo, u2MlibOperation)
                    != SR_FAILURE)
                {
                    SR_TRC2 (SR_UTIL_TRC,
                             "%s:%d Successfully processed RLFA_ROUTE_ADD event\n",
                             __func__, __LINE__);
                }
                break;
            case RLFA_ROUTE_DELETE:

                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d RLFA_ROUTE_DELETE event received\n", __func__,
                         __LINE__);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Destination Address is %x\n",
                         __func__, __LINE__, pLfaRtInfo->u4DestNet);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Primary Next Hop is %x\n",
                         __func__, __LINE__, pLfaRtInfo->u4NextHop);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d RLFA Next Hop is %x\n", __func__,
                         __LINE__, pLfaRtInfo->u4LfaNextHop);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d RLFA Tunnel Id is %x\n", __func__,
                         __LINE__, pLfaRtInfo->uRemoteNodeId.u4RemNodeRouterId);
                u2MlibOperation = MPLS_MLIB_FTN_DELETE;
                if (RLfaRtAddOrDeleteEventHandler (pLfaRtInfo, u2MlibOperation)
                    != SR_FAILURE)
                {
                    SR_TRC2 (SR_UTIL_TRC,
                             "%s:%d Successfully processed RLFA_ROUTE_DELETE event\n",
                             __func__, __LINE__);
                }
                break;
            case RLFA_ROUTE_RESET:
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d RLFA_ROUTE_RESET event received\n", __func__,
                         __LINE__);
                break;
            case RLFA_ROUTE_MODIFY:
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d RLFA_ROUTE_MODIFY event received\n", __func__,
                         __LINE__);
                pLfaRtInfo->u1CmdType = RLFA_ROUTE_DELETE;
                if (RLfaRtAddOrDeleteEventHandler (pLfaRtInfo, MPLS_MLIB_FTN_DELETE)
                                        != SR_FAILURE)
                {
                    SR_TRC2 (SR_UTIL_TRC,
                             "%s:%d Successfully processed RLFA_ROUTE_DELETE event\n ",
                             __func__, __LINE__);
                }

                pLfaRtInfo->u1CmdType = RLFA_ROUTE_ADD;
                if (RLfaRtAddOrDeleteEventHandler (pLfaRtInfo, MPLS_MLIB_FTN_CREATE)
                    != SR_FAILURE)
                {
                    SR_TRC2 (SR_UTIL_TRC,
                             "%s:%d Successfully processed RLFA_ROUTE_ADD event\n ",
                             __func__, __LINE__);
                }
                break;
            default:
                break;
        }
    }
    RTM_PROT_LOCK ();
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return;

}

/*****************************************************************************/
/* Function Name : SrProcessLfaIpv6RtChangeEvent                             */
/* Description   : This routine is called by SR module, for informing the
 *                 LFA Route changes.                                        */
/* Input(s)      : tNetIpv4RtInfo - Route info  as given by RTM              */
/*              u1CmdType - Command to ADD | Delete | Modify the Route.      */
/* Output(s)     : None                                                      */
/* Return(s)     : None                                                      */
/*****************************************************************************/
VOID
SrProcessLfaIpv6RtChangeEvent (tNetIpv6LfaRtInfo * pIpv6LfaRtInfo)
{
    UINT2               u2MlibOperation = SR_ZERO;
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY\n", __func__, __LINE__);
    if (pIpv6LfaRtInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d FAILING pIpv6LfaRtInfo is NULL\n",
                 __func__, __LINE__);
        return;
    }

    if (pIpv6LfaRtInfo->u1LfaType == SR_LFA)
    {

        switch (pIpv6LfaRtInfo->u1CmdType)
        {
            case IPv6_LFA_ROUTE_ADD:
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d IPv6_LFA_ROUTE_ADD event received\n", __func__,
                         __LINE__);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Destination Address is %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pIpv6LfaRtInfo->Ip6Dst));
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Primary Next Hop is %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pIpv6LfaRtInfo->NextHop));
                SR_TRC3 (SR_UTIL_TRC, "%s:%d LFA Next Hop is %s\n", __func__,
                         __LINE__, Ip6PrintAddr (&pIpv6LfaRtInfo->LfaNextHop));
                u2MlibOperation = MPLS_MLIB_FTN_CREATE;
                if (SrV3LfaRtAddOrDeleteEventHandler
                    (pIpv6LfaRtInfo, u2MlibOperation) != SR_FAILURE)
                {
                    SR_TRC2 (SR_UTIL_TRC,
                             "%s:%d Successfully processed IPv6_LFA_ROUTE_ADD event\n",
                             __func__, __LINE__);
                }
                break;
            case IPv6_LFA_ROUTE_DELETE:
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d IPv6_LFA_ROUTE_DELETE event received\n",
                         __func__, __LINE__);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Destination Address is %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pIpv6LfaRtInfo->Ip6Dst));
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Primary Next Hop is %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pIpv6LfaRtInfo->NextHop));
                SR_TRC3 (SR_UTIL_TRC, "%s:%d LFA Next Hop is %s\n", __func__,
                         __LINE__, Ip6PrintAddr (&pIpv6LfaRtInfo->LfaNextHop));
#if 0
                u2MlibOperation = MPLS_MLIB_FTN_DELETE;
                if (SrV3LfaRtAddOrDeleteEventHandler
                    (pIpv6LfaRtInfo, u2MlibOperation) != SR_FAILURE)
                {
                    SR_TRC2 (SR_MAIN_TRC,
                             "%s:%d Successfully processed IPv6_LFA_ROUTE_DELETE event\n ",
                             __func__, __LINE__);
                }
#endif
                break;
            case IPv6_LFA_ROUTE_SET:
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d IPv6_LFA_ROUTE_SET event received\n", __func__,
                         __LINE__);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Destination Address is %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pIpv6LfaRtInfo->Ip6Dst));
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Primary Next Hop is %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pIpv6LfaRtInfo->NextHop));
                SR_TRC3 (SR_UTIL_TRC, "%s:%d LFA Next Hop is %s\n", __func__,
                         __LINE__, Ip6PrintAddr (&pIpv6LfaRtInfo->LfaNextHop));
                u2MlibOperation = MPLS_MLIB_FTN_DELETE;
                if (SrV3LfaRtAddOrDeleteEventHandler
                    (pIpv6LfaRtInfo, u2MlibOperation) != SR_FAILURE)
                {
                    SR_TRC2 (SR_UTIL_TRC,
                             "%s:%d Successfully processed Ipv6_LFA_ROUTE_SET event\n",
                             __func__, __LINE__);
                }
                break;
            case IPv6_LFA_ROUTE_RESET:
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d IPv6_LFA_ROUTE_RESET event received\n",
                         __func__, __LINE__);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Destination Address is %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pIpv6LfaRtInfo->Ip6Dst));
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Primary Next Hop is %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pIpv6LfaRtInfo->NextHop));
                SR_TRC3 (SR_UTIL_TRC, "%s:%d LFA Next Hop is %s\n", __func__,
                         __LINE__, Ip6PrintAddr (&pIpv6LfaRtInfo->LfaNextHop));
                break;
            case IPv6_LFA_ROUTE_MODIFY:
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d IPv6_LFA_ROUTE_MODIFY event received\n",
                         __func__, __LINE__);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Destination Address is %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pIpv6LfaRtInfo->Ip6Dst));
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Primary Next Hop is %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pIpv6LfaRtInfo->NextHop));
                SR_TRC3 (SR_UTIL_TRC, "%s:%d LFA Next Hop is %s\n", __func__,
                         __LINE__, Ip6PrintAddr (&pIpv6LfaRtInfo->LfaNextHop));
                break;
            default:
                break;
        }
    }
    else if (pIpv6LfaRtInfo->u1LfaType == SR_RLFA)
    {
        SrV3RLfaRtChangeEventHandler (pIpv6LfaRtInfo);
    }
    else if (pIpv6LfaRtInfo->u1LfaType == SR_TILFA)
    {
        SrV3TiLfaRtChangeEventHandler (pIpv6LfaRtInfo);
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return;
}

/*****************************************************************************/
/* Function Name : SrV3RLfaRtChangeEventHandler                              */
/* Description   : This routine is called by SR module, for informing the    */
/*                   LFA Route changes.                                      */
/* Input(s)      : tNetIpv4RtInfo - Route info  as given by RTM              */
/*              u1CmdType - Command to ADD | Delete | Modify the Route.      */
/* Output(s)     : None                                                      */
/* Return(s)     : None                                                      */
/*****************************************************************************/
VOID
SrV3RLfaRtChangeEventHandler (tNetIpv6LfaRtInfo * pIpv6LfaRtInfo)
{
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY\n", __func__, __LINE__);
    UINT2               u2MlibOperation;
    if (pIpv6LfaRtInfo == NULL)
    {
        SR_TRC2 (SR_CRITICAL_TRC, "%s:%d FAILING "
                 "pIpv6LfaRtInfo is NULL\n", __func__, __LINE__);
        return;
    }
    switch (pIpv6LfaRtInfo->u1CmdType)
    {
        case IPv6_RLFA_ROUTE_ADD:
            u2MlibOperation = MPLS_MLIB_FTN_CREATE;
            if (SrV3RLfaRtAddOrDeleteEventHandler
                (pIpv6LfaRtInfo, u2MlibOperation) != SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC, "%s:%d Successfully"
                         " processed IPv6_RLFA_ROUTE_ADD event\n ", __func__,
                         __LINE__);
            }
            break;
        case IPv6_RLFA_ROUTE_DELETE:
            u2MlibOperation = MPLS_MLIB_FTN_DELETE;
            if (SrV3RLfaRtAddOrDeleteEventHandler
                (pIpv6LfaRtInfo, u2MlibOperation) != SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC, "%s:%d Successfully"
                         " processed IPv6_RLFA_ROUTE_DELETE event\n ", __func__,
                         __LINE__);
            }
            break;
        case IPv6_RLFA_ROUTE_SET:
            u2MlibOperation = MPLS_MLIB_FTN_DELETE;
            if (SrV3RLfaRtAddOrDeleteEventHandler
                (pIpv6LfaRtInfo, u2MlibOperation) != SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC, "%s:%d Successfully"
                         " processed IPv6_RLFA_ROUTE_SET event\n ", __func__,
                         __LINE__);
            }
            break;
        default:
            break;
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return;
}

/*****************************************************************************/
/* Function Name : SrV3TiLfaRtChangeEventHandler                             */
/* Description   : This routine is called by SR module, for informing the    */
/*                 TILFA Route changes.                                      */
/* Input(s)      : tNetIpv4RtInfo - Route info  as given by RTM              */
/*                 u1CmdType - Command to ADD | Delete | Modify the Route.   */
/* Output(s)     : None                                                      */
/* Return(s)     : None                                                      */
/*****************************************************************************/
VOID
SrV3TiLfaRtChangeEventHandler (tNetIpv6LfaRtInfo * pIpv6LfaRtInfo)
{
    SR_TRC2 (SR_MAIN_TRC, "%s:%d ENTRY\n", __func__, __LINE__);
    UINT2               u2MlibOperation;
    if (pIpv6LfaRtInfo == NULL)
    {
        SR_TRC2 (SR_CRITICAL_TRC, "%s:%d FAILING "
                 "pIpv6LfaRtInfo is NULL\n", __func__, __LINE__);
        return;
    }
    switch (pIpv6LfaRtInfo->u1CmdType)
    {
        case IPv6_TILFA_ROUTE_ADD:
            u2MlibOperation = MPLS_MLIB_FTN_CREATE;
            if (SrV3TiLfaRtAddOrDeleteEventHandler
                (pIpv6LfaRtInfo, u2MlibOperation) != SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC, "%s:%d Successfully processed "
                         "IPv6_TILFA_ROUTE_ADD event\n ", __func__, __LINE__);
            }
            break;
        case IPv6_TILFA_ROUTE_DELETE:
            u2MlibOperation = MPLS_MLIB_FTN_DELETE;
            if (SrV3TiLfaRtAddOrDeleteEventHandler
                (pIpv6LfaRtInfo, u2MlibOperation) != SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC, "%s:%d Successfully processed "
                         "IPv6_TILFA_ROUTE_DELETE event\n ", __func__, __LINE__);
            }
            break;
        case IPv6_TILFA_ROUTE_SET:
            u2MlibOperation = MPLS_MLIB_FTN_DELETE;
            if (SrV3TiLfaRtAddOrDeleteEventHandler
                (pIpv6LfaRtInfo, u2MlibOperation) != SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC, "%s:%d Successfully processed "
                         "IPv6_TILFA_ROUTE_SET event\n ", __func__, __LINE__);
            }
            break;
        default:
            break;
    }
    SR_TRC2 (SR_MAIN_TRC, "%s:%d EXIT\n", __func__, __LINE__);
    return;
}

/*****************************************************************************
 * Function Name : LfaCreateAndDeleteLSP
 * Description   : This routine is uses the event info and create or delete the
                   LFA route.
 *
 *
 * Input(s)      : pSrIpEvtInfo   - Event Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
LfaCreateAndDeleteLSP (UINT2 u2MlibOperation, tSrRtrInfo * pSrRtrInfo,
                       tSrRtrNextHopInfo * pSrRtrNextHopInfo)
{

    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4IpAddress = SR_ZERO;
    UINT4               u4Port = SR_ZERO;
    UINT4               u4L3Ifindex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    UINT4               u4SrGbMinIndex = SR_ZERO;
    UINT4               u4CfaIfindex = SR_ZERO;
    INT4                i4LfaStatus = ALTERNATE_DISABLED;
    tNetIpv4LfaRtInfo   NetIpLfaRtInfo;
    tLfaRtInfoQueryMsg  LfaRtQuery;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tSrInSegInfo        SrInSegInfo;
    tSrTeRtEntryInfo    getSrTeRtEntryInfo;
    tSrTeRtEntryInfo   *pSrTeRtEntryInfo = NULL;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrTilfaPathInfo     SrTilfaPathInfo;
    tSrTilfaPathInfo    *pSrTilfaPathInfo = NULL;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY\n", __func__, __LINE__);

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&NetIpLfaRtInfo, SR_ZERO, sizeof (tNetIpv4LfaRtInfo));
    MEMSET (&LfaRtQuery, SR_ZERO, sizeof (tLfaRtInfoQueryMsg));
    MEMSET (&getSrTeRtEntryInfo, 0, sizeof (tSrTeRtEntryInfo));
    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;

    /* Ignore the Notification if LfaStatus is not ENABLED Administratively */
    nmhGetFsSrV4AlternateStatus (&i4LfaStatus);

    SR_TRC3 (SR_UTIL_TRC, "%s:%d i4LfaStatus %d\n", __func__, __LINE__, i4LfaStatus);

    /* Populate SrInSegInfo from Global SelfNodeInfo */
    if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
    {
        u4SrGbMinIndex = gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
    }

    /*SR TE LFA Processing (both LFA enable code flow starts */
    if ((pSrRtrInfo != NULL) && (pSrRtrInfo->bIsTeconfigured == SR_TRUE))
    {
        if (i4LfaStatus == ALTERNATE_ENABLED)    /* SR TE LFA LSP creation via "Alternate enable in SR */
        {
            /* SR TE LFA LSP creation with destination IP */
            getSrTeRtEntryInfo.destAddr.Addr.u4Addr =
                pSrRtrInfo->prefixId.Addr.u4Addr;
            getSrTeRtEntryInfo.destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
            if ((pSrTeRtEntryInfo =
                 SrGetNextTeRouteTableEntry (&getSrTeRtEntryInfo)) != NULL)
            {
                if (pSrTeRtEntryInfo->mandRtrId.Addr.u4Addr != SR_ZERO)
                {
                    if (SrTeLfaRouteUpdate
                        (&pSrTeRtEntryInfo->destAddr,
                         &pSrTeRtEntryInfo->mandRtrId) == SR_SUCCESS)
                    {
                        if (SrTeLfaCreateFtnIlm (pSrTeRtEntryInfo) ==
                            SR_FAILURE)
                        {
                            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                                     "%s:%d SR TE LFA lsp creation failed for prefix %x \n",
                                     __func__, __LINE__,
                                     pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                            return SR_FAILURE;
                        }
                    }
                    else
                    {
                        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                                 " Line %d func %s SR TE LFA route updated failed \r\n",
                                 __LINE__, __func__);
                        return SR_FAILURE;
                    }
                }

            }
            return SR_SUCCESS;
        }
        else                    /* SR TE LFA LSP deletion via "Alternate disable in SR */
        {
            getSrTeRtEntryInfo.destAddr.Addr.u4Addr =
                pSrRtrInfo->prefixId.Addr.u4Addr;
            getSrTeRtEntryInfo.destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
            if ((pSrTeRtEntryInfo =
                 SrGetNextTeRouteTableEntry (&getSrTeRtEntryInfo)) != NULL)
            {
                if (pSrTeRtEntryInfo->LfaNexthop.Addr.u4Addr != SR_ZERO)
                {
                    if (SrTeLfaDeleteFtnIlm (pSrTeRtEntryInfo) == SR_FAILURE)
                    {
                        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                                 "%s:%d SR TE LFA lsp deletion failed for prefix %x \n",
                                 __func__, __LINE__,
                                 pSrTeRtEntryInfo->destAddr.Addr.u4Addr);
                        return SR_FAILURE;
                    }
                }
            }
            return SR_SUCCESS;

        }

    }
    /* SR FRR processing for NON-TE routes starts */

    if (i4LfaStatus == ALTERNATE_ENABLED)    /* Alternate enable flow in SR */
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
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d Exiting: Not able to get IfIndex from MplsTnlIf\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            if (NetIpv4GetPortFromIfIndex (u4L3Ifindex, &u4Port) == IP_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d Exiting: NetIpv4GetPortFromIfIndex failed\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            LfaRtQuery.u4NextHopIfIndx = u4Port;
            if (NetIpv4LfaGetRoute (&LfaRtQuery, &NetIpLfaRtInfo) !=
                NETIPV4_SUCCESS)
            {
                SR_TRC3 (SR_UTIL_TRC,
                         "%s:%d Exiting: Not able to fetch the lfa route for the destination %x\n",
                         __func__, __LINE__, LfaRtQuery.u4DestinationIpAddress);
                return SR_SUCCESS;
            }
        }
        else
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                     "%s:%d Exiting: pSrRtrInfo or pSrRtrNextHopInfo is NULL\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        if (NetIpLfaRtInfo.u1LfaType == SR_LFA)
        {
            if (pSrRtrInfo->u4LfaNextHop == NetIpLfaRtInfo.u4LfaNextHop)
            {
                SR_TRC2 (SR_UTIL_TRC, "%s:%d Exit: SR LFA already Exists\n",
                         __func__, __LINE__);
                return SR_SUCCESS;
            }

            if (pSrRtrInfo->bIsTeconfigured == SR_TRUE)
            {
                SR_TRC1 (SR_UTIL_TRC,
                         " SR TE is configured for the prefix %x ===so SKIP LFA COMPUTATION \r\n",
                         pSrRtrInfo->prefixId.Addr.u4Addr);
                return SR_SUCCESS;
            }
            else
            {

                SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
                SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_TRUE;
                u4IpAddress =
                    (NetIpLfaRtInfo.u4LfaNextHop & NetIpLfaRtInfo.u4DestMask);
                if (CfaIpIfGetIfIndexForNetInCxt
                    (SR_ZERO, u4IpAddress,
                     &SrRtrAltNextHopInfo.u4OutIfIndex) == CFA_FAILURE)
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d Exiting: Not able to get Out IfIndex \n",
                             __func__, __LINE__);
                }
                u4CfaIfindex = SrRtrAltNextHopInfo.u4OutIfIndex;
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
                        SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
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
                        SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
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
                        (u2MlibOperation, pSrRtrInfo, NULL,
                         &SrRtrAltNextHopInfo) == SR_SUCCESS)
                    {
                        SR_TRC4 (SR_UTIL_TRC,
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
                        SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                                 "%s:%d LFA LSP creation failed for FEC: %x via Next Hop: %x\n",
                                 __func__, __LINE__,
                                 pSrRtrInfo->prefixId.Addr.u4Addr,
                                 pSrRtrInfo->u4LfaNextHop);
                        return SR_FAILURE;
                    }
                    SrRtrAltNextHopInfo.u4SwapOutIfIndex = u4CfaIfindex;
                    SrRtrAltNextHopInfo.bIsLfaNextHop = SR_TRUE;
                    if (SrMplsCreateILM (pSrRtrInfo, &SrRtrAltNextHopInfo) ==
                        SR_SUCCESS)
                    {
                        SR_TRC4 (SR_UTIL_TRC,
                                 "%s:%d LFA ILM created successfully for FEC: %x via Next Hop: %x\n",
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
                                 "%s:%d LFA ILM creation failed for FEC: %x via Next Hop: %x\n",
                                 __func__, __LINE__,
                                 pSrRtrInfo->prefixId.Addr.u4Addr,
                                 pSrRtrInfo->u4LfaNextHop);
                        return SR_FAILURE;
                    }
                }
            }

        }
        else if (NetIpLfaRtInfo.u1LfaType == SR_RLFA)
        {
            if (i4LfaStatus == ALTERNATE_ENABLED)
            {
                if (SrIpv4RlfaFetchAndCreateLSP (&NetIpLfaRtInfo) == SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d Creation of SR RLFA FTN failed\n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
                if (SrIpv4RlfaILMCreation (&NetIpLfaRtInfo, pSrRtrInfo) ==
                    SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d Creation of SR RLFA ILM failed\n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
            }
        }
        else if (NetIpLfaRtInfo.u1LfaType == SR_TILFA)
        {
            /* Update the TILFA info for the Node as Destinaiton , if the info already
                   received from RTM */
            SrTilfaPathInfo.destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
            SrTilfaPathInfo.destAddr.Addr.u4Addr = pSrRtrInfo->prefixId.Addr.u4Addr;
            pSrTilfaPathInfo = RBTreeGet (gSrGlobalInfo.pSrTilfaPathRbTree,
                                        &SrTilfaPathInfo);
            if (NULL == pSrTilfaPathInfo)
            {
                NetIpLfaRtInfo.u1CmdType = TILFA_ROUTE_ADD;
                TiLfaRtAddOrDeleteEventHandler (&NetIpLfaRtInfo,
                                            MPLS_MLIB_FTN_CREATE);
            }
            else
            {
                if(SR_FAILURE == SrIpv4TiLfaUpdateLabelStackInfo(pSrTilfaPathInfo))
                {
                    SR_TRC3 (SR_MAIN_TRC, "%s:%d TILFA label stack update failed for dest: %x\n ",
                             __func__, __LINE__, pSrTilfaPathInfo->destAddr.Addr.u4Addr);
                }

                if (pSrTilfaPathInfo->u1LfaStatusFlags & SR_TILFA_SET_LABEL_STACK)
                {
                    if (SR_FAILURE == SrIpv4TiLfaCreateLSP (pSrTilfaPathInfo))
                    {
                        SR_TRC3 (SR_MAIN_TRC, "%s:%d Creation of TILFA failed for dest: %x\n ",
                                 __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
                        return SR_FAILURE;
                    }
#if 1
                    if (SR_FAILURE == SrIpv4TiLfaCreateILM (pSrTilfaPathInfo))
                    {
                        SR_TRC3 (SR_MAIN_TRC, "%s:%d Creation of TILFA failed for dest: %x \n ",
                                 __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
                        return SR_FAILURE;
                    }
#endif
                }
            }
        }
    }
    else                        /* Alternate disable flow in SR */
    {
        if ((pSrRtrInfo != NULL) && (pSrRtrNextHopInfo != NULL))
        {
            SR_TRC4 (SR_UTIL_TRC, "%s:%d Destination %x Lfa Type %d\n", __func__, __LINE__,
                    pSrRtrInfo->prefixId.Addr.u4Addr, pSrRtrInfo->u1LfaType);

            if ((pSrRtrInfo->u4LfaNextHop != SR_ZERO) && (pSrRtrInfo->u1LfaType == SR_LFA_ROUTE))    /* LFA route delete */
            {
                SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
                SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_TRUE;
                MEMCPY (&SrRtrAltNextHopInfo.nextHop, &pSrRtrInfo->u4LfaNextHop,
                        sizeof (UINT4));

                MEMCPY (&SrRtrAltNextHopInfo.PrimarynextHop,
                        &pSrRtrNextHopInfo->nextHop, sizeof (tGenU4Addr));

                SrRtrAltNextHopInfo.u4OutIfIndex = pSrRtrInfo->u4LfaOutIfIndex;
                u4MplsTnlIfIndex = SrRtrAltNextHopInfo.u4OutIfIndex;
                u2MlibOperation = MPLS_MLIB_FTN_DELETE;
                if (SrMplsCreateOrDeleteLsp
                    (u2MlibOperation, pSrRtrInfo, NULL,
                     &SrRtrAltNextHopInfo) == SR_SUCCESS)
                {
                    SR_TRC4 (SR_UTIL_TRC,
                             "%s:%d LFA LSP created successfully for FEC: %x via Next Hop: %x\n",
                             __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
                             pSrRtrInfo->u4LfaNextHop);
                }
                else
                {
                    SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                             "%s:%d LFA LSP creation failed for FEC: %x via Next Hop: %x\n",
                             __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
                             pSrRtrInfo->u4LfaNextHop);
                    return SR_FAILURE;
                }
                if (CfaUtilGetIfIndexFromMplsTnlIf
                    (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
                {
                    if (CfaIfmDeleteStackMplsTunnelInterface
                        (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                    {
                        SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                                 "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed\n",
                                 __func__, __LINE__, u4MplsTnlIfIndex,
                                 u4L3VlanIf);
                        return SR_FAILURE;
                    }
                }
                SrRtrAltNextHopInfo.u4OutIfIndex = SR_ZERO;
                SrRtrAltNextHopInfo.u4SwapOutIfIndex =
                    pSrRtrInfo->u4LfaSwapOutIfIndex;
                /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
                SrInSegInfo.u4InLabel =
                    (u4SrGbMinIndex + pSrRtrInfo->u4SidValue);
                if (SrMplsDeleteILM
                    (&SrInSegInfo, pSrRtrInfo,
                     &SrRtrAltNextHopInfo) == SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to delete ILM SWAP \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
                pSrRtrInfo->u4LfaNextHop = SR_FALSE;
                pSrRtrInfo->u1LfaType = SR_ZERO;
                pSrRtrInfo->u4LfaOutIfIndex = SR_ZERO;
                pSrRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;

            }
            else if (pSrRtrInfo->u1LfaType == SR_RLFA_ROUTE)    /* RLFA Route delete */
            {
                pSrRlfaPathInfo =
                    SrGetRlfaPathEntryFromDestAddr (&pSrRtrInfo->prefixId);
                if (pSrRlfaPathInfo != NULL)
                {
                    if (pSrRlfaPathInfo->bIsRlfaActive == SR_ONE)
                    {
                        SR_TRC2 (SR_UTIL_TRC,
                                 "%s:%d SR RLFA PATH is Active \n", __func__,
                                 __LINE__);
                        return SR_SUCCESS;
                    }
                    /* RLFA route is not active, hence delete the entries */
                    if (SrRlfaDeleteFTNAndILM
                        (&pSrRtrInfo->prefixId, &pSrRlfaPathInfo->mandRtrId,
                         &pSrRlfaPathInfo->rlfaNextHop, &SrInSegInfo,
                         u4SrGbMinIndex) != SR_SUCCESS)
                    {
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                 "%s:%d Deletion of SR RLFA FTN/ILM Failure\n",
                                 __func__, __LINE__);
                        return SR_FAILURE;
                    }
                }

            }
            else if (pSrRtrInfo->u1LfaType == SR_TILFA_ROUTE)    /* TILFA Route delete */
            {
                /* Delete TILFA for the Rtr node*/
                SrTilfaPathInfo.destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                SrTilfaPathInfo.destAddr.Addr.u4Addr = pSrRtrInfo->prefixId.Addr.u4Addr;
                pSrTilfaPathInfo = RBTreeGet (gSrGlobalInfo.pSrTilfaPathRbTree,
                                            &SrTilfaPathInfo);
                if (NULL != pSrTilfaPathInfo)
                {
                    SR_TRC3 (SR_UTIL_TRC, "%s:%d Ti LFA path found for Dest %x\n",
                            __func__, __LINE__,
                            pSrRtrInfo->prefixId.Addr.u4Addr);
                    TiLfaHandleIpv4RouteDel(pSrTilfaPathInfo);
                }
            }
        }
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    UNUSED_PARAM (u2MlibOperation);
    return SR_SUCCESS;
}

 /*****************************************************************************/
 /* Function Name : SrIpv4RlfaILMCreation                                     */
 /* Description   : Function will create the SR RLFA ILM                      */
 /* Input(s)      : pNetIpv4LfaRtInfo - Pointer to RLFA Route Info from RTM   */
 /*                 pSrRtrInfo - Pointer to Router Info                       */
 /* Output(s)     : None                                                      */
 /* Return(s)     : INT4                                                      */
 /*****************************************************************************/
UINT4
SrIpv4RlfaILMCreation (tNetIpv4LfaRtInfo * pNetIpLfaRtInfo,
                       tSrRtrInfo * pSrRtrInfo)
{
    tSrRtrNextHopInfo  *pSrLfaNextHopInfo = NULL;
    tSrRlfaPathInfo    *pSrRlfaPathInfo   = NULL;
    tSrInSegInfo        SrInSegInfo;
    tSrTeLblStack       SrTeLblStack;
    UINT4               u4L3VlanIf        = SR_ZERO;
    UINT4               u4MplsTnlIfIndex  = SR_ZERO;
    UINT1               u1NHFound         = SR_FALSE;

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (SrInSegInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (SrTeLblStack));

    if (pSrRtrInfo == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL for dest %x\n",
                __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
        return SR_FAILURE;
    }
    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY for Rtr %x, AdvRtr %x\n",
            __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
            pSrRtrInfo->advRtrId.Addr.u4Addr);

    /* Fetching RLFA path entry */
    pSrRlfaPathInfo = SrGetRlfaPathEntryFromDestAddr (&(pSrRtrInfo->prefixId));
    if (pSrRlfaPathInfo == NULL)
    {
        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d RLFA entry not present for Dest=%x\n",
                __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
        return SR_FAILURE;
    }

    if (pSrRtrInfo->u4SidValue == SR_ZERO)
    {
        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d: SID value is 0 for dest %x\n",
                 __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
        return SR_FAILURE;
    }

    if (pSrRtrInfo->u1SIDConflictWin == SR_CONFLICT_NOT_WINNER)
    {
        SR_TRC3 (SR_UTIL_TRC, "%s:%d: Returning since NOT_WINNER for %x \n",
                 __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    if (pSrRtrInfo->u1PrefixConflictMPLSDel == SR_TRUE)
    {
        SR_TRC3 (SR_UTIL_TRC, "%s:%d Returning since PrefixConflictMPLSDel for %x \n",
                 __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    if (pSrRtrInfo->u1OutOfRangeAlarm == SR_ALARM_RAISE)
    {
        SR_TRC3 (SR_UTIL_TRC, "%s:%d Returning since OOR Alarm for %x \n",
                 __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    SrInSegInfo.u4InLabel =  SrReserveMplsInLabel (pSrRtrInfo->u2AddrType,
                                             pSrRtrInfo->u1SidIndexType,
                                             pSrRtrInfo->u4SidValue);

    SR_TRC4 (SR_UTIL_TRC, "%s:%d RLFA ILM Inlabel %d for dest %x \n", __func__,
            __LINE__, SrInSegInfo.u4InLabel, pSrRtrInfo->prefixId.Addr.u4Addr);
    if (SrInSegInfo.u4InLabel == SR_ZERO)
    {
        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d RLFA ILM Inlabel 0 for dest %x \n",
                 __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
        return SR_FAILURE;
    }

    TMO_SLL_Scan (&(pSrRtrInfo->NextHopList), pSrLfaNextHopInfo,
                   tSrRtrNextHopInfo *)
    {
        if ((pSrLfaNextHopInfo->nextHop.u2AddrType == SR_IPV4_ADDR_TYPE) &&
         (pNetIpLfaRtInfo->u4LfaNextHop ==
          pSrLfaNextHopInfo->nextHop.Addr.u4Addr)
         && (pSrLfaNextHopInfo->bIsLfaNextHop == SR_TRUE)
         && (pSrLfaNextHopInfo->u1FRRNextHop == SR_RLFA_NEXTHOP))
        {
            u1NHFound = SR_TRUE;
            break;
        }
    }

    if (u1NHFound == SR_FALSE)
    {
        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d RLFA next hop not found for dest %x\n",
                __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
        return SR_FAILURE;
    }

#ifdef CFA_WANTED
    if (CfaIpIfGetIfIndexForNetInCxt (SR_ZERO, pNetIpLfaRtInfo->u4LfaNextHop,
                                    &(pSrLfaNextHopInfo->u4SwapOutIfIndex))
                                    ==  CFA_FAILURE)
    {
        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                "%s:%d RLFA ILM SwapOut Index not got for dest %x \n",
                __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
    }

    /*pSrRtrInfo->u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
    if (CfaIfmCreateStackMplsTunnelInterface(pSrLfaNextHopInfo->u4SwapOutIfIndex,
                                         &u4MplsTnlIfIndex) == CFA_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC,
                "%s:%d pSrRtrNextHopInfo->u4SwapOutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                __func__, __LINE__, pSrLfaNextHopInfo->u4SwapOutIfIndex,
                u4MplsTnlIfIndex);

        pSrLfaNextHopInfo->u4SwapOutIfIndex = u4MplsTnlIfIndex;
    }
    else
    {
        SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                "%s:%d pSrRtrNextHopInfo->u4SwapOutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                __func__, __LINE__, pSrLfaNextHopInfo->u4SwapOutIfIndex,
                u4MplsTnlIfIndex);
        return SR_FAILURE;
    }
#endif

    SR_TRC4 (SR_UTIL_TRC, "%s:%d RLFA remote-node: 0x%x nextHop: 0x%x \n",
              __func__, __LINE__, pSrRlfaPathInfo->mandRtrId.Addr.u4Addr,
              pSrLfaNextHopInfo->nextHop.Addr.u4Addr);

    SR_TRC5 (SR_UTIL_TRC, "%s:%d RLFA remote-label %d dest-label %d for dest 0x%x \n",
              __func__, __LINE__, pSrRlfaPathInfo->u4RemoteLabel,
              pSrRlfaPathInfo->u4DestLabel, pSrRtrInfo->prefixId.Addr.u4Addr);

    /* Set the TopLabel and the following Label Stack
    * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
    SrTeLblStack.u4TopLabel = pSrRlfaPathInfo->u4RemoteLabel;
    SrTeLblStack.u4LabelStack[SR_ZERO] = pSrRlfaPathInfo->u4DestLabel;
    SrTeLblStack.u1StackSize = SR_ONE;

    if (SrMplsCreateOrDeleteStackLsp(MPLS_MLIB_ILM_CREATE, pSrRtrInfo,
                             &SrInSegInfo, &SrTeLblStack,
                             pSrLfaNextHopInfo, SR_TRUE) == SR_FAILURE)
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                  "%s:%d TILfa ILM creation failed for FEC: %x via Next Hop: %x\n",
                  __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
                  pSrRtrInfo->u4LfaNextHop);
        if (SrReleaseMplsInLabel(pSrRtrInfo->u2AddrType,
                                 pSrRtrInfo->u4SidValue) == SR_FAILURE)
        {
             SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                     "%s:%d Unable to Release RLFA ILM In-Label for dest %x\n",
                     __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
             return SR_FAILURE;
        }

#ifdef CFA_WANTED
        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
                                            != CFA_FAILURE)
        {

            if (CfaIfmDeleteStackMplsTunnelInterface(u4L3VlanIf,
                                    u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                      "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                      __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }

            pSrLfaNextHopInfo->u4SwapOutIfIndex = u4L3VlanIf;
        }
#endif
        return SR_FAILURE;
    }

    pSrRlfaPathInfo->u4RlfaIlmTunIndex = pSrLfaNextHopInfo->u4SwapOutIfIndex;

    SR_TRC4 (SR_UTIL_TRC,
            "%s:%d RLFA ILM created successfully for FEC: %x via Next Hop: %x\n",
            __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
            pSrRtrInfo->u4LfaNextHop);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : LfaRtAddOrDeleteEventHandler
 * Description   : This routine is uses the event info and create or delete the
                   LFA route.
 *
 *
 * Input(s)      : pSrIpEvtInfo   - Event Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
LfaRtAddOrDeleteEventHandler (tNetIpv4LfaRtInfo * pLfaRtInfo,
                              UINT2 u2MlibOperation)
{
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY with \n", __func__, __LINE__);

    tSrTeRtEntryInfo    getSrTeRtEntryInfo;
    tSrTeRtEntryInfo   *pSrTeLfaPathInfo = NULL;
    tSrTeRtEntryInfo   *pSrTeDestRtInfo = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRtrInfo          SrRtrInfo;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tSrRtrNextHopInfo  *pSrRtrPriNxtHopInfo = NULL;
    tSrInSegInfo        SrInSegInfo;
    tRouterId           rtrId;
    tGenU4Addr          DestAddr;
    tGenU4Addr          PrimaryNextHop;
    UINT4               u4IpAddress = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    UINT4               u4SrGbMinIndex = SR_ZERO;
    UINT4               u4CfaIfindex = SR_ZERO;
    UINT4               u4IntIf = SR_ZERO;

    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&SrRtrInfo, SR_ZERO, sizeof (tSrRtrInfo));
    MEMSET (&rtrId, SR_ZERO, sizeof (tRouterId));
    MEMSET (&DestAddr, 0, sizeof (tGenU4Addr));
    MEMSET (&getSrTeRtEntryInfo, 0, sizeof (tSrTeRtEntryInfo));
    MEMSET (&PrimaryNextHop, 0, sizeof (tGenU4Addr));

    SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;

    if ((MPLS_MLIB_FTN_CREATE == u2MlibOperation) ||
        ((MPLS_MLIB_FTN_DELETE == u2MlibOperation)
         && (pLfaRtInfo->u1CmdType != LFA_ROUTE_SET)))
    {
        DestAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        MEMCPY (&DestAddr.Addr.u4Addr, &pLfaRtInfo->u4DestNet, sizeof (UINT4));

        pRtrInfo = SrGetSrRtrInfoFromRtrId (&DestAddr);
        if (pRtrInfo == NULL)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL\n", __func__,
                     __LINE__);
            return SR_FAILURE;
        }

        /* if route is added to delay list, LFA info will be updated once
           primary route update done */
        if ((pRtrInfo->u1OprFlags & SR_OPR_F_ADD_DELAY_LIST)
            == SR_OPR_F_ADD_DELAY_LIST)
        {
            SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                "%s:%d Router node %x with prefix %x added to delay list",
                __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr,
                pLfaRtInfo->u4DestNet);
            return SR_SUCCESS;
        }

        /* Populate SrInSegInfo from Global SelfNodeInfo */
        if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
        {
            u4SrGbMinIndex = gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
        }
    }

    /*Create the LFA LSPs on LFA_ROUTE_ADD */
    if (u2MlibOperation == MPLS_MLIB_FTN_CREATE)
    {
        /* SR TE LFA processing with mandatory node as reference in LFA_ROUTE ADD flow  */

        while ((pSrTeLfaPathInfo =
                SrGetNextTeRouteTableEntry (&getSrTeRtEntryInfo)) != NULL)
        {
            if (pSrTeLfaPathInfo->mandRtrId.Addr.u4Addr ==
                pLfaRtInfo->u4DestNet)
            {
                pSrTeLfaPathInfo->LfaNexthop.Addr.u4Addr =
                    pLfaRtInfo->u4LfaNextHop;
                pSrTeLfaPathInfo->LfaNexthop.u2AddrType = SR_IPV4_ADDR_TYPE;
                if (SrTeFRRFtnCreateOrDel (pSrTeLfaPathInfo, SR_TRUE) ==
                    SR_FAILURE)
                {
                    SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                             "%s:%d SR TE LFA lsp creation failed for prefix %x \n",
                             __func__, __LINE__,
                             pSrTeLfaPathInfo->destAddr.Addr.u4Addr);
                }
            }

            MEMSET (&getSrTeRtEntryInfo, SR_ZERO, sizeof (tSrTeRtEntryInfo));
            getSrTeRtEntryInfo.destAddr.Addr.u4Addr =
                pSrTeLfaPathInfo->destAddr.Addr.u4Addr;
            getSrTeRtEntryInfo.mandRtrId.Addr.u4Addr =
                pSrTeLfaPathInfo->mandRtrId.Addr.u4Addr;
            getSrTeRtEntryInfo.destAddr.u2AddrType =
                pSrTeLfaPathInfo->destAddr.u2AddrType;
            getSrTeRtEntryInfo.mandRtrId.u2AddrType =
                pSrTeLfaPathInfo->mandRtrId.u2AddrType;
        }
        /* SR TE LFA processing with Destiantion node as reference in LFA_ROUTE ADD flow */
        if (pRtrInfo->bIsTeconfigured == SR_TRUE)
        {
            pSrTeDestRtInfo = SrGetTeRouteEntryFromDestAddr (&pRtrInfo->prefixId);
            if (pSrTeDestRtInfo != NULL)
            {
                if (pLfaRtInfo->u4LfaNextHop != SR_ZERO)
                {
                    pSrTeDestRtInfo->FrrIlmNexthop.Addr.u4Addr =
                        pLfaRtInfo->u4LfaNextHop;
                    pSrTeDestRtInfo->FrrIlmNexthop.u2AddrType =
                        SR_IPV4_ADDR_TYPE;
                    if (SrTeFRRIlmAddorDelete
                        (&pRtrInfo->prefixId, pSrTeDestRtInfo,
                         SR_TRUE) == SR_FAILURE)
                    {
                        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                 "%s:%d FRR ILM create via LFA ROUTE ADD for TE configured prefix %x with LFA Nexthop %x has failed \n",
                                 __func__, __LINE__, DestAddr.Addr.u4Addr,
                                 pLfaRtInfo->u4LfaNextHop);
                    }
                }
                SR_TRC3 (SR_UTIL_TRC,
                         "%s:%d Since TE is configured for the prefix %x, created FRR ILM alone \n",
                         __func__, __LINE__, DestAddr.Addr.u4Addr);
                return SR_SUCCESS;
            }
        }

        if (pRtrInfo->u4LfaNextHop == pLfaRtInfo->u4LfaNextHop)
        {
            SR_TRC4 (SR_UTIL_TRC,
                     "%s:%d SR LFA LSP already created for prefix %x with Lfa nexthop %x \n",
                     __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                     pLfaRtInfo->u4LfaNextHop);
            return SR_SUCCESS;
        }
        else if (pRtrInfo->u4LfaNextHop != SR_ZERO)
        {
            /* Deleting the existing lfa, and  add the new lfa */
            SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;
            SrRtrAltNextHopInfo.u4OutIfIndex = pRtrInfo->u4LfaOutIfIndex;
            SrRtrAltNextHopInfo.u4SwapOutIfIndex =
                pRtrInfo->u4LfaSwapOutIfIndex;
            SrRtrAltNextHopInfo.nextHop.Addr.u4Addr = pRtrInfo->u4LfaNextHop;
            SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
            SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
            SrInSegInfo.u4InLabel = (u4SrGbMinIndex + pRtrInfo->u4SidValue);
            if (SrMplsDeleteILM (&SrInSegInfo, pRtrInfo, &SrRtrAltNextHopInfo)
                == SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to delete ILM SWAP \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }

            if (SrMplsCreateOrDeleteLsp
                (MPLS_MLIB_FTN_DELETE, pRtrInfo, NULL,
                 &SrRtrAltNextHopInfo) == SR_SUCCESS)
            {
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d Deletion of LFA LSP successfull\n", __func__,
                         __LINE__);
            }
            else
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Deletion of LFA LSP failed\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            if (CfaUtilGetIfIndexFromMplsTnlIf
                (SrRtrAltNextHopInfo.u4OutIfIndex, &u4IntIf,
                 TRUE) != CFA_FAILURE)
            {
                if (CfaIfmDeleteStackMplsTunnelInterface
                    (u4IntIf, SrRtrAltNextHopInfo.u4OutIfIndex) == CFA_FAILURE)
                {
                    SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed\n",
                             __func__, __LINE__,
                             SrRtrAltNextHopInfo.u4OutIfIndex, u4IntIf);
                    return SR_FAILURE;
                }
            }
            SrRtrAltNextHopInfo.u4OutIfIndex = SR_ZERO;
            SrRtrAltNextHopInfo.u4SwapOutIfIndex = SR_ZERO;
            pRtrInfo->u4LfaNextHop = SR_FALSE;
            pRtrInfo->u1LfaType = SR_ZERO;
            pRtrInfo->u4LfaOutIfIndex = SR_ZERO;
            pRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
            pRtrInfo->bIsLfaActive = SR_ZERO;
            MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
        }

        /* Checking if Primary Lsps are present and not as Pop & Fwd  */
        pSrRtrPriNxtHopInfo = SrV4GetSrNextHopInfoFromRtr (pRtrInfo,
                                                           pLfaRtInfo->
                                                           u4NextHop);
        if ((pSrRtrPriNxtHopInfo == NULL)
            || (pSrRtrPriNxtHopInfo->u1MPLSStatus != SR_FTN_CREATED))
        {
            SR_TRC3 (SR_UTIL_TRC, "%s:%d Lfa Lsp Not created for %x \n",
                     __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
            if (pSrRtrPriNxtHopInfo != NULL)
            {
                SR_TRC1 (SR_UTIL_TRC | SR_FAIL_TRC,
                         " Lfa-Add Failed due to Primary Lsp status %x \n",
                         pSrRtrPriNxtHopInfo->u1MPLSStatus);
            }
            return SR_FAILURE;
        }
        SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
        SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_TRUE;

        u4IpAddress = (pLfaRtInfo->u4LfaNextHop & pLfaRtInfo->u4DestMask);
        if (CfaIpIfGetIfIndexForNetInCxt
            (SR_ZERO, u4IpAddress,
             &SrRtrAltNextHopInfo.u4OutIfIndex) == CFA_FAILURE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d Exiting: Not able to get Out IfIndex \n",
                     __func__, __LINE__);
        }
        u4CfaIfindex = SrRtrAltNextHopInfo.u4OutIfIndex;

        MEMCPY (&SrRtrAltNextHopInfo.nextHop, &pLfaRtInfo->u4LfaNextHop,
                sizeof (UINT4));
        MEMCPY (&SrRtrAltNextHopInfo.PrimarynextHop.Addr.u4Addr,
                &pLfaRtInfo->u4NextHop, MAX_IPV4_ADDR_LEN);
        pRtrInfo->u4LfaNextHop = pLfaRtInfo->u4LfaNextHop;
        pRtrInfo->u1LfaType = SR_LFA_ROUTE;
        /*pSrRtrInfo->u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
        if (CfaIfmCreateStackMplsTunnelInterface
            (SrRtrAltNextHopInfo.u4OutIfIndex,
             &u4MplsTnlIfIndex) == CFA_SUCCESS)
        {
            SR_TRC3 (SR_UTIL_TRC,
                     "%s:%d MPLS tunnel interface creation successful for interface %d: : CFA_SUCCESS\n",
                     __func__, __LINE__, SrRtrAltNextHopInfo.u4OutIfIndex);

            SrRtrAltNextHopInfo.u4OutIfIndex = u4MplsTnlIfIndex;
            pRtrInfo->u4LfaOutIfIndex = u4MplsTnlIfIndex;

        }
        else
        {
            pRtrInfo->u4LfaNextHop = SR_FALSE;
            pRtrInfo->u1LfaType = SR_ZERO;
            pRtrInfo->u4LfaOutIfIndex = SR_ZERO;
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d MPLS tunnel interface creation failed for interface %d: : CFA_FAILURE\n",
                     __func__, __LINE__, SrRtrAltNextHopInfo.u4OutIfIndex);
            return SR_FAILURE;
        }
        if (SrMplsCreateOrDeleteLsp
            (u2MlibOperation, pRtrInfo, NULL,
             &SrRtrAltNextHopInfo) == SR_SUCCESS)
        {
            SR_TRC2 (SR_UTIL_TRC, "%s:%d Creation of LFA LSP successfull\n",
                     __func__, __LINE__);
        }
        else
        {
#ifdef CFA_WANTED
            if (CfaUtilGetIfIndexFromMplsTnlIf
                (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
            {

                if (CfaIfmDeleteStackMplsTunnelInterface
                    (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                {
                    SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d FAILURE: MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                             __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                    return SR_FAILURE;
                }
                SrRtrAltNextHopInfo.u4OutIfIndex = SR_ZERO;
            }

#endif
            pRtrInfo->u4LfaNextHop = SR_FALSE;
            pRtrInfo->u1LfaType = SR_ZERO;
            pRtrInfo->u4LfaOutIfIndex = SR_ZERO;
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Creation of LFA LSP failed\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        SrRtrAltNextHopInfo.u4SwapOutIfIndex = u4CfaIfindex;
        SrRtrAltNextHopInfo.bIsLfaNextHop = SR_TRUE;
        if (SrMplsCreateILM (pRtrInfo, &SrRtrAltNextHopInfo) == SR_SUCCESS)
        {
            SR_TRC2 (SR_UTIL_TRC, "%s:%d Creation of LFA ILM successful\n",
                     __func__, __LINE__);
        }
        else
        {
            pRtrInfo->u1LfaType = SR_ZERO;
            pRtrInfo->u4LfaOutIfIndex = SR_ZERO;
            pRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Creation of LFA ILM failed\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

    }
    /*if LFA path goes down, delete the LFA LSPs */
    if ((pLfaRtInfo->u1CmdType == LFA_ROUTE_DELETE)
        && (u2MlibOperation == MPLS_MLIB_FTN_DELETE))
    {
        if (pRtrInfo != NULL)
        {
            /* SR TE LFA processing with mandatory node as reference in LFA _ROUTE DELETE flow  */
            while ((pSrTeLfaPathInfo =
                    SrGetNextTeRouteTableEntry (&getSrTeRtEntryInfo)) != NULL)
            {
                if (pSrTeLfaPathInfo->mandRtrId.Addr.u4Addr ==
                    pLfaRtInfo->u4DestNet)
                {
                    if (pSrTeLfaPathInfo->bIsSrTeFRRActive == SR_TRUE)
                    {
                        SR_TRC3 (SR_UTIL_TRC,
                                 "%s:%d SR TE LFA ACTIVE , SO LFA lsp Delete SKIPPED for prefix %x \n",
                                 __func__, __LINE__,
                                 pSrTeLfaPathInfo->destAddr.Addr.u4Addr);
                    }
                    else
                    {
                        if (SrTeFRRFtnCreateOrDel (pSrTeLfaPathInfo, SR_FALSE)
                            == SR_FAILURE)
                        {
                            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SR TE LFA NOT-ACTIVE , BUT LFA_DEL FAILED for prefix %x \n",
                                     __func__, __LINE__,
                                     pSrTeLfaPathInfo->destAddr.Addr.u4Addr);
                        }

                    }
                }

                MEMSET (&getSrTeRtEntryInfo, SR_ZERO,
                        sizeof (tSrTeRtEntryInfo));
                getSrTeRtEntryInfo.destAddr.Addr.u4Addr =
                    pSrTeLfaPathInfo->destAddr.Addr.u4Addr;
                getSrTeRtEntryInfo.mandRtrId.Addr.u4Addr =
                    pSrTeLfaPathInfo->mandRtrId.Addr.u4Addr;
                getSrTeRtEntryInfo.destAddr.u2AddrType =
                    pSrTeLfaPathInfo->destAddr.u2AddrType;
                getSrTeRtEntryInfo.mandRtrId.u2AddrType =
                    pSrTeLfaPathInfo->mandRtrId.u2AddrType;
            }
            /* SR TE LFA processing with Destiantion node as reference in LFA _ROUTE DELETE flow  */
            if (pRtrInfo->bIsTeconfigured == SR_TRUE)
            {
                pSrTeDestRtInfo =
                    SrGetTeRouteEntryFromDestAddr (&pRtrInfo->prefixId);
                if (pSrTeDestRtInfo != NULL)
                {
                    if (pRtrInfo->bIsLfaActive == SR_TRUE)
                    {
                        SR_TRC3 (SR_UTIL_TRC,
                                 "%s:%d SR TE LFA ACTIVE , SO LFA lsp Delete SKIPPED for prefix %x \n",
                                 __func__, __LINE__,
                                 pRtrInfo->prefixId.Addr.u4Addr);
                    }
                    else
                    {
                        if (pSrTeDestRtInfo->FrrIlmNexthop.Addr.u4Addr ==
                            pLfaRtInfo->u4LfaNextHop)
                        {
                            if (SrTeFRRIlmAddorDelete
                                (&pRtrInfo->prefixId, pSrTeDestRtInfo,
                                 SR_FALSE) == SR_FAILURE)
                            {
                                SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d LFA ILM Delete via LFA ROUTE DEL for TE configured prefix %x Nexthop %x failed \n",
                                         __func__, __LINE__,
                                         DestAddr.Addr.u4Addr,
                                         pLfaRtInfo->u4NextHop);
                            }
                            SR_TRC4 (SR_UTIL_TRC,
                                     "%s:%d Since TE is configured for the prefix %x Deleted FRR ILM alone with Nexthop %x \n",
                                     __func__, __LINE__, DestAddr.Addr.u4Addr,
                                     pLfaRtInfo->u4NextHop);
                        }
                    }
                }
                return SR_SUCCESS;

            }

            if (pRtrInfo->u4LfaNextHop == SR_FALSE)
            {
                SR_TRC2 (SR_UTIL_TRC,
                         "%s:%d LFA route not present for this destination \n",
                         __func__, __LINE__);
                return SR_SUCCESS;
            }
            else
            {
                SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;

                SrRtrAltNextHopInfo.u4OutIfIndex = pRtrInfo->u4LfaOutIfIndex;
                SrRtrAltNextHopInfo.u4SwapOutIfIndex =
                    pRtrInfo->u4LfaSwapOutIfIndex;
                SrRtrAltNextHopInfo.nextHop.Addr.u4Addr =
                    pRtrInfo->u4LfaNextHop;
                SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
                SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;

                MEMCPY (&SrRtrAltNextHopInfo.PrimarynextHop.Addr.u4Addr,
                        &pLfaRtInfo->u4NextHop, MAX_IPV4_ADDR_LEN);
                SrInSegInfo.u4InLabel = (u4SrGbMinIndex + pRtrInfo->u4SidValue);

                if (SrMplsDeleteILM
                    (&SrInSegInfo, pRtrInfo,
                     &SrRtrAltNextHopInfo) == SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to delete ILM SWAP \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
                if (SrMplsCreateOrDeleteLsp
                    (u2MlibOperation, pRtrInfo, NULL,
                     &SrRtrAltNextHopInfo) == SR_SUCCESS)
                {
                    SR_TRC2 (SR_UTIL_TRC,
                             "%s:%d Deletion of LFA LSP successfull\n",
                             __func__, __LINE__);
                }
                else
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Deletion of LFA LSP failed\n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
                if (CfaUtilGetIfIndexFromMplsTnlIf
                    (SrRtrAltNextHopInfo.u4OutIfIndex, &u4IntIf,
                     TRUE) != CFA_FAILURE)
                {
                    if (CfaIfmDeleteStackMplsTunnelInterface
                        (u4IntIf,
                         SrRtrAltNextHopInfo.u4OutIfIndex) == CFA_FAILURE)
                    {
                        SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                                 "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed\n",
                                 __func__, __LINE__,
                                 SrRtrAltNextHopInfo.u4OutIfIndex, u4IntIf);
                        return SR_FAILURE;
                    }
                }
                SrRtrAltNextHopInfo.u4OutIfIndex = SR_ZERO;
                SrRtrAltNextHopInfo.u4SwapOutIfIndex = SR_ZERO;
                pRtrInfo->u1LfaType = SR_ZERO;
                pRtrInfo->u4LfaOutIfIndex = SR_ZERO;
                pRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
                pRtrInfo->bIsLfaActive = SR_ZERO;
                pRtrInfo->u4LfaNextHop = SR_ZERO;

            }
        }
        else
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                     "%s:%d SrRtrInfor doesnt exist for this prefix\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

    }

    if (u2MlibOperation == MPLS_MLIB_FTN_DELETE)
    {
#ifdef CFA_WANTED

        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {
            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed\n",
                         __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }
        }

        SrRtrAltNextHopInfo.u4OutIfIndex = SR_ZERO;
#endif
        /* Setting MPLS Status to SR_FTN_NOT_CREATED by
         * unsetting the FTN_CREATED Bit*/
        SR_TRC2 (SR_UTIL_TRC,
                 "%s:%d SrRtrAltNextHopInfo.u1MPLSStatus &= ~SR_FTN_CREATED\n",
                 __func__, __LINE__);
        SrRtrAltNextHopInfo.u1MPLSStatus &= (UINT1) (~SR_FTN_CREATED);
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return SR_SUCCESS;
}

#if 1

/*****************************************************************************
 * Function Name : RLfaRtAddOrDeleteEventHandler
 * Description   : This routine is uses the event info and create or delete the
                   LFA route.
 *
 *
 * Input(s)      : pSrIpEvtInfo   - Event Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
RLfaRtAddOrDeleteEventHandler (tNetIpv4LfaRtInfo * pNetIpLfaRtInfo,
                               UINT2 u2MlibOperation)
{
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY\n", __func__, __LINE__);

    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrInSegInfo        SrInSegInfo;
    tSrRtrInfo         *ptmpRtrInfo = NULL;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tGenU4Addr          DestAddr;
    UINT4               u4SrGbMinIndex = SR_ZERO;

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&DestAddr, 0, sizeof (tGenU4Addr));
    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));

    DestAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    MEMCPY (&DestAddr.Addr.u4Addr, &pNetIpLfaRtInfo->u4DestNet, sizeof (UINT4));

    if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
    {
        u4SrGbMinIndex = gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
    }
    /*Create the LFA LSPs on LFA_ADD_ROUTE */
    if (pNetIpLfaRtInfo->u1CmdType == RLFA_ROUTE_ADD)
    {
        /* Is SR-TE Configured Check for the Destination */
        ptmpRtrInfo = SrGetSrRtrInfoFromRtrId (&DestAddr);
        if (ptmpRtrInfo == NULL)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, " %s:%d SrRtrInfo is NULL for %x \n",
                     __func__, __LINE__, DestAddr.Addr.u4Addr);
            return SR_FAILURE;
        }
        if (ptmpRtrInfo->bIsTeconfigured == SR_TRUE)
        {
            SR_TRC3 (SR_UTIL_TRC,
                     " %s:%d Since TE is configured for the prefix %x, no need to create dynamic FRR LSP \n",
                     __func__, __LINE__, DestAddr.Addr.u4Addr);
            return SR_SUCCESS;
        }

        /* if route is added to delay list, LFA info will be updated once
           primary route update done */
        if ((ptmpRtrInfo->u1OprFlags & SR_OPR_F_ADD_DELAY_LIST)
            == SR_OPR_F_ADD_DELAY_LIST)
        {
            SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                "%s:%d Router node %x with prefix %x added to delay list",
                __func__, __LINE__, ptmpRtrInfo->advRtrId.Addr.u4Addr,
                pNetIpLfaRtInfo->u4DestNet);
            return SR_SUCCESS;
        }

        if (SrIpv4RlfaFetchAndCreateLSP (pNetIpLfaRtInfo) == SR_FAILURE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Creation of SR RLFA failed\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        /*Get SrRtrInfo for DestAddr for which ILM is to be created */
        if ((pDestSrRtrInfo = SrGetSrRtrInfoFromRtrId (&DestAddr)) == NULL)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrGetSrRtrInfoFromRtrId FAILURE \n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        if (SrIpv4RlfaILMCreation (pNetIpLfaRtInfo, pDestSrRtrInfo) ==
            SR_FAILURE)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Creation of SR RLFA ILM failed\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

    }

    /*if LFA path goes down, delete the LFA LSPs */
    if ((pNetIpLfaRtInfo->u1CmdType == RLFA_ROUTE_DELETE)
        && (u2MlibOperation == MPLS_MLIB_FTN_DELETE))
    {
        pSrRlfaPathInfo = SrGetRlfaPathEntryFromDestAddr (&DestAddr);
        if (pSrRlfaPathInfo != NULL)
        {
            /* RLFA route is not active, hence delete the entries */
            if (SrRlfaDeleteFTNAndILM (&DestAddr, &pSrRlfaPathInfo->mandRtrId,
                                       &pSrRlfaPathInfo->rlfaNextHop,
                                       &SrInSegInfo,
                                       u4SrGbMinIndex) != SR_SUCCESS)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d Deletion of SR RLFA FTN/ILM Failure \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
        }
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}


#endif

/*****************************************************************************
 * Function Name : SrV3GetPeerRtrInfo
 * Description   : This function will give the SR peer router Info
 *
 *
 * Input(s)      :
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
INT4
SrV3GetPeerRtrInfo (tSrRtrInfo * pSrRtrInfo, tGenU4Addr * prtrId,
                    tNetIpv6LfaRtInfo * pIpv6LfaRtInfo)
{
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tSrRtrInfo         *pTempSrRtrInfo = NULL;

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pTempSrRtrInfo = (tSrRtrInfo *) pRtrNode;

        if (MEMCMP
            (&prtrId->Addr.Ip6Addr, &pTempSrRtrInfo->prefixId.Addr.Ip6Addr,
             SR_IPV6_ADDR_LENGTH) == SR_ZERO)
        {
            MEMCPY (&(pTempSrRtrInfo->LfaNextHop),
                    &(pIpv6LfaRtInfo->LfaNextHop), MAX_IPV6_ADDR_LEN);
            MEMCPY (&(pTempSrRtrInfo->Ip6Dst), &(pIpv6LfaRtInfo->Ip6Dst),
                    MAX_IPV6_ADDR_LEN);
            MEMCPY (pSrRtrInfo, pTempSrRtrInfo, sizeof (tSrRtrInfo));
            SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
            return SR_SUCCESS;
        }
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d FAILED \n", __func__, __LINE__);
    return SR_FAILURE;
}

/*****************************************************************************
 * Function Name : SrV3LfaFetchAndCreateLSP
 * Description   : This routine is uses the event info and create or delete the
                   LFA route.
 *
 *
 * Input(s)      : pSrIpEvtInfo   - Event Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrV3LfaFetchAndCreateLSP (UINT2 u2MlibOperation, tSrRtrInfo * pSrRtrInfo,
                          tSrRtrNextHopInfo * pSrRtrNextHopInfo)
{
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3Ifindex = SR_ZERO;
#ifdef IP6_WANTED
    UINT4               u4Port = SR_ZERO;
#endif
    INT4                i4AltStatus = ALTERNATE_DISABLED;
    tNetIpv6LfaRtInfo   NetIpv6LfaRtInfo;
    tNetIpv6LfaRtInfoQueryMsg NetIpv6LfaRtQuery;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;

    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&NetIpv6LfaRtInfo, SR_ZERO, sizeof (tNetIpv6LfaRtInfo));
    MEMSET (&NetIpv6LfaRtQuery, SR_ZERO, sizeof (tNetIpv6LfaRtInfoQueryMsg));

    SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
    /* Ignore the Notification if SR_V3 LfaStatus is not ENABLED Administratively */
    nmhGetFsSrV6AlternateStatus (&i4AltStatus);

    if (i4AltStatus == ALTERNATE_DISABLED)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Exiting: LFA is not enabled \n", __func__,
                 __LINE__);
        return SR_FAILURE;
    }
    else
    {
        if (pSrRtrInfo != NULL)
        {
            MEMCPY (&(NetIpv6LfaRtQuery.Ip6Dst),
                    &pSrRtrInfo->prefixId.Addr.Ip6Addr, MAX_IPV6_ADDR_LEN);
            MEMCPY (&(NetIpv6LfaRtQuery.PrimaryNextHop),
                    &pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr,
                    MAX_IPV6_ADDR_LEN);
            NetIpv6LfaRtQuery.i1Proto = IP6_OSPF_PROTOID;
            NetIpv6LfaRtQuery.u1Prefixlen = MAX_IPV6_ADDR_LEN;
            NetIpv6LfaRtQuery.u4ContextId = 0;

            if (CfaUtilGetIfIndexFromMplsTnlIf
                (pSrRtrNextHopInfo->u4OutIfIndex, &u4L3Ifindex,
                 TRUE) == CFA_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d Exiting: Not able to get IfIndex from MplsTnlIf\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
#ifdef IP6_WANTED
            if (NetIpv6GetPortFromCfaIfIndex (u4L3Ifindex, &u4Port) ==
                IP_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d Exiting: NetIpv6GetPortFromCfaIfIndex failed\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            NetIpv6LfaRtQuery.u4NxtHopIndex = u4Port;
            if (NetIpv6GetLfaRoute (&NetIpv6LfaRtQuery, &NetIpv6LfaRtInfo) !=
                NETIPV4_SUCCESS)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d Exiting: Not able to fetch SR V3 LFA routes for the destination: %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&NetIpv6LfaRtQuery.Ip6Dst));
                return SR_FAILURE;
            }
#endif
            SR_TRC3 (SR_UTIL_TRC,
                     "%s:%d SR V3 LFA routes fetched successfully for the destination: %s\n",
                     __func__, __LINE__,
                     Ip6PrintAddr (&NetIpv6LfaRtQuery.Ip6Dst));
        }
        else
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Exiting: pSrRtrInfo is NULL\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        if (NetIpv6LfaRtInfo.u1LfaType == SR_LFA)
        {
            SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
            SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;
            SrRtrAltNextHopInfo.u4OutIfIndex = NetIpv6LfaRtInfo.u4Index;

            SR_TRC3 (SR_UTIL_TRC, "%s:%d OutIfIndex is %d\n", __func__,
                     __LINE__, SrRtrAltNextHopInfo.u4OutIfIndex);

            MEMCPY (&SrRtrAltNextHopInfo.nextHop.Addr.Ip6Addr,
                    &NetIpv6LfaRtInfo.LfaNextHop, sizeof (tIp6Addr));

            MEMCPY (&(pSrRtrInfo->u4LfaNextHop), &(NetIpv6LfaRtInfo.LfaNextHop),
                    sizeof (UINT4));
            MEMCPY (&(pSrRtrInfo->u4DestMask), &(NetIpv6LfaRtInfo.u1Prefixlen),
                    sizeof (UINT4));
            MEMCPY (&SrRtrAltNextHopInfo.PrimarynextHop,
                    &pSrRtrNextHopInfo->nextHop, MAX_IPV6_ADDR_LEN);

            /*pSrRtrInfo->u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
            if (CfaIfmCreateStackMplsTunnelInterface
                (SrRtrAltNextHopInfo.u4OutIfIndex,
                 &u4MplsTnlIfIndex) == CFA_SUCCESS)
            {
                SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d SrRtrAltNextHopInfo.u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS\n",
                         __func__, __LINE__, SrRtrAltNextHopInfo.u4OutIfIndex,
                         u4MplsTnlIfIndex);
                SrRtrAltNextHopInfo.u4OutIfIndex = u4MplsTnlIfIndex;
            }
            else
            {
                SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d MPLS tunnel interface creation failed for interface: %d: u4MplsTnlIfIndex %d : CFA_FAILURE\n",
                         __func__, __LINE__, SrRtrAltNextHopInfo.u4OutIfIndex,
                         u4MplsTnlIfIndex);
                return SR_FAILURE;
            }
            if (SrMplsCreateOrDeleteLsp
                (u2MlibOperation, pSrRtrInfo, NULL,
                 &SrRtrAltNextHopInfo) == SR_SUCCESS)
            {
                SR_TRC4 (SR_UTIL_TRC,
                         "%s:%d LFA LSP created successfully for FEC: %s via Next Hop: %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pSrRtrInfo->prefixId.Addr.Ip6Addr),
                         Ip6PrintAddr (&SrRtrAltNextHopInfo.nextHop.Addr.
                                       Ip6Addr));
            }
            else
            {
                SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d Exiting: SR V3 LFA LSP creation failed for FEC: %s via Next Hop: %s\n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pSrRtrInfo->prefixId.Addr.Ip6Addr),
                         Ip6PrintAddr (&SrRtrAltNextHopInfo.nextHop.Addr.
                                       Ip6Addr));
                return SR_FAILURE;
            }
        }
        else if (NetIpv6LfaRtInfo.u1LfaType == SR_RLFA)
        {
            if (SrIpv6RlfaFetchAndCreateLSP (&NetIpv6LfaRtInfo) == SR_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d Creation of SR RLFA FTN failed\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            if (SrIpv6RlfaILMCreation (&NetIpv6LfaRtInfo, pSrRtrInfo) ==
                SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Creation of SR RLFA ILM failed\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
        }

    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrV3LfaRtAddOrDeleteEventHandler
 * Description   : This routine is uses the event info and create or delete the
                   LFA route.
 *
 *
 * Input(s)      : pIpv6LfaRtInfo   - Event Info
                   u2MlibOperation  - FTN_CREATE/FTN_DELETE etc.
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrV3LfaRtAddOrDeleteEventHandler (tNetIpv6LfaRtInfo * pIpv6LfaRtInfo,
                                  UINT2 u2MlibOperation)
{
    tSrRtrInfo          SrRtrInfo;
    tGenU4Addr          rtrId;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;

    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;

    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&SrRtrInfo, SR_ZERO, sizeof (tSrRtrInfo));
    MEMSET (&rtrId, SR_ZERO, sizeof (tGenU4Addr));
    MEMCPY (&rtrId.Addr.Ip6Addr, &pIpv6LfaRtInfo->Ip6Dst, MAX_IPV6_ADDR_LEN);

    SR_TRC3 (SR_UTIL_TRC, "%s:%d Going to get RtrInfo for router id %s\n",
             __func__, __LINE__, Ip6PrintAddr (&rtrId.Addr.Ip6Addr));
    if (SrV3GetPeerRtrInfo (&SrRtrInfo, &rtrId, pIpv6LfaRtInfo) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL \n ", __func__,
                 __LINE__);
        return SR_FAILURE;
    }

    SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
    SrRtrInfo.u2AddrType = SR_IPV6_ADDR_TYPE;

    /*Create the LFA LSPs on LFA_ADD_ROUTE */
    if ((pIpv6LfaRtInfo->u1CmdType == IPv6_LFA_ROUTE_ADD)
        && (u2MlibOperation == MPLS_MLIB_FTN_CREATE))
    {
        SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;

        MEMCPY (&SrRtrAltNextHopInfo.nextHop.Addr.Ip6Addr,
                &pIpv6LfaRtInfo->LfaNextHop, sizeof (tIp6Addr));
        MEMCPY (&SrRtrAltNextHopInfo.PrimarynextHop, &pIpv6LfaRtInfo->NextHop,
                MAX_IPV6_ADDR_LEN);

        /*pSrRtrInfo->u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
        if (CfaIfmCreateStackMplsTunnelInterface
            (pIpv6LfaRtInfo->u4AltIfIndex, &u4MplsTnlIfIndex) == CFA_SUCCESS)
        {
            SR_TRC3 (SR_UTIL_TRC,
                     "%s:%d MPLS tunnel interface creation successful for interface %d: : CFA_SUCCESS\n",
                     __func__, __LINE__, SrRtrAltNextHopInfo.u4OutIfIndex);

            SrRtrAltNextHopInfo.u4OutIfIndex = u4MplsTnlIfIndex;
        }
        else
        {
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                     "%s:%d MPLS tunnel interface creation failed for interface %d: : CFA_FAILURE\n",
                     __func__, __LINE__, SrRtrAltNextHopInfo.u4OutIfIndex);
            return SR_FAILURE;
        }

        if (SrMplsCreateOrDeleteLsp
            (u2MlibOperation, &SrRtrInfo, NULL,
             &SrRtrAltNextHopInfo) == SR_SUCCESS)
        {
            SR_TRC2 (SR_UTIL_TRC, "%s:%d Creation of LFA LSP successfull\n ",
                     __func__, __LINE__);
        }
        else
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Creation of LFA LSP failed\n ",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
    }
    if ((pIpv6LfaRtInfo->u1CmdType == IPv6_LFA_ROUTE_DELETE)
        && (u2MlibOperation == MPLS_MLIB_FTN_DELETE))
    {
        SrRtrAltNextHopInfo.u1FRRNextHop = SR_PRIMARY_NEXTHOP;
        SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;

        MEMCPY (&SrRtrAltNextHopInfo.nextHop.Addr.Ip6Addr,
                &pIpv6LfaRtInfo->LfaNextHop, sizeof (tIp6Addr));
        MEMCPY (&SrRtrAltNextHopInfo.PrimarynextHop, &pIpv6LfaRtInfo->NextHop,
                MAX_IPV6_ADDR_LEN);
        u4MplsTnlIfIndex = SrRtrAltNextHopInfo.u4OutIfIndex;

        if (SrMplsCreateOrDeleteLsp
            (u2MlibOperation, &SrRtrInfo, NULL,
             &SrRtrAltNextHopInfo) == SR_SUCCESS)
        {
            SR_TRC2 (SR_UTIL_TRC,
                     "%s:%d Deletion of SR V3 LFA LSP successfull\n ", __func__,
                     __LINE__);
        }
        else
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Deletion of SR V3 LFA LSP failed\n ",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

    }
    if ((pIpv6LfaRtInfo->u1CmdType == IPv6_LFA_ROUTE_SET)
        && (u2MlibOperation == MPLS_MLIB_FTN_DELETE))
    {
        SrRtrAltNextHopInfo.u1FRRNextHop = SR_PRIMARY_NEXTHOP;
        SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;

        MEMCPY ((&SrRtrAltNextHopInfo.nextHop.Addr.Ip6Addr),
                &pIpv6LfaRtInfo->NextHop, sizeof (tIp6Addr));
        MEMCPY ((&SrRtrAltNextHopInfo.PrimarynextHop), &pIpv6LfaRtInfo->NextHop,
                MAX_IPV6_ADDR_LEN);

        u4MplsTnlIfIndex = SrRtrAltNextHopInfo.u4OutIfIndex;
        if (SrMplsCreateOrDeleteLsp
            (u2MlibOperation, &SrRtrInfo, NULL,
             &SrRtrAltNextHopInfo) == SR_SUCCESS)
        {
            SR_TRC2 (SR_UTIL_TRC,
                     "%s:%d Deletion of primary route successfull\n", __func__,
                     __LINE__);
        }
        else
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Deletion of primary route failed\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

    }
    if (u2MlibOperation == MPLS_MLIB_FTN_DELETE)
    {
#ifdef CFA_WANTED

        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {
            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed\n",
                         __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }
        }

        /*SrRtrAltNextHopInfo.u4OutIfIndex = SR_ZERO; */
#endif
        /* Setting MPLS Status to SR_FTN_NOT_CREATED by
         * unsetting the FTN_CREATED Bit*/
        SR_TRC2 (SR_UTIL_TRC,
                 "%s:%d SrRtrAltNextHopInfo.u1MPLSStatus &= ~SR_FTN_CREATED\n",
                 __func__, __LINE__);
        SrRtrAltNextHopInfo.u1MPLSStatus &= (UINT1) (~SR_FTN_CREATED);

    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrUtilHandleCfaEvent
 * Description   : This function will handle Oper event from CFA
 *
 *
 * Input(s)      :u4IfIndex, u1OperStatus, u1IfType
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
INT4
SrUtilHandleCfaEvent (UINT4 u4IfIndex, UINT1 u1OperStatus, UINT1 u1IfType)
{
    tSrQMsg             SrQMsg;
    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));
    if (u1OperStatus != CFA_IF_UP)
    {
        SR_TRC2 (SR_UTIL_TRC,
                 "%s:%d Interface Down Event is handled through IP route deletion \n",
                 __func__, __LINE__);
        return SR_SUCCESS;
    }

    SrQMsg.u4MsgType = SR_CFA_EVENT;
    SrQMsg.u.SrCfaInfo.i4IfIndex = (INT4) u4IfIndex;
    SrQMsg.u.SrCfaInfo.u1IfType = u1IfType;
    SrQMsg.u.SrCfaInfo.u1IfStatus = u1OperStatus;
    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to EnQ IP Event to SR Task \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrUtilHandleCfaIfStatusChange
 * Description   : This function will handle different type of IfIndex.
 *
 *
 * Input(s)      :u4IfIndex, u1IfStatus
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
INT4
SrUtilHandleCfaIfStatusChange (INT4 i4IfIndex)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tGenU4Addr          tmpAddr;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    UINT4               u4ErrCode = SR_ZERO;
    tRtInfo             InRtInfo;
    tRtInfo            *pRt = NULL;
    tRtInfo            *pTmpRt = NULL;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4L3IfIndex = SR_ZERO;
    UINT4               u4DestAddr = SR_ZERO;
    INT4                i4SrMode = SR_MODE_DEFAULT;
    INT4                i4LfaStatus = ALTERNATE_DISABLED;
    UINT1               u1BestPathMatch = SR_FALSE;
    UINT1               u1IfFound = SR_FALSE;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    MEMSET (&InRtInfo, SR_ZERO, sizeof (tRtInfo));
    MEMSET (&tmpAddr, SR_ZERO, sizeof (tGenU4Addr));
    if (CfaUtilGetL3IfFromMplsIf ((UINT4) i4IfIndex, &u4L3IfIndex, TRUE) !=
        CFA_SUCCESS)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d L3 intf get failed EXIT \n",
                                  __func__, __LINE__);
        return SR_FAILURE;
    }
    nmhGetFsSrMode (&i4SrMode);
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        u1BestPathMatch = SR_FALSE;
        u1IfFound = SR_FALSE;
        u4ErrCode = SR_ZERO;
        pRtrInfo = (tSrRtrInfo *) pRtrNode;

        MEMCPY (&tmpAddr.Addr.u4Addr, &pRtrInfo->prefixId.Addr.u4Addr,
                SR_IPV4_ADDR_LENGTH);
        tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;

        if (SrTeCreateFtnRtChangeEvt (&tmpAddr, &u4ErrCode) == SR_SUCCESS)
        {
            if (u4ErrCode == SR_TE_VIA_DEST)
            {
                SR_TRC3 (SR_UTIL_TRC,
                         "%s:%d SR TE LSP processed via dest in Type10 flow for prefix %x \n",
                         __func__, __LINE__, tmpAddr.Addr.u4Addr);
                continue;
            }
        }
        else
        {
            if (u4ErrCode == SR_TE_VIA_DEST)
            {
                SR_TRC3 (SR_UTIL_TRC,
                         "%s:%d SR TE LSP processing via dest in Type10 flow for prefix %x is failed \n",
                         __func__, __LINE__, tmpAddr.Addr.u4Addr);
                continue;
            }
        }

        /* SR LSP processing for non-TE route */
        u4DestAddr = pRtrInfo->prefixId.Addr.u4Addr;

        InRtInfo.u4DestNet = u4DestAddr;
        InRtInfo.u4DestMask = SR_IPV4_DEST_MASK;
        RtmApiGetBestRouteEntryInCxt (SR_ZERO, InRtInfo, &pRt);
        pTmpRt = pRt;

        if (pTmpRt != NULL)
        {
            SR_TRC4 (SR_UTIL_TRC, "%s:%d ROUTE: NH %x u4OutIfIndex %d \n",
                     __func__, __LINE__, pTmpRt->u4NextHop, pTmpRt->u4RtIfIndx);
        }
#ifdef MPLS_SR_ECMP_WANTED
        for (; ((pTmpRt != NULL) && (pTmpRt->i4Metric1 == pRt->i4Metric1));
             pTmpRt = pTmpRt->pNextAlternatepath)
        {
            if (NetIpv4GetCfaIfIndexFromPort (pTmpRt->u4RtIfIndx,
                                              &u4OutIfIndex) == NETIPV4_FAILURE)
            {
                continue;
            }
            if (u4OutIfIndex == u4L3IfIndex)
            {
                u1BestPathMatch = SR_TRUE;
                break;
            }
        }
#else
       if (pTmpRt != NULL)
       {
           if (NetIpv4GetCfaIfIndexFromPort (pTmpRt->u4RtIfIndx,
                   &u4OutIfIndex) == NETIPV4_FAILURE)
           {
               SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                       "%s:%d No Port is mapped with u4RtIfIndx %d \n", __func__,
                       __LINE__, pTmpRt->u4RtIfIndx);
               return SR_FAILURE;
           }
           if (u4OutIfIndex == u4L3IfIndex)
           {
               u1BestPathMatch = SR_TRUE;
           }
       }
#endif

        if (u1BestPathMatch == SR_FALSE)
        {
            continue;
        }
        else
        {
            if (TMO_SLL_Count (&(pRtrInfo->NextHopList)) == SR_ZERO)
            {
                SR_TRC3 (SR_UTIL_TRC, "%s:%d SID %d \n", __func__, __LINE__,
                         pRtrInfo->u4SidValue);

                if ((pNewSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pNewSrRtrNextHopInfo == NULL \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }

                /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
                pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr = pTmpRt->u4NextHop;
                pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                pNewSrRtrNextHopInfo->u4SwapOutIfIndex = u4OutIfIndex;
                pNewSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
                /* Add NextHop Node in NextHopList in SrRtrNode */
                TMO_SLL_Insert (&(pRtrInfo->NextHopList), NULL,
                                &(pNewSrRtrNextHopInfo->nextNextHop));

                /* Create FTN */
                if (SrMplsCreateFTN (pRtrInfo, pNewSrRtrNextHopInfo) ==
                    SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failure SrMplsCreateFTN \n",
                             __func__, __LINE__);
                    /* Delete the node from the list in case of FTN Creation failure */
                    TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                    (tTMO_SLL_NODE *) pNewSrRtrNextHopInfo);
                    SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                    continue;
                }

                if (SrMplsCreateILM (pRtrInfo, pNewSrRtrNextHopInfo) ==
                    SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateILM  FAILURE \n",
                             __func__, __LINE__);
                    continue;
                }
            }
            else
            {
                /* Case2: Existing FTN/SrNextHopInfo present */

                TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                              pSrRtrNextHopNode, tTMO_SLL_NODE *)
                {
                    pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

                    if (pSrRtrNextHopInfo->u4OutIfIndex == u4L3IfIndex)
                    {
                        if ((pSrRtrNextHopInfo->
                             u1MPLSStatus & SR_ILM_POP_CREATED) ==
                            SR_ILM_POP_CREATED)
                        {
                            if (SrMplsDeleteILMPopAndFwd
                                (pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d SrMplsDeleteILMPopAndFwd FAILURE \n",
                                         __func__, __LINE__);
                                continue;
                            }
                        }
                        if ((pSrRtrNextHopInfo->
                             u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
                        {
                            if (SrMplsCreateFTN (pRtrInfo, pSrRtrNextHopInfo) ==
                                SR_FAILURE)
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d SrMplsCreateFTN  FAILURE \n",
                                         __func__, __LINE__);
                                TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                                (tTMO_SLL_NODE *)
                                                pNewSrRtrNextHopInfo);
                                SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                                continue;
                            }
                            if (SrMplsCreateILM (pRtrInfo, pSrRtrNextHopInfo) ==
                                SR_FAILURE)
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d SrMplsCreateILM  FAILURE \n",
                                         __func__, __LINE__);
                                continue;
                            }
                        }
                        u1IfFound = SR_TRUE;
                        break;
                    }
                }
                if (u1IfFound == SR_FALSE)
                {
                    if ((pNewSrRtrNextHopInfo =
                         SrCreateNewNextHopNode ()) == NULL)
                    {
                        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                                 "%s:%d pNewSrRtrNextHopInfo == NULL \n", __func__,
                                 __LINE__);
                        return SR_FAILURE;
                    }

                    /*Copy data to new next-hop Node & add to SrRtr's NextHop List */
                    pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr =
                        pTmpRt->u4NextHop;
                    pNewSrRtrNextHopInfo->u4OutIfIndex = u4OutIfIndex;
                    pNewSrRtrNextHopInfo->u4SwapOutIfIndex = u4OutIfIndex;
                    pNewSrRtrNextHopInfo->nextHop.u2AddrType =
                        SR_IPV4_ADDR_TYPE;

                    /* Add NextHop Node in NextHopList in SrRtrNode */
                    TMO_SLL_Insert (&(pRtrInfo->NextHopList), NULL,
                                    &(pNewSrRtrNextHopInfo->nextNextHop));

                    /* Create FTN and ILM */
                    if (SrMplsCreateFTN (pRtrInfo, pNewSrRtrNextHopInfo) ==
                        SR_FAILURE)
                    {
                        TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                        (tTMO_SLL_NODE *) pNewSrRtrNextHopInfo);
                        SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failure SrMplsCreateFTN \n",
                                 __func__, __LINE__);
                        continue;
                    }
                    if (SrMplsCreateILM (pRtrInfo, pNewSrRtrNextHopInfo) ==
                        SR_FAILURE)
                    {
                        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateILM  FAILURE \n",
                                 __func__, __LINE__);
                        continue;
                    }
                }
            }
        }
    }
    /* Create LFA/RLFA LSP if present */
    nmhGetFsSrV4AlternateStatus (&i4LfaStatus);
    if (i4LfaStatus == ALTERNATE_ENABLED)
    {
        SrLfaLSPHandle ();
    }
    return SR_SUCCESS;
}

UINT4
SrNbrRtrFromNextHop (UINT4 u4NextHop)
{
    tTMO_SLL_NODE      *pSrV4OspfNbrNode = NULL;
    tSrV4OspfNbrInfo   *pSrV4OspfNbrInfo = NULL;
    UINT4               u4NbrRtrId = SR_ZERO;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    SR_TRC3 (SR_UTIL_TRC, "%s:%d NH = %x \n", __func__, __LINE__, u4NextHop);
    /* Find Neighbor having the Neighbourship link same as NextHop IP */

    TMO_SLL_Scan (&(gSrV4OspfNbrList), pSrV4OspfNbrNode, tTMO_SLL_NODE *)
    {
        pSrV4OspfNbrInfo = (tSrV4OspfNbrInfo *) pSrV4OspfNbrNode;

        SR_TRC3 (SR_UTIL_TRC, "%s:%d u4NbrLinkIpAddr = %x \n", __func__, __LINE__,
                 pSrV4OspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr);
        if (u4NextHop == pSrV4OspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr)
        {
            u4NbrRtrId = pSrV4OspfNbrInfo->ospfV4NbrInfo.u4NbrRtrId;

            SR_TRC3 (SR_UTIL_TRC, "%s:%d Nbr found with RtrId = %x \n", __func__,
                     __LINE__, u4NbrRtrId);
            break;
        }
    }
    return u4NbrRtrId;
}

/*****************************************************************************
 * Function Name : SrIpv4RlfaFetchAndCreateLSP
 * Description   : This routine is uses the event info and create or delete the
                   LFA route.
 *
 *
 * Input(s)      : pSrIpEvtInfo   - Event Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrIpv4RlfaFetchAndCreateLSP (tNetIpv4LfaRtInfo * pNetIpLfaRtInfo)
{
    tGenU4Addr          DestAddr;
    tGenU4Addr          MandatoryRtrId;
    tGenU4Addr          NextHop;
    tSrRtrInfo         *pRtrInfo = NULL;
    UINT4               u4OutIfIndex = SR_ZERO;

    MEMSET (&NextHop, 0, sizeof (tGenU4Addr));
    MEMSET (&DestAddr, 0, sizeof (tGenU4Addr));
    MEMSET (&MandatoryRtrId, 0, sizeof (tGenU4Addr));

    DestAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    DestAddr.Addr.u4Addr = pNetIpLfaRtInfo->u4DestNet;

    pRtrInfo = SrGetSrRtrInfoFromRtrId (&DestAddr);
    if (pRtrInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL\n", __func__, __LINE__);
        return SR_FAILURE;
    }

    NextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
    NextHop.Addr.u4Addr = pNetIpLfaRtInfo->u4LfaNextHop;
    if (CfaIpIfGetIfIndexForNetInCxt
        (SR_ZERO, pNetIpLfaRtInfo->u4LfaNextHop,
         &u4OutIfIndex) == CFA_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Exiting: Not able to get Out IfIndex \n",
                 __func__, __LINE__);
    }
    DestAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    DestAddr.Addr.u4Addr = pNetIpLfaRtInfo->u4DestNet;

    MandatoryRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;
    MandatoryRtrId.Addr.u4Addr = pNetIpLfaRtInfo->uRemoteNodeId.u4RemNodeRouterId;

    if (SrRlfaSetTeTunnel
        (&DestAddr, pNetIpLfaRtInfo->u4DestMask, &MandatoryRtrId, &NextHop,
         u4OutIfIndex) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Creation of SR RLFA failed\n ", __func__,
                 __LINE__);
        return SR_FAILURE;
    }

    return SR_SUCCESS;
}

INT4
SrRlfaSetTeTunnel (tGenU4Addr * pDestAddr, UINT4 u4DestMask,
                   tGenU4Addr * pMandatoryRtrId, tGenU4Addr * prlfaNextHop,
                   UINT4 u4rlfaOutIfIndex)
{
    UINT4               u4ErrCode = SR_ZERO;
    UINT4               u4counter = SR_ZERO;
    UNUSED_PARAM (u4counter);
    tSrRlfaPathInfo    *pSrRlfaDstPathInfo = NULL;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrRlfaPathInfo     SrRlfaPathInfo;

    MEMSET (&SrRlfaPathInfo, SR_ZERO, sizeof (tSrRlfaPathInfo));

    if (pDestAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
    {

        SrRlfaPathInfo.destAddr.Addr.u4Addr = pDestAddr->Addr.u4Addr;
        SrRlfaPathInfo.mandRtrId.Addr.u4Addr = pMandatoryRtrId->Addr.u4Addr;
        SrRlfaPathInfo.mandRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;
        SrRlfaPathInfo.destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;

    }
    else if (pDestAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        MEMCPY (&SrRlfaPathInfo.destAddr.Addr.Ip6Addr,
                &pDestAddr->Addr.Ip6Addr, SR_IPV6_ADDR_LENGTH);
        MEMCPY (&SrRlfaPathInfo.mandRtrId.Addr.Ip6Addr,
                &pMandatoryRtrId->Addr.Ip6Addr, SR_IPV6_ADDR_LENGTH);
        SrRlfaPathInfo.mandRtrId.u2AddrType = SR_IPV6_ADDR_TYPE;
        SrRlfaPathInfo.destAddr.u2AddrType = SR_IPV6_ADDR_TYPE;

    }

    SrRlfaPathInfo.u4DestMask = u4DestMask;
    pSrRlfaPathInfo =
        RBTreeGet (gSrGlobalInfo.pSrRlfaPathRbTree, &SrRlfaPathInfo);
    if (pSrRlfaPathInfo != NULL)
    {
        SR_TRC4 (SR_UTIL_TRC,
                 "%s:%d  Rlfa entry already exists with prefix %x Mandatory-node %x \n",
                 __func__, __LINE__, SrRlfaPathInfo.destAddr.Addr.u4Addr,
                 SrRlfaPathInfo.mandRtrId.Addr.u4Addr);
        return SR_SUCCESS;
    }
    else
    {
        if (SrCreateRlfaPathTableEntry (&SrRlfaPathInfo) == SR_FAILURE)

        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d RLfa route entry addition failed\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

    }
    pSrRlfaDstPathInfo = SrGetRlfaPathEntryFromDestAddr (pDestAddr);

    if ((pSrRlfaDstPathInfo != NULL) && (prlfaNextHop != NULL))
    {
        if (pDestAddr->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            MEMCPY (&(pSrRlfaDstPathInfo->rlfaNextHop.Addr.u4Addr),
                    &(prlfaNextHop->Addr.u4Addr), SR_IPV4_ADDR_LENGTH);
            pSrRlfaDstPathInfo->rlfaNextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        }
        else if (pDestAddr->u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            MEMCPY (&pSrRlfaDstPathInfo->rlfaNextHop.Addr.Ip6Addr,
                    &prlfaNextHop->Addr.Ip6Addr, sizeof (tIp6Addr));
            pSrRlfaDstPathInfo->rlfaNextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
        }
        pSrRlfaDstPathInfo->rlfaOutIfIndex = u4rlfaOutIfIndex;
        pSrRlfaDstPathInfo->u1RowStatus = MPLS_STATUS_ACTIVE;
    }
    else
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Could not assign the RLFA next"
                 "  hop info in Te PATH \n ", __func__, __LINE__);
        if (pSrRlfaDstPathInfo != NULL)
        {
            if (SrDeleteRlfaPathTableEntry (pSrRlfaDstPathInfo) == SR_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d RLfa route RBtree entry deletion failed \n",
                         __func__, __LINE__);
            }
        }
        return SR_FAILURE;
    }
    if (SrRlfaCreateFtnEntry (pDestAddr) == SR_SUCCESS)
    {
        SR_TRC3 (SR_UTIL_TRC, "%s:%d u4ErrCode = %d \n", __func__, __LINE__,
                 u4ErrCode);
        return SR_SUCCESS;
    }
    else
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrRlfaCreateFtnEntry failed \r\n",
                 __func__, __LINE__);
        if (pSrRlfaDstPathInfo != NULL)
        {
            if (SrDeleteRlfaPathTableEntry (pSrRlfaDstPathInfo) == SR_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d RLfa route RBtree entry deletion failed \n",
                         __func__, __LINE__);
            }
        }
        return SR_FAILURE;

    }

    return SR_SUCCESS;
}

/************************************************************************
 *  Function Name   : SrTiLfaPathRbTreeCmpFunc
 *  Description     : RBTree Compare function for Ti Path info for destination
 *  Input           : Two RBTree Nodes to be compared
 *  Output          : None
 *  Returns         : SR_ZERO(FOUND)/SR_ONE(NOT_FOUND)
 ************************************************************************/
PUBLIC INT4
SrTiLfaPathRbTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2)
{
    tSrTilfaPathInfo *srTiLfaPathEntry1 = (tSrTilfaPathInfo *) pRBElem1;
    tSrTilfaPathInfo *srTiLfaPathEntry2 = (tSrTilfaPathInfo *) pRBElem2;

    if (srTiLfaPathEntry1->destAddr.Addr.u4Addr >
        srTiLfaPathEntry2->destAddr.Addr.u4Addr)
    {
        return SR_RB_LESS;
    }
    else if (srTiLfaPathEntry1->destAddr.Addr.u4Addr <
             srTiLfaPathEntry2->destAddr.Addr.u4Addr)
    {
        return SR_RB_GREATER;
    }

    return SR_RB_EQUAL;
}

/************************************************************************
 *  Function Name   : SrLfaNodeTreeCmpFunc
 *  Description     : RBTree Compare function for Router ID
 *  Input           : Two RBTree Nodes to be compared
 *  Output          : None
 *  Returns         : SR_ZERO(FOUND)/SR_ONE(NOT_FOUND)
 ************************************************************************/
PUBLIC INT4
SrLfaNodeTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2)
{
    tSrLfaNodeInfo *srNode1 = (tSrLfaNodeInfo *) pRBElem1;
    tSrLfaNodeInfo *srNode2 = (tSrLfaNodeInfo *) pRBElem2;

    if (srNode1->u4RtrId > srNode2->u4RtrId)
    {
        return SR_RB_LESS;
    }
    else if (srNode1->u4RtrId < srNode2->u4RtrId)
    {
        return SR_RB_GREATER;
    }


    if (srNode1->u4AreaId > srNode2->u4AreaId)
    {
        return SR_RB_LESS;
    }
    else if (srNode1->u4AreaId < srNode2->u4AreaId)
    {
        return SR_RB_GREATER;
    }

    return SR_RB_EQUAL;
}


/************************************************************************
 *  Function Name   : SrLfaAdjTreeCmpFunc
 *  Description     : RBTree Compare function for SR Adj
 *  Input           : Two RBTree Nodes to be compared
 *  Output          : None
 *  Returns         : SR_ZERO(FOUND)/SR_ONE(NOT_FOUND)
 ************************************************************************/
PUBLIC INT4
SrLfaAdjTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2)
{
    tSrLfaAdjInfo *srNode1 = (tSrLfaAdjInfo *) pRBElem1;
    tSrLfaAdjInfo *srNode2 = (tSrLfaAdjInfo *) pRBElem2;

    if (srNode1->AdjIp.Addr.u4Addr > srNode2->AdjIp.Addr.u4Addr)
    {
        return SR_RB_LESS;
    }
    else if (srNode1->AdjIp.Addr.u4Addr < srNode2->AdjIp.Addr.u4Addr)
    {
        return SR_RB_GREATER;
    }

    if (srNode1->u4AreaId > srNode2->u4AreaId)
    {
        return SR_RB_LESS;
    }
    else if (srNode1->u4AreaId < srNode2->u4AreaId)
    {
        return SR_RB_GREATER;
    }

    return SR_RB_EQUAL;
}

/************************************************************************
 *  Function Name   : SrTiLfaFreeAdjSidList
 *  Description     : Function to free Adj Sid Node of the List
 *  Input           : pNode - LFA Adj info
 *  Output          : None
 *  Returns         : None
 ************************************************************************/
VOID
SrTiLfaFreeAdjSidList (tTMO_SLL_NODE * pNode)
{
    tSrTilfaAdjInfo     *pTiLfaAdjInfo  = NULL;

    pTiLfaAdjInfo = (tSrTilfaAdjInfo *) pNode;
    SR_TILFA_ADJ_MEM_FREE(pTiLfaAdjInfo);
    return;
}


/*****************************************************************************/
/* Function Name : SrUpdateAdjLabelforPandQ                                  */
/* Description   : Update Adj Label between P and Q                          */
/* Input(s)      : pTiLfaPathInfo - TI LFA path info                         */
/*                 pSrRtrInfo - Pointer to Router Info                       */
/* Output(s)     : None                                                      */
/* Return(s)     : INT4                                                      */
/*****************************************************************************/
UINT4
SrUpdateAdjLabelforPandQ(tSrTilfaPathInfo    *pTiLfaPathInfo,
                        tSrRtrInfo         *pPSrRtrInfo)
{

    tSrRtrInfo         *pRtrInfo       = NULL;
    tSrTilfaAdjInfo    *pTiLfaAdjInfo  = NULL;
    tAdjSidNode        *pAdjSidInfo    = NULL;
    tTMO_SLL_NODE      *pAdjSid;
    tTMO_SLL_NODE      *pLfaAdjSid;
    tGenU4Addr          GenAddr;
    UINT1               u1AdjFound     = SR_FALSE;

    pRtrInfo = pPSrRtrInfo;

    TMO_SLL_Scan (&(pTiLfaPathInfo->pqAdjSidList), pLfaAdjSid, tTMO_SLL_NODE *)
    {
        pTiLfaAdjInfo = (tSrTilfaAdjInfo *) pLfaAdjSid;

        SR_TRC3 (SR_UTIL_TRC, "%s:%d Adj IP from Path info %x\n",
                __func__, __LINE__, pTiLfaAdjInfo->AdjIp.Addr.u4Addr);

        TMO_SLL_Scan (&(pRtrInfo->adjSidList), pAdjSid, tTMO_SLL_NODE *)
        {
            pAdjSidInfo = (tAdjSidNode *) pAdjSid;
            SR_TRC4 (SR_UTIL_TRC, "%s:%d Adj IP from Rtr node %x info %x\n",
                    __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr,
                    pAdjSidInfo->linkIpAddr.Addr.u4Addr);
            if (pTiLfaAdjInfo->AdjIp.Addr.u4Addr == pAdjSidInfo->linkIpAddr.Addr.u4Addr)
            {
                pTiLfaAdjInfo->u4AdjSid = pAdjSidInfo->u4Label;
                u1AdjFound              = SR_TRUE;
                break;
            }
        }

        if (SR_FALSE == u1AdjFound)
        {
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC , "%s:%d Adj IP from Rtr node %x not found for %x\n",
                    __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr,
                    pTiLfaAdjInfo->AdjIp.Addr.u4Addr);

            return SR_FAILURE;
        }

        if (pAdjSidInfo->u4NbrRtrId == pTiLfaPathInfo->u4QRtrId)
        {
            pTiLfaPathInfo->u1LfaStatusFlags |= SR_TILFA_SET_PQ_ADJ;
            SR_TRC3 (SR_UTIL_TRC, "%s:%d PQ Adj Stack is set for TILFA dest %x\n",
                    __func__, __LINE__, pTiLfaPathInfo->destAddr.Addr.u4Addr);
            return SR_SUCCESS;
        }

        GenAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
        GenAddr.Addr.u4Addr = pAdjSidInfo->u4NbrRtrId;

        /* Get the Adj node SR router info */
        pRtrInfo = SrGetSrRtrInfoFromRtrId (&GenAddr);
        if (pRtrInfo == NULL)
        {
            SR_TRC3 (SR_MAIN_TRC, "%s:%d SrRtrInfo is NULL for Adj Nbr: %x\n",
                    __func__, __LINE__, GenAddr.Addr.u4Addr);
            return SR_FAILURE;
        }
    }

    return SR_SUCCESS;
}

/*****************************************************************************/
/* Function Name : SrDeleteAdjLabelforPandQ                                  */
/* Description   : Delete Adj Label between P and Q                          */
/* Input(s)      : pTiLfaPathInfo - TI LFA path info                         */
/*                 pSrRtrInfo - Pointer to Router Info                       */
/* Output(s)     : None                                                      */
/* Return(s)     : INT4                                                      */
/*****************************************************************************/
UINT4
SrDeleteAdjLabelforPandQ(tSrTilfaPathInfo    *pTiLfaPathInfo,
                        tAdjSidNode   *pAdjSidNode)
{
    tSrTilfaAdjInfo    *pTiLfaAdjInfo  = NULL;
    tTMO_SLL_NODE      *pLfaAdjSid;

    TMO_SLL_Scan (&(pTiLfaPathInfo->pqAdjSidList), pLfaAdjSid, tTMO_SLL_NODE *)
    {
        pTiLfaAdjInfo = (tSrTilfaAdjInfo *) pLfaAdjSid;

        if (pTiLfaAdjInfo->AdjIp.Addr.u4Addr == pAdjSidNode->linkIpAddr.Addr.u4Addr)
        {
            pTiLfaAdjInfo->u4AdjSid = 0;
            pTiLfaPathInfo->u1LfaStatusFlags &= ~SR_TILFA_SET_PQ_ADJ;
            return SR_SUCCESS;
        }
    }

    return SR_FAILURE;
}

/*****************************************************************************
 * Function Name : SrTiLfaPathDelete
 * Description   : This routine is use to cleanup and delete TILfaPath info
 *
 * Input(s)      : pTiLfaPathInfo   - Pointer to TILFA
 *
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID SrTiLfaPathDelete(tSrTilfaPathInfo    *pTiLfaPathInfo)
{

    tSrTilfaAdjInfo     *pTiLfaAdjNode  = NULL;
    tSrTilfaAdjInfo     *pTiLfaAdjNext  = NULL;

    /* Remove the TILFA path information from the NH/P/Q LFA Node Lists */
    SrDelTiLfaPathEntryFromLfaNode(pTiLfaPathInfo);

    /* Remove the TILFA path information from the LFA Adj Lists */
    SrDelTiLfaPathEntryFromLfaAdj(pTiLfaPathInfo);

    /* Remove Ti LFA path entry from Path tree */
    if (RBTreeRem (gSrGlobalInfo.pSrTilfaPathRbTree, pTiLfaPathInfo) == NULL)
    {
       SR_TRC (SR_CRITICAL_TRC | SR_FAIL_TRC,
               "\nDelete Sr-Ti-Path Table : RBTree Node Delete Failed \n");
       return;
    }


    TMO_DYN_SLL_Scan (&(pTiLfaPathInfo->pqAdjSidList), pTiLfaAdjNode, pTiLfaAdjNext,
                       tSrTilfaAdjInfo *)
    {
        TMO_SLL_Delete (&(pTiLfaPathInfo->pqAdjSidList),
                 &(pTiLfaAdjNode->Next));
        SR_TILFA_ADJ_MEM_FREE(pTiLfaAdjNode);
    }

    SR_TILFA_PATH_MEM_FREE (pTiLfaPathInfo);

    return;
}

/*****************************************************************************
 * Function Name : SrTiLfaProcessSrRouterAsLfaNextHop
 * Description   : This routine is use to process SR Node which is TILFA nexthop
 *
 * Input(s)      : pSrNexthopNode   - Nexthop SR Router Info
 *                 pLfaNode         - LFA node info
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrTiLfaProcessSrRouterAsLfaNextHop(tSrRtrInfo    *pSrNexthopNode,
                                tSrLfaNodeInfo   *pLfaNode)
{
    tSrTilfaPathInfo    *pPathInfo     = NULL;
    tSrTilfaPathInfo    *pPathTemp     = NULL;
    tSrRtrInfo          *pPSrRtrInfo   = NULL;
    tGenU4Addr           PRtrId;
    UINT4                u4PnodeLabel  = SR_ZERO;
    UINT4                u4POldLabel   = SR_ZERO;

    MEMSET (&PRtrId, SR_ZERO, sizeof (PRtrId));

    UTL_SLL_OFFSET_SCAN (&(pLfaNode->NexthopLfaList), pPathInfo, pPathTemp, tSrTilfaPathInfo *)
    {
        if (pPathInfo->u4LfaNexthopRtrId != pSrNexthopNode->advRtrId.Addr.u4Addr)
        {
            SR_TRC5 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d TI-LFA Path NH-ID %x is different from LFA Node ID %x for Dest %x\n",
                                __func__, __LINE__, pPathInfo->u4LfaNexthopRtrId,
                                pSrNexthopNode->advRtrId.Addr.u4Addr,
                                pPathInfo->destAddr.Addr.u4Addr);
            continue;
        }

        SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d Added Node %x is Nexthop to TILFA Dest %x with stack flags %x \n",
                __func__, __LINE__, pSrNexthopNode->advRtrId.Addr.u4Addr,
                pPathInfo->destAddr.Addr.u4Addr, pPathInfo->u1LfaStatusFlags);

        u4POldLabel = pPathInfo->u4RemoteLabel;

        /* Get the P node SR router info */
        PRtrId.u2AddrType  = SR_IPV4_ADDR_TYPE;
        PRtrId.Addr.u4Addr = pPathInfo->u4PRtrId;
        pPSrRtrInfo = SrGetSrRtrInfoFromRtrId (&PRtrId);

        if (pPSrRtrInfo == NULL)
        {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d P node %x SrRtrInfo is NULL for Dest %x\n",
                    __func__, __LINE__, PRtrId.Addr.u4Addr, pPathInfo->destAddr);
            continue;
        }

        SrGetLabelFromSIDForRemoteNode(pSrNexthopNode,
                                pPSrRtrInfo->u4SidValue,
                                &u4PnodeLabel);

        if (SR_ZERO != u4PnodeLabel)
        {
            pPathInfo->u4RemoteLabel = u4PnodeLabel;

            SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d TILFA remote label %d (old %d) set for Dest %x\n",
                __func__, __LINE__, pPathInfo->u4RemoteLabel, u4POldLabel,
                pPathInfo->destAddr.Addr.u4Addr);
            if ((u4POldLabel == pPathInfo->u4RemoteLabel)
                && (pPathInfo->u1LfaStatusFlags & SR_TILFA_SET_P_LABEL))
            {
                continue;
            }

            pPathInfo->u1LfaStatusFlags |= SR_TILFA_SET_P_LABEL;
        }
        else
        {
            pPathInfo->u4RemoteLabel = SR_ZERO;
            pPathInfo->u1LfaStatusFlags &= ~SR_TILFA_SET_P_LABEL;
        }

        SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d TILFA label stack flags %x  Mpls status %x for Dest %x\n",
                __func__, __LINE__, pPathInfo->u1LfaStatusFlags,
                pPathInfo->u1MPLSStatus,
                pPathInfo->destAddr.Addr.u4Addr);

        if ((pPathInfo->u1LfaStatusFlags & SR_TILFA_LABEL_STACK_DONE)
                                    == SR_TILFA_LABEL_STACK_DONE)
        {
            pPathInfo->u1LfaStatusFlags |= SR_TILFA_SET_LABEL_STACK;
            if (SR_FAILURE == SrIpv4TiLfaCreateLSP (pPathInfo))
            {
                SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA FTN for Dest %x\n ",
                     __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }
#if 1
            if (SR_FAILURE == SrIpv4TiLfaCreateILM (pPathInfo))
            {
                SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA ILM for Dest %x\n ",
                 __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }
#endif
        }
    }

    return;
}

/*****************************************************************************
 * Function Name : SrTiLfaProcessSrRouterAsLfaPNode
 * Description   : This routine is use to process SR Node which is TILFA P node
 *
 * Input(s)      : pSrPNode   - P Node SR info
 *                 pLfaNode   - LFA node info
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
VOID
SrTiLfaProcessSrRouterAsLfaPNode(tSrRtrInfo    *pSrPNode,
                                tSrLfaNodeInfo *pLfaNode)
{
    tSrTilfaPathInfo    *pPathInfo     = NULL;
    tSrTilfaPathInfo    *pPathTemp     = NULL;
    tSrRtrInfo          *pNexthopInfo  = NULL;
    tGenU4Addr           NexthopRtrId;
    UINT4                u4PnodeLabel  = SR_ZERO;
    UINT4                u4POldLabel   = SR_ZERO;

    MEMSET (&NexthopRtrId, SR_ZERO, sizeof (NexthopRtrId));

    UTL_SLL_OFFSET_SCAN (&(pLfaNode->PLfaList), pPathInfo, pPathTemp, tSrTilfaPathInfo *)
    {
        if (pPathInfo->u4PRtrId != pSrPNode->advRtrId.Addr.u4Addr)
        {
            SR_TRC5 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d TI-LFA Path P-node %x is different from LFA Node ID %x for Dest %x\n",
                                __func__, __LINE__, pPathInfo->u4PRtrId,
                                pSrPNode->advRtrId.Addr.u4Addr,
                                pPathInfo->destAddr.Addr.u4Addr);
            continue;
        }

        SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d Added Node %x is P Node to TILFA Dest %x with stack flags %x\n",
                __func__, __LINE__, pSrPNode->advRtrId.Addr.u4Addr,
                pPathInfo->destAddr.Addr.u4Addr, pPathInfo->u1LfaStatusFlags);

        u4POldLabel = pPathInfo->u4RemoteLabel;

        /* Get the nexthop node SR router info */
        NexthopRtrId.u2AddrType  = SR_IPV4_ADDR_TYPE;
        NexthopRtrId.Addr.u4Addr = pPathInfo->u4LfaNexthopRtrId;
        pNexthopInfo = SrGetSrRtrInfoFromRtrId (&NexthopRtrId);

        if (pNexthopInfo == NULL)
        {
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Nexthop node %x SrRtrInfo is NULL for Dest %x\n",
                    __func__, __LINE__, NexthopRtrId.Addr.u4Addr,
                    pPathInfo->destAddr);
            continue;
        }

        SR_TRC4 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d TILFA Nexthop %x for Dest %x\n",
                __func__, __LINE__, pNexthopInfo->advRtrId.Addr.u4Addr,
                pPathInfo->destAddr.Addr.u4Addr);

        SrGetLabelFromSIDForRemoteNode(pNexthopInfo,
                                pSrPNode->u4SidValue,
                                &u4PnodeLabel);

        if (SR_ZERO != u4PnodeLabel)
        {
            pPathInfo->u4RemoteLabel = u4PnodeLabel;
            SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                    "%s:%d TILFA remote label %d (old %d) set for Dest %x\n",
                    __func__, __LINE__, pPathInfo->u4RemoteLabel, u4POldLabel,
                    pPathInfo->destAddr.Addr.u4Addr);
            if ((u4POldLabel == pPathInfo->u4RemoteLabel)
                && (pPathInfo->u1LfaStatusFlags & SR_TILFA_SET_P_LABEL))
            {
                continue;
            }

            pPathInfo->u1LfaStatusFlags |= SR_TILFA_SET_P_LABEL;
        }
        else
        {
            pPathInfo->u4RemoteLabel = SR_ZERO;
            pPathInfo->u1LfaStatusFlags &= ~SR_TILFA_SET_P_LABEL;
        }

        if (SR_FAILURE == SrUpdateAdjLabelforPandQ(pPathInfo, pSrPNode))
        {
            SR_TRC5 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Ti-LFA Adj Label(s) not derived w.r.to P-Q (%x-%x) node for Dest %x\n ",
                    __func__, __LINE__, pPathInfo->u4PRtrId, pPathInfo->u4QRtrId,
                    pPathInfo->destAddr.Addr.u4Addr);
            continue;
        }

        SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d TILFA label stack flags %x  Mpls status %x for Dest %x\n",
                __func__, __LINE__, pPathInfo->u1LfaStatusFlags,
                pPathInfo->u1MPLSStatus,
                pPathInfo->destAddr.Addr.u4Addr);


        if ((pPathInfo->u1LfaStatusFlags & SR_TILFA_LABEL_STACK_DONE)
                                    == SR_TILFA_LABEL_STACK_DONE)
        {
            pPathInfo->u1LfaStatusFlags |= SR_TILFA_SET_LABEL_STACK;
            if (SR_FAILURE == SrIpv4TiLfaCreateLSP (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA FTN for Dest %x\n ",
                     __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }

#if 1
            if (SR_FAILURE == SrIpv4TiLfaCreateILM (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA ILM for Dest %x\n ",
                 __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }
#endif
        }
    }

    return;
}

/*****************************************************************************
 * Function Name : SrTiLfaProcessSrRouterAsLfaQNode
 * Description   : This routine is use to process SR Node which is TILFA Q node
 *
 * Input(s)      : pSrQNode   - Q Node SR info
 *                 pLfaNode   - LFA node info
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrTiLfaProcessSrRouterAsLfaQNode(tSrRtrInfo    *pSrQNode,
                                tSrLfaNodeInfo *pLfaNode)
{
    tSrTilfaPathInfo    *pPathInfo        = NULL;
    tSrTilfaPathInfo    *pPathTemp        = NULL;
    tSrRtrInfo          *pDestSrRtrInfo   = NULL;
    tGenU4Addr           DestRtrId;
    UINT4                u4DestLabel      = SR_ZERO;
    UINT4                u4OldDestLabel   = SR_ZERO;

    MEMSET (&DestRtrId, SR_ZERO, sizeof (DestRtrId));

    UTL_SLL_OFFSET_SCAN (&(pLfaNode->QLfaList), pPathInfo, pPathTemp, tSrTilfaPathInfo *)
    {

        if (pPathInfo->u4QRtrId != pSrQNode->advRtrId.Addr.u4Addr)
        {
            SR_TRC5 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d TI-LFA Path Q-node %x  is different from LFA Node ID %x for Dest %x\n",
                                __func__, __LINE__, pPathInfo->u4QRtrId,
                                pSrQNode->advRtrId.Addr.u4Addr,
                                pPathInfo->destAddr.Addr.u4Addr);
            continue;
        }

        SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
            "%s:%d Added Node %x is Q Node to TILFA Dest %x with stack flags %x\n",
            __func__, __LINE__, pSrQNode->advRtrId.Addr.u4Addr,
            pPathInfo->destAddr.Addr.u4Addr, pPathInfo->u1LfaStatusFlags);

        u4OldDestLabel = pPathInfo->u4DestLabel;

        /* Get Destination node SR router info */
        DestRtrId.u2AddrType  = SR_IPV4_ADDR_TYPE;
        DestRtrId.Addr.u4Addr = pPathInfo->destAddr.Addr.u4Addr;
        pDestSrRtrInfo = SrGetSrRtrInfoFromNodeId (&DestRtrId);

        if (pDestSrRtrInfo == NULL)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo Node is NULL for Dest %x\n",
                    __func__, __LINE__, DestRtrId.Addr.u4Addr);
            continue;
        }

        SrGetLabelFromSIDForRemoteNode(pSrQNode,
                                pDestSrRtrInfo->u4SidValue,
                                &u4DestLabel);
        if (SR_ZERO != u4DestLabel)
        {
            pPathInfo->u4DestLabel = u4DestLabel;
            SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d TILFA Dest label %d (old %d) set for Dest %x\n",
                __func__, __LINE__, pPathInfo->u4DestLabel, u4OldDestLabel,
                pPathInfo->destAddr.Addr.u4Addr);
            if ((u4OldDestLabel == pPathInfo->u4DestLabel)
                && (pPathInfo->u1LfaStatusFlags & SR_TILFA_SET_DEST_LABEL))
            {
                continue;
            }
            pPathInfo->u1LfaStatusFlags |= SR_TILFA_SET_DEST_LABEL;
        }
        else
        {
            pPathInfo->u4DestLabel = SR_ZERO;
            pPathInfo->u1LfaStatusFlags &= ~SR_TILFA_SET_DEST_LABEL;
        }

        SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d TILFA label stack flags %x  Mpls status %x for Dest %x\n",
                __func__, __LINE__, pPathInfo->u1LfaStatusFlags,
                pPathInfo->u1MPLSStatus,
                pPathInfo->destAddr.Addr.u4Addr);


        if ((pPathInfo->u1LfaStatusFlags & SR_TILFA_LABEL_STACK_DONE)
                                    == SR_TILFA_LABEL_STACK_DONE)
        {
            pPathInfo->u1LfaStatusFlags |= SR_TILFA_SET_LABEL_STACK;
            if (SR_FAILURE == SrIpv4TiLfaCreateLSP (pPathInfo))
            {
                SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA FTN for Dest %x\n ",
                     __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }

#if 1
            if (SR_FAILURE == SrIpv4TiLfaCreateILM (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA ILM for Dest %x\n ",
                 __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }
#endif

        }
    }

    return;
}

/*****************************************************************************
 * Function Name : SrTiLfaDeleteSrRouterAsLfaNextHop
 * Description   : This routine is use to Process TILFA stack update due to
 *                 SR router removed as LFA nexthop
 *
 * Input(s)      : pSrNexthopNode   - Nexthop SR info
 *                 pLfaNode         - LFA node
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrTiLfaDeleteSrRouterAsLfaNextHop(tSrRtrInfo    *pSrNexthopNode,
                                tSrLfaNodeInfo   *pLfaNode)
{
    tSrTilfaPathInfo    *pPathInfo     = NULL;
    tSrTilfaPathInfo    *pPathTemp     = NULL;
    tGenU4Addr           PRtrId;

    MEMSET (&PRtrId, SR_ZERO, sizeof (PRtrId));

    UTL_SLL_OFFSET_SCAN (&(pLfaNode->NexthopLfaList), pPathInfo, pPathTemp, tSrTilfaPathInfo *)
    {
        if (pPathInfo->u4LfaNexthopRtrId != pSrNexthopNode->advRtrId.Addr.u4Addr)
        {
            SR_TRC5 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d TI-LFA Path NH-ID %x is different from LFA Node ID %x for Dest %x\n",
                                __func__, __LINE__, pPathInfo->u4LfaNexthopRtrId,
                                pSrNexthopNode->advRtrId.Addr.u4Addr,
                                pPathInfo->destAddr.Addr.u4Addr);
            continue;
        }

        SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d Deleted Node %x as Nexthop to TILFA Dest %x with stack flags %x\n",
                __func__, __LINE__, pSrNexthopNode->advRtrId.Addr.u4Addr,
                pPathInfo->destAddr.Addr.u4Addr, pPathInfo->u1LfaStatusFlags);

        pPathInfo->u4RemoteLabel = 0;
        pPathInfo->u1LfaStatusFlags &= ~SR_TILFA_SET_P_LABEL;

        SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d TILFA label stack flags %x  Mpls status %x for Dest %x\n",
                __func__, __LINE__, pPathInfo->u1LfaStatusFlags,
                pPathInfo->u1MPLSStatus,
                pPathInfo->destAddr.Addr.u4Addr);

        if (pPathInfo->u1MPLSStatus ==  SR_FTN_CREATED)
        {
            pPathInfo->u1LfaStatusFlags &= ~SR_TILFA_SET_LABEL_STACK;
            if (SR_FAILURE == SrIpv4TiLfaDeleteLSP (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA FTN for Dest %x\n ",
                     __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }

#if 1
            if (SR_FAILURE == SrIpv4TiLfaDeleteILM (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA ILM for Dest %x\n ",
                 __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }
#endif
        }
    }

    return;
}

/*****************************************************************************
 * Function Name : SrTiLfaDeleteSrRouterAsLfaPNode
 * Description   : This routine is use to Process TILFA stack update due to
 *                 SR router removed as LFA P node
 *
 * Input(s)      : pSrPNode   - P node SR info
 *                 pLfaNode   - LFA node
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrTiLfaDeleteSrRouterAsLfaPNode(tSrRtrInfo    *pSrPNode,
                                tSrLfaNodeInfo *pLfaNode)
{
    tSrTilfaPathInfo    *pPathInfo     = NULL;
    tSrTilfaPathInfo    *pPathTemp     = NULL;

    UTL_SLL_OFFSET_SCAN (&(pLfaNode->PLfaList), pPathInfo, pPathTemp, tSrTilfaPathInfo *)
    {
        if (pPathInfo->u4PRtrId != pSrPNode->advRtrId.Addr.u4Addr)
        {
            SR_TRC5 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d TI-LFA Path P-node %x is different from LFA Node ID %x for Dest %x\n",
                                __func__, __LINE__, pPathInfo->u4PRtrId,
                                pSrPNode->advRtrId.Addr.u4Addr,
                                pPathInfo->destAddr.Addr.u4Addr);
            continue;
        }

        SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d Deleted Node %x as P node to TILFA Dest %x with stack flags %x \n",
                __func__, __LINE__, pSrPNode->advRtrId.Addr.u4Addr,
                pPathInfo->destAddr.Addr.u4Addr,
                pPathInfo->u1LfaStatusFlags);

        pPathInfo->u4RemoteLabel = 0;
        pPathInfo->u1LfaStatusFlags &= ~SR_TILFA_SET_P_LABEL;

        SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d TILFA label stack flags %x  Mpls status %x for Dest %x\n",
                __func__, __LINE__, pPathInfo->u1LfaStatusFlags,
                pPathInfo->u1MPLSStatus,
                pPathInfo->destAddr.Addr.u4Addr);

        if (pPathInfo->u1MPLSStatus ==  SR_FTN_CREATED)
        {
            pPathInfo->u1LfaStatusFlags &= ~SR_TILFA_SET_LABEL_STACK;
            if (SR_FAILURE == SrIpv4TiLfaDeleteLSP (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA FTN for Dest %x\n ",
                     __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }

#if 1
            if (SR_FAILURE == SrIpv4TiLfaDeleteILM (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA ILM for Dest %x\n ",
                 __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }
#endif

        }
    }

    return;
}

/*****************************************************************************
 * Function Name : SrTiLfaDeleteSrRouterAsLfaQNode
 * Description   : This routine is use to Process TILFA stack update due to
 *                 SR router removed as LFA Q node
 *
 * Input(s)      : pSrQNode   - SR info of Q node
 *                 pLfaNode   - LFA node
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrTiLfaDeleteSrRouterAsLfaQNode(tSrRtrInfo    *pSrQNode,
                                tSrLfaNodeInfo *pLfaNode)
{
    tSrTilfaPathInfo    *pPathInfo        = NULL;
    tSrTilfaPathInfo    *pPathTemp        = NULL;
    tGenU4Addr           DestRtrId;

    MEMSET (&DestRtrId, SR_ZERO, sizeof (DestRtrId));

    UTL_SLL_OFFSET_SCAN (&(pLfaNode->QLfaList), pPathInfo, pPathTemp, tSrTilfaPathInfo *)
    {

        if (pPathInfo->u4QRtrId != pSrQNode->advRtrId.Addr.u4Addr)
        {
            SR_TRC5 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d TI-LFA Path Q-node %x  is different from LFA Node ID %x for Dest %x\n",
                                __func__, __LINE__, pPathInfo->u4QRtrId,
                                pSrQNode->advRtrId.Addr.u4Addr,
                                pPathInfo->destAddr.Addr.u4Addr);
            continue;
        }

        SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d Deleted Node %x as Q node to TILFA Dest %x with stack flags %x \n",
                __func__, __LINE__, pSrQNode->advRtrId.Addr.u4Addr,
                pPathInfo->destAddr.Addr.u4Addr,
                pPathInfo->u1LfaStatusFlags);

        pPathInfo->u4DestLabel = 0;
        pPathInfo->u1LfaStatusFlags &= ~SR_TILFA_SET_DEST_LABEL;

        SR_TRC5 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d TILFA label stack flags %x  Mpls status %x for Dest %x\n",
                __func__, __LINE__, pPathInfo->u1LfaStatusFlags,
                pPathInfo->u1MPLSStatus,
                pPathInfo->destAddr.Addr.u4Addr);

        if (pPathInfo->u1MPLSStatus ==  SR_FTN_CREATED)
        {
            pPathInfo->u1LfaStatusFlags &= ~SR_TILFA_SET_LABEL_STACK;
            if (SR_FAILURE == SrIpv4TiLfaDeleteLSP (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA FTN for Dest %x\n ",
                     __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }

#if 1
            if (SR_FAILURE == SrIpv4TiLfaDeleteILM (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA ILM for Dest %x\n ",
                 __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }
#endif

        }
    }

    return;
}

/*****************************************************************************
 * Function Name : SrTiLfaProcessAdjInfoUpdate
 * Description   : This routine is use to Process Adj Info Update for Router node
 *
 * Input(s)      : pSrRouterNode   - SR info of the node which has Adj Change
 *                 pLfaAdj         - LFa Adj info
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrTiLfaProcessAdjInfoUpdate(tSrRtrInfo    *pSrRouterNode,
                            tSrLfaAdjInfo *pLfaAdj)
{
    tSrTilfaPathInfo    *pPathInfo     = NULL;
    tSrRtrInfo          *pPNode        = NULL;
    tTMO_SLL_NODE       *pLfaAdjSid    = NULL;
    tSrAdjLfaListNode  *pLfaAdjNode    = NULL;
    tGenU4Addr           PRtrId;

    MEMSET (&PRtrId, SR_ZERO, sizeof (tGenU4Addr));

    TMO_SLL_Scan (&(pLfaAdj->LfaList), pLfaAdjSid, tTMO_SLL_NODE *)
    {
        pLfaAdjNode = (tSrAdjLfaListNode *) pLfaAdjSid;
        pPathInfo = pLfaAdjNode->pLfaPathInfo;

        if (NULL == pPathInfo)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                    "%s:%d PathInfo is NULL for Adj  %x\n",
                    __func__, __LINE__, pLfaAdj->AdjIp.Addr.u4Addr);
            continue;
        }

        if (pPathInfo->u4PRtrId == pSrRouterNode->advRtrId.Addr.u4Addr)
        {
            SR_TRC5 (SR_UTIL_TRC, "%s:%d TI-LFA Path P-node %x is same as Node ID %x for Dest %x\n",
                                __func__, __LINE__, pPathInfo->u4PRtrId,
                                pSrRouterNode->advRtrId.Addr.u4Addr,
                                pPathInfo->destAddr.Addr.u4Addr);

            pPNode = pSrRouterNode;
        }
        else
        {
            SR_TRC5 (SR_UTIL_TRC, "%s:%d TI-LFA Path P-node %x is different from LFA Node ID %x for Dest %x\n",
                                       __func__, __LINE__, pPathInfo->u4PRtrId,
                                       pSrRouterNode->advRtrId.Addr.u4Addr,
                                       pPathInfo->destAddr.Addr.u4Addr);
            /* Get the P node SR router info */
            PRtrId.u2AddrType  = SR_IPV4_ADDR_TYPE;
            PRtrId.Addr.u4Addr = pPathInfo->u4PRtrId;
            pPNode = SrGetSrRtrInfoFromRtrId (&PRtrId);

            if (pPNode == NULL)
            {
                SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d P node %x SrRtrInfo is NULL for Dest %x\n",
                        __func__, __LINE__, PRtrId.Addr.u4Addr,
                        pPathInfo->destAddr);
                continue;
            }
        }

        if (SR_FAILURE == SrUpdateAdjLabelforPandQ(pPathInfo, pPNode))
        {
            SR_TRC5 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Ti-LFA Adj Label(s) not derived w.r.to P-Q (%x-%x) node for Dest %x\n ",
                    __func__, __LINE__, pPathInfo->u4PRtrId, pPathInfo->u4QRtrId,
                    pPathInfo->destAddr.Addr.u4Addr);
            continue;
        }

        if ((pPathInfo->u1LfaStatusFlags & SR_TILFA_LABEL_STACK_DONE)
                                    == SR_TILFA_LABEL_STACK_DONE)
        {
            pPathInfo->u1LfaStatusFlags |= SR_TILFA_SET_LABEL_STACK;
            if (SR_FAILURE == SrIpv4TiLfaCreateLSP (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA FTN for Dest %x\n ",
                     __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }

#if 1
            if (SR_FAILURE == SrIpv4TiLfaCreateILM (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA ILM for Dest %x\n ",
                 __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }
#endif

        }
    }

    return;
}

/*****************************************************************************
 * Function Name : SrTiLfaProcessAdjInfoDelete
 * Description   : This routine is use to Process Adj Info Deletion for TILFA
 *
 * Input(s)      : pAdjSidNode     - Adj SID node
 *                 pLfaAdj         - LFa Adj info
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrTiLfaProcessAdjInfoDelete(tAdjSidNode   *pAdjSidNode,
                            tSrLfaAdjInfo *pLfaAdj)
{
    tSrTilfaPathInfo    *pPathInfo     = NULL;
    tTMO_SLL_NODE       *pLfaAdjSid    = NULL;
    tSrAdjLfaListNode  *pLfaAdjNode    = NULL;
    tGenU4Addr           PRtrId;

    MEMSET (&PRtrId, SR_ZERO, sizeof (tGenU4Addr));

    TMO_SLL_Scan (&(pLfaAdj->LfaList), pLfaAdjSid, tTMO_SLL_NODE *)
    {
        pLfaAdjNode = (tSrAdjLfaListNode *) pLfaAdjSid;
        pPathInfo = pLfaAdjNode->pLfaPathInfo;

        if (NULL == pPathInfo)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                    "%s:%d PathInfo is NULL for Adj  %x\n",
                    __func__, __LINE__, pLfaAdj->AdjIp.Addr.u4Addr);
            continue;
        }

        if (SR_FAILURE == SrDeleteAdjLabelforPandQ(pPathInfo, pAdjSidNode))
        {
            SR_TRC5 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Ti-LFA Adj Label(s) not derived w.r.to P-Q (%x-%x) node for Dest %x\n ",
                    __func__, __LINE__, pPathInfo->u4PRtrId, pPathInfo->u4QRtrId,
                    pPathInfo->destAddr.Addr.u4Addr);
            continue;
        }

        if (pPathInfo->u1MPLSStatus ==  SR_FTN_CREATED)
        {
            pPathInfo->u1LfaStatusFlags &= ~SR_TILFA_SET_LABEL_STACK;
            if (SR_FAILURE == SrIpv4TiLfaDeleteLSP (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA FTN for Dest %x\n ",
                     __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }

#if 1
            if (SR_FAILURE == SrIpv4TiLfaDeleteILM (pPathInfo))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to create TILFA ILM for Dest %x\n ",
                 __func__, __LINE__, pPathInfo->destAddr);
                continue;
            }
#endif

        }
    }

    return;
}

/*****************************************************************************
 * Function Name : SrTiLfaProcessForSrRouterNodeDelete
 * Description   : This routine is use to Process SR Router Node Deletion for TILFA
 *
 * Input(s)      : pSrRtrNode     - SR router Node
 *
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrTiLfaProcessForSrRouterNodeDelete(tSrRtrInfo    *pSrRtrNode)
{

    tSrLfaNodeInfo       LfaTempNode;
    tSrTilfaPathInfo     SrTilfaPathInfo;
    tSrLfaNodeInfo      *pLfaNode         = NULL;
    tSrTilfaPathInfo    *pSrTilfaPathInfo = NULL;

    SR_TRC2 (SR_MAIN_TRC | SR_UTIL_TRC, "%s:%d ENTRY\n", __func__, __LINE__);

    MEMSET (&LfaTempNode, SR_ZERO, sizeof (tSrLfaNodeInfo));
    MEMSET (&SrTilfaPathInfo, SR_ZERO, sizeof (tSrTilfaPathInfo));

    SrTilfaPathInfo.destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    MEMCPY (&SrTilfaPathInfo.destAddr.Addr.u4Addr,
            &pSrRtrNode->prefixId.Addr.u4Addr, SR_IPV4_ADDR_LENGTH);

    pSrTilfaPathInfo = RBTreeGet (gSrGlobalInfo.pSrTilfaPathRbTree,
                                &SrTilfaPathInfo);

    /* Update the TILFA info for the destinaitons which has the current node
       as either TILFA Nexthop, P Node or Q Node */
    LfaTempNode.u4RtrId = pSrRtrNode->advRtrId.Addr.u4Addr;

    pLfaNode = RBTreeGet (gSrGlobalInfo.pSrLfaNodeRbTree, &LfaTempNode);
    if (NULL == pLfaNode)
    {
        SR_TRC2 (SR_MAIN_TRC | SR_UTIL_TRC, "%s:%d EXIT\n", __func__, __LINE__);
        return;
    }

    SR_TRC4 (SR_MAIN_TRC | SR_UTIL_TRC,
            "%s:%d Deleted Node %x Lfa Node %x\n", __func__, __LINE__,
            LfaTempNode.u4RtrId, pLfaNode->u4RtrId);

    /* List of TILFA Destination for which the node act as nexthop */
    if (TMO_SLL_Count (&(pLfaNode->NexthopLfaList)) != SR_ZERO)
    {
        SR_TRC3 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d Deleted Node %x as Nexthop\n", __func__, __LINE__, LfaTempNode.u4RtrId);
        SrTiLfaDeleteSrRouterAsLfaNextHop(pSrRtrNode, pLfaNode);
    }

    /* List of TILFA Destination for which the node act as P node */
    if (TMO_SLL_Count (&(pLfaNode->PLfaList)) != SR_ZERO)
    {
        SR_TRC3 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d Deleted Node %x as P node \n", __func__, __LINE__, LfaTempNode.u4RtrId);

        SrTiLfaDeleteSrRouterAsLfaPNode(pSrRtrNode, pLfaNode);
    }

    /* List of TILFA Destination for which the node act as Q node */
    if (TMO_SLL_Count (&(pLfaNode->QLfaList)) != SR_ZERO)
    {
        SR_TRC3 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d Deleted Node %x as Q node \n", __func__, __LINE__, LfaTempNode.u4RtrId);

        SrTiLfaDeleteSrRouterAsLfaQNode(pSrRtrNode, pLfaNode);
    }

    if (NULL != pSrTilfaPathInfo)
    {
        TiLfaHandleIpv4RouteDel(pSrTilfaPathInfo);
        SR_TRC3 (SR_MAIN_TRC | SR_CRITICAL_TRC,
                "%s:%d Deleted TI LFA entry for dest %x\n", __func__, __LINE__,
                pSrRtrNode->prefixId.Addr.u4Addr);
    }

    SR_TRC2 (SR_MAIN_TRC | SR_UTIL_TRC, "%s:%d EXIT\n", __func__, __LINE__);

    return;
}

/*****************************************************************************
 * Function Name : SrTiLfaProcessForSrRouterNodeChange
 * Description   : This routine is use to Process SR Router Node Change for TILFA
 *
 * Input(s)      : pSrRtrNode     - SR Router node
 *
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrTiLfaProcessForSrRouterNodeChange(tSrRtrInfo    *pSrRtrNode)
{

    tSrLfaNodeInfo       LfaTempNode;
    tSrLfaNodeInfo      *pLfaNode         = NULL;

    SR_TRC2 (SR_MAIN_TRC, "%s:%d ENTRY\n", __func__, __LINE__);

    MEMSET (&LfaTempNode, SR_ZERO, sizeof (tSrLfaNodeInfo));

    /* Update the TILFA info for the destinaitons which has the current node
       as either TILFA Nexthop, P Node or Q Node */
    LfaTempNode.u4RtrId = pSrRtrNode->advRtrId.Addr.u4Addr;

    pLfaNode = RBTreeGet (gSrGlobalInfo.pSrLfaNodeRbTree, &LfaTempNode);
    if (NULL == pLfaNode)
    {
        SR_TRC2 (SR_MAIN_TRC, "%s:%d EXIT\n", __func__, __LINE__);
        return;
    }

    SR_TRC4 (SR_MAIN_TRC | SR_UTIL_TRC,
            "%s:%d Added Node %x Lfa Node %x\n", __func__, __LINE__,
            LfaTempNode.u4RtrId, pLfaNode->u4RtrId);

    /* List of TILFA Destination for which the node act as nexthop */
    if (TMO_SLL_Count (&(pLfaNode->NexthopLfaList)) != SR_ZERO)
    {
        SR_TRC3 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d Added Node %x as Nexthop\n", __func__, __LINE__, LfaTempNode.u4RtrId);
        SrTiLfaProcessSrRouterAsLfaNextHop(pSrRtrNode, pLfaNode);
    }

    /* List of TILFA Destination for which the node act as P node */
    if (TMO_SLL_Count (&(pLfaNode->PLfaList)) != SR_ZERO)
    {
        SR_TRC3 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d Added Node %x as P Node\n", __func__, __LINE__, LfaTempNode.u4RtrId);

        SrTiLfaProcessSrRouterAsLfaPNode(pSrRtrNode, pLfaNode);
    }

    /* List of TILFA Destination for which the node act as Q node */
    if (TMO_SLL_Count (&(pLfaNode->QLfaList)) != SR_ZERO)
    {
        SR_TRC3 (SR_MAIN_TRC | SR_UTIL_TRC,
                "%s:%d Added Node %x as Q Node\n", __func__, __LINE__, LfaTempNode.u4RtrId);
        SrTiLfaProcessSrRouterAsLfaQNode(pSrRtrNode, pLfaNode);
    }

    SR_TRC2 (SR_MAIN_TRC, "%s:%d EXIT\n", __func__, __LINE__);

    return;
}

/*****************************************************************************
 * Function Name : SrTiLfaProcessForSrRouterAdjDel
 * Description   : This routine is use to Process SR Router for Adj Delete in TILFA
 *
 * Input(s)      : pAdjSidNode    - Adj SID node
                   u4AreaId       - AreaId
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrTiLfaProcessForSrRouterAdjDel(tAdjSidNode   *pAdjSidNode, UINT4 u4AreaId)
{
    tSrLfaAdjInfo       LfaTempAdj;
    tSrLfaAdjInfo      *pLfaAdj        = NULL;

    MEMSET (&LfaTempAdj, SR_ZERO, sizeof (tSrLfaAdjInfo));

    /* Update the TILFA info for which this Adj SID is part of PaQ */
    LfaTempAdj.AdjIp.Addr.u4Addr = pAdjSidNode->linkIpAddr.Addr.u4Addr;
    LfaTempAdj.u4AreaId          = u4AreaId;

    pLfaAdj = RBTreeGet (gSrGlobalInfo.pSrLfaAdjRbTree, &LfaTempAdj);
    if (NULL == pLfaAdj)
    {
        SR_TRC2 (SR_MAIN_TRC, "%s:%d EXIT\n", __func__, __LINE__);
        return;
    }

    /* List of TILFA Destination for which the node act as nexthop */
    if (TMO_SLL_Count (&(pLfaAdj->LfaList)) != SR_ZERO)
    {
        SrTiLfaProcessAdjInfoDelete(pAdjSidNode, pLfaAdj);
    }

    return;

}

/*****************************************************************************
 * Function Name : SrTiLfaProcessForSrRouterAdjChange
 * Description   : This routine is use to Process SR Router for Adj Change in TILFA
 *
 * Input(s)      : pSrRtrNode     - SR router Node
 *                 pAdjSidNode    - Adj SID node
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrTiLfaProcessForSrRouterAdjChange(tSrRtrInfo    *pSrRtrNode,
                                tAdjSidNode        *pAdjSidNode)
{
    tSrLfaAdjInfo       LfaTempAdj;
    tSrLfaAdjInfo      *pLfaAdj        = NULL;

    MEMSET (&LfaTempAdj, SR_ZERO, sizeof (tSrLfaAdjInfo));

    /* Update the TILFA info for which this Adj SID is part of PaQ */
    LfaTempAdj.AdjIp.Addr.u4Addr = pAdjSidNode->linkIpAddr.Addr.u4Addr;
    LfaTempAdj.u4AreaId          = pSrRtrNode->u4areaId;

    pLfaAdj = RBTreeGet (gSrGlobalInfo.pSrLfaAdjRbTree, &LfaTempAdj);
    if (NULL == pLfaAdj)
    {
        SR_TRC2 (SR_MAIN_TRC, "%s:%d EXIT\n", __func__, __LINE__);
        return;
    }

    /* List of TILFA Destination for which the node act as nexthop */
    if (TMO_SLL_Count (&(pLfaAdj->LfaList)) != SR_ZERO)
    {
        SrTiLfaProcessAdjInfoUpdate(pSrRtrNode, pLfaAdj);
    }

    return;

}


/*****************************************************************************/
/* Function Name : SrTiLfaRtChangeEventHandler                               */
/* Description   : This routine is called by SR module, for informing the
 *                 TILFA Route changes.                                      */
/* Input(s)      : tNetIpv4RtInfo - Route info  as given by RTM              */
/*                 u1CmdType - Command to ADD | Delete | Modify the Route.   */
/* Output(s)     : None                                                      */
/* Return(s)     : None                                                      */
/*****************************************************************************/
VOID
SrTiLfaRtChangeEventHandler (tNetIpv4LfaRtInfo * pLfaRtInfo)
{
    UINT2               u2MlibOperation = SR_ZERO;
    SR_TRC2 (SR_MAIN_TRC, "%s:%d ENTRY\n", __func__, __LINE__);
    if (pLfaRtInfo == NULL)
    {
        SR_TRC2 (SR_MAIN_TRC, "%s:%d FAILING pLfaRtInfo is NULL \n ", __func__,
                 __LINE__);
        return;
    }
    RTM_PROT_UNLOCK ();
    if (pLfaRtInfo->u1LfaType == SR_TILFA)
    {
        switch (pLfaRtInfo->u1CmdType)
        {
            case TILFA_ROUTE_ADD:
                SR_TRC2 (SR_MAIN_TRC, "%s:%d TILFA_ROUTE_ADD event received\n ",
                         __func__, __LINE__);
                SR_TRC3 (SR_MAIN_TRC, "%s:%d Destination Address is %x\n ",
                         __func__, __LINE__, pLfaRtInfo->u4DestNet);
                SR_TRC3 (SR_MAIN_TRC, "%s:%d Primary Next Hop is %x\n ",
                         __func__, __LINE__, pLfaRtInfo->u4NextHop);
                SR_TRC3 (SR_MAIN_TRC, "%s:%d Primary Next Hop If Index %x\n ", __func__,
                         __LINE__, pLfaRtInfo->u4NextHopIfIndx);
                SR_TRC3 (SR_MAIN_TRC, "%s:%d TILFA Next Hop is %x\n ", __func__,
                         __LINE__, pLfaRtInfo->u4LfaNextHop);
                SR_TRC3 (SR_MAIN_TRC, "%s:%d TILFA Next Hop If Index %x\n ", __func__,
                         __LINE__, pLfaRtInfo->u4AltIfIndx);
                SR_TRC3 (SR_MAIN_TRC, "%s:%d TILFA P Node is %x\n ", __func__,
                         __LINE__, pLfaRtInfo->uRemoteNodeId.u4RemNodeRouterId);
                SR_TRC3 (SR_MAIN_TRC, "%s:%d TILFA Q Node is %x\n ", __func__,
                         __LINE__, pLfaRtInfo->uQNodeId.u4QNodeRouterId);
                SR_TRC3 (SR_MAIN_TRC, "%s:%d TILFA PQ Adj is %x\n ", __func__,
                         __LINE__, pLfaRtInfo->u4PaQAdjLocAddr);

                u2MlibOperation = MPLS_MLIB_FTN_CREATE;
                if (TiLfaRtAddOrDeleteEventHandler (pLfaRtInfo, u2MlibOperation)
                    != SR_FAILURE)
                {
                    SR_TRC2 (SR_MAIN_TRC,
                             "%s:%d Successfully processed TILFA_ROUTE_ADD event\n ",
                             __func__, __LINE__);
                }
                break;
            case TILFA_ROUTE_DELETE:
                SR_TRC2 (SR_MAIN_TRC,
                         "%s:%d TILFA_ROUTE_DELETE event received\n ", __func__,
                         __LINE__);
                SR_TRC3 (SR_MAIN_TRC, "%s:%d Destination Address is %x\n ",
                         __func__, __LINE__, pLfaRtInfo->u4DestNet);
                SR_TRC3 (SR_MAIN_TRC, "%s:%d Primary Next Hop is %x\n ",
                         __func__, __LINE__, pLfaRtInfo->u4NextHop);
                SR_TRC3 (SR_MAIN_TRC, "%s:%d TILFA Next Hop is %x\n ", __func__,
                         __LINE__, pLfaRtInfo->u4LfaNextHop);
                SR_TRC3 (SR_MAIN_TRC, "%s:%d TILFA Tunnel Id is %x\n ", __func__,
                         __LINE__, pLfaRtInfo->uRemoteNodeId.u4RemNodeRouterId);

                u2MlibOperation = MPLS_MLIB_FTN_DELETE;
                if (TiLfaRtAddOrDeleteEventHandler (pLfaRtInfo, u2MlibOperation)
                    != SR_FAILURE)
                {
                    SR_TRC3 (SR_MAIN_TRC,
                             "%s:%d Successfully processed TILFA_ROUTE_DELETE event for dest %x\n ",
                             __func__, __LINE__, pLfaRtInfo->u4DestNet);
                }
                break;
            case TILFA_ROUTE_RESET:
                SR_TRC2 (SR_MAIN_TRC,
                         "%s:%d TILFA_ROUTE_RESET event received\n ", __func__, __LINE__);
                break;
            case TILFA_ROUTE_MODIFY:
                SR_TRC2 (SR_MAIN_TRC,
                         "%s:%d TILFA_ROUTE_MODIFY event received\n ", __func__, __LINE__);
                pLfaRtInfo->u1CmdType = TILFA_ROUTE_DELETE;
                if (TiLfaRtAddOrDeleteEventHandler (pLfaRtInfo,
					MPLS_MLIB_FTN_DELETE) != SR_FAILURE)
                {
                    SR_TRC2 (SR_MAIN_TRC,"%s:%d Successfully processed "
		             "TILFA_ROUTE_DELETE event\n", __func__, __LINE__);
                }

                pLfaRtInfo->u1CmdType = TILFA_ROUTE_ADD;
                if (TiLfaRtAddOrDeleteEventHandler (pLfaRtInfo, MPLS_MLIB_FTN_CREATE)
                    != SR_FAILURE)
                {
                    SR_TRC2 (SR_MAIN_TRC,"%s:%d Successfully processed "
		             "TILFA_ROUTE_ADD event\n", __func__, __LINE__);
                }
                break;
            default:
                break;
        }
    }
    RTM_PROT_LOCK ();
    SR_TRC2 (SR_MAIN_TRC, "%s:%d EXIT\n", __func__, __LINE__);
    return;

}


/*****************************************************************************
 * Function Name : SrIpv4TiLfaCreateLSP
 * Description   : This routine is use to create Ti-LFA LSP
 *
 * Input(s)      : pTiLfaPathInfo   - T-LFA path info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrIpv4TiLfaCreateLSP (tSrTilfaPathInfo    *pTiLfaPathInfo)
{
    tSrTeLblStack        SrTeLblStack;
    tSrTilfaAdjInfo     *pTiLfaAdjInfo       = NULL;
    tSrRtrNextHopInfo   *pTempNextHopInfo   = NULL;
    tSrRtrNextHopInfo   *pTiLfaNextHopInfo   = NULL;
    tSrRtrNextHopInfo   *pPrimaryNextHopInfo = NULL;
    tSrRtrInfo          *pDestSrRtrInfo      = NULL;
    tGenU4Addr           DestAddr;
    tGenU4Addr           PrimaryNextHopAddr;
    UINT4                u4IfIndex  = SR_ZERO;
    UINT4                u4L3VlanIf = SR_ZERO;
    UINT4                u4LfaNextHop = SR_ZERO;
    UINT1                u1Count    = SR_ZERO;
    UINT1                u1NHFound  = SR_FALSE;

    DestAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    DestAddr.Addr.u4Addr = pTiLfaPathInfo->destAddr.Addr.u4Addr;

    u4LfaNextHop = pTiLfaPathInfo->lfaNextHopAddr.Addr.u4Addr;

    SR_TRC3 (SR_UTIL_TRC, "\n%s:%d TILFA LSP create for dest %x\n",
            __func__, __LINE__, DestAddr.Addr.u4Addr);

    /* Get the destination SR router info */
    pDestSrRtrInfo = SrGetSrRtrInfoFromRtrId (&DestAddr);
    if (pDestSrRtrInfo == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL for dest: %x\n",
                __func__, __LINE__, DestAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    /* Set the Label Stack as P-PQAdj(s)-Destination */
    SrTeLblStack.u4TopLabel = pTiLfaPathInfo->u4RemoteLabel;
    SR_TRC3 (SR_UTIL_TRC, "\n%s:%d TILFA LSP Top Lable %d\n",
                            __func__, __LINE__, SrTeLblStack.u4TopLabel);
    TMO_SLL_Scan (&(pTiLfaPathInfo->pqAdjSidList),
                      pTiLfaAdjInfo, tSrTilfaAdjInfo *)
    {
        SrTeLblStack.u4LabelStack[u1Count] = pTiLfaAdjInfo->u4AdjSid;
        SR_TRC4 (SR_UTIL_TRC, "\n%s:%d TILFA LSP Label Stack [%d] [%d]\n",
                            __func__, __LINE__, u1Count,
                            SrTeLblStack.u4LabelStack[u1Count]);
        u1Count++;
    }

    SrTeLblStack.u4LabelStack[u1Count] = pTiLfaPathInfo->u4DestLabel;
    SR_TRC4 (SR_UTIL_TRC, "\n%s:%d TILFA LSP Label Stack [%d] [%d] %x\n",
                            __func__, __LINE__, u1Count,
                            SrTeLblStack.u4LabelStack[u1Count]);
    SrTeLblStack.u1StackSize = (u1Count+1);

    TMO_SLL_Scan (&(pDestSrRtrInfo->NextHopList), pTempNextHopInfo,
                  tSrRtrNextHopInfo *)
    {
        if(pTempNextHopInfo->u1FRRNextHop == SR_PRIMARY_NEXTHOP)
        {
            pPrimaryNextHopInfo = pTempNextHopInfo;
            continue;
        }

        if ((pTempNextHopInfo->nextHop.u2AddrType == SR_IPV4_ADDR_TYPE)
            &&(u4LfaNextHop == pTempNextHopInfo->nextHop.Addr.u4Addr)
            &&(pTempNextHopInfo->bIsLfaNextHop == SR_TRUE)
            &&(pTempNextHopInfo->u1FRRNextHop == SR_TILFA_NEXTHOP))
        {
            pTiLfaNextHopInfo = pTempNextHopInfo;
        }
    }

    if (NULL == pPrimaryNextHopInfo)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                "%s:%d TILFA Primary NH is NULL for dest: %x\n",
                __func__, __LINE__, DestAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    if ((pPrimaryNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) != SR_FTN_CREATED)
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                "%s:%d FTN is not created for Primary NH %x for dest%\n",
                __func__, __LINE__, pPrimaryNextHopInfo->nextHop.Addr.u4Addr,
                DestAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    if (NULL == pTiLfaNextHopInfo)
    {
        /*Create new Next-Hop Node */
        if ((pTiLfaNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                    "%s:%d pNewSrRtrNextHopInfo == NULL",
                    __func__, __LINE__);
            return SR_FAILURE;
        }

        pTiLfaNextHopInfo->nextHop.u2AddrType  = SR_IPV4_ADDR_TYPE;
        pTiLfaNextHopInfo->nextHop.Addr.u4Addr = u4LfaNextHop;

        /* Add NextHop Node in NextHopList in SrRtrNode */
        TMO_SLL_Insert (&(pDestSrRtrInfo->NextHopList), NULL,
                        &(pTiLfaNextHopInfo->nextNextHop));
    }

    if ((pTiLfaNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
    {
        SR_TRC4 (SR_CRITICAL_TRC ,
                "%s:%d TILFA FTN is already created for dest%\n",
                __func__, __LINE__, pPrimaryNextHopInfo->nextHop.Addr.u4Addr,
                DestAddr.Addr.u4Addr);
        return SR_SUCCESS;
    }

    /*Get Primary NextHop for the prefix */
    if (SrGetNHForPrefix(&pTiLfaPathInfo->destAddr, &PrimaryNextHopAddr,
         &u4IfIndex) == SR_FAILURE)
    {
        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrGetNHForPrefix FAILURE",
                 __func__, __LINE__);
        /* If primary NH is not exist, we should not keep Alternative NH */
        TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                        (tTMO_SLL_NODE *) pTiLfaNextHopInfo);
        SR_RTR_NH_LIST_FREE (pTiLfaNextHopInfo);
        return SR_FAILURE;
    }

    if (pTiLfaPathInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        pTiLfaNextHopInfo->u1FRRNextHop     = SR_TILFA_NEXTHOP;
        pTiLfaNextHopInfo->bIsLfaNextHop    = SR_TRUE;
        pDestSrRtrInfo->u1LfaType           = SR_TILFA_ROUTE;
        pDestSrRtrInfo->u4LfaNextHop        = pTiLfaPathInfo->lfaNextHopAddr.Addr.u4Addr;
        pTiLfaNextHopInfo->u4OutIfIndex     = pTiLfaPathInfo->u4OutIfIndex;
        pTiLfaNextHopInfo->u4SwapOutIfIndex = pTiLfaPathInfo->u4OutIfIndex;
        MEMCPY (&(pTiLfaNextHopInfo->PrimarynextHop.Addr.u4Addr),
                &(PrimaryNextHopAddr.Addr.u4Addr), SR_IPV4_ADDR_LENGTH);
    }

    if ((SrUtilCreateMplsTunnelInterface
                     (pTiLfaPathInfo->u4OutIfIndex,
                      pTiLfaPathInfo->destAddr.u2AddrType,
                      &pTiLfaPathInfo->lfaNextHopAddr,
                      pTiLfaNextHopInfo)) == SR_FAILURE)
    {
        SR_TRC (SR_CRITICAL_TRC | SR_FAIL_TRC,
                "Failed to create MPLS tunnel Interface \r\n");
        SR_RTR_NH_LIST_FREE (pTiLfaNextHopInfo);
        return SR_FAILURE;
    }

    pTiLfaPathInfo->u4FtnTunIndex = pTiLfaNextHopInfo->u4OutIfIndex;

    /* Create FTN */
    if (SrMplsCreateOrDeleteStackLsp(MPLS_MLIB_FTN_CREATE, pDestSrRtrInfo,
                                NULL, &SrTeLblStack,
                                pTiLfaNextHopInfo, SR_TRUE) == SR_FAILURE)
    {
        /* Delete all the resources reserved on FAILURE & reset OutIfIndex */
        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d SrMplsCreateOrDeleteLsp SR_FAILURE", __func__,
                 __LINE__);
#ifdef CFA_WANTED
        if (CfaUtilGetIfIndexFromMplsTnlIf
            (pTiLfaPathInfo->u4FtnTunIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
        {
            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, pTiLfaPathInfo->u4FtnTunIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_MAIN_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed ",
                         __func__, __LINE__, pTiLfaPathInfo->u4FtnTunIndex,
                         u4L3VlanIf);
                return SR_FAILURE;
            }
        }
        pTiLfaNextHopInfo->u4OutIfIndex = SR_ZERO;
#endif
        TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                        (tTMO_SLL_NODE *) pTiLfaNextHopInfo);
        SR_RTR_NH_LIST_FREE (pTiLfaNextHopInfo);
        return SR_FAILURE;
    }

    pTiLfaNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;

    /*Update the FTN status */
    pTiLfaPathInfo->u1MPLSStatus |= SR_FTN_CREATED;
    SR_TRC2 (SR_MAIN_TRC, "%s:%d EXIT\n", __func__, __LINE__);
    return SR_SUCCESS;
}


/*****************************************************************************
 * Function Name : SrIpv4TiLfaCreateILM
 * Description   : This routine is use to create Ti-LFA ILM entry
 *
 * Input(s)      : pTiLfaPathInfo   - T-LFA path info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrIpv4TiLfaCreateILM (tSrTilfaPathInfo    *pTiLfaPathInfo)
{
    tGenU4Addr          GenAddr;
    tSrInSegInfo        SrInSegInfo;
    tSrTeLblStack       SrTeLblStack;
    tSrRtrInfo         *pDestSrRtrInfo       = NULL;
    tSrRtrNextHopInfo  *pSrLfaNextHopInfo    = NULL;
    tSrTilfaAdjInfo    *pTiLfaAdjInfo        = NULL;
    UINT4               u4LfaNextHop         = SR_ZERO;
    UINT4               u4L3VlanIf           = SR_ZERO;
    UINT4               u4MplsTnlIfIndex     = SR_ZERO;
    UINT1               u1NHFound            = SR_FALSE;
    UINT1               u1Count              = SR_ZERO;

    MEMSET (&GenAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));

    SR_TRC3 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY for Dest %x \n",
            __func__, __LINE__, pTiLfaPathInfo->destAddr.Addr.u4Addr);

    GenAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    GenAddr.Addr.u4Addr = pTiLfaPathInfo->destAddr.Addr.u4Addr;
    /* Get the destination SR router info */
    pDestSrRtrInfo = SrGetSrRtrInfoFromRtrId (&GenAddr);
    if (pDestSrRtrInfo == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL for dest: %x\n",
                __func__, __LINE__, GenAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    if (pDestSrRtrInfo->u4SidValue == SR_ZERO)
    {
       SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d: FAILURE: pSrRtrInfo->u4SidValue == 0 \n",
                __func__, __LINE__);
       return SR_FAILURE;
    }

    if (pDestSrRtrInfo->u1SIDConflictWin == SR_CONFLICT_NOT_WINNER)
    {
       SR_TRC3 (SR_UTIL_TRC, "%s:%d: Returning since NOT_WINNER for %x \n",
                __func__, __LINE__, pDestSrRtrInfo->advRtrId.Addr.u4Addr);
       return SR_SUCCESS;
    }

    if (pDestSrRtrInfo->u1PrefixConflictMPLSDel == SR_TRUE)
    {
       SR_TRC3 (SR_UTIL_TRC, "%s:%d Returning since PrefixConflictMPLSDel for %x \n",
                __func__, __LINE__, pDestSrRtrInfo->advRtrId.Addr.u4Addr);
       return SR_SUCCESS;
    }

    if (pDestSrRtrInfo->u1OutOfRangeAlarm == SR_ALARM_RAISE)
    {
       SR_TRC3 (SR_UTIL_TRC, "%s:%d Returning since OOR Alarm for %x \n",
                __func__, __LINE__, pDestSrRtrInfo->advRtrId.Addr.u4Addr);
       return SR_SUCCESS;
    }

    SrInSegInfo.u4InLabel =  SrReserveMplsInLabel (pDestSrRtrInfo->u2AddrType,
                                            pDestSrRtrInfo->u1SidIndexType,
                                            pDestSrRtrInfo->u4SidValue);

    SR_TRC3 (SR_UTIL_TRC, "%s:%d SrInSegInfo.u4InLabel == %d \n", __func__,
            __LINE__, SrInSegInfo.u4InLabel);
    if (SrInSegInfo.u4InLabel == SR_ZERO)
    {
       SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrInSegInfo.u4InLabel == SR_ZERO \n",
                __func__, __LINE__);
       return SR_FAILURE;
    }

    u4LfaNextHop = pTiLfaPathInfo->lfaNextHopAddr.Addr.u4Addr;

    TMO_SLL_Scan (&(pDestSrRtrInfo->NextHopList), pSrLfaNextHopInfo,
                  tSrRtrNextHopInfo *)
    {
        if ((pSrLfaNextHopInfo->nextHop.u2AddrType == SR_IPV4_ADDR_TYPE)
            &&(u4LfaNextHop == pSrLfaNextHopInfo->nextHop.Addr.u4Addr)
            &&(pSrLfaNextHopInfo->bIsLfaNextHop == SR_TRUE)
            &&(pSrLfaNextHopInfo->u1FRRNextHop == SR_TILFA_NEXTHOP))
        {
            u1NHFound = SR_TRUE;
            break;
        }
    }

    if (u1NHFound == SR_FALSE)
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d TI-LFA nexthop %x not found for the dest %x",
                __func__, __LINE__, u4LfaNextHop, pTiLfaPathInfo->destAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

#ifdef CFA_WANTED
    /* In case of SR - TE, skip the MPLS create interface in hardware */
    if ((pSrLfaNextHopInfo != NULL) && (pDestSrRtrInfo->u1TeSwapStatus != TRUE))
    {
        /*pSrRtrInfo->u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
        if (CfaIfmCreateStackMplsTunnelInterface(pSrLfaNextHopInfo->u4SwapOutIfIndex,
                                            &u4MplsTnlIfIndex) == CFA_SUCCESS)
        {
            SR_TRC4 (SR_UTIL_TRC,
                     "%s:%d pSrRtrNextHopInfo->u4SwapOutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                     __func__, __LINE__, pSrLfaNextHopInfo->u4SwapOutIfIndex,
                     u4MplsTnlIfIndex);

            pSrLfaNextHopInfo->u4SwapOutIfIndex = u4MplsTnlIfIndex;
        }
        else
        {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                     "%s:%d pSrRtrNextHopInfo->u4SwapOutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                     __func__, __LINE__, pSrLfaNextHopInfo->u4SwapOutIfIndex,
                     u4MplsTnlIfIndex);
            return SR_FAILURE;
        }
    }
#endif

    /* Set the Label Stack as P-PQAdj(s)-Destination */
    SrTeLblStack.u4TopLabel = pTiLfaPathInfo->u4RemoteLabel;
    SR_TRC3 (SR_UTIL_TRC, "\n%s:%d TILFA LSP Top Lable %d\n",
            __func__, __LINE__, SrTeLblStack.u4TopLabel);
    TMO_SLL_Scan (&(pTiLfaPathInfo->pqAdjSidList), pTiLfaAdjInfo,
                tSrTilfaAdjInfo *)
    {
        SrTeLblStack.u4LabelStack[u1Count] = pTiLfaAdjInfo->u4AdjSid;
        SR_TRC4 (SR_UTIL_TRC, "\n%s:%d TILFA LSP Label Stack [%d] [%d]\n",
                            __func__, __LINE__, u1Count,
                            SrTeLblStack.u4LabelStack[u1Count]);
        u1Count++;
    }

    SrTeLblStack.u4LabelStack[u1Count] = pTiLfaPathInfo->u4DestLabel;
    SR_TRC4 (SR_UTIL_TRC, "\n%s:%d TILFA LSP Label Stack [%d] [%d]\n",
                            __func__, __LINE__, u1Count,
                            SrTeLblStack.u4LabelStack[u1Count]);
    SrTeLblStack.u1StackSize = (u1Count+1);

    if (SrMplsCreateOrDeleteStackLsp(MPLS_MLIB_ILM_CREATE, pDestSrRtrInfo,
                                &SrInSegInfo, &SrTeLblStack,
                                pSrLfaNextHopInfo, SR_TRUE) == SR_FAILURE)
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d TILfa ILM creation failed for FEC: %x via Next Hop: %x\n",
                 __func__, __LINE__, pDestSrRtrInfo->prefixId.Addr.u4Addr,
                 pDestSrRtrInfo->u4LfaNextHop);
        if (SrReleaseMplsInLabel(pDestSrRtrInfo->u2AddrType,
                                pDestSrRtrInfo->u4SidValue) == SR_FAILURE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                    "%s:%d Unable to Release In-Label \n",
                    __func__, __LINE__);
            return SR_FAILURE;
        }

#ifdef CFA_WANTED
        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {

            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                         __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }

            pSrLfaNextHopInfo->u4SwapOutIfIndex = u4L3VlanIf;
        }
#endif
        return SR_FAILURE;
    }

    pTiLfaPathInfo->u4IlmTunIndex = pSrLfaNextHopInfo->u4SwapOutIfIndex;
    pDestSrRtrInfo->u4LfaInLabel  = SrInSegInfo.u4InLabel;

    SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
             "%s:%d TILFA ILM created successfully for FEC: %x via Next Hop: %x\n",
             __func__, __LINE__, pDestSrRtrInfo->prefixId.Addr.u4Addr,
             pDestSrRtrInfo->u4LfaNextHop);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrIpv4TiLfaDeleteLSP
 * Description   : This routine is use to delete Ti-LFA LSP
 *
 * Input(s)      : pTiLfaPathInfo   - T-LFA path info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrIpv4TiLfaDeleteLSP (tSrTilfaPathInfo    *pTiLfaPathInfo)
{
    tSrTeLblStack        SrTeLblStack;
    tSrRtrNextHopInfo   *pNextHopInfo      = NULL;
    tSrRtrInfo          *pDestSrRtrInfo    = NULL;
    tGenU4Addr           DestAddr;
    tGenU4Addr           LfaNextHopAddr;
    UINT4                u4L3VlanIf = SR_ZERO;
    UINT1                u1NHFound  = SR_FALSE;

    SR_TRC2 (SR_MAIN_TRC, "%s:%d ENTRY\n", __func__, __LINE__);

    DestAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    DestAddr.Addr.u4Addr = pTiLfaPathInfo->destAddr.Addr.u4Addr;

    /* Get the destination SR router info */
    pDestSrRtrInfo = SrGetSrRtrInfoFromNodeId (&DestAddr);
    if (pDestSrRtrInfo == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL for dest: %x\n",
                __func__, __LINE__, DestAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    LfaNextHopAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    LfaNextHopAddr.Addr.u4Addr = pTiLfaPathInfo->lfaNextHopAddr.Addr.u4Addr;

    TMO_SLL_Scan (&(pDestSrRtrInfo->NextHopList),
                      pNextHopInfo, tSrRtrNextHopInfo *)
    {
        if (pNextHopInfo->nextHop.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            if ((MEMCMP(&(LfaNextHopAddr.Addr.u4Addr),
                        &(pNextHopInfo->nextHop.Addr.u4Addr),
                        SR_IPV4_ADDR_LENGTH) == SR_ZERO)
                && (pNextHopInfo->bIsLfaNextHop == SR_TRUE)
                && (pNextHopInfo->u1FRRNextHop == SR_TILFA_NEXTHOP))
            {
                u1NHFound = SR_TRUE;
                break;
            }
        }
    }

    if (u1NHFound == SR_FALSE)
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d  Nexthop %x not found during deletion of TILFA LSP for dest %x",
                __func__, __LINE__, LfaNextHopAddr.Addr.u4Addr, DestAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    /* Delete FTN */
    if (SrMplsCreateOrDeleteStackLsp(MPLS_MLIB_FTN_DELETE, pDestSrRtrInfo,
                                NULL, &SrTeLblStack,
                                pNextHopInfo, SR_TRUE) == SR_FAILURE)
    {
        /* Delete all the resources reserved on FAILURE & reset OutIfIndex */
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d SrIpv4TiLfaDeleteLSP failed for dest %x", __func__,
                 __LINE__, DestAddr.Addr.u4Addr);
#ifdef CFA_WANTED
        if (CfaUtilGetIfIndexFromMplsTnlIf
            (pTiLfaPathInfo->u4FtnTunIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
        {
            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, pTiLfaPathInfo->u4FtnTunIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed ",
                         __func__, __LINE__, pTiLfaPathInfo->u4FtnTunIndex,
                         u4L3VlanIf);
                return SR_FAILURE;
            }
        }
#endif
        return SR_FAILURE;
    }

    /*Update the FTN status */
    pTiLfaPathInfo->u1MPLSStatus &= (UINT1) (~SR_FTN_CREATED);
    SR_TRC2 (SR_MAIN_TRC, "%s:%d EXIT\n", __func__, __LINE__);

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrIpv4TiLfaFrrModifyDeleteILM
 * Description   : This routine is use to delete Ti-LFA ILM with out resouce clear
 *
 * Input(s)      : pTiLfaPathInfo   - T-LFA path info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrIpv4TiLfaFrrModifyDeleteILM (tSrTilfaPathInfo    *pTiLfaPathInfo)
{
    tSrInSegInfo         SrInSegInfo;
    tSrTeLblStack        SrTeLblStack;
    tSrRtrNextHopInfo   *pSrLfaNextHopInfo  = NULL;
    tSrRtrInfo          *pDestSrRtrInfo     = NULL;
    tGenU4Addr           DestAddr;
    tGenU4Addr           LfaNextHopAddr;
    UINT1                u1NHFound  = SR_FALSE;

    SR_TRC2 (SR_MAIN_TRC, "%s:%d ENTRY\n", __func__, __LINE__);

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));

    DestAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    DestAddr.Addr.u4Addr = pTiLfaPathInfo->destAddr.Addr.u4Addr;

    /* Get the destination SR router info */
    pDestSrRtrInfo = SrGetSrRtrInfoFromNodeId (&DestAddr);
    if (pDestSrRtrInfo == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL for dest: %x\n",
                __func__, __LINE__, DestAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    LfaNextHopAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    LfaNextHopAddr.Addr.u4Addr = pTiLfaPathInfo->lfaNextHopAddr.Addr.u4Addr;

    TMO_SLL_Scan (&(pDestSrRtrInfo->NextHopList),
                      pSrLfaNextHopInfo, tSrRtrNextHopInfo *)
    {
        if (pSrLfaNextHopInfo->nextHop.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            if ((MEMCMP(&(LfaNextHopAddr.Addr.u4Addr),
                        &(pSrLfaNextHopInfo->nextHop.Addr.u4Addr),
                        SR_IPV4_ADDR_LENGTH) == SR_ZERO)
                && (pSrLfaNextHopInfo->bIsLfaNextHop == SR_TRUE)
                && (pSrLfaNextHopInfo->u1FRRNextHop == SR_TILFA_NEXTHOP))
            {
                u1NHFound = SR_TRUE;
                break;
            }
        }
    }

    if (u1NHFound == SR_FALSE)
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d  Nexthop %x not found during deletion of TILFA LSP for dest %x",
                __func__, __LINE__, LfaNextHopAddr.Addr.u4Addr, DestAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    SrInSegInfo.u4InLabel = pDestSrRtrInfo->u4LfaInLabel;

    if (SrMplsCreateOrDeleteStackLsp(MPLS_MLIB_ILM_DELETE, pDestSrRtrInfo,
                                &SrInSegInfo, &SrTeLblStack,
                                pSrLfaNextHopInfo, SR_TRUE) == SR_FAILURE)
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d TILfa ILM delete failed for FEC: %x via Next Hop: %x\n",
                 __func__, __LINE__, pDestSrRtrInfo->prefixId.Addr.u4Addr,
                 pDestSrRtrInfo->u4LfaNextHop);
        return SR_FAILURE;
    }

    SR_TRC2 (SR_MAIN_TRC, "%s:%d EXIT\n", __func__, __LINE__);

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrIpv4TiLfaDeleteILM
 * Description   : This routine is use to delete Ti-LFA ILM
 *
 * Input(s)      : pTiLfaPathInfo   - T-LFA path info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrIpv4TiLfaDeleteILM (tSrTilfaPathInfo    *pTiLfaPathInfo)
{
    tSrInSegInfo         SrInSegInfo;
    tSrTeLblStack        SrTeLblStack;
    tSrRtrNextHopInfo   *pSrLfaNextHopInfo  = NULL;
    tSrRtrInfo          *pDestSrRtrInfo     = NULL;
    tGenU4Addr           DestAddr;
    tGenU4Addr           LfaNextHopAddr;
    UINT1                u1NHFound  = SR_FALSE;

    SR_TRC2 (SR_MAIN_TRC, "%s:%d ENTRY\n", __func__, __LINE__);

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));

    DestAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    DestAddr.Addr.u4Addr = pTiLfaPathInfo->destAddr.Addr.u4Addr;

    /* Get the destination SR router info */
    pDestSrRtrInfo = SrGetSrRtrInfoFromNodeId (&DestAddr);
    if (pDestSrRtrInfo == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL for dest: %x\n",
                __func__, __LINE__, DestAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    LfaNextHopAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    LfaNextHopAddr.Addr.u4Addr = pTiLfaPathInfo->lfaNextHopAddr.Addr.u4Addr;

    TMO_SLL_Scan (&(pDestSrRtrInfo->NextHopList),
                      pSrLfaNextHopInfo, tSrRtrNextHopInfo *)
    {
        if (pSrLfaNextHopInfo->nextHop.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            if ((MEMCMP(&(LfaNextHopAddr.Addr.u4Addr),
                        &(pSrLfaNextHopInfo->nextHop.Addr.u4Addr),
                        SR_IPV4_ADDR_LENGTH) == SR_ZERO)
                && (pSrLfaNextHopInfo->bIsLfaNextHop == SR_TRUE)
                && (pSrLfaNextHopInfo->u1FRRNextHop == SR_TILFA_NEXTHOP))
            {
                u1NHFound = SR_TRUE;
                break;
            }
        }
    }

    if (u1NHFound == SR_FALSE)
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d  Nexthop %x not found during deletion of TILFA LSP for dest %x",
                __func__, __LINE__, LfaNextHopAddr.Addr.u4Addr, DestAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    SrInSegInfo.u4InLabel = pDestSrRtrInfo->u4LfaInLabel;

    if (SrMplsCreateOrDeleteStackLsp(MPLS_MLIB_ILM_DELETE, pDestSrRtrInfo,
                                &SrInSegInfo, &SrTeLblStack,
                                pSrLfaNextHopInfo, SR_TRUE) == SR_FAILURE)
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d TILfa ILM delete failed for FEC: %x via Next Hop: %x\n",
                 __func__, __LINE__, pDestSrRtrInfo->prefixId.Addr.u4Addr,
                 pDestSrRtrInfo->u4LfaNextHop);
    }

    pDestSrRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
    pDestSrRtrInfo->u1LfaType = SR_ZERO;
    pDestSrRtrInfo->bIsLfaActive = SR_FALSE;
    pDestSrRtrInfo->u4LfaNextHop = SR_ZERO;
    pDestSrRtrInfo->u4LfaInLabel = SR_ZERO;

    /*Release all the allocacated memory for this route */
    TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                    (tTMO_SLL_NODE *) pSrLfaNextHopInfo);
    SR_RTR_NH_LIST_FREE (pSrLfaNextHopInfo);

    if (SrMplsDeleteTunIntf (&pTiLfaPathInfo->u4IlmTunIndex) == SR_FAILURE)
    {
       SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete ILM Tunnel ", __func__,
                __LINE__);
       return SR_FAILURE;
    }

    pTiLfaPathInfo->u4IlmTunIndex = SR_ZERO;

    if (SrReleaseMplsInLabel (pDestSrRtrInfo->prefixId.u2AddrType,
                             pDestSrRtrInfo->u4SidValue) == SR_FAILURE)
    {
       SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to Release In-Label", __func__,
                __LINE__);
       return SR_FAILURE;
    }

    SR_TRC2 (SR_MAIN_TRC, "%s:%d EXIT\n", __func__, __LINE__);

    return SR_SUCCESS;
}



PUBLIC UINT4
SrMplsModifyTilfaLSP (tSrIpEvtInfo * pSrIpEvtInfo, tSrRtrInfo *pSrRtrInfo,
                    tSrTilfaPathInfo * pSrTiLfaPathInfo, UINT1 u1IsNewNHeqFRRNH)
{
    tSrRouteEntryInfo  *pRouteInfo = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrInfo         *pTempSrRtrInfo = NULL;
    tSrRtrNextHopInfo  *pSrOldNextHopInfo = NULL;
    tSrRtrNextHopInfo  *pSrTmpNextHopInfo = NULL;
    tSrRtrNextHopInfo  *pSrTilfaNextHopInfo = NULL;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrInSegInfo        SrInSegInfo;
    UINT4               u4TempNbrRtrId = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT1               u1Ret            = SR_ZERO;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    pRouteInfo = (tSrRouteEntryInfo *) (VOID *) pSrIpEvtInfo->pRouteInfo;

    TMO_SLL_Scan (&(pSrRtrInfo->NextHopList),
                          pSrRtrNextHopNode, tTMO_SLL_NODE *)
    {
        pSrTmpNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
        if ((pSrTmpNextHopInfo->bIsLfaNextHop == SR_TRUE)
            && (pSrTmpNextHopInfo->u1FRRNextHop == SR_TILFA_NEXTHOP))
        {
            pSrTilfaNextHopInfo = pSrTmpNextHopInfo;
        }
        else
        {
            pSrOldNextHopInfo = pSrTmpNextHopInfo;
        }
    }

    if ((pSrOldNextHopInfo == NULL) || (pSrTilfaNextHopInfo == NULL))
    {
        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Next Hop information Not found for"
                 " prefix = %x \n", __func__, __LINE__,
                 pSrTiLfaPathInfo->destAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    if (u1IsNewNHeqFRRNH == TRUE)
    {
        pSrOldNextHopInfo->bIsOnlyCPUpdate = SR_TRUE;
    }

    if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
    {
        /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
        SR_GET_SELF_IN_LABEL (&SrInSegInfo.u4InLabel, pSrRtrInfo->u4SidValue);

        if (u1IsNewNHeqFRRNH == TRUE)
        {
            pSrTilfaNextHopInfo->bIsOnlyCPUpdate = SR_TRUE;
        }
    }

    pSrRtrInfo->u4LfaNextHop = pSrTiLfaPathInfo->lfaNextHopAddr.Addr.u4Addr;
    pSrRtrInfo->u4LfaOutIfIndex = pSrTiLfaPathInfo->u4FtnTunIndex;
    pSrRtrInfo->u4LfaSwapOutIfIndex = pSrTiLfaPathInfo->u4IlmTunIndex;


    /*Create new Next-Hop Node */
    if ((pNewSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
    {
        SR_TRC2 (SR_FAIL_TRC | SR_UTIL_TRC,
                "%s:%d pNewSrRtrNextHopInfo == NULL memory alloc failed \n", __func__,
                __LINE__);
        return SR_FAILURE;
    }

    MEMCPY (&pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr,
            &(pRouteInfo->nextHop.Addr.u4Addr),
            MAX_IPV4_ADDR_LEN);
    pTempSrRtrInfo =  SrGetRtrInfoFromNextHop(&pRouteInfo->nextHop);
    if (pTempSrRtrInfo != NULL)
    {
        MEMCPY (&u4TempNbrRtrId,
                &(pTempSrRtrInfo->prefixId.Addr.u4Addr),
                MAX_IPV4_ADDR_LEN);
    }
    else
    {
        u4TempNbrRtrId = SrNbrRtrFromNextHop(pRouteInfo->nextHop.Addr.u4Addr);
    }
    pNewSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
    pNewSrRtrNextHopInfo->nbrRtrId = u4TempNbrRtrId;
    MEMCPY (&pNewSrRtrNextHopInfo->OldnextHop.Addr.u4Addr,
            &(pSrOldNextHopInfo->nextHop.Addr.u4Addr),
            MAX_IPV4_ADDR_LEN);
    pNewSrRtrNextHopInfo->u4OldOutIfIndex = pSrOldNextHopInfo->u4OutIfIndex;

    if (u1IsNewNHeqFRRNH != SR_TRUE)
    {
        pNewSrRtrNextHopInfo->u4OutIfIndex = pSrIpEvtInfo->u4IfIndex;
#ifdef CFA_WANTED
        if (CfaIfmCreateStackMplsTunnelInterface (pNewSrRtrNextHopInfo->u4OutIfIndex,
                    &u4MplsTnlIfIndex) == CFA_SUCCESS)
        {
            SR_TRC4 (SR_UTIL_TRC,
                    "%s:%d pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                    __func__, __LINE__, pNewSrRtrNextHopInfo->u4OutIfIndex,
                    u4MplsTnlIfIndex);
            pNewSrRtrNextHopInfo->u4OutIfIndex = u4MplsTnlIfIndex;
        }
        else
        {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                    "%s:%d FAILURE: pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                    __func__, __LINE__, pNewSrRtrNextHopInfo->u4OutIfIndex,
                    u4MplsTnlIfIndex);
            return SR_FAILURE;
        }
#endif
    }
    else
    {
        pNewSrRtrNextHopInfo->u4OutIfIndex = pSrTilfaNextHopInfo->u4OutIfIndex;
        SR_TRC4 (SR_UTIL_TRC,
                 "%s:%d FRR Modify u4OutIfIndex = %d for the Dest %x:",
                  __func__, __LINE__, pNewSrRtrNextHopInfo->u4OutIfIndex,
                  pRouteInfo->destAddr.Addr.u4Addr);
    }

    SRModifyNexthopHandle(pSrRtrInfo, pNewSrRtrNextHopInfo, pSrOldNextHopInfo);

    if (u1IsNewNHeqFRRNH == SR_TRUE)
    {
        pNewSrRtrNextHopInfo->bIsOnlyCPUpdate = SR_TRUE;
        u1Ret = SrMplsCreateOrDeleteLsp (MPLS_MLIB_FRR_FTN_MODIFY, pSrRtrInfo,
                                        NULL, pNewSrRtrNextHopInfo);
    }
    else
    {
        u1Ret = SrMplsCreateOrDeleteLsp (MPLS_MLIB_FTN_MODIFY, pSrRtrInfo,
                                        NULL, pNewSrRtrNextHopInfo);
    }

    if (u1Ret == SR_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC, "%s:%d FTN Modify Successful for"
                 " prefix = %x Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
    }
    else
    {
        if (SrMplsDeleteTunIntf (&pNewSrRtrNextHopInfo->u4OutIfIndex)
            == SR_FAILURE)
        {
            SR_TRC3 (SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d Failed to Delete FTN Tunnel for"
                     " prefix = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr);
            return SR_FAILURE;
        }

        SR_TRC5 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d FTN Modify Failed for"
                 " prefix = %x Old Next hop = %x New Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pSrOldNextHopInfo->nextHop.Addr.u4Addr, pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
        return SR_FAILURE;
    }


    if (u1IsNewNHeqFRRNH != SR_TRUE)
    {
        pNewSrRtrNextHopInfo->u4SwapOutIfIndex = pSrIpEvtInfo->u4IfIndex;
#ifdef CFA_WANTED
        u4MplsTnlIfIndex = 0;
        if (CfaIfmCreateStackMplsTunnelInterface (pNewSrRtrNextHopInfo->u4SwapOutIfIndex,
                    &u4MplsTnlIfIndex) == CFA_SUCCESS)
        {
            SR_TRC4 (SR_UTIL_TRC,
                    "%s:%d pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                    __func__, __LINE__, pNewSrRtrNextHopInfo->u4SwapOutIfIndex,
                    u4MplsTnlIfIndex);
            pNewSrRtrNextHopInfo->u4SwapOutIfIndex = u4MplsTnlIfIndex;
        }
        else
        {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                    "%s:%d FAILURE: pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                    __func__, __LINE__, pNewSrRtrNextHopInfo->u4SwapOutIfIndex,
                    u4MplsTnlIfIndex);
            return SR_FAILURE;
        }
#endif
    }
    else
    {
        pNewSrRtrNextHopInfo->u4SwapOutIfIndex = pSrTiLfaPathInfo->u4IlmTunIndex;
    }

    pSrRtrInfo->u4LfaNextHop = SR_ZERO;
    pSrRtrInfo->u4LfaOutIfIndex = SR_ZERO;
    pSrRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
    pSrRtrInfo->u1LfaType = SR_ZERO;
    pSrRtrInfo->bIsLfaActive = SR_ZERO;

    if (u1IsNewNHeqFRRNH == SR_TRUE)
    {
        u1Ret = SrMplsCreateOrDeleteLsp (MPLS_MLIB_FRR_ILM_MODIFY, pSrRtrInfo,
                                        &SrInSegInfo, pNewSrRtrNextHopInfo);
    }
    else
    {
        if (SrMplsDeleteTunIntf(&pSrTiLfaPathInfo->u4IlmTunIndex) == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete ILM Tunnel for"
                     " prefix = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr);
        }

        u1Ret = SrMplsCreateOrDeleteLsp (MPLS_MLIB_ILM_MODIFY, pSrRtrInfo,
                                        &SrInSegInfo, pNewSrRtrNextHopInfo);
    }

    if (u1Ret == SR_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC, "%s:%d ILM Create Successful for"
                 " prefix = %x Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
    }
    else
    {
        if (SrReleaseMplsInLabel (pSrRtrInfo->u2AddrType,
                                  pSrRtrInfo->u4SidValue) == SR_FAILURE)
        {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to Release in label for"
                     " prefix = %x Next hop = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr,
                     pSrOldNextHopInfo->nextHop.Addr.u4Addr);
            return SR_FAILURE;
        }

        if (SrMplsDeleteTunIntf (&pSrOldNextHopInfo->u4SwapOutIfIndex)
            == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete ILM Tunnel for"
                     " prefix = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr);
            return SR_FAILURE;
        }

        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d ILM Create Failed for"
                 " prefix = %x Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
        return SR_FAILURE;
    }

    if (u1IsNewNHeqFRRNH == TRUE)
    {
        pNewSrRtrNextHopInfo->bIsOnlyCPUpdate = SR_FALSE;
    }

    TMO_SLL_Delete (&(pSrRtrInfo->NextHopList),
                    (tTMO_SLL_NODE *) pSrTilfaNextHopInfo);
    SR_RTR_NH_LIST_FREE (pSrTilfaNextHopInfo);
    TMO_SLL_Delete (&(pSrRtrInfo->NextHopList),
                    (tTMO_SLL_NODE *) pSrOldNextHopInfo);
    SR_RTR_NH_LIST_FREE (pSrOldNextHopInfo);

    SrTiLfaPathDelete(pSrTiLfaPathInfo);

    TMO_SLL_Insert (&(pSrRtrInfo->NextHopList), NULL,
            &(pNewSrRtrNextHopInfo->nextNextHop));
    pNewSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}


/*****************************************************************************/
/* Function Name : SrGetLabelFromSIDForRemoteNode                            */
/* Description   : GEt the Label for remote SID                              */
/* Input(s)      : pNexthopSrRtrInfo - SR router info                        */
/*                 u4SidValue - SID value                                    */
/* Output(s)     : pLabel                                                    */
/* Return(s)     :                                                           */
/*****************************************************************************/
VOID
SrGetLabelFromSIDForRemoteNode(tSrRtrInfo         *pNexthopSrRtrInfo,
                                UINT4               u4SidValue,
                                UINT4              *pLabel)
{
    UINT4 u4SidRange;

    u4SidRange = ((pNexthopSrRtrInfo->srgbRange.u4SrGbMaxIndex-
                pNexthopSrRtrInfo->srgbRange.u4SrGbMinIndex)+1);

    if (u4SidValue > u4SidRange)
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SID (%d) is out of range w.r.to nexthop: (%x)\n",
                __func__, __LINE__, u4SidValue,
                pNexthopSrRtrInfo->advRtrId.Addr.u4Addr);

        return;
    }

    *pLabel = pNexthopSrRtrInfo->srgbRange.u4SrGbMinIndex + u4SidValue;
    return;
}

/*****************************************************************************
 * Function Name : SrIpv4TiLfaUpdateLabelStackInfo
 * Description   : This routine is use to derive and update Label Stack
 *                 for Ti-LFA
 *
 * Input(s)      : pTiLfaPathInfo   - T-LFA path info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrIpv4TiLfaUpdateLabelStackInfo (tSrTilfaPathInfo    *pTiLfaPathInfo)
{
    tGenU4Addr          GenAddr;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrRtrInfo         *pNextHopSrRtrInfo = NULL;
    tSrRtrInfo         *pPSrRtrInfo = NULL;
    tSrRtrInfo         *pQSrRtrInfo = NULL;
    UINT4               u4PnodeLabel = 0;
    UINT4               u4DestLabel  = 0;

    GenAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    GenAddr.Addr.u4Addr = pTiLfaPathInfo->destAddr.Addr.u4Addr;

    /* Get the destination SR router info */
    pDestSrRtrInfo = SrGetSrRtrInfoFromNodeId (&GenAddr);
    if (pDestSrRtrInfo == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL for dest: %x\n",
                __func__, __LINE__, GenAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    /* if route is added to delay list, LFA info will be updated once
    primary route update done */
    if ((pDestSrRtrInfo->u1OprFlags & SR_OPR_F_ADD_DELAY_LIST)
        == SR_OPR_F_ADD_DELAY_LIST)
    {
        SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
            "%s:%d Router node %x with prefix %x added to delay list",
            __func__, __LINE__, pDestSrRtrInfo->advRtrId.Addr.u4Addr,
            GenAddr.Addr.u4Addr);
        return SR_SUCCESS;
    }

    /* Get the LFA nexthop SR router info */
    GenAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    GenAddr.Addr.u4Addr = pTiLfaPathInfo->u4LfaNexthopRtrId;
    pNextHopSrRtrInfo = SrGetSrRtrInfoFromRtrId (&GenAddr);
    if (pNextHopSrRtrInfo == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL for nexthop: %x\n",
                __func__, __LINE__, GenAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    /* Get the P node SR router info */
    GenAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    GenAddr.Addr.u4Addr = pTiLfaPathInfo->u4PRtrId;
    pPSrRtrInfo = SrGetSrRtrInfoFromRtrId (&GenAddr);
    if (pPSrRtrInfo == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL for P node: %x\n",
                __func__, __LINE__, GenAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    SrGetLabelFromSIDForRemoteNode(pNextHopSrRtrInfo,
                                pPSrRtrInfo->u4SidValue,
                                &u4PnodeLabel);

    pTiLfaPathInfo->u4RemoteLabel = u4PnodeLabel;

    if (SR_ZERO != pTiLfaPathInfo->u4RemoteLabel)
    {
        pTiLfaPathInfo->u1LfaStatusFlags |= SR_TILFA_SET_P_LABEL;
    }

    /* Get the Q node SR router info */
    GenAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    GenAddr.Addr.u4Addr = pTiLfaPathInfo->u4QRtrId;

    pQSrRtrInfo = SrGetSrRtrInfoFromRtrId (&GenAddr);
    if (pQSrRtrInfo == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL for Q node: %x\n",
                __func__, __LINE__, GenAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    SrGetLabelFromSIDForRemoteNode(pQSrRtrInfo,
                                pDestSrRtrInfo->u4SidValue,
                                &u4DestLabel);
    pTiLfaPathInfo->u4DestLabel = u4DestLabel;

    if (SR_ZERO != pTiLfaPathInfo->u4DestLabel)
    {
        pTiLfaPathInfo->u1LfaStatusFlags |= SR_TILFA_SET_DEST_LABEL;
    }

    if (SR_FAILURE == SrUpdateAdjLabelforPandQ(pTiLfaPathInfo, pPSrRtrInfo))
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Ti-LFA Adj Label(s) not derived for dest:%x\n",
                __func__, __LINE__, pTiLfaPathInfo->destAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    if ((pTiLfaPathInfo->u1LfaStatusFlags & SR_TILFA_LABEL_STACK_DONE)
                                    == SR_TILFA_LABEL_STACK_DONE)
    {
        pTiLfaPathInfo->u1LfaStatusFlags |= SR_TILFA_SET_LABEL_STACK;
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrLfaAdjCreate
 * Description   : This routine is uses to create LFA Adj to track TILFA paths
 * Input(s)      : pTiLfaPathInfo   - TILFA path Info
 *
 * Output(s)     : pointer to tSrLfaAdjInfo
 * Return(s)     :
 *****************************************************************************/
tSrLfaAdjInfo *
SrLfaAdjCreate(tSrTilfaAdjInfo    *pTiLfaAdjInfo)
{
    tSrLfaAdjInfo *pLfaAdj = NULL;

    /*Allocate memory for tSrLfaNodeInfo */
    if (SR_LFA_ADJ_MEM_ALLOC (pLfaAdj) == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d Memory allocation failed for Adj IP:%x\n ",
                 __func__, __LINE__, pTiLfaAdjInfo->AdjIp.Addr.u4Addr);
        return NULL;
    }

    MEMSET (pLfaAdj, SR_ZERO, sizeof (tSrLfaAdjInfo));

    pLfaAdj->AdjIp.Addr.u4Addr = pTiLfaAdjInfo->AdjIp.Addr.u4Addr;
    pLfaAdj->u4AreaId          = pTiLfaAdjInfo->u4AreaId;

    TMO_SLL_Init (&(pLfaAdj->LfaList));

    /* Add Lfa Node to LFA Node Tree */
    if (RBTreeAdd (gSrGlobalInfo.pSrLfaAdjRbTree,
                   (tRBElem *) pLfaAdj) != RB_SUCCESS)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d Failed to add Lfa Adj %x to Adj Rbtree\n ",
                         __func__, __LINE__, pTiLfaAdjInfo->AdjIp.Addr.u4Addr);

        SR_LFA_ADJ_MEM_FREE(pLfaAdj);
        return NULL;
    }

    return pLfaAdj;
}

/*****************************************************************************
 * Function Name : SrLfaNodeCreate
 * Description   : This routine is uses to create LFA Node to track TILFA paths
 * Input(s)      : pTiLfaPathInfo   - TILFA path Info
 *
 * Output(s)     : pointer to tSrLfaNodeInfo
 * Return(s)     :
 *****************************************************************************/
tSrLfaNodeInfo *
SrLfaNodeCreate(UINT4 u4RtrId)
{
    tSrLfaNodeInfo *pLfaNode = NULL;

    /*Allocate memory for tSrLfaNodeInfo */
    if (SR_LFA_NODE_MEM_ALLOC (pLfaNode) == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d Memory allocation failed for LFA Node for Rtr:%x\n ",
                 __func__, __LINE__, u4RtrId);
        return NULL;
    }

    MEMSET (pLfaNode, SR_ZERO, sizeof (tSrLfaNodeInfo));

    pLfaNode->u4RtrId = u4RtrId;

    UTL_SLL_Init (&(pLfaNode->NexthopLfaList),
                SR_OFFSET (tSrTilfaPathInfo, nexthopNode));
    UTL_SLL_Init (&(pLfaNode->PLfaList),
                  SR_OFFSET (tSrTilfaPathInfo, pNode));
    UTL_SLL_Init (&(pLfaNode->QLfaList),
                  SR_OFFSET (tSrTilfaPathInfo, qNode));
    /* Add Lfa Node to LFA Node Tree */
    if (RBTreeAdd (gSrGlobalInfo.pSrLfaNodeRbTree,
                   (tRBElem *) pLfaNode) != RB_SUCCESS)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                "%s:%d Failed to add Lfa Node %x to Node Rbtree\n ",
                __func__, __LINE__, u4RtrId);

        SR_LFA_NODE_MEM_FREE(pLfaNode);
        return NULL;
    }

    return pLfaNode;
}

/*****************************************************************************
 * Function Name : SrCreateTilfaAdjEntry
 * Description   : This routine is uses to create Adj list node for TI LFA path
 * Input(s)      : tNetIpv4LfaRtInfo   - Event Info
 * Output(s)     : pointer to tSrLfaNodeInfo
 * Return(s)     :
 *****************************************************************************/
tSrTilfaAdjInfo    *
SrCreateTilfaAdjEntry (tNetIpv4LfaRtInfo * pNetIpLfaRtInfo)
{
    tSrTilfaAdjInfo    *pTiLfaAdjInfo = NULL;

    /*Allocate memory for tSrTilfaAdjInfo */
    if (SR_TILFA_ADJ_MEM_ALLOC (pTiLfaAdjInfo) == NULL)
    {
        SR_TRC3 (SR_MAIN_TRC,
              "%s:%d Memory allocation failed for SR TI-LFA Adj for dest:%x\n ",
              __func__, __LINE__, pNetIpLfaRtInfo->u4DestNet);
        return NULL;
    }

    MEMSET (pTiLfaAdjInfo, SR_ZERO, sizeof (tSrTilfaAdjInfo));
    pTiLfaAdjInfo->AdjIp.u2AddrType  =  SR_IPV4_ADDR_TYPE;
    pTiLfaAdjInfo->AdjIp.Addr.u4Addr = pNetIpLfaRtInfo->u4PaQAdjLocAddr;

    SR_TRC4 (SR_UTIL_TRC,
              "%s:%d PQ Adj ip for TILFA dest %x is %x:%x\n ",
              __func__, __LINE__,
              pNetIpLfaRtInfo->u4DestNet, pTiLfaAdjInfo->AdjIp.Addr.u4Addr);

    return pTiLfaAdjInfo;
}


/*****************************************************************************
 * Function Name : SrCreateTiLfaPathEntry
 * Description   : This routine is uses to create TI-LFA path info for LFA route
 * Input(s)      : tNetIpv4LfaRtInfo   - Event Info
 *
 * Output(s)     : pointer to tSrTilfaPathInfo
 * Return(s)     :
 *****************************************************************************/
tSrTilfaPathInfo    *
SrCreateTiLfaPathEntry (tNetIpv4LfaRtInfo * pNetIpLfaRtInfo)
{
    tGenU4Addr          GenAddr;
    tSrRtrInfo         *pDestSrRtrInfo  = NULL;
    tSrTilfaPathInfo    *pTiLfaPathInfo = NULL;
    tSrTilfaAdjInfo     *pTiLfaAdjInfo  = NULL;
    UINT4                NbrRtrId       = SR_ZERO;
    UINT4                u4OutIfIndex   = SR_ZERO;

    /*Allocate memory for tSrRlfaPathInfo */
    if (SR_TILFA_PATH_MEM_ALLOC (pTiLfaPathInfo) == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d Memory allocation failed for SR TI-LFA path for dest:%x\n ",
                 __func__, __LINE__, pNetIpLfaRtInfo->u4DestNet);
        return NULL;
    }

    MEMSET (pTiLfaPathInfo, SR_ZERO, sizeof (tSrTilfaPathInfo));
    TMO_SLL_Init (&pTiLfaPathInfo->pqAdjSidList);

    /* Todo: Adj list between P and Q support from RTM */
    pTiLfaAdjInfo = SrCreateTilfaAdjEntry(pNetIpLfaRtInfo);

    if (NULL == pTiLfaAdjInfo)
    {
        SR_TILFA_PATH_MEM_FREE(pTiLfaPathInfo);
        return NULL;
    }


    GenAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    GenAddr.Addr.u4Addr = pTiLfaPathInfo->destAddr.Addr.u4Addr;

    /* Get the destination SR router info */
    pDestSrRtrInfo = SrGetSrRtrInfoFromNodeId (&GenAddr);
    if (pDestSrRtrInfo != NULL)
    {
        pTiLfaAdjInfo->u4AreaId = pDestSrRtrInfo->u4areaId;
    }

    TMO_SLL_Add (&(pTiLfaPathInfo->pqAdjSidList), (tTMO_SLL_NODE *)pTiLfaAdjInfo);
    pTiLfaPathInfo->destAddr.u2AddrType    = SR_IPV4_ADDR_TYPE;
    pTiLfaPathInfo->destAddr.Addr.u4Addr   = pNetIpLfaRtInfo->u4DestNet;
    pTiLfaPathInfo->lfaNextHopAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    pTiLfaPathInfo->lfaNextHopAddr.Addr.u4Addr = pNetIpLfaRtInfo->u4LfaNextHop;

    SrGetNbrRtrIdFromNbrAddr(&pTiLfaPathInfo->lfaNextHopAddr, &NbrRtrId);
    if (SR_ZERO == NbrRtrId)
    {
        SR_TRC (SR_CRITICAL_TRC | SR_FAIL_TRC,
                "\nCreate SR-TILFA-Path Table : Nexthop Nbr Router ID not found \n");
        SR_SLL_DELETE_LIST(&(pTiLfaPathInfo->pqAdjSidList), SrTiLfaFreeAdjSidList);
        SR_TILFA_PATH_MEM_FREE (pTiLfaPathInfo);
        return NULL;
    }

    pTiLfaPathInfo->u4LfaNexthopRtrId = NbrRtrId;
    pTiLfaPathInfo->u4PRtrId      = pNetIpLfaRtInfo->uRemoteNodeId.u4RemNodeRouterId;
    pTiLfaPathInfo->u4QRtrId      = pNetIpLfaRtInfo->uQNodeId.u4QNodeRouterId;

    if (CfaIpIfGetIfIndexForNetInCxt
        (SR_ZERO, pNetIpLfaRtInfo->u4LfaNextHop,
         &u4OutIfIndex) == CFA_FAILURE)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                "%s:%d Not able to get Out IfIndex for the LFA nexthop %x  \n",
                 __func__, __LINE__, pNetIpLfaRtInfo->u4LfaNextHop);
    }

    pTiLfaPathInfo->u4OutIfIndex  = u4OutIfIndex;

    if (RBTreeAdd(gSrGlobalInfo.pSrTilfaPathRbTree,
         (tRBElem *) pTiLfaPathInfo) == RB_FAILURE)
    {
        SR_TRC (SR_CRITICAL_TRC | SR_FAIL_TRC,
                "\nCreate SR-TILFA-Path Table : RBTree Node Add Failed \n");
        SR_SLL_DELETE_LIST(&(pTiLfaPathInfo->pqAdjSidList), SrTiLfaFreeAdjSidList);
        SR_TILFA_PATH_MEM_FREE (pTiLfaPathInfo);
        return NULL;
    }

    return pTiLfaPathInfo;
}

/*****************************************************************************
 * Function Name : SrAddTiLfaPathEntryToLfaNode
 * Description   : This routine is uses to ADD TI-LFA path info for LFA Node
 * Input(s)      : pTiLfaPathInfo   - TILFA path Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrAddTiLfaPathEntryToLfaNode(tSrTilfaPathInfo   *pTiLfaPathInfo)
{
    tSrLfaNodeInfo       LfaTempNode;
    tSrLfaNodeInfo      *pLfaNode       = NULL;

    MEMSET (&LfaTempNode, SR_ZERO, sizeof (tSrLfaNodeInfo));

    LfaTempNode.u4RtrId = pTiLfaPathInfo->u4LfaNexthopRtrId;
    pLfaNode = RBTreeGet (gSrGlobalInfo.pSrLfaNodeRbTree, &LfaTempNode);
    if (NULL == pLfaNode)
    {
        pLfaNode = SrLfaNodeCreate(LfaTempNode.u4RtrId);
        if (NULL == pLfaNode)
        {
            return SR_FAILURE;
        }
    }

    TMO_SLL_Add (&(pLfaNode->NexthopLfaList), &(pTiLfaPathInfo->nexthopNode));

    LfaTempNode.u4RtrId = pTiLfaPathInfo->u4PRtrId;

    pLfaNode = RBTreeGet (gSrGlobalInfo.pSrLfaNodeRbTree, &LfaTempNode);
    if (NULL == pLfaNode)
    {
        pLfaNode = SrLfaNodeCreate(LfaTempNode.u4RtrId);
        if (NULL == pLfaNode)
        {
            return SR_FAILURE;
        }
    }

    TMO_SLL_Add (&(pLfaNode->PLfaList), &(pTiLfaPathInfo->pNode));

    LfaTempNode.u4RtrId = pTiLfaPathInfo->u4QRtrId;

    pLfaNode = RBTreeGet (gSrGlobalInfo.pSrLfaNodeRbTree, &LfaTempNode);
    if (NULL == pLfaNode)
    {
        pLfaNode = SrLfaNodeCreate(LfaTempNode.u4RtrId);
        if (NULL == pLfaNode)
        {
            return SR_FAILURE;
        }
    }

    TMO_SLL_Add (&(pLfaNode->QLfaList), &(pTiLfaPathInfo->qNode));

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrProcessDelSllNodeFromLfaNode
 * Description   : This routine is uses to Delete SLL node from LFA Node
 * Input(s)      : pLfaNode        - LFA node
 *                 pTiLfaPathInfo  - LFA path info
 *                 u1NodeType      - Nexthop, P, Q
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrProcessDelSllNodeFromLfaNode(tSrLfaNodeInfo      *pLfaNode,
                            tSrTilfaPathInfo   *pTiLfaPathInfo,
                               UINT1            u1NodeType)
{
    switch (u1NodeType)
    {
        case LFA_NODE_NEXTHOP:
            TMO_SLL_Delete (&(pLfaNode->NexthopLfaList),
                            &(pTiLfaPathInfo->nexthopNode));
            break;
        case LFA_NODE_P:
            TMO_SLL_Delete (&(pLfaNode->PLfaList),
                            &(pTiLfaPathInfo->pNode));
            break;
        case LFA_NODE_Q:
            TMO_SLL_Delete (&(pLfaNode->QLfaList),
                            &(pTiLfaPathInfo->qNode));
            break;
        default:
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "\n%s:%d Received Invalid Type %d for LFA Node %x deletion \n",
                    __func__, __LINE__, pLfaNode->u4RtrId, u1NodeType);
            return;
    }

    if ((SR_ZERO == TMO_SLL_Count(&(pLfaNode->NexthopLfaList)))
        && (SR_ZERO == TMO_SLL_Count(&(pLfaNode->PLfaList)))
        && (SR_ZERO == TMO_SLL_Count(&(pLfaNode->QLfaList))))
    {
        if((RBTreeRemove (gSrGlobalInfo.pSrLfaNodeRbTree, pLfaNode))== RB_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "\n%s:%d TiLfa Node deletion failed for Node %x\n",
                 __func__, __LINE__, pLfaNode->u4RtrId);
            return;
        }

        SR_TRC3 (SR_UTIL_TRC, "%s:%d Free TILFA Node info %x\n",
                    __func__, __LINE__, pLfaNode->u4RtrId);

        SR_LFA_NODE_MEM_FREE(pLfaNode);
    }

    return;
}

/*****************************************************************************
 * Function Name : SrDelTiLfaPathEntryFromLfaNode
 * Description   : This routine is uses to Delete TI-LFA path info from LFA Node
 * Input(s)      : pTiLfaPathInfo   - TILFA path Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
VOID
SrDelTiLfaPathEntryFromLfaNode(tSrTilfaPathInfo   *pTiLfaPathInfo)
{
    tSrLfaNodeInfo       LfaTempNode;
    tSrLfaNodeInfo      *pLfaNode  = NULL;
    tTMO_SLL_NODE	    *pNode     = NULL;

    MEMSET (&LfaTempNode, SR_ZERO, sizeof (tSrLfaNodeInfo));

    pNode = &(pTiLfaPathInfo->nexthopNode);

    if (TMO_SLL_Is_Node_In_List(pNode))
    {
        LfaTempNode.u4RtrId = pTiLfaPathInfo->u4LfaNexthopRtrId;
        pLfaNode = RBTreeGet (gSrGlobalInfo.pSrLfaNodeRbTree, &LfaTempNode);
        if (NULL != pLfaNode)
        {
            SR_TRC3 (SR_UTIL_TRC, "%s:%d Removing TILFA Nexthop %x from LFA Node list\n",
                    __func__, __LINE__, LfaTempNode.u4RtrId);

            SrProcessDelSllNodeFromLfaNode(pLfaNode, pTiLfaPathInfo, LFA_NODE_NEXTHOP);
        }
    }

    pNode = &(pTiLfaPathInfo->pNode);

    if (TMO_SLL_Is_Node_In_List(pNode))
    {
        LfaTempNode.u4RtrId = pTiLfaPathInfo->u4PRtrId;
        pLfaNode = RBTreeGet (gSrGlobalInfo.pSrLfaNodeRbTree, &LfaTempNode);
        if (NULL != pLfaNode)
        {
            SR_TRC3 (SR_UTIL_TRC, "%s:%d Removing TILFA P node %x from LFA Node list\n",
                               __func__, __LINE__, LfaTempNode.u4RtrId);
            SrProcessDelSllNodeFromLfaNode(pLfaNode, pTiLfaPathInfo, LFA_NODE_P);
        }
    }

    pNode = &(pTiLfaPathInfo->qNode);

    if (TMO_SLL_Is_Node_In_List(pNode))
    {
        LfaTempNode.u4RtrId = pTiLfaPathInfo->u4QRtrId;
        pLfaNode = RBTreeGet (gSrGlobalInfo.pSrLfaNodeRbTree, &LfaTempNode);
        if (NULL != pLfaNode)
        {
            SR_TRC3 (SR_UTIL_TRC, "%s:%d Removing TILFA Q node %x from LFA Node list\n",
                                           __func__, __LINE__, LfaTempNode.u4RtrId);
            SrProcessDelSllNodeFromLfaNode(pLfaNode, pTiLfaPathInfo, LFA_NODE_Q);
        }
    }

    return;
}

/*****************************************************************************
 * Function Name : SrDelTiLfaPathEntryFromLfaAdj
 * Description   : This routine is uses to Delete TI-LFA path info from LFA Adj
 * Input(s)      : pTiLfaPathInfo   - TILFA path Info
 *
 * Output(s)     :
 * Return(s)     :
 *****************************************************************************/
VOID
SrDelTiLfaPathEntryFromLfaAdj(tSrTilfaPathInfo   *pTiLfaPathInfo)
{
    tSrLfaAdjInfo       LfaTempAdj;
    tSrLfaAdjInfo      *pLfaAdj        = NULL;
    tTMO_SLL_NODE      *pLfaAdjSid     = NULL;
    tSrTilfaAdjInfo    *pTiLfaAdjInfo  = NULL;
    tSrAdjLfaListNode  *pLfaAdjNode    = NULL;

    MEMSET (&LfaTempAdj, SR_ZERO, sizeof (tSrLfaAdjInfo));

    TMO_SLL_Scan (&(pTiLfaPathInfo->pqAdjSidList), pLfaAdjSid, tTMO_SLL_NODE *)
    {
        pTiLfaAdjInfo = (tSrTilfaAdjInfo *) pLfaAdjSid;
        LfaTempAdj.AdjIp.Addr.u4Addr = pTiLfaAdjInfo->AdjIp.Addr.u4Addr;
        LfaTempAdj.u4AreaId          = pTiLfaAdjInfo->u4AreaId;
        pLfaAdj = RBTreeGet(gSrGlobalInfo.pSrLfaAdjRbTree, &LfaTempAdj);
        if (NULL == pLfaAdj)
        {
            continue;
        }

        TMO_SLL_Scan (&(pLfaAdj->LfaList), pLfaAdjSid, tTMO_SLL_NODE *)
        {
            pLfaAdjNode = (tSrAdjLfaListNode  *)pLfaAdjSid;

            if (NULL == pLfaAdjNode->pLfaPathInfo)
            {
                continue;
            }

            if (pLfaAdjNode->pLfaPathInfo->destAddr.Addr.u4Addr ==
                            pTiLfaPathInfo->destAddr.Addr.u4Addr)
            {
                TMO_SLL_Delete (&(pLfaAdj->LfaList), pLfaAdjSid);
                SR_TRC3 (SR_UTIL_TRC, "%s:%d Removing TILFA Path info %x from LFA Adj list\n",
                               __func__, __LINE__, pTiLfaPathInfo->destAddr.Addr.u4Addr);
                SR_LFA_ADJ_LIST_NODE_MEM_FREE(pLfaAdjNode);
                break;
            }
        }

        if ((SR_ZERO == TMO_SLL_Count(&(pLfaAdj->LfaList))))
        {
            if((RBTreeRemove (gSrGlobalInfo.pSrLfaAdjRbTree, pLfaAdj))== RB_FAILURE)
            {
                SR_TRC4(SR_CRITICAL_TRC | SR_FAIL_TRC, "\n%s:%d TiLfa Node %x deletion failed for Adj %x\n",
                        __func__, __LINE__, pTiLfaPathInfo->destAddr.Addr.u4Addr,
                        pLfaAdj->AdjIp);
                return;
            }

            SR_TRC3 (SR_UTIL_TRC, "%s:%d Feering LFA Adj Node %x\n",
                               __func__, __LINE__, pLfaAdj->AdjIp.Addr.u4Addr);

            SR_LFA_ADJ_MEM_FREE(pLfaAdj);
        }
    }

    return;
}

/*****************************************************************************
 * Function Name : SrLfaCreateAddAdjListNode
 * Description   : This routine is uses to create Adj List node to add TI-LFA path
 * Input(s)      : pTiLfaPathInfo   - TILFA path Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
tSrAdjLfaListNode   *
SrLfaCreateAddAdjListNode(tSrLfaAdjInfo  *pLfaAdj,
                        tSrTilfaPathInfo   *pTiLfaPathInfo)
{
    tSrAdjLfaListNode   *pLfaAdjNode    = NULL;

    /*Allocate memory for tSrAdjLfaListNode */
    if (SR_LFA_ADJ_LIST_NODE_MEM_ALLOC (pLfaAdjNode) == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d Memory allocation failed for List Node Adj IP:%x\n ",
                 __func__, __LINE__, pLfaAdj->AdjIp.Addr.u4Addr);
        return NULL;
    }

    MEMSET (pLfaAdjNode, SR_ZERO, sizeof (tSrAdjLfaListNode));
    TMO_SLL_Init_Node(&(pLfaAdjNode->adjNode));
    pLfaAdjNode->pLfaPathInfo = pTiLfaPathInfo;
    TMO_SLL_Add (&(pLfaAdj->LfaList), &(pLfaAdjNode->adjNode));

    return pLfaAdjNode;
}

/*****************************************************************************
 * Function Name : SrAddTiLfaPathEntryToLfaAdj
 * Description   : This routine is uses to ADD TI-LFA path info to LFA Adj
 * Input(s)      : pTiLfaPathInfo   - TILFA path Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrAddTiLfaPathEntryToLfaAdj(tSrTilfaPathInfo   *pTiLfaPathInfo)
{
    tSrLfaAdjInfo       LfaTempAdj;
    tSrLfaAdjInfo       *pLfaAdj        = NULL;
    tSrAdjLfaListNode   *pLfaAdjNode    = NULL;
    tTMO_SLL_NODE       *pLfaAdjSid     = NULL;
    tSrTilfaAdjInfo     *pTiLfaAdjInfo  = NULL;

    MEMSET (&LfaTempAdj, SR_ZERO, sizeof (tSrLfaAdjInfo));

    TMO_SLL_Scan (&(pTiLfaPathInfo->pqAdjSidList), pLfaAdjSid, tTMO_SLL_NODE *)
    {
        pTiLfaAdjInfo    = (tSrTilfaAdjInfo *) pLfaAdjSid;
        LfaTempAdj.AdjIp.Addr.u4Addr = pTiLfaAdjInfo->AdjIp.Addr.u4Addr;
        LfaTempAdj.u4AreaId          = pTiLfaAdjInfo->u4AreaId;
        pLfaAdj = RBTreeGet(gSrGlobalInfo.pSrLfaAdjRbTree, &LfaTempAdj);
        if (NULL == pLfaAdj)
        {
            pLfaAdj = SrLfaAdjCreate(pTiLfaAdjInfo);
            if (NULL == pLfaAdj)
            {
                return SR_FAILURE;
            }
        }

        pLfaAdjNode = SrLfaCreateAddAdjListNode(pLfaAdj, pTiLfaPathInfo);
        if (NULL == pLfaAdjNode)
        {
            return SR_FAILURE;
        }
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : TiLfaHandleIpv4RouteAdd
 * Description   : This routine is uses the event info to add TI-LFA route.
 * Input(s)      : tNetIpv4LfaRtInfo   - Event Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
TiLfaHandleIpv4RouteAdd(tNetIpv4LfaRtInfo * pNetIpLfaRtInfo)
{
    tSrTilfaPathInfo   *pSrTilfaPathInfo = NULL;

    pSrTilfaPathInfo = SrCreateTiLfaPathEntry (pNetIpLfaRtInfo);
    if (NULL == pSrTilfaPathInfo)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "\n%s:%d TiLfa path entry creation failed for prefix %x\n",
                 __func__, __LINE__, pNetIpLfaRtInfo->u4DestNet);
        return SR_FAILURE;
    }

    if (SR_FAILURE == SrAddTiLfaPathEntryToLfaNode(pSrTilfaPathInfo))
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "\n%s:%d Adding TiLfaPathEntry to LFA node failed %x\n",
                __func__, __LINE__, pNetIpLfaRtInfo->u4DestNet);
        return SR_FAILURE;
    }

    if (SR_FAILURE == SrAddTiLfaPathEntryToLfaAdj(pSrTilfaPathInfo))
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "\n%s:%d Adding TiLfaPathEntry to LFA node failed %x\n",
                __func__, __LINE__, pNetIpLfaRtInfo->u4DestNet);
        return SR_FAILURE;
    }

    if (SR_FAILURE == SrIpv4TiLfaUpdateLabelStackInfo(pSrTilfaPathInfo))
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d TILFA label stack update failed for dest: %x\n ",
                 __func__, __LINE__, pNetIpLfaRtInfo->u4DestNet);
        return SR_FAILURE;
    }

    if (pSrTilfaPathInfo->u1LfaStatusFlags & SR_TILFA_SET_LABEL_STACK)
    {
        if (SR_FAILURE == SrIpv4TiLfaCreateLSP (pSrTilfaPathInfo))
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Creation of TILFA failed for prefix %x\n ",
                     __func__, __LINE__, pNetIpLfaRtInfo->u4DestNet);
            return SR_FAILURE;
        }
#if 1
        if (SR_FAILURE == SrIpv4TiLfaCreateILM (pSrTilfaPathInfo))
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Creation of TILFA failed for prefix %x\n ",
                     __func__, __LINE__, pNetIpLfaRtInfo->u4DestNet);
            return SR_FAILURE;
        }
#endif
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : TiLfaHandleIpv4RouteDel
 * Description   : This routine is uses the event info to delete TI-LFA route.
 * Input(s)      : pSrIpEvtInfo   - Event Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
TiLfaHandleIpv4RouteDel(tSrTilfaPathInfo   *pSrTilfaPathInfo)
{
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tGenU4Addr          GenAddr;

    GenAddr.u2AddrType  = SR_IPV4_ADDR_TYPE;
    GenAddr.Addr.u4Addr = pSrTilfaPathInfo->destAddr.Addr.u4Addr;

    /* Get the destination SR router info */
    pDestSrRtrInfo = SrGetSrRtrInfoFromNodeId (&GenAddr);
    if (pDestSrRtrInfo == NULL)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d SrRtrInfo is NULL for dest: %x\n",
                __func__, __LINE__, GenAddr.Addr.u4Addr);
    }

    SR_TRC3 (SR_UTIL_TRC, "%s:%d Delete TILFA path info dest: %x\n",
                __func__, __LINE__, GenAddr.Addr.u4Addr);

    if (SR_FTN_CREATED == pSrTilfaPathInfo->u1MPLSStatus)
    {
        if (SR_FAILURE == SrIpv4TiLfaDeleteLSP (pSrTilfaPathInfo))
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Deletion of TILFA failed for prefix %x\n ",
                     __func__, __LINE__, GenAddr.Addr.u4Addr);
            return SR_FAILURE;
        }

#if 1
        if (SR_FAILURE == SrIpv4TiLfaDeleteILM (pSrTilfaPathInfo))
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Deletion of TILFA failed for prefix %x\n ",
                     __func__, __LINE__, GenAddr.Addr.u4Addr);
            return SR_FAILURE;
        }
#endif

    }

    SrTiLfaPathDelete(pSrTilfaPathInfo);

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : TiLfaRtAddOrDeleteEventHandler
 * Description   : This routine is uses the event info and create or delete the
                   TI-LFA route.
 * Input(s)      : tNetIpv4LfaRtInfo   - Event Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
TiLfaRtAddOrDeleteEventHandler (tNetIpv4LfaRtInfo * pNetIpLfaRtInfo,
                               UINT2 u2MlibOperation)
{
    tSrTilfaPathInfo    SrTilfaPathInfo;
    tSrTilfaPathInfo   *pSrTilfaPathInfo = NULL;

    UNUSED_PARAM (u2MlibOperation);

    SR_TRC2 (SR_MAIN_TRC, "%s:%d ENTRY\n", __func__, __LINE__);

    SrTilfaPathInfo.destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    MEMCPY (&SrTilfaPathInfo.destAddr.Addr.u4Addr,
            &pNetIpLfaRtInfo->u4DestNet, sizeof (UINT4));

    pSrTilfaPathInfo = RBTreeGet (gSrGlobalInfo.pSrTilfaPathRbTree,
                                &SrTilfaPathInfo);
    if (NULL == pSrTilfaPathInfo)
    {
        if (pNetIpLfaRtInfo->u1CmdType == TILFA_ROUTE_ADD)
        {
            TiLfaHandleIpv4RouteAdd(pNetIpLfaRtInfo);
        }
        else
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "\n%s:%d Ti-LFA path not found for the delete/modify prefix: %x\n",
                    __func__, __LINE__, pNetIpLfaRtInfo->u4DestNet);
            return SR_FAILURE;
        }
    }
    else
    {
        if (pNetIpLfaRtInfo->u1CmdType == TILFA_ROUTE_DELETE)
        {
            TiLfaHandleIpv4RouteDel(pSrTilfaPathInfo);
        }
        else
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "\n%s:%d Add/Modify received for exisitng Path%x\n",
                    __func__, __LINE__, pNetIpLfaRtInfo->u4DestNet);
            /* Delete exisitng TILFA backup (if exisit) and add new backup */
            TiLfaHandleIpv4RouteDel(pSrTilfaPathInfo);
            TiLfaHandleIpv4RouteAdd(pNetIpLfaRtInfo);

            return SR_SUCCESS;
        }
    }

    SR_TRC2 (SR_MAIN_TRC, "%s:%d EXIT\n", __func__, __LINE__);
    return SR_SUCCESS;
}
/*****************************************************************************
 * Function Name : SrLfaLSPHandle
 * Description   : This routine is used to create or delete LSP for Lfa routes
 *
 *
 * Input(s)      : NONE
 *
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrLfaLSPHandle ()
{
    UINT2               u2MlibOperation = MPLS_MLIB_FTN_CREATE;
    tSrRtrInfo         *pSrRtrInfo = NULL;
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tGenU4Addr          rtrId;
    tSrRtrInfo         *pNewRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    MEMSET (&rtrId, SR_ZERO, sizeof (tGenU4Addr));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY\n", __func__, __LINE__);

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pSrRtrInfo = (tSrRtrInfo *) pRtrNode;

        MEMCPY (&rtrId.Addr.u4Addr, &pSrRtrInfo->prefixId, sizeof (UINT4));
        pNewRtrNode = pSrRtrInfo;
        TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                      pSrRtrNextHopNode, tTMO_SLL_NODE *)
        {
            pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

            if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                SR_FTN_CREATED)
            {
                if (LfaCreateAndDeleteLSP
                    (u2MlibOperation, pSrRtrInfo,
                     pSrRtrNextHopInfo) == SR_FAILURE)
                {
                    SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                             "%s:%d Exiting: Failed to Create Self LFA for Prefix = %x\n",
                             __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
                    return SR_FAILURE;
                }
                else
                {
                    break;
                }
            }

        }

    }

    return SR_FAILURE;
}

#ifdef OSPF3_WANTED
/*****************************************************************************/
/* Function Name : SrRegWithOspf3ForInfoGet                                  */
/* Description   : Function for registering with Ospfv3 to get ABR/ASBR      */
/*                 status, RTR ID                                            */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrRegWithOspf3ForInfoGet (VOID)
{
    tSrOspf3StatusGet   SrOspf3StatusGetInfo;

    MEMSET (&SrOspf3StatusGetInfo, 0, sizeof (tSrOspf3StatusGet));
    SrOspf3StatusGetInfo.pAbrChngfunc = SrOspf3AbrStatusChngHandler;
    SrOspf3StatusGetInfo.pRtrIDChngfunc = SrOspf3RtrIDChngHandler;

    Ospf3RegisterforInfoGet (&SrOspf3StatusGetInfo);
    return;
}

/*****************************************************************************/
/* Function Name : SrDeRegWithOspf3ForInfoGet                                */
/* Description   : Function for registering with Ospfv3 to get ABR/ASBR      */
/*                 status, RTR ID                                            */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrDeRegWithOspf3ForInfoGet (VOID)
{
    Ospf3DeRegisterforInfoGet ();
    return;
}

/*****************************************************************************/
/* Function Name : SrOspf3AbrStatusChngHandler                               */
/* Description   : Function for hangle Abr status change                     */
/* Input(s)      : u1AbrStatus - Abr status                                  */
/* Output(s)     : None                                                      */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrOspf3AbrStatusChngHandler (UINT1 u1AbrStatus)
{
    gSrGlobalInfo.u1Ospf3AbrStatus = u1AbrStatus;
    return;
}

/*****************************************************************************/
/* Function Name : SrOspf3RtrIDChngHandler                                   */
/* Description   : Function for handle Rtr ID status change                  */
/* Input(s)      : u1AsbrStatus - Abr status                                 */
/* Output(s)     : None                                                      */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrOspf3RtrIDChngHandler (UINT4 u4OspfRtrID)
{
    gSrGlobalInfo.u4OspfRtrID = u4OspfRtrID;
    return;
}

/************************************************************************
* Function Name   : SrV6GlobalAdminStatus
* Description     : Function to Throw the error Message
*                   when SR  is disabled
* Input           : NONE
* Output          : None
* Returns         : SR_ENABLED/SR_DISABLED
*************************************************************************/
INT4
SrV6GlobalAdminStatus ()
{
    return (INT4) gSrGlobalInfo.u4SrV6ModuleStatus;
}

/************************************************************************
* Function Name   : SrUtilV3CapabilityOpaqueStatus
* Description     : Function to Throw the error Message
*                   when Capability  is disabled
* Input           : NONE
* Output          : None
*************************************************************************/
INT1
SrUtilV3CapabilityOpaqueStatus (UINT1 *pu1RetVal, UINT4 u4SrCxtId)
{

    if (Ospf3CapabilityOpaqueStatus (pu1RetVal, u4SrCxtId) == SNMP_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to Fetch OSPF3  Opaque Status\n ",
                 __func__, __LINE__);
        return SR_FAILURE;
    }
    return SR_SUCCESS;

}
#endif
#ifdef OSPF_WANTED
/************************************************************************
* Function Name   : SrUtilV2CapabilityOpaqueStatus
* Description     : Function to Throw the error Message
*                   when Capability  is disabled
* Input           : NONE
* Output          : None
*************************************************************************/
INT1
SrUtilV2CapabilityOpaqueStatus (UINT1 *pu1RetVal, UINT4 u4SrCxtId)
{

    if (OspfCapabilityOpaqueStatus (pu1RetVal, u4SrCxtId) == SNMP_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to Fetch OSPF  Opaque Status\n ",
                 __func__, __LINE__);
        return SR_FAILURE;
    }

    return SR_SUCCESS;
}

/************************************************************************
* Function Name   : SrV4GlobalAdminStatus
* Description     : Function to Throw the error Message
*                   when SR  is disabled
* Input           : NONE
* Output          : None
* Returns         : SR_ENABLED/SR_DISABLED
*************************************************************************/
INT4
SrV4GlobalAdminStatus ()
{
    return (INT4) gSrGlobalInfo.u4SrV4ModuleStatus;
}

#endif
/*****************************************************************************/
/* Function Name : SrUtilFetchOspf3AreadID                                   */
/* Description   : Function to fetch OSPF3 area id based on Interface Index  */
/* Input(s)      : u4IfIndex, u4AreaId                                       */
/* Output(s)     : Area ID                                                   */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrUtilFetchOspf3AreadID (UINT4 u4IfIndex, UINT4 *u4AreaId)
{
#ifdef OSPF3_WANTED
    if (Ospf3UtilFetchAreaID ((INT4) u4IfIndex, u4AreaId) == SR_TRUE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to Fetch OSPF3 Area ID\n ",
                 __func__, __LINE__);
    }
#else
    UNUSED_PARAM (u4IfIndex);
    UNUSED_PARAM (u4AreaId);
#endif
    return;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrUtilIpAddrIndComp                                      */
/*                                                                           */
/* Description  : This procedure compares the ip address and the ip addrless */
/*                ind fields in the two interfaces.                          */
/*                                                                           */
/* Input        : ifIpAddr1      : Interface address of the IP address 1   */
/*                u4AddrlessIf1  : Interface Index 1                       */
/*                ifIpAddr2      : Interface address of the IP address 2   */
/*                u4AddrlessIf2  : Interface Index 2                       */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : SR_EQUAL, if they are equal                              */
/*                SR_GREATER, if interface1 is greater                     */
/*                SR_LESS, if interface2 is greater                        */
/*                                                                           */
/*****************************************************************************/
PUBLIC UINT1
SrUtilIpAddrIndComp (tIPADDR ifIpAddr1,
                     UINT4 u4AddrlessIf1, tIPADDR ifIpAddr2,
                     UINT4 u4AddrlessIf2)
{

    UINT1               u1Result;

    u1Result = SrUtilIpAddrComp (ifIpAddr1, ifIpAddr2);
    if (u1Result == SR_EQUAL)
    {
        if (u4AddrlessIf1 > u4AddrlessIf2)
        {
            u1Result = SR_GREATER;
        }
        else if (u4AddrlessIf1 < u4AddrlessIf2)
        {
            u1Result = SR_LESS;
        }
    }
    return u1Result;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrUtilIpAddrComp                                          */
/*                                                                           */
/* Description  : This procedure compares two ip addresses                   */
/*                                                                           */
/* Input        : addr1   :   IP address 1                                   */
/*                addr2   :   IP address 2                                   */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : SR_EQUAL, if the IP addresses are equal                  */
/*                SR_GREATER, if IP address 1 is greater                   */
/*                SR_LESS, if IP address 2 is greater                      */
/*                                                                           */
/*****************************************************************************/
PUBLIC UINT1
SrUtilIpAddrComp (tIPADDR addr1, tIPADDR addr2)
{

    UINT4               u4Addr1 = 0;
    UINT4               u4Addr2 = 0;

    /* returns SR_EQUAL if they are equal
     * SR_GREATER if addr1 is greater
     * SR_LESS if addr1 is lesser
     */

    if ((addr1 == NULL) && (addr2 == NULL))
        return SR_EQUAL;

    if (addr1 == NULL)
        return SR_LESS;
    if (addr2 == NULL)
        return SR_GREATER;

    u4Addr1 = SR_CRU_BMC_DWFROMPDU (addr1);
    u4Addr2 = SR_CRU_BMC_DWFROMPDU (addr2);

    if (u4Addr1 > u4Addr2)
        return SR_GREATER;
    else if (u4Addr1 < u4Addr2)
        return SR_LESS;

    return (SR_EQUAL);
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrUtilOspfDwordToPdu                                         */
/*                                                                           */
/* Description  : This procedure converts Address which is in Host Byte      */
/*                Order to Network Byte Order                                */
/*                                                                           */
/* Input        : u4Value      : IP Address in Host Byte Order               */
/*                                                                           */
/* Output       : pu1PduAddr                                                 */
/*                                                                           */
/* Returns      : VOID                                                       */
/*                                                                           */
/*****************************************************************************/
PUBLIC VOID
SrUtilOspfDwordToPdu (UINT1 *pu1PduAddr, UINT4 u4Value)
{
    UINT4               u4TempData = 0;
    u4TempData = OSIX_HTONL (u4Value);
    MEMCPY (pu1PduAddr, &u4TempData, MAX_IPV4_ADDR_LEN);
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrUtilOspfDwordFromPdu                                       */
/*                                                                           */
/* Description  : This procedure converts Address which is in Network Byte   */
/*                Order to Host Byte Order and returns the same              */
/*                                                                           */
/* Input        : pu1PduAddr   : IP address in Network Byte Order            */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : IP address in Host Byte Order                              */
/*                                                                           */
/*****************************************************************************/
PUBLIC UINT4
SrUtilOspfDwordFromPdu (UINT1 *pu1PduAddr)
{
    UINT4               u4TempData = 0;
    MEMCPY (&u4TempData, pu1PduAddr, MAX_IPV4_ADDR_LEN);
    return (OSIX_NTOHL (u4TempData));
}

/*****************************************************************************/
/* Function Name : SrV3EnableAlternate                                       */
/* Description   : This function is used to create the Backup LSP            */
/*                  for the LFA/RLFA v3 routes                               */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : SR_SUCCESS/SR_FAILURE                                     */
/*****************************************************************************/
INT4
SrV3EnableAlternate (VOID)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRlfaPathInfo     SrRlfaPathInfo;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tGenU4Addr          DestAddr;
    tGenU4Addr          MandatoryRtrId;
    UINT2               u2MlibOperation = MPLS_MLIB_FTN_CREATE;

    MEMSET (&SrRlfaPathInfo, SR_ZERO, sizeof (tSrRlfaPathInfo));
    MEMSET (&DestAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&MandatoryRtrId, SR_ZERO, sizeof (tGenU4Addr));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d: Entry\n", __func__, __LINE__);

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;
        TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                      pSrRtrNextHopNode, tTMO_SLL_NODE *)
        {
            pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
            if (SrV3LfaFetchAndCreateLSP
                (u2MlibOperation, pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d creation of IPv6 LFA LSP failed \n",
                         __func__, __LINE__);
            }
        }
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrIpv6RlfaFetchAndCreateLSP
 * Description   : This routine update the RLFA route info in TE-PATH
                   RB-tree and create the RLFA LSP
 *
 * Input(s)      : pNetIpv6LfaRtInfo  - Event Info
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrIpv6RlfaFetchAndCreateLSP (tNetIpv6LfaRtInfo * pNetIpv6LfaRtInfo)
{
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tGenU4Addr          DestAddr;
    tGenU4Addr          MandatoryRtrId;
    tGenU4Addr          NextHop;
    tSrRtrInfo         *pSrRtrInfo = NULL;
    tSrRtrInfo         *pTunnelRtrInfo = NULL;

    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4DestMask = SR_ZERO;
    UINT1               u1TunnelFlag = SR_ZERO;

    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&DestAddr, 0, sizeof (tGenU4Addr));
    MEMSET (&NextHop, 0, sizeof (tGenU4Addr));
    MEMSET (&MandatoryRtrId, 0, sizeof (tGenU4Addr));

    NextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
    MEMCPY (&(NextHop.Addr.Ip6Addr),
            &(pNetIpv6LfaRtInfo->AltRlfaNxtHop), sizeof (tIp6Addr));

    pSrRtrInfo = SrGetRtrInfoFromNextHop (&NextHop);
    if (pSrRtrInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrGetRtrInfoFromNextHop Failure:"
                 " pSrRtrInfo == NULL \n", __func__, __LINE__);
        return SR_FAILURE;
    }
    DestAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
    MEMCPY (&(DestAddr.Addr.Ip6Addr),
            &(pNetIpv6LfaRtInfo->Ip6Dst), sizeof (tIp6Addr));
    u4DestMask = pNetIpv6LfaRtInfo->u1Prefixlen;
    u4OutIfIndex = pNetIpv6LfaRtInfo->u4AltIfIndex;

    /*Scan and get the router details of RLFA remote node */
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pTunnelRtrInfo, tSrRtrInfo *)
    {
        if (pTunnelRtrInfo->advRtrId.Addr.u4Addr ==
            pNetIpv6LfaRtInfo->uRemoteNodeId.u4RemNodeRouterId)
        {
            u1TunnelFlag = SR_ONE;
            break;
        }
    }
    if ((u1TunnelFlag != SR_ONE) || (pTunnelRtrInfo == NULL))
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to get the router"
                 " details of RLFA remote node \n", __func__, __LINE__);
        return SR_FAILURE;
    }

    MandatoryRtrId.u2AddrType = SR_IPV6_ADDR_TYPE;
    MEMCPY (&(MandatoryRtrId.Addr.Ip6Addr),
            &(pTunnelRtrInfo->prefixId.Addr.Ip6Addr), sizeof (tIp6Addr));

    if (SrRlfaSetTeTunnel (&DestAddr, u4DestMask, &MandatoryRtrId,
                           &NextHop, u4OutIfIndex) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Creation of SR RLFA failed\n ", __func__,
                 __LINE__);
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrV3RLfaRtAddOrDeleteEventHandler
 * Description   : This routine is uses the event info and create or delete the
                   LFA route.
 *
 *
 * Input(s)      : pIpv6LfaRtInfo   - Event Info
                   u2MlibOperation  - FTN_CREATE/FTN_DELETE etc.
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrV3RLfaRtAddOrDeleteEventHandler (tNetIpv6LfaRtInfo * pNetIpv6LfaRtInfo,
                                   UINT2 u2MlibOperation)
{
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrInSegInfo        SrInSegInfo;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tGenU4Addr          DestAddr;
    tGenU4Addr          NextHop;
    UINT4               u4SrGbMinIndex = SR_ZERO;

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&DestAddr, 0, sizeof (tGenU4Addr));
    MEMSET (&NextHop, 0, sizeof (tGenU4Addr));
    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));

    MEMCPY (DestAddr.Addr.Ip6Addr.u1_addr, pNetIpv6LfaRtInfo->Ip6Dst.u1_addr,
            SR_IPV6_ADDR_LENGTH);
    DestAddr.u2AddrType = SR_IPV6_ADDR_TYPE;

    if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
    {
        u4SrGbMinIndex = gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;

        SrInSegInfo.Fec.u2AddrFmly =
            gSrGlobalInfo.pV6SrSelfNodeInfo->ipAddrType;
        MEMCPY (SrInSegInfo.Fec.Prefix.Ip6Addr.u1_addr,
                gSrGlobalInfo.pV6SrSelfNodeInfo->ifIpAddr.Addr.Ip6Addr.u1_addr,
                MAX_IPV6_ADDR_LEN);
    }

    /* RLFA route info are update in Te-PATH RBTree and create MPLS FTN and ILM entry in control plane.
     * when RLFA_ROUTE_SET event received, Programming the RLFA routes and its LSP in Hw
     * for the Primary Route */
    if (pNetIpv6LfaRtInfo->u1CmdType == IPv6_RLFA_ROUTE_ADD)
    {
        if (SrIpv6RlfaFetchAndCreateLSP (pNetIpv6LfaRtInfo) == SR_FAILURE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Creation of SR RLFA failed\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        /*Get SrRtrInfo for DestAddr for which ILM is to be created */
        if ((pDestSrRtrInfo = SrGetSrRtrInfoFromRtrId (&DestAddr)) == NULL)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrGetSrRtrInfoFromRtrId FAILURE \n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        if (SrIpv6RlfaILMCreation (pNetIpv6LfaRtInfo, pDestSrRtrInfo) ==
            SR_FAILURE)
        {
            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Creation of SR RLFA ILM failed \n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
    }
    /*If RLFA path is Active, do not delete the LSP */
    if (pNetIpv6LfaRtInfo->u1CmdType == IPv6_RLFA_ROUTE_DELETE)
    {
        if (u2MlibOperation == MPLS_MLIB_FTN_DELETE)
        {
            pSrRlfaPathInfo = SrGetRlfaPathEntryFromDestAddr (&DestAddr);
            if (pSrRlfaPathInfo != NULL)
            {
                if (pSrRlfaPathInfo->bIsRlfaActive == SR_ONE)
                {
                    SR_TRC2 (SR_UTIL_TRC, "%s:%d SR RLFA PATH is Active \n",
                             __func__, __LINE__);
                    return SR_SUCCESS;
                }
                /* RLFA route is not active, hence delete the entries */
                MEMCPY (NextHop.Addr.Ip6Addr.u1_addr,
                        pNetIpv6LfaRtInfo->AltRlfaNxtHop.u1_addr,
                        SR_IPV6_ADDR_LENGTH);
                NextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
                if (SrRlfaDeleteFtnEntry (&DestAddr, &NextHop) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d Deletion of SR RLFA FTN Failure \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
                if (SrRlfaDeleteILM (&DestAddr, &pSrRlfaPathInfo->mandRtrId,
                                     &pSrRlfaPathInfo->rlfaNextHop,
                                     &SrInSegInfo) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                             "%s:%d Deletion of SR RLFA ILM Failure \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
            }
        }
    }
    /*if primary path goes down, delete the primary LSPs and program the RLFA-LSP if present */
    if ((pNetIpv6LfaRtInfo->u1CmdType == IPv6_RLFA_ROUTE_SET)
        && (u2MlibOperation == MPLS_MLIB_FTN_DELETE))
    {
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrInfo, tSrRtrInfo *)
        {
            TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                          pSrRtrNextHopInfo, tSrRtrNextHopInfo *)
            {
                if ((MEMCMP (pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                             pNetIpv6LfaRtInfo->NextHop.u1_addr,
                             SR_IPV6_ADDR_LENGTH) == SR_ZERO))
                {
                    if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                        SR_FTN_CREATED)
                    {
                        MEMSET (&DestAddr, 0, sizeof (tGenU4Addr));
                        DestAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
                        MEMCPY (&(DestAddr.Addr.Ip6Addr),
                                &(pRtrInfo->prefixId.Addr.Ip6Addr),
                                sizeof (tIp6Addr));

                        pSrRlfaPathInfo =
                            SrGetRlfaPathEntryFromDestAddr (&DestAddr);
                        if (pSrRlfaPathInfo != NULL)
                        {
                            pSrRlfaPathInfo->bIsRlfaActive = SR_ONE;
                        }

                        if (gSrGlobalInfo.pV6SrSelfNodeInfo != NULL)
                        {
                            /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
                            SrInSegInfo.u4InLabel =
                                (u4SrGbMinIndex + pRtrInfo->u4SidValue);

                            SR_TRC2 (SR_UTIL_TRC, "%s:%d Deleting ILM SWAP \n",
                                     __func__, __LINE__);

                            if (SrMplsDeleteILM
                                (&SrInSegInfo, pRtrInfo,
                                 pSrRtrNextHopInfo) == SR_FAILURE)
                            {
                                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                         "%s:%d SrMplsDeleteILM  FAILURE \n",
                                         __func__, __LINE__);
                                return SR_FAILURE;
                            }
                        }

                        /* Deleting the Primary LSP */
                        pSrRtrNextHopInfo->u1FRRNextHop = SR_PRIMARY_NEXTHOP;
                        pSrRtrNextHopInfo->bIsFRRHwLsp = SR_FALSE;
                        MEMCPY (&pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr,
                                &pNetIpv6LfaRtInfo->NextHop, sizeof (tIp6Addr));
                        MEMCPY (&pSrRtrNextHopInfo->PrimarynextHop.Addr.Ip6Addr,
                                &pNetIpv6LfaRtInfo->NextHop, sizeof (tIp6Addr));

                        if (SrMplsDeleteFTN (pRtrInfo, pSrRtrNextHopInfo) ==
                            SR_FAILURE)
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SrMplsDeleteFTN FAILURE \n", __func__,
                                     __LINE__);
                            return SR_FAILURE;
                        }

                        /* Remove SrRtrNhNode from NextHopList */
                        TMO_SLL_Delete (&(pRtrInfo->NextHopList),
                                        (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
                        SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);

                        /*Programm the RLFA FTN in HW */
                        if (SrRlfaModifyFtnEntry (&pRtrInfo->prefixId) ==
                            SR_SUCCESS)
                        {
                            SR_TRC2 (SR_UTIL_TRC,
                                     "%s:%d SrRlfaModifyFtnEntry  SUCCESS \n",
                                     __func__, __LINE__);
                        }
                        else
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d SrRlfaModifyFtnEntry  FAILURE \n",
                                     __func__, __LINE__);
                            return SR_FAILURE;
                        }
                        /*Programm the RLFA ILM in HW */
                        if (SrRlfaCreateILM (&pRtrInfo->u4LfaInLabel) !=
                            SR_FAILURE)
                        {
                            SR_TRC2 (SR_UTIL_TRC, "%s:%d RLFA ILMs activated\n",
                                     __func__, __LINE__);
                            return SR_SUCCESS;
                        }
                        else
                        {
                            SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                     "%s:%d RLFA ILM is Not activated\n",
                                     __func__, __LINE__);
                            return SR_FAILURE;
                        }
                    }

                    break;
                }
                else
                {
                    SR_TRC3 (SR_UTIL_TRC,
                             "%s:%d PeerId do not matches with Route Delete Event - %s \n",
                             __func__, __LINE__,
                             Ip6PrintAddr (&pSrRtrNextHopInfo->nextHop.Addr.
                                           Ip6Addr));
                }

            }
        }
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrRlfaModifyFtnEntry
 * Description   : This routine programme the RLFA LSP entry in hardware
 *
 * Input(s)      : pDestAddr - Destination Addr
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
INT4
SrRlfaModifyFtnEntry (tGenU4Addr * pDestAddr)
{
    tSrRlfaPathInfo     SrRlfaPathInfo;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrRtrInfo         *pNextHopSrRtrInfo = NULL;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrTeLblStack       SrTeLblStack;
    tGenU4Addr          nextHopAddr;
    tGenU4Addr          dummyAddr;
    UINT4               u4TopLabel = SR_ZERO;
    UINT4               u4DestLabel = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    UINT2               u2MlibOperation = MPLS_MLIB_FTN_MODIFY;
    UINT1               u1NHFound = SR_FALSE;

    MEMSET (&SrRlfaPathInfo, SR_ZERO, sizeof (tSrRlfaPathInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));

    pSrRlfaPathInfo = SrGetRlfaPathEntryFromDestAddr (pDestAddr);
    if (pSrRlfaPathInfo != NULL)
    {
        /* Get the Label of Rlfa Remote Node through alternate Next hop */
        pNextHopSrRtrInfo =
            SrGetRtrInfoFromNextHop (&pSrRlfaPathInfo->rlfaNextHop);
        if (pNextHopSrRtrInfo == NULL)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pNextHopSrRtrInfo not found\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        SrGetLabelForRLFARemoteNode (&pSrRlfaPathInfo->mandRtrId,
                                     &pNextHopSrRtrInfo->srgbRange.
                                     u4SrGbMinIndex, &u4TopLabel);

        if (SrTeGetLabelForRtrId (&pSrRlfaPathInfo->destAddr,
                                  &pSrRlfaPathInfo->mandRtrId,
                                  &u4DestLabel) == SR_FAILURE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Unable to get u4DestLabel\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

        /* Set the TopLabel and the following Label Stack
         * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
        SrTeLblStack.u4TopLabel = u4TopLabel;
        SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;
        SrTeLblStack.u1StackSize = SR_ONE;

        /*Get SrRtrInfo for DestAddr for which TE path is to be created */
        if ((pDestSrRtrInfo =
             SrGetSrRtrInfoFromRtrId (&pSrRlfaPathInfo->destAddr)) == NULL)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrGetSrRtrInfoFromRtrId FAILURE \n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

        TMO_SLL_Scan (&(pDestSrRtrInfo->NextHopList),
                      pNewSrRtrNextHopInfo, tSrRtrNextHopInfo *)
        {
            if ((pNewSrRtrNextHopInfo->bIsLfaNextHop == SR_TRUE)
                && (pNewSrRtrNextHopInfo->u1FRRNextHop ==SR_RLFA_NEXTHOP))
            {
                u1NHFound = SR_TRUE;
                break;
            }
        }
        if (u1NHFound != SR_TRUE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d RLFA nexthop Not found  \n", __func__,
                     __LINE__);
            return SR_FAILURE;
        }
        pNewSrRtrNextHopInfo->bIsFRRModifyLsp = SR_TRUE;
        /* MODIFY FTN - Programm the RLFA entries in Hardware */
        if (SrMplsCreateOrDeleteStackLsp
            (u2MlibOperation, pDestSrRtrInfo, NULL, &SrTeLblStack,
             pNewSrRtrNextHopInfo, SR_FALSE) == SR_FAILURE)
        {
            /* Delete all the resources reserved on FAILURE & reset OutIfIndex */
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrMplsCreateOrDeleteLsp SR_FAILURE \n",
                     __func__, __LINE__);
            u4MplsTnlIfIndex = pNewSrRtrNextHopInfo->u4OutIfIndex;
#ifdef CFA_WANTED
            if (CfaUtilGetIfIndexFromMplsTnlIf
                (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
            {
                if (CfaIfmDeleteStackMplsTunnelInterface
                    (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                {
                    SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                             "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                             __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                    return SR_FAILURE;
                }
            }
#endif
            return SR_FAILURE;

        }
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrRlfaCreateFtnEntry
 * Description   : This routine Create the RLFA LSP entry
 *
 *
 * Input(s)      : pDestAddr - Destination Addr
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
INT4
SrRlfaCreateFtnEntry (tGenU4Addr * pDestAddr)
{
    tSrRlfaPathInfo     SrRlfaPathInfo;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrRtrInfo         *pDestSrRtrInfo = NULL;
    tSrRtrInfo         *pRemoteSrRtrInfo = NULL;
    tSrRtrInfo         *pNextHopSrRtrInfo = NULL;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrTeLblStack       SrTeLblStack;
    tGenU4Addr          nextHopAddr;
    tGenU4Addr          PrimaryNextHopAddr;
    tGenU4Addr          dummyAddr;
    UINT4               u4IfIndex = SR_ZERO;
    UINT4               u4TopLabel = SR_ZERO;
    UINT4               u4DestLabel = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    UINT2               u2MlibOperation = MPLS_MLIB_FTN_CREATE;
    MEMSET (&SrRlfaPathInfo, SR_ZERO, sizeof (tSrRlfaPathInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&nextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&PrimaryNextHopAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));

    pSrRlfaPathInfo = SrGetRlfaPathEntryFromDestAddr (pDestAddr);
    if (pSrRlfaPathInfo != NULL)
    {
        /* Continuing Case 1: If this function is called with DestAddr IP */
        if (pSrRlfaPathInfo->u1RowStatus != MPLS_STATUS_ACTIVE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d TePathEntry not ACTIVE\n", __func__,
                     __LINE__);
            return SR_FAILURE;
        }
        /* Get the Label of Rlfa Remote Node through alternate Next hop */
        pNextHopSrRtrInfo =
            SrGetRtrInfoFromNextHop (&pSrRlfaPathInfo->rlfaNextHop);
        if (pNextHopSrRtrInfo == NULL)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pNextHopSrRtrInfo not found\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

        pRemoteSrRtrInfo = SrGetSrRtrInfoFromRtrId (&pSrRlfaPathInfo->mandRtrId);
        if (pRemoteSrRtrInfo == NULL)
        {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                "%s:%dFor SR Dest %x RLFA Remote Node %x is not SR capable \n",
                __func__, __LINE__, pDestAddr->Addr.u4Addr,
                pSrRlfaPathInfo->mandRtrId);
            pSrRlfaPathInfo->u4RemoteLabel = 0;
            return SR_FAILURE;
        }

        if (pSrRlfaPathInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            SrGetLabelForRLFARemoteNode (&pSrRlfaPathInfo->mandRtrId,
                                         &pNextHopSrRtrInfo->srgbRange.
                                         u4SrGbMinIndex, &u4TopLabel);
        }
        else if (pSrRlfaPathInfo->destAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            SrGetLabelForRLFARemoteNode (&pSrRlfaPathInfo->mandRtrId,
                                         &pNextHopSrRtrInfo->srgbRange.
                                         u4SrGbV3MinIndex, &u4TopLabel);
        }
        if (SrTeGetLabelForRtrId (&pSrRlfaPathInfo->destAddr,
                                  &pSrRlfaPathInfo->mandRtrId,
                                  &u4DestLabel) == SR_FAILURE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Unable to get u4DestLabel\n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }
        pSrRlfaPathInfo->u4RemoteLabel = u4TopLabel;
        pSrRlfaPathInfo->u4DestLabel = u4DestLabel;

        if ((pSrRlfaPathInfo->u1MPLSStatus & SR_FTN_CREATED) != SR_FTN_CREATED)
        {
            /* Set the TopLabel and the following Label Stack
             * (TopLabel->LabelStack[0]->LabelStack[1].....LabelStack[6])*/
            SrTeLblStack.u4TopLabel = u4TopLabel;
            SrTeLblStack.u4LabelStack[SR_ZERO] = u4DestLabel;
            SrTeLblStack.u1StackSize = SR_ONE;

            /*Get SrRtrInfo for DestAddr for which TE path is to be created */
            if ((pDestSrRtrInfo =
                 SrGetSrRtrInfoFromRtrId (&pSrRlfaPathInfo->destAddr)) == NULL)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrGetSrRtrInfoFromRtrId FAILURE \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }

            /*Create new Next-Hop Node */
            if ((pNewSrRtrNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d pNewSrRtrNextHopInfo == NULL \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }

            /*Get Primary NextHop for the prefix */
            if (SrGetNHForPrefix
                (&pSrRlfaPathInfo->destAddr, &PrimaryNextHopAddr,
                 &u4IfIndex) == SR_FAILURE)
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrGetNHForPrefix FAILURE \n",
                         __func__, __LINE__);
                SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                return SR_FAILURE;
            }
            pNewSrRtrNextHopInfo->bIsLfaNextHop = SR_TRUE;
            pDestSrRtrInfo->u1LfaType = SR_RLFA_ROUTE;
            pDestSrRtrInfo->u4LfaNextHop =
                pSrRlfaPathInfo->rlfaNextHop.Addr.u4Addr;
            if (pSrRlfaPathInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                MEMCPY (&(pNewSrRtrNextHopInfo->PrimarynextHop.Addr.u4Addr),
                        &(PrimaryNextHopAddr.Addr.u4Addr), SR_IPV4_ADDR_LENGTH);
                pNewSrRtrNextHopInfo->u1FRRNextHop = SR_RLFA_NEXTHOP;
            }
            else
            {
                MEMCPY (&pNewSrRtrNextHopInfo->PrimarynextHop.Addr.Ip6Addr,
                        &PrimaryNextHopAddr.Addr.Ip6Addr, sizeof (tIp6Addr));
            }
            if ((SrUtilCreateMplsTunnelInterface
                 (pSrRlfaPathInfo->rlfaOutIfIndex,
                  pSrRlfaPathInfo->destAddr.u2AddrType,
                  &pSrRlfaPathInfo->rlfaNextHop,
                  pNewSrRtrNextHopInfo)) == SR_FAILURE)
            {
                SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC,
                        "Failed to create MPLS tunnel Interface \r\n");
                if (SrDeleteRlfaPathTableEntry (pSrRlfaPathInfo) == SR_FAILURE)
                {
                    SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                             "%s:%d RLFA RBTree deletion failure \n", __func__,
                             __LINE__);
                    SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                    return SR_FAILURE;
                }
                SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                return SR_FAILURE;
            }
            pSrRlfaPathInfo->u4RlfaTunIndex =
                pNewSrRtrNextHopInfo->u4OutIfIndex;
            /* Add NextHop Node in NextHopList in SrRtrNode */
            TMO_SLL_Insert (&(pDestSrRtrInfo->NextHopList), NULL,
                            &(pNewSrRtrNextHopInfo->nextNextHop));

            /* Create FTN */
            if (SrMplsCreateOrDeleteStackLsp
                (u2MlibOperation, pDestSrRtrInfo, NULL, &SrTeLblStack,
                 pNewSrRtrNextHopInfo, SR_TRUE) == SR_FAILURE)
            {
                /* Delete all the resources reserved on FAILURE & reset OutIfIndex */
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d SrMplsCreateOrDeleteLsp SR_FAILURE \n", __func__,
                         __LINE__);
#ifdef CFA_WANTED
                if (CfaUtilGetIfIndexFromMplsTnlIf
                    (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE) != CFA_FAILURE)
                {
                    if (CfaIfmDeleteStackMplsTunnelInterface
                        (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
                    {
                        SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                                 "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                                 __func__, __LINE__, u4MplsTnlIfIndex,
                                 u4L3VlanIf);
                        return SR_FAILURE;
                    }
                }
                SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC,
                        "Failed to get MPLS tunnel Interface \r\n");
                pNewSrRtrNextHopInfo->u4OutIfIndex = SR_ZERO;
#endif
                TMO_SLL_Delete (&(pDestSrRtrInfo->NextHopList),
                                (tTMO_SLL_NODE *) pNewSrRtrNextHopInfo);
                SR_RTR_NH_LIST_FREE (pNewSrRtrNextHopInfo);
                return SR_FAILURE;
            }
            /*Update the FTN status */
            pSrRlfaPathInfo->u1MPLSStatus |= SR_FTN_CREATED;
        }
        else
        {
            SR_TRC2 (SR_UTIL_TRC, "%s:%d FTN entry Already Created \n", __func__,
                     __LINE__);
            return SR_SUCCESS;
        }
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrV3TiLfaRtAddOrDeleteEventHandler
 * Description   : This routine is uses the event info and create or delete the
                   TI-LFA route.
 * Input(s)      : pIpv6LfaRtInfo   - Event Info
                   u2MlibOperation  - FTN_CREATE/FTN_DELETE etc.
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4 SrV3TiLfaRtAddOrDeleteEventHandler (tNetIpv6LfaRtInfo * pNetIpv6LfaRtInfo,
                                   UINT2 u2MlibOperation)
{
    UNUSED_PARAM (pNetIpv6LfaRtInfo);
    UNUSED_PARAM (u2MlibOperation);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrUtilCreateMplsTunnelInterface
 * Description   : This routine is used to create MPLS tunnel interface
 *                  for the given interface
 *
 * Input(s)      : pDestAddr - Destination Addr
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrUtilCreateMplsTunnelInterface (UINT4 u4OutIfIndex, UINT2 u2AddrType,
                                 tGenU4Addr * pNextHop,
                                 tSrRtrNextHopInfo * pNewSrRtrNextHopInfo)
{
    UINT4               u4MplsTnlIfIndex = SR_ZERO;

    if (u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        MEMCPY (&(pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr),
                &(pNextHop->Addr.u4Addr), SR_IPV4_ADDR_LENGTH);
        pNewSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
    }
    else
    {
        MEMCPY (&pNewSrRtrNextHopInfo->nextHop.Addr.Ip6Addr,
                &pNextHop->Addr.Ip6Addr, sizeof (tIp6Addr));
        pNewSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
    }

#ifdef CFA_WANTED
    /*u4OutIfIndex should contain OutInterface eg: 0/2 = 2, 0/3 = 3 */
    if (CfaIfmCreateStackMplsTunnelInterface (u4OutIfIndex,
                                              &u4MplsTnlIfIndex) == CFA_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC,
                 "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                 __func__, __LINE__, u4OutIfIndex, u4MplsTnlIfIndex);
        pNewSrRtrNextHopInfo->u4OutIfIndex = u4MplsTnlIfIndex;
        return SR_SUCCESS;
    }
    else
    {
        SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                 "%s:%d u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                 __func__, __LINE__, u4OutIfIndex, u4MplsTnlIfIndex);
        return SR_FAILURE;
    }
#endif
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrRlfaDeleteFtnEntry
 * Description   : This routine Delete the RLFA LSP entry
 *
 *
 * Input(s)      : pDestAddr - Destination Addr
                   pNextHop  - RLFA NextHop
 *
 * Output(s)     :
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
INT4
SrRlfaDeleteFtnEntry (tGenU4Addr * pDestAddr, tGenU4Addr * pNextHop)
{
    tSrRlfaPathInfo     SrRlfaPathInfo;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrRtrInfo         *pSrRtrInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrTeLblStack       SrTeLblStack;
    tGenU4Addr          tmpNextHop;
    tGenU4Addr          dummyAddr;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4L3VlanIf = SR_ZERO;
    UINT2               u2MlibOperation = MPLS_MLIB_FTN_DELETE;
    UINT1               u1NHFound = SR_FALSE;
    MEMSET (&SrRlfaPathInfo, SR_ZERO, sizeof (tSrRlfaPathInfo));
    MEMSET (&SrTeLblStack, SR_ZERO, sizeof (tSrTeLblStack));
    MEMSET (&tmpNextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, SR_ZERO, sizeof (tGenU4Addr));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Entry \n", __func__, __LINE__);

    pSrRlfaPathInfo = SrGetRlfaPathEntryFromDestAddr (pDestAddr);
    if (pSrRlfaPathInfo != NULL)
    {
        /* Get the SrRtrId */
        if ((pSrRtrInfo = SrGetSrRtrInfoFromRtrId (pDestAddr)) == NULL)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrGetSrRtrInfoFromRtrId FAILURE \n",
                     __func__, __LINE__);
            return SR_FAILURE;
        }

        TMO_SLL_Scan (&(pSrRtrInfo->NextHopList),
                      pSrRtrNextHopInfo, tSrRtrNextHopInfo *)
        {
            if (pNextHop->u2AddrType == SR_IPV4_ADDR_TYPE)
            {
                if ((MEMCMP
                     (&(pNextHop->Addr.u4Addr),
                      &(pSrRtrNextHopInfo->nextHop.Addr.u4Addr),
                      SR_IPV4_ADDR_LENGTH) == SR_ZERO)
                    && ((pSrRtrNextHopInfo->bIsLfaNextHop == SR_TRUE)
                        &&(pSrRtrNextHopInfo->u1FRRNextHop == SR_RLFA_NEXTHOP)))
                {
                    u1NHFound = SR_TRUE;
                    break;
                }
            }
            if (pNextHop->u2AddrType == SR_IPV6_ADDR_TYPE)
            {
                if ((MEMCMP (pNextHop->Addr.Ip6Addr.u1_addr,
                             pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                             SR_IPV6_ADDR_LENGTH) == SR_ZERO) &&
                    ((pSrRtrNextHopInfo->bIsLfaNextHop == SR_TRUE)
                    &&(pSrRtrNextHopInfo->u1FRRNextHop == SR_RLFA_NEXTHOP)))
                {
                    u1NHFound = SR_TRUE;
                    break;
                }
            }
        }

        if (u1NHFound == SR_FALSE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d  u1NHFound == SR_FALSE \n", __func__,
                     __LINE__);
            return SR_FAILURE;
        }

        if ((pSrRlfaPathInfo->u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
        {
            pSrRtrNextHopInfo->u4SwapOutIfIndex =
                pSrRlfaPathInfo->u4RlfaIlmTunIndex;
            if (SrMplsCreateOrDeleteStackLsp
                (u2MlibOperation, pSrRtrInfo, NULL, &SrTeLblStack, pSrRtrNextHopInfo,
                 SR_TRUE) == SR_FAILURE)
            {
                pSrRtrNextHopInfo->u4SwapOutIfIndex = SR_ZERO;
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d SrMplsCreateOrDeleteStackLsp FAILURE\n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
            pSrRtrNextHopInfo->u4SwapOutIfIndex = SR_ZERO;
        }
#ifdef CFA_WANTED
        u4MplsTnlIfIndex = pSrRtrNextHopInfo->u4OutIfIndex;
        if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4L3VlanIf, TRUE)
            != CFA_FAILURE)
        {

            if (CfaIfmDeleteStackMplsTunnelInterface
                (u4L3VlanIf, u4MplsTnlIfIndex) == CFA_FAILURE)
            {
                SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d MPLS Tunnel IF %d L3IF %d deletion Failed \n",
                         __func__, __LINE__, u4MplsTnlIfIndex, u4L3VlanIf);
                return SR_FAILURE;
            }
        }
#endif
        pSrRlfaPathInfo->u1MPLSStatus &= (UINT1) (~SR_FTN_CREATED);

    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : LfaIpv6RtChangeEventHandler
 * Description   : This routine post the LFA routes to SR Main queue
 *
 * Input(s)      : pNetIp6LfaRtInfo - IPv6 Lfa Route Info
 *
 * Output(s)     :
 * Return(s)     : None
 *****************************************************************************/
VOID
LfaIpv6RtChangeEventHandler (tNetIpv6LfaRtInfo * pNetIp6LfaRtInfo)
{

    tSrQMsg             SrQMsg;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY\n", __func__, __LINE__);
    if (pNetIp6LfaRtInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d FAILING pIpv6LfaRtInfo is NULL\n",
                 __func__, __LINE__);
        return;
    }

    MEMCPY (&(SrQMsg.u.SrNetIpv6LfaRtInfo.Ip6Dst),
            &pNetIp6LfaRtInfo->Ip6Dst, sizeof (tIp6Addr));
    MEMCPY (&(SrQMsg.u.SrNetIpv6LfaRtInfo.NextHop),
            &pNetIp6LfaRtInfo->NextHop, sizeof (tIp6Addr));
    MEMCPY (&(SrQMsg.u.SrNetIpv6LfaRtInfo.LfaNextHop),
            &pNetIp6LfaRtInfo->LfaNextHop, sizeof (tIp6Addr));
    MEMCPY (&(SrQMsg.u.SrNetIpv6LfaRtInfo.AltRlfaNxtHop),
            &pNetIp6LfaRtInfo->AltRlfaNxtHop, sizeof (tIp6Addr));

    SrQMsg.u.SrNetIpv6LfaRtInfo.u1Prefixlen = pNetIp6LfaRtInfo->u1Prefixlen;
    SrQMsg.u.SrNetIpv6LfaRtInfo.u4Index = pNetIp6LfaRtInfo->u4Index;
    SrQMsg.u.SrNetIpv6LfaRtInfo.uRemoteNodeId.u4RemNodeRouterId =
        pNetIp6LfaRtInfo->uRemoteNodeId.u4RemNodeRouterId;
    SrQMsg.u.SrNetIpv6LfaRtInfo.u4Metric = pNetIp6LfaRtInfo->u4Metric;
    SrQMsg.u.SrNetIpv6LfaRtInfo.u4AltCost = pNetIp6LfaRtInfo->u4AltCost;
    SrQMsg.u.SrNetIpv6LfaRtInfo.u1CmdType = pNetIp6LfaRtInfo->u1CmdType;
    SrQMsg.u.SrNetIpv6LfaRtInfo.i1Proto = pNetIp6LfaRtInfo->i1Proto;
    SrQMsg.u.SrNetIpv6LfaRtInfo.u4AltIfIndex = pNetIp6LfaRtInfo->u4AltIfIndex;
    SrQMsg.u.SrNetIpv6LfaRtInfo.u1LfaType = pNetIp6LfaRtInfo->u1LfaType;

    SrQMsg.u.SrNetIpv6LfaRtInfo.u4ContextId = pNetIp6LfaRtInfo->u4ContextId;

    SrQMsg.u4MsgType = SR_NETIP6_EVENT;

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to EnQ IP Event to SR Task \n",
                 __func__, __LINE__);
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrIsFlushLsa                                               */
/*                                                                           */
/* Description  : This function deocdes the received opaque lsa. If the Sid  */
/*                value is set to zero, delete all the details related to    */
/*                the node prefix                                            */
/*                                                                           */
/* Input        : pOpqLSAInfo                                                */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : SR_TRUE/SR_FALSE                                           */
/*****************************************************************************/
PUBLIC UINT1
SrIsFlushLsa (tOsLsaInfo * pOsLsaInfo)
{
    UINT1              *pTlvPtr = NULL;
    UINT1              *pTemp = NULL;
    UINT1               u1LsaType = SR_ZERO;
    UINT2               u2SubTlvType = SR_ZERO;
    UINT4               u4Sid = SR_ZERO;
    UINT4               u4Val = SR_ZERO;

    pTlvPtr = pOsLsaInfo->pu1Lsa;
    pTemp = pTlvPtr + SR_FOUR;
    u1LsaType = SR_LGET1BYTE (pTemp);
    if (u1LsaType == OSPF_EXT_PRREFIX_LSA_OPAQUE_TYPE)
    {
        SR_TRC3 (SR_UTIL_TRC, "%s:%d Recvd PREFIX_LSA from AdvRtr Id %x \n",
                 __func__, __LINE__, pOsLsaInfo->u4AdvRtrId);
        pTlvPtr = pOsLsaInfo->pu1Lsa + (SR_NINE + SR_NINE + SR_TWO);
        pTemp = pTlvPtr;
        u2SubTlvType = SR_LGET2BYTE (pTemp);
        if (u2SubTlvType == OSPF_EXT_PRREFIX_TLV_TYPE)
        {
            pTlvPtr += (SR_EIGHT + SR_FOUR);
            u2SubTlvType = SR_LGET2BYTE (pTlvPtr);
            if (u2SubTlvType == PREFIX_SID_SUB_TLV_TYPE)
            {
                u2SubTlvType = SR_LGET2BYTE (pTlvPtr);
                if (u2SubTlvType == PREFIX_SID_SUB_TLV_LEN)
                {
                    pTlvPtr += SR_FOUR;
                    u4Val = SR_LGET4BYTE (pTlvPtr);
                    u4Sid = (UINT4) u4Val;
                    if (u4Sid == SR_ZERO)
                    {
                        SR_TRC3 (SR_UTIL_TRC,
                                 "%s:%d Received Flush LSA from AdvRtr Id %x \n",
                                 __func__, __LINE__, pOsLsaInfo->u4AdvRtrId);
                        return SR_TRUE;
                    }
                }
            }
        }
    }
    return SR_FALSE;
}

/*****************************************************************************/
/* Function Name : SrRegWithOspfForInfoGet                                   */
/* Description   : Function for registering with Ospf to get ABR             */
/*                 status, RTR ID                                            */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrRegWithOspfForInfoGet (VOID)
{
    tSrOspfStatusGet    SrOspfStatusGetInfo;

    MEMSET (&SrOspfStatusGetInfo, 0, sizeof (tSrOspfStatusGet));

    SrOspfStatusGetInfo.pAbrChngfunc = SrOspfAbrStatusChngHandler;
    SrOspfStatusGetInfo.pRtrIDChngfunc = SrOspfRtrIDChngHandler;
#ifdef OSPF_WANTED
    OspfRegisterforInfoGet (&SrOspfStatusGetInfo);
#endif
    return;
}

/*****************************************************************************/
/* Function Name : SrDeRegWithOspfForInfoGet                                 */
/* Description   : Function for registering with Ospf to get ABR             */
/*                 status, RTR ID                                            */
/* Input(s)      : None                                                      */
/* Output(s)     : None                                                      */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrDeRegWithOspfForInfoGet (VOID)
{
#ifdef OSPF_WANTED
    OspfDeRegisterforInfoGet ();
#endif
    return;
}

/*****************************************************************************/
/* Function Name : SrOspfAbrStatusChngHandler                                */
/* Description   : Function for hangle Abr status change                     */
/* Input(s)      : u1AbrStatus - Abr status                                  */
/* Output(s)     : None                                                      */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrOspfAbrStatusChngHandler (UINT1 u1AbrStatus)
{
    gSrGlobalInfo.u1OspfAbrStatus = u1AbrStatus;
    return;
}

/*****************************************************************************/
/* Function Name : SrOspfRtrIDChngHandler                                    */
/* Description   : Function for handle Rtr ID status change                  */
/* Input(s)      : u1AsbrStatus - Abr status                                 */
/* Output(s)     : None                                                      */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/
VOID
SrOspfRtrIDChngHandler (UINT4 u4OspfRtrID)
{
    gSrGlobalInfo.u4Ospfv2RtrID = u4OspfRtrID;
    return;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrIsV3FlushLsa                                             */
/*                                                                           */
/* Description  : This function decodes the received opaque lsa. If the Sid  */
/*                value is set to zero, delete all the details related to    */
/*                the node prefix                                            */
/*                                                                           */
/* Input        : pOpqLSAInfo                                                */
/*                                                                           */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : SR_TRUE/SR_FALSE                                           */
/*****************************************************************************/
PUBLIC UINT1
SrIsV3FlushLsa (tOs3LsaInfo * pOsLsaInfo)
{
    UINT1              *pTlvPtr = NULL;
    UINT1              *pTemp = NULL;
    UINT2               u2TlvType = SR_ZERO;
    UINT2               u2SubTlv = SR_ZERO;
    UINT2               u2LsaType = SR_ZERO;
    UINT4               u4Val = SR_ZERO;
    UINT4               u4Sid = SR_ZERO;

    pTlvPtr = pOsLsaInfo->pu1Lsa;

    pTemp = pTlvPtr + SR_TWO;
    u2LsaType = SR_LGET2BYTE (pTemp);

    if (OSPFV3_EXT_INTRA_AREA_PREFIX_LSA == u2LsaType)
    {
        pTlvPtr = pOsLsaInfo->pu1Lsa + OSPFV3_EXT_INTRA_AREA_PREFIX_LSA_HDR_LEN;
        u2TlvType = SR_LGET2BYTE (pTlvPtr);
        if (u2TlvType == OSPFV3_INTRA_AREA_PRFIX_TLV_TYPE)
        {
            u2SubTlv = SR_LGET2BYTE (pTlvPtr);
            if (u2SubTlv == OSPFV3_INTRA_AREA_PRFIX_TLV_LEN)
            {
                pTlvPtr +=
                    OSPFV3_INTRA_AREA_PRFIX_TLV_ADDR_OFFSET +
                    SR_IPV6_ADDR_LENGTH;
                u2SubTlv = SR_LGET2BYTE (pTlvPtr);
                if (u2SubTlv == PREFIX_SID_SUB_TLV_TYPE_V3)
                {
                    u2SubTlv = SR_LGET2BYTE (pTlvPtr);
                    if (u2SubTlv == PREFIX_SID_SUB_TLV_LEN_V3)
                    {
                        pTlvPtr += SR_FOUR;
                        u4Val = SR_LGET4BYTE (pTlvPtr);
                        u4Sid = (UINT4) u4Val;
                        if (u4Sid == SR_ZERO)
                        {
                            SR_TRC3 (SR_UTIL_TRC,
                                     "%s:%d Received Flush LSA for Rtr Id %x \n",
                                     __func__, __LINE__,
                                     pOsLsaInfo->u4AdvRtrId);
                            return SR_TRUE;
                        }
                    }
                }
            }
        }
    }
    else if (OSPFV3_EXT_INTER_AREA_PREFIX_LSA == u2LsaType)
    {
        pTlvPtr = pOsLsaInfo->pu1Lsa + OSPFV3_EXT_INTER_AREA_PREFIX_LSA_HDR_LEN;
        u2TlvType = SR_LGET2BYTE (pTlvPtr);
        if (u2TlvType == OSPFV3_INTER_AREA_PRFIX_TLV_TYPE)
        {
            u2SubTlv = SR_LGET2BYTE (pTlvPtr);
            if (u2SubTlv == OSPFV3_INTER_AREA_PRFIX_TLV_LEN)
            {
                pTlvPtr +=
                    OSPFV3_INTER_AREA_PRFIX_TLV_ADDR_OFFSET +
                    SR_IPV6_ADDR_LENGTH;
                u2SubTlv = SR_LGET2BYTE (pTlvPtr);
                if (u2SubTlv == PREFIX_SID_SUB_TLV_TYPE_V3)
                {
                    u2SubTlv = SR_LGET2BYTE (pTlvPtr);
                    if (u2SubTlv == PREFIX_SID_SUB_TLV_LEN_V3)
                    {
                        pTlvPtr += SR_FOUR;
                        u4Val = SR_LGET4BYTE (pTlvPtr);
                        u4Sid = (UINT4) u4Val;
                        if (u4Sid == SR_ZERO)
                        {
                            return SR_TRUE;
                        }
                    }
                }
            }
        }
    }
    return SR_FALSE;
}

 /*****************************************************************************/
 /* Function Name : SRLFACreateILM                                            */
 /* Description   : Function will create the SR LFA ILM                       */
 /* Input(s)      : pLfaRtInfo                                                */
 /* Output(s)     : None                                                      */
 /* Return(s)     : INT4                                                      */
 /*****************************************************************************/
INT4
SRLFACreateILM (tInSegment * pInSegment)
{
    if (MplsLfaCreateILM (pInSegment) == SR_FAILURE)
    {
        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Create SR LFA ILMs \n ",
                 __func__, __LINE__);
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/*****************************************************************************/
/* Function Name : SrGetNextHopFromRTM                                       */
/* Description   : Function will retrive the nexthop from RTM for a given    */
/*                  Ip Address                                               */
/* Input(s)      : u4IpAddr                                                  */
/* Output(s)     : Nexthop                                                   */
/* Return(s)     : None                                                      */
/*****************************************************************************/
PUBLIC VOID
SrGetNextHopFromRTM (UINT4 u4IpAddr, tGenU4Addr * pNxtHopIpAddr)
{
    tGenU4Addr          destAddr;
    tRtInfo             InRtInfo;
    UINT4               u2IncarnId = SR_ZERO;
    tRtInfo            *pRt = NULL;

    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&InRtInfo, SR_ZERO, sizeof (tRtInfo));

    MEMCPY (&destAddr.Addr.u4Addr, &u4IpAddr, MAX_IPV4_ADDR_LEN);
    InRtInfo.u4DestNet = destAddr.Addr.u4Addr;
    InRtInfo.u4DestMask = 0xffffffff;
    if (RtmApiGetBestRouteEntryInCxt (u2IncarnId, InRtInfo, &pRt) != IP_FAILURE)
    {
        if ((pRt != NULL) && (pRt->u4NextHop != 0))
        {
            pNxtHopIpAddr->Addr.u4Addr = pRt->u4NextHop;
        }
    }
    return;
}

 /*****************************************************************************/
 /* Function Name : SrIpv6RlfaILMCreation                                     */
 /* Description   : Function will create the SR RLFA ILM                      */
 /* Input(s)      : pNetIpv6LfaRtInfo - Pointer to RLFA Route Info from RTM   */
 /*                 pSrRtrInfo - Pointer to Router Info                       */
 /* Output(s)     : None                                                      */
 /* Return(s)     : INT4                                                      */
 /*****************************************************************************/
UINT4
SrIpv6RlfaILMCreation (tNetIpv6LfaRtInfo * pNetIpv6LfaRtInfo,
                       tSrRtrInfo * pSrRtrInfo)
{

    tSrRtrInfo         *pTunnelRtrInfo = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    UINT1               u1TunnelFlag = SR_ZERO;
    UINT1               u1NHFound = SR_FALSE;
    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));

    MEMCPY (&(SrRtrAltNextHopInfo.nextHop.Addr.Ip6Addr),
            &(pNetIpv6LfaRtInfo->AltRlfaNxtHop), sizeof (tIp6Addr));
    SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV6_ADDR_TYPE;
    SrRtrAltNextHopInfo.u1FRRNextHop = SR_RLFA_NEXTHOP;

    /*Scan and get the router details of RLFA remote node */
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pTunnelRtrInfo, tSrRtrInfo *)
    {
        if (pTunnelRtrInfo->advRtrId.Addr.u4Addr ==
            pNetIpv6LfaRtInfo->uRemoteNodeId.u4RemNodeRouterId)
        {
            u1TunnelFlag = SR_ONE;
            break;
        }
    }
    if ((u1TunnelFlag != SR_ONE) || (pTunnelRtrInfo == NULL))
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to get the router "
                 " details of RLFA remote node \n", __func__, __LINE__);
        return SR_FAILURE;
    }

    MEMCPY (&(SrRtrAltNextHopInfo.RemoteNodeAddr.Addr.Ip6Addr),
            &(pTunnelRtrInfo->prefixId.Addr.Ip6Addr), sizeof (tIp6Addr));

    MEMCPY (&(SrRtrAltNextHopInfo.PrimarynextHop.Addr.Ip6Addr),
            &(pNetIpv6LfaRtInfo->NextHop), sizeof (tIp6Addr));
    TMO_SLL_Scan (&(pSrRtrInfo->NextHopList),
                  pSrRtrNextHopInfo, tSrRtrNextHopInfo *)
    {
        if (pSrRtrNextHopInfo->nextHop.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            if ((MEMCMP (pNetIpv6LfaRtInfo->AltRlfaNxtHop.u1_addr,
                         pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                         SR_IPV6_ADDR_LENGTH) == SR_ZERO) &&
                ((pSrRtrNextHopInfo->bIsLfaNextHop == SR_TRUE)
                && (pSrRtrNextHopInfo->u1FRRNextHop == SR_RLFA_NEXTHOP)))
            {
                u1NHFound = SR_TRUE;
                break;
            }
        }
    }

    if (u1NHFound == SR_FALSE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d  u1NHFound == SR_FALSE \n", __func__,
                 __LINE__);
        return SR_FAILURE;
    }
    SrRtrAltNextHopInfo.u4OutIfIndex = pSrRtrNextHopInfo->u4OutIfIndex;
    if (SrMplsCreateILM (pSrRtrInfo, &SrRtrAltNextHopInfo) == SR_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC,
                 "%s:%d RLFA ILM created successfully for FEC: %s via Next Hop: %s \n",
                 __func__, __LINE__,
                 Ip6PrintAddr (&pSrRtrInfo->prefixId.Addr.Ip6Addr),
                 Ip6PrintAddr (&(SrRtrAltNextHopInfo.nextHop.Addr.Ip6Addr)));
    }
    else
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d RLFA ILM creation failed for FEC: %x via Next Hop: %x\n",
                 __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
                 pSrRtrInfo->u4LfaNextHop);
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrGetLabelForRLFARemoteNode
 * Description   : This routine is used to get the label for RLfa remote
 *                  node.
 *
 * Input(s)      : pRemoteNode - Remote Node address of RLFA Route
 *                 u4SrGbMinIndex - Mininum SRGB range of RLFA NextHop Router
 * Output(s)     : pOutLabel - RLFA remote node label
 * Return(s)     : SUCCESS/FAILURE
 *****************************************************************************/
VOID
SrGetLabelForRLFARemoteNode (tGenU4Addr * pRemoteNode, UINT4 *pSrGbMinIndex,
                             UINT4 *pOutLabel)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tSrRtrInfo         *pRemoteRtrInfo = NULL;

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRemoteRtrInfo = (tSrRtrInfo *) pRtrNode;
        if (pRemoteNode->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            if (MEMCMP
                (&(pRemoteNode->Addr.u4Addr),
                 &(pRemoteRtrInfo->prefixId.Addr.u4Addr),
                 SR_IPV4_ADDR_LENGTH) == SR_ZERO)
            {
                /* Mininum SRGB range of RLFA NextHop Router + Sid value of Remote Node */
                *pOutLabel = pRemoteRtrInfo->u4SidValue + *pSrGbMinIndex;
                SR_TRC4 (SR_UTIL_TRC, "%s:%d Remote Node found 0x%x label: %u \n",
                         __func__, __LINE__, pRemoteNode->Addr.u4Addr,
                         *pOutLabel);
                break;
            }
        }
        else
        {
            if (MEMCMP
                (&(pRemoteNode->Addr.Ip6Addr),
                 &(pRemoteRtrInfo->prefixId.Addr.Ip6Addr),
                 SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                /* Mininum SRGB range of RLFA NextHop Router + Sid value of Remote Node */
                *pOutLabel = pRemoteRtrInfo->u4SidValue + *pSrGbMinIndex;
                SR_TRC4 (SR_UTIL_TRC, "%s:%d Remote Node found 0x%x label: %u \n",
                         __func__, __LINE__,
                         Ip6PrintAddr (&pRemoteNode->Addr.Ip6Addr), *pOutLabel);
                break;
            }
        }
    }
    return;
}

/*****************************************************************************/
/* Function Name : SrRlfaCreateILM                                           */
/* Description   : Function will create the SR RLFA ILM                      */
/* Input(s)      : pu4LfaInLabel - Pointer to Lfa Inlabel                     */
/* Output(s)     : None                                                      */
/* Return(s)     : INT4                                                      */
/*****************************************************************************/
INT4
SrRlfaCreateILM (UINT4 *pu4LfaInLabel)
{
    if (MplsRlfaCreateILM (pu4LfaInLabel) == SR_FAILURE)
    {
        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Create SR RLFA ILMs \n ",
                 __func__, __LINE__);
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrRlfaDeleteILM
 * Description   : This routine is used to Delete MPLS RLFA ILM entry
 *
 * Input(s)      : pDestAddr - Destination Address
 *                 pRemoteAddr - Remote/Tunnel Node address of RLFA Route
 *                 pRlfaNextHop - Rlfa Nexthop to reach Remote Node
 *                 pSrInSegInfo - In-mapping Info to be created
 * Output(s)     : None
 * Return(s)     : SUCCESS/FAILURE
 *****************************************************************************/
INT4
SrRlfaDeleteILM (tGenU4Addr * pDestAddr, tGenU4Addr * pRemoteAddr,
                 tGenU4Addr * pRlfaNextHop,
                 tSrInSegInfo * pSrInSegInfo)
{
    tSrTeLblStack       SrTeLblStack;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pSrRtrInfo        = NULL;
    tSrRlfaPathInfo    *pSrRlfaPathInfo   = NULL;
    UINT1               u1NHFound         = SR_FALSE;

    /*Get SrRtrInfo for DestAddr */
    pSrRlfaPathInfo = SrGetRlfaPathEntryFromDestAddr (pDestAddr);
    pSrRtrInfo = SrGetSrRtrInfoFromRtrId (pDestAddr);

    if ((pSrRtrInfo == NULL) || (pSrRlfaPathInfo == NULL))
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrGetSrRtrInfoFromRtrId FAILURE \n", __func__,
                 __LINE__);
        return SR_FAILURE;
    }

    pSrInSegInfo->u4InLabel = pSrRtrInfo->u4LfaInLabel;

    TMO_SLL_Scan (&(pSrRtrInfo->NextHopList),
                  pSrRtrNextHopInfo, tSrRtrNextHopInfo *)
    {
        if (pSrRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            if ((MEMCMP
                 (&(pSrRtrNextHopInfo->nextHop.Addr.u4Addr),
                  &(pRlfaNextHop->Addr.u4Addr), SR_IPV4_ADDR_LENGTH) == SR_ZERO)
                && ((pSrRtrNextHopInfo->bIsLfaNextHop == TRUE)
                    && (pSrRtrNextHopInfo->u1FRRNextHop == SR_RLFA_NEXTHOP)))
            {
                u1NHFound = SR_TRUE;
                break;
            }
        }
        else if (pSrRtrInfo->prefixId.u2AddrType == SR_IPV6_ADDR_TYPE)
        {
            if ((MEMCMP (pSrRtrNextHopInfo->nextHop.Addr.Ip6Addr.u1_addr,
                         pRlfaNextHop->Addr.Ip6Addr.u1_addr,
                         SR_IPV6_ADDR_LENGTH) == SR_ZERO) &&
                ((pSrRtrNextHopInfo->bIsLfaNextHop == TRUE)
                && (pSrRtrNextHopInfo->u1FRRNextHop == SR_RLFA_NEXTHOP)))
            {
                u1NHFound = SR_TRUE;
                break;
            }
        }
    }
    if ((u1NHFound == SR_FALSE) || (pSrRtrNextHopInfo == NULL))
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d u1NHFound == SR_FALSE: SR_FAILURE \n",
                 __func__, __LINE__);
        return SR_FAILURE;
    }
    pSrRtrNextHopInfo->u1FRRNextHop = SR_RLFA_NEXTHOP;
    if (pSrRtrInfo->prefixId.u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        pSrRtrNextHopInfo->RemoteNodeAddr.Addr.u4Addr =
            pRemoteAddr->Addr.u4Addr;
    }
    else
    {
        MEMCPY (&(pSrRtrNextHopInfo->RemoteNodeAddr.Addr.Ip6Addr),
                &(pRemoteAddr->Addr.Ip6Addr), sizeof (tIp6Addr));
    }

    pSrRtrNextHopInfo->u4SwapOutIfIndex = pSrRlfaPathInfo->u4RlfaIlmTunIndex;

    if (SrMplsCreateOrDeleteStackLsp(MPLS_MLIB_ILM_DELETE, pSrRtrInfo,
                                pSrInSegInfo, &SrTeLblStack,
                                pSrRtrNextHopInfo, SR_TRUE) == SR_FAILURE)
    {
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                 "%s:%d TILfa ILM delete failed for FEC: %x via Next Hop: %x\n",
                 __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr,
                 pSrRtrInfo->u4LfaNextHop);
    }

    pSrRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
    pSrRtrInfo->u1LfaType = SR_ZERO;
    pSrRtrInfo->u4LfaNextHop = SR_ZERO;
    /*Release all the allocacated memory for this route */
    TMO_SLL_Delete (&(pSrRtrInfo->NextHopList),
                    (tTMO_SLL_NODE *) pSrRtrNextHopInfo);
    SR_RTR_NH_LIST_FREE (pSrRtrNextHopInfo);

    if (SrMplsDeleteTunIntf (&pSrRlfaPathInfo->u4RlfaIlmTunIndex) == SR_FAILURE)
    {
        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete ILM Tunnel \n", __func__,
                 __LINE__);
        return SR_FAILURE;
    }

    if (SrDeleteRlfaPathTableEntry (pSrRlfaPathInfo) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d RLFA RBTree deletion failure \n", __func__,
                 __LINE__);
        return SNMP_FAILURE;
    }
    if (SrReleaseMplsInLabel (pSrRtrInfo->prefixId.u2AddrType,
                              pSrRtrInfo->u4SidValue) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Unable to Release In-Label \n", __func__,
                 __LINE__);
        return SR_FAILURE;
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrRlfaDeleteFTNAndILM
 * Description   : This routine is used to Delete MPLS RLFA FTN and ILM entry
 *
 * Input(s)      : pDestAddr - Destination Address
 *                 pNextHop - NextHop Address
 *                 pRemoteAddr - Remote/Tunnel Node address of RLFA Route
 *                 pRlfaNextHop - Rlfa Nexthop to reach Remote Node
 *                 pSrInSegInfo - In-mapping Info to be created
 *                 u4SrGbMinIndex - Self Node SRGB Mininum value
 * Output(s)     : None
 * Return(s)     : SUCCESS/FAILURE
 *****************************************************************************/
INT4
SrRlfaDeleteFTNAndILM (tGenU4Addr * pDestAddr,
                       tGenU4Addr * pRemoteAddr, tGenU4Addr * pRlfaNextHop,
                       tSrInSegInfo * pSrInSegInfo, UINT4 u4SrGbMinIndex)
{
    INT1                i1Return = SR_SUCCESS;
    if (SrRlfaDeleteFtnEntry (pDestAddr, pRlfaNextHop) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Deletion of RLFA FTN Failure \n", __func__,
                 __LINE__);
        i1Return = SR_FAILURE;
    }
    if (SrRlfaDeleteILM (pDestAddr, pRemoteAddr, pRlfaNextHop,
                         pSrInSegInfo) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Deletion of RLFA ILM Failure \n", __func__,
                 __LINE__);
        i1Return = SR_FAILURE;
    }
    return i1Return;
}

/*****************************************************************************
 * Function Name : SrProcessNetIpLfaEvents
 * Description   : This routine process the SR Main queue LFA event
 *
 * Input(s)      : pSrNetIpLfaRtInfo - IPv4 Lfa Route Info
 *
 * Output(s)     :
 * Return(s)     : None
 *****************************************************************************/
VOID
SrProcessNetIpLfaEvents (tSrNetIpLfaRtInfo * pSrNetIpLfaRtInfo)
{

    tNetIpv4LfaRtInfo   NetIpLfaRtInfo;

    MEMSET (&NetIpLfaRtInfo, 0, sizeof (tNetIpv4LfaRtInfo));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);
    if (pSrNetIpLfaRtInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d  pSrNetIpLfaRtInfo is NULL \n", __func__,
                 __LINE__);
        return;
    }

    if (NETIPV4_TILFA_ROUTE == pSrNetIpLfaRtInfo->u1LfaType)
    {

        SR_TRC3 (SR_MAIN_TRC, "%s:%d Dest from RTM is %x\n ", __func__,
                             __LINE__, pSrNetIpLfaRtInfo->u4DestNet);

        SR_TRC3 (SR_MAIN_TRC, "%s:%d TILFA P Node from RTM is %x\n ", __func__,
                     __LINE__, pSrNetIpLfaRtInfo->uRemoteNodeId.u4RemNodeRouterId);
        SR_TRC3 (SR_MAIN_TRC, "%s:%d TILFA Q Node from RTM is %x\n ", __func__,
                             __LINE__, pSrNetIpLfaRtInfo->uQNodeId.u4QNodeRouterId);
        SR_TRC3 (SR_MAIN_TRC, "%s:%d TILFA PQ Adj from RTM is %x\n ", __func__,
                             __LINE__, pSrNetIpLfaRtInfo->u4PaQAdjLocAddr);
    }

    NetIpLfaRtInfo.u4DestNet = pSrNetIpLfaRtInfo->u4DestNet;
    NetIpLfaRtInfo.u4DestMask = pSrNetIpLfaRtInfo->u4DestMask;
    NetIpLfaRtInfo.u4NextHop = pSrNetIpLfaRtInfo->u4NextHop;
    NetIpLfaRtInfo.u4LfaCost = pSrNetIpLfaRtInfo->u4LfaCost;
    NetIpLfaRtInfo.u4NextHopIfIndx = pSrNetIpLfaRtInfo->u4NextHopIfIndx;
    NetIpLfaRtInfo.u4AltIfIndx = pSrNetIpLfaRtInfo->u4AltIfIndx;
    NetIpLfaRtInfo.u4ContextId = pSrNetIpLfaRtInfo->u4ContextId;
    NetIpLfaRtInfo.u4LfaNextHop = pSrNetIpLfaRtInfo->u4LfaNextHop;
    NetIpLfaRtInfo.u4PrimaryCost = pSrNetIpLfaRtInfo->u4PrimaryCost;
    NetIpLfaRtInfo.uRemoteNodeId.u4RemNodeRouterId =
	    pSrNetIpLfaRtInfo->uRemoteNodeId.u4RemNodeRouterId;
    NetIpLfaRtInfo.uQNodeId.u4QNodeRouterId =
	    pSrNetIpLfaRtInfo->uQNodeId.u4QNodeRouterId;
    NetIpLfaRtInfo.u4PaQAdjLocAddr = pSrNetIpLfaRtInfo->u4PaQAdjLocAddr;
    NetIpLfaRtInfo.u2RtProto = pSrNetIpLfaRtInfo->u2RtProto;
    NetIpLfaRtInfo.u1CmdType = pSrNetIpLfaRtInfo->u1CmdType;
    NetIpLfaRtInfo.u1LfaType = pSrNetIpLfaRtInfo->u1LfaType;

    /*Procces LFA/RLFA IPv4 Event */
    SrProcessLfaIpRtChangeEvent (&NetIpLfaRtInfo);
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return;
}

/*****************************************************************************
 * Function Name : LfaIpRtChangeEventHandler
 * Description   : This routine post the LFA routes to SR Main queue
 *
 * Input(s)      : pNetIpLfaRtInfo - IPv4 Lfa Route Info
 *
 * Output(s)     :
 * Return(s)     : None
 *****************************************************************************/
VOID
LfaIpRtChangeEventHandler (tNetIpv4LfaRtInfo * pNetIpLfaRtInfo)
{

    tSrQMsg             SrQMsg;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY\n", __func__, __LINE__);
    if (pNetIpLfaRtInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d pNetIpLfaRtInfo is NULL\n", __func__,
                 __LINE__);
        return;
    }

    SrQMsg.u.SrNetIpLfaRtInfo.u4DestNet = pNetIpLfaRtInfo->u4DestNet;
    SrQMsg.u.SrNetIpLfaRtInfo.u4DestMask = pNetIpLfaRtInfo->u4DestMask;
    SrQMsg.u.SrNetIpLfaRtInfo.u4NextHop = pNetIpLfaRtInfo->u4NextHop;
    SrQMsg.u.SrNetIpLfaRtInfo.u4LfaCost = pNetIpLfaRtInfo->u4LfaCost;
    SrQMsg.u.SrNetIpLfaRtInfo.u4NextHopIfIndx =
        pNetIpLfaRtInfo->u4NextHopIfIndx;
    SrQMsg.u.SrNetIpLfaRtInfo.u4AltIfIndx = pNetIpLfaRtInfo->u4AltIfIndx;
    SrQMsg.u.SrNetIpLfaRtInfo.u4ContextId = pNetIpLfaRtInfo->u4ContextId;
    SrQMsg.u.SrNetIpLfaRtInfo.u4LfaNextHop = pNetIpLfaRtInfo->u4LfaNextHop;
    SrQMsg.u.SrNetIpLfaRtInfo.u4PrimaryCost = pNetIpLfaRtInfo->u4PrimaryCost;
    SrQMsg.u.SrNetIpLfaRtInfo.uRemoteNodeId.u4RemNodeRouterId =
	    pNetIpLfaRtInfo->uRemoteNodeId.u4RemNodeRouterId;
    /* TILFA support */
    SrQMsg.u.SrNetIpLfaRtInfo.uQNodeId.u4QNodeRouterId =
        pNetIpLfaRtInfo->uQNodeId.u4QNodeRouterId;
    SrQMsg.u.SrNetIpLfaRtInfo.u4PaQAdjLocAddr = pNetIpLfaRtInfo->u4PaQAdjLocAddr;

    SrQMsg.u.SrNetIpLfaRtInfo.u2RtProto = pNetIpLfaRtInfo->u2RtProto;
    SrQMsg.u.SrNetIpLfaRtInfo.u1CmdType = pNetIpLfaRtInfo->u1CmdType;
    SrQMsg.u.SrNetIpLfaRtInfo.u1LfaType = pNetIpLfaRtInfo->u1LfaType;

    SrQMsg.u4MsgType = SR_IP_LFA_EVENT;

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to EnQ IP Event to SR Task \n",
                 __func__, __LINE__);
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT\n", __func__, __LINE__);
    return;
}

/*****************************************************************************/
/* Function Name : SrUtilFetchOspfv2AreadID                                  */
/* Description   : Function to fetch OSPF-V2 Area id based on IP address     */
/* Input(s)      : u4OspfIfIpAddress, i4OspfAddressLessIf                    */
/* Output(s)     : pu4RetValOspfIfAreaId                                     */
/* Return(s)     : VOID                                                      */
/*****************************************************************************/

VOID
SrUtilFetchOspfv2AreadID (UINT4 u4OspfIfIpAddress, INT4 i4OspfAddressLessIf,
                          UINT4 *pu4RetValOspfIfAreaId)
{
#ifdef OSPF_WANTED
    if (Ospfv2UtilFetchAreaID (u4OspfIfIpAddress, i4OspfAddressLessIf,
                               pu4RetValOspfIfAreaId) == SR_TRUE)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Failed to Fetch OSPF-V2 Area ID\n ",
                 __func__, __LINE__);
    }

#else
    UNUSED_PARAM (u4OspfIfIpAddress);
    UNUSED_PARAM (i4OspfAddressLessIf);
    UNUSED_PARAM (pu4RetValOspfIfAreaId);

#endif
    return;
}

/*****************************************************************************
 * Function Name : SrLfaCreateLSP
 * Description   : This routine  will create LFA LSP for the
 *                   a. Node-A which is coming up
 *                   b. Nodes which is having LFA next hop as the Node-A
 * Input(s)      : pSrRtrInfo, pSrRtrNextHopInfo
 * Output(s)     : NONE
 * Return(s)     : NONE
 *****************************************************************************/

VOID
SrLfaCreateLSP (tSrRtrInfo * pSrRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tSrRtrInfo         *pNewRtrNode = NULL;
    tSrRtrInfo         *pTempSrRtrInfo = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pTmpSrRtrNextHopInfo = NULL;
    tNetIpv4LfaRtInfo   NetIpLfaRtInfo;
    tLfaRtInfoQueryMsg  LfaRtQuery;
    tGenU4Addr          tempAddr;
    UINT4               u4L3IfIndex = SR_ZERO;
    INT4                i4AltStatus = ALTERNATE_DISABLED;
    UINT4               u4Port = SR_ZERO;
    tGenU4Addr          tempRlfaTnlId;
    tSrRtrInfo         *pTmpSrRlfaTnlInfo = NULL;

    MEMSET (&tempAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&LfaRtQuery, SR_ZERO, sizeof (tLfaRtInfoQueryMsg));
    MEMSET (&NetIpLfaRtInfo, SR_ZERO, sizeof (tNetIpv4LfaRtInfo));
    MEMSET (&tempRlfaTnlId, SR_ZERO, sizeof (tGenU4Addr));

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Entering\n", __func__, __LINE__);

    nmhGetFsSrV4AlternateStatus (&i4AltStatus);

    if (i4AltStatus != ALTERNATE_ENABLED)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Exiting: LFA is globally disabled \n",
                 __func__, __LINE__);
        return;
    }

    SR_TRC3 (SR_UTIL_TRC, "%s:%d Creating LFA for Prefix=%x\n",
             __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
    /* Create Self LFA LSP */
    if (LfaCreateAndDeleteLSP (MPLS_MLIB_FTN_CREATE,
                               pSrRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
    {
        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                 "%s:%d Exiting: Failed to Create Self LFA for Prefix = %x\n",
                 __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
    }

    if (pSrRtrInfo->srgbRange.u4SrGbMinIndex == SR_ZERO)
    {
        SR_TRC3 (SR_UTIL_TRC,
                 "%s:%d Exiting: Prefix = %x is not the directly connected peer\n",
                 __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
        return;
    }

    /* Create LFA entry for node which is having LFA next hop as incoming SR node */
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pNewRtrNode = (tSrRtrInfo *) pRtrNode;

        TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                      pSrRtrNextHopNode, tTMO_SLL_NODE *)
        {
            pTmpSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

            if ((pTmpSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                SR_FTN_CREATED)
            {
                LfaRtQuery.u4DestinationIpAddress =
                    pNewRtrNode->prefixId.Addr.u4Addr;
                LfaRtQuery.u4DestinationSubnetMask = 0xFFFFFFFF;
                LfaRtQuery.u4PrimaryNextHop =
                    pTmpSrRtrNextHopInfo->nextHop.Addr.u4Addr;
                LfaRtQuery.u2RtProto = OSPF_ID;
                LfaRtQuery.u4ContextId = SR_ZERO;
                if (CfaUtilGetIfIndexFromMplsTnlIf
                    (pTmpSrRtrNextHopInfo->u4OutIfIndex, &u4L3IfIndex,
                     SR_TRUE) == SR_LESSER)
                {
                    SR_TRC3 (SR_UTIL_TRC,
                             "%s:%d Exiting: Not able to get IfIndex from MplsTnlIf for Prefix = %x\n",
                             __func__, __LINE__,
                             LfaRtQuery.u4DestinationIpAddress);
                    return;
                }
                if (NetIpv4GetPortFromIfIndex (u4L3IfIndex, &u4Port) ==
                    SR_LESSER)
                {
                    SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                             "%s:%d Exiting: NetIpv4GetPortFromIfIndex failed  for Prefix = %x\n",
                             __func__, __LINE__,
                             LfaRtQuery.u4DestinationIpAddress);
                    return;
                }
                LfaRtQuery.u4NextHopIfIndx = u4Port;
                if (NetIpv4LfaGetRoute (&LfaRtQuery, &NetIpLfaRtInfo) !=
                    SR_ZERO)
                {
                    SR_TRC3 (SR_UTIL_TRC,
                             "%s:%d LFA route not present for the Prefix %x\n",
                             __func__, __LINE__,
                             LfaRtQuery.u4DestinationIpAddress);
                    continue;
                }
                if (NetIpLfaRtInfo.u1LfaType == SR_LFA)
                {
                    tempAddr.Addr.u4Addr = NetIpLfaRtInfo.u4LfaNextHop;
                    tempAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                    pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&tempAddr);
                    if ((pTempSrRtrInfo != NULL) &&
                        (pSrRtrInfo->prefixId.Addr.u4Addr ==
                         pTempSrRtrInfo->prefixId.Addr.u4Addr))
                    {
                        SR_TRC4 (SR_UTIL_TRC,
                                 "%s:%d Creating LFA LSP for prefix %x with lfa NextHop : %x \n",
                                 __func__, __LINE__,
                                 LfaRtQuery.u4DestinationIpAddress,
                                 pTempSrRtrInfo->prefixId.Addr.u4Addr);
                        if (LfaCreateAndDeleteLSP
                            (MPLS_MLIB_FTN_CREATE, pNewRtrNode,
                             pTmpSrRtrNextHopInfo) == SR_FAILURE)
                        {
                            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                                     "%s:%d Exiting: Failed to Create LFA for Prefix = %x\n",
                                     __func__, __LINE__,
                                     pNewRtrNode->prefixId.Addr.u4Addr);
                            return;
                        }
                    }
                }
                else if (NetIpLfaRtInfo.u1LfaType == SR_RLFA)
                {
                    tempAddr.Addr.u4Addr = NetIpLfaRtInfo.u4LfaNextHop;
                    tempAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                    pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&tempAddr);
                    tempRlfaTnlId.Addr.u4Addr = NetIpLfaRtInfo.uRemoteNodeId.u4RemNodeRouterId;
                    tempRlfaTnlId.u2AddrType = SR_IPV4_ADDR_TYPE;
                    pTmpSrRlfaTnlInfo =
                        SrGetSrRtrInfoFromRtrId (&tempRlfaTnlId);
                    /* Create RLFA LSP if Incoming SR node is RLFA Tunnel node for a Prefix */
                    if ((pTmpSrRlfaTnlInfo != NULL) &&
                        (pSrRtrInfo->prefixId.Addr.u4Addr ==
                         pTmpSrRlfaTnlInfo->prefixId.Addr.u4Addr))
                    {
                        SR_TRC4 (SR_UTIL_TRC,
                                 "%s:%d Creating RLFA for prefix %x with Tunnel %x \n",
                                 __func__, __LINE__,
                                 LfaRtQuery.u4DestinationIpAddress,
                                 pTmpSrRlfaTnlInfo->prefixId.Addr.u4Addr);
                        if (LfaCreateAndDeleteLSP
                            (MPLS_MLIB_FTN_CREATE, pNewRtrNode,
                             pTmpSrRtrNextHopInfo) == SR_FAILURE)
                        {
                            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                                     "%s:%d Exiting: Failed to Create LFA for Prefix = %x\n",
                                     __func__, __LINE__,
                                     pNewRtrNode->prefixId.Addr.u4Addr);
                            return;
                        }
                    }
                    /* Create RLFA LSP if incoming SR Node is RLFA Nexthop for a Prefix */
                    else if ((pTempSrRtrInfo != NULL) &&
                             (pSrRtrInfo->prefixId.Addr.u4Addr ==
                              pTempSrRtrInfo->prefixId.Addr.u4Addr))
                    {
                        SR_TRC4 (SR_UTIL_TRC,
                                 "%s:%d Creating RLFA for prefix %x with Rlfa NextHop : %x \n",
                                 __func__, __LINE__,
                                 LfaRtQuery.u4DestinationIpAddress,
                                 pTempSrRtrInfo->prefixId.Addr.u4Addr);
                        if (LfaCreateAndDeleteLSP
                            (MPLS_MLIB_FTN_CREATE, pNewRtrNode,
                             pTmpSrRtrNextHopInfo) == SR_FAILURE)
                        {
                            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                                     "%s:%d Exiting: Failed to Create RLFA for Prefix = %x\n",
                                     __func__, __LINE__,
                                     pNewRtrNode->prefixId.Addr.u4Addr);
                            return;
                        }
                    }
                }
            }
        }
    }

    /* Check and process this router is part of any Ti-LFA path  */
    SrTiLfaProcessForSrRouterNodeChange(pSrRtrInfo);

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Exiting\n", __func__, __LINE__);
    return;
}




/*****************************************************************************
 * Function Name : SrOspfv3UpEventHandler
 * Description   : This routine is used to indicate to SR, to send a Opaque LSA
 * Input(s)      : None
 * Output(s)     : None
 * Return(s)     : None
 *****************************************************************************/
VOID
SrOspfv3UpEventHandler (VOID)
{
    tSrQMsg             SrQMsg;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));

    SrQMsg.u4MsgType = SR_OSPFV3_UP_EVENT;

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                 "%s:%d Failed to EnQ SR_OSPFV3_UP_EVENT Event to SR Task \n",
                 __func__, __LINE__);
    }
    return;

}

INT4
SrCreateRlfaPathTableEntry (tSrRlfaPathInfo * pSrRlfaPathInfo)
{
    tSrRlfaPathInfo    *pNewSrRlfaPathInfo = NULL;

    /*Allocate memory for tSrRlfaPathInfo */
    if (SR_RLFA_PATH_MEM_ALLOC (pNewSrRlfaPathInfo) == NULL)
    {
        SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                 "%s:%d Memory allocation failed for SR RLFA route\n ",
                 __func__, __LINE__);
        return SR_FAILURE;
    }

    MEMSET (pNewSrRlfaPathInfo, SR_ZERO, sizeof (tSrRlfaPathInfo));

    if (pSrRlfaPathInfo->destAddr.u2AddrType == SR_IPV4_ADDR_TYPE)
    {

        pNewSrRlfaPathInfo->destAddr.Addr.u4Addr =
            pSrRlfaPathInfo->destAddr.Addr.u4Addr;
        pNewSrRlfaPathInfo->mandRtrId.Addr.u4Addr =
            pSrRlfaPathInfo->mandRtrId.Addr.u4Addr;
    }
    else if (pSrRlfaPathInfo->destAddr.u2AddrType == SR_IPV6_ADDR_TYPE)
    {

        MEMCPY (pNewSrRlfaPathInfo->destAddr.Addr.Ip6Addr.u1_addr,
                pSrRlfaPathInfo->destAddr.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        MEMCPY (pNewSrRlfaPathInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                pSrRlfaPathInfo->mandRtrId.Addr.Ip6Addr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
    }

    pNewSrRlfaPathInfo->mandRtrId.u2AddrType =
        pSrRlfaPathInfo->mandRtrId.u2AddrType;
    pNewSrRlfaPathInfo->destAddr.u2AddrType =
        pSrRlfaPathInfo->destAddr.u2AddrType;
    pNewSrRlfaPathInfo->u4DestMask = pSrRlfaPathInfo->u4DestMask;

    if (RBTreeAdd
        (gSrGlobalInfo.pSrRlfaPathRbTree,
         (tRBElem *) pNewSrRlfaPathInfo) == RB_FAILURE)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC,
                "Create Sr-Te-Path Table : RBTree Node Add Failed \n");
        SR_RLFA_PATH_MEM_FREE (pNewSrRlfaPathInfo);
        return SR_FAILURE;
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrActivateLFA
 * Description   : This routine will Activate the LFA LSP
 * Input(s)      : pSrRtrInfo - Router Info
 *                 pSrRtrNextHopInfo - Next hop info
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrActivateLFA (tSrRtrInfo * pSrRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo)
{
    tSrRtrInfo         *pSrLfaRtrInfo = NULL;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tGenU4Addr          tempAddr;
    tLspInfo            LspInfo;
    tNHLFE              Nhlfe;
    UINT4               u4LfaOutLabel = SR_ZERO;
    UINT4               u4InLabel = SR_ZERO;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    MEMSET (&tempAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&LspInfo, SR_ZERO, sizeof (tLspInfo));
    MEMSET (&Nhlfe, SR_ZERO, sizeof (tNHLFE));

    /* Populate In-Label */
    SR_GET_SELF_IN_LABEL (&u4InLabel, pSrRtrInfo->u4SidValue);

    switch (pSrRtrInfo->u1LfaType)
    {
        case SR_LFA_ROUTE:
            /* Populate LFA path out-Label */
            tempAddr.Addr.u4Addr = pSrRtrInfo->u4LfaNextHop;
            tempAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
            pSrLfaRtrInfo = SrGetRtrInfoFromNextHop (&tempAddr);
            if (pSrLfaRtrInfo == NULL)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrGetRtrInfoFromNextHop Failed for"
                         "NH=%x \n", __func__, __LINE__,
                         pSrRtrInfo->u4LfaNextHop);
                return SR_FAILURE;
            }
            u4LfaOutLabel =
                pSrRtrInfo->u4SidValue +
                pSrLfaRtrInfo->srgbRange.u4SrGbMinIndex;
            Nhlfe.NextHopAddr.u4Addr = pSrRtrInfo->u4LfaNextHop;
            Nhlfe.u4OutIfIndex = pSrRtrInfo->u4LfaOutIfIndex;    /* LFA Tunnel index */
            Nhlfe.u4OutLabel = u4LfaOutLabel;    /* LFA Out-Label */
            break;
        case SR_RLFA_ROUTE:
            /* Fetching RLFA path entry */
            pSrRlfaPathInfo =
                SrGetRlfaPathEntryFromDestAddr (&(pSrRtrInfo->prefixId));
            if (pSrRlfaPathInfo == NULL)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                         "%s:%d RLFA entry not present for Dest=%x\n",
                         __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
                return SR_FAILURE;
            }
            Nhlfe.NextHopAddr.u4Addr = pSrRlfaPathInfo->rlfaNextHop.Addr.u4Addr;
            Nhlfe.u4OutIfIndex = pSrRlfaPathInfo->u4RlfaTunIndex;    /* RLFA FTN Tunnel index */
            LspInfo.u4LfaIlmTunIndex = pSrRlfaPathInfo->u4RlfaIlmTunIndex;    /* RLFA ILM Tunnel Index */
            LspInfo.u4RemoteNodeLabel = pSrRlfaPathInfo->u4RemoteLabel;    /* RLFA Mandatory Node Label */
            Nhlfe.u4OutLabel = pSrRlfaPathInfo->u4DestLabel;    /* RLFA Dest Out-Label */
            break;
        default:
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Invalid Repair flag =%d present"
                     " for Dest=%x\n", __func__, __LINE__,
                     pSrRtrInfo->u1LfaType, pSrRtrInfo->prefixId.Addr.u4Addr);
            return SR_FAILURE;
    }

    if (pSrRtrNextHopInfo == NULL)
    {
        /* pSrRtrNextHopInfo is NULL during processing of TE ILM alone  */
        LspInfo.u4IfIndex = pSrRtrInfo->u4TeSwapOutIfIndex;    /*primary ILM Tunnel if-index */
        LspInfo.u4InTopLabel = u4InLabel;
        LspInfo.u4LfaIlmTunIndex = pSrRtrInfo->u4LfaSwapOutIfIndex;    /*LFA Ilm Tun Index */
        LspInfo.FecParams.u4TnlId = pSrRtrInfo->u4TeSwapOutIfIndex;    /*primary ILM Tunnel if-index */
        LspInfo.FecParams.u1FecType = SR_FEC_PREFIX_TYPE;
        LspInfo.FecParams.u2AddrType = SR_IPV4_ADDR_TYPE;
        LspInfo.FecParams.DestAddrPrefix.u4Addr = pSrRtrInfo->prefixId.Addr.u4Addr;
        LspInfo.FecParams.DestMask.u4Addr = SR_IPV4_DEST_MASK;
        MEMCPY (&LspInfo.PrimarynextHop, &pSrRtrInfo->teSwapNextHop,
                sizeof (tGenU4Addr));
        LspInfo.Direction = MPLS_DIRECTION_FORWARD;
        LspInfo.u1Owner = MPLS_OWNER_SR_TE;
        LspInfo.bIsTeFtn = SR_FALSE;    /* To Process ILM alone in NP for TE */
        LspInfo.bIsSrTeLsp = SR_FALSE;

    }
    else
    {
        /* Fill Primary path information */
        LspInfo.u4IfIndex = pSrRtrNextHopInfo->u4OutIfIndex;    /*primary FTN Tunnel if-index */
        LspInfo.u4InTopLabel = u4InLabel;
        LspInfo.FecParams.u4TnlId = pSrRtrNextHopInfo->u4SwapOutIfIndex;    /*primary ILM Tunnel if-index */
        LspInfo.FecParams.u1FecType = SR_FEC_PREFIX_TYPE;
        LspInfo.FecParams.u2AddrType = SR_IPV4_ADDR_TYPE;
        LspInfo.FecParams.DestAddrPrefix.u4Addr = pSrRtrInfo->prefixId.Addr.u4Addr;
        LspInfo.FecParams.DestMask.u4Addr = SR_IPV4_DEST_MASK;
        MEMCPY (&LspInfo.PrimarynextHop, &pSrRtrNextHopInfo->nextHop,
                sizeof (tGenU4Addr));
        LspInfo.Direction = MPLS_DIRECTION_FORWARD;
        LspInfo.u1Owner = MPLS_OWNER_SR;
    }

    /* Fill LFA path information */
    Nhlfe.u1NHAddrType = SR_IPV4_ADDR_TYPE;
    Nhlfe.u1OperStatus = MPLS_STATUS_UP;

    LspInfo.pNhlfe = (tNHLFE *) (&Nhlfe);
    LspInfo.u1Protocol = MPLS_PROTOCOL_SR_OSPF;
    if (MplsMlibUpdate (MPLS_LFA_LSP_ACTIVATE, &LspInfo) == MPLS_FAILURE)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d MplsMlibUpdate Failed for"
                 "case:MPLS_LFA_LSP_ACTIVATE \n", __func__, __LINE__);
        return SR_FAILURE;
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/************************************************************************************
 * Function Name : SrMplsModifyLSP
 * Description   : This routine will Update the FTN and ILM entries in Control
 *                 plane.
 * Input(s)      : pSrRtrInfo - Router Information
 *                 pSrOldNextHopInfo - Old Next hop information
 *                 pSrIpEvtInfo - IP Event information
 *                 u1IsNewNHeqFRRNH - Flag to denote if new NH is same as LFA or not
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *************************************************************************************/
PUBLIC UINT4
SrMplsModifyLSP (tSrIpEvtInfo * pSrIpEvtInfo, tSrRtrInfo * pSrRtrInfo,
                    tSrRtrNextHopInfo * pSrOldNextHopInfo, UINT1 u1IsNewNHeqFRRNH)
{
    tSrRouteEntryInfo  *pRouteInfo;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pTempSrRtrInfo = NULL;
    tSrInSegInfo        SrInSegInfo;
    UINT4               u4TempNbrRtrId = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT1               u1Ret            = SR_ZERO;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));
    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));

    pRouteInfo = (tSrRouteEntryInfo *) (VOID *) pSrIpEvtInfo->pRouteInfo;

    if (u1IsNewNHeqFRRNH == TRUE)
    {
        pSrOldNextHopInfo->bIsOnlyCPUpdate = SR_TRUE;
    }

    if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
    {
        /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
        SR_GET_SELF_IN_LABEL (&SrInSegInfo.u4InLabel, pSrRtrInfo->u4SidValue);
        /* LFA ILM Delete */
        if (u1IsNewNHeqFRRNH == TRUE)
        {
            SrRtrAltNextHopInfo.bIsOnlyCPUpdate = SR_TRUE;
        }
        SrRtrAltNextHopInfo.bIsFRRHwLsp = SR_FALSE;
        SrRtrAltNextHopInfo.u4OutIfIndex = pSrRtrInfo->u4LfaOutIfIndex;
        SrRtrAltNextHopInfo.u4SwapOutIfIndex = pSrRtrInfo->u4LfaSwapOutIfIndex;
        SrRtrAltNextHopInfo.nextHop.Addr.u4Addr = pSrRtrInfo->u4LfaNextHop;
        SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
        SrRtrAltNextHopInfo.u1FRRNextHop = SR_LFA_NEXTHOP;
        MEMCPY (&SrRtrAltNextHopInfo.PrimarynextHop.Addr.u4Addr,
                &pSrOldNextHopInfo->nextHop.Addr.u4Addr,
                MAX_IPV4_ADDR_LEN);
        pSrRtrInfo->u4LfaOutIfIndex = 0;
        pSrRtrInfo->u4LfaSwapOutIfIndex = 0;
        pSrRtrInfo->u4LfaNextHop = 0;

    }

    /*Create new Next-Hop Node */
    if ((pNewSrRtrNextHopInfo =
                SrCreateNewNextHopNode ()) == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                "%s:%d pNewSrRtrNextHopInfo == NULL \n", __func__,
                __LINE__);
        return SR_FAILURE;
    }

    MEMCPY (&pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr,
            &(pRouteInfo->nextHop.Addr.u4Addr),
            MAX_IPV4_ADDR_LEN);
    pTempSrRtrInfo = SrGetRtrInfoFromNextHop(&pRouteInfo->nextHop);
    if (pTempSrRtrInfo != NULL)
    {
        MEMCPY (&u4TempNbrRtrId,
                &(pTempSrRtrInfo->prefixId.Addr.u4Addr),
                MAX_IPV4_ADDR_LEN);
    }
    else
    {
        u4TempNbrRtrId =  SrNbrRtrFromNextHop(pRouteInfo->nextHop.Addr.u4Addr);
    }

    pNewSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
    pNewSrRtrNextHopInfo->nbrRtrId = u4TempNbrRtrId;
    MEMCPY (&pNewSrRtrNextHopInfo->OldnextHop.Addr.u4Addr,
            &(pSrOldNextHopInfo->nextHop.Addr.u4Addr),
            MAX_IPV4_ADDR_LEN);
    pNewSrRtrNextHopInfo->u4OldOutIfIndex = pSrOldNextHopInfo->u4OutIfIndex;
    if (u1IsNewNHeqFRRNH != SR_TRUE)
    {
        pNewSrRtrNextHopInfo->u4OutIfIndex = pSrIpEvtInfo->u4IfIndex;
#ifdef CFA_WANTED
        if (CfaIfmCreateStackMplsTunnelInterface (pNewSrRtrNextHopInfo->u4OutIfIndex,
                    &u4MplsTnlIfIndex) == CFA_SUCCESS)
        {
            SR_TRC4 (SR_UTIL_TRC,
                    "%s:%d pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                    __func__, __LINE__, pNewSrRtrNextHopInfo->u4OutIfIndex,
                    u4MplsTnlIfIndex);
            pNewSrRtrNextHopInfo->u4OutIfIndex = u4MplsTnlIfIndex;
        }
        else
        {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                    "%s:%d FAILURE: pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                    __func__, __LINE__, pNewSrRtrNextHopInfo->u4OutIfIndex,
                    u4MplsTnlIfIndex);
            return SR_FAILURE;
        }
#endif
    }
    else
    {
        pNewSrRtrNextHopInfo->u4OutIfIndex = SrRtrAltNextHopInfo.u4OutIfIndex;
        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                 "%s:%d pNewSrRtrNextHopInfo->u4OutIfIndex = %d:",
                  __func__, __LINE__, pNewSrRtrNextHopInfo->u4OutIfIndex);

    }

    SRModifyNexthopHandle(pSrRtrInfo, pNewSrRtrNextHopInfo, pSrOldNextHopInfo);

    if (u1IsNewNHeqFRRNH == SR_TRUE)
    {
        pNewSrRtrNextHopInfo->bIsOnlyCPUpdate = SR_TRUE;
        u1Ret = SrMplsCreateOrDeleteLsp (MPLS_MLIB_FRR_FTN_MODIFY, pSrRtrInfo,
                                        NULL, pNewSrRtrNextHopInfo);
    }
    else
    {
        u1Ret = SrMplsCreateOrDeleteLsp (MPLS_MLIB_FTN_MODIFY, pSrRtrInfo,
                                        NULL, pNewSrRtrNextHopInfo);
    }

    if (u1Ret == SR_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC, "%s:%d FTN Modify Successful for"
                 " prefix = %x Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
    }
    else
    {
        if (SrMplsDeleteTunIntf (&pNewSrRtrNextHopInfo->u4OutIfIndex)
                == SR_FAILURE)
        {
            SR_TRC3 (SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d Failed to Delete FTN Tunnel for"
                    " prefix = %x \n", __func__, __LINE__,
                    pSrRtrInfo->prefixId.Addr.u4Addr);
            return SR_FAILURE;
        }

        SR_TRC5 (SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d FTN Modify Failed for"
                " prefix = %x Old Next hop = %x New Next hop = %x \n", __func__, __LINE__,
                pSrRtrInfo->prefixId.Addr.u4Addr,
                pSrOldNextHopInfo->nextHop.Addr.u4Addr, pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
        return SR_FAILURE;
    }

    if (u1IsNewNHeqFRRNH != SR_TRUE)
    {
        pNewSrRtrNextHopInfo->u4SwapOutIfIndex = pSrIpEvtInfo->u4IfIndex;
#ifdef CFA_WANTED
        u4MplsTnlIfIndex = 0;
        if (CfaIfmCreateStackMplsTunnelInterface (pNewSrRtrNextHopInfo->u4SwapOutIfIndex,
                    &u4MplsTnlIfIndex) == CFA_SUCCESS)
        {
            SR_TRC4 (SR_UTIL_TRC,
                    "%s:%d pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                    __func__, __LINE__,  pNewSrRtrNextHopInfo->u4SwapOutIfIndex,
                    u4MplsTnlIfIndex);
            pNewSrRtrNextHopInfo->u4SwapOutIfIndex = u4MplsTnlIfIndex;
        }
        else
        {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                    "%s:%d FAILURE: pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                    __func__, __LINE__,  pNewSrRtrNextHopInfo->u4SwapOutIfIndex,
                    u4MplsTnlIfIndex);
            return SR_FAILURE;
        }
#endif
    }
    else
    {
        pNewSrRtrNextHopInfo->u4SwapOutIfIndex =  SrRtrAltNextHopInfo.u4SwapOutIfIndex;
        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
        "%s:%d pNewSrRtrNextHopInfo-->u4SwapOutIfIndex = %d:",
         __func__, __LINE__, pNewSrRtrNextHopInfo->u4SwapOutIfIndex);

    }

    if (u1IsNewNHeqFRRNH == SR_TRUE)
    {
        u1Ret = SrMplsCreateOrDeleteLsp (MPLS_MLIB_FRR_ILM_MODIFY, pSrRtrInfo,
                                        &SrInSegInfo, pNewSrRtrNextHopInfo);
    }
    else
    {
        if (SrMplsDeleteTunIntf (&SrRtrAltNextHopInfo.u4SwapOutIfIndex)
            == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete ILM Tunnel for"
                     " prefix = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr);
        }

        u1Ret = SrMplsCreateOrDeleteLsp (MPLS_MLIB_ILM_MODIFY, pSrRtrInfo,
                                        &SrInSegInfo, pNewSrRtrNextHopInfo);
    }

    if (u1Ret == SR_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC, "%s:%d ILM Create Successful for"
                 " prefix = %x Next hop = %x \n ", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
    }
    else
    {
        if (SrReleaseMplsInLabel (pSrRtrInfo->u2AddrType,
                                  pSrRtrInfo->u4SidValue) == SR_FAILURE)
        {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to Release in label for"
                     " prefix = %x Next hop = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr,
                     pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
            return SR_FAILURE;
        }

        if (SrMplsDeleteTunIntf (&pNewSrRtrNextHopInfo->u4SwapOutIfIndex)
            == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete ILM Tunnel for"
                     " prefix = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr);
            return SR_FAILURE;
        }
        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d ILM Create Failed for"
                 " prefix = %x Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
        return SR_FAILURE;
    }

    if (u1IsNewNHeqFRRNH == TRUE)
    {
        pNewSrRtrNextHopInfo->bIsOnlyCPUpdate = SR_FALSE;
    }

    pSrOldNextHopInfo->bIsOnlyCPUpdate = SR_FALSE;
    TMO_SLL_Delete (&(pSrRtrInfo->NextHopList),
                    (tTMO_SLL_NODE *) pSrOldNextHopInfo);
    SR_RTR_NH_LIST_FREE (pSrOldNextHopInfo);

    TMO_SLL_Insert (&(pSrRtrInfo->NextHopList), NULL,
            &(pNewSrRtrNextHopInfo->nextNextHop));
    pNewSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrV4GetSrNextHopInfoFromRtr
 * Description   : This routine will return the Next hop info structure
 * Input(s)      : pSrRtrInfo - Router Info
 *
 * Output(s)     : tSrRtrNextHopInfo - Next hop info
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
tSrRtrNextHopInfo  *
SrV4GetSrNextHopInfoFromRtr (tSrRtrInfo * pSrRtrInfo, UINT4 u4NextHop)
{
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;

    TMO_SLL_Scan (&(pSrRtrInfo->NextHopList),
                  pSrRtrNextHopNode, tTMO_SLL_NODE *)
    {
        pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

        if ((pSrRtrNextHopInfo->nextHop.u2AddrType == SR_IPV4_ADDR_TYPE) &&
            (u4NextHop == pSrRtrNextHopInfo->nextHop.Addr.u4Addr))
        {
            return pSrRtrNextHopInfo;
        }
    }
    return NULL;
}

/*****************************************************************************
 * Function Name : SrHandleRouteSetEvent
 * Description   : This routine will delete the Primary LSPs and activate the
 *                 alternate LSPs (LFA/RLFA) when primary path goes down.
 * Input(s)      : pLfaRtInfo - IPv4 Lfa/Rlfa leak Route Info
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrHandleRouteSetEvent (tNetIpv4LfaRtInfo * pLfaRtInfo)
{
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY with \n", __func__, __LINE__);
    tSrTeRtEntryInfo    getSrTeRtEntryInfo;
    tSrRtrInfo          SrRtrInfo;
    tRouterId           rtrId;
    tSrRtrNextHopInfo   SrRtrAltNextHopInfo;
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRtrInfo         *pSrMandRtrInfo = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tGenU4Addr          DestAddr;
    tGenU4Addr          PrimaryNextHop;
    UINT4               u4SrGbMinIndex = SR_ZERO;

    MEMSET (&SrRtrAltNextHopInfo, SR_ZERO, sizeof (tSrRtrNextHopInfo));
    MEMSET (&SrRtrInfo, SR_ZERO, sizeof (tSrRtrInfo));
    MEMSET (&rtrId, SR_ZERO, sizeof (tRouterId));
    MEMSET (&DestAddr, 0, sizeof (tGenU4Addr));
    MEMSET (&getSrTeRtEntryInfo, 0, sizeof (tSrTeRtEntryInfo));
    MEMSET (&PrimaryNextHop, 0, sizeof (tGenU4Addr));

    SrRtrAltNextHopInfo.nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
    DestAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    DestAddr.Addr.u4Addr = pLfaRtInfo->u4DestNet;

    PrimaryNextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
    PrimaryNextHop.Addr.u4Addr = pLfaRtInfo->u4NextHop;
    pSrMandRtrInfo = SrGetRtrInfoFromNextHop (&PrimaryNextHop);
    if (pSrMandRtrInfo != NULL)
    {
        /* SR TE Processing via Mandatory node */
        if (SrTeLfaFtnActivate (pSrMandRtrInfo) == SR_FAILURE)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                     "%s:%d SR TE LSP Activation failed via Mandatory node for %x \n",
                     __func__, __LINE__, DestAddr.Addr.u4Addr);
        }
    }

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;
        if (pRtrInfo->u1LfaType == SR_LFA_ROUTE)
        {
            SR_TRC3 (SR_UTIL_TRC, "%s %d: Prefix %x is having LFA LSP \r\n",
                     __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
            /* Checking if the RTR is TE - DEstination  */
            if (pRtrInfo->bIsTeconfigured == TRUE)
            {
                /* To check LFA_ROUTE_SET is for TE ILM's Primary NextHop */
                if (pLfaRtInfo->u4NextHop ==
                    pRtrInfo->teSwapNextHop.Addr.u4Addr)
                {
                    SR_TRC3 (SR_UTIL_TRC,
                             "%s:%d Activating LFA ILM for TE prefix = %x\n",
                             __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
                    if (SrActivateLFA (pRtrInfo, NULL) == SR_FAILURE)
                    {
                        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                                 "%s:%d LFA ILM activation failed for TE prefix = %x\n",
                                 __func__, __LINE__,
                                 pRtrInfo->prefixId.Addr.u4Addr);
                        return SR_FAILURE;
                    }
                    pRtrInfo->bIsLfaActive = SR_LFA_ACTIVE;
                    SR_TRC3 (SR_UTIL_TRC,
                             "%s:%d LFA ILM activation success for TE prefix = %x\n",
                             __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);

                }
                /* continue since RTR is a SR-TE Destiantion   */
                continue;
            }
            /* Sr-TE: Ends */
            TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                          pSrRtrNextHopNode, tTMO_SLL_NODE *)
            {
                pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

                if (pSrRtrNextHopInfo->nextHop.Addr.u4Addr ==
                    pLfaRtInfo->u4NextHop)
                {
                    if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                        SR_FTN_CREATED)
                    {
                        SR_TRC3 (SR_UTIL_TRC,
                                 "%s:%d Activating LFA for prefix = %x\n",
                                 __func__, __LINE__,
                                 pRtrInfo->prefixId.Addr.u4Addr);
                        if (SrActivateLFA (pRtrInfo, pSrRtrNextHopInfo) ==
                            SR_FAILURE)
                        {
                            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                                     "%s:%d LFA activation failed for prefix = %x\n",
                                     __func__, __LINE__,
                                     pRtrInfo->prefixId.Addr.u4Addr);
                            return SR_FAILURE;
                        }
                        pRtrInfo->bIsLfaActive = SR_LFA_ACTIVE;
                        SR_TRC3 (SR_UTIL_TRC,
                                 "%s:%d LFA activation success for prefix = %x\n",
                                 __func__, __LINE__,
                                 pRtrInfo->prefixId.Addr.u4Addr);
                    }
                    break;
                }
                else
                {
                    SR_TRC2 (SR_UTIL_TRC, "%s:%d Peer does not match \n",
                             __func__, __LINE__);
                }
            }
        }
        else if (pRtrInfo->u1LfaType == SR_RLFA_ROUTE)
        {
            SR_TRC3 (SR_UTIL_TRC, "%s %d: Prefix %x is having RLFA LSP \r\n",
                     __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
            TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                          pSrRtrNextHopInfo, tSrRtrNextHopInfo *)
            {
                if ((MEMCMP (&(pSrRtrNextHopInfo->nextHop.Addr.u4Addr),
                             &(pLfaRtInfo->u4NextHop),
                             SR_IPV4_ADDR_LENGTH) == SR_ZERO)
                    && (pSrRtrNextHopInfo->bIsLfaNextHop != SR_TRUE))
                {
                    if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                        SR_FTN_CREATED)
                    {
                        SR_TRC3 (SR_UTIL_TRC,
                                 "%s:%d Activating RLFA for prefix = %x\n",
                                 __func__, __LINE__,
                                 pRtrInfo->prefixId.Addr.u4Addr);
                        if (SrActivateLFA (pRtrInfo, pSrRtrNextHopInfo) ==
                            SR_FAILURE)
                        {
                            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                                     "%s:%d RLFA activation failed for prefix = %x\n",
                                     __func__, __LINE__,
                                     pRtrInfo->prefixId.Addr.u4Addr);
                            return SR_FAILURE;
                        }
                        pRtrInfo->bIsLfaActive = SR_LFA_ACTIVE;
                        SR_TRC3 (SR_UTIL_TRC,
                                 "%s:%d RLFA activation success for prefix = %x\n",
                                 __func__, __LINE__,
                                 pRtrInfo->prefixId.Addr.u4Addr);

                        MEMSET (&DestAddr, SR_ZERO, sizeof (tGenU4Addr));
                        DestAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                        MEMCPY (&(DestAddr.Addr.u4Addr),
                                &(pRtrInfo->prefixId.Addr.u4Addr), sizeof (UINT4));
                        pSrRlfaPathInfo =
                            SrGetRlfaPathEntryFromDestAddr (&DestAddr);
                        if (pSrRlfaPathInfo != NULL)
                        {
                            pSrRlfaPathInfo->bIsRlfaActive = SR_ONE;
                        }
                    }
                    break;
                }
                else
                {
                    SR_TRC3 (SR_UTIL_TRC,
                             "%s:%d PeerId do not matches with Route Delete Event - %x \n",
                             __func__, __LINE__,
                             pSrRtrNextHopInfo->nextHop.Addr.u4Addr);
                }
            }
        }

    }                            /*RtrInfo Scan ends */
    UNUSED_PARAM (u4SrGbMinIndex);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrMplsDeleteTunIntf
 * Description   : This routine will delete the Tunnel Interface
 * Input(s)      : pSrRlfaPathInfo - RLFA path entry
 *                 u4MplsTnlIfIndex - Tunnel Index
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsDeleteTunIntf (UINT4 *u4MplsTnlIfIndex)
{
    UINT4               u4L3VlanIf = SR_ZERO;

#ifdef CFA_WANTED
    if (CfaUtilGetIfIndexFromMplsTnlIf (*u4MplsTnlIfIndex, &u4L3VlanIf, SR_TRUE)
        != CFA_FAILURE)
    {
        if (CfaIfmDeleteStackMplsTunnelInterface
            (u4L3VlanIf, *u4MplsTnlIfIndex) == CFA_FAILURE)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                     "%s:%d Failed to Delete ILM Tunnel Index = %d \n", __func__,
                     __LINE__, *u4MplsTnlIfIndex);
            return SR_FAILURE;
        }
        *u4MplsTnlIfIndex = u4L3VlanIf;
    }
    else
    {
        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete ILM Tunnel Index = %d \n",
                 __func__, __LINE__, *u4MplsTnlIfIndex);
        return SR_FAILURE;
    }
#endif
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SRModifyNexthopHandle
 * Description   : This routine update old nexthop info and delete Old Tunnel ID
 * Input(s)      : tSrRtrInfo - Sr Node Info
 *                 pSrNextHopInfo - New Next hop
 *                 pSrOldNextHopInfo - Old Next hop
 * Output(s)     : None
 * Return(s)     : None
 *****************************************************************************/
VOID
SRModifyNexthopHandle(tSrRtrInfo *pSrRtrInfo,
                           tSrRtrNextHopInfo * pSrNextHopInfo,
                           tSrRtrNextHopInfo * pSrOldNextHopInfo)
{
    pSrNextHopInfo->OldnextHop.Addr.u4Addr = pSrOldNextHopInfo->nextHop.Addr.u4Addr;
    pSrNextHopInfo->u4OldOutIfIndex = pSrOldNextHopInfo->u4OutIfIndex;
    if((pSrOldNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)
    {
        /* Delete the Old primary FTN Tunnel */
        if (SrMplsDeleteTunIntf (&pSrOldNextHopInfo->u4OutIfIndex) == SR_FAILURE)
        {
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete FTN Tunnel for"
                     " prefix = %x with old nexthop %x\n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr,
                     pSrOldNextHopInfo->nextHop.Addr.u4Addr);
        }

        /* Delete the primary ILM Tunnel */
        if (SrMplsDeleteTunIntf (&pSrOldNextHopInfo->u4SwapOutIfIndex)
            == SR_FAILURE)
        {
            SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete Primary ILM Tunnel for"
                     " prefix = %x with old nexthop %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr,
                     pSrOldNextHopInfo->nextHop.Addr.u4Addr);

        }
    }

    return;

}

/*****************************************************************************
 * Function Name : SrMplsModifyPrimaryLSP
 * Description   : This routine update old nexthop info and delete Old Tunnel ID
 * Input(s)      : tSrIpEvtInfo   - IP Evt Info
 *                 pSrRtrInfo     - Router Node Info
 *                 pSrNextHopInfo - Exisitng Primary Next hop
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsModifyPrimaryLSP (tSrIpEvtInfo * pSrIpEvtInfo,
                        tSrRtrInfo *pSrRtrInfo,
                        tSrRtrNextHopInfo * pSrNextHopInfo)
{
    tSrRtrNextHopInfo  *pNewNextHopInfo   = NULL;
    tSrRouteEntryInfo  *pRouteInfo        = NULL;
    tSrRtrInfo         *pSrNbrRtrInfo     = NULL;
    UINT4               u4NbrRtrId        = SR_ZERO;
    UINT4               u4MplsTnlIfIndex  = SR_ZERO;
    tSrInSegInfo        SrInSegInfo;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    if ((pSrIpEvtInfo == NULL) || (pSrRtrInfo == NULL) || (pSrNextHopInfo == NULL))
    {
        SR_TRC5 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Invalid Input pointer "
                 " pSrIpEvtInfo %x pSrRtrInfo %x pSrNextHopInfo %x \n", __func__, __LINE__,
                 pSrIpEvtInfo, pSrRtrInfo, pSrNextHopInfo);
        return SR_FAILURE;
    }

    pRouteInfo = (tSrRouteEntryInfo *) (VOID *) pSrIpEvtInfo->pRouteInfo;

    if (SrUtilCheckNodeIdConfigured () != SR_SUCCESS)
    {
        SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC, "%s:%d Node SID is not configured "
                 "for Router %x \n", __func__, __LINE__,
                pSrRtrInfo->advRtrId.Addr.u4Addr);
        return SR_SUCCESS;
    }

    pSrNbrRtrInfo =  SrGetRtrInfoFromNextHop (&pRouteInfo->nextHop);
    if (pSrNbrRtrInfo != NULL)
    {
        u4NbrRtrId = pSrNbrRtrInfo->advRtrId.Addr.u4Addr;
    }
    else
    {
        u4NbrRtrId =  SrNbrRtrFromNextHop (pRouteInfo->nextHop.Addr.u4Addr);
    }

    /*Create new Next-Hop Node */
    if ((pNewNextHopInfo = SrCreateNewNextHopNode ()) == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                "%s:%d pNewSrRtrNextHopInfo == NULL \n", __func__,
                __LINE__);
        return SR_FAILURE;
    }

    pNewNextHopInfo->nextHop.Addr.u4Addr = pRouteInfo->nextHop.Addr.u4Addr;
    pNewNextHopInfo->nextHop.u2AddrType  = SR_IPV4_ADDR_TYPE;
    pNewNextHopInfo->nbrRtrId            = u4NbrRtrId;
    pNewNextHopInfo->u4OutIfIndex        = pSrIpEvtInfo->u4IfIndex;
    pNewNextHopInfo->u4SwapOutIfIndex    = pSrIpEvtInfo->u4IfIndex;
    SRModifyNexthopHandle(pSrRtrInfo, pNewNextHopInfo, pSrNextHopInfo);

#ifdef CFA_WANTED
    if (CfaIfmCreateStackMplsTunnelInterface (pNewNextHopInfo->u4OutIfIndex,
                    &u4MplsTnlIfIndex) == CFA_SUCCESS)
    {
            SR_TRC4 (SR_UTIL_TRC,
                    "%s:%d pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                    __func__, __LINE__, pNewNextHopInfo->u4OutIfIndex,
                    u4MplsTnlIfIndex);
            pNewNextHopInfo->u4OutIfIndex = u4MplsTnlIfIndex;
    }
    else
    {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                    "%s:%d FAILURE: pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                    __func__, __LINE__, pNewNextHopInfo->u4OutIfIndex,
                    u4MplsTnlIfIndex);
            return SR_FAILURE;
    }
#endif

    /* Primary FTN Modify */
    if (SrMplsCreateOrDeleteLsp (MPLS_MLIB_FTN_MODIFY, pSrRtrInfo,
                                 NULL, pNewNextHopInfo) == SR_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC, "%s:%d FTN Modify Successful for"
                 " prefix = %x Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pNewNextHopInfo->nextHop.Addr.u4Addr);
    }
    else
    {
        if (SrMplsDeleteTunIntf (&pNewNextHopInfo->u4OutIfIndex) == SR_FAILURE)
        {
            SR_TRC3 (SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d Failed to Delete FTN Tunnel for"
                     " prefix = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr);
            return SR_FAILURE;
        }

        SR_TRC5 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d FTN Modify Failed for"
                 " prefix = %x Old Next hop = %x New Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pSrNextHopInfo->nextHop.Addr.u4Addr,
                 pNewNextHopInfo->nextHop.Addr.u4Addr);
        return SR_FAILURE;
    }

    SR_GET_SELF_IN_LABEL (&SrInSegInfo.u4InLabel, pSrRtrInfo->u4SidValue);
#ifdef CFA_WANTED
    u4MplsTnlIfIndex = 0;
    if (CfaIfmCreateStackMplsTunnelInterface (pNewNextHopInfo->u4SwapOutIfIndex,
                &u4MplsTnlIfIndex) == CFA_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC,
                "%s:%d pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                __func__, __LINE__,  pNewNextHopInfo->u4SwapOutIfIndex,
                u4MplsTnlIfIndex);
        pNewNextHopInfo->u4SwapOutIfIndex = u4MplsTnlIfIndex;
    }
    else
    {
        SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                "%s:%d FAILURE: pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                __func__, __LINE__,  pNewNextHopInfo->u4SwapOutIfIndex,
                u4MplsTnlIfIndex);
        return SR_FAILURE;
    }
#endif

    /* Primary ILM Modify */
    if (SrMplsCreateOrDeleteLsp (MPLS_MLIB_ILM_MODIFY, pSrRtrInfo,
                                 &SrInSegInfo, pNewNextHopInfo) == SR_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC, "%s:%d ILM Modify Successful for"
                 " prefix = %x Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pNewNextHopInfo->nextHop.Addr.u4Addr);
    }
    else
    {
        if (SrReleaseMplsInLabel (pSrRtrInfo->u2AddrType,
                                  pSrRtrInfo->u4SidValue) == SR_FAILURE)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to Release in label for"
                     " prefix = %x\n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr);
            return SR_FAILURE;
        }

        if (SrMplsDeleteTunIntf (&pNewNextHopInfo->u4SwapOutIfIndex)
            == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete ILM Tunnel for"
                     " prefix = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr);
            return SR_FAILURE;
        }

        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d ILM Create Failed for"
                 " prefix = %x Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pNewNextHopInfo->nextHop.Addr.u4Addr);
        return SR_FAILURE;
    }

    TMO_SLL_Delete (&(pSrRtrInfo->NextHopList),
                    (tTMO_SLL_NODE *) pSrNextHopInfo);
    SR_RTR_NH_LIST_FREE (pSrNextHopInfo);

    TMO_SLL_Insert (&(pSrRtrInfo->NextHopList), NULL,
            &(pNewNextHopInfo->nextNextHop));

    pNewNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}


/*****************************************************************************
 * Function Name : SrMplsModifyRlfaLSP
 * Description   : This routine will Update the FTN and ILM entries in Control
 *                 plane for RLFA.
 * Input(s)      : pSrRlfaPathInfo - RLFA path entry
 *                 pSrIpEvtInfo - IP Event information
 *                 pSrRtrInfo - Router information
 *                 u1IsNewNHeqFRRNH - Flag to denote if new NH is same as RLFA or not
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrMplsModifyRlfaLSP (tSrIpEvtInfo * pSrIpEvtInfo, tSrRtrInfo *pSrRtrInfo,
                    tSrRlfaPathInfo * pSrRlfaPathInfo, UINT1 u1IsNewNHeqFRRNH)
{
    tSrRouteEntryInfo  *pRouteInfo = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrInfo         *pTempSrRtrInfo = NULL;
    tSrRtrNextHopInfo  *pSrOldNextHopInfo = NULL;
    tSrRtrNextHopInfo  *pSrTmpNextHopInfo = NULL;
    tSrRtrNextHopInfo  *pSrRlfaNextHopInfo = NULL;
    tSrRtrNextHopInfo  *pNewSrRtrNextHopInfo = NULL;
    tSrInSegInfo        SrInSegInfo;
    UINT4               u4TempNbrRtrId = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT1               u1Ret            = SR_ZERO;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);
    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    pRouteInfo = (tSrRouteEntryInfo *) (VOID *) pSrIpEvtInfo->pRouteInfo;

    TMO_SLL_Scan (&(pSrRtrInfo->NextHopList),
                          pSrRtrNextHopNode, tTMO_SLL_NODE *)
    {
        pSrTmpNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
        if ((pSrTmpNextHopInfo->bIsLfaNextHop == SR_TRUE)
            && (pSrTmpNextHopInfo->u1FRRNextHop == SR_RLFA_NEXTHOP))
        {
            pSrRlfaNextHopInfo = pSrTmpNextHopInfo;
        }
        else
        {
            pSrOldNextHopInfo = pSrTmpNextHopInfo;
        }
    }

    if ((pSrOldNextHopInfo == NULL) || (pSrRlfaNextHopInfo == NULL))
    {
        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Next Hop information Not found for"
                 " prefix = %x \n", __func__, __LINE__,
                 pSrRlfaPathInfo->destAddr.Addr.u4Addr);
        return SR_FAILURE;
    }

    if (u1IsNewNHeqFRRNH == TRUE)
    {
        pSrOldNextHopInfo->bIsOnlyCPUpdate = SR_TRUE;
    }

    if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
    {
        /* Populate In-Label for SelfNode SRGB and PeerNode's SID Value */
        SR_GET_SELF_IN_LABEL (&SrInSegInfo.u4InLabel, pSrRtrInfo->u4SidValue);

        if (u1IsNewNHeqFRRNH == TRUE)
        {
            pSrRlfaNextHopInfo->bIsOnlyCPUpdate = SR_TRUE;
        }
    }

    pSrRtrInfo->u4LfaNextHop = pSrRlfaPathInfo->rlfaNextHop.Addr.u4Addr;
    pSrRtrInfo->u4LfaOutIfIndex = pSrRlfaPathInfo->u4RlfaTunIndex;
    pSrRtrInfo->u4LfaSwapOutIfIndex = pSrRlfaPathInfo->u4RlfaIlmTunIndex;

    /*Create new Next-Hop Node */
    if ((pNewSrRtrNextHopInfo =
                SrCreateNewNextHopNode ()) == NULL)
    {
        SR_TRC2 (SR_FAIL_TRC | SR_UTIL_TRC,
                "%s:%d pNewSrRtrNextHopInfo == NULL memory alloc failed \n", __func__,
                __LINE__);
        return SR_FAILURE;
    }

    MEMCPY (&pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr,
            &(pRouteInfo->nextHop.Addr.u4Addr),
            MAX_IPV4_ADDR_LEN);
    pTempSrRtrInfo = SrGetRtrInfoFromNextHop (&pRouteInfo->nextHop);
    if (pTempSrRtrInfo != NULL)
    {
        MEMCPY (&u4TempNbrRtrId,
                &(pTempSrRtrInfo->prefixId.Addr.u4Addr),
                MAX_IPV4_ADDR_LEN);
    }
    else
    {
        u4TempNbrRtrId = SrNbrRtrFromNextHop(pRouteInfo->nextHop.Addr.u4Addr);
    }

    pNewSrRtrNextHopInfo->nextHop.u2AddrType = SR_IPV4_ADDR_TYPE;
    pNewSrRtrNextHopInfo->nbrRtrId = u4TempNbrRtrId;
    if (u1IsNewNHeqFRRNH != SR_TRUE)
    {
        pNewSrRtrNextHopInfo->u4OutIfIndex = pSrIpEvtInfo->u4IfIndex;
#ifdef CFA_WANTED
        if (CfaIfmCreateStackMplsTunnelInterface (pNewSrRtrNextHopInfo->u4OutIfIndex,
                    &u4MplsTnlIfIndex) == CFA_SUCCESS)
    {
            SR_TRC4 (SR_UTIL_TRC,
                    "%s:%d pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                    __func__, __LINE__, pNewSrRtrNextHopInfo->u4OutIfIndex,
                    u4MplsTnlIfIndex);
            pNewSrRtrNextHopInfo->u4OutIfIndex = u4MplsTnlIfIndex;
        }
        else
        {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                    "%s:%d FAILURE: pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                    __func__, __LINE__, pNewSrRtrNextHopInfo->u4OutIfIndex,
                    u4MplsTnlIfIndex);
            return SR_FAILURE;
        }
#endif
    }
    else
    {
        pNewSrRtrNextHopInfo->u4OutIfIndex = pSrRlfaNextHopInfo->u4OutIfIndex;
        SR_TRC4 (SR_UTIL_TRC,
                 "%s:%d FRR Modify u4OutIfIndex = %d for the Dest %x:",
                  __func__, __LINE__, pNewSrRtrNextHopInfo->u4OutIfIndex,
                  pRouteInfo->destAddr.Addr.u4Addr);
    }

    SRModifyNexthopHandle(pSrRtrInfo, pNewSrRtrNextHopInfo, pSrOldNextHopInfo);

    if (u1IsNewNHeqFRRNH == SR_TRUE)
    {
        pNewSrRtrNextHopInfo->bIsOnlyCPUpdate = SR_TRUE;
        u1Ret = SrMplsCreateOrDeleteLsp (MPLS_MLIB_FRR_FTN_MODIFY, pSrRtrInfo,
                                        NULL, pNewSrRtrNextHopInfo);
    }
    else
    {
        u1Ret = SrMplsCreateOrDeleteLsp (MPLS_MLIB_FTN_MODIFY, pSrRtrInfo,
                                        NULL, pNewSrRtrNextHopInfo);
    }

    if (u1Ret == SR_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC, "%s:%d FTN Modify Successful for"
                 " prefix = %x Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
    }
    else
    {
        if (SrMplsDeleteTunIntf (&pNewSrRtrNextHopInfo->u4OutIfIndex)
            == SR_FAILURE)
        {
            SR_TRC3 (SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d Failed to Delete FTN Tunnel for"
                     " prefix = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr);
            return SR_FAILURE;
        }

        SR_TRC5 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d FTN Modify Failed for"
                 " prefix = %x Old Next hop = %x New Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pSrOldNextHopInfo->nextHop.Addr.u4Addr, pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
        return SR_FAILURE;
    }

    if (u1IsNewNHeqFRRNH != SR_TRUE)
    {
        pNewSrRtrNextHopInfo->u4SwapOutIfIndex = pSrIpEvtInfo->u4IfIndex;
#ifdef CFA_WANTED
        u4MplsTnlIfIndex = 0;
        if (CfaIfmCreateStackMplsTunnelInterface (pNewSrRtrNextHopInfo->u4SwapOutIfIndex,
                    &u4MplsTnlIfIndex) == CFA_SUCCESS)
        {
            SR_TRC4 (SR_UTIL_TRC,
                    "%s:%d pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_SUCCESS \n",
                    __func__, __LINE__, pNewSrRtrNextHopInfo->u4SwapOutIfIndex,
                    u4MplsTnlIfIndex);
            pNewSrRtrNextHopInfo->u4SwapOutIfIndex = u4MplsTnlIfIndex;
        }
        else
        {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC,
                    "%s:%d FAILURE: pSrRtrNextHopInfo->u4OutIfIndex = %d: u4MplsTnlIfIndex %d : CFA_FAILURE \n",
                    __func__, __LINE__, pNewSrRtrNextHopInfo->u4SwapOutIfIndex,
                    u4MplsTnlIfIndex);
            return SR_FAILURE;
        }
#endif
    }
    else
    {
        pNewSrRtrNextHopInfo->u4SwapOutIfIndex = pSrRlfaPathInfo->u4RlfaIlmTunIndex;
    }

    pSrRtrInfo->u4LfaNextHop = SR_ZERO;
    pSrRtrInfo->u4LfaOutIfIndex = SR_ZERO;
    pSrRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;
    pSrRtrInfo->u1LfaType = SR_ZERO;
    pSrRtrInfo->bIsLfaActive = SR_ZERO;

    if (u1IsNewNHeqFRRNH == SR_TRUE)
    {
        u1Ret = SrMplsCreateOrDeleteLsp (MPLS_MLIB_FRR_ILM_MODIFY, pSrRtrInfo,
                                        &SrInSegInfo, pNewSrRtrNextHopInfo);
    }
    else
    {
        if (SrMplsDeleteTunIntf (&(pSrRlfaPathInfo->u4RlfaIlmTunIndex))
            == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete ILM Tunnel for"
                     " prefix = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr);
        }

        u1Ret = SrMplsCreateOrDeleteLsp (MPLS_MLIB_ILM_MODIFY, pSrRtrInfo,
                                        &SrInSegInfo, pNewSrRtrNextHopInfo);
    }

    if (u1Ret == SR_SUCCESS)
    {
        SR_TRC4 (SR_UTIL_TRC, "%s:%d ILM Create Successful for"
                 " prefix = %x Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
    }
    else
    {
        if (SrReleaseMplsInLabel (pSrRtrInfo->u2AddrType,
                                  pSrRtrInfo->u4SidValue) == SR_FAILURE)
        {
            SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d Failed to Release in label for"
                     " prefix = %x Next hop = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr,
                     pSrOldNextHopInfo->nextHop.Addr.u4Addr);
            return SR_FAILURE;
        }

        if (SrMplsDeleteTunIntf (&pSrOldNextHopInfo->u4SwapOutIfIndex)
            == SR_FAILURE)
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Failed to Delete ILM Tunnel for"
                     " prefix = %x \n", __func__, __LINE__,
                     pSrRtrInfo->prefixId.Addr.u4Addr);
            return SR_FAILURE;
        }

        SR_TRC4 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d ILM Create Failed for"
                 " prefix = %x Next hop = %x \n", __func__, __LINE__,
                 pSrRtrInfo->prefixId.Addr.u4Addr,
                 pNewSrRtrNextHopInfo->nextHop.Addr.u4Addr);
        return SR_FAILURE;
    }

    if (u1IsNewNHeqFRRNH == TRUE)
    {
        pNewSrRtrNextHopInfo->bIsOnlyCPUpdate = SR_FALSE;
    }

    if (SrDeleteRlfaPathTableEntry (pSrRlfaPathInfo) == SR_FAILURE)
    {
        SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                "%s:%d RLFA RBTree deletion failed for prefix = %x \n",
                __func__, __LINE__, pSrRtrInfo->prefixId.Addr.u4Addr);
        return SNMP_FAILURE;
    }

    TMO_SLL_Delete (&(pSrRtrInfo->NextHopList),
                    (tTMO_SLL_NODE *) pSrRlfaNextHopInfo);
    SR_RTR_NH_LIST_FREE (pSrRlfaNextHopInfo);
    TMO_SLL_Delete (&(pSrRtrInfo->NextHopList),
                    (tTMO_SLL_NODE *) pSrOldNextHopInfo);
    SR_RTR_NH_LIST_FREE (pSrOldNextHopInfo);

    TMO_SLL_Insert (&(pSrRtrInfo->NextHopList), NULL,
            &(pNewSrRtrNextHopInfo->nextNextHop));
    pNewSrRtrNextHopInfo->u1MPLSStatus |= SR_FTN_CREATED;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d EXIT \n", __func__, __LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrNbrInfoCleanup
 * Description   : This routine is used to delete All the Nbr info upon
 *                 deregistring from OSPF.
 * Input(s)      : None
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC VOID
SrNbrInfoCleanup (UINT4 u4AddrType)
{
    tTMO_SLL_NODE      *pOspfNbr = NULL;
    tTMO_SLL_NODE      *pTempOspfNbr = NULL;

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d ENTRY \n", __func__, __LINE__);

    switch (u4AddrType)
    {
        case SR_IPV4_ADDR_TYPE:
            TMO_DYN_SLL_Scan (&(gSrV4OspfNbrList), pOspfNbr, pTempOspfNbr,
                              tTMO_SLL_NODE *)
        {
            TMO_SLL_Delete (&(gSrV4OspfNbrList), pOspfNbr);
            SR_OSPFV4_NBR_INFO_MEM_FREE ((tSrV4OspfNbrInfo *) pOspfNbr);
        }
            break;
        case SR_IPV6_ADDR_TYPE:
            TMO_DYN_SLL_Scan (&(gSrV6OspfNbrList), pOspfNbr, pTempOspfNbr,
                              tTMO_SLL_NODE *)
        {
            TMO_SLL_Delete (&(gSrV6OspfNbrList), pOspfNbr);
            SR_OSPFV4_NBR_INFO_MEM_FREE ((tSrV6OspfNbrInfo *) pOspfNbr);
        }
            break;
        default:
            SR_TRC2 (SR_UTIL_TRC, "%s:%d Invalid Addr Type \n", __func__,
                     __LINE__);
            break;
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC, "%s:%d EXIT \n", __func__, __LINE__);
    return;
}

tSrStaticEntryInfo *
SrGetStaticSrTableEntry (tSrStaticEntryInfo * pSrStaticInfo)
{
    tSrStaticEntryInfo  SrStaticInfo;

    MEMSET (&SrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));

    if (gSrGlobalInfo.pSrStaticRbTree == NULL || pSrStaticInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Sr Static Info is NULL \n", __func__,
                                      __LINE__);
        return NULL;
    }

    SrStaticInfo.destAddr.Addr.u4Addr = pSrStaticInfo->destAddr.Addr.u4Addr;
    SrStaticInfo.u4DestMask = pSrStaticInfo->u4DestMask;
    SrStaticInfo.NextHop.Addr.u4Addr = pSrStaticInfo->NextHop.Addr.u4Addr;
    SrStaticInfo.NextHop.u2AddrType = pSrStaticInfo->NextHop.u2AddrType;
    SrStaticInfo.destAddr.u2AddrType = pSrStaticInfo->destAddr.u2AddrType;
    SrStaticInfo.u4LspId = pSrStaticInfo->u4LspId;

    if (SrStaticInfo.destAddr.Addr.u4Addr != SR_ZERO)
    {
        return (RBTreeGet (gSrGlobalInfo.pSrStaticRbTree, &SrStaticInfo));
    }
    return (RBTreeGetNext (gSrGlobalInfo.pSrStaticRbTree, &SrStaticInfo, NULL));
}

/************************************************************************
 * Function Name   : SrGetNextStaticSrTableEntry
 * Description     : Function to get Static Sr from DestAddr, Mask
 *
 * Input           : tSrStaticEntryInfo
 * Output          : None
 * Returns         : Pointer to Static Sr Entry or NULL
 *************************************************************************/
tSrStaticEntryInfo *
SrGetNextStaticSrTableEntry (tSrStaticEntryInfo * pSrStaticInfo)
{
    tSrStaticEntryInfo  SrStaticInfo;
    tSrStaticEntryInfo  dummySrStaticInfo;

    MEMSET (&SrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));
    MEMSET (&dummySrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));

    if (gSrGlobalInfo.pSrStaticRbTree == NULL || pSrStaticInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Sr Static Info is NULL \n", __func__,
                                                       __LINE__);
        return NULL;
    }

    if (MEMCMP (pSrStaticInfo, &dummySrStaticInfo, sizeof (tSrStaticEntryInfo))
        == SR_ZERO)
    {
        return (RBTreeGetFirst (gSrGlobalInfo.pSrStaticRbTree));
    }

    SrStaticInfo.destAddr.Addr.u4Addr = pSrStaticInfo->destAddr.Addr.u4Addr;
    SrStaticInfo.u4DestMask = pSrStaticInfo->u4DestMask;
    SrStaticInfo.NextHop.Addr.u4Addr = pSrStaticInfo->NextHop.Addr.u4Addr;
    SrStaticInfo.NextHop.u2AddrType = pSrStaticInfo->NextHop.u2AddrType;
    SrStaticInfo.destAddr.u2AddrType = pSrStaticInfo->destAddr.u2AddrType;

    return (RBTreeGetNext (gSrGlobalInfo.pSrStaticRbTree, &SrStaticInfo, NULL));
}

/************************************************************************
 *  Function Name   : SrGetStaticSrFromDestAddr
 *  Description     : Fetches Static SR info from Static SR RBTREE
 *  Input           : Destination IP
 *  Output          : (tSrStaticEntryInfo) Static SR info
 *  Returns         : NONE
 *************************************************************************/
tSrStaticEntryInfo *
SrGetStaticSrFromDestAddr (tGenU4Addr * pDestAddr, UINT4 u4LspId)
{
    tSrStaticEntryInfo  SrStaticInfo;

    MEMSET (&SrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));
    SR_TRC3 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC , "%s:%d Entry with %x \n", __func__, __LINE__,
             pDestAddr->Addr.u4Addr);

    if ((gSrGlobalInfo.pSrStaticRbTree == NULL)
        || (pDestAddr->Addr.u4Addr == SR_ZERO))
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Sr Static RbTree is NULL and Destination address is Zero \n",
                __func__,__LINE__);
        return NULL;
    }

    SrStaticInfo.destAddr.Addr.u4Addr = pDestAddr->Addr.u4Addr;
    SrStaticInfo.u4LspId = u4LspId;
    return (RBTreeGet (gSrGlobalInfo.pSrStaticRbTree, &SrStaticInfo));
}

/************************************************************************
 * Function Name   : SrStaticGetDynamicNextHop
 * Description     : Function to find and get dynamic nexthop based on top-sid at creation time
 * Returns         : SR_FAILURE/SR_SUCCESS
 *************************************************************************/
UINT4
SrStaticGetDynamicNextHop (tGenU4Addr* pDestAddr, UINT4 u4LspId)
{
    tSrStaticEntryInfo* pStaticSrInfo = NULL;
    tTMO_SLL_NODE* pRtrNode = NULL;
    tSrRtrInfo* pRtrInfo = NULL;
    tTMO_SLL_NODE* pRtrNextHopNode = NULL;
    tSrRtrNextHopInfo* pRtrNextHopInfo = NULL;

    pStaticSrInfo = SrGetStaticSrFromDestAddr (pDestAddr, u4LspId);
    if (!pStaticSrInfo) {
        SR_TRC4 (SR_FAIL_TRC, "[%s: %d] ERROR! SrStaticEntry not found! prefix: %x lspid: %d\n",
                __func__, __LINE__, pDestAddr->Addr.u4Addr, u4LspId);
        return SR_FAILURE;
    }

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *) {

        pRtrInfo = (tSrRtrInfo*) pRtrNode;
        if (pRtrInfo /*&& (pRtrInfo->u4SidType == SR_SID_NODE)*/
                && ((pRtrInfo->u4SidValue + pRtrInfo->srgbRange.u4SrGbMinIndex) == (pStaticSrInfo->SrStaticLblStack.u4TopLabel))) {

            if (pRtrInfo->u1SIDStatus == SR_SID_STATUS_DOWN) {
                SR_TRC3 (SR_FAIL_TRC, "[%s: %d] Top-SID: %d status DOWN!\n",
                        __func__, __LINE__, pStaticSrInfo->SrStaticLblStack.u4TopLabel);

                SrStaticGenerateAlarm(pStaticSrInfo, SRTE_UNKNOWN_SID_ALARM, SRTE_ALARM_RAISE);
                return SR_FAILURE;
            }

            TMO_SLL_Scan (&(pRtrInfo->NextHopList), pRtrNextHopNode, tTMO_SLL_NODE *) {

                pRtrNextHopInfo = (tSrRtrNextHopInfo*) pRtrNextHopNode;
                if (pRtrNextHopInfo && ((pRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)) {

                    MEMCPY (&(pStaticSrInfo->DynamicNextHop), &(pRtrNextHopInfo->nextHop), sizeof(tGenU4Addr));

                    SR_TRC6 (SR_CRITICAL_TRC, "[%s: %d] SrStaticEntry prefix: %x lspid: %d got and set with dynamic nexthop: %x from Top-SID: %d\n",
                            __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId,
                            pStaticSrInfo->DynamicNextHop.Addr.u4Addr, pStaticSrInfo->SrStaticLblStack.u4TopLabel);
                    return SR_SUCCESS;
                }
            }
        }
    }

    SR_TRC3 (SR_FAIL_TRC, "[%s: %d] Top-SID: %d not found!\n",
            __func__, __LINE__, pStaticSrInfo->SrStaticLblStack.u4TopLabel);
    SrStaticGenerateAlarm(pStaticSrInfo, SRTE_UNKNOWN_SID_ALARM, SRTE_ALARM_RAISE);
    return SR_FAILURE;
}

/************************************************************************
 * Function Name   : SrStaticSetDynamicNextHop
 * Description     : Function to find and set dynamic nexthop based on top-sid events
 * Returns         : SR_FAILURE/SR_SUCCESS
 *************************************************************************/
UINT4
SrStaticSetDynamicNextHop (tSrRtrInfo* pRtrInfo, UINT1 u1SidEvt)
{
    tSrStaticEntryInfo* pStaticSrInfo = NULL;
    tTMO_SLL_NODE* pRtrNextHopNode = NULL;
    tSrRtrNextHopInfo* pRtrNextHopInfo = NULL;

    if (!pRtrInfo) {
        SR_TRC2 (SR_FAIL_TRC, "[%s: %d] ERROR! pRtrInfo is NULL!\n", __func__, __LINE__);
        return SR_FAILURE;
    }

    pStaticSrInfo = RBTreeGetFirst(gSrGlobalInfo.pSrStaticRbTree);
    while (pStaticSrInfo) {

        if (pStaticSrInfo->NextHop.Addr.u4Addr == SR_ZERO) {

        if (/*(pRtrInfo->u4SidType == SR_SID_NODE)
                &&*/ ((pRtrInfo->u4SidValue + pRtrInfo->srgbRange.u4SrGbMinIndex) == (pStaticSrInfo->SrStaticLblStack.u4TopLabel))) {

            switch (u1SidEvt) {

                case SR_SID_STATUS_DOWN:

                    SrStaticSetRowStatus(pStaticSrInfo, NOT_IN_SERVICE);

                    SR_TRC6 (SR_CRITICAL_TRC, "[%s: %d] SrStaticEntry prefix: %x lspid: %d removed dynamic nexthop: %x from Top-SID: %d SR_SID_STATUS_DOWN\n",
                            __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId,
                            pStaticSrInfo->DynamicNextHop.Addr.u4Addr, pStaticSrInfo->SrStaticLblStack.u4TopLabel);

                    MEMSET (&(pStaticSrInfo->DynamicNextHop), 0, sizeof(tGenU4Addr));
                    pStaticSrInfo->pTopSIDRtrInfo = NULL;

                    SrStaticGenerateAlarm(pStaticSrInfo, SRTE_UNKNOWN_SID_ALARM, SRTE_ALARM_RAISE);
                    break;

                case SR_SID_STATUS_UP:

                    TMO_SLL_Scan (&(pRtrInfo->NextHopList), pRtrNextHopNode, tTMO_SLL_NODE *) {

                        pRtrNextHopInfo = (tSrRtrNextHopInfo *) pRtrNextHopNode;
                        if (pRtrNextHopInfo && ((pRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) == SR_FTN_CREATED)) {

                            MEMCPY (&(pStaticSrInfo->DynamicNextHop), &(pRtrNextHopInfo->nextHop), sizeof(tGenU4Addr));
                            pStaticSrInfo->pTopSIDRtrInfo = pRtrInfo;

                            SR_TRC6 (SR_CRITICAL_TRC, "[%s: %d] SrStaticEntry prefix: %x lspid: %d added dynamic nexthop: %x from Top-SID: %d SR_SID_STATUS_UP\n",
                                    __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId,
                                    pStaticSrInfo->DynamicNextHop.Addr.u4Addr, pStaticSrInfo->SrStaticLblStack.u4TopLabel);

                            SrStaticSetRowStatus(pStaticSrInfo, ACTIVE);
                            SrStaticClearAllAlarm(pStaticSrInfo);
                        }
                    }
                    break;

                default:
                    break;
            }
        }
        }
        pStaticSrInfo = RBTreeGetNext(gSrGlobalInfo.pSrStaticRbTree, pStaticSrInfo, NULL);
    }

    return SR_SUCCESS;
}

/************************************************************************
 * Function Name   : SrStaticSetRowStatus
 * Description     : Function to set row status for srstatic entry
 * Returns         : SR_FAILURE/SR_SUCCESS
 *************************************************************************/
UINT4
SrStaticSetRowStatus (tSrStaticEntryInfo* pStaticSrInfo, INT4 i4RowStatus)
{
    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE* pSnmpDestAddr = NULL;
    UINT1 au1DestAddr[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    INT4 i4CurrRowStatus = 0;

    if (!pStaticSrInfo) {
        SR_TRC2 (SR_FAIL_TRC, "[%s: %d] ERROR! pStaticSrInfo is NULL!\n", __func__, __LINE__);
        return SR_FAILURE;
    }

    MEMSET (&snmpDestAddr, SR_ZERO, sizeof(tSNMP_OCTET_STRING_TYPE));
    snmpDestAddr.pu1_OctetList = au1DestAddr;
    pSnmpDestAddr = &snmpDestAddr;
    SR_INTEGER_TO_OCTETSTRING (pStaticSrInfo->destAddr.Addr.u4Addr, pSnmpDestAddr);

    if (nmhGetFsStaticSrRowStatus(pSnmpDestAddr, pStaticSrInfo->u4DestMask, pStaticSrInfo->destAddr.u2AddrType,
                pStaticSrInfo->u4LspId, &i4CurrRowStatus) == SNMP_FAILURE) {
        SR_TRC5 (SR_FAIL_TRC, "[%s: %d] ERROR! prefix: %x lspid: %d rowStatus: %d\n", __func__, __LINE__,
                pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId, i4RowStatus);
        return SR_FAILURE;
    }

    if (i4CurrRowStatus == i4RowStatus) {
        SR_TRC5 (SR_CRITICAL_TRC, "[%s: %d] RowStatus already set. prefix: %x lspid: %d rowStatus: %d\n", __func__, __LINE__,
                pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId, i4RowStatus);
        return SR_SUCCESS;
    }

    if (nmhSetFsStaticSrRowStatus (pSnmpDestAddr, pStaticSrInfo->u4DestMask, pStaticSrInfo->destAddr.u2AddrType,
                pStaticSrInfo->u4LspId, i4RowStatus) == SNMP_FAILURE) {
        SR_TRC5 (SR_FAIL_TRC, "[%s: %d] ERROR! prefix: %x lspid: %d rowStatus: %d\n", __func__, __LINE__,
                pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId, i4RowStatus);
        return SR_FAILURE;
    }

    SR_TRC5 (SR_CRITICAL_TRC, "[%s: %d] RowStatus set. prefix: %x lspid: %d rowStatus: %d\n", __func__, __LINE__,
            pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId, i4RowStatus);

    return SR_SUCCESS;
}

/************************************************************************
 * Function Name   : SrStaticGenerateAlarm
 * Description     : Function to report alarm
 * Returns         : SR_FAILURE/SR_SUCCESS
 *************************************************************************/
UINT4
SrStaticGenerateAlarm (tSrStaticEntryInfo* pStaticSrInfo, UINT1 u1Type, UINT1 u1Raise)
{
    if (!pStaticSrInfo) {
        SR_TRC2 (SR_FAIL_TRC, "[%s: %d] ERROR! pStaticSrInfo is NULL!\n", __func__, __LINE__);
        return SR_FAILURE;
    }
#ifdef TEJAS_WANTED
    UINT1 addrType=SR_IPV4_ADDR_TYPE;
#endif

    switch(u1Type) {

        case SRTE_UNKNOWN_SID_ALARM:
            if (u1Raise == SRTE_ALARM_RAISE) {
                if (pStaticSrInfo->u1UnknownSIDAlarm == SRTE_ALARM_RAISE) {
                    SR_TRC4 (SR_CRITICAL_TRC, "[%s: %d] SRTE_UNKNOWN_SID_ALARM already raised.. Returning. prefix: %x lspid: %d\n",
                            __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId);
                    return SR_SUCCESS;
                }
                pStaticSrInfo->u1UnknownSIDAlarm = SRTE_ALARM_RAISE;
#ifdef TEJAS_WANTED
                gSrTeAlarm(addrType,pStaticSrInfo->destAddr.Addr.u4Addr,pStaticSrInfo->u4LspId,SRTE_UNKNOWN_SID_ALARM,SRTE_ALARM_RAISE);
#endif
                SR_TRC4 (SR_CRITICAL_TRC, "[%s: %d] SRTE_UNKNOWN_SID_ALARM raised! prefix: %x lspid: %d\n",
                    __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId);
            }
            else if (u1Raise == SRTE_ALARM_CLEAR) {
                if (pStaticSrInfo->u1UnknownSIDAlarm == SRTE_ALARM_CLEAR) {
                    SR_TRC4 (SR_CRITICAL_TRC, "[%s: %d] SRTE_UNKNOWN_SID_ALARM already cleared.. Returning. prefix: %x lspid: %d\n",
                            __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId);
                    return SR_SUCCESS;
                }
                pStaticSrInfo->u1UnknownSIDAlarm = SRTE_ALARM_CLEAR;
#ifdef TEJAS_WANTED
                gSrTeAlarm(addrType,pStaticSrInfo->destAddr.Addr.u4Addr,pStaticSrInfo->u4LspId,SRTE_UNKNOWN_SID_ALARM,SRTE_ALARM_CLEAR);
#endif

                SR_TRC4 (SR_CRITICAL_TRC, "[%s: %d] SRTE_UNKNOWN_SID_ALARM cleared! prefix: %x lspid: %d\n",
                    __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId);

            }
            break;

        case SRTE_SID_EXCEEDS_SRGB_ALARM:
            if (u1Raise == SRTE_ALARM_RAISE) {
                if (pStaticSrInfo->u1SIDExceedsSRGBAlarm == SRTE_ALARM_RAISE) {
                    SR_TRC4 (SR_CRITICAL_TRC, "[%s: %d] SRTE_SID_EXCEEDS_SRGB_ALARM already raised.. Returning. prefix: %x lspid: %d\n",
                            __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId);
                    return SR_SUCCESS;
                }
                pStaticSrInfo->u1SIDExceedsSRGBAlarm = SRTE_ALARM_RAISE;
#ifdef TEJAS_WANTED
                gSrTeAlarm(addrType,pStaticSrInfo->destAddr.Addr.u4Addr,pStaticSrInfo->u4LspId,SRTE_SID_EXCEEDS_SRGB_ALARM,   SRTE_ALARM_RAISE);
#endif

                SR_TRC4 (SR_CRITICAL_TRC, "[%s: %d] SRTE_SID_EXCEEDS_SRGB_ALARM raised! prefix: %x lspid: %d\n",
                    __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId);
            }
            else if (u1Raise == SRTE_ALARM_CLEAR) {
                if (pStaticSrInfo->u1SIDExceedsSRGBAlarm == SRTE_ALARM_CLEAR) {
                    SR_TRC4 (SR_CRITICAL_TRC, "[%s: %d] SRTE_SID_EXCEEDS_SRGB_ALARM already cleared.. Returning. prefix: %x lspid: %d\n",
                            __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId);
                    return SR_SUCCESS;
                }
                pStaticSrInfo->u1SIDExceedsSRGBAlarm = SRTE_ALARM_CLEAR;
#ifdef TEJAS_WANTED
                gSrTeAlarm(addrType,pStaticSrInfo->destAddr.Addr.u4Addr,pStaticSrInfo->u4LspId,SRTE_SID_EXCEEDS_SRGB_ALARM,  SRTE_ALARM_CLEAR);
#endif

                SR_TRC4 (SR_CRITICAL_TRC, "[%s: %d] SRTE_SID_EXCEEDS_SRGB_ALARM cleared! prefix: %x lspid: %d\n",
                    __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId);
            }
            break;

        default:
            SR_TRC5 (SR_FAIL_TRC, "[%s: %d] ERROR! Unknown SR-TE Alarm Type! prefix: %x lspid: %d alarmtype: %d\n",
                    __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId, u1Type);
            break;
    }
    return SR_SUCCESS;
}

/************************************************************************
 * Function Name   : SrStaticClearAllAlarm
 * Description     : Function to clear all alarms
 * Returns         : SR_FAILURE/SR_SUCCESS
 *************************************************************************/
UINT4
SrStaticClearAllAlarm (tSrStaticEntryInfo* pStaticSrInfo)
{
    if (!pStaticSrInfo) {
        SR_TRC2 (SR_FAIL_TRC, "[%s: %d] ERROR! pStaticSrInfo is NULL!\n", __func__, __LINE__);
        return SR_FAILURE;
    }

    if (pStaticSrInfo->u1UnknownSIDAlarm == SRTE_ALARM_RAISE) {
        SR_TRC4 (SR_CRITICAL_TRC, "[%s: %d] SRTE_UNKNOWN_SID_ALARM present.. clearing. prefix: %x lspid: %d\n",
                __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId);
        SrStaticGenerateAlarm(pStaticSrInfo, SRTE_UNKNOWN_SID_ALARM, SRTE_ALARM_CLEAR);
    }

    if (pStaticSrInfo->u1SIDExceedsSRGBAlarm == SRTE_ALARM_RAISE) {
        SR_TRC4 (SR_CRITICAL_TRC, "[%s: %d] SRTE_SID_EXCEEDS_SRGB_ALARM present.. clearing. prefix: %x lspid: %d\n",
                __func__, __LINE__, pStaticSrInfo->destAddr.Addr.u4Addr, pStaticSrInfo->u4LspId);
        SrStaticGenerateAlarm(pStaticSrInfo, SRTE_SID_EXCEEDS_SRGB_ALARM, SRTE_ALARM_CLEAR);
    }

    return SR_SUCCESS;
}

/************************************************************************
 * Function Name   : SrStaticProcessCreateOrDel
 * Description     : Function to create or delete Static Segment-Routing
 * Input           : NONE
 * Output          : None
 * Returns         : SR_FAILURE/SR_SUCCESS
 *************************************************************************/
INT4
SrStaticProcessCreateOrDel (tGenU4Addr * pDestAddr, UINT4 u4StaticEnable,
                            UINT4 u4LspId, UINT4 *pu4MplsTunIndex)
{
    tSrStaticEntryInfo *pStaticSrInfo = NULL;
    tGenU4Addr          tmpAddr;
    tGenU4Addr          tmpNextHopAddr;

    /*MplsFTNCreate Starts */
    tLspInfo            LspInfo;
    tNHLFE              Nhlfe;
    UINT2               u2MplsOperation = SR_ZERO;
    UINT4               u4OutIfIndex = SR_ZERO;
    UINT4               u4MplsTnlIfIndex = SR_ZERO;
    UINT4               u4StackCount = SR_ZERO;
    UINT4               u4Label = SR_ZERO;
    UINT4               u4StaticSrLabelStackIndex = SR_ZERO;
    UINT1               au1DestAddr[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE *pSnmpDestAddr = NULL;

    MEMSET (&snmpDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));
    MEMSET (&LspInfo, SR_ZERO, sizeof (tLspInfo));
    MEMSET (&Nhlfe, SR_ZERO, sizeof (tNHLFE));

    MEMSET (&tmpAddr, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&tmpNextHopAddr, SR_ZERO, sizeof (tGenU4Addr));

    pSnmpDestAddr = &snmpDestAddr;
    snmpDestAddr.pu1_OctetList = au1DestAddr;

    pStaticSrInfo = SrGetStaticSrFromDestAddr (pDestAddr, u4LspId);

    if (pStaticSrInfo != NULL)
    {
        if (u4StaticEnable == SR_TRUE)
        {
            /* MplsFTNCreate call starts */
            SR_INTEGER_TO_OCTETSTRING (pDestAddr->Addr.u4Addr, pSnmpDestAddr);

            MEMCPY (&(LspInfo.FecParams.DestAddrPrefix.u4Addr),
                    &(pDestAddr->Addr.u4Addr), SR_IPV4_ADDR_LENGTH);
            LspInfo.FecParams.u2AddrType = SR_IPV4_ADDR_TYPE;

            LspInfo.FecParams.u1FecType = SR_FEC_PREFIX_TYPE;
            LspInfo.FecParams.DestMask.u4Addr = SR_IPV4_DEST_MASK;

            LspInfo.Direction = MPLS_DIRECTION_FORWARD;
            u2MplsOperation = MPLS_OPR_PUSH;

            Nhlfe.u1Operation = (UINT1) u2MplsOperation;

            if (pStaticSrInfo->NextHop.Addr.u4Addr != SR_ZERO) {
            Nhlfe.NextHopAddr.u4Addr = pStaticSrInfo->NextHop.Addr.u4Addr,
                CfaIpIfGetIfIndexForNetInCxt (SR_ZERO,
                                              pStaticSrInfo->NextHop.
                                              Addr.u4Addr, &u4OutIfIndex);
            }
            else if (pStaticSrInfo->DynamicNextHop.Addr.u4Addr != SR_ZERO) {
                Nhlfe.NextHopAddr.u4Addr = pStaticSrInfo->DynamicNextHop.Addr.u4Addr,
                    CfaIpIfGetIfIndexForNetInCxt (SR_ZERO, pStaticSrInfo->DynamicNextHop.
                                              Addr.u4Addr, &u4OutIfIndex);
            }
            else {
                SR_TRC4 (SR_FAIL_TRC, "[%s: %d] ERROR! Both static and dynamic nexthop not set! prefix: %x lspid: %d\n",
                        __func__, __LINE__, pDestAddr->Addr.u4Addr, u4LspId);
                return SR_FAILURE;
            }

            if (CfaIfmCreateStackMplsTunnelInterface
                (u4OutIfIndex, &u4MplsTnlIfIndex) == CFA_SUCCESS)
            {
                SR_TRC4 (SR_UTIL_TRC,
                         "[%s: %d] u4MplsTnlIfIndex %d u4OutIfIndex %d \n",
                         __func__, __LINE__, u4MplsTnlIfIndex, u4OutIfIndex);
            }
            *pu4MplsTunIndex = u4MplsTnlIfIndex;
            nmhGetFsStaticSrLabelStackIndex (pSnmpDestAddr,
                                             pStaticSrInfo->u4DestMask,
                                             (INT4) SR_IPV4_ADDR_TYPE,
                                             pStaticSrInfo->u4LspId,
                                             &u4StaticSrLabelStackIndex);
            for (u4StackCount = SR_ZERO;
                 u4StackCount < MAX_MPLSDB_LABELS_PER_ENTRY; u4StackCount++)
            {
                nmhGetFsStaticSrLabelStackLabel (u4StaticSrLabelStackIndex,
                                                 u4StackCount, &u4Label);
                if (u4Label == SR_ZERO)
                {
#ifdef SR_WANTED
                    LspInfo.SrTeLblStack.u1StackSize =
                        (UINT1) (u4StackCount - 1);
#endif
                    break;
                }
                if (u4StackCount == SR_ZERO)
                {
                    Nhlfe.u4OutLabel = u4Label;
                }
                else
                {
#ifdef SR_WANTED
                    LspInfo.SrTeLblStack.u4LabelStack[u4StackCount - 1] =
                        u4Label;
#endif
                }
            }
#ifdef SR_WANTED
            LspInfo.u1Owner = MPLS_OWNER_SR_TE;
            LspInfo.u1Preference = MPLS_FTN_SR_TE_PREFERENCE;
#endif
            Nhlfe.u1NHAddrType = SR_IPV4_ADDR_TYPE;
            Nhlfe.u1OperStatus = MPLS_STATUS_UP;
            Nhlfe.u4OutIfIndex = u4MplsTnlIfIndex;

            LspInfo.pNhlfe = (tNHLFE *) (&Nhlfe);
            LspInfo.u4LspId = pStaticSrInfo->u4LspId;

            SR_TRC6 (SR_UTIL_TRC, "[%s: %d] Calling MplsMlibUpdate with LSP parameters: Operation: MPLS_MLIB_FTN_CREATE Destination: %x LSP-ID: %d Owner: %d Preference: %d \n",
                    __func__, __LINE__, LspInfo.FecParams.DestAddrPrefix.u4Addr, LspInfo.u4LspId, LspInfo.u1Owner, LspInfo.u1Preference);
            LspInfo.u1Protocol = MPLS_PROTOCOL_SR_OSPF;
            if (MplsMlibUpdate (MPLS_MLIB_FTN_CREATE, &LspInfo) == MPLS_SUCCESS)
            {
                SR_TRC2 (SR_UTIL_TRC, "[%s : %d] MplsMlibUpdate Success\n",
                         __func__, __LINE__);
            }
            else
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "[%s : %d] MplsMlibUpdate Failure\n",
                         __func__, __LINE__);
            }

            /* MplsFTNCreate call ends */
            return SR_SUCCESS;
        }
        else
        {
            SR_INTEGER_TO_OCTETSTRING (pDestAddr->Addr.u4Addr, pSnmpDestAddr);
            /* MplsFTNDelete call starts */
            MEMCPY (&(LspInfo.FecParams.DestAddrPrefix.u4Addr),
                    &(pDestAddr->Addr.u4Addr), SR_IPV4_ADDR_LENGTH);
            LspInfo.FecParams.u2AddrType = SR_IPV4_ADDR_TYPE;

            LspInfo.FecParams.u1FecType = SR_FEC_PREFIX_TYPE;
            LspInfo.FecParams.DestMask.u4Addr = SR_IPV4_DEST_MASK;

            LspInfo.Direction = MPLS_DIRECTION_FORWARD;

            u2MplsOperation = MPLS_OPR_PUSH;

            Nhlfe.u1Operation = (UINT1) u2MplsOperation;

            if (pStaticSrInfo->NextHop.Addr.u4Addr != SR_ZERO) {
            Nhlfe.NextHopAddr.u4Addr = pStaticSrInfo->NextHop.Addr.u4Addr,
                u4MplsTnlIfIndex = *pu4MplsTunIndex;
            }
            else if (pStaticSrInfo->DynamicNextHop.Addr.u4Addr != SR_ZERO) {
                Nhlfe.NextHopAddr.u4Addr = pStaticSrInfo->DynamicNextHop.Addr.u4Addr,
                    u4MplsTnlIfIndex = *pu4MplsTunIndex;
            }
            else {
                SR_TRC4 (SR_FAIL_TRC, "[%s: %d] ERROR! Both static and dynamic nexthop not set! prefix: %x lspid: %d\n",
                        __func__, __LINE__, pDestAddr->Addr.u4Addr, u4LspId);
                return SR_FAILURE;
            }


            nmhGetFsStaticSrLabelStackIndex (pSnmpDestAddr,
                                             pStaticSrInfo->u4DestMask,
                                             (INT4) SR_IPV4_ADDR_TYPE,
                                             pStaticSrInfo->u4LspId,
                                             &u4StaticSrLabelStackIndex);
            for (u4StackCount = SR_ZERO;
                 u4StackCount < MAX_MPLSDB_LABELS_PER_ENTRY; u4StackCount++)
            {
                nmhGetFsStaticSrLabelStackLabel (u4StaticSrLabelStackIndex,
                                                 u4StackCount, &u4Label);
#ifdef SR_WANTED
                if (u4Label == SR_ZERO)
                {
                    LspInfo.SrTeLblStack.u1StackSize =
                        (UINT1) (u4StackCount - 1);
                    break;
                }
#endif
                if (u4StackCount == SR_ZERO)
                {
                    Nhlfe.u4OutLabel = u4Label;
                }
                else
                {
#ifdef SR_WANTED
                    LspInfo.SrTeLblStack.u4LabelStack[u4StackCount] = u4Label;
#endif
                }
            }
#ifdef SR_WANTED

            LspInfo.u1Owner = MPLS_OWNER_SR_TE;
#endif
            Nhlfe.u1NHAddrType = SR_IPV4_ADDR_TYPE;
            Nhlfe.u4OutIfIndex = u4MplsTnlIfIndex;

            LspInfo.pNhlfe = (tNHLFE *) (&Nhlfe);
            LspInfo.u4LspId = pStaticSrInfo->u4LspId;

            SR_TRC6 (SR_UTIL_TRC, "[%s: %d] Calling MplsMlibUpdate with LSP parameters: Operation: MPLS_MLIB_FTN_DELETE Destination: %x LSP-ID: %d Owner: %d Preference: %d \n",
                    __func__, __LINE__, LspInfo.FecParams.DestAddrPrefix.u4Addr, LspInfo.u4LspId, LspInfo.u1Owner, LspInfo.u1Preference);

            if (MplsMlibUpdate (MPLS_MLIB_FTN_DELETE, &LspInfo) == MPLS_SUCCESS)
            {
                SR_TRC2 (SR_UTIL_TRC, "[%s : %d] MplsMlibUpdate Success\n",
                         __func__, __LINE__);
            }
            else
            {
                SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC, "[%s : %d] MplsMlibUpdate Failure\n",
                         __func__, __LINE__);
            }
            if (CfaUtilGetIfIndexFromMplsTnlIf (u4MplsTnlIfIndex, &u4OutIfIndex,
                                                TRUE) != CFA_FAILURE)
            {
                if (CfaIfmDeleteStackMplsTunnelInterface
                    (u4OutIfIndex, u4MplsTnlIfIndex) == CFA_SUCCESS)
                {
                    SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                             "[%s: %d]L u4MplsTnlIfIndex %d u4OutIfIndex %d \n",
                             __func__, __LINE__, u4MplsTnlIfIndex,
                             u4OutIfIndex);
                }
            }
            /* MplsFTNDelete call ends */
            return SR_SUCCESS;
        }
    }
    else
    {
        SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC, "%s:%d SrGetStaticSrFromDestAddr FAILURE for %x \n",
                 __func__, __LINE__, pDestAddr->Addr.u4Addr);
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

tSrStaticEntryInfo *
SrCreateStaticSrTableEntry (tSrStaticEntryInfo * pSrStaticInfo)
{
    tSrStaticEntryInfo *pNewSrStaticInfo = NULL;
    UINT4               u4ErrCode = SR_ZERO;
    UINT1               au1DestAddr[SR_IPV4_ADDR_LENGTH] = { SR_ZERO };
    UINT4               u4LabelIndex = SR_ZERO;
    tSNMP_OCTET_STRING_TYPE snmpDestAddr;
    tSNMP_OCTET_STRING_TYPE *pSnmpDestAddr = NULL;

    /*Allocate memory for tSrStaticEntryInfo */
    if (SR_STATIC_ENTRY_MEM_ALLOC (pNewSrStaticInfo) == NULL)
    {
        SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                "%s:%d Mem Alloc Failure for Static Sr Entry label : \n",
                __func__,__LINE__);
        return NULL;
    }

    MEMSET (pNewSrStaticInfo, SR_ZERO, sizeof (tSrStaticEntryInfo));
    MEMSET (&snmpDestAddr, SR_ZERO, sizeof (tSNMP_OCTET_STRING_TYPE));

    snmpDestAddr.pu1_OctetList = au1DestAddr;

    pNewSrStaticInfo->destAddr.Addr.u4Addr =
        pSrStaticInfo->destAddr.Addr.u4Addr;
    pNewSrStaticInfo->destAddr.u2AddrType = pSrStaticInfo->destAddr.u2AddrType;

    pNewSrStaticInfo->u4DestMask = pSrStaticInfo->u4DestMask;
    pNewSrStaticInfo->u4LspId = pSrStaticInfo->u4LspId;
    pNewSrStaticInfo->pTopSIDRtrInfo = NULL;
    pSnmpDestAddr = &snmpDestAddr;

    TMO_SLL_Init (&pNewSrStaticInfo->srStaticLblListIndex.StaticLblList);

    if (nmhGetFsLabelStackIndexNext (&(u4LabelIndex)) == SNMP_FAILURE)
    {
        SR_TRC1 (SR_UTIL_TRC | SR_FAIL_TRC, "Static Sr List Index not available : %d\n",
                 u4LabelIndex);
        SR_STATIC_ENTRY_MEM_FREE (pNewSrStaticInfo);
        return NULL;
    }
#ifdef MPLS_WANTED
    u4LabelIndex = SrStaticLblListSetIndex (u4LabelIndex);
#endif
    if (RBTreeAdd (gSrGlobalInfo.pSrStaticRbTree, (tRBElem *) pNewSrStaticInfo)
        == RB_FAILURE)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC,
                "Create Sr-Static Table : RBTree Node Add Failed \n");
        SrStaticLblListRelIndex (u4LabelIndex);
        SR_STATIC_ENTRY_MEM_FREE (pNewSrStaticInfo);
        return NULL;
    }

    nmhTestv2FsStaticSrLabelStackIndex (&u4ErrCode, NULL, SR_ZERO, SR_ZERO, SR_ZERO,
                                        u4LabelIndex);

    if (u4ErrCode == SNMP_ERR_NO_ERROR)
    {
        SR_INTEGER_TO_OCTETSTRING (pNewSrStaticInfo->destAddr.Addr.u4Addr,
                                   pSnmpDestAddr);

        nmhSetFsStaticSrLabelStackIndex (pSnmpDestAddr,
                                         pNewSrStaticInfo->u4DestMask,
                                         pNewSrStaticInfo->destAddr.u2AddrType,
                                         pNewSrStaticInfo->u4LspId,
                                         u4LabelIndex);
    }
    else
    {
        SR_TRC (SR_UTIL_TRC, "Invalid NextHop \n");
        SrStaticLblListRelIndex (u4LabelIndex);
        SR_STATIC_ENTRY_MEM_FREE (pNewSrStaticInfo);
        return NULL;
    }

    return pNewSrStaticInfo;
}

INT4
SrDeleteStaticSrTableEntry (tSrStaticEntryInfo * pSrStaticInfo)
{
    tTMO_SLL_NODE      *pSrStaticLbl = NULL;
    tTMO_SLL_NODE      *pTmpSrStaticLbl = NULL;

    if (pSrStaticInfo == NULL)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC,
                "Delete Sr Static Table : pSrStaticInfo == NULL \n");
        return SR_FAILURE;
    }

    if (RBTreeRem (gSrGlobalInfo.pSrStaticRbTree, pSrStaticInfo) == NULL)
    {
        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC,
                "Delete Sr Static Table : RBTree Node Delete Failed \n");
        return SR_FAILURE;
    }
    TMO_DYN_SLL_Scan (&(pSrStaticInfo->srStaticLblListIndex.StaticLblList),
                      pSrStaticLbl, pTmpSrStaticLbl, tTMO_SLL_NODE *)
    {
        TMO_SLL_Delete (&(pSrStaticInfo->srStaticLblListIndex.StaticLblList),
                        (tTMO_SLL_NODE *) pSrStaticLbl);
        SR_STATIC_LBL_MEM_FREE (pSrStaticLbl);
    }

    /*Release memory for tSrStaticEntryInfo */
    SrStaticLblListRelIndex (pSrStaticInfo->srStaticLblListIndex.u4Index);
    SR_STATIC_ENTRY_MEM_FREE (pSrStaticInfo);

    SR_TRC (SR_UTIL_TRC, "Deleted from RbTree \n");
    return SR_SUCCESS;
}

tStaticSrLbl       *
SrCreateStaticSrLblEntry (UINT4 u4LabelStackIndex, UINT4 u4SrLabelIndex)
{
    tSrStaticEntryInfo *pSrTmpStaticInfo = NULL;
    tSrStaticEntryInfo *pSrStaticInfo = NULL;
    tStaticSrLbl       *pSrStaticLbl = NULL;

    pSrTmpStaticInfo = RBTreeGetFirst (gSrGlobalInfo.pSrStaticRbTree);

    while (1)
    {
        if (pSrTmpStaticInfo == NULL)
        {
            SR_TRC2 (SR_UTIL_TRC, "%s:%d Static Sr Table empty : \n",
                     __func__, __LINE__);
            return NULL;
        }

        if (u4LabelStackIndex == pSrTmpStaticInfo->srStaticLblListIndex.u4Index)
        {
            pSrStaticInfo = pSrTmpStaticInfo;
            break;
        }
        pSrTmpStaticInfo =
            RBTreeGetNext (gSrGlobalInfo.pSrStaticRbTree, pSrTmpStaticInfo,
                           NULL);
    }
    if (pSrStaticInfo == NULL)
    {
        SR_TRC3 (SR_UTIL_TRC,
                 "%s:%d No entry found with index %d in Sr Static Table \n",
                 __func__, __LINE__, u4LabelStackIndex);
        return NULL;
    }
    if (SR_STATIC_LBL_MEM_ALLOC (pSrStaticLbl) == NULL)
    {
        SR_TRC2 (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                 "%s:%d Mem Alloc Failure for Static Sr label : \n", __func__,
                 __LINE__);
        return NULL;
    }
    MEMSET (pSrStaticLbl, SR_ZERO, sizeof (tStaticSrLbl));
    pSrStaticLbl->u4LblIndex = u4SrLabelIndex;
    TMO_SLL_Add (&(pSrStaticInfo->srStaticLblListIndex.StaticLblList),
                 (tTMO_SLL_NODE *) pSrStaticLbl);

    return pSrStaticLbl;
}

/************************************************************************
 * **  Function Name   : SrFTNRbTreeCmpFunc
 * **  Description     : RBTree Compare function for FTN Table
 * **  Input           : Two RBTree Nodes to be compared
 * **  Output          : None
 * **  Returns         : SR_RB_LESS/SR_RB_GREATER/SR_RB_EQUAL
 * *************************************************************************/
PUBLIC INT4
SrFTNRbTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2)
{
    tSrTnlIfTable *srTnlIfTable1 = (tSrTnlIfTable *) pRBElem1;
    tSrTnlIfTable *srTnlIfTable2 = (tSrTnlIfTable *) pRBElem2;

    if (srTnlIfTable1->u4Prefix >
        srTnlIfTable2->u4Prefix)
    {
        return SR_RB_LESS;
    }
    else if (srTnlIfTable1->u4Prefix <
             srTnlIfTable2->u4Prefix)
    {
        return SR_RB_GREATER;
    }
    return SR_RB_EQUAL;
}

/************************************************************************
 * **  Function Name   : SrILMRbTreeCmpFunc
 * **  Description     : RBTree Compare function for ILM Tree
 * **  Input           : Two RBTree Nodes to be compared
 * **  Output          : None
 * **  Returns         : SR_RB_LESS/SR_RB_GREATER/SR_RB_EQUAL
 * *************************************************************************/
PUBLIC INT4
SrILMRbTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2)
{
    tSrTnlIfTable *srTnlIfTable1 = (tSrTnlIfTable *) pRBElem1;
    tSrTnlIfTable *srTnlIfTable2 = (tSrTnlIfTable *) pRBElem2;

    if (srTnlIfTable1->u4Prefix >
        srTnlIfTable2->u4Prefix)
    {
        return SR_RB_LESS;
    }
    else if (srTnlIfTable1->u4Prefix <
             srTnlIfTable2->u4Prefix )
    {
        return SR_RB_GREATER;
    }
    return SR_RB_EQUAL;
}

/************************************************************************
 *  Function Name   : SrStaticRbTreeCmpFunc
 *  Description     : RBTree Compare function for Static-SR
 *  Input           : Two RBTree Nodes to be compared
 *  Output          : None
 *  Returns         : SR_ZERO(FOUND)/SR_ONE(NOT_FOUND)
 ************************************************************************/
PUBLIC INT4
SrStaticRbTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2)
{
    tSrStaticEntryInfo *srStaticEntry1 = (tSrStaticEntryInfo *) pRBElem1;
    tSrStaticEntryInfo *srStaticEntry2 = (tSrStaticEntryInfo *) pRBElem2;

    if (srStaticEntry1->destAddr.Addr.u4Addr >
        srStaticEntry2->destAddr.Addr.u4Addr)
    {
        return SR_RB_LESS;
    }
    else if (srStaticEntry1->destAddr.Addr.u4Addr <
             srStaticEntry2->destAddr.Addr.u4Addr)
    {
        return SR_RB_GREATER;
    }

    if (srStaticEntry1->u4LspId >
        srStaticEntry2->u4LspId)
    {
        return SR_RB_LESS;
    }
    else if (srStaticEntry1->u4LspId <
             srStaticEntry2->u4LspId)
    {
        return SR_RB_GREATER;
    }

    return SR_RB_EQUAL;
}

UINT4
SrProcessRouteNextHopChangeEvent (tSrIpEvtInfo * pSrIpEvtInfo)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tSrRtrNextHopInfo  *pSrPrimaryNextHop = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRouteEntryInfo  *pRouteInfo = NULL;
    tGenU4Addr          destAddr;
    UINT4               u4DestAddr = SR_ZERO;
    UINT4               u4PrefixId = SR_ZERO;
    UINT4               u4OldNextHop = SR_ZERO;
    UINT4               u4NextHop    = SR_ZERO;
    UINT1               u1IsOldNHFound   = SR_FALSE;
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));

    if (pSrIpEvtInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                "%s:%d pSrIpEvtInfo == NULL \n",__func__, __LINE__);
        return SR_FAILURE;
    }

    pRouteInfo = (tSrRouteEntryInfo *) (VOID *) pSrIpEvtInfo->pRouteInfo;

    if (pRouteInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                "%s:%d pRouteInfo == NULL \n",__func__, __LINE__);
        return SR_FAILURE;
    }

    MEMCPY (&(destAddr.Addr.u4Addr), &(pRouteInfo->destAddr.Addr.u4Addr),
            SR_IPV4_ADDR_LENGTH);
    destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    u4DestAddr   = pRouteInfo->destAddr.Addr.u4Addr;
    u4NextHop    = pRouteInfo->nextHop.Addr.u4Addr;
    u4OldNextHop = pRouteInfo->u4OldNextHop;

    SR_TRC5 (SR_UTIL_TRC, "%s:%d SR-PRIMARY Modify Route Process for prefix %x"
                " New Next Hop %x Old Next Hop %x \r\n",
            __func__, __LINE__, u4DestAddr, u4NextHop, u4OldNextHop);

    if (u4NextHop == u4OldNextHop)
    {
        SR_TRC5 (SR_CRITICAL_TRC, "%s:%d SR-PRIMARY Modify Route Process for prefix %x"
                    " New Next Hop %x Old Next Hop %x  are same \r\n",
                __func__, __LINE__, u4DestAddr, u4NextHop, u4OldNextHop);
        return SR_SUCCESS;
    }

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;
        MEMCPY (&u4PrefixId, &pRtrInfo->prefixId, sizeof (UINT4));

        if (u4DestAddr == u4PrefixId)
        {
            TMO_SLL_Scan (&(pRtrInfo->NextHopList), pSrRtrNextHopNode,
                        tTMO_SLL_NODE *)
            {
                pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
                if (pSrRtrNextHopInfo->u1FRRNextHop == SR_PRIMARY_NEXTHOP)
                {
                    pSrPrimaryNextHop = pSrRtrNextHopInfo;
                    if (u4OldNextHop == pSrRtrNextHopInfo->nextHop.Addr.u4Addr)
                    {
                        u1IsOldNHFound = SR_TRUE;
                        break;
                    }
                }
            }
            break;
        }
    }

    if (u1IsOldNHFound == SR_TRUE)
    {
        SR_TRC4 (SR_UTIL_TRC,
            "%s:%d Old Primary NH %x is same as Old Nexthop for Prefix %x \n",
            __func__, __LINE__, u4OldNextHop, u4DestAddr);

        if (SrMplsModifyPrimaryLSP (pSrIpEvtInfo, pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                    "%s:%d SrMplsModifyRlfaLSP for Prefix = %x failed \n",
                    __func__, __LINE__, u4DestAddr);
            return SR_FAILURE;
        }
    }
    else if (NULL != pSrPrimaryNextHop)
    {
        SR_TRC5 (SR_UTIL_TRC | SR_CRITICAL_TRC,
            "%s:%d Old Primary NH %x is not same as Old Nexthop %x for Prefix %x \n",
            __func__, __LINE__, pSrPrimaryNextHop->nextHop.Addr.u4Addr,
            u4OldNextHop, u4DestAddr);

        if (SrMplsModifyPrimaryLSP (pSrIpEvtInfo, pRtrInfo, pSrPrimaryNextHop) == SR_FAILURE)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                    "%s:%d SrMplsModifyRlfaLSP for Prefix = %x failed \n",
                    __func__, __LINE__, u4DestAddr);
            return SR_FAILURE;
        }
    }
    else
    {
        pSrIpEvtInfo->u4BitMap = SR_RT_NEW;
        if (SrProcessRouteChangeEvent (pSrIpEvtInfo) == SR_FAILURE)
        {
           SR_TRC4 (SR_FAIL_TRC | SR_UTIL_TRC | SR_CRITICAL_TRC,
                   "%s:%d Addition of New Primary LSP Failed for Route %x Next Hop %x \n",
                   __func__, __LINE__, pRouteInfo->destAddr.Addr.u4Addr,
                   pRouteInfo->nextHop.Addr.u4Addr);
           return SR_FAILURE;
        }
    }

    return SR_SUCCESS;
}


/******************************************************************************
 * Function Name      : SrProcessFRRModifyRoute
 * Description        : This function handles the modify FRR Event
 * Input(s)           : Route information
 * Output(s)          : NONE
 * Return Value(s)    : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/

UINT4
SrProcessFRRModifyRoute(tSrIpEvtInfo       *pSrIpEvtInfo,
                        tSrRtrInfo         *pRtrInfo,
                        tSrRtrNextHopInfo  *pSrRtrNextHopInfo)
{
    tSrRlfaPathInfo    *pSrRlfaPathInfo = NULL;
    tSrTilfaPathInfo    SrTiLfaPathInfo;
    tSrTilfaPathInfo    *pSrTiLfaPathInfo = NULL;
    tSrRouteEntryInfo  *pRouteInfo = NULL;
    tGenU4Addr          destAddr;
    tNetIpv4LfaRtInfo   LfaRtInfo;
    UINT4               u4DestAddr = SR_ZERO;
    UINT1               u1IsNewNHeqFRRNH = SR_ZERO;

    MEMSET (&SrTiLfaPathInfo, SR_ZERO, sizeof (tSrTilfaPathInfo));


    pRouteInfo = (tSrRouteEntryInfo *) (VOID *) pSrIpEvtInfo->pRouteInfo;

    MEMCPY (&(destAddr.Addr.u4Addr), &(pRouteInfo->destAddr.Addr.u4Addr),
            SR_IPV4_ADDR_LENGTH);
    destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    u4DestAddr = pRouteInfo->destAddr.Addr.u4Addr;

    if ((pRtrInfo != NULL) && (pSrRtrNextHopInfo != NULL))
    {
        if (pRtrInfo->u1LfaType == SR_RLFA_ROUTE)
        {
            pSrRlfaPathInfo = SrGetRlfaPathEntryFromDestAddr (&destAddr);
            if (pSrRlfaPathInfo != NULL)
            {
                if (pSrRlfaPathInfo->rlfaNextHop.Addr.u4Addr ==
                        pRouteInfo->nextHop.Addr.u4Addr)
                {
                    SR_TRC2 (SR_UTIL_TRC,
                            "%s:%d Nexthop of RLFA route and converged primary route are same. \n",
                            __func__, __LINE__);
                    u1IsNewNHeqFRRNH = SR_TRUE;
                }
                else
                {
                    SR_TRC2 (SR_UTIL_TRC,
                            "%s:%d Nexthop of RLFA route and converged primary route are different. \n",
                            __func__, __LINE__);
                    u1IsNewNHeqFRRNH = SR_FALSE;
                }

                if (SrMplsModifyRlfaLSP (pSrIpEvtInfo, pRtrInfo, pSrRlfaPathInfo, u1IsNewNHeqFRRNH) == SR_FAILURE)
                {
                    SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC,
                            "%s:%d SrMplsModifyRlfaLSP for Prefix = %x failed \n",
                            __func__, __LINE__, u4DestAddr);
                    return SR_FAILURE;
                }
            }

        }
        else if (pRtrInfo->u1LfaType == SR_TILFA_ROUTE)
        {
            SrTiLfaPathInfo.destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
            SrTiLfaPathInfo.destAddr.Addr.u4Addr = destAddr.Addr.u4Addr;
            pSrTiLfaPathInfo = RBTreeGet (gSrGlobalInfo.pSrTilfaPathRbTree,
                                        &SrTiLfaPathInfo);
            if (pSrTiLfaPathInfo != NULL)
            {
                if (pSrTiLfaPathInfo->lfaNextHopAddr.Addr.u4Addr ==
                        pRouteInfo->nextHop.Addr.u4Addr)
                {
                    SR_TRC2 (SR_UTIL_TRC,
                            "%s:%d Nexthop of TILFA route and converged primary route are same. \n",
                            __func__, __LINE__);
                    u1IsNewNHeqFRRNH = SR_TRUE;
                }
                else
                {
                    SR_TRC2 (SR_UTIL_TRC,
                            "%s:%d Nexthop of TILFA route and converged primary route are different. \n",
                            __func__, __LINE__);
                    u1IsNewNHeqFRRNH = SR_FALSE;
                }

                if (SrMplsModifyTilfaLSP (pSrIpEvtInfo, pRtrInfo, pSrTiLfaPathInfo, u1IsNewNHeqFRRNH) == SR_FAILURE)
                {
                    SR_TRC3 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                            "%s:%d SrMplsModifyTilfaLSP for Prefix = %x failed \n",
                            __func__, __LINE__, u4DestAddr);
                    return SR_FAILURE;
                }
            }
        }
        else
        {
            if (pRtrInfo->u4LfaNextHop ==
                    pRouteInfo->nextHop.Addr.u4Addr)
            {
                SR_TRC2 (SR_UTIL_TRC,
                        "%s:%d Nexthop of LFA route and converged primary route are same. \n",
                        __func__, __LINE__);
                u1IsNewNHeqFRRNH = SR_TRUE;
            }
            else
            {
                SR_TRC2 (SR_UTIL_TRC,
                        "%s:%d Nexthop of LFA route and converged primary route are different. \n",
                        __func__, __LINE__);
                u1IsNewNHeqFRRNH = SR_FALSE;
            }

            if (SrMplsModifyLSP (pSrIpEvtInfo, pRtrInfo, pSrRtrNextHopInfo, u1IsNewNHeqFRRNH) ==
                    SR_FAILURE)
            {
                SR_TRC4 (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                        "%s:%d SrMplsModifyLSP failed for Prefix = %x Nexthop = %x \n",
                        __func__, __LINE__, u4DestAddr,
                        pRouteInfo->nextHop.Addr.u4Addr);
                return SR_FAILURE;
            }
        }
        pRtrInfo->bIsLfaActive = SR_FALSE;
        pRtrInfo->u4LfaNextHop = SR_FALSE;
        pRtrInfo->u1LfaType = SR_ZERO;
        pRtrInfo->u4LfaOutIfIndex = SR_ZERO;
        pRtrInfo->u4LfaSwapOutIfIndex = SR_ZERO;

        return SR_SUCCESS;
    }

    return SR_SUCCESS;
}

/******************************************************************************
 * Function Name      : SrStartRtDelayTimer
 * Description        : This function starts Delay timer for MPLSDB update for Rt change
 * Input(s)           : NONE
 * Output(s)          : NONE
 * Return Value(s)    : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
UINT4
SrStartRtDelayTimer ()
{
    UINT4  u4Ticks      = (SYS_NUM_OF_TIME_UNITS_IN_A_SEC*5);

    if (SR_FALSE == gSrGlobalInfo.bIsDelayTimerStarted)
    {
        if (TmrStartTimer (SR_TMR_LIST_ID, &(SR_TMR_RT_DELAY_NODE),
                            u4Ticks) == TMR_FAILURE)
        {
            SR_TRC2 (SR_FAIL_TRC | SR_CRITICAL_TRC, "%s:%d Failed to Start Delay Timer\n",
                    __func__, __LINE__);
            return SR_FAILURE;
        }
        SR_TRC2 (SR_UTIL_TRC | SR_CRITICAL_TRC, "%s:%d Delay Timer Started\n",
                __func__, __LINE__);
        gSrGlobalInfo.bIsDelayTimerStarted = SR_TRUE;
        return SR_SUCCESS;
    }
    SR_TRC2 (SR_UTIL_TRC | SR_CTRL_TRC, "%s:%d Delay Timer already Running\n",
                __func__, __LINE__);
    return SR_SUCCESS;
}

/******************************************************************************
 * Function Name      : SrProcessFRRModifyRouteEvent
 * Description        : This function handles the modify FRR Event
 * Input(s)           : Route information
 * Output(s)          : NONE
 * Return Value(s)    : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/

UINT4
SrProcessFRRModifyRouteEvent (tSrIpEvtInfo * pSrIpEvtInfo)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRouteEntryInfo  *pRouteInfo = NULL;
    tSrDelayRtInfo     *pDelayRouteInfo = NULL;
    tGenU4Addr          destAddr;
    tNetIpv4LfaRtInfo   LfaRtInfo;
    UINT4               u4DestAddr = SR_ZERO;
    UINT4               u4PrefixId = SR_ZERO;
    UINT4               u4NextHop = SR_ZERO;
    MEMSET (&destAddr, SR_ZERO, sizeof (tGenU4Addr));
    if (pSrIpEvtInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                "%s:%d pSrIpEvtInfo == NULL \n",__func__, __LINE__);
        return SR_FAILURE;
    }
    pRouteInfo = (tSrRouteEntryInfo *) (VOID *) pSrIpEvtInfo->pRouteInfo;
    MEMCPY (&(destAddr.Addr.u4Addr), &(pRouteInfo->destAddr.Addr.u4Addr),
            SR_IPV4_ADDR_LENGTH);
    destAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    u4DestAddr = pRouteInfo->destAddr.Addr.u4Addr;
    SR_TRC4 (SR_UTIL_TRC, "SR-FRR Modify Route Process for prefix %x Mask %x"
                " New Next Hop %x Old Next Hop %x \r\n",
                pRouteInfo->destAddr.Addr.u4Addr, pRouteInfo->u4DestMask,
                pRouteInfo->nextHop.Addr.u4Addr, pRouteInfo->u4OldNextHop);

    /* If FRR is disabled in Segment Routing Module,
       then, consider as Primary NH change event */
    if (gu4AltModuleStatus.u4SrIPV4AltModuleStatus != ALTERNATE_ENABLED)
    {
        SR_TRC2 (SR_UTIL_TRC,
                "%s:%d FRR is disabled in the Segment Routing Module \n",
                __func__, __LINE__);

        if (SrProcessRouteNextHopChangeEvent (pSrIpEvtInfo) == SR_FAILURE)
        {
            SR_TRC2 (SR_UTIL_TRC | SR_FAIL_TRC,
                    "%s:%d SrProcessRouteNextHopChangeEvent Failed for pSrIpEvtInfo \n",
                    __func__,__LINE__);
            return SR_FAILURE;
        }
        return SR_SUCCESS;
    }

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
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
                    break;
                }
            }
            break;
        }
    }

    /* - Skipping the Primary SR-MPLS-NPAPI handling when the New Primary Next Hop
         is the same as the Old Primary Next Hop, which can occur in the event
         of an OSPF cost update scenario.
       - Deleting the FRR Entry for the Old Primary Next Hop */
    if ((pRtrInfo != NULL) &&
            (pRouteInfo->u4OldNextHop == pRouteInfo->nextHop.Addr.u4Addr))
    {
        MEMSET (&LfaRtInfo, SR_ZERO, sizeof (tNetIpv4LfaRtInfo));
        LfaRtInfo.u4DestNet = pRouteInfo->destAddr.Addr.u4Addr;
        LfaRtInfo.u4DestMask = pRouteInfo->u4DestMask;
        LfaRtInfo.u4NextHop = pRouteInfo->u4OldNextHop;
        LfaRtInfo.u4LfaNextHop =  pRtrInfo->u4LfaNextHop;
        LfaRtInfo.u1CmdType = LFA_ROUTE_DELETE;
        switch(pRtrInfo->u1LfaType)
        {
            case SR_LFA_ROUTE:
                LfaRtAddOrDeleteEventHandler (&LfaRtInfo, MPLS_MLIB_FTN_DELETE);
                break;
            case SR_RLFA_ROUTE:
                RLfaRtAddOrDeleteEventHandler (&LfaRtInfo, MPLS_MLIB_FTN_DELETE);
                break;
            case SR_TILFA_ROUTE:
                TiLfaRtAddOrDeleteEventHandler(&LfaRtInfo, MPLS_MLIB_FTN_DELETE);
                break;
            default:
                SR_TRC4 (SR_FAIL_TRC,
                    "%s:%d Not supported LFA Type %d for the route %x \n",
                    __func__, __LINE__, pRouteInfo->destAddr.Addr.u4Addr,
                    pRtrInfo->u1LfaType);
        }
        SR_TRC4 (SR_UTIL_TRC,
                "%s:%d SR-FRR Modify Route for Prefix %x is ignored since"
                " New Primary Next Hop is same as the Old Primary Next Hop %x \n",
                __func__, __LINE__, pRouteInfo->destAddr.Addr.u4Addr,
                pRouteInfo->nextHop.Addr.u4Addr);
        return SR_SUCCESS;
    }

    if (SR_TRUE == gSrGlobalInfo.bIsMicroLoopAvd)
    {
        /* allocate node to store chnage route info data*/
        pDelayRouteInfo = SrCreateDelayRouteNode ();
        if (NULL == pDelayRouteInfo)
        {
            return SR_FAILURE;
        }
        MEMCPY (&(pDelayRouteInfo->srRouteInfo), pRouteInfo, sizeof(tSrRouteEntryInfo));
        pDelayRouteInfo->u4IfIndex = pSrIpEvtInfo->u4IfIndex;
        pDelayRouteInfo->u4BitMap  = pSrIpEvtInfo->u4BitMap;
        pDelayRouteInfo->u4IpAddr.Addr.u4Addr = pSrIpEvtInfo->u4IpAddr.Addr.u4Addr;
        pDelayRouteInfo->u4IpAddr.u2AddrType  = pSrIpEvtInfo->u4IpAddr.u2AddrType;

        /* Add Change route info to delay list */
        TMO_SLL_Add(&(gSrGlobalInfo.rtDelayList), &(pDelayRouteInfo->nextNode));
        SR_TRC4 (SR_UTIL_TRC | SR_CTRL_TRC,
                "%s:%d Prefix %x from Router Node %x addeed to Delay List\n" ,
                __func__, __LINE__, u4DestAddr, pRtrInfo->advRtrId.Addr.u4Addr);

        /* start delay timer if timer is not running */
        if (SrStartRtDelayTimer() == SR_SUCCESS)
        {
            pRtrInfo->u1OprFlags |= SR_OPR_F_ADD_DELAY_LIST;
            return SR_SUCCESS;
        }
    }
    SrProcessFRRModifyRoute(pSrIpEvtInfo, pRtrInfo, pSrRtrNextHopInfo);
    return SR_SUCCESS;
}
/*****************************************************************************/
/* Function     : SrDelMplsEntries                                           */
/*                                                                           */
/* Description  : This function deletes MPLS bindings and ILMs for the       */
/*                provided routerNode                                        */
/*                                                                           */
/* Input        : pRtrInfo - Router info                                     */
/*                i1ConflictDel - Flag indicates delete triggered due to     */
/*                                SID conflict (or) prefix conflict          */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : SUCCESS/FAILURE                                            */
/*****************************************************************************/
PUBLIC VOID
SrDelMplsEntries (tSrRtrInfo * pRtrInfo, INT1 i1ConflictDel)
{
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrInSegInfo        SrInSegInfo;
    UINT4               u4SrGbMinIndex = SR_ZERO;

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC  | SR_CRITICAL_TRC,
             "%s:%d ENTRY for Router %x, AdvRtrId %x\n",
             __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
             pRtrInfo->advRtrId.Addr.u4Addr);

    MEMSET (&SrInSegInfo, SR_ZERO, sizeof (tSrInSegInfo));

    if (pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE)
    {
        u4SrGbMinIndex = gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex;
    }
    else if (pRtrInfo->u2AddrType == SR_IPV6_ADDR_TYPE)
    {
        u4SrGbMinIndex = gSrGlobalInfo.SrContextV3.SrGbRange.u4SrGbV3MinIndex;
    }
    SrInSegInfo.u4InLabel = (u4SrGbMinIndex + pRtrInfo->u4SidValue);

    if (TMO_SLL_Count (&pRtrInfo->NextHopList) != SR_ZERO)
    {
        if (i1ConflictDel == SR_SID_CONFLICT_MPLS_DEL)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                    "%s:%d Setting SIDConflictMPLSDel for Rtr %x\n",
                    __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
            pRtrInfo->u1SIDConflictMPLSDel = SR_TRUE;
        }
        if (i1ConflictDel == SR_PREFIX_CONFLICT_MPLS_DEL)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                    "%s:%d Setting PrefixConflictMPLSDel for Rtr %x\n",
                    __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
            pRtrInfo->u1PrefixConflictMPLSDel = SR_TRUE;
        }

        TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                      pSrRtrNextHopNode, tTMO_SLL_NODE *)
        {
            pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
            if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                SR_FTN_CREATED)
            {
                if (SrMplsDeleteFTN (pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
                {
                    SR_TRC2 (SR_FAIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d Unable to delete FTN\n", __func__,
                             __LINE__);
                }
                if (SrMplsDeleteILM (&SrInSegInfo, pRtrInfo, pSrRtrNextHopInfo)
                        == SR_FAILURE)
                {
                    SR_TRC2 (SR_FAIL_TRC | SR_CRITICAL_TRC,
                            "%s:%d Unable to delete ILM SWAP\n",
                            __func__, __LINE__);
                }
            }
        }
    }
    else
    {
        SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                 "%s:%d NextHop Not Exists for Rtr %x\n",
                 __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC  | SR_CRITICAL_TRC, "%s:%d EXIT \n",__func__,__LINE__);
    return;
}

/*****************************************************************************/
/* Function     : SrAddMplsEntries                                           */
/*                                                                           */
/* Description  : This function creates MPLS bindings and ILMs for the       */
/*                provided routerNode                                        */
/*                                                                           */
/* Input        : pRtrInfo - Router info                                     */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : SUCCESS/FAILURE                                            */
/*****************************************************************************/
PUBLIC UINT4
SrAddMplsEntries (tSrRtrInfo * pRtrInfo)
{
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;

    if (pRtrInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Sr Pointer for Router Info is NULL : \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC  | SR_CRITICAL_TRC,
             "%s:%d ENTRY for Router %x, AdvRtrId %x\n",
             __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
             pRtrInfo->advRtrId.Addr.u4Addr);

    if (TMO_SLL_Count (&pRtrInfo->NextHopList) != SR_ZERO)
    {
        TMO_SLL_Scan (&(pRtrInfo->NextHopList),
                      pSrRtrNextHopNode, tTMO_SLL_NODE *)
        {
            pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
            if (SrMplsCreateFTN (pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
            {
                SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                         "%s:%d Unable to create FTN\n", __func__, __LINE__);
                return SR_FAILURE;
            }

            if (SrUtilCheckNodeIdConfigured () == SR_SUCCESS)
            {
                if (SrMplsCreateILM (pRtrInfo, pSrRtrNextHopInfo) == SR_FAILURE)
                {
                    SR_TRC2 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s:%d Unable to delete ILM SWAP\n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
            }
        }

        pRtrInfo->u1SIDConflictMPLSDel = SR_FALSE;
        SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                 "%s:%d Resetting SIDConflictMPLSDel for Rtr %x\n",
                 __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
    }
    else
    {
        SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                 "%s:%d NextHop Not Exists for Rtr %x\n",
                 __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr);
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC  | SR_CRITICAL_TRC, "%s:%d EXIT \n",__func__,__LINE__);
    return SR_SUCCESS;
}

/*****************************************************************************/
/* Function     : SrChkRtrListSidConflict                                    */
/*                                                                           */
/* Description  : This function checks for SID(received) conflict with the   */
/*                existing routers in the routerList. And deletes the MPLS   */
/*                entries for the routers which are less than the receiving  */
/*                router's advertising RID.                                  */
/*                                                                           */
/* Input        : pNewRtrNode - Received LSA's router info                   */
/*                u4SidVal - Received LSA's SID value                        */
/* Output       : NONE                                                       */
/*                                                                           */
/* Returns      : NONE                                                       */
/*****************************************************************************/
PUBLIC VOID
SrChkRtrListSidConflict (tSrRtrInfo * pNewRtrNode, UINT4 u4SidVal)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    UINT4               u4AdvRtrId = SR_ZERO;
    UINT4               u4HighAdvRtrId = 0;
    INT1                i1ConflictDel = SR_ZERO;

    if (pNewRtrNode == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Sr Pointer for new Router node is NULL : \n",
                __func__, __LINE__);
        return;
    }

    SR_TRC5 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC,
             "%s:%d ENTRY for Rtr %x, AdvRtrId %x, SID %d\n",
             __func__, __LINE__, pNewRtrNode->prefixId.Addr.u4Addr,
             pNewRtrNode->advRtrId.Addr.u4Addr, u4SidVal);
    u4AdvRtrId = pNewRtrNode->advRtrId.Addr.u4Addr;

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;
        if ((u4SidVal == pRtrInfo->u4SidValue)
            && (u4AdvRtrId != pRtrInfo->advRtrId.Addr.u4Addr))
        {
            SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d u4AdvRtrId=%x, pRtrInfo->advRtrId=%x\n", __func__,
                     __LINE__, u4AdvRtrId, pRtrInfo->advRtrId.Addr.u4Addr);
            /* SID conflict - same SID already received from another peer */
            if (u4AdvRtrId > pRtrInfo->advRtrId.Addr.u4Addr)
            {
                if (pNewRtrNode->u1SIDConflictWin == SR_CONFLICT_WINNER)
                {
                    SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d Already SIDConflictWin SET for %x, AdvRtr %x\n",
                             __func__, __LINE__, pNewRtrNode->prefixId.Addr.u4Addr,
                             pNewRtrNode->advRtrId.Addr.u4Addr);
                    continue;
                }

                if (pRtrInfo->u1SIDConflictMPLSDel == SR_TRUE)
                {
                    SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d Already SIDConflictMPLSDel SET for %x, AdvRtr %x\n",
                             __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                             pRtrInfo->advRtrId.Addr.u4Addr);
                    continue;
                }

                SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d Invoking SrDelMplsEntries for %x, AdvRtr %x\n",
                         __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                         pRtrInfo->advRtrId.Addr.u4Addr);

                i1ConflictDel = SR_SID_CONFLICT_MPLS_DEL;
                SrDelMplsEntries (pRtrInfo, i1ConflictDel);

                if (pNewRtrNode->advRtrId.Addr.u4Addr > u4HighAdvRtrId)
                {
                    pNewRtrNode->u1SIDConflictWin = SR_CONFLICT_WINNER;
                    SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d Setting SIDConflictWin for Rtr %x, AdvRtr %x\n",
                             __func__, __LINE__, pNewRtrNode->prefixId.Addr.u4Addr,
                             pNewRtrNode->advRtrId.Addr.u4Addr);
                }
                pRtrInfo->u1SIDConflictWin = SR_CONFLICT_NOT_WINNER;
                SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d Resetting SIDConflictWin for Rtr %x, AdvRtr %x\n",
                         __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                         pRtrInfo->advRtrId.Addr.u4Addr);
            }
            else if (u4AdvRtrId < pRtrInfo->advRtrId.Addr.u4Addr)
            {
                SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d Resetting SIDConflictWin and Setting "
                         "SIDConflictMPLSDel for Rtr %x, AdvRtr %x\n",
                         __func__, __LINE__, pNewRtrNode->prefixId.Addr.u4Addr,
                         pNewRtrNode->advRtrId.Addr.u4Addr);

                pNewRtrNode->u1SIDConflictWin = SR_CONFLICT_NOT_WINNER;
                pNewRtrNode->u1SIDConflictMPLSDel = SR_TRUE;

                if (pRtrInfo->advRtrId.Addr.u4Addr > u4HighAdvRtrId)
                {
                    u4HighAdvRtrId = pRtrInfo->advRtrId.Addr.u4Addr;
                }
            }
        }
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC,
            "%s:%d EXIT \n", __func__, __LINE__);
    return;
}

/*****************************************************************************/
/* Function     : SrChkAndActOnSIDConflict                                   */
/*                                                                           */
/* Description  : This function checks for SID(received) conflicts with the  */
/*                local SID and existing routers in the routerList.          */
/*                If SID conflicts with local router,                        */
/*                  -Local AdvRID is higher, then delete MPLS entries for    */
/*                     receiving router.                                     */
/*                  -Local AdvRID is lower, then POP entry should be deleted */
/*                                                                           */
/* Input        : pNewRtrNode - Received LSA's router info                   */
/*                u4SidVal - Received LSA's SID value                        */
/* Output       : NONE                                                       */
/*                                                                           */
/* Returns      : NONE                                                       */
/*****************************************************************************/
PUBLIC VOID
SrChkAndActOnSIDConflict (tSrRtrInfo * pNewRtrNode, UINT4 u4SidVal,
                          UINT1 u1PeerExist)
{
    tSrSidInterfaceInfo *pSrSidEntry = NULL;
    tGenU4Addr          destPrefix;
    tGenU4Addr          nextHop;
    UINT4               u4AdvRtrId = SR_ZERO;
    UINT4               u4Label = SR_ZERO;
    UINT1               u1TmpSidConflict = SR_FALSE;
    INT1                i1ConflictDel = SR_ZERO;

    if (pNewRtrNode == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Sr Pointer for new Router node is NULL : \n",
                __func__, __LINE__);
        return;
    }

    SR_TRC5 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC,
             "%s:%d ENTRY for Rtr %x, AdvRtrId %x, SID %d\n",
             __func__, __LINE__, pNewRtrNode->prefixId.Addr.u4Addr,
             pNewRtrNode->advRtrId.Addr.u4Addr, u4SidVal);

    MEMSET (&destPrefix, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&nextHop, SR_ZERO, sizeof (tGenU4Addr));

    u4AdvRtrId = pNewRtrNode->advRtrId.Addr.u4Addr;

    if (SrUtilCheckSIDPresent (u4SidVal) == SR_SUCCESS)
    {
        SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                 "%s:%d SID Same as local node for Rtr %x\n",
                 __func__, __LINE__, pNewRtrNode->prefixId.Addr.u4Addr);
        u1TmpSidConflict = SR_TRUE;

        /* ALARM NEED TO RAISE */
        if (pNewRtrNode->u1SIDConflictAlarm != SR_ALARM_RAISE)
        {
            SrGenerateAlarm (pNewRtrNode, SR_ALARM_RAISE, SID_CONFLICT_ALARM);
        }

        /* SID conflict - same SID exists on local router
           As comparing with self Node
           - If LSA's AdvRtrId is greater than self - Allow
           - If LSA's AdvRtrId is lesser than self  - Dont allow to create FTN/ILM
         */
        if (u4AdvRtrId < gSrGlobalInfo.u4Ospfv2RtrID)
        {
            if (u1PeerExist == SR_TRUE)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d Invoking SrDelMplsEntries for Rtr %x\n",
                         __func__, __LINE__, pNewRtrNode->prefixId.Addr.u4Addr);

                i1ConflictDel = SR_SID_CONFLICT_MPLS_DEL;
                SrDelMplsEntries (pNewRtrNode, i1ConflictDel);
            }
            pNewRtrNode->u1SIDConflictWin = SR_CONFLICT_NOT_WINNER;
            SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d Resetting SIDConflictWin for Rtr %x\n",
                     __func__, __LINE__, pNewRtrNode->prefixId.Addr.u4Addr);
        }
        else
        {
            pSrSidEntry = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);
            while (pSrSidEntry != NULL)
            {
                if (pSrSidEntry->u4SidType == SR_SID_ADJACENCY)
                {
                    pSrSidEntry =
                        RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry,
                                       NULL);
                    continue;
                }

                SR_TRC5 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d SidEntry_SID=%d, RcvdSID=%d, SIDConflictPopDel=%d\n",
                         __func__, __LINE__, pSrSidEntry->u4PrefixSidLabelIndex,
                         u4SidVal, pSrSidEntry->u1SidConflictPopDel);

                if ((pSrSidEntry->u4PrefixSidLabelIndex == u4SidVal)
                    && (pSrSidEntry->u1SidConflictPopDel == SR_FALSE))
                {
                    nextHop.u2AddrType = pSrSidEntry->ifIpAddr.u2AddrType;

                    SR_GET_SELF_SID_LABEL (&u4Label, pSrSidEntry);

                    SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d Invoking SrUtilCreateorDeleteILM for ILM_POP_DEL %d\n",
                             __func__, __LINE__, u4Label);
                    if (SrUtilCreateorDeleteILM
                        (pSrSidEntry, u4Label, &destPrefix, &nextHop,
                         MPLS_MLIB_ILM_DELETE, SR_ZERO) == SR_FAILURE)
                    {
                        SR_TRC (SR_UTIL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                                "% Failure: MplsCliStaticXC Failed\n");
                        return;
                    }
                    pSrSidEntry->u1SidConflictPopDel = SR_TRUE;
                    SR_TRC2 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d Setting SIDConflictPopDel\n",
                             __func__, __LINE__);
                }
                pSrSidEntry =
                    RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry,
                                   NULL);
            }

            SrChkRtrListSidConflict (pNewRtrNode, u4SidVal);
        }
    }
    else
    {
        SrChkRtrListSidConflict (pNewRtrNode, u4SidVal);
    }

    if (u1TmpSidConflict == SR_FALSE)
    {
        if (pNewRtrNode->u1SIDConflictAlarm == SR_ALARM_RAISE)
        {
            SrGenerateAlarm (pNewRtrNode, SR_ALARM_CLEAR, SID_CONFLICT_ALARM);
        }
    }

    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC,
             "%s:%d EXIT \n", __func__, __LINE__);
    return;
}

/*****************************************************************************/
/* Function     : SrSidChkAndCreateEntriesForNxtRtr                             */
/*                                                                           */
/* Description  : This function checks for next conflict winner router for   */
/*                creating MPLS entries and checks for creation of POP entry */
/*                on local node.                                             */
/*                                                                           */
/* Input        : u4DelRtrSidVal - Deleted router's SID value                */
/*                u4DelAdvRtrId - Deleted router's AdvRtr ID                 */
/*                                                                           */
/* Output       : NONE                                                       */
/*                                                                           */
/* Returns      : NONE                                                       */
/*****************************************************************************/
PUBLIC VOID
SrSidChkAndCreateEntriesForNxtRtr (UINT4 u4DelRtrSidVal, UINT4 u4DelAdvRtrId)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tSrRtrInfo         *pTmpRtrInfo = NULL;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;
    tGenU4Addr          destPrefix;
    tGenU4Addr          nextHop;
    tGenU4Addr          tmpAddr;
    UINT4               u4HighAdvRtrId = SR_ZERO;
    UINT4               u4Label = SR_ZERO;
    UINT4               u4ConflictPeerCnt = SR_ZERO;
    UINT4               u4RtrIdCreate = SR_ZERO;
    UINT4               u4AdvRtrIdCreate = SR_ZERO;
    BOOL1               bFound = SR_FALSE;

    MEMSET (&destPrefix, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&nextHop, SR_ZERO, sizeof (tGenU4Addr));
    MEMSET (&tmpAddr, 0, sizeof (tGenU4Addr));

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC  | SR_CRITICAL_TRC,
             "%s:%d ENTRY for SID %d AdvRtr %x\n",
             __func__, __LINE__, u4DelRtrSidVal, u4DelAdvRtrId);
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pTmpRtrInfo = (tSrRtrInfo *) pRtrNode;

        if ((pTmpRtrInfo->u4SidValue == u4DelRtrSidVal)
            && (pTmpRtrInfo->advRtrId.Addr.u4Addr != u4DelAdvRtrId))
        {
            SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d Nxt_Rtr %x SID=%d\n",
                     __func__, __LINE__, pTmpRtrInfo->advRtrId.Addr.u4Addr,
                     u4DelRtrSidVal);

            u4ConflictPeerCnt++;
            if (u4DelAdvRtrId > pTmpRtrInfo->advRtrId.Addr.u4Addr)
            {
                if (u4HighAdvRtrId != 0)
                {
                    continue;
                }

                bFound = SR_TRUE;
                if (pTmpRtrInfo->advRtrId.Addr.u4Addr > u4RtrIdCreate)
                {
                    u4RtrIdCreate = pTmpRtrInfo->prefixId.Addr.u4Addr;
                    u4AdvRtrIdCreate = pTmpRtrInfo->advRtrId.Addr.u4Addr;
                    SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                             "%s:%d u4RtrIdCreate=%x, u4AdvRtrIdCreate=%x\n",
                             __func__, __LINE__, u4RtrIdCreate, u4AdvRtrIdCreate);
                }
            }
            else
            {
                if (pTmpRtrInfo->advRtrId.Addr.u4Addr > u4HighAdvRtrId)
                {
                    u4HighAdvRtrId = pTmpRtrInfo->advRtrId.Addr.u4Addr;
                    u4RtrIdCreate = pTmpRtrInfo->prefixId.Addr.u4Addr;
                    u4AdvRtrIdCreate = u4HighAdvRtrId;
                }
            }
        }
    }

    if (u4ConflictPeerCnt != 0)
    {
        tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
        tmpAddr.Addr.u4Addr = u4RtrIdCreate;

        pTmpRtrInfo =
            SrGetSrRtrInfoFromRtrAndAdvRtrId (&tmpAddr, u4AdvRtrIdCreate);
    }

    pSrSidEntry = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);
    while (pSrSidEntry != NULL)
    {
        if (pSrSidEntry->u4SidType == SR_SID_ADJACENCY)
        {
            pSrSidEntry =
                RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry, NULL);
            continue;
        }
        if (pSrSidEntry->u1SidConflictPopDel == SR_FALSE)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d Skip as SidConflictPopDel FALSE for %d\n",
                     __func__, __LINE__, pSrSidEntry->u4PrefixSidLabelIndex);
            pSrSidEntry =
                RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry, NULL);
            continue;
        }

        SR_TRC5 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                 "%s:%d u1SidConflictPopDel=%d u4DelAdvRtrId=%x, u4RtrIdCreate=%x\n",
                 __func__, __LINE__, pSrSidEntry->u1SidConflictPopDel,
                 u4DelAdvRtrId, u4RtrIdCreate);
        SR_GET_SELF_SID_INDEX_VAL (&u4Label, pSrSidEntry);

        if ((pSrSidEntry->u1SidConflictPopDel == SR_TRUE)
            && ((u4DelAdvRtrId > gSrGlobalInfo.u4Ospfv2RtrID)
                && (u4Label == u4DelRtrSidVal))
            && ((u4ConflictPeerCnt == 0)
                || ((pTmpRtrInfo != NULL) &&
                    (gSrGlobalInfo.u4Ospfv2RtrID >
                     pTmpRtrInfo->advRtrId.Addr.u4Addr))))
        {
            nextHop.u2AddrType = pSrSidEntry->ifIpAddr.u2AddrType;
            SR_GET_SELF_SID_LABEL (&u4Label, pSrSidEntry);

            SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d Invoking SrUtilCreateorDeleteILM for ILM_POP_CREAT %d\n",
                     __func__, __LINE__, u4Label);

            gSrGlobalInfo.u4CliCmdMode = SR_TRUE;
            if (SrUtilCreateorDeleteILM (pSrSidEntry, u4Label, &destPrefix,
                                         &nextHop, MPLS_MLIB_ILM_CREATE,
                                         SR_ZERO) == SR_FAILURE)
            {
                SR_TRC (SR_FAIL_TRC | SR_CRITICAL_TRC,
                        "% Failure: SrUtilCreateorDeleteILM Failed\n");
                gSrGlobalInfo.u4CliCmdMode = SR_FALSE;
                return;
            }
            gSrGlobalInfo.u4CliCmdMode = SR_FALSE;

            pSrSidEntry->u1SidConflictPopDel = SR_FALSE;
            SR_TRC2 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d Resetting SIDConflictPopDel\n", __func__, __LINE__);
        }
        pSrSidEntry =
            RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry, NULL);
    }

    if ((bFound == SR_TRUE) && (pTmpRtrInfo != NULL))
    {
        SR_TRC5 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                 "%s:%d AdvRtr %x, SIDConflictMplsDel=%d, SID=%d\n",
                 __func__, __LINE__, pTmpRtrInfo->advRtrId.Addr.u4Addr,
                 pTmpRtrInfo->u1SIDConflictMPLSDel, pTmpRtrInfo->u4SidValue);

        if ((pTmpRtrInfo->u1SIDConflictMPLSDel == SR_TRUE)
            && ((SrUtilCheckSIDPresent (pTmpRtrInfo->u4SidValue) == SR_FAILURE)
                || (pTmpRtrInfo->advRtrId.Addr.u4Addr >
                    gSrGlobalInfo.u4Ospfv2RtrID)))
        {
            SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d Setting SIDConflictWin for Rtr %x\n",
                     __func__, __LINE__, pTmpRtrInfo->prefixId.Addr.u4Addr);

            pTmpRtrInfo->u1SIDConflictWin = SR_CONFLICT_WINNER;
            SrAddMplsEntries (pTmpRtrInfo);
        }
    }

    return;
}

/*****************************************************************************/
/* Function     : SrPrefixChkAndCreateEntriesForNxtRtr                       */
/*                                                                           */
/* Description  : This function checks whether this is the only one router   */
/*                which was prefix conflicting with the flush received       */
/*                router prefix.                                             */
/*                If Yes, it will create MPLS entries for the same           */
/*                                                                           */
/* Input        : u4DelRtrPrefix - Deleted router's Prefix value             */
/*                u4DelAdvRtrId - Deleted router's AdvRtr ID                 */
/*                                                                           */
/* Output       : NONE                                                       */
/*                                                                           */
/* Returns      : NONE                                                       */
/*****************************************************************************/
PUBLIC VOID
SrPrefixChkAndCreateEntriesForNxtRtr (UINT4 u4DelRtrPrefix, UINT4 u4DelAdvRtrId)
{
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tSrRtrInfo         *pTmpRtrInfo = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    UINT4               u4PrefixConflictCnt = 0;

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC  | SR_CRITICAL_TRC,
             "%s:%d ENTRY for Prefix %x AdvRtr %x\n",
             __func__, __LINE__, u4DelRtrPrefix, u4DelAdvRtrId);

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pTmpRtrInfo = (tSrRtrInfo *) pRtrNode;
        if ((pTmpRtrInfo->prefixId.Addr.u4Addr == u4DelRtrPrefix)
            && (pTmpRtrInfo->advRtrId.Addr.u4Addr != u4DelAdvRtrId))
        {
            u4PrefixConflictCnt++;
            pRtrInfo = pTmpRtrInfo;
        }
    }

    if (u4PrefixConflictCnt == 1)
    {
        if (SrUtilCheckNodeIdPresent (u4DelRtrPrefix) == SR_FAILURE)
        {
            SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                    "%s:%d Resetting PrefixConflictMPLSDel for %x"
                    " and invoking SrAddMplsEntries\n",
                    __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr);

            pRtrInfo->u1PrefixConflictMPLSDel = SR_FALSE;
            SrAddMplsEntries (pRtrInfo);
        }
    }

    return;
}

/*****************************************************************************/
/* Function     : SrChkRtrListPrefixConflict                                 */
/*                                                                           */
/* Description  : This function checks for Prefix(rcvd) conflict with the    */
/*                existing routers in the routerList. And deletes the MPLS   */
/*                entries for the routers which are in conflict.             */
/*                                                                           */
/* Input        : pNewRtrNode - Received LSA's router info                   */
/*                u4PrefixAddr - Received LSA's prefix address               */
/* Output       : NONE                                                       */
/*                                                                           */
/* Returns      : NONE                                                       */
/*****************************************************************************/
PUBLIC VOID
SrChkRtrListPrefixConflict (tSrRtrInfo * pNewRtrNode, UINT4 u4PrefixAddr)
{
    tTMO_SLL_NODE     *pRtrNode = NULL;
    tSrRtrInfo        *pRtrInfo = NULL;
    UINT4              u4AdvRtrId = SR_ZERO;
    INT1               i1ConflictDel = SR_ZERO;

    if (pNewRtrNode == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Sr Pointer for new Router node is NULL : \n",
                __func__, __LINE__);
        return;
    }

    u4AdvRtrId = pNewRtrNode->advRtrId.Addr.u4Addr;
    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC,
             "%s:%d ENTRY for Rtr %x, AdvRtrId %x\n", __func__, __LINE__,
             pNewRtrNode->prefixId.Addr.u4Addr, u4AdvRtrId);

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;

        if ((u4PrefixAddr == pRtrInfo->prefixId.Addr.u4Addr)
                && (u4AdvRtrId != pRtrInfo->advRtrId.Addr.u4Addr))
        {
           /* PREFIX conflict - same prefix already received from another peer */
            SR_TRC4 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d u4AdvRtrId=%x, pRtrInfo->advRtrId=%x\n", __func__, __LINE__,
                     u4AdvRtrId,pRtrInfo->advRtrId.Addr.u4Addr);

            if (pRtrInfo->u1PrefixConflictMPLSDel == SR_TRUE)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                         "%s:%d Already PrefixConflictMPLSDel SET for %x\n",
                         __func__, __LINE__,
                         pRtrInfo->advRtrId.Addr.u4Addr);
                continue;
            }

            SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d Setting PrefixConflictMPLSDel and "
                     "Invoking SrDelMplsEntries for %x\n",
                     __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr);

            i1ConflictDel = SR_PREFIX_CONFLICT_MPLS_DEL;
            SrDelMplsEntries (pRtrInfo, i1ConflictDel);

            pNewRtrNode->u1PrefixConflictMPLSDel = SR_TRUE;
            SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                     "%s:%d Setting PrefixConflictMplsDel for %x\n",
                     __func__, __LINE__, pNewRtrNode->advRtrId.Addr.u4Addr);
        }
   }

   SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC,
            "%s:%d EXIT \n", __func__, __LINE__);
   return;
}

/*****************************************************************************/
/* Function     : SrChkAndActOnPrefixConflict                                */
/*                                                                           */
/* Description  : This function checks for Prefix conflicts with the         */
/*                local prefix and existing routers in the routerList.       */
/*                                                                           */
/* Input        : pNewRtrNode - Received LSA's router info                   */
/*                u4Prefix - Received LSA's prefix value                     */
/* Output       : NONE                                                       */
/*                                                                           */
/* Returns      : NONE                                                       */
/*****************************************************************************/
PUBLIC VOID
SrChkAndActOnPrefixConflict (tSrRtrInfo * pNewRtrNode, UINT4 u4Prefix)
{
    UINT1               u1TmpPrefixConflict = SR_FALSE;

    if (pNewRtrNode == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Sr Pointer for new Router node is NULL : \n",
                __func__, __LINE__);
        return;
    }

    SR_TRC4 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC,
             "%s:%d ENTRY for Rtr %x, AdvRtrId %x\n", __func__, __LINE__,
             pNewRtrNode->prefixId.Addr.u4Addr, pNewRtrNode->advRtrId.Addr.u4Addr);

    if (SrUtilCheckNodeIdPresent (u4Prefix) == SR_SUCCESS)
    {
        SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                 "%s:%d Prefix Same as local node for Rtr %x\n",
                 __func__, __LINE__, pNewRtrNode->prefixId.Addr.u4Addr);

        if (pNewRtrNode->u1PrefixConflictAlarm != SR_ALARM_RAISE)
        {
            SrGenerateAlarm (pNewRtrNode, SR_ALARM_RAISE, PREFIX_CONFLICT_ALARM);
        }
        u1TmpPrefixConflict = SR_TRUE;
        pNewRtrNode->u1PrefixConflictMPLSDel = SR_TRUE;
        SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                 "%s:%d Setting PrefixConflictMPLSDel for %x\n",
                 __func__, __LINE__, pNewRtrNode->advRtrId.Addr.u4Addr);

        SrChkRtrListPrefixConflict (pNewRtrNode, u4Prefix);
    }
    else
    {
        SrChkRtrListPrefixConflict (pNewRtrNode, u4Prefix);
    }

    if (u1TmpPrefixConflict == SR_FALSE)
    {
        if (pNewRtrNode->u1PrefixConflictAlarm == SR_ALARM_RAISE)
        {
            SrGenerateAlarm (pNewRtrNode, SR_ALARM_CLEAR, PREFIX_CONFLICT_ALARM);
        }
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC,
             "%s:%d EXIT \n", __func__, __LINE__);
    return;
}

/*****************************************************************************/
/* Function     : SrChkForOORAlarm                                           */
/*                                                                           */
/* Description  : This function checks whether the received SID is in Out of */
/*                range (OOR) to raise/clear OOR Alarm.                      */
/*                                                                           */
/* Input        : pNewRtrNode - Received LSA's router info                   */
/*                u4SidVal - Received LSA's SID value                        */
/*                                                                           */
/* Output       : NONE                                                       */
/*                                                                           */
/* Returns      : NONE                                                       */
/*****************************************************************************/
PUBLIC VOID
SrChkForOORAlarm (tSrRtrInfo *pRtrInfo, UINT4 u4SidVal)
{
    if ((gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex != 0)
            && (gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex != 0))
    {
        if (u4SidVal > (gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMaxIndex
                    - gSrGlobalInfo.SrContext.SrGbRange.u4SrGbMinIndex))
        {
            if (pRtrInfo->u1OutOfRangeAlarm != SR_ALARM_RAISE)
            {
                SrGenerateAlarm (pRtrInfo, SR_ALARM_RAISE, OOR_SID_ALARM);
            }
        }
        else
        {
            if (pRtrInfo->u1OutOfRangeAlarm == SR_ALARM_RAISE)
            {
                SrGenerateAlarm (pRtrInfo, SR_ALARM_CLEAR, OOR_SID_ALARM);
            }
        }
    }
    return;
}

/*****************************************************************************/
/* Function     : SrGenerateAlarm                                            */
/*                                                                           */
/* Description  : This function generates raise/clear alarm for the given    */
/*                alarm type.                                                */
/*                                                                           */
/* Input        : pRtrInfo - Received LSA's router info                      */
/*                u1GenAlarm - Alarm_raise/Alarm_clear                       */
/*                u1Type - Alarm Type                                        */
/*                                                                           */
/* Output       : NONE                                                       */
/*                                                                           */
/* Returns      : NONE                                                       */
/*****************************************************************************/
PUBLIC VOID
SrGenerateAlarm (tSrRtrInfo *pRtrInfo, UINT1 u1GenAlarm, UINT1 u1Type)
{
    if (pRtrInfo == NULL)
    {
        SR_TRC2 (SR_UTIL_TRC, "%s:%d Sr Received LSA Router Info is NULL : \n",
                __func__, __LINE__);
        return;
    }
    SR_TRC2 (SR_UTIL_TRC | SR_FN_ENTRY_EXIT_TRC  | SR_CRITICAL_TRC,
             "%s:%d ENTRY \n", __func__, __LINE__);

    switch (u1Type)
    {
        case PREFIX_CONFLICT_ALARM:
            if (u1GenAlarm == SR_ALARM_RAISE)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                        "%s:%d RAISE ALARM FOR PREFIX CONFLICT with %x\n",
                        __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr);
#ifdef TEJAS_WANTED
                gSrSidAlarm(pRtrInfo->u2AddrType,pRtrInfo->prefixId.Addr.u4Addr,pRtrInfo->u4SidValue,pRtrInfo->advRtrId.Addr.u4Addr,PREFIX_CONFLICT_ALARM,SR_ALARM_RAISE);
#endif
            }
            if (u1GenAlarm == SR_ALARM_CLEAR)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                        "%s:%d CLEAR ALARM FOR PREFIX CONFLICT with %x\n",
                        __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr);
#ifdef TEJAS_WANTED
                gSrSidAlarm(pRtrInfo->u2AddrType,pRtrInfo->prefixId.Addr.u4Addr,pRtrInfo->u4SidValue,pRtrInfo->advRtrId.Addr.u4Addr,PREFIX_CONFLICT_ALARM,SR_ALARM_CLEAR);
#endif
            }

            pRtrInfo->u1PrefixConflictAlarm = u1GenAlarm;

            break;

        case SID_CONFLICT_ALARM:
            if (u1GenAlarm == SR_ALARM_RAISE)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                        "%s:%d RAISE ALARM FOR SID CONFLICT with %x\n",
                        __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr);
                      SR_TRC1 (SR_UTIL_TRC,
                                     "Addr Type: %d",
                                     pRtrInfo->u2AddrType);
#ifdef TEJAS_WANTED
                gSrSidAlarm(pRtrInfo->u2AddrType,pRtrInfo->prefixId.Addr.u4Addr,pRtrInfo->u4SidValue,pRtrInfo->advRtrId.Addr.u4Addr,SID_CONFLICT_ALARM,SR_ALARM_RAISE);
#endif
            }
            if (u1GenAlarm == SR_ALARM_CLEAR)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                        "%s:%d CLEAR ALARM FOR SID CONFLICT with %x\n",
                        __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr);
#ifdef TEJAS_WANTED
                gSrSidAlarm(pRtrInfo->u2AddrType,pRtrInfo->prefixId.Addr.u4Addr,pRtrInfo->u4SidValue,pRtrInfo->advRtrId.Addr.u4Addr,SID_CONFLICT_ALARM,SR_ALARM_CLEAR);
#endif
            }

            pRtrInfo->u1SIDConflictAlarm = u1GenAlarm;

            break;

        case OOR_SID_ALARM:
            if (u1GenAlarm == SR_ALARM_RAISE)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                        "%s:%d RAISE ALARM FOR OOR SID with %x\n",
                        __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr);
#ifdef TEJAS_WANTED
                gSrAddressFamilyAlarm(pRtrInfo->u2AddrType,pRtrInfo->u4SidValue,pRtrInfo->advRtrId.Addr.u4Addr,OOR_SID_ALARM,SR_ALARM_RAISE);
#endif
            }
            if (u1GenAlarm == SR_ALARM_CLEAR)
            {
                SR_TRC3 (SR_UTIL_TRC | SR_CRITICAL_TRC,
                        "%s:%d CLEAR ALARM FOR OOR SID with %x\n",
                        __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr);
#ifdef TEJAS_WANTED
                gSrAddressFamilyAlarm(pRtrInfo->u2AddrType,pRtrInfo->u4SidValue,pRtrInfo->advRtrId.Addr.u4Addr,OOR_SID_ALARM,SR_ALARM_CLEAR);
#endif
            }

            pRtrInfo->u1OutOfRangeAlarm = u1GenAlarm;

            break;

        default:
            break;
    }

    return;
}

/*****************************************************************************/
/* Function     : SrUtilCheckNodeIdPresent                                   */
/*                                                                           */
/* Description  : This function checks whether the given prefix is in        */
/*                conflict with local node prefixes.                         */
/*                                                                           */
/* Input        : u4Addr - prefix address                                    */
/*                                                                           */
/* Output       : NONE                                                       */
/*                                                                           */
/* Returns      : SR_SUCCESS/SR_FAILURE                                      */
/*****************************************************************************/
PUBLIC UINT4
SrUtilCheckNodeIdPresent (UINT4 u4Addr)
{
    tSrSidInterfaceInfo srSidInfo;
    tSrSidInterfaceInfo *pSrSidInfo = NULL;

    MEMSET (&srSidInfo, SR_ZERO, sizeof (tSrSidInterfaceInfo));

    if (SR_ZERO == u4Addr)
    {
        return SR_FAILURE;
    }

    srSidInfo.ifIpAddr.Addr.u4Addr = u4Addr;
    srSidInfo.ipAddrType = SR_IPV4_ADDR_TYPE;

    pSrSidInfo = RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &srSidInfo);
    if (pSrSidInfo != NULL)
    {
        return SR_SUCCESS;
    }
    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
    "%s:%d Pointer to SID structure is NULL \n",__func__, __LINE__);

    return SR_FAILURE;
}

/*****************************************************************************/
/* Function     : SrUtilCheckSIDPresent                                      */
/*                                                                           */
/* Description  : This function checks whether the given SID is in conflict  */
/*                with local node SIDs.                                      */
/*                                                                           */
/* Input        : u4SidVal - SID value                                       */
/*                                                                           */
/* Output       : NONE                                                       */
/*                                                                           */
/* Returns      : SR_SUCCESS/SR_FAILURE                                      */
/*****************************************************************************/
PUBLIC UINT4
SrUtilCheckSIDPresent (UINT4 u4SidVal)
{
    tSrSidInterfaceInfo   *pSrSidIntf = NULL;

    pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    while (pSrSidIntf != NULL)
    {
        if (pSrSidIntf->u4PrefixSidLabelIndex == u4SidVal)
        {
            return SR_SUCCESS;
        }

        pSrSidIntf =
                RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidIntf, NULL);
    }
    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
    "%s:%d Pointer to SID structure is NULL \n",__func__, __LINE__);

    return SR_FAILURE;
}

/************************************************************************
 *  Function Name   : SrIsLoopbackUsed
 *  Description     : API to check whether the loopback is in use by SR or not.
 *  Input           : Loopback IP address
 *  Output          : NONE
 *  Returns         : OSIX_SUCCESS/OSIX_FAILURE
 ************************************************************************/
PUBLIC UINT4
SrIsLoopbackUsed (UINT4 u4IpAddr)
{
    tSrSidInterfaceInfo *pSrSidEntry = NULL;
    pSrSidEntry = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    while (pSrSidEntry != NULL)
    {
        if (u4IpAddr == pSrSidEntry->ifIpAddr.Addr.u4Addr)
        {
            return OSIX_SUCCESS;
        }

        pSrSidEntry =
            RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry, NULL);
    }
    return OSIX_FAILURE;
}

/************************************************************************
 *  Function Name   : SrUtilCheckNbrIsMultiSid
 *  Description     : API to check whether any other prefixID exists
 *                    with same advertising router Id.
 *  Input           : PrefixId IP address
 *                    Advertising router Id
 *  Output          : NONE
 *  Returns         : SR_SUCCESS/SR_FAILURE
 ************************************************************************/
PUBLIC UINT4
SrUtilCheckNbrIsMultiSid (tGenU4Addr *nbrPrefixId, UINT4 u4AdvRtrId)
{
    tTMO_SLL_NODE    *pRtrNode = NULL;
    tSrRtrInfo       *pRtrInfo = NULL;

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;
        if (nbrPrefixId->u2AddrType == SR_IPV4_ADDR_TYPE)
        {
            if (pRtrInfo->advRtrId.Addr.u4Addr == u4AdvRtrId)
            {
               if ((pRtrInfo->prefixId.Addr.u4Addr
                       != nbrPrefixId->Addr.u4Addr)
                   && (pRtrInfo->u1RtrType == OSPF_INTRA_AREA))
               {
                   return SR_SUCCESS;
               }
            }
        }
    }
    return SR_FAILURE;
}

/************************************************************************
 *  Function Name   : SrUtilCheckNodeIdConfigured
 *  Description     : API to check whether the Node SID is configured or not.
 *  Input           : None
 *  Output          : None
 *  Returns         : SR_SUCCESS/SR_FAILURE
 ************************************************************************/
PUBLIC UINT4
SrUtilCheckNodeIdConfigured ()
{
    tSrSidInterfaceInfo   *pSrSidIntf = NULL;

    pSrSidIntf = RBTreeGetFirst(gSrGlobalInfo.pSrSidRbTree);

    while(pSrSidIntf != NULL)
    {
        if(pSrSidIntf->u4SidType == SR_SID_NODE)
        {
            return SR_SUCCESS;
        }
        pSrSidIntf = RBTreeGetNext(gSrGlobalInfo.pSrSidRbTree,pSrSidIntf,NULL);
    }

    SR_TRC2 (SR_FAIL_TRC | SR_UTIL_TRC | SR_CTRL_TRC, "%s:%d : RBTreeGetFirst Failed \n",
            __func__, __LINE__);
    return SR_FAILURE;
}

PUBLIC UINT4
SrUtilSendLSAForSid (UINT1 u1LsaStatus, UINT4 lsaTLVReceived, UINT1 u1lsaType,
                     tSrSidInterfaceInfo *pSrSidIntf)
{
    tOpqLSAInfo         opqLSAInfo;
    tOpqLSAInfo         opqLSAInfo1;
    tOpqLSAInfo         opqLSAInfo2;
    UINT1               au1RtrAddrTlv[SR_RI_LSA_TLV_LEN];
    UINT1               au1RtrAddrTlv1[SR_RI_LSA_TLV_LEN];

    MEMSET (&opqLSAInfo, 0, sizeof (tOpqLSAInfo));
    MEMSET (&opqLSAInfo1, 0, sizeof (tOpqLSAInfo));
    MEMSET (&opqLSAInfo2, 0, sizeof (tOpqLSAInfo));
    MEMSET (au1RtrAddrTlv, 0, sizeof (SR_RI_LSA_TLV_LEN));
    opqLSAInfo.pu1LSA = au1RtrAddrTlv;
    opqLSAInfo1.pu1LSA = au1RtrAddrTlv1;
    opqLSAInfo2.pu1LSA = au1RtrAddrTlv1;

    if (u1LsaStatus == FLUSHED_LSA)
    {
        if (pSrSidIntf != NULL)
        {
            if (pSrSidIntf->u4SidType != SR_SID_NODE)
            {
                SR_TRC2 (SR_FAIL_TRC | SR_UTIL_TRC | SR_CTRL_TRC, "%s:%d : pSrSidIntf->u4SidType is not equal to SR_SID_NONE \n",
                        __func__, __LINE__);
                return SR_FAILURE;
            }

            /*if ((lsaTLVReceived == RI_LSA_OPAQUE_TYPE)
                || (lsaTLVReceived == SR_ZERO))
            {
                if (SrConstructRtrInfoLsa (&opqLSAInfo, u1LsaStatus, u1lsaType) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_FAIL_TRC | SR_UTIL_TRC | SR_CTRL_TRC, "%s:%d : SrConstructRtrInfoLsa Failed\r\n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }

                if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA,
                                    OSPF_DEFAULT_CXT_ID, &opqLSAInfo) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_FAIL_TRC | SR_UTIL_TRC | SR_CTRL_TRC, "%s:%d : RtrInfoLsa Sent Failed\r\n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
            }*/

            /*Send own NODE SID */
            if (lsaTLVReceived == OSPF_EXT_PRREFIX_TLV_TYPE ||
                    lsaTLVReceived == SR_ZERO)
            {
                if (SrConstructExtPrefixOpqLsa
                        (&opqLSAInfo1, u1LsaStatus, u1lsaType, pSrSidIntf) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_FAIL_TRC | SR_UTIL_TRC | SR_CTRL_TRC,
                            "%s:%d : SrConstructExtPrefixOpqLsa Failed \n", __func__,
                            __LINE__);
                    return SR_FAILURE;
                }
                if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA,
                            OSPF_DEFAULT_CXT_ID,
                            &opqLSAInfo1) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_FAIL_TRC | SR_UTIL_TRC | SR_CTRL_TRC, "%s:%d : ExtPrefixOpqLsa Sent Failed \n",
                            __func__, __LINE__);
                    return SR_FAILURE;
                }
            }
        }
        if (lsaTLVReceived == OSPF_EXT_LINK_LSA_OPAQUE_TYPE || lsaTLVReceived == SR_ZERO)
        {
            SrHandleEventforAllExtLinkLsa (u1LsaStatus);
        }
        return SR_SUCCESS;
    }

    if ((pSrSidIntf != NULL) &&
            (pSrSidIntf->u4SidType == SR_SID_NODE))
    {
        /*if (lsaTLVReceived == RI_LSA_OPAQUE_TYPE || lsaTLVReceived == SR_ZERO)
        {
            if (SrConstructRtrInfoLsa (&opqLSAInfo, u1LsaStatus, u1lsaType) !=
                   SR_SUCCESS)
            {
                SR_TRC2 (SR_FAIL_TRC | SR_UTIL_TRC | SR_CTRL_TRC, "%s:%d : SrConstructRtrInfoLsa Failed \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }

            if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA,
                                OSPF_DEFAULT_CXT_ID, &opqLSAInfo) != SR_SUCCESS)
            {
                SR_TRC2 (SR_FAIL_TRC | SR_UTIL_TRC | SR_CTRL_TRC, "%s:%d : RtrInfoLsa Sent Failed \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
        }*/

        /*Send own NODE SID */
        if (lsaTLVReceived == OSPF_EXT_PRREFIX_TLV_TYPE ||
               lsaTLVReceived == SR_ZERO)
        {
            if (SrConstructExtPrefixOpqLsa
                (&opqLSAInfo1, u1LsaStatus, u1lsaType, pSrSidIntf) != SR_SUCCESS)
            {
                SR_TRC2 (SR_FAIL_TRC | SR_UTIL_TRC | SR_CTRL_TRC,
                         "%s:%d : SrConstructExtPrefixOpqLsa Failed \n", __func__,
                         __LINE__);
                return SR_FAILURE;
            }
            if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA,
                                OSPF_DEFAULT_CXT_ID,
                                &opqLSAInfo1) != SR_SUCCESS)
            {
                SR_TRC2 (SR_FAIL_TRC | SR_UTIL_TRC | SR_CTRL_TRC, "%s:%d : ExtPrefixOpqLsa Sent Failed \n",
                         __func__, __LINE__);
                return SR_FAILURE;
            }
        }
    }

    if (lsaTLVReceived == OSPF_EXT_LINK_LSA_OPAQUE_TYPE || lsaTLVReceived == SR_ZERO)
    {
        SrHandleEventforAllExtLinkLsa (u1LsaStatus);
    }

    return SR_SUCCESS;
}

/*****************************************************************************/
/* Function Name : SrUtlCheckTnlInTnlTable                                   */
/* Description   : This routine Check the presence of a SR Tunnel Index      */
/*                 info in the RB Tree.                                      */
/* Input(s)      : gSrGlobalInfo  - Pointer to the SR Global information.    */
/*                 dbOperation    -  Operation Type(FTN/ILM)                 */
/*                 u4Prefix       - Prefix Ip                                */
/*                 u4NextHop      - Next Hop Ip                              */
/* Output(s)     : pSrTnlInfo  - Pointer to pointer of the tunnel info       */
/*                 whose index matches u4TunnelIndex.                        */
/* Return(s)     : SR_SUCCESS in case of tunnel being present, otherwise     */
/*                 SR_FAILURE                                                */
/*****************************************************************************/
UINT1
SrUtlCheckTnlInTnlDbTable (UINT1 dbOperation, UINT4 u4Prefix, UINT4 u4NextHop, tSrTnlIfTable** pSrTnlInfo)
{
    tSrTnlIfTable srTnlTable;

    MEMSET (&srTnlTable, SR_ZERO, sizeof (tSrTnlIfTable));

    SR_TRC3 (SR_MAIN_TRC, "SrUtlCheckTnlInTnlDbTable: dbOperation: %d u4Prefix: %x u4NextHop: %x\n", dbOperation, u4Prefix, u4NextHop);

    srTnlTable.u4Prefix = u4Prefix;
    srTnlTable.u4NextHop = u4NextHop;

    if (dbOperation == SR_FTN_TNL_INFO_TABLE)
    {
        *pSrTnlInfo =
                      (tSrTnlIfTable *) RBTreeGet (gSrGlobalInfo.SrFTNTnlDbTree,
                      (tRBElem *) &srTnlTable);
    }
    else if (dbOperation == SR_ILM_TNL_INFO_TABLE)
    {
        *pSrTnlInfo =
                      (tSrTnlIfTable *) RBTreeGet (gSrGlobalInfo.SrILMTnlDbTree,
                      (tRBElem *) &srTnlTable);
    }

    if (*pSrTnlInfo != NULL)
    {
        SR_TRC (SR_CRITICAL_TRC, "SrUtlCheckTnlInTnlDbTable : EXIT \n");
        return SR_SUCCESS;
    }

    SR_TRC (SR_FAIL_TRC, "SrUtlCheckTnlInTnlDbTable Entry Not Found: INTMD-EXIT \n");
    return SR_FAILURE;
}

#endif

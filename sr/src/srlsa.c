/********************************************************************
 * Copyright (C) 2007 Aricent Inc . All Rights Reserved
 *
 * $Id$
 *
 * Description: This file contains utility routines for SR LSA
 *
 *********************************************************************/
#include "srincs.h"
#include "srlsa.h"
#include "srglob.h"
#include "ospfcli.h"
PUBLIC INT4         OspfV3SrEnqueueMsgFromOspf (tV3OsToOpqApp * pOspfV3Msg);

PUBLIC UINT4
       SrV3GetAddrPrefix (tOs3LsaInfo * pOsLsaInfo, tIp6Addr * pPrefixAddr);

PUBLIC INT4 V3OpqAppRegisterInCxt PROTO ((UINT4 u4OspfCxtId,
                                          UINT1 u1AppOpqType,
                                          UINT1 u1OpqLSATypesSupported,
                                          UINT4 u4InfoFromOSPF,
                                          VOID *OpqAppCallBackFn));

PUBLIC INT4 V3OpqAppDeRegisterInCxt PROTO ((UINT4 u4OspfCxtId,
                                            UINT1 u1AppOpqType));

PUBLIC INT4 V3OpqAppToOpqModuleSendInCxt PROTO ((UINT4 u4OspfCxtId,
                                                 tV3OpqLSAInfo * pOpqLSAInfo));

PUBLIC INT4




SrConstructV3PrefixSidSubTlv (UINT1 **pTempCurrent, UINT4 SidLabelType,
                              UINT4 u4Val);

PUBLIC INT4
SrSndMsgToOspf (UINT1 u1MsgType, UINT4 u4SrCxtId, tOpqLSAInfo * pOpqLSAInfo)
{
    switch (u1MsgType)
    {
#ifdef OSPF_WANTED
        case SR_OSPF_REG:
            if (OpqAppRegisterInCxt (u4SrCxtId,
                                     EXT_PREFIX_LSA_OPQ_TYPE,
                                     SR_OPQ_LSA_SUPPORTED,
                                     SR_OSPF_INFO,
                                     (VOID *) OspfSrEnqueueMsgFromOspf) !=
                OSPF_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                        "EXT_PREFIX_LSA_OPQ_TYPE registration with OSPF failure \n");
                return SR_FAILURE;
            }
            SR_TRC (SR_CTRL_TRC,
                    "EXT_PREFIX_LSA_OPQ_TYPE registration with OSPF success \n");

            if (OpqAppRegisterInCxt (u4SrCxtId,
                                     RI_LSA_OPQ_TYPE,
                                     SR_OPQ_LSA_SUPPORTED,
                                     SR_OSPF_INFO,
                                     (VOID *) OspfSrEnqueueMsgFromOspf) !=
                OSPF_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                        "RI_LSA_OPQ_TYPE registration with OSPF failure \n");
                return SR_FAILURE;
            }
            SR_TRC (SR_CTRL_TRC,
                    "RI_LSA_OPQ_TYPE registration with OSPF success \n");

            if (OpqAppRegisterInCxt (u4SrCxtId,
                                     EXT_LINK_LSA_OPQ_TYPE,
                                     SR_OPQ_LSA_SUPPORTED,
                                     SR_OSPF_INFO,
                                     (VOID *) OspfSrEnqueueMsgFromOspf) !=
                OSPF_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                        "EXT_LINK_LSA_OPQ_TYPE registration with OSPF failure \n");
                return SR_FAILURE;
            }
            SR_TRC (SR_CTRL_TRC,
                    "EXT_LINK_LSA_OPQ_TYPE registration with OSPF success \n");

            /* Register with OSPF to get ABR status and Router ID */
            SrRegWithOspfForInfoGet ();
#endif
            gSrGlobalInfo.u1SrOspfRegState[u4SrCxtId] = SR_TRUE;
            break;

        case SR_OSPF_SEND_OPQ_LSA:
#ifdef OSPF_WANTED
            if (OpqAppToOpqModuleSendInCxt (u4SrCxtId,
                                            pOpqLSAInfo) != OSPF_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "LSA Sending Failed \n");
                return SR_FAILURE;
            }
            else
            {
                SR_TRC (SR_CTRL_TRC, "LSA Sending successful \n");
            }
#endif
            break;

        case SR_OSPF_DEREG:
#ifdef OSPF_WANTED
            if (OpqAppDeRegisterInCxt (u4SrCxtId,
                                       RI_LSA_OPQ_TYPE) != OSPF_SUCCESS)
            {
                SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "LSA DeRegistration Failed for RI_LSA_OPQ_TYPE %d \n",
                         RI_LSA_OPQ_TYPE);
                return SR_FAILURE;
            }
            if (OpqAppDeRegisterInCxt (u4SrCxtId,
                                       EXT_PREFIX_LSA_OPQ_TYPE) != OSPF_SUCCESS)
            {
                SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "LSA DeRegistration Failed for EXT_PREFIX_LSA_OPQ_TYPE %d \n",
                         EXT_PREFIX_LSA_OPQ_TYPE);
                return SR_FAILURE;
            }

            if (OpqAppDeRegisterInCxt (u4SrCxtId,
                                       EXT_LINK_LSA_OPQ_TYPE) != OSPF_SUCCESS)
            {
                SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "LSA DeRegistration Failed for EXT_LINK_LSA_OPQ_TYPE %d \n",
                         EXT_LINK_LSA_OPQ_TYPE);
                return SR_FAILURE;
            }
#endif
            SrNbrInfoCleanup (SR_IPV4_ADDR_TYPE);
            /* De-Register with OSPF */
            SrRegWithOspfForInfoGet ();
            gSrGlobalInfo.u1StartupExtPrfLSASent = SR_FALSE;
            gSrGlobalInfo.u1StartupExtLinkLSASent = SR_FALSE;
            gSrGlobalInfo.u1StartupRILSASent = SR_FALSE;
            gSrGlobalInfo.u1SrOspfRegState[u4SrCxtId] = SR_FALSE;
            break;

        case SR_OSPF_LSA_REQ:
            if (OpqAppReqLsaInfoInCxt (u4SrCxtId,
                        RI_LSA_OPQ_TYPE) != OSPF_SUCCESS)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                        "%s %d : RI_LSA_OPQ_TYPE registration with OSPF failed\n",
                        __func__, __LINE__);

                return SR_FAILURE;
            }

            if (OpqAppReqLsaInfoInCxt (u4SrCxtId,
                        EXT_PREFIX_LSA_OPQ_TYPE) != OSPF_SUCCESS)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                        "%s %d :EXT_PREFIX_LSA_OPQ_TYPE registration with OSPF failed\n",
                        __func__, __LINE__);

                return SR_FAILURE;
            }

            if (OpqAppReqLsaInfoInCxt (u4SrCxtId,
                        EXT_LINK_LSA_OPQ_TYPE) != OSPF_SUCCESS)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                        "%s  %d : EXT_LINK_LSA_OPQ_TYPE  registration with OSPF failed\n",
                        __func__, __LINE__);

                return SR_FAILURE;
            }
            break;

        default:
            break;
    }
    return SR_SUCCESS;
}

PUBLIC INT4
SrSndMsgToOspfV3 (UINT1 u1MsgType, UINT4 u4SrCxtId, tV3OpqLSAInfo * pOpqLSAInfo)
{
    switch (u1MsgType)
    {
#ifdef OSPF3_WANTED
        case SR_OSPFV3_REG:
            if (V3OpqAppRegisterInCxt (u4SrCxtId,
                                       OSPFV3_EXT_INTER_AREA_PREFIX_TLV,
                                       SR_OPQ_LSA_SUPPORTED,
                                       SR_OSPF_INFO,
                                       (VOID *) OspfV3SrEnqueueMsgFromOspf) !=
                OSPF_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                        "OSPFV3_EXT_INTER_AREA_PREFIX_TLV registration with OSPF Failed \n");
                return SR_FAILURE;
            }
            SR_TRC (SR_CTRL_TRC,
                    "OSPFV3_EXT_INTER_AREA_PREFIX_TLV registration with OSPF success \n");

            if (V3OpqAppRegisterInCxt (u4SrCxtId,
                                       OSPFV3_EXT_INTRA_AREA_PREFIX_TLV,
                                       SR_OPQ_LSA_SUPPORTED,
                                       SR_OSPF_INFO,
                                       (VOID *) OspfV3SrEnqueueMsgFromOspf) !=
                OSPF_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                        "OSPFV3_EXT_INTRA_AREA_PREFIX_TLV registration with OSPF Failed \n");
                return SR_FAILURE;
            }
            SR_TRC (SR_CTRL_TRC,
                    "OSPFV3_EXT_INTRA_AREA_PREFIX_TLV registration with OSPF success \n");

            if (V3OpqAppRegisterInCxt (u4SrCxtId,
                                       AREA_SCOPE_OPQ_LSA_MASK,
                                       SR_OPQ_LSA_SUPPORTED,
                                       SR_OSPF_INFO,
                                       (VOID *) OspfV3SrEnqueueMsgFromOspf) !=
                OSPF_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                        "AREA_SCOPE_OPQ_LSA_MASK registration with OSPF Failed \n");
                return SR_FAILURE;
            }
            SR_TRC (SR_CTRL_TRC,
                    "AREA_SCOPE_OPQ_LSA_MASK registration with OSPF success \n");
            /* Registeration of Sr with OSPF v3 to receive the ABR and ASBR status */
            SrRegWithOspf3ForInfoGet ();
            SrNbrInfoCleanup (SR_IPV6_ADDR_TYPE);

#endif
            gSrGlobalInfo.u1SrOspfV3RegState[u4SrCxtId] = SR_TRUE;
            break;

        case SR_OSPFV3_DEREG:
#ifdef OSPF3_WANTED
            if (V3OpqAppDeRegisterInCxt (u4SrCxtId,
                                         OSPFV3_EXT_INTER_AREA_PREFIX_TLV) !=
                OSPF_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                        "LSA DeRegistration Failed for OSPFV3_EXT_INTER_AREA_PREFIX_TLV \n");
                return SR_FAILURE;
            }
            SR_TRC (SR_CTRL_TRC,
                    "LSA DeRegistration for OSPFV3_EXT_INTER_AREA_PREFIX_TLV with OSPF success \n");

            if (V3OpqAppDeRegisterInCxt (u4SrCxtId,
                                         OSPFV3_EXT_INTRA_AREA_PREFIX_TLV) !=
                OSPF_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                        "LSA DeRegistration Failed for  OSPFV3_EXT_INTRA_AREA_PREFIX_TLV \n");
                return SR_FAILURE;
            }
            SR_TRC (SR_CTRL_TRC,
                    "LSA DeRegistration for OSPFV3_EXT_INTRA_AREA_PREFIX_TLV with OSPF success \n");

            if (V3OpqAppDeRegisterInCxt (u4SrCxtId,
                                         AREA_SCOPE_OPQ_LSA_MASK) !=
                OSPF_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                        "LSA DeRegistration Failed for AREA_SCOPE_OPQ_LSA_MASK \n");
                return SR_FAILURE;
            }
            SR_TRC (SR_CTRL_TRC,
                    "LSA DeRegistration for AREA_SCOPE_OPQ_LSA_MASK with OSPF success \n");
            /* De-Registeration of Sr with OSPF v3 to receive the ABR and ASBR status */
            SrDeRegWithOspf3ForInfoGet ();
#endif
            break;

        case SR_OSPFV3_SEND_OPQ_LSA:
#ifdef OSPF3_WANTED
            if (V3OpqAppToOpqModuleSendInCxt (u4SrCxtId,
                                              pOpqLSAInfo) != OSPF_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "OSPFv3_LSA Sending Failed \n");
                return SR_FAILURE;
            }
            else
            {
                SR_TRC (SR_CTRL_TRC, "OSPFv3_LSA Sending successful \n");
            }
            break;
#endif
        default:
            break;
    }
    UNUSED_PARAM (pOpqLSAInfo);
    return SR_SUCCESS;
}

PUBLIC INT4
SrConstructRtrInfoLsa (tOpqLSAInfo * pOpqLSAInfo, UINT1 u1LsaStatus,
                       UINT1 u1LsaType, UINT4 u4AreaId)
{
    UINT1              *pRtrAdrTlv = NULL;
    UINT1              *pCurrent = NULL;
    UINT4               u4AdvRtrID = SR_ZERO;
    UINT4               u4IpAddr = SR_ZERO;
    UINT4               nodeLabel = SR_ZERO;
    INT4                i4AbrStatus = SR_FALSE;

    if (gSrGlobalInfo.u1OspfAbrStatus == SR_TRUE)
    {
        i4AbrStatus = SR_TRUE;
    }

    u4AdvRtrID = gSrGlobalInfo.u4Ospfv2RtrID;

    pRtrAdrTlv = pOpqLSAInfo->pu1LSA;
    pCurrent = pRtrAdrTlv;
    /** Create the SR-ALgorithm TLV **/
    if (SrConstructRtrInfoSrAlgoTlv (&pCurrent) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                "%s:%d : SrConstructRtrInfoSrAlgoTlv Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }
    /*** Create SID/Label Range TLV ***/
    if (SrConstructRtrInfoSidLabelRangeTlv (&pCurrent) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                "%s:%d : SrConstructRtrInfoSidLabelRangeTlv Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }
    /*** RI LSA Header **/
    pOpqLSAInfo->LsId[SR_ZERO] = RI_LSA_OPAQUE_TYPE;
    pOpqLSAInfo->LsId[SR_TWO] = 0;
    pOpqLSAInfo->LsId[SR_THREE] = 0;

    MEMCPY (pOpqLSAInfo->AdvRtrID, &u4AdvRtrID, sizeof (UINT4));
    pOpqLSAInfo->u4AreaID = u4AreaId;
    pOpqLSAInfo->u2LSALen = (UINT2) (pCurrent - pRtrAdrTlv);
    pOpqLSAInfo->u1OpqLSAType = u1LsaType;
    SR_UPDT_LSA_FLAG (u1LsaStatus, pOpqLSAInfo);

    if (FLUSHED_LSA != u1LsaStatus)
    {
        pOpqLSAInfo->i1SrSid = (INT1) SR_ONE;
    }

    return SR_SUCCESS;
}

PUBLIC INT4
SrConstructRtrInfoSrAlgoTlv (UINT1 **pTempCurrent)
{
    UINT1              *pCurrent = NULL;
    pCurrent = *pTempCurrent;

    /** Create the SR-ALgorithm TLV **/

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) RI_LSA_SR_ALGO_TLV_TYPE);

    /* Filling Length Value */
    SR_LADD2BYTE (pCurrent, (UINT2) RI_LSA_SR_ALGO_TLV_LEN);

    /* Filling Value filed -- Router Id */
    SR_LADD1BYTE (pCurrent, (UINT1) RI_LSA_SR_ALGO_SPF);

    /* Padding to align to 4-octet boundary */
    SR_LADD3BYTE (pCurrent, (UINT1) SR_ZERO);

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

PUBLIC INT4
SrConstructSidLabelSubTlv (UINT1 **pTempCurrent,
                           UINT4 SidLabelType, UINT4 u4Val)
{
    UINT1              *pCurrent = NULL;
    pCurrent = *pTempCurrent;

    /** Create the SR-ALgorithm TLV **/

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) SID_LABEL_SUB_TLV_TYPE);

    /* Filling Length Value */
    if (SidLabelType == SR_SID_TYPE_LABEL)
    {
        SR_LADD2BYTE (pCurrent, (UINT2) SID_LABEL_SUB_TLV_LABEL_LENGTH);

        /* Filling SID Value  */
        SR_LADD3BYTE (pCurrent, u4Val);
        SR_LADD1BYTE (pCurrent, SR_ZERO);
    }
    else if (SidLabelType == SR_SID_TYPE_INDEX)
    {
        SR_LADD2BYTE (pCurrent, (UINT2) SID_LABEL_SUB_TLV_SID_LENGTH);
        /* Filling SID Value  */
        SR_LADD4BYTE (pCurrent, u4Val);
    }
    else
    {
        return SR_FAILURE;
    }

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

PUBLIC INT4
SrConstructPrefixSidSubTlv (UINT1 **pTempCurrent,
                            UINT4 SidLabelType, UINT4 u4Val)
{
    UINT1              *pCurrent = NULL;
    pCurrent = *pTempCurrent;

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) PREFIX_SID_SUB_TLV_TYPE);

    /* Filling Length Value */
    SR_LADD2BYTE (pCurrent, (UINT2) PREFIX_SID_SUB_TLV_LEN);

    /* Filling Flags */
    SR_LADD1BYTE (pCurrent, (PREFIX_SID_NP_FLAG));

    /* Filling Reserved field */
    SR_LADD1BYTE (pCurrent, (UINT1) SR_ZERO);

    /* Filling MT-ID field */
    SR_LADD1BYTE (pCurrent, (UINT1) SR_ZERO);

    /* Filling Algo field */
    SR_LADD1BYTE (pCurrent, (UINT1) RI_LSA_SR_ALGO_SPF);

    /* Filling SID Value */
    if (SidLabelType == SR_SID_TYPE_LABEL)
    {
        SR_LADD3BYTE (pCurrent, u4Val);
        SR_LADD1BYTE (pCurrent, SR_ZERO);
    }
    else
    {
        SR_LADD4BYTE (pCurrent, u4Val);
    }

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

PUBLIC INT4
SrConstructRtrInfoSidLabelRangeTlv (UINT1 **pTempCurrent)
{
    UINT1              *pCurrent = NULL;
    pCurrent = *pTempCurrent;

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) RI_LSA_SR_SID_LABEL_RANGE_TLV_TYPE);

    /* Filling Length Value */
    SR_LADD2BYTE (pCurrent, (UINT2) RI_LSA_SR_SID_LABEL_RANGE_TLV_LEN);

    /* Filling Range Size */
    SR_LADD3BYTE (pCurrent, (SR_V4_SRGB_MAX_VAL - SR_V4_SRGB_MIN_VAL + SR_ONE));

    /* Filling Reserved field */
    SR_LADD1BYTE (pCurrent, (UINT1) SR_ZERO);

    /* Add SID/Label Range sub-TLV */
    if (SrConstructSidLabelSubTlv
        (&pCurrent, SR_SID_TYPE_LABEL, SR_V4_SRGB_MIN_VAL) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                "%s:%d : SrConstructSidLabelSubTlv Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

PUBLIC INT4
SrConstructExtPrefixTlv (UINT1 **pTempCurrent, UINT1 u1LsaStatus,
                         tSrSidInterfaceInfo *pSrSidEntry)
{
    UINT1              *pCurrent = NULL;
    UINT1              *pTlvLength = NULL;
    UINT4               nodeLabel = SR_ZERO;
    UINT4               u4TmpAddr = SR_ZERO;
    UINT4               u4AddressPrefix = SR_ZERO;

    pCurrent = *pTempCurrent;
    MEMCPY (&u4AddressPrefix, &pSrSidEntry->ifIpAddr.Addr.u4Addr,
            sizeof (UINT4));

    /** Create the SR-ALgorithm TLV **/

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) OSPF_EXT_PRREFIX_TLV_TYPE);

    /* Filling Length Value */
    pTlvLength = pCurrent;
    UNUSED_PARAM (pTlvLength);
    SR_LADD2BYTE (pCurrent, (UINT2) OSPF_EXT_PRREFIX_TLV_LENGTH);    /*ExtPrefixTlv Data + SidLabelSubTlv */

    /*Filling Route Type */
    SR_LADD1BYTE (pCurrent, (UINT1) OSPF_INTRA_AREA);

    /*Filling Prefix Length */
    SR_LADD1BYTE (pCurrent, (UINT1) (SR_IPV4_ADDR_LENGTH * 8));

    /*Filling Address Family */
    SR_LADD1BYTE (pCurrent, (UINT1) SR_ZERO);    /*Address Family --> IPv4 */

    /*Filling Flags */
    SR_LADD1BYTE (pCurrent, (UINT1) EXT_PREFIX_N_FLAG);    /*N-bit is enabled */

    /*Filling Address Prefix */
    MEMCPY (&u4TmpAddr, &pSrSidEntry->ifIpAddr.Addr.u4Addr,
            sizeof (tRouterId));
    SR_LADD4BYTE (pCurrent, (UINT4) u4TmpAddr);

    if (SrUtilCheckNodeIdConfigured () == SR_FAILURE)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d gSrGlobalInfo.pSrSelfNodeInfo == NULL \n", __func__,
                __LINE__);
        return SR_FAILURE;
    }

    SR_GET_SELF_SID_INDEX_VAL (&nodeLabel, pSrSidEntry);

    /* Add SID/Label Range sub-TLV */
    if (SrConstructPrefixSidSubTlv (&pCurrent, SR_SID_TYPE_INDEX, nodeLabel)
        != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                "%s:%d : SrConstructPrefixSidSubTlv Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }
    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

PUBLIC INT4
SrConstructExtPrefixOpqLsa (tOpqLSAInfo * pOpqLSAInfo,
                            UINT1 u1LsaStatus, UINT1 u1LsaType,
                            tSrSidInterfaceInfo *pSrSidEntry)
{
    UINT1              *pRtrAdrTlv = NULL;
    UINT1              *pCurrent = NULL;
    UINT4               u4AdvRtrID = SR_ZERO;
    UINT4               u4IpAddr = SR_ZERO;
    UINT4               nodeLabel = SR_ZERO;
    INT4                i4AbrStatus = SR_FALSE;

    MEMCPY (&u4IpAddr, &pSrSidEntry->ifIpAddr.Addr.u4Addr,
            sizeof (UINT4));

    if (gSrGlobalInfo.u1OspfAbrStatus == SR_TRUE)
    {
        i4AbrStatus = SR_TRUE;
    }

    u4AdvRtrID = gSrGlobalInfo.u4Ospfv2RtrID;

    pRtrAdrTlv = pOpqLSAInfo->pu1LSA;
    pCurrent = pRtrAdrTlv;

    SR_GET_SELF_SID_INDEX_VAL (&nodeLabel, pSrSidEntry);
    if (SrConstructExtPrefixTlv (&pCurrent,
                                 u1LsaStatus, pSrSidEntry) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                "%s:%d : SrConstructExtPrefixTlv Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    /*** RI LSA Header **/
    pOpqLSAInfo->LsId[SR_ZERO] = OSPF_EXT_PRREFIX_LSA_OPAQUE_TYPE;
    pOpqLSAInfo->LsId[SR_TWO] = (UINT1) (nodeLabel & SR_MASK_TYPE1);
    pOpqLSAInfo->LsId[SR_THREE] = (UINT1) (nodeLabel & SR_MASK_TYPE2);

    MEMCPY (pOpqLSAInfo->AdvRtrID, &u4AdvRtrID, sizeof (UINT4));
    pOpqLSAInfo->u4AreaID = pSrSidEntry->u4AreaId;
    pOpqLSAInfo->u2LSALen = (UINT2) (pCurrent - pRtrAdrTlv);
    pOpqLSAInfo->u1OpqLSAType = u1LsaType;
    SR_UPDT_LSA_FLAG (u1LsaStatus, pOpqLSAInfo);

    if (FLUSHED_LSA != u1LsaStatus)
    {
       if (SR_TRUE == i4AbrStatus)
       {
           pOpqLSAInfo->i1SrSid = (INT1) SR_TWO;
       }
       else
       {
           pOpqLSAInfo->i1SrSid = (INT1) SR_ONE;
       }
    }

    return SR_SUCCESS;
}

#if 1

PUBLIC INT4
SrConstructAdjSidSubTlv (UINT1 **pTempCurrent, tAdjSidNode *pAdjSidNode)
{
    UINT1              *pCurrent = NULL;

    pCurrent  = *pTempCurrent;

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) ADJ_SID_SUB_TLV_TYPE);

    /* Filling Length Value */
    if (pAdjSidNode->u1Flags & ADJ_SID_V_FLAG)
    {
        SR_LADD2BYTE (pCurrent, (UINT2) ADJ_SID_LABEL_SUB_TLV_LEN);
    }
    else
    {
        SR_LADD2BYTE (pCurrent, (UINT2) ADJ_SID_INDEX_SUB_TLV_LEN);
    }

    /* Filling Flags */
    SR_LADD1BYTE (pCurrent, pAdjSidNode->u1Flags);

    /* Filling Reserved field */
    SR_LADD1BYTE (pCurrent, (UINT1) SR_ZERO);

    /* Filling MT-ID field */
    SR_LADD1BYTE (pCurrent, (UINT1) pAdjSidNode->u1MTID);

    /* Filling Weight field */
    SR_LADD1BYTE (pCurrent, (UINT1) pAdjSidNode->u1Weight);

    /* Filling SID Value */
    if (pAdjSidNode->u1Flags & ADJ_SID_V_FLAG)
    {
        SR_LADD3BYTE (pCurrent, pAdjSidNode->u4Label);
        SR_LADD1BYTE (pCurrent, SR_ZERO); /*padding */
    }
    else
    {
        SR_LADD4BYTE (pCurrent, pAdjSidNode->u4Label);
    }

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

#else
PUBLIC INT4
SrConstructAdjSidSubTlv (UINT1 **pTempCurrent, UINT4 SidLabelType, UINT4 u4Val)
{
    UINT1              *pCurrent = NULL;
    pCurrent = *pTempCurrent;

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) ADJ_SID_SUB_TLV_TYPE);

    /* Filling Length Value */
    SR_LADD2BYTE (pCurrent, (UINT2) ADJ_SID_SUB_TLV_LEN);

    /* Filling Flags */
    SR_LADD1BYTE (pCurrent, (ADJ_SID_V_FLAG | ADJ_SID_L_FLAG));

    /* Filling Reserved field */
    SR_LADD1BYTE (pCurrent, (UINT1) SR_ZERO);

    /* Filling MT-ID field */
    SR_LADD1BYTE (pCurrent, (UINT1) SR_ZERO);

    /* Filling Weight field */
    SR_LADD1BYTE (pCurrent, (UINT1) SR_ZERO);

    /* Filling SID Value */
    if (SidLabelType == SR_SID_TYPE_LABEL)
    {
        SR_LADD3BYTE (pCurrent, u4Val);
        SR_LADD1BYTE (pCurrent, SR_ZERO);
    }
    else
    {
        SR_LADD4BYTE (pCurrent, u4Val);
    }

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

#endif

#if 1
PUBLIC INT4
SrConstructExtLinkTlv (tSrExtLinkLSAInfo  *pExtLinkInfo, UINT1 **pTempCurrent)
{
    UINT1               *pCurrent      = NULL;
    UINT1               *pTlvLength    = NULL;
    tAdjSidNode         *pAdjSidNode   = NULL;

    pCurrent = *pTempCurrent;

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) OSPF_EXT_LINK_TLV_TYPE);

    /* Get the TLV length pointer and fill it after updaitng sub TLVs */
    pTlvLength = pCurrent;

    /* skip the TLV length field which will be updated at end */
    pCurrent = pCurrent+2;

    /*Filling Link Type */
    SR_LADD1BYTE (pCurrent, (UINT1) pExtLinkInfo->LinkInfo.u1LinkType);

    /*Reserved bytes */
    SR_LADD3BYTE (pCurrent, (UINT4) SR_ZERO);

    /*Filling Link ID */
    SR_LADD4BYTE (pCurrent, pExtLinkInfo->LinkInfo.u4LinkId);

    /*Filling Link Data */
    SR_LADD4BYTE (pCurrent, pExtLinkInfo->LinkInfo.u4LinkData);

     /* Scan through the ADJ Sid list and Type10 LSAs present */
    TMO_SLL_Scan (&(pExtLinkInfo->adjSidSubTlvList), pAdjSidNode, tAdjSidNode *)
    {
        /* If the absolute value of the label is 0, then continue */
        if (0 == pAdjSidNode->u4Label)
        {
            continue;
        }

        if (OSPF_LINK_TYPE_P2P == pExtLinkInfo->LinkInfo.u1LinkType)
        {
            SrConstructAdjSidSubTlv (&pCurrent, pAdjSidNode);
        }

        /* currenlty copy the label as LS ID, later we may use running number */
        if (0 == pExtLinkInfo->u4LsaId)
        {
            pExtLinkInfo->u4LsaId= pAdjSidNode->u4Label;
            /*set New flag as originating LSA first time */
            pExtLinkInfo->ulFlags |= OSPF_EXT_LINK_INFO_NEW;
        }
    }

    /* Filling Length value */
    SR_LADD2BYTE (pTlvLength, (UINT2) (pCurrent-(*pTempCurrent+4)));

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

#else

PUBLIC INT4
SrConstructExtLinkTlv (tSrSidInterfaceInfo * pSrSidEntry, UINT1 **pTempCurrent)
{
    UINT1              *pCurrent = NULL;
    UINT1              *pTlvLength = NULL;
    UINT4               u4IpAddr = SR_ZERO;
    pCurrent = *pTempCurrent;

    /** Create the SR-ALgorithm TLV **/

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) OSPF_EXT_LINK_TLV_TYPE);

    /* Filling Length Value */
    pTlvLength = pCurrent;
    UNUSED_PARAM (pTlvLength);
    SR_LADD2BYTE (pCurrent, (UINT2) (12));

    /*Filling Link Type */
    SR_LADD1BYTE (pCurrent, (UINT1) 2);

    /*Reserved bytes */
    SR_LADD3BYTE (pCurrent, (UINT4) SR_ZERO);

    MEMCPY (&u4IpAddr, &pSrSidEntry->ifIpAddr.Addr.u4Addr, sizeof (UINT4));
    /*Filling Link ID */
    SR_LADD4BYTE (pCurrent, u4IpAddr);

    /*Filling Link Data */
    SR_LADD4BYTE (pCurrent, u4IpAddr);

    SrConstructAdjSidSubTlv (&pCurrent, SR_SID_TYPE_LABEL,
                             pSrSidEntry->u4PrefixSidLabelIndex);

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}
#endif

#if 1
PUBLIC INT4
SrConstructExtLinkOpqLsa (tOpqLSAInfo * pOpqLSAInfo,
                          tSrExtLinkLSAInfo  *pExtLinkInfo,
                          UINT4               u4AreaId)
{
    UINT1              *pExtLinkTlv  = NULL;
    UINT1              *pCurrent     = NULL;

    pExtLinkTlv = pOpqLSAInfo->pu1LSA;
    pCurrent = pExtLinkTlv;

    if (SrConstructExtLinkTlv (pExtLinkInfo, &pCurrent) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                "%s:%d : SrConstructExtLinkTlv Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    /*LSA ID, currenlty generated from ADJ SID, later we may use genric log */
    pOpqLSAInfo->LsId[SR_ZERO]  = OSPF_EXT_LINK_LSA_OPAQUE_TYPE;
    pOpqLSAInfo->LsId[SR_ONE]   = (UINT1)((pExtLinkInfo->u4LsaId & SR_ADJ_SID_MASK1) >> 16);
    pOpqLSAInfo->LsId[SR_TWO]   = (UINT1)((pExtLinkInfo->u4LsaId & SR_ADJ_SID_MASK2) >> 8);
    pOpqLSAInfo->LsId[SR_THREE] = (UINT1)(pExtLinkInfo->u4LsaId & SR_ADJ_SID_MASK3);

    MEMCPY (pOpqLSAInfo->AdvRtrID, &gSrGlobalInfo.u4Ospfv2RtrID, sizeof (UINT4));

    pOpqLSAInfo->u4AreaID = u4AreaId;
    pOpqLSAInfo->u2LSALen = (UINT2) (pCurrent - pExtLinkTlv);
    pOpqLSAInfo->u1OpqLSAType = TYPE10_OPQ_LSA;
    return SR_SUCCESS;
}

#else

PUBLIC INT4
SrConstructExtLinkOpqLsa (tOpqLSAInfo * pOpqLSAInfo,
                          UINT1 u1LsaStatus, UINT1 u1LsaType)
{
    UINT1              *pRtrAdrTlv = NULL;
    UINT1              *pCurrent = NULL;
    tSrSidInterfaceInfo *pSrSidEntry = NULL;
    UINT4               u4AdvRtrID = SR_ZERO;
    UINT4               u4AreaId = SR_ZERO;
    UINT4               u4IpAddr = SR_ZERO;
    UINT4               nodeLabel = SR_ZERO;

    MEMCPY (&u4IpAddr, &gSrGlobalInfo.SrContext.rtrId.Addr.u4Addr,
            sizeof (UINT4));

    /* Fetching Area ID based on IP address */
    SrUtilFetchOspfv2AreadID (u4IpAddr, SR_ZERO, &u4AreaId);

    gSrGlobalInfo.SrContext.u4areaId = u4AreaId;
    u4AdvRtrID = gSrGlobalInfo.u4Ospfv2RtrID;

    pRtrAdrTlv = pOpqLSAInfo->pu1LSA;
    pCurrent = pRtrAdrTlv;

    /** Create the SR-ALgorithm TLV **/

    pSrSidEntry = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);
    while (pSrSidEntry != NULL)
    {
        if (pSrSidEntry->u4SidType == SR_SID_ADJACENCY)
        {
            if (SrConstructExtLinkTlv (pSrSidEntry, &pCurrent) != SR_SUCCESS)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                        "%s:%d : SrConstructExtLinkTlv Failed \n",
                        __func__, __LINE__);
                return SR_FAILURE;
            }
        }
        pSrSidEntry =
            RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidEntry, NULL);
    }

    SR_GET_SELF_SID_INDEX_VAL (&nodeLabel);
    /***LSA Header **/
    pOpqLSAInfo->LsId[SR_ZERO] = OSPF_EXT_LINK_LSA_OPAQUE_TYPE;
    pOpqLSAInfo->LsId[SR_TWO] = (UINT1) (nodeLabel & SR_MASK_TYPE1);
    pOpqLSAInfo->LsId[SR_THREE] = (UINT1) (nodeLabel & SR_MASK_TYPE2);

    MEMCPY (pOpqLSAInfo->AdvRtrID, &u4AdvRtrID, sizeof (UINT4));

    pOpqLSAInfo->u4AreaID = gSrGlobalInfo.SrContext.u4areaId;
    pOpqLSAInfo->u2LSALen = (UINT2) (pCurrent - pRtrAdrTlv);
    pOpqLSAInfo->u1OpqLSAType = u1LsaType;
    SR_UPDT_LSA_FLAG (u1LsaStatus, pOpqLSAInfo);
    return SR_SUCCESS;
}

#endif

PUBLIC INT4
OspfSrEnqueueMsgFromOspf (tOsToOpqApp * pOspfMsg)
{
    tSrQMsg             SrQMsg;
    tOspfSrLsa          *pSrLsa;

    MEMSET (&SrQMsg, SR_ZERO, sizeof (tSrQMsg));
    SrQMsg.u4MsgType = SR_OSPF_MSG_EVENT;

    if (pOspfMsg->u4MsgSubType == OSPF_TE_LSA_INFO)
    {
        if (pOspfMsg->lsaInfo.u2LsaLen > MAX_SR_LSAS)
        {
            SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC, "SR LSA Length can't be more than %d \n",
                    MAX_SR_LSAS);
            return SR_FAILURE;
        }

        if (SR_OSPF_LSA_MEM_ALLOC (pSrLsa) == NULL)
        {
            SR_TRC (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                    "Memory Allocation Failure for SR OSPf  \n");
            return SR_FAILURE;
        }

        MEMCPY (pSrLsa->au1Lsa, pOspfMsg->lsaInfo.pu1Lsa,
                pOspfMsg->lsaInfo.u2LsaLen);
        pOspfMsg->lsaInfo.pu1Lsa = pSrLsa->au1Lsa;
    }

    MEMCPY (&(SrQMsg.u.OstoSrMsgParams), pOspfMsg, sizeof (tOsToOpqApp));

    if (SrEnqueueMsgToSrQ (SR_MSG_EVT, &SrQMsg) == SR_FAILURE)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                "%s:%d Failed to EnQ SR_OSPF_MSG_EVENT Event to SR Task \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrProcessRtrLsa
 * Description   : This routine to process the self router LSA received from OSPF
 * Input(s)      : pOspfMsg  - Pointer to OSPF msg
 *                 pOsSrArea  - Pointer to area ID
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrProcessRtrLsa (tOsToOpqApp * pOspfMsg, tOsSrArea * pOsSrArea)
{
    tOsLsaInfo         *pOsLsaInfo = NULL;
    tRouterId           rtrId;
    tOpqLSAInfo         opqLSAInfo;
    tSrSidInterfaceInfo *pSrSidIntf = NULL;
    UINT1               au1RtrAddrTlv[SR_RI_LSA_TLV_LEN];
    UINT1               u1LsaStatus = NEW_LSA;
    UINT1               u1lsaType = TYPE10_OPQ_LSA;
    UINT4               u4AreaId;
    UINT1               u1ret = SR_SUCCESS;
    UINT1               u1PrefixLSASent = SR_FALSE;
    UINT1               u1RILSASent = SR_FALSE;

    UNUSED_PARAM (pOsSrArea);

    pOsLsaInfo = &(pOspfMsg->lsaInfo);
    u4AreaId = pOspfMsg->u4AreaId;

    MEMSET (rtrId, SR_ZERO, sizeof (tRouterId));
    MEMCPY (rtrId, &(pOsLsaInfo->u4AdvRtrId), sizeof (UINT4));

    MEMSET (&opqLSAInfo, SR_ZERO, sizeof (tOpqLSAInfo));
    MEMSET (au1RtrAddrTlv, SR_ZERO, sizeof (SR_RI_LSA_TLV_LEN));

    opqLSAInfo.pu1LSA = au1RtrAddrTlv;

    if (gSrGlobalInfo.u4Ospfv2RtrID != pOsLsaInfo->u4AdvRtrId)
    {
        SR_TRC4 (SR_CTRL_TRC,
                "%s:%d :  Rtr LSA's Rtr Id %x is not same as Node Rtr Id %x\n",
                __func__, __LINE__, pOsLsaInfo->u4AdvRtrId,
                gSrGlobalInfo.u4Ospfv2RtrID);

        return SR_SUCCESS;
    }

    SR_TRC4 (SR_CTRL_TRC,
            "%s:%d : Rtr LSA %x Status %d\n",
            __func__, __LINE__, pOsLsaInfo->u4AdvRtrId, pOsLsaInfo->u1LsaStatus);
    if ((pOspfMsg->u1AppId == RI_LSA_OPQ_TYPE)
                     && (!SR_RI_LSA_SENT))
    {
         SR_TRC2 (SR_CTRL_TRC,
                  "%s:%d RI_LSA_OPQ_TYPE Registration success with OSPF,"
                  " So request to Send RI NEW_LSA \r\n", __func__,
                    __LINE__);

         if (SrConstructRtrInfoLsa (&opqLSAInfo, u1LsaStatus, u1lsaType, u4AreaId)
                                            != SR_SUCCESS)
         {
                 SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                          "%s:%d : SrConstructRtrInfoLsa Failed \n", __func__,
                           __LINE__);
                 return SR_FAILURE;
         }

         if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA,
                             OSPF_DEFAULT_CXT_ID,
                              &opqLSAInfo) != SR_SUCCESS)
         {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : RtrInfoLsa Sent Failed \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
         }
                u1RILSASent = SR_TRUE;
    }
    pSrSidIntf = RBTreeGetFirst (gSrGlobalInfo.pSrSidRbTree);

    /*if (pSrSidIntf == NULL)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                "%s:%d : pSrSidIntf == NULL \n",__func__, __LINE__);
        return SR_FAILURE;
    }*/

    while (pSrSidIntf != NULL)
    {
        if (pSrSidIntf->u4SidType != SR_SID_NODE)
        {
            pSrSidIntf = RBTreeGetNext (gSrGlobalInfo.pSrSidRbTree, pSrSidIntf, NULL);
            continue;
        }

        if (pSrSidIntf->u4PrefixSidLabelIndex != SR_ZERO)
        {
            if ((pOspfMsg->u1AppId == EXT_PREFIX_LSA_OPQ_TYPE)
                && (!SR_EXT_PRF_LSA_SENT))
            {
                SR_TRC2 (SR_CTRL_TRC,
                         "%s:%d EXT_PREFIX_LSA_OPQ_TYPE Registration success with OSPF,"
                         " So request to Send Ext Prefix NEW_LSA \r\n",
                         __func__, __LINE__);

                if (SrConstructExtPrefixOpqLsa
                       (&opqLSAInfo, u1LsaStatus, u1lsaType, pSrSidIntf) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d : SrConstructExtPrefixOpqLsa Failed \n",
                             __func__, __LINE__);
                    return SR_FAILURE;
                }
                if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA,
                                    OSPF_DEFAULT_CXT_ID,
                                    &opqLSAInfo) != SR_SUCCESS)
                {
                    SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%s:%d : ExtPrefixOpqLsa Sent Failed \n", __func__,
                             __LINE__);
                    return SR_FAILURE;
                }
                u1PrefixLSASent = SR_TRUE;
            }
        }
        pSrSidIntf = RBTreeGetNext(gSrGlobalInfo.pSrSidRbTree,pSrSidIntf,NULL);
    }

    if (SR_TRUE == u1RILSASent)
    {
        gSrGlobalInfo.u1StartupRILSASent = SR_TRUE;
    }
    if (SR_TRUE == u1PrefixLSASent)
    {
       gSrGlobalInfo.u1StartupExtPrfLSASent = SR_TRUE;
    }

    if ((pOspfMsg->u1AppId == EXT_LINK_LSA_OPQ_TYPE)
          && (!SR_EXT_LINK_LSA_SENT))
    {
        u1ret = SrProcessRtrLsaforUpdateExtLinkInfo(pOsLsaInfo, u4AreaId);
    }

    return u1ret;
}

/*****************************************************************************
 * Function Name : SrUtilExtLinkInfoRbTreeCmpFunc
 * Description   : This routine to compare rb nodes of Router Links
 * Input(s)      : pRBElem1 - rb tree node
 *                 pRBElem2 - key of input data
 * Output(s)     : None
 * Return(s)     : SR_RB_GREATER / SR_RB_LESS/SR_RB_EQUAL
 *****************************************************************************/
PUBLIC INT4
SrUtilExtLinkInfoRbTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2)
{
    tSrExtLinkLSAInfo   *srLinkInfokey1   = (tSrExtLinkLSAInfo *) pRBElem1;
    tSrExtLinkLSAInfo   *srLinkInfokey2   = (tSrExtLinkLSAInfo *) pRBElem2;

        if (srLinkInfokey1->LinkInfo.u1LinkType > srLinkInfokey2->LinkInfo.u1LinkType)
        {
            return SR_RB_GREATER;
        }
        else if (srLinkInfokey1->LinkInfo.u1LinkType < srLinkInfokey2->LinkInfo.u1LinkType)
        {
            return SR_RB_LESS;
        }
        else if (srLinkInfokey1->LinkInfo.u4LinkId > srLinkInfokey2->LinkInfo.u4LinkId)
        {
            return SR_RB_GREATER;
        }
        else if (srLinkInfokey1->LinkInfo.u4LinkId < srLinkInfokey2->LinkInfo.u4LinkId)
        {
            return SR_RB_LESS;
        }
        else if (srLinkInfokey1->LinkInfo.u4LinkData > srLinkInfokey2->LinkInfo.u4LinkData)
        {
            return SR_RB_GREATER;
        }
        else if (srLinkInfokey1->LinkInfo.u4LinkData < srLinkInfokey2->LinkInfo.u4LinkData)
        {
            return SR_RB_LESS;
        }
        else
        {
            return SR_RB_EQUAL;
        }
}

/*****************************************************************************
 * Function Name : SrCreateNewAreaLinkInfoNode
 * Description   : This routine to allocate memory for DB to store Router Links
 *                 per area
 * Input(s)      : None
 * Output(s)     : pointer to Link info node
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC tareaLinkInfo  *
SrCreateNewAreaLinkInfoNode (VOID)
{
    tareaLinkInfo   *ptempAreaNode  = NULL;

    /* allocate memory for area node from mem pool */
    if (SR_AREA_LINK_INFO_MEM_ALLOC (ptempAreaNode) == NULL)
    {
        SR_TRC (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                "Memory Allocation Failure for SR Area node \n");
        return NULL;
    }

    MEMSET (ptempAreaNode, SR_ZERO, sizeof (tareaLinkInfo));

    /* initilize rb tree to store ext link lsa generaiton info, the key parameters
       of the node to identify are link type, linkid and link data which are
       carried in router links */
    ptempAreaNode->pExtLinkLsaInfoTree = RBTreeCreateEmbedded (
                              SR_OFFSET (tSrExtLinkLSAInfo, RbNode),
                              SrUtilExtLinkInfoRbTreeCmpFunc);
    if(NULL == ptempAreaNode->pExtLinkLsaInfoTree)
    {
        SR_AREA_LINK_INFO_MEM_FREE(ptempAreaNode);
        SR_TRC (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                "Memory Allocation Failure for SR Area node for External link LSA \n");
        return NULL;
    }

    return ptempAreaNode;
}

/*****************************************************************************
 * Function Name : RbWalkFnForExtLinkStaleEntry
 * Description   : This routine to stale all router links of rb tree in the area
 * Input(s)      : pRBElem - rb tree node
 * Output(s)     : None
 * Return(s)     : RB_WALK_CONT
 *****************************************************************************/
INT4
RbWalkFnForExtLinkStaleEntry (tRBElem * pRBElem, eRBVisit visit, UINT4 u4Level,
                          void *pArg, void *pOut)
{
    tSrExtLinkLSAInfo     *pLinkInfo = NULL;

    UNUSED_PARAM (u4Level);
    UNUSED_PARAM (pArg);
    UNUSED_PARAM (pOut);

    if (visit == postorder || visit == leaf)
    {
        if (pRBElem != NULL)
        {
            pLinkInfo = (tSrExtLinkLSAInfo *) pRBElem;

            /* set the stale flag for the Link info */
            pLinkInfo->ulFlags |= OSPF_EXT_LINK_INFO_STALE;
        }
    }

    return RB_WALK_CONT;
}

/*****************************************************************************
 * Function Name : RbWalkFnForExtLinkEntryProcess
 * Description   : This routine to process the Link Info with router LSA update
 * Input(s)      : pRBElem  - rb tree node
 * Output(s)     : None
 * Return(s)     : RB_WALK_CONT
 *****************************************************************************/
INT4
RbWalkFnForExtLinkEntryProcess (tRBElem * pRBElem, eRBVisit visit, UINT4 u4Level,
                          void *pArg, void *pOut)
{
    tSrExtLinkLSAInfo     *pLinkInfo = NULL;
    UINT4                  u4AreaId  = 0;

    UNUSED_PARAM (u4Level);
    UNUSED_PARAM (pOut);

    u4AreaId = *((UINT4 *)pArg);

    if (visit == postorder || visit == leaf)
    {
        if (pRBElem != NULL)
        {
            pLinkInfo = (tSrExtLinkLSAInfo *) pRBElem;
            SrProcessExtLinkInfoUpdate(pLinkInfo, u4AreaId);
        }
    }

    return RB_WALK_CONT;
}

/*****************************************************************************
 * Function Name : SrProcessExtLinkInfoUpdate
 * Description   : This routine to originate Extended Link LSA based on Link
 *                 update received from Router LSA
 * Input(s)      : pExtLinkInfo - pointer to Link Info
                   u4AreaId     - area Id
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrProcessExtLinkInfoUpdate(tSrExtLinkLSAInfo  *pExtLinkInfo, UINT4 u4AreaId)
{
    tOpqLSAInfo         opqLSAInfo;
    UINT1               au1ExtLinkLsa[SR_EXT_LINK_LSA_LEN];
    UINT1               u1LsaStatus = 0;

    MEMSET (&opqLSAInfo, 0, sizeof (tOpqLSAInfo));
    MEMSET (au1ExtLinkLsa, 0, sizeof (SR_EXT_LINK_LSA_LEN));
    opqLSAInfo.pu1LSA = au1ExtLinkLsa;

    if (0 == TMO_SLL_Count (&(pExtLinkInfo->adjSidSubTlvList)))
    {
        SR_TRC4 (SR_CTRL_TRC,
            "%s:%d SrConstructExtLinkOpqLsa not triggred for link (ID:%x, Data:%x) \n",
                            __func__, __LINE__, pExtLinkInfo->LinkInfo.u4LinkId,
                            pExtLinkInfo->LinkInfo.u4LinkData);
        return SR_SUCCESS;
    }

    if (SrConstructExtLinkOpqLsa(&opqLSAInfo, pExtLinkInfo,
                                  u4AreaId) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,  "%s:%d : SrConstructExtLinkOpqLsa Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    SR_UPDT_LSA_STATUS(pExtLinkInfo, u1LsaStatus);

    if (0 == u1LsaStatus)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,  "%s:%d : LSA status set Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    SR_TRC6 (SR_CTRL_TRC,  "%s:%d : LSA ID: %d.%d.%d.%d \n",
            __func__, __LINE__, opqLSAInfo.LsId[SR_ZERO], opqLSAInfo.LsId[SR_ONE],
            opqLSAInfo.LsId[SR_TWO], opqLSAInfo.LsId[SR_THREE]);

    SR_TRC4 (SR_CTRL_TRC,  "%s:%d : LSA status %d and LSA Flags %x \n",
            __func__, __LINE__, u1LsaStatus, pExtLinkInfo->ulFlags);

    /* reset new  and originate flags */
    pExtLinkInfo->ulFlags &= ~(OSPF_EXT_LINK_INFO_NEW | OSPF_EXT_LINK_INFO_ORG);

    opqLSAInfo.u1LSAStatus = u1LsaStatus;
    opqLSAInfo.i1SrSid = (INT1) SR_ONE;

    if (u1LsaStatus == FLUSHED_LSA)
    {
        opqLSAInfo.u1FlushTriggr = SR_TRUE;
    }

    if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA, OSPF_DEFAULT_CXT_ID,
                              &opqLSAInfo) != SR_SUCCESS)
    {
        SR_TRC2 (SR_FAIL_TRC | SR_CTRL_TRC, "%s:%d : ExtLinkOpqLsa Sent Failed \n",
               __func__, __LINE__);
        return SR_FAILURE;
    }

    SR_TRC2 (SR_CTRL_TRC, "%s:%d : ExtLinkOpqLsa Sent Successfull \n",
               __func__, __LINE__);

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrProcessExtLinkInfoUpdate
 * Description   : This routine to remove stale Link information after
 *                 update received from Router LSA
 * Input(s)      : None
 * Output(s)     : pointer to Link info node
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
VOID
SrProcessDeleteStaleExtLinkInfo(tareaLinkInfo   *pSrAreaLinkInfo)
{
    tSrExtLinkLSAInfo     *pLinkInfo = NULL;
    tSrExtLinkLSAInfo     *pNextLinkInfo = NULL;

    pLinkInfo = RBTreeGetFirst(pSrAreaLinkInfo->pExtLinkLsaInfoTree);

    while (NULL != pLinkInfo)
    {
        pNextLinkInfo = RBTreeGetNext(pSrAreaLinkInfo->pExtLinkLsaInfoTree,
                                      pLinkInfo, NULL);
        if (pLinkInfo->ulFlags & OSPF_EXT_LINK_INFO_STALE)
        {
            SrDeleteExtLinkInfoNode(pSrAreaLinkInfo, pLinkInfo);
        }

        pLinkInfo = pNextLinkInfo;
    }


}

/*****************************************************************************
 * Function Name : SrProcessRtrLsaforUpdateExtLinkInfo
 * Description   : This routine to process router LSA to add
 *                 router links to Area Db to map Adj-SIDs
 * Input(s)      : pOsLsaInfo - Pointer to Router LSA info received from OSPF
 *                 u4AreaId   - Area ID of the Router LSA originated area
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrProcessRtrLsaforUpdateExtLinkInfo (tOsLsaInfo  *pOsLsaInfo, UINT4   u4AreaId)
{

    tareaLinkInfo   *pSrAreaLinkInfo  = NULL;
    tTMO_SLL_NODE   *pAreaNode        = NULL;
    UINT1            u1AreaFound      = SR_FALSE;
    UINT4            u4LinkInfoCount  = 0;

    /* search for area node to add router links to generate Ext Link LSAs
    for the links in that area if Adj SID is configured */
    if (TMO_SLL_Count (&(gSrGlobalInfo.linkInfoList)) != 0)
    {
        TMO_SLL_Scan (&(gSrGlobalInfo.linkInfoList), pAreaNode, tTMO_SLL_NODE *)
        {
            pSrAreaLinkInfo = (tareaLinkInfo *) pAreaNode;
            if (pSrAreaLinkInfo->u4AreaId == u4AreaId)
            {
                u1AreaFound = SR_TRUE;
                break;
            }
        }
    }

    /* area node not exist, add area node */
    if (SR_FALSE == u1AreaFound)
    {
        if (FLUSHED_LSA == pOsLsaInfo->u1LsaStatus)
        {
            return SR_SUCCESS;
        }

        pSrAreaLinkInfo = SrCreateNewAreaLinkInfoNode();
        if (NULL == pSrAreaLinkInfo)
        {
            SR_TRC2 (SR_FAIL_TRC | SR_CTRL_TRC,
                    "%s:%d : pSrAreaLinkInfo == NULL \n", __func__, __LINE__);
            return SR_FAILURE;
        }

        pSrAreaLinkInfo->u4AreaId = u4AreaId;
        TMO_SLL_Add (&(gSrGlobalInfo.linkInfoList),
                            (tTMO_SLL_NODE *) pSrAreaLinkInfo);
        SR_TRC3 (SR_CTRL_TRC, "%s:%d : Added area node (Id: 0x%x) to linkInfoList \n",
            __func__, __LINE__, pSrAreaLinkInfo->u4AreaId);
    }
    else
    {
        /* If node exist stale all links and reset stale flag if same link
           received in router LSA again */
        RBTreeWalk (pSrAreaLinkInfo->pExtLinkLsaInfoTree,
                     (tRBWalkFn) RbWalkFnForExtLinkStaleEntry, 0, 0);
    }

    if (FLUSHED_LSA != pOsLsaInfo->u1LsaStatus)
    {
        SrParseRtrLsaAddRtrLinks(pOsLsaInfo, pSrAreaLinkInfo);
    }

    RBTreeWalk (pSrAreaLinkInfo->pExtLinkLsaInfoTree,
                    (tRBWalkFn) RbWalkFnForExtLinkEntryProcess,
                    &(pSrAreaLinkInfo->u4AreaId), 0);

    SrProcessDeleteStaleExtLinkInfo(pSrAreaLinkInfo);
    RBTreeCount (pSrAreaLinkInfo->pExtLinkLsaInfoTree, &u4LinkInfoCount);
    if (SR_ZERO == u4LinkInfoCount)
    {
        RBTreeDelete (pSrAreaLinkInfo->pExtLinkLsaInfoTree);
        TMO_SLL_Delete (&(gSrGlobalInfo.linkInfoList),
                            (tTMO_SLL_NODE *) pSrAreaLinkInfo);
        SR_AREA_LINK_INFO_MEM_FREE(pSrAreaLinkInfo);
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrCreateAdjSidInfoNode
 * Description   : This routine to allocate memory for  Adj SID info node
 * Input(s)      : None
 * Output(s)     : None
 * Return(s)     : tAdjSidNode - pointer to Adj Sid node
 *****************************************************************************/
PUBLIC tAdjSidNode  *
SrCreateAdjSidInfoNode (VOID)
{
    tAdjSidNode   *ptempAdjSidNode  = NULL;

    /* Allocate Adj Sid node frm mem pool */
    if (SR_PEER_ADJ_SID_MEM_ALLOC (ptempAdjSidNode) == NULL)
    {
        SR_TRC (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                "Memory Allocation Failure for Adjacent SID node \n");
        return NULL;
    }

    MEMSET (ptempAdjSidNode, SR_ZERO, sizeof (tAdjSidNode));
    return ptempAdjSidNode;
}

/*****************************************************************************
 * Function Name : SrUpdateExtRtrLinkAdjInfo
 * Description   : This routine to get the cfg Adj Sid and assigned to Link info
 * Input(s)      : tSrExtLinkLSAInfo - pointer to link info
 * Output(s)     : pExtLinkNode
 * Return(s)     : SR_SUCCESS/ SR_FAILURE
 *****************************************************************************/
PUBLIC UINT1
SrUpdateExtRtrLinkAdjInfo(tSrExtLinkLSAInfo   *pExtLinkNode)
{
    tSrSidInterfaceInfo SrSidIntf     = {0};
    tSrSidInterfaceInfo *pSrSidEntry  = NULL;
    UINT1                ulRet        = SR_SUCCESS;

    /* set Adj SID key parameters to search for config node */
    SrSidIntf.ipAddrType           =  SR_IPV4_ADDR_TYPE;

    /* intf ip address on which adj sid may be configured */
    SrSidIntf.ifIpAddr.Addr.u4Addr = pExtLinkNode->LinkInfo.u4LinkData;

    /* get the config node from the Sid tree */
    pSrSidEntry = RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

    SR_TRC3 (SR_CTRL_TRC, "%s:%d : pSrSidEntry : %x \n", __func__, __LINE__, pSrSidEntry);

    /* if Adj Sid configured onnterface with interface IP same as link data */
    if ((NULL != pSrSidEntry) && (pSrSidEntry->u4SidType == SR_SID_ADJACENCY))
    {
        ulRet = SrProcessAddAdjTlvtoLinkInfo(pExtLinkNode, pSrSidEntry);
    }

    return ulRet;
}

/*****************************************************************************
 * Function Name : SrCreateNewExtLinkInfoNode
 * Description   : This routine to allocate memory for Link info node
 * Input(s)      : None
 * Output(s)     : None
 * Return(s)     : pointer to Link info node/NULL
 *****************************************************************************/
PUBLIC tSrExtLinkLSAInfo  *
SrCreateNewExtLinkInfoNode (VOID)
{
    tSrExtLinkLSAInfo   *ptempExtLinkNode  = NULL;

    /* allocate new Link info node from mem pool */
    if (SR_EXT_LINK_LSA_INFO_MEM_ALLOC (ptempExtLinkNode) == NULL)
    {
        SR_TRC (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                "Memory Allocation Failure for SR External or new link Info node \n");
        return NULL;
    }

    MEMSET (ptempExtLinkNode, SR_ZERO, sizeof (tSrExtLinkLSAInfo));
    TMO_SLL_Init (&(ptempExtLinkNode->adjSidSubTlvList));
    return ptempExtLinkNode;
}

/*****************************************************************************
 * Function Name : SrDeleteExtLinkInfoNode
 * Description   : This routine to delete Link info node from Rb tree
 * Input(s)      : tareaLinkInfo      - pointer to area link info
                 : tSrExtLinkLSAInfo  - pointer to link info
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/ SR_FAILURE
 *****************************************************************************/
INT4
SrDeleteExtLinkInfoNode (tareaLinkInfo  *pSrAreaLinkInfo,
                         tSrExtLinkLSAInfo   *pExtLinkNode)
{
    tAdjSidNode         *pAdjSidNode      = NULL;
    tAdjSidNode         *pAdjSidNextNode  = NULL;

    if ((NULL == pSrAreaLinkInfo) || (NULL == pExtLinkNode))
    {
        SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                "%s:%d ExtLinkInfoNode Del: NULL Input (%x, %x) \n",
                __func__, __LINE__, pSrAreaLinkInfo, pExtLinkNode);
        return SR_FAILURE;
    }

    if (RBTreeRem (pSrAreaLinkInfo->pExtLinkLsaInfoTree, pExtLinkNode) == NULL)
    {
        SR_TRC4 (SR_CTRL_TRC | SR_FAIL_TRC,
                "%s:%d ExtLinkInfoNode Del(Link ID:%x Link Data:%x): Node Delete Failed \n",
                __func__, __LINE__, pExtLinkNode->LinkInfo.u4LinkId,
                pExtLinkNode->LinkInfo.u4LinkData);
        return SR_FAILURE;
    }

    SR_TRC4 (SR_CTRL_TRC,
            "%s:%d ExtLinkInfoNode Del(Link ID:%x Link Data:%x): Node removed  from Rb tree \n",
            __func__, __LINE__, pExtLinkNode->LinkInfo.u4LinkId,
            pExtLinkNode->LinkInfo.u4LinkData);

    TMO_DYN_SLL_Scan (&(pExtLinkNode->adjSidSubTlvList), pAdjSidNode, pAdjSidNextNode,
                      tAdjSidNode *)
    {
        TMO_SLL_Delete (&(pExtLinkNode->adjSidSubTlvList),
                        &(pAdjSidNode->nextAdjSid));
        SR_PEER_ADJ_SID_MEM_FREE(pAdjSidNode);
    }

    SR_EXT_LINK_LSA_INFO_MEM_FREE (pExtLinkNode);

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrAddExtRtrLinkInfo
 * Description   : This routine to create and add Link info node from Rb tree
 * Input(s)      : tareaLinkInfo      - pointer to area link info
                 : tSrLinkInfoKey     - pointer to link info key
 * Output(s)     : None
 * Return(s)     : tSrExtLinkLSAInfo
 *****************************************************************************/
PUBLIC tSrExtLinkLSAInfo *
SrAddExtRtrLinkInfo (tareaLinkInfo  *pSrAreaLinkInfo, tSrLinkInfoKey *pLinkNode)
{
    tSrExtLinkLSAInfo   *pExtLinkNode = NULL;

    /* Allocate Link node and set link key parameters */
    pExtLinkNode = SrCreateNewExtLinkInfoNode();
    if (NULL == pExtLinkNode)
    {
        SR_TRC2 (SR_CTRL_TRC,
                  "%s:%d : pExtLinkNode == NULL \n", __func__, __LINE__);
        return NULL;
    }

    pExtLinkNode->LinkInfo.u1LinkType = pLinkNode->u1LinkType;
    pExtLinkNode->LinkInfo.u4LinkId   = pLinkNode->u4LinkId;
    pExtLinkNode->LinkInfo.u4LinkData = pLinkNode->u4LinkData;

    /* Add Link node to Area Link info tree */
    if (RBTreeAdd (pSrAreaLinkInfo->pExtLinkLsaInfoTree,
                   (tRBElem *) pExtLinkNode) != RB_SUCCESS)
    {
        SR_EXT_LINK_LSA_INFO_MEM_FREE(pExtLinkNode);
        return NULL;
    }

    SR_TRC5 (SR_CTRL_TRC,
        "%s:%d : Added link info (Type : 0x%x Id: 0x%x Data: 0x%x) to linkInfoTree \n",
            __func__, __LINE__, pExtLinkNode->LinkInfo.u1LinkType,
            pExtLinkNode->LinkInfo.u4LinkId, pExtLinkNode->LinkInfo.u4LinkData);

    return pExtLinkNode;

}

/*****************************************************************************
 * Function Name : SrParseRtrLsaAddRtrLinks
 * Description   : This routine to parse Rtr LSA and add Rtr Links
 *                 to Area Link info
 * Input(s)      : pOsLsaInfo       - pointer to LSA info
                 : pSrAreaLinkInfo  - pointer to area link info
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/ SR_FAILURE
 *****************************************************************************/
PUBLIC UINT4
SrParseRtrLsaAddRtrLinks (tOsLsaInfo  *pOsLsaInfo, tareaLinkInfo *pSrAreaLinkInfo)
{
    UINT1               *pCurrPtr     = NULL;
    tSrExtLinkLSAInfo   *pExtLinkNode = NULL;
    tSrLinkInfoKey       LinkNode     = {0};
    tSrExtLinkLSAInfo    ExtLinkNode  = {0};
    UINT1                u1NewRtLsa   = SR_FALSE;
    UINT1                u1NewLink    = SR_FALSE;
    UINT4                u4Count      = 0;

    RBTreeCount (pSrAreaLinkInfo->pExtLinkLsaInfoTree, &u4Count);

    /* For newly received router LSA, EXtLinkLSA count is 0 */
    if (0 == u4Count)
    {
        u1NewRtLsa= SR_TRUE;
    }

    /* Increment the memory pointer of LSA to point the first Router link */
    pCurrPtr = pOsLsaInfo->pu1Lsa +
                     (OSPF_LSA_HEADER_LENGTH + OSPF_ROUTER_LSA_OFFSET);

    while ((pCurrPtr = OspfSrUtilGetLinkFromLsa (pOsLsaInfo->pu1Lsa,
                            pOsLsaInfo->u2LsaLen,
                            pCurrPtr, &LinkNode)) != NULL)
    {
        u1NewLink    = SR_FALSE;
        /* Currently we support to Adj SID for P2P links */
        if (OSPF_LINK_TYPE_P2P == LinkNode.u1LinkType)
        {
            /* If LSA is new, we can add router link directly */
            if (SR_TRUE == u1NewRtLsa)
            {
                pExtLinkNode = SrAddExtRtrLinkInfo(pSrAreaLinkInfo, &LinkNode);
            }
            else /* updated route LSA */
            {
                ExtLinkNode.LinkInfo.u1LinkType = LinkNode.u1LinkType;
                ExtLinkNode.LinkInfo.u4LinkId   = LinkNode.u4LinkId;
                ExtLinkNode.LinkInfo.u4LinkData = LinkNode.u4LinkData;

                /* Search for router link in exisitng links */
                pExtLinkNode = RBTreeGet(pSrAreaLinkInfo->pExtLinkLsaInfoTree,
                                                    (tRBElem *) &ExtLinkNode);

                if (NULL == pExtLinkNode)
                {
                    u1NewLink = SR_TRUE;
                    /* Add new link node to the Link info Tree */
                    pExtLinkNode = SrAddExtRtrLinkInfo(pSrAreaLinkInfo,
                                                       &LinkNode);
                }
            }
        }

        if (NULL != pExtLinkNode)
        {
            /* Reset stale flag, if already set for existing router links */
            pExtLinkNode->ulFlags &= ~(OSPF_EXT_LINK_INFO_STALE);
            if ((SR_TRUE == u1NewRtLsa) || (SR_TRUE == u1NewLink))
            {
                /* Update Adj SID info from config node if Sid is configured*/
                SrUpdateExtRtrLinkAdjInfo(pExtLinkNode);
            }
        }
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : OspfSrUtilGetLinkFromLsa
 * Description   : This routine to get each link info from LSA buffer
 * Input(s)      : pLsa              - pointer to LSA info
                 : u2LsaLen          - LSA length
                 : pCurrPtr          - current buffer pointer
 * Output(s)     : pLinkNode
 * Return(s)     : pointer to Current buffer
 *****************************************************************************/
PUBLIC UINT1*
OspfSrUtilGetLinkFromLsa (UINT1 *pLsa, UINT2 u2LsaLen,
                           UINT1 *pCurrPtr, tSrLinkInfoKey * pLinkNode)
{
    INT1                i1TosCount = 0;
    UINT1              *pPtr       = NULL;
    UINT2               u2LinkCost = 0;

    if (pCurrPtr >= pLsa + u2LsaLen)
    {
        pPtr = NULL;
    }
    else
    {
        /* Exctracting Link Id field */
        pLinkNode->u4LinkId = SR_LGET4BYTE (pCurrPtr);

        /* Exctracting Link Data field */
        pLinkNode->u4LinkData = SR_LGET4BYTE (pCurrPtr);

        /* Exctracting Link Type filed */
        pLinkNode->u1LinkType = SR_LGET1BYTE (pCurrPtr);

        i1TosCount = (INT1) SR_LGET1BYTE (pCurrPtr);

        /* Extracting TOS-0 metric */
        u2LinkCost = SR_LGET2BYTE (pCurrPtr);

        pCurrPtr = pCurrPtr + (i1TosCount * MAX_IP_ADDR_LEN);

        pPtr = pCurrPtr;
    }
    return pPtr;
}

/*****************************************************************************
 * Function Name : SrProcessAddAdjTlvtoLinkInfo
 * Description   : This routine to process adding Adj Tlv to Link Info
 * Input(s)      : pExtLinkNode      - pointer to Link info
                 : pSrSidEntry       - SID Info
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
PUBLIC UINT1
SrProcessAddAdjTlvtoLinkInfo(tSrExtLinkLSAInfo   *pExtLinkNode,
                            tSrSidInterfaceInfo *pSrSidEntry)
{
    tAdjSidNode     *pAdjSidInfo      = NULL;
    tTMO_SLL_NODE   *pAdjSidSllNode   = NULL;
    UINT4            u4AdjSidTlvCount = 0;
    UINT1            u1AdjFound      = SR_FALSE;

    u4AdjSidTlvCount = TMO_SLL_Count (&(pExtLinkNode->adjSidSubTlvList));

    if (0 != u4AdjSidTlvCount)
    {
        TMO_SLL_Scan (&(pExtLinkNode->adjSidSubTlvList),
                        pAdjSidSllNode, tTMO_SLL_NODE *)
        {
            pAdjSidInfo = (tAdjSidNode *) pAdjSidSllNode;
            if (pAdjSidInfo->u4Label == pSrSidEntry->u4PrefixSidLabelIndex)
            {
                u1AdjFound = SR_TRUE;
                break;
            }
        }

        /* Adj SID already exist, so no need to add again */
        if (SR_TRUE == u1AdjFound)
        {
            /* Set originate flag to refresh LSA */
            pExtLinkNode->ulFlags |= OSPF_EXT_LINK_INFO_ORG;
            SR_TRC5 (SR_CTRL_TRC, "%s:%d : AdjSid Node already Exist (SID: %d F:0x%x LNF:0x%x)\n",
                __func__, __LINE__, pAdjSidInfo->u4Label, pAdjSidInfo->u1Flags,
                pExtLinkNode->ulFlags);
            return SR_SUCCESS;
        }
    }

    /* allocate node to store adj sid data */
    pAdjSidInfo = SrCreateAdjSidInfoNode();
    if (NULL == pAdjSidInfo)
    {
        SR_TRC2 (SR_FAIL_TRC | SR_CTRL_TRC, "%s:%d : pAdjSidInfo == NULL \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    /* Add Adj Sid node to list, as list has multiple adj sids for broadcast/lan
       interfaces or multiple sids can be configured on same interface
       with different flags */
    TMO_SLL_Add(&(pExtLinkNode->adjSidSubTlvList),
                    &(pAdjSidInfo->nextAdjSid));
    pAdjSidInfo->u4Label = pSrSidEntry->u4PrefixSidLabelIndex;
    pAdjSidInfo->u1Flags = pSrSidEntry->u1Flags;

    if (0 == u4AdjSidTlvCount)
    {
        /* Originaitng newly as this Adj SID is the first TLV for the LSA */
        pExtLinkNode->ulFlags |= OSPF_EXT_LINK_INFO_NEW;
    }
    else
    {
        /* set flag to originate Extended Link LSA for newer instance as
           LSA is already originated */
        pExtLinkNode->ulFlags |= OSPF_EXT_LINK_INFO_ORG;
    }

    SR_TRC5 (SR_CTRL_TRC, "%s:%d : Added AdjSid Node (SID: %d F:0x%x LNF:0x%x) to Sub Tlv list \n",
                __func__, __LINE__, pAdjSidInfo->u4Label, pAdjSidInfo->u1Flags,
                pExtLinkNode->ulFlags);

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrProcessDelAdjTlvtoLinkInfo
 * Description   : This routine to process deleting Adj Tlv to Link Info
 * Input(s)      : pExtLinkNode      - pointer to Link info
                 : pSrSidEntry       - SID Info
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
 *****************************************************************************/
PUBLIC UINT1
SrProcessDelAdjTlvtoLinkInfo(tSrExtLinkLSAInfo   *pExtLinkNode,
                            tSrSidInterfaceInfo  *pSrSidEntry)
{
    tAdjSidNode     *pAdjSidInfo      = NULL;
    tTMO_SLL_NODE   *pAdjSidSllNode   = NULL;
    UINT4            u4AdjSidTlvCount = 0;
    UINT1            u1AdjFound       = SR_FALSE;

    /* get the count of Adj Sid Tlvs */
    u4AdjSidTlvCount = TMO_SLL_Count (&(pExtLinkNode->adjSidSubTlvList));

    /* if exisring count is zero means we did not added this SID info and
        LSA is not generated, add as failed log and return */
    if (0 == u4AdjSidTlvCount)
    {
        return SR_SUCCESS;
    }

    TMO_SLL_Scan (&(pExtLinkNode->adjSidSubTlvList), pAdjSidSllNode, tTMO_SLL_NODE *)
    {
       pAdjSidInfo = (tAdjSidNode *) pAdjSidSllNode;
       if (pAdjSidInfo->u4Label == pSrSidEntry->u4PrefixSidLabelIndex)
       {
           u1AdjFound = SR_TRUE;
           break;
       }
    }

    if (SR_TRUE == u1AdjFound)
    {
        if (1 == u4AdjSidTlvCount)
        {
            /* Flush the LSA and deleted Adj SID Tlv later after LSA Flush
               but keep the link Info as only Sid config is deleted */
            pExtLinkNode->ulFlags |= OSPF_EXT_LINK_INFO_FLUSH;
        }
        else
        {
            /* Delete the Adj SID info */
            TMO_SLL_Delete (&(pExtLinkNode->adjSidSubTlvList),
                        &(pAdjSidInfo->nextAdjSid));

            SR_PEER_ADJ_SID_MEM_FREE(pAdjSidInfo);

            /* set flag to originate Extended Link LSA for newer instance as
               with remaining Adj Sids */
            pExtLinkNode->ulFlags |= OSPF_EXT_LINK_INFO_ORG;
        }
    }

    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrProcessFlushLsaExtLinkInfo
 * Description   : This routine to delete All Adj SID Info from Link Info
 * Input(s)      : pExtLinkNode      - pointer to Link info
 * Output(s)     : None
 * Return(s)     : None
 *****************************************************************************/
VOID
SrProcessFlushLsaExtLinkInfo(tSrExtLinkLSAInfo  *pExtLinkNode)
{
    tAdjSidNode         *pAdjSidNode      = NULL;
    tAdjSidNode         *pAdjSidNextNode  = NULL;

    /* Delete all Adj Links as LSA is Flushed */
    TMO_DYN_SLL_Scan (&(pExtLinkNode->adjSidSubTlvList), pAdjSidNode,
                    pAdjSidNextNode, tAdjSidNode *)
    {
        TMO_SLL_Delete (&(pExtLinkNode->adjSidSubTlvList),
                        &(pAdjSidNode->nextAdjSid));
        SR_PEER_ADJ_SID_MEM_FREE(pAdjSidNode);
    }

    /* reset flush flag */
    pExtLinkNode->ulFlags &= ~(OSPF_EXT_LINK_INFO_FLUSH);

    /* reset LSA ID */
    pExtLinkNode->u4LsaId = 0;

    return;
}

/*****************************************************************************
 * Function Name : SrProcessAdjSidChange
 * Description   : This routine to process Add/Del Adj SID config
 * Input(s)      : pSrSidInfo        - pointer to SID info
                 : pOspfNbr          - pointer to Ospf Nbr on that Intf
 * Output(s)     : None
 * Return(s)     : pointer to Current buffer
 *****************************************************************************/
PUBLIC UINT1
SrProcessAdjSidChange(tSrSidInterfaceInfo * pSrSidInfo,
                      tSrV4OspfNbrInfo    *pOspfNbr)
{
    tareaLinkInfo      *pSrAreaLinkInfo  = NULL;
    tTMO_SLL_NODE      *pAreaNode        = NULL;
    tSrExtLinkLSAInfo  *pExtLinkNode     = NULL;
    tSrLinkInfoKey      LinkNode;
    tSrExtLinkLSAInfo   ExtLinkNode;
    UINT1               u1AreaFound      = SR_FALSE;
    UINT1               ulRet            = SR_SUCCESS;

    MEMSET (&LinkNode, SR_ZERO, sizeof (tSrLinkInfoKey));
    MEMSET (&ExtLinkNode, SR_ZERO, sizeof (tSrExtLinkLSAInfo));

    /* search for area node to add router links to generate Ext Link LSAs
    for the links in that area if Adj SID is configured */
    if (TMO_SLL_Count (&(gSrGlobalInfo.linkInfoList)) != 0)
    {
       TMO_SLL_Scan (&(gSrGlobalInfo.linkInfoList), pAreaNode, tTMO_SLL_NODE *)
       {
           pSrAreaLinkInfo = (tareaLinkInfo *) pAreaNode;
           if (pSrAreaLinkInfo->u4AreaId == pSrSidInfo->u4AreaId)
           {
               u1AreaFound = SR_TRUE;
               break;
           }
       }
    }

    if (SR_FALSE == u1AreaFound)
    {
        SR_TRC3 (SR_CTRL_TRC,
            "%s:%d Area node not found for Area Id %x",
            __func__, __LINE__, pSrSidInfo->u4AreaId);
        return ulRet;
    }

    SR_TRC4 (SR_CTRL_TRC,
            "%s:%d Found the area node (0x%x) for Adj SID (%d) \n",
            __func__, __LINE__, pSrAreaLinkInfo->u4AreaId,
            pSrSidInfo->u4PrefixSidLabelIndex);

    /* create key to search for router Link info node in Area link info tree */
    ExtLinkNode.LinkInfo.u1LinkType = OSPF_LINK_TYPE_P2P;
    ExtLinkNode.LinkInfo.u4LinkId   = pOspfNbr->ospfV4NbrInfo.u4NbrRtrId;
    ExtLinkNode.LinkInfo.u4LinkData = pOspfNbr->u4IfIpAddr;

    /* Search for router link in exisitng links */
    pExtLinkNode = RBTreeGet(pSrAreaLinkInfo->pExtLinkLsaInfoTree,
                           (tRBElem *) &ExtLinkNode);
    if (NULL == pExtLinkNode)
    {
     SR_TRC5 (SR_CTRL_TRC,
            "%s:%d Ext Link node not found for(LinkType:%x ID:0x%x Data0x%x)",
            __func__, __LINE__, ExtLinkNode.LinkInfo.u1LinkType,
            ExtLinkNode.LinkInfo.u4LinkId,
            ExtLinkNode.LinkInfo.u4LinkData);
        return ulRet;
    }

    SR_TRC5 (SR_CTRL_TRC,
            "%s:%d Found the Ext Link node (ID:0x%x Data0x%x) for Adj SID (%d) \n",
            __func__, __LINE__, pExtLinkNode->LinkInfo.u4LinkId,
            pExtLinkNode->LinkInfo.u4LinkData,
            pSrSidInfo->u4PrefixSidLabelIndex);

    switch(pSrSidInfo->u1AdjSidOprFlags)
    {
        case SR_ADJ_SID_OP_CFG_ADD:
        case SR_ADJ_SID_OP_ADD_LSA:
            ulRet = SrProcessAddAdjTlvtoLinkInfo(pExtLinkNode, pSrSidInfo);
            break;
        case SR_ADJ_SID_OP_CFG_DEL:
            ulRet = SrProcessDelAdjTlvtoLinkInfo(pExtLinkNode, pSrSidInfo);
            break;
        case SR_ADJ_SID_OP_DEL_LSA:
            pExtLinkNode->ulFlags |= OSPF_EXT_LINK_INFO_FLUSH;
            break;
        default:
            return SR_FAILURE;
    }

    SR_TRC4 (SR_CTRL_TRC,
            "%s:%d Ext Link Info Node Flags 0x%x for Adj SID (%d) \n",
            __func__, __LINE__, pExtLinkNode->ulFlags,
            pSrSidInfo->u4PrefixSidLabelIndex);

    if (0 != pExtLinkNode->ulFlags)
    {
        ulRet = SrProcessExtLinkInfoUpdate(pExtLinkNode,
                                    pSrAreaLinkInfo->u4AreaId);
        if (pExtLinkNode->ulFlags & OSPF_EXT_LINK_INFO_FLUSH)
        {
           /* Delete the Adj Sid Info but Ext Link will be present */
           SrProcessFlushLsaExtLinkInfo(pExtLinkNode);
        }

        if (pExtLinkNode->ulFlags & OSPF_EXT_LINK_INFO_STALE)
        {
           /* Delete Extended Link info along with Adj SID info */
           SrDeleteExtLinkInfoNode(pSrAreaLinkInfo, pExtLinkNode);
        }
    }

    return ulRet;

}

PUBLIC UINT4
SrProcessNetworkLsa (tOsToOpqApp * pOspfMsg, tOsSrArea * pOsSrArea)
{
    UNUSED_PARAM (pOspfMsg);
    UNUSED_PARAM (pOsSrArea);
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrProcessRecvAdjSidInfo
 * Description   : This routine to process recv Adj SID info from Nbr Node
 * Input(s)      :
 *               : pRtrInfo         - pointer to SR router Info
 *                 pRecvAdjSid      - pointer to Adj Sid Info
 *                 u1LsaStatus      - Flush/Update
 * Output(s)     : None
 * Return(s)     : None
 *****************************************************************************/
VOID
SrProcessRecvAdjSidInfo(tSrRtrInfo *pRtrInfo, tAdjSidNode *pRecvAdjSid,
                        UINT1 u1LsaStatus)
{
    tTMO_SLL_NODE      *pAdjSid        = NULL;
    tAdjSidNode        *pAdjSidInfo    = NULL;
    UINT1               isAdjExist     = SR_FALSE;

    TMO_SLL_Scan (&(pRtrInfo->adjSidList), pAdjSid, tTMO_SLL_NODE *)
    {
        pAdjSidInfo = (tAdjSidNode *) pAdjSid;

        if ((pAdjSidInfo->u1LinkType == pRecvAdjSid->u1LinkType)
            && (pAdjSidInfo->u4LinkId == pRecvAdjSid->u4LinkId)
            && (pAdjSidInfo->linkIpAddr.u2AddrType == pRecvAdjSid->linkIpAddr.u2AddrType)
            && (pAdjSidInfo->linkIpAddr.Addr.u4Addr == pRecvAdjSid->linkIpAddr.Addr.u4Addr)
            && (pAdjSidInfo->u4NbrRtrId == pRecvAdjSid->u4NbrRtrId))
        {
            isAdjExist = SR_TRUE;
            break;
        }
    }

    if ((u1LsaStatus == FLUSHED_LSA) && (SR_FALSE == isAdjExist))
    {
        SR_TRC3 (SR_CTRL_TRC, "%s:%d Recv Flush for non exist Adj SID Info from Rtr %x\n",
                __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr);

        return;
    }

    if (u1LsaStatus == FLUSHED_LSA)
    {
        TMO_SLL_Delete (&(pRtrInfo->adjSidList), &(pAdjSidInfo->nextAdjSid));
        SR_TRC3 (SR_MAIN_TRC, "%s:%d Deleted Adj SID Info from Rtr node %x\n",
                __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr);
        SrTiLfaProcessForSrRouterAdjDel(pAdjSidInfo, pRtrInfo->u4areaId);
        SR_PEER_ADJ_SID_MEM_FREE(pAdjSidInfo);

        if (TMO_SLL_Count (&(pRtrInfo->adjSidList)) == SR_ZERO)
        {
            if (!(pRtrInfo->u1RtrSrAttrFlags & SR_PREFIX_SID_PRESENT))
            {
                TMO_SLL_Delete (&(gSrGlobalInfo.routerList),
                                (tTMO_SLL_NODE *) pRtrInfo);
                SR_RTR_LIST_FREE (pRtrInfo);
            }
        }

        return;
    }

    if (SR_FALSE == isAdjExist)
    {
        if (SR_PEER_ADJ_SID_MEM_ALLOC (pAdjSidInfo) == NULL)
        {
            SR_TRC (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                    "Memory Allocation Failure for Adjacency SID \n");
            return;
        }

        pAdjSidInfo->u1LinkType = pRecvAdjSid->u1LinkType;
        pAdjSidInfo->u4LinkId   = pRecvAdjSid->u4LinkId;
        pAdjSidInfo->u4NbrRtrId = pRecvAdjSid->u4NbrRtrId;
        pAdjSidInfo->u4Label    = pRecvAdjSid->u4Label;
        pAdjSidInfo->u1Flags    = pRecvAdjSid->u1Flags;
        pAdjSidInfo->u1MTID     = pRecvAdjSid->u1MTID;
        pAdjSidInfo->u1Weight   = pRecvAdjSid->u1Weight;

        pAdjSidInfo->linkIpAddr.u2AddrType  = pRecvAdjSid->linkIpAddr.u2AddrType;
        pAdjSidInfo->linkIpAddr.Addr.u4Addr = pRecvAdjSid->linkIpAddr.Addr.u4Addr;
        TMO_SLL_Insert (&(pRtrInfo->adjSidList), NULL, &(pAdjSidInfo->nextAdjSid));
        SR_TRC3 (SR_MAIN_TRC, "%s:%d Inserted Adj SID Info to Rtr node %x\n",
                __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr);
        SrTiLfaProcessForSrRouterAdjChange(pRtrInfo, pAdjSidInfo);
        return;
    }

    if (pAdjSidInfo->u4Label != pRecvAdjSid->u4Label)
    {
        pAdjSidInfo->u4Label    = pRecvAdjSid->u4Label;
        SrTiLfaProcessForSrRouterAdjChange(pRtrInfo, pAdjSidInfo);
    }

    /* Currenlty Adj SID is used for TILFA, in which label is used and change
       in MTID and Weight will not required and flags for Label is moslty Absolute */
    pAdjSidInfo->u1Flags    = pRecvAdjSid->u1Flags;
    pAdjSidInfo->u1MTID     = pRecvAdjSid->u1MTID;
    pAdjSidInfo->u1Weight   = pRecvAdjSid->u1Weight;

    SR_TRC3 (SR_MAIN_TRC, "%s:%d Updated Adj SID Info to Rtr node %x\n",
                __func__, __LINE__, pRtrInfo->advRtrId.Addr.u4Addr);

    return;
}

/*****************************************************************************
 * Function Name : SrProcessOpqExtLinkAdjSidSubTlv
 * Description   : This routine to process recv Adj SID Sub TLV
 * Input(s)      : ptlvPtr          - pointer to Current buffer
 *               : pAdjSidInfo      - pointer to Adj Sid Info
 *                 u2SubTlvType     - Adj Type
 * Output(s)     : pointer to Current buffer
 * Return(s)     : None
 *****************************************************************************/
VOID
SrProcessOpqExtLinkAdjSidSubTlv(UINT1   **ptlvPtr, tAdjSidNode *pAdjSidInfo,
                                UINT2 u2SubTlvType)
{
    UINT1   *tlvPtr            = *ptlvPtr;
    UINT1   ulReserved         = SR_ZERO;

    pAdjSidInfo->u1Flags = SR_LGET1BYTE (tlvPtr);
    /* reserved byte */
    ulReserved = SR_LGET1BYTE (tlvPtr);
    /* MT-ID */
    pAdjSidInfo->u1MTID = SR_LGET1BYTE (tlvPtr);

    /* Weight */
    pAdjSidInfo->u1Weight  = SR_LGET1BYTE (tlvPtr);

    if (LAN_ADJ_SID_TLV == u2SubTlvType)
    {
        pAdjSidInfo->u4NbrRtrId   = SR_LGET4BYTE (tlvPtr);
    }

    if (pAdjSidInfo->u1Flags & ADJ_SID_V_FLAG)
    {
        pAdjSidInfo->u4Label   = SR_LGET3BYTE (tlvPtr);
        ulReserved             = SR_LGET1BYTE (tlvPtr);
    }
    else
    {
        pAdjSidInfo->u4Label   = SR_LGET4BYTE (tlvPtr);
    }

    *ptlvPtr = tlvPtr;

    return;
}

/*****************************************************************************
 * Function Name : SrProcessOpqExtLinkLsa
 * Description   : This routine to process recv Ext Link LSA
 * Input(s)      : pOsLsaInfo       - pointer to LSA data *
                   pRtrInfo         - pointer to SR router Info

 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
*****************************************************************************/
PUBLIC UINT4
SrProcessOpqExtLinkLsa (tOsLsaInfo *pOsLsaInfo, tSrRtrInfo *pRtrInfo)
{
    tAdjSidNode   AdjSidInfoTemp;

    UINT1   *tlvPtr            = NULL;
    UINT1   *tlvTemp           = NULL;
    UINT4   u4OpqLsaLen        = SR_ZERO;
    UINT4   u4LsaProcessed     = SR_ZERO;
    UINT4   u4Temp             = SR_ZERO;
    UINT2   u2TlvType          = SR_ZERO;
    UINT2   u2TlvLen           = SR_ZERO;
    UINT2   u2SubTlvType       = SR_ZERO;
    UINT2   u2SubTlvLen        = SR_ZERO;

    MEMSET (&AdjSidInfoTemp, SR_ZERO, sizeof (tAdjSidNode));

    tlvPtr = pOsLsaInfo->pu1Lsa + OSPF_LSA_HEADER_LENGTH;    /* 20 bytes lsheader */
    u4OpqLsaLen = (UINT4) (pOsLsaInfo->u2LsaLen - OSPF_LSA_HEADER_LENGTH);

    while (u4LsaProcessed < u4OpqLsaLen)
    {
        tlvTemp   = tlvPtr;
        u2TlvType = SR_LGET2BYTE (tlvPtr);
        u2TlvLen  = SR_LGET2BYTE (tlvPtr);
        if (u2TlvType == OSPF_EXT_LINK_TLV_TYPE)
        {
            if (u2TlvLen < OSPF_EXT_LINK_TLV_FIXED_LENGTH)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                    "EXT LINK TLV length is less than required for parsing\n");
                return SR_FAILURE;
            }

            /* Link Type */
            AdjSidInfoTemp.u1LinkType               = SR_LGET1BYTE (tlvPtr);
            /* reserved bytes */
            u4Temp                                  = SR_LGET3BYTE (tlvPtr);
            /* Link Id */
            AdjSidInfoTemp.u4LinkId                 = SR_LGET4BYTE (tlvPtr);
            /* Link Data */
            AdjSidInfoTemp.linkIpAddr.Addr.u4Addr   = SR_LGET4BYTE (tlvPtr);

            u2SubTlvType = SR_LGET2BYTE (tlvPtr);
            u2SubTlvLen  = SR_LGET2BYTE (tlvPtr);
            switch (u2SubTlvType)
            {
                case ADJ_SID_TLV:
                case LAN_ADJ_SID_TLV:
                    if (u2SubTlvLen < ADJ_SID_LABEL_SUB_TLV_LEN)
                    {
                        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                            "Adj SID SUB TLV length is less than required for parsing\n");
                        return SR_FAILURE;
                    }

                    SrProcessOpqExtLinkAdjSidSubTlv(&tlvPtr, &AdjSidInfoTemp,
                                                    u2SubTlvType);
                    if (OSPF_LINK_TYPE_P2P == AdjSidInfoTemp.u1LinkType)
                    {
                        AdjSidInfoTemp.u4NbrRtrId = AdjSidInfoTemp.u4LinkId;
                    }

                    SR_TRC3 (SR_CTRL_TRC, "%s:%d Recv Adj SID Info from Rtr %x\n",
                             __func__, __LINE__, pOsLsaInfo->u4AdvRtrId);
                    SR_TRC6 (SR_CTRL_TRC, "Link Type %d Link Id %x Link Data %x "
                            "Nbr ID %x Flags %x Label %d \n",
                             AdjSidInfoTemp.u1LinkType, AdjSidInfoTemp.u4LinkId,
                             AdjSidInfoTemp.linkIpAddr.Addr.u4Addr,
                             AdjSidInfoTemp.u4NbrRtrId, AdjSidInfoTemp.u1Flags,
                             AdjSidInfoTemp.u4Label);

                    SrProcessRecvAdjSidInfo(pRtrInfo, &AdjSidInfoTemp,
                                            pOsLsaInfo->u1LsaStatus);
                    break;

                    default: /* Non supported Sub TLV type */
                        tlvPtr += u2SubTlvLen;
                        break;
            }
        }
        else /* Non supported TLV type */
        {
            tlvPtr += u2TlvLen;
        }

        u4LsaProcessed += (tlvPtr-tlvTemp);
    }

    return SR_SUCCESS;

}

/*****************************************************************************
 * Function Name : SrProcessFlushRtrInfoLsa
 * Description   : This routine to process recv RtrInfo Flush LSA
 * Input(s)      : u4AdvRtrId    - Advertising routerId
                   u4OpqAreaId   - Area Id

 * Output(s)     : None
 * Return(s)     : None
*****************************************************************************/
PUBLIC VOID
SrProcessFlushRtrInfoLsa (UINT4 u4AdvRtrId, UINT4 u4OpqAreaId)
{
    tSrOsRiLsInfo       SrOsRiLsInfo;
    tSrOsRiLsInfo      *pSrOsRiLsNode = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tGenU4Addr          tmpAddr;

    MEMSET (&SrOsRiLsInfo, SR_ZERO, sizeof (tSrOsRiLsInfo));

    SrOsRiLsInfo.advRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;
    SrOsRiLsInfo.advRtrId.Addr.u4Addr = u4AdvRtrId;

    SR_TRC4 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                 "%s:%d Received RILSA flush for Rtr %x in Area %x \n",
                 __func__, __LINE__, u4AdvRtrId, u4OpqAreaId);

    pSrOsRiLsNode =
        RBTreeGet (gSrGlobalInfo.pSrOsRiLsRbTree, &SrOsRiLsInfo);

    if (pSrOsRiLsNode != NULL)
    {
        SR_TRC3 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                 "%s:%d Removing RI LsaInfo for advRtr %x from RBTree\n",
                 __func__, __LINE__, u4AdvRtrId);
        RBTreeRem (gSrGlobalInfo.pSrOsRiLsRbTree, pSrOsRiLsNode);
        SR_OSPF_RI_LSA_MEM_FREE (pSrOsRiLsNode);
    }

    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;

        if ((pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE)
            && (u4AdvRtrId == pRtrInfo->advRtrId.Addr.u4Addr)
            && (u4OpqAreaId == pRtrInfo->u4areaId))
        {
            if (SR_FAILURE == SrDeleteMplsEntry (pRtrInfo))
            {
                SR_TRC4 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                         "%s:%d Unable to Delete MPLS Entry for prefix %x, "
                         "AdvRtrId %x\n", __func__, __LINE__,
                         pRtrInfo->prefixId.Addr.u4Addr, u4AdvRtrId);
                return SR_FAILURE;
            }

            SR_TRC4 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                "%s:%d Deleted MPLS entry for the advRtr %x dest %x\n",
                __func__, __LINE__, u4AdvRtrId, pRtrInfo->prefixId.Addr.u4Addr);

            MEMSET (&tmpAddr, 0, sizeof (tGenU4Addr));
            tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
            tmpAddr.Addr.u4Addr = pRtrInfo->prefixId.Addr.u4Addr;

            /* Multi-SID scenario, Nbr should not be treat as DOWN
             * as there can be other SID exists on Nbr.
             */
            if (SrUtilCheckNbrIsMultiSid (&tmpAddr, u4AdvRtrId) == SR_FAILURE)
            {
                if (SrDeleteMplsBindingOnNbrSrDown (&tmpAddr, u4AdvRtrId) == SR_FAILURE)
                {
                    SR_TRC4 (SR_CTRL_TRC,
                             "%s:%d RouterId %x AdvRtr %x is not Neighbour of any other node\n",
                             __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr,
                             u4AdvRtrId);
                }
            }
        }
    }

    return;
}

/*****************************************************************************
 * Function Name : SrProcessFlushExtPrefixOpqLsa
 * Description   : This routine to process recv Ext prefix SID Flush LSA
 * Input(s)      : pRtrInfo - pointer to SR router Info

 * Output(s)     : None
 * Return(s)     : SR_SUCCESS/SR_FAILURE
*****************************************************************************/
PUBLIC UINT4
SrProcessFlushExtPrefixOpqLsa (tSrRtrInfo *pRtrInfo)
{
    tGenU4Addr     tmpAddr;
    UINT4          u4DestAddr = SR_ZERO;
    UINT4          u4DelAdvRtrId = SR_ZERO;
    UINT4          u4DelRtrSidVal = SR_ZERO;
    UINT4          u4ErrCode = SR_ZERO;

    if (pRtrInfo == NULL)
    {
        return SR_FAILURE;
    }

    u4DestAddr = pRtrInfo->prefixId.Addr.u4Addr;
    u4DelAdvRtrId = pRtrInfo->advRtrId.Addr.u4Addr;
    u4DelRtrSidVal = pRtrInfo->u4SidValue;

    MEMSET (&tmpAddr, 0, sizeof (tGenU4Addr));
    tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
    tmpAddr.Addr.u4Addr = u4DestAddr;

    /* To delete TE LSP in Type10 flow */
    if (SrTeDeleteFtnType10 (&tmpAddr, &u4ErrCode) == SR_SUCCESS)
    {
        if (u4ErrCode == SR_TE_VIA_DEST)
        {
            SR_TRC4 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                     "%s:%d SR TE LSP deleted via dest in Type10 flow for"
                     " prefix %x, AdvRtrId %x\n",
                     __func__, __LINE__, u4DestAddr, u4DelAdvRtrId);
        }
    }
    /* Sr-TE: End */
    if (u4ErrCode != SR_TE_VIA_DEST)
    {
        if (SR_FAILURE == SrDeleteMplsEntry (pRtrInfo))
        {
            SR_TRC4 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                     "%s:%d Unable to Delete MPLS Entry for prefix %x, "
                     "AdvRtrId %x\n", __func__, __LINE__, u4DestAddr, u4DelAdvRtrId);
            return SR_FAILURE;
        }
    }

    if (gSrGlobalInfo.u1OspfAbrStatus == SR_TRUE)
    {
        /* Send flush for the LSA that SR originated when a Type10 LSA was received */
        SrConstructSendExtPrefixOpqLsa (FLUSHED_LSA, pRtrInfo->u4SidValue,
                                        pRtrInfo->prefixId.Addr.u4Addr,
                                        pRtrInfo->u4areaId, SR_TWO);
    }

    /* Multi-SID scenario, Nbr should not be treat as DOWN
     * as there can be other SID exists on Nbr.
     */
    if (SrUtilCheckNbrIsMultiSid (&tmpAddr, u4DelAdvRtrId) == SR_FAILURE)
    {
        if (SrDeleteMplsBindingOnNbrSrDown (&tmpAddr, u4DelAdvRtrId) == SR_FAILURE)
        {
            SR_TRC4 (SR_CTRL_TRC,
                     "%s:%d RouterId %x AdvRtr %x is not Neighbour of any other node\n",
                     __func__, __LINE__, u4DestAddr, u4DelAdvRtrId);
        }
    }

    if (pRtrInfo->u1SIDConflictMPLSDel == SR_TRUE)
    {
        pRtrInfo->u1SIDConflictMPLSDel = SR_FALSE;
        SR_TRC4 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                 "%s:%d Resetting SIDConflictMPLSDel for PrefixId %x, AdvRtrId %x\n",
                 __func__, __LINE__, pRtrInfo->prefixId.Addr.u4Addr, u4DelAdvRtrId);
    }

    if (pRtrInfo->u1SIDConflictAlarm == SR_ALARM_RAISE)
    {
        SrGenerateAlarm (pRtrInfo, SR_ALARM_CLEAR, SID_CONFLICT_ALARM);
    }

    if (pRtrInfo->u1PrefixConflictMPLSDel == SR_TRUE)
    {
        pRtrInfo->u1PrefixConflictMPLSDel = SR_FALSE;
        SR_TRC4 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                 "%s:%d Resetting PrefixConflictMPLSDel for Rtr %x, advRtr %x\n",
                 __func__, __LINE__, u4DestAddr, u4DelAdvRtrId);
    }

    if (pRtrInfo->u1PrefixConflictAlarm == SR_ALARM_RAISE)
    {
        SrGenerateAlarm (pRtrInfo, SR_ALARM_CLEAR, PREFIX_CONFLICT_ALARM);
    }

    if (pRtrInfo->u1OutOfRangeAlarm == SR_ALARM_RAISE)
    {
        SrGenerateAlarm (pRtrInfo, SR_ALARM_CLEAR, OOR_SID_ALARM);
    }

    pRtrInfo->u1RtrSrAttrFlags &= ~SR_PREFIX_SID_PRESENT;

    if (TMO_SLL_Count (&(pRtrInfo->adjSidList)) == SR_ZERO)
    {
        SR_TRC4 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                 "%s:%d Deleting Prefix %x, AdvRtr %x from PeerList\n",
                 __func__, __LINE__, u4DestAddr, u4DelAdvRtrId);
        TMO_SLL_Delete (&(gSrGlobalInfo.routerList),
                        (tTMO_SLL_NODE *) pRtrInfo);
        SR_RTR_LIST_FREE (pRtrInfo);
    }

    SrSidChkAndCreateEntriesForNxtRtr (u4DelRtrSidVal, u4DelAdvRtrId);
    SrPrefixChkAndCreateEntriesForNxtRtr (u4DestAddr, u4DelAdvRtrId);

    return SR_SUCCESS;
}

PUBLIC UINT4
SrProcessOpqType10Lsa (tOsToOpqApp * pOspfMsg, tOsSrArea * pOsSrArea)
{
    tOsLsaInfo         *pOsLsaInfo = NULL;
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRtrInfo         *pRtrInfo1 = NULL;
    tSrRtrInfo         *pNewRtrNode = NULL;
    tOpqLSAInfo         opqLSAInfo;
    tOpqLSAInfo         opqLSAInfo1;
    tIPADDR             adjlinkAddr = { 0 };
    tRouterId           tmpRtrId;
    tRouterId           tmpLinkIp;
    tGenU4Addr          tmpAddr;
    tGenU4Addr          dummyAddr;
    tSrOsRiLsInfo       SrOsRiLsInfo;
    tSrOsRiLsInfo      *pSrOsRiLsNode = NULL;
    tSrOsRiLsInfo      *pSrOsRiLsNode1 = NULL;

    UINT4               u4AddrlessIf = SR_ZERO;
    UINT4               u4sidVal = SR_ZERO;
    UINT4               u4sumTlvLen = SR_ZERO;
    UINT4               u4TtlvLenCount = SR_ZERO;
    UINT2               u2ExtTlvLen = SR_ZERO;
    UINT4               u4MinSrgbRange = SR_ZERO;
    UINT4               u4MaxSrgbRange = SR_ZERO;
    UINT4               u4SrgbRangeSize = SR_ZERO;
    UINT4               u4AlgoTlvLen = SR_ZERO;
    UINT4               u4AlgoTlvCounter = SR_ZERO;
    UINT4               u4SidLabelRangeTlvLen = SR_ZERO;
    UINT4               u4ErrCode = SR_ZERO;
    UINT4               u4DestAddr = SR_ZERO;
    UINT4               u4NextHop = SR_ZERO;
    UINT4               u4Temp = SR_ZERO;
    UINT4               u4OpqAreaId = SR_ZERO;
    UINT4               u4linkIpAddr = SR_ZERO;
    UINT4               u4PrefixAddr = SR_ZERO;
    UINT4               u4SelfAdvRtrId = SR_ZERO;
    UINT4               u4AdvRtrId = SR_ZERO;
    UINT4               u4TempRtrId = SR_ZERO;
    UINT4               u4AddressPrefix = SR_ZERO;
    UINT4               u1LSASent = SR_TRUE;
    UINT4               u4DelRtrSidVal = SR_ZERO;
    UINT4               u4DelAdvRtrId = SR_ZERO;
    INT4                i4AbrStatus = SR_FALSE;

    UINT2               u2Temp = SR_ZERO;
    UINT1              *tlvPtr = NULL;
    UINT1               isPeerExist = SR_FALSE;
    UINT1               au1RtrAddrTlv[SR_RI_LSA_TLV_LEN];
    UINT1               au1RtrAddrTlv1[SR_RI_LSA_TLV_LEN];
    UINT1              *pTemp = NULL;
    UINT1               u1Temp = SR_ZERO;
    UINT1               u1FtnCreated = SR_ZERO;
    UINT1               u1OpqType = SR_ZERO;
    tOpqLSAInfo         opqDelLsa;
    tOpqLSAInfo         opqDelRtrInfoLsa;

    UNUSED_PARAM (pOsSrArea);
    MEMSET (&tmpAddr, 0, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, 0, sizeof (tGenU4Addr));
    MEMSET (&opqLSAInfo, 0, sizeof (tOpqLSAInfo));
    MEMSET (&opqLSAInfo1, 0, sizeof (tOpqLSAInfo));
    MEMSET (au1RtrAddrTlv, 0, sizeof (SR_RI_LSA_TLV_LEN));
    MEMSET (&opqDelLsa, 0, sizeof (tOpqLSAInfo));
    MEMSET (&opqDelRtrInfoLsa, 0, sizeof (tOpqLSAInfo));
    MEMSET (tmpRtrId, 0, sizeof (tRouterId));
    MEMSET (tmpLinkIp, 0, sizeof (tRouterId));
    opqLSAInfo.pu1LSA = au1RtrAddrTlv;
    opqLSAInfo1.pu1LSA = au1RtrAddrTlv1;
    opqDelLsa.pu1LSA = au1RtrAddrTlv;
    opqDelRtrInfoLsa.pu1LSA = au1RtrAddrTlv;

    pOsLsaInfo = &(pOspfMsg->lsaInfo);
    if (gSrGlobalInfo.u1OspfAbrStatus == SR_TRUE)
    {
        i4AbrStatus = SR_TRUE;
    }

    u4SelfAdvRtrId = gSrGlobalInfo.u4Ospfv2RtrID;
    u4AdvRtrId = pOsLsaInfo->u4AdvRtrId;
    u4OpqAreaId = pOspfMsg->u4AreaId;

    tlvPtr = pOsLsaInfo->pu1Lsa;
    pTemp = tlvPtr + 4;
    u1Temp = SR_LGET1BYTE (pTemp);
    u1OpqType = u1Temp;

    if (u1OpqType == OSPF_EXT_PRREFIX_LSA_OPAQUE_TYPE)
    {
        u4PrefixAddr = SrGetAddrPrefix (pOsLsaInfo);
        u4PrefixAddr = OSIX_HTONL (u4PrefixAddr);
    }

    /* If SR Degregistation is going to happen, OSPF packets should not be processed */
    if ((u4SelfAdvRtrId != u4AdvRtrId)
        && (gSrGlobalInfo.u1OspfDeRegInProgress == SR_TRUE))
    {
        return SR_SUCCESS;
    }

    if ((u4SelfAdvRtrId == u4AdvRtrId)
        || (u4SelfAdvRtrId == 0))
    {
        SR_TRC3 (SR_CTRL_TRC, "%s:%d Self LSA Recv %x, Returning\n", __func__,
                 __LINE__, pOsLsaInfo->u4AdvRtrId);
        return SR_SUCCESS;
    }
    else
    {
        if (u1OpqType == OSPF_EXT_PRREFIX_LSA_OPAQUE_TYPE)
        {
            SR_TRC5 (SR_CTRL_TRC, "%s:%d Ext Prefix(7) LSA Recv from AdvRtrId %x Area Id %x LsaStatus %d\n", __func__,
               __LINE__, pOsLsaInfo->u4AdvRtrId, u4OpqAreaId, pOsLsaInfo->u1LsaStatus);
        }
        else if(u1OpqType == RI_LSA_OPAQUE_TYPE)
        {
                SR_TRC5 (SR_CTRL_TRC, "%s:%d RI(4) LSA Recv from AdvRtrId %x Area Id %x LsaStatus %d\n", __func__,
                   __LINE__, pOsLsaInfo->u4AdvRtrId, u4OpqAreaId, pOsLsaInfo->u1LsaStatus);
        }
        else if(u1OpqType == OSPF_EXT_LINK_LSA_OPAQUE_TYPE)
        {
                SR_TRC5 (SR_CTRL_TRC, "%s:%d Ext Link(8) LSA Recv from AdvRtrId %x Area Id %x LsaStatus %d\n", __func__,
                   __LINE__, pOsLsaInfo->u4AdvRtrId, u4OpqAreaId, pOsLsaInfo->u1LsaStatus);
        }
        else
        {
            SR_TRC5 (SR_CTRL_TRC, "%s:%d Invalid LSA Recv from AdvRtrId %x Area Id %x LsaStatus %d\n", __func__,
             __LINE__, pOsLsaInfo->u4AdvRtrId, u4OpqAreaId, pOsLsaInfo->u1LsaStatus);
        }

    }

    if (u4PrefixAddr != SR_ZERO)
    {
        pRtrInfo = NULL;
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            pRtrInfo = (tSrRtrInfo *) pRtrNode;
            MEMCPY (&u4TempRtrId, &pRtrInfo->prefixId.Addr.u4Addr, sizeof (UINT4));
            if ((u4PrefixAddr == u4TempRtrId) &&
                (pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE) &&
                (u4AdvRtrId == pRtrInfo->advRtrId.Addr.u4Addr))
            {
                SR_TRC4 (SR_CTRL_TRC, "%s:%d SR node found with the Adv Rtr Id %x for the prefix ID %x\n",
                         __func__, __LINE__,u4AdvRtrId, u4PrefixAddr);
                MEMCPY (&u4DestAddr, &pRtrInfo->prefixId.Addr.u4Addr,
                        sizeof (UINT4));
                isPeerExist = SR_TRUE;
                pNewRtrNode = pRtrInfo;
                break;
            }
        }
    }
    else
    {
        pRtrInfo = NULL;
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            pRtrInfo = (tSrRtrInfo *) pRtrNode;

            if ((pRtrInfo->u2AddrType == SR_IPV4_ADDR_TYPE)
                && (u4AdvRtrId == pRtrInfo->advRtrId.Addr.u4Addr)
                && (u4OpqAreaId == pRtrInfo->u4areaId))
            {
                SR_TRC4 (SR_CTRL_TRC, "%s:%d SR node found with the Adv Rtr Id %x Area Id %x \n",
                         __func__, __LINE__, u4AdvRtrId, u4OpqAreaId);
                MEMCPY (&u4DestAddr, &pRtrInfo->prefixId.Addr.u4Addr,
                        sizeof (UINT4));
                isPeerExist = SR_TRUE;
                pNewRtrNode = pRtrInfo;
                break;
            }
        }
    }

    MEMSET (&SrOsRiLsInfo, SR_ZERO, sizeof (tSrOsRiLsInfo));

    SrOsRiLsInfo.advRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;
    SrOsRiLsInfo.advRtrId.Addr.u4Addr = u4AdvRtrId;

    pSrOsRiLsNode =
        RBTreeGet (gSrGlobalInfo.pSrOsRiLsRbTree, &SrOsRiLsInfo);

    if (pOsLsaInfo->u1LsaStatus == FLUSHED_LSA)
    {
        SR_TRC3 (SR_CTRL_TRC, "Recvd FLUSHED_LSA for %x, AdvRtrId %x, OpqType=%d\n",
                 u4DestAddr, u4AdvRtrId, u1OpqType);

        if (u1OpqType == RI_LSA_OPAQUE_TYPE)
        {
            SrProcessFlushRtrInfoLsa (u4AdvRtrId, u4OpqAreaId);
            return SR_SUCCESS;
        }
        else if (u1OpqType == OSPF_EXT_PRREFIX_LSA_OPAQUE_TYPE)
        {
            /*
               A Received Flush for Node-A will be ignored only if
               1. The Node-A is not present in peer info list.
               2. Flush is received with different advRtr ID(Node-C) and
               Peer info is present for the advrtr(Node-B) of Node-A
               in peer info list.
             */
            if ((isPeerExist == SR_FALSE)
                || (u4AdvRtrId != pRtrInfo->advRtrId.Addr.u4Addr))
            {
                SR_TRC4 (SR_CTRL_TRC,
                         "%s:%d Flush received from advRtrId diff from peerList prefix %x AdvRtrId %x\n",
                         __func__, __LINE__, u4DestAddr, u4AdvRtrId);
                return SR_SUCCESS;
            }

            if (SrProcessFlushExtPrefixOpqLsa (pRtrInfo) != SR_SUCCESS)
            {
                SR_TRC4 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                         "%s:%d Flushing ExtPrefixOpqLsa failed for prefix %x AdvRtrId %x\n",
                         __func__, __LINE__, u4DestAddr, u4AdvRtrId);
                return SR_FAILURE;
            }
            return SR_SUCCESS;
        }
        else if (u1OpqType == OSPF_EXT_LINK_LSA_OPAQUE_TYPE)
        {
            /*
            A Received Flush for Node-A will be ignored only if
            1. The Node-A is not present in peer info list.
            2. Flush is received with different advRtr ID(Node-C) and
            Peer info is present for the advrtr(Node-B) of Node-A
            in peer info list.
            */
            if ((isPeerExist == SR_FALSE)
                || (u4AdvRtrId != pRtrInfo->advRtrId.Addr.u4Addr))
            {
                return SR_SUCCESS;
            }

            if (SR_FAILURE == SrProcessOpqExtLinkLsa (pOsLsaInfo, pNewRtrNode))
            {
                SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC,
                    "%s %d Processing of recv Flush Ext Link LSA failed for AdvRtr ID %x\n",
                    __func__, __LINE__, pOsLsaInfo->u4AdvRtrId);
                return SR_FAILURE;
            }

            return SR_SUCCESS;
        }

        return SR_SUCCESS;
    }

    if ((isPeerExist == SR_FALSE) && (u1OpqType != RI_LSA_OPAQUE_TYPE))
    {
        SR_TRC3 (SR_CTRL_TRC, "%s:%d Creating New Router Node %x\n", __func__,
                 __LINE__, u4PrefixAddr);
        if ((pNewRtrNode = SrCreateNewRouterNode ()) == NULL)
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrCreateNewNextHopNode returns NULL \n",
                    __func__, __LINE__);
            return SR_FAILURE;
        }

        pNewRtrNode->u2AddrType = SR_IPV4_ADDR_TYPE;
        pNewRtrNode->advRtrId.Addr.u4Addr = u4AdvRtrId;
        pNewRtrNode->u4areaId = pOspfMsg->u4AreaId;
        pNewRtrNode->u1LSASent = SR_FALSE;

        if (u1OpqType == OSPF_EXT_PRREFIX_LSA_OPAQUE_TYPE)
        {
            pNewRtrNode->prefixId.u2AddrType = SR_IPV4_ADDR_TYPE;
            pNewRtrNode->prefixId.Addr.u4Addr = u4PrefixAddr;
            u4DestAddr = u4PrefixAddr;

        }
    }

    if (u1Temp == OSPF_EXT_PRREFIX_LSA_OPAQUE_TYPE)
    {
        SR_TRC4 (SR_CTRL_TRC, "%s:%d Recvd PREFIX_LSA for Prefix Id %x, AdvRtrId %x\n",
                 __func__, __LINE__, u4PrefixAddr, pOsLsaInfo->u4AdvRtrId);

        tlvPtr = pOsLsaInfo->pu1Lsa + (SR_NINE + SR_NINE + SR_TWO);    /* 20 bytes lsheader */
        pTemp = tlvPtr;
        /* Get the TLV Type */
        u2Temp = SR_LGET2BYTE (pTemp);
        if (u2Temp == OSPF_EXT_PRREFIX_TLV_TYPE)
        {
            tlvPtr += SR_TWO;
            u2ExtTlvLen = SR_LGET2BYTE (pTemp);
            /* The total length will include additional 4 bytes,
             * (2 Byte Type + 2 Byte Length),
             * subtracting 4 bytes to validate the TLV length */
            if (u2ExtTlvLen == OSPF_EXT_PRREFIX_TLV_LENGTH)
            {
                pNewRtrNode->u1RtrType = SR_LGET1BYTE (pTemp);
                tlvPtr += (SR_EIGHT + SR_TWO);
                u2Temp = SR_LGET2BYTE (tlvPtr);
                if (u2Temp == PREFIX_SID_SUB_TLV_TYPE)
                {
                    u2Temp = SR_LGET2BYTE (tlvPtr);
                    if (u2Temp == PREFIX_SID_SUB_TLV_LEN)
                    {
                        u1Temp = SR_LGET1BYTE (tlvPtr);
                        pNewRtrNode->u1Flags = u1Temp;

                        tlvPtr++;    /*This is for Reserved field */

                        u1Temp = SR_LGET1BYTE (tlvPtr);
                        pNewRtrNode->u1MTID = u1Temp;

                        tlvPtr++;    /*Done for Algo, as already received in RI LSA */

                        u4Temp = SR_LGET4BYTE (tlvPtr);
                        u4sidVal = (UINT4) u4Temp;

                        if (isPeerExist == SR_TRUE)
                        {
                            SR_TRC5 (SR_CTRL_TRC,
                                     "%s:%d AdvRtrId %x RtrSid %d RcvdSid %d \n",
                                     __func__, __LINE__, pOsLsaInfo->u4AdvRtrId,
                                     pNewRtrNode->u4SidValue, u4sidVal);

                            if (u4sidVal != pNewRtrNode->u4SidValue)
                            {
                                SR_TRC (SR_CTRL_TRC, "SID Value Changed \n");
                                /* SID Value Changed, So delete all the FTNs with all NH for this SrRtrIP */
                                tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                                tmpAddr.Addr.u4Addr = u4DestAddr;
                                /* To delete TE LSP in Type10 flow */
                                if (SrTeDeleteFtnType10 (&tmpAddr, &u4ErrCode)
                                    == SR_SUCCESS)
                                {
                                    if (u4ErrCode == SR_TE_VIA_DEST)
                                    {
                                        SR_TRC3 (SR_CTRL_TRC,
                                                 "%s:%d SR TE LSP deleted via dest in Type10 flow for prefix %x \n",
                                                 __func__, __LINE__,
                                                 tmpAddr.Addr.u4Addr);
                                    }
                                }
                                /* Sr-TE: End */
                                if (u4ErrCode != SR_TE_VIA_DEST)
                                {
                                    if (SR_FAILURE ==
                                        SrDeleteMplsEntry (pRtrInfo))
                                    {
                                        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                                                "Unable to Delete MPLS Entry \n");
                                    }
                                }
                                pNewRtrNode->u1LSASent = SR_FALSE;
                            }
                        }
                        pNewRtrNode->u4SidValue = u4sidVal;
                        pNewRtrNode->u1SidIndexType = SR_SID_TYPE_INDEX;
                        /* Check and raise/clear OOR Alarm */
                        SrChkForOORAlarm (pNewRtrNode, u4sidVal);
                    }
                    else
                    {
                        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                                "Invalid Prefix SID Sub TLV Length \n");
                        SR_PEER_RTR_LIST_FREE (isPeerExist, pNewRtrNode);
                        return SR_FAILURE;
                    }
                }
                else
                {
                    SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Invalid Prefix SID Sub TLV Type \n");
                    SR_PEER_RTR_LIST_FREE (isPeerExist, pNewRtrNode);
                    return SR_FAILURE;
                }
            }
            else
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Invalid Ext Prefix TLV Length \n");
                SR_PEER_RTR_LIST_FREE (isPeerExist, pNewRtrNode);
                return SR_FAILURE;
            }
        }
        else
        {
            SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Invalid Ext Prefix TLV Type \n");
            SR_PEER_RTR_LIST_FREE (isPeerExist, pNewRtrNode);
            return SR_FAILURE;
        }

        if ((pSrOsRiLsNode != NULL)
            && (pNewRtrNode->u1RtrType == OSPF_INTRA_AREA))
        {
            pNewRtrNode->u1Algo = pSrOsRiLsNode->u1Algo;
            pNewRtrNode->srgbRange.u4SrGbMinIndex = pSrOsRiLsNode->u4SrGbMinIndex;
            pNewRtrNode->srgbRange.u4SrGbMaxIndex = pSrOsRiLsNode->u4SrGbMaxIndex;
        }

        pNewRtrNode->u1RtrSrAttrFlags |= SR_PREFIX_SID_PRESENT;

    }
    else if (u1OpqType == RI_LSA_OPAQUE_TYPE)
    {
        SR_TRC3 (SR_CTRL_TRC, "%s:%d Recvd RI_LSA(4) for AdvRtrId %x\n",
                 __func__, __LINE__, pOsLsaInfo->u4AdvRtrId);

        pSrOsRiLsNode1 = RBTreeGet (gSrGlobalInfo.pSrOsRiLsRbTree, &SrOsRiLsInfo);
        if (pSrOsRiLsNode1 == NULL)
        {
            if (SR_OSPF_RI_LSA_MEM_ALLOC (pSrOsRiLsNode) == NULL)
            {
                SR_TRC (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                        "Memory Allocation Failure for SR RI LSA INFO\n");
                return SR_FAILURE;
            }

            MEMSET (pSrOsRiLsNode, SR_ZERO, sizeof (tSrOsRiLsInfo));

            pSrOsRiLsNode->advRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;
            pSrOsRiLsNode->advRtrId.Addr.u4Addr = u4AdvRtrId;
        }
        else
        {
            pSrOsRiLsNode = pSrOsRiLsNode1;
        }

        tlvPtr = pOsLsaInfo->pu1Lsa + (SR_NINE + SR_NINE + SR_TWO);    /* 20 bytes lsheader */
        u4sumTlvLen =
            (UINT4) pOsLsaInfo->u2LsaLen - (SR_NINE + SR_NINE + SR_TWO);
        while (u4TtlvLenCount < u4sumTlvLen)
        {
            u2Temp = SR_LGET2BYTE (tlvPtr);
            if (u2Temp == RI_LSA_SR_ALGO_TLV_TYPE)
            {
                u2Temp = SR_LGET2BYTE (tlvPtr);    /*2 bytes are also incremented */
                u4AlgoTlvLen = u2Temp;
                if ((u4AlgoTlvLen == SR_ONE) || (u4AlgoTlvLen == SR_TWO))
                {
                    while (u4AlgoTlvCounter < u4AlgoTlvLen)
                    {
                        pTemp = tlvPtr + u4AlgoTlvCounter;
                        u1Temp = SR_LGET1BYTE (pTemp);

                        pSrOsRiLsNode->u1Algo = u1Temp;
                        if (pNewRtrNode != NULL)
                        {
                            pNewRtrNode->u1Algo |= u1Temp;
                        }

                        u4AlgoTlvCounter++;
                    }
                    if (u4AlgoTlvLen % SR_FOUR)
                    {
                        tlvPtr +=
                            u4AlgoTlvLen + (SR_FOUR - (u4AlgoTlvLen % SR_FOUR));
                        u4TtlvLenCount +=
                            u4AlgoTlvLen + (SR_FOUR -
                                            (u4AlgoTlvLen % SR_FOUR)) + SR_FOUR;
                    }
                    else
                    {
                        tlvPtr += u4AlgoTlvLen;
                        u4TtlvLenCount += u4AlgoTlvLen + SR_FOUR;
                    }
                }
                else
                {
                    SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Not Supported SR-Algorithm TLV Length \n");
                    tlvPtr = tlvPtr+u4AlgoTlvLen;
                    u4TtlvLenCount += u4AlgoTlvLen + SR_FOUR;
                }
            }
            else if (u2Temp == RI_LSA_SR_SID_LABEL_RANGE_TLV_TYPE)
            {
                u2Temp = SR_LGET2BYTE (tlvPtr);
                u4SidLabelRangeTlvLen = u2Temp;
                if (u4SidLabelRangeTlvLen == RI_LSA_SR_SID_LABEL_RANGE_TLV_LEN)
                {
                    u4Temp = (UINT4) SR_LGET3BYTE (tlvPtr);
                    tlvPtr++;    /*Incremented for Reserved byte */
                    u4SrgbRangeSize = u4Temp;

                    u2Temp = SR_LGET2BYTE (tlvPtr);
                    if (u2Temp == SID_LABEL_SUB_TLV_TYPE)
                    {
                        u2Temp = SR_LGET2BYTE (tlvPtr);
                        if (u2Temp == SID_LABEL_SUB_TLV_LABEL_LENGTH)
                        {
                            u4Temp = (UINT4) SR_LGET3BYTE (tlvPtr);
                            u4MinSrgbRange = (UINT4) u4Temp;
                            u4MaxSrgbRange =
                                u4MinSrgbRange + u4SrgbRangeSize - SR_ONE;
                            if (isPeerExist == SR_TRUE)
                            {
                                SR_TRC5 (SR_CTRL_TRC,
                                         "%s:%d AdvRtrId %x RtrMin %d RtrMax %d \n",
                                         __func__, __LINE__, u4DestAddr,
                                         pNewRtrNode->srgbRange.u4SrGbMinIndex,
                                         pNewRtrNode->srgbRange.u4SrGbMaxIndex);

                                if (((pNewRtrNode->srgbRange.u4SrGbMinIndex !=
                                      u4MinSrgbRange) &&
                                      (pNewRtrNode->srgbRange.u4SrGbMinIndex != 0))
                                   || ((pNewRtrNode->srgbRange.u4SrGbMaxIndex !=
                                        u4MaxSrgbRange) &&
                                        (pNewRtrNode->srgbRange.u4SrGbMaxIndex != 0)))
                                {
                                    SR_TRC (SR_CTRL_TRC, "SRGB Changed \n");
                                    tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                                    tmpAddr.Addr.u4Addr = u4DestAddr;
                                    if (SrDeleteMplsBindingOnNbrSrDown
                                        (&tmpAddr, u4AdvRtrId) == SR_FAILURE)
                                    {
                                        SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC,
                                                 "%x RouterId is not Neighbour of any other node \n",
                                                 u4DestAddr);
                                    }
                                    /* To delete TE LSP in Type10 flow */
                                    if (SrTeDeleteFtnType10
                                        (&tmpAddr, &u4ErrCode) == SR_SUCCESS)
                                    {
                                        if (u4ErrCode == SR_TE_VIA_DEST)
                                        {
                                            SR_TRC3 (SR_CTRL_TRC,
                                                     "%s:%d SR TE LSP deleted via dest in Type10 flow for prefix %x \n",
                                                     __func__, __LINE__,
                                                     tmpAddr.Addr.u4Addr);
                                        }
                                    }
                                    /* Sr-TE: End */
                                    if (u4ErrCode != SR_TE_VIA_DEST)
                                    {
                                        if (SR_FAILURE ==
                                            SrDeleteMplsEntry (pNewRtrNode))
                                        {
                                            SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                                                    "Unable to Delete MPLS Entry \n");
                                        }
                                    }

                                    pNewRtrNode->u1LSASent = SR_FALSE;
                                }
                            }

                            pSrOsRiLsNode->u4SrGbMinIndex = u4MinSrgbRange;
                            pSrOsRiLsNode->u4SrGbMaxIndex = u4MaxSrgbRange;
                            if (pNewRtrNode != NULL)
                            {
                                pNewRtrNode->srgbRange.u4SrGbMinIndex =
                                    u4MinSrgbRange;
                                pNewRtrNode->srgbRange.u4SrGbMaxIndex =
                                    u4MaxSrgbRange;
                            }
                        }
                        else
                        {
                            SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                                    "Invalid SID Label Sub TLV Length \n");

                            if (pNewRtrNode != NULL)
                            {
                                SR_PEER_RTR_LIST_FREE (isPeerExist, pNewRtrNode);
                            }
                            if (pSrOsRiLsNode1 == NULL)
                            {
                                SR_OSPF_RI_LSA_MEM_FREE (pSrOsRiLsNode);
                            }

                            return SR_FAILURE;
                        }
                    }
                    else
                    {
                        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Invalid SID Label Sub TLV Type \n");

                        if (pNewRtrNode != NULL)
                        {
                            SR_PEER_RTR_LIST_FREE (isPeerExist, pNewRtrNode);
                        }
                        if (pSrOsRiLsNode1 == NULL)
                        {
                            SR_OSPF_RI_LSA_MEM_FREE (pSrOsRiLsNode);
                        }

                        return SR_FAILURE;
                    }
                    u4TtlvLenCount += u4SidLabelRangeTlvLen + SR_FOUR;
                }
                else
                {
                    SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Invalid SID Label Range TLV Length \n");

                    if (pNewRtrNode != NULL)
                    {
                        SR_PEER_RTR_LIST_FREE (isPeerExist, pNewRtrNode);
                    }
                    if (pSrOsRiLsNode1 == NULL)
                    {
                        SR_OSPF_RI_LSA_MEM_FREE (pSrOsRiLsNode);
                    }

                    return SR_FAILURE;
                }
            }
            else
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Unsupported TLV Type \n");
                /* get the length of the TLV and increment the TLV length */
                u2Temp = SR_LGET2BYTE (tlvPtr);    /*2 bytes are also incremented */
                tlvPtr = tlvPtr + u2Temp;
                u4TtlvLenCount += u2Temp + SR_FOUR;
            }
        }

        if (pSrOsRiLsNode1 == NULL)
        {
            RBTreeAdd (gSrGlobalInfo.pSrOsRiLsRbTree, (tRBElem *)pSrOsRiLsNode);
        }

        if((NULL == pNewRtrNode) || (pNewRtrNode->prefixId.Addr.u4Addr == SR_ZERO))
        {
           return SR_SUCCESS;
        }
    }
    else if (u1OpqType == OSPF_EXT_LINK_LSA_OPAQUE_TYPE)
    {
        SR_TRC3 (SR_CTRL_TRC, "%s:%d Recvd EXT_LINK LSA from AdvRtr Id %x\n",
                 __func__, __LINE__, pOsLsaInfo->u4AdvRtrId);
        if (SR_FAILURE == SrProcessOpqExtLinkLsa (pOsLsaInfo, pNewRtrNode))
        {
            SR_TRC3 (SR_CRITICAL_TRC | SR_FAIL_TRC, "%s %d Parsing of recv Ext Link LSA failed from AdvRtr ID %x\n",
                    __func__, __LINE__, pOsLsaInfo->u4AdvRtrId);
            SR_PEER_RTR_LIST_FREE (isPeerExist, pNewRtrNode);
            return SR_FAILURE;
        }
    }
    else
    {
        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Invalid Opaque LSA Type not supported by SR \n");
        SR_PEER_RTR_LIST_FREE (isPeerExist, pNewRtrNode);
        return SR_FAILURE;
    }

    if ((pNewRtrNode != NULL)
        && (pOsLsaInfo->u1LsaStatus != FLUSHED_LSA))
    {
        if (u1OpqType == OSPF_EXT_PRREFIX_LSA_OPAQUE_TYPE)
        {
            /* Check for Prefix conflict */
            SrChkAndActOnPrefixConflict (pNewRtrNode, u4PrefixAddr);
            if (pNewRtrNode->u4SidValue != 0)
            {
                /* Check for SID conflict */
                SrChkAndActOnSIDConflict (pNewRtrNode, pNewRtrNode->u4SidValue, isPeerExist);
            }
        }

        if (isPeerExist == SR_FALSE)
        {
            TMO_SLL_Insert (&(gSrGlobalInfo.routerList), NULL,
                    &(pNewRtrNode->nextRtrId));
            SR_TRC4 (SR_CTRL_TRC, "%s:%d Rtr_info Added for PrefixId %x, AdvRtrId %x\n",
                    __func__, __LINE__, pNewRtrNode->prefixId.Addr.u4Addr,
                    pNewRtrNode->advRtrId.Addr.u4Addr);
        }

        if (pNewRtrNode->u1LSASent == SR_FALSE)
        {
            pNewRtrNode->u1LSASent = SR_TRUE;
            if (SR_FAILURE ==
                    SrUtilSendLSA (NEW_LSA_INSTANCE, SR_ZERO, TYPE10_OPQ_LSA))
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                        "%s:%d Unable to Send TYPE10_OPQ_LSA Self LSA \n", __func__,
                        __LINE__);
            }

            if (SR_TRUE == i4AbrStatus)
            {
                pRtrNode = NULL;
                TMO_SLL_Scan (&(gSrGlobalInfo.routerList),
                        pRtrNode, tTMO_SLL_NODE *)
                {
                    pRtrInfo1 = (tSrRtrInfo *) pRtrNode;
                    MEMCPY (&u4AddressPrefix, &pRtrInfo1->prefixId.Addr.u4Addr,
                            sizeof (UINT4));
                    if (u4AddressPrefix != SR_ZERO
                            && pRtrInfo1->u4SidValue != SR_ZERO
                            && (pRtrInfo1->u2AddrType == SR_IPV4_ADDR_TYPE)
                            && (u1OpqType == OSPF_EXT_PRREFIX_LSA_OPAQUE_TYPE))
                    {
                        if (pRtrInfo1->prefixId.Addr.u4Addr == u4PrefixAddr)
                        {
                            /* SENDING NEW RTR DETAILS to ALL AREAS  */
                            SrConstructSendExtPrefixOpqLsa (NEW_LSA,
                                    pRtrInfo1->u4SidValue,
                                    pRtrInfo1->prefixId.Addr.
                                    u4Addr, u4OpqAreaId,
                                    SR_TWO);
                            pRtrInfo1->u1LSASent = SR_TRUE;
                        }
                        else
                        {
                            if (pRtrInfo1->u4areaId != u4OpqAreaId)
                            {
                                /* All other RTR details to the New Peer Received Area.
                                 * Need not to send to same area of peer received area */
                                SrConstructSendExtPrefixOpqLsa (NEW_LSA,
                                        pRtrInfo1->u4SidValue,
                                        pRtrInfo1->prefixId.Addr.u4Addr,
                                        u4OpqAreaId,
                                        SR_ONE);
                            }
                        }
                    }
                    else if (pRtrInfo1->u4SidValue == SR_ZERO)
                    {
                        pRtrInfo1->u1LSASent = SR_FALSE;
                    }
                }
            }
        }

        SR_TRC4 (SR_CTRL_TRC, "%s:%d SID %d SrGbMinIndex %d \n", __func__, __LINE__,
                pNewRtrNode->u4SidValue, pNewRtrNode->srgbRange.u4SrGbMinIndex);
        if (pNewRtrNode->u4SidValue != SR_ZERO)
        {
            SR_TRC3 (SR_CTRL_TRC, "%s:%d Checking if TE is configured for destAddr %x \n",
                    __func__, __LINE__, u4DestAddr);
            /* Sr-TE: Start */

            tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
            tmpAddr.Addr.u4Addr = u4DestAddr;

            /* To create TE LSP in the Type10 flow */
            /* It will also create TE LFA Lsp, when SR_LFA is enabled */
            if (SrTeCreateFtnType10 (&tmpAddr, &u4ErrCode) == SR_SUCCESS)
            {
                if (u4ErrCode == SR_TE_VIA_DEST)
                {
                    SR_TRC3 (SR_CTRL_TRC,
                            "%s:%d SR TE LSP processed via dest in Type10 flow for prefix %x \n",
                            __func__, __LINE__, tmpAddr.Addr.u4Addr);
                    return SR_SUCCESS;
                }
            }
            else
            {
                if (u4ErrCode == SR_TE_VIA_DEST)
                {
                    SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                            "%s:%d SR TE LSP processing via dest in Type10 flow for prefix %x is failed \n",
                            __func__, __LINE__, tmpAddr.Addr.u4Addr);
                    return SR_FAILURE;
                }
            }

            /* Sr-TE: End */
            if (SR_ZERO == pNewRtrNode->srgbRange.u4SrGbMinIndex ||
                SR_ZERO == pNewRtrNode->srgbRange.u4SrGbMaxIndex)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                        "%s:%d SRGB is invalid. SR LSP processing via dest in Type10 flow for prefix %x is failed \n",
                        __func__, __LINE__, tmpAddr.Addr.u4Addr);
                return SR_SUCCESS;
            }

            /* SR LSP processing for non-TE prefix starts */
            /* Case1: No Existing FTN/SrNextHopInfo present */
            if (TMO_SLL_Count (&(pNewRtrNode->NextHopList)) == SR_ZERO)
            {
                SR_TRC3 (SR_CTRL_TRC, "%s:%d SID %d \n", __func__, __LINE__,
                        pNewRtrNode->u4SidValue);

                /* Non-TE: Start */
                SrCreateMplsEntry (pNewRtrNode, NULL);
                tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                tmpAddr.Addr.u4Addr = u4DestAddr;
                if (SrCreateMplsBindingOnNbrSrUp (&tmpAddr, u4AdvRtrId) ==
                        SR_FAILURE)
                {
                    SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC,
                            "%x AdvRouterId is not Neighbour of any other node \n",
                            pOsLsaInfo->u4AdvRtrId);
                }
                /* Non-TE: End */
            }
            else
            {
                /* Case2: Existing FTN/SrNextHopInfo present */

                TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                        pSrRtrNextHopNode, tTMO_SLL_NODE *)
                {
                    pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;
                    MEMCPY (&u4NextHop, &pSrRtrNextHopInfo->nextHop.Addr.u4Addr,
                            sizeof (UINT4));

                    SR_TRC5 (SR_CTRL_TRC,
                            "%s:%d Existing SrNextHopInfo present for %x %x \n",
                            __func__, __LINE__, pOsLsaInfo->u4AdvRtrId, u4NextHop,
                            pSrRtrNextHopInfo->u1MPLSStatus);

                    if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                            SR_FTN_CREATED)
                    {
                        SR_TRC5 (SR_CTRL_TRC | SR_CRITICAL_TRC,
                                "%s:%d FTN already created for %x %x (Type: %x) \n",
                                __func__, __LINE__, pOsLsaInfo->u4AdvRtrId,
                                u4NextHop, pSrRtrNextHopInfo->u1FRRNextHop);
                        if(pSrRtrNextHopInfo->u1FRRNextHop == SR_PRIMARY_NEXTHOP)
                        {
                            u1FtnCreated = SR_TRUE;
                        }

                        continue;
                    }
                    SR_TRC4 (SR_CTRL_TRC,
                            "%s:%d pSrRtrNextHopInfo info %d %d \n",
                            __func__, __LINE__, pSrRtrNextHopInfo->u1FRRNextHop,
                            pSrRtrNextHopInfo->u1MPLSStatus);
                    /* Non-TE: Start */
                    if(pSrRtrNextHopInfo->u1FRRNextHop == SR_PRIMARY_NEXTHOP)
                    {
                        SrCreateMplsEntry (pNewRtrNode, pSrRtrNextHopInfo);
                        break;
                    }

                    /* Non-TE: End */
                }

                /* If FTN is created, POP & Fwd will not be present */
                if (u1FtnCreated == SR_FALSE)
                {
                    tmpAddr.u2AddrType = SR_IPV4_ADDR_TYPE;
                    tmpAddr.Addr.u4Addr = u4DestAddr;
                    if (SrCreateMplsBindingOnNbrSrUp (&tmpAddr, u4AdvRtrId) ==
                            SR_FAILURE)
                    {
                        SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC,
                                "%x RouterId is not Neighbour of any other node \n",
                                pOsLsaInfo->u4AdvRtrId);
                    }
                    else
                    {
                        SR_TRC1 (SR_CTRL_TRC,
                                "%x Mpls Binding on NbrSrUp Completed for %x \n",
                                pOsLsaInfo->u4AdvRtrId);
                    }
                }
            }
        }
    }

    UNUSED_PARAM (u1LSASent);
    return SR_SUCCESS;
}

PUBLIC UINT4
SrV3ProcessOpqType10Lsa (tV3OsToOpqApp * pOspfMsg)
{
    tOs3LsaInfo        *pOsLsaInfo = NULL;
    tIp6Addr            prefixAddr;
    tIp6Addr            Ipv6Addr;
    UINT2               u2LsaType = SR_ZERO;
    tGenU4Addr          destAddr;
    tGenU4Addr          dummyAddr;
    tRouterId           rtrId;
    tTMO_SLL_NODE      *pRtrNode = NULL;
    tTMO_SLL_NODE      *pSrRtrNextHopNode = NULL;
    tSrRtrNextHopInfo  *pSrRtrNextHopInfo = NULL;
    tSrRtrInfo         *pRtrInfo = NULL;
    tSrRtrInfo         *pRtrInfo1 = NULL;
    tSrRtrInfo         *pNewRtrNode = NULL;
    tSrRtrInfo         *pTmpRtrInfo = NULL;
    UINT1              *tlvPtr = NULL;
    UINT1               isPeerExist = SR_FALSE;
    UINT1               au1NullIpv6Addr[SR_IPV6_ADDR_LENGTH];
    tOpqLSAInfo         opqLSAInfo;
    tOpqLSAInfo         opqLSAInfo1;
    UINT1               au1RtrAddrTlv[SR_RI_LSA_TLV_LEN];
    UINT1               au1RtrAddrTlv1[SR_RI_LSA_TLV_LEN];
    UINT4               u4sidVal = SR_ZERO;
    UINT4               u4sumTlvLen = SR_ZERO;
    UINT4               u4TtlvLenCount = SR_ZERO;
    UINT4               u4MinSrgbRange = SR_ZERO;
    UINT4               u4MaxSrgbRange = SR_ZERO;
    UINT4               u4SrgbRangeSize = SR_ZERO;
    UINT4               u4AlgoTlvLen = SR_ZERO;
    UINT4               u4AlgoTlvCounter = SR_ZERO;
    UINT4               u4SidLabelRangeTlvLen = SR_ZERO;
    UINT4               u4ErrCode = SR_ZERO;
    UINT4               u4DestAddr = SR_ZERO;
    UINT1              *pTemp = NULL;
    UINT1               u1Temp = SR_ZERO;
    UINT2               u2Temp = SR_ZERO;
    UINT2               u2lsaType = OSPFV3_EXT_INTER_AREA_PREFIX_LSA;
    UINT4               u4Temp = SR_ZERO;
    UINT1               u1FtnCreated = SR_ZERO;
    UINT1               u1NeighFlag = SR_FALSE;
    UINT4               u4SelfAddr = SR_ZERO;
    INT4                i4AbrStatus = SR_FALSE;
    UINT4               u4AdvRtrId = SR_ZERO;
    UINT4               u1LSASent = SR_FALSE;

    MEMSET (&destAddr, 0, sizeof (tGenU4Addr));
    MEMSET (&dummyAddr, 0, sizeof (tGenU4Addr));
    MEMSET (&opqLSAInfo, 0, sizeof (tOpqLSAInfo));
    MEMSET (&opqLSAInfo1, 0, sizeof (tOpqLSAInfo));
    MEMSET (au1RtrAddrTlv, 0, sizeof (SR_RI_LSA_TLV_LEN));
    MEMSET (rtrId, 0, sizeof (tRouterId));
    MEMSET (destAddr.Addr.Ip6Addr.u1_addr, SR_ZERO, SR_IPV6_ADDR_LENGTH);
    opqLSAInfo.pu1LSA = au1RtrAddrTlv;
    opqLSAInfo1.pu1LSA = au1RtrAddrTlv1;
    pOsLsaInfo = &(pOspfMsg->o3LsaInfo);

    MEMSET (Ipv6Addr.u1_addr, SR_ZERO, SR_IPV6_ADDR_LENGTH);
    MEMSET (au1NullIpv6Addr, SR_ZERO, SR_IPV6_ADDR_LENGTH);
    MEMSET (prefixAddr.u1_addr, SR_ZERO, SR_IPV6_ADDR_LENGTH);
    u4AdvRtrId = pOsLsaInfo->u4AdvRtrId;

    if (gSrGlobalInfo.u1Ospf3AbrStatus == SR_TRUE)
    {
        i4AbrStatus = SR_TRUE;
    }

    u4SelfAddr = gSrGlobalInfo.u4OspfRtrID;

    /* If SR Degregistation is going to happen, OSPFv3 packets should not be processed */
    if ((u4SelfAddr != u4AdvRtrId)
        && (gSrGlobalInfo.u1Ospfv3DeRegInProgress == SR_TRUE))
    {
        return SR_SUCCESS;
    }

    if (SrV3GetAddrPrefix (pOsLsaInfo, &prefixAddr) == SR_SUCCESS)
    {
        destAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (destAddr.Addr.Ip6Addr.u1_addr, prefixAddr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
    }
    u2LsaType = pOsLsaInfo->u2LsaType;

    if ((u4SelfAddr == u4AdvRtrId) ||
        (MEMCMP
         (prefixAddr.u1_addr,
          gSrGlobalInfo.SrContextV3.rtrId.Addr.Ip6Addr.u1_addr,
          SR_IPV6_ADDR_LENGTH) == SR_ZERO))
    {
        SR_TRC3 (SR_CTRL_TRC, "%s:%d Self LSA Recv %x, Returning \n", __func__,
                 __LINE__, pOsLsaInfo->u4AdvRtrId);
        return SR_SUCCESS;
    }

    pRtrInfo = NULL;
    TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
    {
        pRtrInfo = (tSrRtrInfo *) pRtrNode;
        if ((MEMCMP
             (prefixAddr.u1_addr, pRtrInfo->prefixId.Addr.Ip6Addr.u1_addr,
              SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            && (pRtrInfo->u2AddrType == SR_IPV6_ADDR_TYPE))
        {
            isPeerExist = SR_TRUE;
            pNewRtrNode = pRtrInfo;
            break;
        }
    }

    if (SrIsV3FlushLsa (pOsLsaInfo) == SR_TRUE)
    {
        pTmpRtrInfo = NULL;
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            pTmpRtrInfo = (tSrRtrInfo *) pRtrNode;
            if ((pTmpRtrInfo->advRtrId.Addr.u4Addr ==
                 pRtrInfo->advRtrId.Addr.u4Addr)
                && (pTmpRtrInfo->srgbRange.u4SrGbV3MinIndex != SR_ZERO))
            {
                u1NeighFlag = SR_TRUE;
            }
        }

        /*
           A Received Flush for Node-A will be ignored only if
           1. The Node-A is not present in peer info list.
           2. Flush is received with different advRtr ID(Node-C) and
           Peer info is present for the advrtr(Node-B) of Node-A
           in peer info list.
         */

        if ((isPeerExist == SR_FALSE) ||
            ((u4AdvRtrId != pRtrInfo->advRtrId.Addr.u4Addr) &&
             (u1NeighFlag == SR_TRUE)))
        {
            return SR_SUCCESS;
        }

        SR_TRC1 (SR_CTRL_TRC, "Recvd FLUSHED_LSA for %s \n",
                 (Ip6PrintAddr (&prefixAddr)));
        /* Sr-TE: Start */
        destAddr.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (destAddr.Addr.Ip6Addr.u1_addr, prefixAddr.u1_addr,
                SR_IPV6_ADDR_LENGTH);

        /* To delete TE LSP in Type10 flow */
        if (SrTeDeleteFtnType10 (&destAddr, &u4ErrCode) == SR_SUCCESS)
        {
            if (u4ErrCode == SR_TE_VIA_DEST)
            {
                SR_TRC3 (SR_CTRL_TRC,
                         "%s:%d SR TE LSP deleted via dest in Type10 flow for prefix %x \n",
                         __func__, __LINE__, destAddr.Addr.Ip6Addr.u1_addr);
            }
        }
        /* Sr-TE: End */
        /* SR LSP processing for non-TE prefix starts */
        if (u4ErrCode != SR_TE_VIA_DEST)
        {
            if (SR_FAILURE == SrDeleteMplsEntry (pRtrInfo))
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Unable to Delete MPLS Entry \n");
                return SR_FAILURE;
            }
        }

        if (i4AbrStatus == SR_TRUE)
        {
            /* Send flush for the LSA that SR originated */
            SrV3ConstructSendExtLsa (FLUSHED_LSA, pRtrInfo->u4SidValue,
                                     pRtrInfo->prefixId.Addr.Ip6Addr,
                                     pRtrInfo->u4areaId,
                                     OSPFV3_EXT_INTER_AREA_PREFIX_LSA);
        }
        if (SrDeleteMplsBindingOnNbrSrDown (&destAddr, u4AdvRtrId) ==
            SR_FAILURE)
        {
            SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "RouterId is not Neighbour of any other node \n");
        }

        TMO_SLL_FreeNodes ((tTMO_SLL *) & pRtrInfo->adjSidList,
                           SrMemPoolIds[MAX_SR_PEER_ADJ_SIZING_ID]);
        TMO_SLL_Delete (&(gSrGlobalInfo.routerList),
                        (tTMO_SLL_NODE *) pRtrInfo);
        SR_RTR_LIST_FREE (pRtrInfo);

        return SR_SUCCESS;
    }

    if (isPeerExist == SR_FALSE)
    {
        if (pOsLsaInfo->u1LsaStatus == FLUSHED_LSA)
        {
            return SR_SUCCESS;
        }

        if ((pNewRtrNode = SrCreateNewRouterNode ()) == NULL)
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d SrCreateNewNextHopNode returns NULL \n",
                    __func__, __LINE__);
            return SR_FAILURE;
        }
        pNewRtrNode->prefixId.u2AddrType = SR_IPV6_ADDR_TYPE;
        MEMCPY (pNewRtrNode->prefixId.Addr.Ip6Addr.u1_addr, prefixAddr.u1_addr,
                SR_IPV6_ADDR_LENGTH);
        pNewRtrNode->u4areaId = pOspfMsg->u4AreaId;
        pNewRtrNode->u1LSASent = SR_FALSE;
        pNewRtrNode->u2AddrType = SR_IPV6_ADDR_TYPE;
        pNewRtrNode->advRtrId.Addr.u4Addr = u4AdvRtrId;
        pNewRtrNode->advRtrId.u2AddrType = SR_IPV4_ADDR_TYPE;
    }

    tlvPtr = pOsLsaInfo->pu1Lsa;

    pTemp = tlvPtr + 2;
    u2Temp = SR_LGET2BYTE (pTemp);
    if (V3_RI_LSA_OPAQUE_TYPE == (0xc & u2Temp))
    {
        SR_TRC2 (SR_CTRL_TRC, "%s:%d Recvd V3_RI_LSA ", __func__, __LINE__);
        tlvPtr = pOsLsaInfo->pu1Lsa + 20;
        u4sumTlvLen = (UINT4) (pOsLsaInfo->u2LsaLen - 20);
        while (u4TtlvLenCount < u4sumTlvLen)
        {
            u2Temp = SR_LGET2BYTE (tlvPtr);
            if (u2Temp == OSPFV3_INTRA_AREA_PRFIX_TLV_TYPE)
            {
                tlvPtr += (OSPFV3_INTRA_AREA_PRFIX_TLV_HDR_LEN - 2);
                u4TtlvLenCount += OSPFV3_INTRA_AREA_PRFIX_TLV_HDR_LEN;
            }
            else if (u2Temp == RI_LSA_SR_ALGO_TLV_TYPE)
            {
                u2Temp = SR_LGET2BYTE (tlvPtr);    /*2 bytes are also incremented */
                u4AlgoTlvLen = u2Temp;
                while (u4AlgoTlvCounter < u4AlgoTlvLen)
                {
                    pTemp = tlvPtr + u4AlgoTlvCounter;
                    u1Temp = SR_LGET1BYTE (pTemp);
                    pNewRtrNode->u1Algo |= u1Temp;
                    u4AlgoTlvCounter++;
                }
                if (u4AlgoTlvLen % 4)
                {
                    tlvPtr += u4AlgoTlvLen + (4 - (u4AlgoTlvLen % 4));
                    u4TtlvLenCount +=
                        u4AlgoTlvLen + (4 - (u4AlgoTlvLen % 4)) + 4;
                }
                else
                {
                    tlvPtr += u4AlgoTlvLen;
                    u4TtlvLenCount += u4AlgoTlvLen + 4;
                }
            }
            else if (u2Temp == RI_LSA_SR_SID_LABEL_RANGE_TLV_TYPE)
            {
                u2Temp = SR_LGET2BYTE (tlvPtr);
                u4SidLabelRangeTlvLen = u2Temp;
                u4Temp = (UINT4) SR_LGET3BYTE (tlvPtr);
                tlvPtr++;        /*Incremented for Reserved byte */
                u4SrgbRangeSize = u4Temp;

                u2Temp = SR_LGET2BYTE (tlvPtr);
                if (u2Temp == SID_LABEL_SUB_TLV_TYPE_V3)
                {
                    u2Temp = SR_LGET2BYTE (tlvPtr);
                    if (u2Temp == SID_LABEL_SUB_TLV_LABEL_LENGTH)
                    {
                        u4Temp = (UINT4) SR_LGET3BYTE (tlvPtr);
                        tlvPtr++;
                        u4MinSrgbRange = (UINT4) u4Temp;
                        u4MaxSrgbRange =
                            u4MinSrgbRange + u4SrgbRangeSize - SR_ONE;
                        if (isPeerExist == SR_TRUE)
                        {
                            if ((pNewRtrNode->srgbRange.u4SrGbV3MinIndex !=
                                 u4MinSrgbRange)
                                || (pNewRtrNode->srgbRange.u4SrGbV3MaxIndex !=
                                    u4MaxSrgbRange))
                            {
                                SR_TRC (SR_CTRL_TRC, "SRGB Changed");
                                pNewRtrNode->advRtrId.Addr.u4Addr = u4AdvRtrId;
                                if (SrDeleteMplsBindingOnNbrSrDown
                                    (&destAddr, u4AdvRtrId) == SR_FAILURE)
                                {
                                    SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC,
                                             "%x RouterId is not Neighbour of any other node \n",
                                             pOsLsaInfo->u4AdvRtrId);
                                }
                                /* To delete TE LSP in Type10 flow */
                                if (SrTeDeleteFtnType10 (&destAddr, &u4ErrCode)
                                    == SR_SUCCESS)
                                {
                                    if (u4ErrCode == SR_TE_VIA_DEST)
                                    {
                                        SR_TRC3 (SR_CTRL_TRC,
                                                 "%s:%d SR TE LSP deleted via dest in Type10 flow for prefix %x \n",
                                                 __func__, __LINE__,
                                                 destAddr.Addr.Ip6Addr.u1_addr);
                                    }
                                }
                                /* Sr-TE: End */
                                /* SR LSP processing for non-TE prefix starts */
                                if (u4ErrCode != SR_TE_VIA_DEST)
                                {
                                    if (SR_FAILURE ==
                                        SrDeleteMplsEntry (pRtrInfo))
                                    {
                                        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                                                "Unable to Delete MPLS Entry \n");
                                    }
                                }
                            }
                        }
                        pNewRtrNode->srgbRange.u4SrGbV3MinIndex =
                            u4MinSrgbRange;
                        pNewRtrNode->srgbRange.u4SrGbV3MaxIndex =
                            u4MaxSrgbRange;
                    }
                    else
                    {
                        SR_TRC (SR_CTRL_TRC,
                                "Invalid SID Label Sub TLV Length \n");
                        return SR_FAILURE;
                    }
                }
                else
                {
                    SR_TRC (SR_CTRL_TRC, "Invalid SID Label Sub TLV Type \n");
                }
                u4TtlvLenCount += u4SidLabelRangeTlvLen + 4;
            }
            else
            {
                SR_TRC (SR_CTRL_TRC, "Invalid RI Opaque LSA TLV Type \n");
            }
        }
    }
    else if (OSPFV3_EXT_INTRA_AREA_PREFIX_LSA == u2Temp)
    {
        SR_TRC2 (SR_CTRL_TRC, "%s:%d Recvd V3_E_INTRA_AREA_PREFIX_LSA_TYPE \n",
                 __func__, __LINE__);
        tlvPtr = pOsLsaInfo->pu1Lsa + OSPFV3_EXT_INTRA_AREA_PREFIX_LSA_HDR_LEN;
        u2Temp = SR_LGET2BYTE (tlvPtr);
        if (u2Temp == OSPFV3_INTRA_AREA_PRFIX_TLV_TYPE)
        {
            u2Temp = SR_LGET2BYTE (tlvPtr);
            if (u2Temp == OSPFV3_INTRA_AREA_PRFIX_TLV_LEN)
            {
                tlvPtr += OSPFV3_INTRA_AREA_PRFIX_TLV_ADDR_OFFSET;
                MEMCPY (pNewRtrNode->prefixId.Addr.Ip6Addr.u1_addr, tlvPtr,
                        SR_IPV6_ADDR_LENGTH);

                tlvPtr += SR_IPV6_ADDR_LENGTH;
                u2Temp = SR_LGET2BYTE (tlvPtr);
                if (u2Temp == PREFIX_SID_SUB_TLV_TYPE_V3)
                {
                    u2Temp = SR_LGET2BYTE (tlvPtr);
                    if (u2Temp == PREFIX_SID_SUB_TLV_LEN_V3)
                    {
                        u1Temp = SR_LGET1BYTE (tlvPtr);
                        pNewRtrNode->u1Flags = u1Temp;

                        tlvPtr += 3;    /*Algo + Reserved */

                        u4Temp = SR_LGET4BYTE (tlvPtr);
                        u4sidVal = (UINT4) u4Temp;
                        if (isPeerExist == SR_TRUE)
                        {
                            if (u4sidVal != pNewRtrNode->u4SidValue)
                            {
                                SR_TRC (SR_CTRL_TRC, "SID Value Changed \n");
                                /* SID Value Changed, So delete all the FTNs with all NH for this SrRtrIP */
                                /* To delete TE LSP in Type10 flow */
                                if (SrTeDeleteFtnType10 (&destAddr, &u4ErrCode)
                                    == SR_SUCCESS)
                                {
                                    if (u4ErrCode == SR_TE_VIA_DEST)
                                    {
                                        SR_TRC3 (SR_CTRL_TRC,
                                                 "%s:%d SR TE LSP deleted via dest in Type10 flow for prefix %x \n",
                                                 __func__, __LINE__,
                                                 destAddr.Addr.Ip6Addr.u1_addr);
                                    }
                                }
                                /* Sr-TE: End */
                                /* SR LSP processing for non-TE prefix starts */
                                if (u4ErrCode != SR_TE_VIA_DEST)
                                {
                                    if (SR_FAILURE ==
                                        SrDeleteMplsEntry (pRtrInfo))
                                    {
                                        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                                                "Unable to Delete MPLS Entry \n");
                                    }
                                }
                            }
                        }
                        pNewRtrNode->u4SidValue = u4sidVal;
                        pNewRtrNode->u1SidIndexType = SR_SID_TYPE_INDEX;
                    }
                }
            }
        }
    }
    else if (OSPFV3_EXT_INTER_AREA_PREFIX_LSA == u2Temp)
    {
        SR_TRC2 (SR_CTRL_TRC, "%s:%d Recvd V3_E_INTER_AREA_PREFIX_LSA_TYPE \n",
                 __func__, __LINE__);
        tlvPtr = pOsLsaInfo->pu1Lsa + OSPFV3_EXT_INTER_AREA_PREFIX_LSA_HDR_LEN;
        u2Temp = SR_LGET2BYTE (tlvPtr);
        if (u2Temp == OSPFV3_INTER_AREA_PRFIX_TLV_TYPE)
        {
            u2Temp = SR_LGET2BYTE (tlvPtr);
            if (u2Temp == OSPFV3_INTER_AREA_PRFIX_TLV_LEN)
            {
                tlvPtr += OSPFV3_INTER_AREA_PRFIX_TLV_ADDR_OFFSET;
                MEMCPY (pNewRtrNode->prefixId.Addr.Ip6Addr.u1_addr, tlvPtr,
                        SR_IPV6_ADDR_LENGTH);

                tlvPtr += SR_IPV6_ADDR_LENGTH;
                u2Temp = SR_LGET2BYTE (tlvPtr);
                if (u2Temp == PREFIX_SID_SUB_TLV_TYPE_V3)
                {
                    u2Temp = SR_LGET2BYTE (tlvPtr);
                    if (u2Temp == PREFIX_SID_SUB_TLV_LEN_V3)
                    {
                        u1Temp = SR_LGET1BYTE (tlvPtr);
                        pNewRtrNode->u1Flags = u1Temp;

                        tlvPtr += 3;    /*Algo + Reserved */

                        u4Temp = SR_LGET4BYTE (tlvPtr);
                        u4sidVal = (UINT4) u4Temp;
                        if (isPeerExist == SR_TRUE)
                        {
                            if (u4sidVal != pNewRtrNode->u4SidValue)
                            {
                                SR_TRC (SR_CTRL_TRC, "SID Value Changed \n");
                                /* SID Value Changed, So delete all the FTNs with all NH for this SrRtrIP */
                                /* To delete TE LSP in Type10 flow */
                                if (SrTeDeleteFtnType10 (&destAddr, &u4ErrCode)
                                    == SR_SUCCESS)
                                {
                                    if (u4ErrCode == SR_TE_VIA_DEST)
                                    {
                                        SR_TRC3 (SR_CTRL_TRC,
                                                 "%s:%d SR TE LSP deleted via dest in Type10 flow for prefix %x \n",
                                                 __func__, __LINE__,
                                                 destAddr.Addr.Ip6Addr.u1_addr);
                                    }
                                }
                                /* Sr-TE: End */
                                if (u4ErrCode != SR_TE_VIA_DEST)
                                {
                                    if (SR_FAILURE ==
                                        SrDeleteMplsEntry (pRtrInfo))
                                    {
                                        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC,
                                                "Unable to Delete MPLS Entry \n");
                                    }
                                }

                            }
                        }
                        pNewRtrNode->u4SidValue = u4sidVal;
                        pNewRtrNode->u1SidIndexType = SR_SID_TYPE_INDEX;
                    }
                }
            }
        }
    }
    else
    {
        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "Invalid Opaque LSA Type not supported by SR \n");
        return SR_FAILURE;
    }

    if (isPeerExist == SR_FALSE)
    {
        TMO_SLL_Insert (&(gSrGlobalInfo.routerList), NULL,
                        &(pNewRtrNode->nextRtrId));
    }

    if (pOsLsaInfo->u1LsaStatus == NEW_LSA)
    {
        u1LSASent = SR_FALSE;
    }
    else
    {
        u1LSASent = pNewRtrNode->u1LSASent;
    }

    if (pNewRtrNode->u1LSASent == SR_FALSE
        || pOsLsaInfo->u1LsaStatus == NEW_LSA)
    {
        pNewRtrNode->u1LSASent = SR_TRUE;
        if (SR_FAILURE == SrV3UtilSendLSA (NEW_LSA_INSTANCE, SR_ZERO))
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d Unable to Send TYPE10_OPQ_LSA Self LSA \n", __func__,
                     __LINE__);
        }
    }

    if (SR_TRUE == i4AbrStatus)
    {
        u2lsaType = OSPFV3_EXT_INTER_AREA_PREFIX_LSA;
        SR_TRC6 (SR_CTRL_TRC,
                 "LSAType= %d PrfxAddr=%s, AdvRtrId= %x LSASentFlag=%d, NodeLSAFlag=%d, lsatype=%d\r\n",
                 u2LsaType, Ip6PrintAddr (&prefixAddr), u4AdvRtrId, u1LSASent,
                 pNewRtrNode->u1LSASent, pOsLsaInfo->u1LsaStatus);

        pRtrNode = NULL;
        TMO_SLL_Scan (&(gSrGlobalInfo.routerList), pRtrNode, tTMO_SLL_NODE *)
        {
            pRtrInfo1 = (tSrRtrInfo *) pRtrNode;
            MEMCPY (&Ipv6Addr.u1_addr, pRtrInfo1->prefixId.Addr.Ip6Addr.u1_addr,
                    SR_IPV6_ADDR_LENGTH);

            if ((MEMCMP (Ipv6Addr.u1_addr, au1NullIpv6Addr, SR_IPV6_ADDR_LENGTH)
                 != SR_ZERO) && (pRtrInfo1->u4SidValue != SR_ZERO)
                && ((u1LSASent == SR_FALSE)
                    || (pRtrInfo1->u1LSASent == SR_FALSE))
                && (pRtrInfo1->u2AddrType == SR_IPV6_ADDR_TYPE))
            {
                SrV3ConstructSendExtLsa (NEW_LSA, pRtrInfo1->u4SidValue,
                                         Ipv6Addr, pNewRtrNode->u4areaId,
                                         u2lsaType);
                pRtrInfo1->u1LSASent = SR_TRUE;
            }
            else if (pRtrInfo1->u4SidValue == SR_ZERO)
            {
                pRtrInfo1->u1LSASent = SR_FALSE;
            }
            MEMSET (Ipv6Addr.u1_addr, SR_ZERO, SR_IPV6_ADDR_LENGTH);
        }
    }

    SR_TRC4 (SR_CTRL_TRC, "%s:%d SID %d SrGbMinIndex %d \n", __func__, __LINE__,
             pNewRtrNode->u4SidValue, pNewRtrNode->srgbRange.u4SrGbV3MinIndex);

    if ((((u2LsaType == OSPFV3_EXT_INTER_AREA_PREFIX_LSA))
         && (pNewRtrNode->u4SidValue != SR_ZERO)) ||
        ((pNewRtrNode->u4SidValue != SR_ZERO) &&
         (pNewRtrNode->srgbRange.u4SrGbV3MinIndex != SR_ZERO)))
    {
        SR_TRC3 (SR_CTRL_TRC, "%s:%d Checking if TE is configured for %x \n",
                 __func__, __LINE__, pOsLsaInfo->u4AdvRtrId);
        /* Sr-TE: Start */
        /* To create TE LSP in the Type10 flow */
        /* It will also create TE LFA Lsp, when SR_LFA is enabled */
        if (SrTeCreateFtnType10 (&destAddr, &u4ErrCode) == SR_SUCCESS)
        {
            if (u4ErrCode == SR_TE_VIA_DEST)
            {
                SR_TRC3 (SR_CTRL_TRC,
                         "%s:%d SR TE LSP processed via dest in Type10 flow for prefix %x \n",
                         __func__, __LINE__, destAddr.Addr.u4Addr);
                SR_UNLOCK ();
                return SR_SUCCESS;
            }
        }
        else
        {
            if (u4ErrCode == SR_TE_VIA_DEST)
            {
                SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%s:%d SR TE LSP processing via dest in Type10 flow for prefix %x is failed \n",
                         __func__, __LINE__, destAddr.Addr.u4Addr);
                SR_UNLOCK ();
                return SR_FAILURE;
            }
        }

        /* Sr-TE: End */
        /* SR LSP processing for non-TE prefix starts */
        /* Case1: No Existing FTN/SrNextHopInfo present */
        if (TMO_SLL_Count (&(pNewRtrNode->NextHopList)) == SR_ZERO)
        {
            SR_TRC3 (SR_CTRL_TRC, "%s:%d SID %d \n", __func__, __LINE__,
                     pNewRtrNode->u4SidValue);

            /* Non-TE: Start */
            SrCreateMplsEntry (pNewRtrNode, NULL);
            if (SrCreateMplsBindingOnNbrSrUp (&destAddr, u4AdvRtrId) ==
                SR_FAILURE)
            {
                SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC,
                         "%x RouterId is not Neighbour of any other node \n",
                         pOsLsaInfo->u4AdvRtrId);
            }
            /* Non-TE: End */
        }
        else
        {
            /* Case2: Existing FTN/SrNextHopInfo present */

            TMO_SLL_Scan (&(pNewRtrNode->NextHopList),
                          pSrRtrNextHopNode, tTMO_SLL_NODE *)
            {
                pSrRtrNextHopInfo = (tSrRtrNextHopInfo *) pSrRtrNextHopNode;

                if ((pSrRtrNextHopInfo->u1MPLSStatus & SR_FTN_CREATED) ==
                    SR_FTN_CREATED)
                {
                    u1FtnCreated = SR_TRUE;
                    continue;
                }

                /* Non-TE: Start */
                SrCreateMplsEntry (pNewRtrNode, pSrRtrNextHopInfo);

                /* Non-TE: End */
            }

            /* If FTN is created, POP & Fwd will not be present */
            if (u1FtnCreated == SR_FALSE)
            {
                if (SrCreateMplsBindingOnNbrSrUp (&destAddr, u4AdvRtrId) ==
                    SR_FAILURE)
                {
                    SR_TRC1 (SR_CTRL_TRC | SR_FAIL_TRC,
                             "%x RouterId is not Neighbour of any other node \n",
                             u4DestAddr);
                }
                else
                {
                    SR_TRC1 (SR_CTRL_TRC,
                             "%x Mpls Binding on NbrSrUp Completed for %x \n",
                             u4DestAddr);
                }
            }
        }
    }
    return SR_SUCCESS;
}

/*****************************************************************************
 * Function Name : SrProcessOspfNbrADjSidILMUpdate
 * Description   : This routine create or delete ILM entry based on Nbr add/del
 * Input(s)      : pSrOspfNbrInfo - Pointer to SID structure
 *                 isILMDel  - ILM Add/del
 * Output(s)     : None
 * Return(s)     : SR_SUCCESS / SR_FAILURE
 *****************************************************************************/
PUBLIC INT4
SrProcessOspfNbrADjSidILMUpdate(tSrV4OspfNbrInfo  *pSrOspfNbrInfo, UINT1  isILMDel)
{
    tSrSidInterfaceInfo  SrSidIntf       = {0};
    tSrSidInterfaceInfo *pSrSidEntry     = NULL;
    UINT2                u2MlibOperation = 0;
    UINT4                u4OutIfIndex    = 0;
    tGenU4Addr           nextHop;
    tGenU4Addr           destPrefix;

    if (NULL == pSrOspfNbrInfo)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                "%s:%d Pointer to SID structure is NULL \n",__func__, __LINE__);
        return SR_FAILURE;
    }

    /* set Adj SID key parameters to search for config node */
    SrSidIntf.ipAddrType           =  SR_IPV4_ADDR_TYPE;

    /* intf ip address on which adj sid may be configured */
    SrSidIntf.ifIpAddr.Addr.u4Addr = pSrOspfNbrInfo->u4IfIpAddr;

    /* get the config node from the Sid tree */
    pSrSidEntry = RBTreeGet (gSrGlobalInfo.pSrSidRbTree, &SrSidIntf);

    if (NULL == pSrSidEntry)
    {
        return SR_SUCCESS;
    }

    destPrefix.u2AddrType  = pSrSidEntry->ifIpAddr.u2AddrType;
    destPrefix.Addr.u4Addr = pSrSidEntry->ifIpAddr.Addr.u4Addr;

    nextHop.u2AddrType  =  pSrSidEntry->ifIpAddr.u2AddrType;
    nextHop.Addr.u4Addr =  pSrOspfNbrInfo->ospfV4NbrInfo.u4NbrIpAddr;

#ifdef CFA_WANTED
    if (CfaIpIfGetIfIndexFromHostIpAddressInCxt (MPLS_DEF_CONTEXT_ID,
            pSrOspfNbrInfo->u4IfIpAddr, &u4OutIfIndex) ==  CFA_FAILURE)
    {
        SR_TRC3 (SR_FAIL_TRC | SR_CRITICAL_TRC,
                "%s:%d Unable to fetch interface index for IP :0x%x \n",
                 __func__, __LINE__, pSrOspfNbrInfo->u4IfIpAddr);
        return SR_FAILURE;
    }
#endif

    if (SR_TRUE == isILMDel)
    {
        u2MlibOperation = MPLS_MLIB_ILM_DELETE;
    }
    else
    {
        u2MlibOperation = MPLS_MLIB_ILM_CREATE;
    }

    gSrGlobalInfo.u4CliCmdMode = SR_TRUE;

    if (SrUtilCreateorDeleteILM (pSrSidEntry, pSrSidEntry->u4PrefixSidLabelIndex,
                                &destPrefix, &nextHop, (UINT2) u2MlibOperation,
                                 u4OutIfIndex) == SR_FAILURE)
    {
        if(MPLS_MLIB_ILM_CREATE == u2MlibOperation)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                "%s:%d Error in adding MPLS ILM for Adj Sid (%u) in config flow \n",
                __func__, __LINE__, pSrSidEntry->u4PrefixSidLabelIndex);
        }
        else
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC | SR_CRITICAL_TRC,
                "%s:%d Error in deleting MPLS ILM for Adj Sid (%u) in config flow \n",
                __func__, __LINE__, pSrSidEntry->u4PrefixSidLabelIndex);
        }
        SR_TRC (SR_FAIL_TRC | SR_CRITICAL_TRC,
                "% Failure: SrUtilCreateorDeleteILM Failed\n");
        return SR_FAILURE;
    }

    gSrGlobalInfo.u4CliCmdMode = SR_FALSE;

    /* set or reset ILM entry create flag based on ILM operation */
    if (MPLS_MLIB_ILM_CREATE == u2MlibOperation)
    {
        pSrSidEntry->u1AdjSidInfoFlags |= SR_ADJ_SID_ILM_CREATE;
        SR_TRC3 (SR_CRITICAL_TRC | SR_CTRL_TRC,
                "%s:%d Added MPLS ILM for Adj Sid (%d) in Nbr processing flow\n",
                __func__, __LINE__, pSrSidEntry->u4PrefixSidLabelIndex);
    }
    else
    {
        pSrSidEntry->u1AdjSidInfoFlags &= ~SR_ADJ_SID_ILM_CREATE;
        SR_TRC3 (SR_CRITICAL_TRC | SR_CTRL_TRC,
                "%s:%d Deleted MPLS ILM for Adj Sid (%d) in Nbr processing flow \n",
                __func__, __LINE__, pSrSidEntry->u4PrefixSidLabelIndex);
    }

    return SR_SUCCESS;

}

PUBLIC INT4
SrProcessOspfMsg (tOsToOpqApp * pOspfMsg)
{
    tOsSrArea          *pOsSrArea = NULL;
    UINT4               u4OspfSrCxtId = OSPF_DEFAULT_CXT_ID;
    tIPADDR             rtrId;
    INT4                i4SrStatus = SR_ZERO;
    MEMSET (&rtrId, 0, sizeof (UINT4));

    UNUSED_PARAM (u4OspfSrCxtId);
    nmhGetFsSrV4Status (&i4SrStatus);
    if (i4SrStatus == SR_DISABLED)
    {
        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "SrStatus is disabled \n");
        return SR_FAILURE;
    }

    switch (pOspfMsg->lsaInfo.u1LsaType)
    {
        case ROUTER_LSA:
            if (SrProcessRtrLsa (pOspfMsg, pOsSrArea) != SR_SUCCESS)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                        "%s:%d : SrProcessRtrLsa Failed \n",
                        __func__, __LINE__);
                return SR_FAILURE;
            }
            break;

        case NETWORK_LSA:
            if (SrProcessNetworkLsa (pOspfMsg, pOsSrArea) != SR_SUCCESS)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                        "%s:%d : SrProcessNetworkLsa Failed \n",
                        __func__, __LINE__);
                return SR_FAILURE;
            }
            break;
        case TYPE10_OPQ_LSA:

        case TYPE11_OPQ_LSA:
            if (SrProcessOpqType10Lsa (pOspfMsg, pOsSrArea) != SR_SUCCESS)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                        "%s:%d : SrProcessOpqType10Lsa Failed \n",
                        __func__, __LINE__);
                return SR_FAILURE;
            }
            break;

        default:
            SR_TRC1 (SR_CTRL_TRC, "Default case for LSA : %d \n",
                     pOspfMsg->u4MsgSubType);
            break;
    }
    return SR_SUCCESS;
}

PUBLIC INT4
SrProcessOspfV3Msg (tV3OsToOpqApp * pOspfMsg)
{
    UINT4               u4OspfSrCxtId = OSPF_DEFAULT_CXT_ID;
    tIPADDR             rtrId;
    INT4                i4SrStatus = SR_ZERO;
    MEMSET (&rtrId, 0, sizeof (UINT4));

    UNUSED_PARAM (u4OspfSrCxtId);
    nmhGetFsSrV6Status (&i4SrStatus);
    if (i4SrStatus == SR_DISABLED)
    {
        SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "SrStatus is disabled \n");
        return SR_FAILURE;
    }

    switch (pOspfMsg->o3LsaInfo.u2LsaType)
    {
#if 0
        case ROUTER_LSA:
            if (SrProcessRtrLsa (pOspfMsg, pOsSrArea) != SR_SUCCESS)
            {
                return SR_FAILURE;
            }
            break;
#endif
        case OSPFV3_AREA_SCOPE_OPQ_LSA:
        case OSPFV3_EXT_INTRA_AREA_PREFIX_LSA:
        case OSPFV3_EXT_INTER_AREA_PREFIX_LSA:
            if (SrV3ProcessOpqType10Lsa (pOspfMsg) != SR_SUCCESS)
            {
                SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                        "%s:%d : SrV3ProcessOpqType10Lsa Failed \n",
                        __func__, __LINE__);
                return SR_FAILURE;
            }
            break;
        default:
            SR_TRC1 (SR_CTRL_TRC, "Default case for LSA : %d \n",
                     pOspfMsg->u4MsgSubType);
            break;
    }
    return SR_SUCCESS;
}

PUBLIC UINT4
SrGetAddrPrefix (tOsLsaInfo * pOsLsaInfo)
{
    UINT1              *tlvPtr = NULL;
    UINT1              *pTemp = NULL;
    UINT1               u1Temp = SR_ZERO;
    UINT2               u2Temp = SR_ZERO;
    UINT4               u4Temp = SR_ZERO;

    tlvPtr = pOsLsaInfo->pu1Lsa;

    pTemp = tlvPtr + 4;
    u1Temp = SR_LGET1BYTE (pTemp);
    if (u1Temp == OSPF_EXT_PRREFIX_LSA_OPAQUE_TYPE)
    {
        tlvPtr = pOsLsaInfo->pu1Lsa + 20;
        pTemp = tlvPtr;
        u2Temp = SR_LGET2BYTE (pTemp);
        if (u2Temp == OSPF_EXT_PRREFIX_TLV_TYPE)
        {
            tlvPtr += 8;
            MEMCPY (&u4Temp, tlvPtr, sizeof (UINT4));

            return u4Temp;
        }
    }
    return SR_ZERO;
}

PUBLIC UINT4
SrV3GetAddrPrefix (tOs3LsaInfo * pOsLsaInfo, tIp6Addr * pPrefixAddr)
{
    UINT1              *tlvPtr = NULL;
    UINT1              *pTemp = NULL;
    UINT2               u2Temp = SR_ZERO;

    tlvPtr = pOsLsaInfo->pu1Lsa;

    pTemp = tlvPtr + 2;
    u2Temp = SR_LGET2BYTE (pTemp);
    if (u2Temp == OSPFV3_EXT_INTRA_AREA_PREFIX_LSA)
    {
        tlvPtr = pOsLsaInfo->pu1Lsa + OSPFV3_EXT_INTRA_AREA_PREFIX_LSA_HDR_LEN;
        u2Temp = SR_LGET2BYTE (tlvPtr);
        if (u2Temp == OSPFV3_INTRA_AREA_PRFIX_TLV_TYPE)
        {
            u2Temp = SR_LGET2BYTE (tlvPtr);
            if (u2Temp == OSPFV3_INTRA_AREA_PRFIX_TLV_LEN)
            {
                tlvPtr += 8;
                MEMCPY (pPrefixAddr->u1_addr, tlvPtr, SR_IPV6_ADDR_LENGTH);
            }
        }
    }
    else if (u2Temp == OSPFV3_EXT_INTER_AREA_PREFIX_LSA)
    {
        tlvPtr = pOsLsaInfo->pu1Lsa + OSPFV3_EXT_INTER_AREA_PREFIX_LSA_HDR_LEN;
        u2Temp = SR_LGET2BYTE (tlvPtr);
        if (u2Temp == OSPFV3_INTER_AREA_PRFIX_TLV_TYPE)
        {
            u2Temp = SR_LGET2BYTE (tlvPtr);
            if (u2Temp == OSPFV3_INTER_AREA_PRFIX_TLV_LEN)
            {
                tlvPtr += 8;
                MEMCPY (pPrefixAddr->u1_addr, tlvPtr, SR_IPV6_ADDR_LENGTH);
            }
        }
    }
    else if ((0xc & u2Temp) == V3_RI_LSA_OPAQUE_TYPE)
    {
        tlvPtr = pOsLsaInfo->pu1Lsa + 20;
        u2Temp = SR_LGET2BYTE (tlvPtr);
        if (u2Temp == OSPFV3_INTRA_AREA_PRFIX_TLV_TYPE)
        {
            u2Temp = SR_LGET2BYTE (tlvPtr);
            if (u2Temp == 24)
            {
                tlvPtr += 8;
                MEMCPY (pPrefixAddr->u1_addr, tlvPtr, SR_IPV6_ADDR_LENGTH);
            }
        }
    }
    else
    {
        return SR_FAILURE;
    }
    return SR_SUCCESS;
}

PUBLIC VOID
SrConstructSendExtPrefixOpqLsa (UINT1 u1LsaStatus, UINT4 nodeLabel,
                                UINT4 advRtrID, UINT4 u4AreaId, UINT1 u1SrSid)
{
    tOpqLSAInfo         OpqLSAInfo;
    UINT1               au1RtrAddrTlv[SR_RI_LSA_TLV_LEN];
    UINT1              *pRtrAdrTlv = NULL;
    UINT1              *pCurrent = NULL;
    UINT1              *pTlvLength = NULL;
    UINT4               u4AddressPrefix = SR_ZERO;
    UINT4               u4AdvRtrID = SR_ZERO;
    UINT4               u4NodeSid = 0;

    u4AdvRtrID = gSrGlobalInfo.u4Ospfv2RtrID;
    MEMCPY (&u4AddressPrefix, &advRtrID, sizeof (UINT4));
    MEMSET (&OpqLSAInfo, 0, sizeof (tOpqLSAInfo));
    MEMSET (au1RtrAddrTlv, 0, sizeof (SR_RI_LSA_TLV_LEN));
    OpqLSAInfo.pu1LSA = au1RtrAddrTlv;

    pRtrAdrTlv = OpqLSAInfo.pu1LSA;
    pCurrent = pRtrAdrTlv;

    u4NodeSid = nodeLabel;
    /** Create the SR-ALgorithm TLV **/

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) OSPF_EXT_PRREFIX_TLV_TYPE);

    /* Filling Length Value */
    pTlvLength = pCurrent;
    UNUSED_PARAM (pTlvLength);
    SR_LADD2BYTE (pCurrent, (UINT2) OSPF_EXT_PRREFIX_TLV_LENGTH);    /*ExtPrefixTlv Data + SidLabelSubTlv */

    /*Filling Route Type */
    SR_LADD1BYTE (pCurrent, (UINT1) OSPF_INTRA_AREA);

    /*Filling Prefix Length */
    SR_LADD1BYTE (pCurrent, (UINT1) (SR_IPV4_ADDR_LENGTH * 8));

    /*Filling Address Family */
    SR_LADD1BYTE (pCurrent, (UINT1) SR_ZERO);    /*Address Family --> IPv4 */

    /*Filling Flags */
    SR_LADD1BYTE (pCurrent, (UINT1) 0x40);    /*N-bit is enabled */

    /*Filling Address Prefix */
    SR_LADD4BYTE (pCurrent, u4AddressPrefix);

    /* Add SID/Label Range sub-TLV */

    if (SrConstructPrefixSidSubTlv (&pCurrent, SR_SID_TYPE_INDEX, nodeLabel)
        != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                "%s:%d : SrConstructPrefixSidSubTlv Failed \n",
                __func__, __LINE__);
        return;
    }

    OpqLSAInfo.LsId[SR_ZERO] = OSPF_EXT_PRREFIX_LSA_OPAQUE_TYPE;
    OpqLSAInfo.LsId[SR_TWO] = (UINT1) (u4NodeSid & SR_MASK_TYPE1);
    OpqLSAInfo.LsId[SR_THREE] = (UINT1) (u4NodeSid & SR_MASK_TYPE2);

    SR_TRC4 (SR_CTRL_TRC, "filling Unique ID to OPQ APP  LSID :%d.%d.%d.%d\n",
             OpqLSAInfo.LsId[0], OpqLSAInfo.LsId[1], OpqLSAInfo.LsId[2],
             OpqLSAInfo.LsId[3]);

    MEMCPY (&OpqLSAInfo.AdvRtrID, &u4AdvRtrID, sizeof (UINT4));

    OpqLSAInfo.u4AreaID = u4AreaId;
    OpqLSAInfo.u2LSALen = (UINT2) (pCurrent - pRtrAdrTlv);

    OpqLSAInfo.u1OpqLSAType = TYPE10_OPQ_LSA;

    if (u1LsaStatus == FLUSHED_LSA)
    {
        OpqLSAInfo.u1LSAStatus = NEW_LSA_INSTANCE;
        OpqLSAInfo.i1SrSid = SR_INVALID_SID;
        OpqLSAInfo.u1FlushTriggr = SR_TRUE;
    }
    else
    {
        OpqLSAInfo.u1LSAStatus = u1LsaStatus;
        OpqLSAInfo.i1SrSid = (INT1) u1SrSid;
    }

    if (gSrGlobalInfo.u1OspfAbrStatus == SR_TRUE)
    {
        OpqLSAInfo.u4PrefixAddr = u4AddressPrefix;
    }

    if (SrSndMsgToOspf (SR_OSPF_SEND_OPQ_LSA,
                        OSPF_DEFAULT_CXT_ID, &OpqLSAInfo) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : ExtPrefixOpqLsa Sent Failed \n", __func__,
                 __LINE__);
        return;
    }
    return;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrV3ConstructSendExtLsa                                    */
/*                                                                           */
/* Description  : This function construct and Send a ospf v3 External LSAs   */
/*                                                                           */
/*                                                                           */
/* Input        : u1LsaStatus, nodeLabel, Ipv6Address, u4AreaId, u2lsaType   */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : None                                                       */
/*****************************************************************************/
PUBLIC VOID
SrV3ConstructSendExtLsa (UINT1 u1LsaStatus, UINT4 nodeLabel,
                         tIp6Addr Ipv6Address, UINT4 u4AreaId, UINT2 u2lsaType)
{
    tV3OpqLSAInfo       OpqLSAInfo;
    UINT1               au1RtrAddrTlv[V3_SR_RI_LSA_TLV_LEN];
    UINT1              *pRtrAdrTlv = NULL;
    UINT1              *pCurrent = NULL;
    UINT4               u4AdvRtrID = SR_ZERO;
    UINT4               u4NodeSid = SR_ZERO;
    UINT2               u4UniqID = SR_ZERO;

    MEMSET (&OpqLSAInfo, 0, sizeof (tV3OpqLSAInfo));
    MEMSET (au1RtrAddrTlv, 0, sizeof (V3_SR_RI_LSA_TLV_LEN));
    OpqLSAInfo.pu1LSA = au1RtrAddrTlv;

    u4AdvRtrID = gSrGlobalInfo.u4OspfRtrID;

    pRtrAdrTlv = OpqLSAInfo.pu1LSA;
    pCurrent = pRtrAdrTlv;
    u4NodeSid = nodeLabel;

    /* Filling Type value */
    if (u2lsaType == OSPFV3_EXT_INTRA_AREA_PREFIX_LSA)
    {
        SR_LADD2BYTE (pCurrent, (UINT2) OSPFV3_INTRA_AREA_PRFIX_TLV_TYPE);
    }
    else
    {
        SR_LADD2BYTE (pCurrent, (UINT2) OSPFV3_INTER_AREA_PRFIX_TLV_TYPE);
    }

    /* Filling Length Value */
    SR_LADD2BYTE (pCurrent, (UINT2) OSPFV3_EXTENDED_PREFIX_TLV_LEN);

    /* Filling 0 & Metric */
    SR_LADD4BYTE (pCurrent, (SR_ZERO));

    /* Filling prefix length */
    SR_LADD1BYTE (pCurrent, 128);

    SR_LADD3BYTE (pCurrent, (SR_ZERO));

    /* Filling Ipv6 Addr */
    SR_LADDSTR (pCurrent, &Ipv6Address, SR_IPV6_ADDR_LENGTH);
    if (u1LsaStatus == FLUSHED_LSA)
    {
        nodeLabel = 0;
    }

    /* Filling SID Sub-TLV */
    if (SrConstructPrefixSidSubTlvV3 (&pCurrent, SR_SID_TYPE_INDEX, nodeLabel)
        != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : SrConstructPrefixSidSubTlvV3 Failed \n",
                __func__,__LINE__);
        return;
    }

    if (u2lsaType == OSPFV3_EXT_INTRA_AREA_PREFIX_LSA)
    {
        OpqLSAInfo.LsId[SR_ZERO] = OSPFV3_EXT_INTRA_AREA_PREFIX_TLV;
    }
    else
    {
        OpqLSAInfo.LsId[SR_ZERO] = OSPFV3_EXT_INTER_AREA_PREFIX_TLV;
    }

    u4UniqID = (UINT2) u4NodeSid;
    OpqLSAInfo.LsId[SR_TWO] = (UINT1) (u4UniqID & SR_MASK_TYPE1);
    OpqLSAInfo.LsId[SR_THREE] = (UINT1) (u4UniqID & SR_MASK_TYPE2);

    MEMCPY (OpqLSAInfo.AdvRtrID, &u4AdvRtrID, sizeof (UINT4));

    OpqLSAInfo.u2SubType = OSPFV3_ROUTER_LSA;
    OpqLSAInfo.u4AreaID = u4AreaId;
    OpqLSAInfo.u2LsaLen = (UINT2) (pCurrent - pRtrAdrTlv);
    OpqLSAInfo.u2OpqLSAType = u2lsaType;

    if (u1LsaStatus == FLUSHED_LSA)
    {
        OpqLSAInfo.u1LSAStatus = NEW_LSA_INSTANCE;
        OpqLSAInfo.i1SrSid = SR_INVALID_SID;
    }
    else
    {
        OpqLSAInfo.u1LSAStatus = u1LsaStatus;
        OpqLSAInfo.i1SrSid = SR_ONE;
    }

    if (SrSndMsgToOspfV3 (SR_OSPFV3_SEND_OPQ_LSA,
                          OSPF_DEFAULT_CXT_ID, &OpqLSAInfo) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : SrV3ConstructSendExtLsa Sent Failed \n",
                 __func__, __LINE__);
        return;
    }
    return;
}

PUBLIC INT4
SrConstructPrefixTlv (UINT1 **pTempCurrent, tSrSidInterfaceInfo *pSrSidIntf)
{
    UINT1              *pTlvLength = NULL;
    UINT1              *pCurrent = NULL;
    pCurrent = *pTempCurrent;
    UINT4               u4AddressPrefix = SR_ZERO;

    MEMCPY (&u4AddressPrefix, &pSrSidIntf->ifIpAddr.Addr.u4Addr,
            sizeof (UINT4));

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) OSPF_EXT_PRREFIX_TLV_TYPE);

    /* Filling Length Value */
    pTlvLength = pCurrent;
    SR_LADD2BYTE (pCurrent, (UINT2) (8));    /*ExtPrefixTlv Data + SidLabelSubTlv */

    /*Filling Route Type */
    SR_LADD1BYTE (pCurrent, (UINT1) OSPF_INTRA_AREA);

    /*Filling Prefix Length */
    SR_LADD1BYTE (pCurrent, (UINT1) (SR_IPV4_ADDR_LENGTH * 8));

    /*Filling Address Family */
    SR_LADD1BYTE (pCurrent, (UINT1) SR_ZERO);    /*Address Family --> IPv4 */

    /*Filling Flags */
    SR_LADD1BYTE (pCurrent, (UINT1) EXT_PREFIX_N_FLAG);    /*N-bit is enabled */

    /*Filling Address Prefix */
    SR_LADD4BYTE (pCurrent, (UINT4) u4AddressPrefix);
    *pTempCurrent = pCurrent;
    UNUSED_PARAM (pTlvLength);
    return SR_SUCCESS;
}

PUBLIC INT4
SrConstructSidLabelSubTlvV3 (UINT1 **pTempCurrent,
                             UINT4 SidLabelType, UINT4 u4Val)
{
    UINT1              *pCurrent = NULL;
    pCurrent = *pTempCurrent;

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) SID_LABEL_SUB_TLV_TYPE_V3);

    /* Filling Length Value */
    if (SidLabelType == SR_SID_TYPE_LABEL)
    {
        SR_LADD2BYTE (pCurrent, (UINT2) SID_LABEL_SUB_TLV_LABEL_LENGTH);

        /* Filling SID Value  */
        SR_LADD3BYTE (pCurrent, u4Val);
        SR_LADD1BYTE (pCurrent, SR_ZERO);
    }
    else if (SidLabelType == SR_SID_TYPE_INDEX)
    {
        SR_LADD2BYTE (pCurrent, (UINT2) SID_LABEL_SUB_TLV_SID_LENGTH);
        /* Filling SID Value  */
        SR_LADD4BYTE (pCurrent, u4Val);
    }
    else
    {
        return SR_FAILURE;
    }

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

PUBLIC INT4
SrConstructRtrInfoSidLabelRangeTlvV3 (UINT1 **pTempCurrent)
{
    UINT1              *pCurrent = NULL;
    pCurrent = *pTempCurrent;

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) RI_LSA_SR_SID_LABEL_RANGE_TLV_TYPE);

    /* Filling Length Value */
    SR_LADD2BYTE (pCurrent, (UINT2) RI_LSA_SR_SID_LABEL_RANGE_TLV_LEN);

    /* Filling Range Size */
    SR_LADD3BYTE (pCurrent, (SR_V6_SRGB_MAX_VAL - SR_V6_SRGB_MIN_VAL + SR_ONE));

    /* Filling Reserved field */
    SR_LADD1BYTE (pCurrent, (UINT1) SR_ZERO);

    /* Add SID/Label Range sub-TLV */
    if (SrConstructSidLabelSubTlvV3
        (&pCurrent, SR_SID_TYPE_LABEL, SR_V6_SRGB_MIN_VAL) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : SrConstructSidLabelSubTlvV3 Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

PUBLIC INT4
SrConstructPrefixSidSubTlvV3 (UINT1 **pTempCurrent,
                              UINT4 SidLabelType, UINT4 u4Val)
{
    UINT1              *pCurrent = NULL;
    pCurrent = *pTempCurrent;

    /* Filling Type value */
    SR_LADD2BYTE (pCurrent, (UINT2) PREFIX_SID_SUB_TLV_TYPE_V3);

    /* Filling Length Value */
    SR_LADD2BYTE (pCurrent, (UINT2) PREFIX_SID_SUB_TLV_LEN_V3);

    /* Filling Flags */
    SR_LADD1BYTE (pCurrent, (PREFIX_SID_NP_FLAG));

    /* Filling Algo field */
    SR_LADD1BYTE (pCurrent, (UINT1) RI_LSA_SR_ALGO_SPF);

    /* Filling Reserved field */
    SR_LADD2BYTE (pCurrent, (UINT1) SR_ZERO);

    /* Filling SID Value */
    if (SidLabelType == SR_SID_TYPE_LABEL)
    {
        SR_LADD3BYTE (pCurrent, u4Val);
        SR_LADD1BYTE (pCurrent, SR_ZERO);
    }
    else
    {
        SR_LADD4BYTE (pCurrent, u4Val);
    }

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

PUBLIC INT4
SrConstructRtrInfoLsaV3 (tV3OpqLSAInfo * pOpqLSAInfo,
                         UINT1 u1LsaStatus, UINT2 u2LsaType)
{
    UINT1              *pRtrAdrTlv = NULL;
    UINT1              *pCurrent = NULL;
    UINT4               u4AdvRtrID = SR_ZERO;
    UINT4               u4AreaId = SR_ZERO;
    UINT4               u4IpAddr = SR_ZERO;
    UINT4               u4IfIndex = SR_ZERO;
    UINT4               u4nodeLabel = SR_ZERO;

    MEMCPY (&u4IpAddr, &gSrGlobalInfo.SrContextV3.rtrId.Addr.u4Addr,
            sizeof (UINT4));

    u4AdvRtrID = gSrGlobalInfo.u4OspfRtrID;

#ifdef IP6_WANTED
    /* Fetching Area */
    IP6_TASK_LOCK ();
    NetIp6AddrTblGetIfIndexFromIp6Add (&u4IfIndex,
                                       &gSrGlobalInfo.SrContextV3.rtrId.
                                       Addr.Ip6Addr, 128);
    IP6_TASK_UNLOCK ();
#endif
    /* Fetching Area ID based on Interface Index */
    SrUtilFetchOspf3AreadID (u4IfIndex, &u4AreaId);

    pRtrAdrTlv = pOpqLSAInfo->pu1LSA;
    pCurrent = pRtrAdrTlv;

    SR_GET_V6_SELF_SID_INDEX_VAL (&u4nodeLabel);
    /* Construct External TLV Header */
    if (SrConstructV3ExtendedPrefixTlv
        (&pCurrent, OSPFV3_EXT_INTRA_AREA_PREFIX_LSA) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : SrConstructV3ExtendedPrefixTlv Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    /** Create the SR-ALgorithm TLV **/
    if (SrConstructRtrInfoSrAlgoTlv (&pCurrent) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : SrConstructRtrInfoSrAlgoTlv Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }
    /*** Create SID/Label Range TLV ***/
    if (SrConstructRtrInfoSidLabelRangeTlvV3 (&pCurrent) != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : SrConstructRtrInfoSidLabelRangeTlvV3 Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    /*** RI LSA Header **/
    pOpqLSAInfo->LsId[SR_ZERO] = AREA_SCOPE_OPQ_LSA_MASK;
    pOpqLSAInfo->LsId[SR_TWO] = (UINT1) (u4nodeLabel & SR_MASK_TYPE1);
    pOpqLSAInfo->LsId[SR_THREE] = (UINT1) (u4nodeLabel & SR_MASK_TYPE2);

    MEMCPY (pOpqLSAInfo->AdvRtrID, &u4AdvRtrID, sizeof (UINT4));

    pOpqLSAInfo->u4AreaID = u4AreaId;
    pOpqLSAInfo->u2LsaLen = (UINT2) (pCurrent - pRtrAdrTlv);
    pOpqLSAInfo->u2OpqLSAType = u2LsaType;
    SR_UPDT_LSA_FLAG (u1LsaStatus, pOpqLSAInfo);
    return SR_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrConstructV3ExtendedPrefixTlv                             */
/*                                                                           */
/* Description  : This function construct External Prefix TLV                */
/*                                                                           */
/*                                                                           */
/* Input        : pTempCurrent, u2lsaType                                    */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : SUCCESS/FAILURE                                            */
/*****************************************************************************/
PUBLIC INT4
SrConstructV3ExtendedPrefixTlv (UINT1 **pTempCurrent, UINT2 u2lsaType)
{
    UINT1              *pCurrent = NULL;
    pCurrent = *pTempCurrent;

    /* Filling Type value */
    if (u2lsaType == OSPFV3_EXT_INTRA_AREA_PREFIX_LSA)
    {
        SR_LADD2BYTE (pCurrent, (UINT2) OSPFV3_INTRA_AREA_PRFIX_TLV_TYPE);
    }
    else if (u2lsaType == OSPFV3_EXT_INTER_AREA_PREFIX_LSA)
    {
        SR_LADD2BYTE (pCurrent, (UINT2) OSPFV3_INTER_AREA_PRFIX_TLV_TYPE);
    }

    /* Filling Length Value */
    SR_LADD2BYTE (pCurrent, (UINT2) 24);

    /* Filling 0 & Metric */
    SR_LADD4BYTE (pCurrent, (SR_ZERO));

    /* Filling prefix length */
    SR_LADD1BYTE (pCurrent, 128);

    SR_LADD3BYTE (pCurrent, (SR_ZERO));

    /* Filling Ipv6 Addr */
    SR_LADDSTR (pCurrent, &gSrGlobalInfo.SrContextV3.rtrId.Addr.Ip6Addr,
                SR_IPV6_ADDR_LENGTH);

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

PUBLIC INT4
SrConstructV3ExtPrefixTlv (UINT1 **pTempCurrent, UINT2 u2lsaType,
                           UINT1 u1LsaStatus)
{
    UINT1              *pCurrent = NULL;
    UINT4               nodeLabel = SR_ZERO;
    pCurrent = *pTempCurrent;

    /* Filling Type value */
    if (u2lsaType == OSPFV3_EXT_INTRA_AREA_PREFIX_LSA)
    {
        SR_LADD2BYTE (pCurrent, (UINT2) OSPFV3_INTRA_AREA_PRFIX_TLV_TYPE);
    }
    else if (u2lsaType == OSPFV3_EXT_INTER_AREA_PREFIX_LSA)
    {
        SR_LADD2BYTE (pCurrent, (UINT2) OSPFV3_INTER_AREA_PRFIX_TLV_TYPE);
    }

    /* Filling Length Value */
    SR_LADD2BYTE (pCurrent, (UINT2) OSPFV3_EXTENDED_PREFIX_TLV_LEN);

    /* Filling 0 & Metric */
    SR_LADD4BYTE (pCurrent, (SR_ZERO));

    /* Filling prefix length */
    SR_LADD1BYTE (pCurrent, 128);

    SR_LADD3BYTE (pCurrent, (SR_ZERO));

    /* Filling Ipv6 Addr */
    SR_LADDSTR (pCurrent, &gSrGlobalInfo.SrContextV3.rtrId.Addr.Ip6Addr,
                SR_IPV6_ADDR_LENGTH);

    SR_GET_V6_SELF_SID_INDEX_VAL (&nodeLabel);

    if (u1LsaStatus == FLUSHED_LSA)
    {
        /* If the Lsa Status is set to Flushed LSA,
         * then SID value should be filled with zero (invalid value) */
        nodeLabel = SR_ZERO;
    }

    if (SrConstructPrefixSidSubTlvV3 (&pCurrent, SR_SID_TYPE_INDEX, nodeLabel)
        != SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : SrConstructPrefixSidSubTlvV3 Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    *pTempCurrent = pCurrent;
    return SR_SUCCESS;
}

PUBLIC INT4
OspfV3SrEnqueueMsgFromOspf (tV3OsToOpqApp * pOspfV3Msg)
{
    tSrV6OspfNbrInfo   *pSrV6OspfNbrInfo = NULL;
    tSrV6OspfNbrInfo   *pNewSrV6OspfNbrInfo = NULL;
    tTMO_SLL_NODE      *pOspfV6Nbr = NULL;
    tRouterId           rtrId;
    tIPADDR             AdvrtrId;
    tIPADDR             TmpAdvrtrId;
    UINT1               isNbrExist = SR_FALSE;
    INT4                i4RetStatus = SR_SUCCESS;

    MEMSET (rtrId, SR_ZERO, sizeof (tRouterId));
    MEMSET (AdvrtrId, SR_ZERO, sizeof (tIPADDR));
    MEMSET (TmpAdvrtrId, SR_ZERO, sizeof (tIPADDR));

    SR_LOCK ();

    if (pOspfV3Msg->u4MsgSubType == OSPFV3_TE_LSA_INFO)
    {
        if (pOspfV3Msg->o3LsaInfo.u2LsaLen > MAX_SR_LSAS)
        {
            SR_TRC3 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : OspfV3 Lsa Length should not greater than %d \n",
                    __func__, __LINE__, MAX_SR_LSAS);
            SR_UNLOCK ();
            return SR_FAILURE;
        }
        if (SrProcessOspfV3Msg (pOspfV3Msg) != SR_SUCCESS)
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : SrProcessOspfV3Msg failed \n",
                    __func__, __LINE__);
            SR_UNLOCK ();
            return SR_FAILURE;
        }
    }
    else if (pOspfV3Msg->u4MsgSubType == OSPFV3_REGISTER_WITH_OPQ_APP)
    {
        if (pOspfV3Msg->u4ContextId == OSPFV3_DEFAULT_CXT_ID)
        {
            gSrGlobalInfo.u1SrOspfV3Disable = SR_TRUE;
            if (SrSndMsgToOspfV3 (SR_OSPF_DEREG,
                                  pOspfV3Msg->u4ContextId, NULL) != SR_SUCCESS)
            {
                SR_TRC (SR_CTRL_TRC | SR_FAIL_TRC, "De-Registration failed\n");
                SR_UNLOCK ();
                return CLI_FAILURE;
            }
            SR_TRC2 (SR_CTRL_TRC, "%s:%d De-reg successfully \n", __func__,
                     __LINE__);
        }
    }
    else if (pOspfV3Msg->u4MsgSubType == OSPFV3_NBR_DOWN_IND_TO_OPQ_APP)
    {
        TMO_SLL_Scan (&(gSrV6OspfNbrList), pOspfV6Nbr, tTMO_SLL_NODE *)
        {
            pSrV6OspfNbrInfo = (tSrV6OspfNbrInfo *) pOspfV6Nbr;
            if (MEMCMP
                (pSrV6OspfNbrInfo->ospfV6NbrInfo.nbrIpv6Addr.u1_addr,
                 pOspfV3Msg->o3NbrInfo.nbrIpv6Addr.u1_addr,
                 SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                isNbrExist = SR_TRUE;
                break;
            }

        }
        if (isNbrExist == SR_TRUE)
        {
            TMO_SLL_Delete (&(gSrV6OspfNbrList),
                            (tTMO_SLL_NODE *) pSrV6OspfNbrInfo);
            SR_OSPFV6_NBR_INFO_MEM_FREE (pSrV6OspfNbrInfo);
        }

    }

    else if (pOspfV3Msg->u4MsgSubType == OSPFV3_NBR_UP_IND_TO_OPQ_APP)
    {
        TMO_SLL_Scan (&(gSrV6OspfNbrList), pOspfV6Nbr, tTMO_SLL_NODE *)
        {
            pSrV6OspfNbrInfo = (tSrV6OspfNbrInfo *) pOspfV6Nbr;
            if (MEMCMP
                (pSrV6OspfNbrInfo->ospfV6NbrInfo.nbrIpv6Addr.u1_addr,
                 pOspfV3Msg->o3NbrInfo.nbrIpv6Addr.u1_addr,
                 SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                isNbrExist = SR_TRUE;
                break;
            }
        }
        if (isNbrExist == SR_FALSE)
        {
            if (SR_OSPFV6_NBR_INFO_MEM_ALLOC (pNewSrV6OspfNbrInfo) == NULL)
            {
                SR_TRC (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                        "Memory Allocation Failure for Neighbor Info \n");
                SR_UNLOCK ();
                return SR_FAILURE;
            }

            MEMCPY (&pNewSrV6OspfNbrInfo->ospfV6NbrInfo, &pOspfV3Msg->o3NbrInfo,
                    sizeof (tOs3NbrInfo));
            TMO_SLL_Insert (&(gSrV6OspfNbrList), NULL,
                            &(pNewSrV6OspfNbrInfo->nextNbrNode));
        }

        if (SR_FAILURE == SrV3UtilSendLSA (NEW_LSA, SR_ZERO))
        {
            SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC,
                     "%s:%d Unable to Send TYPE10_OPQ_LSA Self LSA \n", __func__,
                     __LINE__);
        }
    }
    else if (pOspfV3Msg->u4MsgSubType == OSPFV3_TE_NBR_IP_ADDR_INFO)
    {
        /* When SR is enabled later after OSPF settlement, then During Opqaue
           registration, NBR info will be sent with this message type */

        TMO_SLL_Scan (&(gSrV6OspfNbrList), pOspfV6Nbr, tTMO_SLL_NODE *)
        {
            pSrV6OspfNbrInfo = (tSrV6OspfNbrInfo *) pOspfV6Nbr;
            if (MEMCMP
                (pSrV6OspfNbrInfo->ospfV6NbrInfo.nbrIpv6Addr.u1_addr,
                 pOspfV3Msg->o3NbrInfo.nbrIpv6Addr.u1_addr,
                 SR_IPV6_ADDR_LENGTH) == SR_ZERO)
            {
                isNbrExist = SR_TRUE;
                break;
            }
        }
        if (isNbrExist == SR_FALSE)
        {
            if (SR_OSPFV6_NBR_INFO_MEM_ALLOC (pNewSrV6OspfNbrInfo) == NULL)
            {
                SR_TRC (SR_RESOURCE_TRC | SR_CRITICAL_TRC,
                        "Memory Allocation Failure for Neighbor Info \n");
                SR_UNLOCK ();
                return SR_FAILURE;
            }

            MEMCPY (&pNewSrV6OspfNbrInfo->ospfV6NbrInfo, &pOspfV3Msg->o3NbrInfo,
                    sizeof (tOs3NbrInfo));
            TMO_SLL_Insert (&(gSrV6OspfNbrList), NULL,
                            &(pNewSrV6OspfNbrInfo->nextNbrNode));
        }
    }

    SR_UNLOCK ();
    return i4RetStatus;
}

/*****************************************************************************/
/*                                                                           */
/* Function     : SrOspfV3ConstructExtendedLSA                               */
/*                                                                           */
/* Description  : This function construct External Prefix TLV and SUB-TLV    */
/*                                                                           */
/*                                                                           */
/* Input        : pOpqLSAInfo, u1LsaStatus, u2lsaType                        */
/* Output       : None                                                       */
/*                                                                           */
/* Returns      : SUCCESS/FAILURE                                            */
/*****************************************************************************/
PUBLIC INT4
SrOspfV3ConstructExtendedLSA (tV3OpqLSAInfo * pOpqLSAInfo,
                              UINT1 u1LsaStatus, UINT2 u2lsaType)
{
    UINT1              *pRtrAdrTlv = NULL;
    UINT1              *pCurrent = NULL;
    UINT4               u4AdvRtrID = SR_ZERO;
    UINT4               u4AreaId = SR_ZERO;
    UINT4               u4IpAddr = SR_ZERO;
    UINT4               u4IfIndex = SR_ZERO;
    UINT4               u4nodeLabel = SR_ZERO;

    MEMCPY (&u4IpAddr, &gSrGlobalInfo.SrContextV3.rtrId.Addr.u4Addr,
            sizeof (UINT4));

    u4AdvRtrID = gSrGlobalInfo.u4OspfRtrID;

#ifdef IP6_WANTED
    /* Fetching Area */
    IP6_TASK_LOCK ();
    NetIp6AddrTblGetIfIndexFromIp6Add (&u4IfIndex,
                                       &gSrGlobalInfo.SrContextV3.rtrId.
                                       Addr.Ip6Addr, 128);
    IP6_TASK_UNLOCK ();
#endif
    /* Fetching Area ID based on Interface Index */
    SrUtilFetchOspf3AreadID (u4IfIndex, &u4AreaId);

    pRtrAdrTlv = pOpqLSAInfo->pu1LSA;
    pCurrent = pRtrAdrTlv;

    SR_GET_V6_SELF_SID_INDEX_VAL (&u4nodeLabel);

    if (SrConstructV3ExtPrefixTlv (&pCurrent, u2lsaType, u1LsaStatus) !=
        SR_SUCCESS)
    {
        SR_TRC2 (SR_CTRL_TRC | SR_FAIL_TRC, "%s:%d : SrConstructV3ExtPrefixTlv Failed \n",
                __func__, __LINE__);
        return SR_FAILURE;
    }

    if (u2lsaType == OSPFV3_EXT_INTRA_AREA_PREFIX_LSA)
    {
        pOpqLSAInfo->LsId[SR_ZERO] = OSPFV3_EXT_INTRA_AREA_PREFIX_TLV;
    }
    else
    {
        pOpqLSAInfo->LsId[SR_ZERO] = OSPFV3_EXT_INTER_AREA_PREFIX_TLV;
    }
    pOpqLSAInfo->LsId[SR_TWO] = (UINT1) (u4nodeLabel & SR_MASK_TYPE1);
    pOpqLSAInfo->LsId[SR_THREE] = (UINT1) (u4nodeLabel & SR_MASK_TYPE2);

    MEMCPY (pOpqLSAInfo->AdvRtrID, &u4AdvRtrID, sizeof (UINT4));

    pOpqLSAInfo->u2SubType = OSPFV3_ROUTER_LSA;
    pOpqLSAInfo->u4AreaID = u4AreaId;
    pOpqLSAInfo->u2LsaLen = (UINT2) (pCurrent - pRtrAdrTlv);
    pOpqLSAInfo->u2OpqLSAType = u2lsaType;
    SR_UPDT_LSA_FLAG (u1LsaStatus, pOpqLSAInfo);
    return SR_SUCCESS;
}

/***************************************************************************/
/* Function    :  V3OpqAppRegisterInCxt                                    */
/* Input       :  u1AppOpqType,                                            */
/*                u1OpqLSATypesRegistered                                  */
/*                u4InfoFromOSPF                                           */
/*                OpqAppCallBackFn                                         */
/* Output      :  pu4OpqAppID                                              */
/* Description :  Application is registering with the Opaque Module        */
/* Returns     :  OSPFV3_SUCCESS or OSPFV3_FAILURE                         */
/***************************************************************************/
INT4
V3OpqAppRegisterInCxt (UINT4 u4ContextId, UINT1 u1AppOpqType,
                       UINT1 u1OpqLSATypesRegistered,
                       UINT4 u4InfoFromOSPF, VOID *OpqAppCallBackFn)
{
    UNUSED_PARAM (u4ContextId);
    UNUSED_PARAM (u1AppOpqType);
    UNUSED_PARAM (u1OpqLSATypesRegistered);
    UNUSED_PARAM (u4InfoFromOSPF);
    UNUSED_PARAM (OpqAppCallBackFn);
    return SR_SUCCESS;
}

/****************************************************************************/
/*  Function    :  V3OpqAppDeRegister                                       */
/*  Input       :  u1AppOpqType                                             */
/*  Output      :  None                                                     */
/*  Description :  Application Deregisters by posting a message to the APPQ */
/*  Returns     :  OSPFV3_SUCCESS or OSPFV3_FAILURE                             */
/****************************************************************************/
INT4
V3OpqAppDeRegisterInCxt (UINT4 u4ContextId, UINT1 u1AppOpqType)
{
    UNUSED_PARAM (u4ContextId);
    UNUSED_PARAM (u1AppOpqType);
    return SR_SUCCESS;
}

/***************************************************************************/
/*  Function    :  V3OpqAppToOpqModuleSendInCxt                            */
/*  Input       :  tV3OpqLSAInfo,                                          */
/*  Output      :  None                                                    */
/*  Description :  Application generates an Opaque LSA which is passed to  */
/*                 the Opaque Module.The Send Message is posted to the APPQ*/
/*  Returns     :  OSPFV3_SUCCESS or OSPFV3_FAILURE                        */
/****************************************************************************/
INT4
V3OpqAppToOpqModuleSendInCxt (UINT4 u4ContextId,
                              tV3OpqLSAInfo * pInputOpqLSAInfo)
{
    UNUSED_PARAM (u4ContextId);
    UNUSED_PARAM (pInputOpqLSAInfo);
    return SR_SUCCESS;
}

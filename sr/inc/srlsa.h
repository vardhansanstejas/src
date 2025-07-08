/* Copyright (C) 2017 Aricent Inc . All Rights Reserved
*******************************************************************************
**    $Id$
**    FILE  NAME             : srlsa.h
**    PRINCIPAL AUTHOR       : Aricent Inc.
**    SUBSYSTEM NAME         : SR
**    MODULE NAME            : SR
**    LANGUAGE               : ANSI-C
**    TARGET ENVIRONMENT     : Linux  (Portable)
**    DATE OF FIRST RELEASE  :
**    DESCRIPTION            : Contains macros and data structure definitions
							   for SR Opaque LSA handling.
**---------------------------------------------------------------------------*/
#ifndef _SR_LSA_H_
#define _SR_LSA_H_


PUBLIC INT4
SrSndMsgToOspf (UINT1 u1MsgType, UINT4 u4SrCxtId, tOpqLSAInfo * pOpqLSAInfo);

PUBLIC INT4
SrConstructRtrInfoSrAlgoTlv(UINT1  **pTempCurrent);

PUBLIC INT4
SrConstructSidLabelSubTlv(UINT1  **pTempCurrent,UINT4 SidLabelType,UINT4 u4Val);

PUBLIC INT4
SrConstructRtrInfoSidLabelRangeTlv(UINT1  **pTempCurrent);

PUBLIC INT4
OspfSrEnqueueMsgFromOspf (tOsToOpqApp * pOspfMsg);

VOID
SrProcessRecvAdjSidInfo(tSrRtrInfo *pRtrInfo, tAdjSidNode *pRecvAdjSid, UINT1 u1LsaStatus);

VOID
SrProcessOpqExtLinkAdjSidSubTlv(UINT1   **ptlvPtr, tAdjSidNode *pAdjSidInfo,
                                UINT2 u2SubTlvType);

PUBLIC UINT4
SrProcessOpqExtLinkLsa (tOsLsaInfo *pOsLsaInfo, tSrRtrInfo *pRtrInfo);

PUBLIC UINT4
SrProcessOpqType10Lsa(tOsToOpqApp * pOspfMsg,tOsSrArea *pOsSrArea);

PUBLIC UINT4
SrProcessFlushExtPrefixOpqLsa (tSrRtrInfo *pRtrInfo);

PUBLIC VOID
SrProcessFlushRtrInfoLsa (UINT4 u4AdvRtrId, UINT4 u4OpqAreaId);

PUBLIC INT4
SrProcessOspfNbrADjSidILMUpdate(tSrV4OspfNbrInfo  *pSrOspfNbrInfo, UINT1  isILMDel);

PUBLIC INT4
SrProcessOspfMsg(tOsToOpqApp * pOspfMsg);

PUBLIC UINT4
SrProcessRtrLsa(tOsToOpqApp * pOspfMsg,tOsSrArea* pOsSrArea);

PUBLIC INT4
SrUtilExtLinkInfoRbTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2);

PUBLIC tareaLinkInfo  *
SrCreateNewAreaLinkInfoNode (VOID);

INT4
RbWalkFnForExtLinkStaleEntry (tRBElem * pRBElem, eRBVisit visit, UINT4 u4Level,
                          void *pArg, void *pOut);
INT4
RbWalkFnForExtLinkEntryProcess (tRBElem * pRBElem, eRBVisit visit, UINT4 u4Level,
                          void *pArg, void *pOut);

PUBLIC UINT4
SrProcessExtLinkInfoUpdate(tSrExtLinkLSAInfo  *pExtLinkInfo, UINT4 u4AreaId);

VOID
SrProcessDeleteStaleExtLinkInfo(tareaLinkInfo   *pSrAreaLinkInfo);

PUBLIC UINT4
SrProcessRtrLsaforUpdateExtLinkInfo (tOsLsaInfo  *pOsLsaInfo, UINT4   u4AreaId);

PUBLIC tAdjSidNode  *
SrCreateAdjSidInfoNode (VOID);

PUBLIC UINT1
SrUpdateExtRtrLinkAdjInfo(tSrExtLinkLSAInfo   *pExtLinkNode);

PUBLIC tSrExtLinkLSAInfo  *
SrCreateNewExtLinkInfoNode (VOID);

INT4
SrDeleteExtLinkInfoNode (tareaLinkInfo  *pSrAreaLinkInfo,
                         tSrExtLinkLSAInfo   *pExtLinkNode);

PUBLIC tSrExtLinkLSAInfo *
SrAddExtRtrLinkInfo (tareaLinkInfo  *pSrAreaLinkInfo, tSrLinkInfoKey *pLinkNode);

PUBLIC UINT4
SrParseRtrLsaAddRtrLinks (tOsLsaInfo  *pOsLsaInfo, tareaLinkInfo *pSrAreaLinkInfo);

PUBLIC UINT1*
OspfSrUtilGetLinkFromLsa (UINT1 *pLsa, UINT2 u2LsaLen,
                           UINT1 *pCurrPtr, tSrLinkInfoKey * pLinkNode);

PUBLIC UINT1
SrProcessAddAdjTlvtoLinkInfo(tSrExtLinkLSAInfo   *pExtLinkNode,
                            tSrSidInterfaceInfo *pSrSidEntry);
PUBLIC UINT1
SrProcessDelAdjTlvtoLinkInfo(tSrExtLinkLSAInfo   *pExtLinkNode,
                            tSrSidInterfaceInfo  *pSrSidEntry);
VOID
SrProcessFlushLsaExtLinkInfo(tSrExtLinkLSAInfo  *pExtLinkNode);
PUBLIC UINT1
SrProcessAdjSidChange(tSrSidInterfaceInfo * pSrSidInfo,
                      tSrV4OspfNbrInfo    *pOspfNbr);
PUBLIC UINT4
SrProcessNetworkLsa(tOsToOpqApp * pOspfMsg,tOsSrArea *pOsSrArea);

PUBLIC INT4
SrConstructExtPrefixTlv (UINT1 **pTempCurrent, UINT1 u1LsaStatus,
                         tSrSidInterfaceInfo *pSrSidEntry);

PUBLIC INT4
SrConstructExtPrefixOpqLsa (tOpqLSAInfo * pOpqLSAInfo,
                            UINT1 u1LsaStatus, UINT1 u1LsaType,
                            tSrSidInterfaceInfo *pSrSidEntry);

PUBLIC INT4
SrConstructRtrInfoLsa (tOpqLSAInfo * pOpqLSAInfo, UINT1 u1LsaStatus,UINT1 u1LsaType, UINT4 AreaId);

#if 1
PUBLIC INT4
SrConstructAdjSidSubTlv (UINT1 **pTempCurrent, tAdjSidNode *pAdjSidNode);
#else
PUBLIC INT4
SrConstructAdjSidSubTlv(UINT1 **pTempCurrent, UINT4 SidLabelType, UINT4 u4Val);
#endif

#if 1
PUBLIC INT4
SrConstructExtLinkTlv (tSrExtLinkLSAInfo  *pExtLinkInfo, UINT1 **pTempCurrent);
#else
PUBLIC INT4
SrConstructExtLinkTlv(tSrSidInterfaceInfo *pSrSidEntry, UINT1 **pTempCurrent);
#endif

#if 1
PUBLIC INT4
SrConstructExtLinkOpqLsa (tOpqLSAInfo * pOpqLSAInfo,
                          tSrExtLinkLSAInfo  *pExtLinkInfo,
                          UINT4               u4AreaId);
#else
PUBLIC INT4
SrConstructExtLinkOpqLsa (tOpqLSAInfo * pOpqLSAInfo,
						 UINT1 u1LsaStatus,UINT1 u1LsaType);
#endif

PUBLIC INT4
SrConstructPrefixSidSubTlv(UINT1  **pTempCurrent, UINT4 SidLabelType,UINT4 u4Val);

PUBLIC UINT4
SrGetAddrPrefix(tOsLsaInfo *pOsLsaInfo);

PUBLIC VOID
SrConstructSendExtPrefixOpqLsa(UINT1 u1LsaStatus, UINT4 nodeLabel, UINT4 advRtrID,UINT4 u4AreaId, UINT1 u1SrSid);

PUBLIC VOID
SrV3ConstructSendExtLsa (UINT1 u1LsaStatus, UINT4 nodeLabel,tIp6Addr Ipv6Address,
                        UINT4 u4AreaId, UINT2 u2lsaType);

PUBLIC INT4
SrConstructPrefixTlv (UINT1 **pTempCurrent, tSrSidInterfaceInfo *pSrSidIntf);

PUBLIC INT4
SrConstructSidLabelSubTlvV3(UINT1  **pTempCurrent,UINT4 SidLabelType,UINT4 u4Val);

PUBLIC INT4
SrConstructRtrInfoSidLabelRangeTlvV3(UINT1  **pTempCurrent);

PUBLIC INT4
SrConstructPrefixSidSubTlvV3(UINT1  **pTempCurrent, UINT4 SidLabelType,UINT4 u4Val);

#endif

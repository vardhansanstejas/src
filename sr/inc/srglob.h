/*****************************************************************************/
/* Copyright (C) 2007 Altran Inc . All Rights Reserved
 *      $Id$
 * 	
 * 	DESCRIPTION            : This file contains prototypes of routines for 
 *    						   Segment Routing.
 *---------------------------------------------------------------------------*/
#ifndef _SR_GLOB_H_
#define _SR_GLOB_H_
#include "../../mpls/mplsdb/mplslsr.h"
#include "srdb.h"
extern tSrGlobalInfo gSrGlobalInfo;
extern tTMO_SLL gSrV6OspfNbrList;
extern tTMO_SLL gSrV4OspfNbrList;

/*srmain.c*/
PUBLIC INT4 SrMainTaskLock (VOID);
PUBLIC INT4 SrMainTaskUnLock (VOID);

/*srutil.c*/
PUBLIC VOID  SrUtilGetLblId(UINT2 *pu2SrLblId);
PUBLIC INT4 SrUtilSidRbTreeCmpFunc(tRBElem * pRBElem1, tRBElem * pRBElem2);
PUBLIC INT4 SrUtilRlfaPathRbTreeCmpFunc(tRBElem * pRBElem1, tRBElem * pRBElem2);
PUBLIC INT4 SrUtilTeRouteRbTreeCmpFunc(tRBElem * pRBElem1, tRBElem * pRBElem2);
PUBLIC INT4 SrUtilLfaTmrRbTreeCmpFunc(tRBElem * pRBElem1, tRBElem * pRBElem2);
PUBLIC INT4 SrStaticRbTreeCmpFunc(tRBElem * pRBElem1, tRBElem * pRBElem2);
PUBLIC INT4 SrUtilOsRiLsCmpFunc(tRBElem * pRBElem1, tRBElem * pRBElem2);
PUBLIC UINT4 SrSidEntryAddToRBtree(tSrSidInterfaceInfo *pSrSidInterface);
PUBLIC INT4 SrFTNRbTreeCmpFunc(tRBElem * pRBElem1, tRBElem * pRBElem2);
PUBLIC INT4 SrILMRbTreeCmpFunc(tRBElem * pRBElem1, tRBElem * pRBElem2);
INT4 SrUtilHandleCfaEvent(UINT4 u4IfIndex,UINT1 u1OperStatus,UINT1 u1IfType);
INT4 SrUtilHandleCfaIfStatusChange(INT4 i4IfIndex);
PUBLIC INT4
SrConstructRtrInfoLsaV3(tV3OpqLSAInfo * pOpqLSAInfo, UINT1 u1LsaStatus,UINT2 u2LsaType);

PUBLIC INT4
SrOspfV3ConstructExtendedLSA (tV3OpqLSAInfo* pOpqLSAInfo,
                        UINT1 u1LsaStatus, UINT2 u2lsaType);
PUBLIC INT4
SrSndMsgToOspfV3 (UINT1 u1MsgType, UINT4 u4SrCxtId, tV3OpqLSAInfo *pOpqLSAInfo);
PUBLIC INT4
SrConstructV3ExtPrefixTlv(UINT1  **pTempCurrent ,UINT2 u2lsaType, UINT1 u1LsaStatus);
PUBLIC INT4
SrConstructV3ExtendedPrefixTlv(UINT1  **pTempCurrent, UINT2 u2lsaType);


PUBLIC UINT4
SrInitSidInterface(tSrSidInterfaceInfo *pSrSidInterface, tGenU4Addr *pIfIpAddr);

PUBLIC INT4
SrProcessOspfV3Msg (tV3OsToOpqApp * pOspfMsg);

PUBLIC UINT4
SrProcessIp6Events (tSrIpEvtInfo *pSrIpEvtInfo);
VOID SrOspfv3UpEventHandler (VOID);

PUBLIC UINT4
SrV3ProcessOpqType10Lsa(tV3OsToOpqApp * pOspfMsg);
PUBLIC tSrSidInterfaceInfo* SrCreateSidInterface (tGenU4Addr *pIfIpAddr);
PUBLIC UINT4 SrResvLabelSpace (VOID);
PUBLIC UINT4 SrPeerInfoCleanUp (UINT2 u2AddrType);

PUBLIC UINT4
SrStaticCreateILM ( tSrSidInterfaceInfo* ,UINT4 u4SidValue,tGenU4Addr destPrefix, 
					tGenU4Addr nextHop, UINT2 u2MlibOperation, UINT4 u4OutIfIndex);

PUBLIC UINT4
SrUtilCreateorDeleteILM( tSrSidInterfaceInfo *pSrSidInfo, UINT4 u4SidValue,tGenU4Addr *pDestPrefix,
									tGenU4Addr *pNextHop, UINT2 u2MlibOperation, UINT4 u4OutIfIndex);
UINT4 LfaCreateAndDeleteLSP (UINT2 u2MlibOperation, tSrRtrInfo * pSrRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo);
PUBLIC UINT4 SrLfaLSPHandle(VOID);
UINT4 SrV3LfaFetchAndCreateLSP (UINT2 u2MlibOperation, tSrRtrInfo *pSrRtrInfo, tSrRtrNextHopInfo *pSrRtrNextHopInfo);
UINT4 SrV3LfaRtAddOrDeleteEventHandler(tNetIpv6LfaRtInfo *pIpv6LfaRtInfo, UINT2 u2MlibOperation);

VOID LfaIpv6RtChangeEventHandler (tNetIpv6LfaRtInfo *pLfaRtInfo);

VOID SrProcessLfaIpv6RtChangeEvent (tNetIpv6LfaRtInfo *pIpv6LfaRtInfo);
VOID
SrProcessNetIp6LfaEvents (tSrNetIpv6LfaRtInfo *pSrNetIp6LfaRtInfo);

VOID
SrProcessNetIpLfaEvents (tSrNetIpLfaRtInfo *pSrNetIpLfaRtInfo);
VOID SrProcessLfaIpRtChangeEvent (tNetIpv4LfaRtInfo *pLfaRtInfo);
VOID LfaIpRtChangeEventHandler (tNetIpv4LfaRtInfo *pNetIpLfaRtInfo);

UINT4 LfaRtAddOrDeleteEventHandler(tNetIpv4LfaRtInfo *pLfaRtInfo, UINT2 u2MlibOperation);
UINT4 SrIpv4RlfaFetchAndCreateLSP (tNetIpv4LfaRtInfo *pNetIpLfaRtInfo);
UINT4 SrIpv4RlfaILMCreation (tNetIpv4LfaRtInfo *pNetIpLfaRtInfo, tSrRtrInfo * pSrRtrInfo);

VOID SrRLfaRtChangeEventHandler (tNetIpv4LfaRtInfo *pLfaRtInfo);
UINT4 SrV3RLfaRtAddOrDeleteEventHandler (tNetIpv6LfaRtInfo *pNetIpv6LfaRtInfo, UINT2 u2MlibOperation);
UINT4 RLfaRtAddOrDeleteEventHandler(tNetIpv4LfaRtInfo *pLfaRtInfo, UINT2 u2MlibOperation);
/* TI-LFA */
PUBLIC INT4 SrTiLfaPathRbTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2);
PUBLIC INT4 SrLfaNodeTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2);
PUBLIC INT4 SrLfaAdjTreeCmpFunc (tRBElem * pRBElem1, tRBElem * pRBElem2);
VOID SrTiLfaFreeAdjSidList (tTMO_SLL_NODE * pNode);
UINT4 SrUpdateAdjLabelforPandQ(tSrTilfaPathInfo    *pTiLfaPathInfo,
                        tSrRtrInfo         *pPSrRtrInfo);
UINT4
SrDeleteAdjLabelforPandQ(tSrTilfaPathInfo    *pTiLfaPathInfo,
                        tAdjSidNode   *pAdjSidNode);

VOID SrTiLfaPathDelete(tSrTilfaPathInfo    *pTiLfaPathInfo);

VOID SrTiLfaProcessSrRouterAsLfaNextHop(tSrRtrInfo    *pSrNexthopNode,
                                tSrLfaNodeInfo   *pLfaNode);
VOID SrTiLfaProcessSrRouterAsLfaPNode(tSrRtrInfo    *pSrPNode,
                                tSrLfaNodeInfo *pLfaNode);
VOID SrTiLfaProcessSrRouterAsLfaQNode(tSrRtrInfo    *pSrQNode,
                                tSrLfaNodeInfo *pLfaNode);
VOID SrTiLfaDeleteSrRouterAsLfaNextHop(tSrRtrInfo    *pSrNexthopNode,
                                tSrLfaNodeInfo   *pLfaNode);
VOID SrTiLfaDeleteSrRouterAsLfaPNode(tSrRtrInfo    *pSrPNode,
                                tSrLfaNodeInfo *pLfaNode);
VOID SrTiLfaDeleteSrRouterAsLfaQNode(tSrRtrInfo    *pSrQNode,
                                tSrLfaNodeInfo *pLfaNode);
VOID SrTiLfaProcessAdjInfoUpdate(tSrRtrInfo    *pSrRouterNode,
                            tSrLfaAdjInfo *pLfaAdj);
VOID SrTiLfaProcessAdjInfoDelete(tAdjSidNode   *pAdjSidNode,
                            tSrLfaAdjInfo *pLfaAdj);
VOID SrTiLfaProcessForSrRouterNodeDelete(tSrRtrInfo    *pSrRtrNode);
VOID SrTiLfaProcessForSrRouterNodeChange(tSrRtrInfo    *pSrRtrNode);
VOID SrTiLfaProcessForSrRouterAdjDel(tAdjSidNode   *pAdjSidNode, UINT4 u4AreaId);
VOID SrTiLfaProcessForSrRouterAdjChange(tSrRtrInfo    *pSrRtrNode,
                                tAdjSidNode        *pAdjSidNode);

VOID SrTiLfaRtChangeEventHandler (tNetIpv4LfaRtInfo *pLfaRtInfo);
UINT4 SrIpv4TiLfaCreateLSP (tSrTilfaPathInfo    *pTiLfaPathInfo);
UINT4 SrIpv4TiLfaCreateILM (tSrTilfaPathInfo    *pTiLfaPathInfo);
UINT4 SrIpv4TiLfaDeleteLSP (tSrTilfaPathInfo    *pTiLfaPathInfo);
UINT4 SrIpv4TiLfaFrrModifyDeleteILM (tSrTilfaPathInfo    *pTiLfaPathInfo);
UINT4 SrIpv4TiLfaDeleteILM (tSrTilfaPathInfo    *pTiLfaPathInfo);
PUBLIC UINT4 SrMplsModifyTilfaLSP (tSrIpEvtInfo * pSrIpEvtInfo, tSrRtrInfo *pSrRtrInfo,
                    tSrTilfaPathInfo * pSrTiLfaPathInfo, UINT1 u1IsNewNHeqFRRNH);
UINT4 SrIpv4TiLfaUpdateLabelStackInfo (tSrTilfaPathInfo    *pTiLfaPathInfo);
tSrLfaAdjInfo *SrLfaAdjCreate(tSrTilfaAdjInfo    *pTiLfaAdjInfo);
tSrLfaNodeInfo *SrLfaNodeCreate(UINT4 u4RtrId);
tSrTilfaAdjInfo *SrCreateTilfaAdjEntry (tNetIpv4LfaRtInfo * pNetIpLfaRtInfo);
tSrTilfaPathInfo *SrCreateTiLfaPathEntry (tNetIpv4LfaRtInfo * pNetIpLfaRtInfo);
UINT4 SrAddTiLfaPathEntryToLfaNode(tSrTilfaPathInfo   *pTiLfaPathInfo);
VOID SrProcessDelSllNodeFromLfaNode(tSrLfaNodeInfo      *pLfaNode,
                                    tSrTilfaPathInfo   *pTiLfaPathInfo,
                                    UINT1            u1NodeType);
VOID SrDelTiLfaPathEntryFromLfaNode(tSrTilfaPathInfo   *pTiLfaPathInfo);
VOID SrDelTiLfaPathEntryFromLfaAdj(tSrTilfaPathInfo   *pTiLfaPathInfo);
tSrAdjLfaListNode *SrLfaCreateAddAdjListNode(tSrLfaAdjInfo  *pLfaAdj,
                                             tSrTilfaPathInfo   *pTiLfaPathInfo);
UINT4 SrAddTiLfaPathEntryToLfaAdj(tSrTilfaPathInfo   *pTiLfaPathInfo);
UINT4 TiLfaHandleIpv4RouteAdd(tNetIpv4LfaRtInfo * pNetIpLfaRtInfo);
UINT4 TiLfaHandleIpv4RouteDel(tSrTilfaPathInfo   *pSrTilfaPathInfo);
UINT4 TiLfaRtAddOrDeleteEventHandler(tNetIpv4LfaRtInfo *pLfaRtInfo, UINT2 u2MlibOperation);
VOID SrV3TiLfaRtChangeEventHandler (tNetIpv6LfaRtInfo *pIpv6LfaRtInfo);
UINT4 SrV3TiLfaRtAddOrDeleteEventHandler (tNetIpv6LfaRtInfo *pNetIpv6LfaRtInfo, UINT2 u2MlibOperation);

INT4
SrRlfaSetTeTunnel (tGenU4Addr *pDestAddr, UINT4 u4DestMask, tGenU4Addr *pMandatoryRtrId, tGenU4Addr *prlfaNextHop, UINT4 u4rlfaOutIfIndex);
UINT4 SrIpv6RlfaFetchAndCreateLSP (tNetIpv6LfaRtInfo *pNetIpv6LfaRtInfo);
UINT4 SrEnableLFA(VOID);
INT4 SrV3EnableLFA(VOID);
VOID SrV3RLfaRtChangeEventHandler (tNetIpv6LfaRtInfo *pIpv6LfaRtInfo);
INT4 SRLFACreateILM(tInSegment *pInSegment);
INT4
SrRlfaCreateFtnEntry (tGenU4Addr *pDestAddr);
INT4
SrRlfaDeleteFtnEntry (tGenU4Addr *pDestAddr,tGenU4Addr *pNextHop);
INT4
SrRlfaModifyFtnEntry (tGenU4Addr *pDestAddr);

UINT4
SrUtilCreateMplsTunnelInterface (UINT4 u4OutIfIndex, UINT2 u2AddrType,
         tGenU4Addr *pNextHop, tSrRtrNextHopInfo *pNewSrRtrNextHopInfo);
UINT4 SrIpv6RlfaILMCreation (tNetIpv6LfaRtInfo *pNetIpv6LfaRtInfo, tSrRtrInfo  *pSrRtrInfo);
VOID SrGetLabelFromSIDForRemoteNode(tSrRtrInfo         *pNexthopSrRtrInfo,
                                UINT4               u4SidValue,
                                UINT4              *pLabel);
INT4 SrRlfaCreateILM (UINT4 *pu4LfaInLabel);
INT4 SrRlfaDeleteILM (tGenU4Addr *pDestAddr, tGenU4Addr *pRemoteAddr, tGenU4Addr *pRlfaNextHop,
                             tSrInSegInfo* pSrInSegInfo);
INT4 SrRlfaDeleteFTNAndILM ( tGenU4Addr *pDestAddr, tGenU4Addr *pRemoteAddr,
                       tGenU4Addr *pRlfaNextHop,tSrInSegInfo* pSrInSegInfo, UINT4 u4SrGbMinIndex);
VOID
SrGetLabelForRLFARemoteNode (tGenU4Addr *pRemoteNode, UINT4 *pSrGbMinIndex, UINT4 *pOUTLabel);
/* SR-MPLS APIs - start*/
PUBLIC UINT4 SrReserveMplsInLabel (UINT2 u2AddrType, UINT1, UINT4);
PUBLIC UINT4 SrReleaseMplsInLabel (UINT2 u2AddrType, UINT4 u4SidValue);
PUBLIC UINT4 SrMplsCreateFTN (tSrRtrInfo * pSrRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo);
PUBLIC UINT4 SrMplsCreateILM (tSrRtrInfo * pSrRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo);
PUBLIC UINT4 SrMplsDeleteFTN (tSrRtrInfo * pSrRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo);
PUBLIC UINT4 SrMplsDeleteILM (tSrInSegInfo * pSrInSegInfo, tSrRtrInfo * pSrRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo);
PUBLIC UINT4 SrMplsCreateOrDeleteLsp (UINT2 u2MlibOperation, tSrRtrInfo * pSrRtrInfo,
                                        tSrInSegInfo * pSrInSegInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo);
PUBLIC UINT4 SrMplsDeleteAllILM (UINT2 u2AddrType);
PUBLIC UINT4 SrMplsDeleteAllFTN (UINT2 u2AddrType);
PUBLIC UINT4 SrMplsCleanUp (UINT2 u2AddrType);

PUBLIC tSrRtrInfo *
SrGetRtrInfoFromNextHop(tGenU4Addr *pNextHop);

PUBLIC VOID
SrGetNextHopFromNbrRtrId(tGenU4Addr *pNbrRtrId, tGenU4Addr *pNbrLinkIpAddr);

PUBLIC VOID
SrGetNbrRtrIdFromNbrAddr(tGenU4Addr * pNbrLinkIpAddr, UINT4 * pNbrRtrId);

PUBLIC VOID
SrIPv4UpdateMplsBindingOnNbrSrUp(tSrRtrInfo         *pRtrInfo,
                                tSrRtrNextHopInfo  *pSrRtrNextHopInfo);
#ifdef IP6_WANTED
PUBLIC VOID
SrIPv6UpdateMplsBindingOnNbrSrUp(tSrRtrInfo         *pRtrInfo,
                                tSrRtrNextHopInfo  *pSrRtrNextHopInfo);
#endif


PUBLIC UINT4
SrCreateMplsBindingOnNbrSrUp(tGenU4Addr *nbrRtrId, UINT4 u4AdvRtrId);

PUBLIC UINT4
SrDeleteMplsBindingOnNbrSrDown(tGenU4Addr *nbrRtrId, UINT4 u4AdvRtrId);

PUBLIC UINT4
SrCreateMplsEntry (tSrRtrInfo *pRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo);
PUBLIC UINT4
SrDeleteMplsEntry (tSrRtrInfo *pRtrInfo);

PUBLIC UINT4
SrMplsCreateILMPopAndFwd (tSrRtrInfo *pRtrInfo, tSrRtrNextHopInfo *pSrRtrNextHopInfo);
PUBLIC UINT4
SrMplsDeleteILMPopAndFwd (tSrRtrInfo *pRtrInfo, tSrRtrNextHopInfo *pSrRtrNextHopInfo);

/* SR-MPLS APIs - end*/

/* SR-IP APIs - start*/
VOID SrIpRtChgEventHandler (tNetIpv4RtInfo * pNetIpv4RtInfo, tNetIpv4RtInfo * pNetIpv4OldRtInfo, UINT1 u1CmdType);
VOID LfaIpRtChgEventHandler (tNetIpv4RtInfo * pNetIpv4RtInfo, UINT1 u1CmdType);
UINT4 LfaProcessRouteChangeEvent (tSrIpEvtInfo *pSrIpEvtInfo);
VOID LfaRtChangeEventHandler (tNetIpv4LfaRtInfo *pLfaRtInfo);
PUBLIC UINT4 SrEnqueueMsgToSrQ (UINT1 u1EvtType, tSrQMsg *pSrQMsg);
PUBLIC UINT4 SrProcessIpEvents (tSrIpEvtInfo *pSrIpEvtInfo);
UINT4 SrProcessRouteChangeEvent (tSrIpEvtInfo *pSrIpEvtInfo);
UINT4 SrProcessFRRModifyRoute(tSrIpEvtInfo       *pSrIpEvtInfo,
                        tSrRtrInfo         *pRtrInfo,
                        tSrRtrNextHopInfo  *pSrRtrNextHopInfo);
UINT4 SrStartRtDelayTimer ();
UINT4 SrProcessFRRModifyRouteEvent (tSrIpEvtInfo *pSrIpEvtInfo);
UINT4 SrProcessRouteNextHopChangeEvent (tSrIpEvtInfo * pSrIpEvtInfo);
/* SR-IP APIs - end*/
VOID SrOspfUpEventHandler(void);

PUBLIC tSrRtrInfo *SrCreateNewRouterNode(VOID);
INT4
SrCliConfigAddrFamily (UINT4 u4Afi, UINT4 u4Set);
tSrRtrEntry *
SrCreateOptRtrEntry(UINT4 u4RtrListIndex, UINT4 u4OptRtrIndex);
VOID SrDeleteRtrFromDelayList(tSrRtrInfo  *pRtrInfo);
/*srcli.c*/
PUBLIC UINT1 UtilIpAddrIndComp ( tIPADDR  ifIpAddr1,
                                        UINT4 u4AddrlessIf1,
                                        tIPADDR ifIpAddr2,
                                        UINT4 u4AddrlessIf2);
INT4 SrCliProcessiAdminUpCmd(VOID);
INT4  SrCliProcessAdminDownCmd(UINT4);

VOID
SrHandleEventforAllExtLinkLsa(UINT1 u1LsaStatus);
/*OSPF*/
PUBLIC UINT4 SrUtilSendLSA(UINT1 u1LsaStatus,UINT4 lsaTLVReceived,UINT1 u1lsaType);

PUBLIC UINT4 
SrV3UtilSendLSA(UINT1 u1LsaStatus,UINT4 lsaTLVReceived);

PUBLIC UINT4 SrGetNHForPrefix (tGenU4Addr *pDestAddr, tGenU4Addr *pNextHop, UINT4 *pu4OutIfIndex);
PUBLIC UINT4 SrMplsDynamicCleanUp(UINT2 u2AddrType);

INT4
SrCreateRlfaPathTableEntry(tSrRlfaPathInfo *pSrRlfaPathInfo);

/*Sr Static Api's - start  */

INT4
SrStaticProcessCreateOrDel(tGenU4Addr *pDestAddr, UINT4 u4StaticEnable, UINT4 u4LspId, UINT4 *pu4MplsTunIndex);

INT4
SrDeleteStaticSrTableEntry (tSrStaticEntryInfo *pSrStaticInfo);

tStaticSrLbl   *
SrCreateStaticSrLblEntry (UINT4 u4LabelStackIndex, UINT4 u4SrLabelIndex);

tSrStaticEntryInfo     *
SrGetStaticSrTableEntry (tSrStaticEntryInfo *pSrStaticInfo);

tSrStaticEntryInfo     *
SrGetNextStaticSrTableEntry (tSrStaticEntryInfo *pSrStaticInfo);

tSrStaticEntryInfo   *
SrGetStaticSrFromDestAddr (tGenU4Addr *pDestAddr, UINT4 u4LspId);

tSrStaticEntryInfo     *
SrCreateStaticSrTableEntry (tSrStaticEntryInfo *pSrStaticInfo);

UINT4
SrStaticGetDynamicNextHop (tGenU4Addr* pDestAddr, UINT4 u4LspId);

UINT4
SrStaticSetDynamicNextHop (tSrRtrInfo* pRtrInfo, UINT1 u1SidEvt);

UINT4
SrStaticSetRowStatus (tSrStaticEntryInfo* pStaticSrInfo, INT4 i4RowStatus);

UINT4
SrStaticGenerateAlarm (tSrStaticEntryInfo* pStaticSrInfo, UINT1 u1Type, UINT1 u1Raise);

UINT4
SrStaticClearAllAlarm (tSrStaticEntryInfo* pStaticSrInfo);

/*Sr Static Api's - ends  */


/* SR-TE APIs - start*/
tSrTeRtEntryInfo      *
SrCreateTeRouteTableEntry(tSrTeRtEntryInfo *pSrTePathInfo);

INT4 SrDeleteRlfaPathTableEntry(tSrRlfaPathInfo *pSrRlfaPathInfo);

tSrRlfaPathInfo   *
SrGetNextRlfaRtEntry (tSrRlfaPathInfo *pSrRlfaPathInfo);

INT4
SrDeleteILMFromSrId (tGenU4Addr *pDestAddr, tGenU4Addr *pNextHop);

UINT4
SrTeGetLabelForRtrId (tGenU4Addr *pRouterId, tGenU4Addr *pPrevRouterId, UINT4 *u4RouterLabel);

tSrRtrInfo *
SrGetSrRtrInfoFromRtrId (tGenU4Addr *pDestAddr);

tSrRtrInfo         *
SrGetSrRtrInfoFromNodeId (tGenU4Addr * pDestAddr);

tSrRtrInfo *
SrGetSrRtrInfoFromRtrAndAdvRtrId (tGenU4Addr *pDestAddr, UINT4 u4AdvRtrId);

INT4
SrMplsCreateOrDeleteStackLsp  (UINT2 u2MlibOperation, tSrRtrInfo * pSrRtrInfo,
                                tSrInSegInfo * pSrInSegInfo,
                                tSrTeLblStack * pSrTeLblStack,
                                tSrRtrNextHopInfo * pSrRtrNextHopInfo,
                                BOOL1 bFRRHwStatus);

PUBLIC tSrDelayRtInfo *SrCreateDelayRouteNode (VOID);
PUBLIC tSrRtrNextHopInfo * SrCreateNewNextHopNode(VOID);

tSrRlfaPathInfo   *
SrGetRlfaPathEntryFromDestAddr (tGenU4Addr *pDestAddr);
 
/* SR TE */
tSrTeRtEntryInfo   *
SrGetNextTeRouteTableEntry (tSrTeRtEntryInfo *pSrTePathInfo);

tSrTeRtEntryInfo   *
SrGetTeRouteEntryFromDestAddr (tGenU4Addr *pDestAddr);

INT4
SrDeleteTeRouteTableEntry(tSrTeRtEntryInfo *pSrTePathInfo);

UINT4
SrTeProgramAlternateLsp (tSrTeRtEntryInfo *pSrTeRtEntryInfo);

/* SR-TE APIs - end*/
INT4 
SrDeletePopNFwdUsingRtInfo (tGenU4Addr *pDestAddr, tGenU4Addr *pNextHop);

UINT4 SrNbrRtrFromNextHop(UINT4 u4NextHop);

VOID
SrIpv6RtChgEventHandler (tNetIpv6HliParams * pNetIpv6HlParams);

#ifdef OSPF3_WANTED
VOID 
SrRegWithOspf3ForInfoGet (VOID);
VOID 
SrDeRegWithOspf3ForInfoGet (VOID);
VOID 
SrOspf3AbrStatusChngHandler (UINT1 u1AbrStatus);
VOID
SrOspf3RtrIDChngHandler (UINT4 u4OspfRtrID);
INT1
SrUtilV3CapabilityOpaqueStatus (UINT1 *pu1RetVal,UINT4 u4SrCxtId);
#endif
#ifdef OSPF_WANTED
INT1
SrUtilV2CapabilityOpaqueStatus (UINT1 *pu1RetVal,UINT4 u4SrCxtId);
#endif
INT4
SrV3GetPeerRtrInfo(tSrRtrInfo *pSrRtrInfo, tGenU4Addr *prtrId, tNetIpv6LfaRtInfo *pIpv6LfaRtInfo);
VOID
SrUtilFetchOspf3AreadID (UINT4 u4IfIndex, UINT4 *u4AreaId);

PUBLIC UINT1
SrUtilIpAddrIndComp (tIPADDR ifIpAddr1,
            UINT4 u4AddrlessIf1, tIPADDR ifIpAddr2, UINT4 u4AddrlessIf2);

PUBLIC VOID
SrUtilOspfDwordToPdu (UINT1 *pu1PduAddr, UINT4 u4Value);

PUBLIC UINT1
SrUtilIpAddrComp (tIPADDR addr1, tIPADDR addr2);

PUBLIC UINT4
SrUtilOspfDwordFromPdu (UINT1 *pu1PduAddr);

PUBLIC INT4
SrDeletePeerNode (tSrRtrInfo *pRtrInfo);

VOID
SrRegWithOspfForInfoGet (VOID);
VOID
SrDeRegWithOspfForInfoGet (VOID);
VOID
SrOspfAbrStatusChngHandler (UINT1 u1AbrStatus);
VOID
SrOspfRtrIDChngHandler (UINT4 u4OspfRtrID);
PUBLIC UINT1 SrIsFlushLsa (tOsLsaInfo * pOsLsaInfo);
PUBLIC UINT4 SrDeleteAdvRtrIdForPrefix (UINT4 u4NodePrefix, UINT4 u4AdvRtrId);
PUBLIC UINT1 SrIsV3FlushLsa (tOs3LsaInfo * pOsLsaInfo);
PUBLIC VOID  SrGetNextHopFromRTM (UINT4 u4IpAddr, tGenU4Addr *pNxtHopIpAddr);
VOID
SrUtilFetchOspfv2AreadID (UINT4 u4OspfIfIpAddress, INT4 i4OspfAddressLessIf,
                          UINT4 *pu4RetValOspfIfAreaId);
INT4 SrTeProcess(tGenU4Addr *destAddr, UINT4);
VOID
SrLfaCreateLSP (tSrRtrInfo * pSrRtrInfo,
                        tSrRtrNextHopInfo * pSrRtrNextHopInfo);
INT4 SrDelFTNWithFRRForTE  (tGenU4Addr *pDestAddr, tSrRtrInfo * pRtrInfo);

/* SR TE and TE_LFA related functions prototype */
INT4 SrGetLabelForLFATEMandNode (tGenU4Addr *pLfaMandNode, tGenU4Addr *pMandNode,
                                                         UINT4 *pOutLabel);
INT4 SrTeLfaCreateFtnIlm (tSrTeRtEntryInfo *pSrTeRtEntryInfo);
UINT4 SrTeLfaRouteUpdate(tGenU4Addr *pDestAddr,tGenU4Addr *pMandatoryRtrId);
tSrTeRtEntryInfo   *
SrGetTeRouteTableEntry (tSrTeRtEntryInfo *pSrTeRtInfo);
INT4
SrTeCreateFtnRtChangeEvt (tGenU4Addr *destAddr, UINT4 *pu4ErrCode);
INT4
SrTeCreateFtnType10 (tGenU4Addr *destAddr, UINT4 *pu4ErrCode);
INT4
SrTeDeleteFtnRtChangeEvt (tGenU4Addr *pDestAddr, UINT4 *pu4ErrCode, tGenU4Addr *pNextHop);
INT4
SrTeDeleteFtnType10 (tGenU4Addr *pDestAddr, UINT4 *pu4ErrCode);
INT4 SrTeFtnIlmCreateDelViaDestRt(tSrTeRtEntryInfo *pSrTeDestRtInfo, UINT1 u1cmdType);
INT4 SrTeFtnIlmCreateDelViaMandRt(tGenU4Addr *destAddr, UINT1 u1cmdType, UINT1 u1RouteSetFlag);
INT4 SrTeFtnIlmCreateDelWithLfaViaMandRt(tGenU4Addr *destAddr, UINT1 u1cmdType);
INT4
SrTeLfaDeleteFtnIlm (tSrTeRtEntryInfo *pSrTeRtEntryInfo);

UINT4
SrActivateLFA (tSrRtrInfo *pSrRtrInfo, tSrRtrNextHopInfo *pSrRtrNextHopInfo);

PUBLIC UINT4
SrMplsModifyLSP (tSrIpEvtInfo * pSrIpEvtInfo, tSrRtrInfo * pSrRtrInfo,
                    tSrRtrNextHopInfo * pSrOldNextHopInfo, UINT1 u1IsNewNHeqFRRNH);
PUBLIC UINT4
SrMplsModifyRlfaLSP (tSrIpEvtInfo * pSrIpEvtInfo, tSrRtrInfo *pSrRtrInfo,
                    tSrRlfaPathInfo * pSrRlfaPathInfo, UINT1 u1IsNewNHeqFRRNH);

PUBLIC UINT4
SrMplsDeleteTunIntf (UINT4* u4MplsTnlIfIndex);

VOID
SRModifyNexthopHandle(tSrRtrInfo *pSrRtrInfo,
                        tSrRtrNextHopInfo * pSrNextHopInfo,
                        tSrRtrNextHopInfo * pSrOldNextHopInfo);
PUBLIC UINT4
SrMplsModifyPrimaryLSP (tSrIpEvtInfo * pSrIpEvtInfo,
                        tSrRtrInfo *pSrRtrInfo,
                        tSrRtrNextHopInfo * pSrNextHopInfo);
tSrRtrNextHopInfo *
SrV4GetSrNextHopInfoFromRtr (tSrRtrInfo *pSrRtrInfo, UINT4 u4NextHop);

UINT4
SrHandleRouteSetEvent (tNetIpv4LfaRtInfo *pLfaRtInfo);

INT4 SrTeFRRIlmAddorDelete(tGenU4Addr *destAddr,tSrTeRtEntryInfo *pSrTeRtEntryInfo, UINT1 u1cmdType);
INT4 SrTeLfaRtQuery(tGenU4Addr *pDestAddr, tNetIpv4LfaRtInfo  *pNetIpLfaRtInfo);

UINT4
SrFrrFtnCreateOrDelete (tSrRtrInfo *pRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo, UINT1 u1cmdType);
UINT4
SrFtnCreateorDelete (tSrRtrInfo *pRtrInfo, tSrRtrNextHopInfo * pSrRtrNextHopInfo, UINT1 u1cmdType);

INT4 SrTeFtnCreateOrDel(tSrTeRtEntryInfo *pSrTeDestRtInfo, UINT1 u1cmdType);
INT4
SrTeFRRFtnCreateOrDel (tSrTeRtEntryInfo *pSrTeRtEntryInfo,UINT1 u1cmdType);
PUBLIC UINT4
SrTeLfaFtnActivate(tSrRtrInfo* pSrTeMandRtrInfo );

PUBLIC UINT4
SrTeMplsModifyLSP (tSrRtrInfo *pSrRtrInfo, tSrTeRtEntryInfo *pSrTeDestRtInfo, UINT1 u1IsTeMandRtr);

PUBLIC VOID
SrNbrInfoCleanup (UINT4 u4AddrType);

PUBLIC VOID
SrDelMplsEntries (tSrRtrInfo *pNewRtrNode, INT1 i1ConflictDel);

PUBLIC UINT4
SrAddMplsEntries (tSrRtrInfo *pNewRtrNode);

PUBLIC VOID
SrChkAndActOnSIDConflict (tSrRtrInfo *pNewRtrNode, UINT4 u4SidVal, UINT1 u1PeerExist);

PUBLIC VOID
SrChkRtrListSidConflict (tSrRtrInfo *pNewRtrNode, UINT4 u4SidVal);

PUBLIC VOID
SrSidChkAndCreateEntriesForNxtRtr (UINT4 u4DelRtrSidVal, UINT4 u4DelAdvRtrId);

PUBLIC VOID
SrPrefixChkAndCreateEntriesForNxtRtr (UINT4 u4DelRtrPrefix, UINT4 u4DelAdvRtrId);

PUBLIC VOID
SrChkRtrListPrefixConflict (tSrRtrInfo * pNewRtrNode, UINT4 u4PrefixAddr);

PUBLIC VOID
SrChkAndActOnPrefixConflict (tSrRtrInfo *pNewRtrNode, UINT4 u4Prefix);

PUBLIC VOID
SrChkForOORAlarm (tSrRtrInfo *pRtrInfo, UINT4 u4SidVal);

PUBLIC VOID
SrGenerateAlarm (tSrRtrInfo *pRtrInfo, UINT1 u1GenAlarm, UINT1 u1Type);

PUBLIC UINT4
SrUtilCheckNodeIdPresent (UINT4 u4Addr);

PUBLIC UINT4
SrUtilCheckSIDPresent (UINT4 u4SidVal);

PUBLIC UINT4 SrUtilCheckNodeIdConfigured (VOID);

PUBLIC UINT4
SrUtilSendLSAForSid (UINT1 u1LsaStatus, UINT4 lsaTLVReceived, UINT1 u1lsaType,
             tSrSidInterfaceInfo *pSrSidIntf);

PUBLIC UINT4
SrUtilCheckNbrIsMultiSid (tGenU4Addr *nbrPrefixId, UINT4 u4AdvRtrId);

PUBLIC VOID
SrGenExtLsaOnAbrStatChng ();

UINT1
SrUtlCheckTnlInTnlDbTable (UINT1 dbOperation, UINT4 u4Prefix, UINT4 u4NextHop, tSrTnlIfTable** pSrTnlInfo);

INT4
SrFtnCfaIfmCreateStackMplsTunnelInterface(UINT4 prefix, UINT4 nexthop, UINT4 u4IfIndex, UINT4 *pu4MplsTnlIfIndex);

INT4
SrIlmCfaIfmCreateStackMplsTunnelInterface(UINT4 prefix, UINT4 nexthop, UINT4 u4IfIndex, UINT4 *pu4MplsTnlIfIndex);

INT4
SrFtnCfaIfmDeleteStackMplsTunnelInterface(UINT4 prefix, UINT4 nexthop, UINT4 u4IfIndex, UINT4 pu4MplsTnlIfIndex);

INT4
SrIlmCfaIfmDeleteStackMplsTunnelInterface(UINT4 prefix, UINT4 nexthop, UINT4 u4IfIndex, UINT4 pu4MplsTnlIfIndex);

#endif

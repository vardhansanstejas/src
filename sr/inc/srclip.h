/********************************************************************
 * Copyright (C) 2006 Altran Inc . All Rights Reserved
 *
 * $Id$
 *
 * Description: This file contains function prototypes.
 *
 ******************************************************************/
#ifndef _SR_CLI_H_
#define _SR_CLI_H_


/*Funtion Prototypes*/
INT4  SrCliProcessV4Srgb (UINT4 u4MinSid,UINT4 u4MaxSid);
INT4  SrCliProcessV6Srgb (UINT4 u4MinSid,UINT4 u4MaxSid);
INT4  SrConfigSrV4Status (UINT1 u1SrStatus);
INT4  SrCliProcessAdminUpCmdForV4 (VOID);
INT4  SrCliProcessAdminDownCmdForV4 (UINT4 u4CmdStatus);
INT4  SrCliProcessAdminUpCmdForV6 (VOID);
INT4  SrCliProcessAdminDownCmdForV6 (UINT4 u4CmdStatus);
INT4  SrCliProcessAddSid(tSrSidInterfaceInfo *pSrSidInfo, tGenU4Addr *pIfIpAddr, UINT4 u4IpAddrMask);

INT4  SrCliProcessAdjSidCfg (tSrSidInterfaceInfo * pSrSidInfo);

INT4  SrCliProcessDelSid(tCliHandle CliHandle,  tGenU4Addr *pIfIpAddr, UINT4 u4IpAddrMask);

INT4  SrCliAddStaticBinding(tCliHandle CliHandle, tGenU4Addr *pDestAddr,
							UINT4 u4Mask, tGenU4Addr *pNextHopAddr, UINT4 topLabel,
							UINT4 sidLabel[],UINT4 sidLabelDepth, UINT4 u4TnlId);

INT4 SrCliDelStaticBinding(tCliHandle CliHandle,tGenU4Addr *pDestAddr,
							UINT4 u4Mask, tGenU4Addr *pNextHopAddr, UINT4 topLabel,
							UINT4 sidLabel[],UINT4 sidLabelDepth, UINT4 u4TnlId);

INT4 SrCliShowConfig(tCliHandle CliHandle);
INT4 SrCliShowSidInterfaceList(tCliHandle CliHandle);
INT4 SrCliShowRouteInfo(tCliHandle CliHandle, UINT4 u4Prefix, UINT4 u4Mask);
INT4 SrCliShowPeerInfoList(tCliHandle CliHandle, UINT4 u4Command);
INT4 SrCliShowOspfSrCapability(tCliHandle CliHandle);

tSrSidInterfaceInfo *
SrCliGetSidInterface(tCliHandle CliHandle, UINT4 u4ifIpAddr,
							UINT4 u4SidLabelValue, UINT4 u4sidType, UINT4 u4ifIpAddrMask);

INT4 SrCliDefaultSrgb (UINT4 u1Afi);

INT4
SrCliSidIntfWalkFn(tRBElem * node, eRBVisit visit, UINT4 u4Level,
                            VOID *pArg, VOID *pOut);

UINT4
SrMplsSetAllFTNRowStatus (INT4 i4RowStatus);

UINT4
SrMplsSetAllILMRowStatus (INT4 i4RowStatus);

UINT4
SrSetMplsILMRowStatus (UINT4 u4XcIndex, UINT4 u4InIndex,
                        UINT4 u4OutIndex, INT4 i4SetValMplsXCRowStatus);

INT4 SrDeleteSidTable (UINT1 u1AddrType);
INT4 SrHandleAddSidReq(UINT4 u4PrefixSidType,UINT4 u4SidIndexType,UINT4 u4SidValue, tGenU4Addr *pGenAddr, UINT4 u4IpAddrMask);

INT4 SrDelSid(tSrSidInterfaceInfo *pSrSidEntry);

PUBLIC VOID
SrCliShowOptionalRouters(tCliHandle CliHandle, tGenU4Addr *pDestAddr,
							UINT4 u4DestMask, tGenU4Addr *pMandRtrId);
INT4 SrTeSetAllTePathInfoRowStatus (INT4 i4RowStatus, UINT2 u2AddrType);
INT4 SrV3LfaCliConfig(INT4 i4SetValFsLfaStatus);

VOID
SrAddOrDelILMSidTable (UINT1 u1AddrType, UINT2 u2IlmFlag);

UINT4
SrSetDebugLevel (tCliHandle CliHandle, UINT4 u4TrcVal, UINT4 u4TrcFlag);

VOID
SrAddorDelILM (tSrSidInterfaceInfo *pSrSidEntry, UINT2 u2IlmFlag);

#endif

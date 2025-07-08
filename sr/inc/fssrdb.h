/********************************************************************
* Copyright (C) 2006 Aricent Inc . All Rights Reserved
*
* $Id: wrap_code.c,v 1.1.1.1 2017/07/17 13:33:08 siva Exp $
*
* Description: Protocol Mib Data base
*********************************************************************/
#ifndef _FSSRDB_H
#define _FSSRDB_H

UINT1 FsSrSIDTableINDEX [] = {SNMP_DATA_TYPE_INTEGER ,SNMP_DATA_TYPE_OCTET_PRIM ,SNMP_DATA_TYPE_UNSIGNED32};
UINT1 FsSrTePathTableINDEX [] = {SNMP_DATA_TYPE_OCTET_PRIM ,SNMP_DATA_TYPE_UNSIGNED32 ,SNMP_DATA_TYPE_OCTET_PRIM ,SNMP_DATA_TYPE_INTEGER};
UINT1 FsSrTeRtrListTableINDEX [] = {SNMP_DATA_TYPE_UNSIGNED32 ,SNMP_DATA_TYPE_UNSIGNED32};
UINT1 FsStaticSrLabelStackTableINDEX [] = {SNMP_DATA_TYPE_UNSIGNED32 ,SNMP_DATA_TYPE_UNSIGNED32};
UINT1 FsStaticSrTableINDEX [] = {SNMP_DATA_TYPE_OCTET_PRIM ,SNMP_DATA_TYPE_UNSIGNED32 ,SNMP_DATA_TYPE_INTEGER};

UINT4 fssr [] ={1,3,6,1,4,1,2076,163,1};
tSNMP_OID_TYPE fssrOID = {9, fssr};


UINT4 FsSrMode [ ] ={1,3,6,1,4,1,2076,163,1,0,1};
UINT4 FsSrV4MinSrGbValue [ ] ={1,3,6,1,4,1,2076,163,1,0,2};
UINT4 FsSrV4MaxSrGbValue [ ] ={1,3,6,1,4,1,2076,163,1,0,3};
UINT4 FsSrSIDType [ ] ={1,3,6,1,4,1,2076,163,1,0,4,1,1};
UINT4 FsSrSidIndexType [ ] ={1,3,6,1,4,1,2076,163,1,0,4,1,2};
UINT4 FsSrSIDValue [ ] ={1,3,6,1,4,1,2076,163,1,0,4,1,3};
UINT4 FsSrSIDAddrType [ ] ={1,3,6,1,4,1,2076,163,1,0,4,1,4};
UINT4 FsSrSIDNetIp [ ] ={1,3,6,1,4,1,2076,163,1,0,4,1,5};
UINT4 FsSrSIDMask [ ] ={1,3,6,1,4,1,2076,163,1,0,4,1,6};
UINT4 FsSrFlags [ ] ={1,3,6,1,4,1,2076,163,1,0,4,1,7};
UINT4 FsSrMTID [ ] ={1,3,6,1,4,1,2076,163,1,0,4,1,8};
UINT4 FsSrAlgo [ ] ={1,3,6,1,4,1,2076,163,1,0,4,1,9};
UINT4 FsSrWeight [ ] ={1,3,6,1,4,1,2076,163,1,0,4,1,10};
UINT4 FsSrSIDRowStatus [ ] ={1,3,6,1,4,1,2076,163,1,0,4,1,11};
UINT4 FsSrDebugFlag [ ] ={1,3,6,1,4,1,2076,163,1,0,5};
UINT4 FsRouterListIndexNext [ ] ={1,3,6,1,4,1,2076,163,1,0,6};
UINT4 FsSrTeDestAddrType [ ] ={1,3,6,1,4,1,2076,163,1,0,7,1,1};
UINT4 FsSrTeDestAddr [ ] ={1,3,6,1,4,1,2076,163,1,0,7,1,2};
UINT4 FsSrTeDestMask [ ] ={1,3,6,1,4,1,2076,163,1,0,7,1,3};
UINT4 FsSrTeRouterId [ ] ={1,3,6,1,4,1,2076,163,1,0,7,1,4};
UINT4 FsSrTeRouterListIndex [ ] ={1,3,6,1,4,1,2076,163,1,0,7,1,5};
UINT4 FsSrTePathRowStatus [ ] ={1,3,6,1,4,1,2076,163,1,0,7,1,6};
UINT4 FsSrTeRtrListIndex [ ] ={1,3,6,1,4,1,2076,163,1,0,8,1,1};
UINT4 FsSrTeRouterIndex [ ] ={1,3,6,1,4,1,2076,163,1,0,8,1,2};
UINT4 FsSrTeOptRtrAddrType [ ] ={1,3,6,1,4,1,2076,163,1,0,8,1,3};
UINT4 FsSrTeOptRouterId [ ] ={1,3,6,1,4,1,2076,163,1,0,8,1,4};
UINT4 FsSrTeRtrRowStatus [ ] ={1,3,6,1,4,1,2076,163,1,0,8,1,5};
UINT4 FsSrV4Status [ ] ={1,3,6,1,4,1,2076,163,1,0,9};
UINT4 FsSrV4AlternateStatus [ ] ={1,3,6,1,4,1,2076,163,1,0,10};
UINT4 FsSrV6MinSrGbValue [ ] ={1,3,6,1,4,1,2076,163,1,0,11};
UINT4 FsSrV6MaxSrGbValue [ ] ={1,3,6,1,4,1,2076,163,1,0,12};
UINT4 FsSrV6AlternateStatus [ ] ={1,3,6,1,4,1,2076,163,1,0,13};
UINT4 FsSrV6Status [ ] ={1,3,6,1,4,1,2076,163,1,0,14};
UINT4 FsSrGlobalAdminStatus [ ] ={1,3,6,1,4,1,2076,163,1,0,15};
UINT4 FsSrIpv4AddrFamily [ ] ={1,3,6,1,4,1,2076,163,1,0,16};
UINT4 FsSrIpv6AddrFamily [ ] ={1,3,6,1,4,1,2076,163,1,0,17};
UINT4 FsStaticLabelStackIndex [ ] ={1,3,6,1,4,1,2076,163,1,0,18,1,1};
UINT4 FsStaticSrLabelIndex [ ] ={1,3,6,1,4,1,2076,163,1,0,18,1,2};
UINT4 FsStaticSrLabelStackLabel [ ] ={1,3,6,1,4,1,2076,163,1,0,18,1,3};
UINT4 FsStaticSrDestAddr [ ] ={1,3,6,1,4,1,2076,163,1,0,19,1,1};
UINT4 FsStaticSrDestMask [ ] ={1,3,6,1,4,1,2076,163,1,0,19,1,2};
UINT4 FsStaticSrDestAddrType [ ] ={1,3,6,1,4,1,2076,163,1,0,19,1,3};
UINT4 FsStaticSrNextHopAddr [ ] ={1,3,6,1,4,1,2076,163,1,0,19,1,4};
UINT4 FsStaticSrLabelStackIndex [ ] ={1,3,6,1,4,1,2076,163,1,0,19,1,5};
UINT4 FsStaticSrRowStatus [ ] ={1,3,6,1,4,1,2076,163,1,0,19,1,6};
UINT4 FsLabelStackIndexNext [ ] ={1,3,6,1,4,1,2076,163,1,0,20};



tMbDbEntry fssrMibEntry[]= {

{{11,FsSrMode}, NULL, FsSrModeGet, FsSrModeSet, FsSrModeTest, FsSrModeDep, SNMP_DATA_TYPE_INTEGER32, SNMP_READWRITE, NULL, 0, 0, 0, "0"},

{{11,FsSrV4MinSrGbValue}, NULL, FsSrV4MinSrGbValueGet, FsSrV4MinSrGbValueSet, FsSrV4MinSrGbValueTest, FsSrV4MinSrGbValueDep, SNMP_DATA_TYPE_UNSIGNED32, SNMP_READWRITE, NULL, 0, 0, 0, NULL},

{{11,FsSrV4MaxSrGbValue}, NULL, FsSrV4MaxSrGbValueGet, FsSrV4MaxSrGbValueSet, FsSrV4MaxSrGbValueTest, FsSrV4MaxSrGbValueDep, SNMP_DATA_TYPE_UNSIGNED32, SNMP_READWRITE, NULL, 0, 0, 0, NULL},

{{13,FsSrSIDType}, GetNextIndexFsSrSIDTable, FsSrSIDTypeGet, FsSrSIDTypeSet, FsSrSIDTypeTest, FsSrSIDTableDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, FsSrSIDTableINDEX, 3, 0, 0, NULL},

{{13,FsSrSidIndexType}, GetNextIndexFsSrSIDTable, FsSrSidIndexTypeGet, FsSrSidIndexTypeSet, FsSrSidIndexTypeTest, FsSrSIDTableDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, FsSrSIDTableINDEX, 3, 0, 0, "1"},

{{13,FsSrSIDValue}, GetNextIndexFsSrSIDTable, FsSrSIDValueGet, FsSrSIDValueSet, FsSrSIDValueTest, FsSrSIDTableDep, SNMP_DATA_TYPE_UNSIGNED32, SNMP_READWRITE, FsSrSIDTableINDEX, 3, 0, 0, NULL},

{{13,FsSrSIDAddrType}, GetNextIndexFsSrSIDTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_INTEGER, SNMP_NOACCESS, FsSrSIDTableINDEX, 3, 0, 0, NULL},

{{13,FsSrSIDNetIp}, GetNextIndexFsSrSIDTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_OCTET_PRIM, SNMP_NOACCESS, FsSrSIDTableINDEX, 3, 0, 0, NULL},

{{13,FsSrSIDMask}, GetNextIndexFsSrSIDTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_UNSIGNED32, SNMP_NOACCESS, FsSrSIDTableINDEX, 3, 0, 0, NULL},

{{13,FsSrFlags}, GetNextIndexFsSrSIDTable, FsSrFlagsGet, FsSrFlagsSet, FsSrFlagsTest, FsSrSIDTableDep, SNMP_DATA_TYPE_INTEGER32, SNMP_READWRITE, FsSrSIDTableINDEX, 3, 0, 0, NULL},

{{13,FsSrMTID}, GetNextIndexFsSrSIDTable, FsSrMTIDGet, FsSrMTIDSet, FsSrMTIDTest, FsSrSIDTableDep, SNMP_DATA_TYPE_INTEGER32, SNMP_READWRITE, FsSrSIDTableINDEX, 3, 0, 0, NULL},

{{13,FsSrAlgo}, GetNextIndexFsSrSIDTable, FsSrAlgoGet, FsSrAlgoSet, FsSrAlgoTest, FsSrSIDTableDep, SNMP_DATA_TYPE_INTEGER32, SNMP_READWRITE, FsSrSIDTableINDEX, 3, 0, 0, NULL},

{{13,FsSrWeight}, GetNextIndexFsSrSIDTable, FsSrWeightGet, FsSrWeightSet, FsSrWeightTest, FsSrSIDTableDep, SNMP_DATA_TYPE_INTEGER32, SNMP_READWRITE, FsSrSIDTableINDEX, 3, 0, 0, NULL},

{{13,FsSrSIDRowStatus}, GetNextIndexFsSrSIDTable, FsSrSIDRowStatusGet, FsSrSIDRowStatusSet, FsSrSIDRowStatusTest, FsSrSIDTableDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, FsSrSIDTableINDEX, 3, 0, 1, NULL},

{{11,FsSrDebugFlag}, NULL, FsSrDebugFlagGet, FsSrDebugFlagSet, FsSrDebugFlagTest, FsSrDebugFlagDep, SNMP_DATA_TYPE_UNSIGNED32, SNMP_READWRITE, NULL, 0, 0, 0, "0"},

{{11,FsRouterListIndexNext}, NULL, FsRouterListIndexNextGet, NULL, NULL, NULL, SNMP_DATA_TYPE_UNSIGNED32, SNMP_READONLY, NULL, 0, 0, 0, NULL},

{{13,FsSrTeDestAddrType}, GetNextIndexFsSrTePathTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_INTEGER, SNMP_NOACCESS, FsSrTePathTableINDEX, 4, 0, 0, NULL},

{{13,FsSrTeDestAddr}, GetNextIndexFsSrTePathTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_OCTET_PRIM, SNMP_NOACCESS, FsSrTePathTableINDEX, 4, 0, 0, NULL},

{{13,FsSrTeDestMask}, GetNextIndexFsSrTePathTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_UNSIGNED32, SNMP_NOACCESS, FsSrTePathTableINDEX, 4, 0, 0, NULL},

{{13,FsSrTeRouterId}, GetNextIndexFsSrTePathTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_OCTET_PRIM, SNMP_NOACCESS, FsSrTePathTableINDEX, 4, 0, 0, NULL},

{{13,FsSrTeRouterListIndex}, GetNextIndexFsSrTePathTable, FsSrTeRouterListIndexGet, FsSrTeRouterListIndexSet, FsSrTeRouterListIndexTest, FsSrTePathTableDep, SNMP_DATA_TYPE_UNSIGNED32, SNMP_READWRITE, FsSrTePathTableINDEX, 4, 0, 0, NULL},

{{13,FsSrTePathRowStatus}, GetNextIndexFsSrTePathTable, FsSrTePathRowStatusGet, FsSrTePathRowStatusSet, FsSrTePathRowStatusTest, FsSrTePathTableDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, FsSrTePathTableINDEX, 4, 0, 1, NULL},

{{13,FsSrTeRtrListIndex}, GetNextIndexFsSrTeRtrListTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_UNSIGNED32, SNMP_NOACCESS, FsSrTeRtrListTableINDEX, 2, 0, 0, NULL},

{{13,FsSrTeRouterIndex}, GetNextIndexFsSrTeRtrListTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_UNSIGNED32, SNMP_NOACCESS, FsSrTeRtrListTableINDEX, 2, 0, 0, NULL},

{{13,FsSrTeOptRtrAddrType}, GetNextIndexFsSrTeRtrListTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_INTEGER, SNMP_NOACCESS, FsSrTeRtrListTableINDEX, 2, 0, 0, NULL},

{{13,FsSrTeOptRouterId}, GetNextIndexFsSrTeRtrListTable, FsSrTeOptRouterIdGet, FsSrTeOptRouterIdSet, FsSrTeOptRouterIdTest, FsSrTeRtrListTableDep, SNMP_DATA_TYPE_OCTET_PRIM, SNMP_READWRITE, FsSrTeRtrListTableINDEX, 2, 0, 0, NULL},

{{13,FsSrTeRtrRowStatus}, GetNextIndexFsSrTeRtrListTable, FsSrTeRtrRowStatusGet, FsSrTeRtrRowStatusSet, FsSrTeRtrRowStatusTest, FsSrTeRtrListTableDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, FsSrTeRtrListTableINDEX, 2, 0, 1, NULL},

{{11,FsSrV4Status}, NULL, FsSrV4StatusGet, FsSrV4StatusSet, FsSrV4StatusTest, FsSrV4StatusDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, NULL, 0, 0, 0, "2"},

{{11,FsSrV4AlternateStatus}, NULL, FsSrV4AlternateStatusGet, FsSrV4AlternateStatusSet, FsSrV4AlternateStatusTest, FsSrV4AlternateStatusDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, NULL, 0, 0, 0, "2"},

{{11,FsSrV6MinSrGbValue}, NULL, FsSrV6MinSrGbValueGet, FsSrV6MinSrGbValueSet, FsSrV6MinSrGbValueTest, FsSrV6MinSrGbValueDep, SNMP_DATA_TYPE_UNSIGNED32, SNMP_READWRITE, NULL, 0, 0, 0, NULL},

{{11,FsSrV6MaxSrGbValue}, NULL, FsSrV6MaxSrGbValueGet, FsSrV6MaxSrGbValueSet, FsSrV6MaxSrGbValueTest, FsSrV6MaxSrGbValueDep, SNMP_DATA_TYPE_UNSIGNED32, SNMP_READWRITE, NULL, 0, 0, 0, NULL},

{{11,FsSrV6AlternateStatus}, NULL, FsSrV6AlternateStatusGet, FsSrV6AlternateStatusSet, FsSrV6AlternateStatusTest, FsSrV6AlternateStatusDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, NULL, 0, 0, 0, "2"},

{{11,FsSrV6Status}, NULL, FsSrV6StatusGet, FsSrV6StatusSet, FsSrV6StatusTest, FsSrV6StatusDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, NULL, 0, 0, 0, "2"},

{{11,FsSrGlobalAdminStatus}, NULL, FsSrGlobalAdminStatusGet, FsSrGlobalAdminStatusSet, FsSrGlobalAdminStatusTest, FsSrGlobalAdminStatusDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, NULL, 0, 0, 0, "2"},

{{11,FsSrIpv4AddrFamily}, NULL, FsSrIpv4AddrFamilyGet, FsSrIpv4AddrFamilySet, FsSrIpv4AddrFamilyTest, FsSrIpv4AddrFamilyDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, NULL, 0, 0, 0, "2"},

{{11,FsSrIpv6AddrFamily}, NULL, FsSrIpv6AddrFamilyGet, FsSrIpv6AddrFamilySet, FsSrIpv6AddrFamilyTest, FsSrIpv6AddrFamilyDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, NULL, 0, 0, 0, "2"},

{{13,FsStaticLabelStackIndex}, GetNextIndexFsStaticSrLabelStackTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_UNSIGNED32, SNMP_NOACCESS, FsStaticSrLabelStackTableINDEX, 2, 0, 0, NULL},

{{13,FsStaticSrLabelIndex}, GetNextIndexFsStaticSrLabelStackTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_UNSIGNED32, SNMP_NOACCESS, FsStaticSrLabelStackTableINDEX, 2, 0, 0, NULL},

{{13,FsStaticSrLabelStackLabel}, GetNextIndexFsStaticSrLabelStackTable, FsStaticSrLabelStackLabelGet, FsStaticSrLabelStackLabelSet, FsStaticSrLabelStackLabelTest, FsStaticSrLabelStackTableDep, SNMP_DATA_TYPE_UNSIGNED32, SNMP_READWRITE, FsStaticSrLabelStackTableINDEX, 2, 0, 0, NULL},

{{13,FsStaticSrDestAddr}, GetNextIndexFsStaticSrTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_OCTET_PRIM, SNMP_NOACCESS, FsStaticSrTableINDEX, 3, 0, 0, NULL},

{{13,FsStaticSrDestMask}, GetNextIndexFsStaticSrTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_UNSIGNED32, SNMP_NOACCESS, FsStaticSrTableINDEX, 3, 0, 0, NULL},

{{13,FsStaticSrDestAddrType}, GetNextIndexFsStaticSrTable, NULL, NULL, NULL, NULL, SNMP_DATA_TYPE_INTEGER, SNMP_NOACCESS, FsStaticSrTableINDEX, 3, 0, 0, NULL},

{{13,FsStaticSrNextHopAddr}, GetNextIndexFsStaticSrTable, FsStaticSrNextHopAddrGet, FsStaticSrNextHopAddrSet, FsStaticSrNextHopAddrTest, FsStaticSrTableDep, SNMP_DATA_TYPE_OCTET_PRIM, SNMP_READWRITE, FsStaticSrTableINDEX, 3, 0, 0, NULL},

{{13,FsStaticSrLabelStackIndex}, GetNextIndexFsStaticSrTable, FsStaticSrLabelStackIndexGet, FsStaticSrLabelStackIndexSet, FsStaticSrLabelStackIndexTest, FsStaticSrTableDep, SNMP_DATA_TYPE_UNSIGNED32, SNMP_READWRITE, FsStaticSrTableINDEX, 3, 0, 0, NULL},

{{13,FsStaticSrRowStatus}, GetNextIndexFsStaticSrTable, FsStaticSrRowStatusGet, FsStaticSrRowStatusSet, FsStaticSrRowStatusTest, FsStaticSrTableDep, SNMP_DATA_TYPE_INTEGER, SNMP_READWRITE, FsStaticSrTableINDEX, 3, 0, 1, NULL},

{{11,FsLabelStackIndexNext}, NULL, FsLabelStackIndexNextGet, NULL, NULL, NULL, SNMP_DATA_TYPE_UNSIGNED32, SNMP_READONLY, NULL, 0, 0, 0, NULL},
};
tMibData fssrEntry = { 46, fssrMibEntry };

#endif /* _FSSRDB_H */


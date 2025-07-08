/********************************************************************
* Copyright (C) 2006 Aricent Inc . All Rights Reserved
*
* $Id: fssrlw.h,v 1.13 2019/01/03 12:13:51 artee Exp $
*
* Description: Proto types for Low Level  Routines
*********************************************************************/

/* Proto type for Low Level GET Routine All Objects.  */

INT1
nmhGetFsSrMode ARG_LIST((INT4 *));

INT1
nmhGetFsSrV4MinSrGbValue ARG_LIST((UINT4 *));

INT1
nmhGetFsSrV4MaxSrGbValue ARG_LIST((UINT4 *));

/* Low Level SET Routine for All Objects.  */

INT1
nmhSetFsSrMode ARG_LIST((INT4 ));

INT1
nmhSetFsSrV4MinSrGbValue ARG_LIST((UINT4 ));

INT1
nmhSetFsSrV4MaxSrGbValue ARG_LIST((UINT4 ));

/* Low Level TEST Routines for.  */

INT1
nmhTestv2FsSrMode ARG_LIST((UINT4 *  ,INT4 ));

INT1
nmhTestv2FsSrV4MinSrGbValue ARG_LIST((UINT4 *  ,UINT4 ));

INT1
nmhTestv2FsSrV4MaxSrGbValue ARG_LIST((UINT4 *  ,UINT4 ));

/* Low Level DEP Routines for.  */

INT1
nmhDepv2FsSrMode ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

INT1
nmhDepv2FsSrV4MinSrGbValue ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

INT1
nmhDepv2FsSrV4MaxSrGbValue ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

/* Proto Validate Index Instance for FsSrSIDTable. */
INT1
nmhValidateIndexInstanceFsSrSIDTable ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4 ));

/* Proto Type for Low Level GET FIRST fn for FsSrSIDTable  */

INT1
nmhGetFirstIndexFsSrSIDTable ARG_LIST((INT4 * , tSNMP_OCTET_STRING_TYPE *  , UINT4 *));

/* Proto type for GET_NEXT Routine.  */

INT1
nmhGetNextIndexFsSrSIDTable ARG_LIST((INT4 , INT4 * , tSNMP_OCTET_STRING_TYPE *, tSNMP_OCTET_STRING_TYPE *  , UINT4 , UINT4 *));

/* Proto type for Low Level GET Routine All Objects.  */

INT1
nmhGetFsSrSIDType ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4 ,INT4 *));

INT1
nmhGetFsSrSidIndexType ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4 ,INT4 *));

INT1
nmhGetFsSrSIDValue ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4 ,UINT4 *));

INT1
nmhGetFsSrFlags ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4 ,INT4 *));

INT1
nmhGetFsSrMTID ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4 ,INT4 *));

INT1
nmhGetFsSrAlgo ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4 ,INT4 *));

INT1
nmhGetFsSrWeight ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4 ,INT4 *));

INT1
nmhGetFsSrSIDRowStatus ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4 ,INT4 *));

/* Low Level SET Routine for All Objects.  */

INT1
nmhSetFsSrSIDType ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

INT1
nmhSetFsSrSidIndexType ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

INT1
nmhSetFsSrSIDValue ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,UINT4 ));

INT1
nmhSetFsSrFlags ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

INT1
nmhSetFsSrMTID ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

INT1
nmhSetFsSrAlgo ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

INT1
nmhSetFsSrWeight ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

INT1
nmhSetFsSrSIDRowStatus ARG_LIST((INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

/* Low Level TEST Routines for.  */

INT1
nmhTestv2FsSrSIDType ARG_LIST((UINT4 *  ,INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

INT1
nmhTestv2FsSrSidIndexType ARG_LIST((UINT4 *  ,INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

INT1
nmhTestv2FsSrSIDValue ARG_LIST((UINT4 *  ,INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,UINT4 ));

INT1
nmhTestv2FsSrFlags ARG_LIST((UINT4 *  ,INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

INT1
nmhTestv2FsSrMTID ARG_LIST((UINT4 *  ,INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

INT1
nmhTestv2FsSrAlgo ARG_LIST((UINT4 *  ,INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

INT1
nmhTestv2FsSrWeight ARG_LIST((UINT4 *  ,INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

INT1
nmhTestv2FsSrSIDRowStatus ARG_LIST((UINT4 *  ,INT4  , tSNMP_OCTET_STRING_TYPE * , UINT4  ,INT4 ));

/* Low Level DEP Routines for.  */

INT1
nmhDepv2FsSrSIDTable ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

/* Proto type for Low Level GET Routine All Objects.  */

INT1
nmhGetFsSrDebugFlag ARG_LIST((UINT4 *));

INT1
nmhGetFsRouterListIndexNext ARG_LIST((UINT4 *));

/* Low Level SET Routine for All Objects.  */

INT1
nmhSetFsSrDebugFlag ARG_LIST((UINT4 ));

/* Low Level TEST Routines for.  */

INT1
nmhTestv2FsSrDebugFlag ARG_LIST((UINT4 *  ,UINT4 ));

/* Low Level DEP Routines for.  */

INT1
nmhDepv2FsSrDebugFlag ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

/* Proto Validate Index Instance for FsSrTePathTable. */
INT1
nmhValidateIndexInstanceFsSrTePathTable ARG_LIST((tSNMP_OCTET_STRING_TYPE * , UINT4  , tSNMP_OCTET_STRING_TYPE * , INT4 ));

/* Proto Type for Low Level GET FIRST fn for FsSrTePathTable  */

INT1
nmhGetFirstIndexFsSrTePathTable ARG_LIST((tSNMP_OCTET_STRING_TYPE *  , UINT4 * , tSNMP_OCTET_STRING_TYPE *  , INT4 *));

/* Proto type for GET_NEXT Routine.  */

INT1
nmhGetNextIndexFsSrTePathTable ARG_LIST((tSNMP_OCTET_STRING_TYPE *, tSNMP_OCTET_STRING_TYPE *  , UINT4 , UINT4 * , tSNMP_OCTET_STRING_TYPE *, tSNMP_OCTET_STRING_TYPE *  , INT4 , INT4 *));

/* Proto type for Low Level GET Routine All Objects.  */

INT1
nmhGetFsSrTeRouterListIndex ARG_LIST((tSNMP_OCTET_STRING_TYPE * , UINT4  , tSNMP_OCTET_STRING_TYPE * , INT4 ,UINT4 *));

INT1
nmhGetFsSrTePathRowStatus ARG_LIST((tSNMP_OCTET_STRING_TYPE * , UINT4  , tSNMP_OCTET_STRING_TYPE * , INT4 ,INT4 *));

/* Low Level SET Routine for All Objects.  */

INT1
nmhSetFsSrTeRouterListIndex ARG_LIST((tSNMP_OCTET_STRING_TYPE * , UINT4  , tSNMP_OCTET_STRING_TYPE * , INT4  ,UINT4 ));

INT1
nmhSetFsSrTePathRowStatus ARG_LIST((tSNMP_OCTET_STRING_TYPE * , UINT4  , tSNMP_OCTET_STRING_TYPE * , INT4  ,INT4 ));

/* Low Level TEST Routines for.  */

INT1
nmhTestv2FsSrTeRouterListIndex ARG_LIST((UINT4 *  ,tSNMP_OCTET_STRING_TYPE * , UINT4  , tSNMP_OCTET_STRING_TYPE * , INT4  ,UINT4 ));

INT1
nmhTestv2FsSrTePathRowStatus ARG_LIST((UINT4 *  ,tSNMP_OCTET_STRING_TYPE * , UINT4  , tSNMP_OCTET_STRING_TYPE * , INT4  ,INT4 ));

/* Low Level DEP Routines for.  */

INT1
nmhDepv2FsSrTePathTable ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

/* Proto Validate Index Instance for FsSrTeRtrListTable. */
INT1
nmhValidateIndexInstanceFsSrTeRtrListTable ARG_LIST((UINT4  , UINT4 ));

/* Proto Type for Low Level GET FIRST fn for FsSrTeRtrListTable  */

INT1
nmhGetFirstIndexFsSrTeRtrListTable ARG_LIST((UINT4 * , UINT4 *));

/* Proto type for GET_NEXT Routine.  */

INT1
nmhGetNextIndexFsSrTeRtrListTable ARG_LIST((UINT4 , UINT4 * , UINT4 , UINT4 *));

/* Proto type for Low Level GET Routine All Objects.  */

INT1
nmhGetFsSrTeOptRouterId ARG_LIST((UINT4  , UINT4 ,tSNMP_OCTET_STRING_TYPE * ));

INT1
nmhGetFsSrTeRtrRowStatus ARG_LIST((UINT4  , UINT4 ,INT4 *));

/* Low Level SET Routine for All Objects.  */

INT1
nmhSetFsSrTeOptRouterId ARG_LIST((UINT4  , UINT4  ,tSNMP_OCTET_STRING_TYPE *));

INT1
nmhSetFsSrTeRtrRowStatus ARG_LIST((UINT4  , UINT4  ,INT4 ));

/* Low Level TEST Routines for.  */

INT1
nmhTestv2FsSrTeOptRouterId ARG_LIST((UINT4 *  ,UINT4  , UINT4  ,tSNMP_OCTET_STRING_TYPE *));

INT1
nmhTestv2FsSrTeRtrRowStatus ARG_LIST((UINT4 *  ,UINT4  , UINT4  ,INT4 ));

/* Low Level DEP Routines for.  */

INT1
nmhDepv2FsSrTeRtrListTable ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

/* Proto type for Low Level GET Routine All Objects.  */

INT1
nmhGetFsSrV4Status ARG_LIST((INT4 *));

INT1
nmhGetFsSrV4AlternateStatus ARG_LIST((INT4 *));

INT1
nmhGetFsSrV6MinSrGbValue ARG_LIST((UINT4 *));

INT1
nmhGetFsSrV6MaxSrGbValue ARG_LIST((UINT4 *));

INT1
nmhGetFsSrV6AlternateStatus ARG_LIST((INT4 *));

INT1
nmhGetFsSrV6Status ARG_LIST((INT4 *));

INT1
nmhGetFsSrGlobalAdminStatus ARG_LIST((INT4 *));

INT1
nmhGetFsSrIpv4AddrFamily ARG_LIST((INT4 *));

INT1
nmhGetFsSrIpv6AddrFamily ARG_LIST((INT4 *));

/* Low Level SET Routine for All Objects.  */

INT1
nmhSetFsSrV4Status ARG_LIST((INT4 ));

INT1
nmhSetFsSrV4AlternateStatus ARG_LIST((INT4 ));

INT1
nmhSetFsSrV6MinSrGbValue ARG_LIST((UINT4 ));

INT1
nmhSetFsSrV6MaxSrGbValue ARG_LIST((UINT4 ));

INT1
nmhSetFsSrV6AlternateStatus ARG_LIST((INT4 ));

INT1
nmhSetFsSrV6Status ARG_LIST((INT4 ));

INT1
nmhSetFsSrGlobalAdminStatus ARG_LIST((INT4 ));

INT1
nmhSetFsSrIpv4AddrFamily ARG_LIST((INT4 ));

INT1
nmhSetFsSrIpv6AddrFamily ARG_LIST((INT4 ));

/* Low Level TEST Routines for.  */

INT1
nmhTestv2FsSrV4Status ARG_LIST((UINT4 *  ,INT4 ));

INT1
nmhTestv2FsSrV4AlternateStatus ARG_LIST((UINT4 *  ,INT4 ));

INT1
nmhTestv2FsSrV6MinSrGbValue ARG_LIST((UINT4 *  ,UINT4 ));

INT1
nmhTestv2FsSrV6MaxSrGbValue ARG_LIST((UINT4 *  ,UINT4 ));

INT1
nmhTestv2FsSrV6AlternateStatus ARG_LIST((UINT4 *  ,INT4 ));

INT1
nmhTestv2FsSrV6Status ARG_LIST((UINT4 *  ,INT4 ));

INT1
nmhTestv2FsSrGlobalAdminStatus ARG_LIST((UINT4 *  ,INT4 ));

INT1
nmhTestv2FsSrIpv4AddrFamily ARG_LIST((UINT4 *  ,INT4 ));

INT1
nmhTestv2FsSrIpv6AddrFamily ARG_LIST((UINT4 *  ,INT4 ));

/* Low Level DEP Routines for.  */

INT1
nmhDepv2FsSrV4Status ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

INT1
nmhDepv2FsSrV4AlternateStatus ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

INT1
nmhDepv2FsSrV6MinSrGbValue ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

INT1
nmhDepv2FsSrV6MaxSrGbValue ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

INT1
nmhDepv2FsSrV6AlternateStatus ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

INT1
nmhDepv2FsSrV6Status ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

INT1
nmhDepv2FsSrGlobalAdminStatus ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

INT1
nmhDepv2FsSrIpv4AddrFamily ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

INT1
nmhDepv2FsSrIpv6AddrFamily ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

/* Proto Validate Index Instance for FsStaticSrLabelStackTable. */
INT1
nmhValidateIndexInstanceFsStaticSrLabelStackTable ARG_LIST((UINT4  , UINT4 ));

/* Proto Type for Low Level GET FIRST fn for FsStaticSrLabelStackTable  */

INT1
nmhGetFirstIndexFsStaticSrLabelStackTable ARG_LIST((UINT4 * , UINT4 *));

/* Proto type for GET_NEXT Routine.  */

INT1
nmhGetNextIndexFsStaticSrLabelStackTable ARG_LIST((UINT4 , UINT4 * , UINT4 , UINT4 *));

/* Proto type for Low Level GET Routine All Objects.  */

INT1
nmhGetFsStaticSrLabelStackLabel ARG_LIST((UINT4  , UINT4 ,UINT4 *));

/* Low Level SET Routine for All Objects.  */

INT1
nmhSetFsStaticSrLabelStackLabel ARG_LIST((UINT4  , UINT4  ,UINT4 ));

/* Low Level TEST Routines for.  */

INT1
nmhTestv2FsStaticSrLabelStackLabel ARG_LIST((UINT4 *  ,UINT4  , UINT4  ,UINT4 ));

/* Low Level DEP Routines for.  */

INT1
nmhDepv2FsStaticSrLabelStackTable ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

/* Proto Validate Index Instance for FsStaticSrTable. */
INT1
nmhValidateIndexInstanceFsStaticSrTable ARG_LIST((tSNMP_OCTET_STRING_TYPE * , UINT4  , INT4, UINT4 ));

/* Proto Type for Low Level GET FIRST fn for FsStaticSrTable  */

INT1
nmhGetFirstIndexFsStaticSrTable ARG_LIST((tSNMP_OCTET_STRING_TYPE *  , UINT4 * , INT4 *, UINT4 *));

/* Proto type for GET_NEXT Routine.  */

INT1
nmhGetNextIndexFsStaticSrTable ARG_LIST((tSNMP_OCTET_STRING_TYPE *, tSNMP_OCTET_STRING_TYPE *  , UINT4 , UINT4 * , INT4 , INT4 *, UINT4 , UINT4 *));

/* Proto type for Low Level GET Routine All Objects.  */

INT1
nmhGetFsStaticSrNextHopAddr ARG_LIST((tSNMP_OCTET_STRING_TYPE * , UINT4  , INT4 , UINT4, tSNMP_OCTET_STRING_TYPE * ));

INT1
nmhGetFsStaticSrLabelStackIndex ARG_LIST((tSNMP_OCTET_STRING_TYPE * , UINT4  , INT4 , UINT4, UINT4 *));

INT1
nmhGetFsStaticSrRowStatus ARG_LIST((tSNMP_OCTET_STRING_TYPE * , UINT4  , INT4 ,UINT4, INT4 *));

/* Low Level SET Routine for All Objects.  */

INT1
nmhSetFsStaticSrNextHopAddr ARG_LIST((tSNMP_OCTET_STRING_TYPE * , UINT4  , INT4  ,UINT4, tSNMP_OCTET_STRING_TYPE *));

INT1
nmhSetFsStaticSrLabelStackIndex ARG_LIST((tSNMP_OCTET_STRING_TYPE * , UINT4  , INT4  ,UINT4, UINT4 ));

INT1
nmhSetFsStaticSrRowStatus ARG_LIST((tSNMP_OCTET_STRING_TYPE * , UINT4  , INT4  , UINT4, INT4 ));

/* Low Level TEST Routines for.  */

INT1
nmhTestv2FsStaticSrNextHopAddr ARG_LIST((UINT4 *  ,tSNMP_OCTET_STRING_TYPE * , UINT4  , INT4  , UINT4, tSNMP_OCTET_STRING_TYPE *));

INT1
nmhTestv2FsStaticSrLabelStackIndex ARG_LIST((UINT4 *  ,tSNMP_OCTET_STRING_TYPE * , UINT4  , INT4  , UINT4, UINT4 ));

INT1
nmhTestv2FsStaticSrRowStatus ARG_LIST((UINT4 *  ,tSNMP_OCTET_STRING_TYPE * , UINT4  , INT4  , UINT4, INT4 ));

/* Low Level DEP Routines for.  */

INT1
nmhDepv2FsStaticSrTable ARG_LIST((UINT4 *, tSnmpIndexList*, tSNMP_VAR_BIND*));

/* Proto type for Low Level GET Routine All Objects.  */

INT1
nmhGetFsLabelStackIndexNext ARG_LIST((UINT4 *));


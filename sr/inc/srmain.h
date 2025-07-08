
/* Copyright (C) 2017 Aricent Inc . All Rights Reserved
*******************************************************************************
**    FILE  NAME             : srmain.h
**    PRINCIPAL AUTHOR       : Aricent Inc.
**    SUBSYSTEM NAME         : SR
**    MODULE NAME            : SR
**    LANGUAGE               : ANSI-C
**    TARGET ENVIRONMENT     : Linux  (Portable)
**    DATE OF FIRST RELEASE  :
**    DESCRIPTION            : Contains macros and data structure definitions 
							   for SR task handling.
**---------------------------------------------------------------------------*/
#ifndef _SR_MAIN_H_
#define _SR_MAIN_H_


PUBLIC INT4 SrMainTaskInit (VOID);
/* MOved to sr.h as needed in isscli at reload
PUBLIC VOID SrMainTaskDeInit (VOID);
*/
PUBLIC UINT4 SrMainMemInit(VOID);
PUBLIC VOID SrMainMemClear(VOID);
PUBLIC VOID SrMainTask (VOID);
PUBLIC INT4 SrProcessQMsg (UINT4 u4Event);
PUBLIC VOID SrQueueHandler (tSrQMsg *pQMsg);
PUBLIC INT4 SrOspfMsgHandler (tOsToOpqApp * pOsToSrMsg);
PUBLIC VOID SRProcessDelayedRouteInfo(tSrRtrInfo  *pRtrInfo);
PUBLIC VOID SrProcessTimerEvent ();
#endif

/********************************************************************
 *** Copyright (C) 2024 Aricent Inc . All Rights Reserved
 ***
 *** $Id: srdb.h,v 1.1.1.1 2024/10/02
 ***
 *** Description: This file contains definition of structures
 ***              associated with stub db operation.
 ********************************************************************/

#ifndef _SRDB_H
#define _SRDB_H

#include "stubdb.h"

/* Table Name Macro definition */
#define SR_FTN_TNL_INFO_TBL_NAME "SrFTNTnlInfoTable"
#define SR_ILM_TNL_INFO_TBL_NAME "SrILMTnlInfoTable"

#define MAX_SR_TABLE_NAME_LEN       30
#define MAX_SR_TABLE_KEY_LEN        100
#define MAX_SR_TABLE_ENTRY_DATA_LEN 255

/* RB tree */
typedef struct _SRTnlIfTable
{
  tRBNodeEmbd   RbNode;
  UINT4   u4SrTnlIfIndex; /* data */
  UINT4   u4Prefix;       /* key1 */
  UINT4   u4NextHop;      /* key2 */
  UINT1   u1stale;
}tSrTnlIfTable;

#endif

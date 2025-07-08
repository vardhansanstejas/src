/*****************************************************************************/
/* Copyright (C) 2017 Aricent Inc . All Rights Reserved
 ******************************************************************************
 *    FILE  NAME             : srincs.c
 *    PRINCIPAL AUTHOR       : Aricent Inc. 
 *    SUBSYSTEM NAME         : SR 
 *    MODULE NAME            : SR
 *    LANGUAGE               : ANSI-C
 *    TARGET ENVIRONMENT     : Linux (Portable)                         
 *    DATE OF FIRST RELEASE  :
 *    DESCRIPTION            : This file includes all the header files required
 *    						   by Segment Routing so that the same need not be 
 *    						   done in individual files.
 *---------------------------------------------------------------------------*/
#ifndef _SR_INCS_H
#define _SR_INCS_H

#include "lr.h"
#include "ip.h"
#include "cfa.h"
#include "cli.h"
#include "trace.h"
#include "sr.h"
#include "srcli.h"
#include "srdb.h"
#include "mplscli.h"
#include "lblmgrex.h"
#include "srtrc.h"
#include "srdef.h"
#include "srtdefs.h"
#include "srmacs.h"
#include "srsz.h"
#include "fssrlw.h"
#include "mplsdefs.h"
#include "mplsdiff.h"
#include "mplfmext.h"
#include "mplcmndb.h"
#include "mplsprot.h"
#include "rtm.h"
#include "trie.h"

#ifdef MPLS_WANTED
#include "mpls.h"
#include "inmgrex.h"
#include "indexmgr.h"
#endif
#include "redblack.h"

#include "srlsa.h"
#include "ospf.h"

/*#include "osinc.h"*/
/*#include "osdefn.h"
#include "osbufif.h"
#include "oscfgvar.h"
#include "ostmrif.h"
#include "ostdfs.h"
#include "osmacs.h"
#include "ososif.h"
#include "osport.h"*/
#include "ospf3.h"
#include "ipv6.h"
#include "ip6util.h"
#include "srglob.h"
#include "lblmgrex.h"
#include "srrm.h"
#endif 

#!/bin/csh
# (C) 2017 Aricent Technologies Holdings Ltd.
# +--------------------------------------------------------------------------+
# |   FILE  NAME             : make.h                                        |
# |                                                                          |
# |   PRINCIPAL AUTHOR       : Aricent                                       |
# |                                                                          |
# |   MAKE TOOL(S) USED      : Eg: GNU MAKE                                  |
# |                                                                          |
# |   TARGET ENVIRONMENT     : LINUX                                         |
# |                                                                          |
# |   DATE                   : 27 august 2017                                |
# |                                                                          |
# |   DESCRIPTION            : Provide the following information in order -  |
# |                            1. Number of Submodules present if Main       |
# |                               makefile.                                  |
# |                            2. Clean option                               |
# +--------------------------------------------------------------------------+

###########################################################################
#               COMPILATION SWITCHES                                      #
###########################################################################

TOTAL_OPNS =  $(GENERAL_COMPILATION_SWITCHES) $(SYSTEM_COMPILATION_SWITCHES)

############################################################################
#                         Directories                                      #
############################################################################

SR_BASE_DIR = ${BASE_DIR}/mpls/sr
SR_SRC_DIR  = ${SR_BASE_DIR}/src
SR_INC_DIR  = ${SR_BASE_DIR}/inc
SR_OBJ_DIR  = ${SR_BASE_DIR}/obj
CLI_INC_DIR  = ${BASE_DIR}/inc/cli
OSPF_INC_DIR = $(BASE_DIR)/ospf/inc
OSPFV3_INC_DIR = $(BASE_DIR)/ospf3/inc
MPLS_INC_DIR   = $(BASE_DIR)/inc
############################################################################
##                     INCLUDE OPTIONS                                    ##
############################################################################

GLOBAL_INCLUDES  =  -I${SR_INC_DIR} \
                    -I$(MPLS_INC_DIR) \
                    -I$(SR_INC_DIR) \
                    -I$(CLI_INC_DIR) \
					-I$(BASE_DIR)/mpls/mplsinc \
					-I$(BASE_DIR)/mpls/mplsdb
INCLUDES         = ${GLOBAL_INCLUDES} ${COMMON_INCLUDE_DIRS}

#############################################################################


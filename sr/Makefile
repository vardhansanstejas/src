#####################################################################
#### (C) 2017 Aricent Group                                      ####
##|                                                               |##
##|    FILE NAME               ::  Makefile                       |##
##|                                                               |##
##|    DESCRIPTION             ::  Makefile for SR                |##
##|                                                               |##
##|###############################################################|##


.PHONY :  START ECHO DONE clean
include ../../LR/make.h
include ../../LR/make.rule
include ../make.h
include ../make.rule
include make.h


C_FLAGS          = ${CC_FLAGS} ${TOTAL_OPNS} ${INCLUDES}

SR_OBJS = \
	${SR_OBJ_DIR}/srcli.o \
	${SR_OBJ_DIR}/srmain.o  \
	${SR_OBJ_DIR}/srsz.o   \
	${SR_OBJ_DIR}/srtrc.o   \
	${SR_OBJ_DIR}/srutil.o  \
	${SR_OBJ_DIR}/fssrlw.o	\
	${SR_OBJ_DIR}/srlsa.o   \
	${SR_OBJ_DIR}/fssrwr.o \
	${SR_OBJ_DIR}/srte.o \
	${SR_OBJ_DIR}/srrm.o \
	${SR_OBJ_DIR}/srgr.o \
	${SR_OBJ_DIR}/srrmport.o \
	${SR_OBJ_DIR}/srdb.o

#ifeq (${CLI}, YES)
#SR_OBJS += ${SR_OBJ_DIR}/srcli.o
#endif
#ifeq (${SNMP_2}, YES)		
#SR_OBJS +=\
#${SR_OBJ_DIR}/stdtlmwr.o \
#${SR_OBJ_DIR}/fstlmwr.o
#endif

#Final Object
SR_FINAL_OBJ    = ${SR_OBJ_DIR}/FutureSR.o

START : ECHO ${SR_FINAL_OBJ} DONE

ECHO  :
	 @echo Making SR Module...

DONE  :
	 @echo SR Module done.

#object module dependency
$(SR_FINAL_OBJ): obj ${SR_OBJS}
	${LD} -r -o $(SR_FINAL_OBJ) ${SR_OBJS}

EXTERNAL_DEPENDENCIES = \
    $(COMN_INCL_DIR)/lr.h \
#    $(COMN_INCL_DIR)/fssnmp.h \	
#    $(COMN_INCL_DIR)/ospfte.h \
	$(COMN_INCL_DIR)/sr.h

INTERNAL_DEPENDENCIES = \
	${SR_BASE_DIR}/make.h \
	${SR_BASE_DIR}/Makefile \
        ${SR_INC_DIR}/srmacs.h \
        ${SR_INC_DIR}/srtdefs.h \
        ${SR_INC_DIR}/srclip.h \
        ${SR_INC_DIR}/srglob.h \
        ${SR_INC_DIR}/srsz.h \
        ${SR_INC_DIR}/srincs.h \
        ${SR_INC_DIR}/srtrc.h \
		${SR_INC_DIR}/srdef.h \
		${SR_INC_DIR}/srlsa.h \
		${SR_INC_DIR}/srrm.h \
		${SR_INC_DIR}/srdb.h
DEPENDENCIES =  \
	${COMMON_DEPENDENCIES} \
	${EXTERNAL_DEPENDENCIES} \
	${INTERNAL_DEPENDENCIES}

obj:
ifdef MKDIR
	$(MKDIR) $(MKDIR_FLAGS) $(SR_OBJ_DIR)
endif

$(SR_OBJ_DIR)/srmain.o : \
	$(SR_SRC_DIR)/srmain.c \
	${DEPENDENCIES}
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/srmain.c

$(SR_OBJ_DIR)/srutil.o : \
	$(SR_SRC_DIR)/srutil.c \
	${DEPENDENCIES}
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/srutil.c

$(SR_OBJ_DIR)/srcli.o : \
	$(SR_SRC_DIR)/srcli.c \
	${DEPENDENCIES} 
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/srcli.c

$(SR_OBJ_DIR)/srsz.o : \
	$(SR_SRC_DIR)/srsz.c \
	${DEPENDENCIES}
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/srsz.c

$(SR_OBJ_DIR)/srtrc.o : \
	$(SR_SRC_DIR)/srtrc.c \
	${DEPENDENCIES}
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/srtrc.c

$(SR_OBJ_DIR)/fssrlw.o: \
	$(SR_SRC_DIR)/fssrlw.c \
	${DEPENDENCIES}
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/fssrlw.c

$(SR_OBJ_DIR)/fssrwr.o : \
	$(SR_SRC_DIR)/fssrwr.c \
	${DEPENDENCIES}
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/fssrwr.c

$(SR_OBJ_DIR)/srlsa.o : \
	$(SR_SRC_DIR)/srlsa.c \
	${DEPENDENCIES}
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/srlsa.c

$(SR_OBJ_DIR)/srte.o : \
	$(SR_SRC_DIR)/srte.c \
	${DEPENDENCIES}
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/srte.c

$(SR_OBJ_DIR)/srrm.o : \
	$(SR_SRC_DIR)/srrm.c \
	${DEPENDENCIES}
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/srrm.c

$(SR_OBJ_DIR)/srgr.o : \
	$(SR_SRC_DIR)/srgr.c \
	${DEPENDENCIES}
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/srgr.c

$(SR_OBJ_DIR)/srrmport.o : \
	$(SR_SRC_DIR)/srrmport.c \
	${DEPENDENCIES}
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/srrmport.c

$(SR_OBJ_DIR)/srdb.o : \
	$(SR_SRC_DIR)/srdb.c \
	${DEPENDENCIES}
	$(CC) $(C_FLAGS) -o $@ $(SR_SRC_DIR)/srdb.c

clean:
	$(RM) $(RM_FLAGS) $(SR_FINAL_OBJ) $(SR_OBJS)
	echo "Cleaning the SR object files"


#pkg:
#	CUR_PWD=${PWD};\
        cd ${BASE_DIR}/../..;\
        tar cvzf ${ISS_PKG_CREATE_DIR}/sr.tgz -T ${BASE_DIR}/sr/FILES.NEW;\
        cd ${CUR_PWD};



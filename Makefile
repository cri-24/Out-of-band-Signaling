#cartelle dei file
LIB=./lib
BUILD=./build
TEST=./test
HEADER=./header

#compilatore
CC		    =  gcc
AR          =  ar
CCFLAGS	    = -std=c99 -Wall -g -D_POSIX_C_SOURCE=199309L 
OPTFLAGS	= -O3 
ARFLAGS     =  rvs
TH          = -pthread

#esecuzione
TARGETS=$(BUILD)/supervisor\
 		 $(BUILD)/client\
		 $(BUILD)/server 
			
.PHONY: all test clean cleanall 
all		: $(TARGETS)

#eseguibile del client
$(BUILD)/client: $(BUILD)/client.o $(BUILD)/support_client.o $(LIB)/libclient.a
	$(CC) $(CCFLAGS) $(OPTFLAGS) -I $(HEADER) -L $(LIB) $^ -o $@ 

#eseguibile del server
$(BUILD)/server: $(BUILD)/server.o $(BUILD)/support_server.o $(BUILD)/worker.o $(LIB)/libserver.a 
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(TH) -I $(HEADER) -L $(LIB) $^ -o $@ 

#eseguibile del supervisor
$(BUILD)/supervisor: $(BUILD)/supervisor.o $(BUILD)/support_supervisor.o $(LIB)/libsupervisor.a 
	$(CC) $(CCFLAGS) $(OPTFLAGS) -I $(HEADER) -L $(LIB) $^ -o $@ 

#libreria del supervisor
$(LIB)/libsupervisor.a: $(BUILD)/supervisor.o $(BUILD)/support_supervisor.o
	$(AR) $(ARFLAGS) $@ $^

#libreria del server
$(LIB)/libserver.a: $(BUILD)/server.o $(BUILD)/support_server.o $(BUILD)/worker.o 
	$(AR) $(ARFLAGS) $@ $^

#libreria del client
$(LIB)/libclient.a: $(BUILD)/client.o $(BUILD)/support_client.o 
	$(AR) $(ARFLAGS) $@ $^

#compilazione di tutti i file sorgenti
$(BUILD)/supervisor.o: supervisor.c $(HEADER)/supervisor.h $(HEADER)/utils.h
	$(CC) $(CCFLAGS) $(OPTFLAGS) -I $(HEADER) -o $@ -c $<

$(BUILD)/support_supervisor.o: support_supervisor.c $(HEADER)/supervisor.h $(HEADER)/utils.h
	$(CC) $(CCFLAGS) $(OPTFLAGS) -I $(HEADER) -o $@ -c $<

$(BUILD)/server.o: server.c $(HEADER)/server.h $(HEADER)/utils.h
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(TH) -I $(HEADER) -o $@ -c $<

$(BUILD)/support_server.o: support_server.c $(HEADER)/server.h $(HEADER)/utils.h
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(TH) -I $(HEADER) -o $@ -c $<

$(BUILD)/worker.o: worker.c $(HEADER)/worker.h $(HEADER)/utils.h
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(TH) -I $(HEADER) -o $@ -c $<

$(BUILD)/client.o: client.c $(HEADER)/client.h $(HEADER)/utils.h
	$(CC) $(CCFLAGS) $(OPTFLAGS) -I $(HEADER) -o $@ -c $<

$(BUILD)/support_client.o: support_client.c $(HEADER)/client.h $(HEADER)/utils.h
	$(CC) $(CCFLAGS) $(OPTFLAGS) -I $(HEADER) -o $@ -c $<

test:
		bash ./test/test.sh

clean		: 
	rm -f $(TARGETS)

cleanall	: clean
	rm -f $(BUILD)/*.o $(LIB)/*.a OOB* $(BUILD)/*.log
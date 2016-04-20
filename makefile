common_dir=./common/src
tinyxml_dir=./tinyxml/src
leader_dir=./consensus_leader/src
follower_dir=./consensus_follower/src
client_dir=./consensus_client/src
test_dir=./consensus_test/src
bin_dir=./bin

common_include= $(common_dir)/*.h
tinyxml_include=$(tinyxml_dir)/*.h
leader_include= $(leader_dir)/*.h
follower_include= $(follower_dir)/*.h
client_include= $(client_dir)/*.h
test_include= $(test_dir)/*.h

.PHONY: default
default:
	#@mkdir -p bin
	@[ -d bin ] || mkdir bin 
	@[ -f bin/follower_conf.xml ] || \
		cp ./consensus_follower/follower_conf.xml $(bin_dir)/
	@[ -f bin/leader_conf.xml ] || \
		cp ./consensus_leader/leader_conf.xml $(bin_dir)/
	make all
.PHONY: all
all :   $(bin_dir)/libcommon.a \
		$(bin_dir)/libtinyxml.a \
		$(bin_dir)/leader \
		$(bin_dir)/follower  \
		$(bin_dir)/client \
		$(bin_dir)/test
	@echo "done"

########################################################################
#                            common
########################################################################
$(bin_dir)/libcommon.a:$(bin_dir)/Message.o $(bin_dir)/IO.o
	@echo common
	ar -r $@ $^

$(bin_dir)/Message.o : $(common_dir)/Message.cpp $(common_dir)/Message.h
	g++ -g -c $< -o $@

$(bin_dir)/IO.o : $(common_dir)/IO.cpp $(common_dir)/IO.h
	g++ -g -c $< -o $@

########################################################################
#                            tinyxml
########################################################################
$(bin_dir)/libtinyxml.a : $(bin_dir)/tinyxml.o \
						  $(bin_dir)/tinystr.o \
						  $(bin_dir)/tinyxmlerror.o \
						  $(bin_dir)/tinyxmlparser.o
	@echo tinyxml
	ar -r $@ $^

$(bin_dir)/tinyxml.o : $(tinyxml_dir)/tinyxml.cpp $(tinyxml_dir)/*.h
	g++ -g -c $< -I $(tinyxml_dir) -o $@

$(bin_dir)/tinystr.o : $(tinyxml_dir)/tinystr.cpp $(tinyxml_dir)/*.h
	g++ -g -c $< -I $(tinyxml_dir) -o $@

$(bin_dir)/tinyxmlerror.o : $(tinyxml_dir)/tinyxmlerror.cpp \
							$(tinyxml_dir)/*.h
	g++ -g -c $< -I $(tinyxml_dir) -o $@
	
$(bin_dir)/tinyxmlparser.o : $(tinyxml_dir)/tinyxmlparser.cpp \
							 $(tinyxml_dir)/*.h
	g++ -g -c $< -I $(tinyxml_dir) -o $@

	
########################################################################
#                            leader
########################################################################
$(bin_dir)/leader : $(bin_dir)/consensus_leader.o \
					$(bin_dir)/ProcessCmdThread.o \
					$(bin_dir)/SynInitDateThread.o \
					$(bin_dir)/SynTemporaryDataThread.o
	@echo leader
	g++ -g $^ -lpthread -lcommon -ltinyxml -L $(bin_dir) -o $@

$(bin_dir)/consensus_leader.o : $(leader_dir)/consensus_leader.cpp \
								$(common_include) \
								$(tinyxml_include) \
								$(leader_include)
	g++ -g -c $< -I $(leader_dir) -I $(common_dir) -I $(tinyxml_dir) -o $@

$(bin_dir)/ProcessCmdThread.o : $(leader_dir)/ProcessCmdThread.cpp \
								$(common_include) \
								$(leader_include)
	g++ -g -c $< -I $(leader_dir) -I $(common_dir) -o $@

$(bin_dir)/SynInitDateThread.o : $(leader_dir)/SynInitDateThread.cpp \
								$(common_include) \
								$(leader_include)
	g++ -g -c $< -I $(leader_dir) -I $(common_dir) -o $@

$(bin_dir)/SynTemporaryDataThread.o : $(leader_dir)/SynTemporaryDataThread.cpp \
								$(common_include) \
								$(leader_include)
	g++ -g -c $< -I $(leader_dir) -I $(common_dir) -o $@


########################################################################
#                            follower
########################################################################
$(bin_dir)/follower : $(bin_dir)/ClientProcessClient.o \
						$(bin_dir)/ConnectToLeader.o \
						$(bin_dir)/consensus_follower.o \
						$(bin_dir)/LeaderCmdThread.o \
						$(bin_dir)/LeaderInitThread.o \
						$(bin_dir)/LeaderSynThread.o
	@echo follower
	g++ -g $^ -lpthread -lcommon -ltinyxml -L $(bin_dir) -o $@

$(bin_dir)/consensus_follower.o : $(follower_dir)/consensus_follower.cpp \
									$(follower_include) \
									$(common_include) \
									$(tinyxml_include)
	g++ -g -c $< -I $(follower_dir) -I $(common_dir) -I $(tinyxml_dir) -o $@

$(bin_dir)/ClientProcessClient.o : $(follower_dir)/ClientProcessClient.cpp \
									$(follower_include) \
									$(common_include)
	g++ -g -c $< -I $(follower_dir) -I $(common_dir) -o $@

$(bin_dir)/ConnectToLeader.o : $(follower_dir)/ConnectToLeader.cpp \
									$(follower_include) \
									$(common_include)
	g++ -g -c $< -I $(follower_dir) -I $(common_dir) -o $@


$(bin_dir)/LeaderCmdThread.o : $(follower_dir)/LeaderCmdThread.cpp \
									$(follower_include) \
									$(common_include)
	g++ -g -c $< -I $(follower_dir) -I $(common_dir) -o $@

$(bin_dir)/LeaderInitThread.o : $(follower_dir)/LeaderInitThread.cpp \
									$(follower_include) \
									$(common_include)
	g++ -g -c $< -I $(follower_dir) -I $(common_dir) -o $@

$(bin_dir)/LeaderSynThread.o : $(follower_dir)/LeaderSynThread.cpp \
									$(follower_include) \
									$(common_include)
	g++ -g -c $< -I $(follower_dir) -I $(common_dir) -o $@


########################################################################
#                            client
########################################################################
$(bin_dir)/client:$(bin_dir)/consensus_client.o \
					$(bin_dir)/DistributedLock.o
	@echo client
	g++ -g $^ -L $(bin_dir) -lcommon -o $@

$(bin_dir)/consensus_client.o:$(client_dir)/consensus_client.cpp \
								$(common_inlude) \
								$(client_include)
	g++ -g -c $< -I $(common_dir) -I $(client_dir) -o $@

$(bin_dir)/DistributedLock.o : $(client_dir)/DistributedLock.cpp \
								$(common_inlude) \
								$(client_include)
	g++ -g -c $< -I $(common_dir) -I $(client_dir) -o $@
########################################################################
#                            test
########################################################################
$(bin_dir)/test:$(bin_dir)/consensus_test.o \
					$(bin_dir)/DistributedLock.o
	@echo test
	g++ -g $^ -L $(bin_dir) -lcommon -o $@

$(bin_dir)/consensus_test.o:$(test_dir)/consensus_test.cpp \
								$(common_inlude) \
								$(test_include)
	g++ -g -c $< -I $(common_dir) -I $(test_dir) -o $@

$(bin_dir)/DistributedLock.o : $(test_dir)/DistributedLock.cpp \
								$(common_inlude) \
								$(test_include)
	g++ -g -c $< -I $(common_dir) -I $(test_dir) -o $@
##########################################################################
.PHONY: clean
clean:
	-rm -r ./bin

common_dir=./common/src
leader_dir=./consensus_leader/src
follower_dir=./consensus_follower/src
client_dir=./consensus_client/src
bin_dir=./bin

common_include= $(common_dir)/*.h
leader_include= $(leader_dir)/*.h
follower_include= $(follower_dir)/*.h
client_include= $(client_dir)/*.h

.PHONY: default
default:
	#@mkdir -p bin
	@[ -d bin ] || mkdir bin 
	@echo $(common_dir)/test
	make all
.PHONY: all
all : $(bin_dir)/libcommon.a \
		$(bin_dir)/leader \
		$(bin_dir)/follower  \
		$(bin_dir)/client
	@echo "done"

$(bin_dir)/libcommon.a:$(bin_dir)/Message.o $(bin_dir)/IO.o
	@echo common
	ar -r $@ $^

$(bin_dir)/Message.o : $(common_dir)/Message.cpp $(common_dir)/Message.h
	g++ -g -c $< -o $@

$(bin_dir)/IO.o : $(common_dir)/IO.cpp $(common_dir)/IO.h
	g++ -g -c $< -o $@


########################################################################
#                            leader
########################################################################
$(bin_dir)/leader : $(bin_dir)/consensus_leader.o \
					$(bin_dir)/ProcessCmdThread.o \
					$(bin_dir)/SynInitDateThread.o \
					$(bin_dir)/SynTemporaryDataThread.o
	@echo leader
	g++ -g $^ -lpthread -lcommon -L $(bin_dir) -o $@

$(bin_dir)/consensus_leader.o : $(leader_dir)/consensus_leader.cpp \
								$(common_include) \
								$(leader_include)
	g++ -g -c $< -I $(leader_dir) -I $(common_dir) -o $@

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
	g++ -g $^ -lpthread -lcommon -L $(bin_dir) -o $@

$(bin_dir)/ClientProcessClient.o : $(follower_dir)/ClientProcessClient.cpp \
									$(follower_include) \
									$(common_include)
	g++ -g -c $< -I $(follower_dir) -I $(common_dir) -o $@

$(bin_dir)/ConnectToLeader.o : $(follower_dir)/ConnectToLeader.cpp \
									$(follower_include) \
									$(common_include)
	g++ -g -c $< -I $(follower_dir) -I $(common_dir) -o $@

$(bin_dir)/consensus_follower.o : $(follower_dir)/consensus_follower.cpp \
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
.PHONY: clean
clean:
	-rm -r ./bin

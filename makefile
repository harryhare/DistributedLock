common_dir=./common/src
leader_dir=./consensus_leader/src
follower_dir=./consensus_follower/src
client_dir=./consensus_client/src
bin_dir=./bin

common_include= $(common_dir)/Message.h
leader_include= 
follower_include= 
client_include= $(client_dir)/DistributedLock.h

default:
	@-mkdir ./bin >/dev/null 2>&1
	@echo $(common_dir)/test
	make all

all : $(bin_dir)/libcommon.a \
		$(bin_dir)/leader \
		$(bin_dir)/follower  \
		$(bin_dir)/client
	@echo "done"

$(bin_dir)/libcommon.a:$(bin_dir)/Message.o
	@echo common
	ar -r $@ $^

$(bin_dir)/Message.o : $(common_dir)/Message.cpp $(common_dir)/Message.h
	g++ -g -c $(common_dir)/Message.cpp  -o $@

$(bin_dir)/leader : 
	@echo leader
#	g++ -g -o ./bin/leader

$(bin_dir)/follower : $(bin_dir)/consensus_follower.o
	@echo follower
	g++ -g $^ -lcommon -L $(bin_dir) -o $@

$(bin_dir)/consensus_follower.o : $(follower_dir)/consensus_follower.cpp \
									$(follower_include) \
									$(common_include)
	g++ -g -c $< -I $(follower_dir) -I $(common_dir) -o $@

$(bin_dir)/client:$(bin_dir)/consensus_client.o \
					$(bin_dir)/DistributedLock.o
	@echo client
	g++ -g $^ -L $(bin_dir) -lcommon -o $@

$(bin_dir)/consensus_client.o:$(client_dir)/consensus_client.cpp \
								$(common_inlude) \
								$(client_include)
	g++ -g -c $< -I $(common_dir) -I $(client_dir) -o $@

$(bin_dir)/DistributedLock.o : $(client_dir)/DistributedLock.* \
								$(common_inlude) \
								$(client_include)
	g++ -g -c $< -I $(common_dir) -I $(client_dir) -o $@

clean:
	-rm -r ./bin

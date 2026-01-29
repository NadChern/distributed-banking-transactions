CPPFLAGS = -std=c++20 -Wall -Werror -pedantic -ggdb -pthread
HDRS = TCPServer.h TCPClient.h Protocol.h 2PC_Participant.h 2PC_Coordinator.h
PARTICIPANT = participant
COORDINATOR = coordinator

# Define the script files
RUN-SCRIPT = runPC.sh
CLEAN-LOGS = clean-logs.sh

# Define the object files
%.o : %.cpp $(HDRS)
	g++ $(CPPFLAGS) -c $< -o $@

# Define the targets
participant : participant.o TCPServer.o TCPClient.o 2PC_Participant.o
	g++ -lpthread $^ -o $@

coordinator : coordinator.o TCPServer.o TCPClient.o 2PC_Coordinator.o
	g++ -lpthread $^ -o $@

# Define the build
manual:
	cmake -S . -B build
	cmake --build build


p: $(PARTICIPANT)

c: $(COORDINATOR)

all: $(PARTICIPANT) $(COORDINATOR)

# Define the default goal
.DEFAULT_GOAL := all

.PHONY: run-p run-c clean clean-logs manual all p c

# Run participants
run-p:
	chmod +x $(RUN-SCRIPT)
	./$(RUN-SCRIPT) participants

# Run coordinator
run-c:
	chmod +x $(RUN-SCRIPT)
	./$(RUN-SCRIPT) coordinator


# Clean the log files
clean-logs:
	chmod +x $(CLEAN-LOGS)
	./$(CLEAN-LOGS)

# Clean the object files and executables with the log files
clean: 
	chmod +x $(CLEAN-LOGS)
	./$(CLEAN-LOGS)
	rm -rf *.o $(PARTICIPANT) $(COORDINATOR) build
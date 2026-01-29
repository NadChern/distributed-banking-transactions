# Project Overview

This project involves developing a client-server system in C++ using the C socket system calls. The client will act as a bank transaction coordinator that manages a 2-phase commit (2PC) protocol with other banks. There will be two instances of the server, one for each of two other banks, each running as a 2PC participant.

---

### Key Components

1. Server (Participant)

   - Each server instance represents a bank participating in the transaction.
   - The server listens for transaction requests from the coordinator and responds accordingly.

2. Client (Coordinator)

   - The client acts as the transaction coordinator, managing the 2PC protocol.
   - It initiates a transaction request, collects votes from participants, and decides whether to commit or abort the transaction.

### 2-Phase Commit Protocol

The coordinator handles a transaction in two phases:

1. Phase 1: Voting Phase

   - The coordinator sends a VOTE-REQUEST to all participants.
   - Each participant responds with either VOTE-COMMIT (approve) or VOTE-ABORT (disapprove).

2. Phase 2: Commit/Abort Phase

   - If all participants send VOTE-COMMIT, the coordinator sends GLOBAL-COMMIT to all participants, instructing them to commit the transaction.
   - If any participant sends a VOTE-ABORT, the coordinator sends a GLOBAL-ABORT to those who previously responded with a VOTE-COMMIT, instructing them to abort the transaction.
   - Participants acknowledge receipt of the global decision with an ACK (except those who sent a VOTE-ABORT).

### Protocol Messages

- VOTE-REQUEST: Coordinator requests participants to vote on the transaction.
- VOTE-COMMIT: Participant approves the transaction.
- VOTE-ABORT: Participant disapproves the transaction.
- GLOBAL-COMMIT: Coordinator instructs participants to commit the transaction.
- GLOBAL-ABORT: Coordinator instructs participants to abort the transaction.
- ACK: Participant acknowledges the coordinator's decision.

### Failure recovery (for extra points)
The Participant class handles failure recovery by implementing a rollback of any uncommitted changes to accounts. 
The rollback is triggered in scenarios such as:

- Receiving a GLOBAL-ABORT command.
- Encountering an error during initialization or transaction processing.
- Receiving Ctrl-C signals.

In the case of a GLOBAL-ABORT, the rollback method:
1. Clears the holding amounts, which are temporary changes that have not been finalized.
2. Reloads the account information from the accounts file.
3. Logs a message indicating the completion of the rollback.

In the case of errors, the handleServerError method:
1. Logs the encountered error message.
2. Rolls back any changes to accounts.
3. Stops the server.

The actions taken when receiving a Ctrl-C signal are similar to those taken 
in the case of errors.

Command-Line Validation and Exception Handling with Try-Catch for both 
Participant and Coordinator classes are implemented to reduce likelihood 
of failures during execution.

(TCPServer class is changed.)
### Compilation:

Compile source code

```sh
make
```

### Run participants.

Command will run script run.sh with 2 participants in parallel with these parameters:

./participant 2233 acc1.txt log1.txt \
./participant 2234 acc2.txt log2.txt

```sh
make run-p
```

Or run manually with params:

```sh
./participant <port> <account_file> <log_file>
```

### Run coordinator.

Command will run script run.sh with coordinator with parameters:

./coordinator log.txt 100 localhost 2233 0982838-88 localhost 2234 0933310-04-27.6

```sh
make run-c
```

Or run manually with params:

```sh
./coordinator <log_file> <amount> <server1_host> <server1_port> <account_from> <server2_host> <server2_port> <account_to>
```

### Clean log files.

Command will run clean-logs.sh script and clean logs from LOG_FILES variable.

```sh
make clean-logs
```

### Clean all.

Command will clean build files and log files

```sh
make clean
```

### Note

Ensure that the server instances are running before starting the client. 
To make the `run.sh` script executable you can use:

```sh
chmod +x run.sh

CC = gcc
CFLAGS = -Wall -g -std=c99 -Wextra

JAVAC = javac
JFLAGS = -g

JAVA = java

BUILD_DIR = ./build

TRACKER_SRC_DIR = ./src/tracker
TRACKER_BUILD_DIR = $(BUILD_DIR)/tracker
TRACKER_INSTALL_DIR = ./install/tracker

TRACKER_SOURCES = $(wildcard $(TRACKER_SRC_DIR)/*.c)
TRACKER_OBJECTS = $(TRACKER_SOURCES:$(TRACKER_SRC_DIR)/%.c=$(TRACKER_BUILD_DIR)/%.o)
TRACKER_TARGET = $(TRACKER_BUILD_DIR)/tracker

PEERS_BUILD_DIR = $(BUILD_DIR)/peers
PEERS_INSTALL_DIR = ./install/peers

PEERS_TARGETS = $(PEERS_BUILD_DIR)/Peers.class

all: build_tracker build_peers

$(shell mkdir -p $(PEERS_BUILD_DIR) $(TRACKER_BUILD_DIR))


build_tracker: $(TRACKER_TARGET)

$(TRACKER_BUILD_DIR)/%: $(TRACKER_OBJECTS)
	$(CC) -o $@ $(CFLAGS) $^
	cp $@ $(TRACKER_INSTALL_DIR)

$(TRACKER_BUILD_DIR)/%.o: $(TRACKER_SRC_DIR)/%.c
	$(CC) -o $@ $(CFLAGS) -c $< -I $(TRACKER_SRC_DIR)


build_peers:
	$(JAVAC) -cp $(PEERS_BUILD_DIR) -d $(PEERS_BUILD_DIR) src/peers/peer/*.java
	$(JAVAC) -cp $(PEERS_BUILD_DIR) -d $(PEERS_BUILD_DIR) src/peers/*.java
	cp $(PEERS_TARGETS) $(PEERS_INSTALL_DIR)

run_tracker: 
	$(TRACKER_INSTALL_DIR)/tracker


run_peers: 
	$(JAVA) -cp $(PEERS_BUILD_DIR) Peers

clean:
	rm -rf $(TRACKER_BUILD_DIR)
	rm -rf $(PEERS_BUILD_DIR)
	rm -rf $(BUILD_DIR)

.PHONY: all run_tracker run_peers clean

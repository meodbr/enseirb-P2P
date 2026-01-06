# Network Project – ENSEIRB-MATMECA

Méo Desbois-Renaudin, Antoine l'Honoré, Melek Mkadem, Mathis Reinert, Inès Boukhars

## Personal notes

This project was part of a computer networks course. My team built a Peer-to-Peer (P2P) file-sharing system with a central tracker and multiple peers. I was solely responsible for designing and implementing the **tracker**, which manages peer connections, shared file metadata, and concurrency.  

I was responsible for the tracker part, which i coded in **C**, implementing a **thread pool** for handling multiple simultaneous connections, and network interface. This allowed the peers to announce their files, search for files, and download them efficiently from other peers.  

---

## Project Overview

This project is a Peer-to-Peer (P2P) file sharing system built around a central tracker.
Peers periodically announce their shared files to the tracker and can search for and
download files from other peers using a multi-threaded P2P transfer system.

The project includes:
- A tracker to manage peers and shared files
- Multiple peers that share and download files
- A graphical interface for file search and download
- Multithreaded networking for scalability and performance

## Project Structure

```bash
install/
├── tracker/
│   └── config.ini
└── peers/
    ├── peer1_files/
    ├── peer2_files/
    ├── ...
    └── config.ini
```

## Build Instructions

To compile the project, run the following command from the project root:

```bash
make
```

This will compile all required components (tracker and peers).

## Running the Project

### 1. Monitor Network Connections (Optional)

You can monitor active connections on the used ports with:

```bash
watch -n 1 "netstat -antpu | grep 500"
```

This is useful for debugging and observing peer-to-peer connections.

### 2. Start the Tracker

In a dedicated terminal:

```bash
make run_tracker
```

This launches the tracker, which manages peers and shared file metadata.

### 3. Start the Peers

In another terminal:

```bash
make run_peers
```

This starts multiple peers simultaneously to simulate a P2P network.

## Configuration

### Tracker Configuration

Located at:

[install/tracker/config.ini](install/tracker/config.ini)

Default values:

```bash
tracker_ip = 127.0.0.100  
tracker_port = 5000  
tracker_interval = 10000  
```

tracker_interval defines the time (in milliseconds) between maintenance operations
such as cache cleanup.

### Peer Configuration

Located at:

[install/peers/config.ini](install/peers/config.ini)

Default configuration for peer X:

```bash
peerX_ip = 127.0.0.X  
peerX_port = 5001  
peerX_interval = 100000  
peerX_directory_path = install/peers/peerX_files  
peerX_max_peers = 5  
```

Each peer has its own directory (peerX_files) containing:
- Files it shares
- Files it downloads

## Features

### Tracker

The tracker is responsible for:
- Managing connected peers
- Storing metadata about shared files
- Returning the list of available files
- Returning the list of peers owning a given file

The tracker does not store file data, only metadata.

### Peer

Each peer can:
- Share files from its local directory
- Search for files via the tracker
- Download files from other peers using P2P connections

Peers periodically announce their available files to the tracker.

Downloaded files are stored in the same peerX_files directory.

### Graphical User Interface (GUI)

Each peer includes a GUI that allows:
- Real-time display of local and downloaded files
- File search via the tracker

Search example:

filename="file.txt"

This retrieves all peers sharing file.txt.

To download a file:
1. Copy and paste the result line into the search field
2. Click the Download button
3. The peer retrieves the list of peers owning the file
4. The file is downloaded in parallel chunks from multiple peers
5. File integrity is verified
6. If successful, the file appears in peerX_files

## Multithreading

### Tracker

The tracker is multithreaded and can handle multiple peer connections concurrently.

### Peer

Each peer:
- Manages multiple incoming and outgoing connections
- Downloads files using parallel chunk-based transfers
- Uses synchronization mechanisms (locks) to avoid race conditions

### Peer Launcher

The peer launcher is multithreaded and allows multiple peers to run simultaneously,
simulating a real P2P network.

## Caching

### Tracker Cache

- Stores peer and file metadata
- Implemented as a dynamic structure
- Entries are removed when their expiration time is reached
- Cache is updated on every peer announcement

### Peer Cache

- Stores search results retrieved from the tracker
- Used to populate the GUI
- Cache is cleared on each new search

## Logging and Error Handling

- All logs are displayed directly in the terminal
- No log files are generated
- All errors are handled and clearly reported in terminal output

## Network Configuration (IP / Port)

### Tracker

- Uses a configurable IP address and port
- Can be changed in config.ini

### Peer

- Each peer has a configurable IP address and port
- Settings can be changed in config.ini

### Current Implementation Details

- Peer server sockets bind only to the configured port
- Peer client sockets bind to both IP address and port
- Tracker socket binds to both IP address and port

## Compatibility

The project can be deployed on:
- A single machine (localhost)
- Multiple machines within the same subnet

Peers and tracker can run on different hosts by updating configuration files.

## Notes

This project was developed as an academic network programming exercise focusing on:
- Socket programming
- Multithreading
- Peer-to-peer architecture
- Synchronization and concurrency control

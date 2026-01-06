package peer;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import javax.swing.SwingUtilities;
import java.util.concurrent.ConcurrentHashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Peer implements Runnable {

    private PeerConfig config;
    private PeerFiles files;
    private ServerSocket server;
    private ExecutorService pool;
    private ScheduledExecutorService scheduler;
    private PeerCache cache;
    private PeerTcpConnection tracker;
    private MainFrame mainFrame;

    public Peer(int peerNumber) {
        this.config = new PeerConfig(peerNumber);
        this.files = new PeerFiles(config.getDirectory());
        try {
            this.server = new ServerSocket(this.config.getPort());
        } catch (IOException e) {
            System.err.println("Error: Could not create server.");
            System.exit(1);
        }
        this.scheduler = Executors.newScheduledThreadPool(1);
        this.pool = Executors.newFixedThreadPool(config.getMaxPeers());
        this.cache = new PeerCache();
        try {
            this.tracker = new PeerTcpConnection(config.getIpAdressTracker(), config.getPortTracker());
        } catch (Exception e) {
            System.err.println("Error: Could not connect to tracker.");
            System.exit(1);
        }
        try {
            SwingUtilities.invokeLater(() -> {
                mainFrame = new MainFrame(this, peerNumber);
                mainFrame.setVisible(true);
            });
        } catch (Exception e) {
            System.err.println("Error: Could not create main frame.");
            stop();
            System.exit(1);
        }
    }

    public void stop() {
        try {
            if (server != null && !server.isClosed()) {
                server.close();
            }
        } catch (IOException e) {
            System.err.println("Error: Could not close server socket.");
        }
        pool.shutdownNow();
        try {
            if (!pool.awaitTermination(5, TimeUnit.SECONDS)) {
                System.err.println("Pool did not terminate; forcing shutdown.");
            }
        } catch (InterruptedException ie) {
            Thread.currentThread().interrupt();
        }
        scheduler.shutdownNow();
        try {
            if (!scheduler.awaitTermination(5, TimeUnit.SECONDS)) {
                System.err.println("Scheduler did not terminate.");
            }
        } catch (InterruptedException ie) {
            Thread.currentThread().interrupt();
        }
        try {
            tracker.closeConnection();
        } catch (Exception e) {
            System.err.println("Error: Could not close tracker connection.");
            e.printStackTrace();
        }
        if (mainFrame != null) {
            mainFrame.dispose();
        }
    }

    private void startRoutine() {
        scheduler.scheduleAtFixedRate(this::announceToTracker, 0, config.getInterval(), TimeUnit.MILLISECONDS);
        while (!server.isClosed()) {
            try {
                Socket clientSocket = server.accept();
                pool.submit(() -> handleConnection(clientSocket));
            } catch (IOException e) {
                System.err.println("Server socket closed.");
            }
        }
    }

    public void announceToTracker() {
        try {
            tracker.send(
                    "announce listen " + config.getPort() + " seed [" + files.getSeed() + "] leech [" + files.getLeech()
                            + "]");
            String response = tracker.read();
            if (!response.equals("ok")) {
                System.err.println("Error: Unexpected response from tracker: " + response + ".");
            }
        } catch (Exception e) {
            System.err.println("Error communicating with tracker: " + e.getMessage());
        }
    }

    public void lookForFiles(String searchCriteria) {
        cache.clear();
        try {
            tracker.send("look [" + searchCriteria + "]");
            String response = tracker.read();

            if (response.startsWith("list [") && response.endsWith("]")) {
                String content = response.substring(6, response.length() - 1);
                if (content.isEmpty()) {
                    System.err.println("Error: No files found.");
                    return;
                }
                String[] fileEntries = content.split(" ");
                List<FileMetadata> files = new ArrayList<>();
                for (int i = 0; i < fileEntries.length; i += 4) {
                    String filename = fileEntries[i];
                    long length = Long.parseLong(fileEntries[i + 1]);
                    int pieceSize = Integer.parseInt(fileEntries[i + 2]);
                    String key = fileEntries[i + 3];
                    files.add(new FileMetadata(filename, length, pieceSize, key));
                }
                cache.cacheFileSearchResults(searchCriteria, files);
            } else {
                System.err.println("Unexpected response format: " + response);
            }
        } catch (Exception e) {
            System.err.println("Error communicating with tracker: " + e.getMessage());
        }
    }

    public String[] getFile(String fileKey) {
        try {
            tracker.send("getfile " + fileKey);
            String response = tracker.read();
            if (response == null || response.isEmpty()) {
                System.err.println("Error: Empty response from tracker.");
                return null;
            }
            String[] responseArray = response.split(" ", 3);
            if (responseArray.length != 3 || !responseArray[0].equals("peers")) {
                System.err.println("Error: Invalid tracker response format.");
                return null;
            }
            String peersList = responseArray[2];
            if (peersList.startsWith("[") && peersList.endsWith("]")) {
                peersList = peersList.substring(1, peersList.length() - 1);

                String[] listOfPeers = peersList.split(" ");
                if (listOfPeers.length == 0) {
                    System.err.println("Error: No peers found.");
                    return null;
                }
                System.out.println("Received list of peers: " + Arrays.toString(listOfPeers));

                return listOfPeers;
            } else {
                System.err.println("Error: Peers list not enclosed in brackets.");
                return null;
            }
        } catch (Exception e) {
            System.err.println("Error communicating with tracker: " + e.getMessage());
            return null;
        }
    }

    public String interested(String fileKey, String ip, int port) {
        PeerTcpConnection client = new PeerTcpConnection(ip, port);
        client.send("interested " + fileKey);
        String response = client.read();

        if (response.startsWith("have " + fileKey)) {
            String bufferMap = response.substring(("have " + fileKey + " ").length());
            System.out.println("Received buffer map from " + ip + ":" + port + ": "
                    + bufferMap);
            return bufferMap;
        } else {
            System.err.println("Invalid response from " + ip + ":" + port);
        }
        return null;
    }

    public enum PieceStatus {
        NEEDED, REQUESTED, DOWNLOADED
    }
    

    public void downloadFile(String fileDescString) {
        System.out.println("Downloading " + fileDescString);
        String[] parts = fileDescString.split(", ");
        String filename = parts[0].split(": ")[1];
        int length = Integer.parseInt(parts[1].split(": ")[1]);
        int pieceSize = Integer.parseInt(parts[2].split(": ")[1]);
        String key = parts[3].split(": ")[1];
        int totalPieces = (int) Math.ceil((double) length / pieceSize);
        System.out.println(
                "Filename: " + filename + ", Length: " + length + ", Piece size: " + pieceSize + ", Key: " + key);
        files.addFile(filename, length, pieceSize, key);
        String[] listOfPeers = getFile(key);
        if (listOfPeers == null) {
            System.err.println("Error: Could not get file from tracker.");
            return;
        }
    
        ConcurrentHashMap<Integer, PieceStatus> pieceStatuses = new ConcurrentHashMap<>();
        for (int i = 0; i < totalPieces; i++) {
            pieceStatuses.put(i, PieceStatus.NEEDED); // Initialize all pieces as needed
        }
    
        ExecutorService downloadExecutor = Executors.newFixedThreadPool(config.getMaxPeers());
        List<Future<?>> futures = new ArrayList<>();
    
        // Launching threads for downloading
        for (int i = 0; i < totalPieces; i++) {
            futures.add(downloadExecutor.submit(() -> {
                while (!pieceStatuses.isEmpty()) {
                    String pieceIndices = getPieceIndices(pieceStatuses, listOfPeers, totalPieces);
                    if (pieceIndices.isEmpty()) {
                        break; // No more pieces are needed
                    }
    
                    boolean pieceDownloaded = false;
                    List<String> shuffledPeers = new ArrayList<>(Arrays.asList(listOfPeers));
                    Collections.shuffle(shuffledPeers);
                    for (String peer : shuffledPeers) {
                        String[] ipPort = peer.split(":");
                        String bufferMap = interested(key, ipPort[0], Integer.parseInt(ipPort[1]));
                        if (bufferMap != null) {
                            pieceDownloaded = getPieces(key, pieceIndices, ipPort[0], Integer.parseInt(ipPort[1]));
                            if (pieceDownloaded) {
                                updatePieceStatuses(pieceStatuses, pieceIndices, PieceStatus.DOWNLOADED);
                                break;
                            }
                        }
                    }
    
                    if (!pieceDownloaded) {
                        updatePieceStatuses(pieceStatuses, pieceIndices, PieceStatus.NEEDED); // Reset status if failed
                        System.err.println("Failed to download pieces: " + pieceIndices + " for file " + filename);
                    }
                }
                return null;
            }));
        }
    
        downloadExecutor.shutdown();
        try {
            downloadExecutor.awaitTermination(1, TimeUnit.HOURS);
        } catch (InterruptedException e) {
            System.err.println("Download interrupted: " + e.getMessage());
        }
    
        if (files.fileIsCorrupted(key)) {
            System.err.println("Error: File is corrupted.");
            files.removeFile(key);
        } else {
            System.out.println("File " + filename + " downloaded successfully.");
        }
    }
    
    private String getPieceIndices(ConcurrentHashMap<Integer, PieceStatus> pieceStatuses, String[] listOfPeers, int totalPieces) {
        StringBuilder indices = new StringBuilder();
        int count = 0;
        for (Map.Entry<Integer, PieceStatus> entry : pieceStatuses.entrySet()) {
            if (entry.getValue() == PieceStatus.NEEDED && count < 3) {
                indices.append(entry.getKey()).append(",");
                pieceStatuses.replace(entry.getKey(), PieceStatus.REQUESTED);
                count++;
            }
            if (count == 3) break;
        }
        if (indices.length() > 0) indices.setLength(indices.length() - 1); // Remove the last comma
        return indices.toString();
    }
    
    private void updatePieceStatuses(ConcurrentHashMap<Integer, PieceStatus> pieceStatuses, String pieceIndices, PieceStatus status) {
        for (String index : pieceIndices.split(",")) {
            pieceStatuses.replace(Integer.parseInt(index), status);
        }
    }
    
    

    public boolean getPieces(String key, String pieces, String ipAddress, int port) {
        PeerTcpConnection client = new PeerTcpConnection(ipAddress, port);

        // Send the request to get pieces
        client.send("getpieces " + key + " [" + pieces + "]");
        String response = client.read();

        // Assuming response format: "data <key> [76:data 77:data 78:data]"
    // First, trim off everything before the square bracket starts
    int dataStartIndex = response.indexOf('[') + 1;
    int dataEndIndex = response.lastIndexOf(']');
    if (dataStartIndex == -1 || dataEndIndex == -1 || dataStartIndex >= dataEndIndex) {
        System.err.println("Error: Invalid response format.");
        return false;
    }
    String rawData = response.substring(dataStartIndex, dataEndIndex);

    // Regex to find patterns of "index:data"
    Pattern p = Pattern.compile("(\\d+):([^:]+?(?=(\\d+:)|$))");
    Matcher m = p.matcher(rawData);

    // Process each matched piece
    while (m.find()) {
        try {
            int index = Integer.parseInt(m.group(1)); // The piece index
            String pieceData = m.group(2).trim(); // The associated data, trimmed
            System.out.println("Downloading piece " + index + ": " + pieceData);
            // Download the file piece
            files.downloadFile(key, index, pieceData);
        } catch (NumberFormatException e) {
            System.err.println("Error parsing piece index: " + m.group(1));
            return false;
        }
    }
        client.closeConnection();
        return true;
    }

    public void handleConnection(Socket clientSocket) {
        PeerTcpConnection client = new PeerTcpConnection(clientSocket);
        String request = client.read();
        String[] requestArray = request.split(" ");
        String response = null;
        if (requestArray.length == 0) {
            System.err.println("Error: Invalid request.");
            return;
        }
        switch (requestArray[0]) {
            case "interested":
                response = "have " + files.getHave(requestArray[1]);
                break;
            case "getpieces":
                String key = requestArray[1];
                String piecesIndexStr = requestArray[2].replaceAll("\\[|\\]|\\{|\\}", "").trim();
                String[] piecesIndices = piecesIndexStr.split(",\\s*");

                StringBuilder sb = new StringBuilder();
                try {
                    for (String indexStr : piecesIndices) {
                        int index = Integer.parseInt(indexStr);
                        sb.append(files.getPiece(key, index));
                        sb.append(" ");
                    }
                } catch (NumberFormatException e) {
                    System.err.println("Invalid index format: " + e.getMessage());
                }

                response = "data " + key + " [" + sb.toString().trim() + "]";
                break;

            default:
                System.err.println("Error: Invalid request.");
                break;
        }
        client.send(response);
        client.closeConnection();
    }

    public String getConfiguration() {
        return config.toString();
    }

    public String getFiles() {
        return files.toString();
    }

    public String getResult() {
        return cache.toString();
    }

    @Override
    public void run() {
        startRoutine();
    }

}
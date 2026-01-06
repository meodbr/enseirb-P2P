package peer;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

class PeerCache {
    private Map<String, List<FileMetadata>> fileSearchCache; // Cache pour les recherches de fichiers

    public PeerCache() {
        fileSearchCache = new ConcurrentHashMap<>();
    }

    public void cacheFileSearchResults(String criteria, List<FileMetadata> results) {
        fileSearchCache.put(criteria, results);
    }

    public List<FileMetadata> getFileSearchResults(String criteria) {
        return fileSearchCache.getOrDefault(criteria, Collections.emptyList());
    }

    public String getFileSearchResultsKey(String key) {
        String result = "";
        for (Map.Entry<String, List<FileMetadata>> entry : fileSearchCache.entrySet()) {
            if (entry.getKey().equals(key)) {
                for (FileMetadata file : entry.getValue()) {
                    result += file.toString() + "\n";
                }
            }
        }
        return result;
    }

    public void clear() {
        fileSearchCache.clear();
    }

    public String toString() {
        String result = "";
        for (Map.Entry<String, List<FileMetadata>> entry : fileSearchCache.entrySet()) {
            result += entry.getKey() + ":\n";
            for (FileMetadata file : entry.getValue()) {
                result += file.toString() + "\n";
            }
        }
        return result;
    }
}

class FileMetadata {
    String filename;
    long length;
    int pieceSize;
    String key;

    public FileMetadata(String filename, long length, int pieceSize, String key) {
        this.filename = filename;
        this.length = length;
        this.pieceSize = pieceSize;
        this.key = key;
    }

    public String toString() {
        return "Filename: " + filename + ", Length: " + length + ", Piece Size: " + pieceSize + ", Key: " + key;
    }
}

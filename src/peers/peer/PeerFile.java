package peer;

import java.util.BitSet;

public class PeerFile {

    private String fileName;
    private long fileSize;
    private int pieceSize;
    private String key;
    private BitSet bufferMap;

    public PeerFile(String fileName, long fileSize, int pieceSize, String key) {
        this.fileName = fileName;
        this.fileSize = fileSize;
        this.pieceSize = pieceSize;
        this.key = key;
        this.bufferMap = new BitSet((int) Math.ceil((double) fileSize / pieceSize)); 
    }

    public String getFileName() {   
        return fileName;
    }

    public long getFileSize() {
        return fileSize;
    }

    public int getPieceSize() {
        return pieceSize;
    }

    public String getKey() {
        return key;
    }

    public BitSet getBufferMap() {
        return bufferMap;
    }

    public void setBuffermap(int index) {
        bufferMap.set(index);
    }
}

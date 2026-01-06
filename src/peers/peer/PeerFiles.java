package peer;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.charset.StandardCharsets;
import java.security.DigestInputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.BitSet;

public class PeerFiles {

    private String directory;
    private ArrayList<PeerFile> files;

    public PeerFiles(String directoryPath) {
        this.directory = directoryPath;
        this.files = new ArrayList<>();

        File directory = new File(directoryPath);
        String[] files = directory.list();

        if (files != null) {
            for (String file : files) {
                File currentFile = new File(directoryPath + "/" + file);
                if (currentFile.isFile()) {
                    String key = MD5Calculator(currentFile);
                    addFile(file, currentFile.length(), 1024, key);
                    PeerFile f = getFile(key);
                    for (int i = 0; i < (int) Math.ceil((double) currentFile.length() / 1024); i++) {
                        f.setBuffermap(i);
                    }
                }
            }
        }
    }

    private PeerFile getFile(String key) {
        for (PeerFile file : files) {
            if (file != null && file.getKey().equals(key)) {
                return file;
            }
        }
        return null;
    }

    public BitSet getBitMapIndex(String key, int index) {
        PeerFile file = getFile(key);
        if (file != null) {
            return file.getBufferMap();
        } else {
            return null;
        }
    }

    // Check if the file is corrupted
    // Return true if the file is corrupted
    // Return false if the file is not corrupted
    public Boolean fileIsCorrupted(String key) {
        PeerFile file = getFile(key);
        if (file != null) {       
            return !MD5Calculator(new File(directory + "/" + getFile(key).getFileName())).equals(file.getKey());
        } else {
            return null;
        }
    }

    private String MD5Calculator(File file) {
        String key = null;
        try {
            MessageDigest md = MessageDigest.getInstance("MD5");

            // Utilisation de try-with-resources pour assurer la fermeture du flux
            try (DigestInputStream dis = new DigestInputStream(new FileInputStream(file), md);
                    BufferedInputStream bis = new BufferedInputStream(dis)) {
                byte[] buffer = new byte[8192]; // Utilisation d'un buffer pour la lecture
                while (bis.read(buffer) != -1) {
                    // La lecture dans le buffer est suffisante pour mettre à jour le digest
                }
            }

            // Récupération du digest (le hash MD5)
            byte[] digest = md.digest();

            // Conversion du tableau de bytes en chaîne hexadécimale
            StringBuilder result = new StringBuilder();
            for (byte b : digest) {
                result.append(String.format("%02x", b));
            }
            key = result.toString();
        } catch (NoSuchAlgorithmException | IOException e) {
            System.err.println("Error: Could not calculate MD5 hash.");
        }
        return key;
    }

    public void addFile(String fileName, long fileSize, int pieceSize, String key) {
        // Créer le PeerFile et l'ajouter à la liste
        PeerFile newPeerFile = new PeerFile(fileName, fileSize, pieceSize, key);
        files.add(newPeerFile); // Ajouter le nouveau PeerFile à la liste

        // Créer physiquement le fichier sur le disque
        File newFile = new File(directory + "/" + fileName);
        try (RandomAccessFile raf = new RandomAccessFile(newFile, "rw")) {
            raf.setLength(fileSize); // Allouer l'espace pour le fichier sans remplir la mémoire
        } catch (IOException e) {
            System.err.println("Erreur lors de la création du fichier : " + e.getMessage());
        }
    }

    public void removeFile(String key) {
        PeerFile file = getFile(key);
        if (file != null) {
            files.remove(file);
            File f = new File(directory + "/" + file.getFileName());
            if (f.delete()) {
                System.out.println("File " + file.getFileName() + " deleted.");
            } else {
                System.err.println("Error: Could not delete file " + file.getFileName());
            }
        } else {
            System.err.println("Error: File not found with key " + key);
        }
    }

    public void downloadFile(String key, int index, String piece) {
        boolean fileFound = false;
        for (PeerFile file : files) {
            if (file != null && key.equals(file.getKey())) {
                fileFound = true;
                try {
                    long offset = (long) index * (long) file.getPieceSize();
                    try (RandomAccessFile raf = new RandomAccessFile(directory + "/" + file.getFileName(), "rw")) {
                        raf.seek(offset);
                        raf.write(piece.getBytes(StandardCharsets.UTF_8));
                    }
                    file.setBuffermap(index);
                    System.out.println("Piece " + index + " of file " + file.getFileName() + " downloaded.");
                    
                } catch (IOException e) {
                    System.err.println("Error writing to file: " + e.getMessage());
                }
                break; // Only process the first matching file
            }
        }
    
        if (!fileFound) {
            System.err.println("No matching file found for key: " + key);
        }
    }
    

    public String getSeed() {
        StringBuilder sb = new StringBuilder();
        String separator = "";
        for (PeerFile file : files) {
            if (file == null) {
                break;
            }
            sb.append(separator).append(file.getFileName()).append(" ").append(file.getFileSize()).append(" ")
                    .append(file.getPieceSize())
                    .append(" ").append(file.getKey());
            separator = " ";
        }
        return sb.toString();
    }

    public String getLeech() {
        StringBuilder sb = new StringBuilder();
        for (PeerFile file : files) {
            if (file == null) {
                break;
            }
            if (file.getBufferMap().cardinality() != (int) Math.ceil((double) file.getFileSize() / file.getPieceSize())){
                sb.append(file.getKey()).append(" ");
            }
        }
        return sb.toString();
    }

    public String getPiece(String key, int index) {
        for (PeerFile file : files) {
            if (file != null && file.getKey().equals(key)) {
                long offset = (long) index * (long) file.getPieceSize();
                byte[] data = new byte[file.getPieceSize()];

                try (RandomAccessFile raf = new RandomAccessFile(directory + "/" + file.getFileName(), "r")) {
                    raf.seek(offset);
                    int bytesRead = raf.read(data);
                    if (bytesRead == -1) {
                        return null; 
                    }
                    return index + ":" + new String(data, 0, bytesRead, "UTF-8");
                } catch (IOException e) {
                    System.err.println("Error reading file: " + e.getMessage());
                    return null;
                }
            }
        }
        return null; 
    }

    public String getHave(String key) {
        for (PeerFile file : files) {
            if (file == null) {
                break;
            }
            if (file.getKey().equals(key)) {
                return file.getKey() + " " + file.getBufferMap();
            }
        }
        return null;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        for (PeerFile file : files) {
            if (file == null) {
                break;
            }
            sb.append(file.getFileName()).append(" ").append(file.getFileSize()).append(" ").append(file.getPieceSize())
                    .append(" ").append(file.getKey()).append(" ").append("\n");
        }
        return sb.toString();
    }
}
package peer;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

public class PeerConfig {

    private String ipAddress;
    private int port;
    private String ipAdressTracker;
    private int portTracker;
    private int interval;
    private String directory;
    private int maxPeers;

    public PeerConfig(int peerNumber) {
        String fileName = "install/peers/config.ini";

        try (Scanner scanner = new Scanner(new File(fileName))) {
            while (scanner.hasNextLine()) {
                String line = scanner.nextLine().trim();

                if (line.startsWith("peer" + peerNumber + "_ip")) {
                    ipAddress = line.split("=")[1].trim();
                } else if (line.startsWith("peer" + peerNumber + "_port")) {
                    port = Integer.parseInt(line.split("=")[1].trim());
                } else if (line.startsWith("tracker_ip")) {
                    ipAdressTracker = line.split("=")[1].trim();
                } else if (line.startsWith("tracker_port")) {
                    portTracker = Integer.parseInt(line.split("=")[1].trim());
                } else if (line.startsWith("peer" + peerNumber + "_interval")) {
                    interval = Integer.parseInt(line.split("=")[1].trim());
                } else if (line.startsWith("peer" + peerNumber + "_directory_path")) {
                    directory = line.split("=")[1].trim();
                } else if (line.startsWith("peer" + peerNumber + "_max_peers")) {
                    maxPeers = Integer.parseInt(line.split("=")[1].trim());
                } else if (line.startsWith("peer" + (peerNumber) + "_max_peers")) {
                    maxPeers = Integer.parseInt(line.split("=")[1].trim());
                }
                if (ipAddress != null && port != 0 && ipAdressTracker != null && portTracker != 0 && interval != 0
                        && directory != null && maxPeers != 0) {
                    break;
                }
            }
        } catch (FileNotFoundException e) {
            System.err.println("Error: Configuration file not found.");
            System.exit(1);
        } catch (Exception e) {
            System.err.println("Error: Getting configuration values.");
            System.exit(1);
        }
    }

    public String getIpAddress() {
        return ipAddress;
    }

    public int getPort() {
        return port;
    }

    public String getIpAdressTracker() {
        return ipAdressTracker;
    }

    public int getPortTracker() {
        return portTracker;
    }

    public int getInterval() {
        return interval;
    }

    public String getDirectory() {
        return directory;
    }

    public int getMaxPeers() {
        return maxPeers;
    }

    @Override
    public String toString() {
        return "ipAddress=" + ipAddress + "\nport=" + port + "\nipAdressTracker=" + ipAdressTracker
                + "\nportTracker=" + portTracker + "\ninterval=" + interval + "\ndirectory=" + directory + "\nmaxPeers="
                + maxPeers;
    }
}

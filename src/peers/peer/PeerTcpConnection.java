package peer;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class PeerTcpConnection {

    private Socket socket;
    private BufferedReader in;
    private PrintWriter out;

    public PeerTcpConnection(String ipAdressDest, int portDest) {
        try {
            socket = new Socket(ipAdressDest, portDest);
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            out = new PrintWriter(socket.getOutputStream(), true);
            System.out.println("Connected to " + ipAdressDest + ":" + portDest + ".");
        } catch (IOException e) {
            System.err.println("Error: Could not connect to " + ipAdressDest + ":" + portDest + " : " + e.getMessage()
                    + " => Connection is closed.");
            closeConnection();
        }
    }
    
    public PeerTcpConnection(Socket ClientSocket) {
        try {
            socket = ClientSocket;
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            out = new PrintWriter(socket.getOutputStream(), true);
            System.out.println("Connected to " + socket.getInetAddress() + ":" + socket.getPort() + ".");
        } catch (IOException e) {
            System.err.println("Error: Could not connect to " + socket.getInetAddress() + ":" + socket.getPort() + " : "
                    + e.getMessage() + " => Connection is closed.");
            closeConnection();
        }
    }

    public void closeConnection() {
        try {
            if (out != null) {
                out.close();
            }
            if (in != null) {
                in.close();
            }
            if (socket != null && !socket.isClosed()) {
                System.out.println("Connection with " + socket.getInetAddress() + ":" + socket.getPort() + " closed.");
                socket.close();
            }
        } catch (IOException e) {
            System.err.println("Error: Could not close connection.");
        }
    }

    public void send(String message) {
        if (socket != null && !socket.isClosed() && out != null) {
            System.out.println("Sending message to" + socket.getInetAddress() + ":" + socket.getPort() + ": " + message);
            out.println(message);
        } else {
            System.err.println("Error: Could not send request. => Connection is closed.");
            closeConnection();
        }
    }

    public String read() {
        if (in != null) {
            try {
                String response = in.readLine().trim();
                if (response != null) {
                    System.out.println("Received message from " + socket.getInetAddress() + ":" + socket.getPort() + ": "
                            + response);
                    return response;
                } else {
                    System.err.println("Error: Could not read response. => Connection is closed.");
                    closeConnection();
                    return "";
                }
            } catch (IOException e) {
                System.err.println("Error: Could not read response. => Connection is closed.");
                closeConnection();
                return "";
            }
        } else {
            System.err.println("Error: Could not read response. Connection is closed.");
            closeConnection();
            return "";
        }
    }
}
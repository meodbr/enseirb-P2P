import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import peer.Peer;

public class Peers {
    public static void main(String[] args) {
        ExecutorService pool = Executors.newFixedThreadPool(5);

        Peer peer1 = new Peer(1);
        Peer peer2 = new Peer(2);
        Peer peer3 = new Peer(3);
        Peer peer4 = new Peer(4);
        Peer peer5 = new Peer(5);

        pool.submit(() -> {
            try {
                peer1.run();
            } catch (Exception e) {
                System.out.println("Error in peer1: " + e.getMessage());
            }
        });
        pool.submit(() -> {
            try {
                peer2.run();
            } catch (Exception e) {
                System.out.println("Error in peer2: " + e.getMessage());
            }
        });
        pool.submit(() -> {
            try {
                peer3.run();
            } catch (Exception e) {
                System.out.println("Error in peer3: " + e.getMessage());
            }
        });
        pool.submit(() -> {
            try {
                peer4.run();
            } catch (Exception e) {
                System.out.println("Error in peer4: " + e.getMessage());
            }
        });
        pool.submit(() -> {
            try {
                peer5.run();
            } catch (Exception e) {
                System.out.println("Error in peer5: " + e.getMessage());
            }
        });
        pool.shutdown();

        try {
            // Wait a sufficient time for all tasks to finish
            if (!pool.awaitTermination(1, TimeUnit.SECONDS)) {
                System.out.println("Some tasks were not completed.");
            }
        } catch (InterruptedException ie) {
            // (Re-)Cancel if current thread also interrupted
            pool.shutdownNow();
            // Preserve interrupt status
            Thread.currentThread().interrupt();
            System.out.println("Thread was interrupted, shutdown now.");
        }

        System.out.println("All peers have completed or the time limit was reached.");
    }

}

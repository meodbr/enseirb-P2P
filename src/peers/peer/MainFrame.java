package peer;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;

public class MainFrame extends JFrame {

    private JTextArea filesTextArea;
    private JButton searchButton;
    private JTextField searchField;
    private JButton quitButton;
    private JTextArea resultsTextArea;
    private JTextField downloadField;
    private JButton downloadButton;

    public MainFrame(Peer peer, int peerNumber) {
        setTitle("Application peer-to-peer");
        setSize(800, 550);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLayout(new BorderLayout(10, 10));

        initializeComponents(peer, peerNumber);
        setupActions(peer);
        setVisible(true);
    }

    private void initializeComponents(Peer peer, int peerNumber) {
        // Configuration de westPanel
        JPanel westPanel = new JPanel(new GridBagLayout());
        westPanel.setBorder(BorderFactory.createTitledBorder("Peer " + peerNumber));

        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.anchor = GridBagConstraints.WEST;
        gbc.weightx = 1;
        gbc.weighty = 0;
        gbc.insets = new Insets(10, 10, 10, 10);

        westPanel.add(new JLabel("Configuration"), gbc);
        gbc.gridy++;

        JTextArea configTextArea = new JTextArea(7, 20);
        configTextArea.setEditable(false);
        configTextArea.setText(peer.getConfiguration());
        westPanel.add(new JScrollPane(configTextArea), gbc);
        gbc.gridy++;

        westPanel.add(new JLabel("Fichiers"), gbc);
        gbc.gridy++;

        filesTextArea = new JTextArea(10, 20);
        filesTextArea.setEditable(false);
        filesTextArea.setText(peer.getFiles());
        westPanel.add(new JScrollPane(filesTextArea), gbc);
        gbc.gridy++;

        // Configuration de eastPanel
        JPanel eastPanel = new JPanel(new GridBagLayout());
        eastPanel.setBorder(BorderFactory.createTitledBorder("Recherche et téléchargement"));

        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.anchor = GridBagConstraints.WEST;
        gbc.weightx = 1;
        gbc.weighty = 0;

        searchButton = new JButton("Rechercher");
        eastPanel.add(searchButton, gbc);
        gbc.gridy++;

        searchField = new JTextField(20);
        eastPanel.add(searchField, gbc);
        gbc.gridy++;

        eastPanel.add(new JLabel("Résultats"), gbc);
        eastPanel.add(new JLabel("Résultats"), gbc);
        gbc.gridy++;

        resultsTextArea = new JTextArea(10, 20);
        resultsTextArea.setEditable(false);
        resultsTextArea.setText(peer.getResult());
        eastPanel.add(new JScrollPane(resultsTextArea), gbc);
        gbc.gridy++;

        downloadButton = new JButton("Télécharger");
        eastPanel.add(downloadButton, gbc);
        gbc.gridy++;

        downloadField = new JTextField(20);
        eastPanel.add(downloadField, gbc);
        gbc.gridy++;

        JPanel bottomPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        quitButton = new JButton("Quitter");
        bottomPanel.add(quitButton);
        getContentPane().add(bottomPanel, BorderLayout.SOUTH);

        // JSplitPane pour diviser l'espace
        JSplitPane splitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, westPanel, eastPanel);
        splitPane.setDividerLocation(320);
        getContentPane().add(splitPane, BorderLayout.CENTER);
    }

    private void setupActions(Peer peer) {

        quitButton.addActionListener((ActionEvent e) -> {
            peer.stop();
        });

        searchButton.addActionListener((ActionEvent e) -> {
            String criteria = searchField.getText();
            if (criteria.isEmpty()) {
                return;
            }
            peer.lookForFiles(criteria);
            resultsTextArea.setText(peer.getResult());
        });

        downloadButton.addActionListener((ActionEvent e) -> {
            String filename = downloadField.getText();
            if (filename.isEmpty()) {
                return;
            }
            peer.downloadFile(filename);
        });

        Timer timer = new Timer(1000, e -> {
            filesTextArea.setText(peer.getFiles());
        });
        timer.start();
        if (!timer.isRunning()) {
        }
    }
}
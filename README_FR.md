# Projet de réseau - team MIAMM

## Lancement du projet

### Compilation
Dans un terminal lancer la commande suivante :
```bash
make
```
Cela permet de compiler le projet.

### Suivi des connexions
Dans un terminal lancer la commande suivante :

```bash
watch -n 1 "netstat -antpu | grep 500"
```
Cela permet de voir les connexions sur les port utilisés.

### Lancement du tracker
Dans un autre terminal lancer la commande suivante :

```bash
make run_tracker
```
Cela permet de lancer le tracker.

### Lancement des peers
Dans un autre terminale lancer la commande suivante :

    ```bash
    make run_peers
    ```
Cela permet de lancer les peers.

## Configuration
Il y a respectivement dans les répertoires `install/tracker` et `install/peers` les fichiers de configuration `config.ini` pour le tracker et les peers.

La configuration de peerX par défaut est la suivante :
peerX_ip = 127.0.0.X
peerX_port = 5001
peerX_interval = 100000
peerX_directory_path = install/peers/peerX_files 
peerX_max_peers = 5

Le répertoire install/peers/peerX_files contient des fichiers du peerX de test pour les peers.

La configuration du tracker par défaut est la suivante :
tracker_ip = 127.0.0.100
tracker_port = 5000
tracker_interval = 10000


## Fonctionnalités
### Tracker
Le tracker permet de gérer les peers et les fichiers partagés. Il permet de récupérer la liste des fichiers partagés par les peers et de récupérer la liste des peers possédant un fichier donné.

### Peer
Le peer permet de partager des fichiers et de télécharger des fichiers partagés par d'autres peers. Il permet de se connecter au tracker pour récupérer la liste des fichiers partagés par les autres peers et de se connecter aux autres peers pour télécharger les fichiers.

Chaques peer X possède un répertoire `peerX_files` contenant les fichiers qu'il partage.
Les fichiers téléchargés sont stockés dans ce même répertoire.

Les peers annonces au tracker à intervalle régulier les fichiers qu'ils partagent.

### Interface graphique
Chaque peer possède une interface graphique permettant de voir sont en continue les fichiers contenues dans le répertoire `peerX_files` et les fichiers téléchargés.

Il est possible d'effectuer une recherche tel que `filename="fichier.txt"` pour récupérer les fichiers dont le nom est `fichier.txt`.

Copier coller la ligne correspondant au fichier que vous souhaitez télécharger dans le champs de recherche pour télécharger le fichier.

Cliquer sur le bouton `Télécharger` pour Télécharger le fichier.
    Cette action va créer un nouveau fichier dans le répertoire `peerX_files` du peer courant.
    Récupérer les peers possédant le fichier et effectuer un téléchargement multithreader en P2P pour récupérer le fichier.
    A la fin du téléchargement, le fichier est disponible dans le répertoire `peerX_files` du peer courant si la vérification de l'intégrité du fichier est correcte.

### Logs
Toutes les logs sont dans le terminal et non redirigées dans un fichier.

### Gestion des erreurs
Toutes les erreurs sont gérées et affichées dans le terminal.

## Multithreading
### Tracker
Le tracker est multithreadé pour gérer les connexions simultanées des peers.

### Peer
Le peer est multithreadé pour gérer les connexions simultanées des peers et le téléchargement des fichiers.
Le téléchargement des fichiers est fait de manière sécurisé avec des vérous sur les indices du tableau de téléchargement afin d'éviter les conflits.
Le téléchargement des fichiers est multithreadé pour permettre de télécharger plusieurs morceaux de fichiers en même temps et provenant de plusieurs peers.

### Peers
Peers est multithreadé pour permettre de lancer plusieurs peer en même temps et de simuler un réseau P2P.


## Cache
### Tracker
Le tracker possède un cache pour stocker les informations des peers et des fichiers partagés est en réalité un tableau de structure dynamique. Le cache est clean des que la date d'expiration est dépassée. Le cache est mis à jour à chaque annonce de peer.

### Peer
Le peer possède un cache pour stocker le résultat de la recherche par critère addresser au tracker. Cela permet de l'afficher dans l'interface graphique. Le cache est vidé à chaque nouvelle recherche.


## IP/Port
### Tracker
Le tracker possède une adresse IP et un port par défaut. Il est possible de changer l'adresse IP et le port dans le fichier de configuration `config.ini`.

### Peer
Le peer possède une adresse IP et un port par défaut. Il est possible de changer l'adresse IP et le port dans le fichier de configuration `config.ini`.

### Actuelement
Le serveur de chaque peer possède une socket qui est bind seulement avec le port.
Le client de chaque peer bind avec l'adresse IP et le port.
Le tracker possède une socket qui est bind avec l'adresse IP et le port.

### Compatisibilité
Le projet est compatible avec les adresses IP si on veut le déployer dans un sous réseau avec des machines différentes.


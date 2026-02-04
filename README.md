# 🪨 Simulateur de Mineurs avec Sémaphores et zones de mémoire partagée System V

Un programme en C qui simule des mineurs partageant des outils de travail en utilisant les sémaphores System V pour la synchronisation entre processus. et qui gère la quantité de ressource reçu avec les zones de mémoire partagée.

## 📋 Description

Ce projet illustre la synchronisation de processus concurrents à l'aide de sémaphores System V. Des mineurs (processus fils) se partagent un nombre limité d'outils (ressources) pour travailler. Seul un nombre défini de mineurs peut travailler simultanément, les autres doivent attendre qu'un outil se libère. chaque mineure récupère une cantité d'or aléatoire, la somme est faites dans une zone de mémoire partagé puis récupéré par le pere à la fin de l'execution de tous les fils.

## 🎯 Objectifs pédagogiques

- Utilisation des **sémaphores System V** (`semget`, `semctl`, `semop`)
- Synchronisation de **processus** (pas de threads)
- Gestion de **ressources partagées limitées**
- Communication inter-processus (IPC)
- Manipulation de `fork()` et `wait()`
- Manipulation des **zones de mémoire partagée**.
- programme pouvant très bien etre adapté pour de l'utilisation en entreprise.

## 🛠️ Prérequis

- Compilateur GCC
- Système Linux/Unix
- Bibliothèques standard C

## 📦 Compilation

```bash
gcc -o mineurs mineurs.c
```

## 🚀 Utilisation

```bash
./mineurs (nb mineurs),(nb_ressource dispo)  
```

### Paramètres

- `nombre_de_mineurs` : Nombre total de mineurs à créer
- `nombre_d_outils` : Nombre d'outils disponibles (ressources partagées)

### Exemples

```bash
# 5 mineurs avec 2 outils disponibles
./mineurs 5 2

# 10 mineurs avec 3 outils disponibles
./mineurs 10 3

# 3 mineurs avec 1 seul outil (travail séquentiel)
./mineurs 3 1
```

## 📊 Comportement du programme

1. **Création des mineurs** : Le processus parent crée N mineurs (processus fils)
2. **Temps d'attente aléatoire** : Chaque mineur attend 0-4 heures avant de commencer
3. **Prise d'outil** : Le mineur tente de prendre un outil (opération P sur le sémaphore)
   - Si un outil est disponible : il le prend et commence à travailler
   - Si tous les outils sont pris : il attend qu'un outil se libère
4. **Travail** : Le mineur travaille pendant 0-4 heures (sleep aléatoire)
5. **Caisse** : ouverture du coffre par le mineurs avant de rendre les outils pour placé la quantité d'or récupéré
6. **Libération d'outil** : Le mineur rend l'outil (opération V sur le sémaphore)
7. **Fin** : Le processus parent attend que tous les mineurs terminent

## 🔍 Exemple de sortie

```
création du mineur 1 12345
création du mineur 2 12346
création du mineur 3 12347
le mineur 1 attend 2 heure avant de travailler 
le mineur 2 attend 0 heure avant de travailler 
le mineur 3 attend 3 heure avant de travailler 
le mineur 2 est entrain de travailler et prend les outils  
le mineur 1 est entrain de travailler et prend les outils  
le mineur 2 a fini de travailler et rend les outils il a récolté X grammes d'or
le mineur 3 est entrain de travailler et prend les outils  
le mineur 1 a fini de travailler et rend les outils il a récolté X grammes d'or
le mineur 3 a fini de travailler et rend les outils il a récolté X grammes d'or 
fin du travail des mineurs la somme totale récupéré est : X grammes d'or
```

## 🔧 Détails techniques

### Structure du sémaphore

```c
union semun {
    int val;                    // Valeur pour SETVAL
    struct semid_ds *buf;       // Buffer pour IPC_STAT, IPC_SET
    unsigned short *array;      // Tableau pour GETALL, SETALL
    struct seminfo *__buf;      // Buffer pour IPC_INFO
};
```

### Opérations principales

- **Création** : `semget(key, 1, IPC_CREAT | 0644)`
- **Initialisation** : `semctl(semid, 0, SETVAL, arg)`
- **P (acquérir)** : `semop(semid, {0, -1, 0}, 1)`
- **V (libérer)** : `semop(semid, {0, +1, 0}, 1)`
- **Suppression** : `semctl(semid, 0, IPC_RMID)`

### Clé IPC

La clé est générée avec `ftok("main", 'T')`, assurez-vous qu'un fichier nommé `main` existe dans le répertoire courant.(Attention je genere deux clées differente pour les zones de mémoire partagé et les semaphores mais basé sur le meme fichier "main"

## ⚠️ Points importants

- Le sémaphore est **partagé** entre tous les processus
- `sem_flg = 0` : le processus **attend** si la ressource n'est pas disponible
- `sem_flg = IPC_NOWAIT` : le processus **échoue immédiatement** si la ressource n'est pas disponible
- Le sémaphore est nettoyé avec `IPC_RMID` à la fin du programme
- les zones de mémoire partagé sont ouverte par chaque processus quand il en a besoin, grace aux semaphores un seul processus à la fois peut ecrire dans la zone de mémoire partagé

## 🐛 Nettoyage des sémaphores

Si le programme plante, les sémaphores peuvent rester en mémoire. Pour les nettoyer :

```bash
# Lister les sémaphores
ipcs -s

# Supprimer un sémaphore spécifique
ipcrm -s 

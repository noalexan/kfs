### 🏗️ Part 1: Foundations (Data & Memory Structures)

**Objectif :** Définir les structures de données statiques qui représentent un processus en mémoire. Rien ne bouge encore, on définit juste "ce qui existe".

#### 1. Le PCB (`struct task_struct`)
Le cœur du système. voici un exemple de ce qu'il peu contenir :

* **Identité :** `pid_t pid`.
* **État :** `enum process_state state` (NEW, RUNNING, WAITING, ZOMBIE).
* **Contexte CPU :** `uint32_t esp` (Le pointeur de pile noyau sauvegardé lors d'un switch).
* **Mémoire :** `uint32_t cr3` (L'adresse physique du Page Directory du processus).
* **Maillage :** Pointeurs `struct task_struct *next` (pour la liste circulaire / ready queue).
* **Pile Noyau :** `void *kernel_stack` (Pointeur vers le bas de la pile allouée pour ce process).

#### 2. La "Trap Frame" (Register Snapshot)

**Objectif**
Définir la structure C (`struct trap_frame`) qui mappe exactement les données empilées lors d'une interruption. Elle servira d'interface entre l'Assembleur (ISR) et le C.

**Composition (du haut de la Stack vers le bas / Struct C)**

La structure doit contenir 3 couches distinctes :
1.  **Registres de Segments :** `ds`, `es`, `fs`, `gs` (sauvegardés manuellement).
2.  **Registres Généraux (GPRs) :** `edi`, `esi`, `ebp`, `esp`, `ebx`, `edx`, `ecx`, `eax` (correspondant à l'instruction `pusha`).
3.  **Hardware Context :** `eip`, `cs`, `eflags`, + `user_esp`, `user_ss` (poussés automatiquement par le CPU lors du changement de Ring).

#### 3. Memory Layout (Virtual Memory)

**Objectif**
Définir la cartographie de l'espace virtuel (4GB) propre à chaque processus.

**A. L'Espace d'Adressage (The Map)**
Division stricte de la mémoire virtuelle :
* **Kernel Space (Haut)** : Identique pour tous les processus. Inaccessible en Ring 3.
* **User Space (Bas)** : Zone privée du processus.
    * *Code/Data* : Début de la mémoire basse.
    * *Heap* : Grandit vers le haut.
    * *User Stack* : Part du haut de l'espace utilisateur et grandit vers le bas.

**B. Isolation (CR3)**
Chaque processus possède sa propre copie du **Page Directory**.
* Il ne voit que *ses* données et le Kernel.
* Il est physiquement isolé des autres processus.

**C. La Stratégie des Piles (Dual Stacks)**
Un processus a besoin de deux piles distinctes :
* **User Stack (Ring 3)** : Pour l'exécution normale du programme.
* **Kernel Stack (Ring 0)** : Petite pile fixe (allouée par `kmalloc` dans le PCB) utilisée exclusivement lors des appels système et interruptions.

#### 4. Concurrency

Deux outils distincts pour protéger le noyau contre la concurence, utilisables au choix selon le besoin :

**A. L'Interrupteur Global (`irq_disable` / `irq_enable`)**
* **C'est quoi ?** Le bouton "Stop tout". Il coupe les interruptions CPU (CLI).
* **À quoi ça sert ?** Pour les sections ultra-critiques et courtes où le noyau ne doit *absolument pas* être dérangé (ex: pendant le changement de contexte).
* **Mécanisme :** Doit inclure un compteur pour pouvoir être appelé plusieurs fois de suite sans tout réactiver trop tôt.

**B. Le Verrou d'Objet (`spinlock`)**
* **C'est quoi ?** Un cadenas qu'on attache à une donnée précise (ex: `tty_lock`, `process_list_lock`).
* **À quoi ça sert ?** Pour protéger une ressource partagée. C'est plus propre : on sait *pourquoi* on bloque (on attend l'accès au TTY).

#### 5. Le TSS (Task State Segment)

**Objectif**
Structure matérielle x86 obligatoire pour gérer la transition de privilège (Ring 3 vers Ring 0).

* **Stratégie "Single TSS" :** Nous n'utilisons pas le multitâche matériel d'Intel. Une seule instance `TSS` est déclarée globalement pour tout le noyau.
* **Rôle Unique :** Stocker le pointeur `ESP0`.
    * À chaque `context_switch`, le noyau mettra à jour ce champ `ESP0` dans le TSS avec l'adresse de la pile noyau du *nouveau* processus.
    * Lors d'une interruption, le CPU lit ce champ pour savoir où empiler les données en toute sécurité.
* **GDT :** Nécessite une entrée spécifique dans la Global Descriptor Table.

#### 6. Le PID Manager (Pour la Part 1)
**Le Problème :** Dans la struct PCB, tu as `pid_t pid`. Mais comment garantis-tu que chaque PID est unique, surtout après 10 000 créations/destructions de processus ? Un simple `++` finit par "wrap around" (revenir à 0) et créer des collisions.

**L'Ajout (Dans Part 1 - Foundations) :**
* **PID Allocator :** Un système simple (Bitmap ou Liste de PIDs libres) pour savoir quels ID sont disponibles.

---------------------------

### ⚙️ Part 2: Execution (Lifecycle & Operations)

**Objectif :** Définir les mécanismes de bas niveau permettant à un processus de naître, de changer de contexte, et de mourir proprement.

#### 1. Context Switch (`switch_to`)
Le cœur du multitâche. Une fonction purement assembleur.
* **Prototype :** `void switch_to(struct task_struct *prev, struct task_struct *next);`
* **Mécanisme (Stack Switching) :**
    1.  Sauvegarder les registres "Callee-Saved" (EBX, ESI, EDI, EBP) sur la stack de `prev`.
    2.  Sauvegarder `ESP` actuel dans `prev->esp`.
    3.  Charger `next->esp` dans le registre `ESP`.
    4.  **Crucial :** Mettre à jour `g_tss.esp0` avec la nouvelle adresse de stack noyau (pour les futures interruptions).
    5.  Restaurer les registres "Callee-Saved" depuis la (nouvelle) stack.
    6.  `RET` (Le CPU dépile l'EIP de retour... qui appartient au nouveau processus !).

#### 2. Creation : Le clonage (`fork`)
La fonction la plus complexe à implémenter.
* **Memory Ops :**
    * `kmalloc` d'un nouveau PCB.
    * Duplication du Page Directory (VMM).
    * Utilisation d'un system copy on write (attention au dirty cow)
* **Kernel Stack Crafting (La partie magique) :**
    * Il faut construire artificiellement la stack du fils.
    * Y placer une "Trap Frame" factice pour que, quand le scheduler le choisira, il "retombe" à l'instruction suivant le fork.
    * Forcer la valeur de retour `EAX = 0` dans cette stack (pour que le fils sache qu'il est le fils).

### 2.5. Le Bootstrap du Premier Processus 
**Le Problème :** `fork()` copie un processus existant. Mais comment naît le **tout premier** processus (`init`) puisqu'il n'a pas de parent à copier ?

**L'Ajout (Dans Part 2 - Execution) :**
* **Manual Crafting (Process 0/1) :** Une fonction spéciale `init_process_creation()` qui :
    1.  Alloue un PCB à la main.
    2.  Configure manuellement sa stack et ses registres.
    3.  Le place de force dans la `Ready Queue` pour lancer la machine.


#### 3. Loading : L'exécution (`execve`)
* **Rôle :** Remplacer l'image mémoire du processus actuel par un nouveau programme (ELF).
* **Étapes :**
    1.  Vider l'espace d'adressage actuel (unmap).
    2.  Parser le fichier ELF.
    3.  Mapper les segments (Code, Data) en mémoire.
    4.  Allouer une nouvelle pile utilisateur et y copier les arguments (`argv`, `envp`).
    5.  Reset des registres (EIP = Entry Point du ELF).

#### 4. Termination : La mort (`exit` & `wait`)

Gestion de la fin de vie et de l'état ZOMBIE.
* **`exit(int status)` :**
    * Libère toute la mémoire (VMM, User Stack, Heap).
    * Ferme les fichiers ouverts.
    * **NE LIBÈRE PAS** le PCB ni la Kernel Stack.
    * Passe l'état à `ZOMBIE`.
    * Envoie un signal (`SIGCHLD`) au parent.
    * Appelle `schedule()` (ne revient jamais).
* **`wait(int *status)` :**
    * Le parent se met en pause (WAITING) tant qu'il n'a pas de fils Zombie.
    * Si un fils est Zombie : Récupère le `status`, et appelle enfin `kfree(child_pcb)`.

#### 5. Orphan Management (Adoption)

Que se passe-t-il si le Parent meurt *avant* le Fils ?
* **Le problème :** Si le fils fait `exit`, personne ne fera `wait`, donc il restera Zombie pour l'éternité (Memory Leak).
* **La solution (Reparenting) :** Lors du `exit()` d'un processus, s'il a des enfants, on change leur `parent_id` pour le mettre à `1` (Processus `init`). `init` a pour seule mission de faire des `wait()` en boucle pour nettoyer les orphelins.

#### 6. Système de Signaux

**Objectif**
Gérer les interruptions asynchrones en détournant temporairement l'exécution du processus.

**A. Les Données**
Le PCB doit simplement savoir :
1.  Quels signaux sont arrivés (Bitmap `pending`).
2.  Comment les traiter (Tableau d'actions : Tuer, Ignorer, ou Fonction Perso).

**B. Le Mécanisme : "Le Détournement"**
Au moment de rendre la main au processus, si un signal est présent :
1.  Le noyau **sauvegarde** l'état du processus sur sa propre pile.
2.  Il **force** le CPU à exécuter la fonction du signal (Handler) au lieu de continuer le programme.

**C. Le Retour**
Une fois la fonction du signal terminée, le processus utilise un mécanisme de "Trampoline" (Syscall `sigreturn`) pour **restaurer** ses registres sauvegardés et reprendre son travail exactement là où il s'était arrêté.


---------------------------

### ⏱️ Part 3: Scheduling & Dispatching

**Objectif :** Définir "Qui" a le droit d'utiliser le CPU, "Quand", et gérer les risques liés à l'interruption brutale des processus.

#### 1. Core Architecture (Queues & States)
* **State Machine :** Implémentation de l'enum `process_state`.
    
    * **New** : En création.
    * **Running** : Sur le CPU.
    * **Waiting** : Attend une ressource (Clavier, Disque).
    * **Ready** : Attend le CPU.
    * **Terminated** : Fini.
* **Dynamic Queues :**
    * Utilisation de listes chaînées pour créer dynamiquement des files d'attente pour les futurs drivers (Keyboard queue, Disk queue, etc.).

### 1.5. La Tâche "Idle" (Pour la Part 3)
**Le Problème :** Que se passe-t-il si la `Ready Queue` est vide ? (Ex: tous les processus attendent une I/O clavier ou disque).
Si `schedule()` ne trouve personne à exécuter, il retourne `NULL` -> `switch_to(current, NULL)` -> **Crash Kernel Panic**.

**L'Ajout (Dans Part 3 - Scheduling) :**
* **The Idle Task (PID 0) :** Un processus spécial, créé au démarrage, qui a la priorité la plus basse possible.
* **Rôle :** Il ne fait qu'une boucle infinie contenant l'instruction `hlt` (pour économiser l'énergie du CPU).
* **Logique :** Il est toujours `READY`. Si le Scheduler ne trouve personne d'autre, il choisit *toujours* Idle.

#### 2. Concurrency & Preemption Challenges
L'ordonnancement préemptif introduit le risque critique de **Race Condition**.

* **Le Problème :** Deux processus (ou un processus et une interruption) tentent de modifier la même structure de données (ex: la Ready Queue) en même temps.
* **La Solution (Critical Sections) :**
    * **Syscalls :** Doivent garantir la cohérence des données avant de rendre la main.
    * **ISR & Kernel Code :** Toute manipulation de structure partagée doit être protégée.
    * **Mécanisme :** Utiliser un des mecanisme fait dans fondation donc spin lock ou deactivation des interrupt

#### 3. The Dispatcher
C'est le module de bas niveau qui exécute la décision du scheduler. Il doit être optimisé pour réduire la **Dispatch Latency**.
**Fonctions clés :**
1.  **Context Switch :** Sauvegarde l'état A, charge l'état B.
2.  **Mode Switch :** Transition Ring 0 $\to$ Ring 3.
3.  **Restore EIP :** Saut vers la prochaine instruction du programme utilisateur.

#### 4. Scheduling Algorithms 
Ordre d'implémentation pour le développement :
1.  **FCFS (First-Come, First-Served)** : Simple file d'attente. Le processus doit `yield` volontairement. (Dev/Debug only).
2.  **Round Robin (RR) - *Objectif V1*** : FCFS + Timer Interrupt (Quantum de temps). C'est le standard des systèmes à temps partagé.
3.  **MLFQ (Multi-Level Feedback Queue) - *Objectif V2*** : Files multiples avec priorités dynamiques (favorise les I/O bound, punit les CPU bound).

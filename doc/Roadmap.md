# KFS

<!-- ///////////////////////////////////////////////////////////////////////////////// -->

## k1   [x] Hello world kernel
## k2   [x] GDT
## k4   [x] IDT                                                <!---Deadline - 25 Mai 2025->
## lk2  [x] Driver and interrupt 


## rt	[ ] Random task : implement backspace and TSS
## k3   [ ] Memory
<!-- ///////////////////////////////////////////////////////////////////////////////// -->

<!-- Pause de 1/2semaine -->

<!-- ///////////////////////////////////////////////////////////////////////////////// -->

## lk2 [] : Process and Memory :

-   Cree un syscall custom qui affiche le PID d'un process
-   recoder les syscall `kill` `wait` `mmap` `fork`

## k5 [ ] Process Management :                                <!---Deadline - 30 Juin 2025->

<!-- Genere par IA a verifier avec le sujet et de la doc  -->

### [ ] PCB (Process Control Block)

Définir la structure struct task_struct avec :
-   PID, état (RUN/ZOMBIE/THREAD), UID
-   Pointeurs parent/children, file de signaux (struct list_head)
-   Contexte CPU (registres ESP/EIP), répertoire de pages (CR3).
-   Integrer BSS et DATA section dans la struct

### [ ] Mémoire
-   Initialiser stack/heap via le paging existant (1 page par défaut).
-   Track mémoire : table des pages virtuelle → physique.

### [ ] Multitâche + Interrupts
-   Scheduler Round-Robin avec timer interrupt (10 ms).
-   Fonction switch_to(context_old, context_new) en ASM.
-   Liens IDT → gestion signaux syscalls.

### [ ] Fonctions

-   fork() : Copie PCB + duplication "lazy" de la mémoire.
-   kill(pid) : Ajout d'un signal (ex: SIGKILL) à la queue.
-   wait()/exit() : Gestion des zombies.
    mmap() : Mapping mémoire à la demande.

### [ ] Tests

`exec_fn()` : Charger une fonction en mémoire et lancer un processus.
prototype : `void exec_fn(unsigned int *addr:32, unsigned int *function:32, unsigned int size);`

Tester :
-   2 processus alternant via schedule().
-   fork() → vérifier PID enfant et mémoire.
-   Vérifier l'intégrité des sections BSS/DATA après fork

<!-- ///////////////////////////////////////////////////////////////////////////////// -->

## k6 [ ] Process Management                                                    <!---Deadline - 25 Juillet 2025->

### lk3 : Filesystem
### lk0 : Userspace Digressions

 <!-- Les prochaine deadline sont dans un monde ideal, donc a ne pas prendre en compte -->

<!-- ///////////////////////////////////////////////////////////////////////////////// -->

## k7 [ ] SysCall                                                               <!---Deadline - Mi Aout 2025->

<!-- ///////////////////////////////////////////////////////////////////////////////// -->

## k8 [ ] Kernel Module                                                         <!---Deadline - Fin Aout 2025->

<!-- ///////////////////////////////////////////////////////////////////////////////// -->

## k9 [ ] ELF Parser                                                            <!---Deadline - Fin Aout 2025->

<!-- ///////////////////////////////////////////////////////////////////////////////// -->

## kx [ ] The End                                                               <!---Deadline - Mi septembre 2025->

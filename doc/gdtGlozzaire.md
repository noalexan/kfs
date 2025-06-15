# Global Description Table

Usefull links :

- [os-dev tuto](https://wiki.osdev.org/GDT_Tutorial)
- [kfs3](https://samypesse.gitbook.io/how-to-create-an-operating-system/chapter-6)
- [navi](https://segmentfault.com/a/1190000040187304/en)

## Definitions

- `Segment` : Plage d'adresses continues dans un espace d'adressage linéaire, possède des propriétés cohérentes (type, droits d'accès, taille). Constitue une unité logique définie par une adresse de base et une limite.

- `Segment Register` :
Les registres de segment sont des registres spécialisés de 16 bits qui pointent vers différentes zones de la mémoire :
  - CS (Code Segment): Pointe vers le segment contenant les instructions du programme à exécuter.
  - DS (Data Segment): Pointe vers le segment contenant les données du programme.
  - SS (Stack Segment): Pointe vers le segment de pile utilisé pour les appels de fonctions et variables locales.
  - ES (Extra Segment): Utilisé pour les données supplémentaires dans les programmes multi-segments.
  - FS et GS: Registres additionnels pour les données en mode protégé.
Ces registres permettent au processeur d'accéder à différentes parties de la mémoire selon leur fonction spécifique

- `Segment Selector` :
Un sélecteur de segment est une valeur de 16 bits qui sert de référence à un descripteur dans une table de descripteurs. Sa structure comprend:
  - Bits 3-15 (Index): Indique l'adresse du descripteur dans la table (13 bits).
  - Bit 2 (TI): Spécifie la table à utiliser (0 = GDT, 1 = LDT).
  - Bits 0-1 (RPL): Niveau de privilège demandé (0-3).
Le sélecteur est utilisé par le processeur pour retrouver le descripteur correspondant dans la table appropriée

- `Segment Descriptor` :
Un descripteur de segment est une structure de données de 8 octets qui définit les caractéristiques d'un segment mémoire:
  - Base Address: Adresse de début du segment dans l'espace d'adressage linéaire.
  - Segment Limit: Taille maximale du segment.
  - Granularity (G): Détermine l'unité de mesure de la limite (octets ou pages).
  - Type: Définit le type de segment (code, données, système).
  - DPL (Descriptor Privilege Level): Niveau de privilège du segment.
  - Present Bit (P): Indique si le segment est présent en mémoire.
  - Autres flags: D/B, L, S, AVL pour diverses fonctionnalités.
Lorsqu'un sélecteur est chargé dans un registre de segment, le descripteur associé est automatiquement chargé dans un registre caché associé, permettant au processeur d'accéder rapidement aux caractéristiques du segment

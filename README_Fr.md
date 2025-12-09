# MiniDB: "Projet Final de C (ESGI)"

Un SGBDR simplifié développé entièrement en C, avec création de tables,
insertion, sélection, suppression, opérations de JOIN et persistance sur disque.

Projet final de C pour ESGI 2A3 (2025–2026) réalisé par
**Minh Cat DO**, **Paco ROBINET-CAMPOY** et **Cheikh Ahmadou Bamba CISSE**.

## Table_des_matieres

1. Description du projet
2. Prérequis
3. Installation
4. Fonctionnalités
5. Utilisation
6. Choix techniques
7. Contributeurs

## 1. Description_du_projet

MiniDB est une simulation d'un moteur de base de données relationnelle léger. Il prend en charge des commandes de type SQL, stockées en mémoire via :

- Des listes chaînées (tables, colonnes, lignes)

- Un index de table de hachage (concentration moyenne de recherche O(1) pour les clés primaires et les colonnes uniques)

- Une gestion dynamique de la mémoire avec une discipline stricte d'allocation/libération de mémoire

- L'import/export de fichiers binaires pour la persistance des données

- Une implémentation de jointure de type tri-fusion pour les relations entre les tables

Ce moteur est conçu pour simuler le comportement d'un SGBD minimal tout en préservant la clarté interne, la sécurité mémoire et les performances.

## 2. Pré-requis

Pour compiler et faire tourner le programme:

- Windows OS (Makefile fourni)
- gcc
- mingw32-make.exe

## 3. Installation

```bash
git clone https://github.com/DoMinhCat/MiniDB.git
cd MiniDB/
mingw32-make clean
mingw32-make
./MiniDB.exe
```

## 4. Fonctionnalités

### Général

- Pas besoin de guillemets pour les chaînes.
- Sensibilité à la casse pour noms de tables/colonnes.
- Types supportés: int, string, double.
- Plusieurs commandes possibles.
- Commande terminée par un `;`.

### Insertion et integrite

- Ordre des colonnes respecté.
- Toutes les colonnes doivent être remplies.
- Auto-incrément sur PK int.
- PK/FK doivent être int ou string (int > 0).
- Indexation UNIQUE/PK via hash.
- FK : ON DELETE RESTRICT.

### Select

- Projection + filtrage simples.
- `JOIN` Sort-Merge.
- Conflits de noms : la première table l'emporte.
- `WHERE` + `JOIN` : `WHERE` appliqué sur la première table.
- Pas encor>e de `AND`/`OR`.

### Limitations

- Tables non modifiables après création.
- Pas de mise à jour de ligne (delete + insert).
- Pas de NOT NULL/DEFAULT.
- Une seule PK → pas de many-to-many.
- Ordre de suppression dépendant des FK.

### Import

- Fichiers à importer dans le répertoire courant.

### Limites internes

- max_columns: 50
- max_string_length: 256
- max_tables: 100
- max_name_length: 100
- row_warning_at: 10000

### Sortie

- Utilisez `quit;` ou `exit;`

### 5. Utilisation

- `create table tab1 ( col1 int pk, col2 string fk references tab_to_refer col_to_refer, col3 double );`
- `show tables;`
- `describe table tab1;`
- `insert into tab1 ( col1, col2 ) values ( val1, val2 );`
- `delete from tab1 [where col1 = 6];`
- `drop table tab1 [, tab2, tab3,...];`
- `select ( col1, col2, col3 ) from tab1 [join tab2 on col1 = col2] [where col1 = xyz];`  
  *L'ordre des colonnes dans JOIN doit respecter l'ordre des tables passé dans la commande.*

---

### 6. Choix techniques

- Structures bas niveau en C (listes chaînées)
- Table de hachage avec chaînage pour PK/UNIQUE
- Parser remplissant une `struct Query` compacte via `union`
- JOIN implémenté en **Sort-Merge**, adapté aux listes chaînées
- Manipulation des données *in-place* via pointeurs
- Gestion mémoire stricte (`malloc`/`free`) avec module de nettoyage
- Persistance via fichiers binaires (import/export)

---

### 7. Contributeurs

- **Minh Cat DO**  
- **Paco ROBINET-CAMPOY**  
- **Cheikh Ahmadou Bamba CISSE**

*ESGI Paris – Promotion 2025-2026*

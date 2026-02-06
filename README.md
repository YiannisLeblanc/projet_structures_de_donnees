# Projet ALSI51 — Compilation et exécution d’un mini-langage

Ce projet consiste à développer un compilateur et un exécuteur pour un mini-langage comportant des variables globales, des structures conditionnelles (`if`), des boucles (`while`), des impressions (`print`) et une valeur de retour (`return`).  
Le pipeline repose sur trois étapes : analyse lexicale, analyse syntaxique, puis exécution via un AST.

---

## Compilation et exécution

### Flag DEVELOPPEMENT

Deux modes d'exécution sont disponibles dans `main.c` contrôlés par le flag `#define DEVELOPPEMENT` :

- **Mode développement** (`#define DEVELOPPEMENT` actif)  
  Utilise la fonction `main` fournie par l'enseignant. Le fichier testé est défini directement dans le code (`../code/monCode.txt`).

- **Mode release** (`#define DEVELOPPEMENT` commenté)  
  Utilise une fonction `main` prenant **un seul argument** (`argc/argv`) : le fichier à exécuter.

### Recommandations

**Il est fortement conseillé de compiler et exécuter le programme en mode release** (sans le flag `DEVELOPPEMENT`) pour utiliser le programme comme un logiciel classique :

```bash
./compiler_proj monCode.txt
```

Cela permet de passer le fichier en paramètre et d'exécuter n'importe quel script sans modifier le code source du programme.

Le mode développement est utile pour tester et déboguer lors du développement, mais le flag doit être commenté pour une utilisation en production.

---

## Partie 1 — Exécution d’un AST

Objectif : exécuter un programme représenté sous forme d’AST.

Fonctionnalités principales :

- Évaluation des expressions RPN avec gestion des variables.
- Implémentation des opérateurs dans `apply_op`.
- Implémentation de la libération mémoire de l’AST (`destroy_ast`).
- Exécution récursive du programme via `run` et une fonction auxiliaire.

---

## Partie 2 — Analyse syntaxique

Objectif : transformer une liste de tokens en AST.

Travail réalisé :

- Implémentation des fonctions `parse` et `parse_aux`.
- Construction de la structure AST à partir des tokens produits.

> Lors des tests, un bug a été identifié dans la fonction `parse_expr` fournie :  
> les opérateurs `<` et `>` consommaient le caractère suivant lorsqu’il n’y avait pas d’espace.  
> Un correctif a été appliqué.

---

## Partie 3 — Extensions

Extension choisie : **analyseur lexical (lexer)**.

Fonctionnalités implémentées :

- Génération des tokens à partir du code source.
- Ajout du caractère `#` pour gérer les **commentaires**.

Seule la partie lexer a été réalisée dans cette section.

---

## Usage de l’IA

L’utilisation de l’IA a été strictement limitée à la recherche de fonctions existantes dans le projet afin de faciliter la navigation dans le code (ex. vérifier si une fonctionnalité était déjà implémentée).

---

## Résumé

- Exécution complète d’un AST (Partie 1).
- Parsing fonctionnel vers AST (Partie 2).
- Lexer partiellement étendu avec gestion des commentaires `#` (Partie 3).
- Deux modes d’exécution via compilation conditionnelle.

# Redstone Logic Synthesis Tool
# Rapport de Projet

## Introduction

## Structure du projet

Le projet est composé d'une bibliothèque, codée dans `rlst_core`, et d'un lanceur, codé dans `rlst`.

Le lanceur contient juste un `main`.

La bibliothèque est divisé en différents packages:
- `cell_reader`: lecture des fichiers décrivant les gates
- `par`: placement et routage
- `svg`: représentation en svg des cellules et des routes

## Lecture du VHDL avec Yosys

### Portes logiques disponibles

Les portes logiques sont définies dans des fichiers json (un par porte) qui décrivent:
- leur taille
- la position dans la porte des ports entrants et sortants
- les blocs qui les composent (utile plus tard pour l'édition du monde Minecraft)

Les portes `AND`, `NOT` et `OR` sont décrites dans ces fichiers, elles sont suffisantes pour former les autres portes en les combinant, mais il serait aussi possible d'ajouter des fichiers décrivant d'autres portes.

Les fichiers sont lus un par la fonction `generate_gate` dans le package `cell_reader`.
Elle est appelée dans `main`.

## Placement

## Routage

### Routage

Le routage est fait par la fonction `route` dans le package `par`.

Pour router, le programme profite du fait que le monde Minecraft est en trois dimensions pour avoir des superpositions entre les routes.
Deux routes qui ne sont pas au même niveau sur l'axe z peuvent s'intersecter sur les autres axes.
Pour changer de niveau sur l'axe z, nous générons des structures qui permettent de faire monter le signal sur les ports sortants qui font partie de net, et de faire descendre le signal sur les ports entrants qui font partie d'un net.
Tous les signaux sont au moins montés / descendus d'un niveau pour être par dessus les cellules.
Monter et descendre de niveau a un coût en temps.

Pour que deux signaux au même niveau sur l'axe z ne s'activent pas l'un l'autre, il faut que la distance |x_bloc_1 - x_bloc_2| + |y_bloc_1 - y_bloc_2| d'au moins 2.
Ceci peut rendre difficile de router par dessus certaines portes car les routes peuvent se faire activer par un port en passant à côté et les ports peuvent être assez serrés sur les portes.
Deux portes adjacentes peuvent aussi s'activer l'une l'autre en ayant des ports qui sont adjacents.
Pour éviter ces problèmes les portes sont artificiellement agrandies (+3 de hauteur et +3 de largeur).
Ceci garantit qu'elles n'ont pas de ports adjacents (aucune rotation n'est effectuée sur les portes lors du placement) et qu'il y a la place entre les cellules pour router si elles sont trop petites pour router par dessus.

L'algorithme de routage est le suivant
```
pour chaque route
    meilleur_chemin = NULL

    rendre accessibles les blocs adjacents aux ports source \
        et destination

    pour chaque niveau
        A_star pour trouver un chemin entre les deux ports
        si le chemin est meilleur que meilleur_chemin
            le sauvegarder comme meilleur
        si il est impossible de trouver une meilleur chemin
            break
    sauvegarder le meilleur chemin

    rendre les blocs du meilleur chemin indisponibles aux \
        chemins suivants

    rendre inaccessibles les blocs adjacents aux ports source \
        et destination
```

Les blocs indisponibles sont rangés dans une matrice en 3 dimensions.

Les routes sont triés par distance de Manhattan, de la moins longue à la plus longue, placer les plus courtes en premier donne de meilleurs résultats.

Le coût d'un chemin est sa longueur et le coût d'accéder au niveau où il est (4*16, cette valeur vient des 4 ticks (unités de temps) de Minecraft qu'il faut pour monter puis descendre d'un niveau, et du fait que tous les 16 blocs de longueur on perd un tick pour re-maximiser la puissance du signal).

Il est impossible de trouver un meilleur chemin quand le coût de meilleur chemin trouvé jusqu'à présent est inférieur au coût d'accéder au prochain niveau et de prendre le chemin qui correspond à la distance de Manhattan dessus.

Le A star utilise la distance de Manhattan comme heuristique, elle a l'avantage d'être admissible (ne surestime jamais le coût) et consistante (l'estimation est toujours inférieure ou égale à celle des voisins plus le coût d'aller à ces voisins).
L'heuristique admissible est nécessaire au fonctionnement de A star.
L'heuristique consistante permet de faire que quand un sommet est enlevé des sommets découverts le coût optimal pour y accéder a été trouvé ce qui améliore un peu l'algorithme.
L'algorithme est implémenté avec des maps pour accéder rapidement aux estimations, aux coûts, et aux sommets précédents.

L'algorithme a pour limite qu'il ne profite pas du fait que certains signaux pourraient partager des routes.
Ceci est compensé par le grand nombre de niveaux disponibles qui rendent possible beaucoup de superpositions de routes.

### Finalisation des routes

La fonction route se charge aussi de noter sur chaque `PlacedPort` le niveau le plus élevé utilisé.
Ceci permet à la fonction `generate_routing_blocks` de générer des structures de blocs qui permettent de faire descendre le signal depuis le niveau approprié pour les ports entrants, et de le faire monter jusqu'au niveau approprié pour les ports sortants.

La sortie de la fonction route contient aussi l'information de quels blocs des routes sont des `repeaters` (et de dans quel direction ils sont orientés), ces blocs re-maximisent la puissance du signal car sinon il meurt au bout de 16 blocs.
Ceci permet à la fonction `generate_routing_blocks` de générer les structures de blocs repréntant les connexions entre les portes.

## SVG

Le package `svg` contient quelques fonctions pour tester les différentes parties de la bibliothèque avant d'avoir accès aux fonctions d'édition du monde Minecraft.
Une fonction permet de dessiner des cellules, avec leurs ports et leur nom.
Une fonction permet de dessiner des points, sous forme de rectangle d'une couleur passée en paramètres, pour représenter des blocs.

## Edition du monde Minecraft

## Conclusion

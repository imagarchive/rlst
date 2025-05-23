---
header-includes: |
  \usepackage{nicefrac}
  \usepackage{physics}
  \DeclareMathOperator{\sign}{sign}
---

# Redstone Logic Synthesis Tool - Rapport de Projet

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

### Avant-propos

L'algorithme de placement repose sur plusieurs structure de données dont on détaillera l'utilité ultérieurement.
En particulier, on a :

- `rlst::core::grid`,
- `rlst::core::details::grid_iterator`, et
- `rlst::par::placement_grid`.

`rlst::core::grid` est un type _template_ dont l'objectif est de fournir une interface pratique pour une matrice à deux dimensions.
Ce type est donc un conteneur au sens du C++ : il respecte les _named requierements_ `Container` and `ReversibleContainer`.

Un itérateur `rlst::core::details::grid_iterator` a été implémenté afin de simplifier l'itération sur une sous-matrice, c'est-à-dire que pour n'importe quelle sous-matrice d'une `rlst::core::grid`, il est capable de fournir une manière séquentielle d'itérer dessus.
Il respecte les _named requierements_ `LegacyIterator`, `LegacyInputIterator`, `LegacyOutputIterator`, `LegacyForwardIterator`, `LegacyBidirectionalIterator` et `LegacyRandomAccessIterator`.

`rlst::par::placement_grid` est une matrice de dimension trois représentant les recouvrements de cellules pour chaque point du plan.
Cette classe implémente `rlst::core::grid` et suit les mêmes _named requierements_ que cette dernière.

En s'appuyant sur les fonctionnalités offertes par `rlst::core::grid`, on est capable de récupérer la sous-matrice correspondant à une cellule (en prenant en considération sa taille ainsi que sa position).
On implémente également la méthode `insert()` (resp. `erase()`) dont l'objectif est d'ajouter (resp. de supprimer) une cellule sur la surface correspondante.
On a ainsi une classe qui nous permet d'obtenir pour chaque point du plan l'ensemble des cellules (potentiellement plusieurs ou aucune) qui se trouve à cet emplacement.

### Recuit simulé

Le principe de fonctionnement de l'algorithme de placement repose sur le [recuit simulé](https://fr.wikipedia.org/wiki/Recuit_simulé).
Au cours de l'étude bibliographique, on s'est appuyé sur les écrits de K. Shahookar et P. Mazumder[^1] et sur la page Wikipédia précédemment citée.
On a légérement modifié cet algorithme afin de donner une importance prépondérante au coût associé à la proportion de chevauchement.

```
PROCÉDURE recuit simulé;
    estimation de la taille de l'espace de travail;

    FAIRE
        initialisations;
        génération de la configuration initiale;

        POUR i de 0 à 140 FAIRE
            POUR j de 0 à 32 FAIRE
                génération d'une nouvelle configuration;
                évaluation de la nouvelle configuration;

                calcul du coût de superposition de la nouvelle
                configuration;

                SI le coût est plus faible ALORS
                    calcul des autres coûts pour la nouvelle
                    configuration;

                    SI le coût est plus faible ALORS
                        on sauvegarde la nouvelle configuration;
                        on continue;
                    SINON SI accepte(coût, température) > uniform() ALORS
                        on sauvegarde la nouvelle configuration;
                        on continue;
                    FIN SI
                FIN SI

                on restaure l'ancienne configuration;
            FIN POUR
        FIN POUR
    TANT QUE le coût de chevauchement n'est pas nul
FIN PROCÉDURE
```

On a empiriquement observé que ne pas rejeter précocement les configurations avec un surcoût de chevauchement induisait un bouclage infini.

La **fonction d'acceptation** est

\begin{align}
\exp \biggl( \frac{100 \cdot \Delta C}{T} \biggr)
\end{align}

La **température initiale** est

\begin{align}
T = 500
\end{align}

Empiriquement, on a montré que $\Delta C \simeq 1$ et $T \simeq 100$ ; ainsi pour que ces deux grandeurs est le même ordre de grandeur on multiplie $\Delta C$ par un facteur 100.

### Évaluation du coût d'une configuration

Certains coût ont le mauvais goût de ne pas être bornée ; cela pose problème car certains coût pourrait écraser totalement les autres.
Pour éviter cela, on a proposé d'utilisé une fonction de normalisation afin d'avoir des valeurs restreinte dans $[0 ; 100]$.

La **fonction de normalisation** a la forme suivante

\begin{align}
\nicefrac{100}{\pi} \cdot \sign x \cdot \atan \Bigl( \log \bigl( \abs{x} + 1 \bigr) \Bigr) + 50
\end{align}

Cette fonction a en effet le bon goût d'être

- bornée,
- symétrique,
- a une croissance relativement faible, et
- coupe l'axe des ordonnées à 50 pour $x = 0$.

Les coûts évaluent

- la proportion de chevauchements (bornée naturellement),
- la longueur estimée des fils (normalisée),
- la proportion de cases vides (bornée naturellement),
- l'écart relatif séparant la moyenne de la longueur des lignes de celle désirée (normalisée),
- la hauteur de la configuration (normalisée), et
- l'espace séparant l'axe des abscisses de la cellule la plus haute (normalisée).

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

Les blocs indisponibles sont rangés dans un vecteur à 3 dimensions.

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

Un monde minecraft est divsié en plusieurs régions de 32 * 32 = 1024 chunks.
Un chunk est une portion de région qui peut contenir jusqu'à 16 blocs en largeur, 16 blocs en longueur et 256 blocs de hauteur.
Les données de chaque région sont répertoriées dans des fichiers dont le nom est de type r.x.z.mca, avec x et z des entiers relatifs correspondant aux coordonnées de la région dans le monde.
Pour ce projet, nous nous limitons à la région r.0.0.mca, cependant, elle est suffisemment large pour nous permettre de réaliser nos objectifs.

L'édition du monde Minecraft a consisté en plusieurs étapes.
Tout d'abord, nous avons lu l'intégralité du fichier mca. Pour chaque chunk, il nous a fallu trouver où se trouvaient ses données dans le fichier à partir du header. Ce dernier contenait 4 octets par chunk, 3 pour encoder la position de ce dernier dans le fichier et 1 pour sa taille.
Nous avons notamment dû passer par certaines conversions, les fichiers de Minecraft étant en big endian.
Nous avons ensuite décompressé les données de chaque chunk à l'aide de la bibliothèque zlib et nous les avons placées dans une classe nommée "Chunk".
N'ayant aucun moyen de connaître à l'avance la taille des données décompressées, nous avons légèrement surestimé cette taille.
Nous avons également stocké les données compressées dans les objets chunk, cela nous a permis d'éviter une recompression future des données des chunks que nous n'avions pas modifiés.

Chaque chunk était accessible à partir de son indice, calculé en fonction de ses coordonnées, depuis un objet que nous avons nommé MCA.

Les données des chunks sont composées de structures NBT.
Il s'agit d'un format binaire hiérarchique sous forme de paires clé-valeur typées, organisées en arbres de tags imbriqués.
Pour les modifier, nous nous sommes aidés de la bibliothèque [lib nbt](https://github.com/djytw/libnbt). 

Pour modifier les blocs, il nous a fallu parcourir la structure arborescente jusqu'à trouver les noeuds "palette" et "blockstates" que nous avons modifié en prêtant notamment attention au boutisme.

Une fois les données modifiées, nous avons implémenté une fonction qui les recompresse sous le format zlib. Elle s'appuie notamment sur une fonction qui nous était fournie dans libnbt. Nous avons ensuite stocké les données compressées des chunks modifiés dans les objets chunks correspondants.

La dernière étape a été d'écrire toutes ces données dans un nouveau fichier d'extension .mca que crée notre programme. Pour cela, nous avons dû être particulièrement vigilants face au format strict de minecraft, que ce soit pour le header ou les données. Ces dernières devaient notamment être précédées de la taille des données compressées et du type de compression et devaient être alignées dans des blocs de 4096 octets appelés secteurs.


## Conclusion

[^1]: VLSI cell placement techniques (https://doi.org/10.1145/103724.103725)

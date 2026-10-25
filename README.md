# Moteur 3D temps réel (OpenGL)

TP d'informatique graphique temps réel du master Informatique ISICG (Université de Limoges, 2023-2024), en C++ et OpenGL 4.5.

Le projet part d'un squelette fourni par l'université (application SDL2, interface ImGui, gestion des TP). Chaque TP s'ajoute dans `src/lab_works/` et se choisit dans le menu de l'application.

## Contenu

- **TP 1 à 3** : premiers rendus OpenGL (VAO, VBO, EBO, shaders GLSL), transformations et caméra.
- **TP 4** : chargement de maillages et éclairage de Blinn-Phong (diffus et spéculaire).
- **TP 5** : textures et normal mapping (modèle Bunny texturé).
- **TP 6** : **rendu différé** (deferred shading) sur la scène Sponza : passe de géométrie qui remplit un **G-buffer** (positions, normales, couleurs ambiante, diffuse et spéculaire), puis passe d'éclairage sur un quad plein écran ; visualisation des textures du G-buffer dans l'interface.
- **Projet** : scène texturée personnelle.

## Compilation

Projet Visual Studio (Windows) : ouvrir `M3D_ISICG.sln`, compiler, puis lancer depuis la racine du dépôt (les shaders et les modèles sont chargés par chemin relatif). Les bibliothèques (SDL2, ImGui, gl3w, glm, assimp, stb) sont incluses dans `lib/`.

## Technologies

C++, OpenGL 4.5, GLSL, SDL2, ImGui, assimp, glm.

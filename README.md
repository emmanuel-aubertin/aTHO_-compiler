
# aTHO_ compiler

Ce projet est le fruit du travail d'Aubertin Emmanuel (L2G1), étant un étudiant récidiviste vous trouverez sûrement quelques commits de l'année dernière. Tous ces commits sont les miens et correspondent à ceux de ce dépôt : https://gitlab.com/aTHO_/aTHO_compiler

Pour toute la documentation, vous trouverez un exemple "statique" dans le dossier `documentation`. Pour la version interactive, vous devez vous rendre sur le wiki du dépôt (https://gitlab.com/aTHO_/aTHO_compiler/-/wikis/home).

Tout ce qui a été implémenté dans le compilateur peut être trouvé sur la page de documentation du langage appelée 'aTHOlang'. Toutes sont présentes dans le programe de test `input.aTHO`. Elles sont toutes présentes dans le programme de test `input.aTHO`.Vous trouverez toutes les implémentations unitaires dans le dossier `aTHOlang_example`.


Si vous avez des questions ou des problèmes techniques liés au rendu, je suis à votre entière disposition.

Cordialement,
Aubertin Emmanuel.

### Début du Readme :

Current version : [![pipeline status](https://gitlab.com/aTHO_/ue_compilateur/badges/master/pipeline.svg)](https://gitlab.com/aTHO_/ue_compilateur/-/commits/master)

A simple compiler. 

From : aTHOlang imperative LL(k) langage

To : 64 bit 80x86 assembly langage (AT&T)

## Installation :

Do `make install`

Or Ubuntu/Debian :

```
git clone https://gitlab.com/aTHO_/aTHO_compiler.git
cd ue_compilateur && make install
```

For all other distributions, it is sufficient to install all these dependencies:

`flex`, `flexc++`, `libfl-dev`, `g++`, `gcc`

## Documentation :

If you want more documentation (MakeFile, aTHOlang ...), you can go [here](https://gitlab.com/aTHO_/ue_compilateur/-/wikis/home)

## See also :

aTHOlang official VS-code extension [Click here](https://marketplace.visualstudio.com/items?itemName=aTHO.atholang&ssr=false#overview)

Updated 25 May 2021 by [Aubertin Emmanuel](https://www.linkedin.com/in/emmanuel-aubertin/)
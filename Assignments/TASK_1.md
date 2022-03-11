# Gestion des ressources

## Analyse de la gestion des avions

La création des avions est aujourd'hui gérée par les fonctions `TowerSimulation::create_aircraft` et `TowerSimulation::create_random_aircraft`.
Chaque avion créé est ensuite placé dans les files `GL::display_queue` et `GL::move_queue`.

Si à un moment quelconque du programme, vous souhaitiez accéder à l'avion ayant le numéro de vol "AF1250", que devriez-vous faire ?

---

Je pourrais le chercher dans la `move_queue` ou la `display_queue`.

## Objectif 1 - Référencement des avions

### A - Choisir l'architecture

Pour trouver un avion particulier dans le programme, ce serait pratique d'avoir une classe qui référence tous les avions et qui peut donc nous renvoyer celui qui nous intéresse.

Vous avez 2 choix possibles :
- créer une nouvelle classe, `AircraftManager`, qui assumera ce rôle,
- donner ce rôle à une classe existante.

Réfléchissez aux pour et contre de chacune de ces options.

Pour le restant de l'exercice, vous partirez sur le premier choix.

### B - Déterminer le propriétaire de chaque avion

Vous allez introduire une nouvelle liste de références sur les avions du programme.
Il serait donc bon de savoir qui est censé détruire les avions du programme, afin de déterminer comment vous allez pouvoir mettre à jour votre gestionnaire d'avions lorsque l'un d'entre eux disparaît.

Répondez aux questions suivantes :
1. Qui est responsable de détruire les avions du programme ? (si vous ne trouvez pas, faites/continuez la question 4 dans TASK_0)

`opengl_interface` lorsqu'un avion a été desservi et qu'il n'est plus au terminal.

2. Quelles autres structures contiennent une référence sur un avion au moment où il doit être détruit ?

`Displayable::display_queue` est de type `std::vector<const Displayable*>`
`DynamicObject::move_queue` est de type `std::unordered_set<DynamicObject*>`
`Terminal::current_aircraft` est de type `Aircraft*`
`Tower::reserved_terminals` est de type `std::map<const Aircraft*, size_t>`

3. Comment fait-on pour supprimer la référence sur un avion qui va être détruit dans ces structures ?

On utilise la fonction `erase` des conteneurs quand il faut supprimer l'avion du programme.
D'abord dans la boucle sur `move_queue` et dans le destructeur de `Displayable` pour `display_queue`

4. Pourquoi n'est-il pas très judicieux d'essayer d'appliquer la même chose pour votre `AircraftManager` ?

Je sais pas ?

Pour simplifier le problème, vous allez déplacer l'ownership des avions dans la classe `AircraftManager`.
Vous allez également faire en sorte que ce soit cette classe qui s'occupe de déplacer les avions, et non plus la fonction `timer`.

### C - C'est parti !

Ajoutez un attribut `aircrafts` dans le gestionnaire d'avions.
Choisissez un type qui met bien en avant le fait que `AircraftManager` est propriétaire des avions.

Ajoutez un nouvel attribut `aircraft_manager` dans la classe `TowerSimulation`.

Modifiez ensuite le code afin que `timer` passe forcément par le gestionnaire d'avions pour déplacer les avions.
Faites le nécessaire pour que le gestionnaire supprime les avions après qu'ils aient décollé.

Enfin, faites ce qu'il faut pour que `create_aircraft` donne l'avion qu'elle crée au gestionnaire.
Testez que le programme fonctionne toujours.

---

j'ai une classe AircraftManager :

```cpp
class AircraftManager : public GL::DynamicObject
{
private:
    std::vector<std::unique_ptr<Aircraft>> aircrafts;
public:
    bool update() override;
    void add_aircraft(std::unique_ptr<Aircraft> aircraft);
};
```

À l'initialisation de l'aéroport, on cree le `AircraftManager` et on l'ajoute à la `move_queue` :

```cpp
void TowerSimulation::init_airport()
{
    airport = new Airport { one_lane_airport, Point3D { 0, 0, 0 },
                            new img::Image { one_lane_airport_sprite_path.get_full_path() } };

    aircraft_manager = new AircraftManager {};
    GL::move_queue.emplace(airport);
    GL::move_queue.emplace(aircraft_manager);
}
```

Et à la création des avions, on les ajoute dans l'AircraftManager :

```cpp
aircraft_manager->add_aircraft(std::make_unique<Aircraft>(type, flight_number, start, direction, airport->get_tower()));
```

Il n'y a plus besoin de gérer la suppression des avions dans le timer :

```cpp
for (auto dynamic_object : move_queue)
{
    dynamic_object->update();
}
```

## Objectif 2 - Usine à avions

### A - Création d'une factory

La création des avions est faite à partir des composants suivants :
- `create_aircraft`
- `create_random_aircraft`
- `airlines`
- `aircraft_types`.

Pour éviter l'usage de variables globales, vous allez créer une classe `AircraftFactory` dont le rôle est de créer des avions.

Définissez cette classe, instanciez-la à l'endroit qui vous paraît le plus approprié, et refactorisez le code pour l'utiliser.
Vous devriez du coup pouvoir supprimer les variables globales `airlines` et `aircraft_types`.

```cpp
#pragma once

#include "aircraft.hpp"
#include "airport.hpp"

class AircraftFactory
{
public:
    static std::unique_ptr<Aircraft> with_airport(Airport* airport)
    {
        const std::string flight_number = airlines[std::rand() % 8] + std::to_string(1000 + (rand() % 9000));
        const float angle       = (rand() % 1000) * 2 * 3.141592f / 1000.f; // random angle between 0 and 2pi
        const Point3D start     = Point3D { std::sin(angle), std::cos(angle), 0 } * 3 + Point3D { 0, 0, 2 };
        const Point3D direction = (-start).normalize();
        return std::make_unique<Aircraft>(*(aircraft_types[rand() % 3]), flight_number, start, direction, airport->get_tower());
    }
    static inline void init_aircraft_types()
    {
        aircraft_types[0] = new AircraftType { .02f, .05f, .02f, MediaPath { "l1011_48px.png" } };
        aircraft_types[1] = new AircraftType { .02f, .05f, .02f, MediaPath { "b707_jat.png" } };
        aircraft_types[2] = new AircraftType { .04f, .1f, .04f, MediaPath { "concorde_af.png" } };
    };
private:
    static inline const std::string airlines[8] = { "AF", "LH", "EY", "DL", "KL", "BA", "AY", "EY" };
    static inline const AircraftType* aircraft_types[3];
};
```

### B - Conflits

Il est rare, mais possible, que deux avions soient créés avec le même numéro de vol.
Ajoutez un conteneur dans votre classe `AircraftFactory` contenant tous les numéros de vol déjà utilisés.
Faites maintenant en sorte qu'il ne soit plus possible de créer deux fois un avion avec le même numéro de vol.

```cpp
private:
  std::set<std::string> flight_numbers;

std::unique_ptr<Aircraft> with_airport(Airport* airport)
{
    std::string flight_number;
    do {
        flight_number = airlines[std::rand() % 8] + std::to_string(1000 + (rand() % 9000));
    } while(flight_numbers.find(flight_number) != flight_numbers.end());
    //...
}
```

### C - Data-driven AircraftType (optionnel)

On aimerait pouvoir charger les paramètres des avions depuis un fichier.

Définissez un format de fichier qui contiendrait les informations de chaque `AircraftType` disponible dans le programme.\
Ajoutez une fonction `AircraftFactory::LoadTypes(const MediaPath&)` permettant de charger ce fichier.
Les anciens `AircraftTypes` sont supprimés.

Modifiez ensuite le `main`, afin de permettre à l'utilisateur de passer le chemin de ce fichier via les paramètres du programme.
S'il ne le fait pas, on utilise la liste de type par défaut.

Si vous voulez de nouveaux sprites, vous pouvez en trouver sur [cette page](http://www.as-st.com/ttd/planes/planes.html)
(un peu de retouche par GIMP est necessaire)

---

## Objectif 3 - Pool de textures (optionnel)

Pour le moment, chacun des `AircraftType` contient et charge ses propres sprites.
On pourrait néanmoins avoir différents `AircraftType` qui utilisent les mêmes sprites.
Ils seraient donc chargés plusieurs fois depuis le disque pour rien.

Pour rendre le programme un peu plus performant, implémentez une classe `TexturePool` qui s'occupe de charger, stocker et fournir les textures.
Réfléchissez bien au type que vous allez utiliser pour référencer les textures, afin d'exprimer correctement l'ownership.

Listez les classes qui ont besoin de `TexturePool`.
Sachant que vous n'aurez qu'une seule instance de `TexturePool` dans votre programme, quelle classe devra assumer l'ownership de cet objet ?\
Instanciez `TexturePool` au bon endroit et refactorisez le code afin que tous les chargements de textures utilisent ce nouvel objet.
Assurez-vous que votre implémentation ne génère pas des fuites de mémoire au moment de sa destruction.

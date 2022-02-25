# Se familiariser avec l'existant

## A- Exécution

Compilez et lancez le programme.

Allez dans le fichier `tower_sim.cpp` et recherchez la fonction responsable de gérer les inputs du programme.
Sur quelle touche faut-il appuyer pour ajouter un avion ?

<hr />
C
<hr />

Comment faire pour quitter le programme ?

<hr />
x ou q
<hr />

A quoi sert la touche 'F' ?

<hr />
toggle_fullscreen
<hr />

Ajoutez un avion à la simulation et attendez.
Que est le comportement de l'avion ?

<hr />
l'avion atterrit, puis il est desservi, puis il repart 
<hr />

Quelles informations s'affichent dans la console ?

<hr />

```
LH1778 is now landing...
now servicing DL2186...
done servicing DL2186
DL2186 lift off
```

<hr />

Ajoutez maintenant quatre avions d'un coup dans la simulation.
Que fait chacun des avions ?

<hr />
la même chose
<hr />

## B- Analyse du code

Listez les classes du programme à la racine du dossier src/.

<hr />

- Aircraft
- AircraftType
- Airport
- AirportType
- Terminal
- Tower
- TowerSimulation
- Point2D
- Point3D
- Runway
- Waypoint

<hr />

Pour chacune d'entre elle, expliquez ce qu'elle représente et son rôle dans le programme.

<hr />

| classe        | représentation                                                                                  |
|---------------|-------------------------------------------------------------------------------------------------|
| aircraft_type | représente les différents types d'avions, permets de stocker les informations propre à un avion |
| aircraft      | représente les à proprement parler un avion ainsi que ses actions                               | 
| airport_type  | représente les informations d'un aéroports                                                      |
| airport       | représente à proprement parler un aéroport                                                      |
| geometry      | représente les actions possible sur les géométrie du projet                                     |
| main          | permet de lancer la simulation                                                                  |
| runaway       | représente le point dans l'espace qui indique aux avions qu'ils se sont éloignés de l'aéroport  |
| terminal      | représente les terminaux de l'aéroport, permets aux avions de récupérer des passagers           |
| tower_sim     | gère toute la simulations ainsi que les inputs de l'utilisateur                                 |
| tower         | représente les tours de controle                                                                |
| waypoint      | représente un point sur le chemin d'un avion                                                    |

<hr />

Pour les classes `Tower`, `Aircaft`, `Airport` et `Terminal`, listez leurs fonctions-membre publiques et expliquez précisément à quoi elles servent.

- Tower

```cpp
public:
  Tower(Airport& airport_) : airport { airport_ } {}
  // produce instructions for aircraft
  WaypointQueue get_instructions(Aircraft& aircraft);
  void arrived_at_terminal(const Aircraft& aircraft);
```

- constructeur
- genère une liste de points qui répresentent le chemin qu'un avion doit suivre
- démarre le service d'un avion

- Aircraft

```cpp
public:
    Aircraft(const AircraftType& type_, const std::string_view& flight_number_, const Point3D& pos_,
             const Point3D& speed_, Tower& control_) :
        GL::Displayable { pos_.x() + pos_.y() },
        type { type_ },
        flight_number { flight_number_ },
        pos { pos_ },
        speed { speed_ },
        control { control_ }
    {
        speed.cap_length(max_speed());
    }

    const std::string& get_flight_num() const { return flight_number; }
    float distance_to(const Point3D& p) const { return pos.distance_to(p); }

    void display() const override;
    void move() override;

    friend class Tower;
```

- constructeur
- renvoie le numero du vol
- renvoie la distance entre l'avion et un point donné
- dessine l'avion autour du point ou il est placé
- déplace l'avion vers son prochain waypoint

- Airport

```cpp
public:
    Airport(const AirportType& type_, const Point3D& pos_, const img::Image* image, const float z_ = 1.0f) :
        GL::Displayable { z_ },
        type { type_ },
        pos { pos_ },
        texture { image },
        terminals { type.create_terminals() },
        tower { *this }
    {}

    Tower& get_tower() { return tower; }

    void display() const override { texture.draw(project_2D(pos), { 2.0f, 2.0f }); }

    void move() override
    {
        for (auto& t : terminals)
        {
            t.move();
        }
    }

    friend class Tower;
```

- constructeur
- renvoie la tour de contrôle de l'aéroport
- dessine l'aéroport
- avance les terminaux de l'aéroport

- Terminal

```cpp
public:
    Terminal(const Point3D& pos_) : pos { pos_ } {}

    bool in_use() const { return current_aircraft != nullptr; }
    bool is_servicing() const { return service_progress < SERVICE_CYCLES; }
    void assign_craft(const Aircraft& aircraft) { current_aircraft = &aircraft; }

    void start_service(const Aircraft& aircraft)
    {
        assert(aircraft.distance_to(pos) < DISTANCE_THRESHOLD);
        std::cout << "now servicing " << aircraft.get_flight_num() << "...\n";
        service_progress = 0;
    }

    void finish_service()
    {
        if (!is_servicing())
        {
            std::cout << "done servicing " << current_aircraft->get_flight_num() << '\n';
            current_aircraft = nullptr;
        }
    }

    void move() override
    {
        if (in_use() && is_servicing())
        {
            ++service_progress;
        }
    }
```

- constructeur
- renvoie vrai si le terminal est occupé par un avion
- renvoie vrai si le terminal est en train de déservir un avion
- commence le service d'un avion
- termine le service d'un avion
- avance le service d'un avion

<hr />

Réalisez ensuite un schéma présentant comment ces différentes classes interagissent ensemble.

<hr />

> TODO

<hr />

Quelles classes et fonctions sont impliquées dans la génération du chemin d'un avion ?

<hr />

> TODO

<hr />

Quel conteneur de la librairie standard a été choisi pour représenter le chemin ?

<hr />

> TODO

<hr />

Expliquez les intérêts de ce choix.

<hr />

> TODO

<hr />

## C- Bidouillons !

1) Déterminez à quel endroit du code sont définies les vitesses maximales et accélération de chaque avion.

<hr />

Dans `aircraft_types.hpp` ligne 30

```cpp
inline void init_aircraft_types()
{
    aircraft_types[0] = new AircraftType { .02f, .05f, .02f, MediaPath { "l1011_48px.png" } };
    aircraft_types[1] = new AircraftType { .02f, .05f, .02f, MediaPath { "b707_jat.png" } };
    aircraft_types[2] = new AircraftType { .02f, .05f, .02f, MediaPath { "concorde_af.png" } };
}
```

<hr />

Le Concorde est censé pouvoir voler plus vite que les autres avions.
Modifiez le programme pour tenir compte de cela.

<hr />

```cpp
inline void init_aircraft_types()
{
    aircraft_types[0] = new AircraftType { .02f, .05f, .02f, MediaPath { "l1011_48px.png" } };
    aircraft_types[1] = new AircraftType { .02f, .05f, .02f, MediaPath { "b707_jat.png" } };
    aircraft_types[2] = new AircraftType { .04f, .1f, .04f, MediaPath { "concorde_af.png" } };
}
```

<hr />

2) Identifiez quelle variable contrôle le framerate de la simulation.

<hr />

GL/opengl_interface.hpp ligne 21
```cpp
inline unsigned int ticks_per_sec = DEFAULT_TICKS_PER_SEC;
```

<hr />

Ajoutez deux nouveaux inputs au programme permettant d'augmenter ou de diminuer cette valeur.

<hr />

opengl_interface.cpp
```cpp
void increase_fps()
{
    ticks_per_sec++;
}

void decrease_fps()
{
    ticks_per_sec--;
}
```

tower_sim.cpp
```cpp
GL::keystrokes.emplace('>', []() { GL::increase_fps(); });
GL::keystrokes.emplace('<', []() { GL::decrease_fps(); });
```

<hr />

Essayez maintenant de mettre en pause le programme en manipulant ce framerate. Que se passe-t-il ?\

<hr>

Le programme crache.

<hr />

Ajoutez une nouvelle fonctionnalité au programme pour mettre le programme en pause, et qui ne passe pas par le framerate.

<hr />

Ok

<hr />

3) Identifiez quelle variable contrôle le temps de débarquement des avions et doublez-le.

<hr />

Ok

<hr />

4) Lorsqu'un avion a décollé, il réatterrit peu de temps après. Faites en sorte qu'à la place, il soit retiré du programme.

Indices :
- A quel endroit pouvez-vous savoir que l'avion doit être supprimé ?
- Pourquoi n'est-il pas sûr de procéder au retrait de l'avion dans cette fonction ?
- A quel endroit de la callstack pourriez-vous le faire à la place ?
- Que devez-vous modifier pour transmettre l'information de la première à la seconde fonction ?



5) Lorsqu'un objet de type `Displayable` est créé, il faut ajouter celui-ci manuellement dans la liste des objets à afficher.
Il faut également penser à le supprimer de cette liste avant de le détruire.
Faites en sorte que l'ajout et la suppression de `display_queue` soit "automatiquement gérée" lorsqu'un `Displayable` est créé ou détruit.
Pourquoi n'est-il pas spécialement pertinent d'en faire de même pour `DynamicObject` ?

6) La tour de contrôle a besoin de stocker pour tout `Aircraft` le `Terminal` qui lui est actuellement attribué, afin de pouvoir le libérer une fois que l'avion décolle.
Cette information est actuellement enregistrée dans un `std::vector<std::pair<const Aircraft*, size_t>>` (size_t représentant l'indice du terminal).
Cela fait que la recherche du terminal associé à un avion est réalisée en temps linéaire, par rapport au nombre total de terminaux.
Cela n'est pas grave tant que ce nombre est petit, mais pour préparer l'avenir, on aimerait bien remplacer le vector par un conteneur qui garantira des opérations efficaces, même s'il y a beaucoup de terminaux.\
Modifiez le code afin d'utiliser un conteneur STL plus adapté. Normalement, à la fin, la fonction `find_craft_and_terminal(const Aicraft&)` ne devrait plus être nécessaire.

## D- Théorie

1) Comment a-t-on fait pour que seule la classe `Tower` puisse réserver un terminal de l'aéroport ?

2) En regardant le contenu de la fonction `void Aircraft::turn(Point3D direction)`, pourquoi selon-vous ne sommes-nous pas passer par une réference ?
Pensez-vous qu'il soit possible d'éviter la copie du `Point3D` passé en paramètre ?

## E- Bonus

Le temps qui s'écoule dans la simulation dépend du framerate du programme.
La fonction move() n'utilise pas le vrai temps. Faites en sorte que si.
Par conséquent, lorsque vous augmentez le framerate, la simulation s'exécute plus rapidement, et si vous le diminuez, celle-ci s'exécute plus lentement.

Dans la plupart des jeux ou logiciels que vous utilisez, lorsque le framerate diminue, vous ne le ressentez quasiment pas (en tout cas, tant que celui-ci ne diminue pas trop).
Pour avoir ce type de résultat, les fonctions d'update prennent généralement en paramètre le temps qui s'est écoulé depuis la dernière frame, et l'utilise pour calculer le mouvement des entités.

Recherchez sur Internet comment obtenir le temps courant en C++ et arrangez-vous pour calculer le dt (delta time) qui s'écoule entre deux frames.
Lorsque le programme tourne bien, celui-ci devrait être quasiment égale à 1/framerate.
Cependant, si le programme se met à ramer et que la callback de glutTimerFunc est appelée en retard (oui oui, c'est possible), alors votre dt devrait être supérieur à 1/framerate.

Passez ensuite cette valeur à la fonction `move` des `DynamicObject`, et utilisez-la pour calculer les nouvelles positions de chaque avion.
Vérifiez maintenant en exécutant le programme que, lorsque augmentez le framerate du programme, vous n'augmentez pas la vitesse de la simulation.

Ajoutez ensuite deux nouveaux inputs permettant d'accélérer ou de ralentir la simulation.

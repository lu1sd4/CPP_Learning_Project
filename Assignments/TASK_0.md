# Se familiariser avec l'existant

## A- Exécution

Compilez et lancez le programme.

Allez dans le fichier `tower_sim.cpp` et recherchez la fonction responsable de gérer les inputs du programme.
Sur quelle touche faut-il appuyer pour ajouter un avion ?

---
Il faut appuyer sur la touche 'c', comme nous pouvons voir dans la ligne suivante :

```cpp
GL::keystrokes.emplace('c', [this]() { create_random_aircraft(); });
```

---

Comment faire pour quitter le programme ?

---
Il faut appuyer sur la touche 'x' ou la touche 'q', comme nous pouvons constater dans ces lignes :

```cpp
GL::keystrokes.emplace('x', []() { GL::exit_loop(); });
GL::keystrokes.emplace('q', []() { GL::exit_loop(); });
```
---

A quoi sert la touche 'F' ?

---
Elle sert à appeler la fonction `toggle_fullscreen`, comme nous pouvons voir dans :

```cpp
GL::keystrokes.emplace('f', []() { GL::toggle_fullscreen(); });
```

Cette fonction a pour but de basculer le programme entre les modes *fullscreen* et *windowed* en utilisant des fonctions de la librairie Glut.

---

Ajoutez un avion à la simulation et attendez.
Que est le comportement de l'avion ?

---

l'avion fait un tour, ensuite il atterrit sur une des pistes de l'aéroport, ensuite il est desservi dans un terminal, et finalement il repart. 

---

Quelles informations s'affichent dans la console ?

---

Nous voyons les actions de l'avion (identifié par son numéro de vol) dans l'aéroport.

```
LH1778 is now landing...
now servicing DL2186...
done servicing DL2186
DL2186 lift off
```

---

Ajoutez maintenant quatre avions d'un coup dans la simulation.
Que fait chacun des avions ?

---

Trois des avions se dirigent vers l'aéroport et sont desservis normalement, tandis que l'autre avion circulent autour de l'aéroport jusqu'à qu'un des terminales soit libéré, pour ensuite atterrir et être desservi.

---

## B- Analyse du code

Listez les classes du programme à la racine du dossier src/.

---

Nous trouvons les types suivantes :

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

---

Pour chacune d'entre elle, expliquez ce qu'elle représente et son rôle dans le programme.

---

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

---

Pour les classes `Tower`, `Aircaft`, `Airport` et `Terminal`, listez leurs fonctions-membre publiques et expliquez précisément à quoi elles servent.

---

### Tower

Les fonctions-membre publiques sont :

```cpp
public:
  Tower(Airport& airport_) : airport { airport_ } {}
  // produce instructions for aircraft
  WaypointQueue get_instructions(Aircraft& aircraft);
  void arrived_at_terminal(const Aircraft& aircraft);
```

```cpp
Tower(Airport& airport_) : airport { airport_ } {}
```

C'est un constructeur de la classe 'Tower'

```cpp
WaypointQueue get_instructions(Aircraft& aircraft);
```

Sert à genérer une liste de points représentant le chemin qu'un avion doit suivre

```cpp
void arrived_at_terminal(const Aircraft& aircraft);
```

Sert à démarrer le service d'un avion

### Aircraft

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

```cpp
Aircraft(const AircraftType& type_,
         const std::string_view& flight_number_,
         const Point3D& pos_,
         const Point3D& speed_,
         Tower& control_)
```

C'est un constructeur de la classe `Aircraft`

```cpp
const std::string& get_flight_num() const { return flight_number; }
```

Sert à renvoyer le numéro du vol de l'avion

```cpp
float distance_to(const Point3D& p) const { return pos.distance_to(p); }
```

Renvoie la distance entre l'avion et un point donné

```cpp
void display() const override;
```

Dessine l'avion autour du point ou il est placé. C'est un `override` de la fonction `display` de la classe `GL::Displayable` dont `aircraft` hérite.

```cpp
void move() override;
```

Sert à "avancer" l'avion dans la simulation, en mettant à jour les attributs de l'objet selon son état actuel : position, liste de waypoints... 

### Airport

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

```cpp
Airport(const AirportType& type_,
        const Point3D& pos_,
        const img::Image* image,
        const float z_ = 1.0f)
```

C'est le constructeur de la classe Airport.

```cpp
Tower& get_tower() { return tower; }
```

Renvoie la tour de contrôle de l'aéroport

```cpp
void display() const override { texture.draw(project_2D(pos), { 2.0f, 2.0f }); }
```

Sert à déssiner l'aéroport autour du point où il est placé. C'est un `override` de la fonction `display` de la classe `GL::Displayable` dont `airport` hérite.

```cpp
void move() override
    {
        for (auto& t : terminals)
        {
            t.move();
        }
    }
```

"Avance" les terminaux de l'aéroport dans la simulation.

### Terminal

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

```cpp
Terminal(const Point3D& pos_) : pos { pos_ } {}
```

C'est le constructeur de la classe `Terminal`

```cpp
bool in_use() const { return current_aircraft != nullptr; }
```

Renvoie *vrai* si le terminal est occupé par un avion et *faux* sinon. 

```cpp
bool is_servicing() const { return service_progress < SERVICE_CYCLES; }
```

Renvoie *vrai* si le terminal est en train de desservir un avion et *faux* si le cycle de service est terminé.

```cpp
void assign_craft(const Aircraft& aircraft) { current_aircraft = &aircraft; }
```

C'est un *setter* pour l'attribut `current_aircraft`

```cpp
void start_service(const Aircraft& aircraft)
```

Commence le service d'un avion en remettant `service_progress` à 0. `is_servicing` renverra donc *vrai* si on l'appelle ultérieurement, jusqu'à que le service soit fini.

```cpp
void finish_service();
```

Remet l'avion à `nullptr`, ce qui signale la fin du service en cours. `in_use()` renverra *faux* jusqu'à qu'un avion soit affecté au terminal avec `assign_craft`.
- avance le service d'un avion

---

Quelles classes et fonctions sont impliquées dans la génération du chemin d'un avion ?

---

L'avion stocke les waypoints qui représentent le chemin à suivre dans une queue. Il demande à la tour de contrôle de lui fournir ce chemin. 

```cpp
WaypointQueue Tower::get_instructions(Aircraft& aircraft);
```

L'avion fait un appel à cette fonction pour récupérer un chemin une fois que sa queue de `waypoints` est vide et il n'a donc plus d'instructions à suivre. Le choix de la nature du chemin à suivre est fait ici, selon la position de l'avion et l'état des terminaux.

```cpp
WaypointQueue Tower::get_circle() const;
```

Si l'avion n'est pas dans un terminal et s'il est trop loin ou s'il n'y a pas de terminal disponible, un chemin fixe représentant un cercle autour de l'aéroport est fourni.

```cpp
std::pair<WaypointQueue, size_t> Tower::reserve_terminal(Aircraft& aircraft);
```

Si l'avion n'est pas dans un terminal, la tour de contrôle essaie de lui en reserver un. Si cela est possible, un chemin vers le terminal est fourni.

```cpp
WaypointQueue AirportType::air_to_terminal(const Point3D& offset,
                              const size_t runway_num,
                              const size_t terminal_num)
```

Cette fonction fournit un chemin vers un terminal. Elle est appelée depuis la fonction `reserve_terminal` décrite précédemment, seulement si le terminal était disponible et a pu être reservé.

```cpp
WaypointQueue Airport::start_path(const size_t terminal_number);
```

Si l'avion est dans un terminal et le service est terminé, cette fonction est appelée pour donner à l'avion un chemin de départ. 

```cpp
WaypointQueue AirportType::terminal_to_air(const Point3D& offset,
                              const size_t runway_num,
                              const size_t terminal_num) const;
```

Renvoie un chemin vers l'air depuis un terminal donné. Cette fonction est appelée seulement si l'avion est dans un terminal et a déjà été desservi.

---

Quel conteneur de la librairie standard a été choisi pour représenter le chemin ?

---

C'est une `std::deque`, comme l'on peut constater dans cette ligne :

```cpp
using WaypointQueue = std::deque<Waypoint>;
```

Dans le fichier `waypoint.hpp`. 

---

Expliquez les intérêts de ce choix.

---

Une `std::deque` permet d'insérer et de rétirer des éléments de ses extremités efficacement.

Ces deux opérations sont celles dont on a besoin pour le chemin.

Lorsque l'on donne des points à l'avion, on les insère à la fin de la queue, et lorsque l'avion a besoin de connaitre le point vers lequel il devrait se diriger, il peut être tiré du debut de la queue et retiré une fois qu'il est atteint.

---

## C- Bidouillons !

1) Déterminez à quel endroit du code sont définies les vitesses maximales et accélération de chaque avion.

---

Dans `aircraft_types.hpp` ligne 30

```cpp
inline void init_aircraft_types()
{
    aircraft_types[0] = new AircraftType { .02f, .05f, .02f, MediaPath { "l1011_48px.png" } };
    aircraft_types[1] = new AircraftType { .02f, .05f, .02f, MediaPath { "b707_jat.png" } };
    aircraft_types[2] = new AircraftType { .02f, .05f, .02f, MediaPath { "concorde_af.png" } };
}
```

---

Le Concorde est censé pouvoir voler plus vite que les autres avions.
Modifiez le programme pour tenir compte de cela.

---

```cpp
inline void init_aircraft_types()
{
    aircraft_types[0] = new AircraftType { .02f, .05f, .02f, MediaPath { "l1011_48px.png" } };
    aircraft_types[1] = new AircraftType { .02f, .05f, .02f, MediaPath { "b707_jat.png" } };
    aircraft_types[2] = new AircraftType { .04f, .1f, .04f, MediaPath { "concorde_af.png" } };
}
```

---

2) Identifiez quelle variable contrôle le framerate de la simulation.

---

Cela se trouve dans `GL/opengl_interface.hpp` ligne 21 :

```cpp
inline unsigned int ticks_per_sec = DEFAULT_TICKS_PER_SEC;
```

---

Ajoutez deux nouveaux inputs au programme permettant d'augmenter ou de diminuer cette valeur.

---

Dans `opengl_interface.cpp`, on ajoute deux fonctions :

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

Et dans `tower_sim.cpp`, on ajoute deux touches :

```cpp
GL::keystrokes.emplace('>', []() { GL::increase_fps(); });
GL::keystrokes.emplace('<', []() { GL::decrease_fps(); });
```

---

Essayez maintenant de mettre en pause le programme en manipulant ce framerate. Que se passe-t-il ?\

---

Le programme s'arrête avec une erreur.

---

Ajoutez une nouvelle fonctionnalité au programme pour mettre le programme en pause, et qui ne passe pas par le framerate.

---


On introduit la variable booleene `GL::running`, *true* par défaut.

```cpp
inline bool running               = true;
```

On fait en sorte que la simulation n'avance que si `running` est *true*.  Ceci peut être fait dans `GL::timer`

```cpp
void timer(const int step)
{
    if (running)
    {
        // ...
    }
    glutPostRedisplay();
    glutTimerFunc(1000u / ticks_per_sec, timer, step + 1);
}
```


On ajoute également la fonction `GL::toggle_pause` qui bascule la variable `running` entre *true* et *false* 

```cpp
void GL::toggle_pause()
{
    running = !running;
}
```

Finalement, on ajoute la touche `p` comme suit :

```cpp
GL::keystrokes.emplace('p', []() { GL::toggle_pause(); });
```

---

3) Identifiez quelle variable contrôle le temps de débarquement des avions et doublez-le.

---

Il s'agit de la variable `SERVICE_CYCLES`, dans `config.hpp` (si j'ai bien compris le sens de "débarquement")

```cpp
// number of cycles needed to service an aircraft at a terminal
constexpr unsigned int SERVICE_CYCLES = 20u;
```

---

4) Lorsqu'un avion a décollé, il réatterrit peu de temps après. Faites en sorte qu'à la place, il soit retiré du programme.

Indices :
- A quel endroit pouvez-vous savoir que l'avion doit être supprimé ?

---

Dans `src/aircraft.cpp` quand la fonction `Aircraft::update` vérifie si l'avion n'a pas de *waypoints*, si l'avion a déjà été desservi, on peut le retirer de l'application.

---

- Pourquoi n'est-il pas sûr de procéder au retrait de l'avion dans cette fonction ?

---

Parce que `update()` est appelée dans une boucle qui parcourt les éléments dynamiques de la simulation.

---

- A quel endroit de la callstack pourriez-vous le faire à la place ?

---

Dans la boucle `timer` où on parcourt les `DynamicObject`.

---

- Que devez-vous modifier pour transmettre l'information de la première à la seconde fonction ?

---

Nous pouvons faire en sorte que la fonction `update` renvoie un booléen, qui indique si l'objet doit être supprimé, mettre à jour les `DynamicObject` de façon à ce que `update` renvoie `true`.

Ensuite, dans `Aircraft` nous pouvons ajouter le flag `has_been_serviced`, qui va être mis à jour une fois l'avion a été desservi, et qui va être vérifié dans `Aircraft::update`.

Si l'avion n'a pas de waypoints et qu'il a déjà été desservi, `Aircraft::update` renvoie false.

Pour tous les autres `DynamicObject`, comme nous ne voulons pas les supprimer, la fonction `update` devrait renvoyer *false* dans tous les cas.  

---

5) Lorsqu'un objet de type `Displayable` est créé, il faut ajouter celui-ci manuellement dans la liste des objets à afficher.
Il faut également penser à le supprimer de cette liste avant de le détruire.
Faites en sorte que l'ajout et la suppression de `display_queue` soit "automatiquement gérée" lorsqu'un `Displayable` est créé ou détruit.
Pourquoi n'est-il pas spécialement pertinent d'en faire de même pour `DynamicObject` ?

---

Nous pouvons faire en sorte que les displayable soient ajoutés et supprimés de la queue au moment de leur création et de leur suppression en rendant `display_queue` un membre statique publique de la classe `Displayable`.

Ensuite, nous pouvons changer le constructeur comme suit :

```cpp
Displayable(const float z_) : z { z_ } {
    display_queue.emplace_back(this);
}
```

Et supprimer l'instruction

```cpp
GL::display_queue.emplace_back(aircraft);
```

de `void TowerSimulation::create_random_aircraft() const;`

---

6) La tour de contrôle a besoin de stocker pour tout `Aircraft` le `Terminal` qui lui est actuellement attribué, afin de pouvoir le libérer une fois que l'avion décolle.
Cette information est actuellement enregistrée dans un `std::vector<std::pair<const Aircraft*, size_t>>` (size_t représentant l'indice du terminal).
Cela fait que la recherche du terminal associé à un avion est réalisée en temps linéaire, par rapport au nombre total de terminaux.
Cela n'est pas grave tant que ce nombre est petit, mais pour préparer l'avenir, on aimerait bien remplacer le vector par un conteneur qui garantira des opérations efficaces, même s'il y a beaucoup de terminaux.
Modifiez le code afin d'utiliser un conteneur STL plus adapté. Normalement, à la fin, la fonction `find_craft_and_terminal(const Aicraft&)` ne devrait plus être nécessaire.

---

On peut utiliser `std::map<const Aircraft*, size_t>` qui stocke pour un avion, le terminal qui lui est attribué.

---

## D- Théorie

1) Comment a-t-on fait pour que seule la classe `Tower` puisse réserver un terminal de l'aéroport ?

---

On a déclaré la classe `Tower` comme *friend* class dans `Airport` :

```cpp
friend class Tower;
```

---

2) En regardant le contenu de la fonction `void Aircraft::turn(Point3D direction)`, pourquoi selon-vous ne sommes-nous pas passés par une référence ?
Pensez-vous qu'il soit possible d'éviter la copie du `Point3D` passé en paramètre ?

---

Nous voulons une copie du point dans la fonction parce que la `Point3D::cap_length` fait des changements sur l'objet que l'on ne souhaite pas garder dans le `Point` passé en paramètre.

Non, sauf si on veut garder le changement effectué par `Point3D::cap_length` ou si l'on modifie cette fonction de façon a ce qu'elle renvoie un nouveau `Point3D` sans modifier `this`.

---

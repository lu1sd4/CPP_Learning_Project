# Algorithmes

## Objectif 1 - Refactorisation de l'existant

### A - Structured Bindings

`TowerSimulation::display_help()` est chargé de l'affichage des touches disponibles.
Dans sa boucle, remplacez `const auto& ks_pair` par un structured binding adapté.

---

```cpp
for (const auto& [ key, action ] : GL::keystrokes)
{
    std::cout << key << ' ';
}
```

---

### B - Algorithmes divers

1. `AircraftManager::move()` (ou bien `update()`) supprime les avions de la `move_queue` dès qu'ils sont "hors jeux".
En pratique, il y a des opportunités pour des pièges ici. Pour les éviter, `<algorithm>` met à disposition la fonction `std::remove_if`.
Remplacez votre boucle avec un appel à `std::remove_if`.

**Attention**: pour cela c'est necessaire que `AircraftManager` stocke les avion dans un `std::vector` ou `std::list` (c'est déjà le cas pour la solution filé).

---

Voici `bool AircraftManager::update()` après les changements.

```cpp
bool AircraftManager::update() {
    aircrafts.erase(
        std::remove_if(
            aircrafts.begin(),
            aircrafts.end(),
            [](auto& a) { return !a->update(); }),
    aircrafts.end());
    return true;
}
```

---

2. Pour des raisons de statistiques, on aimerait bien être capable de compter tous les avions de chaque airline.
A cette fin, rajoutez des callbacks sur les touches `0`..`7` de manière à ce que le nombre d'avions appartenant à `airlines[x]` soit affiché en appuyant sur `x`.
Rendez-vous compte de quelle classe peut acquérir cet information. Utilisez la bonne fonction de `<algorithm>` pour obtenir le résultat.

---

On ajoute une fonction-membre publique dans `AircraftManager` :

```cpp
int AircraftManager::count_in_airline(const std::string& airline) const
{
    return std::count_if(aircrafts.begin(), aircrafts.end(), [airline](const auto& a) {
         return a->get_flight_num().substr(0,2) == airline;
    });
}
```

Et également dans `TowerSimulation` :

```cpp
void TowerSimulation::display_count_airline(const int& airline_index) const
{
    const auto airline_name = aircraft_factory->airlines[airline_index];
    std::cout << aircraft_manager->count_in_airline(airline_name) << " aircrafts in " << airline_name << std::endl;
}
```

Et finalement, pour les nouvelles touches :

```cpp
GL::keystrokes.emplace('m', [this]() { display_crashed_planes(); });
for (int i = 0; i <= 7; ++i)
{
    GL::keystrokes.emplace('0'+i, [this, i]() { display_count_airline(i); });
}
```

---

### C - Relooking de Point3D

La classe `Point3D` présente beaucoup d'opportunités d'appliquer des algorithmes.
Particulairement, des formulations de type `x() = ...; y() = ...; z() = ...;` se remplacent par un seul appel à la bonne fonction de la librairie standard.
Remplacez le tableau `Point3D::values` par un `std::array` et puis,
remplacez le code des fonctions suivantes en utilisant des fonctions de `<algorithm>` / `<numeric>`:

1. `Point3D::operator*=(const float scalar)`
2. `Point3D::operator+=(const Point3D& other)` et `Point3D::operator-=(const Point3D& other)`
3. `Point3D::length() const`

---

Operator `*=` :

```cpp
Point3D& operator*=(const float scalar)
{
    std::transform (values.begin(), values.end(), values.begin(), [&scalar](const float& coord) { return coord * scalar; });
    return *this;
}
```

Operator `+=` et `-=` :

```cpp
Point3D& operator+=(const Point3D& other)
{
    std::transform (values.begin(), values.end(), other.values.begin(), values.begin(), std::plus<>());
    return *this;
}

Point3D& operator-=(const Point3D& other)
{
    std::transform (values.begin(), values.end(), other.values.begin(), values.begin(), std::minus<>());
    return *this;
}
```

`float Point3D::length() const` :

```cpp
float length() const {
    return std::sqrt(std::accumulate(values.begin(), values.end(), 0., [](const auto& acc, const auto& current) { return acc + current * current; }));
}
```

## Objectif 2 - Rupture de kérosène

Vous allez introduire la gestion de l'essence dans votre simulation.\
Comme le but de ce TP est de vous apprendre à manipuler les algorithmes de la STL, avant d'écrire une boucle, demandez-vous du coup s'il n'existe pas une fonction d'`<algorithm>` ou de `<numeric>` qui permet de faire la même chose.

La notation tiendra compte de votre utilisation judicieuse de la librairie standard. 

### A - Consommation d'essence

Ajoutez un attribut `fuel` à `Aircraft`, et initialisez-le à la création de chaque avion avec une valeur aléatoire comprise entre `150` et `3'000`.\
Décrémentez cette valeur dans `Aircraft::update` si l'avion est en vol.\
Lorsque cette valeur atteint 0, affichez un message dans la console pour indiquer le crash, et faites en sorte que l'avion soit supprimé du manager.

N'hésitez pas à adapter la borne `150` - `3'000`, de manière que des avions se crashent de temps en temps.

---

Dans `aircraft.hpp` :

```cpp
#include <random>
```

Dans le constructeur, pour les nombres aléatoires :

```cpp
std::random_device rd;
std::uniform_int_distribution<int> dist(150, 3000);
fuel = dist(rd);
```

Dans `Aircraft::update` :

```cpp
fuel--;
if (fuel <= 0) {
    std::cout << flight_number << " out of fuel" << std::endl;
    return false;
}
```

---

### B - Un terminal s'il vous plaît

Afin de minimiser les crashs, il va falloir changer la stratégie d'assignation des terminaux aux avions.
Actuellement, chaque avion interroge la tour de contrôle pour réserver un terminal dès qu'il atteint son dernier `Waypoint`.
Si un terminal est libre, la tour lui donne le chemin pour l'atteindre, sinon, elle lui demande de tourner autour de l'aéroport.
Pour pouvoir prioriser les avions avec moins d'essence, il faudrait déjà que les avions tentent de réserver un terminal tant qu'ils n'en n'ont pas (au lieu de ne demander que lorsqu'ils ont terminé leur petit tour).

1. Introduisez une fonction `bool Aircraft::has_terminal() const` qui indique si un terminal a déjà été réservé pour l'avion (vous pouvez vous servir du type de `waypoints.back()`).

---

```cpp
bool Aircraft::has_terminal() const
{
    return !waypoints.empty() && waypoints.back().is_at_terminal();
}
```

---

2. Ajoutez une fonction `bool Aircraft::is_circling() const` qui indique si l'avion attend qu'on lui assigne un terminal pour pouvoir attérir.

---

```cpp
bool Aircraft::is_circling() const
{
    return !waypoints.empty() && !waypoints.front().is_on_ground() && !has_been_serviced;
}
```

---

3. Introduisez une fonction `WaypointQueue Tower::reserve_terminal(Aircraft& aircraft)` qui essaye de réserver un `Terminal`. Si c'est possible, alors elle retourne un chemin vers ce `Terminal`, et un chemin vide autrement (vous pouvez vous inspirer / réutiliser le code de `Tower::get_instructions`).

---

Dans `Tower` :

```cpp
WaypointQueue Tower::reserve_terminal(Aircraft& aircraft)
{
    const auto vp = airport.reserve_terminal(aircraft);
    if (!vp.first.empty())
    {
        reserved_terminals[&aircraft] = vp.second;
        return vp.first;
    }
    else
    {
        return {};
    }
}
```

Et dans `Aircraft::update` :

```cpp
if (waypoints.empty()) {
    if (has_been_serviced)
    {
        return false;
    } else
    {
        waypoints = control.get_instructions(*this);
    }
}
if (!has_terminal() && is_circling())
{
    auto path_to_terminal = control.reserve_terminal(*this);
    if (path_to_terminal.empty())
    {
        waypoints = control.get_instructions(*this);
    }
}
```

---

4. Modifiez la fonction `move()` (ou bien `update()`) de `Aircraft` afin qu'elle appelle `Tower::reserve_terminal` si l'avion est en attente. Si vous ne voyez pas comment faire, vous pouvez essayer d'implémenter ces instructions :\
\- si l'avion a terminé son service et sa course, alors on le supprime de l'aéroport (comme avant),\
\- si l'avion attend qu'on lui assigne un terminal, on appelle `Tower::reserve_terminal` et on modifie ses `waypoints` si le terminal a effectivement pu être réservé,\
\- si l'avion a terminé sa course actuelle, on appelle `Tower::get_instructions` (comme avant).

---

Dans `Aircraft::update` :

```cpp
if (!has_terminal() && is_circling())
{
    auto path_to_terminal = control.reserve_terminal(*this);
    if (!path_to_terminal.empty())
    {
        waypoints.clear();
        std::copy(path_to_terminal.begin(), path_to_terminal.end(), std::back_inserter(waypoints));
    }
}
```

---

### C - Minimiser les crashs

Grâce au changement précédent, dès lors qu'un terminal est libéré, il sera réservé lors du premier appel à `Aircraft::update` d'un avion recherchant un terminal.
Pour vous assurez que les terminaux seront réservés par les avions avec le moins d'essence, vous allez donc réordonner la liste des `aircrafts` avant de les mettre à jour.

Vous devrez placer au début de la liste les avions qui ont déjà réservé un terminal.\
Ainsi, ils pourront libérer leurs terminaux avant que vous mettiez à jour les avions qui essayeront de les réserver.

La suite de la liste sera ordonnée selon le niveau d'essence respectif de chaque avion.

Par exemple :
```b
A - Reserved / Fuel: 100
B - NotReserved / Fuel: 50
C - NotReserved / Fuel: 300
D - NotReserved / Fuel: 150
E - Reserved / Fuel: 2500
```
pourra être réordonné en
```b
A - Reserved / Fuel: 100
E - Reserved / Fuel: 2500
B - NotReserved / Fuel: 50
D - NotReserved / Fuel: 150
C - NotReserved / Fuel: 300
```

Assurez-vous déjà que le conteneur `AircraftManager::aircrafts` soit ordonnable (`vector`, `list`, etc).\
Au début de la fonction `AircraftManager::move` (ou `update`), ajoutez les instructions permettant de réordonner les `aircrafts` dans l'ordre défini ci-dessus.

----

Pour le comparateur, je regarde d'abord si l'avion a un terminal et ensuite je compare les valeurs de `fuel` des deux avions.

```cpp
bool AircraftManager::update() {
    std::sort(
        aircrafts.begin(),
        aircrafts.end(),
        [](const std::unique_ptr<Aircraft>& a1, const std::unique_ptr<Aircraft>& a2)
        {
            if (a1->has_terminal()) return false; // if a1 has terminal it's not ahead of a2
            return a1->fuel < a2->fuel; // if a1 has less fuel than a2 it's ahead of a2
        }
    );
    ...
```

---

### D - Réapprovisionnement 

Afin de pouvoir repartir en toute sécurité, les avions avec moins de `200` unités d'essence doivent être réapprovisionnés par l'aéroport pendant qu'ils sont au terminal.

1. Ajoutez une fonction `bool Aircraft::is_low_on_fuel() const`, qui renvoie `true` si l'avion dispose de moins de `200` unités d'essence.\
Modifiez le code de `Terminal` afin que les avions qui n'ont pas suffisamment d'essence restent bloqués.\
Testez votre programme pour vérifier que certains avions attendent bien indéfiniment au terminal.
Si ce n'est pas le cas, essayez de faire varier la constante `200`.

---

Voici ma fonction `Aircraft::is_low_on_fuel` :

```cpp
bool is_low_on_fuel() const
{
    return fuel < 200;
}
```

Et un appel à `Aircraft::is_low_on_fuel` dans `Terminal::update`

```cpp
bool update() override
{
    if (in_use() && is_servicing() && !current_aircraft->is_low_on_fuel())
    {
        ++service_progress;
    }
    return true;
}
```

---

2. Dans `AircraftManager`, implémentez une fonction `get_required_fuel`, qui renvoie la somme de l'essence manquante (le plein, soit `3'000`, moins la quantité courante d'essence) pour les avions vérifiant les conditions suivantes :\
\- l'avion est bientôt à court d'essence\
\- l'avion n'est pas déjà reparti de l'aéroport.

---

```cpp
int AircraftManager::get_required_fuel() const
{
    return std::accumulate(
        aircrafts.begin(),
        aircrafts.end(),
        0,
        [](int sum, const std::unique_ptr<Aircraft>& aircraft) {
            if (aircraft->is_low_on_fuel() && !aircraft->has_been_serviced) {
                return sum + (3000 - aircraft->fuel);
            }
            return sum;
        }
   );
}
```

---

3. Ajoutez deux attributs `fuel_stock` et `ordered_fuel` dans la classe `Airport`, que vous initialiserez à 0.\
Ajoutez également un attribut `next_refill_time`, aussi initialisé à 0.\
Enfin, faites en sorte que la classe `Airport` ait accès à votre `AircraftManager` de manière à pouvoir l'interroger.

---

```cpp
AircraftManager *manager;
int fuel_stock;
int ordered_fuel;
int next_refill_time;
```

```cpp
manager { _manager },
fuel_stock { 0 },
ordered_fuel { 0 },
next_refill_time { 0 }
```

---

4. Ajoutez une fonction `refill` à la classe `Aircraft`, prenant un paramètre `fuel_stock` par référence non-constante.
Cette fonction rempliera le réservoir de l'avion en soustrayant ce dont il a besoin de `fuel_stock`.
Bien entendu, `fuel_stock` ne peut pas devenir négatif.\
Indiquez dans la console quel avion a été réapprovisionné ainsi que la quantité d'essence utilisée.

---

```cpp
void refill(int& fuel_stock)
{
    int to_refill = std::min(fuel_stock, missing_fuel());
    fuel_stock -= to_refill;
    fuel += to_refill;
    std::cout << "refilled " << get_flight_num() << " with " << to_refill << " fuel units" << std::endl;
}
```

---

5. Définissez maintenant une fonction `refill_aircraft_if_needed` dans la classe `Terminal`, prenant un paramètre `fuel_stock` par référence non-constante.
Elle devra appeler la fonction `refill` sur l'avion actuellement au terminal, si celui-ci a vraiment besoin d'essence.  

---

```cpp
void refill_aircraft_if_needed(int& fuel_stock)
{
    if (in_use() && current_aircraft->is_low_on_fuel()) {
        current_aircraft->refill(fuel_stock);
    }
}
```

---

6. Modifiez la fonction `Airport::update`, afin de mettre-en-oeuvre les étapes suivantes.\
\- Si `next_refill_time` vaut 0 :\
    \* `fuel_stock` est incrémenté de la valeur de `ordered_fuel`.\
    \* `ordered_fuel` est recalculé en utilisant le minimum entre `AircraftManager::get_required_fuel()` et `5'000` (il s'agit du volume du camion citerne qui livre le kérosène).\
    \* `next_refill_time` est réinitialisé à `100`.\
    \* La quantité d'essence reçue, la quantité d'essence en stock et la nouvelle quantité d'essence commandée sont affichées dans la console.\
\- Sinon `next_refill_time` est décrémenté.\
\- Chaque terminal réapprovisionne son avion s'il doit l'être.

---

```cpp
bool update() override
{
    if (next_refill_time == 0) {
        fuel_stock += ordered_fuel;
        std::cout << "fuel: restock " << ordered_fuel << " - now in stock " << fuel_stock;
        ordered_fuel = std::min(manager->get_required_fuel(), 5000);
        std::cout << " - order " << ordered_fuel << std::endl;
        next_refill_time = 100;
    } else {
        next_refill_time--;
    }
    for (auto& t : terminals) {
        t.refill_aircraft_if_needed(fuel_stock);
        t.update();
    }
    return true;
}
```

---

### E - Déréservation

Si vous avez suffisamment testé votre programme, vous avez dû vous apercevoir que parfois, certains terminaux arrêtaient d'être réservés et utilisés.\
En effet, lorsque les avions se crashent alors qu'ils avaient un terminal de réservé, rien n'a été fait pour s'assurer que le terminal allait de nouveau être libre.

Pour garantir cela, vous allez modifier le destructeur de `Aircraft`. Si l'avion a réservé un terminal, assurez-vous que celui-ci est correctement libéré. Pour cela, vous aurez besoin de rajouter une fonction dans la classe `Tower`. Choisissez-lui un nom qui décrit correctement ce qu'elle fait.

---

On ajoute la fonction `Terminal::release`

```cpp
void release()
{
    current_aircraft = nullptr;
}
```

Et dans `Tower` :

```cpp
void Tower::release_terminal(Aircraft& aircraft)
{
    auto terminal_num = reserved_terminals.find(&aircraft);
    if (terminal_num != reserved_terminals.end()) {
        Terminal& terminal = airport.get_terminal(terminal_num->second);
        terminal.release();
        reserved_terminals.erase(&aircraft);
    }
}
```

Et finalement dans le destructeur de `Aircraft` :

```cpp
~Aircraft()
{
    if (has_terminal()) {
        control.release_terminal(*this);
    }
}
```

---

# Assertions et exceptions

## Objectif 1 - Crash des avions

Actuellement, quand un avion s'écrase, une exception de type `AircraftCrash` (qui est un alias de `std::runtime_error` déclaré dans `config.hpp`) est lancée.

1. Faites en sorte que le programme puisse continuer de s'exécuter après le crash d'un avion. Pour cela, remontez l'erreur jusqu'à un endroit approprié pour procéder à la suppression de cet avion (assurez-vous bien que plus personne ne référence l'avion une fois l'exception traitée). Vous afficherez également le message d'erreur de l'exception dans `cerr`.

---

Dans le parcours des avions dans `AircraftManager::update`, en cas de `AircraftCrash` on peut supprimer l'avion comme suit :

```cpp
[](auto& a) {
    try {
        bool result = a->update();
        return !result;
    } catch (const AircraftCrash &e) {
        std::cerr << e.what() << std::endl;
        return true;
    }
}),
```

---

2. Introduisez un compteur qui est incrémenté chaque fois qu'un avion s'écrase. Choisissez une touche du clavier qui n'a pas encore été utilisée (`m` par exemple ?) et affichez ce nombre dans la console lorsque l'utilisateur appuie dessus.

---

Dans le même bout de code de la question précédente, j'incrémente le compteur :

```cpp
    aircrafts.erase(
        std::remove_if(
            aircrafts.begin(),
            aircrafts.end(),
            [this](auto& a) {
                try {
                    bool result = a->update();
                    return !result;
                } catch (const AircraftCrash &e) {
                    std::cerr << e.what() << std::endl;
                    crashed_planes++;
                    return true;
                }
            }),
    aircrafts.end());
```

Et dans `TowerSimulation` :

```cpp
void TowerSimulation::display_crashed_planes() const
{
    std::cout << aircraft_manager->get_crashed_planes() << " aircrafts have crashed" << std::endl;
}

int AircraftManager::get_crashed_planes() const
{
    return crashed_planes;
}
```

Et finalement, pour la touche :

```cpp
GL::keystrokes.emplace('m', [this]() { display_crashed_planes(); });
```

---

3. Si vous avez fini d'implémenter la gestion du kérosène (Task_2 - Objectif 2 - A), lancez une exception de type `AircraftCrash` lorsqu'un avion tombe à court d'essence. Normalement, cette exception devrait être traitée de la même manière que lorsqu'un avion s'écrase parce qu'il a atterri trop vite.

---

Dans `Aircraft::update` :

```cpp
if (!is_at_terminal)
{
    fuel--;
    if (fuel <= 0) {
        using namespace std::string_literals;
        throw AircraftCrash { flight_number + " out of fuel"s };
    }
```

---

## Objectif 2 - Détecter les erreurs de programmation

Pour sécuriser votre code, repassez sur les différentes fonctions de votre programme et ajoutez des assertions permettant de vérifier qu'elles sont correctement utilisées.
Voici quelques idées :
- fonctions d'initialisation appelées une seule fois
- état attendu d'un objet lorsqu'une fonction est appelée dessus
- vérification de certains paramètres de fonctions
- ...

---

```cpp
std::pair<WaypointQueue, size_t> reserve_terminal(Aircraft& aircraft)
{
    assert(!aircraft.has_terminal());
```

```cpp
// when we arrive at a terminal, signal the tower
void Aircraft::arrive_at_terminal()
{
    assert(!is_at_terminal);
    // we arrived at a terminal, so start servicing
    control.arrived_at_terminal(*this);
    is_at_terminal = true;
}
```

Des assertions sur les pointeurs...

---

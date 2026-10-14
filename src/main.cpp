#include <iostream>

#include "entity_manager.h"
struct Position {
    float x = 0;
    float y = 0;
};

struct Health {
    int hp = 100;
};

int main() {
    Registry<Health> healths;
    //
    size_t a = healths.store(Health{5});  
    Health h{20};
    healths.store(h);
    healths.remove(a);
    size_t c = healths.store(Health{15});
    std::cout << "size: " << healths.size() << "\n";
    std::cout << "valid(999): " << healths.valid(999) << "\n";

    EntityManager<Position, Health> em;
    EntityID e0 = em.createEntity();
    em.addComponent(e0, Position{1, 2});  
    em.addComponent(e0, h);               
    std::cout << "entity0 has Position: " << em.hasComponent<Position>(e0) << "\n";
    std::cout << "validEntity(42): " << em.validEntity(42) << "\n";

    em.getComponent<Health>(e0).hp -= 5;  
    std::cout << "e0 hp: " << em.getComponent<Health>(e0).hp << "\n";
    return 0;
}

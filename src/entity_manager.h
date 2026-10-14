#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <cstddef>  // size_t

#include "pack.h"  // Pack from prep
#include <array>
#include <cassert>
#include <cstddef>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include "registry.h"

using EntityID = size_t;

template <typename... ComponentTypes>
class EntityManager {
    public:
        [[nodiscard]] EntityID createEntity();

        bool validEntity(EntityID id) const;

        template <typename ComponentType>
        bool hasComponent(EntityID id) const;

        template <typename ComponentType>
        ComponentType& getComponent(EntityID id);

        template <typename ComponentType>
        void addComponent(EntityID id, ComponentType&& component);
        
    private:
        using Components = Pack<ComponentTypes...>;
        static constexpr std::size_t INVALID_INDEX = std::numeric_limits<std::size_t>::max();
        static constexpr std::size_t NUM_COMPONENTS = sizeof...(ComponentTypes);
        std::vector<std::array<std::size_t, NUM_COMPONENTS>> mComponentIndices;
        std::vector<bool> mEntityValid;
        std::vector<EntityID> mFreeEntities;
        std::tuple<Registry<ComponentTypes>...> mRegistries;

        template <typename ComponentType>
        static constexpr std::size_t indexOf() {
            constexpr int slot = Components::template index<ComponentType>;
            static_assert(slot != -1, "ComponentType is not in EM");
            return static_cast<std::size_t>(slot);
        }

        template <typename ComponentType>
        Registry<ComponentType>& registry() {
            return std::get<Registry<ComponentType>>(mRegistries);
        }

};

template <typename... ComponentTypes>
EntityID EntityManager<ComponentTypes...>::createEntity() {
    EntityID id;
    if (!mFreeEntities.empty()) {
        id = mFreeEntities.back();
        mFreeEntities.pop_back();
        mEntityValid[id] = true;
    } else {
        id = mComponentIndices.size();
        mComponentIndices.emplace_back();
        mEntityValid.push_back(true);
    }
    mComponentIndices[id].fill(INVALID_INDEX);  // a new (or recycled) entity has no components
    return id;
}

template <typename... ComponentTypes>
bool EntityManager<ComponentTypes...>::validEntity(EntityID id) const {
    return id < mEntityValid.size() && mEntityValid[id];
}

template <typename... ComponentTypes>
template <typename ComponentType>
bool EntityManager<ComponentTypes...>::hasComponent(EntityID id) const{
    if (!validEntity(id)) {
        return false;
    }
    size_t column = indexOf<ComponentType>();
    size_t registryIndex = mComponentIndices[id][column];
    return registryIndex != INVALID_INDEX;
}

template <typename... ComponentTypes>
template <typename ComponentType>
ComponentType& EntityManager<ComponentTypes...>::getComponent(EntityID id) {
    assert(hasComponent<ComponentType>(id) && "No component type here.");
    size_t column = indexOf<ComponentType>();
    size_t registryIndex = mComponentIndices[id][column];
    Registry<ComponentType>& registry2 = registry<ComponentType>();
    return registry2[registryIndex];
}


template <typename... ComponentTypes>
template <typename ComponentType>
void EntityManager<ComponentTypes...>::addComponent(EntityID id, ComponentType&& component) {
    using BaseType = std::remove_cvref_t<ComponentType>;
    assert(validEntity(id) && "Invalid Entity");
    assert(!hasComponent<BaseType>(id) && "Entity Does not have specified type");
    size_t column = indexOf<BaseType>();
    Registry<BaseType>& registry2 = registry<BaseType>();
    size_t registryIndex = registry2.store(std::forward<ComponentType>(component));
    mComponentIndices[id][column] = registryIndex;
}
#endif  // ENTITY_MANAGER_H

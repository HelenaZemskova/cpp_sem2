// g++ -std=c++23 -Wall -Wextra -o 09_07 09_07.cpp && ./09_07

// Почему обычный Pimpl может быть медленнее:
//
//  1. На каждый объект нужен new/delete.
// Это лишняя работа аллокатора и дополнительные служебные данные в куче.
// 
//  2. Данные лежат отдельно от Entity. При обращении через указатель чаще бывают промахи кэша.
//
//  3. Меньше локальность данных.
// Entity в одном месте, Implementation в другом - процессору неудобнее.
//
// Что меняется при inline-хранилище:
//
//  1. Память выделяется один раз вместе с Entity. Отдельный new/delete для Implementation больше не нужен.
//
//  2. Entity и Implementation находятся рядом в памяти. Обычно это лучше для кэша и быстрее на доступе.
//
//  3. Минус: размер Entity фиксирован и всегда включает эти 16 байт.

#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <memory>
#include <new>
#include <print>
#include <utility>

class Entity
{
    class Implementation;

public :

    Entity();

    Entity(Entity && other);

    Entity(Entity const &)             = delete;

    auto & operator=(Entity const &)   = delete;

   ~Entity();

//  -----------------------------------------------------------------------

    auto & operator=(Entity && other);

//  -----------------------------------------------------------------------

    void test() const;

//  -----------------------------------------------------------------------

          Implementation * get();

    Implementation const * get() const;

private :

    alignas(std::max_align_t) std::array < std::byte, 16 > m_storage;
};

////////////////////////////////////////////////////////////////////////////////

class Entity::Implementation
{
public :

    Implementation() = default;

    Implementation(Implementation &&) = default;

//  ----------------------------

    void test() const
    {
        std::print("Entity::Implementation::test\n");
    }
};

////////////////////////////////////////////////////////////////////////////////

Entity::Entity()
{
    static_assert
    (
        sizeof(Implementation) <= 16,
        "Implementation does not fit into storage (16 bytes)"
    );

    static_assert
    (
        alignof(Implementation) <= alignof(std::max_align_t),
        "Implementation alignment exceeds storage alignment"
    );

    new (m_storage.data()) Implementation();
}

////////////////////////////////////////////////////////////////////////////////

Entity::Entity(Entity && other)
{
    new (m_storage.data()) Implementation(std::move(*other.get()));

    std::destroy_at(other.get());
    new (other.m_storage.data()) Implementation();
}

////////////////////////////////////////////////////////////////////////////////

Entity::~Entity()
{
    std::destroy_at(get());
}

////////////////////////////////////////////////////////////////////////////////

auto & Entity::operator=(Entity && other)
{
    if (this != &other)
    {
        std::destroy_at(get());

        new (m_storage.data()) Implementation(std::move(*other.get()));

        std::destroy_at(other.get());
        new (other.m_storage.data()) Implementation();
    }

    return *this;
}

////////////////////////////////////////////////////////////////////////////////

void Entity::test() const
{
    get()->test();
}

////////////////////////////////////////////////////////////////////////////////

Entity::Implementation * Entity::get()
{
    return std::launder(std::bit_cast < Implementation * > (m_storage.data()));
}

Entity::Implementation const * Entity::get() const
{
    return std::launder(std::bit_cast < Implementation const * > (m_storage.data()));
}

////////////////////////////////////////////////////////////////////////////////

int main()
{
    Entity entity_1;

    assert(entity_1.get() != nullptr);

    entity_1.test();

//  -----------------------------------------------------------------------

    Entity entity_2 = std::move(entity_1);

    assert(entity_2.get() != nullptr);
    assert(entity_1.get() != nullptr);  // перемещённый объект переинициализирован

    entity_2.test();

//  -----------------------------------------------------------------------

    entity_1 = std::move(entity_2);

    assert(entity_1.get() != entity_2.get());   // разные адреса хранилищ

    entity_1.test();
    std::print("All tests passed successfully.\n");
}

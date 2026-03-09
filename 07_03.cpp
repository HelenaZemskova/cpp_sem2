// void test(Person const & person)
// {
//     std::cout << "test: " << person.name() << '\n';
//
//     if (person.grade() == 10 || person.salary() > 1'000'000)
//     {
//         save(Status::success, person.id());
//     }
//     else
//     {
//         save(Status::failure, person.id());
//     }
// }


// Нормальное ветвление:
//   if / else — в зависимости от условия выполняется ветка success или failure
//   operator|| (short-circuit) — если grade() == 10, salary() не вычисляем

// Источники исключений:
//   A - person.name()          std::bad_alloc или пользовательское исключение
//   B - std::cout << ...       std::ios_base::failure при ошибке записи
//   C - person.grade()         пользовательское исключение
//   D - person.salary()        пользовательское исключение (условно, из-за short-circuit)
//   E - person.id()            пользовательское исключение (до вызова save)
//   F - save(Status::..., ...) пользовательское или I/O исключение


#include <iostream>
#include <stdexcept>
#include <string>


enum class Status { success, failure };


class Person
{
public:
    explicit Person(std::string name, int grade, long salary, int id)
        : m_name(std::move(name)), m_grade(grade), m_salary(salary), m_id(id)
    {}

    std::string name()   const { return m_name; }       // A
    int         grade()  const { return m_grade; }      // C
    long        salary() const { return m_salary; }     // D
    int         id()     const                          // E
    {
        if (m_id < 0)
            throw std::invalid_argument("Person::id() : id must not be negative");
        return m_id;
    }

private:
    std::string m_name;
    int         m_grade  = 0;
    long        m_salary = 0;
    int         m_id     = 0;
};


void save(Status status, int id) // F
{
    std::cout << "save: id=" << id
              << " status=" << (status == Status::success ? "success" : "failure")
              << '\n';
}


void test(Person const & person)
{
    std::cout << "test: " << person.name() << '\n'; // B

    if (person.grade() == 10 || person.salary() > 1'000'000) // C, D
    {
        save(Status::success, person.id()); // E, F
    }
    else
    {
        save(Status::failure, person.id()); // E, F
    }
}


int main()
{
    // ветка success по grade, salary() не вызывается (short-circuit)
    { Person alice("Alice", 10, 500, 1);    test(alice); }

    // ветка success по salary
    { Person bob("Bob", 8, 2'000'000, 2);  test(bob); }

    // ветка failure
    { Person carol("Carol", 7, 50'000, 3); test(carol); }

    // исключение из id() — E
    {
        Person dave("Dave", 5, 100, -1);

        try
        {
            test(dave);
        }
        catch (std::exception const & e)
        {
            std::cerr << "Caught std::exception: " << e.what() << '\n';
        }
        catch (...)
        {
            std::cerr << "Caught unknown exception\n";
        }
    }
}
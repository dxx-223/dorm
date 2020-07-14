# Dxx-223's ORM

A small library that parses SQL in C++ source files to generate helper objects that provide database support.

## Helper object generation

The program `generate_object` scans `.hpp` files for comments containing an SQL `CREATE TABLE` statement.

Common *member functions* are emitted, like fetching from DB using keys, others are inherited, like `deleterow()`.

The column definitions from the SQL statement are used to build C++ *data members* with corresponding types.
e.g. SQL `age int unsigned` would become C++ `uint32_t age`

Column definitions also produce other member functions like `bool defined_<column>()` and the typical getters/setters.

Some *cpp* `#define` macros, like `CHILD_OBJECT(Person, get_person)` produce extra *member functions* for fetching
related records from the database, in this case: `Person get_person()` for fetching a `Person` object.

## Other features

### Minimal saves
If "minimal saves" are enabled, then when saving an object to the database, only columns that have *changed* are actually included in the SQL `UPDATE` statement.

If "minimal saves" are disabled, then all columns are sent to the SQL engine, potentially overwriting all prior values.

### Searching / queries / SELECT
Database queries can be built by setting column values directly, for equality matches, 
or using "search modifiers" which encode more complex SQL column comparisons via *member functions*.

Example:
```
// Person_ is the generated helper
class Person: public Person_ {
  public:
    DORM::SearchMod<unsigned int> younger_than;
    
    void search_prep(DORM::Query &query) const {
      if (younger_than)
        query.and_where(DORM::sqlLt<unsigned int>("age", younger_than()));
    };
};
```

Complex database JOINs are also supported:
```
Person people;
people.younger_than(12);

Treat treats;
treats.flavour("choc");

// How many people under 12 who like chocolate treats?
uint64_t found_rows = people.search({ treats });
```

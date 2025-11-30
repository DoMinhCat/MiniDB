# C_Final

Projet final de C à ESGI

## Table of contents

1. Project Description
2. Pre-requisites

    - Windows OS (for Makefile to work properly)
    - gcc
    - mingw32-make.exe
3. Installation

    ```shell
    mingw32-make clean
    mingw32-make
    ./MiniDB.exe
    ```

4. Features

    - Strings does not need to be wrapped inside quotes or double quotes
    - Table and column names are case sensitive
    - Available types : int, string, and double
    - Multiple commands execution supported, each command must be separated by a new-line character and the command chain must end with a semicolon
    - Insert: values will be inserted in the order of which columns are created and all columns must be inserted (can't insert only 4 values into a table with 5 columns)
    - Operators AND and OR are not available yet
    - With SELECT using JOIN, if both tables have a column with the same name as the selected column, by default that column will refer to the first table
    - With SELECT using both WHERE and JOIN, the WHERE column is assumed to belong to the first table. If both tables contain a column with the same name, avoid using such columns in the WHERE clause to prevent ambiguity.
    - Tables can't be altered one created, altering must be done through deleting the table, recreate it and manually reinsert all data :)
    - NOT NULL constraint is not available yet
    - Only one primary key is allowed for each table => relation many-many not available yet
    - Primary key and foreign key must be of type int or string, can't be 0 or negative if is int
    - Primary key type INT auto-incrementation supported
    - All foreign keys are ON DELETE RESTRICT
    - The order of tables passed for DROP matters (foreign key constraint violation)
    - "exit" or "quit" to exit the program

    - Import file must be in the current directory

    - Max col per table = 50
    - Max length for str values 256 (\0 excluded)
    - Max table = 100
    - Max chars for table/col name = 100
    - Row per table warning threshold = 10 000

5. Usage

    - `show tables`
    - `describe table tab1`
    - `delete from tab1 [where col1 = 6]`
    - `drop table tab1 [, tab2, tab3,...]`
    - `create table tab1 ( col1 int pk, col2 string fk references tab_to_refer col_to_refer, col3 double )`, spaces between parentheses are obligated for the program to parse the command correctly
    - `insert into tab1 ( col1, col2 ) values ( val1, val2 )`
    - `select ( col1, col2, col3 )/* from tab1 [join tab2 on col1 = col2] [where col1 = xyz]`, order of col1 and col2 in JOIN clause matters, col1 and col2 correspond to tab1 and tab2 respectively

6. Technical choice clarification

7. Contributors

    - Minh Cat DO
    - Paco ROBINET-CAMPOY
    - Cheikh Ahmadou Bamba CISSE

---

Current quick start :
gcc main.c ui/parser.c ui/create.c ui/delete.c ui/drop.c ui/insert.c ui/select.c ui/show.c ui/describe.c ui/helper_ui.c clean/cmd.c clean/db.c init/query.c init/db.c init/hash_table.c db/create.c db/helper_db.c db/select.c db/drop.c db/show.c db/insert.c db/describe.c db/where.c db/join.c db/delete.c global_var.c hash/hash.c file/export.c -o sb.exe

Commands to test:

create table customers ( id int pk, name string unique, age int )
create table orders ( order_id int pk, customer_id int fk references customers id, amount double )

insert into customers ( id, name, age ) values ( 1, Alice, 30 )
insert into customers ( id, name, age ) values ( 2, Bob, 25 )
insert into customers ( id, name, age ) values ( 3, Carol, 40 )
insert into customers ( id, name, age ) values ( 4, Dave, 50 )
insert into customers ( id ) values ( 45 )
insert into customers ( id ) values ( 46 )

insert into orders ( order_id, customer_id, amount ) values ( 10, 1, 99.5 )
insert into orders ( order_id, customer_id, amount ) values ( 11, 1, 20 )
insert into orders ( order_id, customer_id, amount ) values ( 12, 3, 250.75 );

select ( id, name ) from customers
select ( id, name ) from customers where id = 2
select ( id, name ) from customers where name = Alice
select ( order_id, amount ) from orders where amount = 20.0;

select ( id, amount ) from customers join orders on id = customer_id
select ( name, amount ) from customers join orders on id = customer_id;

select ( id, name, amount ) from customers join orders on id = customer_id where id = 1
select ( id, name, amount ) from customers join orders on id = customer_id where amount = 250.75
select ( name, amount ) from customers join orders on id = customer_id where name = Alice;

Edge cases:
select ( id ) from customers join orders on id = customer_id    // select id will return id of customers even when orders also has id column
select ( id, id ) from customers join orders on id = customer_id    // will only return id of customers

delete from customers where name = null
delete from customers where id = 4
delete from customers where id = 2;

delete from customers
delete from customers where id = 1
delete from customers where id = 3;

drop table customers, orders;

**Ideas to improve (if time allows)**
    - Select col AS abc (if not too complicated)
    - Default constraint

# A set of test commands for MiniDB

## Create tables

```sql
create table customers ( id int pk, name string unique, age int )
create table orders ( order_id int pk, customer_id int fk references customers id, amount double );
```

---

## Insert data

```sql
insert into customers ( id, name, age ) values ( 1, Alice, 30 )
insert into customers ( id, name, age ) values ( 2, Bob, 25 )
insert into customers ( id, name, age ) values ( 3, Carol, 40 )
insert into customers ( id, name, age ) values ( 4, Dave, 50 )
insert into customers ( id ) values ( 47 )
insert into customers ( id ) values ( 48 )

insert into orders ( order_id, customer_id, amount ) values ( 10, 1, 99.5 )
insert into orders ( order_id, customer_id, amount ) values ( 11, 1, 20 )
insert into orders ( order_id, customer_id, amount ) values ( 12, 3, 250.75 );
```

---

## Select data with WHERE and JOIN

```sql
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
```

---

## Delete data

```sql
delete from customers where name = null
delete from customers where id = 4
delete from customers where id = 2;

delete from customers
delete from customers where id = 1
delete from customers where id = 3;
```

---

## Delete tables

```sql
drop table customers, orders;
drop table orders, customers;

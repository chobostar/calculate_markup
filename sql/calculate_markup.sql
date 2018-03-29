CREATE EXTENSION calculate_markup;
SELECT price_modifiers._calculate_markup(75000, '{{100000,7.0},{50000,8.0},{10000,10.0},{1000,12.0},{100,22.0},{0,22.0}}'::numeric[]);

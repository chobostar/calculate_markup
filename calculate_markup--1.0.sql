\echo Use "CREATE EXTENSION calculate_markup" to load this file. \quit

CREATE SCHEMA IF NOT EXISTS price_modifiers;

CREATE OR REPLACE FUNCTION price_modifiers._calculate_markup(
    numeric,
    numeric[])
  RETURNS numeric AS
'$libdir/calculate_markup', 'calculate_markup'
  LANGUAGE c IMMUTABLE STRICT
  COST 1;

COMMENT ON FUNCTION price_modifiers._calculate_markup IS 'Вспомогательная функция вычисления значения наценки методом линейной интерполяции';

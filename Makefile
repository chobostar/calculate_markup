MODULES = calculate_markup

EXTENSION = calculate_markup
DATA = calculate_markup--1.0.sql
PGFILEDESC = "calculate_markup - extension with function for calculate markup by linear interpolation"

REGRESS = calculate_markup

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

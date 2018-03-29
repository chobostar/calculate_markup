#include "postgres.h"
#include "funcapi.h"
#include "fmgr.h"
#include "catalog/pg_type.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/numeric.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(calculate_markup);

Datum
calculate_markup(PG_FUNCTION_ARGS)
{
	Numeric *inputCost = PG_GETARG_NUMERIC(0);        
	ArrayType *array = PG_GETARG_ARRAYTYPE_P(1);

	if (ARR_NDIM(array) != 2)
		ereport(ERROR, (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("2-dimensional array needed")));

	Datum  *datums;
	bool   *nulls;
	int     count;
	int16   elemWidth;
	Oid     elemType = ARR_ELEMTYPE(array);
	bool    elemTypeByVal, isOpenInterval;
	char    elemAlignmentCode;

	Numeric cost0, markup0, cost1, markup1, deltaMarkup, deltaCost, deltaInputCost;

	get_typlenbyvalalign(elemType, &elemWidth, &elemTypeByVal, &elemAlignmentCode);
	deconstruct_array(array, elemType, elemWidth, elemTypeByVal, elemAlignmentCode, &datums, &nulls, &count);

	//реализация функции подразумевает что _markups приходят отсортированными по cost в порядке убывания
	isOpenInterval = true;
	for (int i = 0; i < count; i += 2 )
	{
		if ((datums[i] == NULL) || (datums[i+1] == NULL))
		{
			PG_RETURN_NULL();
		}

		cost0 = DatumGetNumeric(datums[i]);
		markup0 = DatumGetNumeric(datums[i+1]);		

		if (DatumGetBool(DirectFunctionCall2(numeric_ge, inputCost, cost0)))
		{
			// при совпадении значения с точкой на графике, возвращаем известное значение наценки
			// для отрезка [x0,) используем f(x0)
			if (DatumGetBool(DirectFunctionCall2(numeric_eq, inputCost, cost0)) || isOpenInterval)
			{
				PG_RETURN_NUMERIC(markup0);
			}

		        // возвращаем интерполированное значение наценки
			deltaMarkup = DatumGetNumeric(DirectFunctionCall2(numeric_sub, markup1, markup0));
			deltaCost = DatumGetNumeric(DirectFunctionCall2(numeric_sub, cost1, cost0));
			deltaInputCost = DatumGetNumeric(DirectFunctionCall2(numeric_sub, inputCost, cost0));

		       // ROUND(_markup0 + (_markup1 - _markup0) / (_cost1 - _cost0) * (_cost - _cost0), 2)
			PG_RETURN_NUMERIC(DatumGetNumeric(DirectFunctionCall2(numeric_round, (DirectFunctionCall2(numeric_add, markup0, (DirectFunctionCall2(numeric_mul, (DirectFunctionCall2(numeric_div, deltaMarkup, deltaCost)), deltaInputCost)))), 2)));
		}
		
		cost1 = cost0;
		markup1 = markup0;
		isOpenInterval = false;
	}

	//для отрезка (,x0) используем f(x0)
	PG_RETURN_NUMERIC(markup0);
}



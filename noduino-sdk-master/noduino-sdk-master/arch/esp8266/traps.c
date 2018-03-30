/*
 *  Copyright (c) 2015 - 2025 MaiKe Labs
 *
 *  Written by Jack Tan <jiankemeng@gmail.com>
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "xtos.h"

void trap_init(void)
{
	_xtos_set_exception_handler(EXCCAUSE_UNALIGNED, default_exception_handler);
	_xtos_set_exception_handler(EXCCAUSE_ILLEGAL, default_exception_handler);
	_xtos_set_exception_handler(EXCCAUSE_INSTR_ERROR, default_exception_handler);
	_xtos_set_exception_handler(EXCCAUSE_LOAD_PROHIBITED, default_exception_handler);
	_xtos_set_exception_handler(EXCCAUSE_STORE_PROHIBITED, default_exception_handler);
	_xtos_set_exception_handler(EXCCAUSE_PRIVILEGED, default_exception_handler);

	_xtos_set_exception_handler(EXCCAUSE_LOAD_STORE_ERROR, unaligned_handler);
}

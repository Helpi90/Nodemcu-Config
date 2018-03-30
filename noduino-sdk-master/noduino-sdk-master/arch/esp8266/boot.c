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

#include "user_config.h"
#include "xtos.h"

extern void call_user_start(void);
extern void Cache_Read_Disable(void);
extern void __real_Cache_Read_Enable(int a, int b, int c);

void arch_init (void)
{
	trap_init();

	Cache_Read_Disable();
	call_user_start();
}

void __wrap_Cache_Read_Enable(int a, int b, int c)
{
	__real_Cache_Read_Enable(a, b, 0);
}

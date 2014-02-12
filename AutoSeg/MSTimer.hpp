/*
 * Copyright (C) 2014  mingspy@163.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
/*
* implementation of ResGuard.
* See <POSIX Muti-threads programming>
* by xiuleili 2013-7-5
*/

#pragma once
#include<iostream>
#include<ctime>
using namespace std;

namespace mingspy
{
class MSTimer
{
private:
    clock_t start_time;
    clock_t end_time;
public:
    MSTimer()
    {
        start_time = clock();
    }

    void restart()
    {
        start_time = clock();
    }

    double elapsed()
    {
        end_time = clock();
        return (double)(end_time - start_time) / CLOCKS_PER_SEC;
    }

    friend ostream & operator<< (ostream & out, MSTimer & timer)
    {
        out<<" elapsed:"<<timer.elapsed()<<"s.";
        return out;
    }
};

}

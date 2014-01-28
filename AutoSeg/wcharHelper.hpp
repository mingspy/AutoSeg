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
#pragma once

namespace mingspy{
    enum wtype{
        OTHER = 0,  // 
        EOS,        // [0]
        ALPHA,      // single:[41,5a] [61,7a], wide:[ff21,ff3a] [ff41,ff5a]
        DIGIT,      // single:[30,39]   wide:[ff10,ff19]
        DELIMITER,  // [21,2f][3a,3f][5b,5f],[7b.7f] 
                    // wide:[ff01,ff0f],[ff1a,ff1f][ff3b,ff3f][ff5b,ff5f]
        WHITESPACE, // 9,20
        CHINESE,    // [4e00,9fba],[e815,e864],[f920,fa20]
        //WIDE_ALPHA, //
        //WIDE_DIGIT, // 
        //WIDE_DELIMITER, // 
    };

    inline wtype getWcharType(wchar_t ch){
        if((ch >= 0x4e00 && ch <= 0x9fba) 
            || (ch >= 0xe815 && ch <= 0xe864)
            || (ch >= 0xf920 && ch <= 0xfa20)){
                return CHINESE;
        }else if(ch < 0x80){
            if((ch>=0x41 && ch <= 0x5a)||(ch>=0x61 && ch <= 0x7a)){
                return ALPHA;
            }else if(ch >= 0x30 && ch <= 0x39){
                return DIGIT;
            }else if(ch == 0x09 || ch == 0x20){
                return WHITESPACE;
            }else if(ch == 0){
                return EOS;
            }
            return DELIMITER;
        }else if(ch > 0xff00){
            if((ch>=0xff21 && ch <= 0xff3a)||(ch>=0xff41 && ch <= 0xff5a)){
                return ALPHA;
            }else if(ch >= 0xff10 && ch <= 0xff19){
                return DIGIT;
            }
            return DELIMITER;
        }
        return OTHER;
    }
}
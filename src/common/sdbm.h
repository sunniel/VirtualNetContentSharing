//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef SDBM_H_
#define SDBM_H_

class sdbm {
public:
    sdbm();
    virtual ~sdbm();

    static unsigned long encode(const char* str) {
        unsigned long hash = 0;
        int c;
        while ((c = *str++) != 0){
            hash = c + (hash << 6) + (hash << 16) - hash;
        }
        return hash;
    }
};

#endif /* SDBM_H_ */

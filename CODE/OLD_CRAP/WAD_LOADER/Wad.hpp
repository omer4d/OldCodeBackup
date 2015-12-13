#ifndef _WAD_HPP_
#define _WAD_HPP_

#include "Vec2f.hpp"

#include <stdio.h>
#include <fstream>
#include <string>

#define FREAD(file, dest) file.read((char*)&dest, sizeof(dest))

void readDoomStr(std::ifstream& file, char str[9])
{
   file.read(str, 8);
   str[8] = 0;
}

struct Wad {

    struct Header {
        char id[4];
        int lumpNum;
        int dirOffs;

        Header(std::ifstream & file)
        {
            FREAD(file, id);
            FREAD(file, lumpNum);
            FREAD(file, dirOffs);
        }

        void print()
        {
            printf("id: %c%c%c%c\n", id[0], id[1], id[2], id[3]);
            printf("lumpNum: %d\n", lumpNum);
            printf("dirOffs: %d\n", dirOffs);
        }
    };

    struct DirEnt {
        int lumpOffs;
        int lumpSize;
        char name[9];

        DirEnt()
        {
             memset(this, 0, sizeof(DirEnt));
        }

        DirEnt(std::ifstream & file)
        {
            FREAD(file, lumpOffs);
            FREAD(file, lumpSize);
            readDoomStr(file, name);
        }

        void print()
        {
            printf("%s\n", name);
        }
    };

// **********
// * VERTEX *
// **********

    struct Vertex {
        short x, y;
        static const int size = 4;

        Vertex()
        {
            memset(this, 0, sizeof(Vertex));
        }

        Vertex(Wad const& wad, std::ifstream & file)
        {
            FREAD(file, x);
            FREAD(file, y);
        }
        
        Vec2f toVec2f() const
        {
            return Vec2f(x, y);
        }
    };

// **********
// * SECTOR *
// **********

   struct Sector {
          short floorHeight, ceilHeight;
          char floorTexName[9], ceilTexName[9];
          short light, type, tag;
          
          static const int size = 26;
          
          Sector()
          {
              memset(this, 0, sizeof(Sector));
          }
          
          Sector(Wad const& wad, std::ifstream & file)
          {
              FREAD(file, floorHeight);
              FREAD(file, ceilHeight);
              readDoomStr(file, floorTexName);
              readDoomStr(file, ceilTexName);
              FREAD(file, light);
              FREAD(file, type);
              FREAD(file, tag);
          }
   };
   
// ***********
// * SIDEDEF *
// ***********

   struct Sidedef {
          short xOffs, yOffs;
          char hiTexName[9], lowTexName[9], midTexName[9];
          Sector* sector;
          
          static const int size = 30;
          
          Sidedef()
          {
              memset(this, 0, sizeof(Sidedef));
          }
          
          Sidedef(Wad const& wad, std::ifstream & file)
          {
              FREAD(file, xOffs);
              FREAD(file, yOffs);
              
              readDoomStr(file, hiTexName);
              readDoomStr(file, lowTexName);
              readDoomStr(file, midTexName);
              
              short sectorIndex;
              FREAD(file, sectorIndex);
              sector = &wad.sectorLump->arr[sectorIndex];
          }
   };

// ***********
// * LINEDEF *
// ***********

   struct Linedef {
          Vec2f v1, v2;
          short flags, specialFlag, sectorTag;
          Sidedef *posSidedef, *negSidedef;
          
          static const int size = 14;
          
          Linedef()
          {
              memset(this, 0, sizeof(Linedef));
          }
          
          Linedef(Wad const& wad, std::ifstream & file)
          {
              short temp;
              
              FREAD(file, temp);
              v1.init(wad.vertexLump->arr[temp].x, wad.vertexLump->arr[temp].y);
              
              FREAD(file, temp);
              v2.init(wad.vertexLump->arr[temp].x, wad.vertexLump->arr[temp].y);
              
              FREAD(file, flags);
              FREAD(file, specialFlag);
              FREAD(file, sectorTag);
              
              FREAD(file, temp);
              if(temp == -1) posSidedef = NULL;
              else           posSidedef = &wad.sidedefLump->arr[temp];
              
              FREAD(file, temp);
              if(temp == -1) negSidedef = NULL;
              else           negSidedef = &wad.sidedefLump->arr[temp];
          }
   };

// ********
// * LUMP *
// ********

    template <class T>
    struct Lump {
           int n;
           T* arr;
           
           Lump(Wad const& wad, std::ifstream& file, DirEnt const& dirEnt)
           {
                n = dirEnt.lumpSize / T::size;
                arr = new T[n];

                file.seekg(dirEnt.lumpOffs, std::ios_base::beg);

                for(int i = 0; i < n; i++)
                {
                   arr[i] = T(wad, file);
                }
           }
           
           ~Lump()
           {
               delete[] arr;
           }
    };

// **************************
// * Properties and methods *
// **************************

    Header header;
    std::map<std::string, DirEnt> dir;

    Lump<Vertex>* vertexLump;
    Lump<Sector>* sectorLump;
    Lump<Sidedef>* sidedefLump;
    Lump<Linedef>* linedefLump;

    Wad(std::ifstream & file) : header(file)
    {
        header.print();
        printf("---------\n");

        file.seekg(header.dirOffs);

        while(file.good())
        {
            DirEnt dirEnt(file);
            dirEnt.print();
            dir[dirEnt.name] = dirEnt;
        }
        
        file.clear();
        
        printf("-----------\n");
        
        vertexLump = new Lump<Vertex>(*this, file, dir["VERTEXES"]);
        sectorLump = new Lump<Sector>(*this, file, dir["SECTORS"]);
        sidedefLump = new Lump<Sidedef>(*this, file, dir["SIDEDEFS"]);
        linedefLump = new Lump<Linedef>(*this, file, dir["LINEDEFS"]);
    }

    ~Wad()
    {
        delete vertexLump;
        delete sectorLump;
        delete sidedefLump;
        delete linedefLump;
    }
};

#endif

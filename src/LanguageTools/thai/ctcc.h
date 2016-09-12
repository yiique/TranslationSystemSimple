#ifndef CTCC_H
#define  CTCC_H
#include <vector>
#include <map>
#include "utf8.h"
using namespace std;

 namespace thaiSeg{
class Ctcc {
public:
    typedef unsigned short Short;
    map<Short, bool> con, fcon,ton,thd,lad,alp,sym,spa;
    Ctcc(){
        for (Short i = 0xe01; i <=0xe2e;++i) {
            con[i] = true;
            fcon[i] = true;
        }

        fcon[0xe03] = false;
        fcon[0xe05] = false;
        fcon[0xe09] = false;
        fcon[0xe0c] = false;
        fcon[0xe1c] = false;
        fcon[0xe1d] = false;
        fcon[0xe24] = false;
        fcon[0xe26] = false;
        fcon[0xe2b] = false;
        fcon[0xe2e] = false;

        for (Short i = 0xe48; i <= 0xe4b;++i) {
            ton[i] = true;
        }

        for ( Short i=0xe50;i<=0xe59;i++) {
            thd[i] = true;
        }

        Short i;
        for (i=0x30;i<=0x39;i++) {
            lad[i] = true;
        }

        //Alphabet
        for (i=0x41;i<=0x5A;i++ ){
            alp[i] = true;
        }
        for (i=0x61;i<=0x7A;i++ ){
            alp[i] = true;
        }

        //Symbol
        for (i=0x21;i<=0x2F;i++) {
            sym[i] = true;
        }
        for (i=0x3A;i<=0x40;i++) {
            sym[i] = true;
        }
        for (i=0x5B;i<=0x60;i++) {
            sym[i] = true;
        }
        for (i=0x7B;i<=0x7E;i++) {
            sym[i] = true;
        }

        //Space
        for (i=0x2000;i<=0x200B;i++) {
            spa[i] = true;
        }
        spa[0x9] = spa[0x20] = spa[0xA0]= spa[0x202F]= spa[0xFEFF] = true;


    }

    string tok(string& input) {
        vector<unsigned short> utf16;
        vector<unsigned short> output;
        utf8::utf8to16(input.begin(), input.end(), back_inserter(utf16));
        int state = 0;
        size_t len = utf16.size();
        for (int i = 0; i < len; ++i) {
            Short c = utf16[i];
            switch (state) {
                case -1:
                    {
                        state = 0;
                        break;
                    }
                case 2121:
                    {
                        if (c == 0xe22) state = 21211;
                        else state = 0;
                        break;
                    }
                case 311:
                case 2111:
                case 215:
                case 21211:
                case 2122:
                case 21311:
                case 2132:
                case 1121:
                case 111:
                case 411:
                    {
                        if (c==0xe30) { state = -1; } 
                        else
                        { state = 0; }
                        break;
                    }

                case 2131:
                    {
                        if (c==0xe2D) { state = 21311;} 
                        else
                        { state = 0;  /**error**/ };
                        break;
                    }
                    

                case 112:
                    {
                        if (c==0xe27) { state = 1121; } 
                        else if (fcon[c]) { state = -1; } 
                        else
                            { state = 0;  /**error**/ }
                        break;
                    }
                    

                case 211:
                    {

                    
                    if (c==0xe32) {state = 2111;} 
                    else if (c==0xe30) {state = -1;} 
                    else
                        { state = 0;}
                    break;
                    }

                case 212:
                    {

                    
                    if (ton[c]) { state =2121; } 
                    else if (c==0xe22) { state = 2122;} 
                    else
                        { state = 0; /**error**/ }
                    break;
                    }

                case 213:
                    {

                    
                    if (ton[c]) { state =2131; } 
                    else if (c==0xe2D) { state = 2132;} 
                    else
                        { state = 0; /**error**/ }
                    break;
                    }

                case 312:
                case 214:
                    {

                    
                    if (fcon[c]) { state = -1; } else
                    { state = 0; /**error**/ }
                    break;
                    }

                case 11:
                    {

                    
                    if (c==0xe27 ){ state = 111; } 
                    else if (fcon[c] ){ state = -1; } 
                    else if (ton[c]) { state = 112; } 
                    else
                    { state = 0; /**error**/ }
                    break;
                    }

                case 12:
                    {

                    
                    if (c==0xe30 || c==0xe32 || c==0xe33) { state = -1; } 
                    else
                    { state = 0;}
                    break;
                    }
                case 13:
                case 51:
                    {

                    
                    if (ton[c] ){ state = -1 ;} 
                    else 
                    { state = 0; }
                    break;
                    }

                case 14:
                    {

                    
                    if (c==0xe4C || ton[c]) {state= -1;} 
                    else
                    { state = 0; }
                    break;
                    }

                case 21:
                    {

                    
                    if (ton[c]) {state = 211;} else
                        if (c==0xe35) {state = 212;} else
                            if (c==0xe37) {state = 213;} else
                                if (c==0xe47) {state = 214;} else
                                    if (c==0xe32) {state = 215;} else
                                        if (c==0xe30) {state = -1;} else
                                        {state = 0;}
                    break;
                    }

                case 31:
                    {

                    
                    if (ton[c]) {state = 311;} else
                        if (c==0xe30) {state = -1;} else
                            if (c==0xe47) {state = 312;} else
                            { state = 0 ;}
                    break;
                    }

                case 41:
                    {

                    
                    if (ton[c]) {state = 411;} else
                        if (c==0xe30) {state = -1;} else
                        { state = 0; }
                    break;
                    }
                case 1:
                    {

                    
                    if (c==0xe31 ){ state = 11; } else
                        if (c==0xe47) { state = -1; } else
                            if (ton[c] ){ state = 12;} else
                                if (c==0xe30 || c==0xe32 || c==0xe33) { state = -1 ;} else
                                    if (c>=0xe35 && c<=0xe39 ){ state=13; } else
                                        if (c==0xe34 ){ state = 14 ;} else
                                            if (c==0xe4C) { state= -1 ;} else 
                                            { state = 0; }
                    break;
                    }
                case 2:
                    {

                    
                    if (con[c]){state = 21;} else
                    { state = 0; /**error**/ }
                    break;
                    }

                case 3:
                    {

                    
                    if (con[c]) {state = 31;} else
                    { state = 0; /**error**/ }
                    break;
                    }
                case 4:
                    {

                    
                    if (con[c]) {state = 41;} else
                    { state = 0; /**error**/ }
                    break;
                    }
                case 5:
                    {

                    
                    if (con[c] ){state = 51;} else
                    { state = 0; /**error**/ }
                    break;
                    }
                case 6:
                    {

                    
                    if( !thd[c]) {state = 0;}
                    break;
                    }
                case 7:
                    {

                    
                    if (!lad[c]) {state = 0;}
                    break;
                    }
                case 8:
                    {
                    
                    if (!alp[c]) {state = 0;}
                    break;
                    }
                case 9:
                    {

                    
                    if (!spa[c]) {state = 0;}
                    break;
                    }
            }//switch
            if (state == 0) {
                if (con[int(c)]) { state = 1 ;} else 
                    if (c==0xe40) { state = 2 ;} else
                        if (c==0xe41) { state = 3 ;} else 
                            if (c==0xe42) { state = 4; } else 
                                if (c==0xe43 || c==0xe44) { state = 5 ;} else
                                    if (thd[c]) { state = 6 ;} else
                                        if (lad[c]) { state = 7; } else 
                                            if (alp[c]) { state = 8; } else
                                                if (sym[c]) { state = -1; } else
                                                    if (spa[c]) { state = 9; } else
                                                    { state = 0; }
                                                    output.push_back(0x20);
            }//if
            output.push_back(c);

            }//for
            
            string utf8;
            utf8::utf16to8(output.begin(), output.end(), back_inserter(utf8));
            return utf8;

        }
    
};

}
#endif

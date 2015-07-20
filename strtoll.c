#include "strtoll.h"
#include <math.h>

unsigned HexDigitValue(char c){
    if(c<='9') return c-'0';
    if(c<='F') return c-'A';
    return c-'a';
}

int IsDecDigit(char c){
    return c>='0' && c<='9';
}

int IsHexDigit(char c){
    return IsDecDigit(c) || (c>='A' && c<='F') || (c>='a' && c<='f');
}

int IsOctDigit(char c){
    return c>='0' && c<'8';
}

int IsBinDigit(char c){
    return c=='0' || c=='1';
}

int DecStrToInt64(const char *string, uint64_t *dest){
    while(*string!='\0'){
        if(!IsDecDigit(*string)) return -1;
        
        dest[0]*=10;
        dest[0]+=(*string)-'0';
        string++;
    }
    return 1;
}

int HexStrToInt64(const char *string, uint64_t *dest){
    while(*string!='\0'){
        if(!IsHexDigit(*string)) return -1;
        
        dest[0]<<=4;
        dest[0]+=HexDigitValue(*string);
        string++;
    }
    return 1;
}

int OctStrToInt64(const char *string, uint64_t *dest){
    while(*string!='\0'){
        if(!IsOctDigit(*string)) return -1;
        
        dest[0]<<=3;
        dest[0]+=(*string)-'0';
        string++;
    }
    return 1;
}

int BinStrToInt64(const char *string, uint64_t *dest){
    while(*string!='\0'){
        
        dest[0]<<=1;

        if(*string=='1'){
            dest[0]++;
        }
        else if(*string=='0');
        else return -1;
        
        string++;
    }
    return 1;
}

int StrToInt64(const char *string, int64_t *dest){

    int negated = 0;
    uint64_t value = 0;

    while(*string!='\0'){
        if(! (*string==' ' ||
            *string=='\t' ||
            *string=='\n' ||
            *string=='\r'))
            break;
        string++;
    }

    if(*string=='-'){
        negated = 1;
        string++;
    }
    else if(*string=='+'){
        negated = 0;
        string++;
    }

    if(*string=='0'){
        string++;
        if(*string=='x' || *string=='X'){
            string++;
            if(!HexStrToInt64(string, &value)) return -1;
        }
        else if(*string=='b' || *string=='B'){
            string++;
            if(!BinStrToInt64(string, &value)) return -1;
        }
        else if(!OctStrToInt64(string, &value)) return -1;
    }
    else if(!DecStrToInt64(string, &value)) return -1;
    
    dest[0] = value;
    if(negated) dest[0] = -dest[0];
    return 1;
}

int StrToFloat(const char *string, float *dest){

    int negated = 0;
    uint64_t numerator = 0, denominator = 0;

    /* Skip any whitespace */
    while(*string!='\0'){
        if(! (*string==' ' ||
            *string=='\t' ||
            *string=='\n' ||
            *string=='\r'))
            break;
        string++;
    }

    /* Get a sign if one exists */
    if(*string=='-'){
        negated = 1;
        string++;
    }
    else if(*string=='+'){
        negated = 0;
        string++;
    }

    /* Get the whole number part of the string */
    if(!IsDecDigit(*string)) return -1;

    do{
    
        if(!IsDecDigit(*string)){
            numerator*=10;
            numerator+=(*string)-'0';
        }
    
    
    }while(IsDecDigit(*string));
    
    /* Apply the sign. We do it early to make returning integers simpler. */
    if(negated) numerator = -numerator;
    
    /* If this is the end of the string (it was just an integer!?), return now */
    if(*string=='\0'){
        dest[0] = numerator;
        return 1;
    }
     
    /* The decimal point */
    if(*string!='.') return -1;
    
    string++;
    {
        unsigned decimal_places = 0;
        /* Get the decimal portion of the number */
        do{
            if(!IsDecDigit(*string)) return -1;
            
            denominator*=10;
            denominator+=(*string)-'0';
            decimal_places++;
        }while(*string!='\0');
  
        /* We already applied the sign, so just do the division and return */
        dest[0] = 
            /* Whole number portion */
            ((double)numerator)
            /* The remains  - - - - - Divided by ten times the number of decimal places removed */
            / (((double)denominator)/pow(10.0, decimal_places));
    }

    return 1;
}

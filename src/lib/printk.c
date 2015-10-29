#include "common.h"


uint32_t uint2Str (char *str,uint32_t n, uint32_t base) {
    uint32_t reminder = n % base;
    uint32_t quotient = n / base;
    char c;
    uint32_t pos = 0;
    //uint32_t len = 0;
    
    if(n > 0 )
        pos = uint2Str(str,quotient,base);
    else //base == 0
        return 0;

    switch (base) {
        case 10:
            c = '0' + reminder;
            break;
        case 16:
            if(reminder > 9) {
                reminder -= 10;
                c = 'a' + reminder;
            }else {
                c = '0' + reminder;
            }
            break;
        default:
            assert(0);
    }
    str[pos] = c;
    return pos+1;
}
uint32_t int2Str(char* str,int32_t n,int32_t base) {    
    
    if (n==0) {
        str[0] = '0';
        str[1] = '\0';
        return 2;
    }

    if(base == 16)
        return uint2Str(str,(uint32_t)n,16);


    if(n < 0) {
        str[0] = '-';
        n  = 0 - n;
        return uint2Str(str+1,n,base);
    }
    return uint2Str(str,n,base);
    
}
void printStr(void (*printer)(char),const char *str) {
    while(*str != '\0'){
        printer(*str);
        str++;
    }
}
/* implement this function to support printk */
void vfprintf(void (*printer)(char), const char *ctl, void **args) {
	//const char *str = __FUNCTION__;
	//for(;*str != '\0'; str ++) printer(*str);
	char buf[20] = {'\0'};
	uint32_t integer;
    for(; *ctl != '\0'; ctl++){
        if(*ctl == '%')
        {
            ctl ++;
            if( *ctl == '\0')
                return;
            switch(*ctl){
                case 'd':
                    integer = (uint32_t)*args;
                    int2Str(buf,integer,10);
                    printStr(printer,buf);
                    args++;

                    break;
                case 'c':
                    printer((char)(((uint32_t)*args) & 0xff));
                    args++;
                    break;
                case 's':
                    printStr(printer,(char*)*args);
                    args++;
                    break;
                case '%':
                    printer('%');
                    break;
                case 'x':
                    integer = (uint32_t)*args;
                    int2Str(buf,integer,16);
                    printStr(printer,buf);
                    args++;
                    break;
                //case '%':
                //    break;
                default:
                    assert(0);
            }

        }else
        {
            printer(*ctl);
            //printer('y');
        }
       // ctl++;
    }
	//char *str = ": vfprintf() is not implemented!\n";
	//for(;*ctl != '\0'; ctl ++) printer(*ctl);
}

extern void serial_printc(char);

/* __attribute__((__noinline__))  here is to disable inlining for this function to avoid some optimization problems for gcc 4.7 */
void __attribute__((__noinline__)) 
printk(const char *ctl, ...) {
	void **args = (void **)&ctl + 1;
	vfprintf(serial_printc, ctl, args);
}

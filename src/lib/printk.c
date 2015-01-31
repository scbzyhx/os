#include "common.h"
void recurInt2Str (char **str,int n, int base) {
    if(n > 0) {
        recurInt2Str(str,n / base , base);
        switch(base)
        {
            case 10:
                **str = n % base + '0';
                break;
            case 16:
                if ( n % base > 9 )
                {
                    **str = n % base - 10 + 'a';

                }else
                    **str = n % base + '0';
                break;
            default:
                assert(0);
        }
        //**str = n % base + '0';
        (*str)++;
    }
}
void int2Str(char* str,uint32_t n,uint32_t base) {    
    if(base == 16)
    {
        *str = '0';
        str++;
        *str = 'x';
        str++;
    }
    recurInt2Str(&str,n,base);
    //last 
    *str = '\0';
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
	char buf[20];
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
                    //buf[0]= 'a';
                    //buf[1] = '\0';
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

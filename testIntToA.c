#include <string.h>
#include <stdlib.h>


void main(void){
	char hoursB[2];
	hoursB = itoaSelf(35, *hoursB);
	char minutesB[2];
	minutesB = itoaSelf(23, *minutesB);
	char secondsB[2];
	secondsB = itoaSelf(36, *secondsB);
	char fullString[8];
	strlcpy(fullString, *hoursB,2);
	strcat(fullString,":");
	strcat(fullString,*minutesB);
	strcat(fullString,".");
	strcat(fullString,*secondsB);

	printf(fullString);

}

char* itoaSelf(int i, char b[]){
	char const digit[] = "0123456789";
	char* p = b;
	if(i<0){
		*p++ = '-';
		i = -1;
	}
	int shifter = i;
	do{ //Move to where representation ends
		++p;
		shifter = shifter/10;
	}while(shifter);
	*p = '\0';
	do{ //Move back, inserting digits as u go
		*--p = digit[i%10];
		i = i/10;
	}while(i);
	return b;
}

size_t
strlcpy(char *dst, const char *src, size_t siz)
{
	char       *d = dst;
	const char *s = src;
	size_t      n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0)
	{
		while (--n != 0)
		{
			if ((*d++ = *s++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0)
	{
		if (siz != 0)
			*d = '\0';          /* NUL-terminate dst */
		while (*s++)
			;
	}



	return (s - src - 1);       /* count does not include NUL */
}

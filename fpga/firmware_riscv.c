void delay()
{
	for (int i = 0; i < 1000000; ++i)
	{
		*(volatile char*)0x1000000 = 0xaa;
	}
}

void putc(char c)
{
	while (*(volatile char*)0x10000);
	*(volatile char*)0x10000 = c;
}

void puts(const char* s)
{
	while (*s) putc(*s++);
}

void* memcpy(void* dest, const void* src, int n)
{
	while (n) {
		n--;
		((char*)dest)[n] = ((char*)src)[n];
	}
	return dest;
}

void main()
{
	char message[] = "$Uryyb+Jbeyq!+Vs+lbh+pna+ernq+guvf+zrffntr+gura$gur+CvpbEI32+PCH"
		"+frrzf+gb+or+jbexvat+whfg+svar.$$++++++++++++++++GRFG+CNFFRQ!$$";
	for (int i = 0; message[i]; i++)
		switch (message[i])
		{
		case 'a' ... 'm':
		case 'A' ... 'M':
			message[i] += 13;
			break;
		case 'n' ... 'z':
		case 'N' ... 'Z':
			message[i] -= 13;
			break;
		case '$':
			message[i] = '\n';
			break;
		case '+':
			message[i] = ' ';
			break;
		}
	while (1)
	{
		puts(message);
		delay();
	}
}

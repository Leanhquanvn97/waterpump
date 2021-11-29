#include<iostream>
using namespace std;

int lenstr(char *s)
{
	int n = 0;
	while (s[n] != '\0')
	{
		n++;
	}
	return n;
}
void cpystr(char *p, char *s)
{
	int len = lenstr(s);
	for (int i = 0; i < len; i++)
	{
		p[i] = s[i];
	}

	p[len] = '\0';
}
char *substr(char *s, int start, int end)
{
	static char p[100];
	int indext = 0;

	for (int i = start; i <= end; i++)
	{
		p[indext] = s[i];
		indext++;
	}
	p[indext] = '\0';

	return p;
}
int main()
{
	char s[] = "ABCDEF";
	char p[100];

	cpystr(p, substr(s, 1, 3));
	cout << p;
	system("pause");
	return 0;
}
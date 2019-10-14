#include <cstdio>
#include <unistd.h>
#include <string>
#include <cstring>

int main()
{
	FILE* fp;
	char buff[1024] = { 0 };
	char comm[128] = { 0 };

	fgets(buff, sizeof(buff) - 1, stdin);

	int n = strlen(buff);
	if (buff[n - 1] == '\n')
		--n;
	snprintf(comm, sizeof(comm), "cat %s", buff);
	fp = popen(comm, "r");	//在该进程与comm命令之间建立一个管道，返回一个指针，这个指针用来读/"w"用来写

	memset(buff, 0, sizeof(buff));
	while (fgets(buff, 1024, fp) != NULL)
		fputs(buff, stdout);

	pclose(fp);
	_exit(0);
}
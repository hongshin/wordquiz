#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

/*단어 퀴즈 프로그램*/
typedef
enum {
	C_ZERO,
	C_LIST,
	C_SHOW,
	C_TEST,
	C_EXIT,
}
command_t;

// 파일로부터 한 줄을 읽어오는 함수
char* read_a_line(FILE* fp)
{
	static char buf[BUFSIZ];
	static int buf_n = 0;
	static int curr = 0;

	if (feof(fp) && curr == buf_n - 1)
		return 0x0;

	char* s = 0x0;
	size_t s_len = 0;
	do {
		int end = curr;
		while (!(end >= buf_n || !iscntrl(buf[end]))) {
			end++;
		}
		if (curr < end && s != 0x0) {
			curr = end;
			break;
		}
		curr = end;
		while (!(end >= buf_n || iscntrl(buf[end]))) {
			end++;
		}
		if (curr < end) {
			if (s == 0x0) {
				s = strndup(buf + curr, end - curr);
				s_len = end - curr;
			}
			else {
				s = realloc(s, s_len + end - curr + 1);
				s = strncat(s, buf + curr, end - curr);
				s_len = s_len + end - curr;
			}
		}
		if (end < buf_n) {
			curr = end + 1;
			break;
		}

		buf_n = fread(buf, 1, sizeof(buf), fp);
		curr = 0;
	} while (buf_n > 0);
	return s;
}


//메뉴를 출력하는 함수
void print_menu() {

	printf("1. List all wordbooks\n");
	printf("2. Show the words in a wordbook\n");
	printf("3. Test with a wordbook\n");
	printf("4. Exit\n");
}
//사용자로부터 명령어를 입력받는 함수
int get_command() {
	int cmd;

	printf(">");
	scanf("%d", &cmd);
	return cmd;
}
//단어장 목록을 출력하는 함수
void list_wordbooks()
{

	DIR* d = opendir("wordbooks");

	printf("\n  ----\n");

	struct dirent* wb;
	while ((wb = readdir(d)) != NULL) {
		if (strcmp(wb->d_name, ".") != 0 && strcmp(wb->d_name, "..") != 0) {
			printf("  %s\n", wb->d_name);
		}
	}
	closedir(d);

	printf("  ----\n");
}
//단어장의 단어와 뜻을 출력하는 함수
void show_words()
{
	char wordbook[128];
	char filepath[256];

	list_wordbooks();

	printf("Type in the name of the wordbook?\n");
	printf(">");
	scanf("%s", wordbook);

	sprintf(filepath, "wordbooks/%s", wordbook);

	FILE* fp = fopen(filepath, "r");

	printf("\n  -----\n");
	char* line;
	while (line = read_a_line(fp)) {
		char* word = strtok(line, "\"");
		strtok(NULL, "\"");
		char* meaning = strtok(NULL, "\"");

		printf("  %s : %s\n", word, meaning);

		free(line);
	}
	printf("  -----\n\n");

	fclose(fp);
}

//단어 퀴즈를 실행하는 함수
void run_test()
{
	char wordbook[128];
	char filepath[256];

	printf("Type in the name of the wordbook?\n");
	printf(">");
	scanf("%s", wordbook);

	sprintf(filepath, "wordbooks/%s", wordbook);

	FILE* fp = fopen(filepath, "r");

	printf("\n-----\n");

	int n_questions = 0;
	int n_correct = 0;

	char* line;
	while (line = read_a_line(fp)) {
		char* word = strtok(line, "\"");
		strtok(NULL, "\"");
		char* meaning = strtok(NULL, "\"");

		printf("Q. %s\n", meaning);
		printf("?  ");

		char answer[128];
		scanf("%s", answer);

		if (strcmp(answer, word) == 0) {
			printf("- correct\n");
			n_correct++;
		}
		else {
			printf("- wrong: %s\n", word);
		}

		n_questions++;
		free(line);
	}

	printf("(%d/%d)\n", n_correct, n_questions);

	printf("-----\n\n");

	fclose(fp);
}

//메인함수
int main()
{

	printf(" *** Word Quiz *** \n\n");

	int cmd;
	do {
		print_menu();

		cmd = get_command();
		switch (cmd) {
		case C_LIST: {
			list_wordbooks();
			break;
		}

		case C_SHOW: {
			show_words();
			break;
		}

		case C_TEST: {
			run_test();
			break;
		}

		case C_EXIT: {
			return EXIT_SUCCESS;
		}
		}
	} while (cmd != C_EXIT);


	return EXIT_SUCCESS;
}
